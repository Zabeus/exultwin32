/*
 *	Copyright (C) 2003 MaxSt ( maxst@hiend3d.com )
 *
 *	Adapted for Exult: 4/7/07 - JSF
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *	License as published by the Free Software Foundation; either
 *	version 2 of the License, or (at your option) any later version.
 *
 *	This library is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	Library General Public License for more details.
 *
 *	You should have received a copy of the GNU Library General Public
 *	License along with this library; if not, write to the
 *	Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *	Boston, MA  02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "SDL_video.h"

#include "imagewin.h"
#include <cstdlib>
#include <cstring>

#include "exult_types.h"

#include "manip.h"
#include "scale_hqnx.h"
#include "scale_hq2x.h"

//
// Hq2x Filtering
//
void Image_window::show_scaled8to16_Hq2x
	(
	int x, int y, int w, int h	// Area to show.
	)
	{
	Manip8to16 manip(paletted_surface->format->palette->colors,
						inter_surface->format);
	Scale_Hq2x<uint16, Manip8to16>
		((uint8*)draw_surface->pixels, x+guard_band, y+guard_band, w, h,
		    ibuf->line_width, ibuf->height+guard_band, 
		    (uint16 *) inter_surface->pixels, 
			inter_surface->pitch/
				inter_surface->format->BytesPerPixel,
			manip);
	}

void Image_window::show_scaled8to555_Hq2x
	(
	int x, int y, int w, int h	// Area to show.
	)
	{
	Manip8to555 manip(paletted_surface->format->palette->colors,
						inter_surface->format);
	Scale_Hq2x<uint16, Manip8to555>
		((uint8*)draw_surface->pixels, x+guard_band, y+guard_band, w, h,
		    ibuf->line_width, ibuf->height+guard_band, 
		    (uint16 *) inter_surface->pixels, 
			inter_surface->pitch/
				inter_surface->format->BytesPerPixel,
			manip);
	}

void Image_window::show_scaled8to565_Hq2x
	(
	int x, int y, int w, int h	// Area to show.
	)
	{
	Manip8to565 manip(paletted_surface->format->palette->colors,
						inter_surface->format);
	Scale_Hq2x<uint16, Manip8to565>
		((uint8*)draw_surface->pixels, x+guard_band, y+guard_band, w, h,
		    ibuf->line_width, ibuf->height+guard_band, 
		    (uint16 *) inter_surface->pixels, 
			inter_surface->pitch/
				inter_surface->format->BytesPerPixel,
			manip);
	}

void Image_window::show_scaled8to32_Hq2x
	(
	int x, int y, int w, int h	// Area to show.
	)
	{
	Manip8to32 manip(paletted_surface->format->palette->colors,
						inter_surface->format);
	Scale_Hq2x<uint32, Manip8to32>
		((uint8*)draw_surface->pixels, x+guard_band, y+guard_band, w, h,
			ibuf->line_width, ibuf->height+guard_band, 
			(uint32 *) inter_surface->pixels,
			inter_surface->pitch/
				inter_surface->format->BytesPerPixel,
								manip);
	}
