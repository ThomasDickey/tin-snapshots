/*
 *  Project   : tin - a Usenet reader
 *  Module    : screen.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 25-07-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	<stdarg.h>

#include	"tin.h"
#include	"tcurses.h"

char msg[LEN];

#if !USE_CURSES
	struct t_screen *screen;
#endif

/*
 * Move the cursor to the lower-left of the screen, where it won't be annoying
 */
void
stow_cursor(void)
{
	if (!cmd_line)
		MoveCursor (cLINES, 0);
}

/*
 * Centre a formatted colour message at the bottom of the screen
 */
void
info_message (const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	clear_message ();
#ifdef HAVE_COLOR
	fcol(col_message);
#endif

	vsprintf (msg, fmt, ap);
	center_line (cLINES, FALSE, msg);	/* center the message at screen bottom */

#ifdef HAVE_COLOR
	fcol(col_normal);
#endif
	stow_cursor();

	va_end(ap);
}


/*
 * Print a formatted colour message at the bottom of the screen, wait a while
 */
void
wait_message (int delay, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	clear_message ();
#ifdef HAVE_COLOR
	fcol(col_message);
#endif

	vsprintf (msg, fmt, ap);
	my_fputs (msg, stdout);

#ifdef HAVE_COLOR
	fcol(col_normal);
#endif
	cursoron ();
	my_flush();

	sleep(delay);
/*	clear_message(); would be nice, but tin doesn't expect this yet */
	va_end(ap);
}


/*
 * Print a formatted message to stderr, no colour is added.
 * Interesting - this function implicitly clears 'errno'
 */
void
error_message (const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	errno = 0;

	clear_message ();

	vsprintf (msg, fmt, ap);
	my_fprintf (stderr, msg);
	my_fflush (stderr);

	if (cmd_line) {
		my_fputc ('\n', stderr);
		fflush (stderr);
	} else {
		stow_cursor();
		sleep (2);
	}

	va_end(ap);
}


/*
 * Print a formatted error message to stderr, no colour is added.
 * This function implicitly clears 'errno'
 */
void
perror_message (const char *fmt, ...)
{
#ifndef HAVE_STRERROR
#	ifdef HAVE_SYS_ERRLIST
#		ifdef M_AMIGA
#			ifndef sys_errlist
				extern char *__sys_errlist[];
#				define sys_errlist	__sys_errlist
#			endif
#		else
#			if DECL_SYS_ERRLIST
				extern char *sys_errlist[];
#			endif
#		endif
#	endif
#endif

	int err;
	va_list ap;

	err = errno;
	va_start(ap, fmt);

	clear_message ();

	vsprintf (msg, fmt, ap);

#ifdef HAVE_STRERROR
	my_fprintf (stderr, "%s: Error: %s", msg, strerror(err));
#else
#	ifdef HAVE_SYS_ERRLIST
		my_fprintf (stderr, "%s: %s", msg, sys_errlist[err]);
#	else
		my_fprintf (stderr, "%s: Errno: %i", msg, err);
#	endif
#endif
	errno = 0;

	if (cmd_line) {
		my_fputc ('\n', stderr);
		fflush (stderr);
	} else {
		stow_cursor();
		sleep (3);
	}

	va_end(ap);
}


void
clear_message (void)
{
	if (!cmd_line) {
		MoveCursor (cLINES, 0);
		CleartoEOLN ();
		cursoroff ();
#if !USE_CURSES
		my_flush();
#endif
	}
}


void
center_line (
	int line,
	int inverse,
	const char *str)
{
	int pos;
	char buffer[256];

	strncpy(buffer, str, 255);
	buffer[255]='\0';

	if (!cmd_line) {
		if (cCOLS >= (int) strlen (str))
			pos = (cCOLS - (int) strlen (str)) / 2;
		else
			pos = 1;

		MoveCursor (line, pos);
		if (inverse) {
			StartInverse ();
			my_flush();
		}
	}

	/* protect terminal... */
	Convert2Printable (buffer);

	if ((int) strlen (buffer) >= cCOLS) {
		char buf[256];
		sprintf(buf, "%-.*s%s", cCOLS-6, buffer, " ...");
		my_fputs (buf, stdout);
	} else
		my_fputs (buffer, stdout);

	if (cmd_line)
		my_flush();
	else {
		if (inverse)
			EndInverse ();
	}
}


void
draw_arrow (
	int line)
{
	MoveCursor (line, 0);

	if (draw_arrow_mark)
		my_fputs ("->", stdout);
	else {
#if USE_CURSES
		char buffer[BUFSIZ];
		char *s = screen_contents(line, 0, buffer);
#else
		char *s = screen[line-INDEX_TOP].col;
#endif
		StartInverse ();
		my_fputs (s, stdout);
		EndInverse ();
	}
	stow_cursor();
}


void
erase_arrow (
	int line)
{
	MoveCursor (line, 0);

	if (draw_arrow_mark)
		my_fputs ("  ", stdout);
	else {
#if USE_CURSES
		char buffer[BUFSIZ];
		char *s = screen_contents(line, 0, buffer);
#else
		char *s = screen[line-INDEX_TOP].col;
#endif
		EndInverse ();
		my_fputs (s, stdout);
	}
}


void
show_title (
	char *title)
{
	int col;

	col = (cCOLS - (int) strlen (txt_type_h_for_help))+1;
	if (col) {
		MoveCursor (0, col);
#ifdef HAVE_COLOR
		fcol(col_title);
#endif
		if (mail_check ())		/* you have mail message in */
			my_fputs (txt_you_have_mail, stdout);
		else
			my_fputs (txt_type_h_for_help, stdout);

#ifdef HAVE_COLOR
		fcol(col_normal);
#endif
	}
	center_line (0, TRUE, title);
}


void
ring_bell (void)
{
#if USE_CURSES
	if (!cmd_line)
		beep();
	else {
#endif
	my_fputc ('\007', stdout);
	my_flush();
#if USE_CURSES
	}
#endif
}


void
spin_cursor (void)
{
	static const char *buf = "|/-\\|/-\\";
	static int i = 0;

	if (batch_mode)
		return;

	if (i > 7)
		i = 0;

	my_printf ("\b%c", buf[i++]);
	my_flush();
}


/*
 *  show how many items we've processed.  Allow
 *  four digits for each number, with the possibility that it will be
 *  wider.
 */

void
show_progress (
	const char *txt,
	int count,
	int total)
{
	if (batch_mode)
		return;

	MoveCursor(cLINES, 0);
#if 1 /* see also search.c search_art_body () */
	if (total < 0)
		my_printf ("%s", txt);
	else
#endif
		my_printf ("%s%4d/%-4d", txt, count, total);
	my_flush();
}

#if 0
These are the original fixed argument versions
void
info_message (
	const char *str)
{
	clear_message ();			/* Clear any old messages hanging around */
#ifdef HAVE_COLOR
	fcol(col_message);
#endif
	center_line (cLINES, FALSE, str);	/* center the message at screen bottom */
#ifdef HAVE_COLOR
	fcol(col_normal);
#endif
	stow_cursor();
}


void
wait_message (
	const char *str)
{
	clear_message ();	  /* Clear any old messages hanging around */
#ifdef HAVE_COLOR
	fcol(col_message);
#endif
	my_fputs (str, stdout);
#ifdef HAVE_COLOR
	fcol(col_normal);
#endif
	cursoron ();
	my_flush();
}


void
error_message (
	const char *template,
	const char *str)
{
	errno = 0;

	clear_message ();	  /* Clear any old messages hanging around */

	my_fprintf (stderr, template, str);
	my_fflush (stderr);

	if (cmd_line) {
		my_fputc ('\n', stderr);
		fflush (stderr);
	} else {
		stow_cursor();
		sleep (3);
	}
}


void
perror_message (
	const char *template,
	const char *str)
{
#ifndef HAVE_STRERROR
#	ifdef HAVE_SYS_ERRLIST
#		ifdef M_AMIGA
#			ifndef sys_errlist
				extern char *__sys_errlist[];
#				define sys_errlist	__sys_errlist
#			endif
#		else
#			if DECL_SYS_ERRLIST
				extern char *sys_errlist[];
#			endif
#		endif
#	endif
#endif

	char str2[512];
	int err;

	clear_message ();	  /* Clear any old messages hanging around */

	sprintf (str2, template, str);
	err = errno;
#ifdef HAVE_STRERROR
	my_fprintf (stderr, "%s: Error: %s", str2, strerror(err));
#else
#  ifdef HAVE_SYS_ERRLIST
	my_fprintf (stderr, "%s: %s", str2, sys_errlist[err]);
#  else
	my_fprintf (stderr, "%s: Error: %i", str2, err);
#  endif
#endif
	errno = 0;

	if (cmd_line) {
		my_fputc ('\n', stderr);
		fflush (stderr);
	} else {
		stow_cursor();
		sleep (3);
	}
}
#endif
