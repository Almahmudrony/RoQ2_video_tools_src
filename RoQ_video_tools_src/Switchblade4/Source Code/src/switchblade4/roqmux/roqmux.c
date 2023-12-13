/*
** Copyright (C) 2003 Eric Lasota/Orbiter Productions
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>

#include "mux.h"
#include "LibSNDFile/sndfile.h"

unsigned long addedData=0;

#define FPUTC addedData++; fputc

// ***************************************************************
#define MAX_DPCM 16129		// 127*127
unsigned char dpcmValues[MAX_DPCM];

short previousValues[2];
unsigned char stepIndex[2];

// Simple best-match error-minimizing DPCM table generation.
// Not much more complicated than just rounding down, and
// just as fast to look up, and still doesn't need sqrt.
//
// Basically just keeps two values for a range, which I call
// the forward projection and the baseline projection.
//
// The baseline is the current best match for the square
// root of a value, while the forward projection is the
// next value up the square list.
//
// Once values are closer to the forward projection, the
// forward projection and baseline are both moved one
// spot up the square list.
//
// The end result is a list with each inverse lookup being
// the entry that will produce the least error.  The only
// problem is that it will produce the possibility of
// bounds exceeding in the encoder, but that's obviously
// checked for.

void dpcmInit(void)
{
	unsigned short i;

	short diff;
	short diff2;
	unsigned short baseline;
	unsigned short projection;

	unsigned short projectionIndex;

	projectionIndex = 0;
	baseline = 0;
	projection = 1;

	// Create a table of quick DPCM values
	for(i=0;i<MAX_DPCM;i++)
	{
		// Check the difference of the last projection
		// and (possibly) the next projection
		diff = i - baseline;
		diff2 = i - projection;

		if(diff < 0)
			diff = -diff;
		if(diff2 < 0)
			diff2 = -diff2;

		// Move the DPCM index up a notch if it's closer
		if(diff2 < diff)
		{
			projectionIndex++;
			baseline = projection;
			projection = (projectionIndex+1)*(projectionIndex+1);
		}
		dpcmValues[i] = (unsigned char)projectionIndex;
	}

	previousValues[0] = 0;
	previousValues[1] = 0;
}
// ***************************************************************




#define SAMPLES_PER_FRAME 735	// 22050/30
#define MAX_SAMPLES       (SAMPLES_PER_FRAME * 8)

short channel1[MAX_SAMPLES*2];
short channel2[MAX_SAMPLES*2];

short temp[MAX_SAMPLES*4];

FILE *videoFile;
FILE *muxFile;

void CopyBytes(unsigned long len)
{
	unsigned char bytes[1024];

	while(len > 1024)
	{
		fread(bytes, 1024, 1, videoFile);
		fwrite(bytes, 1024, 1, muxFile);
		len -= 1024;
	}
	fread(bytes, len, 1, videoFile);
	fwrite(bytes, len, 1, muxFile);
}

short CopyChunk(void)
{
	unsigned short id;
	unsigned long size;

	unsigned char buffer[8];

	// Read the chunk header
	if(!fread(buffer, 8, 1, videoFile))
		return 0;

	// Parse info
	id = buffer[0] | (buffer[1] << 8);
	size = buffer[2] | (buffer[3] << 8) | (buffer[4] << 16) | (buffer[5] << 24);


	// Don't copy audio chunks
	if(id == 0x1020 || id == 0x1021)
	{
		fseek(videoFile, size, SEEK_CUR);
		return id;
	}
	
	// Copy the chunk header back out
	fwrite(buffer, 8, 1, muxFile);

	// Copy the rest
	CopyBytes(size);

	return id;
}


short EncodeDPCM(int previous, int current)
{
	unsigned char signMask;
	int output;
	int diff;

	diff = current - previous;

	if(diff < 0)
	{
		signMask = 128;
		diff = -diff;
	}
	else
		signMask = 0;

	if(diff >= MAX_DPCM)
		output = 127;
	else
		output = dpcmValues[diff];

	diff = output*output;
	if(signMask) diff = -diff;

	if(previous + diff < -32768 || previous + diff > 32767)
		output--;	// Tone down to prevent overflow

	diff = output*output;
	if(signMask) diff = -diff;

	FPUTC(output | signMask, muxFile);
	
	return previous + diff;
}


void EncodeStereo(unsigned long samples)
{
	unsigned long doubleSamples=samples*2;
	unsigned long i;

	FPUTC(0x21, muxFile);	// Stereo audio
	FPUTC(0x10, muxFile);

	FPUTC(doubleSamples & 255, muxFile);
	FPUTC((doubleSamples>>8) & 255, muxFile);
	FPUTC((doubleSamples>>16) & 255, muxFile);
	FPUTC((doubleSamples>>24) & 255, muxFile);

	previousValues[0] &= 0xFF00;
	previousValues[1] &= 0xFF00;

	FPUTC((previousValues[1] >> 8), muxFile);
	FPUTC((previousValues[0] >> 8), muxFile);

	// Encode samples
	for(i=0;i<samples;i++)
	{
		previousValues[0] = EncodeDPCM(previousValues[0], channel1[i]);
		previousValues[1] = EncodeDPCM(previousValues[1], channel2[i]);
	}
}


void EncodeMono(unsigned long samples)
{
	unsigned long i;

	FPUTC(0x20, muxFile);	// Mono audio
	FPUTC(0x10, muxFile);

	FPUTC(samples & 255, muxFile);
	FPUTC((samples>>8) & 255, muxFile);
	FPUTC((samples>>16) & 255, muxFile);
	FPUTC((samples>>24) & 255, muxFile);

	FPUTC(previousValues[0] & 255, muxFile);
	FPUTC((previousValues[0] >> 8) & 255, muxFile);

	// Encode samples
	for(i=0;i<samples;i++)
		previousValues[0] = EncodeDPCM(previousValues[0], channel1[i]);
}


unsigned char ADPCMEncode(short *pSample, unsigned char *pIndex, short newSample);

void EncodeStereoADPCM(unsigned long samples)
{
	unsigned long dataAmount=samples;
	unsigned long i;

	unsigned char lowBits;
	unsigned char highBits;

	FPUTC(0x31, muxFile);	// Stereo ADPCM
	FPUTC(0x10, muxFile);

	dataAmount+=2;

	FPUTC(dataAmount & 255, muxFile);
	FPUTC((dataAmount>>8) & 255, muxFile);
	FPUTC((dataAmount>>16) & 255, muxFile);
	FPUTC((dataAmount>>24) & 255, muxFile);

	previousValues[0] &= 0xFF00;
	previousValues[1] &= 0xFF00;

	FPUTC((previousValues[1] >> 8), muxFile);
	FPUTC((previousValues[0] >> 8), muxFile);

	FPUTC(stepIndex[0], muxFile);
	FPUTC(stepIndex[1], muxFile);

	// Encode samples
	for(i=0;i<samples;i++)
	{
		highBits = ADPCMEncode(&previousValues[0], &stepIndex[0], channel1[i]) << 4;
		lowBits = ADPCMEncode(&previousValues[1], &stepIndex[1], channel2[i]);

		FPUTC(highBits | lowBits, muxFile);
	}
}


void EncodeMonoADPCM(unsigned long samples)
{
	unsigned long i;
	unsigned long dataAmount = samples / 2;

	unsigned char bits = 0;
	unsigned char resultBits;

	unsigned long bufferOrder = 0;	// Flush if 1

	if(samples & 1)
		dataAmount++;

	dataAmount++;	// For step index

	FPUTC(0x30, muxFile);	// Mono ADPCM
	FPUTC(0x10, muxFile);

	FPUTC(dataAmount & 255, muxFile);
	FPUTC((dataAmount>>8) & 255, muxFile);
	FPUTC((dataAmount>>16) & 255, muxFile);
	FPUTC((dataAmount>>24) & 255, muxFile);

	FPUTC(previousValues[0] & 255, muxFile);
	FPUTC((previousValues[0] >> 8) & 255, muxFile);

	if(samples & 1)
	{
		// Decoder will know to drop the last sample from this
		FPUTC(stepIndex[0] | 128, muxFile);
	}
	else
	{
		FPUTC(stepIndex[0], muxFile);
	}

	// Encode samples
	for(i=0;i<samples;i++)
	{
		resultBits = ADPCMEncode(&previousValues[0], &stepIndex[0], channel1[i]);

		if(!bufferOrder)
			bits = resultBits << 4;
		else
		{
			FPUTC(bits | resultBits, muxFile);
		}
		bufferOrder = !bufferOrder;
	}

	// Flush the final bits if needed
	if(bufferOrder)
	{
		FPUTC(bits, muxFile);
	}
}


void ConvertAudio(SNDFILE *f, unsigned long samples, int samplerate, int channels, int sourceChannels, int adpcm)
{
	unsigned long realSamples;
	unsigned long samplesRemaining;
	short *in, *out1, *out2;
	short chunkSamples[2];
	int numChunkSamples;

	if(samplerate == 11025)
		samples /= 2;
	else if(samplerate == 44100)
		samples *= 2;

	// FIXME: Memset
	memset(temp, 0, sizeof(temp));

	realSamples = sf_read_short(f, temp, samples*sourceChannels);

	// Copy the samples
	samplesRemaining = realSamples;

	in = temp;
	out1 = channel1;
	out2 = channel2;

	while(samplesRemaining)
	{
		printf("Out offset: %i   Input sample offset: %i\n", out1 - channel1, in - temp);
		if(samplerate == 11025)
		{
			numChunkSamples = 2;
			samplesRemaining -= sourceChannels;
		}
		else if(samplerate == 22050)
		{
			numChunkSamples = 1;
			samplesRemaining -= sourceChannels;
		}
		else if(samplerate == 44100)
		{
			numChunkSamples = 1;
			if(samplesRemaining < sourceChannels * 2)
				samplesRemaining = 0;
			else
				samplesRemaining -= sourceChannels*2;
		}

		chunkSamples[0] = *in++;
		if(sourceChannels == 2)
			chunkSamples[1] = *in++;
		else
			chunkSamples[1] = chunkSamples[0];

		if(samplerate == 44100)
			in += sourceChannels;

		while(numChunkSamples)
		{
			if(channels == 1)
				*out1++ = ( (int)chunkSamples[0] + (int)chunkSamples[1]) / 2;
			else
			{
				*out1++ = chunkSamples[0];
				*out2++ = chunkSamples[1];
			}

			numChunkSamples--;
		}
	}

	realSamples /= sourceChannels;

	if(samplerate == 11025)
		realSamples *= 2;
	else if(samplerate == 44100)
		realSamples /= 2;

	if(!realSamples)
		return;

	if(channels == 2)
	{
		if(adpcm)
			EncodeStereoADPCM(realSamples);
		else
			EncodeStereo(realSamples);
	}
	else
	{
		if(adpcm)
			EncodeMonoADPCM(realSamples);
		else
			EncodeMono(realSamples);
	}
}

// RoQ video multiplexer/audio encoder
int muxFiles(const char *audioName, const char *videoName, const char *outputName, int flags)
{
	SNDFILE *sf;
	SF_INFO formatInfo;
	short tag;

	int encodeChannels;

	int firstFrame = 1;

	formatInfo.format = 0;
	sf = sf_open(audioName, SFM_READ, &formatInfo);
	if(!sf) { printf("Could not open sound file %s\n", audioName); return 1; }

	switch(formatInfo.samplerate)
	{
	case 11025:
	case 22050:
	case 44100:
		break;
	default:
		printf("Only 11025, 22050, and 44100 sampling rates are supported\n");
	}

	videoFile = fopen(videoName, "rb");
	if(!videoFile) { printf("Could not open video file\n"); return 1; }
	muxFile = fopen(outputName, "wb");
	if(!videoFile) { printf("Could not open output file\n"); return 1; }

	dpcmInit();

	stepIndex[0] = stepIndex[1] = 0;

	printf("Multiplexing... ");

	if(flags & ROQMUX_ALLOW_MONO)
		encodeChannels = formatInfo.channels;
	else
		encodeChannels = 2;

	// Copy the header
	CopyBytes(8);

	// Copy data until the video runs out
	while(1)
	{
		// Output audio
		if(firstFrame)
			ConvertAudio(sf, MAX_SAMPLES, formatInfo.samplerate, encodeChannels, formatInfo.channels, flags & ROQMUX_ADPCM);
		else
			ConvertAudio(sf, SAMPLES_PER_FRAME, formatInfo.samplerate, encodeChannels, formatInfo.channels, flags & ROQMUX_ADPCM);
		
		firstFrame = 0;

		// Copy chunks until a video frame is reached
		tag = CopyChunk();
		while(tag && tag != 0x1011)
			tag = CopyChunk();

		if(!tag)
			break;
	}
	printf("Done\n");
	printf("%i bytes of compressed audio added\n", addedData);

	fclose(videoFile);
	fclose(muxFile);

	sf_close(sf);

	return 0;
}
