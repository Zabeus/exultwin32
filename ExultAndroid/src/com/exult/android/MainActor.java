package com.exult.android;

public class MainActor extends Actor {
	static Tile stepFrom = new Tile();
	public MainActor(String nm, int shapenum) {
		super(nm, shapenum, -1, -1);
		frames = avatarFrames; 
	}
	/*
	 *	Handle a time event (for TimeSensitive).
	 */
	public void handleEvent(int ctime, Object udata) {
		/* ++++++++FINISH
		if (action)			// Doing anything?
				{			// Do what we should.
				int speed = action->get_speed();
				int delay = action->handle_event(this);
				if (!delay)
					{	// Action finished.
						// This makes for a smoother scrolling and prevents the
						// avatar from skipping a step when walking.
					frame_time = speed;
					if (!frame_time)	// Not a path. Add a delay anyway.
						frame_time = gwin->get_std_delay();
					delay = frame_time;
					set_action(0);
					}

				gwin->get_tqueue()->add(
						curtime + delay, this, udata);
				}
			else if (in_usecode_control() || get_flag(Obj_flags::paralyzed))
				// Keep trying if we are in usecode control.
				gwin->get_tqueue()->add(
						curtime + gwin->get_std_delay(), this, udata);
			else if (schedule)
				schedule->now_what();
	*/
	}
	public boolean step(Tile t, int frame, boolean force) {
		restTime = 0;			// Reset counter.
		t.fixme();
						// Get chunk.
		int cx = t.tx/EConst.c_tiles_per_chunk, cy = t.ty/EConst.c_tiles_per_chunk;
						// Get rel. tile coords.
		int tx = t.tx%EConst.c_tiles_per_chunk, ty = t.ty%EConst.c_tiles_per_chunk;
		MapChunk nlist = gmap.getChunk(cx, cy);
		/*
		boolean water, poison;		// Get tile info.
		get_tile_info(this, gwin, nlist, tx, ty, water, poison);
		if (is_blocked(t, 0, force ? MOVE_ALL : 0))
			{
			if (is_really_blocked(t, force))
				{
				if (schedule)		// Tell scheduler.
					schedule->set_blocked(t);
				stop();
				return (0);
				}
			}
		if (poison && t.tz == 0)
			Actor::set_flag(static_cast<int>(Obj_flags::poisoned));
		*/
						// Check for scrolling.
		gwin.scrollIfNeeded(this, t);
		addDirty(false);			// Set to update old location.
						// Get old chunk, old tile.
		MapChunk olist = getChunk();
		getTile(stepFrom);
						// Move it.
		movef(olist, nlist, tx, ty, frame, t.tz);
		addDirty(true);			// Set to update new.
						// In a new chunk?
		if (olist != nlist)
			this.switchedChunks(olist, nlist);
		/* +++++++++FINISH
		int roof_height = nlist->is_roof (tx, ty, t.tz);
		gwin->set_ice_dungeon(nlist->is_ice_dungeon(tx, ty));
		if (gwin->set_above_main_actor (roof_height))
			{
			gwin->set_in_dungeon(nlist->has_dungeon()?
						nlist->is_dungeon(tx, ty):0);
			gwin->set_all_dirty();
			}
		else if (roof_height < 31 && gwin->set_in_dungeon(nlist->has_dungeon()?
	 					nlist->is_dungeon(tx, ty):0))
			gwin->set_all_dirty();
						// Near an egg?  (Do this last, since
						//   it may teleport.)
		nlist->activate_eggs(this, t.tx, t.ty, t.tz,
							stepFrom.tx, stepFrom.ty);
		quake_on_walk();
		*/
		return true;
	}
	public void switchedChunks(MapChunk olist, MapChunk nlist) {
		int newcx = nlist.getCx(), newcy = nlist.getCy();
		int xfrom, xto, yfrom, yto;	// Get range of chunks.
		if (olist == null ||		// No old, or new map?  Use all 9.
		     olist.getMap() != nlist.getMap()) {
			xfrom = newcx > 0 ? newcx - 1 : newcx;
			xto = newcx < EConst.c_num_chunks - 1 ? newcx + 1 : newcx;
			yfrom = newcy > 0 ? newcy - 1 : newcy;
			yto = newcy < EConst.c_num_chunks - 1 ? newcy + 1 : newcy;
		} else {
			int oldcx = olist.getCx(), oldcy = olist.getCy();
			if (newcx == oldcx + 1)
				{
				xfrom = newcx;
				xto = newcx < EConst.c_num_chunks - 1 ? newcx + 1 : newcx;
				}
			else if (newcx == oldcx - 1)
				{
				xfrom = newcx > 0 ? newcx - 1 : newcx;
				xto = newcx;
				}
			else
				{
				xfrom = newcx > 0 ? newcx - 1 : newcx;
				xto = newcx < EConst.c_num_chunks - 1 ? newcx + 1 : newcx;
				}
			if (newcy == oldcy + 1)
				{
				yfrom = newcy;
				yto = newcy < EConst.c_num_chunks - 1 ? newcy + 1 : newcy;
				}
			else if (newcy == oldcy - 1)
				{
				yfrom = newcy > 0 ? newcy - 1 : newcy;
				yto = newcy;
				}
			else
				{
				yfrom = newcy > 0 ? newcy - 1 : newcy;
				yto = newcy < EConst.c_num_chunks - 1 ? newcy + 1 : newcy;
				}
			}
		for (int y = yfrom; y <= yto; y++)
			for (int x = xfrom; x <= xto; x++)
				nlist.getMap().getChunk(Tile.fix(x), Tile.fix(y)).setupCache();

		// If change in Superchunk number, apply Old Style caching emulation
		// +++++FINISH gwin->emulate_cache(olist, nlist);
	}
}