INCLUDES = -I.. $(SDL_INCLUDES) $(KMID_INCLUDES) $(INCDIRS) $(WINDOWING_SYSTEM) $(DEBUG_FLAGS) $(CPPFLAGS) -I../files -I../conf

SUBDIRS = midi_drivers

lib_LTLIBRARIES = libaudio.la

libaudio_la_SOURCES =	\
	Audio.cc	\
	Audio.h		\
	Midi.cc		\
	Midi.h		\
	Mixer.cc	\
	Mixer.h		\
	SDL_mapping.cc	\
	SDL_mapping.h

EXTRA_DIST = 		\
	README
