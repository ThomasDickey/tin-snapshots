# Source Makefile for tin - for configuration options read the ../INSTALL file.
# 
# Compiler, linker & parser commands
CC		= cc
#CC		= gcc
#CC		= checkergcc
DEBUG	= # -DDEBUG -DDEBUG_NEWSRC -DPROFILE
DEBUG	= #-g -DDEBUG
# -Wstrict-prototypes -Wnested-externs -Wmissing-prototypes -Wredundant-decls
COPTS	= -c  -DHAVE_PGP -DHAVE_COREDUMP -g -I$(INCDIR) $(DEBUG) -DNNTP_ONLY # -DHAVE_MAIL_HANDLING
#COPTS	= -I$(INCDIR) $(DEBUG) -c -Wall -Wconversion -Wstrict-prototypes -Wmissing-prototypes  -DHAVE_MAIL_HANDLING
LD		= $(CC)
#LD		= cc
YACC	= bison -y
#YACC    = yacc

# From: address in posted articles (don't use both - read the INSTALL file)
# Put the gateway name (ie. news.foo.com) / domain name (ie. .foo.com) here
INEWS_MAIL_GATEWAY=/etc/inews_mail_gateway
INEWS_MAIL_DOMAIN=
#.scn.de
#.subdomain.domain

# Default nntp server (can be overridden by NNTPSERVER environment variable)
NNTP_DEFAULT_SERVER=
#news.scn.de
#news-server.subdomain.domain

# Where are your news libdir & spooldir?
LIBDIR	= /usr/lib/news
SPOOLDIR= /usr/spool/news
NOVROOTDIR= $(SPOOLDIR)

# Where do you want the binary & manual page installed?
INS_BINARY_DIR	= /usr/local/bin
INS_DAEMON_DIR	= $(LIBDIR)
INS_MANUAL_DIR	= /usr/local/man/man
INS_MANUAL_EXT	= 1

# Use INN clientlibs config functions (add -DUSE_INN_NNTPLIB to COPTS line)
INN_NNTPLIB=

# Project
PROJECT	= tin
EXE		= tin
EXED	= tind
VER		= 1.30
MANEXT	= 1

# directory structure
DOCDIR	= ../doc
INCDIR	= ../include
BINDIR	= .
OBJDIR	= .
SRCDIR	= .

# standard commands
CD		= cd
CHGRP	= chown
CHMOD	= chmod
CHOWN	= chown
CFLOW	= cflow
CP		= cp
CPROTO	= cproto
CSCOPE	= cscope
ECHO	= echo
LINT	= lint
LN		= ln
LS		= ls -l
MAKE	= make
MV		= mv
RM		= rm
SHELL	= /bin/sh
STRIP	= strip
STRIP2	= mcs -d
SU		= su
TAGS	= ctags

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
	$(SRCDIR)/prompt.c\
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

OFILES	= \
	$(OBJDIR)/active.o \
	$(OBJDIR)/actived.o \
	$(OBJDIR)/amiga.o \
	$(OBJDIR)/art.o \
	$(OBJDIR)/attrib.o \
	$(OBJDIR)/charset.o \
	$(OBJDIR)/config.o \
	$(OBJDIR)/curses.o \
	$(OBJDIR)/debug.o\
	$(OBJDIR)/envarg.o \
	$(OBJDIR)/feed.o \
	$(OBJDIR)/filter.o \
	$(OBJDIR)/getline.o \
	$(OBJDIR)/group.o \
	$(OBJDIR)/hashstr.o \
	$(OBJDIR)/help.o\
	$(OBJDIR)/inews.o \
	$(OBJDIR)/init.o \
	$(OBJDIR)/lang.o \
	$(OBJDIR)/list.o \
	$(OBJDIR)/mail.o \
	$(OBJDIR)/main.o \
	$(OBJDIR)/memory.o \
	$(OBJDIR)/misc.o \
	$(OBJDIR)/newsrc.o\
	$(OBJDIR)/nntplib.o \
	$(OBJDIR)/nntpw32.o \
	$(OBJDIR)/open.o \
	$(OBJDIR)/os_2.o \
	$(OBJDIR)/page.o \
	$(OBJDIR)/parsdate.o \
	$(OBJDIR)/pgp.o \
	$(OBJDIR)/post.o \
	$(OBJDIR)/prompt.o\
	$(OBJDIR)/save.o \
	$(OBJDIR)/screen.o \
	$(OBJDIR)/search.o \
	$(OBJDIR)/select.o \
	$(OBJDIR)/sigfile.o \
	$(OBJDIR)/signal.o \
	$(OBJDIR)/strftime.o \
	$(OBJDIR)/thread.o \
	$(OBJDIR)/wildmat.o \
	$(OBJDIR)/win32.o \
	$(OBJDIR)/xref.o \
	$(OBJDIR)/vms.o


ALL_FILES = $(HFILES) $(CFILES) $(NNTP)

LINTFLAGS=-a -c -h -n -x 

all: 
	@$(ECHO) "Makefile for the TIN v$(VER) Usenet newsreader. Configuration:"
	@$(ECHO) " "
	@$(ECHO) "  Compiler=[$(CC)]  Copts=[$(COPTS)]  INN Lib=[$(INN_NNTPLIB)]"
	@$(ECHO) "  Install Bindir=[$(INS_BINARY_DIR)]  Mandir=[$(INS_MANUAL_DIR)$(INS_MANUAL_EXT)]  Manext=[$(INS_MANUAL_EXT)]"
	@$(ECHO) "  News Lib=[$(LIBDIR)]  Spool=[$(SPOOLDIR)]  Nov=[$(NOVROOTDIR)]"
	@$(ECHO) "  News Gateway=[$(INEWS_MAIL_GATEWAY)]  Domain=[$(INEWS_MAIL_DOMAIN)]  Server=[$(NNTP_DEFAULT_SERVER)]"
	@$(ECHO) " "
	@$(ECHO) "If the above options are OK make one of the following targets:"
	@$(ECHO) " "
	@$(ECHO) "  make bsd    [ BSD[I] / DGUX / NeXT / OSF1 / Pyramid / SunOS / Ultrix (nntp) ]"
	@$(ECHO) "  make sysv   [ SysV / HPUX 7&8 ]    make sysvr4  [ SysVR4 / HPUX 9 ]"
	@$(ECHO) "  make aix    [ IBM AIX (nntp) ]     make apollo  [ Apollo DomainOS ]"
	@$(ECHO) "  make coh    [ Coherent 4.0 ]       make dynix   [ Sequent DYNIX (nntp) ]"
	@$(ECHO) "  make irix   [ SGI Irix ]           make linux   [ Linux (nntp) ]"
	@$(ECHO) "  make minix  [ Minix 386 ]          make mips    [ Mips / CDC EPIX ]"
	@$(ECHO) "  make ptx    [ Sequent PTX ]        make qnx     [ QNX 4.2 ]"
	@$(ECHO) "  make sco    [ SCO Unix ]           make sinix   [ SNI Sinix ]"
	@$(ECHO) "  make tower  [ NCR Tower]           make xenix   [ SCO Xenix 386 ]"
	@$(ECHO) "  make sol    [ Solaris 2.X ]"
	@$(ECHO) " "
	@$(ECHO) "Note that targets marked with '(nntp)' have -DNNTP_ABLE defined automatically."
	@$(ECHO) " "

.c.o:
	$(CC) $(CFLAGS) $*.c

# Uncomment for COHERENT os
#.c.y:
#	$(YACC) $*.y


# For IBM AIX
aix:
	@$(ECHO) "Compiling $(EXE) v$(VER) for IBM AIX (NNTP Ready)..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -DSYSV -DRS6000 \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lcurses -ltermcap $(INN_NNTPLIB)" \
			EXE=tin linkit

# For Apollo 
apollo:
	@$(ECHO) "Compiling $(EXE) v$(VER) for Apollo..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -D_INCLUDE_BSD_SOURCE \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lcurses -ltermcap $(INN_NNTPLIB)" \
			EXE=tin linkit

# For BSD-derived systems
bsd:
	@$(ECHO) "Compiling $(EXE) v$(VER) for BSD/BSDI/DGUX/NeXT/OSF1/Pyramid/SunOS/Ultrix (NNTP Ready)..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -DBSD \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lcurses -ltermcap $(INN_NNTPLIB)" \
			EXE=tin linkit

# For Coherent 4.0
#			 NETLIBS="-lsocket" \
#
coherent:
	@$(ECHO) "Compiling $(EXE) v$(VER) for Coherent 4.0..."
	@$(MAKE) CC=$(CC) CFLAGS='-c -DSYSV \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lterm $(INN_NNTPLIB) $(INN_NNTPLIB)" \
			LFLAGS= EXE=tin linkit

# For Sequent DYNIX
dynix:
	@$(ECHO) "Compiling $(EXE) v$(VER) for Sequent DYNIX (NNTP Ready)..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -DBSD \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lcurses -ltermcap -lseq $(INN_NNTPLIB)" \
			EXE=tin linkit

# For SGI Irix
irix:
	@$(ECHO) "Compiling $(EXE) v$(VER) for SGI Irix..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -cckr -DSYSV \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lsun -lcurses -ltermcap $(INN_NNTPLIB)" \
			EXE=tin linkit

# For Linux
linux:
	@$(ECHO) "Compiling $(EXE) v$(VER) for Linux (NNTP Ready)..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lcurses -ltermcap $(INN_NNTPLIB)" \
			EXE=tin linkit

# For Minix 386
minix:
	@$(ECHO) "Compiling $(EXE) v$(VER) for Minix 386..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -DMINIX -D_POSIX_SOURCE \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lcurses $(INN_NNTPLIB)" \
			EXE=tin linkit
	chmem +131072 $(EXE) 

# For Mips/CDC EPIX
mips:
	@$(ECHO) "Compiling $(EXE) v$(VER) for Mips/CDC EPIX..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -DBSD -DEPIX -systype bsd43 \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-L/bsd43/usr/lib -lcurses -ltermcap -lc $(INN_NNTPLIB)" \
			EXE=tin linkit

# For Sequent PTX
#			 NETLIBS="-linet -lnsl" \
#
ptx:
	@$(ECHO) "Compiling $(EXE) v$(VER) for Sequent PTX..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -DPTX \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lsocket -lcurses -ltermcap $(INN_NNTPLIB)" \
			EXE=tin linkit

# For QNX
qnx:
	@$(ECHO) "Compiling $(EXE) v$(VER) for QNX 4.2 (Watcom C 9.5)..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -3 -M -fi=unix.h -D_POSIX_SOURCE_ -DQNX42 \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lcurses $(INN_NNTPLIB)" \
			LFLAGS="-M" \
			EXE=tin linkit

# For SCO Unix
#			 NETLIBS="-lnsl_s -lsocket" \
#
sco:
	@$(ECHO) "Compiling $(EXE) v$(VER) for SCO Unix..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -DSYSV -DSCO_UNIX -UM_XENIX \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lcurses -lgen -lc_s $(INN_NNTPLIB)" \
			EXE=tin linkit

# For SNI Sinix
sinix:
	@$(ECHO) "Compiling $(EXE) v$(VER) for SNI Sinix..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) \
			-DINEWS_MAIL_GATEWAY="\\"$(INEWS_MAIL_GATEWAY)\\" \
			-DINEWS_MAIL_DOMAIN="\\"$(INEWS_MAIL_DOMAIN)\\" \
			-DNNTP_DEFAULT_SERVER="\\"$(NNTP_DEFAULT_SERVER)\\" \
			-DLIBDIR=\\"$(LIBDIR)\\" 
			-DSPOOLDIR=\\"$(SPOOLDIR)\\" \
			-DNOVROOTDIR=\\"$(NOVROOTDIR)\\"' \
			LIBS="-lcurses $(INN_NNTPLIB)" \
			EXE=tin linkit

# For System V
#			 NETLIBS="-lnet -lnsl_s" \
#
sysv:
	@$(ECHO) "Compiling $(EXE) v$(VER) for System V..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -DSYSV \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lcurses -ltermcap $(INN_NNTPLIB)" \
			EXE=tin linkit

# For SUN Solaris 2.X
#
sol:
	@$(ECHO) "Compiling $(EXE) v$(VER) for SUN Solaris 2.X..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -DSVR4 \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lcurses -ltermcap $(INN_NNTPLIB)" \
			NETLIBS="-lnsl -lsocket" \
			EXE=tin linkit

# For System V Release 4
#			 NETLIBS="-lnsl -lsocket" \
#
sysvr4:
	@$(ECHO) "Compiling $(EXE) v$(VER) for System V Release 4..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -DSVR4 \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lcurses -ltermcap $(INN_NNTPLIB)" \
			EXE=tin linkit

# For NCR Tower
#			 NETLIBS="-lnet -lnsl_s" \
#
tower:
	@$(ECHO) "Compiling $(EXE) v$(VER) for NCR Tower..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -DSYSV -DNCR -DISTRING \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lcurses -ltermcap $(INN_NNTPLIB)" \
			EXE=tin linkit

# For Xenix 386
#			 NETLIBS="-lsocket" \
#
xenix:
	@$(ECHO) "Compiling $(EXE) v$(VER) for Xenix 386..."
	@$(MAKE) CC=$(CC) CFLAGS='-c -Zi -DSYSV \
			-DINEWS_MAIL_GATEWAY="\\"$(INEWS_MAIL_GATEWAY)\\" \
			-DINEWS_MAIL_DOMAIN="\\"$(INEWS_MAIL_DOMAIN)\\" \
			-DNNTP_DEFAULT_SERVER="\\"$(NNTP_DEFAULT_SERVER)\\" \
			-DLIBDIR=\\"$(LIBDIR)\\" 
			-DSPOOLDIR=\\"$(SPOOLDIR)\\" \
			-DNOVROOTDIR=\\"$(NOVROOTDIR)\\"' \
			LIBS="-lcurses -ltinfo -lx $(INN_NNTPLIB)" \
			LFLAGS=-Zi EXE=tin linkit

# !!!
# !!!  THE FOLLOWING ARE SITE SPECIFIC - IGNORE  !!!
# !!!

os2unix:
	@$(ECHO) "Compiling $(EXE) v$(VER) for OS2UNIX with NNTP ONLY..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -DSYSV -DNNTP_ONLY \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			LIBS="-lcurses -ltermcap $(INN_NNTPLIB)" \
			NETLIBS="-linet -lnsl_s" \
			EXE=tin linkit

mx351:
	@$(ECHO) "Compiling $(EXE) v$(VER) for MX351 with NNTP ONLY..."
	@$(MAKE) CC=$(CC) CFLAGS='$(COPTS) -DSVR4 -DNNTP_ONLY \
			-DINEWS_MAIL_GATEWAY=\"$(INEWS_MAIL_GATEWAY)\" \
			-DINEWS_MAIL_DOMAIN=\"$(INEWS_MAIL_DOMAIN)\" \
			-DNNTP_DEFAULT_SERVER=\"$(NNTP_DEFAULT_SERVER)\" \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"$(NOVROOTDIR)\"' \
			NETLIBS="-lnsl -lsocket" \
			LIBS="-lcurses -ltermlib $(INN_NNTPLIB)" \
			EXE=tin linkit

daemon:
	@$(ECHO) "Compiling $(EXE) v$(VER) for ANL433 with INDEX DAEMON..."
	@$(MAKE) CC=$(CC) CFLAGS='-c -O -I$(INCDIR) -DSYSV -DINDEX_DAEMON -DDEBUG \
			-DLIBDIR=\"$(LIBDIR)\" \
			-DSPOOLDIR=\"$(SPOOLDIR)\" \
			-DNOVROOTDIR=\"/usr/spool/nov\"' \
			 EXE=$(EXED) linkit

actived: actived.c mail.c
	@$(ECHO) "Compiling actived..."
	$(CC) $(COPTS) -DACTIVE_DAEMON -DINDEX_DAEMON actived.c mail.c
	$(CC) actived.o mail.o -o actived $(LIBS)
	@$(LS) actived


linkit: $(BINDIR)/$(EXE) 
$(BINDIR)/$(EXE): $(OFILES)
	@$(ECHO) "Linking $(EXE) v$(VER)..."
	$(LD) $(LFLAGS) -o $(BINDIR)/$(EXE) $(OFILES) $(NNTPLIB) $(NETLIBS) $(LIBS) $(DEBUG)
	@$(LS) $(BINDIR)/$(EXE)

install_manpage:
	@$(ECHO) "Installing $(EXE) manual page to $(INS_MANUAL_DIR)$(INS_MANUAL_EXT)..."
	@$(CP) $(DOCDIR)/$(EXE).$(MANEXT) $(INS_MANUAL_DIR)$(INS_MANUAL_EXT)/$(EXE).$(INS_MANUAL_EXT)

install:
	@$(ECHO) "Installing $(EXE) v$(VER) to $(INS_BINARY_DIR)..."
	@$(STRIP) $(BINDIR)/$(EXE)
	@-$(STRIP2) $(BINDIR)/$(EXE) > /dev/null 2>&1
	@-$(MV) $(BINDIR)/$(EXE) $(INS_BINARY_DIR)/$(EXE)
	@-$(RM) $(INS_BINARY_DIR)/r$(EXE)
	@$(LN) $(INS_BINARY_DIR)/$(EXE) $(INS_BINARY_DIR)/r$(EXE)
	@$(CHMOD) 755 $(INS_BINARY_DIR)/$(EXE)
	@$(LS) $(INS_BINARY_DIR)/$(EXE) $(INS_BINARY_DIR)/r$(EXE)
	@$(MAKE) install_manpage

install_setuid:
	@$(ECHO) "Installing SETUID $(EXE) v$(VER) to $(INS_BINARY_DIR)..."
	@$(STRIP) $(BINDIR)/$(EXE)
	@-$(STRIP2) $(BINDIR)/$(EXE) > /dev/null 2>&1 
	@-$(MV) $(BINDIR)/$(EXE) $(BINDIR)/$(EXE)
	@$(CHOWN) news $(BINDIR)/$(EXE) 
	@$(CHGRP) news $(BINDIR)/$(EXE) 
	@-$(RM) $(BINDIR)/r$(EXE)
	@$(LN) $(BINDIR)/$(EXE) $(BINDIR)/r$(EXE)
	@$(SU) news -c "chmod 6755 $(BINDIR)/$(EXE)"
	@$(LS) $(BINDIR)/$(EXE) $(BINDIR)/r$(EXE)
	@$(MAKE) install_manpage

install_daemon:
	@$(ECHO) "Installing index daemon $(EXED) v$(VER) to $(INS_DAEMON_DIR)..."
	@$(STRIP) $(EXED)
	@-$(STRIP2) $(EXED) > /dev/null 2>&1
	@-$(MV) $(EXED) $(INS_DAEMON_DIR)/$(EXED)
	@$(CHOWN) news $(INS_DAEMON_DIR)/$(EXED) 
	@$(CHMOD) 0750 $(INS_DAEMON_DIR)/$(EXED)
	@$(LS) $(INS_DAEMON_DIR)/$(EXED)

proto:
	@$(ECHO) "Generating function prototypes to $(INCDIR)/proto.h..."
	@-$(RM) -f $(INCDIR)/protoold.h $(INCDIR)/protonew.h
	@-$(CP) $(INCDIR)/proto.h $(INCDIR)/protoold.h
	@$(CPROTO) -e -m -I$(INCDIR) -I/linux/include $(CFILES) | sed -f proto.sed > $(INCDIR)/protonew.h
	@-$(CHMOD) 0644 $(INCDIR)/proto.h
	@-$(MV) $(INCDIR)/protonew.h $(INCDIR)/proto.h
	@$(LS) $(INCDIR)/proto.h

tags:
	@$(ECHO) "Generating tags (results in ./tags)..."
	@-$(RM) -f tags
	@$(TAGS) $(HFILES) $(CFILES) 

lint:
	@$(ECHO) "Linting source (results in ./LINT)..."
	@$(LINT) $(LINTFLAGS) -DNNTP_ABLE $(CFILES) > LINT

clean:
	@$(ECHO) "Cleaning..."
	@-$(RM) -f $(OFILES)
	@-$(RM) -f *~

clobber:
	@$(ECHO) "Clobbering..."
	@-$(RM) -f $(OFILES) $(BINDIR)/$(EXE) $(BINDIR)/$(EXED) tags 

cflow:
	@$(ECHO) "Creating cflow for $(PROJECT)..."
	@$(CFLOW) $(CFILES) > cflow.$(PROJECT) &

cscope:
	@$(ECHO) "Creating cscope database $(PROJECT)..."
	@$(CSCOPE) $(ALL_FILES)

$(OBJDIR)/active.o:		$(SRCDIR)/active.c $(HFILES)
$(OBJDIR)/actived.o:	$(SRCDIR)/actived.c $(HFILES)
$(OBJDIR)/amiga.o:		$(SRCDIR)/amiga.c $(HFILES)
$(OBJDIR)/art.o:		$(SRCDIR)/art.c $(HFILES)
$(OBJDIR)/attrib.o:		$(SRCDIR)/attrib.c $(HFILES)
$(OBJDIR)/charset.o:	$(SRCDIR)/charset.c $(HFILES)
$(OBJDIR)/config.o:		$(SRCDIR)/config.c $(HFILES)
$(OBJDIR)/curses.o:		$(SRCDIR)/curses.c $(HFILES)
$(OBJDIR)/debug.o:		$(SRCDIR)/debug.c $(HFILES)
$(OBJDIR)/envarg.o:		$(SRCDIR)/envarg.c $(HFILES)
$(OBJDIR)/feed.o:		$(SRCDIR)/feed.c $(HFILES)
$(OBJDIR)/filter.o:		$(SRCDIR)/filter.c $(HFILES)
$(OBJDIR)/getline.o:	$(SRCDIR)/getline.c $(HFILES)
$(OBJDIR)/group.o:		$(SRCDIR)/group.c $(HFILES)
$(OBJDIR)/hashstr.o:	$(SRCDIR)/hashstr.c $(HFILES)
$(OBJDIR)/help.o:		$(SRCDIR)/help.c $(HFILES)
$(OBJDIR)/inews.o:		$(SRCDIR)/inews.c $(HFILES)
$(OBJDIR)/init.o:		$(SRCDIR)/init.c $(HFILES)
$(OBJDIR)/lang.o:		$(SRCDIR)/lang.c $(HFILES)
$(OBJDIR)/list.o:		$(SRCDIR)/list.c $(HFILES)
$(OBJDIR)/mail.o:		$(SRCDIR)/mail.c $(HFILES)
$(OBJDIR)/main.o:		$(SRCDIR)/main.c $(HFILES)
$(OBJDIR)/memory.o:		$(SRCDIR)/memory.c $(HFILES)
$(OBJDIR)/msmail.o:		$(SRCDIR)/msmail.c $(HFILES)
$(OBJDIR)/misc.o:		$(SRCDIR)/misc.c $(HFILES)
$(OBJDIR)/newsrc.o:		$(SRCDIR)/newsrc.c $(HFILES)
$(OBJDIR)/nntplib.o:	$(SRCDIR)/nntplib.c $(HFILES)
$(OBJDIR)/nntpw32.o:	$(SRCDIR)/nntpw32.c $(HFILES)
$(OBJDIR)/open.o:		$(SRCDIR)/open.c $(HFILES)
$(OBJDIR)/os_2.o:		$(SRCDIR)/os_2.c $(HFILES)
$(OBJDIR)/page.o:		$(SRCDIR)/page.c $(HFILES)
$(OBJDIR)/parsdate.o:	$(SRCDIR)/parsdate.y $(HFILES)
$(OBJDIR)/pgp.o:		$(SRCDIR)/pgp.c $(HFILES)
$(OBJDIR)/post.o:		$(SRCDIR)/post.c $(HFILES)
$(OBJDIR)/prompt.o:		$(SRCDIR)/prompt.c $(HFILES)
$(OBJDIR)/save.o:		$(SRCDIR)/save.c $(HFILES)
$(OBJDIR)/screen.o:		$(SRCDIR)/screen.c $(HFILES)
$(OBJDIR)/search.o:		$(SRCDIR)/search.c $(HFILES)
$(OBJDIR)/select.o:		$(SRCDIR)/select.c $(HFILES)
$(OBJDIR)/sigfile.o:	$(SRCDIR)/sigfile.c $(HFILES)
$(OBJDIR)/signal.o:		$(SRCDIR)/signal.c $(HFILES)
$(OBJDIR)/strftime.o:	$(SRCDIR)/strftime.c $(HFILES)
$(OBJDIR)/thread.o:		$(SRCDIR)/thread.c $(HFILES)
$(OBJDIR)/wildmat.o:	$(SRCDIR)/wildmat.c $(HFILES)
$(OBJDIR)/win32.o:		$(SRCDIR)/win32.c
$(OBJDIR)/xref.o:		$(SRCDIR)/xref.c $(HFILES)
$(OBJDIR)/vms.o:		$(SRCDIR)/vms.c $(HFILES)
