# Top level Makefile for tin - for configuration options read the ./INSTALL file.
# 
PROJECT		= tin
EXE		= tin
MANEXT		= 1
BASE_VER	= 950824
LVER		= 1.3_$(BASE_VER)
PPREFIX		= MC
PVER		= 960609
VER		= $(LVER)BETA_PL$(PPREFIX).$(PVER)
MAIL_ADDR 	= "urs@akk.uni-karlsruhe.de"

# directory structure
TOPDIR	= .
DOCDIR	= ./doc
INCDIR	= ./include
OBJDIR	= ./src
SRCDIR	= ./src

HFILES	= \
	$(INCDIR)/amiga.h \
	$(INCDIR)/amigatcp.h \
	$(INCDIR)/config.h \
	$(INCDIR)/extern.h \
	$(INCDIR)/gst.h \
	$(INCDIR)/menukeys.h \
	$(INCDIR)/msmail.h \
	$(INCDIR)/nntplib.h \
	$(INCDIR)/os_2.h \
	$(INCDIR)/patchlev.h \
	$(INCDIR)/proto.h \
	$(INCDIR)/stpwatch.h \
	$(INCDIR)/tin.h \
	$(INCDIR)/win32.h \
	$(INCDIR)/win32tcp.h

CFILES	= \
	$(SRCDIR)/active.c \
	$(SRCDIR)/actived.c \
	$(SRCDIR)/amiga.c \
	$(SRCDIR)/amigatcp.c \
	$(SRCDIR)/art.c \
	$(SRCDIR)/attrib.c \
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
	$(SRCDIR)/lang.c \
	$(SRCDIR)/list.c \
	$(SRCDIR)/mail.c \
	$(SRCDIR)/main.c \
	$(SRCDIR)/memory.c \
	$(SRCDIR)/misc.c \
	$(SRCDIR)/msmail.c \
	$(SRCDIR)/newsrc.c\
	$(SRCDIR)/nntplib.c \
	$(SRCDIR)/nntpw32.c \
	$(SRCDIR)/open.c \
	$(SRCDIR)/os_2.c \
	$(SRCDIR)/page.c \
	$(SRCDIR)/parsdate.y \
	$(SRCDIR)/pgp.c \
	$(SRCDIR)/post.c \
	$(SRCDIR)/prompt.c \
	$(SRCDIR)/proto.sed \
	$(SRCDIR)/refs.c \
	$(SRCDIR)/rfc1521.c \
	$(SRCDIR)/rfc1522.c \
	$(SRCDIR)/save.c \
	$(SRCDIR)/screen.c \
	$(SRCDIR)/search.c \
	$(SRCDIR)/select.c \
	$(SRCDIR)/sigfile.c \
	$(SRCDIR)/signal.c \
	$(SRCDIR)/strftime.c \
	$(SRCDIR)/thread.c \
	$(SRCDIR)/wildmat.c \
	$(SRCDIR)/win32.c \
	$(SRCDIR)/win32tcp.c \
	$(SRCDIR)/xref.c \
	$(SRCDIR)/vms.c

DOC	= \
	$(DOCDIR)/$(EXE).$(MANEXT) \

TOP	= \
	$(TOPDIR)/CHANGES \
	$(TOPDIR)/TODO \
	$(TOPDIR)/Makefile \
	$(TOPDIR)/INSTALL \
	$(TOPDIR)/MANIFEST \
	$(TOPDIR)/README \
	$(TOPDIR)/tinpp \
	$(TOPDIR)/aclocal.m4 \
	$(TOPDIR)/configure \
	$(TOPDIR)/configure.in \
	$(TOPDIR)/install.sh \
	$(TOPDIR)/makefile.in \
	$(TOPDIR)/conf-tin

ALL_FILES = $(TOP) $(DOC) $(HFILES) $(CFILES) \
	$(INCDIR)/autoconf.hin \
	$(SRCDIR)/Makefile.in

ALL_DIRS = $(TOPDIR) $(DOCDIR) $(SRCDIR) $(INCDIR)

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
TROFF		= groff
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
	@$(CHMOD) 755 ./conf-tin ./tinpp ./configure

tar:
	@$(ECHO) "Generating gzipped tar file..."
	@-$(RM) $(PROJECT)$(VER).tgz > /dev/null 2>&1  
	@$(TAR) cvzf $(PROJECT)$(VER).tgz -C ../ \
	`$(ECHO) $(ALL_FILES) \
	| $(TR) -s '[[:space:]]' "[\012*]" \
	| $(SED) 's/^/$(PVER)\//' \
 	| $(TR) "[\012]" " "`
	@$(CHMOD) 644 $(PROJECT)$(VER).tgz
	@$(LS) $(PROJECT)$(VER).tgz

dist:
	@$(MAKE) chmod
	@$(MAKE) manifest
	@$(MAKE) tar

configure: configure.in aclocal.m4
	autoconf

config.status: configure
	./config.status --recheck
