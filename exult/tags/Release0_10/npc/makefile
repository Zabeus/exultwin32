#
#	Makefile for NPC's.
#

OBJECTS = convers.o fsm.o npc.o cond.o vec.o event.o hash.o lists.o

CXXFLAGS = -g

test2:	libnpc.a test2.o
	g++ -g -o test2 test2.o libnpc.a

test1:	libnpc.a test1.o
	g++ -g -o test1 test1.o libnpc.a

%.o:: %.cc
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $<

libnpc.a: $(OBJECTS)
	$(AR) r $@ $?

#libnpc.a(%.o):
#	$(AR) r $@ $*.o

clean:
	-rm *.o *.a


