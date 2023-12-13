#ifndef __SB4LSFREADER_H__
#define __SB4LSFREADER_H__

#include "sb4soundreader.h"
#include "../roqmux/LibSNDFile/sndfile.h"


// Common sound reader template class
class LSFSoundReader : public SoundReader
{
private:
	void *_sf;

	unsigned int _samplerate;
	unsigned int _channels;

public:
	virtual unsigned int SampleRate();
	virtual unsigned int Channels();

	virtual bool Open(const char *filename);
	virtual size_t Read(short *ptr, size_t len);
	virtual void Close();
};

#endif
