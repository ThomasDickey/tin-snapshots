/*
 *  Project   : tin - a Usenet reader
 *  Module    : tcurses.c
 *  Author    : Thomas Dickey
 *  Created   : 02.03.97
 *  Updated   : 02.03.97
 *  Notes     : This is a set of wrapper functions adapting the termcap
 *		interface of tin to use SVr4 curses (e.g., ncurses).
 *
 *  Copyright 1997 by Thomas Dickey
 *		You may  freely  copy or  redistribute	this software,
 *		so  long as there is no profit made from its use, sale
 *		trade or  reproduction.  You may not change this copy-
 *		right notice, and it must be included in any copy made
 */

#include "tin.h"
#include "tcurses.h"

#if USE_CURSES

#include <stdarg.h>

#ifndef attr_get
#define	attr_get() getattrs(stdscr)
#endif

int cLINES;
int cCOLS;
t_bool inverse_okay = TRUE;

/*
 */
void setup_screen (void) {
	/* FIXME */
	cmd_line = FALSE;
	bcol(col_back);
}

/*
 */
int InitScreen (void)
{
#if 0
	/*trace(TRACE_CALLS|TRACE_MOVE|TRACE_UPDATE);*/
	trace(TRACE_MAXIMUM);
#endif
	initscr();
	set_win_size(&cLINES, &cCOLS);
	cCOLS = COLS;
	cLINES = LINES - 1;
	raw(); noecho(); cbreak();

	keypad(stdscr, TRUE);
	if (has_colors()) {
		start_color();
	}
#ifdef NCURSES_VERSION
	(void) mousemask(
		(BUTTON1_CLICKED|BUTTON2_CLICKED|BUTTON3_CLICKED),
		(mmask_t *)0);
#endif
	return (TRUE);
}


/*
 */
void InitWin(void)
{
	/* FIXME */
}

/*
 */
void EndWin(void)
{
	endwin();
}

static int _inraw;

/*
 */
void Raw(int state)
{
	if (state && !_inraw) {
		reset_prog_mode();
		_inraw = TRUE;
	} else if (!state && _inraw) {
		reset_shell_mode();
		_inraw = FALSE;
	}
}

/*
 */
int RawState(void)
{
	return _inraw;
}


/*
 */
void StartInverse(void)
{
	if (inverse_okay) {
		if (use_color) {
			bcol(col_invers_bg);
			fcol(col_invers_fg);
		} else {
			attrset(A_REVERSE);
		}
	}
}

/*
 */
void ToggleInverse(void)
{
	if (attr_get() & A_REVERSE)
		EndInverse();
	else
		StartInverse();
}

/*
 */
void EndInverse(void)
{
	if (inverse_okay) {
		fcol(col_normal);
		bcol(col_back);
		attroff(A_REVERSE);
	}
}


/*
 */
void cursoron (void) { /* FIXME */ }

/*
 */
void cursoroff (void) { /* FIXME */ }


/*
 */
void set_keypad_on (void) { /* FIXME */ }

/*
 */
void set_keypad_off (void) { /* FIXME */ }


/*
 */
void set_xclick_on (void) { /* FIXME */ }

/*
 */
void set_xclick_off (void) { /* FIXME */ }

void
MoveCursor(int row, int col)
{
	/* _tracef("MoveCursor(%d,%d)", row, col);*/
	move(row,col);
}


int
ReadCh(void)
{
	int ch;

	if (cmd_line)
		ch = cmdReadCh();
	else {
		ch = getch();
		if (ch == ESC || ch >= KEY_MIN) {
			ungetch(ch);
			ch = ESC;
		}
	}
	return ch;
}

void
my_printf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if (cmd_line)
		vprintf(fmt, ap);
	else
		vwprintw(stdscr, fmt, ap);
	va_end(ap);
}

void
my_fprintf(FILE *stream, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if (cmd_line)
		vfprintf(stream, fmt, ap);
	else
		vwprintw(stdscr, fmt, ap);
	va_end(ap);
}

void
my_fputc(int ch, FILE *fp)
{
	if (cmd_line)
		fputc (ch, fp);
	else
		addch (ch);
}

void
my_fputs(const char *str, FILE *fp)
{
	if (cmd_line)
		fputs (str, fp);
	else
		addstr(str);
}

void my_erase()
{
	erase();

	/* FIXME:  curses doesn't actually do an erase() until refresh() is
	 * called.  Ncurses 4.0 (and lower) reset the background color when
	 * doing an erase().  So the only way to ensure we'll get the same
	 * background colors is to reset them here.
	 *
	 * (Need to verify if SVr4 does the same thing).
	 */
	refresh();
	refresh_color();
}

void
my_fflush(FILE *stream)
{
	if (cmd_line)
		fflush(stream);
	else
		refresh();
}

char *
screen_contents(int row, int col, char *buffer)
{
	int y, x;
	int len = BUFSIZ-1;
	getyx(stdscr, y, x);
	move(row, col);
	/* _tracef("screen_contents(%d,%d)", row, col);*/
	if (innstr(buffer, len) == ERR)
		len = 0;
	buffer[len] = '\0';
	/* FIXME move(y, x); */
	/* _tracef("...screen_contents(%d,%d) %s", y, x, _nc_visbuf(buffer));*/
	return buffer;
}

#else

void my_dummy(void) { }	/* ANSI C requires non-empty file */

#endif
