#ifndef __SB4AVISOUNDREADER_H__
#define __SB4AVISOUNDREADER_H__

#include "sb4soundreader.h"

class AVISoundReader : public SoundReader
{
private:
	void *_avi;

	unsigned int _samplerate;
	unsigned int _channels;

	unsigned int _bytesPerSample;

	size_t _offset;

public:
	virtual unsigned int SampleRate();
	virtual unsigned int Channels();

	virtual bool Open(const char *filename);
	virtual size_t Read(short *ptr, size_t len);
	virtual void Close();
};

#endif
