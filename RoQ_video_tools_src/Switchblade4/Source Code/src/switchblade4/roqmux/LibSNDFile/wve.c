/*
** Copyright (C) 2002,2003 Erik de Castro Lopo <erikd@zip.com.au>
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

#include	<stdio.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<string.h>
#include	<ctype.h>

#include	"sndfile.h"
#include	"config.h"
#include	"sfendian.h"
#include	"common.h"

#if (ENABLE_EXPERIMENTAL_CODE == 0)

int
wve_open	(SF_PRIVATE *psf)
{	if (psf)
		return SFE_UNIMPLEMENTED ;
	return (psf && 0) ;
} /* wve_open */

#else

#define	SFE_WVE_NOT_WVE	666

/*------------------------------------------------------------------------------
** Macros to handle big/little endian issues.
*/

#define ALAW_MARKER		MAKE_MARKER ('A', 'L', 'a', 'w')
#define SOUN_MARKER		MAKE_MARKER ('S', 'o', 'u', 'n')
#define DFIL_MARKER		MAKE_MARKER ('d', 'F', 'i', 'l')

/*------------------------------------------------------------------------------
** Private static functions.
*/

static int	wve_read_header (SF_PRIVATE *psf) ;

/*------------------------------------------------------------------------------
** Public function.
*/

int
wve_open (SF_PRIVATE *psf)
{	int	subformat, error = 0 ;

	if (psf->mode == SFM_WRITE || psf->mode == SFM_RDWR)
		return SFE_UNIMPLEMENTED ;

	if ((error = wve_read_header (psf)))
		return error ;

	if ((psf->sf.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_WVE)
		return	SFE_BAD_OPEN_FORMAT ;

	subformat = psf->sf.format & SF_FORMAT_SUBMASK ;

	return error ;
} /* wve_open */

/*------------------------------------------------------------------------------
*/

static int
wve_read_header (SF_PRIVATE *psf)
{	int marker ;

	/* Set position to start of file to begin reading header. */
	psf_binheader_readf (psf, "pm", 0, &marker) ;
	if (marker != ALAW_MARKER)
		return SFE_WVE_NOT_WVE ;

	psf_binheader_readf (psf, "m", &marker) ;
	if (marker != SOUN_MARKER)
		return SFE_WVE_NOT_WVE ;

	psf_binheader_readf (psf, "m", &marker) ;
	if (marker != DFIL_MARKER)
		return SFE_WVE_NOT_WVE ;

	psf_log_printf (psf, "Read only : Psion Palmtop Alaw (.wve)\n"
			"  Sample Rate : 8000\n"
			"  Channels    : 1\n"
			"  Encoding    : A-law\n") ;

	psf->dataoffset = 0x20 ;
	psf->datalength = psf->filelength - psf->dataoffset ;

	psf->sf.format     = SF_FORMAT_WVE | SF_FORMAT_ALAW ;
	psf->sf.samplerate = 8000 ;
	psf->sf.frames     = psf->datalength ;
	psf->sf.channels   = 1 ;

	return alaw_init (psf) ;
} /* wve_read_header */

/*------------------------------------------------------------------------------
*/

#endif
