#
# DESCRIP.MMS for VMS routines of TIN
#

.IFDEF VAXC
CC = cc
CDEFS = /include=([-.include],[-.vms],[-.PCRE])/nowarning/list/show=expans
.ELSE
.IFDEF GNUC
CC = gcc
CDEFS = /include=([-.include],[-.vms],[-.PCRE])
.ELSE
CC = cc/decc
CDEFS = /include=([-.include],[-.vms],[-.PCRE])/nowarning/list/show=expans
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
CFLAGS= $(DBG) $(OPTS) $(CDEFS)


OBJECTS = $(MODULES:.obj)

LIB = libvms$(OLB)

all : $(LIB)($(MODULES))
    @ WRITE SYS$OUTPUT "Built $(LIB)"

clean :
    @ WRITE SYS$OUTPUT "Cleaning $(LIB)"
    @ if f$search("*.obj;*") .nes. "" then Delete/Log *.obj;*
    @ if f$search("*.olb;*") .nes. "" then Delete/Log *.olb;*
    @ if f$search("*.lis;*") .nes. "" then Delete/Log *.lis;*
    @ if f$search("tin.opt;*") .nes. "" then Purge/Log tin.opt

.IFDEF DECC
.FIRST :

.ELSE
.FIRST :
        define sys sys$library

.LAST :
        deassign sys
.ENDIF
