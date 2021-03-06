/*
Copyright (C) 2001 The Exult Team

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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "actors.h"
#include "game.h"
#include "gamewin.h"
#include "gump_utils.h"
#include "misc_buttons.h"
#include "CombatStats_gump.h"
#include "Paperdoll_gump.h"


/*
 *	Statics:
 */

static const int colx = 110;
static const int coldx = 29;
static const int rowy[7] = {15, 29, 42, 73, 87, 93, 106};

/*
 *	Create stats display.
 */
CombatStats_gump::CombatStats_gump(int initx, int inity) : 
	Gump(0, initx, inity, game->get_shape("gumps/cstats/1"))
{
	Game_window *gwin = Game_window::get_game_window();

	party_size = gwin->get_party(party, 1);

	Gump::shapenum = game->get_shape("gumps/cstats/1") + party_size - 1;
	ShapeID::set_shape(Gump::shapenum, 0);

	for (int i = 0; i < party_size; i++) {
		halo_btn[i] = new Halo_button(this, colx + i*coldx, rowy[4], party[i]);
		cmb_btn[i] = new Combat_mode_button(this, colx + i*coldx + 1, rowy[3],
											party[i]);
		face_btn[i] = new Face_button(this, colx + i*coldx - 13, rowy[0],
									  party[i]);
	}
	for (int i = party_size; i < 9; i++) {
		halo_btn[i] = 0;
		cmb_btn[i] = 0;
		face_btn[i] = 0;
	}
}

CombatStats_gump::~CombatStats_gump()
{
	for (int i = 0; i < 9; i++) {
		delete halo_btn[i];
		delete cmb_btn[i];
		delete face_btn[i];
	}
}

/*
 *	Paint on screen.
 */

void CombatStats_gump::paint(Game_window *gwin)
{
	Gump::paint(gwin);

	// stats for all party members
	for (int i = 0; i < party_size; i++) {
		paint_button(gwin, face_btn[i]);

		Paint_num(gwin, party[i]->get_property(Actor::combat),
				  x + colx + i*coldx, y + rowy[1]);		
		Paint_num(gwin, party[i]->get_property(Actor::health),
				  x + colx + i*coldx, y + rowy[2]);

		paint_button(gwin, halo_btn[i]);
		paint_button(gwin, cmb_btn[i]);
	}

	// magic stats only for Avatar
  	Paint_num(gwin, party[0]->get_property(Actor::magic),
						x + colx, y + rowy[5]);
  	Paint_num(gwin, party[0]->get_property(Actor::mana),
						x + colx, y + rowy[6]);	
}

Gump_button* CombatStats_gump::on_button(Game_window *gwin, int mx, int my)
{
	Gump_button *btn = Gump::on_button(gwin, mx, my);
	if (btn)
		return btn;
	for (int i = 0; i < party_size; i++) {
		if (halo_btn[i]->on_button(gwin, mx, my))
			return halo_btn[i];
		if (cmb_btn[i]->on_button(gwin, mx, my))
			return cmb_btn[i];
		if (face_btn[i]->on_button(gwin, mx, my))
			return face_btn[i];
	}
	return 0;
}
