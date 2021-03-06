#ifndef	__Astar_h_
#define	__Astar_h_

#include "PathFinder.h"



class	Astar: public virtual PathFinder
	{
	// Find a path from sx,sy to dx,dy
	// Return 0 if no path can be traced.
	// Return !0 if path found
	virtual	int	NewPath(int sx,int sy,int dx,int dy,int (*tileclassifier)(int,int));

	// Retrieve the coordinates of the next step on the path
	virtual	int	GetNextStep(int &nx,int &ny);

public:
	virtual ~Astar();
	};

#endif
