# Top level Makefile for tin
# - for configuration options read the doc/INSTALL file.
#
# Updated: 1999-11-13
#

PROJECT	= tin
LVER	= 1
PVER	= 4
SVER	= 7
VER	= $(LVER).$(PVER).$(SVER)
DVER	= 20030312
EXE	= tin
MANEXT	= 1

# directory structure
TOPDIR	= .
DOCDIR	= ./doc
INCDIR	= ./include
OBJDIR	= ./src
SRCDIR	= ./src
AMGDIR	= ./amiga
VMSDIR	= ./vms
PCREDIR	= ./pcre
CANDIR	= ./libcanlock
TOLDIR	= ./tools

HFILES	= \
	$(INCDIR)/bool.h \
	$(INCDIR)/bugrep.h \
	$(INCDIR)/config.h \
	$(INCDIR)/extern.h \
	$(INCDIR)/menukeys.h \
	$(INCDIR)/nntplib.h \
	$(INCDIR)/policy.h \
	$(INCDIR)/proto.h \
	$(INCDIR)/stpwatch.h \
	$(INCDIR)/tcurses.h \
	$(INCDIR)/tin.h \
	$(INCDIR)/tinrc.h \
	$(INCDIR)/tnntp.h \
	$(INCDIR)/trace.h \
	$(INCDIR)/version.h

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
	$(DOCDIR)/CHANGES.old \
	$(DOCDIR)/DEBUG_REFS \
	$(DOCDIR)/INSTALL \
	$(DOCDIR)/TODO \
	$(DOCDIR)/WHATSNEW \
	$(DOCDIR)/auth.txt \
	$(DOCDIR)/filtering \
	$(DOCDIR)/internals.txt \
	$(DOCDIR)/iso2asc.txt \
	$(DOCDIR)/good-netkeeping-seal \
	$(DOCDIR)/pgp.txt \
	$(DOCDIR)/rcvars.txt \
	$(DOCDIR)/reading-mail.txt \
	$(DOCDIR)/umlaute.txt \
	$(DOCDIR)/umlauts.txt \
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
	$(TOPDIR)/README.WIN \
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
	$(PCREDIR)/LICENCE \
	$(PCREDIR)/Makefile.in \
	$(PCREDIR)/Makefile.orig \
	$(PCREDIR)/README \
	$(PCREDIR)/RunTest \
	$(PCREDIR)/Tech.Notes \
	$(PCREDIR)/pgrep.1 \
	$(PCREDIR)/pgrep.1.html \
	$(PCREDIR)/pgrep.1.txt \
	$(PCREDIR)/pcre.3 \
	$(PCREDIR)/pcre.3.html \
	$(PCREDIR)/pcre.3.txt \
	$(PCREDIR)/pcreposix.3 \
	$(PCREDIR)/pcreposix.3.html \
	$(PCREDIR)/pcreposix.3.txt \
	$(PCREDIR)/dftables.c \
	$(PCREDIR)/get.c \
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
	$(PCREDIR)/testinput1 \
	$(PCREDIR)/testinput2 \
	$(PCREDIR)/testinput3 \
	$(PCREDIR)/testinput4 \
	$(PCREDIR)/testoutput1 \
	$(PCREDIR)/testoutput2 \
	$(PCREDIR)/testoutput3 \
	$(PCREDIR)/testoutput4 \
	$(PCREDIR)/dll.mk \
	$(PCREDIR)/pcre.def

CAN	= \
	$(CANDIR)/Build \
	$(CANDIR)/CHANGES \
	$(CANDIR)/MANIFEST \
	$(CANDIR)/README \
	$(CANDIR)/base64.c \
	$(CANDIR)/base64.h \
	$(CANDIR)/canlock.h \
	$(CANDIR)/canlock_md5.c \
	$(CANDIR)/canlock_misc.c \
	$(CANDIR)/canlock_sha1.c \
	$(CANDIR)/canlocktest.c \
	$(CANDIR)/endian.c \
	$(CANDIR)/hmac_md5.c \
	$(CANDIR)/hmac_md5.h \
	$(CANDIR)/hmac_sha1.c \
	$(CANDIR)/hmac_sha1.h \
	$(CANDIR)/hmactest.c \
	$(CANDIR)/main.c \
	$(CANDIR)/md5.c \
	$(CANDIR)/md5.h \
	$(CANDIR)/sha1.c \
	$(CANDIR)/sha1.h \
	$(CANDIR)/doc/HOWTO \
	$(CANDIR)/doc/draft-ietf-usefor-cancel-lock-01.txt \
	$(CANDIR)/doc/rfc2104.txt \
	$(CANDIR)/doc/rfc2202.txt \
	$(CANDIR)/doc/rfc2286.txt

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

ALL_FILES = $(TOP) $(DOC) $(TOL) $(HFILES) $(CFILES) $(AMIGA) $(VMS) $(PCRE) $(MISC) $(CAN)

ALL_DIRS = $(TOPDIR) $(DOCDIR) $(SRCDIR) $(INCDIR) $(AMGDIR) $(VMSDIR) $(PCREDIR) $(CANDIR) $(CANDIR)/doc

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
TAR	= tar
GZIP	= gzip
BZIP2	= bzip2
WC	= wc
SED	= sed
TR	= tr

all:
	@$(ECHO) "Top level Makefile for the $(PROJECT) v$(VER) Usenet newsreader."
	@$(ECHO) " "
	@$(ECHO) "To compile the source code type 'make build' or change to the"
	@$(ECHO) "source directory by typing 'cd src' and then type 'make'."
	@$(ECHO) " "
	@$(ECHO) "This Makefile offers the following general purpose options:"
	@$(ECHO) " "
	@$(ECHO) "    make build           [ Compile $(PROJECT) ]"
	@$(ECHO) "    make clean           [ Delete all object and backup files ]"
	@$(ECHO) "    make dist            [ Create a gziped distribution tar file ]"
	@$(ECHO) "    make distclean       [ Delete all config, object and backup files ]"
	@$(ECHO) "    make install         [ Install the binary and the manual page ]"
	@$(ECHO) "    make install_daemon  [ Install the index daemon binary ]"
	@$(ECHO) "    make install_setuid  [ Install the binary setuid & the manual page ]"
	@$(ECHO) "    make install_sysdefs [ Install the system-wide-defaults file ]"
	@$(ECHO) "    make manpage         [ Create nroff version of manual page ]"
	@$(ECHO) "    make manifest        [ Create MANIFEST ]"
	@$(ECHO) " "

build:
	@$(CD) $(SRCDIR) && $(MAKE)

install:
	@$(CD) $(SRCDIR) && $(MAKE) install

install_setuid:
	@$(CD) $(SRCDIR) && $(MAKE) install_setuid

install_daemon:
	@$(CD) $(SRCDIR) && $(MAKE) install_daemon

install_sysdefs:
	@$(CD) $(SRCDIR) && $(MAKE) install_sysdefs

clean:
	@-$(RM) -f \
	*~ \
	$(DOCDIR)/*~ \
	$(INCDIR)/*~ \
	$(SRCDIR)/*~ \
	$(PCREDIR)/*~
	@-if test -f $(SRCDIR)/Makefile ; then $(CD) $(SRCDIR) && $(MAKE) clean ; fi
	@-if test -f $(PCREDIR)/Makefile ; then $(CD) $(PCREDIR) && $(MAKE) clean ; fi

man:
	@$(MAKE) manpage

manpage:
	@$(ECHO) "Creating $(NROFF) man page for $(EXE)-$(VER)..."
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
	@$(CHMOD) 755 \
	$(ALL_DIRS) \
	./conf-tin \
	./config.guess \
	./config.sub \
	./configure \
	./install.sh \
	./mkdirs.sh \
	$(TOLDIR)/tinpp \
	$(TOLDIR)/metamutt \
	$(PCREDIR)/perltest \
	$(CANDIR)/Build

tar:
	@$(ECHO) "Generating gzipped tar file..."
	@-$(RM) $(PROJECT)-$(VER).tgz > /dev/null 2>&1
	@$(TAR) cvf $(PROJECT)-$(VER).tar -C ../ \
	`$(ECHO) $(ALL_FILES) \
	| $(TR) -s '[[:space:]]' "[\012*]" \
	| $(SED) 's,^\./,$(PROJECT)-$(VER)/,' \
	| $(TR) "[\012]" " "`
	@$(GZIP) -9 $(PROJECT)-$(VER).tar
	@$(CHMOD) 644 $(PROJECT)-$(VER).tar.gz
	@$(LS) $(PROJECT)-$(VER).tar.gz

bzip2:
	@$(ECHO) "Generating bzipped tar file..."
	@-$(RM) $(PROJECT)-$(VER).tar.bz2 > /dev/null 2>&1
	@$(TAR) cvf $(PROJECT)-$(VER).tar -C ../ \
	`$(ECHO) $(ALL_FILES) \
	| $(TR) -s '[[:space:]]' "[\012*]" \
	| $(SED) 's,^\./,$(PROJECT)-$(VER)/,' \
	| $(TR) "[\012]" " "`
	@$(BZIP2) -9 $(PROJECT)-$(VER).tar
	@$(CHMOD) 644 $(PROJECT)-$(VER).tar.bz2
	@$(LS) $(PROJECT)-$(VER).tar.bz2

#
# I know it's ugly, but it works
#
name:
	@DATE=`date +%Y%m%d` ; NAME=`basename \`pwd\`` ;\
	if test $$NAME != "$(PROJECT)-$(VER)" ; then \
		$(MV) ../$$NAME ../$(PROJECT)-$(VER) ;\
	fi ;\
	$(SED) "s,^DVER[[:space:]]*=[[:print:]]*,DVER	= $$DATE," ./Makefile > ./Makefile.tmp && \
	$(MV) ./Makefile.tmp ./Makefile ;\
	$(SED) "s,RELEASEDATE[[:space:]]*\"[[:print:]]*\",RELEASEDATE	\"$$DATE\"," $(INCDIR)/version.h > $(INCDIR)/version.h.tmp && \
	$(SED) "s, VERSION[[:space:]]*\"[[:print:]]*\", VERSION 	\"$(VER)\"," $(INCDIR)/version.h.tmp > $(INCDIR)/version.h && $(RM) -f $(INCDIR)/version.h.tmp ;\
	$(SED) "s,^DVER[[:space:]]*=[[:print:]]*,DVER		= $$DATE," ./makefile.in > ./makefile.in.tmp && \
	$(MV) ./makefile.in.tmp ./makefile.in

dist:
	@$(MAKE) name
	@$(MAKE) configure
	@$(MAKE) manifest
	@$(MAKE) chmod
	@$(MAKE) tar

version :
	@$(ECHO) "$(PROJECT)-$(VER)"

distclean:
	@-$(MAKE) clean
	@-$(RM) -f \
	makefile \
	config.cache \
	config.log \
	config.status \
	td-conf.out \
	$(INCDIR)/autoconf.h \
	$(PCREDIR)/chartables.c \
	$(PCREDIR)/dftables \
	$(PCREDIR)/Makefile \
	$(SRCDIR)/Makefile \
	$(CANDIR)/*.[oa] \
	$(CANDIR)/endian.h \
	$(CANDIR)/canlocktest \
	$(CANDIR)/endian \
	$(CANDIR)/hmactest

configure: configure.in aclocal.m4
	autoconf

config.status: configure
	./config.status --recheck
