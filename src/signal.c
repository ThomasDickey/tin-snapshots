/*
 *  Project   : tin - a Usenet reader
 *  Module    : signal.c
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 21-12-94
 *  Notes     : signal handlers for different modes and window resizing
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"
#include	"trace.h"

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

#ifdef MINIX
#	undef SIGTSTP
#endif /* !MINIX */

static unsigned int time_remaining;

/*
 * local prototypes
 */
static const char * signal_name(int code);
#ifdef SIGTSTP
	static void handle_suspend (void);
#endif /* SIGTSTP */


#ifndef WEXITSTATUS
#	define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif /* !WEXITSTATUS */
#ifndef WIFEXITED
#	define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif /* !WIFEXITED */

#ifdef SIGTSTP
	static t_bool do_sigtstp = FALSE;
#endif /* SIGTSTP */

enum context { cMain, cArt, cConfig, cGroup, cHelp, cPage, cSelect, cThread };
enum context my_context;

static const struct {
	int	code;
	const char *name;
} signal_list[] = {
#ifdef SIGINT
	{ SIGINT,	"SIGINT" },	/* ctrl-C */
#endif
#ifdef SIGQUIT
	{ SIGQUIT,	"SIGQUIT " },	/* ctrl-\ */
#endif
#ifndef WIN32
#ifdef SIGILL
	{ SIGILL,	"SIGILL" },	/* illegal instruction */
#endif
#ifdef SIGFPE
	{ SIGFPE,	"SIGFPE" },	/* floating point exception */
#endif
#ifdef SIGBUS
	{ SIGBUS,	"SIGBUS" },	/* bus error */
#endif
#ifdef SIGSEGV
	{ SIGSEGV,	"SIGSEGV" },	/* segmentation violation */
#endif
#endif /* WIN32 */
#ifdef SIGPIPE
	{ SIGPIPE,	"SIGPIPE" },	/* broken pipe */
#endif
#ifdef SIGCHLD
	{ SIGCHLD,	"SIGCHLD" },	/* death of a child process */
#endif
#ifdef SIGPWR
	{ SIGPWR,	"SIGPWR" },	/* powerfail */
#endif
#ifdef SIGTSTP
	{ SIGTSTP,	"SIGTSTP" },	/* terminal-stop */
#endif
#ifdef SIGUSR1
	{ SIGUSR1,	"SIGUSR1" },	/* User-defined signal 1 */
#endif
#ifdef SIGTERM
	{ SIGTERM,	"SIGTERM" },	/* termination */
#endif
#ifdef SIGWINCH
	{ SIGWINCH,	"SIGWINCH" },	/* window-size change */
#endif
};

#ifdef HAVE_POSIX_JC

/*
 * for POSIX systems we know RETSIGTYPE is void
 */

RETSIGTYPE (*sigdisp(sig, func))(SIG_ARGS)
	int sig,
	RETSIGTYPE (*func)(SIG_ARGS))
{
	struct sigaction sa, osa;

	sa.sa_handler = func;
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
#	ifdef SA_RESTART
		sa.sa_flags |= SA_RESTART;
#	endif
	if (sigaction (sig, &sa, &osa) < 0)
		return SIG_ERR;

	return (osa.sa_handler);
}

#else
#ifdef HAVE_NESTED_PARAMS
RETSIGTYPE (*sigdisp(int sig, RETSIGTYPE (_CDECL *func)(SIG_ARGS)))(SIG_ARGS)
#else
RETSIGTYPE (*sigdisp(sig, func))(SIG_ARGS)
	int sig;
	RETSIGTYPE (_CDECL *func)(SIG_ARGS);
#endif
{
	return (signal (sig, func));
}

#endif /* HAVE_POSIX_JC */

static const char *
signal_name(int code)
{
	size_t n;
	const char *name = "unknown";
	for (n = 0; n < SIZEOF(signal_list); n++) {
		if (signal_list[n].code == code) {
			name = signal_list[n].name;
			break;
		}
	}
	return name;
}

#if defined(SIGWINCH) || defined(SIGTSTP)
void
handle_resize (int repaint)
{
#ifdef SIGWINCH
	repaint |= set_win_size (&cLINES, &cCOLS);
	signal (SIGWINCH, signal_handler);
#endif

	if (cLINES < MIN_LINES_ON_TERMINAL || cCOLS < MIN_COLUMNS_ON_TERMINAL) {
		ring_bell ();
		wait_message(3, txt_screen_too_small_exiting);
		tin_done (EXIT_ERROR);
	}

	TRACE(("handle_resize(%d:%d)", (int)my_context, repaint))

	if (repaint) {
#if USE_CURSES
#ifdef HAVE_RESIZETERM
		resizeterm(cLINES+1, cCOLS+1);
#else
		my_retouch();
#endif
#endif
		switch (my_context) {
		case cArt:
			ClearScreen ();
			wait_message (0, txt_group, glob_art_group);
			break;
		case cConfig:
			refresh_config_page (-1);
			break;
		case cHelp:
			display_info_page (TRUE);
			break;
		case cGroup:
			ClearScreen ();
			show_group_page ();
			break;
		case cMain:
			break;
		case cPage:
			ClearScreen ();
			redraw_page (glob_page_group, glob_respnum);
			break;
		case cSelect:
			ClearScreen ();
			show_selection_page ();
			break;
		case cThread:
			ClearScreen ();
			show_thread_page ();
			break;
		}
		my_fflush(stdout);
	}
}
#endif /* defined(SIGWINCH) || defined(SIGTSTP) */

#ifdef SIGTSTP
static void
handle_suspend (void)
{
	TRACE(("handle_suspend(%d)", (int)my_context))

	set_keypad_off ();
	set_xclick_off ();
	Raw (FALSE);
	wait_message (0, txt_suspended_message);

	kill (0, SIGSTOP);

	sigdisp (SIGTSTP, signal_handler);

	if (!batch_mode) {
		Raw (TRUE);
		handle_resize (TRUE);
	}
	set_keypad_on ();
	set_xclick_on ();
}
#endif

void _CDECL signal_handler (int sig)
{
#ifdef SIGCHLD
#if HAVE_TYPE_UNIONWAIT
	union wait wait_status;
#else
	int wait_status = 1;
#endif
#endif

	/* In this case statement, we handle only the non-fatal signals */
	switch (sig) {
#ifdef SIGINT
		case SIGINT:
#	if !defined(M_AMIGA) && !defined(__SASC)
			if (!batch_mode) {
				signal (sig, signal_handler);
				return;
			}
#	endif
			break;
#endif

#ifdef SIGCHLD
		case SIGCHLD:
			wait (&wait_status);
			signal (sig, signal_handler);	/* death of a child */
			system_status = WEXITSTATUS(wait_status);
			return;
#endif

#if defined(SIGALRM) && !defined(DONT_REREAD_ACTIVE_FILE)
		case SIGALRM:
			set_alarm_signal ();
			reread_active_file = TRUE;
			return;
#endif
#ifdef SIGPIPE
		case SIGPIPE:
			got_sig_pipe = TRUE;
			signal(sig, signal_handler);
			return;
#endif
#ifdef SIGTSTP
		case SIGTSTP:
			handle_suspend();
			return;
#endif
#ifdef SIGWINCH
		case SIGWINCH:
			handle_resize(FALSE);
			return;
#endif
		default:
			break;
	}
	Raw (FALSE);
	EndWin ();
	fprintf (stderr, "\n%s: signal handler caught %s signal (%d).\n", progname, signal_name(sig), sig);
#if defined(SIGUSR1)
	if (sig == SIGUSR1)
		tin_done (- SIGUSR1);
#endif
#if defined(SIGTERM)
	if (sig == SIGTERM)
		tin_done (- SIGTERM);
#endif
#if defined(SIGBUS) || defined(SIGSEGV)
	if (
#	ifdef SIGBUS
		sig == SIGBUS
#	endif
#	if defined(SIGBUS) && defined(SIGSEGV)
		||
#	endif
#	ifdef SIGSEGV
		sig == SIGSEGV
#	endif
		) {
		vPrintBugAddress ();
	}
#endif
	cleanup_tmp_files ();

#if 1
/* #if defined(apollo) || defined(HAVE_COREFILE) */
	/* do this so we can get a traceback (doesn't dump core) */
	abort();
#else
	exit (EXIT_ERROR);
#endif
}

void set_signal_catcher (int flag)
{
#ifdef SIGTSTP
	if (do_sigtstp)
		signal(SIGTSTP, flag ? signal_handler : SIG_DFL);
#endif
#ifdef SIGWINCH
	signal(SIGWINCH, flag ? signal_handler : SIG_DFL);
#endif
}

void set_signal_handlers (void)
{
	size_t n;
	int code;
	RETSIGTYPE (*ptr)(SIG_ARGS);

	my_context = cMain;
	for (n = 0; n < SIZEOF(signal_list); n++) {
		switch ((code = signal_list[n].code)) {
#ifdef SIGTSTP
		case SIGTSTP:
			ptr = signal (code, SIG_DFL);
			signal (code, ptr);
			if (ptr == SIG_IGN)
				break;
			/*
			 * SIGTSTP is ignored when starting from shells
			 * without job-control
			 */
			do_sigtstp = TRUE;
			/* FALLTHRU */
#endif
		default:
			signal (code, signal_handler);
		}
	}
}


void set_alarm_signal (void)
{
#ifndef DONT_REREAD_ACTIVE_FILE
	(void) alarm (0);
#if !defined(M_OS2) && !defined(WIN32) && !defined(M_AMIGA)
	signal (SIGALRM, signal_handler);
#endif
	alarm (reread_active_file_secs);
#endif
	reread_active_file = FALSE;
}


void set_alarm_clock_on (void)
{
#ifndef DONT_REREAD_ACTIVE_FILE
	alarm (time_remaining);
#endif
}


void set_alarm_clock_off (void)
{
#ifndef DONT_REREAD_ACTIVE_FILE
	time_remaining = alarm (0);
#endif
}


int
set_win_size (
	int *num_lines,
	int *num_cols)
{
	int	old_lines;
	int	old_cols;
#ifdef TIOCGSIZE
	struct ttysize win;
#else
#  ifdef TIOCGWINSZ
	struct winsize win;
#  endif
#endif

	old_lines = *num_lines;
	old_cols = *num_cols;

	init_screen_array (FALSE);		/* deallocate screen array */

#ifdef TIOCGSIZE
	if (ioctl (0, TIOCGSIZE, &win) == 0) {
		if (win.ts_lines != 0)
			*num_lines = win.ts_lines - 1;
		if (win.ts_cols != 0)
			*num_cols = win.ts_cols;
	}
#else
#  ifdef TIOCGWINSZ
	if (ioctl (0, TIOCGWINSZ, &win) == 0) {
		if (win.ws_row != 0)
			*num_lines = win.ws_row - 1;
		if (win.ws_col != 0)
			*num_cols = win.ws_col;
	}
#  else
#    ifdef M_AMIGA
	AmiGetWinSize(num_lines, num_cols);
	(*num_lines)--;
#    endif
#  endif
#endif

	init_screen_array (TRUE);		/* allocate screen array for resize */

	set_subj_from_size (*num_cols);

	RIGHT_POS = *num_cols - 20;
	MORE_POS  = *num_cols - 15;
	if (beginner_level)
		NOTESLINES = *num_lines - INDEX_TOP - MINI_HELP_LINES;
	else
		NOTESLINES = *num_lines - INDEX_TOP - 1;
	if (NOTESLINES <= 0)
		NOTESLINES = 1;

	if (*num_lines != old_lines || *num_cols != old_cols)
		return TRUE;
	else
		return FALSE;
}

void set_signals_art    (void) { my_context = cArt; }
void set_signals_config (void) { my_context = cConfig; }
void set_signals_group  (void) { my_context = cGroup; }
void set_signals_help   (void) { my_context = cHelp; }
void set_signals_page   (void) { my_context = cPage; }
void set_signals_select (void) { my_context = cSelect; }
void set_signals_thread (void) { my_context = cThread; }
