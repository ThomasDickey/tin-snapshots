/*
 *  Project   : tin - a Usenet reader
 *  Module    : config.h
 *  Author    : I.Lea
 *  Created   : 03-09-92
 *  Updated   : 18-06-95
 *  Notes     : #defines to determine different OS capabilites
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *		You may  freely  copy or  redistribute	this software,
 *		so  long as there is no profit made from its use, sale
 *		trade or  reproduction.  You may not change this copy-
 *		right notice, and it must be included in any copy made
 */

/* functions that we'll assume we have unless there's a special reason */
#define HAVE_GETCWD
#define HAVE_MKDIR
#define HAVE_TZSET
#define HAVE_MEMCMP
#define HAVE_MEMCPY
#define HAVE_MEMSET
#define HAVE_STRCHR
#define HAVE_STRTOL

/* VAXC cant tell the difference between 'varA' and 'vara' */
/*	and, what a surprise, suffers from a limit of 31 chars for vars */

#if defined(VMS)
#   if defined(VAXC) || defined(__DECC)
#	define CASE_PROBLEM
#	define VAR_LENGTH_PROBLEM
#   endif
#endif

#if !defined(M_AMIGA) && !defined(M_OS2) && !defined(M_UNIX) && !defined(WIN32) && !defined(VMS)
#	define	M_UNIX
#endif

#ifndef VMS
#	define HAVE_UNLINK
#endif

#ifdef VMS
#   define  HAVE_IS_XTERM
#endif

/*
 * HP/Apollo CC 6.8 is reasonably close to ANSI
 */

#undef DECL_GETENV		/* assume 'getenv()' is declared */
#undef pre_CC_6_8
#if defined(apollo)
#	if !defined(__STDCPP__) && !defined(__GNUC__)
#		define  DECL_GETENV
#		define pre_CC_6_8
#	endif
#endif

#if defined(M_XENIX)
#	define	HAVE_PROTOTYPES_H
#endif

#if defined(AUX) || defined(EPIX) || defined (__hpux) || defined(PTX)
#	define	HAVE_TERMIO_H
#endif

#if defined(QNX42) || defined(UMAXV)
#	define	HAVE_TERMIOS_H
#endif

#if defined(_POSIX_SOURCE) || defined(__386BSD__) || defined(DGUX) || \
    defined(SVR4) || defined(UMAXV) || defined(__NetBSD__)
#	if !defined(M_OS2)
#		define	HAVE_POSIX_JC
#	endif
#endif

#if defined(M_OS2) || defined(SVR4)
#	define	HAVE_LOCALE_H
#endif

#if defined(SVR4)
#	define	HAVE_SETLOCALE
#	undef	sinix		/* SNI Sinix (nsc32000) */
#endif

#if !defined(M_AMIGA)
#	define	HAVE_CURSES_H
#endif

#if defined(SIGCHLD) || defined(__hpux)
#	define	HAVE_SYS_WAIT_H
#endif

#if !defined(M_OS2) && !defined(WIN32) && !defined(M_AMIGA)
#	define	HAVE_PWD_H
#endif

#if !defined(M_OS2) && !defined(WIN32) && !defined(M_AMIGA) && !defined(VMS)
#	define	HAVE_SYS_PARAM_H
#endif

#if !defined(apollo) && !defined(gould) && !defined(MACH) && \
    !defined(mips) && !defined(__NeXT__) && !defined(M_OS2) && \
	!defined(WIN32) && !defined (M_AMIGA)
#	define	HAVE_UNISTD_H
#endif

#if !defined(M_AMIGA) && !defined(COHERENT) && !defined(MINIX) && \
    !defined(M_OS2) && !defined(WIN32) && !defined(VMS)
#	define	HAVE_SYS_IOCTL_H
#endif

#if !defined(M_AMIGA) && !defined(apollo) && !defined(BSD) && \
    !defined(M_OS2) && !defined(sinix) && !defined(RS6000) && \
	!defined(WIN32) && !defined(VMS)
#	define	HAVE_SYS_UTSNAME_H
#endif

#if !defined(pre_CC_6_8) && !defined(EPIX) && !defined(pyr) && \
    !defined(sequent) && !defined(sysV68) && !defined(UTS) && \
    !defined(u3b2)
#	define	HAVE_STDLIB_H
#endif

/*
 * prefer <string.h> if we can get it, because it's ANSI
 */
#if defined(BSD) && !defined(__STDC__)
#	undef	HAVE_STRCHR
#	define	HAVE_STRINGS_H
#else
#	define	HAVE_STRING_H
#endif

#if defined(apollo) || defined(BSD) || defined(EPIX) || defined(M_AMIGA) || \
    defined(M_OS2) || defined(__osf__) || defined(UMAXV) || defined(WIN32)
#	define	HAVE_FCNTL_H
#endif

#if !defined(__hpux)
#	define	HAVE_SYS_STREAM_H
#endif

#if !defined(apollo) && !defined(COHERENT) && !defined(__hpux) && \
    !defined(M_OS2) && !defined(QNX42) && !defined(sinix) && \
    !defined(UMAXV) && !defined(WIN32)
#	define	HAVE_SYS_PTEM_H
#endif

#if !defined(apollo) && !defined(COHERENT) && !defined(M_OS2) && \
    !defined(QNX42) && !defined(SCO_UNIX) && !defined(sinix) && \
    !defined(SVR4) && !defined(WIN32)
#	define	HAVE_SYS_PTY_H
#	define	XWIN	/* stops ISC bitching */
#endif

#if defined(__386BSD__) || defined(apollo) || defined(BSD) || \
    defined(__hpux) || defined(linux) || defined(M_OS2) || \
    defined(__osf__) || defined(RS6000) || defined(sinix) || \
    defined(UMAXV)
#	define	HAVE_NETDB_H
#endif

#if defined(M_OS2) || defined(WIN32)
#	define	HAVE_NETLIB_H
#endif

#if !defined(pyr)
#	define	HAVE_TIME_H
#endif

#if !defined(M_OS2) && !defined(SCO_UNIX) && !defined(u3b2) && \
    !defined(WIN32) && !defined (M_AMIGA) && !defined(VMS)
#	define	HAVE_SYS_TIME_H
#endif

#if defined(SCO_UNIX) || defined(u3b2)
#	define	HAVE_SYS_TIMES_H
#endif

#define TIME_WITH_SYS_TIME

#if defined(PTX) || defined(QNX42) || defined(RS6000) || defined(SCO_UNIX)
#	define	HAVE_SYS_SELECT_H
#endif

#if defined(M_AMIGA) || defined(COHERENT) || defined(M_OS2) || \
	defined(QNX42) || defined(WIN32) || defined(VMS)
#	define	HAVE_ERRNO_H
#endif

#if defined(__GNUC__) || defined(HAVE_POSIX_JC)
#	define	RETSIGTYPE void
#else
#	if defined(sony)
#		define	RETSIGTYPE int
#	else
#		if __STDC__ || defined(atthcx) || defined(__hpux) || \
		   defined(__osf__) || defined(M_OS2) || defined(PTX) || \
		   defined(RS6000) || defined(sgi) || defined(sinix) || \
		   defined(sysV68) || defined(sun) || defined(SVR4) || \
		   defined(u3b2) || defined(ultrix)	|| defined(WIN32)
#			define	RETSIGTYPE void
#		else
#			define	RETSIGTYPE int
#		endif
#	endif
#endif

#if defined(__STDC__) && (defined(apollo) || defined(sun))
#	define DECL_SIG_CONST
#endif

#if defined(M_OS2) || defined(apollo) || defined(linux) || defined(SVR4) || \
	defined(WIN32) || defined (M_AMIGA)
#	if !defined(HAVE_COMPTYPE_CHAR)
#		define	HAVE_COMPTYPE_VOID
#	endif
#else
#	if !defined(HAVE_COMPTYPE_VOID)
#		define	HAVE_COMPTYPE_CHAR
#	endif
#endif

#if !defined(__GNUC__)
#	define DECL_TGETSTR
#endif

#undef	DONT_HAVE_TM_GMTOFF

#if defined(apollo) || defined(AUX) || defined(BSD) || defined(linux) || \
    defined(__hpux) || defined(__osf__) || defined(PTX) || defined(QNX42) || \
    defined(RS6000) || defined(sinix) || defined(SVR4) || defined(UMAXV) || \
    defined(WIN32) || defined (M_AMIGA)
#	define	HAVE_LONG_FILENAMES
#endif

#if defined(apollo) || defined(BSD) || defined(__hpux) || defined(linux) || \
    defined(__osf__) || defined(M_OS2) || defined(RS6000) || defined(sinix) || \
    defined(UMAXV) || defined(VMS)
#	define	HAVE_GETHOSTBYNAME
#endif

/*
 * Used in tin.h
 */

#if __STDC__ || defined(SVR4)
#	if !defined(pre_CC_6_8) && !defined(__hpux) && !defined(sun)
#		define	HAVE_ANSI_ASSERT
#	endif
#endif

#if defined(M_UNIX)
#	define	HAVE_COREFILE
#endif

#if defined(M_UNIX)
#	define	HAVE_SET_GID_UID
#endif

#if defined(M_UNIX) && !defined(__386BSD__) && !defined(apollo) && \
    !defined(__NeXT__)
#	define	HAVE_UNAME
#endif

#if defined(MACH) || defined(__NeXT__) || defined(M_OS2)
#	define	DONT_HAVE_SIGWINCH
#endif

#if defined(BSD) || defined(EPIX) && !defined(__386BSD__) && \
    !defined(sinix)
#	undef	HAVE_GETCWD
#endif

#if defined(pyr) || defined(sequent)
#	undef	HAVE_MEMCMP
#endif

#if defined(pyr)
#	undef	HAVE_MEMCPY
#endif

#if defined(pyr)
#	undef	HAVE_MEMSET
#endif

#if (defined(BSD) && defined(sequent))
#	undef	HAVE_STRTOL
#endif

#if defined(__arm) || defined(COHERENT) || defined(pyr) || \
    defined(sequent) || defined (M_AMIGA)
#	undef	HAVE_TZSET
#endif

/*
 * Used in parsedate.y
 */

#if defined(apollo) || defined(__arm) || defined(__convex__) || \
    defined(DGUX) || defined(pyr) || defined(sequent) || !defined(BSD)
#	define	DONT_HAVE_TM_GMTOFF
#endif

/*
 * Use poll()/select() in input_pending()
 */

#if defined(SVR4) || defined(__hp9000s700) || defined(__hp9000s900) || \
	defined(HAVE_POLL)
#	if !defined(__hpux)
#		define	HAVE_STROPTS_H
#	endif
#	define	HAVE_POLL_H
#	if !defined(HAVE_POLL)
#		define	HAVE_POLL
#	endif
#	undef HAVE_SELECT
#endif

#if !defined(apollo) && !defined(M_AMIGA) && !defined(COHERENT) && \
    !defined(M_OS2) && !defined(supermax) && !defined(u3b2) && \
    !defined(HAVE_POLL) && !defined(WIN32)
#	define	HAVE_SELECT
#endif

#if defined(M_AMIGA)
#	define	SMALL_MEMORY_MACHINE
#endif

#if defined(M_AMIGA)
#	define	SIG_ARGS /*nothing, since compiler doesn't handle it*/
#	undef   DECL_SIG_CONST
#endif

#if defined(M_AMIGA)
#	define	DONT_REREAD_ACTIVE_FILE
#endif

#if defined(M_AMIGA)
#	define	HAVE_STDDEF_H
#endif

#if defined(COHERENT)
#	define	HAVE_SETTZ
#endif

#if defined(M_UNIX)
#	define	HAVE_FORK
#endif

#if defined(M_OS2) || defined(linux) || defined(RS6000) || defined(SVR4)
#	define	HAVE_STRFTIME
#endif

#if !defined(M_OS2) && !defined(VMS)
#	define	HAVE_SYSERRLIST
#endif

#if defined(__hpux)
#	define	HAVE_KEYPAD
#endif

#if defined(RS6000)
#	define	READ_CHAR_HACK
#endif

#if defined(sinix)
#	undef	HAVE_SYS_STREAM_H
#	undef	HAVE_MKDIR
#endif

#if defined(supermax)
#	define	HAVE_BROKEN_TGETSTR
#endif

#if defined(QNX42)
#	define	HAVE_TCGETATTR
#	define	HAVE_TCSETATTR
#endif

#if defined(M_AMIGA) || defined(QNX42) || defined(VMS)
#	define HAVE_KEY_PREFIX
#endif

#if defined(M_UNIX) || defined (M_AMIGA)
#	define HAVE_METAMAIL
#endif

#if defined(linux)
#	if !defined(HAVE_ISPELL)
#		define	HAVE_ISPELL
#	endif
#endif

/*
 * Hack used to try and get a compile on Sun i386 & old SunOS 4.0.2
 */

#if defined(sun) && defined(i386)
#	undef	HAVE_STDLIB_H
#endif

#if defined(BSD) || defined(__osf__) || defined(_POSIX_SOURCE)
#	define	HAVE_REWINDDIR
#endif

/*
 * Various hacks used to try and get a compile on the strange ones...
 */

#if defined(u3b2)
#	define	size_t	unsigned long int
#endif

/*
 * Lets try and be a wise ass and make a nntp able binary
 * for machines where the netlibs are in the libc library
 */

#if defined(BSD) || defined(linux) || defined(RS6000)
#	if !defined(NNTP_ABLE)
#		define	NNTP_ABLE
#	endif
#endif

/*
 * UCX (aka TCP/IP services for OpenVMS doesn't implement getservbyname 
 */

#if defined(VMS) && defined(UCX)
#	define NO_GETSERVBYNAME
#endif
