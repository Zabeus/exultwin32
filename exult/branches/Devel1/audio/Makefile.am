AUTOMAKE_OPTIONS=foreign

all-local:	libaudio.a 

INCLUDES +=	$(SDL_INCLUDES)
INCLUDES +=	$(KMID_INCLUDES)
INCLUDES +=	-I../conf -I.

CXXFLAGS += $(INCDIRS) -Wall 
CXXFLAGS += $(PEDANTIC)
CXXFLAGS += $(DEBUG_FLAGS)
CXXFLAGS += $(WINDOWING_SYSTEM)

SUBDIRS = midi_drivers

#LIBS +=  -L/usr/X11R6/lib/ -lc -lstdc++ -lgcc -lm -lXi -lXext -lX11 


OFILES=	Mixer.o Midi.o Audio.o SDL_mapping.o

SOURCES = Audio.cc Audio.h Midi.cc Midi.h \
		Mixer.cc Mixer.h SDL_mapping.cc SDL_mapping.h \
		common.h test.cc

libaudio.a:	$(OFILES)
	ar rs libaudio.a $(OFILES) midi_drivers/*.o

u7audiotool:	libaudio.a u7audiotool.o 
	g++ -o u7audiotool u7audiotool.o -L../conf -L. -laudio -lconf  $(KMID_LIBS) $(SDL_LIBS)  -lX11 -lXext -lstdc++ -lgcc -lc -lgcc -L/usr/X11R6/lib


clean:
	rm -f libaudio.a core $(OFILES)

tar:
	rm -rf /tmp/audio
	mkdir /tmp/audio
	cp Audio.cc Audio.h Flex.cc Flex.h Midi.cc Midi.h Mixer.cc Mixer.h SDL_mapping.h Makefile /tmp/audio
	cd /tmp && tar czvpf exult_audio_lib.tar.gz audio


%.o : %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

# Dependencies

Audio.o: Audio.cc Audio.h ../autoconfig.h SDL_mapping.h Flex.h common.h Mixer.h Midi.h
Flex.o: Flex.cc Flex.h ../autoconfig.h common.h
IFF.o: IFF.cc IFF.h ../autoconfig.h common.h
Midi.o: Midi.cc Midi.h ../autoconfig.h SDL_mapping.h Flex.h common.h Mixer.h
Mixer.o: Mixer.cc Mixer.h ../autoconfig.h SDL_mapping.h Flex.h common.h Audio.h Midi.h
SDL_mapping.o: SDL_mapping.cc SDL_mapping.h ../autoconfig.h
test.o: test.cc Audio.h ../autoconfig.h SDL_mapping.h Flex.h common.h Mixer.h Midi.h
libaudio.a: midi_drivers/KMIDI.o midi_drivers/forked_player.o midi_drivers/Timidity_binary.o
