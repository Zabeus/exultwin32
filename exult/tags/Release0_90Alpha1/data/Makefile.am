# Never ever change the ordere here: Exult depends on it
EXULT_FLX_OBJECTS = \
	exult_quotes.shp	\
	exult_credits.shp	\
	quotes.txt		\
	credits.txt		\
	exult_logo.shp		\
	exult0.pal		\
	black_gate.shp		\
	serpent_isle.shp	\
	meditown.mid		\
	font.shp		\
	setup.shp		\
	play_intro.shp		\
	full_screen.shp		\
	cheating.shp		\
	ok.shp			\
	cancel.shp		\
	pointers.shp		\
	exit.shp		\
	play_1st_scene.shp	\
	extras.shp

flexdir = $(datadir)/exult

flex_DATA = exult.flx midisfx.flx

CLEANFILES = \
	exult.flx *~

EXTRA_DIST = $(EXULT_FLX_OBJECTS) midisfx.flx

all-local: exult.flx

exult.flx: $(EXULT_FLX_OBJECTS)
	../tools/expack -c exult.flx $(EXULT_FLX_OBJECTS)
