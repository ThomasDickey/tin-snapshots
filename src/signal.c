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

static unsigned int time_remaining;

#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif

#ifdef SIGTSTP
int do_sigtstp = 0;
#endif

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
	if (sigaction (sig, &sa, &osa) < 0) {
		return SIG_ERR;
	}
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

void set_signal_handlers (void)
{
#ifdef SIGINT
	signal (SIGINT, signal_handler);	/* ctrl-C */
#endif
#ifdef SIGQUIT
	signal (SIGQUIT, signal_handler);	/* ctrl-\ */
#endif
#ifndef WIN32
#ifdef SIGILL
	signal (SIGILL, signal_handler);	/* illegal instruction */
#endif
#ifdef SIGFPE
	signal (SIGFPE, signal_handler);	/* floating point exception */
#endif
#ifdef SIGBUS
	signal (SIGBUS, signal_handler);	/* bus error */
#endif
#ifdef SIGSEGV
	signal (SIGSEGV, signal_handler);	/* segmentation violation */
#endif
#endif /* WIN32 */
#ifdef SIGPIPE
	signal (SIGPIPE, signal_handler);	/* brocken pipe */
#endif
#ifdef SIGCHLD
	signal (SIGCHLD, signal_handler);	/* death of a child process */
#endif
#ifdef SIGPWR
	signal (SIGPWR, signal_handler);	/* powerfail */
#endif
/*
#ifdef SIGHUP
	signal (SIGHUP, signal_handler);
#endif
*/
#ifdef SIGWINCH
	if (debug == 2) {
		wait_message ("SIGWINCH setting signal...");
		sleep (2);
	}
	signal (SIGWINCH, main_resize);
#endif

#if defined(SIGTSTP) && !defined(MINIX)
	{
		RETSIGTYPE (*ptr)(SIG_ARGS);
		ptr = signal (SIGTSTP, SIG_DFL);
		signal (SIGTSTP, ptr);
		if (ptr != SIG_IGN) {
			/*
			 *  SIGTSTP is ignored when starting from shells
			 *  without job-control
			 */
			do_sigtstp = 1;
			signal (SIGTSTP, main_suspend);
		}
	}
#endif
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


void _CDECL signal_handler (
	int sig)
{
	const char *sigtext;
#ifdef SIGCHLD
#if HAVE_TYPE_UNIONWAIT
	union wait wait_status;
#else
	int wait_status = 1;
#endif
#endif

	switch (sig) {
#ifdef SIGINT
		case SIGINT:
			sigtext = "SIGINT ";
#	if !defined(M_AMIGA) && !defined(__SASC)
			if (!update) {
				signal (SIGINT, signal_handler);
				return;
			}
#	endif
			break;
#endif
#ifdef SIGQUIT
		case SIGQUIT:
			sigtext = "SIGQUIT ";
			break;
#endif
#ifdef SIGHUP
		case SIGHUP:
			sigtext = "SIGHUP ";
			break;
#endif



#ifdef SIGCHLD
		case SIGCHLD:
			wait (&wait_status);
			signal (SIGCHLD, signal_handler);	/* death of a child */
			system_status = WEXITSTATUS(wait_status);
			return;
#endif

#ifdef SIGPWR
		case SIGPWR:
			sigtext = "SIGPWR ";
			break;
#endif
#ifdef SIGFPE
		case SIGFPE:
			sigtext = "SIGFPE ";
			break;
#endif
#ifdef SIGBUS
		case SIGBUS:
			sigtext = "SIGBUS ";
			break;
#endif
#ifdef SIGSEGV
		case SIGSEGV:
			sigtext = "SIGSEGV ";
			break;
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
			signal(SIGPIPE, signal_handler);
			return;
#endif
		default:
			sigtext = "";
			break;
	}
	Raw (FALSE);
	EndWin ();
	fprintf (stderr, "\n%s: signal handler caught signal %s(%d).\n",
		progname, sigtext, sig);
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

#if defined(apollo) || defined(HAVE_COREFILE)
	/* do this so we can get a traceback (doesn't dump core) */
	abort();
#else
	exit (1);
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
		if (win.ts_lines != 0) {
			*num_lines = win.ts_lines - 1;
		}
		if (win.ts_cols != 0) {
			*num_cols = win.ts_cols;
		}
	}
#else
#  ifdef TIOCGWINSZ
	if (ioctl (0, TIOCGWINSZ, &win) == 0) {
		if (win.ws_row != 0) {
			*num_lines = win.ws_row - 1;
		}
		if (win.ws_col != 0) {
			*num_cols = win.ws_col;
		}
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
	if (beginner_level) {
		NOTESLINES = *num_lines - INDEX_TOP - MINI_HELP_LINES;
	} else {
		NOTESLINES = *num_lines - INDEX_TOP - 1;
	}
	if (NOTESLINES <= 0) {
		NOTESLINES = 1;
	}

	if (*num_lines != old_lines || *num_cols != old_cols) {
		return TRUE;
	} else {
		return FALSE;
	}
}



void set_signals_art (void)
{
#ifdef SIGTSTP
	if (do_sigtstp) {
		sigdisp (SIGTSTP, art_suspend);
	}
#endif

#ifdef SIGWINCH
	signal (SIGWINCH, art_resize);
#endif
}


void set_signals_config (void)
{
#ifdef SIGTSTP
	if (do_sigtstp) {
		sigdisp (SIGTSTP, config_suspend);
	}
#endif

#ifdef SIGWINCH
	signal (SIGWINCH, config_resize);
#endif
}


void set_signals_group (void)
{
#ifdef SIGTSTP
	if (do_sigtstp) {
		sigdisp (SIGTSTP, group_suspend);
	}
#endif

#ifdef SIGWINCH
	signal (SIGWINCH, group_resize);
#endif
}


void set_signals_help (void)
{
#ifdef SIGTSTP
	if (do_sigtstp) {
		sigdisp (SIGTSTP, help_suspend);
	}
#endif

#ifdef SIGWINCH
	signal (SIGWINCH, help_resize);
#endif
}


void set_signals_page (void)
{
#ifdef SIGTSTP
	if (do_sigtstp) {
		sigdisp (SIGTSTP, page_suspend);
	}
#endif

#ifdef SIGWINCH
	signal (SIGWINCH, page_resize);
#endif
}


void set_signals_select (void)
{
#ifdef SIGTSTP
	if (do_sigtstp) {
		sigdisp (SIGTSTP, select_suspend);
	}
#endif

#ifdef SIGWINCH
	signal (SIGWINCH, select_resize);
#endif
}


void set_signals_thread (void)
{
#ifdef SIGTSTP
	if (do_sigtstp) {
		sigdisp (SIGTSTP, thread_suspend);
	}
#endif

#ifdef SIGWINCH
	signal (SIGWINCH, thread_resize);
#endif
}


#ifdef SIGTSTP

/* ARGSUSED0 */
void art_suspend (
	int sig)
{
	set_keypad_off ();
	set_xclick_off ();
	Raw (FALSE);
	wait_message (txt_suspended_message);

	kill (0, SIGSTOP);

	sigdisp (SIGTSTP, art_suspend);

	if (!update) {
		Raw (TRUE);
		art_resize (0);
	}
	set_keypad_on ();
	set_xclick_on ();
}


/* ARGSUSED0 */
void main_suspend (
	int sig)
{
	set_keypad_off ();
	set_xclick_off ();
	Raw (FALSE);
	wait_message (txt_suspended_message);

	kill (0, SIGSTOP);

	sigdisp (SIGTSTP, main_suspend);

	if (!update) {
		Raw (TRUE);
		main_resize (0);
	}
	set_keypad_on ();
	set_xclick_on ();
}


/* ARGSUSED0 */
void select_suspend (
	int sig)
{
	set_keypad_off ();
	set_xclick_off ();
	Raw (FALSE);
	wait_message (txt_suspended_message);

	kill (0, SIGSTOP);

	sigdisp (SIGTSTP, select_suspend);

	if (!update) {
		Raw (TRUE);
		select_resize (0);
	}
	set_keypad_on ();
	set_xclick_on ();
}


/* ARGSUSED0 */
void group_suspend (
	int sig)
{
	set_keypad_off ();
	set_xclick_off ();
	Raw (FALSE);
	wait_message (txt_suspended_message);

	kill (0, SIGSTOP);

	sigdisp (SIGTSTP, group_suspend);

	if (!update) {
		Raw (TRUE);
		group_resize (0);
	}
	set_keypad_on ();
	set_xclick_on ();
}


/* ARGSUSED0 */
void help_suspend (
	int sig)
{
	set_keypad_off ();
	set_xclick_off ();
	Raw (FALSE);
	wait_message (txt_suspended_message);

	kill (0, SIGSTOP);

	sigdisp (SIGTSTP, help_suspend);

	if (!update) {
		Raw (TRUE);
		help_resize (0);
	}
	set_keypad_on ();
	set_xclick_on ();
}


/* ARGSUSED0 */
void page_suspend (
	int sig)
{
	set_keypad_off ();
	set_xclick_off ();
	Raw (FALSE);
	wait_message (txt_suspended_message);

	kill (0, SIGSTOP);

	sigdisp (SIGTSTP, page_suspend);

	if (!update) {
		Raw (TRUE);
		page_resize (0);
	}
	set_keypad_on ();
	set_xclick_on ();
}


/* ARGSUSED0 */
void thread_suspend (
	int sig)
{
	set_keypad_off ();
	set_xclick_off ();
	Raw (FALSE);
	wait_message (txt_suspended_message);

	kill (0, SIGSTOP);

	sigdisp (SIGTSTP, thread_suspend);

	if (!update) {
		Raw (TRUE);
		thread_resize (0);
	}
	set_keypad_on ();
	set_xclick_on ();
}


/* ARGSUSED0 */
void config_suspend (
	int sig)
{
	set_keypad_off ();
	set_xclick_off ();
	Raw (FALSE);
	wait_message (txt_suspended_message);

	kill (0, SIGSTOP);

	sigdisp (SIGTSTP, config_suspend);

	Raw (TRUE);
	set_keypad_on ();
	set_xclick_on ();
	refresh_config_page (0, TRUE);
}

#endif /* SIGTSTP */


/* ARGSUSED0 */
void art_resize (
	int sig)
{
	char buf[LEN];

#ifdef SIGWINCH
	(void) set_win_size (&cLINES, &cCOLS);
	signal (SIGWINCH, art_resize);
#endif
	ClearScreen ();
	sprintf (buf, txt_group, glob_art_group);
	wait_message (buf);
}


void config_resize (
	int sig)
{
	int resized = TRUE;
#ifdef SIGWINCH
	resized = set_win_size (&cLINES, &cCOLS);
	signal (SIGWINCH, config_resize);
#endif
	if (resized || sig == 0) {
		refresh_config_page (0, TRUE);	/* force rebuild of option page */
	}
}


/* ARGSUSED0 */
void main_resize (
	int sig)
{
#ifdef SIGWINCH
	(void) set_win_size (&cLINES, &cCOLS);
	signal (SIGWINCH, main_resize);
#endif
}


/* ARGSUSED0 */
void select_resize (
	int sig)
{
	int resized = TRUE;

#ifdef SIGWINCH
	resized = set_win_size (&cLINES, &cCOLS);
	signal (SIGWINCH, select_resize);
#endif

	if (resized || sig == 0) {
		ClearScreen ();
		show_selection_page ();
	}
}


/* ARGSUSED0 */
void group_resize (
	int sig)
{
	int resized = TRUE;

#ifdef SIGWINCH
	resized = set_win_size (&cLINES, &cCOLS);
	signal (SIGWINCH, group_resize);
#endif

	if (resized || sig == 0) {
		ClearScreen ();
		show_group_page ();
	}
}


/* ARGSUSED0 */
void help_resize (
	int sig)
{
	int resized = TRUE;

#ifdef SIGWINCH
	resized = set_win_size (&cLINES, &cCOLS);
	signal (SIGWINCH, help_resize);
#endif

	if (resized || sig == 0) {
		display_info_page ();
	}
}

/* ARGSUSED0 */
void page_resize (
	int sig)
{
	int resized = TRUE;

#ifdef SIGWINCH
	resized = set_win_size (&cLINES, &cCOLS);
	signal (SIGWINCH, page_resize);
#endif

	if (resized || sig == 0) {
		ClearScreen ();
		redraw_page (glob_page_group, glob_respnum);
	}
}


/* ARGSUSED0 */
void thread_resize (
	int sig)
{
	int resized = TRUE;

#ifdef SIGWINCH
	resized = set_win_size (&cLINES, &cCOLS);
	signal (SIGWINCH, thread_resize);
#endif

	if (resized || sig == 0) {
		ClearScreen ();
		show_thread_page ();
	}
}
