/**	-*-tab-width: 8; -*-
Copyright (C) 2000  Dancer A.L Vesperman

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef	__Astar_h_
#define	__Astar_h_

#include "PathFinder.h"


class	Astar: public virtual PathFinder
	{
	Tile_coord *path;		// Coords. to goal, ending with -1's.
	int next_index;			// Index of next tile to return.
public:
	Astar() : path(0), next_index(0)
		{  }
	// Find a path from sx,sy,sz to dx,dy,dz
	// Return 0 if no path can be traced.
	// Return !0 if path found
	virtual	int	NewPath(Tile_coord s, Tile_coord d,
						int (*tileclassifier)(int,int,int&));

	// Retrieve the coordinates of the next step on the path
	virtual	int	GetNextStep(Tile_coord& n);

	virtual ~Astar();
	};

#endif
