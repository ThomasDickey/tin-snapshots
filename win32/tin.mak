# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=XNS Debug
!MESSAGE No configuration specified.  Defaulting to XNS Debug.
!ENDIF 

!IF "$(CFG)" != "XNS Debug" && "$(CFG)" != "XNS Release" && "$(CFG)" !=\
 "TCP/IP Debug" && "$(CFG)" != "TCP/IP Release" && "$(CFG)" != "Pipes Debug" &&\
 "$(CFG)" != "Pipes Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "tin.mak" CFG="XNS Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XNS Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "XNS Release" (based on "Win32 (x86) Console Application")
!MESSAGE "TCP/IP Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "TCP/IP Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Pipes Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "Pipes Release" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Pipes Release"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XNS Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "XNS_Debu"
# PROP BASE Intermediate_Dir "XNS_Debu"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug.xns"
# PROP Intermediate_Dir "debug.xns"
OUTDIR=.\debug.xns
INTDIR=.\debug.xns

ALL : $(OUTDIR)/tin.exe $(OUTDIR)/tin.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /ML /W3 /GX /Zi /YX"tin.h" /Od /I "..\include" /D "WIN32XNS" /D "DEBUG" /D "_DEBUG" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D "NNTP_ABLE" /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D INEWS_MAIL_GATEWAY=\"\" /D "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING" /FR /c
CPP_PROJ=/nologo /ML /W3 /GX /Zi /YX"tin.h" /Od /I "..\include" /D "WIN32XNS"\
 /D "DEBUG" /D "_DEBUG" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D\
 "NNTP_ABLE" /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D\
 INEWS_MAIL_GATEWAY=\"\" /D "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"tin.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"tin.pdb" /c 
CPP_OBJS=.\debug.xns/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"tin.bsc" 
BSC32_SBRS= \
	$(INTDIR)/xref.sbr \
	$(INTDIR)/strftime.sbr \
	$(INTDIR)/group.sbr \
	$(INTDIR)/os_2.sbr \
	$(INTDIR)/list.sbr \
	$(INTDIR)/actived.sbr \
	$(INTDIR)/mail.sbr \
	$(INTDIR)/help.sbr \
	$(INTDIR)/filter.sbr \
	$(INTDIR)/sigfile.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/newsrc.sbr \
	$(INTDIR)/config.sbr \
	$(INTDIR)/prompt.sbr \
	.\debug.xns\nntpw32.sbr \
	$(INTDIR)/envarg.sbr \
	$(INTDIR)/hashstr.sbr \
	$(INTDIR)/charset.sbr \
	$(INTDIR)/active.sbr \
	$(INTDIR)/search.sbr \
	$(INTDIR)/page.sbr \
	$(INTDIR)/debug.sbr \
	$(INTDIR)/select.sbr \
	$(INTDIR)/main.sbr \
	$(INTDIR)/screen.sbr \
	$(INTDIR)/post.sbr \
	$(INTDIR)/getline.sbr \
	$(INTDIR)/attrib.sbr \
	$(INTDIR)/inews.sbr \
	$(INTDIR)/amiga.sbr \
	$(INTDIR)/amigatcp.sbr \
	$(INTDIR)/win32.sbr \
	$(INTDIR)/feed.sbr \
	$(INTDIR)/memory.sbr \
	$(INTDIR)/wildmat.sbr \
	$(INTDIR)/lang.sbr \
	$(INTDIR)/signal.sbr \
	$(INTDIR)/thread.sbr \
	$(INTDIR)/save.sbr \
	$(INTDIR)/open.sbr \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/art.sbr \
	$(INTDIR)/parsdate.sbr

$(OUTDIR)/tin.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib netapi32.lib wsock32.lib advapi32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /NODEFAULTLIB
LINK32_FLAGS=kernel32.lib user32.lib netapi32.lib wsock32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes /PDB:$(OUTDIR)/"tin.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"tin.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/xref.obj \
	$(INTDIR)/strftime.obj \
	$(INTDIR)/group.obj \
	$(INTDIR)/os_2.obj \
	$(INTDIR)/list.obj \
	$(INTDIR)/actived.obj \
	$(INTDIR)/mail.obj \
	$(INTDIR)/help.obj \
	$(INTDIR)/filter.obj \
	$(INTDIR)/sigfile.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/newsrc.obj \
	$(INTDIR)/config.obj \
	$(INTDIR)/prompt.obj \
	.\debug.xns\nntpw32.obj \
	$(INTDIR)/envarg.obj \
	$(INTDIR)/hashstr.obj \
	$(INTDIR)/charset.obj \
	$(INTDIR)/active.obj \
	$(INTDIR)/search.obj \
	$(INTDIR)/page.obj \
	$(INTDIR)/debug.obj \
	$(INTDIR)/select.obj \
	$(INTDIR)/main.obj \
	$(INTDIR)/screen.obj \
	$(INTDIR)/post.obj \
	$(INTDIR)/getline.obj \
	$(INTDIR)/attrib.obj \
	$(INTDIR)/inews.obj \
	$(INTDIR)/amiga.obj \
	$(INTDIR)/amigatcp.obj \
	$(INTDIR)/win32.obj \
	$(INTDIR)/feed.obj \
	$(INTDIR)/memory.obj \
	$(INTDIR)/wildmat.obj \
	$(INTDIR)/lang.obj \
	$(INTDIR)/signal.obj \
	$(INTDIR)/thread.obj \
	$(INTDIR)/save.obj \
	$(INTDIR)/open.obj \
	$(INTDIR)/misc.obj \
	$(INTDIR)/art.obj \
	$(INTDIR)/parsdate.obj

$(OUTDIR)/tin.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "XNS Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "XNS_Rele"
# PROP BASE Intermediate_Dir "XNS_Rele"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release.xns"
# PROP Intermediate_Dir "release.xns"
OUTDIR=.\release.xns
INTDIR=.\release.xns

ALL : $(OUTDIR)/tin.exe $(OUTDIR)/tin.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /ML /W3 /GX /Zi /YX"tin.h" /O2 /I "..\include" /D "NDEBUG" /D NNTP_INEWS_GATEWAY=\"microsoft.com\" /D "WIN32XNS" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D "NNTP_ABLE" /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D INEWS_MAIL_GATEWAY=\"\" /D "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING" /FR /c
CPP_PROJ=/nologo /ML /W3 /GX /Zi /YX"tin.h" /O2 /I "..\include" /D "NDEBUG" /D\
 NNTP_INEWS_GATEWAY=\"microsoft.com\" /D "WIN32XNS" /D "DONT_HAVE_TM_GMTOFF" /D\
 "WIN32" /D "_CONSOLE" /D "NNTP_ABLE" /D\
 INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D INEWS_MAIL_GATEWAY=\"\" /D\
 "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING" /FR$(INTDIR)/ /Fp$(OUTDIR)/"tin.pch"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"tin.pdb" /c 
CPP_OBJS=.\release.xns/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"tin.bsc" 
BSC32_SBRS= \
	$(INTDIR)/xref.sbr \
	$(INTDIR)/strftime.sbr \
	$(INTDIR)/group.sbr \
	$(INTDIR)/os_2.sbr \
	$(INTDIR)/list.sbr \
	$(INTDIR)/actived.sbr \
	$(INTDIR)/mail.sbr \
	$(INTDIR)/help.sbr \
	$(INTDIR)/filter.sbr \
	$(INTDIR)/sigfile.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/newsrc.sbr \
	$(INTDIR)/config.sbr \
	$(INTDIR)/prompt.sbr \
	.\release.xns\nntpw32.sbr \
	$(INTDIR)/envarg.sbr \
	$(INTDIR)/hashstr.sbr \
	$(INTDIR)/charset.sbr \
	$(INTDIR)/active.sbr \
	$(INTDIR)/search.sbr \
	$(INTDIR)/page.sbr \
	$(INTDIR)/debug.sbr \
	$(INTDIR)/select.sbr \
	$(INTDIR)/main.sbr \
	$(INTDIR)/screen.sbr \
	$(INTDIR)/post.sbr \
	$(INTDIR)/getline.sbr \
	$(INTDIR)/attrib.sbr \
	$(INTDIR)/inews.sbr \
	$(INTDIR)/amiga.sbr \
	$(INTDIR)/amigatcp.sbr \
	$(INTDIR)/win32.sbr \
	$(INTDIR)/feed.sbr \
	$(INTDIR)/memory.sbr \
	$(INTDIR)/wildmat.sbr \
	$(INTDIR)/lang.sbr \
	$(INTDIR)/signal.sbr \
	$(INTDIR)/thread.sbr \
	$(INTDIR)/save.sbr \
	$(INTDIR)/open.sbr \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/art.sbr \
	$(INTDIR)/parsdate.sbr

$(OUTDIR)/tin.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib netapi32.lib wsock32.lib advapi32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /NODEFAULTLIB
LINK32_FLAGS=kernel32.lib user32.lib netapi32.lib wsock32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes /PDB:$(OUTDIR)/"tin.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"tin.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/xref.obj \
	$(INTDIR)/strftime.obj \
	$(INTDIR)/group.obj \
	$(INTDIR)/os_2.obj \
	$(INTDIR)/list.obj \
	$(INTDIR)/actived.obj \
	$(INTDIR)/mail.obj \
	$(INTDIR)/help.obj \
	$(INTDIR)/filter.obj \
	$(INTDIR)/sigfile.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/newsrc.obj \
	$(INTDIR)/config.obj \
	$(INTDIR)/prompt.obj \
	.\release.xns\nntpw32.obj \
	$(INTDIR)/envarg.obj \
	$(INTDIR)/hashstr.obj \
	$(INTDIR)/charset.obj \
	$(INTDIR)/active.obj \
	$(INTDIR)/search.obj \
	$(INTDIR)/page.obj \
	$(INTDIR)/debug.obj \
	$(INTDIR)/select.obj \
	$(INTDIR)/main.obj \
	$(INTDIR)/screen.obj \
	$(INTDIR)/post.obj \
	$(INTDIR)/getline.obj \
	$(INTDIR)/attrib.obj \
	$(INTDIR)/inews.obj \
	$(INTDIR)/amiga.obj \
	$(INTDIR)/amigatcp.obj \
	$(INTDIR)/win32.obj \
	$(INTDIR)/feed.obj \
	$(INTDIR)/memory.obj \
	$(INTDIR)/wildmat.obj \
	$(INTDIR)/lang.obj \
	$(INTDIR)/signal.obj \
	$(INTDIR)/thread.obj \
	$(INTDIR)/save.obj \
	$(INTDIR)/open.obj \
	$(INTDIR)/misc.obj \
	$(INTDIR)/art.obj \
	$(INTDIR)/parsdate.obj

$(OUTDIR)/tin.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TCP_IP_D"
# PROP BASE Intermediate_Dir "TCP_IP_D"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug.tcp"
# PROP Intermediate_Dir "debug.tcp"
OUTDIR=.\debug.tcp
INTDIR=.\debug.tcp

ALL : $(OUTDIR)/tin.exe $(OUTDIR)/tin.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /ML /W3 /GX /Zi /YX"tin.h" /Od /I "..\include" /D "WIN32IP" /D "DEBUG" /D "_DEBUG" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D "NNTP_ABLE" /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D INEWS_MAIL_GATEWAY=\"\" /D "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING" /FR /c
CPP_PROJ=/nologo /ML /W3 /GX /Zi /YX"tin.h" /Od /I "..\include" /D "WIN32IP" /D\
 "DEBUG" /D "_DEBUG" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D\
 "NNTP_ABLE" /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D\
 INEWS_MAIL_GATEWAY=\"\" /D "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"tin.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"tin.pdb" /c 
CPP_OBJS=.\debug.tcp/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"tin.bsc" 
BSC32_SBRS= \
	$(INTDIR)/xref.sbr \
	$(INTDIR)/strftime.sbr \
	$(INTDIR)/group.sbr \
	$(INTDIR)/os_2.sbr \
	$(INTDIR)/list.sbr \
	$(INTDIR)/actived.sbr \
	$(INTDIR)/mail.sbr \
	$(INTDIR)/help.sbr \
	$(INTDIR)/filter.sbr \
	$(INTDIR)/sigfile.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/newsrc.sbr \
	$(INTDIR)/config.sbr \
	$(INTDIR)/prompt.sbr \
	.\debug.tcp\nntpw32.sbr \
	$(INTDIR)/envarg.sbr \
	$(INTDIR)/hashstr.sbr \
	$(INTDIR)/charset.sbr \
	$(INTDIR)/active.sbr \
	$(INTDIR)/search.sbr \
	$(INTDIR)/page.sbr \
	$(INTDIR)/debug.sbr \
	$(INTDIR)/select.sbr \
	$(INTDIR)/main.sbr \
	$(INTDIR)/screen.sbr \
	$(INTDIR)/post.sbr \
	$(INTDIR)/getline.sbr \
	$(INTDIR)/attrib.sbr \
	$(INTDIR)/inews.sbr \
	$(INTDIR)/amiga.sbr \
	$(INTDIR)/amigatcp.sbr \
	$(INTDIR)/win32.sbr \
	$(INTDIR)/feed.sbr \
	$(INTDIR)/memory.sbr \
	$(INTDIR)/wildmat.sbr \
	$(INTDIR)/lang.sbr \
	$(INTDIR)/signal.sbr \
	$(INTDIR)/thread.sbr \
	$(INTDIR)/save.sbr \
	$(INTDIR)/open.sbr \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/art.sbr \
	$(INTDIR)/parsdate.sbr

$(OUTDIR)/tin.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib netapi32.lib wsock32.lib advapi32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /NODEFAULTLIB
LINK32_FLAGS=kernel32.lib user32.lib netapi32.lib wsock32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes /PDB:$(OUTDIR)/"tin.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"tin.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/xref.obj \
	$(INTDIR)/strftime.obj \
	$(INTDIR)/group.obj \
	$(INTDIR)/os_2.obj \
	$(INTDIR)/list.obj \
	$(INTDIR)/actived.obj \
	$(INTDIR)/mail.obj \
	$(INTDIR)/help.obj \
	$(INTDIR)/filter.obj \
	$(INTDIR)/sigfile.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/newsrc.obj \
	$(INTDIR)/config.obj \
	$(INTDIR)/prompt.obj \
	.\debug.tcp\nntpw32.obj \
	$(INTDIR)/envarg.obj \
	$(INTDIR)/hashstr.obj \
	$(INTDIR)/charset.obj \
	$(INTDIR)/active.obj \
	$(INTDIR)/search.obj \
	$(INTDIR)/page.obj \
	$(INTDIR)/debug.obj \
	$(INTDIR)/select.obj \
	$(INTDIR)/main.obj \
	$(INTDIR)/screen.obj \
	$(INTDIR)/post.obj \
	$(INTDIR)/getline.obj \
	$(INTDIR)/attrib.obj \
	$(INTDIR)/inews.obj \
	$(INTDIR)/amiga.obj \
	$(INTDIR)/amigatcp.obj \
	$(INTDIR)/win32.obj \
	$(INTDIR)/feed.obj \
	$(INTDIR)/memory.obj \
	$(INTDIR)/wildmat.obj \
	$(INTDIR)/lang.obj \
	$(INTDIR)/signal.obj \
	$(INTDIR)/thread.obj \
	$(INTDIR)/save.obj \
	$(INTDIR)/open.obj \
	$(INTDIR)/misc.obj \
	$(INTDIR)/art.obj \
	$(INTDIR)/parsdate.obj

$(OUTDIR)/tin.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TCP/IP Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TCP_IP_R"
# PROP BASE Intermediate_Dir "TCP_IP_R"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release.tcp"
# PROP Intermediate_Dir "release.tcp"
OUTDIR=.\release.tcp
INTDIR=.\release.tcp

ALL : $(OUTDIR)/tin.exe $(OUTDIR)/tin.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /ML /W3 /GX /Zi /YX"tin.h" /O2 /I "..\include" /D "NDEBUG" /D "WIN32IP" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D "NNTP_ABLE" /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D INEWS_MAIL_GATEWAY=\"\" /D "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING" /FR /c
CPP_PROJ=/nologo /ML /W3 /GX /Zi /YX"tin.h" /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32IP" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D "NNTP_ABLE" /D\
 INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D INEWS_MAIL_GATEWAY=\"\" /D\
 "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING" /FR$(INTDIR)/ /Fp$(OUTDIR)/"tin.pch"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"tin.pdb" /c 
CPP_OBJS=.\release.tcp/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"tin.bsc" 
BSC32_SBRS= \
	$(INTDIR)/xref.sbr \
	$(INTDIR)/strftime.sbr \
	$(INTDIR)/group.sbr \
	$(INTDIR)/os_2.sbr \
	$(INTDIR)/list.sbr \
	$(INTDIR)/actived.sbr \
	$(INTDIR)/mail.sbr \
	$(INTDIR)/help.sbr \
	$(INTDIR)/filter.sbr \
	$(INTDIR)/sigfile.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/newsrc.sbr \
	$(INTDIR)/config.sbr \
	$(INTDIR)/prompt.sbr \
	.\release.tcp\nntpw32.sbr \
	$(INTDIR)/envarg.sbr \
	$(INTDIR)/hashstr.sbr \
	$(INTDIR)/charset.sbr \
	$(INTDIR)/active.sbr \
	$(INTDIR)/search.sbr \
	$(INTDIR)/page.sbr \
	$(INTDIR)/debug.sbr \
	$(INTDIR)/select.sbr \
	$(INTDIR)/main.sbr \
	$(INTDIR)/screen.sbr \
	$(INTDIR)/post.sbr \
	$(INTDIR)/getline.sbr \
	$(INTDIR)/attrib.sbr \
	$(INTDIR)/inews.sbr \
	$(INTDIR)/amiga.sbr \
	$(INTDIR)/amigatcp.sbr \
	$(INTDIR)/win32.sbr \
	$(INTDIR)/feed.sbr \
	$(INTDIR)/memory.sbr \
	$(INTDIR)/wildmat.sbr \
	$(INTDIR)/lang.sbr \
	$(INTDIR)/signal.sbr \
	$(INTDIR)/thread.sbr \
	$(INTDIR)/save.sbr \
	$(INTDIR)/open.sbr \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/art.sbr \
	$(INTDIR)/parsdate.sbr

$(OUTDIR)/tin.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib netapi32.lib wsock32.lib advapi32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /NODEFAULTLIB
LINK32_FLAGS=kernel32.lib user32.lib netapi32.lib wsock32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes /PDB:$(OUTDIR)/"tin.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"tin.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/xref.obj \
	$(INTDIR)/strftime.obj \
	$(INTDIR)/group.obj \
	$(INTDIR)/os_2.obj \
	$(INTDIR)/list.obj \
	$(INTDIR)/actived.obj \
	$(INTDIR)/mail.obj \
	$(INTDIR)/help.obj \
	$(INTDIR)/filter.obj \
	$(INTDIR)/sigfile.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/newsrc.obj \
	$(INTDIR)/config.obj \
	$(INTDIR)/prompt.obj \
	.\release.tcp\nntpw32.obj \
	$(INTDIR)/envarg.obj \
	$(INTDIR)/hashstr.obj \
	$(INTDIR)/charset.obj \
	$(INTDIR)/active.obj \
	$(INTDIR)/search.obj \
	$(INTDIR)/page.obj \
	$(INTDIR)/debug.obj \
	$(INTDIR)/select.obj \
	$(INTDIR)/main.obj \
	$(INTDIR)/screen.obj \
	$(INTDIR)/post.obj \
	$(INTDIR)/getline.obj \
	$(INTDIR)/attrib.obj \
	$(INTDIR)/inews.obj \
	$(INTDIR)/amiga.obj \
	$(INTDIR)/amigatcp.obj \
	$(INTDIR)/win32.obj \
	$(INTDIR)/feed.obj \
	$(INTDIR)/memory.obj \
	$(INTDIR)/wildmat.obj \
	$(INTDIR)/lang.obj \
	$(INTDIR)/signal.obj \
	$(INTDIR)/thread.obj \
	$(INTDIR)/save.obj \
	$(INTDIR)/open.obj \
	$(INTDIR)/misc.obj \
	$(INTDIR)/art.obj \
	$(INTDIR)/parsdate.obj

$(OUTDIR)/tin.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Pipes Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Pipes_De"
# PROP BASE Intermediate_Dir "Pipes_De"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug.pip"
# PROP Intermediate_Dir "debug.pip"
OUTDIR=.\debug.pip
INTDIR=.\debug.pip

ALL : $(OUTDIR)/tin.exe $(OUTDIR)/tin.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /I "..\include" /D "_DEBUG" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D "NNTP_ABLE" /D NNTP_INEWS_DOMAIN=\"$NNTP_INEWS_DOMAIN\" /D "WIN32IP" /FR /c
# ADD CPP /nologo /ML /W3 /GX /Zi /YX"tin.h" /Od /I "..\include" /D "WIN32PIPES" /D "DEBUG" /D "_DEBUG" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D "NNTP_ABLE" /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D INEWS_MAIL_GATEWAY=\"\" /D "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING" /FR /c
CPP_PROJ=/nologo /ML /W3 /GX /Zi /YX"tin.h" /Od /I "..\include" /D "WIN32PIPES"\
 /D "DEBUG" /D "_DEBUG" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D\
 "NNTP_ABLE" /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D\
 INEWS_MAIL_GATEWAY=\"\" /D "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"tin.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"tin.pdb" /c 
CPP_OBJS=.\debug.pip/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"tin.bsc" 
BSC32_SBRS= \
	$(INTDIR)/xref.sbr \
	$(INTDIR)/strftime.sbr \
	$(INTDIR)/group.sbr \
	$(INTDIR)/os_2.sbr \
	$(INTDIR)/list.sbr \
	$(INTDIR)/actived.sbr \
	$(INTDIR)/mail.sbr \
	$(INTDIR)/help.sbr \
	$(INTDIR)/filter.sbr \
	$(INTDIR)/sigfile.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/newsrc.sbr \
	$(INTDIR)/config.sbr \
	$(INTDIR)/prompt.sbr \
	.\debug.pip\nntpw32.sbr \
	$(INTDIR)/envarg.sbr \
	$(INTDIR)/hashstr.sbr \
	$(INTDIR)/charset.sbr \
	$(INTDIR)/active.sbr \
	$(INTDIR)/search.sbr \
	$(INTDIR)/page.sbr \
	$(INTDIR)/debug.sbr \
	$(INTDIR)/select.sbr \
	$(INTDIR)/main.sbr \
	$(INTDIR)/screen.sbr \
	$(INTDIR)/post.sbr \
	$(INTDIR)/getline.sbr \
	$(INTDIR)/attrib.sbr \
	$(INTDIR)/inews.sbr \
	$(INTDIR)/amiga.sbr \
	$(INTDIR)/amigatcp.sbr \
	$(INTDIR)/win32.sbr \
	$(INTDIR)/feed.sbr \
	$(INTDIR)/memory.sbr \
	$(INTDIR)/wildmat.sbr \
	$(INTDIR)/lang.sbr \
	$(INTDIR)/signal.sbr \
	$(INTDIR)/thread.sbr \
	$(INTDIR)/save.sbr \
	$(INTDIR)/open.sbr \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/art.sbr \
	$(INTDIR)/parsdate.sbr

$(OUTDIR)/tin.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib netapi32.lib wsock32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib netapi32.lib wsock32.lib advapi32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /NODEFAULTLIB
LINK32_FLAGS=kernel32.lib user32.lib netapi32.lib wsock32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes /PDB:$(OUTDIR)/"tin.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"tin.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/xref.obj \
	$(INTDIR)/strftime.obj \
	$(INTDIR)/group.obj \
	$(INTDIR)/os_2.obj \
	$(INTDIR)/list.obj \
	$(INTDIR)/actived.obj \
	$(INTDIR)/mail.obj \
	$(INTDIR)/help.obj \
	$(INTDIR)/filter.obj \
	$(INTDIR)/sigfile.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/newsrc.obj \
	$(INTDIR)/config.obj \
	$(INTDIR)/prompt.obj \
	.\debug.pip\nntpw32.obj \
	$(INTDIR)/envarg.obj \
	$(INTDIR)/hashstr.obj \
	$(INTDIR)/charset.obj \
	$(INTDIR)/active.obj \
	$(INTDIR)/search.obj \
	$(INTDIR)/page.obj \
	$(INTDIR)/debug.obj \
	$(INTDIR)/select.obj \
	$(INTDIR)/main.obj \
	$(INTDIR)/screen.obj \
	$(INTDIR)/post.obj \
	$(INTDIR)/getline.obj \
	$(INTDIR)/attrib.obj \
	$(INTDIR)/inews.obj \
	$(INTDIR)/amiga.obj \
	$(INTDIR)/amigatcp.obj \
	$(INTDIR)/win32.obj \
	$(INTDIR)/feed.obj \
	$(INTDIR)/memory.obj \
	$(INTDIR)/wildmat.obj \
	$(INTDIR)/lang.obj \
	$(INTDIR)/signal.obj \
	$(INTDIR)/thread.obj \
	$(INTDIR)/save.obj \
	$(INTDIR)/open.obj \
	$(INTDIR)/misc.obj \
	$(INTDIR)/art.obj \
	$(INTDIR)/parsdate.obj

$(OUTDIR)/tin.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Pipes Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Pipes_Re"
# PROP BASE Intermediate_Dir "Pipes_Re"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release.pip"
# PROP Intermediate_Dir "release.pip"
OUTDIR=.\release.pip
INTDIR=.\release.pip

ALL : $(OUTDIR)/tin.exe $(OUTDIR)/tin.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /I "..\include" /D "NDEBUG" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D "NNTP_ABLE" /D NNTP_INEWS_DOMAIN=\"$NNTP_INEWS_DOMAIN\" /D "WIN32IP" /FR /c
# ADD CPP /nologo /ML /W3 /GX /Zi /YX"tin.h" /O2 /I "..\include" /D "NDEBUG" /D "WIN32PIPES" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D "NNTP_ABLE" /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D INEWS_MAIL_GATEWAY=\"\" /D "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING" /FR /c
CPP_PROJ=/nologo /ML /W3 /GX /Zi /YX"tin.h" /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32PIPES" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D "NNTP_ABLE"\
 /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D INEWS_MAIL_GATEWAY=\"\" /D\
 "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING" /FR$(INTDIR)/ /Fp$(OUTDIR)/"tin.pch"\
 /Fo$(INTDIR)/ /Fd$(OUTDIR)/"tin.pdb" /c 
CPP_OBJS=.\release.pip/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"tin.bsc" 
BSC32_SBRS= \
	$(INTDIR)/xref.sbr \
	$(INTDIR)/strftime.sbr \
	$(INTDIR)/group.sbr \
	$(INTDIR)/os_2.sbr \
	$(INTDIR)/list.sbr \
	$(INTDIR)/actived.sbr \
	$(INTDIR)/mail.sbr \
	$(INTDIR)/help.sbr \
	$(INTDIR)/filter.sbr \
	$(INTDIR)/sigfile.sbr \
	$(INTDIR)/init.sbr \
	$(INTDIR)/newsrc.sbr \
	$(INTDIR)/config.sbr \
	$(INTDIR)/prompt.sbr \
	.\release.pip\nntpw32.sbr \
	$(INTDIR)/envarg.sbr \
	$(INTDIR)/hashstr.sbr \
	$(INTDIR)/charset.sbr \
	$(INTDIR)/active.sbr \
	$(INTDIR)/search.sbr \
	$(INTDIR)/page.sbr \
	$(INTDIR)/debug.sbr \
	$(INTDIR)/select.sbr \
	$(INTDIR)/main.sbr \
	$(INTDIR)/screen.sbr \
	$(INTDIR)/post.sbr \
	$(INTDIR)/getline.sbr \
	$(INTDIR)/attrib.sbr \
	$(INTDIR)/inews.sbr \
	$(INTDIR)/amiga.sbr \
	$(INTDIR)/amigatcp.sbr \
	$(INTDIR)/win32.sbr \
	$(INTDIR)/feed.sbr \
	$(INTDIR)/memory.sbr \
	$(INTDIR)/wildmat.sbr \
	$(INTDIR)/lang.sbr \
	$(INTDIR)/signal.sbr \
	$(INTDIR)/thread.sbr \
	$(INTDIR)/save.sbr \
	$(INTDIR)/open.sbr \
	$(INTDIR)/misc.sbr \
	$(INTDIR)/art.sbr \
	$(INTDIR)/parsdate.sbr

$(OUTDIR)/tin.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib netapi32.lib wsock32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib netapi32.lib wsock32.lib advapi32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /NODEFAULTLIB
LINK32_FLAGS=kernel32.lib user32.lib netapi32.lib wsock32.lib advapi32.lib\
 /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes /PDB:$(OUTDIR)/"tin.pdb" /DEBUG\
 /MACHINE:I386 /OUT:$(OUTDIR)/"tin.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/xref.obj \
	$(INTDIR)/strftime.obj \
	$(INTDIR)/group.obj \
	$(INTDIR)/os_2.obj \
	$(INTDIR)/list.obj \
	$(INTDIR)/actived.obj \
	$(INTDIR)/mail.obj \
	$(INTDIR)/help.obj \
	$(INTDIR)/filter.obj \
	$(INTDIR)/sigfile.obj \
	$(INTDIR)/init.obj \
	$(INTDIR)/newsrc.obj \
	$(INTDIR)/config.obj \
	$(INTDIR)/prompt.obj \
	.\release.pip\nntpw32.obj \
	$(INTDIR)/envarg.obj \
	$(INTDIR)/hashstr.obj \
	$(INTDIR)/charset.obj \
	$(INTDIR)/active.obj \
	$(INTDIR)/search.obj \
	$(INTDIR)/page.obj \
	$(INTDIR)/debug.obj \
	$(INTDIR)/select.obj \
	$(INTDIR)/main.obj \
	$(INTDIR)/screen.obj \
	$(INTDIR)/post.obj \
	$(INTDIR)/getline.obj \
	$(INTDIR)/attrib.obj \
	$(INTDIR)/inews.obj \
	$(INTDIR)/amiga.obj \
	$(INTDIR)/amigatcp.obj \
	$(INTDIR)/win32.obj \
	$(INTDIR)/feed.obj \
	$(INTDIR)/memory.obj \
	$(INTDIR)/wildmat.obj \
	$(INTDIR)/lang.obj \
	$(INTDIR)/signal.obj \
	$(INTDIR)/thread.obj \
	$(INTDIR)/save.obj \
	$(INTDIR)/open.obj \
	$(INTDIR)/misc.obj \
	$(INTDIR)/art.obj \
	$(INTDIR)/parsdate.obj

$(OUTDIR)/tin.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=src\xref.c
DEP_XREF_=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/xref.obj :  $(SOURCE)  $(DEP_XREF_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/xref.obj :  $(SOURCE)  $(DEP_XREF_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/xref.obj :  $(SOURCE)  $(DEP_XREF_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/xref.obj :  $(SOURCE)  $(DEP_XREF_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/xref.obj :  $(SOURCE)  $(DEP_XREF_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/xref.obj :  $(SOURCE)  $(DEP_XREF_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\strftime.c
DEP_STRFT=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/strftime.obj :  $(SOURCE)  $(DEP_STRFT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/strftime.obj :  $(SOURCE)  $(DEP_STRFT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/strftime.obj :  $(SOURCE)  $(DEP_STRFT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/strftime.obj :  $(SOURCE)  $(DEP_STRFT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/strftime.obj :  $(SOURCE)  $(DEP_STRFT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/strftime.obj :  $(SOURCE)  $(DEP_STRFT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\group.c
DEP_GROUP=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/group.obj :  $(SOURCE)  $(DEP_GROUP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/group.obj :  $(SOURCE)  $(DEP_GROUP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/group.obj :  $(SOURCE)  $(DEP_GROUP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/group.obj :  $(SOURCE)  $(DEP_GROUP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/group.obj :  $(SOURCE)  $(DEP_GROUP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/group.obj :  $(SOURCE)  $(DEP_GROUP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\os_2.c
DEP_OS_2_=\
	include\tin.h\
	include\os_2.h\
	include\config.h\
	include\amiga.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/os_2.obj :  $(SOURCE)  $(DEP_OS_2_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/os_2.obj :  $(SOURCE)  $(DEP_OS_2_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/os_2.obj :  $(SOURCE)  $(DEP_OS_2_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/os_2.obj :  $(SOURCE)  $(DEP_OS_2_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/os_2.obj :  $(SOURCE)  $(DEP_OS_2_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/os_2.obj :  $(SOURCE)  $(DEP_OS_2_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\list.c
DEP_LIST_=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/list.obj :  $(SOURCE)  $(DEP_LIST_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/list.obj :  $(SOURCE)  $(DEP_LIST_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/list.obj :  $(SOURCE)  $(DEP_LIST_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/list.obj :  $(SOURCE)  $(DEP_LIST_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/list.obj :  $(SOURCE)  $(DEP_LIST_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/list.obj :  $(SOURCE)  $(DEP_LIST_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\actived.c
DEP_ACTIV=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/actived.obj :  $(SOURCE)  $(DEP_ACTIV) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/actived.obj :  $(SOURCE)  $(DEP_ACTIV) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/actived.obj :  $(SOURCE)  $(DEP_ACTIV) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/actived.obj :  $(SOURCE)  $(DEP_ACTIV) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/actived.obj :  $(SOURCE)  $(DEP_ACTIV) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/actived.obj :  $(SOURCE)  $(DEP_ACTIV) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\mail.c
DEP_MAIL_=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/mail.obj :  $(SOURCE)  $(DEP_MAIL_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/mail.obj :  $(SOURCE)  $(DEP_MAIL_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/mail.obj :  $(SOURCE)  $(DEP_MAIL_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/mail.obj :  $(SOURCE)  $(DEP_MAIL_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/mail.obj :  $(SOURCE)  $(DEP_MAIL_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/mail.obj :  $(SOURCE)  $(DEP_MAIL_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\help.c
DEP_HELP_=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/help.obj :  $(SOURCE)  $(DEP_HELP_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/help.obj :  $(SOURCE)  $(DEP_HELP_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/help.obj :  $(SOURCE)  $(DEP_HELP_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/help.obj :  $(SOURCE)  $(DEP_HELP_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/help.obj :  $(SOURCE)  $(DEP_HELP_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/help.obj :  $(SOURCE)  $(DEP_HELP_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\filter.c
DEP_FILTE=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/filter.obj :  $(SOURCE)  $(DEP_FILTE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/filter.obj :  $(SOURCE)  $(DEP_FILTE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/filter.obj :  $(SOURCE)  $(DEP_FILTE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/filter.obj :  $(SOURCE)  $(DEP_FILTE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/filter.obj :  $(SOURCE)  $(DEP_FILTE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/filter.obj :  $(SOURCE)  $(DEP_FILTE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\sigfile.c
DEP_SIGFI=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/sigfile.obj :  $(SOURCE)  $(DEP_SIGFI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/sigfile.obj :  $(SOURCE)  $(DEP_SIGFI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/sigfile.obj :  $(SOURCE)  $(DEP_SIGFI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/sigfile.obj :  $(SOURCE)  $(DEP_SIGFI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/sigfile.obj :  $(SOURCE)  $(DEP_SIGFI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/sigfile.obj :  $(SOURCE)  $(DEP_SIGFI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\init.c
DEP_INIT_=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/init.obj :  $(SOURCE)  $(DEP_INIT_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\newsrc.c
DEP_NEWSR=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/newsrc.obj :  $(SOURCE)  $(DEP_NEWSR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/newsrc.obj :  $(SOURCE)  $(DEP_NEWSR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/newsrc.obj :  $(SOURCE)  $(DEP_NEWSR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/newsrc.obj :  $(SOURCE)  $(DEP_NEWSR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/newsrc.obj :  $(SOURCE)  $(DEP_NEWSR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/newsrc.obj :  $(SOURCE)  $(DEP_NEWSR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\config.c
DEP_CONFI=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/config.obj :  $(SOURCE)  $(DEP_CONFI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/config.obj :  $(SOURCE)  $(DEP_CONFI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/config.obj :  $(SOURCE)  $(DEP_CONFI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/config.obj :  $(SOURCE)  $(DEP_CONFI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/config.obj :  $(SOURCE)  $(DEP_CONFI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/config.obj :  $(SOURCE)  $(DEP_CONFI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\prompt.c
DEP_PROMP=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/prompt.obj :  $(SOURCE)  $(DEP_PROMP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/prompt.obj :  $(SOURCE)  $(DEP_PROMP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/prompt.obj :  $(SOURCE)  $(DEP_PROMP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/prompt.obj :  $(SOURCE)  $(DEP_PROMP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/prompt.obj :  $(SOURCE)  $(DEP_PROMP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/prompt.obj :  $(SOURCE)  $(DEP_PROMP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\nntpw32.c
DEP_NNTPW=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

# PROP Intermediate_Dir "debug.xns"
INTDIR_SRC=.\debug.xns

.\debug.xns\nntpw32.obj :  $(SOURCE)  $(DEP_NNTPW) $(INTDIR_SRC)
   $(CPP) /nologo /ML /W3 /GX /Zi /YX"tin.h" /Od /I "..\include" /D "WIN32XNS"\
 /D "DEBUG" /D "_DEBUG" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D\
 "NNTP_ABLE" /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D\
 INEWS_MAIL_GATEWAY=\"\" /D "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING"\
 /FR"debug.xns/" /Fp$(OUTDIR)/"tin.pch" /Fo"debug.xns/" /Fd$(OUTDIR)/"tin.pdb"\
 /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

# PROP Intermediate_Dir "release.xns"
INTDIR_SRC=.\release.xns

.\release.xns\nntpw32.obj :  $(SOURCE)  $(DEP_NNTPW) $(INTDIR_SRC)
   $(CPP) /nologo /ML /W3 /GX /Zi /YX"tin.h" /O2 /I "..\include" /D "NDEBUG" /D\
 NNTP_INEWS_GATEWAY=\"microsoft.com\" /D "WIN32XNS" /D "DONT_HAVE_TM_GMTOFF" /D\
 "WIN32" /D "_CONSOLE" /D "NNTP_ABLE" /D\
 INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D INEWS_MAIL_GATEWAY=\"\" /D\
 "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING" /FR"release.xns/"\
 /Fp$(OUTDIR)/"tin.pch" /Fo"release.xns/" /Fd$(OUTDIR)/"tin.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

# PROP Intermediate_Dir "debug.tcp"
INTDIR_SRC=.\debug.tcp

.\debug.tcp\nntpw32.obj :  $(SOURCE)  $(DEP_NNTPW) $(INTDIR_SRC)
   $(CPP) /nologo /ML /W3 /GX /Zi /YX"tin.h" /Od /I "..\include" /D "WIN32IP"\
 /D "DEBUG" /D "_DEBUG" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D\
 "NNTP_ABLE" /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D\
 INEWS_MAIL_GATEWAY=\"\" /D "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING"\
 /FR"debug.tcp/" /Fp$(OUTDIR)/"tin.pch" /Fo"debug.tcp/" /Fd$(OUTDIR)/"tin.pdb"\
 /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

# PROP Intermediate_Dir "release.tcp"
INTDIR_SRC=.\release.tcp

.\release.tcp\nntpw32.obj :  $(SOURCE)  $(DEP_NNTPW) $(INTDIR_SRC)
   $(CPP) /nologo /ML /W3 /GX /Zi /YX"tin.h" /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32IP" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D "NNTP_ABLE" /D\
 INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D INEWS_MAIL_GATEWAY=\"\" /D\
 "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING" /FR"release.tcp/"\
 /Fp$(OUTDIR)/"tin.pch" /Fo"release.tcp/" /Fd$(OUTDIR)/"tin.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

# PROP Intermediate_Dir "debug.pip"
INTDIR_SRC=.\debug.pip

.\debug.pip\nntpw32.obj :  $(SOURCE)  $(DEP_NNTPW) $(INTDIR_SRC)
   $(CPP) /nologo /ML /W3 /GX /Zi /YX"tin.h" /Od /I "..\include" /D\
 "WIN32PIPES" /D "DEBUG" /D "_DEBUG" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D\
 "_CONSOLE" /D "NNTP_ABLE" /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D\
 INEWS_MAIL_GATEWAY=\"\" /D "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING"\
 /FR"debug.pip/" /Fp$(OUTDIR)/"tin.pch" /Fo"debug.pip/" /Fd$(OUTDIR)/"tin.pdb"\
 /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

# PROP Intermediate_Dir "release.pip"
INTDIR_SRC=.\release.pip

.\release.pip\nntpw32.obj :  $(SOURCE)  $(DEP_NNTPW) $(INTDIR_SRC)
   $(CPP) /nologo /ML /W3 /GX /Zi /YX"tin.h" /O2 /I "..\include" /D "NDEBUG" /D\
 "WIN32PIPES" /D "DONT_HAVE_TM_GMTOFF" /D "WIN32" /D "_CONSOLE" /D "NNTP_ABLE"\
 /D INEWS_MAIL_DOMAIN=\"$INEWS_MAIL_DOMAIN\" /D INEWS_MAIL_GATEWAY=\"\" /D\
 "DONT_LOG_USER" /D "HAVE_MAIL_HANDLING" /FR"release.pip/"\
 /Fp$(OUTDIR)/"tin.pch" /Fo"release.pip/" /Fd$(OUTDIR)/"tin.pdb" /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\envarg.c
DEP_ENVAR=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/envarg.obj :  $(SOURCE)  $(DEP_ENVAR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/envarg.obj :  $(SOURCE)  $(DEP_ENVAR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/envarg.obj :  $(SOURCE)  $(DEP_ENVAR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/envarg.obj :  $(SOURCE)  $(DEP_ENVAR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/envarg.obj :  $(SOURCE)  $(DEP_ENVAR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/envarg.obj :  $(SOURCE)  $(DEP_ENVAR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\hashstr.c
DEP_HASHS=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/hashstr.obj :  $(SOURCE)  $(DEP_HASHS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/hashstr.obj :  $(SOURCE)  $(DEP_HASHS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/hashstr.obj :  $(SOURCE)  $(DEP_HASHS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/hashstr.obj :  $(SOURCE)  $(DEP_HASHS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/hashstr.obj :  $(SOURCE)  $(DEP_HASHS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/hashstr.obj :  $(SOURCE)  $(DEP_HASHS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\charset.c

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/charset.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/charset.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/charset.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/charset.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/charset.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/charset.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\active.c
DEP_ACTIVE=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/active.obj :  $(SOURCE)  $(DEP_ACTIVE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/active.obj :  $(SOURCE)  $(DEP_ACTIVE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/active.obj :  $(SOURCE)  $(DEP_ACTIVE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/active.obj :  $(SOURCE)  $(DEP_ACTIVE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/active.obj :  $(SOURCE)  $(DEP_ACTIVE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/active.obj :  $(SOURCE)  $(DEP_ACTIVE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\search.c
DEP_SEARC=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/search.obj :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/search.obj :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/search.obj :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/search.obj :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/search.obj :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/search.obj :  $(SOURCE)  $(DEP_SEARC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\page.c
DEP_PAGE_=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/page.obj :  $(SOURCE)  $(DEP_PAGE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/page.obj :  $(SOURCE)  $(DEP_PAGE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/page.obj :  $(SOURCE)  $(DEP_PAGE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/page.obj :  $(SOURCE)  $(DEP_PAGE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/page.obj :  $(SOURCE)  $(DEP_PAGE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/page.obj :  $(SOURCE)  $(DEP_PAGE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\debug.c
DEP_DEBUG=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/debug.obj :  $(SOURCE)  $(DEP_DEBUG) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\select.c
DEP_SELEC=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/select.obj :  $(SOURCE)  $(DEP_SELEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/select.obj :  $(SOURCE)  $(DEP_SELEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/select.obj :  $(SOURCE)  $(DEP_SELEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/select.obj :  $(SOURCE)  $(DEP_SELEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/select.obj :  $(SOURCE)  $(DEP_SELEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/select.obj :  $(SOURCE)  $(DEP_SELEC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\main.c
DEP_MAIN_=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/main.obj :  $(SOURCE)  $(DEP_MAIN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\screen.c
DEP_SCREE=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/screen.obj :  $(SOURCE)  $(DEP_SCREE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/screen.obj :  $(SOURCE)  $(DEP_SCREE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/screen.obj :  $(SOURCE)  $(DEP_SCREE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/screen.obj :  $(SOURCE)  $(DEP_SCREE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/screen.obj :  $(SOURCE)  $(DEP_SCREE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/screen.obj :  $(SOURCE)  $(DEP_SCREE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\post.c
DEP_POST_=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/post.obj :  $(SOURCE)  $(DEP_POST_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/post.obj :  $(SOURCE)  $(DEP_POST_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/post.obj :  $(SOURCE)  $(DEP_POST_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/post.obj :  $(SOURCE)  $(DEP_POST_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/post.obj :  $(SOURCE)  $(DEP_POST_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/post.obj :  $(SOURCE)  $(DEP_POST_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\getline.c
DEP_GETLI=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/getline.obj :  $(SOURCE)  $(DEP_GETLI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/getline.obj :  $(SOURCE)  $(DEP_GETLI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/getline.obj :  $(SOURCE)  $(DEP_GETLI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/getline.obj :  $(SOURCE)  $(DEP_GETLI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/getline.obj :  $(SOURCE)  $(DEP_GETLI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/getline.obj :  $(SOURCE)  $(DEP_GETLI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\attrib.c
DEP_ATTRI=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/attrib.obj :  $(SOURCE)  $(DEP_ATTRI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/attrib.obj :  $(SOURCE)  $(DEP_ATTRI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/attrib.obj :  $(SOURCE)  $(DEP_ATTRI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/attrib.obj :  $(SOURCE)  $(DEP_ATTRI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/attrib.obj :  $(SOURCE)  $(DEP_ATTRI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/attrib.obj :  $(SOURCE)  $(DEP_ATTRI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\inews.c
DEP_INEWS=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/inews.obj :  $(SOURCE)  $(DEP_INEWS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/inews.obj :  $(SOURCE)  $(DEP_INEWS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/inews.obj :  $(SOURCE)  $(DEP_INEWS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/inews.obj :  $(SOURCE)  $(DEP_INEWS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/inews.obj :  $(SOURCE)  $(DEP_INEWS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/inews.obj :  $(SOURCE)  $(DEP_INEWS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\amiga.c
DEP_AMIGA=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/amiga.obj :  $(SOURCE)  $(DEP_AMIGA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/amiga.obj :  $(SOURCE)  $(DEP_AMIGA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/amiga.obj :  $(SOURCE)  $(DEP_AMIGA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/amiga.obj :  $(SOURCE)  $(DEP_AMIGA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/amiga.obj :  $(SOURCE)  $(DEP_AMIGA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/amiga.obj :  $(SOURCE)  $(DEP_AMIGA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\amigatcp.c
DEP_AMIGAT=\
	include\amigatcp.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/amigatcp.obj :  $(SOURCE)  $(DEP_AMIGAT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/amigatcp.obj :  $(SOURCE)  $(DEP_AMIGAT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/amigatcp.obj :  $(SOURCE)  $(DEP_AMIGAT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/amigatcp.obj :  $(SOURCE)  $(DEP_AMIGAT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/amigatcp.obj :  $(SOURCE)  $(DEP_AMIGAT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/amigatcp.obj :  $(SOURCE)  $(DEP_AMIGAT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\win32.c
DEP_WIN32=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/win32.obj :  $(SOURCE)  $(DEP_WIN32) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/win32.obj :  $(SOURCE)  $(DEP_WIN32) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/win32.obj :  $(SOURCE)  $(DEP_WIN32) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/win32.obj :  $(SOURCE)  $(DEP_WIN32) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/win32.obj :  $(SOURCE)  $(DEP_WIN32) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/win32.obj :  $(SOURCE)  $(DEP_WIN32) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\feed.c
DEP_FEED_=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/feed.obj :  $(SOURCE)  $(DEP_FEED_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/feed.obj :  $(SOURCE)  $(DEP_FEED_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/feed.obj :  $(SOURCE)  $(DEP_FEED_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/feed.obj :  $(SOURCE)  $(DEP_FEED_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/feed.obj :  $(SOURCE)  $(DEP_FEED_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/feed.obj :  $(SOURCE)  $(DEP_FEED_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\memory.c
DEP_MEMOR=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/memory.obj :  $(SOURCE)  $(DEP_MEMOR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/memory.obj :  $(SOURCE)  $(DEP_MEMOR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/memory.obj :  $(SOURCE)  $(DEP_MEMOR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/memory.obj :  $(SOURCE)  $(DEP_MEMOR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/memory.obj :  $(SOURCE)  $(DEP_MEMOR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/memory.obj :  $(SOURCE)  $(DEP_MEMOR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\wildmat.c
DEP_WILDM=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/wildmat.obj :  $(SOURCE)  $(DEP_WILDM) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/wildmat.obj :  $(SOURCE)  $(DEP_WILDM) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/wildmat.obj :  $(SOURCE)  $(DEP_WILDM) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/wildmat.obj :  $(SOURCE)  $(DEP_WILDM) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/wildmat.obj :  $(SOURCE)  $(DEP_WILDM) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/wildmat.obj :  $(SOURCE)  $(DEP_WILDM) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\lang.c
DEP_LANG_=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/lang.obj :  $(SOURCE)  $(DEP_LANG_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/lang.obj :  $(SOURCE)  $(DEP_LANG_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/lang.obj :  $(SOURCE)  $(DEP_LANG_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/lang.obj :  $(SOURCE)  $(DEP_LANG_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/lang.obj :  $(SOURCE)  $(DEP_LANG_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/lang.obj :  $(SOURCE)  $(DEP_LANG_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\signal.c
DEP_SIGNA=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/signal.obj :  $(SOURCE)  $(DEP_SIGNA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/signal.obj :  $(SOURCE)  $(DEP_SIGNA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/signal.obj :  $(SOURCE)  $(DEP_SIGNA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/signal.obj :  $(SOURCE)  $(DEP_SIGNA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/signal.obj :  $(SOURCE)  $(DEP_SIGNA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/signal.obj :  $(SOURCE)  $(DEP_SIGNA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\thread.c
DEP_THREA=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/thread.obj :  $(SOURCE)  $(DEP_THREA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/thread.obj :  $(SOURCE)  $(DEP_THREA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/thread.obj :  $(SOURCE)  $(DEP_THREA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/thread.obj :  $(SOURCE)  $(DEP_THREA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/thread.obj :  $(SOURCE)  $(DEP_THREA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/thread.obj :  $(SOURCE)  $(DEP_THREA) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\save.c
DEP_SAVE_=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/save.obj :  $(SOURCE)  $(DEP_SAVE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/save.obj :  $(SOURCE)  $(DEP_SAVE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/save.obj :  $(SOURCE)  $(DEP_SAVE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/save.obj :  $(SOURCE)  $(DEP_SAVE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/save.obj :  $(SOURCE)  $(DEP_SAVE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/save.obj :  $(SOURCE)  $(DEP_SAVE_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\open.c
DEP_OPEN_=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/open.obj :  $(SOURCE)  $(DEP_OPEN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/open.obj :  $(SOURCE)  $(DEP_OPEN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/open.obj :  $(SOURCE)  $(DEP_OPEN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/open.obj :  $(SOURCE)  $(DEP_OPEN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/open.obj :  $(SOURCE)  $(DEP_OPEN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/open.obj :  $(SOURCE)  $(DEP_OPEN_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\misc.c
DEP_MISC_=\
	include\tin.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/misc.obj :  $(SOURCE)  $(DEP_MISC_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=src\art.c
DEP_ART_C=\
	include\tin.h\
	include\stpwatch.h\
	include\config.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/art.obj :  $(SOURCE)  $(DEP_ART_C) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/art.obj :  $(SOURCE)  $(DEP_ART_C) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/art.obj :  $(SOURCE)  $(DEP_ART_C) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/art.obj :  $(SOURCE)  $(DEP_ART_C) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/art.obj :  $(SOURCE)  $(DEP_ART_C) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/art.obj :  $(SOURCE)  $(DEP_ART_C) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\parsdate.c
DEP_PARSD=\
	include\tin.h\
	include\config.h\
	include\curses.h\
	include\amiga.h\
	include\os_2.h\
	include\win32.h\
	include\patchlev.h\
	include\extern.h\
	include\nntplib.h\
	include\proto.h\
	include\menukeys.h

!IF  "$(CFG)" == "XNS Debug"

$(INTDIR)/parsdate.obj :  $(SOURCE)  $(DEP_PARSD) $(INTDIR)

!ELSEIF  "$(CFG)" == "XNS Release"

$(INTDIR)/parsdate.obj :  $(SOURCE)  $(DEP_PARSD) $(INTDIR)

!ELSEIF  "$(CFG)" == "TCP/IP Debug"

$(INTDIR)/parsdate.obj :  $(SOURCE)  $(DEP_PARSD) $(INTDIR)

!ELSEIF  "$(CFG)" == "TCP/IP Release"

$(INTDIR)/parsdate.obj :  $(SOURCE)  $(DEP_PARSD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Pipes Debug"

$(INTDIR)/parsdate.obj :  $(SOURCE)  $(DEP_PARSD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Pipes Release"

$(INTDIR)/parsdate.obj :  $(SOURCE)  $(DEP_PARSD) $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
