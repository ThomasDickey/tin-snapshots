/*
 *  Project   : tin - a Usenet reader
 *  Module    : color.c
 *  Original  : Olaf Kaluza <olaf@criseis.ruhr.de>
 *  Author    : Roland Rosenfeld <roland@spinnaker.rhein.de>
 *              Giuseppe De Marco <gdm@rebel.net> (light-colors)
 *              Julien Oster <fuzzy@cu8.cum.de> (word highlighting)
 *              T.Dickey <dickey@clark.net> (curses support)
 *  Created   : 1995-06-02
 *  Updated   : 1996-12-15
 *  Notes     : This are the basic function for ansi-color
 *              and word highlighting
 *  Copyright : (c) 1995-98 by Olaf Kalzuga and Roland Rosenfeld
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"
#include "tcurses.h"
#include "trace.h"

#ifdef HAVE_COLOR

#define MIN_COLOR -1	/* -1 is default, otherwise 0-7 or 0-15 */

int default_fcol = 7;
int default_bcol = 0;

static int current_fcol = 7;
static int current_bcol = 0;

#define isalp(c)	(isgraph ((unsigned char) (c)))


/*
 * local prototypes
 */
static t_bool check_valid_mark (const char *s);
static void put_mark_char (int c, FILE *stream);
static void color_fputs (const char *s, FILE *stream, int color);

#ifdef USE_CURSES
static void
set_colors (
	int fcolor,
	int bcolor)
{
	static struct LIST {
		struct LIST *link;
		int pair;
		int fg;
		int bg;
	} *list;
	static int nextpair;

#	ifndef HAVE_USE_DEFAULT_COLORS
	if (fcolor < 0)
		fcolor = default_fcol;
	if (bcolor < 0)
		bcolor = default_bcol;
#	endif /* !HAVE_USE_DEFAULT_COLORS */
	if (cmd_line || !use_color || !has_colors()) {
		current_fcol = default_fcol;
		current_bcol = default_bcol;
	} else if (COLORS > 1 && COLOR_PAIRS > 1) {
		chtype attribute = A_NORMAL;
		int pair = 0;

		TRACE(("set_colors (%d, %d)", fcolor, bcolor))

		/* fcolor/bcolor may be negative, if we're using ncurses
		 * function use_default_colors().
		 */
		if (fcolor > COLORS-1) {
			attribute |= A_BOLD;
			fcolor %= COLORS;
		}
		if (bcolor > 0)
			bcolor %= COLORS;

		/* curses assumes white/black */
		if (fcolor != COLOR_WHITE || bcolor != COLOR_BLACK) {
			int found = FALSE;
			struct LIST *p;

			for (p = list; p != 0; p = p->link) {
				if (p->fg == fcolor && p->bg == bcolor) {
					found = TRUE;
					break;
				}
			}
			if (found)
				pair = p->pair;
			else if (++nextpair < COLOR_PAIRS) {
				p = (struct LIST *) my_malloc(sizeof(struct LIST));
				p->fg = fcolor;
				p->bg = bcolor;
				p->pair = pair = nextpair;
				p->link = list;
				list = p;
				init_pair(pair, fcolor, bcolor);
			} else
				pair = 0;
		}

		bkgdset(attribute | COLOR_PAIR(pair) | ' ');
	}
}

void refresh_color(void)
{
	set_colors(current_fcol, current_bcol);
}
#endif /* USE_CURSES */

/* setting foreground-color */
void
fcol (
	int color)
{
	TRACE(("fcol(%d) %s", color, txt_colors[color-MIN_COLOR]))
	if (use_color) {
		if (color >= MIN_COLOR && color <= MAX_COLOR) {
#ifdef USE_CURSES
			set_colors(color, current_bcol);
#else
			int bold;
			if (color < 0)
				color = default_fcol;
			bold = (color >> 3); /* bitwise operation on signed value? ouch */
			my_printf("\033[%d;%dm", bold, ((color & 7) + 30));
			if (!bold)
				bcol(current_bcol);
#endif /* USE_CURSES */
			current_fcol = color;
		}
	}
#ifdef USE_CURSES
	else
		set_colors(default_fcol, default_bcol);
#endif /* USE_CURSES */
}

/* setting background-color */
void
bcol (
	int color)
{
	TRACE(("bcol(%d) %s", color, txt_colors[color-MIN_COLOR]))
	if (use_color) {
		if (color >= MIN_COLOR && color <= MAX_BACKCOLOR) {
#ifdef USE_CURSES
			set_colors(current_fcol, color);
#else
			if (color < 0)
				color = default_bcol;
			my_printf("\033[%dm", (color + 40));
#endif /* USE_CURSES */
			current_bcol = color;
		}
	}
#ifdef USE_CURSES
	else
		set_colors(default_fcol, default_bcol);
#endif /* USE_CURSES */
}


/*
 * Lookahead to find matching closing highlight character
 */
static t_bool
check_valid_mark (
	const char *s)
{
	const char *p;
	int c = *s;

	if (s[1] == '\0' || s[1] == c || !isalp(s[1]))
		return FALSE;
	p = strpbrk(s+2, "*_");
	return (p != NULL && *p == c && isalp(p[-1]) && !isalp(p[1]));
}


static void
put_mark_char (
	int c,
	FILE *stream)
{
	switch (word_h_display_marks) {
		case 1: /* print mark */
			my_fputc(c, stream);
			break;
		case 2: /* print space */
			my_fputc(' ', stream);
			break;
		default: /* print nothing */
			break;
	}
}


/*
 * Like fputs(), but highlights words denoted by * and _ in colour
 */
static void
color_fputs (
	const char *s,
	FILE *stream,
	int color)
{
	const char *p;
	const char* eos = strchr(s, '\0');
	t_bool hilite = FALSE;

	for (p=s; p < eos; p++) {
		if (*p == '*' || *p == '_') {
			if (! hilite) {
				if ((p == s || !isalp(p[-1])) && check_valid_mark(p)) {
					hilite = TRUE;
					fcol(*p == '*' ? col_markstar : col_markdash);
					put_mark_char(*p, stream);
				} else /* print normal character */
					my_fputc(*p, stream);
			} else {
				hilite = FALSE;
				put_mark_char(*p, stream);
				fcol(color);
			}
		} else {
			my_fputc(*p, stream);
		}
	}
}

/*
 * Output a line of text to the screen with colour if needed
 * word highlights, signatures and 'quote' lines will be coloured
 */
void
print_color (
	char *str,
	t_bool signature)
{
	int color = col_text;

	if (use_color) {
		if (signature) {
			fcol (col_signature);
			color = col_signature;
		} else {
			if (str[0] == '>'
				 || str[0] == '|'
				 || str[0] == ']'
/*				 || str[0] == '»' */
				 || (str[0] == ':' && str[1] != '-')
				 || (str[1] == '>' && str[0] != '-')
				 || (str[2] == '>' && str[1] != '-')
				 || (str[3] == '>' && str[2] != '-')
				 || (str[0] == ' ' && str[1] == ':' && str[2] != '-')) {
				fcol (col_quote);
				color = col_quote;
			} else if (in_headers) {
				color = col_newsheaders;
				fcol (col_newsheaders);
			} else
				fcol (col_text);
		}
	}

	if (word_highlight && use_color)
		color_fputs(str, stdout, color);
	else
		my_fputs(str, stdout);

	my_fputs(cCRLF, stdout);
}
#endif
