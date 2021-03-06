/**	-*-mode: Fundamental; tab-width: 8; -*-
 **
 **	Readnpcs.cc - Read in NPC's from npc.dat & schedule.dat.  Also writes
 **		npc.dat back out.
 **
 **	Written: 5/13/99 - JSF
 **/

/*
Copyright (C) 1999  Jeffrey S. Freedman

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

#include <string.h>
#include "gamewin.h"
#include "actors.h"
#include "usecode.h"
#include "utils.h"
#include "fnames.h"
#include "schedule.h"
//#include "items.h"			/* Debugging only */

/*
 *	Read in the NPC's, plus the monster info.
 */

void Game_window::read_npcs
	(
	)
	{
	ifstream nfile;
	u7open(nfile, NPC_DAT);
	num_npcs1 = Read2(nfile);	// Get counts.
	int cnt2 = Read2(nfile);
	num_npcs = num_npcs1 + cnt2;
	npcs = new Actor *[num_npcs];
	memset(npcs, 0, num_npcs*sizeof(npcs[0]));
					// Create main actor.
	npcs[0] = main_actor = new Main_actor(nfile, 0, 0);
	if (usecode->get_global_flag(Usecode_machine::did_first_scene))
		main_actor->clear_flag(Actor::dont_render);
	else
		main_actor->set_flag(Actor::dont_render);
	int i;
	for (i = 1; i < num_npcs; i++)	// Create the rest.
		npcs[i] = new Npc_actor(nfile, i, i < num_npcs1);
	nfile.close();
	main_actor->set_actor_shape();
	if (u7open(nfile, MONSNPCS, 1))	// Monsters.
		{			// (Won't exist the first time.)
		int cnt = Read2(nfile);
		while (cnt--)
			{		// (Placed automatically.)
			new Monster_actor(nfile, -1, 1);
			}
		}
	center_view(main_actor->get_abs_tile_coord());
	read_schedules();		// Now get their schedules.
	if (!monster_info)		// Might be a 'restore'.
		{
		ifstream mfile;		// Now get monster info.
		u7open(mfile, MONSTERS);
		num_monsters = Read1(mfile);
					// Create list, and read it in.
		monster_info = new Monster_info[num_monsters];
		unsigned char monster[25];
		for (i = 0; i < num_monsters; i++)
			{
			int shape = Read2(mfile);
			mfile.read((char*)monster, 23);// Get the rest.
					// Point to flags.
			unsigned char *ptr = &monster[7];
			unsigned short flags = Read2(ptr);
			ptr += 3;	// Get equip.dat offset.
			unsigned int equip = *ptr;
			monster_info[i].set(shape, monster[0], monster[1],
				monster[2], monster[3], monster[4], monster[5],
				flags, equip);
			}
		mfile.close();
		u7open(mfile, EQUIP);	// Get 'equip.dat'.
		int num_recs = Read1(mfile);
		Equip_record *equip = new Equip_record[num_recs];
		for (i = 0; i < num_recs; i++)
			{
			Equip_record& rec = equip[i];
					// 10 elements/record.
			for (int elem = 0; elem < 10; elem++)
				{
				int shnum = Read2(mfile);
				unsigned prob = Read1(mfile);
				unsigned quant = Read1(mfile);
				Read2(mfile);
				rec.set(elem, shnum, prob, quant);
				}
			}
					// Monster_info owns this.
		Monster_info::set_equip(equip, num_recs);
		}
	}

/*
 *	Write NPC (and monster) data back out.
 *
 *	Output:	0 if error, already reported.
 */

int Game_window::write_npcs
	(
	)
	{
	ofstream nfile;
	if (!U7open(nfile, NPC_DAT))
		{			// +++++Better error???
		cerr << "Exult:  Error opening '" << NPC_DAT <<
				"' for writing"<<endl;
		return (0);
		}
	Write2(nfile, num_npcs1);	// Start with counts.
	Write2(nfile, num_npcs - num_npcs1);
	int i;
	for (i = 0; i < num_npcs; i++)
		npcs[i]->write(nfile);
	nfile.flush();
	int result = nfile.good();
	if (!result)			// ++++Better error system needed??
		{
		cerr << "Exult:  Error writing '" << NPC_DAT << "'"<<endl;
		return (0);
		}
	nfile.close();
					// Now write out monsters in world.
	if (!U7open(nfile, MONSNPCS))
		{			// +++++Better error???
		cerr << "Exult:  Error opening '" << MONSNPCS <<
				"' for writing"<<endl;
		return (0);
		}
					// Start with count.
	int cnt = Monster_actor::get_num_in_world();
	Write2(nfile, cnt);
	for (Monster_actor *mact = Monster_actor::get_first_in_world();
					mact; mact = mact->get_next_in_world())
		mact->write(nfile);
	nfile.flush();
	result = nfile.good();
	if (!result)			// ++++Better error system needed??
		{
		cerr << "Exult:  Error writing '" << MONSNPCS << "'"<<endl;
		return (0);
		}
	return (result);
	}

/*
 *	Read NPC schedules.
 */

void Game_window::read_schedules
	(
	)
	{
	ifstream sfile;
	u7open(sfile, SCHEDULE_DAT);
	int num_npcs = Read4(sfile);	// # of NPC's, not include Avatar.

	short *offsets = new short[num_npcs];
	int i;				// Read offsets with list of scheds.
	for (i = 0; i < num_npcs; i++)
		offsets[i] = Read2(sfile);
	for (i = 0; i < num_npcs - 1; i++)	// Do each NPC, except Avatar.
		{
					// Avatar isn't included here.
		Npc_actor *npc = (Npc_actor *) npcs[i + 1];
		int cnt = offsets[i + 1] - offsets[i];
					// Read schedules into this array.
		Schedule_change *schedules = new Schedule_change[cnt];
		for (int j = 0; j < cnt; j++)
			{
			unsigned char ent[4];
			sfile.read((char*)ent, 4);
			schedules[j].set(ent);
			}
					// Store in NPC.
		npc->set_schedules(schedules, cnt);
		}
	delete [] offsets;		// Done with this.
	}


