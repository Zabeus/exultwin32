INCLUDES = -I$(srcdir)/../headers -I$(srcdir)/.. -I$(srcdir)/../files -I$(srcdir)/../imagewin \
	$(SDL_CFLAGS) $(FREETYPE2_INCLUDES) \
	$(INCDIRS) $(WINDOWING_SYSTEM) $(DEBUG_FLAGS) $(CPPFLAGS)

noinst_LTLIBRARIES = libshapes.la

libshapes_la_SOURCES = \
	bodies.cc \
	bodies.h \
	font.cc \
	font.h \
	fontgen.cc \
	fontgen.h \
	fontvga.cc \
	fontvga.h \
	glshape.cc \
	glshape.h \
	items.cc \
	items.h \
	monstinf.cc \
	monstinf.h \
	pngio.cc \
	pngio.h \
	shapeinf.cc \
	shapeinf.h \
	shapevga.cc \
	shapevga.h \
	shapewrite.cc \
	u7drag.cc \
	u7drag.h \
	vgafile.cc \
	vgafile.h

CLEANFILES = *~

