INCLUDES = -I. -I../files $(DEBUG_FLAGS) $(CPPFLAGS) -DRELNUM=$(RELNUM) 

if LEXYACC
UCC= ucc
else
UCC=
endif

if HAVE_GIMPTOOL
GIMP_PLUGINS=u7shpload
else
GIMP_PLUGINS=
endif

noinst_PROGRAMS = txt2cc expack wuc wud mklink rip ucdump $(UCC) $(GIMP_PLUGINS)

expack_SOURCES = \
	expack.cc

expack_LDADD = \
	../files/libu7file.la

txt2cc_SOURCES = \
	txt2cc.cc

txt2cc_LDADD = \
	../files/libu7file.la

wuc_SOURCES = \
	wuc.c \
	uctools.h

wud_SOURCES = wud.c

mklink_SOURCES = mklink.c

rip_SOURCES = rip.c

ucdump_SOURCES = ucdump.c

ucc_SOURCES = \
	ucclex.l \
	uccparse.y

ucclex.c: ucclex.l
	$(LEX) $< && mv lex.yy.c $@
uccparse.c: uccparse.y
	$(YACC) $< && mv y.tab.c $@
	
uccparse.o: ucclex.c uccparse.c
	$(COMPILE) -o $@ -c uccparse.c

ucc: uccparse.o
	$(LINK) uccparse.o

u7shpload: u7shpload.c
	gimptool --build $<

install-exec-local:
	gimptool --install-bin u7shpload

EXTRA_DIST = \
	intrins1.txt \
	intrins2.txt \
	README \
	u7bgflag.txt \
	u7siflag.txt \
	ucformat.txt
