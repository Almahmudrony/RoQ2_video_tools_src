#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <vfw.h>
#include "../sb4sdl.h"
#include "../sb4.h"
#include "../sb4image.h"
#include "../sb4random.h"
#include "../elbg.h"


#include "../../tdp/tdp_string.hpp"

using namespace TDP;


FILE *cbFile;

SDL_Surface *windowSurf;

int compareInt6(const void *a, const void *b)
{
	const int *ia = (const int *)a;
	const int *ib = (const int *)b;

	unsigned int i;
	int at = 0;
	int bt = 0;

	for(i=0;i<4;i++)
	{
		at += *ia++;
		bt += *ib++;
	}

	return at-bt;
}

int compareInt24(const void *a, const void *b)
{
	const int *ia = (const int *)a;
	const int *ib = (const int *)b;

	unsigned int i,j;
	int at = 0;
	int bt = 0;

	for(i=0;i<24;i+=6)
	{
		for(j=0;j<4;j++)
		{
			at += *ia++;
			bt += *ib++;
		}
		ia+=2;
		ib+=2;
	}

	return at-bt;
}


void MakeCodebooks(AVRandomState *rnd, const SB4Image *img, SB4Image *feedback, int feedbackDiv)
{
	unsigned int w, h, numClusters, blockx, blocky, subx, suby, x, y, i, j;
	SB4Block<2> block2;
	terVector<int> points;
	terVector<int> nearest;
	terVector<int> cb2int;
	terVector<int> cb4int;
	int *ptr;
	unsigned char yuv[3][2][2];
	unsigned char cb2[1536];
	unsigned char cb4[1024];
	SB4Block<2> cb2unpacked[256];
	SB4Block<4> cb4unpacked[256];

	w = img->Width();
	h = img->Height();

	numClusters = (w/2) * (h/2);

	cb4int.Alloc(256 * 24);
	cb2int.Alloc(256 * 6);

	points.Alloc( numClusters * 6 );
	ptr = points;

	// Map in CB4 cluster order to allow the same inputs to be reused
	for(blocky=0;blocky<h;blocky+=4)
	{
		for(blockx=0;blockx<w;blockx+=4)
		{
			for(suby=0;suby<4;suby+=2)
			{
				for(subx=0;subx<4;subx+=2)
				{
					x = blockx+subx;
					y = blocky+suby;

					block2 = SB4Block<2>(img, x, y);

					*ptr++ = block2.Row(0, 0)[0];
					*ptr++ = block2.Row(0, 0)[1];
					*ptr++ = block2.Row(0, 1)[0];
					*ptr++ = block2.Row(0, 1)[1];
					*ptr++ = block2.Row(1, 0)[0] + block2.Row(1, 0)[1] + block2.Row(1, 1)[0] + block2.Row(1, 1)[1];
					*ptr++ = block2.Row(2, 0)[0] + block2.Row(2, 0)[1] + block2.Row(2, 1)[0] + block2.Row(2, 1)[1];
				}
			}
		}
	}

	// Generate codebooks
	nearest.Alloc(numClusters);
	ff_init_elbg(points, 6, numClusters, cb2int, 256, 1, nearest, rnd);
	ff_do_elbg(points, 6, numClusters, cb2int, 256, 1, nearest, rnd);

	qsort(cb2int, 256, 6*sizeof(int), compareInt6);

	nearest.Alloc(numClusters / 4);
	ff_init_elbg(points, 24, numClusters/4, cb4int, 256, 1, nearest, rnd);
	ff_do_elbg(points, 24, numClusters/4, cb4int, 256, 1, nearest, rnd);

	qsort(cb4int, 256, 24*sizeof(int), compareInt24);

	// Store and unpack CB2
	for(i=0;i<1536;i+=6)
	{
		cb2[i+0] = cb2int[i+0];
		cb2[i+1] = cb2int[i+1];
		cb2[i+2] = cb2int[i+2];
		cb2[i+3] = cb2int[i+3];
		cb2[i+4] = (cb2int[i+4] + 2) / 4;
		cb2[i+5] = (cb2int[i+5] + 2) / 4;

		UnpackCB2(&cb2int[i+0], &cb2unpacked[i/6]);
	}

	// Index CB4 to CB2 and store
	for(i=0;i<6144;i+=6)
	{
		UnpackCB2(&cb4int[i+0], &block2);
		cb4[i/6] = block2.IndexList(cb2unpacked, 256, 0);
	}

	// Unpack all CB4 entries
	UnpackAllCB4(cb2unpacked, cb4, cb4unpacked);

	// Export to the CBC file
	fwrite(cb2, 6, 256, cbFile);
	fwrite(cb4, 4, 256, cbFile);

	// Write feedback
	for(i=0;i<256;i++)
	{
		// CB4 feedback
		x = (i % feedbackDiv) * 4;
		y = (i / feedbackDiv) * 4;
		cb4unpacked[i].Blit(feedback, x, y);

		// CB2 feedback
		x = (i % feedbackDiv) * 2 + (feedbackDiv * 4);
		y = (i / feedbackDiv) * 2;
		cb2unpacked[i].Blit(feedback, x, y);
	}
}

void FillProgressBar(SB4Image *pbar, unsigned int barwidth)
{
	unsigned int x, y, w, h, p;
	int hue;
	unsigned char *rows[3];

	w = pbar->Width();
	h = pbar->Height();

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
	terString cbcName;
	sb4aviinfo_t aviInfo;
	sb4vidstream_t *vs;
	terVector<unsigned char> scratch;
	char status[1000];

	int windowWidth, windowHeight;

	int numFrames;
	SB4Image *img;
	SB4Image feedback;
	SB4Image pbar;

	int feedbackDiv;
	int feedbackHeight;

	SDL_Surface *surf;
	SDL_Rect movieIn, movieOut;
	SDL_Rect feedbackIn, feedbackOut;
	SDL_Rect pbarIn, pbarOut;
	
	SDL_Event event;

	AVRandomState random;

	av_init_random(1, &random);


	SDL_Init(SDL_INIT_VIDEO);
	SB4AVIInit();

	if(argc == 2)
		baseFile = argv[1];
	else
		baseFile = SB4GetFile(NULL, "AVI video file (*.avi)\0" "*.avi\0", "Select input AVI file");

	if(baseFile.SubString(baseFile.Length() - 4, 4).NCSEquals(".avi"))
		cbcName = baseFile.SubString(0, baseFile.Length() - 4);
	else
		cbcName = baseFile;
	cbcName += ".cbc";

	cbFile = fopen(cbcName.CStr(), "wb");
	if(!cbFile)
		return 1;

	SB4GetAVIInfo(baseFile.CStr(), &aviInfo);

	windowWidth = aviInfo.width;
	windowHeight = aviInfo.height;

	if(windowWidth < 96) windowWidth = 96;

	feedbackDiv = windowWidth / 6;
	feedbackHeight = (256 + feedbackDiv - 1) / feedbackDiv * 4;

	// Partition window
	movieIn.x = movieIn.y = 0;
	movieIn.w = aviInfo.width;
	movieIn.h = aviInfo.height;

	movieOut.x = 0;
	movieOut.y = 0;
	movieOut.w = aviInfo.width;
	movieOut.h = aviInfo.height;

	pbarIn.x = pbarIn.y = 0;
	pbarIn.w = windowWidth;
	pbarIn.h = 8;

	pbarOut.x = 0;
	pbarOut.y = aviInfo.height;
	pbarOut.w = windowWidth;
	pbarOut.h = 8;

	feedbackIn.x = feedbackIn.y = 0;
	feedbackIn.w = feedbackDiv * 6;
	feedbackIn.h = feedbackHeight;

	feedbackOut.x = 0;
	feedbackOut.y = aviInfo.height + 8;
	feedbackOut.w = feedbackDiv * 6;
	feedbackOut.h = feedbackHeight;


	// Figure out how many rows to use for feedback

	windowSurf = SDL_SetVideoMode(windowWidth, windowHeight + feedbackHeight + 8, 32, SDL_SWSURFACE);

	SB4GetAVIVideoStream(baseFile.CStr(), &vs);

	if(aviInfo.width % 16 || aviInfo.height % 16)
	{
		MessageBox(NULL, "Video dimensions not divisible by 16", NULL, 0);
		exit(1);
	}

	feedback.Init(feedbackDiv * 6, feedbackHeight);
	pbar.Init(windowWidth, 8);

	numFrames = aviInfo.numFrames;
	while(numFrames--)
	{
		img = SB4GetAVIVideoFrame(vs);

		if(SB4FlushSDLEvents())
			break;

		MakeCodebooks(&random, img, &feedback, feedbackDiv);

		sprintf(status, "sbcodebooks4: Frame %i / %i", aviInfo.numFrames - numFrames, aviInfo.numFrames);
		SDL_WM_SetCaption(status, NULL);


		FillProgressBar(&pbar, (double)(aviInfo.numFrames - numFrames) * (double)windowWidth / (double)(aviInfo.numFrames));

		surf = SurfaceFromImage(&feedback, scratch);
		SDL_BlitSurface(surf, &feedbackIn, windowSurf, &feedbackOut);
		SDL_FreeSurface(surf);

		surf = SurfaceFromImage(img, scratch);
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

	return 1;
}
