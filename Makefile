# Top level Makefile for tin
# - for configuration options read the doc/INSTALL file.
#
# Updated: 1998-02-15
#

PROJECT	= tin
EXE	= tin
MANEXT	= 1
LVER	= 1.4
PVER	= 981114
VER	= pre-$(LVER)-$(PVER)

# directory structure
TOPDIR	= .
DOCDIR	= ./doc
INCDIR	= ./include
OBJDIR	= ./src
SRCDIR	= ./src
AMGDIR	= ./amiga
VMSDIR	= ./vms
PCREDIR	= ./pcre
TOLDIR	= ./tools
OLDDIR	= ./old

HFILES	= \
	$(INCDIR)/bool.h \
	$(INCDIR)/bugrep.h \
	$(INCDIR)/config.h \
	$(INCDIR)/extern.h \
	$(INCDIR)/menukeys.h \
	$(INCDIR)/nntplib.h \
	$(INCDIR)/proto.h \
	$(INCDIR)/stpwatch.h \
	$(INCDIR)/tcurses.h \
	$(INCDIR)/tin.h \
	$(INCDIR)/tnntp.h \
	$(INCDIR)/trace.h \
	$(INCDIR)/version.h \

CFILES	= \
	$(SRCDIR)/active.c \
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
	$(SRCDIR)/header.c \
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
	$(SRCDIR)/newsrc.c\
	$(SRCDIR)/nntplib.c \
	$(SRCDIR)/nrctbl.c \
	$(SRCDIR)/open.c \
	$(SRCDIR)/page.c \
	$(SRCDIR)/parsdate.y \
	$(SRCDIR)/pgp.c \
	$(SRCDIR)/post.c \
	$(SRCDIR)/prompt.c \
	$(SRCDIR)/read.c \
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
	$(SRCDIR)/xref.c

AMIGA	= \
	$(AMGDIR)/README \
	$(AMGDIR)/tin.readme \
	$(AMGDIR)/smakefile \
	$(AMGDIR)/actived.c \
	$(AMGDIR)/amiga.c \
	$(AMGDIR)/amigatcp.c \
	$(AMGDIR)/amiga.h \
	$(AMGDIR)/amigatcp.h

VMS	= \
	$(VMSDIR)/dir.h \
	$(VMSDIR)/filetypes.h \
	$(VMSDIR)/getopt.c \
	$(VMSDIR)/getopt.h \
	$(VMSDIR)/getopt1.c \
	$(VMSDIR)/getpass.c \
	$(VMSDIR)/isxterm.c \
	$(VMSDIR)/libvms.mms \
	$(VMSDIR)/ndir.h \
	$(VMSDIR)/parsdate.c \
	$(VMSDIR)/parse.c \
	$(VMSDIR)/parse.h \
	$(VMSDIR)/pwd.h \
	$(VMSDIR)/qsort.c \
	$(VMSDIR)/select.h \
	$(VMSDIR)/strings.h \
	$(VMSDIR)/uaf.h \
	$(VMSDIR)/vms.c \
	$(VMSDIR)/vmsdir.c \
	$(VMSDIR)/vmsfile.c \
	$(VMSDIR)/vmspwd.c \
	$(VMSDIR)/vmstimval.h

DOC	= \
	$(DOCDIR)/CHANGES \
	$(DOCDIR)/DEBUG_REFS \
	$(DOCDIR)/INSTALL \
	$(DOCDIR)/TODO \
	$(DOCDIR)/WHATSNEW \
	$(DOCDIR)/auth.txt \
	$(DOCDIR)/filtering \
	$(DOCDIR)/internals.txt \
	$(DOCDIR)/iso2asc.txt \
	$(DOCDIR)/good-netkeeping-seal \
	$(DOCDIR)/umlaute.txt \
	$(DOCDIR)/tin.defaults \
	$(DOCDIR)/$(EXE).$(MANEXT)

TOL	= \
	$(TOLDIR)/metamutt \
	$(TOLDIR)/tinpp \
	$(TOLDIR)/expand_aliases.tgz

TOP	= \
	$(TOPDIR)/Makefile \
	$(TOPDIR)/MANIFEST \
	$(TOPDIR)/README \
	$(TOPDIR)/README.VMS \
	$(TOPDIR)/aclocal.m4 \
	$(TOPDIR)/conf-tin \
	$(TOPDIR)/config.guess \
	$(TOPDIR)/config.sub \
	$(TOPDIR)/configure \
	$(TOPDIR)/configure.in \
	$(TOPDIR)/install.sh \
	$(TOPDIR)/makefile.in \
	$(TOPDIR)/mkdirs.sh

PCRE	= \
	$(PCREDIR)/ChangeLog \
	$(PCREDIR)/Makefile.in \
	$(PCREDIR)/Makefile.orig \
	$(PCREDIR)/RunTest \
	$(PCREDIR)/README \
	$(PCREDIR)/Tech.Notes \
	$(PCREDIR)/pgrep.1 \
	$(PCREDIR)/pcre.3 \
	$(PCREDIR)/pcreposix.3 \
	$(PCREDIR)/deftables.c \
	$(PCREDIR)/maketables.c \
	$(PCREDIR)/pcre.c \
	$(PCREDIR)/pcreposix.c \
	$(PCREDIR)/pgrep.c \
	$(PCREDIR)/study.c \
	$(PCREDIR)/internal.h \
	$(PCREDIR)/pcre.h \
	$(PCREDIR)/pcreposix.h \
	$(PCREDIR)/pcretest.c \
	$(PCREDIR)/perltest \
	$(PCREDIR)/testinput \
	$(PCREDIR)/testinput2 \
	$(PCREDIR)/testoutput \
	$(PCREDIR)/testoutput2

OLD	= \
	$(OLDDIR)/msmail.c \
	$(OLDDIR)/nntpvms.c \
	$(OLDDIR)/nntpw32.c \
	$(OLDDIR)/os_2.c \
	$(OLDDIR)/win32.c \
	$(OLDDIR)/win32tcp.c \
	$(OLDDIR)/msmail.h \
	$(OLDDIR)/os_2.h \
	$(OLDDIR)/win32.h \
	$(OLDDIR)/win32tcp.h

MISC	= \
	$(INCDIR)/autoconf.hin \
	$(SRCDIR)/Makefile.in \
	$(SRCDIR)/ibm437_l1.tab \
	$(SRCDIR)/ibm850_l1.tab \
	$(SRCDIR)/l1_ibm437.tab \
	$(SRCDIR)/l1_ibm850.tab \
	$(SRCDIR)/l1_next.tab \
	$(SRCDIR)/next_l1.tab \
	$(SRCDIR)/tincfg.tbl \
	$(SRCDIR)/descrip.mms \
	$(PCREDIR)/pcre.mms

ALL_FILES = $(TOP) $(DOC) $(TOL) $(HFILES) $(CFILES) $(AMIGA) $(VMS) $(PCRE) $(MISC) $(OLD)

ALL_DIRS = $(TOPDIR) $(DOCDIR) $(SRCDIR) $(INCDIR) $(AMGDIR) $(VMSDIR) $(PCREDIR) $(OLDDIR)

# standard commands
CD	= cd
CHMOD	= chmod
CP	= cp -p
ECHO	= echo
LS	= ls -l
MAKE	= make
MV	= mv
NROFF	= groff -Tascii
RM	= rm
SHELL	= /bin/sh
TAR	= gtar
BZIP2	= bzip2
WC	= wc
SED	= sed
TR	= tr

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
	@$(CD) $(SRCDIR) && $(MAKE) install

install_setuid:
	@$(CD) $(SRCDIR) && $(MAKE) install_setuid

install_daemon:
	@$(CD) $(SRCDIR) && $(MAKE) install_daemon

clean:
	@-$(RM) -f *~
	@-$(RM) -f $(DOCDIR)/*~
	@-$(RM) -f $(INCDIR)/*~
	@-$(RM) -f $(SRCDIR)/*~
	@-$(RM) -f $(PCREDIR)/*~
	@-test -e $(SRCDIR)/Makefile && $(CD) $(SRCDIR) && $(MAKE) clean

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
	@$(CHMOD) 755 ./conf-tin ./config.guess ./config.sub ./configure ./install.sh ./mkdirs.sh
	@$(CHMOD) 755 $(TOLDIR)/tinpp $(TOLDIR)/metamutt $(PCREDIR)/perltest

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

bzip2:
	@$(ECHO) "Generating bzipped tar file..."
	@-$(RM) $(PROJECT)$(VER).tgz > /dev/null 2>&1
	@$(TAR) cvf $(PROJECT)$(VER).tar -C ../ \
	`$(ECHO) $(ALL_FILES) \
	| $(TR) -s '[[:space:]]' "[\012*]" \
	| $(SED) 's,^\./,$(PROJECT)-$(PVER)/,' \
	| $(TR) "[\012]" " "`
	@$(BZIP2) $(PROJECT)$(VER).tar
	@$(CHMOD) 644 $(PROJECT)$(VER).tar.bz2
	@$(LS) $(PROJECT)$(VER).tar.bz2

#
# I know it's ugly, but it works
#
name:
	@DATE=`date +%y%m%d`; if test `pwd | cut -d '-' -f 2` != $$DATE ; then \
	$(MV) ../`basename \`pwd\`` ../tin-$$DATE ; \
	$(SED) "s,^PVER[[:space:]]*=[[:print:]]*,PVER	= $$DATE," ./Makefile > ./Makefile.tmp \
	&& $(MV) ./Makefile.tmp ./Makefile ; \
	$(SED) "s,RELEASEDATE[[:space:]]*\"[[:print:]]*\",RELEASEDATE	\"$$DATE\"," $(INCDIR)/version.h > $(INCDIR)/version.h.tmp \
	&& $(MV) $(INCDIR)/version.h.tmp $(INCDIR)/version.h ; \
	$(SED) "s,^PVER[[:space:]]*=[[:print:]]*,PVER		= $$DATE," ./makefile.in > ./makefile.in.tmp \
	&& $(MV) ./makefile.in.tmp ./makefile.in ; \
	fi

dist:
	@$(MAKE) name
	@$(MAKE) configure
	@$(MAKE) manifest
	@$(MAKE) chmod
	@$(MAKE) tar

version :
	@$(ECHO) "TIN $(VER)"

distclean:
	@-$(MAKE) clean
	@-$(RM) -f config.cache config.log config.status
	@-$(RM) -f $(INCDIR)/autoconf.h
	@-$(RM) -f $(PCREDIR)/maketables $(PCREDIR)/chartables.c
	@-$(RM) -f $(PCREDIR)/Makefile
	@-$(RM) -f $(SRCDIR)/Makefile
	@-$(RM) -f td-conf.out
	@-$(RM) -f makefile

configure: configure.in aclocal.m4
	autoconf

config.status: configure
	./config.status --recheck
