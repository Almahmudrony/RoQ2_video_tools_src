/*
** Copyright (C) 2003 Eric Lasota/Orbiter Productions
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
#include <stdio.h>

#include "mux.h"

int main(int argc, char **argv)
{
	printf("roqmux - Multiplexes audio with an RoQ video\n");
	printf("  libsndfile (c)1999-2003 Erik de Castro Lopo\n");
	printf("Usage: roqmux [-adpcm] <audio file> <video file> <output video file>\n");

	if(argc != 4)
	{
		if(argc == 5 && !strcmp(argv[1], "-adpcm"))
		{
			muxFiles(argv[2], argv[3], argv[4], ROQMUX_ALLOW_MONO | ROQMUX_ADPCM);
			return 0;
		}
		else
			return 1;
	}

	muxFiles(argv[1], argv[2], argv[3], 0);

	return 0;
}
