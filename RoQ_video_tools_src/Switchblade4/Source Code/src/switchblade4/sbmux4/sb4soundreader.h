#ifndef __SB4SOUNDREADER_H__
#define __SB4SOUNDREADER_H__

// Common sound reader template class
class SoundReader
{
public:
	virtual unsigned int SampleRate() = 0;
	virtual unsigned int Channels() = 0;

	virtual bool Open(const char *filename) = 0;
	virtual size_t Read(short *ptr, size_t len) = 0;
	virtual void Close() = 0;
};

#endif
