/**	-*-mode: Fundamental; tab-width: 8; -*-
 **
 **	Gamewin.cc - X-windows Ultima7 map browser.
 **
 **	Written: 7/22/98 - JSF
 **/

/*
Copyright (C) 1998  Jeffrey S. Freedman

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

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#if defined(MACOS)
  #include <stat.h>
#else
  #include <sys/stat.h>
#endif
#include "gamewin.h"
#include "egg.h"
#include "items.h"
#include "utils.h"
#include "fnames.h"
#include "usecode.h"
#include "npcnear.h"
#include "spells.h"
#include "effects.h"
#include "segfile.h"
#include "Audio.h"
#include "files/U7file.h"
#include "flic/playfli.h"
#include "Configuration.h"
#include "schedule.h"

extern	Configuration *config;
					// THE game window:
Game_window *Game_window::game_window = 0;

/*
 *	Create game window.
 */

Game_window::Game_window
	(
	int width, int height		// Window dimensions.
	) : 
	    usecode(new Usecode_machine(this)),mode(splash), combat(0),
            tqueue(new Time_queue()), clock(tqueue),
	    npc_prox(new Npc_proximity_handler(this)),
	    effects(0), open_gumps(0), num_faces(0), last_face_shown(-1),
	    conv_choices(0), painted(0), focus(1), shapes(),
	    faces(FACES_VGA), gumps(GUMPS_VGA), fonts(FONTS_VGA),
	    sprites(SPRITES_VGA), mainshp(MAINSHP_FLX),
	    endshape(ENDSHAPE_FLX),
	    main_actor(0), skip_above_actor(31), npcs(0),
	    monster_info(0), 
	    scrolltx(64*tiles_per_chunk), 
	    scrollty(136*tiles_per_chunk), 	// Start in Trinsic (BG).
	    palette(-1), brightness(100), user_brightness(100), faded_out(0),
	    dragging(0), dragging_save(0),
	    skip_lift(16), paint_eggs(1), debug(0)
	{
	game_window = this;		// Set static ->.
	if (!shapes.is_good())
		abort("Can't open 'shapes.vga' file.");
	if (!faces.is_good())
		abort("Can't open 'faces.vga' file.");
	if (!gumps.is_good())
		abort("Can't open 'gumps.vga' file.");
	if (!fonts.is_good())
		abort("Can't open 'fonts.vga' file.");
	if (!sprites.is_good())
		abort("Can't open 'sprites.vga' file.");
	
	u7open(chunks, U7CHUNKS);
	u7open(u7map, U7MAP);
	ifstream textflx;	
  	u7open(textflx, TEXT_FLX);
	Setup_item_names(textflx);	// Set up list of item names.
					// Read in shape dimensions.
	if (!shapes.read_info())
		abort(
		"Can't read shape data (tfa.dat, wgtvol.dat, shpdims.dat).");
	Segment_file xf(XFORMTBL);	// Read in translucency tables.
	int len, nxforms = sizeof(xforms)/sizeof(xforms[0]);
	for (int i = 0; i < nxforms; i++)
		if (!xf.read_segment(i, xforms[nxforms - 1 - i], len))
			abort("Error reading %s.", XFORMTBL);
	struct stat sbuf;		// Create gamedat files 1st time.
	if (stat(U7NBUF_DAT, &sbuf) != 0 &&
	    stat(NPC_DAT, &sbuf) != 0)
		{
		cout << "Creating 'gamedat' files."<<endl;
		restore_gamedat(INITGAME);
		}
	else
		{
			cout << "Checking consistency of static/gamedat: ";
			char *static_identity = get_game_identity(INITGAME);
			cout << static_identity << "/";
			ifstream identity_file;
			u7open(identity_file, IDENTITY);
			char gamedat_identity[256];
			identity_file.read(gamedat_identity, 256);
			char *ptr = gamedat_identity;
			for(; (*ptr!=0x1a && *ptr!=0x0d); ptr++);
			*ptr = 0;
			cout << gamedat_identity;
			if(strcmp(static_identity, gamedat_identity))
				{
					cout << " ->BAD" << endl;
					cout << "Creating 'gamedat' files."<<endl;
					restore_gamedat(INITGAME);
				}
			else
				{
					cout << " ->GOOD" << endl;
				}
			delete[] static_identity;
			read_gwin();	// Read in 'gamewin.dat' to set clock,
					//   scroll coords.
		}
	read_save_names();		// Read in saved-game names.
	string	fullscreenstr;		// Check config. for fullscreen mode.
	bool	fullscreen=false;
	config->value("config/video/fullscreen",fullscreenstr,"no");
	if(fullscreenstr=="yes")
		fullscreen=true;
	config->set("config/video/fullscreen",fullscreenstr,true);
	int scale;			// Let 'config' override scale.
	config->value("config/video/scale", scale, 1);
					// Create 8-bit depth window.
	win = new Image_window8(width, height, scale, fullscreen);
					// Set title.
	win->set_title("Exult Ultima7 Engine");

	unsigned long timer = SDL_GetTicks();
	srand(timer);			// Use time to seed rand. generator.
					// Force clock to start.
	tqueue->add(timer, &clock, (long) this);
					// Clear object lists, flags.
#if 1
	for (int i1 = 0; i1 < num_chunks; i1++)
		for (int i2 = 0; i2 < num_chunks; i2++)
			objects[i1][i2] = 0;
#else	/* Old way +++++++*/
	memset((char *) objects, 0, sizeof(objects));
#endif
	memset((char *) schunk_read, 0, sizeof(schunk_read));
	clock.set_palette();		// Set palette for correct time.
	brighten(20);			// Brighten 20%.
	}

/*
 *	Deleting game window.
 */

Game_window::~Game_window
	(
	)
	{
	clear_world();			// Delete all objects, chunks.
	delete win;
	delete dragging_save;
	delete [] conv_choices;
	delete usecode;
	}

/*
 *	Abort.
 */

void Game_window::abort
	(
	const char *msg,
	...
	)
	{
	va_list ap;
	va_start(ap, msg);
	char buf[512];
	vsprintf(buf, msg, ap);		// Format the message.
	cerr << "Exult (fatal): " << buf << endl;
	delete this;
	exit(-1);
	}

/* 
 *	Get monster info for a given shape.
 */

Monster_info *Game_window::get_monster_info
	(
	int shapenum
	)
	{
	for (int i = 0; i < num_monsters; i++)
		if (shapenum == monster_info[i].get_shapenum())
			return &monster_info[i];
	return (0);
	}

/*
 *	Add a 'path' egg to our list.
 */

void Game_window::add_path_egg
	(
	Egg_object *egg
	)
	{
	int qual = egg->get_quality();
	if (qual >= 0 && qual < 255)
		{
		path_eggs.reserve(qual + 1);
		path_eggs[qual] = egg;
		}
	}

/*
 *	Toggle combat mode.
 */

void Game_window::toggle_combat
	(
	)
	{ 
	combat = !combat;
					// Change party member's schedules.
	int newsched = combat ? Schedule::combat : Schedule::follow_avatar;
	int cnt = usecode->get_party_count();
	for (int i = 0; i < cnt; i++)
		{
		int party_member=usecode->get_party_member(i);
		Actor *person=get_npc(party_member);
		if (person && person->get_schedule_type() != newsched)
			person->set_schedule_type(newsched);
		}
	if (main_actor->get_schedule_type() != newsched)
		main_actor->set_schedule_type(newsched);
	}

/*
 *	Resize event occurred.
 */

void Game_window::resized
	(
	unsigned int neww, 
	unsigned int newh
	)
	{			
	win->resized(neww, newh);
	paint();
	}

/*
 *	Open a file, trying the original name (lower case), and the upper
 *	case version of the name.  If it can't be opened either way, we
 *	abort with an error message.
 *
 *	Output: 0 if couldn't open and dont_abort == 1.
 */

int Game_window::u7open
	(
	ifstream& in,			// Input stream to open.
	const char *fname,			// May be converted to upper-case.
	int dont_abort			// 1 to just return 0.
	)
	{
	if (!U7open(in, fname))
		if (dont_abort)
			return (0);
		else
			abort("Can't open '%s'.", fname);
	return (1);
	}

/*
 *	Set the scroll boundaries.
 */

void Game_window::set_scroll_bounds
	(
	)
	{
					// Let's try 6x6 tiles.
	scroll_bounds.w = scroll_bounds.h = 6;
	scroll_bounds.x = scrolltx + 
			(get_width()/tilesize - scroll_bounds.w)/2;
	scroll_bounds.y = scrollty + 
			(get_height()/tilesize - scroll_bounds.h)/2;
	}

/*
 *	Clear out world's contents.  Should be used during a 'restore'.
 */

void Game_window::clear_world
	(
	)
	{
	delete tqueue;			// Want a fresh queue.
	tqueue = new Time_queue();
	clear_dirty();
					// Delete all chunks (& their objs).
	for (int y = 0; y < num_chunks; y++)
		for (int x = 0; x < num_chunks; x++)
			{
			delete objects[x][y];
			objects[x][y] = 0;
			}
	Monster_actor::delete_all();	// To be safe, del. any still around.
	Dead_body::delete_all();
	main_actor = 0;
	num_npcs = num_npcs1 = 0;
	delete [] npcs;			// NPC's already deleted above.
		//++++++++Clear monsters list when we have it.
					// Clear 'read' flags.
	memset((char *) schunk_read, 0, sizeof(schunk_read));
	}

/*
 *	Center view around a given tile.
 */

void Game_window::center_view
	(
	Tile_coord t
	)
	{
					// Figure in tiles.
	int tw = get_width()/tilesize, th = get_height()/tilesize;
	scrolltx = t.tx - tw/2;
	scrollty = t.ty - th/2;
	if (scrolltx < 0)
		scrolltx = 0;
	if (scrollty < 0)
		scrollty = 0;
	if (scrolltx + tw > num_chunks*tiles_per_chunk)
		scrolltx = num_chunks*tiles_per_chunk - tw - 1;
	if (scrollty + th > num_chunks*tiles_per_chunk)
		scrollty = num_chunks*tiles_per_chunk - th - 1;
	set_scroll_bounds();		// Set scroll-control.
					// See who's nearby.
	add_nearby_npcs(scrolltx/tiles_per_chunk, scrollty/tiles_per_chunk,
		(scrolltx + get_width()/tilesize)/tiles_per_chunk,
		(scrollty + get_height()/tilesize)/tiles_per_chunk);
	paint();
	}

/*
 *	Scroll if necessary.
 *
 *	Output:	1 if scrolled (screen updated).
 */

int Game_window::scroll_if_needed
	(
	Tile_coord t
	)
	{
	int scrolled = 0;
	if (t.tx == scroll_bounds.x - 1)
		{
		view_left();
		scrolled = 1;
		}
	else if (t.tx == scroll_bounds.x + scroll_bounds.w)
		{
		view_right();
		scrolled = 1;
		}
	if (t.ty == scroll_bounds.y - 1)
		{
		view_up();
		scrolled = 1;
		}
	else if (t.ty == scroll_bounds.y + scroll_bounds.h)
		{
		view_down();
		scrolled = 1;
		}
	return (scrolled);
	}

/*
 *	Show the absolute game location, where each coordinate is of the
 *	8x8 tiles clicked on.
 */

void Game_window::show_game_location
	(
	int x, int y			// Point on screen.
	)
	{
	x = get_scrolltx() + x/tilesize;
	y = get_scrollty() + y/tilesize;
	cout << "Game location is (" << x << ", " << y << ")"<<endl;
	}

/*
 *	Get screen area used by a gump.
 */

Rectangle Game_window::get_gump_rect
	(
	Gump_object *gump
	)
	{
	Shape_frame *s = gumps.get_shape(gump->get_shapenum(),
						gump->get_framenum());
	return Rectangle(gump->get_x() - s->xleft, 
			gump->get_y() - s->yabove,
					s->get_width(), s->get_height());
	}

/*
 *	Get the map objects and scenery for a superchunk.
 */

void Game_window::get_map_objects
	(
	int schunk			// Superchunk # (0-143).
	)
	{
	u7map.seekg(schunk * 16*16*2);	// Get to desired chunk.
	unsigned char buf[16*16*2];
	u7map.read((char*)buf, sizeof(buf));	// Read in the chunk #'s.
	int scy = 16*(schunk/12);	// Get abs. chunk coords.
	int scx = 16*(schunk%12);
	unsigned char *mapdata = buf;
					// Go through chunks.
	for (int cy = 0; cy < 16; cy++)
		for (int cx = 0; cx < 16; cx++)
			{
					// Get chunk #.
			int chunk_num = *mapdata++;
			chunk_num += (*mapdata++) << 8;
			get_chunk_objects(scx + cx, scy + cy, chunk_num);
			}
	}

/*
 *	Read in graphics data into window's image.
 */

void Game_window::get_chunk_objects
	(
	int cx, int cy,			// Chunk index within map.
	int chunk_num			// Desired chunk # within file.
	)
	{
	chunks.seekg(chunk_num * 512);	// Get to desired chunk.
	unsigned char buf[16*16*2];	// Read in 16x16 2-byte shape #'s.
	chunks.read((char*)buf, sizeof(buf));
	unsigned char *data = &buf[0];
					// Get list we'll store into.
	Chunk_object_list *olist = get_objects(cx, cy);
					// A chunk is 16x16 tiles.
	for (int tiley = 0; tiley < tiles_per_chunk; tiley++)
		for (int tilex = 0; tilex < tiles_per_chunk; tilex++)
			{
			ShapeID id(data[0], (unsigned char) (data[1]&0x7f));
			Shape_frame *shape = get_shape(id);
			if (!shape)
				{
				cout << "Chunk shape is null!"<<endl;
				data += 2;
				continue;
				}
			if (shape->rle)
				{
				int shapenum = id.get_shapenum();
				Shape_info& info = shapes.get_info(shapenum);
				Game_object *obj = info.is_animated() ?
					new Animated_object(
					    data[0], data[1], tilex, tiley)
					: new Game_object(
					    data[0], data[1], tilex, tiley);
				olist->add(obj);
				}
			else		// Flat.
				olist->set_flat(tilex, tiley, id);
			data += 2;
			}
	}


/*
 *	Read in the objects for a superchunk from one of the "u7ifix" files.
 */

void Game_window::get_ifix_objects
	(
	int schunk			// Superchunk # (0-143).
	)
	{
	char fname[128];		// Set up name.
	strcpy(fname, U7IFIX);
	int len = strlen(fname);
	fname[len] = '0' + schunk/16;
	int lb = schunk%16;
	fname[len + 1] = lb < 10 ? ('0' + lb) : ('a' + (lb - 10));
	fname[len + 2] = 0;
	ifstream ifix;			// There it is.
	u7open(ifix, fname);
	int scy = 16*(schunk/12);	// Get abs. chunk coords.
	int scx = 16*(schunk%12);
					// Go through chunks.
	for (int cy = 0; cy < 16; cy++)
		for (int cx = 0; cx < 16; cx++)
			{
					// Get to index entry for chunk.
			int chunk_num = cy*16 + cx;
			ifix.seekg(0x80 + chunk_num*8);
					// Get location, length.
			long shapesoff = Read4(ifix);
			if (!shapesoff) // Nothing there?
				continue;
			unsigned long shapeslen = Read4(ifix);
			get_ifix_chunk_objects(ifix, shapesoff, shapeslen/4,
				scx + cx, scy + cy);
			}
	}

/*
 *	Get the objects from one ifix chunk entry onto the screen.
 */

void Game_window::get_ifix_chunk_objects
	(
	ifstream& ifix,
	long filepos,			// Where chunk's data lies.
	int cnt,			// # entries (objects).
	int cx, int cy			// Absolute chunk #'s.
	)
	{
	ifix.seekg(filepos);		// Get to actual shape.
					// Get buffer to hold entries' indices.
	unsigned char *entries = new unsigned char[4*cnt];
	unsigned char *ent = entries;
	ifix.read((char*)entries, 4*cnt);	// Read them in.
					// Get object list for chunk.
	Chunk_object_list *olist = get_objects(cx, cy);
	for (int i = 0; i < cnt; i++, ent += 4)
		{
		Game_object *obj = new Game_object(ent);
		olist->add(obj);
		}
	delete[] entries;		// Done with buffer.
	}

/*
 *	Get the name of an ireg file.
 *
 *	Output:	->fname, where name is stored.
 */

char *Game_window::get_ireg_name
	(
	int schunk,			// Superchunk # (0-143).
	char *fname			// Name is stored here.
	)
	{
	strcpy(fname, U7IREG);
	int len = strlen(fname);
	fname[len] = '0' + schunk/16;
	int lb = schunk%16;
	fname[len + 1] = lb < 10 ? ('0' + lb) : ('a' + (lb - 10));
	fname[len + 2] = 0;
	return (fname);
	}

/*
 *	Write out one of the "u7ireg" files.
 *
 *	Output:	0 if error, which is reported.
 */

int Game_window::write_ireg_objects
	(
	int schunk			// Superchunk # (0-143).
	)
	{
	char fname[128];		// Set up name.
	ofstream ireg;			// There it is.
	if (!U7open(ireg, get_ireg_name(schunk, fname)))
		{			// +++++Better error???
		cerr << "Exult:  Error opening '" << fname <<
				"' for writing"<<endl;
		return (0);
		}
	int scy = 16*(schunk/12);	// Get abs. chunk coords.
	int scx = 16*(schunk%12);
					// Go through chunks.
	for (int cy = 0; cy < 16; cy++)
		for (int cx = 0; cx < 16; cx++)
			{
			Chunk_object_list *chunk = get_objects(scx + cx,
							       scy + cy);
			for (Game_object *obj = chunk->get_first(); obj;
						obj = chunk->get_next(obj))
				obj->write_ireg(ireg);
			Write2(ireg, 0);// End with 2 0's.
			}
	ireg.flush();
	int result = ireg.good();
	if (!result)			// ++++Better error system needed??
		cerr << "Exult:  Error writing '" << fname << "'"<<endl;
	return (result);
	}

/*
 *	Read in the objects for a superchunk from one of the "u7ireg" files.
 *	(These are the moveable objects.)
 */

void Game_window::get_ireg_objects
	(
	int schunk			// Superchunk # (0-143).
	)
	{
	char fname[128];		// Set up name.
	ifstream ireg;			// There it is.
	if (!u7open(ireg, get_ireg_name(schunk, fname), 1))
		return;			// Just don't show them.
	int scy = 16*(schunk/12);	// Get abs. chunk coords.
	int scx = 16*(schunk%12);
	read_ireg_objects(ireg, scx, scy);
	}

/*
 *	Read a list of ireg objects.  They are either placed in the desired
 *	game chunk, or added to their container.
 */

void Game_window::read_ireg_objects
	(
	istream& ireg,			// File to read from.
	int scx, int scy,		// Abs. chunk coords. of superchunk.
	Game_object *container		// Container, or null.
	)
	{
	int entlen;			// Gets entry length.
					// Go through entries.
	while (((entlen = Read1(ireg), ireg.good())))
		{
		if (!entlen || entlen == 1)
			if (container)
				return;	// Skip 0's & ends of containers.
			else
				continue;
		if (entlen != 6 && entlen != 12 && entlen != 18)
			{
			long pos = ireg.tellg();
			cout << "Unknown entlen " << entlen << " at pos. " <<
					pos << endl;
			ireg.seekg(pos + entlen);
			continue;	// Only know these two types.
			}
		unsigned char entry[18];// Get entry.
		ireg.read((char*)entry, entlen);
		int cx = entry[0] >> 4; // Get chunk indices within schunk.
		int cy = entry[1] >> 4;
					// Get coord. #'s where shape goes.
		int tilex = entry[0] & 0xf;
		int tiley = entry[1] & 0xf;
					// Get shape #.
		int shapeid = entry[2]+256*(entry[3]&3);
		Shape_info& info = shapes.get_info(shapeid);
		unsigned int lift, quality, type;
		Game_object *obj;
					// An "egg"?
		if (info.get_shape_class() == Shape_info::hatchable)
			{
			Egg_object *egg = create_egg(entry,
							info.is_animated());
			get_objects(scx + cx, scy + cy)->add_egg(egg);
			continue;
			}
		else if (entlen == 6)	// Simple entry?
			{
			type = 0;
			lift = entry[4] >> 4;
			quality = entry[5];
			if (info.is_animated())
				obj = new Animated_object(
				   entry[2], entry[3], tilex, tiley, lift, 1);
			else
				obj = new Ireg_game_object(
				   entry[2], entry[3], tilex, tiley, lift);
			}
		else if (entlen == 12)	// Container?
			{
			type = entry[4] + 256*entry[5];
			lift = entry[9] >> 4;
			quality = entry[7];
			if (shapeid == 961)
				obj = new Barge_object(
				    entry[2], entry[3], tilex, tiley, lift);
			else if (quality == 1 && entry[8] >= 0x80)
				obj = new Dead_body(
				    entry[2], entry[3], tilex, tiley, lift,
							entry[8] - 0x80);
			else
				obj = new Container_game_object(
				    entry[2], entry[3], tilex, tiley, lift);
					// Read container's objects.
			if (type)	// ???Don't understand this yet.
				read_ireg_objects(ireg, scx, scy, obj);
			}
		else			// Length 18 means it's a spellbook.
			{		// Get all 9 spell bytes.
			quality = 0;
			unsigned char circles[9];
			memcpy(&circles[0], &entry[4], 5);
			lift = entry[9] >> 4;
			memcpy(&circles[5], &entry[10], 4);
			unsigned char *ptr = &entry[14];
			unsigned long flags = Read4(ptr);
			obj = new Spellbook_object(
				entry[2], entry[3], tilex, tiley, lift,
				&circles[0], flags);
			}
		obj->set_quality(quality);
					// Add, but skip volume check.
		if (!container || !container->add(obj, 1))
			get_objects(scx + cx, scy + cy)->add(obj);
		}
	}

/*
 *	Create an "egg".
 */

Egg_object *Game_window::create_egg
	(
	unsigned char *entry,		// 1-byte ireg entry.
	int animated
	)
	{
	unsigned short type = entry[4] + 256*entry[5];
	int prob = entry[6];		// Probability (1-100).
	int data1 = entry[7] + 256*entry[8];
	int lift = entry[9] >> 4;
	int data2 = entry[10] + 256*entry[11];
	Egg_object *obj = animated ?
		new Animated_egg_object(entry[2], entry[3], 
			entry[0]&0xf, entry[1]&0xf, lift, type, prob,
						data1, data2)
		: new Egg_object(entry[2], entry[3], 
			entry[0]&0xf, entry[1]&0xf, lift, type, prob,
						data1, data2);
	return (obj);
	}

/*
 *	Read in the objects in a superchunk.
 */

void Game_window::get_superchunk_objects
	(
	int schunk			// Superchunk #.
	)
	{
	get_map_objects(schunk);	// Get map objects/scenery.
	get_ifix_objects(schunk);	// Get objects from ifix.
	get_ireg_objects(schunk);	// Get moveable objects.
	schunk_read[schunk] = 1;	// Done this one now.
	}

/*
 *	Put the actor(s) in the world.
 */

void Game_window::init_actors
	(
	)
	{
	if (main_actor)			// Already done?
		return;
	read_npcs();			// Read in all U7 NPC's.
	}

/*
 *	Save game by writing out to the 'gamedat' directory.
 *
 *	Output:	0 if error, already reported.
 */

int Game_window::write
	(
	)
	{
					// Write each superchunk to Iregxx.
	for (int schunk = 0; schunk < 12*12 - 1; schunk++)
					// Only write what we've read.
		if (schunk_read[schunk] && !write_ireg_objects(schunk))
			return (0);
	if (!write_npcs())		// Write out npc.dat.
		return (0);
	if (!usecode->write())		// Usecode.dat (party, global flags).
		return (0);
	return (write_gwin());		// Write our data.
	}

/*
 *	Restore game by reading in 'gamedat'.
 *
 *	Output:	0 if error, already reported.
 */

int Game_window::read
	(
	)
	{
	clear_world();			// Wipe clean.
	if (!read_gwin())		// Read our data.
		return (0);
	end_gump_mode();		// Kill gumps, and paint new data.
	read_npcs();			// Read in NPC's, monsters.
	if (!usecode->read())		// Usecode.dat (party, global flags).
		return (0);
	clock.set_palette();		// Set palette for time-of-day.
	set_all_dirty();		// Force entire repaint.
	return (1);
	}

/*
 *	Write data for the game.
 *
 *	Output:	0 if error.
 */

int Game_window::write_gwin
	(
	)
	{
	ofstream gout;
	if (!U7open(gout, GWINDAT))	// Gamewin.dat.
		return (0);
					// Start with scroll coords (in tiles).
	Write2(gout, get_scrolltx());
	Write2(gout, get_scrollty());
					// Write clock.
	Write2(gout, clock.get_day());
	Write2(gout, clock.get_hour());
	Write2(gout, clock.get_minute());
	gout.flush();
	return (gout.good());
	}

/*
 *	Read data for the game.
 *
 *	Output:	0 if error.
 */

int Game_window::read_gwin
	(
	)
	{
	ifstream gin;
	if (!U7open(gin, GWINDAT))	// Gamewin.dat.
		return (0);
					// Start with scroll coords (in tiles).
	scrolltx = Read2(gin);
	scrollty = Read2(gin);
					// Read clock.
	clock.set_day(Read2(gin));
	clock.set_hour(Read2(gin));
	clock.set_minute(Read2(gin));
	return (gin.good());
	}

/*
 *	Read in superchunk data to cover the screen.
 */

void Game_window::read_map_data
	(
	)
	{
	int scrolltx = get_scrolltx(), scrollty = get_scrollty();
	int w = get_width(), h = get_height();
					// Start one tile to left.
	int firstsx = (scrolltx - 1)/tiles_per_schunk, 
	    firstsy = (scrollty - 1)/tiles_per_schunk;
					// End 8 tiles to right.
	int lastsx = (scrolltx + (w + tilesize - 2)/tilesize + 
					tiles_per_chunk/2)/tiles_per_schunk;
	int lastsy = (scrollty + (h + tilesize - 2)/tilesize + 
					tiles_per_chunk/2)/tiles_per_schunk;
	if (lastsx >= 12)		// Don't go past end.
		lastsx = 11;
	if (lastsy >= 12)
		lastsy = 11;
					// Read in "map", "ifix" objects for
					//  all visible superchunks.
	for (int sy = firstsy; sy <= lastsy; sy++)
		for (int sx = firstsx; sx <= lastsx; sx++)
			{
					// Figure superchunk #.
			int schunk = 12*sy + sx;
					// Read it if necessary.
			if (!schunk_read[schunk])
				get_superchunk_objects(schunk);
			}
	}	

/*
 *      Paint splash screen
 */

void Game_window::paint_splash
	(
	)
	{
		Vga_file shapes(ENDSHAPE_FLX);
		int x = get_width()/2-160;
		int y = get_height()/2-100;
		set_palette("static/intropal.dat",3);
		paint_shape(x,y,shapes.get_shape(0x11,0));
		paint_text_box(0,"With the help of Jeff Freedman, Dancer Vesperman, " \
				 "Willem Jan Palenstijn, Tristan Tarrant", x, y+160, 320, 200);
		set_palette("static/intropal.dat",3,1);
		SDL_Delay(2000);
		set_palette("static/intropal.dat",3,-1);
		
		
		paint_shape(x,y,shapes.get_shape(0x12,0));
		paint_shape(x+160,y+30,shapes.get_shape(0x0D,0));
		paint_text_box(0,"Driven by the Exult game engine", x, y+160, 320, 200);
		set_palette("static/intropal.dat",4,1);
		SDL_Delay(1500);
		set_palette("static/intropal.dat",4,-1);
	
		Vga_file menushapes(MAINSHP_FLX);
		
		paint_shape(x,y,menushapes.get_shape(0x2,0));
		set_palette("static/intropal.dat",0,1);
		
		paint_shape(x+120,y+110,menushapes.get_shape(0x4,0));
		paint_shape(x+120,y+120,menushapes.get_shape(0x5,0));
		paint_shape(x+120,y+130,menushapes.get_shape(0x8,0));
		paint_shape(x+120,y+140,menushapes.get_shape(0x6,0));
		show(1);
		SDL_Delay(1500);

		// The main man :)
		if(get_usecode()->get_global_flag(Usecode_machine::did_first_scene)==0)	{
		set_palette("static/intropal.dat",2,0);
		// First
		for(int i=9; i>0; i--) {
			win->fill8(0,get_width(),get_height(),0,0);
			paint_shape(x,y,shapes.get_shape(0x21,i));
			win->show();
			SDL_Delay(50);
		}
		for(int i=1; i<10; i++) {
			win->fill8(0,get_width(),get_height(),0,0);
			paint_shape(x,y,shapes.get_shape(0x21,i));
			win->show();
			SDL_Delay(50);
		}
		// Second 
		for(int i=0; i<10; i++) {
			win->fill8(0,get_width(),get_height(),0,0);
			paint_shape(x,y,shapes.get_shape(0x22,i));
			win->show();
			SDL_Delay(50);
		}
		for(int i=9; i>=0; i--) {
			win->fill8(0,get_width(),get_height(),0,0);
			paint_shape(x,y,shapes.get_shape(0x22,i));
			win->show();
			SDL_Delay(50);
		}
		for(int i=0; i<16; i++) {
			win->fill8(0,get_width(),get_height(),0,0);
			paint_shape(x,y,shapes.get_shape(0x23,i));
			win->show();
			SDL_Delay(50);
		}
		paint_shape(x,y,shapes.get_shape(0x20,1));
		win->show();
		// Guardian speech
		audio->playfile(INTROSND,false);
		}
		win->fill8(0,get_width(),get_height(),0,0);
	}

/*
 *	Paint a rectangle in the window by pulling in vga chunks.
 */

void Game_window::paint
	(
	int x, int y, int w, int h	// Rectangle to cover.
	)
	{
	if (!win->ready())
		return;
	win->set_clip(x, y, w, h);	// Clip to this area.
#if 0	/* ++++Now called from exult.cc. */
	if (mode == splash) {
		paint_splash();
		return;
	}
#endif
	int light_sources = 0;		// Count light sources found.
	int scrolltx = get_scrolltx(), scrollty = get_scrollty();
					// Get chunks to start with, starting
					//   1 tile left/above.
	int start_chunkx = (scrolltx + x/tilesize - 1)/tiles_per_chunk;
	if (start_chunkx < 0)
		start_chunkx = 0;
	int start_chunky = (scrollty + y/tilesize - 1)/tiles_per_chunk;
	if (start_chunky < 0)
		start_chunky = 0;
					// End 8 tiles to right.
	int stop_chunkx = 1 + (scrolltx + (x + w + tilesize - 2)/tilesize + 
					tiles_per_chunk/2)/tiles_per_chunk;
	int stop_chunky = 1 + (scrollty + (y + h + tilesize - 2)/tilesize + 
					tiles_per_chunk/2)/tiles_per_chunk;
	if (stop_chunkx > num_chunks)
		stop_chunkx = num_chunks;
	if (stop_chunky > num_chunks)
		stop_chunky = num_chunks;


#if 0	/* +++++Old way. */
					// Which chunks to start with:
					// Watch for shapes 1 chunk to left.
	int start_chunkx = chunkx + x/chunksize - 1;
	if (start_chunkx < 0)
		start_chunkx = 0;
	int start_chunky = chunky + y/chunksize - 1;
	if (start_chunky < 0)
		start_chunky = 0;
	int stopx = x + w;
	int stopy = y + h;
					// Go 1 chunk past end.
	int stop_chunkx = chunkx + stopx/chunksize + 
						(stopx%chunksize != 0) + 1;
	if (stop_chunkx > num_chunks)
		stop_chunkx = num_chunks;
	int stop_chunky = chunky + stopy/chunksize + 
						(stopy%chunksize != 0) + 1;
	if (stop_chunky > num_chunks)
		stop_chunky = num_chunks;
#endif
#if 0	/*+++++++Done in read_map_data().  Called from paint(void). */
					// Read in "map", "ifix" objects for
					//  all visible superchunks.
	for (cy = start_chunky; cy < stop_chunky; )
		{
		int schunky = cy/16;	// 16 chunks/superchunk.
					// How far to bottom of superchunk?
		int num_chunks_y = 16 - (cy%16);
		if (cy + num_chunks_y > stop_chunky)
			{
			num_chunks_y = stop_chunky - cy;
			if (num_chunks_y <= 0)
				break;	// Past end of world.
			}
		for (cx = start_chunkx; cx < stop_chunkx; )
			{
			int schunkx = cx/16;
			int num_chunks_x = 16 - (cx%16);
			if (cx + num_chunks_x > stop_chunkx)
				{
				num_chunks_x = stop_chunkx - cx;
				if (num_chunks_x <= 0)
					break;
				}
					// Figure superchunk #.
			int schunk = 12*schunky + schunkx;
					// Read it if necessary.
			if (!schunk_read[schunk])
				get_superchunk_objects(schunk);
					// Increment x coords.
			cx += num_chunks_x;
			}
					// Increment y coords.
		cy += num_chunks_y;
		}
#endif
	int cx, cy;			// Chunk #'s.
					// Paint all the flat scenery.
	for (cy = start_chunky; cy < stop_chunky; cy++)
		for (cx = start_chunkx; cx < stop_chunkx; cx++)
			{
			paint_chunk_flats(cx, cy);
			paint_chunk_objects(0, cx, cy, 1);
			}
					// Draw the chunks' objects.
	for (cy = start_chunky; cy < stop_chunky; cy++)
		for (cx = start_chunkx; cx < stop_chunkx; cx++)
			{
			paint_chunk_objects(-1, cx, cy, 0);
					// Also check for light sources.
			Chunk_object_list *chunk = get_objects(cx, cy);
			if (is_main_actor_inside() == chunk->is_roof())
				light_sources += chunk->get_light_sources();
			}

					// Draw gumps.
	for (Gump_object *gmp = open_gumps; gmp; gmp = gmp->get_next())
		gmp->paint(this);
					// Draw text, sprites.
	for (Special_effect *txt = effects; txt; txt = txt->next)
		txt->paint(this);
	win->clear_clip();
					// Complete repaint?
	if (!x && !y && w == get_width() && h == get_height() && main_actor)
		{			// Look for lights.
		int carried_light = main_actor->has_light_source();
		int cnt = usecode->get_party_count();
		for (int i = 0; !carried_light && i < cnt; i++)
			{
			int	party_member=usecode->get_party_member(i);
			Actor *person=get_npc(party_member);
			carried_light = person->has_light_source();
			}
			// Set palette for lights.
		clock.set_light_source(carried_light + (light_sources > 0));
		}
	painted = 1;
	}

/*
 *	Paint the flat (non-rle) shapes in a chunk.
 */

void Game_window::paint_chunk_flats
	(
	int cx, int cy			// Chunk coords (0 - 12*16).
	)
	{
	int xoff = (cx*tiles_per_chunk - get_scrolltx())*tilesize;
	int yoff = (cy*tiles_per_chunk - get_scrollty())*tilesize;
	Chunk_object_list *olist = get_objects(cx, cy);
					// Go through array of tiles.
	for (int tiley = 0; tiley < tiles_per_chunk; tiley++)
		for (int tilex = 0; tilex < tiles_per_chunk; tilex++)
			{
			ShapeID id = olist->get_flat(tilex, tiley);
			if (!id.is_invalid())
				{	// Draw flat.
				Shape_frame *shape = get_shape(id);
				win->copy8(shape->data, tilesize, tilesize, 
					xoff + tilex*tilesize,
					yoff + tiley*tilesize);
				}
			}
	}

/*
 *	Paint a chunk's objects, left-to-right, top-to-bottom.
 */

void Game_window::paint_chunk_objects
	(
	int at_lift,			// Only paint this lift.  -1=all.
	int cx, int cy,			// Chunk coords (0 - 12*16).
	int flat_only			// Only paint 0-height objects if 1,
					//   >0 height if 0.
	)
	{
	Game_object *obj;
	Chunk_object_list *olist = get_objects(cx, cy);
	int save_skip = skip_lift;
	if (skip_above_actor < skip_lift)
		skip_lift = skip_above_actor;
					// +++++Clear flag.
	for (obj = olist->get_first(); obj; obj = olist->get_next(obj))
		obj->rendered = 0;
	for (obj = olist->get_first(); obj; obj = olist->get_next(obj))
		if (!obj->rendered)
			paint_object(obj, at_lift, flat_only);
	skip_lift = save_skip;
	}

/*
 *	Render an object after first rendering any that it depends on.
 */

void Game_window::paint_object
	(
	Game_object *obj,
	int at_lift,			// Only paint this lift.  -1=all.
	int flat_only			// Only paint 0-height objects if 1,
					//   >0 height if 0.
	)
	{
	obj->rendered = 1;
	int lift = obj->get_lift();
	if (at_lift >= 0 && at_lift != lift)
		return;
	if (lift >= skip_lift)
		return;
					// Check height.
	Shape_info& info = shapes.get_info(obj->get_shapenum());
	if ((info.get_3d_height() == 0) != flat_only)
		return;
	int cnt = obj->get_dependency_count();
	for (int i = 0; i < cnt; i++)
		{
		Game_object *dep = obj->get_dependency(i);
		if (dep && !dep->rendered)
			paint_object(dep, at_lift, flat_only);
		}
	obj->paint(this);		// Finally, paint this one.
	}

/*
 *	Fade the current palette in or out.
 *	Note:  If pal_num != -1, the current palette is set to it.
 */

void Game_window::fade_palette
	(
	int cycles,			// Length of fade.
	int inout,			// 1 to fade in, 0 to fade to black.
	int pal_num			// 0-11, or -1 for current.
	)
	{
	if (cycles < 1)
		return;
	if (pal_num != -1)
		palette = pal_num;
	ifstream pal;
	u7open(pal, PALETTES_FLX);
	pal.seekg(256 + 3*256*palette);	// Get to desired palette.
	unsigned char colors[3*256];	// Read it in.	
	pal.read((char*)colors, sizeof(colors));
	int dir, start, stop;
	if (inout > 0)			// Fading in?
		{
		dir = 1;
		start = 0;
		stop = cycles + 1;
		faded_out = 0;
		}
	else
		{
		dir = -1;
		start = cycles;
		stop = -1;
		faded_out = 1;
		}
	unsigned char fade_pal[3*256];	// Animate.
	for (int i = start; i != stop; i += dir)
		{
		for(int c=0; c < 3*256; c++)
			fade_pal[c] = (colors[c]*i)/cycles;
		win->set_palette(fade_pal, 63, brightness);
		win->show();
		SDL_Delay(20);
		}
	}

/*
 *	Flash the current palette red.
 */

void Game_window::flash_palette_red
	(
	)
	{
#if 0		/* Cycle through palettes. +++*/
	int savepal = palette;
	for (int i = 0; i < 12; i++)
		{
		set_palette(i);
		win->show();
		SDL_Delay(2000);
		}
	set_palette(savepal);
#else
	int savepal = palette;
	set_palette(8);			// Palette 8 is the red one.
	win->show();
	SDL_Delay(100);
	set_palette(savepal);
	painted = 1;
#endif
	}

/*
 *	Read in a palette.
 */

void Game_window::set_palette
	(
	int pal_num,			// 0-11, or -1 to leave unchanged.
	int new_brightness		// New percentage, or -1.
	)
	{
	if (palette == pal_num && brightness == new_brightness)
		return;			// Already set.
	if (pal_num != -1)
		palette = pal_num;	// Store #.
	if (new_brightness > 0)
		brightness = new_brightness;
	if (faded_out)
		return;			// In the black.
	ifstream pal;
	u7open(pal, PALETTES_FLX);
	pal.seekg(256 + 3*256*palette); // Get to desired palette.
	unsigned char colors[3*256];	// Read it in.	
	pal.read((char*)colors, sizeof(colors));
					// They use 6 bits.
	win->set_palette(colors, 63, brightness);
	}

void Game_window::set_palette
	(
	const char *fname,
	int pal_num,
	int fade
	)
	{
	U7object pal(fname, pal_num);
	char *colors_raw = 0;
	size_t len;
	pal.retrieve(&colors_raw,len);
	unsigned char colors[768];
	for(int i=0;i<768;i++)
		colors[i]=colors_raw[i*2];
					// They use 6 bits.
	switch(fade) 
		{
		case -1:
			{
				unsigned char fade_pal[768];
				for(int i=30;i>=0;i--) {
					for(int c=0;c<768;c++)
						fade_pal[c] = colors[c]*i/30;
					win->set_palette(fade_pal, 63);
					win->show();
					SDL_Delay(20);
				}
				break;
			}
		case 0:
			win->set_palette(colors, 63);
			break;
		case 1:
			{
				unsigned char fade_pal[768];
				for(int i=0;i<=30;i++) {
					for(int c=0;c<768;c++)
						fade_pal[c] = colors[c]*i/30;
					win->set_palette(fade_pal, 63);
					win->show();
					SDL_Delay(20);
				}
				break;
			}
		default:
			break;
		}
	
	delete[] colors_raw;
	}

/*
 *	Brighten/darken palette for the user.
 */

void Game_window::brighten
	(
	int per				// +- percentage to change.
	)
	{
	int new_brightness = brightness + per;
	if (new_brightness < 20)	// Have a min.
		new_brightness = 20;
	user_brightness = new_brightness;
	set_palette(palette, new_brightness);
//	paint();	++++Not needed since we're always 8-bit now.
	}

/*
 *	Restore brightness to user's setting.
 */

void Game_window::restore_users_brightness
	(
	)
	{
	set_palette(-1, user_brightness);
	}

/*
 *	Shift view by one tile.
 */

void Game_window::view_right
	(
	)
	{
	if (scrolltx + get_width()/tilesize >= num_chunks*tiles_per_chunk - 1)
		return;
	int w = get_width(), h = get_height();
					// Get current rightmost chunk.
	int old_rcx = (scrolltx + (w - 1)/tilesize)/tiles_per_chunk;
	scrolltx++;			// Increment offset.
	scroll_bounds.x++;
	if (mode == gump)		// Gump on screen?
		{
		paint();
		return;
		}
	read_map_data();		// Be sure objects are present.
					// Shift image to left.
	win->copy(tilesize, 0, w - tilesize, h, 0, 0);
	dirty.x -= tilesize;		// Shift dirty rect.
	dirty = clip_to_win(dirty);
					// Paint 1 column to right.
//	add_dirty(Rectangle(w - tilesize, 0, tilesize, h));
	paint(w - tilesize, 0, tilesize, h);
					// Find newly visible NPC's.
	int new_rcx = (scrolltx + (w - 1)/tilesize)/tiles_per_chunk;
	if (new_rcx != old_rcx)
		add_nearby_npcs(new_rcx, scrollty/tiles_per_chunk, 
			new_rcx + 1, 
		    (scrollty + (h + tilesize - 1)/tilesize)/tiles_per_chunk);
	}
void Game_window::view_left
	(
	)
	{
	if (scrolltx <= 0)
		return;
	scrolltx--;
	scroll_bounds.x--;
	if (mode == gump)		// Gump on screen?
		{
		paint();
		return;
		}
	read_map_data();		// Be sure objects are present.
	win->copy(0, 0, get_width() - tilesize, get_height(), tilesize, 0);
	dirty.x += tilesize;		// Shift dirty rect.
	dirty = clip_to_win(dirty);
	int h = get_height();
//	add_dirty(Rectangle(0, 0, tilesize, h));
	paint(0, 0, tilesize, h);
					// Find newly visible NPC's.
	int new_lcx = scrolltx/tiles_per_chunk;
	if (new_lcx != (scrolltx + 1)/tiles_per_chunk)
		add_nearby_npcs(new_lcx, scrollty/tiles_per_chunk, 
			new_lcx + 1, 
		    (scrollty + (h + tilesize - 1)/tilesize)/tiles_per_chunk);
	}
void Game_window::view_down
	(
	)
	{
	if (scrollty + get_height()/tilesize >= num_chunks*tiles_per_chunk - 1)
		return;
	int w = get_width(), h = get_height();
					// Get current bottomost chunk.
	int old_bcy = (scrollty + (h - 1)/tilesize)/tiles_per_chunk;
	scrollty++;
	scroll_bounds.y++;
	if (mode == gump)		// Gump on screen?
		{
		paint();
		return;
		}
	read_map_data();		// Be sure objects are present.
	win->copy(0, tilesize, w, h - tilesize, 0, 0);
	dirty.y -= tilesize;		// Shift dirty rect.
	dirty = clip_to_win(dirty);
//	add_dirty(Rectangle(0, h - tilesize, w, tilesize));
	paint(0, h - tilesize, w, tilesize);
					// Find newly visible NPC's.
	int new_bcy = (scrollty + (h - 1)/tilesize)/tiles_per_chunk;
	if (new_bcy != old_bcy)
	add_nearby_npcs(scrolltx/tiles_per_chunk, new_bcy, 
		    (scrolltx + (w + tilesize - 1)/tilesize)/tiles_per_chunk,
			new_bcy + 1);
	}
void Game_window::view_up
	(
	)
	{
	if (scrollty <= 0)
		return;
	scrollty--;
	scroll_bounds.y--;
	if (mode == gump)		// Gump on screen?
		{
		paint();
		return;
		}
	read_map_data();		// Be sure objects are present.
	int w = get_width();
	win->copy(0, 0, w, get_height() - tilesize, 0, tilesize);
	dirty.y += tilesize;		// Shift dirty rect.
	dirty = clip_to_win(dirty);
//	add_dirty(Rectangle(0, 0, w, tilesize));
	paint(0, 0, w, tilesize);
					// Find newly visible NPC's.
	int new_tcy = scrollty/tiles_per_chunk;
	if (new_tcy != (scrollty + 1)/tiles_per_chunk)
		add_nearby_npcs(scrolltx/tiles_per_chunk, new_tcy,
		    (scrolltx + (w + tilesize - 1)/tilesize)/tiles_per_chunk,
								new_tcy + 1);
	}

/*
 *	Stop the actor.
 */

void Game_window::stop_actor
	(
	)
	{
	main_actor->stop();		// Stop and set resting state.
	paint();	// ++++++Necessary?
	main_actor->get_followers();
	}

#if 0
/*
 *	Find a "roof" in the given chunk.
 *
 *	Output: 1 if found, else 0.
 */

int Game_window::find_roof
	(
	int cx, int cy			// Absolute chunk coords.
	)
	{
	Chunk_object_list *olist = objects[cx][cy];
	if (!olist)
		return (0);
	return (olist->is_roof());
#if 0
	Game_object *obj;
	for (obj = olist->get_first(); obj; obj = olist->get_next(obj))
		if (obj->get_lift() >= 5)
			return (1);	// Found one.
	return (0);
#endif
	}
#endif

/*
 *	Find the highest gump that the mouse cursor is on.
 *
 *	Output:	->gump, or null if none.
 */

Gump_object *Game_window::find_gump
	(
	int x, int y			// Pos. on screen.
	)
	{
	Gump_object *gmp;
	Gump_object *found = 0;		// We want last found in chain.
	for (gmp = open_gumps; gmp; gmp = gmp->get_next())
		{
		Rectangle box = get_gump_rect(gmp);
		if (box.has_point(x, y))
			{		// Check the shape itself.
			Shape_frame *s = gumps.get_shape(gmp->get_shapenum(),
							gmp->get_framenum());
			if (s->has_point(x - gmp->get_x(), y - gmp->get_y()))
				found = gmp;
			}
		}
	return (found);
	}

/*
 *	Find gump containing a given object.
 */

Gump_object *Game_window::find_gump
	(
	Game_object *obj
	)
	{
					// Get container object is in.
	Game_object *owner = obj->get_owner();
	if (!owner)
		return (0);
					// Look for container's gump.
	for (Gump_object *gmp = open_gumps; gmp; gmp = gmp->get_next())
		if (gmp->get_container() == owner)
			return (gmp);
	return (0);
	}

/*
 *	Find the top object that can be selected, dragged, or activated.
 *	The one returned is the 'highest'.
 *
 *	Output:	->object, or null if none.
 */

Game_object *Game_window::find_object
	(
	int x, int y			// Pos. on screen.
	)
	{
cout << "Clicked at tile (" << get_scrolltx() + x/tilesize << ", " <<
		get_scrollty() + y/tilesize << ")"<<endl;
	Game_object *found[100];
	int cnt = 0;
	int actor_lift = main_actor->get_lift();
	int start = actor_lift > 0 ? -1 : 0;
	int not_above = skip_lift;
	if (skip_above_actor < not_above)
		not_above = skip_above_actor;
					// See what was clicked on.
	for (int lift = start + actor_lift; lift < not_above; lift++)
		cnt += find_objects(lift, x, y, &found[cnt]);
	if (!cnt)
		return (0);		// Nothing found.
					// Find 'best' one.
	Game_object *obj = found[cnt - 1];
	for (int i = 0; i < cnt - 1; i++)
		if (obj->lt(*found[i]) == 1 ||
					// Try to avoid 'transparent' objs.
		    shapes.get_info(obj->get_shapenum()).is_transparent())
			obj = found[i];
	return (obj);
	}

/*
 *	Find objects at a given position on the screen with a given lift.
 *
 *	Output: # of objects, stored in list.
 */

int Game_window::find_objects
	(
	int lift,			// Look for objs. with this lift.
	int x, int y,			// Pos. on screen.
	Game_object **list		// Objects found are stored here.
	)
	{
					// Figure chunk #'s.
	int start_cx = (get_scrolltx() + 
				(x + 4*lift)/tilesize)/tiles_per_chunk;
	int start_cy = (get_scrollty() + 
				(y + 4*lift)/tilesize)/tiles_per_chunk;
	Game_object *obj;
	int cnt = 0;			// Count # found.
					// Check 1 chunk down & right too.
	for (int ycnt = 0; ycnt < 2; ycnt++)
		{
		int cy = start_cy + ycnt;
		for (int xcnt = 0; xcnt < 2; xcnt++)
			{
			int cx = start_cx + xcnt;
			Chunk_object_list *olist = objects[cx][cy];
			if (!olist)
				continue;
			for (obj = olist->get_first(); obj; 
						obj = olist->get_next(obj))
				{
				if (obj->get_lift() != lift)
					continue;
				Rectangle r = get_shape_rect(obj);
				if (!r.has_point(x, y))
					continue;
					// Check the shape itself.
				Shape_frame *s = get_shape(*obj);
				int ox, oy;
				get_shape_location(obj, ox, oy);
				if (s->has_point(x - ox, y - oy))
					list[cnt++] = obj;
				}
			}
		}
	return (cnt);
	}

/*
 *	Show the names of the items the mouse is clicked on.
 */

void Game_window::show_items
	(
	int x, int y			// Coords. in window.
	)
	{
					// Look for obj. in open gump.
	Gump_object *gump = find_gump(x, y);
	Game_object *obj;		// What we find.
	if (gump)
		obj = gump->find_object(x, y);
	else				// Search rest of world.
		obj = find_object(x, y);
	if (obj)
					// Show name.
		add_text(obj->get_name().c_str(), x, y);
//++++++++Testing
#if 1
	int shnum, frnum;
	if (obj)
		{
		shnum = obj->get_shapenum(), frnum = obj->get_framenum();
		Shape_info& info = shapes.get_info(shnum);
		cout << "Object " << shnum << ':' << frnum <<
					" has 3d tiles (x, y, z): " <<
			info.get_3d_xtiles() << ", " <<
			info.get_3d_ytiles() << ", " <<
			info.get_3d_height() << ", sched = " <<
			obj->get_schedule_type() << ", align = " <<
			obj->get_alignment()
			<< endl;
		int tx, ty, tz;
		obj->get_abs_tile(tx, ty, tz);
		cout << "tx = " << tx << ", ty = " << ty << ", tz = " <<
			tz << ", quality = " <<
			obj->get_quality() << endl;
		cout << "Volume = " << info.get_volume() << endl;
		cout << "obj = " << (void *) obj << endl;
		}
	else				// Obj==0
		{
		int tx = get_scrolltx() + x/tilesize;
		int ty = get_scrollty() + y/tilesize;
		int cx = tx/tiles_per_chunk, cy = ty/tiles_per_chunk;
		tx = tx%tiles_per_chunk;
		ty = ty%tiles_per_chunk;
		Chunk_object_list *chunk = get_objects(cx, cy);
		ShapeID id = chunk->get_flat(tx, ty);
		shnum = id.get_shapenum();
		cout << "Clicked on flat shape " << 
			shnum << ':' << id.get_framenum() << endl;
		}
	Shape_info& info = shapes.get_info(shnum);
#if 1
	cout << "TFA[1][0-6]= " << (((int) info.get_tfa(1))&127) << endl;
	cout << "TFA[0][0-1]= " << (((int) info.get_tfa(0)&3)) << endl;
	cout << "TFA[0][3-4]= " << (((int) (info.get_tfa(0)>>3)&3)) << endl;
#endif
	if (info.is_animated())
		cout << "Object is ANIMATED" << endl;
	if (info.has_translucency())
		cout << "Object has TRANSLUCENCY" << endl;
	if (info.is_transparent())
		cout << "Object is TRANSPARENT" << endl;
	if (info.is_light_source())
		cout << "Object is LIGHT_SOURCE" << endl;
	if (info.is_door())
		cout << "Object is a DOOR" << endl;
#endif
	}

/*
 *	Add a text object at a given spot.
 */

void Game_window::add_text
	(
	const char *msg,
	int x, int y			// Pixel coord. on screen.
	)
	{
	Text_effect *txt = new Text_effect(msg,
		get_scrolltx() + x/tilesize, get_scrollty() + y/tilesize,
				8 + get_text_width(0, msg),
				8 + get_text_height(0));
	txt->paint(this);		// Draw it.
	painted = 1;
	add_effect(txt);
					// Show for a couple seconds.
	unsigned long curval = SDL_GetTicks();
	tqueue->add(curval + 2000, txt, (long) this);
	}

/*
 *	Add an effect.
 */

void Game_window::add_effect
	(
	Special_effect *effect
	)
	{
	effect->next = effects;		// Insert into chain.
	effect->prev = 0;
	if (effect->next)
		effect->next->prev = effect;
	effects = effect;
	}

/*
 *	Remove a text item/sprite from the chain and delete it.
 *	Note:  It better not still be in the time queue.
 */

void Game_window::remove_effect
	(
	Special_effect *effect
	)
	{
	if (effect->next)
		effect->next->prev = effect->prev;
	if (effect->prev)
		effect->prev->next = effect->next;
	else				// Head of chain.
		effects = effect->next;
	delete effect;
	}

/*
 *	Remove all text items.
 */

void Game_window::remove_all_effects
	(
	)
	{
	if (!effects)
		return;
	while (effects)
		{
		tqueue->remove(effects);// Remove from time queue if there.
		remove_effect(effects);
		}
	paint();			// Just paint whole screen.
	}

/*
 *	Remove weather effects.
 */

void Game_window::remove_weather_effects
	(
	)
	{
	Special_effect *each = effects;
	while (each)
		{
		Special_effect *next = each->next;
		if (each->is_weather())
			{
			tqueue->remove(each);
			remove_effect(each);
			}
		each = next;
		}
	set_all_dirty();
	}

/*
 *	Handle a double-click.
 */

void Game_window::double_clicked
	(
	int x, int y			// Coords in window.
	)
	{
					// Look for obj. in open gump.
	Gump_object *gump = find_gump(x, y);
	Game_object *obj;
	if (gump)
		obj = gump->find_object(x, y);
	else				// Search rest of world.
		obj = find_object(x, y);
	if (obj)
		{
		if (combat && !gump && obj != main_actor &&
						obj->get_party_id() < 0)
			{		// In combat mode.
					// Want everyone to be in combat.
			combat = 0;
			toggle_combat();
			main_actor->set_opponent(obj);
			return;
			}
		remove_all_effects();	// Remove text msgs. from screen.
		cout << "Object name is " << obj->get_name() << endl;
		init_faces();		// Be sure face list is empty.
		Game_mode savemode = mode;
		obj->activate(usecode);
		npc_prox->wait(4);	// Delay "barking" for 4 secs.
		if (mode == conversation)
			{
			mode = savemode;
			paint();
			}
		}
	}

/*
 *	Store information about an NPC's face and text on the screen during
 *	a conversation:
 */
class Npc_face_info
	{
	int shape;			// NPC's shape #.
	unsigned char text_pending;	// Text has been written, but user
					//   has not yet been prompted.
	Rectangle face_rect;		// Rectangle where face is shown.
	Rectangle text_rect;		// Rectangle NPC statement is shown in.
	int last_text_height;		// Height of last text painted.
	friend class Game_window;
	Npc_face_info(int sh) : shape(sh), text_pending(0)
		{  }
	};

/*
 *	Initialize face list.
 */

void Game_window::init_faces
	(
	)
	{
	for (int i = 0; i < num_faces; i++)
		{
		delete face_info[i];
		face_info[i] = 0;
		}
	num_faces = 0;
	last_face_shown = -1;
	}

/*
 *	Show a "face" on the screen.  Npc_text_rect is also set.
 */

void Game_window::show_face
	(
	int shape,			// Shape (NPC #).
	int frame
	)
	{
	const int max_faces = sizeof(face_info)/sizeof(face_info[0]);
	mode = conversation;		// Make sure mode is set right.
	if (num_faces == max_faces)
		{
		cout << "Can't show more than " << max_faces << " faces" << endl;
		return;
		}
					// Get character's portrait.
	Shape_frame *face = faces.get_shape(shape, frame);
	Npc_face_info *info = 0;
	Rectangle actbox;		// Gets box where face goes.
					// See if already on screen.
	for (int i = 0; i < num_faces; i++)
		if (face_info[i] && face_info[i]->shape == shape)
			{
			info = face_info[i];
			last_face_shown = i;
			break;
			}
//++++++Really should look for empty slot & use it.
	if (!info)			// New one?
		{
					// Get last one shown.
		Npc_face_info *prev = num_faces ? face_info[num_faces - 1] : 0;
		info = new Npc_face_info(shape);
		last_face_shown = num_faces;
		face_info[num_faces++] = info;
					// Get text height.
		int text_height = get_text_height(0);
					// Figure starting y-coord.
		int starty;
		if (prev)
			{
			starty = prev->text_rect.y + prev->last_text_height;
			if (starty < prev->face_rect.y + prev->face_rect.h)
				starty = prev->face_rect.y + prev->face_rect.h;
			starty += 2*text_height;
			}
		else
			starty = 1;
		actbox = clip_to_win(Rectangle(8, starty,
			face->get_width() + 4, face->get_height() + 4));
		info->face_rect = actbox;
					// This is where NPC text will go.
		info->text_rect = clip_to_win(Rectangle(
			actbox.x + actbox.w + 3, actbox.y,
			get_width() - actbox.x - actbox.w - 6,
							11*text_height));
		info->last_text_height = info->text_rect.h;
		}
	else
		actbox = info->face_rect;
	win->set_clip(0, 0, get_width(), get_height());
					// Draw whom we're talking to.
	paint_shape(actbox.x + face->get_xleft(),
			actbox.y + face->get_yabove(), face);
	win->clear_clip();
	}

/*
 *	Remove face from screen.
 */

void Game_window::remove_face
	(
	int shape
	)
	{
	int i;				// See if already on screen.
	for (i = 0; i < num_faces; i++)
		if (face_info[i] && face_info[i]->shape == shape)
			break;
	if (i == num_faces)
		return;			// Not found.
	Npc_face_info *info = face_info[i];
	paint(info->face_rect);
	paint(info->text_rect);
	delete face_info[i];
	face_info[i] = 0;
	num_faces--;
	if (last_face_shown == i)	// Just in case.
		last_face_shown = num_faces - 1;
	}

/*
 *	Show what the NPC had to say.
 */

void Game_window::show_npc_message
	(
	char *msg
	)
	{
	if (last_face_shown == -1)
		return;
	Npc_face_info *info = face_info[last_face_shown];
	Rectangle& box = info->text_rect;
	paint(box);			// Clear what was there before.
	int height = paint_text_box(0, msg, box.x, box.y, box.w, box.h);
					// All fit?  Store height painted.
	info->last_text_height = height > 0 ? height : box.h;
	info->text_pending = 1;
	painted = 1;
	show();
	}

/*
 *	Is there NPC text that the user hasn't had a chance to read?
 */

int Game_window::is_npc_text_pending
	(
	)
	{
	for (int i = 0; i < num_faces; i++)
		if (face_info[i] && face_info[i]->text_pending)
			return (1);
	return (0);
	}

/*
 *	Clear text-pending flags.
 */

void Game_window::clear_text_pending
	(
	)
	{
	for (int i = 0; i < num_faces; i++)	// Clear 'pending' flags.
		face_info[i]->text_pending = 0;
	}

/*
 *	Show the Avatar's conversation choices (and face).
 */

void Game_window::show_avatar_choices
	(
	int num_choices,
	char **choices
	)
	{
	mode = conversation;
					// Get screen rectangle.
	Rectangle sbox = get_win_rect();
	int x = 0, y = 0;		// Keep track of coords. in box.
	int height = get_text_height(0);
	int space_width = get_text_width(0, "   ");
					// Get main actor's portrait.
	Shape_frame *face = faces.get_shape(main_actor->get_face_shapenum());
					// Get last one shown.
	Npc_face_info *prev = num_faces ? face_info[num_faces - 1] : 0;
	int fx = prev ? prev->face_rect.x + prev->face_rect.w + 4 : 16;
	int fy;
	if (!prev)
		fy = sbox.h - face->get_height() - 3*height;
	else
		{
		fy = prev->text_rect.y + prev->last_text_height;
		if (fy < prev->face_rect.y + prev->face_rect.h)
			fy = prev->face_rect.y + prev->face_rect.h;
		fy += height;
		}
	Rectangle mbox(fx, fy, face->get_width(), face->get_height());
	mbox = mbox.intersect(sbox);
	avatar_face = mbox;		// Repaint entire width.
					// Draw portrait.
	paint_shape(mbox.x + face->xleft, mbox.y + face->yabove, face);
					// Set to where to draw sentences.
	Rectangle tbox(mbox.x + mbox.w + 16, mbox.y + 4,
				sbox.w - mbox.x - mbox.w - 16,
				sbox.h - mbox.y - 16);
	tbox = tbox.intersect(sbox);
	paint(tbox);			// Paint background.
	delete [] conv_choices;		// Set up new list of choices.
	conv_choices = new Rectangle[num_choices + 1];
	for (int i = 0; i < num_choices; i++)
		{
		char *text = choices[i];
		int width = get_text_width(0, text);
		if (x > 0 && x + width > tbox.w)
			{		// Start a new line.
			x = 0;
			y += height;
			}
					// Store info.
		conv_choices[i] = Rectangle(tbox.x + x, tbox.y + y,
					width, height);
		conv_choices[i] = conv_choices[i].intersect(sbox);
		avatar_face = avatar_face.add(conv_choices[i]);
		paint_text(0, text, tbox.x + x, tbox.y + y);
		x += width + space_width;
		}
	avatar_face.enlarge(6);		// Encloses entire area.
	avatar_face = avatar_face.intersect(sbox);
					// Terminate the list.
	conv_choices[num_choices] = Rectangle(0, 0, 0, 0);
	clear_text_pending();
	painted = 1;
	}

void Game_window::show_avatar_choices
	(
	vector<string> &choices
	)
	{
	char	**result;

	result=new char *[choices.size()];
	for(size_t i=0;i<choices.size();i++)
		{
		result[i]=new char[choices[i].size()+1];
		strcpy(result[i],choices[i].c_str());
		}
	show_avatar_choices(choices.size(),result);
	for(size_t i=0;i<choices.size();i++)
		{
		delete [] result[i];
		}
	delete result;
	}


/*
 *	User clicked during a conversation.
 *
 *	Output:	Index (0-n) of choice, or -1 if not on a choice.
 */

int Game_window::conversation_choice
	(
	int x, int y			// Where mouse was clicked.
	)
	{
	int i;
	for (i = 0; conv_choices[i].w != 0 &&
			!conv_choices[i].has_point(x, y); i++)
		;
	if (conv_choices[i].w != 0)	// Found one?
		{
		paint(avatar_face);	// Repaint.
		return (i);
		}
	else
		return (-1);
	}

/*
 *	Show a gump.
 */

void Game_window::show_gump
	(
	Game_object *obj,		// Object gump represents.
	int shapenum			// Shape # in 'gumps.vga'.
	)
	{
	static int cnt = 0;		// For staggering them.
	Gump_object *gmp;		// See if already open.
	for (gmp = open_gumps; gmp; gmp = gmp->get_next())
		if (gmp->get_owner() == obj &&
		    gmp->get_shapenum() == shapenum)
			break;
	if (gmp)			// Found it?
		{			// Move it to end.
		if (gmp->get_next())
			{
			gmp->remove_from_chain(open_gumps);
			gmp->append_to_chain(open_gumps);
			}
		paint();
		return;
		}
	int x = (1 + cnt)*get_width()/10, 
	    y = (1 + cnt)*get_height()/10;
	Shape_frame *shape = get_gump_shape(shapenum, 0);
	if (x + shape->get_xright() > get_width() ||
	    y + shape->get_ybelow() > get_height())
		{
		cnt = 0;
		x = get_width()/10;
		y = get_width()/10;
		}
	Gump_object *new_gump = (shapenum >= ACTOR_FIRST_GUMP &&
		shapenum <= ACTOR_LAST_GUMP) ?
			new Actor_gump_object((Container_game_object *) obj, 
							x, y, shapenum)
			: shapenum == STATSDISPLAY ?
				new Stats_gump_object(
					(Container_game_object *) obj, x, y)
			: shapenum == SPELLBOOK ?
				new Spellbook_gump((Spellbook_object *) obj)
			: new Gump_object((Container_game_object *) obj, 
							x, y, shapenum);
					// Paint new one last.
	new_gump->append_to_chain(open_gumps);
	if (++cnt == 8)
		cnt = 0;
	paint();			// Show everything.
	mode = gump;			// Special mode.
	}

/*
 *	End gump mode.
 */

void Game_window::end_gump_mode
	(
	)
	{
	while (open_gumps)		// Remove all gumps.
		{
		Gump_object *gmp = open_gumps;
		open_gumps = gmp->get_next();
		delete gmp;
		}
	mode = normal;
	npc_prox->wait(4);		// Delay "barking" for 4 secs.
	paint();
	}

/*
 *	Remove a gump.
 */

void Game_window::remove_gump
	(
	Gump_object *gump
	)
	{
	gump->remove_from_chain(open_gumps);
	delete gump;
	if (!open_gumps)		// Last one?  Out of gump mode.
		mode = normal;
	}

/*
 *	Add an NPC to the 'nearby' list.
 */

void Game_window::add_nearby_npc
	(
	Npc_actor *npc
	)
	{
	if (!npc->is_nearby())
		{
		npc->set_nearby();
		npc_prox->add(SDL_GetTicks(), npc);
		}
	}

/*
 *	Add NPC's in a given range of chunk to the queue for nearby NPC's.
 */

void Game_window::add_nearby_npcs
	(
	int from_cx, int from_cy,	// Starting chunk coord.
	int stop_cx, int stop_cy	// Go up to, but not including, these.
	)
	{
	unsigned long curtime = SDL_GetTicks();
	for (int cy = from_cy; cy != stop_cy; cy++)
		for (int cx = from_cx; cx != stop_cx; cx++)
			for (Npc_actor *npc = get_objects(cx, cy)->get_npcs();
						npc; npc = npc->get_next())
				if (!npc->is_nearby())
					{
					npc->set_nearby();
					npc_prox->add(curtime, npc);
					}
	}

/*
 *	Add all nearby NPC's to the given list.
 */

void Game_window::get_nearby_npcs
	(
	Slist& list
	)
	{
	npc_prox->get_all(list);
	}

/*
 *	Tell all npc's to update their schedules at a new 3-hour period.
 */

void Game_window::schedule_npcs
	(
	int hour3			// 0=midnight, 1=3am, 2=6am, etc.
	)
	{
					// Go through npc's.
	for (int i = 1; i < num_npcs; i++)
		((Npc_actor *) npcs[i])->update_schedule(this, hour3);
	paint();			// Repaint all.
	}

/*
 *	Gain focus.
 */

void Game_window::get_focus
	(
	)
	{
	focus = 1; 
	npc_prox->wait(4);		// Delay "barking" for 4 secs.
	}

/*
 *	Prepare for game
 */

void Game_window::setup_game
	(
	)
	{
	
	mode = normal;
	set_palette(0);
	brighten(20);
		
	init_actors();		// Set up actors if not already done.
				// This also sets up initial 
				//   schedules and positions.
	paint();
	audio->cancel();
				// Want to activate first egg.
	Chunk_object_list *olist = get_objects(
			main_actor->get_cx(), main_actor->get_cy());
	olist->setup_cache();
	Tile_coord t = main_actor->get_abs_tile_coord();
	olist->activate_eggs(main_actor, t.tx, t.ty, -1, -1);
	}

const char *Game_window::get_shape_file_name
	(
	int n
	)
	{		// FIXME: should be dynamic
	switch(n) {
	case 0:
		return SHAPES_VGA;
	case 1:
		return FACES_VGA;
	case 2:
		return GUMPS_VGA;
	case 3:
		return SPRITES_VGA;
	case 4:
		return MAINSHP_FLX;
	case 5:
		return ENDSHAPE_FLX;
	default:
		return 0;
	}
	}

int Game_window::get_shape_file_count
	(
	)
	{
	return 6;	// FIXME: should be dynamic
	}

Vga_file *Game_window::get_shape_file_data
	(
	int n
	)
	{
	switch(n) {	// FIXME: should be dynamic
	case 0:
		return &shapes;
	case 1:
		return &faces;
	case 2:
		return &gumps;
	case 3:
		return &sprites;
	case 4:
		return &mainshp;
	case 5:
		return &endshape;
	default:
		return 0;
	}
	}

void Game_window::play_flic(const char *archive, int index) 
	{
		U7object flic(archive, index);
		flic.retrieve("flic.fli");
		playfli fli("flic.fli");
		fli.play(win);
	}

void Game_window::end_game() 
	{
		// Clear screen
		win->fill8(0,get_width(),get_height(),0,0);
		// Start endgame music.
		// It should actually play endscore.xmi, but we can't
		// handle XMIs yet... :-(
		audio->start_music(40,false);
		play_flic(ENDGAME,0);
		play_flic(ENDGAME,1);
		play_flic(ENDGAME,2);
	}

