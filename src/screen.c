/*
 *  Project   : tin - a Usenet reader
 *  Module    : screen.c
 *  Author    : I. Lea & R. Skrenta
 *  Created   : 1991-04-01
 *  Updated   : 1997-12-31
 *  Notes     :
 *  Copyright : (c) Copyright 1991-99 by Iain Lea & Rich Skrenta
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

char mesg[LEN];

#ifndef USE_CURSES
	struct t_screen *screen;
#endif /* !USE_CURSES */

/*
 * Move the cursor to the lower-left of the screen, where it won't be annoying
 */
void
stow_cursor (
	void)
{
	if (!cmd_line)
		MoveCursor (cLINES, 0);
}

/*
 * Centre a formatted colour message at the bottom of the screen
 */
void
info_message (
	const char *fmt,
	...)
{
	va_list ap;

	va_start(ap, fmt);

	clear_message ();
#ifdef HAVE_COLOR
	fcol(tinrc.col_message);
#endif /* HAVE_COLOR */

	vsprintf (mesg, fmt, ap);

	center_line (cLINES, FALSE, mesg);	/* center the message at screen bottom */

#ifdef HAVE_COLOR
	fcol(tinrc.col_normal);
#endif /* HAVE_COLOR */
	stow_cursor();

	va_end(ap);
}


/*
 * Print a formatted colour message at the bottom of the screen, wait a while
 */
void
wait_message (
	int delay,
	const char *fmt,
	...)
{
	va_list ap;

	va_start(ap, fmt);

	clear_message ();
#ifdef HAVE_COLOR
	fcol(tinrc.col_message);
#endif /* HAVE_COLOR */

	vsprintf (mesg, fmt, ap);
	my_fputs (mesg, stdout);

#ifdef HAVE_COLOR
	fcol(tinrc.col_normal);
#endif /* HAVE_COLOR */
	cursoron ();
	my_flush();

	(void) sleep(delay);
/*	clear_message(); would be nice, but tin doesn't expect this yet */
	va_end(ap);
}


/*
 * Print a formatted message to stderr, no colour is added.
 * Interesting - this function implicitly clears 'errno'
 */
void
error_message (
	const char *fmt,
	...)
{
	va_list ap;

	va_start(ap, fmt);

	errno = 0;

	clear_message ();

	vsprintf (mesg, fmt, ap);

	my_fprintf (stderr, mesg);
	my_fflush (stderr);

	if (cmd_line) {
		my_fputc ('\n', stderr);
		fflush (stderr);
	} else {
		stow_cursor();
		(void) sleep (2);
		clear_message();
	}

	va_end(ap);
}


/*
 * Print a formatted error message to stderr, no colour is added.
 * This function implicitly clears 'errno'
 */
void
perror_message (
	const char *fmt,
	...)
{
	int err;
	va_list ap;

	err = errno;
	va_start(ap, fmt);

	clear_message ();

	vsprintf (mesg, fmt, ap);

	my_fprintf (stderr, "%s: Error: %s", mesg, strerror(err));
	errno = 0;

	if (cmd_line) {
		my_fputc ('\n', stderr);
		fflush (stderr);
	} else {
		stow_cursor();
		(void) sleep (3);
	}

	va_end(ap);
}


void
clear_message (
	void)
{
	if (!cmd_line) {
		MoveCursor (cLINES, 0);
		CleartoEOLN ();
		cursoroff ();
#ifndef USE_CURSES
		my_flush();
#endif /* !USE_CURSES */
	}
}


void
center_line (
	int line,
	t_bool inverse,
	const char *str)
{
	int pos;
	char buffer[256];

	STRCPY(buffer, str);

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
draw_arrow_mark (
	int line)
{
	MoveCursor (line, 0);

	if (tinrc.draw_arrow)
		my_fputs ("->", stdout);
	else {
#ifdef USE_CURSES
		char buffer[BUFSIZ];
		char *s = screen_contents(line, 0, buffer);
#else
		char *s = screen[line-INDEX_TOP].col;
#endif /* USE_CURSES */
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

	if (tinrc.draw_arrow)
		my_fputs ("  ", stdout);
	else {
#ifdef USE_CURSES
		char buffer[BUFSIZ];
		char *s = screen_contents(line, 0, buffer);
#else
		char *s = screen[line-INDEX_TOP].col;
#endif /* USE_CURSES */
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
		fcol(tinrc.col_title);
#endif /* HAVE_COLOR */
		/* you have mail message in */
		my_fputs ((mail_check () ? txt_you_have_mail : txt_type_h_for_help), stdout);

#ifdef HAVE_COLOR
		fcol(tinrc.col_normal);
#endif /* HAVE_COLOR */
	}
	center_line (0, TRUE, title);
}


void
ring_bell (
	void)
{
#ifdef USE_CURSES
	if (!cmd_line)
		beep();
	else {
#endif /* USE_CURSES */
	my_fputc ('\007', stdout);
	my_flush();
#ifdef USE_CURSES
	}
#endif /* USE_CURSES */
}


void
spin_cursor (
	void)
{
	static const char buf[] = "|/-\\|/-\\ "; /* don't remove the taling sapce! */
	static unsigned short int i = 0;

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
#endif /* 1 */
		my_printf ("%s%6d/%-6d", txt, count, total);
	my_flush();
}
