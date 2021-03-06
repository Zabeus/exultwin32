INCLUDES = -I$(srcdir) -I$(srcdir)/headers -I$(srcdir)/imagewin -I$(srcdir)/shapes \
	-I$(srcdir)/server -I$(srcdir)/data \
	-I$(srcdir)/objs -I$(srcdir)/conf -I$(srcdir)/files -I$(srcdir)/gumps \
	-I$(srcdir)/audio -I$(srcdir)/pathfinder -I$(srcdir)/usecode \
	$(SDL_CFLAGS) $(INCDIRS) $(WINDOWING_SYSTEM) \
	$(DEBUG_FLAGS) $(CPPFLAGS) -DEXULT_DATADIR=\"$(EXULT_DATADIR)\"

CXXLINK = $(LIBTOOL) --mode=link $(CXXLD) $(AM_CXXFLAGS) $(DEBUG_FLAGS) $(CXXFLAGS) $(LDFLAGS) -o $(EXE_TARGET)

SUBDIRS =  files conf shapes imagewin tools data flic audio pathfinder gumps \
	usecode objs server mapedit debian desktop docs

bin_PROGRAMS = exult

exult_SOURCES =	\
	actions.cc	\
	actions.h	\
	actorio.cc	\
	actors.cc	\
	actors.h	\
	alloc.cc	\
	alpha_kludges.h	\
	args.cc		\
	args.h		\
	autoarray.h	\
	bggame.cc	\
	bggame.h	\
	browser.cc	\
	browser.h	\
	cheat.cc	\
	cheat.h		\
	cheat_screen.cc	\
	cheat_screen.h	\
	combat.cc	\
	combat.h	\
	combat_opts.h	\
	delobjs.cc	\
	delobjs.h	\
	devgame.cc	\
	devgame.h	\
	dir.cc		\
	dir.h		\
	drag.cc		\
	drag.h		\
	effects.cc	\
	effects.h	\
	exult.cc	\
	exult.h		\
	exulticon.h	\
	exultmenu.cc	\
	exultmenu.h	\
	exult_types.h	\
	exult_constants.h	\
	fnames.h	\
	frameseq.h	\
	game.cc		\
	game.h		\
	gameclk.cc	\
	gameclk.h	\
	gamedat.cc	\
	gamemap.cc	\
	gamemap.h	\
	gamerend.cc	\
	gamerend.h	\
	gamewin.cc	\
	gamewin.h	\
	hash_utils.h	\
	keys.cc		\
	keys.h		\
	keyactions.cc	\
	keyactions.h	\
	lists.h		\
	menulist.cc	\
	menulist.h	\
	monsters.cc	\
	monsters.h	\
	mouse.cc	\
	mouse.h		\
	npcnear.cc	\
	npcnear.h	\
	npctime.cc	\
	npctime.h	\
	palette.cc	\
	palette.h	\
	party.cc	\
	party.h		\
	paths.cc	\
	paths.h		\
	readnpcs.cc	\
	ready.h		\
	rect.h		\
	schedule.cc	\
	schedule.h	\
	segfile.cc	\
	segfile.h	\
	shapeid.cc	\
	shapeid.h	\
	sigame.cc	\
	sigame.h	\
	singles.h	\
	tiles.h		\
	tqueue.cc	\
	tqueue.h	\
	txtscroll.cc	\
	txtscroll.h	\
	vec.h 		\
	version.cc	\
	version.h	\
	xdrag.cc	\
	xdrag.h

EXULTLIBS = \
	pathfinder/libpathfinder.la		\
	audio/libaudio.la	\
	audio/midi_drivers/libmididrv.la	\
	flic/libflic.la		\
	conf/libconf.la		\
	imagewin/libimagewin.la	\
	server/libserver.la \
	usecode/libusecode.la	\
	shapes/libshapes.la \
	objs/libobjs.la \
	gumps/libgumps.la \
	files/libu7file.la \
	files/zip/libminizip.la

exult_LDADD = \
	$(EXULTLIBS) \
	$(SDL_LIBS) $(KMID_LIBS) $(SYSLIBS) $(x_libraries) $(ICON_FILE) \
	$(ZLIB_LIBS) $(OPENGL_LIBS)

exult_DEPENDENCIES = $(ICON_FILE) $(EXULTLIBS)

noinst_HEADERS = \
	alpha_kludges.h

EXTRA_DIST = 	\
	autogen.sh \
	README.1ST \
	README \
	FAQ \
	README.win32 \
	README.axp \
	README.BeOS \
	README.MacOS \
	README.MacOSX \
	README.AmigaOS \
	README.MorphOS \
	README.Qtopia \
	Makefile.be \
	Makefile.common \
	Makefile.mingw \
	Makefile.studio.mingw \
	Makefile.MorphOS \
	Info.plist.in \
	exult.spec \
	windrag.cc	\
	windrag.h \
	win32/exult.ico \
	win32/exultico.rc \
	win32/exultstudio.ico \
	win32/exultstudioico.rc \
	beos/exult.rsrc \
	mac/exult_icon.rsrc.bin \
	mac/ExultMac.mcp.xml.sit \
	mac/exult_version.rsrc.bin \
	mac/mac_autoconfig.h \
	mac/mac_prefix.h \
	macosx/exult.icns \
	msvcstuff/expack.vcproj \
	msvcstuff/Exult.vcproj \
	msvcstuff/Exult.sln \
	msvcstuff/Exult_bg_flx.vcproj \
	msvcstuff/Exult_flx.vcproj \
	msvcstuff/Exult_si_flx.vcproj \
	msvcstuff/mklink.vcproj \
	msvcstuff/msvc_kludges.cc \
	msvcstuff/msvc_kludges.h \
	msvcstuff/rip.vcproj \
	msvcstuff/shp2pcx.vcproj \
	msvcstuff/splitshp.vcproj \
	msvcstuff/ucdump.vcproj \
	msvcstuff/unistd.h \
	msvcstuff/wuc.vcproj \
	msvcstuff/wud.vcproj \
	msvcstuff/Zip.vcproj \
	msvcstuff/exconfig/exconfig.cpp \
	msvcstuff/exconfig/exconfig.def \
	msvcstuff/exconfig/exconfig.vcproj \
	msvcstuff/exconfig/exconfig.h \
	msvcstuff/exconfig/exconfig.rc \
	msvcstuff/exconfig/resource.h \
	msvcstuff/exconfig/StdAfx.cpp \
	msvcstuff/exconfig/StdAfx.h \
	headers/common_types.h \
	headers/exceptions.h \
	headers/gamma.h	\
	server/servewin32.cc \
	server/servewin32.h

win32/exultico.o: $(srcdir)/win32/exultico.rc $(srcdir)/win32/exult.ico
	-mkdir win32
	cp $(srcdir)/win32/exult.ico win32
	windres $(srcdir)/win32/exultico.rc win32/exultico.o

CLEANFILES = *~

if MACOSX

bundle_name = Exult.app

Exult_app: ${EXE_TARGET}
	mkdir -p $(bundle_name)/Contents/MacOS
	mkdir -p $(bundle_name)/Contents/Resources
	echo "APPL????" > $(bundle_name)/Contents/PkgInfo
	$(INSTALL_DATA) Info.plist $(bundle_name)/Contents/
	$(INSTALL_PROGRAM) $< $(bundle_name)/Contents/MacOS/
	$(INSTALL_DATA) macosx/exult.icns $(bundle_name)/Contents/Resources/

install-exec-local: Exult_app
	mkdir -p $(DESTDIR)/Applications/
	cp -r $(bundle_name) $(DESTDIR)/Applications/

else
install-exec-local:

endif
