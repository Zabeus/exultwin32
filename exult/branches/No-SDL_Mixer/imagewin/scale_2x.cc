/*
 * Scale2X algorithm by Andrea Mazzoleni.
 *
 * Copyright (C) 2001-2002 Andrea Mazzoleni
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
 *	You should have received a copy of the GNU General Public
 *	License along with this library; if not, write to the
 *	Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *	Boston, MA  02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "imagewin.h"
#ifndef ALPHA_LINUX_CXX
#  include <cstring>
#endif
#include "exult_types.h"
#include "manip.h"
#include "scale_2x.h"

//
// Scale2x (no blurring) by Andrea Mazzoleni.
//
void Image_window::show_scaled8bit_2x_noblur
	(
	int x, int y, int w, int h	// Area to show.
	)
	{
	Manip8to8 manip(surface->format->palette->colors,
						surface->format);
	Scale2x_noblur<uint8, Manip8to8>
		((unsigned char *)ibuf->get_bits(), x, y, w, h,
		    ibuf->line_width, ibuf->height, 
				(uint8 *) surface->pixels, 
				surface->pitch,
			manip);

	SDL_UpdateRect(surface, 2*x, 2*y, 2*w, 2*h);
	}

void Image_window::show_scaled8to16_2x_noblur
	(
	int x, int y, int w, int h	// Area to show.
	)
	{
	Manip8to16 manip(surface->format->palette->colors,
						scaled_surface->format);
	Scale2x_noblur<unsigned char, uint16, Manip8to16>
		(ibuf->get_bits(), x, y, w, h,
		    ibuf->line_width, ibuf->height, 
		    (uint16 *) scaled_surface->pixels, 
			scaled_surface->pitch/
				scaled_surface->format->BytesPerPixel,
			manip);
	SDL_UpdateRect(scaled_surface, 2*x, 2*y, 2*w, 2*h);
	}

void Image_window::show_scaled8to555_2x_noblur
	(
	int x, int y, int w, int h	// Area to show.
	)
	{
	Manip8to555 manip(surface->format->palette->colors);
	Scale2x_noblur<unsigned char, uint16, Manip8to555>
		(ibuf->get_bits(), x, y, w, h,
		    ibuf->line_width, ibuf->height, 
		    (uint16 *) scaled_surface->pixels, 
			scaled_surface->pitch/
				scaled_surface->format->BytesPerPixel,
			manip);
	SDL_UpdateRect(scaled_surface, 2*x, 2*y, 2*w, 2*h);
	}

void Image_window::show_scaled8to565_2x_noblur
	(
	int x, int y, int w, int h	// Area to show.
	)
	{
	Manip8to565 manip(surface->format->palette->colors);
	Scale2x_noblur<unsigned char, uint16, Manip8to565>
		(ibuf->get_bits(), x, y, w, h,
		    ibuf->line_width, ibuf->height, 
		    (uint16 *) scaled_surface->pixels, 
			scaled_surface->pitch/
				scaled_surface->format->BytesPerPixel,
			manip);
	SDL_UpdateRect(scaled_surface, 2*x, 2*y, 2*w, 2*h);
	}

void Image_window::show_scaled8to32_2x_noblur
	(
	int x, int y, int w, int h	// Area to show.
	)
	{
	Manip8to32 manip(surface->format->palette->colors,
						scaled_surface->format);
	Scale2x_noblur<unsigned char, uint32, Manip8to32>
		(ibuf->get_bits(), x, y, w, h,
		    ibuf->line_width, ibuf->height, 
		    (uint32 *) scaled_surface->pixels, 
			scaled_surface->pitch/
				scaled_surface->format->BytesPerPixel,
			manip);
	SDL_UpdateRect(scaled_surface, 2*x, 2*y, 2*w, 2*h);
	}
