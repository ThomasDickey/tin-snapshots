#
# DESCRIP.MMS for VMS routines of TIN
#

.IFDEF VAXC
CC = cc/vaxc
.ELSE
.IFDEF GNUC
CC = gcc
.ELSE
CC = cc/decc
.ENDIF
.ENDIF

.IFDEF DEBUG
DBG             = /debug /nooptim
.ELSE # not debug

.IFDEF VAXC
DBG             = /optim
.ELSE
.IFDEF GNUC
DBG             = /optim=2
.ELSE
DBG             = /optim
.ENDIF
.ENDIF

.ENDIF

.IFDEF VAXC
OPTS            =
.ELSE
.IFDEF GNUC
OPTS            =
.ELSE
OPTS            = /warning=(disable=implicitfunc)
.ENDIF
.ENDIF

CFLAGS= $(DBG) $(OPTS) /include=([])

MODULES = vmsdir, vmspwd, vmsfile, parse, getopt, getopt1, qsort, isxterm

OBJECTS = $(MODULES:.obj)

LIB = libvms$(OLB)

all : $(LIB)($(MODULES))
    WRITE SYS$OUTPUT "Built $(LIB)"

clean :
    delete/log *.obj;*,*.olb;*

.IFDEF DECC
.ELSE
.FIRST :
	define sys sys$library

.LAST :
	deassign sys
.ENDIF

#parse.c : parse.h
#vmsfile.c : parse.h
isxterm.obj : isxterm.c
    $(CC) $(CFLAGS) $(CFLAGS1) $?
