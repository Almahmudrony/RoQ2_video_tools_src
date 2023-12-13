#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <vfw.h>
#include "../sb4sdl.h"
#include "../sb4.h"
#include "../sb4image.h"
#include "../sb4videoenc.h"
#include "../sb4input.h"


#include "../../tdp/tdp_string.hpp"

using namespace TDP;

FILE *cbFile;
FILE *roqFile;

SDL_Surface *windowSurf;

void writecallback(void *h, const void *bytes, size_t numBytes)
{
	fwrite(bytes, 1, numBytes, (FILE *)h);
}


void FillProgressBar(SB4Image *pbar, unsigned int barwidth)
{
	unsigned int x, y, w, h, p;
	int hue;
	unsigned char *rows[3];

	w = pbar->Width();
	h = pbar->Height();

	int ShowInputBox(CIB_OBJ HWND hParentWindow, LPCTSTR lpszTitle, LPCTSTR lpszQuery, LPSTR szResult, WORD wMax);

	for(y=0;y<h;y++)
	{
		for(p=0;p<3;p++)
			rows[p] = pbar->Plane(p)->Row(y);

		for(x=0;x<w;x++)
		{
			if(x > barwidth)
				rgb2yuv(92, 92, 92, rows[0], rows[1], rows[2]);
			else if(x == 0 || y == 0)
				rgb2yuv(192, 192, 255, rows[0], rows[1], rows[2]);
			else if(x == barwidth || y == 7)
				rgb2yuv(64, 64, 255, rows[0], rows[1], rows[2]);
			else
			{
				hue = (32 - (barwidth - x)) * 4;
				if(hue > 127) hue = 127; else if(hue < 0) hue = 0;
				rgb2yuv(128 + hue, 128 + hue, 255, rows[0], rows[1], rows[2]);
			}

			for(p=0;p<3;p++)
				rows[p]++;
		}
	}
}

int main(int argc, char **argv)
{
	terString baseFile;
	terString baseName;
	terString cbcName;
	terString roqName;
	sb4aviinfo_t aviInfo;
	sb4vidstream_t *vs;
	terVector<unsigned char> scratch;
	char status[1000];
	char bitratestr[1000];
	CInputBox	myinp(NULL);
	double npp;

	static const unsigned char roqHeader[] =
		{0x84, 0x10, 0xff, 0xff, 0xff, 0xff, 0x1e, 0x00};

	int numFrames;
	SB4Image *img;
	SB4Image pbar;

	SDL_Surface *surf;
	SDL_Rect movieIn, movieOut;
	SDL_Rect pbarIn, pbarOut;
	
	SDL_Event event;

	sb4videoenc_t enc;

	SDL_Init(SDL_INIT_VIDEO);

	SB4AVIInit();

	if(argc == 2)
		baseFile = argv[1];
	else
		baseFile = SB4GetFile(NULL, "AVI video file (*.avi)\0" "*.avi\0", "Select input AVI file");

	if(baseFile.SubString(baseFile.Length() - 4, 4).NCSEquals(".avi"))
		baseName = baseFile.SubString(0, baseFile.Length() - 4);
	else
		baseName = baseFile;
	cbcName = baseName + ".cbc";

	roqName = baseName + "_video.roq";

	cbFile = fopen(cbcName.CStr(), "rb");
	if(!cbFile)
	{
		MessageBox(NULL, "Codebook cache wasn't generated, please run sbcodebooks4 on this video first", NULL, 0);
		return 1;
	}

	roqFile = fopen(roqName.CStr(), "wb");
	if(!cbFile)
	{
		MessageBox(NULL, "Couldn't open output RoQ file", NULL, 0);
		return 1;
	}

	strcpy(bitratestr, "3000000");

	if(!myinp.ShowInputBox(NULL, "Bitrate", "Enter output rate in bits per second", bitratestr, sizeof(bitratestr)-1))
		return 1;


	SB4GetAVIInfo(baseFile.CStr(), &aviInfo);

	// Partition window
	movieIn.x = movieIn.y = 0;
	movieIn.w = aviInfo.width;
	movieIn.h = aviInfo.height;

	movieOut.x = 0;
	movieOut.y = 0;
	movieOut.w = aviInfo.width;
	movieOut.h = aviInfo.height;

	pbarIn.x = pbarIn.y = 0;
	pbarIn.w = aviInfo.width;
	pbarIn.h = 8;

	pbarOut.x = 0;
	pbarOut.y = aviInfo.height;
	pbarOut.w = aviInfo.width;
	pbarOut.h = 8;


	// Figure out how many rows to use for feedback

	windowSurf = SDL_SetVideoMode(aviInfo.width, aviInfo.height + 8, 32, SDL_SWSURFACE);

	SB4GetAVIVideoStream(baseFile.CStr(), &vs);

	if(aviInfo.width % 16 || aviInfo.height % 16)
	{
		MessageBox(NULL, NULL, "Input video size must be divisible by 16.", 0);
		exit(1);
	}

	// Write RoQ header
	fwrite( roqHeader, 8, 1, roqFile );

	pbar.Init(aviInfo.width, 8);

	roq_encode_init(&enc, aviInfo.width, aviInfo.height, 999999, atoi(bitratestr) * 2, atoi(bitratestr));

	numFrames = aviInfo.numFrames;
	while(numFrames--)
	{
		img = SB4GetAVIVideoFrame(vs);

		if(SB4FlushSDLEvents())
			break;

		fread(enc.cb2, 1536, 1, cbFile);
		fread(enc.cb4, 1024, 1, cbFile);
		roq_encode_frame(&enc, img, roqFile, writecallback);

		npp = (double)(aviInfo.width * aviInfo.height);
		sprintf(status, "Frame %i / %i  Sz:%i  Slip: %i  Dpp:%4.4f", aviInfo.numFrames - numFrames, aviInfo.numFrames, enc.final_size, enc.slip, (double)enc.dist / npp);
		SDL_WM_SetCaption(status, NULL);

		FillProgressBar(&pbar, (double)(aviInfo.numFrames - numFrames) * (double)aviInfo.width / (double)(aviInfo.numFrames));

		surf = SurfaceFromImage(enc.current_frame, scratch);
		SDL_BlitSurface(surf, &movieIn, windowSurf, &movieOut);
		SDL_FreeSurface(surf);

		surf = SurfaceFromImage(&pbar, scratch);
		SDL_BlitSurface(surf, &pbarIn, windowSurf, &pbarOut);
		SDL_FreeSurface(surf);

		SDL_UpdateRect(windowSurf, 0, 0, 0, 0);

		scratch.Free();
		delete img;
	}
	SB4CloseAVIVideoStream(vs);

	fclose(cbFile);
	fclose(roqFile);

	return 0;
}
