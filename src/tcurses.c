/*
 *  Project   : tin - a Usenet reader
 *  Module    : tcurses.c
 *  Author    : Thomas Dickey
 *  Created   : 02.03.97
 *  Updated   : 02.09.97
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

#ifndef KEY_MIN
#define KEY_MIN KEY_BREAK	/* SVr3 curses */
#endif

#include <stdarg.h>

#include "trace.h"

#ifndef attr_get
#define	attr_get() getattrs(stdscr)
#endif

int cLINES;
int cCOLS;
t_bool inverse_okay = TRUE;

/*
 */
void setup_screen (void)
{
	/* FIXME */
	cmd_line = FALSE;
	bcol(col_back);
}

/*
 */
int InitScreen (void)
{
#ifdef NCURSES_VERSION
#ifdef USE_TRACE
	trace(TRACE_CALLS|TRACE_CCALLS);
#endif
#endif
	TRACE(("InitScreen"))
	initscr();
	cCOLS = COLS;
	cLINES = LINES - 1;
	set_win_size(&cLINES, &cCOLS);
	raw(); noecho(); cbreak();
	cmd_line = FALSE;	/* ...so fcol/bcol will succeed */

	keypad(stdscr, TRUE);
	if (has_colors()) {
		start_color();
#ifdef HAVE_USE_DEFAULT_COLORS
		if (use_default_colors() != ERR) {
			fcol(default_fcol = -1);
			bcol(default_bcol = -1);
		}
#endif
	}
	postinit_colors();
#ifdef NCURSES_MOUSE_VERSION
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
	TRACE(("InitWin"))
	Raw(TRUE);		/* FIXME */
	cmd_line = FALSE;
}

/*
 */
void EndWin(void)
{
	TRACE(("EndWin (%d)", cmd_line))
	if (!cmd_line) {
		Raw(FALSE);		/* FIXME */
		endwin();
		cmd_line = TRUE;
	}
}

static int _inraw;

/*
 */
void Raw(int state)
{
	if (state && !_inraw) {
		TRACE(("reset_prog_mode"))
		reset_prog_mode();
		_inraw = TRUE;
	} else if (!state && _inraw) {
		TRACE(("reset_shell_mode"))
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

static int isInverse(void)
{
	if (use_color) {
		short pair = PAIR_NUMBER(attr_get());
		short fg, bg;
		pair_content(pair, &fg, &bg);
		return (fg == col_invers_fg) && (bg == col_invers_bg);
	}

	return (attr_get() & A_REVERSE);
}

/*
 */
void ToggleInverse(void)
{
	if (isInverse())
		EndInverse();
	else
		StartInverse();
}

/*
 */
void EndInverse(void)
{
	if (inverse_okay && !cmd_line) {
		fcol(col_normal);
		bcol(col_back);
		attroff(A_REVERSE);
	}
}


/*
 */
void cursoron (void) { if (!cmd_line) curs_set(1); }

/*
 */
void cursoroff (void) { if (!cmd_line) curs_set(0); }


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
	TRACE(("MoveCursor %d,%d", row,col))
	if (!cmd_line)
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
		if (ch == KEY_BACKSPACE)
			ch = '\010';    /* fix for Ctrl-H - show headers */
		else if (ch == ESC || ch >= KEY_MIN) {
			ungetch(ch);
			ch = ESC;
		}
	}
	TRACE(("ReadCh(%s)", _tracechar(ch)))
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
	TRACE(("my_fprintf(%s)", fmt))
	if (cmd_line)
		vfprintf(stream, fmt, ap);
	else
		vwprintw(stdscr, fmt, ap);
	va_end(ap);
}

void
my_fputc(int ch, FILE *fp)
{
	TRACE(("my_fputc(%s)", _tracechar(ch)))
	if (cmd_line)
		fputc (ch, fp);
	else
		addch ((unsigned char) ch);
}

void
my_fputs(const char *str, FILE *fp)
{
	TRACE(("my_fputs(%s)", _nc_visbuf(str)))
	if (cmd_line)
		fputs (str, fp);
	else
		addstr(str);
}

void my_erase()
{
	TRACE(("my_erase"))

	if (!cmd_line) {
		erase();

		/* FIXME:  curses doesn't actually do an erase() until
		 * refresh() is called.  Ncurses 4.0 (and lower) reset the
		 * background color when doing an erase().  So the only way to
		 * ensure we'll get the same background colors is to reset them
		 * here.
		 *
		 * (Need to verify if SVr4 does the same thing).
		 */
		refresh();
		refresh_color();
	}
}

void
my_fflush(FILE *stream)
{
	if (cmd_line)
		fflush(stream);
	else {
		TRACE(("my_fflush"))
		refresh();
	}
}

void
my_retouch(void)
{
	TRACE(("my_retouch"))
	if (!cmd_line) {
		wrefresh(curscr);
	}
}

char *
screen_contents(int row, int col, char *buffer)
{
	int y, x;
	int len = COLS - col;
	getyx(stdscr, y, x);
	move(row, col);
	TRACE(("screen_contents(%d,%d)", row, col))
	if (innstr(buffer, len) == ERR)
		len = 0;
	buffer[len] = '\0';
	/* FIXME move(y, x); */
	TRACE(("...screen_contents(%d,%d) %s", y, x, _nc_visbuf(buffer)))
	return buffer;
}

#else

void my_dummy(void) { }	/* ANSI C requires non-empty file */

#endif
