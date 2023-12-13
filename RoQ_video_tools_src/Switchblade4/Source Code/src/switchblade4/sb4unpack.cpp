#include <string.h>

#include "sb4image.h"

void UnpackCB2(const int *cb2, SB4Block<2> *block)
{
	block->Row(0, 0)[0] = cb2[0];
	block->Row(0, 0)[1] = cb2[1];
	block->Row(0, 1)[0] = cb2[2];
	block->Row(0, 1)[1] = cb2[3];

	block->Row(1, 0)[0] =
	block->Row(1, 0)[1] =
	block->Row(1, 1)[0] =
	block->Row(1, 1)[1] = (cb2[4] + 2) / 4;

	block->Row(2, 0)[0] =
	block->Row(2, 0)[1] =
	block->Row(2, 1)[0] =
	block->Row(2, 1)[1] = (cb2[5] + 2) / 4;
}

static void UnpackSingleCB4(const SB4Block<2> *cb2unpacked, const unsigned char *cb4, SB4Block<4> *block)
{
	const SB4Block<2> *ptr;
	unsigned int i, x, y, p;
	static const int offsets[4][2] = { {0, 0}, {2, 0}, {0, 2}, {2, 2} };

	for(p=0;p<3;p++)
		for(i=0;i<4;i++)
		{
			ptr = cb2unpacked + cb4[i];

			memcpy(block->Row(p, offsets[i][1]+0) + offsets[i][0], ptr->Row(p, 0), 2);
			memcpy(block->Row(p, offsets[i][1]+1) + offsets[i][0], ptr->Row(p, 1), 2);
		}
}

void UnpackAllCB4(const SB4Block<2> cb2unpacked[256], const unsigned char cb4[1024], SB4Block<4> *cb4unpacked)
{
	unsigned int i;

	for(i=0;i<256;i++)
		UnpackSingleCB4(cb2unpacked, cb4 + i*4, cb4unpacked + i);
}

void UnpackAllCB2(const unsigned char cb2[1536], SB4Block<2> *cb2unpacked)
{
	int temp[6];
	unsigned int i;
	SB4Block<2> *block;
	const unsigned char *cb2ptr;

	block = cb2unpacked;
	cb2ptr = cb2;
	for(i=0;i<256;i++)
	{
		block->Row(0, 0)[0] = *cb2ptr++;
		block->Row(0, 0)[1] = *cb2ptr++;
		block->Row(0, 1)[0] = *cb2ptr++;
		block->Row(0, 1)[1] = *cb2ptr++;

		block->Row(1, 0)[0] =
		block->Row(1, 0)[1] =
		block->Row(1, 1)[0] =
		block->Row(1, 1)[1] = *cb2ptr++;

		block->Row(2, 0)[0] =
		block->Row(2, 0)[1] =
		block->Row(2, 1)[0] =
		block->Row(2, 1)[1] = *cb2ptr++;

		block++;
	}
}

SB4Block<8> Enlarge4to8(const SB4Block<4> &b4)
{
	unsigned int x, y, p;
	SB4Block<8> b8;

	for(p=0;p<3;p++)
		for(y=0;y<8;y++)
			for(x=0;x<8;x++)
				b8.Row(p, y)[x] = b4.Row(p, y/2)[x/2];
	return b8;
}

