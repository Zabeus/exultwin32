/**
 **	Objserial.h - Object serialization.
 **
 **	Written: 5/25/2001 - JSF
 **/

/*
Copyright (C) 2001  The Exult Team

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

#ifndef OBJSERIAL_H
#define OBJSERIAL_H	1

#include <string>
#include "utils.h"
#include "servemsg.h"

class Serial_out
	{
	unsigned char *& buf;
public:
	Serial_out(unsigned char *& b) : buf(b)
		{  }
	Serial_out& operator<<(int v)
		{ Write4(buf, v); return *this; }
	Serial_out& operator<<(unsigned long v)
		{ WriteN(buf, v); return *this; }
	Serial_out& operator<<(short v)
		{ Write2(buf, v); return *this; }
	Serial_out& operator<<(bool v)
		{ *buf++ = (v ? 1 : 0); return *this; }
	Serial_out& operator<<(unsigned char c)
		{ *buf++ = c; return *this; }
	Serial_out& operator<<(std::string& s);
	};

/*
 *	Decode.
 */
class Serial_in
	{
	unsigned char *& buf;
public:
	Serial_in(unsigned char *& b) : buf(b)
		{  }
	Serial_in& operator<<(int& v)
		{ v = Read4(buf); return *this; }
	Serial_in& operator<<(unsigned long& v)
		{ v = ReadN<unsigned long>(buf); return *this; }
	Serial_in& operator<<(short& v)
		{ v = Read2(buf); return *this; }
	Serial_in& operator<<(bool &v)
		{ v = *buf++ ? true : false; return *this; }
	Serial_in& operator<<(unsigned char &c)
		{ c = *buf++; return *this; }
	Serial_in& operator<<(std::string& s);
	};


extern int Object_out
	(
	int fd,				// Socket.
	Exult_server::Msg_type id,	// Message id.
	unsigned long addr,		// Address.
	int tx, int ty, int tz,		// Absolute tile coords.
	int shape, int frame,
	int quality,
	std::string name
	);
extern int Object_in
	(
	unsigned char *data,		// Data that was read.
	int datalen,			// Length of data.
	unsigned long& addr,		// Address.
	int& tx, int& ty, int& tz,	// Absolute tile coords.
	int& shape, int& frame,
	int& quality,
	std::string& name
	);

extern int Container_out
	(
	int fd,				// Socket.
	unsigned long addr,		// Address.
	int tx, int ty, int tz,		// Absolute tile coords.
	int shape, int frame,
	int quality,
	std::string name,
	unsigned char resistance,
	bool invisible, bool okay_to_take
	);
extern int Container_in
	(
	unsigned char *data,		// Data that was read.
	int datalen,			// Length of data.
	unsigned long& addr,		// Address.
	int& tx, int& ty, int& tz,	// Absolute tile coords.
	int& shape, int& frame,
	int& quality,
	std::string& name,
	unsigned char& resistance,
	bool& invisible, bool& okay_to_take
	);

extern int Barge_object_out
	(
	int fd,				// Socket.
	unsigned long addr,		// Address.
	int tx, int ty, int tz,	// Absolute tile coords.
	int shape, int frame,
	int xtiles,
	int ytiles,
	int dir
	);
extern int Barge_object_in
	(
	unsigned char *data,		// Data that was read.
	int datalen,			// Length of data.
	unsigned long& addr,		// Address.
	int& tx, int& ty, int& tz,	// Absolute tile coords.
	int& shape, int& frame,
	int &xtiles,
	int &ytiles,
	int &dir
	);

extern int Egg_object_out
	(
	int fd,				// Socket.
	unsigned long addr,		// Address.
	int tx, int ty, int tz,	// Absolute tile coords.
	int shape, int frame,
	int type,
	int criteria,
	int probability,
	int distance,
	bool nocturnal,
	bool once,
	bool hatched,
	bool auto_reset,
	int data1, int data2, int data3,
	std::string str1
	);
extern int Egg_object_in
	(
	unsigned char *data,		// Data that was read.
	int datalen,			// Length of data.
	unsigned long& addr,		// Address.
	int& tx, int& ty, int& tz,	// Absolute tile coords.
	int& shape, int& frame,
	int& type,
	int& criteria,
	int& probability,
	int& distance,
	bool& nocturnal,
	bool& once,
	bool& hatched,
	bool& auto_reset,
	int& data1, int& data2, int& data3,
	std::string& str1
	);

struct Serial_schedule			// For passing a schedule change.
	{
	short time;			// 0-7 (3-hour period).
	short type;			// Schedule type (mostly 0-31).
	int tx, ty, tz;			// Tile to go to.
	};

int Npc_actor_out
	(
	int fd,				// Socket.
	unsigned long addr,		// Address.
	int tx, int ty, int tz,		// Absolute tile coords.
	int shape, int frame, int face,
	std::string name,
	short npc_num,
	short ident,
	int usecode,
	std::string usecodefun,
	int *properties,		// 12 entries.
	short attack_mode,
	short alignment,
	unsigned long oflags,		// Object flags.
	unsigned long xflags,		// Extra object flags.
	unsigned long type_flags,	// Movement flags.
	short num_schedules,		// # of schedule changes.
	Serial_schedule *schedules	// Schedule changes.
	);
int Npc_actor_in
	(
	unsigned char *data,		// Data that was read.
	int datalen,			// Length of data.
	unsigned long& addr,		// Address.
	int& tx, int& ty, int& tz,	// Absolute tile coords.
	int& shape, int& frame, int& face,
	std::string& name,
	short& npc_num,
	short& ident,
	int& usecode,
	std::string& usecodefun,
	int *properties,		// Must have room for 12.
	short& attack_mode,
	short& alignment,
	unsigned long& oflags,		// Object flags.
	unsigned long& xflags,		// Extra object flags.
	unsigned long& type_flags,	// Movement flags.
	short& num_schedules,		// # of schedule changes.
	Serial_schedule *schedules	// Schedule changes.  Room for 8.
	);

extern int Game_info_out
	(
	int fd,				// Socket.
	int version,			// Server/client version.
	int edit_lift,			// Lift being edited.
	int hide_lift,			// Lift being hidden.
	bool map_editing,		// In 'map-editing' mode.
	bool tile_grid,			// Showing tile grid.
	bool map_modified,		// Map was changed.
	int edit_mode			// Mode we're in.
	);
extern int Game_info_in
	(
	unsigned char *data,		// Data that was read.
	int datalen,			// Length of data.
	int& version,			// Server/client version.
	int& edit_lift,			// Lift being edited.
	int& hide_lift,			// Lift being hidden.
	bool& map_editing,		// In 'map-editing' mode.
	bool& tile_grid,		// Showing tile grid.
	bool& map_modified,		// Map was changed.
	int& edit_mode			// Mode we're in.
	);

#endif




