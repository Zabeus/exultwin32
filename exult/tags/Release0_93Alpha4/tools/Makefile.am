INCLUDES = -I$(srcdir)/../files -I$(srcdir)/../usecode -I$(srcdir)/.. \
		$(DEBUG_FLAGS) $(CPPFLAGS) $(SDL_INCLUDES)

if LEXYACC
UCC= ucc
else
UCC=
endif

if BUILD_TOOLS
noinst_PROGRAMS = expack wuc wud mklink rip ucdump shp2pcx splitshp $(UCC)
endif

expack_SOURCES = \
	expack.cc

expack_LDADD = \
	../files/libu7file.la \
	$(SYSLIBS)

wuc_SOURCES = \
	wuc.c \
	uctools.h

wud_SOURCES = wud.c

mklink_SOURCES = mklink.c

rip_SOURCES = rip.c

ucdump_SOURCES = ucdump.c

shp2pcx_SOURCES = shp2pcx.cc

shp2pcx_LDADD = $(SYSLIBS)

splitshp_SOURCES = splitshp.cc

splitshp_LDADD = $(SYSLIBS)

ucc_SOURCES = \
	ucclex.l \
	uccparse.y

ucclex.c: ucclex.l
	$(LEX) $< && mv lex.yy.c $@
uccparse.c: uccparse.y
	$(YACC) $< && mv y.tab.c $@

uccparse.o: ucclex.c uccparse.c
	$(COMPILE) -o $@ -c $(srcdir)/uccparse.c

ucc: uccparse.o
	$(LINK) uccparse.o


EXTRA_DIST = \
	intrins1.txt \
	intrins2.txt \
	README \
	u7bgflag.txt \
	u7siflag.txt \
	ucformat.txt

CLEANFILES = *~
