EXULT_FLX_OBJECTS = \
	flx.in			\
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
	extras.shp		\
	midi_conversion.shp	\
	sfx_conversion.shp	\
	palette_fades.shp	\
	scaling_method.shp	\
	savegump.shp		\
	sav_downdown.shp	\
	sav_down.shp		\
	sav_up.shp		\
	sav_upup.shp		\
	sav_slider.shp		\
	sav_selected.shp	\
	gameplayoptions.shp	\
	gamemenu.shp		\
	audiooptions.shp	\
	videooptions.shp	\
	hp_bar.shp		\
	sfx_icon.shp

EXULT_BG_FLX_OBJECTS = \
	bg/flx.in			\
	bg/hoe_of_destruction.shp	\
	bg/caddellite_helmet.shp	\
	bg/magic_boomerang.shp		\
	bg/great_dagger.shp		\
	bg/gorget.shp			\
	bg/magicgorget.shp		\
	bg/cleaver.shp			\
	bg/faces.shp			\
	bg/faces2.shp			\
	bg/amulets.shp			\
	bg/bgfiredoom.shp		\
	bg/fellowshipstaff.shp		\
	bg/BGmap.shp			\
	bg/defaultkeys.txt		\
	bg/dupre_face.shp		\
	bg/fem_av_face.shp		\
	bg/iolo_face.shp		\
	bg/male_av_face.shp		\
	bg/shamino_face.shp		\
	bg/greaves.shp			\
	bg/spiked_shield.shp		\
	bg/mr_faces.shp			\
	bg/mr_intro.shp


EXULT_SI_FLX_OBJECTS = \
	si/flx.in			\
	si/SImap.shp			\
	si/defaultkeys.txt

flexdir = $(datadir)/exult

if DATA_FILES
EXULT_FLX = exult.flx
EXULT_BG_FLX = exult_bg.flx
EXULT_SI_FLX = exult_si.flx
else
EXULT_FLX =
EXULT_BG_FLX =
EXULT_SI_FLX =
endif

flex_DATA = $(EXULT_FLX) $(EXULT_BG_FLX) $(EXULT_SI_FLX) midisfx.flx

CLEANFILES = exult.flx exult_bg.flx exult_si.flx *~

EXTRA_DIST = flx.in $(EXULT_FLX_OBJECTS) $(EXULT_BG_FLX_OBJECTS) \
	$(EXULT_SI_FLX_OBJECTS) midisfx.flx

exult.flx: $(EXULT_FLX_OBJECTS)
	../tools/expack -i $(srcdir)/flx.in

exult_bg.flx: $(EXULT_BG_FLX_OBJECTS)
	../tools/expack -i $(srcdir)/bg/flx.in

exult_si.flx: $(EXULT_SI_FLX_OBJECTS)
	../tools/expack -i $(srcdir)/si/flx.in
