#include "sb4lsfreader.h"

bool LSFSoundReader::Open(const char *filename)
{
	SF_INFO formatInfo;

	formatInfo.format = 0;
	_sf = sf_open(filename, SFM_READ, &formatInfo);

	if(_sf == NULL)
		return false;

	_channels = formatInfo.channels;
	_samplerate = formatInfo.samplerate;

	return true;
}

unsigned int LSFSoundReader::SampleRate()
{
	return _samplerate;
}

unsigned int LSFSoundReader::Channels()
{
	return _channels;
}

size_t LSFSoundReader::Read(short *ptr, size_t len)
{
	return sf_read_short((SNDFILE *)_sf, ptr, len);
}

void LSFSoundReader::Close()
{
	sf_close((SNDFILE *)_sf);
}
