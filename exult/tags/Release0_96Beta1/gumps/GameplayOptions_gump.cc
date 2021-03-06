/*
 *  Copyright (C) 2001  The Exult Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <iostream>

#include "SDL_events.h"

#include "gump_utils.h"
#include "Configuration.h"
#include "Gump_button.h"
#include "Gump_ToggleButton.h"
#include "GameplayOptions_gump.h"
#include "exult.h"
#include "exult_flx.h"
#include "game.h"
#include "gamewin.h"
#include "mouse.h"
#include "cheat.h"
#include "Face_stats.h"

using std::cerr;
using std::endl;
using std::string;

static const int rowy[] = { 5, 18, 31, 44, 57, 70, 83, 96, 109, 122, 146 };
static const int colx[] = { 35, 50, 120, 195, 192 };

class GameplayOptions_button : public Gump_button {
public:
	GameplayOptions_button(Gump *par, int px, int py, int shapenum)
		: Gump_button(par, shapenum, px, py, SF_EXULT_FLX)
		{ }
					// What to do when 'clicked':
	virtual void activate(Game_window *gwin);
};

void GameplayOptions_button::activate(Game_window *gwin)
{
	switch (get_shapenum()) {
	case EXULT_FLX_AUD_CANCEL_SHP:
		((GameplayOptions_gump*)parent)->cancel();
		break;
	case EXULT_FLX_AUD_OK_SHP:
		((GameplayOptions_gump*)parent)->close(gwin);
		break;
	}
}

class GameplayToggle : public Gump_ToggleButton {
public:
	GameplayToggle(Gump* par, int px, int py, int shapenum, int selectionnum, int numsel)
		: Gump_ToggleButton(par, px, py, shapenum, selectionnum, numsel) { }

	friend class GameplayOptions_gump;
	virtual void toggle(int state) { 
		((GameplayOptions_gump*)parent)->toggle((Gump_button*)this, state);
	}
};


class GameplayTextToggle : public Gump_ToggleTextButton {
public:
	GameplayTextToggle(Gump* par, std::string *s, int px, int py, int width, int selectionnum, int numsel)
		: Gump_ToggleTextButton(par, s, selectionnum, numsel, px, py, width) { }

	friend class GameplayOptions_gump;
	virtual void toggle(int state) { 
		((GameplayOptions_gump*)parent)->toggle((Gump_button*)this, state);
	}
};
void GameplayOptions_gump::close(Game_window* gwin)
{
	save_settings();
	done = 1;
}

void GameplayOptions_gump::cancel()
{
	done = 1;
}

void GameplayOptions_gump::toggle(Gump_button* btn, int state)
{
	if (btn == buttons[0])
		facestats = state;
	else if (btn == buttons[1])
		fastmouse = state;
	else if (btn == buttons[2])
		mouse3rd = state;
	else if (btn == buttons[3])
		doubleclick = state;
	else if (btn == buttons[4])
		cheats = state;
	else if (btn == buttons[5])
		paperdolls = state;
	else if (btn == buttons[6])
		text_bg = state;
	else if (btn == buttons[7])
		walk_after_teleport = state;
	else if (btn == buttons[8])
		frames = state;
}

void GameplayOptions_gump::build_buttons()
{
	buttons[0] = new GameplayTextToggle (this, stats, colx[3], rowy[0], 59, facestats, 4);
	buttons[6] = new GameplayTextToggle (this, textbgcolor, colx[3]-21, rowy[1], 80, text_bg, 12);
	if (GAME_BG)
		buttons[5] = new GameplayToggle(this, colx[3], rowy[2], EXULT_FLX_AUD_ENABLED_SHP, paperdolls, 2);
	else if (GAME_SI)
		buttons[7] = new GameplayToggle(this, colx[3], rowy[2], EXULT_FLX_AUD_ENABLED_SHP, walk_after_teleport, 2);
	buttons[1] = new GameplayToggle(this, colx[3], rowy[3], EXULT_FLX_AUD_ENABLED_SHP, fastmouse, 2);
	buttons[2] = new GameplayToggle(this, colx[3], rowy[4], EXULT_FLX_AUD_ENABLED_SHP, mouse3rd, 2);
	buttons[3] = new GameplayToggle(this, colx[3], rowy[5], EXULT_FLX_AUD_ENABLED_SHP, doubleclick, 2);
	buttons[4] = new GameplayToggle(this, colx[3], rowy[7], EXULT_FLX_AUD_ENABLED_SHP, cheats, 2);
	buttons[8] = new GameplayTextToggle(this, framenums, colx[3], rowy[8], 59, frames, 5);
}

void GameplayOptions_gump::load_settings()
{
	Game_window *gwin = Game_window::get_game_window();
	fastmouse = gwin->get_fastmouse();
	mouse3rd = gwin->get_mouse3rd();
	walk_after_teleport = gwin->get_walk_after_teleport();
	cheats = cheat();
	facestats = Face_stats::get_state() + 1;
	doubleclick = 0;
	paperdolls = false;
	string pdolls;
	paperdolls = gwin->get_bg_paperdolls();
	doubleclick = gwin->get_double_click_closes_gumps();
	text_bg = gwin->get_text_bg()+1;
	frames = 1000/gwin->get_std_delay();
	if (frames < 2)
		frames = 2;
	if (frames > 10)
		frames = 10;
	frames = frames/2 - 1;		// 2,4,6,8,10 are the choices.
}

GameplayOptions_gump::GameplayOptions_gump() : Modal_gump(0, EXULT_FLX_GAMEPLAYOPTIONS_SHP, SF_EXULT_FLX)
{
	set_object_area(Rectangle(0, 0, 0, 0), 8, 162);//++++++ ???

	for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++)
		buttons[i] = 0;
	stats = new std::string[4];
	stats[0] = "Disabled";
	stats[1] = "Left";
	stats[2] = "Middle";
	stats[3] = "Right";
	textbgcolor = new std::string[12];
	textbgcolor[0] = "Disabled";
	textbgcolor[1] = "Purple";
	textbgcolor[2] = "Orange";
	textbgcolor[3] = "Light Gray";
	textbgcolor[4] = "Green";
	textbgcolor[5] = "Yellow";
	textbgcolor[6] = "Pale Blue";
	textbgcolor[7] = "Dark Green";
	textbgcolor[8] = "Red";
	textbgcolor[9] = "Bright White";
	textbgcolor[10] = "Dark gray";
	textbgcolor[11] = "White";

	framenums = new std::string[5];
	framenums[0] = "2 fps";
	framenums[1] = "4 fps";
	framenums[2] = "6 fps";
	framenums[3] = "8 fps";
	framenums[4] = "10 fps";

	load_settings();
	
	build_buttons();

	// Ok
	buttons[9] = new GameplayOptions_button(this, colx[0], rowy[10], EXULT_FLX_AUD_OK_SHP);
	// Cancel
	buttons[10] = new GameplayOptions_button(this, colx[4], rowy[10], EXULT_FLX_AUD_CANCEL_SHP);
}

GameplayOptions_gump::~GameplayOptions_gump()
{
	for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++)
		if (buttons[i])
			delete buttons[i];

#if 0
	// For some reason these crash Exult
	delete[] stats;
	delete[] textbgcolor;
#endif
}

void GameplayOptions_gump::save_settings()
{
	Game_window *gwin = Game_window::get_game_window();
	gwin->set_text_bg(text_bg-1);
	config->set("config/gameplay/textbackground", text_bg-1, true);
	int fps = 2*(frames + 1);
	gwin->set_std_delay(1000/fps);
	config->set("config/video/fps", fps, true);
	gwin->set_fastmouse(fastmouse!=false);
	config->set("config/gameplay/fastmouse", fastmouse ? "yes" : "no", true);
	gwin->set_mouse3rd(mouse3rd!=false);
	config->set("config/gameplay/mouse3rd", mouse3rd ? "yes" : "no", true);
	gwin->set_walk_after_teleport(walk_after_teleport!=false);
	config->set("config/gameplay/walk_after_teleport", walk_after_teleport ? "yes" : "no", true);
	gwin->set_double_click_closes_gumps(doubleclick!=false);
	config->set("config/gameplay/double_click_closes_gumps", doubleclick ? "yes" : "no", true);
	cheat.set_enabled(cheats!=false);
	while (facestats != Face_stats::get_state() + 1)
		Face_stats::AdvanceState();
	Face_stats::save_config(config);
	if (GAME_BG && gwin->can_use_paperdolls())
		gwin->set_bg_paperdolls(paperdolls!=false);
	config->set("config/gameplay/bg_paperdolls", paperdolls ? "yes" : "no", true);
}

void GameplayOptions_gump::paint(Game_window* gwin)
{
	Gump::paint(gwin);
	for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++)
		if (buttons[i])
			buttons[i]->paint(gwin);

	gwin->paint_text(2, "Status Bars:", x + colx[0], y + rowy[0] + 1);
	gwin->paint_text(2, "Text Background:", x + colx[0], y + rowy[1] + 1);
	if (GAME_BG)
		gwin->paint_text(2, "Paperdolls:", x + colx[0], y + rowy[2] + 1);
	else if (GAME_SI)
		gwin->paint_text(2, "Walk after Teleport:", x + colx[0], y + rowy[2] + 1);
	gwin->paint_text(2, "Fast Mouse:", x + colx[0], y + rowy[3] + 1);
	gwin->paint_text(2, "Use Middle Mouse Button:", x + colx[0], y + rowy[4] + 1);
	gwin->paint_text(2, "Doubleclick closes Gumps:", x + colx[0], y + rowy[5] + 1);
	gwin->paint_text(2, "Cheats:", x + colx[0], y + rowy[7] + 1);
	gwin->paint_text(2, "Speed:", x + colx[0], y + rowy[8] + 1);
	gwin->set_painted();
}

void GameplayOptions_gump::mouse_down(int mx, int my)
{
	Game_window *gwin = Game_window::get_game_window();
	pushed = Gump::on_button(gwin, mx, my);
					// First try checkmark.
	// Try buttons at bottom.
	if (!pushed)
		for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++)
			if (buttons[i] && buttons[i]->on_button(gwin, mx, my)) {
				pushed = buttons[i];
				break;
			}

	if (pushed)			// On a button?
	{
		pushed->push(gwin);
		return;
	}
}

void GameplayOptions_gump::mouse_up(int mx, int my)
{
	Game_window *gwin = Game_window::get_game_window();
	if (pushed)			// Pushing a button?
	{
		pushed->unpush(gwin);
		if (pushed->on_button(gwin, mx, my))
			((Gump_button*)pushed)->activate(gwin);
		pushed = 0;
	}
}
