/*
Copyright (C) 2003-2005  The Pentagram Team
 
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

#include "pent_include.h"
#include "CoreMidiDriver.h"

#ifdef USE_CORE_MIDI

//#include <pthread.h>
//#include <sched.h>
#include <iomanip>
using namespace std;

const MidiDriver::MidiDriverDesc CoreMidiDriver::desc = 
		MidiDriver::MidiDriverDesc ("CoreMidi", createInstance);

CoreMidiDriver::CoreMidiDriver() : 
	mClient(0), mOutPort(0), mDest(0)
{
	OSStatus err;
	err = MIDIClientCreate(CFSTR("Pentagram MIDI Driver for OS X"), NULL, NULL, &mClient);

}

CoreMidiDriver::~CoreMidiDriver() {
	if (mClient)
		MIDIClientDispose(mClient);
	mClient = 0;
}

int CoreMidiDriver::open() {
	if (mDest)
		return 1;

	OSStatus err = noErr;

	mOutPort = 0;

	int dests = MIDIGetNumberOfDestinations();
	if (dests > 0 && mClient) {
		mDest = MIDIGetDestination(0);
		err = MIDIOutputPortCreate( mClient,
									CFSTR("exult_output_port"),
									&mOutPort);
	} else {
		return 3;
	}

	if (err != noErr)
		return 1;

	return 0;
}

void CoreMidiDriver::close() {

	if (mOutPort && mDest) {
		MIDIPortDispose(mOutPort);
		mOutPort = 0;
		mDest = 0;
	}
}

void CoreMidiDriver::send(uint32 b) {
	assert(mOutPort != NULL);
	assert(mDest != NULL);

	// Extract the MIDI data
	uint8 status_byte = (b & 0x000000FF);
	uint8 first_byte = (b & 0x0000FF00) >> 8;
	uint8 second_byte = (b & 0x00FF0000) >> 16;

	// Generate a single MIDI packet with that data
	MIDIPacketList packetList;
	MIDIPacket *packet = &packetList.packet[0];

	packetList.numPackets = 1;

	packet->timeStamp = 0;
	packet->data[0] = status_byte;
	packet->data[1] = first_byte;
	packet->data[2] = second_byte;

	// Compute the correct length of the MIDI command. This is important,
	// else things may screw up badly...
	switch (status_byte & 0xF0) {
	case 0x80:	// Note Off
	case 0x90:	// Note On
	case 0xA0:	// Polyphonic Aftertouch
	case 0xB0:	// Controller Change
	case 0xE0:	// Pitch Bending
		packet->length = 3;
		break;
	case 0xC0:	// Programm Change
	case 0xD0:	// Monophonic Aftertouch
		packet->length = 2;
		break;
	default:
		perr << "CoreMIDI driver encountered unsupported status byte: 0x" << hex << setw(2) << status_byte << endl;
		packet->length = 3;
		break;
	}

	// Finally send it out to the synthesizer.
	MIDISend(mOutPort, mDest, &packetList);
}

void CoreMidiDriver::send_sysex(uint8 status, const uint8 *msg, uint16 length) {
	assert(mOutPort != NULL);
	assert(mDest != NULL);

	uint8 buf[384];
	MIDIPacketList *packetList = (MIDIPacketList *)buf;
	MIDIPacket *packet = packetList->packet;

	assert(sizeof(buf) >= sizeof(UInt32) + sizeof(MIDITimeStamp) + sizeof(UInt16) + length + 2);

	packetList->numPackets = 1;

	packet->timeStamp = 0;

	// Add SysEx frame
	packet->length = length + 2;
	packet->data[0] = 0xF0;
	memcpy(packet->data + 1, msg, length);
	packet->data[length + 1] = 0xF7;

	// Send it
	MIDISend(mOutPort, mDest, packetList);
}

void CoreMidiDriver::increaseThreadPriority()
{
	pthread_t self;
	int policy;
	struct sched_param param;
	
	self = pthread_self();
	pthread_getschedparam(self, &policy, &param);
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam(self, policy, &param);
}

void CoreMidiDriver::yield()
{
	sched_yield();
}

#endif // MACOSX
