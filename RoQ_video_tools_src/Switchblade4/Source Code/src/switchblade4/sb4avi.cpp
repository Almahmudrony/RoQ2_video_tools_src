#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vfw.h>

#include "sb4.h"
#include "sb4image.h"
#include "../tdp/tdp_guard.hpp"

using namespace TDP;

#define MAKECALL(x) if(hr = (x)) { return hr - MAKE_AVIERR(0); }

void SB4AVIInit()
{
	AVIFileInit();
}

int SB4GetAVIInfo(const char *fileName, sb4aviinfo_t *aviinfo)
{
	PAVIFILE     file;
	AVIFILEINFO  info;
	HRESULT hr;

	MAKECALL(AVIFileOpen(&file, fileName, OF_READ, NULL));
	MAKECALL(AVIFileInfo(file, &info, sizeof(AVIFILEINFO)));
	MAKECALL(AVIFileRelease(file));

	aviinfo->width = info.dwWidth;
	aviinfo->height = info.dwHeight;
	aviinfo->numFrames = info.dwLength;

	return 0;
}

struct sb4vidstream_s
{
	PAVISTREAM   avi;
	AVISTREAMINFO streamInfo;
	PGETFRAME pFrame;

	int frameNum;
};

int SB4GetAVIVideoStream(const char *fileName, sb4vidstream_t **vsp)
{
	HRESULT hr;
	terSafePointer<sb4vidstream_t> vs;

	vs = new sb4vidstream_t();
	vs->pFrame = NULL;

	MAKECALL(AVIStreamOpenFromFile(&vs->avi, fileName, streamtypeVIDEO, 0, OF_READ, NULL));
	MAKECALL(AVIStreamInfo(vs->avi, &vs->streamInfo, sizeof(AVISTREAMINFO)));
	vs->pFrame = AVIStreamGetFrameOpen(vs->avi, NULL);

	vs->frameNum = 0;

	*vsp = vs.Unguard();

	return 0;
}

SB4Image *SB4GetAVIVideoFrame(sb4vidstream_t *vs)
{
	static LPBITMAPINFOHEADER bitmapHeader;
	SB4ImagePlane *planes[3];
	unsigned char *data;
	unsigned char *rows[3];
	unsigned int x, y, plane, width, height;
	terSafePointer<SB4Image> img;

	bitmapHeader = (LPBITMAPINFOHEADER)AVIStreamGetFrame(vs->pFrame, vs->frameNum++);

	if(bitmapHeader->biBitCount != 24)
		return NULL;

	if(bitmapHeader->biCompression != BI_RGB)
		return NULL;

	data = (unsigned char *)bitmapHeader;
	data += sizeof(BITMAPINFOHEADER);

	width = bitmapHeader->biWidth;
	height = bitmapHeader->biHeight;

	// Create SB4Image
	img = new SB4Image();
	img->Init(width, height);

	// Convert from bottom-up interlaced BGR to top-down planar RGB
	for(plane=0;plane<3;plane++)
		planes[plane] = img->Plane(plane);

	for(y=0;y<height;y++)
	{
		for(plane=0;plane<3;plane++)
			rows[plane] = planes[plane]->Row(height - y - 1);

		for(x=0;x<width;x++)
		{
			rgb2yuv(data[2], data[1], data[0], rows[0], rows[1], rows[2]);
			for(plane=0;plane<3;plane++)
				rows[plane]++;
			data += 3;
		}
	}

	return img.Unguard();
}

int SB4CloseAVIVideoStream(sb4vidstream_t *vs)
{
	HRESULT hr;

	if(vs->pFrame)
		MAKECALL(AVIStreamGetFrameClose(vs->pFrame));
	if(vs->avi)
		MAKECALL(AVIStreamRelease(vs->avi));

	return 0;
}
