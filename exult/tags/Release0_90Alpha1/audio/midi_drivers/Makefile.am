INCLUDES = -I.. -I../../files $(SDL_INCLUDES) $(KMID_INCLUDES) $(INCDIRS) $(WINDOWING_SYSTEM) $(DEBUG_FLAGS) $(CPPFLAGS) -I../../conf -I../../files

noinst_LTLIBRARIES = libmididrv.la

libmididrv_la_SOURCES =	\
	KMIDI.cc	\
	KMIDI.h		\
	Timidity_binary.cc	\
	Timidity_binary.h	\
	forked_player.cc	\
	forked_player.h		\
	win_MCI.cc		\
	win_MCI.h		\
	win_midiout.cc		\
	win_midiout.h		\
	be_midi.cc		\
	be_midi.h

EXTRA_DIST = 		\
	README
