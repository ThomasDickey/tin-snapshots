#
# MMS description file for PCRE routines of TIN
#

.IFDEF VAXC
CC = cc
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
OPTS            = /warning=(disable=implicitfunc) /prefix=all
.ENDIF
.ENDIF

# CFLAGS= $(DBG) $(OPTS) /include=([])
CFLAGS= $(DBG) $(OPTS)

MODULES = chartables, study, pcre

OBJECTS = $(MODULES:.obj)

LIB = pcrelib$(OLB)

all : $(LIB)($(MODULES))
    @ WRITE SYS$OUTPUT "Built $(LIB)"

clean :
    @ WRITE SYS$OUTPUT "Cleaning $(LIB)"
    @ if f$search("*.obj;*") .nes. "" then Delete/Log *.obj;*
    @ if f$search("*.olb;*") .nes. "" then Delete/Log *.olb;*
    @ if f$search("*.lis;*") .nes. "" then Delete/Log *.lis;*
    @ if f$search("chartables.c;*") .nes. "" then Delete/Log chartables.c;*
    @ if f$search("maketables.exe;*") .nes. "" then Delete/Log maketables.exe;*

.IFDEF DECC
.ELSE
.FIRST :
	define sys sys$library

.LAST :
	deassign sys
.ENDIF

pcre.obj :         pcre.c pcre.h internal.h
chartables.obj :   chartables.c
study.obj :        study.c pcre.h internal.h
maketables.obj :   maketables.c

# An auxiliary program makes the character tables

chartables.c :    maketables.exe
        CURRENT_OUTPUT = f$trnlnm("SYS$OUTPUT")
	DEFINE SYS$OUTPUT chartables.c
	run maketables
	DEASS SYS$OUTPUT
	DEFINE SYS$OUTPUT 'CURRENT_OUTPUT'           !nun wieder aufs Terminal

maketables.exe :     maketables.obj
        link maketables, [-.vms]tin.opt/option
