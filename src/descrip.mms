# Makefile for tin - for tin compiler flag options read INSTALL and README.
#
.IFDEF TCP
.ELSE
TCP = MULTINET
.ENDIF

.IFDEF VAXC
CC = cc/vaxc
OPTFILE = VAXC-$(TCP).OPT
.ELSE
.IFDEF  GNUC
CC	= gcc
OPTFILE = GCC-$(TCP).OPT

.ELSE
CC = cc/decc
OPTFILE = DECC-$(TCP).OPT
.ENDIF
.ENDIF

DEFINES = HAVE_ISPELL,DONT_HAVE_NNTP_EXTS,VMS,$(TCP),NNTP_DEFAULT_SERVER="""news.vuw.ac.nz""",OS="""VMS"""
CDEFS = /include=([-.include],[-.vms],[-.sio])/define=($(DEFINES))/nowarning

.IFDEF DEBUG

DBG = /debug /nooptim
LFLAGS = /debug/nomap
TARGET = tin_debug$(EXE)
TINLIB = TINDBG.OLB

.ELSE # !DEBUG

TARGET = tin$(EXE)
TINLIB = TIN.OLB

.IFDEF VAXC
DBG = /optim
.ELSE
.IFDEF GNUC
DBG = /optim=2
.ELSE
DBG = /optim 
.ENDIF
.ENDIF

LFLAGS = /nomap
.ENDIF # !DEBUG

.IFDEF VAXC
OPTS    =
.ELSE
.IFDEF GNUC
OPTS    =
.ELSE
OPTS    = /warning=(disable=implicitfunc)
.ENDIF
.ENDIF

CFLAGS = $(DBG) $(OPTS) $(CDEFS)

LIBS    = [-.vms]libvms/libr, [-.sio]libsio/libr
INCDIR  = [-.include]

.IFDEF GNUC
YACC = bison/fixed
YACCOUTPUT = y_tab.c
.ELSE
YACC = POSIX/Run/Path=POSIX "/bin/yacc"
YACCOUTPUT = ytab.c
.ENDIF

MODULES = active, actived, amiga, amigatcp, art, attrib, -
          charset, config, curses, debug, -
	      envarg, feed, filter, getline, group, hashstr, -
          help, inews, init, lang, list, mail, main, -
          memory, misc, msmail, newsrc, nntplib, nntpw32, -
          open, os_2, page, parsdate, pgp, post, prompt, -
          save, screen, search, select, sigfile, signal, -
          strftime, thread, wildmat, win32, win32tcp, -
          xref, vms

CFILES = $(MODULES:=.c)
OFILES = $(CFILES:.c=.obj)
#HFILES  = $(INCDIR)config.h, $(INCDIR)tin.h, $(INCDIR)extern.h, $(INCDIR)nntplib.h, \
#          $(INCDIR)proto.h, $(INCDIR)stpwatch.h, $(INCDIR)amiga.h, \
#          $(INCDIR)os_2.h, $(INCDIR)win32.h
HFILES   = $(INCDIR)tin.h

all : vmslib sio tin
    continue

tin : $(TINLIB)($(MODULES))
    link $(LFLAGS) /exec=$(TARGET) $(TINLIB)/LIBRARY/INCL=MAIN, -
        [-.vms]$(OPTFILE)/option, $(LIBS)

vmslib :
    set def [-.vms]
    mms $(MMSQUALIFIERS) $(MMSTARGETS)
    set def [-.src]

sio : 
    set def [-.sio]
    mms $(MMSQUALIFIERS) $(MMSTARGETS)
    set def [-.src]

active.obj :    active.c, $(HFILES)
actived.obj :   actived.c, $(HFILES)
art.obj :       art.c, $(HFILES), $(INCDIR)stpwatch.h
attrib.obj :    attrib.c, $(HFILES)
charset.obj :   charset.c, $(HFILES)
config.obj :    config.c, $(HFILES)
curses.obj :    curses.c, $(HFILES)
debug.obj :     debug.c, $(HFILES)
envarg.obj :    envarg.c, $(HFILES)
feed.obj :      feed.c, $(HFILES)
filter.obj :    filter.c, $(HFILES)
getline.obj :   getline.c, $(HFILES)
group.obj :     group.c, $(HFILES)
hashstr.obj :   hashstr.c, $(HFILES)
help.obj :      help.c, $(HFILES)
inews.obj :     inews.c, $(HFILES)
init.obj :      init.c, $(HFILES)
lang.obj :      lang.c, $(HFILES)
list.obj :      list.c, $(HFILES)
mail.obj :      mail.c, $(HFILES)
main.obj :      main.c, $(HFILES)
memory.obj :    memory.c, $(HFILES)
misc.obj :      misc.c, $(HFILES)
msmail.obj :    msmail.c, $(HFILES)
newsrc.obj :    newsrc.c, $(HFILES)
nntplib.obj :   nntplib.c, $(HFILES)
nntpw32.obj :   nntpw32.c, $(HFILES)
open.obj :      open.c, $(HFILES)
os_2.obj :      os_2.c, $(HFILES), $(INCDIR)os_2.h
page.obj :      page.c, $(HFILES)
parsdate.obj :  parsdate.c, $(HFILES)
post.obj :      post.c, $(HFILES)
prompt.obj :    prompt.c, $(HFILES)
save.obj :      save.c, $(HFILES)
screen.obj :    screen.c, $(HFILES)
search.obj :    search.c, $(HFILES)
select.obj :    select.c, $(HFILES)
sigfile.obj :   sigfile.c, $(HFILES)
signal.obj :    signal.c, $(HFILES)
strftime.obj :  strftime.c, $(HFILES)
thread.obj :    thread.c, $(HFILES)
wildmat.obj :   wildmat.c, $(HFILES)
win32.obj :     win32.c, $(HFILES)
xref.obj :      xref.c, $(HFILES)
vms.obj :       vms.c, $(HFILES)

parsdate.c : parsdate.y
    $(YACC) $?
    rename/log $(YACCOUTPUT) $@

c_vmslib :
    set def [.vms]
    mms $(MMSQUALIFIERS) clean
    set def [-]

c_sio : 
    set def [.sio]
    mms $(MMSQUALIFIERS) clean
    set def [-]

clean : #c_vmslib, c_sio
    Delete/Log *.obj;*
    Delete/log $(TARGET);*
    Delete/Log parsdate.c;*

.IFDEF VAXC

.IFDEF MULTINET
.FIRST :
    Define SYS 		Multinet_Common_Root:[Multinet.Include.Sys],Sys$Library
    Define VAXC$INCLUDE	Multinet_Common_Root:[Multinet.Include.Sys],Sys$Library
.ELSE
.FIRST :
    Define SYS 		Sys$Library
    Define VAXC$INCLUDE	Sys$Library
.ENDIF

.LAST :
    deassign sys
    deassing vaxc$include

.ELSE
.IFDEF GNUC

.FIRST :
   define sys sys$share

.LAST :
    deassign sys

.ENDIF
.ENDIF

