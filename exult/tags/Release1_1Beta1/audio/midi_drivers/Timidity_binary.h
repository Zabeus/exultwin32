//-*-Mode: C++;-*-
/*
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

#ifndef _MIDI_driver_Timidity_binary_h_
#define _MIDI_driver_Timidity_binary_h_

#include "Midi.h"

#if HAVE_TIMIDITY_BIN
class	Timidity_binary : virtual public MidiAbstract
{
public:
	virtual void	start_track(XMIDIEventList *, bool repeat);
	virtual void	start_sfx(XMIDIEventList *);
	virtual void	stop_track(void);
	virtual void	stop_sfx(void);
	virtual	bool	is_playing(void);
	virtual const	char *copyright(void);

	static void music_complete_callback(void);
//	void	player(void);
//	void	sfxplayer(void);

	Timidity_binary();
	virtual ~Timidity_binary();
};
#endif

#endif
