/*
 *  Project   : tin - a Usenet reader
 *  Module    : tcurses.c
 *  Author    : Thomas Dickey <dickey@clark.net>
 *  Created   : 1997-03-02
 *  Updated   : 1998-04-21
 *  Notes     : This is a set of wrapper functions adapting the termcap
 *	             interface of tin to use SVr4 curses (e.g., ncurses).
 * Copyright :  (c) Copyright 1997-99 by Thomas Dickey
 *	             You may  freely  copy or  redistribute  this software,
 *	             so  long as there is no profit made from its use, sale
 *	             trade or  reproduction.  You may not change this copy-
 *	             right notice, and it must be included in any copy made
 */

#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */
#ifndef TCURSES_H
#	include "tcurses.h"
#endif /* !TCURSES_H */

#ifdef USE_CURSES

#	ifndef KEY_MIN
#		define KEY_MIN KEY_BREAK	/* SVr3 curses */
#	endif /* !KEY_MIN */

#	include "trace.h"

#	ifndef attr_get
#		define attr_get()	getattrs(stdscr)
#	endif /* !attr_get */

int cLINES;
int cCOLS;

/*
 * Most of the logic corresponding to the termcap version is done in InitScreen.
 */
void setup_screen (void)
{
	cmd_line = FALSE;
#ifdef HAVE_COLOR
	bcol(tinrc.col_back);
#endif /* HAVE_COLOR */
	scrollok(stdscr, TRUE);
	set_win_size (&cLINES, &cCOLS);
}

/*
 */
int InitScreen (void)
{
#	ifdef NCURSES_VERSION
#		ifdef USE_TRACE
#			ifdef TRACE_CCALLS
	trace(TRACE_CALLS|TRACE_CCALLS);
#			else
	trace(TRACE_CALLS);
#			endif /* TRACE_CCALLS */
#		endif /* USE_TRACE */
#	endif /* NCURSES_VERSION */
	TRACE(("InitScreen"))
	initscr();
	cCOLS = COLS;
	cLINES = LINES - 1;
/*	set_win_size(&cLINES, &cCOLS);*/
	raw(); noecho(); cbreak();
	cmd_line = FALSE;	/* ...so fcol/bcol will succeed */

	set_keypad_on();
#	ifdef HAVE_COLOR
	if (has_colors()) {
		start_color();
#		ifdef HAVE_USE_DEFAULT_COLORS
		if (use_default_colors() != ERR) {
			fcol(default_fcol = -1);
			bcol(default_bcol = -1);
		}
#		endif /* HAVE_USE_DEFAULT_COLORS */
	} else {
		use_color = FALSE;
	}

	postinit_colors();
#	endif /* HAVE_COLOR */
	set_xclick_on();
	return (TRUE);
}


/*
 */
void InitWin(void)
{
	TRACE(("InitWin"))
	Raw(TRUE);
	cmd_line = FALSE;
	set_keypad_on();
}

/*
 */
void EndWin(void)
{
	TRACE(("EndWin (%d)", cmd_line))
	if (!cmd_line) {
		Raw(FALSE);
		endwin();
		cmd_line = TRUE;
	}
}

static int _inraw;

/*
 */
void Raw(
	int state)
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
	if (tinrc.inverse_okay) {
#ifdef HAVE_COLOR
		if (use_color) {
			bcol(tinrc.col_invers_bg);
			fcol(tinrc.col_invers_fg);
		} else
#endif /* HAVE_COLOR */
		{
			attrset(A_REVERSE);
		}
	}
}

static int isInverse(void)
{
#ifdef HAVE_COLOR
	if (use_color) {
		short pair = PAIR_NUMBER(getattrs(stdscr));
		short fg, bg;
		pair_content(pair, &fg, &bg);
		return (fg == tinrc.col_invers_fg) && (bg == tinrc.col_invers_bg);
	}
#endif /* HAVE_COLOR */

	return (getattrs(stdscr) & A_REVERSE);
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
	if (tinrc.inverse_okay && !cmd_line) {
#ifdef HAVE_COLOR
		fcol(tinrc.col_normal);
		bcol(tinrc.col_back);
#endif /* HAVE_COLOR */
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
void set_keypad_on (void) { if (!cmd_line) keypad(stdscr, TRUE); }

/*
 */
void set_keypad_off (void) { if (!cmd_line) keypad(stdscr, FALSE); }


/*
 * Ncurses mouse support is turned on/off when the keypad code is on/off,
 * as well as when we enable/disable the mousemask.
 */
void
set_xclick_on (void)
{
#	ifdef NCURSES_MOUSE_VERSION
	if (tinrc.use_mouse)
		mousemask(
			(BUTTON1_CLICKED|BUTTON2_CLICKED|BUTTON3_CLICKED),
			(mmask_t *)0);
#	endif /* NCURSES_MOUSE_VERSION */
}

/*
 */
void
set_xclick_off (void)
{
#	ifdef NCURSES_MOUSE_VERSION
	(void) mousemask(0, (mmask_t *)0);
#	endif /* NCURSES_MOUSE_VERSION */
}

void
MoveCursor(
	int row,
	int col)
{
	TRACE(("MoveCursor %d,%d", row, col))
	if (!cmd_line)
		move(row, col);
}


int
ReadCh(void)
{
	int ch;

	if (cmd_line)
		ch = cmdReadCh();
	else {
		allow_resize (TRUE);
		ch = getch();
		allow_resize (FALSE);
		if (need_resize) {
			handle_resize ((need_resize == cRedraw) ? TRUE : FALSE);
			need_resize = cNo;
		}
		if (ch == KEY_BACKSPACE)
			ch = '\010';	/* fix for Ctrl-H - show headers */
		else if (ch == ESC || ch >= KEY_MIN) {
			ungetch(ch);
			ch = ESC;
		}
	}
	TRACE(("ReadCh(%s)", _tracechar(ch)))
	return ch;
}

void
my_printf(
	const char *fmt,
	...)
{
	va_list ap;
	va_start(ap, fmt);
	if (cmd_line) {
		int flag = _inraw;
		if (flag)
			Raw(FALSE);
		vprintf(fmt, ap);
		if (flag)
			Raw(TRUE);
	} else {
		vwprintw(stdscr, (char *)fmt, ap);
	}
	va_end(ap);
}

void
my_fprintf(
	FILE *stream,
	const char *fmt,
	...)
{
	va_list ap;
	va_start(ap, fmt);
	TRACE(("my_fprintf(%s)", fmt))
	if (cmd_line) {
		int flag = _inraw && isatty(fileno(stream));
		if (flag)
			Raw(FALSE);
		vfprintf(stream, fmt, ap);
		if (flag)
			Raw(TRUE);
	} else {
		vwprintw(stdscr, (char *)fmt, ap);
	}
	va_end(ap);
}

void
my_fputc(
	int ch,
	FILE *fp)
{
	TRACE(("my_fputc(%s)", _tracechar(ch)))
	if (cmd_line) {
		if (_inraw && ch == '\n')
			fputc ('\r', fp);
		fputc (ch, fp);
	} else {
		addch ((unsigned char) ch);
	}
}

void
my_fputs(
	const char *str,
	FILE *fp)
{
	TRACE(("my_fputs(%s)", _nc_visbuf(str)))
	if (cmd_line) {
		if (_inraw) {
			while (*str)
				my_fputc(*str++, fp);
		} else
			fputs (str, fp);
	} else {
		addstr(str);
	}
}

void my_erase(void)
{
	TRACE(("my_erase"))

	if (!cmd_line) {
		erase();

		/* Curses doesn't actually do an erase() until refresh() is
		 * called.  Ncurses 4.0 (and lower) reset the background color
		 * when doing an erase().  So the only way to ensure we'll get
		 * the same background colors is to reset them here.
		 */
		refresh();
#ifdef HAVE_COLOR
		refresh_color();
#endif /* HAVE_COLOR */
	}
}

void
my_fflush(
	FILE *stream)
{
	if (cmd_line)
		fflush(stream);
	else {
		TRACE(("my_fflush"))
		refresh();
	}
}

/*
 * Needed if non-curses output has corrupted curses understanding of the screen
 */
void
my_retouch(void)
{
	TRACE(("my_retouch"))
	if (!cmd_line) {
		wrefresh(curscr);
	}
}

char *
screen_contents(
	int row,
	int col,
	char *buffer)
{
	int y, x;
	int len = COLS - col;
	getyx(stdscr, y, x);
	move(row, col);
	TRACE(("screen_contents(%d,%d)", row, col))
	if (innstr(buffer, len) == ERR)
		len = 0;
	buffer[len] = '\0';
	TRACE(("...screen_contents(%d,%d) %s", y, x, _nc_visbuf(buffer)))
	return buffer;
}

void
write_line(
	int row,
	char *buffer)
{
	int len = strlen(buffer);

	if (len > cCOLS)
		buffer[len = cCOLS] = '\0';
	mvaddstr(row, 0, buffer);
	if (len < cCOLS)
		clrtoeol();
}


int
get_arrow_key (
	int prech)
{
#	ifdef NCURSES_MOUSE_VERSION
	MEVENT my_event;
#	endif /* NCURSES_MOUSE_VERSION */
	int ch = getch();
	int code = KEYMAP_UNKNOWN;

	switch (ch) {
		case KEY_DC:
			code = KEYMAP_DEL;
			break;
		case KEY_IC:
			code = KEYMAP_INS;
			break;
		case KEY_UP:
			code = KEYMAP_UP;
			break;
		case KEY_DOWN:
			code = KEYMAP_DOWN;
			break;
		case KEY_LEFT:
			code = KEYMAP_LEFT;
			break;
		case KEY_RIGHT:
			code = KEYMAP_RIGHT;
			break;
		case KEY_NPAGE:
			code = KEYMAP_PAGE_DOWN;
			break;
		case KEY_PPAGE:
			code = KEYMAP_PAGE_UP;
			break;
		case KEY_HOME:
			code = KEYMAP_HOME;
			break;
		case KEY_END:
			code = KEYMAP_END;
			break;
#	ifdef NCURSES_MOUSE_VERSION
		case KEY_MOUSE:
			if (getmouse(&my_event) != ERR) {
				switch ((int) my_event.bstate) {
					case BUTTON1_CLICKED:
						xmouse = MOUSE_BUTTON_1;
						break;
					case BUTTON2_CLICKED:
						xmouse = MOUSE_BUTTON_2;
						break;
					case BUTTON3_CLICKED:
						xmouse = MOUSE_BUTTON_3;
						break;
				}
				xcol = my_event.x;	/* column */
				xrow = my_event.y;	/* row */
				code = KEYMAP_MOUSE;
			}
			break;
#	endif /* NCURSES_MOUSE_VERSION */
	}
	return code;
}

#else
void my_tcurses(void); /* proto-type */
void my_tcurses(void) { }	/* ANSI C requires non-empty file */
#endif /* USE_CURSES */
