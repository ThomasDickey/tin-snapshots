#
# DESCRIP.MMS for VMS routines of TIN
#

# Start with MMS /DESCRIPTION=LIBVMS /MACRO=(Comp=1,ostyp=1)
# where Comp is one of GNUC, DECC or DECCXX and ostyp is ALPHA or VAX
#

.IFDEF ALPHA
OS_TYP = ALPHA
.ENDIF

.IFDEF VAX
OS_TYP = VAX
.ENDIF

.IFDEF OS_TYP
.ELSE
.FIRST
        @ WRITE SYS$OUTPUT "   Please start MMS (or MMK) at least with following switches:"
        @ WRITE SYS$OUTPUT ""
        @ WRITE SYS$OUTPUT "   on OpenVMS/Alpha:"
        @ WRITE SYS$OUTPUT "   $MMS /DESCRIPTION=LIBVMS /MACRO=(ALPHA=1,GNUC=1)"
        @ WRITE SYS$OUTPUT "   $MMS /DESCRIPTION=LIBVMS /MACRO=(ALPHA=1,DECC=1)"
        @ WRITE SYS$OUTPUT ""
        @ WRITE SYS$OUTPUT "   on OpenVMS/Vax:"
        @ WRITE SYS$OUTPUT "   $MMS /DESCRIPTION=LIBVMS /MACRO=(VAX=1,GNUC=1)"
        @ WRITE SYS$OUTPUT "   $MMS /DESCRIPTION=LIBVMS /MACRO=(VAX=1,DECC=1)"
        @ WRITE SYS$OUTPUT ""
        @ return 44  ! %SYSTEM-F-ABORT, abort

.ENDIF

#
# DEC C Compiler
#
.IFDEF DECC
COMPFLAG = /DECC/WARNING=(DISABLE=IMPLICITFUNC)/PREFIX_LIBRARY_ENTRIES=ALL_ENTRIES
INCLUDES = /INCLUDE=([-.INCLUDE],[-.VMS],[-.PCRE])
COMPILER = DEC C
COMP     = DECC
.ELSE
#
# GNU C Compiler
#
.IFDEF GNUC
CC       = gcc
COMPFLAG = 
INCLUDES = /INCLUDE=([-.INCLUDE],[-.VMS],[-.PCRE])
COMPILER = GNU C
COMP     = GNUC
.ELSE
#
# VAX C Compiler
#
.IFDEF VAXC
COMPFLAG = 
INCLUDES = /INCLUDE=([-.INCLUDE],[-.VMS],[-.PCRE])
COMPILER = VAX C
COMP     = VAXC
#
# No Compiler specified
#
.ELSE
COMPILER = none specified
COMP     = NONE
CC       = $!
.ENDIF
.ENDIF
.ENDIF

.IFDEF DEBUGGER
VMSLIB    = LIBVMSDBG_$(COMP)_$(OS_TYP).OLB
CFLAGS     = $(COMPFLAG)$(INCLUDES)/LIST/NOWARN/NOOPTIMIZE/DEBUG
LFLAGS     = /DEBUG
.ELSE
VMSLIB    = LIBVMS_$(COMP)_$(OS_TYP).OLB
CFLAGS     = $(COMPFLAG)$(INCLUDES)/NOWARNING/OPTIMIZE
.ENDIF

INCDIR  = [-.include]

############################################################################

OBJECTS = $(MODULES:.obj)


all : checklib $(VMSLIB)($(MODULES))
    @ WRITE SYS$OUTPUT "Built $(LIB)"


checklib :
    @ If F$Search("$(VMSLIB)") .EQS. "" Then Library/Create $(VMSLIB)


$(VMSLIB)(vms) :    vms.c $(INCDIR)tin.h
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        @- DELETE /NOLOG $(MMS$TARGET_NAME).OBJ;*

$(VMSLIB)(vmsdir) :    vmsdir.c strings.h ndir.h
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        @- DELETE /NOLOG $(MMS$TARGET_NAME).OBJ;*

$(VMSLIB)(vmspwd) :    vmspwd.c pwd.h
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        @- DELETE /NOLOG $(MMS$TARGET_NAME).OBJ;*

$(VMSLIB)(vmsfile) :    vmsfile.c parse.h
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        @- DELETE /NOLOG $(MMS$TARGET_NAME).OBJ;*

$(VMSLIB)(parse) :    parse.c 
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        @- DELETE /NOLOG $(MMS$TARGET_NAME).OBJ;*

$(VMSLIB)(parsdate) :    parsdate.c 
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        @- DELETE /NOLOG $(MMS$TARGET_NAME).OBJ;*

$(VMSLIB)(getopt) :    getopt.c $(INCDIR)config.h
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        @- DELETE /NOLOG $(MMS$TARGET_NAME).OBJ;*

$(VMSLIB)(getopt1) :    getopt1.c getopt.h $(INCDIR)config.h 
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        @- DELETE /NOLOG $(MMS$TARGET_NAME).OBJ;*

$(VMSLIB)(getpass) :    getpass.c 
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        @- DELETE /NOLOG $(MMS$TARGET_NAME).OBJ;*

$(VMSLIB)(qsort) :    qsort.c 
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        @- DELETE /NOLOG $(MMS$TARGET_NAME).OBJ;*

$(VMSLIB)(isxterm) :    isxterm.c
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        @- DELETE /NOLOG $(MMS$TARGET_NAME).OBJ;*




############################################################################

clean :
    @ WRITE SYS$OUTPUT "Cleaning $(LIB)"
    @ if f$search("*.obj;*") .nes. "" then Delete/Log *.obj;*
    @ if f$search("*.olb;*") .nes. "" then Delete/Log *.olb;*
    @ if f$search("*.lis;*") .nes. "" then Delete/Log *.lis;*
    @ if f$search("tin.opt;*") .nes. "" then Purge/Log tin.opt

.IFDEF DECC

.ELSE
.FIRST :
        define sys sys$library

.LAST :
        deassign sys
.ENDIF
