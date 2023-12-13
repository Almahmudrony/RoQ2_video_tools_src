#ifndef __SB4VIDEOENC_H__
#define __SB4VIDEOENC_H__

#include "sb4image.h"

typedef struct
{
    int dx, dy;
} motion_vect;

typedef struct
{
	SB4Image frames[2];

	SB4Image *last_frame;
    SB4Image *current_frame;
    const SB4Image *frame_to_enc;

	terVector<motion_vect> motion8[2];
	terVector<motion_vect> motion4[2];

    motion_vect *this_motion4;
    motion_vect *last_motion4;

    motion_vect *this_motion8;
    motion_vect *last_motion8;

	int width, height;

	int minBytes;
	int maxBytes;

	int framesSinceKeyframe;
	int framesToKeyframe;
	int keyrate;
	int first_frame;

	int ibitrate;
	int pbitrate;

	unsigned char cb2[1536];
	unsigned char cb4[1024];

	int final_size;

	int slip;
	__int64 dist;
	__int64 target_dist;
} sb4videoenc_t;

typedef void (*sb4writecallback_t) (void *h, const void *bytes, size_t numBytes);

int roq_encode_init(sb4videoenc_t *enc, int width, int height, int keyrate, int irate, int prate);
int roq_encode_frame(sb4videoenc_t *enc, const SB4Image *img, void *h, sb4writecallback_t cb);

#endif
