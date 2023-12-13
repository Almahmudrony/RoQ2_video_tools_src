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

// Based loosely on Kosta Koeman's avi2bmp source code

#include "SDL/SDL.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vfw.h>
#include <stdio.h>
#include "libsb3/sb3.h"

#include "roqmux/mux.h"

void SDLProgressSetup(int width, int height);
int SDLProcessStats(sb3_encoder_t *enc, int frameNum, int frameMax, unsigned int bytes);
void SDLShutdown();


#define MAKECALL(x, y) if(hr = (x y)) { printf("Error: %s failed.  Code %x   AVI err %i\n", #x, hr, hr - MAKE_AVIERR(0)); return 1; }

#define BUILTIN_FRAMERATE 30

typedef struct
{
	char riff[4];
	unsigned long totalSize;
	char wave[4];
} waveHeader_t;

typedef struct
{
	char fmt_[4];
	unsigned long chunkSize;
} fmtBlock_t;

typedef struct
{
	char data[4];
	unsigned long chunkSize;
} dataBlock_t;

sb3_rgbapixel_t *convertMemory;

// By default, AVI frame gets are in upside-down BGR order
sb3_rgbapixel_t *MakeUprightRGB(unsigned char *d, unsigned int w, unsigned int h)
{
	unsigned int x,y;
	unsigned int inOffset;
	unsigned int outOffset;

	outOffset = 0;
	for(y=0;y<h;y++)
	{
		inOffset = (h-y-1)*w*3;
		for(x=0;x<w;x++)
		{
			convertMemory[outOffset].b = d[inOffset++];
			convertMemory[outOffset].g = d[inOffset++];
			convertMemory[outOffset].r = d[inOffset++];

			outOffset++;
		}
	}

	return convertMemory;
}


#define INTERNAL_FRAMERATE (BUILTIN_FRAMERATE*10000)

FILE *cbFile;
FILE *roqFile;
FILE *wavFile;

int neverCache = 0;
int forceRegen = 0;
int noMux = 0;

unsigned int keyBits = 0;
unsigned int motionBits = 0;

unsigned int keyPerc = 600;
unsigned int motionPerc = 200;

unsigned int keyRate=120;

unsigned int cull=64;



int ReadCodebook(void *handle, void *c)
{
	if(forceRegen)
		return 0;
	return fread(c, SB3_CODEBOOK_CACHE_SIZE, 1, cbFile);
}

void SaveCodebook(void *handle, const void *c)
{
	if(neverCache)
		return;
	fwrite(c, SB3_CODEBOOK_CACHE_SIZE, 1, cbFile);
}


static unsigned int bytesWritten = 0;
void WriteBuffer(void *f, const void *buffer, unsigned long size)
{
	bytesWritten += size;
	fwrite(buffer, size, 1, (FILE *)f);
}



void Usage()
{
	printf("Usage: avi2roq [options] input.avi\n");
	printf("   SwitchBlade 3.1 Copyright (c) 2004 Eric Lasota\n");
	printf("   NeuQuant Copyright (c) 1994 Anthony Dekker\n");
	printf("   Portions based on avi2bmp by Kosta Koeman\n\n");
	printf("   Generates input.RoQ (video), input.cbc (codebook cache), and input.wav\n");
	printf("Options\n");
	printf("  -isize bits    Sets bit maximum for key (I) frames (Default 1:20 ratio)\n");
	printf("  -psize bits    Sets bit maximum for motion (P) frames (Default 1:50 ratio\n");
	printf("  -io10k N       Sets bit maximum proportional to size for I frames\n");
	printf("  -po10k N       Sets bit maximum proportional to size for P frames\n");
	printf("  -keyrate rate  Sets rate of keyframes (1/30th sec)\n");
	printf("  -nevercache    Don't save generated codebooks\n");
	printf("  -forceregen    Force codebook regeneration\n");
	printf("  -nomux         Don't automatically multiplex audio/video after\n");
	printf("  -stdvq         Use old stdvq quantizer instead of NeuQuant\n");
	printf("\n");
	printf("  io10k and po10k values determine size by (original x N) / 10000\n");
	printf("  For example, a value of 200 will limit each video frame to 1/50th of its\n");
	printf("  original size as uncompressed RGB data\n");


	exit(1);
}

int main(int argc, char **argv)
{
	PAVISTREAM   avi;
	AVISTREAMINFO streamInfo;
	PAVIFILE     file;
	AVIFILEINFO  info;
	LPBITMAPINFOHEADER   bitmapHeader;
	PCMWAVEFORMAT           waveHeader;
	char *fileName;
	HRESULT hr;
	PGETFRAME pFrame;
	struct roq_compressor_s *comp;

	unsigned char *data;

	unsigned int frameNum;
	unsigned int actuallyRead;

	unsigned int nextKeyframe = 0;

	static char reformat[1024];
	static char fullName[1024];

	static char videoName[2048];
	static char audioName[2048];
	static char outName[2048];

	int curDot = 0;
	int nextDot;

	int i;
	int lastPeriod;

	unsigned int remaining;
	int sampleDivider;

	waveHeader_t wHeader;
	fmtBlock_t fHeader;
	dataBlock_t dHeader;

	sb3_hostapi_t hostAPI;
	sb3_encoder_t encoder;
	sb3_config_t config;

	unsigned char soundBuffer[4096];

	printf("avi2roq - Simple RoQ video converter\n");

	if(argc < 2)
		Usage();

	config.useNeuQuant = 1;

	for(i=1;i<argc;i++)
	{
		if(!strcmp(argv[i], "-ibits"))
		{
			i++;  if(i == argc) Usage();
			keyBits = atoi(argv[i]);
			printf("--- %i bits per I frame\n", keyBits);
		}
		else if(!strcmp(argv[i], "-pbits"))
		{
			i++;  if(i == argc) Usage();
			motionBits = atoi(argv[i]);
			printf("--- %i bits per P frame\n", motionBits);
		}
		if(!strcmp(argv[i], "-io10k"))
		{
			i++;  if(i == argc) Usage();
			keyPerc = atoi(argv[i]);
			printf("--- %i/10000 size ratio for I frames\n", keyPerc);
		}
		else if(!strcmp(argv[i], "-po10k"))
		{
			i++;  if(i == argc) Usage();
			motionPerc = atoi(argv[i]);
			printf("--- %i/10000 size ratio for P frames\n", motionPerc);
		}
		else if(!strcmp(argv[i], "-keyrate"))
		{
			i++;  if(i == argc) Usage();
			keyRate = atoi(argv[i]);
			printf("--- Keyframe every %i seconds\n", keyRate);
		}
		else if(!strcmp(argv[i], "-threshold"))
		{
			i++;  if(i == argc) Usage();
			cull = atoi(argv[i]);
		}
		else if(!strcmp(argv[i], "-nevercache"))
		{
			neverCache = 1;
			printf("--- Not saving codebooks\n");
		}
		else if(!strcmp(argv[i], "-forceregen"))
		{
			forceRegen = 1;
			printf("--- Not using cached codebooks\n");
		}
		else if(!strcmp(argv[i], "-nomux"))
		{
			noMux = 1;
			printf("--- Not multiplexing\n");
		}
		else if(!strcmp(argv[i], "-stdvq"))
		{
			config.useNeuQuant = 0;
			printf("--- using STDVQ\n");
		}
		else if(argv[i][0] == '-')
			Usage();
		else
			break;
	}

	// Prepare for reformatting
	fileName = argv[i];

	strcpy(reformat, fileName);
	lastPeriod = i = 0;
	while(reformat[i])
	{
		if(reformat[i] == '.')
			lastPeriod = i;
		i++;
	}

	if(!lastPeriod)
		lastPeriod = i;
	reformat[lastPeriod] = '\0';

	strcpy(fullName, reformat);   strcat(fullName, ".RoQ");
	roqFile = fopen(fullName, "wb");
	if(!roqFile) { printf("Could not open RoQ file\n"); return 1; }

	strcpy(fullName, reformat);   strcat(fullName, ".cbc");
	if(!forceRegen)
	{
		cbFile = fopen(fullName, "rb");
		if(!cbFile)
			forceRegen = 1;
		else
			neverCache = 1;
	}

	if(forceRegen && !neverCache)
	{
		cbFile = fopen(fullName, "wb");
		if(!cbFile) { printf("Could not open codebook cache file\n"); return 1; }
	}

	// Interface with the RoQ encoder
	hostAPI.readCodebookCache = ReadCodebook;
	hostAPI.writeCodebookCache = SaveCodebook;
	hostAPI.writebytes = WriteBuffer;

	AVIFileInit();

	// Open the file and get info
	printf("Encoding AVI file: %s\n", fileName);
	MAKECALL(AVIFileOpen, (&file, fileName, OF_READ, NULL));
	MAKECALL(AVIFileInfo, (file, &info, sizeof(AVIFILEINFO)));
	MAKECALL(AVIFileRelease, (file));

	printf("Video size: %i x %i\n", info.dwWidth, info.dwHeight);
	printf("Press Shift-Q to cancel encode!\n", info.dwWidth, info.dwHeight);

	convertMemory = malloc(info.dwWidth * info.dwHeight * sizeof(sb3_rgbapixel_t));
	if(!convertMemory)
	{
		printf("Could not create conversion map\n");
		return 1;
	}

	if(!keyBits)
		keyBits = (info.dwWidth * info.dwHeight * (3*8)) * keyPerc / 10000;
	if(!motionBits)
		motionBits = (info.dwWidth * info.dwHeight * (3*8)) * motionPerc / 10000;

	MAKECALL(AVIStreamOpenFromFile, (&avi, fileName, streamtypeVIDEO, 0, OF_READ, NULL));
	MAKECALL(AVIStreamInfo, (avi, &streamInfo, sizeof(AVISTREAMINFO)));

	// Write the RoQ header
	config.width = info.dwWidth;
	config.height = info.dwHeight;
	config.refinementPasses = 1;

	if(!SwitchBlade3_InitializeEncoder(&encoder, &config)) return 1;

	printf("Starting video\n");
	if(!SwitchBlade3_BeginFile(&encoder, &hostAPI, roqFile)) return 1;
	if(!SwitchBlade3_BeginVideo(&encoder, &hostAPI, roqFile)) return 1;
	frameNum = 0;


	SDLProgressSetup(info.dwWidth, info.dwHeight);

	pFrame = AVIStreamGetFrameOpen(avi, NULL);

	printf("Converting...\n");
	while(frameNum < info.dwLength)
	{
		// Load a frame
		if(!pFrame)
		{
			printf("Failed to open frame %i\n", frameNum);
			exit(1);
		}
		bitmapHeader = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pFrame, frameNum++);

		if(bitmapHeader->biBitCount != 24)
		{
			printf("\n\nOnly 24-bit input video is supported");
			return 1;
		}

		if(bitmapHeader->biCompression != BI_RGB)
		{
			printf("\n\nCompressed BMP streams are not supported");
			return 1;
		}

		data = (unsigned char *)bitmapHeader;
		data += sizeof(BITMAPINFOHEADER);

		if(!nextKeyframe)
		{
			SwitchBlade3_IFrame(&encoder);
			encoder.maxBytes = (keyBits+7) / 8;
			nextKeyframe = keyRate;
		}
		else
			encoder.maxBytes = (motionBits+7) / 8;

		nextKeyframe--;

		if(!SwitchBlade3_EncodeVideo(&encoder, MakeUprightRGB(data, info.dwWidth, info.dwHeight), &hostAPI, roqFile ))
		{
			printf("\n\nFrame encode failed!! Are the image dimensions divisible by 16?");
			return 1;
		}

		if(SDLProcessStats(&encoder, frameNum, info.dwLength, bytesWritten))
			break;
	}
	MAKECALL(AVIStreamGetFrameClose, (pFrame));

	SDLShutdown();

	printf("Shutting down compressor\n\n");
	SwitchBlade3_DestroyEncoder(&encoder);

	printf("\n\nVideo compression complete!\n\n");

	// Clean up
	MAKECALL(AVIStreamRelease, (avi));
	fclose(roqFile);

	if(!forceRegen && !neverCache)
		fclose(cbFile);

	// Extract audio
    if(!AVIStreamOpenFromFile(&avi, fileName, streamtypeAUDIO, 0, OF_READ, NULL))
	{
		if(!noMux)
		{
			sprintf(outName, "%s.RoQ", reformat);
			sprintf(videoName, "%s_video.RoQ", reformat);
			sprintf(audioName, "%s.wav", reformat);

			// Rename the existing RoQ
			remove(videoName);
			rename(outName, videoName);
		}

		MAKECALL(AVIStreamInfo, (avi, &streamInfo, sizeof(AVISTREAMINFO)));

		frameNum = sizeof(waveHeader);		// Cheap var recycling shit
		MAKECALL(AVIStreamReadFormat, (avi, AVIStreamStart(avi), &waveHeader, &frameNum));

		strcpy(fullName, reformat);   strcat(fullName, ".wav");
		wavFile = fopen(fullName, "wb");

		printf("Stripping WAV data to %s\n", fullName);

		if(!wavFile) { printf("Could not open WAV file\n"); return 1; }

		printf("%i audio samples\n", streamInfo.dwLength);

		// Find exact data size
		sampleDivider = waveHeader.wBitsPerSample*waveHeader.wf.nChannels/8;
		remaining = streamInfo.dwLength*sampleDivider;

		// Write stuff
		wHeader.riff[0] = 'R';
		wHeader.riff[1] = 'I';
		wHeader.riff[2] = 'F';
		wHeader.riff[3] = 'F';

		wHeader.totalSize = sizeof(fmtBlock_t) + sizeof(PCMWAVEFORMAT) + sizeof(dataBlock_t) + remaining + 4;

		wHeader.wave[0] = 'W';
		wHeader.wave[1] = 'A';
		wHeader.wave[2] = 'V';
		wHeader.wave[3] = 'E';

		fHeader.fmt_[0] = 'f';
		fHeader.fmt_[1] = 'm';
		fHeader.fmt_[2] = 't';
		fHeader.fmt_[3] = ' ';

		fHeader.chunkSize = sizeof(PCMWAVEFORMAT);

		dHeader.data[0] = 'd';
		dHeader.data[1] = 'a';
		dHeader.data[2] = 't';
		dHeader.data[3] = 'a';

		dHeader.chunkSize = remaining;

		fwrite(&wHeader, sizeof(wHeader), 1, wavFile);
		fwrite(&fHeader, sizeof(fHeader), 1, wavFile);
		fwrite(&waveHeader, sizeof(PCMWAVEFORMAT), 1, wavFile);
		fwrite(&dHeader, sizeof(dHeader), 1, wavFile);


		frameNum = 0;
		while(frameNum < streamInfo.dwLength)
		{
			MAKECALL(AVIStreamRead, (avi, frameNum, 4096/sampleDivider, soundBuffer, 4096, &actuallyRead, NULL));
			fwrite(soundBuffer, actuallyRead, 1, wavFile);

			frameNum += actuallyRead/sampleDivider;
		}
		printf("Done\n");

		MAKECALL(AVIStreamRelease, (avi));
		fclose(wavFile);

		if(!noMux)
		{
			muxFiles(audioName, videoName, outName, 0);
			remove(videoName);
			remove(audioName);
		}
	}

	AVIFileExit();

	printf("Finished!  Press 'Enter' to exit\n");
	getchar();

	return 0;
}
