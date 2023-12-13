#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vfw.h>

#include <stdio.h>

#include "sb4avisoundreader.h"

bool AVISoundReader::Open(const char *filename)
{
	AVISTREAMINFO streamInfo;
	PCMWAVEFORMAT waveHeader;
	HRESULT hr;
	long waveHeaderSize;

	waveHeaderSize = sizeof(PCMWAVEFORMAT);

	hr = AVIStreamOpenFromFile((PAVISTREAM *)&_avi, filename, streamtypeAUDIO, 0, OF_READ, NULL);
	if(hr)
		return false;
	if(AVIStreamInfo((PAVISTREAM)_avi, &streamInfo, sizeof(AVISTREAMINFO)))
		return false;
	if(AVIStreamReadFormat((PAVISTREAM)_avi, AVIStreamStart((PAVISTREAM)_avi), &waveHeader, &waveHeaderSize))
		return false;

	if(waveHeader.wBitsPerSample % 8)
		return false;
	
	_bytesPerSample = waveHeader.wBitsPerSample / 8;
	_channels = waveHeader.wf.nChannels;
	_samplerate = waveHeader.wf.nSamplesPerSec;

	_offset = 0;

	return true;
}

unsigned int AVISoundReader::SampleRate()
{
	return _samplerate;
}

unsigned int AVISoundReader::Channels()
{
	return _channels;
}

size_t AVISoundReader::Read(short *ptr, size_t len)
{
	size_t numRead;
	long actuallyRead;

	// Read considers each channel one sample, while AVIStreamRead treats both channels as one sample
	if(_bytesPerSample == 2)
	{
		AVIStreamRead((PAVISTREAM)_avi, _offset, len / _channels, ptr, len * _bytesPerSample * _channels, &actuallyRead, NULL);
		_offset += actuallyRead / _channels / _bytesPerSample;
		return actuallyRead / _bytesPerSample;
	}
	else
		return 0;
}

void AVISoundReader::Close()
{
	AVIStreamRelease((PAVISTREAM)_avi);
}
