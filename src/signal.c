/*
 *  Project   : tin - a Usenet reader
 *  Module    : signal.c
 *  Author    : I.Lea
 *  Created   : 1991-04-01
 *  Updated   : 1994-12-21
 *  Notes     : signal handlers for different modes and window resizing
 *  Copyright : (c) Copyright 1991-99 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */
#ifndef TCURSES_H
#	include "tcurses.h"
#endif /* !TCURSES_H */
#ifndef included_trace_h
#	include "trace.h"
#endif /* !included_trace_h */


#if defined (VMS) && defined (SIGBUS)
	/*
	 * SIGBUS works on VMS, but many useful information will be hidden
	 * if catched by signal handler.
	 */
#	undef SIGBUS
#endif /* VMS && SIGBUS */

/*
 * Needed for resizing under an xterm
 */

#ifdef HAVE_TERMIOS_H
#	include <termios.h>
#else
#	ifdef HAVE_TERMIO_H
#		include <termio.h>
#	endif /* HAVE_TERMIO_H */
#endif /* HAVE_TERMIOS_H */

#ifdef NEED_PTEM_H
#	include <sys/stream.h>
#	include <sys/ptem.h>
#endif /* NEED_PTEM_H */

#if defined(SIGWINCH) && !defined(DONT_HAVE_SIGWINCH)
#	if !defined(TIOCGWINSZ) && !defined(TIOCGSIZE)
#		ifdef HAVE_SYS_STREAM_H
#			include <sys/stream.h>
#		endif /* HAVE_SYS_STREAM_H */
#		ifdef HAVE_SYS_PTY_H
#			if !defined(_h_BSDTYPES) && defined(HAVE_SYS_BSDTYPES_H)
#				include <sys/bsdtypes.h>
#			endif /* !_h_BSDTYPES && HAVE_SYS_BSDTYPES_H */
#			include <sys/pty.h>
#		endif /* HAVE_SYS_PTY_H */
#	endif /* !TIOCGWINSZ && !TIOCGSIZE */
#endif /* SIGWINCH && !DONT_HAVE_SIGWINCH */

#ifdef MINIX
#	undef SIGTSTP
#endif /* MINIX */

/*
 * local prototypes
 */
static const char *signal_name(int code);
#ifdef SIGTSTP
	static void handle_suspend (void);
#endif /* SIGTSTP */
static void _CDECL signal_handler (SIG_ARGS);

#ifndef WEXITSTATUS
#	define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif /* !WEXITSTATUS */
#ifndef WIFEXITED
#	define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif /* !WIFEXITED */

#ifdef SIGTSTP
	static t_bool do_sigtstp = FALSE;
#endif /* SIGTSTP */

int signal_context = cMain;
int need_resize = cNo;

static const struct {
	int code;
	const char *name;
} signal_list[] = {
#ifdef SIGINT
	{ SIGINT,	"SIGINT" },	/* ctrl-C */
#endif /* SIGINT */
#ifdef SIGQUIT
	{ SIGQUIT,	"SIGQUIT " },	/* ctrl-\ */
#endif /* SIGQUIT */
#ifndef WIN32
#	ifdef SIGILL
	{ SIGILL,	"SIGILL" },	/* illegal instruction */
#	endif /* SIGILL */
#	ifdef SIGFPE
	{ SIGFPE,	"SIGFPE" },	/* floating point exception */
#	endif /* SIGFPE */
#	ifdef SIGBUS
	{ SIGBUS,	"SIGBUS" },	/* bus error */
#	endif /* SIGBUS */
#	ifdef SIGSEGV
	{ SIGSEGV,	"SIGSEGV" },	/* segmentation violation */
#	endif /* SIGSEGV */
#endif /* !WIN32 */
#ifdef SIGPIPE
	{ SIGPIPE,	"SIGPIPE" },	/* broken pipe */
#endif /* SIGPIPE */
#ifdef SIGCHLD
	{ SIGCHLD,	"SIGCHLD" },	/* death of a child process */
#endif /* SIGCHLD */
#ifdef SIGPWR
	{ SIGPWR,	"SIGPWR" },	/* powerfail */
#endif /* SIGPWR */
#ifdef SIGTSTP
	{ SIGTSTP,	"SIGTSTP" },	/* terminal-stop */
#endif /* SIGTSTP */
#ifdef SIGHUP
	{ SIGHUP,	"SIGHUP" },	/* hang up */
#endif /* SIGHUP */
#ifdef SIGUSR1
	{ SIGUSR1,	"SIGUSR1" },	/* User-defined signal 1 */
#endif /* SIGUSR1 */
#ifdef SIGTERM
	{ SIGTERM,	"SIGTERM" },	/* termination */
#endif /* SIGTERM */
#ifdef SIGWINCH
	{ SIGWINCH,	"SIGWINCH" },	/* window-size change */
#endif /* SIGWINCH */
};

#	ifdef HAVE_NESTED_PARAMS
RETSIGTYPE (*sigdisp(int signum, RETSIGTYPE (_CDECL *func)(SIG_ARGS)))(SIG_ARGS)
#	else
RETSIGTYPE (*sigdisp(signum, func))(SIG_ARGS)
	int signum;
	RETSIGTYPE (_CDECL *func)(SIG_ARGS);
#	endif /* HAVE_NESTED_PARAMS */
{
#ifdef HAVE_POSIX_JC
#	define RESTORE_HANDLER(x, y)
	struct sigaction sa, osa;

	sa.sa_handler = func;
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
#	ifdef SA_RESTART
		sa.sa_flags |= SA_RESTART;
#	endif /* SA_RESTART */
	if (sigaction (signum, &sa, &osa) < 0)
		return SIG_ERR;

	return (osa.sa_handler);
#else
#	define RESTORE_HANDLER(x, y)  signal (x, y)
	return (signal (signum, func));
#endif /* HAVE_POSIX_JC */
}

/*
 * Block/unblock SIGWINCH/SIGTSTP restarting syscalls
 */
void
allow_resize (
	t_bool allow)
{
#	ifdef HAVE_POSIX_JC
	struct sigaction sa, osa;

	sa.sa_handler = signal_handler;
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
#	ifdef SA_RESTART
	if (!allow)
		sa.sa_flags |= SA_RESTART;
#	endif /* SA_RESTART */
#	ifdef SIGWINCH
	sigaction(SIGWINCH, &sa, &osa);
#	endif /* SIGWINCH */
	sigaction(SIGTSTP, &sa, &osa);
#endif /* HAVE_POSIX_JC */
}

static const char *
signal_name (
	int code)
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

/*
 * Rescale the display buffer and redraw the contents according to
 * the current context
 * This should NOT be called from an interrupt context
 */
#if defined(SIGWINCH) || defined(SIGTSTP)
void
handle_resize (
	t_bool repaint)
{
#	ifdef SIGWINCH
	repaint |= set_win_size (&cLINES, &cCOLS);
#	endif /* SIGWINCH */

	if (cLINES < MIN_LINES_ON_TERMINAL || cCOLS < MIN_COLUMNS_ON_TERMINAL) {
		ring_bell ();
		wait_message(3, txt_screen_too_small_exiting);
		tin_done (EXIT_FAILURE);
	}

	TRACE(("handle_resize(%d:%d)", signal_context, repaint))

	if (!repaint)
		return;

#	ifdef USE_CURSES
#		ifdef HAVE_RESIZETERM
/*	resizeterm(cLINES+1, cCOLS+1); */
	resizeterm(cLINES+1, cCOLS);
#		else
	my_retouch();
#		endif /* HAVE_RESIZETERM */
#	endif /* USE_CURSES */

	switch (signal_context) {
		case cArt:
			show_art_msg (glob_group);
			break;
		case cConfig:
			refresh_config_page (-1);
			break;
		case cHelp:
#	ifdef USE_CURSES
			display_info_page (TRUE);
#	else
			display_info_page();
#	endif /* USE_CURSES*/
			break;
		case cGroup:
			ClearScreen ();
			show_group_page ();
			break;
		case cPage:
			ClearScreen ();
			redraw_page (glob_group, glob_respnum);
			break;
		case cSelect:
			ClearScreen ();
			show_selection_page ();
			break;
		case cThread:
			ClearScreen ();
			show_thread_page ();
			break;
		case cMain:
			break;
	}
	my_fflush(stdout);
}
#endif /* SIGWINCH || SIGTSTP */

#ifdef SIGTSTP
static void
handle_suspend (
	void)
{
	TRACE(("handle_suspend(%d)", signal_context))

	set_keypad_off ();
	if (!cmd_line)
		set_xclick_off ();

	Raw (FALSE);
	wait_message (0, txt_suspended_message);

	kill (0, SIGSTOP);				/* Put ourselves to sleep */

	RESTORE_HANDLER (SIGTSTP, signal_handler);

	if (!batch_mode) {
		Raw (TRUE);
		need_resize = cRedraw;		/* Flag a redraw */
	}
	set_keypad_on ();
	if (!cmd_line)
		set_xclick_on ();
}
#endif /* SIGTSTP */

static void _CDECL
signal_handler (
	int sig)
{
#ifdef SIGCHLD
#	ifdef HAVE_TYPE_UNIONWAIT
	union wait wait_status;
#	else
	int wait_status = 1;
#	endif /* HAVE_TYPE_UNIONWAIT */
#endif /* SIGCHLD */

	/* In this case statement, we handle only the non-fatal signals */
	switch (sig) {
#ifdef SIGINT
		case SIGINT:
#	if !defined(M_AMIGA) && !defined(__SASC)
			if (!batch_mode) {
				RESTORE_HANDLER (sig, signal_handler);
				return;
			}
#	endif /* !M_AMIGA && !__SASC */
			break;
#endif /* SIGINT */

#ifdef SIGCHLD
		case SIGCHLD:
			wait (&wait_status);
			RESTORE_HANDLER (sig, signal_handler);	/* death of a child */
			system_status = WEXITSTATUS(wait_status);
			return;
#endif /* SIGCHLD */

#ifdef SIGPIPE
		case SIGPIPE:
			got_sig_pipe = TRUE;
			RESTORE_HANDLER(sig, signal_handler);
			return;
#endif /* SIGPIPE */
#ifdef SIGTSTP
		case SIGTSTP:
			handle_suspend();
			return;
#endif /* SIGTSTP */
#ifdef SIGWINCH
		case SIGWINCH:
			need_resize = cYes;
			RESTORE_HANDLER (SIGWINCH, signal_handler);
			return;
#endif /* SIGWINCH */
		default:
			break;
	}
	fprintf (stderr, "\n%s: signal handler caught %s signal (%d).\n", tin_progname, signal_name(sig), sig);
#if defined(SIGHUP)
	if (sig == SIGHUP) {
		dangerous_signal_exit = TRUE;
		tin_done (- SIGHUP);
	}
#endif /* SIGHUP */
#if defined(SIGUSR1)
	if (sig == SIGUSR1) {
		dangerous_signal_exit = TRUE;
		tin_done (- SIGUSR1);
	}
#endif /* SIGUSR1 */
#if defined(SIGTERM)
	if (sig == SIGTERM) {
		dangerous_signal_exit = TRUE;
		tin_done (- SIGTERM);
	}
#endif /* SIGTERM */
#if defined(SIGBUS) || defined(SIGSEGV)
	if (
#	ifdef SIGBUS
		sig == SIGBUS
#	endif /* SIGBUS */
#	if defined(SIGBUS) && defined(SIGSEGV)
		||
#	endif /* SIGBUS && SIGSEGV */
#	ifdef SIGSEGV
		sig == SIGSEGV
#	endif /* SIGSEGV */
		) {
		vPrintBugAddress ();
	}
#endif /* SIGBUS || SIGSEGV */
	cleanup_tmp_files ();

#if 1
/* #if defined(apollo) || defined(HAVE_COREFILE) */
	/* do this so we can get a traceback (doesn't dump core) */
	abort();
#else
	giveup();
#endif /* 1 */ /* apollo || HAVE_COREFILE */
}

/*
 * Turn on (flag==TRUE) our signal handler for TSTP and WINCH
 * Otherwise revert to the default handler
 */
void
set_signal_catcher (
	int flag)
{
#ifdef SIGTSTP
	if (do_sigtstp)
		sigdisp (SIGTSTP, flag ? signal_handler : SIG_DFL);
#endif /* SIGTSTP */

#ifdef SIGWINCH
	sigdisp (SIGWINCH, flag ? signal_handler : SIG_DFL);
#endif /* SIGWINCH */
}

void
set_signal_handlers (
	void)
{
	size_t n;
	int code;
#ifdef SIGTSTP
	RETSIGTYPE (*ptr)(SIG_ARGS);
#endif /* SIGTSTP */

	for (n = 0; n < SIZEOF(signal_list); n++) {
		switch ((code = signal_list[n].code)) {
#ifdef SIGTSTP
		case SIGTSTP:
			ptr = sigdisp (code, SIG_DFL);
			sigdisp (code, ptr);
			if (ptr == SIG_IGN)
				break;
			/*
			 * SIGTSTP is ignored when starting from shells
			 * without job-control
			 */
			do_sigtstp = TRUE;
			nobreak; /* FALLTHROUGH */
#endif /* SIGTSTP */
		default:
			sigdisp (code, signal_handler);
		}
	}
}


t_bool
set_win_size (
	int *num_lines,
	int *num_cols)
{
	int old_lines;
	int old_cols;
#ifdef TIOCGSIZE
	struct ttysize win;
#else
#	ifdef TIOCGWINSZ
	struct winsize win;
#	endif /* TIOCGWINSZ */
#endif /* TIOCGSIZE */

	old_lines = *num_lines;
	old_cols = *num_cols;

#ifndef USE_CURSES
	init_screen_array (FALSE);		/* deallocate screen array */
#endif /* !USE_CURSES */

#ifdef TIOCGSIZE
	if (ioctl (0, TIOCGSIZE, &win) == 0) {
		if (win.ts_lines != 0)
			*num_lines = win.ts_lines - 1;
		if (win.ts_cols != 0)
			*num_cols = win.ts_cols;
	}
#else
#	ifdef TIOCGWINSZ
	if (ioctl (0, TIOCGWINSZ, &win) == 0) {
		if (win.ws_row != 0)
			*num_lines = win.ws_row - 1;
		if (win.ws_col != 0)
			*num_cols = win.ws_col;
	}
#	else
#		ifdef M_AMIGA
	AmiGetWinSize(num_lines, num_cols);
	(*num_lines)--;
#		endif /* M_AMIGA */
#	endif /* TIOCGWINSZ */
#endif /* TIOCGSIZE */

#ifndef USE_CURSES
	init_screen_array (TRUE);		/* allocate screen array for resize */
#endif /* !USE_CURSES */

	set_subj_from_size (*num_cols);

	RIGHT_POS = *num_cols - 20;
	MORE_POS  = *num_cols - 15;
	NOTESLINES = *num_lines - INDEX_TOP - (tinrc.beginner_level ? MINI_HELP_LINES : 1);
	if (NOTESLINES <= 0)
		NOTESLINES = 1;

	return (*num_lines != old_lines || *num_cols != old_cols);
}
