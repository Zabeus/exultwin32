package com.exult.android;
import android.graphics.Canvas;
import android.graphics.Point;

public final class Mouse extends GameSingletons {
	private VgaFile.ShapeFile pointers; // Pointers from 'pointers.shp'.
	private ImageBuf backup;			// Stores image below mouse shape.Rectangle box;			// Area backed up.
	Rectangle dirty;		// Dirty area from mouse move.
	Rectangle box;			// Area backed up.
	Point avLoc;
	int mousex, mousey;		// Last place where mouse was.
	int cur_framenum;		// Frame # of current shape.
	ShapeFrame cur;		// Current shape.
	boolean onscreen;			// true if mouse is drawn on screen.
	// Frame #'s of short arrows, by int (0-7, 0=east).
	private static int shortArrows[] =
							{8, 9, 10, 11, 12, 13, 14, 15};
	private static int medArrows[] =  {16, 17, 18, 19, 20, 21, 22, 23};
	private static int longArrows[] = {24, 25, 26, 27, 28, 29, 30, 31};
	private static int shortCombatArrows[] = 
							{32, 33, 34, 35, 36, 37, 38, 39};
	private static int medCombatArrows[] =
							{40, 41, 42, 43, 44, 45, 46, 47};
	private void setShape0(int framenum) {	// Set shape without checking first.
		cur_framenum = framenum;
		cur = pointers.getFrame(framenum); 
		while (cur == null)			// For newly-created games.
			cur = pointers.getFrame(--framenum);
						// Set backup box to cover mouse.
		box.x = mousex - cur.getXLeft();
		box.y = mousey - cur.getYAbove();
		dirty.add(box);		// Update dirty area.
	}
	private void init() {
		dirty = new Rectangle();
		box = new Rectangle();
		avLoc = new Point();
		int cnt = pointers.getNumFrames();
		int maxleft = 0, maxright = 0, maxabove = 0, maxbelow = 0;
		for (int i = 0; i < cnt; i++) {
			ShapeFrame frame = pointers.getFrame(i);
			int xleft = frame.getXLeft(), xright = frame.getXRight();
			int yabove = frame.getYAbove(), ybelow = frame.getYBelow();
			if (xleft > maxleft)
				maxleft = xleft;
			if (xright > maxright)
				maxright = xright;
			if (yabove > maxabove)
				maxabove = yabove;
			if (ybelow > maxbelow)
				maxbelow = ybelow;
		}
		int maxw = maxleft + maxright, maxh = maxabove + maxbelow;
						// Create backup buffer.
		backup = new ImageBuf(maxw, maxh);
		box.w = maxw;
		box.h = maxh;
		
		onscreen = false;                   // initially offscreen
	}
	public static final int // enum int :List of shapes' frame #'s.
		dontchange = 1000,	// Flag to not change.
		hand = 0,
		redx = 1,
		greenselect = 2,	// For modal select.
		tooheavy = 3,
		outofrange = 4,
		outofammo = 5,
		wontfit = 6,
		hourglass = 7,
		greensquare = 23,
		blocked = 49;

	/* Avatar speed, relative to standard delay:
	 * avatarSpeed = standard_delay * 100 / avatarSpeedFactor
	 *
	 *
	 * Experimental results, Serpent Isle
	 *
	 * "short" arrow within central 0.4 of screen in each dimension
	 * "middle" arrow within central 0.8 of screen in each dimension
	 * "long" arrow (non-combat, non-threat only) outside 
	 *
	 * relative speeds:
         * (movement type           - time for a certain dist. - rel. speed)
	 *  non-combat short arrow  -          8               -   1
	 *  non-combat medium arrow -          4               -   2
	 *  non-combat long arrow   -          2               -   4
	 *  combat short arrow      -          8               -   1
	 *  combat medium arrow     -          6               -   4/3
	 */
	public static final int // enum AvatarSpeedFactors
		slowSpeedFactor          = 100,
		mediumCombatSpeedFactor = 150,
		mediumSpeedFactor        = 200,
		fastSpeedFactor          = 400;
	public int avatarSpeed;

	static boolean mouse_update;
	static Mouse mouse;

	Mouse() {
		pointers = new VgaFile.ShapeFile(EFile.POINTERS);
		init();
	}
	void show() { // Paint it.
		if (!onscreen){
			onscreen = true;
						// Save background.
			gwin.getWin().get(backup, box.x, box.y);
						// Paint new location.
			cur.paintRle(gwin.getWin(), mousex, mousey);
		}
	}
	void hide() {			// Restore area under mouse.
		if (onscreen) {
			onscreen = false;
			gwin.getWin().put(backup, box.x, box.y);
			dirty.set(box);	// Init. dirty to box.
			}
	}
	void setShape(int framenum) {	// Set to desired shape.
		if (framenum != cur_framenum)
			setShape0(framenum);
		}
	int getShape()
		{ return cur_framenum; }
	void move(int x, int y) {	// Move to new location (mouse motion).
		// Shift to new position.
		box.shift(x - mousex, y - mousey);
		dirty.add(box);	// Enlarge dirty area.
		mousex = x;
		mousey = y;
	}
	void blitDirty(Canvas c) {	// Blit dirty area.
		gwin.getWin().show(c, dirty.x - 1, dirty.y - 1, dirty.w + 2, 
							dirty.h + 2); 
		}
	void setLocation(int x, int y) {// Set to given location.
		mousex = x;
		mousey = y;
		box.x = mousex - cur.getXLeft();
		box.y = mousey - cur.getYAbove();
	}
					// Flash desired shape for 1/2 sec.
	void flashShape(int flash) {
		//+++++++++++FINISH
	}
					// Set to short arrow.
	int getShortArrow(int dir)
		{ return (shortArrows[(dir)]); }
					// Set to medium arrow.
	int getMediumArrow(int dir)
		{ return (medArrows[(dir)]); }
					// Set to long arrow.
	int getLongArrow(int dir)
		{ return (longArrows[(dir)]); }
					// Set to short combat mode arrow.
	int getShortCombatArrow(int dir)
		{ return (shortCombatArrows[(dir)]); }
					// Set to medium combat mode arrow.
	int getMediumCombatArrow(int dir)
		{ return (medCombatArrows[(dir)]); }

	boolean isOnscreen() { 
		return onscreen; 
	}
	// Sets hand or speed cursors
	void setSpeedCursor() {
		int cursor = dontchange;
		int ax, ay;			// Get Avatar/barge screen location.

		// Check if we are in dont_move mode, in this case display the hand cursor
		if (gwin.mainActorDontMove())
				cursor = hand;
		else if (gumpman.gumpMode()) {
			cursor = hand;
		}
/* +++++++FINISH
		else if (gwin.getDraggingGump()) 
			cursor = hand;
		
		else if (Combat::is_paused())
			cursor = shortCombat_arrows[0];	// Short N red arrow.
		*/
		if (cursor == dontchange) {
			/* ++++++++++FINISH
			Barge_object *barge = gwin.get_moving_barge();
			if (barge)
			{			// Use center of barge.
				gwin.get_shape_location(barge, ax, ay);
				ax -= barge.get_xtiles()*(c_tilesize/2);
				ay -= barge.get_ytiles()*(c_tilesize/2);
			}
			else	
			*/
			{
				gwin.getShapeLocation(avLoc, gwin.getMainActor());
				ax = avLoc.x; ay = avLoc.y;
			}
			int dy = ay - mousey, dx = mousex - ax;
			int dir = EUtil.getDirection(dy, dx);
			int gamew = gwin.getWidth(), gameh = gwin.getHeight();
			float speed_section = Math.max( Math.max( -(float)dx/ax, (float)dx/(gamew-ax)), 
					Math.max((float)dy/ay, -(float)dy/(gameh-ay)) );
			boolean nearby_hostile = false; //+++++++ gwin.isHostileNearby();
			boolean has_active_nohalt_scr = false;
			UsecodeScript scr = null;
			Actor act = gwin.getMainActor();
			while ((scr = UsecodeScript.findActive(act, scr)) != null)
				// We should only be here is scripts are nohalt, but just
				// in case...
				if (scr.isNoHalt()) {
					has_active_nohalt_scr = true;
					break;
				}

			int baseSpeed = 200;
			if(speed_section < 0.4 ) {
				if( gwin.inCombat() )
					cursor = getShortCombatArrow( dir );
				else
					cursor = getShortArrow( dir );
				avatarSpeed = baseSpeed/slowSpeedFactor;
			}
			else if( speed_section < 0.8 || gwin.inCombat() || nearby_hostile 
						|| has_active_nohalt_scr) {
				if( gwin.inCombat() )
					cursor = getMediumCombatArrow( dir );
				else
					cursor = getMediumArrow( dir );
				if( gwin.inCombat() || nearby_hostile )
					avatarSpeed = baseSpeed/mediumCombatSpeedFactor;
				else
					avatarSpeed = baseSpeed/mediumSpeedFactor;
			}
			else /* Fast - NB, we can't get here in combat mode; there is no
			      * long combat arrow, nor is there a fast combat speed. */
			{
			cursor = getLongArrow( dir );
				avatarSpeed = baseSpeed/fastSpeedFactor;
			}
		}
		
		if (cursor != dontchange)
			setShape(cursor);
	}
}