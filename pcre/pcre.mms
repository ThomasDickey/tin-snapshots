# Make file for PCRE (Perl-Compatible Regular Expression) library.
#
# Supported Compilers are GNU C, DEC C and DEC C++
#
# Start with MMS /DESCRIPTION=PCRE /MACRO=(Comp=1,ostyp=1)
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
        @ WRITE SYS$OUTPUT "   $MMS /DESCRIPTION=PCRE /MACRO=(ALPHA=1,GNUC=1)"
        @ WRITE SYS$OUTPUT "   $MMS /DESCRIPTION=PCRE /MACRO=(ALPHA=1,DECC=1)"
        @ WRITE SYS$OUTPUT "   $MMS /DESCRIPTION=PCRE /MACRO=(ALPHA=1,DECCXX=1)"
        @ WRITE SYS$OUTPUT ""
        @ WRITE SYS$OUTPUT "   on OpenVMS/Vax:"
        @ WRITE SYS$OUTPUT "   $MMS /DESCRIPTION=PCRE /MACRO=(VAX=1,GNUC=1)"
        @ WRITE SYS$OUTPUT "   $MMS /DESCRIPTION=PCRE /MACRO=(VAX=1,DECC=1)"
        @ WRITE SYS$OUTPUT "   $MMS /DESCRIPTION=PCRE /MACRO=(VAX=1,DECCXX=1)"
        @ WRITE SYS$OUTPUT ""
        @ return 44  ! %SYSTEM-F-ABORT, abort

.ENDIF

#
# DEC C Compiler
#
.IFDEF DECC
COMPFLAG = /PREFIX_LIBRARY_ENTRIES=ALL_ENTRIES
COMPILER = DEC C
COMP     = DECC
OPTFILE  =
.ELSE
#
# GNU C Compiler
#
.IFDEF GNUC
CC       = gcc
COMPFLAG = 
COMPILER = GNU C
COMP     = GNUC
OPTFILE  = PCRE.OPT
LOPT     = ,$(OPTFILE)/OPT
.ELSE
#
# DEC C++ Compiler
#
.IFDEF DECCXX
CC = cxx
COMPFLAG = /PREFIX_LIBRARY_ENTRIES=ALL_ENTRIES
COMPILER = DEC C++
COMP     = DECCXX
OPTFILE  = 
#
# No Compiler specified
#
.ELSE
COMPFLAG = /PREFIX_LIBRARY_ENTRIES=ALL_ENTRIES
COMPILER = DEC C
COMP     = DECC
OPTFILE  =
.ENDIF
.ENDIF
.ENDIF

.IFDEF DEBUGGER
PCRELIB    = LIBPCREDBG_$(COMP)_$(OS_TYP).OLB
PCREPOSLIB = LIBPCREPOSIXDBG_$(COMP)_$(OS_TYP).OLB
CFLAGS     = $(COMPFLAG)/NOOPTIMIZE/DEBUG
LFLAGS     = /DEBUG
.ELSE
PCRELIB    = LIBPCRE_$(COMP)_$(OS_TYP).OLB
PCREPOSLIB = LIBPCREPOSIX_$(COMP)_$(OS_TYP).OLB
CFLAGS     = $(COMPFLAG)/OPTIMIZE
.ENDIF

##########################################################################

MODPCRE     = maketables.obj get.obj study.obj pcre.obj
MODPCREPOS  = pcreposix.obj

all           : libpcre libpcreposix pcretest.exe pgrep.exe
    @ WRITE SYS$OUTPUT "Built  pcretest and pgrep"

libpcre       : $(PCRELIB)
    @ WRITE SYS$OUTPUT "Built $(MMS$TARGET)"

libpcreposix  : $(PCREPOSLIB)
    @ WRITE SYS$OUTPUT "Built $(MMS$TARGET)"

$(PCRELIB)    : checklibpcre $(PCRELIB)($(MODPCRE))
    @- continue

$(PCREPOSLIB) : checklibpcreposix $(PCREPOSLIB)($(MODPCREPOS))
    @- continue

checklibpcre      :
    @ If F$Search("$(PCRELIB)") .EQS. "" Then Library/Create $(PCRELIB)

checklibpcreposix :    
    @ If F$Search("$(PCREPOSLIB)") .EQS. "" Then Library/Create $(PCREPOSLIB)

pgrep.exe     :   pgrep.obj $(PCRELIB) $(OPTFILE)
		LINK pgrep.obj, $(PCRELIB)/lib  $(LOPT)

pcretest.exe  : pcretest.obj  $(PCRELIB) $(PCREPOSLIB) $(OPTFILE)
		LINK pcretest.obj, $(PCRELIB)/lib, $(PCREPOSLIB)/lib $(LOPT)


$(PCRELIB)(study)           :  study.c pcre.h internal.h
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        delete /nolog $(MMS$TARGET_NAME).OBJ;*

$(PCRELIB)(pcre)            :  pcre.c pcre.h chartables.c internal.h
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        delete /nolog $(MMS$TARGET_NAME).OBJ;*

$(PCRELIB)(chartables)      :  chartables.c
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        delete /nolog $(MMS$TARGET_NAME).OBJ;*

$(PCREPOSLIB)(pcreposix)    :  pcreposix.c pcreposix.h internal.h
        $(CC)$(CFLAGS) $(MMS$SOURCE)
        LIBR/REPLACE $(MMS$TARGET) $(MMS$TARGET_NAME)
        delete /nolog $(MMS$TARGET_NAME).OBJ;*

##########################################################################

PCRE.OPT      :
.IFDEF GNUC
    @ write sys$output "Linker Options File for $(COMPILER)"
    @ open/write optfile PCRE.OPT
.IFDEF VAX
    @ write optfile "!Linker Options File for $(COMPILER) on OpenVMS/VAX"
    @ write optfile "gnu_cc:[000000]gcclib/libr"
    @ write optfile "sys$share:vaxcrtl/share"
.ELSE
    @ write optfile "!Linker Options File for $(COMPILER) on OpenVMS/Alpha"
    @ write optfile "gnu_cc_library:libgcc/libr"
    @ write optfile "sys$share:vaxcrtl/libr"
    @ write optfile "gnu_cc_library:crt0.obj"
.ENDIF
    @ close optfile
.ENDIF



pcretest.obj   :  pcretest.c pcre.h
pgrep.obj      :  pgrep.c pcre.h

# An auxiliary program makes the character tables

chartables.obj :  chartables.c
maketables.obj :  maketables.c

chartables.c :    dftables.exe
        CURRENT_OUTPUT = f$trnlnm("SYS$OUTPUT")
	DEFINE SYS$OUTPUT chartables.c
	run dftables
	DEASS SYS$OUTPUT
	DEFINE SYS$OUTPUT 'CURRENT_OUTPUT'

dftables.exe :     dftables.obj maketables.obj $(OPTFILE)
        link dftables $(LOPT)

# We deliberately omit dftables and chartables.c from 'make clean'; once made
# chartables.c shouldn't change, and if people have edited the tables by hand,
# you don't want to throw them away.

clean :
    @ WRITE SYS$OUTPUT "Cleaning $(LIB)"
    @ if f$search("*.obj;*") .nes. "" then Delete/Log *.obj;*
    @ if f$search("*.lis;*") .nes. "" then Delete/Log *.lis;*
    @ if f$search("pcre.opt;*") .nes. "" then Delete/Log pcre.opt;*
    @ if f$search("pcretest.exe;*") .nes. "" then Delete/Log pcretest.exe;*
    @ if f$search("pgrep.exe;*") .nes. "" then Delete/Log pgrep.exe;*
    @ if f$search("$(PCRELIB);*") .nes. "" then Delete/Log $(PCRELIB);*
    @ if f$search("$(PCREPOSLIB);*") .nes. "" then Delete/Log $(PCREPOSLIB);*
    @ ! 
    @ if f$search("chartables.c;*") .nes. "" then Delete/Log chartables.c;*
    @ if f$search("dftables.exe;*") .nes. "" then Delete/Log dftables.exe;*

