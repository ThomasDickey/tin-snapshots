/*
 *  Project   : tin - a Usenet reader
 *  Module    : tin.h
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 22-08-95
 *  Notes     : #include files, #defines & struct's
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *		You may  freely  copy or  redistribute	this software,
 *		so  long as there is no profit made from its use, sale
 *		trade or  reproduction.  You may not change this copy-
 *		right notice, and it must be included in any copy made
 */

/*
 * OS specific doda's
 */

#ifdef HAVE_CONFIG_H
#include	<autoconf.h>	/* FIXME: normally we use 'config.h' */
#else
#include	"config.h"
#endif

#ifdef __DECC
#include	<unixio.h>
#else
#include	<stdio.h>
#endif
#include	<signal.h>
#include	<errno.h>

#ifdef VMS
#define NNTP_ONLY
#define NNTP_INEWS
#define DONT_LOG_USER
#define NO_PIPING
#define DONT_HAVE_PIPING
#define NO_SHELL_ESCAPE
#define USE_CLEARSCREEN
extern char *get_uaf_fullname();

#ifdef MULTINET
#include "MULTINET_ROOT:[MULTINET.INCLUDE.SYS]TYPES.H"
#include "MULTINET_ROOT:[MULTINET.INCLUDE.SYS]TIME.H"
#else
#include "select.h"
#ifdef VAXC
#include "vmstimval.h"
#endif
#include <types.h>
#endif /* !MULTINET */
#include <stat.h>
#endif

#ifdef HAVE_STDDEF_H
#	include <stddef.h>
#else
#	include <sys/types.h>
#endif
#include <sys/stat.h>

#ifdef TIME_WITH_SYS_TIME
#	include <sys/time.h>
#	include <time.h>
#else
#	ifdef HAVE_SYS_TIME_H
#		include <sys/time.h>
#	else
#		include <time.h>
#	endif
#endif

#ifdef HAVE_SYS_TIMES_H
#	include <sys/times.h>
#endif

#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif
#ifdef HAVE_PWD_H
#	include <pwd.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#	include <sys/param.h>
#endif

#include	<ctype.h>

#ifdef HAVE_DBMALLOC
#	include "dbmalloc.h"
#else
#	ifdef HAVE_STDLIB_H
#		include <stdlib.h>
#	endif
#endif

	/* prefer string.h because it's Posix */
#ifdef HAVE_STRING_H
#	include <string.h>
#else
#	ifdef HAVE_STRINGS_H
#		include <strings.h>
#	endif
#endif

#ifdef HAVE_FCNTL_H
#	include <fcntl.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#	include <sys/ioctl.h>
#endif

#ifdef HAVE_PROTOTYPES_H
#	include <prototypes.h>
#endif

#ifdef HAVE_LOCALE_H
#	include <locale.h>
#endif

#ifdef HAVE_SYS_UTSNAME_H
#	include <sys/utsname.h>
#endif

/*
 * Needed for catching child processes
 */

#ifdef HAVE_SYS_WAIT_H
#	include <sys/wait.h>
#endif

/*
 * Needed for timeout in user abort of indexing a group (BSD & SYSV variaties)
 */

#ifdef HAVE_SYS_SELECT_H
#	if SYSTEM_LOOKS_LIKE_SCO
#		define timeval fake_timeval
#	endif
#	include <sys/select.h>
#	if SYSTEM_LOOKS_LIKE_SCO
#		undef timeval
#	endif
#endif

#ifdef HAVE_STROPTS_H
#	include <stropts.h>
#endif

#ifdef HAVE_POLL_H
#	include <poll.h>
#endif

/*
 * Needed for resizing under an xterm
 */

#ifdef HAVE_TERMIOS_H
#	include <termios.h>
#endif

#if SYSTEM_LOOKS_LIKE_SCO
#	include <sys/stream.h>
#	include <sys/ptem.h>
#endif

#if defined(SIGWINCH) && !defined(DONT_HAVE_SIGWINCH)
#	if !defined(TIOCGWINSZ) && !defined(TIOCGSIZE)
#		ifdef HAVE_SYS_STREAM_H
#			include <sys/stream.h>
#		endif
#		ifdef HAVE_TERMIO_H
#			include <termio.h>
#		else
#			ifdef HAVE_SYS_PTEM_H
#				include <sys/ptem.h>
#				include <sys/tty.h>
#			endif
#			ifdef HAVE_SYS_PTY_H
#				if !defined(_h_BSDTYPES) && defined(HAVE_SYS_BSDTYPES_H)
#					include <sys/bsdtypes.h>
#				endif
#				include <sys/pty.h>
#			endif
#		endif
#	endif
#endif

#ifdef HAVE_CURSES_H
#	include <curses.h>
#endif

/*
 * Directory handling code
 */

#ifdef HAVE_CONFIG_H
#if HAVE_DIRENT_H
#	include <dirent.h>
#	define	DIR_BUF struct dirent
#	define	D_NAMLEN(p)	(p)->d_reclen
#else
#	if HAVE_SYS_DIR_H
#		include <sys/dir.h>
#	endif
#	if HAVE_SYS_NDIR_H
#		include <sys/ndir.h>
#	endif
#	define	DIR_BUF struct direct
#	define	D_NAMLEN(p)	(p)->d_namlen
#endif

#else	/* FIXME: most of the rest of this isn't necessary with autoconf */

#if defined(BSD) && !defined(__386BSD__) && !defined(M_OS2)
#	ifdef sinix
#		include <dir.h>
#	else
#		if defined(__arm) || defined(__osf__)
#			include <dirent.h>
#			define	DIR_BUF struct dirent
#		else
#			include <sys/dir.h>
#		endif
#	endif
#	ifndef DIR_BUF
#		define	DIR_BUF 	struct direct
#	endif
#	define		D_NAMLEN(p)	(p)->d_namlen
#endif
#ifdef M_AMIGA
#	include "amiga.h"
#	define		DIR_BUF 	struct dirent
#	define		D_NAMLEN(p)	(p)->d_reclen
#endif
#ifdef M_OS2
#	include "os_2.h"
#	define		DIR_BUF 	struct dirent
#	define		D_NAMLEN(p)	(p)->d_reclen
#endif
#ifdef WIN32
#	include "win32.h"
#	define		DIR_BUF 	struct direct
#	define		D_NAMLEN(p)	(p)->d_namlen
#endif
#ifdef M_XENIX
#	include <sys/ndir.h>
#	define		DIR_BUF 	struct direct
#	define		D_NAMLEN(p)	(p)->d_namlen
#endif
#ifdef VMS
#include "ndir.h"
#define 	DIR_BUF 	struct direct
#define 	D_NAMLEN(p)	(p)->d_namlen
#endif
#endif	/* !HAVE_CONFIG_H */

#ifndef DIR_BUF
#	include <dirent.h>
#	define		DIR_BUF 	struct dirent
#	define		D_NAMLEN(p)	(p)->d_reclen
#endif

#ifndef HAVE_UNLINK
#	define unlink(file) remove(file)
#endif

/*
 * If native OS has'nt defined STDIN_FILENO be a smartass and do it
 */

#if !defined(STDIN_FILENO)
#	define	STDIN_FILENO	0
#endif

/*
 * Setup support for reading from NNTP
 */

#if defined(NNTP_ABLE) || defined(NNTP_ONLY)
#	ifndef NNTP_ABLE
#		define	NNTP_ABLE
#	endif
#	ifndef NNTP_INEWS
#		define	NNTP_INEWS
#	endif
#endif

/*
 *  Time idle after which to check (via STAT) if nntp connection is still ok
 */

#define NNTP_IDLE_RETRY_SECS	300

/*
 * Index file daemon version of tin. Will create/update index files from cron
 * on NNTP server machine so clients can retreive index file with NNTP XINDEX
 * command from server. Also can be used on machines that just want one copy
 * of all the index files in one place. In this case the normal tin must have
 * access to the index directory (-I dir option) or be setuid news.
 */

#ifdef INDEX_DAEMON
#	define	LOCK_FILE "tind.LCK"
#	undef	HAVE_POLL
#	undef	HAVE_SELECT
#	undef	NNTP_ABLE
#	undef	NNTP_ONLY
#	undef	NNTP_INEWS
#endif

/*
 * Specify News spool & control directories
 */

#ifndef SPOOLDIR
#ifdef VMS
#   define	SPOOLDIR    "NEWSSPOOL:[000000]"
#else
#	define		SPOOLDIR	"/usr/spool/news"
#endif /* !VMS */
#endif
#ifndef LIBDIR
#ifdef VMS
#define LIBDIR		"NEWSLIB:[000000]"
#else
#define LIBDIR		"/usr/lib/news"
#endif
#endif
#ifndef NOVROOTDIR
#	define		NOVROOTDIR	SPOOLDIR
#endif
#ifndef INEWSDIR
#	define		INEWSDIR	LIBDIR
#endif

/*
 * Determine machine configuration for external programs & directories
 */

#ifndef BSD
#	ifndef HAVE_BCOPY
#		define	bcopy(a,b,c)	memcpy(b,a,c)
#	endif
#	ifndef HAVE_BZERO
#		define	bzero(a,b)		memset(a,'\0',b)
#	endif
#	ifndef HAVE_BFILL
#		define	bfill(a,b,c)	memset(a,c,b)
#	endif
#endif

#ifdef BSD
#	ifndef HAVE_MEMCMP
#		define		memcmp(s1, s2, n)	bcmp(s2, s1, n)
#	endif
#	ifndef HAVE_MEMCPY
#		define		memcpy(s1, s2, n)	bcopy(s2, s1, n)
#	endif
#	ifndef HAVE_MEMSET
#		define		memset(s1, s2, n)	bfill(s1, n, s2)
#	endif
#	ifndef HAVE_STRCHR
#		define		strchr(str, ch) 	index(str, ch)
#		define		strrchr(str, ch)	rindex(str, ch)
#	endif
#	define		DEFAULT_SHELL		"/bin/csh"
#	if defined(__386BSD__) || defined(__bsdi__) || defined(__NetBSD__) || defined(__FreeBSD__)
#		ifndef DEFAULT_EDITOR
#			define	DEFAULT_EDITOR	"/usr/bin/vi"
#		endif
#		define	DEFAULT_PRINTER "/usr/bin/lpr"
#		define	DEFAULT_MAILER	"/usr/sbin/sendmail"
#		define	DEFAULT_MAILBOX "/var/mail"
#	else
#		ifndef DEFAULT_EDITOR
#			define	DEFAULT_EDITOR	"/usr/ucb/vi"
#		endif
#		define	DEFAULT_PRINTER "/usr/ucb/lpr"
#	endif
#	define		DEFAULT_SUM		"sum"
#	ifdef DGUX
#		define	DEFAULT_MAILBOX "/usr/mail"
#		define	USE_INVERSE_HACK
#	endif
#	ifdef pyr
#		define	DEFAULT_MAILER	"/usr/.ucbucb/mail"
#	endif
#	ifndef DEFAULT_MAILER
#		define	DEFAULT_MAILER	"/usr/lib/sendmail"
#	endif
#	ifndef DEFAULT_MAILBOX
#		define	DEFAULT_MAILBOX "/usr/spool/mail"
#	endif
#else
#	if defined(NCR) || defined(atthcx) || defined(PTX) || defined(sinix)
#		define	DEFAULT_MAILER	"/usr/bin/mailx"
#	endif
#	if defined(__hpux) || defined(u3b2)
#		define	DEFAULT_MAILER	"/usr/lib/sendmail"
#	endif
#	ifdef linux
#		define	DEFAULT_MAILBOX "/var/spool/mail"
#		define	DEFAULT_MAILER	"/usr/sbin/sendmail"
#		define	DEFAULT_PRINTER "/usr/bin/lpr"
#	endif
#	ifdef M_AMIGA
#		ifndef DEFAULT_EDITOR
#			define	DEFAULT_EDITOR		"c:ed"
#		endif
#		define	DEFAULT_MAILBOX 	"uumail:"
#		define	DEFAULT_MAILER		"uucp:c/sendmail"
#		define	DEFAULT_POSTER		"uucp:c/postnews %s"
#		define	DEFAULT_PRINTER 	"c:copy to PRT:"
#		define	DEFAULT_BBS_PRINTER	"c:copy to NIL:"
#		define	DEFAULT_SHELL		"c:newshell"	/* Not Yet Implemented */
#		define	DEFAULT_UUDECODE	"uudecode %s"
#		define	DEFAULT_UNSHAR		"unshar %s"
#	endif
#ifdef VMS
#	define	DEFAULT_EDITOR		"EDIT/TPU"
#	define	DEFAULT_MAILBOX 	"SYS$LOGIN:"
#	define	DEFAULT_MAILER		"MAIL"
#	define	MAILER_FORMAT	    "MAIL /SUBJECT=\"%S\" %F \"IN%%\"\"%T\"\""
#	define	DEFAULT_POSTER		"inews %s"
#	define	DEFAULT_PRINTER 	"PRINT/DELETE"
#	define	DEFAULT_UUDECODE	"uudecode %s"
#	define	DEFAULT_UNSHAR		"unshar %s"
#endif
#	ifdef M_OS2
#		ifndef DEFAULT_EDITOR
#			define	DEFAULT_EDITOR		"epm /m"
#		endif
#		define	DEFAULT_MAILBOX 	"/mail"
#		define	DEFAULT_MAILER		"sendmail -af %s -f %s %s"
#		define	DEFAULT_POSTER		"postnews %s"
#		define	DEFAULT_PRINTER 	"lpt1"
#		define	DEFAULT_SHELL		"cmd.exe"
#		define	DEFAULT_UUDECODE	"uudecode %s"
#		define	DEFAULT_UNSHAR		"unshar %s"
#	endif
#	ifdef WIN32
#		ifndef DEFAULT_EDITOR
#			define	DEFAULT_EDITOR		"vi"
#		endif
#		define	DEFAULT_MAILBOX 	"/mail"
#		define	DEFAULT_MAILER		"sendmail"
#		define	DEFAULT_POSTER		"postnews %s"
#		define	DEFAULT_PRINTER 	"lpt1"
#		define	DEFAULT_SHELL		"cmd.exe"
#		define	DEFAULT_UUDECODE	"uudecode %s"
#		define	DEFAULT_UNSHAR		"unshar %s"
#	endif
#	ifdef M_XENIX
#		ifndef DEFAULT_EDITOR
#			define	DEFAULT_EDITOR	"/bin/vi"
#		endif
#		define	DEFAULT_MAILER	"/usr/bin/mail"
#		define	DEFAULT_MAILBOX "/usr/spool/mail"
#	endif
#	ifdef QNX42
#		ifndef DEFAULT_EDITOR
#			define	DEFAULT_EDITOR		"/bin/vedit"
#		endif
#		define	DEFAULT_MAILER		"/usr/bin/sendmail"
#		define	DEFAULT_MAILBOX 	"/usr/spool/mail"
#	endif
#	ifdef RS6000
#		define	DEFAULT_MAILER	"/usr/lib/sendmail"
#		define	DEFAULT_PRINTER "/bin/lp"
#	endif
#	ifdef SCO_UNIX
#		define	HAVE_MMDF_MAILER
#	endif
#	ifdef sinix
#		define	DEFAULT_PRINTER "/bin/lpr"
#	endif
#	ifdef sysV68
#		define	DEFAULT_MAILER	"/bin/rmail"
#	endif
#	ifdef UNIXPC
#		define	DEFAULT_MAILER	"/bin/rmail"
#	endif
#	ifndef DEFAULT_SHELL
#		define	DEFAULT_SHELL	"/bin/sh"
#	endif
#	ifndef DEFAULT_EDITOR
#		define	DEFAULT_EDITOR	"/usr/bin/vi"
#	endif
#	ifndef DEFAULT_MAILBOX
#		define	DEFAULT_MAILBOX "/usr/mail"
#	endif
#	ifndef DEFAULT_MAILER
#		define	DEFAULT_MAILER	"/bin/mail"
#	endif
#	ifndef DEFAULT_PRINTER
#		define	DEFAULT_PRINTER "/usr/bin/lp"
#	endif
#	define		DEFAULT_SUM		"sum -r"
#endif

#ifdef HAVE_LONG_FILENAMES
#	define		LONG_PATH_PART	"part"
#	define		LONG_PATH_PATCH "patch"
#else
#	define		LONG_PATH_PART	""
#	define		LONG_PATH_PATCH "p"
#endif

/*
 * Useful for logging user usage
 */

#define 	LOG_USER_FILE	".tin_log"

/*
 * Should active file be reread for new news & if so how often
 */

#ifdef DONT_REREAD_ACTIVE_FILE
#	define	REREAD_ACTIVE_FILE_SECS 0
#else
#	ifndef REREAD_ACTIVE_FILE_SECS
#		define	REREAD_ACTIVE_FILE_SECS 1200	/* seconds (20 mins) */
#	endif
#endif

/*
 * Initial sizes of internal arrays for small (<4MB) & large memory machines
 */

#ifdef SMALL_MEMORY_MACHINE
#	define		DEFAULT_ACTIVE_NUM	1800
#	define		DEFAULT_ARTICLE_NUM	600
#	define		DEFAULT_SAVE_NUM	10
#	define		DEFAULT_SPOOLDIR_NUM	5
#	define		DEFAULT_NEWNEWS_NUM	5
#else
#	define		DEFAULT_ACTIVE_NUM	1800
#	define		DEFAULT_ARTICLE_NUM	1200
#	define		DEFAULT_SAVE_NUM	30
#	define		DEFAULT_SPOOLDIR_NUM	10
#	define		DEFAULT_NEWNEWS_NUM	5
#endif

#define 	NEWSRC_FILE		".newsrc"
#define 	NEWNEWSRC_FILE		".newnewsrc"
#define 	OLDNEWSRC_FILE		".oldnewsrc"
#ifdef VMS
#define 	ATTRIBUTES_FILE 	"attributes"
#define 	CONFIG_FILE		"tinrc"
#define 	RCDIR			"TIN"
#define 	INDEX_MAILDIR		"MAILIDX"
#define 	INDEX_NEWSDIR		"INDEX"
#define 	INDEX_SAVEDIR		"SAVE"
#else
#define 	RCDIR			".tin"
#define 	ATTRIBUTES_FILE 	"attributes"
#define 	CONFIG_FILE		"tinrc"
#define 	INDEX_MAILDIR		".mail"
#define 	INDEX_NEWSDIR		".news"
#define 	INDEX_SAVEDIR		".save"
#endif
#define 	ACTIVE_FILE		"active"
#define 	ACTIVE_MAIL_FILE	"active.mail"
#define 	ACTIVE_SAVE_FILE	"active.save"
#define 	ACTIVE_TIMES_FILE	"active.times"
#define 	FILTER_FILE		"filter"
#define 	GROUP_TIMES_FILE	"group.times"
#define 	POSTED_FILE		"posted"
#define 	DEFAULT_MAILDIR 	"Mail"
#define 	DEFAULT_SAVEDIR 	"News"
#define 	MAILGROUPS_FILE 	"mailgroups"
#define 	MSG_HEADERS_FILE	"headers"
#define 	MOTD_FILE		"motd"
#define 	OVERVIEW_FILE		".overview"
#define 	OVERVIEW_FMT		"overview.fmt"
#define 	SUBSCRIPTIONS_FILE	"subscriptions"

#define 	_CONF_FROMHOST		"fromhost"
#define 	_CONF_ORGANIZATION	"organization"
#define 	_CONF_SERVER		"server"

#ifndef TRUE
#	define TRUE		1
#endif

#ifndef FALSE
#	define FALSE		0
#endif

#ifndef MAX
#	define MAX(a,b)		((a > b) ? a : b)
#endif

#ifndef forever
#	define forever		for(;;)
#endif

#define STRCMPEQ(s1, s2)		(*(s1) == *(s2) && strcmp((s1), (s2)) == 0)
#define STRNCMPEQ(s1, s2, n)		(*(s1) == *(s2) && strncmp((s1), (s2), n) == 0)
#define STRNCASECMPEQ(s1, s2, n)	(strncasecmp((s1), (s2), n) == 0)

#ifdef VMS
#	define	LEN			512
#	define	PATH_LEN		256
#endif
#ifdef M_AMIGA
#	define	LEN			512
#	define	PATH_LEN		256
#endif
#if defined(M_OS2) || defined(M_UNIX)
#	ifndef MAXPATHLEN
#		define MAXPATHLEN 	256
#	endif
#	define	PATH_LEN		MAXPATHLEN
#	define	LEN			1024
#endif
#if defined(WIN32)
#	define	PATH_LEN		MAX_PATH
#	define	LEN			1024
#endif
#define		NEWSRC_LINE		8192
#define		OVERVIEW_LINE		2048
#ifdef HAVE_MAIL_HANDLER
#	define	HEADER_LEN		8192
#else
#	define	HEADER_LEN		2048
#endif
#define 	MODULO_COUNT_NUM	10
#define 	TABLE_SIZE		1409
#define 	MAX_PAGES		1000
#define 	ctrl(c) 		((c) & 0x1F)

#ifndef DEFAULT_ISO2ASC
#	define	DEFAULT_ISO2ASC "-1 "	/* ISO -> Ascii charset conversion */
#endif

#ifndef DEFAULT_COMMENT
#	define	DEFAULT_COMMENT ": "	/* used when by follow-ups & replys */
#endif
#ifndef ART_MARK_UNREAD
#	define	ART_MARK_UNREAD '+'		/* used to show that an art is unread */
#endif
#ifndef ART_MARK_RETURN
#	define	ART_MARK_RETURN '-'		/* used to show that an art will return */
#endif
#ifndef ART_MARK_SELECTED
#	define	ART_MARK_SELECTED	'*'	/* used to show that an art was auto selected */
#endif
#ifndef ART_MARK_READ
#	define	ART_MARK_READ		' '	/* used to show that an art was not read or seen  */
#endif
#ifndef ART_MARK_DELETED
#	define	ART_MARK_DELETED	'D'	/* art has been marked for deletion (mailgroup) */
#endif
#ifndef MARK_INRANGE
#	define	MARK_INRANGE		'#'	/* group/art within a range (# command) */
#endif

#ifdef USE_INVERSE_HACK
#	define		SELECT_MISC_COLS	21
#	define		BLANK_GROUP_COLS	2
#	define		BLANK_PAGE_COLS 	2
#else
#	define		SELECT_MISC_COLS	21
#	define		BLANK_GROUP_COLS	0
#	define		BLANK_PAGE_COLS 	0
#endif

/*
 * Maximum permissible colour number
 */
#define		MAX_COLOR			15

#define 	SCREEN_READ_UNREAD		6		/* position for "  +" / "   " */
#define 	INDEX_TOP			2

#ifdef NO_REGEX
#	define STR_MATCH(s1,s2) (str_str (s1, s2, strlen (s2)) != 0)
#else
#	define STR_MATCH(s1,pat)	(wildmat (s1, pat))
#endif

#define IGNORE_ART(i)	((arts[i].killed) || (arts[i].thread == ART_EXPIRED))

/*
 *  News/Mail group types
 */

#define 	GROUP_TYPE_MAIL 		0
#define 	GROUP_TYPE_NEWS 		1
#define 	GROUP_TYPE_SAVE 		2

/*
 *  used by get_arrow_key()
 */

#ifdef WIN32
#define 	KEYMAP_UNKNOWN			0
#define 	KEYMAP_UP				0xA6
#define 	KEYMAP_DOWN				0xA8
#define 	KEYMAP_LEFT				0xA5
#define 	KEYMAP_RIGHT			0xA7
#define 	KEYMAP_PAGE_UP			0xA1
#define 	KEYMAP_PAGE_DOWN		0xA2
#define 	KEYMAP_HOME			0xA4
#define 	KEYMAP_END			0xA3
#define 	KEYMAP_MOUSE			0
#else
#define 	KEYMAP_UNKNOWN			0
#define 	KEYMAP_UP			1
#define 	KEYMAP_DOWN			2
#define 	KEYMAP_LEFT			3
#define 	KEYMAP_RIGHT			4
#define 	KEYMAP_PAGE_UP			5
#define 	KEYMAP_PAGE_DOWN		6
#define 	KEYMAP_HOME			7
#define 	KEYMAP_END			8
#define 	KEYMAP_MOUSE			9
#endif

/*
 *  used by feed_articles() & show_mini_help()
 */

#define 	SELECT_LEVEL			1
#define 	SPOOLDIR_LEVEL			2
#define 	GROUP_LEVEL			3
#define 	THREAD_LEVEL			4
#define 	PAGE_LEVEL			5

#define 	MINI_HELP_LINES 		5

#define 	FEED_MAIL			1
#define 	FEED_PIPE			2
#define 	FEED_PRINT			3
#define 	FEED_SAVE			4
#define 	FEED_SAVE_TAGGED		5
#define 	FEED_REPOST			6

#define		THREAD_NONE			0
#define		THREAD_SUBJ			1
#define		THREAD_REFS			2

#define		SHOW_FROM_NONE			0
#define		SHOW_FROM_ADDR			1
#define		SHOW_FROM_NAME			2
#define		SHOW_FROM_BOTH			3

/*
 * used in feed.c & save.c
 */

#define		POST_PROC_NONE			0
#define		POST_PROC_SHAR			1
#define		POST_PROC_UUDECODE		2
#define		POST_PROC_UUD_LST_ZOO		3
#define		POST_PROC_UUD_EXT_ZOO		4
#define		POST_PROC_UUD_LST_ZIP		5
#define		POST_PROC_UUD_EXT_ZIP		6

/*
 *  used in art.c & rcfile.c
 */

#define 	SORT_BY_NOTHING 		0		/* sort types on arts[] array */
#define 	SORT_BY_SUBJ_DESCEND		1
#define 	SORT_BY_SUBJ_ASCEND		2
#define 	SORT_BY_FROM_DESCEND		3
#define 	SORT_BY_FROM_ASCEND		4
#define 	SORT_BY_DATE_DESCEND		5
#define 	SORT_BY_DATE_ASCEND		6

/*
 *  used in help.c
 */

#define 	HELP_INFO			0
#define 	POST_INFO			1

/*
 *  used in save.c
 */

#define 	CHECK_ANY_NEWS			0
#define 	START_ANY_NEWS			1
#define 	MAIL_ANY_NEWS			2
#define 	SAVE_ANY_NEWS			3

/*
 *  used in help.c
 */

#define 	HEADER_TO			0
#define 	HEADER_SUBJECT			1
#define 	HEADER_NEWSGROUPS		2

/*
 *  used in page.c & post.c
 */

#define 	POSTED_NONE			0
#define 	POSTED_REDRAW			1
#define 	POSTED_OK			2

/*
 * index_point variable values used throughout many modules
 */

#define 	GRP_UNINDEXED		-1		/* Stop reading group */
#define 	GRP_QUIT			-2		/* Set by 'Q' */
#define 	GRP_GOTONEXT		-3		/* Goto another group */
#define 	GRP_CONTINUE		-4		/* set in show_page() */
#define 	GRP_NOREDRAW		-5		/* Unclear meaning ? */
#define 	GRP_KILLED			-6		/* thread was killed at art level */

/*
 *
 */

#define 	EXIT_OK 			0
#define 	EXIT_ERROR			1
#define 	EXIT_NNTP_ERROR 	2

/*
 *  Assertion verifier
 */

#if !defined(M_OS2) && !defined(WIN32)
#	ifdef HAVE_ANSI_ASSERT
#		define	assert(p)	if(! (p)) asfail(__FILE__, __LINE__, #p); else (void)0;
#	else
#		define	assert(p)	if(! (p)) asfail(__FILE__, __LINE__, "p"); else (void)0;
#	endif
#endif

#define ESC	27

#ifdef HAVE_CR_AS_CHAR
#	define		CR		'\r'
#else
#	define		CR		10
#endif

/*
 * return codes for change_config_file ()
 */

#define 	NO_FILTERING		0
#define 	FILTERING			1
#define 	DEFAULT_FILTER_DAYS		28

/*
 *  art.thread
 */

#define 	ART_NORMAL		-1
#define 	ART_EXPIRED		-2

/*
 * art.parent, art.sibling, art.child
 */
#define		NO_THREAD		-1

/*
 *  art.status
 */

#define 	ART_READ		0
#define 	ART_UNREAD		1
#define 	ART_WILL_RETURN 	2

#define 	ART_UNAVAILABLE 	-1

/*
 * used by t_group & my_group[]
 */

#define 	UNSUBSCRIBED	'!'
#define 	SUBSCRIBED		':'

/*
 * filter_type used in struct t_filter
 */

#define FILTER_KILL				0
#define FILTER_SELECT				1

#define FILTER_SUBJ_CASE_SENSITIVE		0
#define FILTER_SUBJ_CASE_IGNORE 		1
#define FILTER_FROM_CASE_SENSITIVE		2
#define FILTER_FROM_CASE_IGNORE 		3
#define FILTER_MSGID				4
#define FILTER_LINES				5

#define FILTER_LINES_EQ 			0
#define FILTER_LINES_LT 			1
#define FILTER_LINES_GT 			2

/*
 * used in checking article header before posting
 */

#define NGLIMIT 20	/* Max. num. of crossposted groups before warning */
#define MAX_COL 78	/* Max. line length before issuing a warning */

/*
 * The following macros are used to simplify and speed up the
 * manipulation of the bitmaps in memory which record which articles
 * are read or unread in each news group.
 *
 * Data representation:
 *
 * Each bitmap is handled as an array of bytes; the least-significant
 * bit of the 0th byte is the 0th bit; the most significant bit of
 * the 0th byte is the 7th bit.  Thus, the most-significant bit of the
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
 * or expired; a 1 represents an unread article.  The 0th bit corresponds
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
 * another.  Both use the memset function, which is assumed to be
 * optimized for the target architecture.  NSETBLK is currently used to
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
#define NMAXBIT 	7
#define NBITPOS 	3
#define NBITSON 	0xff
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
 *  struct t_article - article header
 *
 *  article.thread:
 *	-1 initial default (ART_NORMAL)
 *	-2 means article has expired (wasn't found in file search) (ART_EXPIRED)
 *	of spool directory for the group)
 *	>=0 points to another arts[] (struct t_article)
 *
 *  article.inthread:
 *	FALSE for the first article in a thread, TRUE for all
 *	following articles in thread
 */

struct t_article
{
	long artnum;			/* Article number in spool directory for group */
	char *subject;			/* Subject: line from mail header */
	char *from;			/* From: line from mail header (address) */
	char *name;			/* From: line from mail header (full name) */
	time_t date;			/* Date: line from header in seconds */
	char *xref;			/* Xref: cross posted article reference line */
	char *msgid;			/* Message-ID: unique message identifier */
	char *refs;			/* References: article reference id's */
	int lines;			/* Lines: number of lines in article */
	char *archive;			/* Archive-name: line from mail header */
	char *part;			/* part  no. of archive */
	char *patch;			/* patch no. of archive */
	int tagged;			/* 0 = not tagged, >0 = tagged */
	int thread;
#ifdef REF_THREADING
	int parent;			/* Article followed up to */
	int sibling;			/* Next followup to parent */
	int child;			/* First followup to this article */
#endif
	unsigned int inthread:1;	/* 0 = thread head, 1 = thread follower */
	unsigned int status:2;		/* 0 = read, 1 = unread, 2 = will return */
	unsigned int killed:1;		/* 0 = not killed, 1 = killed */
	unsigned int selected:1;	/* 0 = not selected, 1 = selected */
	unsigned int zombie:1;		/* 1 = was alive (unread) before 'X' command */
	unsigned int delete:1;		/* 1 = delete art when leaving group [mail group] */
	unsigned int inrange:1; 	/* 1 = article selected via # range command */
};

/*
 *  struct t_attribute - configurable attributes on a per group basis
 */

struct t_attribute
{
	char *maildir;				/* mail dir if other than ~/Mail */
	char *savedir;				/* save dir if other than ~/News */
	char *savefile; 			/* save articles to specified file */
	char *sigfile;				/* sig file if other than ~/.Sig */
	char *organization;			/* organization name */
	char *followup_to;			/* where posts should be redirected */
	char *printer;				/* printer command & parameters */
	char *quick_kill_scope; 		/* quick filter kill scope */
	char *quick_select_scope;		/* quick filter select scope */
	char *mailing_list;			/* mail list email address */
	char *x_headers;			/* extra headers for message header */
	char *x_body;				/* bolierplate text for message body */
	unsigned int global:1;			/* global/group specific */
	unsigned int quick_kill_header:3;	/* quick filter kill header */
	unsigned int quick_kill_expire:1;	/* quick filter kill limited/unlimited time */
	unsigned int quick_kill_case:1; 	/* quick filter kill case sensitive? */
	unsigned int quick_select_header:3;	/* quick filter select header */
	unsigned int quick_select_expire:1;	/* quick filter select limited/unlimited time */
	unsigned int quick_select_case:1;	/* quick filter select case sensitive? */
	unsigned int auto_save_msg:1;		/* 0=none, 1=save copy of posted article */
	unsigned int auto_select:1;		/* 0=show all unread, 1='X' just hot arts */
	unsigned int auto_save:1;		/* 0=none, 1=save */
	unsigned int batch_save:1;		/* 0=none, 1=save -S/mail -M  */
	unsigned int delete_tmp_files:1;	/* 0=leave, 1=delete */
	unsigned int show_only_unread:1;	/* 0=all, 1=only unread */
	unsigned int thread_arts:2;		/* 0=unthread, 1=subject, 2=refs */
	unsigned int show_author:4;		/* 0=none, 1=name, 2=addr, 3=both */
	unsigned int sort_art_type:4;		/* 0=none, 1=subj descend, 2=subj ascend,
						   3=from descend, 4=from ascend,
						   5=date descend, 6=date ascend */
	unsigned int post_proc_type:4;		/* 0=none, 1=shar, 2=uudecode,
						   3=uud & list zoo, 4=uud & ext zoo*/
	unsigned int x_comment_to:1;		/* insert X-Comment-To: in Followup */
	char *news_quote_format;		/* another way to begin a posting format */
};

/*
 *  struct t_newsrc - newsrc related info.
 */

struct t_newsrc
{
	int		present;			/* update newsrc ? */
	long	num_unread;			/* unread articles in group */
	long	xmax;				/* newsrc max */
	long	xmin;				/* newsrc min */
	long	xbitlen;			/* bitmap length (max-min+1) */
	t_bitmap *xbitmap;			/* bitmap read/unread (max-min+1+7)/8 */
};

/*
 *  struct t_group - newsgroup info from active file
 */

struct t_group
{
	char *name;				/* newsgroup / mailbox name */
	char *description;		/* text from LIBDIR/newsgroups file */
	char *spooldir; 		/* groups spool directory */
	char moderated; 		/* state of group moderation */
	long count;				/* article number count */
	long xmax;				/* max. article number */
	long xmin;				/* min. article number */
	unsigned int type:4;	/* grouptype - newsgroup / mailbox / savebox */
	unsigned int inrange:4; /* 1 = group selected via # range command */
	unsigned int read_during_session:1;	/* marked TRUE if group entered during session */
	unsigned int art_was_posted:1;		/* marked TRUE if art was posted to group */
	int next;				/* next active entry in hash chain */
	int subscribed; 		/*  subscribed/unsubscribed to group */
	struct t_newsrc newsrc; /* newsrc bitmap specific info. */
	struct t_attribute *attribute;	/* group specific attributes */
	struct t_filters *glob_filter;	/* points to global filter array */
	struct t_filters *grps_filter;	/* group specific filters */
#ifdef INDEX_DAEMON
	time_t last_updated_time;		/* last time group dir was changed */
#endif
};

/*
 *  used in hashstr.c
 */

struct t_hashnode
{
	char *s;				/* the string we're saving */
	struct t_hashnode *next;			/* chain for spillover */
};

/*
 *  used in filter.c
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
 *  struct t_filter - local & global filtering (ie. kill & auto-selection)
 */

struct t_filter
{
	char *scope;				/* NULL='*' (all groups) or 'comp.os.*'   */
	char *subj;					/* Subject: line    */
	char *from;					/* From: line	    */
	char *msgid;				/* Message-ID: line */
	char lines_cmp; 			/* Lines compare <> */
	int  lines_num; 			/* Lines: line	    */
	time_t time;					/* expire time in seconds */
#if defined(_AIX) && !defined(__GNUC__)
	unsigned int inscope:4; /* if group matches scope ie. 'comp.os.*' */
	unsigned int type:2;		/* kill/auto select */
	unsigned int icase:2;		/* Case sensitive filtering */
#else
	unsigned char inscope:4;	/* if group matches scope ie. 'comp.os.*' */
	unsigned char type:2;		/* kill/auto select */
	unsigned char icase:2;		/* Case sensitive filtering */
#endif
};

/*
 *  struct t_filter_rule - provides parameters to build filter rule from
 */

struct t_filter_rule
{
	char text[PATH_LEN];
	char scope[PATH_LEN];
	int  counter;
	int  global;
	int  icase;
	int  lines_cmp;
	int  lines_num;
	int  from_ok;
	int  lines_ok;
	int  msgid_ok;
	int  subj_ok;
	int  type;
	int  expire_time;
	int  check_string;
	int  ignore_case;
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
	int is_mailbox;
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
	int total;	/* total article count */
	int unread;	/* number of unread articles (does not include seen) arts */
	int seen;	/* number of seen articles (ART_WILL_RETURN) */
	int deleted;	/* number of articles marked for deletion (mailgroups) */
	int inrange;	/* number of articles marked in a range */
	int selected_total;	/* total selected count */
	int selected_unread;	/* selected and unread */
	int selected_seen;	/* selected and seen */
	char art_mark;	/* mark to use for this thread - not used for groups */
};

/*
 * Used by spooldir command
 */

struct t_spooldir
{
	int state;
	char *name;
	char *comment;
};

/*
 * Used for detecting changes in active file size on different news servers
 */

struct t_newnews
{
	char *host;
	time_t time;
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

/*
 * Used for detecting new groups when reading news locally.  It's easy to be
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

/*
 * Determine signal return type
 */

#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif

/*
 * Determine qsort compare type
 */

#ifdef HAVE_COMPTYPE_VOID
#	ifdef __STDC__
typedef const void t_comptype;
#	else
typedef void t_comptype;
#	endif
#endif

#ifdef HAVE_COMPTYPE_CHAR
typedef char t_comptype;
#endif

#ifdef M_OS2
#	define	_CDECL	_cdecl
#	define	_FAR_	_far16
#	define	SEPDIR	'\\'
#else
#	define	_CDECL
#	define	_FAR_
#	define	SEPDIR	'/'
#endif

/*
 * mouse buttons for use in xterm
 */
 
#define		MOUSE_BUTTON_1	0
#define		MOUSE_BUTTON_2	1
#define		MOUSE_BUTTON_3	2

#define	EDITOR_FORMAT_OFF	"%E %F"
#define	ISPELL_CMD			"ispell"

#ifdef WIN32
#	define	my_fputc(ch, stream)		cmd_line ? fputc (ch, stream) : addch (ch)
#	define	my_fputs(str, stream)		cmd_line ? fputs (str, stream) : addstr(str)
#	define	my_fprintf (stream, str)	cmd_line ? fprintf (stream, str) : addstr(str)
#else
#	define	my_fputc(ch, stream)		fputc (ch, stream)
#	define	my_fputs(str, stream)		fputs (str, stream)
#	define	my_fprintf (stream, str)	fprintf (stream, str)
#endif

#ifdef M_AMIGA
#	define	NEWSGROUPS_FILE 	"newsdescrip"
#	define	BUG_REPORT_ADDRESS	"mark@garden.southern.gen.nz"
#	define	REDIRECT_OUTPUT 	"> NIL:"
#	define	REDIRECT_PGP_OUTPUT "> NIL:"
#	define	ENV_VAR_GROUPS		"TIN_GROUPS"
#	define	ENV_VAR_MAILER		"TIN_MAIL"
#	define	ENV_VAR_POSTER		"TIN_POST"
#	define	ENV_VAR_SHELL		"SHELL"
#	define	EDITOR_FORMAT_ON	"%E %F"
#	define	MAILER_FORMAT		"%M <%F -f %U"
#	define	METAMAIL_CMD		"metamail -e -p -m \"tin\""
#	define	TMPDIR			"T:"
#	ifdef	HAVE_KEY_PREFIX
#		define	KEY_PREFIX	0x9b
#	endif
extern void joinpath (char *result, char *dir, char *file);
#endif
#ifdef VMS
#	define	NEWSGROUPS_FILE 	"newsgroups"
#	define	BUG_REPORT_ADDRESS	"mcquill@next.duq.edu"
#	define	REDIRECT_OUTPUT 	""
#	define	REDIRECT_PGP_OUTPUT ""
#	define	ENV_VAR_MAILER		"TIN_MAILER"
/*#	define	ENV_VAR_SHELL		"SHELL"*/
#	define	ENV_VAR_POSTER		"TIN_POST"
#	define	EDITOR_FORMAT_ON	"%E %F"
#	define METAMAIL_CMD "metamail -e -p -m \"tin\""
#	define	TMPDIR "SYS$SCRATCH:"
#	ifdef	HAVE_KEY_PREFIX
#		define	KEY_PREFIX	0x9b
#	endif
extern void joinpath (char *result, char *dir, char *file);
extern void joindir (char *result, char *dir, char *file);
#endif /* VMS */
#ifdef M_OS2
#	define	NEWSGROUPS_FILE 	"newsgroups"
#	define	BUG_REPORT_ADDRESS	"andreas@scilink.org"
#	define	REDIRECT_OUTPUT 	"> NUL"
#	define	REDIRECT_PGP_OUTPUT	"> NUL"
#	define	ENV_VAR_GROUPS		"TIN_GROUPS"
#	define	ENV_VAR_MAILER		"TIN_MAIL"
#	define	ENV_VAR_POSTER		"TIN_POST"
#	define	ENV_VAR_SHELL		"COMSPEC"
#	define	EDITOR_FORMAT_ON	"%E %F"
#	define	METAMAIL_CMD		"metamail -e -p -m \"tin\""
extern void joinpath (char *result, char *dir, char *file);
#endif
#ifdef WIN32
#	define	NEWSGROUPS_FILE 	"newsgroups"
#	define	BUG_REPORT_ADDRESS	"nigele@microsoft.com"
#	define	REDIRECT_OUTPUT 	"> NUL"
#	define	REDIRECT_PGP_OUTPUT "> NUL"
#	define	ENV_VAR_GROUPS		"TIN_GROUPS"
#	define	ENV_VAR_MAILER		"TIN_MAIL"
#	define	ENV_VAR_POSTER		"TIN_POST"
#	define	ENV_VAR_SHELL		"COMSPEC"
#	define	EDITOR_FORMAT_ON	"%E +%N %F"
#	define	MAILER_FORMAT		"%M -t %T -f %U -s \"%S\" -F %F"
#	define	METAMAIL_CMD		"metamail -e -p -m \"tin\""
extern void joinpath (char *result, char *dir, char *file);
#endif
#ifdef M_UNIX
#	define	NEWSGROUPS_FILE 	"newsgroups"
#	define	BUG_REPORT_ADDRESS	"iain@scn.de"
#	define	REDIRECT_OUTPUT 	"> /dev/null 2>&1"
#	define	REDIRECT_PGP_OUTPUT "> /dev/null"
#	define	ENV_VAR_MAILER		"MAILER"
#	define	ENV_VAR_SHELL		"SHELL"
#	define	EDITOR_FORMAT_ON	"%E +%N %F"
#	define	MAILER_FORMAT		"%M \"%T\" < %F"
#	define	METAMAIL_CMD		"metamail -e -p -m \"tin\""
#	define	TMPDIR "/tmp/"
#	ifdef	HAVE_KEY_PREFIX
#		define	KEY_PREFIX	0xff
#	endif
#	define joinpath(result,dir,file)	sprintf (result,"%s/%s", dir, (file) ? file : "")
#endif

#if !defined(S_ISDIR)
#	if defined(M_AMIGA)
#		define st_mode st_attr
#		define S_ISDIR(m)   (((m) & ST_DIRECT) == ST_DIRECT)
#	endif
#	if defined(WIN32)
#		define S_ISDIR(m)	((m) & _S_IFDIR)
#	else
#		if defined(M_OS2)
#			define S_ISDIR(m)   ((m) & S_IF_DIR)
#		endif
#	endif
#	if defined(M_UNIX)
#		define S_ISDIR(m)   (((m) & S_IFMT) == S_IFDIR)
#	endif
#	if defined(VMS)
#		define S_ISDIR(m)   (((m) & S_IFMT) == S_IFDIR)
#	endif
#endif

#if !defined(S_ISREG)
#	define S_ISREG(m)   (((m) & S_IFMT) == S_IFREG)
#endif

#ifdef DONT_HAVE_PIPING
#ifdef VMS
#define TIN_PRINTFILE "Sys$Scratch:TIN_PRINT%d.TMP"
#else
#define TIN_PRINTFILE "tinprint%d.tmp"
#endif
#endif

/*
 * Defines for access()
 */
#ifndef R_OK
#	define	R_OK	4	/* Test for Read permission */
#endif
#ifndef W_OK
#	define	W_OK	2	/* Test for Write permission */
#endif
#ifndef X_OK
#	define	X_OK	1	/* Test for eXecute permission */
#endif
#ifndef F_OK
#	define	F_OK	0	/* Test for existence of File */
#endif


#define my_malloc(size) my_malloc1(__FILE__, __LINE__, (size))
#define my_realloc(ptr, size)	my_realloc1(__FILE__, __LINE__, (ptr), (size))

#define FreeIfNeeded(p) if (p != 0) free((char *)p)

/*
 *  function prototypes & extern definitions
 */

/* #include	"filebug.h" */

#ifndef SIG_ARGS
#	if defined(__STDC__)
#		define SIG_ARGS	int sig
#	endif
#endif

/* stifle complaints about not-a-prototype from gcc */
#ifdef DECL_SIG_CONST
# undef  SIG_DFL
# define SIG_DFL	(void (*)(SIG_ARGS))0
# undef  SIG_IGN
# define SIG_IGN	(void (*)(SIG_ARGS))1
# undef  SIG_ERR
# define SIG_ERR	(void (*)(SIG_ARGS))-1
#endif	/* DECL_SIG_CONST */

#ifdef M_AMIGA
typedef const char __far constext;
#else
typedef char constext;
#endif


#include	"patchlev.h"
#include	"extern.h"
#include	"nntplib.h"
#include	"proto.h"
#include	"menukeys.h"
#include	"msmail.h"

#if __STDC__ || defined(__cplusplus)
#	define P_(s) s
#else
#	define P_(s) ()
#endif


#ifdef DBMALLOC
#	undef strchr
#	undef strrchr
#	undef NSET1
#	undef NSET0
#	define NSET1(n,b) memset(n+NOFFSET(b), n[NOFFSET(b)] |	NTEST(n,b), 1)
#	define NSET0(n,b) memset(n+NOFFSET(b), n[NOFFSET(b)] & ~NTEST(n,b), 1)
#	include "malloc.h" /* dbmalloc 1.4 */
#endif

#if defined(WIN32) && defined(DEBUG) && defined(CHECKHEAP)
#	undef malloc
#	undef realloc
#	undef calloc
#	undef free
#	define malloc(cb)			mymalloc(cb, __LINE__, __FILE__)
#	define realloc(pv,cb)		myrealloc(pv, cb, __LINE__, __FILE__)
#	define calloc(cbs,cbe)	mycalloc(cbs, cbe, __LINE__, __FILE__)
#	define free(pv) 		myfree(pv, __LINE__, __FILE__)
#endif

#ifdef	DOALLOC
# if	__STDC__
#  define ANSI_VARARGS 1
#  include <stdarg.h>
# else
#  define ANSI_VARARGS 0
#  include <varargs.h>
#endif
extern	char	*doalloc  P_((char *, size_t));
extern	char	*docalloc P_((size_t, size_t));
extern	void	dofree	  P_((char *));
# undef malloc
# undef realloc
# undef calloc
# undef free
# define malloc(n)	doalloc((char *)0, n)
# define realloc	doalloc
# define calloc 	docalloc
# define free		dofree

extern	void	fail_alloc P_(( char *, char * ));
extern	void	Trace P_(( char *, ... ));
extern	void	Elapsed P_(( char * ));
extern	void	WalkBack P_(( void ));
extern	void	show_alloc P_(( void ));
extern	void	no_leaks P_(( void ));
extern	void	hist_reclaim P_(( void ));
#endif	/* DOALLOC */

#ifdef __DECC
#define ferror(x)	(0)
#define EndWin		EndWind
#endif
