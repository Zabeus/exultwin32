INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../imagewin -I$(srcdir)/../files \
		-I$(srcdir)/../objs -I$(srcdir)/../shapes\
	 $(SDL_INCLUDES) $(INCDIRS) $(WINDOWING_SYSTEM) $(DEBUG_FLAGS) $(CPPFLAGS)

noinst_LTLIBRARIES = libflic.la

libflic_la_SOURCES =	\
	playfli.cc	\
	playfli.h

CLEANFILES = *~
