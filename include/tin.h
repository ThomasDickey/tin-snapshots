/*
 *  Project   : tin - a Usenet reader
 *  Module    : tin.h
 *  Author    : I. Lea & R. Skrenta
 *  Created   : 1991-04-01
 *  Updated   : 1997-12-31
 *  Notes     : #include files, #defines & struct's
 *  Copyright : (c) Copyright 1991-99 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

/*
 * OS specific doda's
 */

#ifndef TIN_H
#define TIN_H 1

#ifdef HAVE_CONFIG_H
#	include	<autoconf.h>	/* FIXME: normally we use 'config.h' */
#else
#	ifndef HAVE_CONFDEFS_H
#		include	"config.h"
#	endif /* !HAVE_CONFDEFS_H */
#endif /* HAVE_CONFIG_H */

/*
 * Non-autoconf'able definitions for Amiga Developer Environment (gcc 2.7.2,
 * etc).
 */
#if defined(__amiga__) || defined(__amiga)
#	define SMALL_MEMORY_MACHINE
#	if !defined(__GNUC__)
#		undef M_UNIX
#		define M_AMIGA
#		define SIG_ARGS /*nothing, since compiler doesn't handle it*/
#		undef DECL_SIG_CONST
#	endif /* !__GNUC__ */
#endif /* __amiga__ || __amiga */

#include	<signal.h>

enum context { cMain, cArt, cConfig, cGroup, cHelp, cPage, cSelect, cThread };
enum resizer { cNo, cYes, cRedraw };

#ifdef VMS
#	ifdef __DECC
#		include <unixio.h>
#	else
#		ifndef __VMS_VER  /* assume old types.h */
			typedef unsigned short mode_t;
#			undef HAVE_STRFTIME
#		endif /* !__VMS_VER */
#		include <stdio.h>
#	endif /* __DECC */
#	ifdef SOCKETSHR_TCP
#		include <socketshr.h>
#		include <unistd.h>
#		ifndef SOCKETSHR_HAVE_DUP
#			define dup
#		endif /* !SOCKETSHR_HAVE_DUP */
#		ifndef SOCKETSHR_HAVE_FERROR
#			define ferror(a) (0)
#		endif /* !SOCKETSHR_HAVE_FERROR */
#	endif /* SOCKETSHR_TCP */
#	include <curses.h>
#	include <stat.h>
#	undef HAVE_SELECT
#	define XHDR_XREF	/* enable crosspost support */
#	define CASE_PROBLEM
#	define HAVE_ERRNO_H
#	define NNTP_ONLY
#	define NNTP_INEWS
#	define DONT_HAVE_PIPING
#	define NO_SHELL_ESCAPE
#	define USE_CLEARSCREEN
#	ifndef MM_CHARSET
#		define MM_CHARSET "ISO-8859-1"
#	endif /* !MM_CHARSET */
	/* Apparently this means fileops=create if not already there - no idea
	 * why this should be needed. Standard fopen() implies this in arg 2
	 */
	extern char *get_uaf_fullname();
#	ifdef MULTINET
#		include "MULTINET_ROOT:[MULTINET.INCLUDE.SYS]TYPES.H"
#		include "MULTINET_ROOT:[MULTINET.INCLUDE.SYS]TIME.H"
#	else
#		include "select.h"
#		ifdef VAXC
#			include "vmstimval.h"
#		endif /* VAXC */
#		include <types.h>
#	endif /* !MULTINET */
#	define FOPEN_OPTS       , "fop=cif"
#else
#	define FOPEN_OPTS
#endif /* VMS */

#include <stdio.h>
#ifdef HAVE_ERRNO_H
#	include	<errno.h>
#else
#	include	<sys/errno.h>
#endif /* HAVE_ERRNO_H */
#if !defined(errno)
#	ifdef DECL_ERRNO
		extern int errno;
#	endif /* DECL_ERRNO */
#endif /* !errno */

#ifdef HAVE_STDDEF_H
#	include <stddef.h>
#endif /* HAVE_STDDEF_H */
#include <sys/types.h>

#ifdef M_AMIGA
#	include "include:stat.h"	/* FIXME: Problem with AmiTCP-includes, AmiTCP's fstat() needs */
#else									/* a running TCP-Stack. OTOH fstat() ist used with local spool */
#	include <sys/stat.h>
#endif /* M_AMIGA */

#ifdef TIME_WITH_SYS_TIME
#	include <sys/time.h>
#	include <time.h>
#else
#	ifdef HAVE_SYS_TIME_H
#		include <sys/time.h>
#	else
#		include <time.h>
#	endif /* HAVE_SYS_TIME_H */
#endif /* TIME_WITH_SYS_TIME */

#ifdef HAVE_SYS_TIMES_H
#	include <sys/times.h>
#endif /* HAVE_SYS_TIMES_H */

#ifdef HAVE_LIBC_H
#	include <libc.h>
#endif /* HAVE_LIBC_H */

#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef HAVE_PWD_H
#	include <pwd.h>
#endif /* HAVE_PWD_H */

#ifdef HAVE_SYS_PARAM_H
#	include <sys/param.h>
#endif /* HAVE_SYS_PARAM_H */

#include	<ctype.h>

#ifdef HAVE_STDLIB_H
#	include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#include <stdarg.h>

#ifdef HAVE_GETOPT_H
#	include <getopt.h>
#endif /* HAVE_GETOPT_H */

/* prefer string.h because it's Posix */
#ifdef HAVE_STRING_H
#	include <string.h>
#else
#	ifdef HAVE_STRINGS_H
#		include <strings.h>
#	endif /* HAVE_STRINGS_H */
#endif /* HAVE_STRING_H */

#ifdef HAVE_FCNTL_H
#	include <fcntl.h>
#endif /* HAVE_FCNTL_H */

#ifdef HAVE_SYS_IOCTL_H
#	include <sys/ioctl.h>
/* We don't need/use these, and they cause redefinition errors with SunOS 4.x
 * when we include termio.h or termios.h
 */
#	if defined(sun) && !defined(__svr4)
#		undef NL0
#		undef NL1
#		undef CR0
#		undef CR1
#		undef CR2
#		undef CR3
#		undef TAB0
#		undef TAB1
#		undef TAB2
#		undef XTABS
#		undef BS0
#		undef BS1
#		undef FF0
#		undef FF1
#		undef ECHO
#		undef NOFLSH
#		undef TOSTOP
#		undef FLUSHO
#		undef PENDIN
#	endif /* sun && !__svr4 */
#endif /* HAVE_SYS_IOCTL_H */

#ifdef HAVE_PROTOTYPES_H
#	include <prototypes.h>
#endif /* HAVE_PROTOTYPES_H */

#if defined(HAVE_LOCALE_H) && !defined(NO_LOCALE)
#	include <locale.h>
#endif /* HAVE_LOCALE_H && !NO_LOCALE */

#ifdef HAVE_SYS_UTSNAME_H
#	include <sys/utsname.h>
#endif /* HAVE_SYS_UTSNAME_H */

/*
 * Needed for catching child processes
 */
#ifdef HAVE_SYS_WAIT_H
#	include <sys/wait.h>
#endif /* HAVE_SYS_WAIT_H */

/*
 * Needed for timeout in user abort of indexing a group (BSD & SYSV variaties)
 */
#ifdef HAVE_SYS_SELECT_H
#	ifdef NEED_TIMEVAL_FIX
#		define timeval fake_timeval
#		include <sys/select.h>
#		undef timeval
#	else
#		include <sys/select.h>
#	endif /* NEED_TIMEVAL_FIX */
#endif /* HAVE_SYS_SELECT_H */

#ifdef HAVE_STROPTS_H
#	include <stropts.h>
#endif /* HAVE_STROPTS_H */

#ifdef HAVE_POLL_H
#	include <poll.h>
#endif /* HAVE_POLL_H */

/*
 * Directory handling code
 */
#ifdef HAVE_CONFIG_H
#	ifdef HAVE_DIRENT_H
#		include <dirent.h>
#		define DIR_BUF	struct dirent
#	else
#		ifdef HAVE_SYS_DIR_H
#			include <sys/dir.h>
#		endif /* HAVE_SYS_DIR_H */
#		ifdef HAVE_SYS_NDIR_H
#			include <sys/ndir.h>
#		endif /* HAVE_SYS_NDIR_H */
#		define DIR_BUF	struct direct
#	endif /* HAVE_DIRENT_H */
#else
#	ifdef M_AMIGA
#		include "amiga.h"
#		define DIR_BUF	struct dirent
#	endif /* M_AMIGA */
#	ifdef M_OS2
#		include "os_2.h"
#		define DIR_BUF	struct dirent
#	endif /* M_OS2 */
#	ifdef WIN32
#		include "win32.h"
#		define DIR_BUF	struct direct
#	endif /* WIN32 */
#	ifdef M_XENIX
#		include <sys/ndir.h>
#		define DIR_BUF	struct direct
#	endif /* M_XENIX */
#	ifdef VMS
#		include "ndir.h"
#		define DIR_BUF	struct direct
#	endif /* VMS */
#endif /* !HAVE_CONFIG_H */

#ifndef DIR_BUF
#	include <dirent.h>
#	define DIR_BUF	struct dirent
#endif /* !DIR_BUF */

#ifndef HAVE_UNLINK
#	define unlink(file)	remove(file)
#endif /* !HAVE_UNLINK */

/*
 * If native OS has'nt defined STDIN_FILENO be a smartass and do it
 */
#if !defined(STDIN_FILENO)
#	define STDIN_FILENO	0
#endif /* !STDIN_FILENO */

/*
 * If OS misses the isascii() function
 */
#if !defined(HAVE_ISASCII) && !defined(isascii)
#	define isascii(c) (!((c) & ~0177))
#endif /* !HAVE_ISASCII && !isascii */

/*
 * Setup support for reading from NNTP
 */
#if defined(NNTP_ABLE) || defined(NNTP_ONLY)
#	ifndef NNTP_ABLE
#		define NNTP_ABLE	1
#	endif /* !NNTP_ABLE */
#	ifndef NNTP_INEWS
#		define NNTP_INEWS	1
#	endif /* !NNTP_INEWS */
#endif /* NNTP_ABLE || NNTP_ONLY */

#if defined(INDEX_DAEMON) && defined(FORGERY)
#	undef FORGERY
#endif /* INDEX_DAEMON && FORGERY */

#define FAKE_NNTP_FP		(FILE *) 9999

/*
 *  Max time between the first character of a VT terminal escape sequence
 *  for special keys and the following characters to arrive (msec)
 */
#define SECOND_CHARACTER_DELAY	200

/*
 * Maximum time (seconds) for a VT terminal escape sequence
 */
#define VT_ESCAPE_TIMEOUT	1

/*
 * Index file daemon version of tin. Will create/update index files from cron
 * on NNTP server machine so clients can retreive index file with NNTP XINDEX
 * command from server. Also can be used on machines that just want one copy
 * of all the index files in one place. In this case the normal tin must have
 * access to the index directory (-I dir option) or be setuid news.
 */
#ifdef INDEX_DAEMON
#	define LOCK_FILE	"tind.LCK"
#	undef	HAVE_POLL
#	undef	HAVE_SELECT
#	undef	NNTP_ABLE
#	undef	NNTP_ONLY
#	undef	NNTP_INEWS
#endif /* INDEX_DAEMON */

/*
 * Specify News spool & control directories if not running NNTP_ONLY
 * (on machines who can run configure this is not needed)
 */
#ifndef HAVE_CONFIG_H
#	ifndef NNTP_ONLY
#		ifndef SPOOLDIR
#			ifdef VMS
#				define SPOOLDIR	"NEWSSPOOL:[000000]"
#			else
#				define SPOOLDIR	"/var/spool/news"
#			endif /* VMS */
#		endif /* !SPOOLDIR */
#		ifndef NEWSLIBDIR
#			ifdef VMS
#				define NEWSLIBDIR	"NEWSLIB:[000000]"
#			else
#				ifdef M_AMIGA
#					define NEWSLIBDIR	"uulib:"
#				else
#					define NEWSLIBDIR	"/usr/lib/news"
#				endif /* M_AMIGA */
#			endif /* VMS */
#		endif /* !NEWSLIBDIR */
#		ifndef NOVROOTDIR
#			define NOVROOTDIR	SPOOLDIR
#		endif /* !NOVROOTDIR */
#		ifndef INEWSDIR
#			define INEWSDIR	NEWSLIBDIR
#		endif /* INEWSDIR */
#	else
#		undef	SPOOLDIR
#		undef	NEWSLIBDIR
#		undef	NOVROOTDIR
#	endif /* !NNTP_ONLY */
#endif /* !HAVE_CONFIG_H */

/*
 * Determine machine configuration for external programs & directories
 */
#if defined(BSD) && !defined(M_AMIGA)
/*
 * To catch 4.3 Net2 code base or newer
 * (e.g. FreeBSD 1.x, 4.3/Reno, NetBSD 0.9, 386BSD, BSD/386 1.1 and below).
 * use
 * #if (defined(BSD) && (BSD >= 199103))
 *
 * To detect if the code is being compiled on a 4.4 code base or newer
 * (e.g. FreeBSD 2.x, 4.4, NetBSD 1.0, BSD/386 2.0 or above).
 * use
 * #if (defined(BSD) && (BSD >= 199306))
 *
 * (defined in <sys/param.h>)
 */
#	ifndef HAVE_MEMCMP
#		define memcmp(s1, s2, n)	bcmp(s2, s1, n)
#	endif /* !HAVE_MEMCMP */
#	ifndef HAVE_MEMCPY
#		define memcpy(s1, s2, n)	bcopy(s2, s1, n)
#	endif /* !HAVE_MEMCPY */
#	ifndef HAVE_MEMSET
#		define memset(s1, s2, n)	bfill(s1, n, s2)
#	endif /* !HAVE_MEMSET */
#	ifndef HAVE_STRCHR
#		define strchr(str, ch)	index(str, ch)
#		define strrchr(str, ch)	rindex(str, ch)
#	endif /* !HAVE_STRCHR */
#	define DEFAULT_SHELL	"/bin/csh"
#	if defined(__386BSD__) || defined(__bsdi__) || defined(__NetBSD__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#		define DEFAULT_PRINTER	"/usr/bin/lpr"
#		define DEFAULT_SUM	"/usr/bin/cksum -o 1 <" /* use tailing <, otherwise get filename output too */
#	else
#		define DEFAULT_PRINTER	"/usr/ucb/lpr"
#		define DEFAULT_SUM	"sum"
#	endif /* __386BSD__ || __bsdi__ || __NetBSD__ ||__FreeBSD__ || __OpenBSD__ */
#	ifdef DGUX
#		define USE_INVERSE_HACK
#	endif /* DGUX */
#	ifdef pyr
#		define DEFAULT_MAILER	"/usr/.ucbucb/mail"
#	endif /* pyr */
#else /* !BSD */
#	ifdef linux
#		define DEFAULT_PRINTER	"/usr/bin/lpr"
#	endif /* linux */
#	ifdef M_AMIGA
#		ifndef DEFAULT_EDITOR
#			define DEFAULT_EDITOR	"c:ed"
#		endif /* !DEFAULT_EDITOR */
#		define DEFAULT_MAILBOX	"uumail:"
#		define DEFAULT_MAILER	"uucp:c/sendmail"
#		define DEFAULT_POSTER	"uucp:c/postnews %s"
#		define DEFAULT_PRINTER	"c:copy to PRT:"
#		define DEFAULT_BBS_PRINTER	"c:copy to NIL:"
#		define DEFAULT_SHELL	"c:newshell"	/* Not Yet Implemented */
#		define DEFAULT_UUDECODE	"uudecode %s"
#		define DEFAULT_UNSHAR	"unshar %s"
#	endif /* M_AMIGA */
#	ifdef VMS
#		define DEFAULT_EDITOR	"EDIT/TPU"
#		define DEFAULT_MAILBOX	"SYS$LOGIN:"
#		define DEFAULT_MAILER	"MAIL"
#		define MAILER_FORMAT	"MAIL /SUBJECT=\"%S\" %F MX%%\"%T\""
#		define DEFAULT_POSTER	"inews %s."
#		define DEFAULT_PRINTER	"PRINT/DELETE"
#		define DEFAULT_UUDECODE	"uudecode %s."
#		define DEFAULT_UNSHAR	"unshar %s."
#	endif /* VMS */
#	ifdef M_OS2
#		ifndef DEFAULT_EDITOR
#			define DEFAULT_EDITOR	"epm /m"
#		endif /* !DEFAULT_EDITOR */
#		define DEFAULT_MAILBOX	"/mail"
#		define DEFAULT_MAILER	"sendmail -af %s -f %s %s"
#		define DEFAULT_POSTER	"postnews %s"
#		define DEFAULT_PRINTER	"lpt1"
#		define DEFAULT_SHELL	"cmd.exe"
#		define DEFAULT_UUDECODE	"uudecode %s"
#		define DEFAULT_UNSHAR	"unshar %s"
#	endif /* M_OS2 */
#	ifdef WIN32
#		ifndef DEFAULT_EDITOR
#			define DEFAULT_EDITOR	"vi"
#		endif /* !DEFAULT_EDITOR */
#		define DEFAULT_MAILBOX	"/mail"
#		define DEFAULT_MAILER	"sendmail"
#		define DEFAULT_POSTER	"postnews %s"
#		define DEFAULT_PRINTER	"lpt1"
#		define DEFAULT_SHELL	"cmd.exe"
#		define DEFAULT_UUDECODE	"uudecode %s"
#		define DEFAULT_UNSHAR	"unshar %s"
#	endif /* WIN32 */
#	ifdef QNX42
#		ifndef DEFAULT_EDITOR
#			define DEFAULT_EDITOR	"/bin/vedit"
#		endif /* !DEFAULT_EDITOR */
#	endif /* QNX42 */
#	ifdef _AIX
#		define DEFAULT_PRINTER	"/bin/lp"
#		define READ_CHAR_HACK
#	endif /* _AIX */
#	ifdef SCO_UNIX
#		define HAVE_MMDF_MAILER
#	endif /* SCO_UNIX */
#	ifdef sinix
#		define DEFAULT_PRINTER	"/bin/lpr"
#	endif /* sinix */
#	ifdef sysV68
#		define DEFAULT_MAILER	"/bin/rmail"
#	endif /* sysV68 */
#	ifdef UNIXPC
#		define DEFAULT_MAILER	"/bin/rmail"
#	endif /* UNIXPC */

/* HP-UX >= 10 defines __STDC_EXT__ (ANSI) || __CLASSIC_C__ (K&R) */
#	if defined (__hpux)
#		if defined (__STDC_EXT__) || defined (__CLASSIC_C__)
#			define DEFAULT_SHELL "/usr/bin/sh"
#		endif /* __STDC_EXT__ || __CLASSIC_C__ */
#	endif /* __hpux */

#	ifndef DEFAULT_SHELL
#		define DEFAULT_SHELL	"/bin/sh"
#	endif /* !DEFAULT_SHELL */
#	ifndef DEFAULT_PRINTER
#		define DEFAULT_PRINTER	"/usr/bin/lp"
#	endif /* !DEFAULT_PRINTER */
#	ifndef PATH_SUM
#		define DEFAULT_SUM	"sum -r"
#	endif /* !PATH_SUM */
#endif /* BSD */

/*
 * fallback values
 */
#ifndef DEFAULT_EDITOR
#	define DEFAULT_EDITOR	"/usr/bin/vi"
#endif /* !DEFAULT_EDITOR */
#ifndef DEFAULT_MAILER
#	define DEFAULT_MAILER	"/usr/lib/sendmail"
#endif /* !DEFAULT_MAILER */
#ifndef DEFAULT_MAILBOX
#	define DEFAULT_MAILBOX	"/usr/spool/mail"
#endif /* !DEFAULT_MAILBOX */


/* FIXME: remove absolut-paths! */
/*
 * Miscellaneous program-paths
 */
#ifndef PATH_ISPELL
#	define PATH_ISPELL	"ispell"
#endif /* !PATH_ISPELL */

#ifndef PATH_METAMAIL
#	define PATH_METAMAIL	"metamail"
#endif /* !PATH_METAMAIL */

/*
 * Fix up the 'sum' path and parameter for './configure'd systems
 */
#ifdef PATH_SUM
#	ifdef DEFAULT_SUM
#		undef DEFAULT_SUM
#	endif /* DEFAULT_SUM */
#	ifdef SUM_TAKES_DASH_R
#		define DEFAULT_SUM PATH_SUM_R
#	else
#		define DEFAULT_SUM PATH_SUM
#	endif /* SUM_TAKES_DASH_R */
#endif /* PATH_SUM */

#ifdef HAVE_LONG_FILE_NAMES
#	define LONG_PATH_PART	"part"
#	define LONG_PATH_PATCH	"patch"
#else
#	define LONG_PATH_PART	""
#	define LONG_PATH_PATCH	"p"
#endif /* HAVE_LONG_FILE_NAMES */

/*
 * How often should the active file be reread for new news
 */
#ifndef REREAD_ACTIVE_FILE_SECS
#	define REREAD_ACTIVE_FILE_SECS 1200	/* seconds (20 mins) */
#endif /* !REREAD_ACTIVE_FILE_SECS */

/*
 * Initial sizes of internal arrays for small (<4MB) & large memory machines
 */
#ifdef SMALL_MEMORY_MACHINE
#	define DEFAULT_ARTICLE_NUM	600
#	define DEFAULT_SAVE_NUM	10
#else
#	define DEFAULT_ARTICLE_NUM	1200
#	define DEFAULT_SAVE_NUM	30
#endif /* SMALL_MEMORY_MACHINE */
#define DEFAULT_ACTIVE_NUM	1800
#define DEFAULT_NEWNEWS_NUM	5

#ifdef VMS
#	define RCDIR	"TIN"
#	define INDEX_MAILDIR	"MAILIDX"
#	define INDEX_NEWSDIR	"INDEX"
#	define INDEX_SAVEDIR	"SAVE"
#else
#	define RCDIR	".tin"
#	define INDEX_MAILDIR	".mail"
#	define INDEX_NEWSDIR	".news"
#	define INDEX_SAVEDIR	".save"
#endif /* VMS */

#define ACTIVE_FILE	"active"
#define ACTIVE_MAIL_FILE	"active.mail"
#define ACTIVE_SAVE_FILE	"active.save"
#define ACTIVE_TIMES_FILE	"active.times"
#define ATTRIBUTES_FILE	"attributes"
#define CONFIG_FILE	"tinrc"
#define DEFAULT_MAILDIR	"Mail"
#define DEFAULT_SAVEDIR	"News"
#define FILTER_FILE	"filter"
#define GROUP_TIMES_FILE	"group.times"
#define INPUT_HISTORY_FILE	".inputhistory"
#define MAILGROUPS_FILE	"mailgroups"
#define MSG_HEADERS_FILE	"headers"
#define NEWSRC_FILE	".newsrc"
#define NEWSRCTABLE_FILE	"newsrctable"
/* ifdef APPEND_PID (default) NEWNEWSRC_FILE will be .newnewsrc<pid> */
#define NEWNEWSRC_FILE	".newnewsrc"
#define OLDNEWSRC_FILE	".oldnewsrc"
#ifndef	OVERVIEW_FILE
#	define OVERVIEW_FILE	".overview"
#endif /* !OVERVIEW_FILE */
#define OVERVIEW_FMT	"overview.fmt"
#define POSTED_FILE	"posted"
#define POSTPONED_FILE	"postponed.articles"
#define SUBSCRIPTIONS_FILE	"subscriptions"

#define _CONF_FROMHOST	"fromhost"
#define _CONF_ORGANIZATION	"organization"
#define _CONF_SERVER	"server"

#include <bool.h>

#ifndef MAX
#	define MAX(a,b)	((a > b) ? a : b)
#endif /* !MAX */

#ifndef forever
/*@notfunction@*/
#	define forever	for(;;)
#endif /* !forever */

#ifndef nop
#	define nop	((void)0)
#endif /* !nop */

#ifndef nobreak
#	define nobreak
#endif /* !nobreak */

/* safe strcpy into fixed-legth buffer */
#define STRCPY(dst, src)	(dst[sizeof(dst) - 1] = '\0', strncpy(dst, src, sizeof(dst) -1))

#define STRCMPEQ(s1, s2)	(*(s1) == *(s2) && strcmp((s1), (s2)) == 0)
#define STRNCMPEQ(s1, s2, n)	(*(s1) == *(s2) && strncmp((s1), (s2), n) == 0)
#define STRNCASECMPEQ(s1, s2, n)	(strncasecmp((s1), (s2), n) == 0)


#if defined(VMS) || defined(M_AMIGA)
#	define LEN	512
#	define PATH_LEN	256
#endif /* VMS || M_AMIGA */
#if defined(M_OS2) || defined(M_UNIX)
#	ifndef MAXPATHLEN
#		define MAXPATHLEN	256
#	endif /* !MAXPATHLEN */
#	define LEN	1024
#	define PATH_LEN	MAXPATHLEN
#endif /* M_OS2 || M_UNIX */
#if defined(WIN32)
#	define LEN	1024
#	define PATH_LEN	MAX_PATH
#endif /* WIN32 */

#define NEWSRC_LINE	8192
#define MAXLINELEN	1024

#ifdef HAVE_MAIL_HANDLER	/* what is that? */
#	define HEADER_LEN	2048
#else
#	define HEADER_LEN	1024
#endif /* HAVE_MAIL_HANDLER */

#define MODULO_COUNT_NUM	50
#define TABLE_SIZE	1409
#define MAX_PAGES	2000	/* maximum article pages */
/* when prompting for subject, display no more than 20 characters */
#define DISPLAY_SUBJECT_LEN	20

#define ctrl(c)	((c) & 0x1F)

#ifndef DEFAULT_ISO2ASC
#	define DEFAULT_ISO2ASC	"-1 "	/* ISO -> Ascii charset conversion */
#endif /* !DEFAULT_ISO2ASC */

#ifndef DEFAULT_COMMENT
#	define DEFAULT_COMMENT	"> "	/* used when by follow-ups & replys */
#endif /* !DEFAULT_COMMENT */
#ifndef ART_MARK_UNREAD
#	define ART_MARK_UNREAD	'+'	/* used to show that an art is unread */
#endif /* !ART_MARK_UNREAD */
#ifndef ART_MARK_RETURN
#	define ART_MARK_RETURN	'-'	/* used to show that an art will return */
#endif /* !ART_MARK_RETURN */
#ifndef ART_MARK_SELECTED
#	define ART_MARK_SELECTED	'*'	/* used to show that an art was auto selected */
#endif /* !ART_MARK_SELECTED */
#ifndef ART_MARK_READ
#	define ART_MARK_READ	' '	/* used to show that an art was not read or seen */
#endif /* !ART_MARK_READ */
#ifndef ART_MARK_DELETED
#	define ART_MARK_DELETED	'D'	/* art has been marked for deletion (mailgroup) */
#endif /* !ART_MARK_DELETED */
#ifndef MARK_INRANGE
#	define MARK_INRANGE	'#'	/* group/art within a range (# command) */
#endif /* !MARK_INRANGE */

/*
 * position of the unread/will_return/hot-mark
 * (used in group.c/thread.c)
 */
#define MARK_OFFSET	9

#define SELECT_MISC_COLS	21
#ifdef USE_INVERSE_HACK
#	define BLANK_GROUP_COLS	2
#	define BLANK_PAGE_COLS	2
#else
#	define BLANK_GROUP_COLS	0
#	define BLANK_PAGE_COLS	0
#endif /* USE_INVERSE_HACK */

/*
 * Return values for tin_errno
 */
#define TIN_ABORT		1			/* User requested abort or timeout */
#define TIN_TIMEOUT		2			/* Client side timeout */

/*
 * Number of MIME Encodings
 */
#define NUM_MIME_ENCODINGS	4

#define MIME_ENCODING_8BIT	0
#define MIME_ENCODING_BASE64	1
#define MIME_ENCODING_QP	2
#define MIME_ENCODING_7BIT	3

/*
 * Number of charset-traslation tables (iso2asci)
 */
#define NUM_ISO_TABLES	7

/*
 * Maximum permissible colour number
 */
#define MAX_COLOR	15
#define MAX_BACKCOLOR	7

/*
 * Maximal permissible word mark type
 */
#define MAX_MARK		2

#define INDEX_TOP	2

#define GROUP_MATCH(s1, pat, case)		(wildmat (s1, pat, case))

#define REGEX_MATCH(s1, pat, case)	(wildcard_func (s1, pat, case))
#define REGEX_FMT (tinrc.wildcard ? "%s" : "*%s*")

#define IGNORE_ART(i)	((arts[i].killed) || (arts[i].thread == ART_EXPIRED))

/* TRUE if basenote has responses */
#define HAS_FOLLOWUPS(i)	(arts[base[i]].thread != -1)

/*
 * Only close off our stream when reading on local spool
 */
#ifdef NNTP_ABLE
#	define TIN_FCLOSE(x)	if (x != FAKE_NNTP_FP) fclose(x)
#else
#	define TIN_FCLOSE(x)	fclose(x)
#endif /* NNTP_ABLE */

/*
 * Often used macro to point to the group we are currenty in
 */
#define CURR_GROUP	(active[my_group[cur_groupnum]])

/*
 * Defines an unread group
 */
#define UNREAD_GROUP(i)		(!active[my_group[i]].bogus && active[my_group[i]].newsrc.num_unread > 0)
/*
 * Some informational message are only shown if we're running in
 * the background or some other non-curses mode
 */
#define INTERACTIVE	(!batch_mode || update_fork)
#define INTERACTIVE2	((cmd_line && !(batch_mode || verbose)) || (batch_mode && update_fork))

/*
 * News/Mail group types
 */
#define GROUP_TYPE_MAIL	0
#define GROUP_TYPE_NEWS	1
#define GROUP_TYPE_SAVE	2	/* saved news, read with tin -R */

/*
 * used by get_arrow_key()
 */
#ifdef WIN32
#	define KEYMAP_UNKNOWN		0
#	define KEYMAP_UP		0xA6
#	define KEYMAP_DOWN		0xA8
#	define KEYMAP_LEFT		0xA5
#	define KEYMAP_RIGHT		0xA7
#	define KEYMAP_PAGE_UP		0xA1
#	define KEYMAP_PAGE_DOWN	0xA2
#	define KEYMAP_HOME		0xA4
#	define KEYMAP_END		0xA3
#	define KEYMAP_DEL		0
#	define KEYMAP_INS		0
#	define KEYMAP_MOUSE		0
#else
#	define KEYMAP_UNKNOWN		0
#	define KEYMAP_UP		1
#	define KEYMAP_DOWN		2
#	define KEYMAP_LEFT		3
#	define KEYMAP_RIGHT		4
#	define KEYMAP_PAGE_UP		5
#	define KEYMAP_PAGE_DOWN	6
#	define KEYMAP_HOME		7
#	define KEYMAP_END		8
#	define KEYMAP_DEL		9
#	define KEYMAP_INS		10
#	define KEYMAP_MOUSE		11
#endif /* WIN32 */


/*
 * used in curses.c and signal.c
 *     it's useless trying to run tin below these sizes
 *     (values acquired by testing ;-) )
 */
#define MIN_LINES_ON_TERMINAL		 8
#define MIN_COLUMNS_ON_TERMINAL		50


/*
 * used by feed_articles() & show_mini_help()
 */
#define SELECT_LEVEL	1
#define GROUP_LEVEL	2
#define THREAD_LEVEL	3
#define PAGE_LEVEL	4

#define MINI_HELP_LINES		5

#define FEED_MAIL		1
#define FEED_PIPE		2
#define FEED_PRINT		3
#define FEED_SAVE		4
#define FEED_AUTOSAVE_TAGGED		5
#define FEED_REPOST		6

#if 0
#	define DEBUG_IO(x)	fprintf x
#else
#	define DEBUG_IO(x)	/* nothing */
#endif /* 0 */


/*
 * Threading strategies available
 * NB: The ordering is important in that threading methods that don't use
 *     references should be < THREAD_REFS
 */
#define THREAD_NONE		0
#define THREAD_SUBJ		1
#define THREAD_REFS		2
#define THREAD_BOTH		3

#define THREAD_MAX		THREAD_BOTH

/*
 * Values for show_author
 */
#define SHOW_FROM_NONE		0
#define SHOW_FROM_ADDR		1
#define SHOW_FROM_NAME		2
#define SHOW_FROM_BOTH		3

/*
 * used in feed.c & save.c
 */
#define POST_PROC_NONE		0
#define POST_PROC_SHAR		1
#define POST_PROC_UUDECODE	2
#define POST_PROC_UUD_LST_ZOO	3
#define POST_PROC_UUD_EXT_ZOO	4
#define POST_PROC_UUD_LST_ZIP	5
#define POST_PROC_UUD_EXT_ZIP	6


/*
 * used in art.c
 * sort types on arts[] array
 */
#define SORT_BY_NOTHING		0
#define SORT_BY_SUBJ_DESCEND	1
#define SORT_BY_SUBJ_ASCEND	2
#define SORT_BY_FROM_DESCEND	3
#define SORT_BY_FROM_ASCEND	4
#define SORT_BY_DATE_DESCEND	5
#define SORT_BY_DATE_ASCEND	6
#define SORT_BY_SCORE_DESCEND	7
#define SORT_BY_SCORE_ASCEND	8

/*
 * Search keys
 */
#define SEARCH_SUBJ		1
#define SEARCH_AUTH		2

/*
 * Different values of strip_bogus - the ways to handle bogus groups
 */
#define BOGUS_KEEP		0
#define BOGUS_REMOVE		1
#define BOGUS_ASK		2

/*
 * used in help.c
 */
#define HELP_INFO		0
#define POST_INFO		1


/*
 * used in save.c/main.c
 */
#define CHECK_ANY_NEWS		0
#define START_ANY_NEWS		1
#define MAIL_ANY_NEWS		2
#define SAVE_ANY_NEWS		3


/*
 * used in post.c
 */
#define HEADER_TO		0
#define HEADER_SUBJECT		1
#define HEADER_NEWSGROUPS	2

#define POSTED_NONE		0
#define POSTED_REDRAW		1
#define POSTED_OK		2


/*
 * index_point variable values used throughout tin
 */

/*
 * -1 is kind of overloaded as an error from which_thread() and other functions
 * where we wish to return to the next level up
 */
#define GRP_RETURN		-1	/* Stop reading group ('T' command) -> return to selection screen */
#define GRP_QUIT		-2	/* Set by 'Q' when coming all the way out */
#define GRP_NEXTUNREAD		-3	/* goto next unread group */
#define GRP_NEXT		-4	/* (catchup) & goto next group */
#define GRP_ARTFAIL		-5	/* show_page() only. Failed to get into the article */
#define GRP_KILLED		-6	/* Thread was killed at pager level */
#define GRP_GOTOTHREAD	-7	/* show_page() only. Enter thread menu */

#ifndef EXIT_SUCCESS
#	define EXIT_SUCCESS	0	/* Successful exit status */
#endif /* !EXIT_SUCCESS */

#ifndef EXIT_FAILURE
#	define EXIT_FAILURE	1	/* Failing exit status */
#endif /* !EXIT_FAILURE */

#define NNTP_ERROR_EXIT	2

/*
 * Assertion verifier
 */
#if !defined(M_OS2) && !defined(WIN32)
#	undef assert
#	ifdef CPP_DOES_EXPAND
#		define assert(p)	if(! (p)) asfail(__FILE__, __LINE__, #p); else (void)0;
#	else
#		define assert(p)	if(! (p)) asfail(__FILE__, __LINE__, "p"); else (void)0;
#	endif /* CPP_DOES_EXPAND */
#endif /*!M_OS2 && !WIN32*/

#define ESC	27


/*
 * return codes for change_config_file ()
 */
#define NO_FILTERING		0
#define FILTERING		1
#define DEFAULT_FILTER_DAYS		28


/*
 * art.thread (Can't ART_NORMAL be better named ?)
 */
#define ART_NORMAL		-1
#define ART_EXPIRED		-2

/*
 * art.status
 */
#define ART_READ		0
#define ART_UNREAD		1
#define ART_WILL_RETURN		2
#define ART_UNAVAILABLE		-1 /* Also used by msgid.article */

/*
 * Additionally used for user aborts in art_open()
 */
#define ART_ABORT		-2

/*
 * used by t_group & my_group[]
 */
#define UNSUBSCRIBED	'!'
#define SUBSCRIBED	':'

/* Converts subscription status to char for .newsrc */
#define SUB_CHAR(x)	(x ? SUBSCRIBED : UNSUBSCRIBED)
/* Converts .newsrc subscription char to boolean */
#define SUB_BOOL(x)	(x == SUBSCRIBED)

/*
 * filter_type used in struct t_filter
 */
#define FILTER_KILL		0
#define FILTER_SELECT		1

#define SCORE_MAX		10000

#define SCORE_DEFAULT		100
#define SCORE_KILL		-(SCORE_DEFAULT)
#define SCORE_SELECT		SCORE_DEFAULT

/* TODO: the next two should be configurable at runtime */
#define SCORE_LIM_KILL		-50
#define SCORE_LIM_SEL		50

#define FILTER_SUBJ_CASE_SENSITIVE		0
#define FILTER_SUBJ_CASE_IGNORE		1
#define FILTER_FROM_CASE_SENSITIVE		2
#define FILTER_FROM_CASE_IGNORE		3
#define FILTER_MSGID		4
#define FILTER_MSGID_LAST	5
#define FILTER_MSGID_ONLY	6
#define FILTER_REFS_ONLY	7
#define FILTER_LINES		8

#define FILTER_LINES_NO		0
#define FILTER_LINES_EQ		1
#define FILTER_LINES_LT		2
#define FILTER_LINES_GT		3

/*
 * used in checking article header before posting
 */
#define NGLIMIT		20	/* Max. num. of crossposted groups before warning */
#define MAX_COL		78	/* Max. line length before issuing a warning */
#define MAX_SIG_LINES	4	/* Max. num. of signature lines before warning */

/*
 * The following macros are used to simplify and speed up the
 * manipulation of the bitmaps in memory which record which articles
 * are read or unread in each news group.
 *
 * Data representation:
 *
 * Each bitmap is handled as an array of bytes; the least-significant
 * bit of the 0th byte is the 0th bit; the most significant bit of
 * the 0th byte is the 7th bit. Thus, the most-significant bit of the
 * 128th byte is the 1023rd bit, and in general the mth bit of the nth
 * byte is considered to be bit (n*8)+m of the map as a whole.	Conversely,
 * the position of bit q in the map is the bit (q & 7) of byte (q >> 3).
 * A bitmap of b bits will be allocated as ((b+7) >> 3) bytes.
 *
 * The routines could be changed to operate on a word-oriented bitmap by
 * changing the constants used from 8 to 16, 3 to 4, 7 to 15, etc. and
 * changing the allocate/deallocate routines.
 *
 * In the newsrc context, a 0 bit represents an article which is read
 * or expired; a 1 represents an unread article. The 0th bit corresponds
 * to the minimum article number for this group, and (max-min+7)/8 bytes
 * are allocated to the bitmap.
 *
 * Constants:
 *
 * NBITS   = total number of bits per byte;
 * NMAXBIT = number of bits per byte or word;
 * NBITPOS = number of bit in NMAXBIT;
 * NBITSON = byte/word used to set all bits in byte/word to 1;
 * NBITNEG1 = binary negation of 1, used in constructing masks.
 *
 * Macro naming and use:
 *
 * The NOFFSET and NBITIDX macro construct the byte and bit indexes in
 * the map, given a bit number.
 *
 * The NSET0 macro sets a bit to binary 0
 * The NSET1 macro sets a bit to binary 1
 * The NSETBLK0 macro sets the same bit or bits to binary 0
 * The NSETBLK1 macro sets the same bit or bits to binary 1
 * The NTEST macro tests a single bit.
 * These are used frequently to access the group bitmap.
 *
 * NSETBLK0 and NSETBLK1 operate on whole numbers of bytes, and are
 * mainly useful for initializing complete bitmaps to one state or
 * another. Both use the memset function, which is assumed to be
 * optimized for the target architecture. NSETBLK is currently used to
 * initialize the group bitmap to 1s (unread).
 *
 * NSETRNG0 and NSETRNG1 operate on ranges of bits, from a low bit number
 * to a high bit number (inclusive), and are especially useful for
 * efficiently setting a contiguous range of bits to one state or another.
 * NSETRNG0 is currently used on the group bitmap to mark the ranges the
 * newsrc file says are read or expired.
 *
 * The algorithm is this. If the high number is less than the low, then
 * do nothing (error); if both fall within the same byte, construct a
 * single mask expressing the range and AND or OR it into the byte; else:
 * construct a mask for the byte containing the low bit, AND or OR it in;
 * use memset to fill in the intervening bytes efficiently; then construct
 * a mask for the byte containing the high bit, and AND or OR this mask
 * in.	Masks are constructed by left-shift of 0xff (to set high-order bits
 * to 1), negating a left-shift of 0xfe (to set low-order bits to 1), and
 * the various negations and combinations of the same.	This procedure is
 * complex, but 1 to 2 orders of magnitude faster than a shift inside a
 * loop for each bit inside a loop for each individual byte.
 *
 */
#define NBITS		8
#define NMAXBIT	7
#define NBITPOS	3
#define NBITSON	0xff
#define NBITNEG1	0xfe
#define NOFFSET(b)	((b) >> NBITPOS)
#define NBITIDX(b)	((b) & NMAXBIT)

#define NBITMASK(beg,end)	(unsigned char) ~(((1 << (((NMAXBIT - beg) - (NMAXBIT - end)) + 1)) - 1) << (NMAXBIT - end))

#define NSET1(n,b)	(n[NOFFSET(b)] |=  (1 << NBITIDX(b)))
#define NSET0(n,b)	(n[NOFFSET(b)] &= ~(1 << NBITIDX(b)))
#define NTEST(n,b)	(n[NOFFSET(b)] &   (1 << NBITIDX(b)))

#define NSETBLK1(n,i)	(memset (n, NBITSON, (size_t) NOFFSET(i)+1))
#define NSETBLK0(n,i)	(memset (n, 0, (size_t) NOFFSET(i)+1))

typedef unsigned char	t_bitmap;

/*
 * Keys for add_msgid()
 */
#define REF_REF				1		/* Add a ref->ref entry */
#define MSGID_REF			2		/* Add a msgid->ref entry */

/*
 * Size of msgid hash table
 */
#define MSGID_HASH_SIZE		2609

/*
 * These will probably go away when filtering is rewritten
 * Easier access to hashed msgids. Note that in REFS(), y must be free()d
 * msgid is mandatory in an article and cannot be NULL
 */
#define MSGID(x)			(x->refptr->txt)
#define REFS(x,y)			((y = get_references(x->refptr->parent)) ? y : "")

/*
 *	struct t_msgid - message id
 */
struct t_msgid
{
	struct t_msgid *next;		/* Next in hash chain */
	struct t_msgid *parent;		/* Message-id followed up to */
	struct t_msgid *sibling;	/* Next followup to parent */
	struct t_msgid *child;		/* First followup to this article */
	int article;			/* index in arts[] or ART_NORMAL */
	char txt[1];			/* The actual msgid */
};

/*
 * struct t_article - article header
 *
 * article.thread:
 *	-1 (ART_NORMAL)  initial default
 *	-2 (ART_EXPIRED) article has expired (wasn't found in search of spool
 *	   directory for the group)
 *	>=0 points to another arts[] (struct t_article)
 *
 * article.inthread:
 *	FALSE for the first article in a thread, TRUE for all
 *	following articles in thread
 *
 */
struct t_article
{
	long artnum;			/* Article number in spool directory for group */
	char *subject;			/* Subject: line from mail header */
/* t_article.subject is casted to (int *) in art.c :-( */
	char *from;			/* From: line from mail header (address) */
	char *name;			/* From: line from mail header (full name) */
	int gnksa_code;			/* From: line from mail header (GNKSA error code) */
	time_t date;			/* Date: line from header in seconds */
	char *xref;			/* Xref: cross posted article reference line */
	/* NB: The msgid and refs are only retained until the reference tree is built */
	char *msgid;			/* Message-ID: unique message identifier */
	char *refs;			/* References: article reference id's */
	struct t_msgid *refptr;		/* Pointer to us in the reference tree */
	int lines;			/* Lines: number of lines in article */
	char *archive;			/* Archive-name: line from mail header */
	char *part;			/* part no. of archive */
	char *patch;			/* patch no. of archive */
	int tagged;			/* 0 = not tagged, >0 = tagged */
	int thread;
	int score;			/* score article has reached after filtering */
	unsigned int inthread:1;	/* 0 = thread head, 1 = thread follower */
	unsigned int status:2;		/* 0 = read, 1 = unread, 2 = will return */
	unsigned int killed:1;		/* 0 = not killed, 1 = killed */
	unsigned int selected:1;	/* 0 = not selected, 1 = selected */
	unsigned int zombie:1;		/* 1 = was alive (unread) before 'X' command */
	unsigned int delete_it:1;	/* 1 = delete art when leaving group [mail group] */
	unsigned int inrange:1;		/* 1 = article selected via # range command */
};

/*
 * struct t_attribute - configurable attributes on a per group basis
 */
struct t_attribute
{
	char *maildir;				/* mail dir if other than ~/Mail */
	char *savedir;				/* save dir if other than ~/News */
	char *savefile;			/* save articles to specified file */
	char *sigfile;				/* sig file if other than ~/.Sig */
	char *organization;			/* organization name */
	char *followup_to;			/* where posts should be redirected */
	char *printer;				/* printer command & parameters */
	char *quick_kill_scope; 		/* quick filter kill scope */
	char *quick_select_scope;		/* quick filter select scope */
	char *mailing_list;			/* mail list email address */
	char *x_headers;			/* extra headers for message header */
	char *x_body;				/* bolierplate text for message body */
	char *from;				/* from line */
	unsigned global:1;			/* global/group specific */
	unsigned quick_kill_header:3;		/* quick filter kill header */
	unsigned quick_kill_expire:1;		/* quick filter kill limited/unlimited time */
	unsigned quick_kill_case:1;		/* quick filter kill case sensitive? */
	unsigned quick_select_header:3;		/* quick filter select header */
	unsigned quick_select_expire:1;		/* quick filter select limited/unlimited time */
	unsigned quick_select_case:1;		/* quick filter select case sensitive? */
	unsigned auto_select:1;			/* 0=show all unread, 1='X' just hot arts */
	unsigned auto_save:1;			/* 0=none, 1=save */
	unsigned batch_save:1;			/* 0=none, 1=save -S/mail -M */
	unsigned delete_tmp_files:1;		/* 0=leave, 1=delete */
	unsigned show_only_unread:1;		/* 0=all, 1=only unread */
	unsigned thread_arts:2;			/* 0=unthread, 1=subject, 2=refs, 3=both */
	unsigned show_author:4;			/* 0=none, 1=name, 2=addr, 3=both */
	unsigned sort_art_type:4;		/* 0=none, 1=subj descend, 2=subj ascend,
						   3=from descend, 4=from ascend,
						   5=date descend, 6=date ascend,
						   7=score descend, 8=score ascend */
	unsigned int post_proc_type:4;		/* 0=none, 1=shar, 2=uudecode,
						   3=uud & list zoo, 4=uud & ext zoo*/
	unsigned int x_comment_to:1;		/* insert X-Comment-To: in Followup */
	char *news_quote_format;		/* another way to begin a posting format */
	char *quote_chars;			/* string to precede quoted text on each line */
};

/*
 * struct t_newsrc - newsrc related info.
 */
struct t_newsrc
{
	t_bool present;		/* update newsrc ? */
	long num_unread;		/* unread articles in group */
	long xmax;				/* newsrc max */
	long xmin;				/* newsrc min */
	long xbitlen;			/* bitmap length (max-min+1) */
	t_bitmap *xbitmap;	/* bitmap read/unread (max-min+1+7)/8 */
};

/*
 * struct t_group - newsgroup info from active file
 */
struct t_group
{
	char *name;				/* newsgroup / mailbox name */
	char *aliasedto;		/* =new.group in active file, NULL if not */
	char *description;	/* text from NEWSLIBDIR/newsgroups file */
	char *spooldir;		/* groups spool directory */
	char moderated;		/* state of group moderation */
	long count;				/* article number count */
	long xmax;				/* max. article number */
	long xmin;				/* min. article number */
	unsigned int type:4;			/* grouptype - newsgroup / mailbox / savebox */
	unsigned int inrange:4;			/* 1 = group selected via # range command */
	t_bool read_during_session:1;		/* TRUE if group entered during session */
	t_bool art_was_posted:1;		/* TRUE if art was posted to group */
	t_bool subscribed:1;			/* TRUE if subscribed to group */
	t_bool newgroup:1;				/* TRUE if group was new this session */
	t_bool bogus:1;					/* TRUE if group is not in active list */
	int next;				/* next active entry in hash chain */
	struct t_newsrc newsrc;				/* newsrc bitmap specific info. */
	struct t_attribute *attribute;	/* group specific attributes */
	struct t_filters *glob_filter;	/* points to filter array */
#ifdef INDEX_DAEMON
	time_t last_updated_time;		/* last time group dir was changed */
#endif /* INDEX_DAEMON */
};

/*
 * used in hashstr.c
 */
struct t_hashnode
{
	struct t_hashnode *next;		/* chain for spillover */
	int aptr;				/* used in subject threading */
	char txt[1];			/* stub for the string data, \0 terminated */
};

/*
 * used in filter.c
 *
 *  Create 2 filter arrays - global & local. Local will be part of group_t
 *  structure and will have priority over global filter. Should help to
 *  speed kill/selecting within a group. The long value number that is in
 *  ~/.tin/kill will be replaced by group name so that it is more human
 *  readable and that if hash routine is changed it will still work.
 *
 *  Add time period to filter_t struct to allow timed kills & auto-selection
 *  Default kill & select time 28 days. Store as a long and compare when
 *  loading against present time. If time secs is passed set flag to save
 *  filter file and don't load expired entry. Renamed to filter because of
 *  future directions in adding other retrieval methods to present kill &
 *  auto selection.
 *
 *  Also seperate kill/select screen to allow ^K=kill ^A=auto-select
 */
struct t_filters
{
	int max;
	int num;
	struct t_filter *filter;
};

/*
 * struct t_filter - local & global filtering (ie. kill & auto-selection)
 */
struct t_filter
{
	char *scope;			/* NULL='*' (all groups) or 'comp.os.*' */
	char *subj;			/* Subject: line */
	char *from;			/* From: line */
	char *msgid;			/* Message-ID: line */
	char lines_cmp;			/* Lines compare <> */
	int  lines_num;			/* Lines: line */
	char gnksa_cmp;			/* GNKSA compare <> */
	int  gnksa_num;			/* GNKSA code */
	int  score;			/* score to give if rule matches */
	char *xref;			/* groups in xref line */
	int xref_max;			/* maximal number of groups in newsgroups line */
	int xref_score_cnt;
	int xref_scores[10];
	char *xref_score_strings[10];
	time_t time;			/* expire time in seconds */
	struct t_filter *next;		/* next rule valid in group */
	unsigned int inscope:4;		/* if group matches scope ie. 'comp.os.*' */
	unsigned int type:2;		/* kill/auto select */
	unsigned int icase:2;		/* Case sensitive filtering */
	unsigned int fullref:4;		/* use full references or last entry only */
};

/*
 * struct t_filter_rule - provides parameters to build filter rule from
 */
struct t_filter_rule
{
	char text[PATH_LEN];
	char scope[PATH_LEN];
	int counter;
	int icase;
	int fullref;
	int lines_cmp;
	int lines_num;
	t_bool from_ok:1;
	t_bool lines_ok:1;
	t_bool msgid_ok:1;
	t_bool subj_ok:1;
	t_bool check_string:1;
	int type;
	int score;
	int expire_time;
};

struct t_header_list
{
	char header[HEADER_LEN];
	char content[HEADER_LEN];
	struct t_header_list *next;
};

struct t_header
{
	char from[HEADER_LEN];		/* From: */
	char path[HEADER_LEN];		/* Path: */
	char date[HEADER_LEN];		/* Date: */
	char subj[HEADER_LEN];		/* Subject: */
	char org[HEADER_LEN];		/* Organization: */
	char newsgroups[HEADER_LEN];	/* Newsgroups: */
	char messageid[HEADER_LEN];	/* Message-ID: */
	char references[HEADER_LEN];	/* References: */
	char distrib[HEADER_LEN];	/* Distribution: */
	char keywords[HEADER_LEN];	/* Keywords: */
	char summary[HEADER_LEN];	/* Summary: */
	char followup[HEADER_LEN];	/* Followup-To: */
	char mimeversion[HEADER_LEN];	/* Mime-Version: */
	char contenttype[HEADER_LEN];	/* Content-Type: */
	char contentenc[HEADER_LEN];	/* Content-Transfer-Encoding: */
	char ftnto[HEADER_LEN];		/* Old X-Comment-To: (Used by FIDO) */
	char authorids[HEADER_LEN];	/* Author-IDs: (USEFOR, 2nd Son of 1036) */
	struct t_header_list *persist;	/* P-ersistent headers (USEFOR, 2nd Son of 1036) */
};

struct t_save
{
	char *subject;
	char *dir;
	char *file;
	char *archive;
	char *part;
	char *patch;
	int index;
	int saved;
	t_bool is_mailbox;
};

struct t_screen
{
	char *col;
};

struct t_posted
{
	char date[10];
	char group[80];
	char action;
	char subj[120];
};

struct t_art_stat
{
	int total;		/* total article count */
	int unread;		/* number of unread articles (does not include seen) arts */
	int seen;		/* number of seen articles (ART_WILL_RETURN) */
	int deleted;		/* number of articles marked for deletion (mailgroups) */
	int inrange;		/* number of articles marked in a range */
	int selected_total;	/* total selected count */
	int selected_unread;	/* selected and unread */
	int selected_seen;	/* selected and seen */
	char art_mark;		/* mark to use for this thread - not used for groups */
	int score;		/* maximum score */
};


/*
 * Used for detecting changes in active file size on different news servers
 */
struct t_newnews
{
	char *host;
	time_t time;
};

#ifdef M_AMIGA
	typedef const char /*__far*/ constext;
#else
	typedef const char constext;
#endif /* M_AMIGA */

/*
 * Used for building option menu
 */
struct t_option {
	int var_type;		/* type of variable (see tincfg.h) */
	int var_index;		/* index in corresponding table */
	int *variable;		/* ptr to variable to change */
	constext **opt_list;	/* ptr to list entries if OPT_LIST */
	int opt_count;		/* no. of list entries if OPT_LIST */
	constext *option_text;	/* text to print as information on option */
	constext *help_text;	/* text to print as help text when option selected */
};

/*
 * Time functions.
 */
typedef struct _TIMEINFO
{
	time_t	time;
	long	usec;
	long	tzone;
} TIMEINFO;

#if 0		/* Does anyone know what this was going to do ? */
/*
 * Used for detecting new groups when reading news locally. It's easy to be
 * confused by arrays of pointers to pointers, so typedef's are used for the
 * first level pointers to keep it clearer.
 */
struct t_notify
{
	char *name;
	int visited;
};

typedef struct t_group *group_p;
typedef struct t_notify *notify_p;
#endif /* 0 */

/*
 * Determine signal return type
 */
#ifndef RETSIGTYPE
#	define RETSIGTYPE void
#endif /* !RETSIGTYPE */

/*
 * Determine qsort compare type
 */
#ifdef HAVE_COMPTYPE_VOID
#	ifdef __STDC__
		typedef const void t_comptype;
#	else
		typedef void t_comptype;
#	endif /* __STDC__ */
#endif /* HAVE_COMPTYPE_VOID */

#ifdef HAVE_COMPTYPE_CHAR
	typedef char t_comptype;
#endif /* HAVE_COMPTYPE_CHAR */

#ifdef M_OS2
#	define _CDECL	_cdecl
#	define _FAR_	_far16
#	define SEPDIR	'\\'
#else
#	define _CDECL
#	define _FAR_
#	define SEPDIR	'/'
#endif /* M_OS2 */

/*
 * mouse buttons for use in xterm
 */
#define MOUSE_BUTTON_1		0
#define MOUSE_BUTTON_2		1
#define MOUSE_BUTTON_3		2

#define TIN_EDITOR_FMT_OFF		"%E %F"

#ifdef M_AMIGA
#	define NEWSGROUPS_FILE	"newsdescrip"
#	define REDIRECT_OUTPUT	"> NIL:"
#	define REDIRECT_PGP_OUTPUT	"> NIL:"
#	define ENV_VAR_GROUPS		"TIN_GROUPS"
#	define ENV_VAR_MAILER		"TIN_MAIL"
#	define ENV_VAR_POSTER		"TIN_POST"
#	define ENV_VAR_SHELL		"SHELL"
#	define TIN_EDITOR_FMT_ON	"%E %F"
#	define MAILER_FORMAT		"%M <%F -f %U"
#	define METAMAIL_CMD		"%s -e -p -m \"tin\""
#	define TMPDIR			"T:"
#	define KEY_PREFIX		0x9b
extern void joinpath (char *result, const char *dir, const char *file);
#endif /* M_AMIGA */
#ifdef VMS
#	define NEWSGROUPS_FILE	"newsgroups"
#	define REDIRECT_OUTPUT	""
#	define REDIRECT_PGP_OUTPUT	""
#	define ENV_VAR_MAILER		"TIN_MAILER"
/*#	define ENV_VAR_SHELL		"SHELL"*/
#	define ENV_VAR_POSTER		"TIN_POST"
#	define TIN_EDITOR_FMT_ON	"%E %F"
#	define METAMAIL_CMD		"%s -e -p -m \"tin\""
#	define TMPDIR "SYS$SCRATCH:"
#	ifdef	HAVE_KEY_PREFIX
#		define KEY_PREFIX	0x9b
#	endif /* HAVE_KEY_PREFIX */
extern void joinpath (char *result, const char *dir, const char *file);
extern void joindir (char *result, const char *dir, const char *file);
#endif /* VMS */

#ifdef M_OS2
#	define NEWSGROUPS_FILE	"newsgroups"
#	define REDIRECT_OUTPUT	"> NUL"
#	define REDIRECT_PGP_OUTPUT	"> NUL"
#	define ENV_VAR_GROUPS		"TIN_GROUPS"
#	define ENV_VAR_MAILER		"TIN_MAIL"
#	define ENV_VAR_POSTER		"TIN_POST"
#	define ENV_VAR_SHELL		"COMSPEC"
#	define TIN_EDITOR_FMT_ON	"%E %F"
#	define METAMAIL_CMD		"%s -e -p -m \"tin\""
extern void joinpath (char *result, char *dir, char *file);
#endif /* M_OS2 */

#ifdef WIN32
#	define NEWSGROUPS_FILE	"newsgroups"
#	define REDIRECT_OUTPUT	"> NUL"
#	define REDIRECT_PGP_OUTPUT "> NUL"
#	define ENV_VAR_GROUPS		"TIN_GROUPS"
#	define ENV_VAR_MAILER		"TIN_MAIL"
#	define ENV_VAR_POSTER		"TIN_POST"
#	define ENV_VAR_SHELL		"COMSPEC"
#	define TIN_EDITOR_FMT_ON	"%E +%N %F"
#	define MAILER_FORMAT		"%M -t %T -f %U -s \"%S\" -F %F"
#	define METAMAIL_CMD		"%s -e -p -m \"tin\""
extern void joinpath (char *result, char *dir, char *file);
#endif /* WIN32 */

#ifdef M_UNIX
#	define NEWSGROUPS_FILE		"newsgroups"
#	define REDIRECT_OUTPUT		"> /dev/null 2>&1"
#	define REDIRECT_PGP_OUTPUT		"> /dev/null"
#	define ENV_VAR_MAILER		"MAILER"
#	define ENV_VAR_SHELL		"SHELL"
#	define TIN_EDITOR_FMT_ON		"%E +%N %F"
#	define MAILER_FORMAT		"%M -t < %F"
#	define METAMAIL_CMD		"%s -e -p -m \"tin\""
#	define TMPDIR		"/tmp/"
#	ifdef	HAVE_KEY_PREFIX
#		define KEY_PREFIX		0x8f: case 0x9b
#	endif /* HAVE_KEY_PREFIX */
#endif /* M_UNIX */

/* fallback values */
/* FIXME! */
#ifndef NEWSGROUPS_FILE
#	define NEWSGROUPS_FILE		""
#endif /* !NEWSGROUPS_FILE */
#ifndef REDIRECT_OUTPUT
#	define REDIRECT_OUTPUT		""
#endif /* !REDIRECT_OUTPUT */
#ifndef REDIRECT_PGP_OUTPUT
#	define REDIRECT_PGP_OUTPUT		""
#endif /* !REDIRECT_PGP_OUTPUT */
#ifndef ENV_VAR_MAILER
#	define ENV_VAR_MAILER		""
#endif /* !ENV_VAR_MAILER */
#ifndef ENV_VAR_SHELL
#	define ENV_VAR_SHELL		""
#endif /* !ENV_VAR_SHELL */
#ifndef TIN_EDITOR_FMT_ON
#	define TIN_EDITOR_FMT_ON		""
#endif /* !TIN_EDITOR_FMT_ON */
#ifndef MAILER_FORMAT
#	define MAILER_FORMAT		""
#endif /* !MAILER_FORMAT */
#ifndef METAMAIL_CMD
#	define METAMAIL_CMD		""
#endif /* !METAMAIL_CMD */
#ifndef TMPDIR
#	define TMPDIR		""
#endif /* !TMPDIR */

#if !defined(S_ISDIR)
#	if defined(WIN32)
#		define S_ISDIR(m)	((m) & _S_IFDIR)
#	else
#		if defined(M_OS2)
#			define S_ISDIR(m)	((m) & S_IF_DIR)
#		endif /* M_OS2 */
#	endif /* WIN32 */
#	if defined(M_UNIX) || defined(VMS) || defined(M_AMIGA)
#		define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#	endif /* M_UNIX || VMS || M_AMIGA */
#endif /* !S_ISDIR */

#if !defined(S_ISREG)
#	define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#endif /* !S_ISREG */

#ifndef S_IRWXU /* should be defined in <sys/stat.h> */
#	define S_IRWXU	0000700	/* read, write, execute permission (owner) */
#	define S_IRUSR	0000400	/* read permission (owner) */
#	define S_IWUSR	0000200	/* write permission (owner) */
#	define S_IXUSR	0000100	/* execute permission (owner) */

#	define S_IRWXG	0000070	/* read, write, execute permission (group) */
#	define S_IRGRP	0000040	/* read permission (group) */
#	define S_IWGRP	0000020	/* write permission (group) */
#	define S_IXGRP	0000010	/* execute permission (group) */

#	define S_IRWXO	0000007	/* read, write, execute permission (other) */
#	define S_IROTH	0000004	/* read permission (other) */
#	define S_IWOTH	0000002	/* write permission (other) */
#	define S_IXOTH	0000001	/* execute permission (other) */
#endif /* !S_IRWXU */

#ifndef S_IRWXUGO
#	define S_IRWXUGO	(S_IRWXU|S_IRWXG|S_IRWXO)	/* read, write, execute permission (all) */
#	define S_IRUGO	(S_IRUSR|S_IRGRP|S_IROTH)	/* read permission (all) */
#	define S_IWUGO	(S_IWUSR|S_IWGRP|S_IWOTH)	/* write permission (all) */
#	define S_IXUGO	(S_IXUSR|S_IXGRP|S_IXOTH)	/* execute permission (all) */
#endif /* !S_IRWXUGO */

#ifdef DONT_HAVE_PIPING
#	ifdef VMS
#		define TIN_PRINTFILE "Sys$Scratch:TIN_PRINT%d.TMP"
#	else
#		define TIN_PRINTFILE "tinprint%d.tmp"
#	endif /* VMS */
#endif /* DONT_HAVE_PIPING */

/*
 * Defines for access()
 */
#ifndef R_OK
#	define R_OK	4	/* Test for Read permission */
#endif /* !R_OK */
#ifndef W_OK
#	define W_OK	2	/* Test for Write permission */
#endif /* !W_OK */
#ifndef X_OK
#	define X_OK	1	/* Test for eXecute permission */
#endif /* !X_OK */
#ifndef F_OK
#	define F_OK	0	/* Test for existence of File */
#endif /* !F_OK */

#ifdef USE_DBMALLOC
#	define my_malloc(size)	malloc(size)
#	define my_realloc(ptr, size)	realloc((ptr), (size))
#else
#	define my_malloc(size)	my_malloc1(__FILE__, __LINE__, (size))
#	define my_realloc(ptr, size)	my_realloc1(__FILE__, __LINE__, (ptr), (size))
#endif /* USE_DBMALLOC */

#define SIZEOF(array)	((int)(sizeof array / sizeof array[0]))

#define FreeIfNeeded(p)	if (p != (char *)0) free((char *)p)
#define FreeAndNull(p)	if (p != (char *)0) { free((char *)p); p = (char *)0; }

#define my_group_find(x)	add_my_group(x, FALSE)
#define my_group_add(x)	add_my_group(x, TRUE)

/*
 * Cast for the (few!) places where we need to examine 8-bit characters w/o
 * sign-extension, and a corresponding test-macro.
 */
#define EIGHT_BIT(ptr)	(unsigned char *)ptr
#define is_EIGHT_BIT(p)	((*EIGHT_BIT(p) < 32 && !isspace((int)*p)) || *EIGHT_BIT(p) > 127)

/*
 * function prototypes & extern definitions
 */
/* #include	"filebug.h" */

#ifndef SIG_ARGS
#	if defined(__STDC__)
#		define SIG_ARGS	int sig
#	endif /* __STDC__ */
#endif /* !SIG_ARGS */

/* stifle complaints about not-a-prototype from gcc */
#ifdef DECL_SIG_CONST
#	undef	SIG_DFL
#	define SIG_DFL	(void (*)(SIG_ARGS))0
#	undef	SIG_IGN
#	define SIG_IGN	(void (*)(SIG_ARGS))1
#	undef	SIG_ERR
#	define SIG_ERR	(void (*)(SIG_ARGS))-1
#endif /* DECL_SIG_CONST */

/*
 * tputs() function-param
 */
#ifdef OUTC_RETURN
#	define OUTC_RETTYPE	int
#else
#	define OUTC_RETTYPE	void
#endif /* OUTC_RETURN */

#ifndef OUTC_ARGS
#	define OUTC_ARGS	int c
#endif /* !OUTC_ARGS */

#if __STDC__ || defined(__cplusplus)
#	define OUTC_FUNCTION(func)	OUTC_RETTYPE func (OUTC_ARGS)
#else
#	define OUTC_FUNCTION(func)	OUTC_RETTYPE func (c) int c;
#endif /* __STDC__ || __cplusplus */

typedef	OUTC_RETTYPE (*OutcPtr) (OUTC_ARGS);

#ifdef M_AMIGA
	typedef	struct __tcpbuf TCP;
#	include "amigatcp.h"
#else
	typedef	FILE	TCP;
#endif /* M_AMIGA */

#include	"extern.h"
#include	"tinrc.h"
#include	"nntplib.h"

#ifndef __CPROTO__
#	include	"proto.h"
#endif /* !__CPROTO__ */

#if defined(WIN32)
#	include	"msmail.h"
#endif /* WIN32 */

/* Philip Hazel's Perl regular expressions library */
#include	<pcre.h>

/*
 * rfc1521/rfc1522 interface
 */
typedef void (*BodyPtr) (char *, FILE *, int);

#ifdef USE_DBMALLOC
#	undef strchr
#	undef strrchr
#	undef NSET1
#	undef NSET0
#	define NSET1(n,b) memset(n+NOFFSET(b), n[NOFFSET(b)] | NTEST(n,b), 1)
#	define NSET0(n,b) memset(n+NOFFSET(b), n[NOFFSET(b)] & ~NTEST(n,b), 1)
#	include <dbmalloc.h> /* dbmalloc 1.4 */
#endif /* USE_DBMALLOC */

#ifdef USE_DMALLOC
#	include <dmalloc.h>
#	define DMALLOC_FUNC_CHECK
#endif /* USE_DMALLOC */

#if defined(WIN32) && defined(DEBUG) && defined(CHECKHEAP)
#	undef malloc
#	undef realloc
#	undef calloc
#	undef free
#	define malloc(cb)		mymalloc(cb, __LINE__, __FILE__)
#	define realloc(pv,cb)		myrealloc(pv, cb, __LINE__, __FILE__)
#	define calloc(cbs,cbe)		mycalloc(cbs, cbe, __LINE__, __FILE__)
#	define free(pv)		myfree(pv, __LINE__, __FILE__)
#endif /* WIN32 && DEBUG && CHECKHEAP */

#ifdef DOALLOC
	extern char *doalloc (char *, size_t);
	extern char *docalloc (size_t, size_t);
	extern void	dofree (char *);
#	undef malloc
#	undef realloc
#	undef calloc
#	undef free
#	define malloc(n)		doalloc((char *)0, n)
#	define realloc		doalloc
#	define calloc		docalloc
#	define free		dofree
	extern void	fail_alloc ( char *, char * );
	extern void	Trace ( char *, ... );
	extern void	Elapsed ( char * );
	extern void	WalkBack ( void );
	extern void	show_alloc ( void );
	extern void	no_leaks ( void );
#endif /* DOALLOC */

#ifdef __DECC		/* VMS */
#	ifndef ferror
#		define ferror(x)		(0)
#		define EndWin		EndWind
#	endif /* !ferror */
#endif /* __DECC */

#define IS_PLURAL(x)	(x != 1 ? txt_plural : "")

/* FIXME - check also for mktemp/mkstemp/tmpfile */
#ifdef HAVE_TEMPNAM
#	define my_tempnam(a,b)	tempnam(a,b)
#else
#	ifdef HAVE_TMPNAM
#		define my_tempnam(a,b)	tmpnam((char *)0)
#	endif /* HAVE_TMPNAM */
#endif /* HAVE_TEMPNAM */

/* define some standard places to look for a tin.defaults file */
#define TIN_DEFAULTS_BUILTIN "/etc/opt/tin","/etc/tin","/etc","/usr/local/lib/tin","/usr/local/lib","/usr/local/etc/tin","/usr/local/etc","/usr/lib/tin","/usr/lib",NULL
#ifdef TIN_DEFAULTS_DIR
#	define TIN_DEFAULTS TIN_DEFAULTS_DIR,TIN_DEFAULTS_BUILTIN
#else
#	define TIN_DEFAULTS TIN_DEFAULTS_BUILTIN
#endif /* TIN_DEFAULTS_DIR */

/*
 * We force this include-ordering since socks.h contains redefinitions of
 * functions that probably are prototyped via other includes.  The socks.h
 * definitions have to be included everywhere, since they're making wrappers
 * for the stdio functions as well as the network functions.
 */
#ifdef USE_SOCKS5
#	define SOCKS
#	include	<socks.h>
/* socks.h doesn't define prototypes for use */
extern size_t read(int, char *, size_t);
extern int dup(int);
extern int close(int);
extern int fprintf(FILE *, const char *, ...);
extern int fclose(FILE *);
extern struct tm *localtime(time_t *);
#endif /* USE_SOCKS5 */

#ifdef SETVBUF_REVERSED
#	define SETVBUF(stream, buf, mode, size)	setvbuf(stream, mode, buf, size)
#else
#	define SETVBUF(stream, buf, mode, size)	setvbuf(stream, buf, mode, size)
#endif /* SETVBUF_REVERSED */

#ifdef CLOSEDIR_VOID
#	define CLOSEDIR(DIR)	closedir(DIR)
#else
#	define CLOSEDIR(DIR)	if (closedir(DIR)) error_message("closedir() failed: %s %s", __FILE__, __LINE__)
#endif /* CLOSEDIR_VOID */

#endif /* !TIN_H */
