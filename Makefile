# Top level Makefile for tin - for configuration options read the ./INSTALL file.
# 
PROJECT		= tin
EXE		= tin
MANEXT		= 1
LVER		= 1.3
PVER		= 970613
VER		= $(LVER)-unoff-BETA-$(PVER)
MAIL_ADDR 	= "urs@akk.uni-karlsruhe.de"

# directory structure
TOPDIR	= .
DOCDIR	= ./doc
INCDIR	= ./include
OBJDIR	= ./src
SRCDIR	= ./src
AMGDIR	= ./amiga
TOLDIR	= ./tools

HFILES	= \
	$(INCDIR)/config.h \
	$(INCDIR)/extern.h \
	$(INCDIR)/menukeys.h \
	$(INCDIR)/msmail.h \
	$(INCDIR)/nntplib.h \
	$(INCDIR)/os_2.h \
	$(INCDIR)/proto.h \
	$(INCDIR)/stpwatch.h \
	$(INCDIR)/tcurses.h \
	$(INCDIR)/tin.h \
	$(INCDIR)/tnntp.h \
	$(INCDIR)/trace.h \
	$(INCDIR)/version.h \
	$(INCDIR)/win32.h \
	$(INCDIR)/win32tcp.h

CFILES	= \
	$(SRCDIR)/active.c \
	$(SRCDIR)/actived.c \
	$(SRCDIR)/art.c \
	$(SRCDIR)/attrib.c \
	$(SRCDIR)/auth.c \
	$(SRCDIR)/charset.c \
	$(SRCDIR)/color.c \
	$(SRCDIR)/config.c \
	$(SRCDIR)/curses.c \
	$(SRCDIR)/debug.c\
	$(SRCDIR)/envarg.c \
	$(SRCDIR)/feed.c \
	$(SRCDIR)/filter.c \
	$(SRCDIR)/getline.c \
	$(SRCDIR)/group.c \
	$(SRCDIR)/hashstr.c \
	$(SRCDIR)/help.c\
	$(SRCDIR)/inews.c \
	$(SRCDIR)/init.c \
	$(SRCDIR)/joinpath.c \
	$(SRCDIR)/lang.c \
	$(SRCDIR)/list.c \
	$(SRCDIR)/mail.c \
	$(SRCDIR)/main.c \
	$(SRCDIR)/makecfg.c \
	$(SRCDIR)/memory.c \
	$(SRCDIR)/misc.c \
	$(SRCDIR)/msmail.c \
	$(SRCDIR)/newsrc.c\
	$(SRCDIR)/nntplib.c \
	$(SRCDIR)/nntpw32.c \
	$(SRCDIR)/nrctbl.c \
	$(SRCDIR)/open.c \
	$(SRCDIR)/os_2.c \
	$(SRCDIR)/page.c \
	$(SRCDIR)/parsdate.y \
	$(SRCDIR)/pgp.c \
	$(SRCDIR)/post.c \
	$(SRCDIR)/prompt.c \
	$(SRCDIR)/proto.sed \
	$(SRCDIR)/refs.c \
	$(SRCDIR)/regex.c \
	$(SRCDIR)/rfc1521.c \
	$(SRCDIR)/rfc1522.c \
	$(SRCDIR)/save.c \
	$(SRCDIR)/screen.c \
	$(SRCDIR)/search.c \
	$(SRCDIR)/select.c \
	$(SRCDIR)/sigfile.c \
	$(SRCDIR)/signal.c \
	$(SRCDIR)/strftime.c \
	$(SRCDIR)/string.c \
	$(SRCDIR)/tcurses.c \
	$(SRCDIR)/thread.c \
	$(SRCDIR)/trace.c \
	$(SRCDIR)/wildmat.c \
	$(SRCDIR)/win32.c \
	$(SRCDIR)/win32tcp.c \
	$(SRCDIR)/xref.c \
	$(SRCDIR)/vms.c

AMIGA	=\
	$(AMGDIR)/README \
	$(AMGDIR)/smakefile \
	$(AMGDIR)/actived.c \
	$(AMGDIR)/amiga.c \
	$(AMGDIR)/amigatcp.c \
	$(AMGDIR)/amiga.h \
	$(AMGDIR)/amigatcp.h

DOC	= \
	$(DOCDIR)/CHANGES \
	$(DOCDIR)/DEBUG_REFS \
	$(DOCDIR)/Good-Netkeeping-Seal \
	$(DOCDIR)/INSTALL \
	$(DOCDIR)/TODO \
	$(DOCDIR)/WHATSNEW \
	$(DOCDIR)/auth.txt \
	$(DOCDIR)/internals.txt \
	$(DOCDIR)/$(EXE).$(MANEXT)

TOL	= \
	$(TOLDIR)/tinpp \
	$(TOLDIR)/expand_aliases.tgz

TOP	= \
	$(TOPDIR)/Makefile \
	$(TOPDIR)/MANIFEST \
	$(TOPDIR)/README \
	$(TOPDIR)/aclocal.m4 \
	$(TOPDIR)/config.guess \
	$(TOPDIR)/config.sub \
	$(TOPDIR)/configure \
	$(TOPDIR)/configure.in \
	$(TOPDIR)/install.sh \
	$(TOPDIR)/makefile.in \
	$(TOPDIR)/conf-tin

ALL_FILES = $(TOP) $(DOC) $(TOL) $(HFILES) $(CFILES) $(AMIGA) \
	$(INCDIR)/autoconf.hin \
	$(SRCDIR)/Makefile.in \
	$(SRCDIR)/l1_next.tab \
	$(SRCDIR)/next_l1.tab \
	$(SRCDIR)/tincfg.tbl

ALL_DIRS = $(TOPDIR) $(DOCDIR) $(SRCDIR) $(INCDIR) $(AMGDIR)

# standard commands
CD		= cd
CHMOD		= chmod
CP		= cp -p
ECHO		= echo
LS		= ls -l
MAKE		= make
MV		= mv
NROFF		= groff -Tascii
RM		= rm
SHELL		= /bin/sh
TAR		= gtar
WC		= wc
SED		= sed
TR		= tr

all: 
	@$(ECHO) "Top level Makefile for the TIN v$(VER) Usenet newsreader."
	@$(ECHO) " "
	@$(ECHO) "To compile the source code change to the source directory by"
	@$(ECHO) "typing 'cd src' and then type 'make' for more instructions."
	@$(ECHO) " "
	@$(ECHO) "This Makefile offers the following general purpose options:"
	@$(ECHO) " "
	@$(ECHO) "    make clean           [ Delete all object files & backup files ]"
	@$(ECHO) "    make install         [ Install the binary & the manual page ]"
	@$(ECHO) "    make install_daemon  [ Install the index daemon binary ]"
	@$(ECHO) "    make install_setuid  [ Install the binary setuid & the manual page ]"
	@$(ECHO) "    make manpage         [ Create nroff version of manual page ]"
	@$(ECHO) "    make manifest        [ Create MANIFEST ]"
	@$(ECHO) "    make dist            [ Create a gziped distribution tar file ]"

install:
	@$(CD) $(SRCDIR); $(MAKE) install

install_setuid:
	@$(CD) $(SRCDIR); $(MAKE) install_setuid

install_daemon:
	@$(CD) $(SRCDIR); $(MAKE) install_daemon

clean:
	@$(CD) $(SRCDIR); $(MAKE) clean
	@$(RM) -f *~
	@$(RM) -f $(DOCDIR)/*~
	@$(RM) -f $(INCDIR)/*~
	@$(RM) -f $(SRCDIR)/*~

man:
	@$(MAKE) manpage

manpage:
	@$(ECHO) "Creating $(NROFF) man page for $(EXE)$(VER)..."
	@$(NROFF) -man $(DOCDIR)/$(EXE).1 > $(DOCDIR)/$(EXE).nrf

# Use 2 passes for creating MANIFEST because its size changes (it's not likely
# that we'll need 3 passes, since that'll happen only when the grand total's
# digits change).
manifest:
	@$(ECHO) "Creating MANIFEST..."
	@$(ECHO) "MANIFEST for $(PROJECT)-$(VER) (`date`)" > MANIFEST.tmp
	@$(ECHO) "----------------------------------------------------" >> MANIFEST.tmp
	@$(CP) MANIFEST.tmp MANIFEST
	@$(WC) -c $(ALL_FILES) >> MANIFEST
	@$(WC) -c $(ALL_FILES) >> MANIFEST.tmp
	@$(MV) MANIFEST.tmp MANIFEST

chmod:
	@$(ECHO) "Setting the file permissions..."
	@$(CHMOD) 644 $(ALL_FILES)
	@$(CHMOD) 755 $(ALL_DIRS)
	@$(CHMOD) 755 ./conf-tin ./config.guess ./config.sub ./configure ./install.sh tools/tinpp

tar:
	@$(ECHO) "Generating gzipped tar file..."
	@-$(RM) $(PROJECT)$(VER).tgz > /dev/null 2>&1  
	@$(TAR) cvzf $(PROJECT)$(VER).tgz -C ../ \
	`$(ECHO) $(ALL_FILES) \
	| $(TR) -s '[[:space:]]' "[\012*]" \
	| $(SED) 's,^\./,$(PROJECT)-$(PVER)/,' \
 	| $(TR) "[\012]" " "`
	@$(CHMOD) 644 $(PROJECT)$(VER).tgz
	@$(LS) $(PROJECT)$(VER).tgz

#
# I know it's ugly, but it works
#
name:
	@DATE=`date +%y%m%d`;  if test `pwd | cut -d '-' -f 2` != $$DATE ; then \
	mv ../`basename \`pwd\`` ../tin-$$DATE ; \
	sed "s,^PVER[[:space:]]*=[[:print:]]*,PVER		= $$DATE," ./Makefile > ./Makefile.tmp \
	&& mv ./Makefile.tmp ./Makefile ; \
	sed "s,RELEASEDATE[[:space:]]*\"[[:print:]]*\",RELEASEDATE	\"$$DATE\"," $(INCDIR)/version.h > $(INCDIR)/version.h.tmp \
	&& mv $(INCDIR)/version.h.tmp $(INCDIR)/version.h ; \
	sed "s,^PVER[[:space:]]*=[[:print:]]*,PVER		= $$DATE," ./makefile.in > ./makefile.in.tmp \
	&& mv ./makefile.in.tmp ./makefile.in ; \
	fi

dist:
	@$(MAKE) name
	@$(MAKE) configure
	@$(MAKE) manifest
	@$(MAKE) chmod
	@$(MAKE) tar

version :
	@$(ECHO) "TIN v$(VER)"

distclean:
	-@$(MAKE) clean
	@$(RM) -f config.cache config.log config.status
	@$(RM) -f $(INCDIR)/autoconf.h
	@$(RM) -f $(SRCDIR)/Makefile
	@$(RM) -f td-conf.out
	@$(RM) -f makefile
	
configure: configure.in aclocal.m4
	autoconf

config.status: configure
	./config.status --recheck
