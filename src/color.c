/*
 *  Project   : tin - a Usenet reader
 *  Module    : color.c
 *  Original  : Olaf Kaluza <olaf@criseis.ruhr.de>
 *  Author    : Roland Rosenfeld <roland@spinnaker.rhein.de>
 *              Giuseppe De Marco <gdm@rebel.net> (light-colors)
 *              Julien Oster <fuzzy@cu8.cum.de> (word highlighting)
 *  Created   : 02-06-95
 *  Updated   : 06-03-95, 30-03-96, 22-04-96, 15-12-96
 *  Notes     : This are the basic function for ansi-color
 *              and word highlightning
 *  Copyright : (c) 1995 by Olaf Kalzuga and Roland Rosenfeld
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"
#include "tcurses.h"

#ifdef HAVE_COLOR

static int current_fcol = 7;
static int current_bcol = 0;

/*
 * local prototypes
 */
static t_bool check_valid_mark (const char *s, int c);
static t_bool isalp (int c);
static void color_fputs (const char *s, FILE *stream, int color);
#if USE_CURSES
	static void set_colors (int fcolor, int bcolor);
#endif /* USE_CURSES */

#if USE_CURSES
static void
set_colors (
	int fcolor,
	int bcolor)
{
	static bool FIXME[64];

	if (!cmd_line) {
		chtype attribute = A_NORMAL;
		int pair;

		if (fcolor > 7) {
			fcolor &= 7;
			attribute |= A_BOLD;
		}
		bcolor &= 7;
		pair = (fcolor * 8) + bcolor;	/* FIXME: assumes 64-colors */
#if 0		/* FIXME: curses assumes white/black */
		if (fcolor == COLOR_WHITE
		 && bcolor == COLOR_BLACK)
			pair = 0;
#endif
		if ((pair != 0)
		 && FIXME[pair] == FALSE) {
			init_pair(pair, fcolor, bcolor);
			FIXME[pair] = TRUE;
		}

		bkgdset(attribute | COLOR_PAIR(pair) | ' ');
	}
}

void refresh_color(void)
{
	set_colors(current_fcol, current_bcol);
}
#endif /* USE_CURSES */

/* setting foregroundcolor */
void
fcol (
	int color)
{
	if (use_color) {
		if (color >= 0 && color <= 15) {
#if USE_CURSES
			set_colors(color, current_bcol);
#else
			int bold = (color >> 3);
			my_printf("\033[%d;%dm", bold, ((color & 7) + 30));
			if (!bold)
				bcol(current_bcol);
#endif /* USE_CURSES */
			current_fcol = color;
		}
	}
#if USE_CURSES
	else set_colors(7, 0);
#endif /* USE_CURSES */
}

/* setting backgroundcolor */
void
bcol (
	int color)
{
	if (use_color) {
		if (color >= 0 && color <= 7) {
#if USE_CURSES
			set_colors(current_fcol, color);
#else
			my_printf("\033[%dm", (color + 40));
#endif
			current_bcol = color;
		}
	}
#if USE_CURSES
	else set_colors(7, 0);
#endif
}

static t_bool
isalp (
	int c)
{
	if (isalnum(c)) {
		return TRUE;
	}
	switch (c) {
/*		case '.': */
		case ':':
		case '!':
		case '\"':		/* for emacs: " */
		case '\'':
/*		case ',': */
		case '(':
		case ')':
		case '{':
		case '}':
		case '[':
		case ']':
		case '=':
		case '<':
/*		case '>': */
		case '$':
		case '%':
		case '&':
/*		case '/': */
		case '?':
#ifdef NO_LOCALE
		/* if you have not installed locale and want to reed german umlauts: */
		case 'ß':
		case 'ä':
		case 'ö':
		case 'ü':
		case 'Ä':
		case 'Ö':
		case 'Ü':
#endif
		case '+':
		case '-':
		case '#':
		case ';':
		case '@':
		case '\\':
			return TRUE;

		default:
			return FALSE;
	}
}

/*
 * Lookahead to find matching closing highlight character
 */
static t_bool
check_valid_mark (
	const char *s,
	int c)
{
	const char *p;

	for (p=s+2; p < (s+strlen(s)); p++) {
		if (!isalp(*p) && *(p + 1) == c) {
			return FALSE;
		} else {
			if (!isalp(*(p + 1)) && *p == c) {
				return TRUE;
			}
		}
	}
	return FALSE;
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
	int col1=0, col2=0;

	for (p=s; p < (s + strlen(s)); p++) {
		switch (*p) {
			case '*':
				if ((p > s) && !isalp(*(p - 1)) && !isalp(*(p + 1))) {
					my_fputc(*p, stream);
					break;
				} else if ((p == s) || !isalp(*(p - 1))) {
					if (check_valid_mark(p, '*')) {
						col1=1;
						fcol(col_markstar);
						switch (word_h_display_marks) {
						case 1: /* print mark */
							my_fputc(*p, stream);
							break;
						case 2: /* print space */
							my_fputc(' ', stream);
							break;
						default: /* print nothing */
							break;
						}
					} else { /* print normal character */
						my_fputc(*p, stream);
					}
				} else {
					if (!isalp(*(p + 1)) && col1) {
						switch (word_h_display_marks) {
						case 1: /* print mark */
							my_fputc(*p, stream);
							break;
						case 2: /* print space */
							my_fputc(' ', stream);
							break;
						default: /* print nothing */
							break;
						}
						fcol(color);
					} else {
						my_fputc(*p, stream);
					}
				}
				break;

			case '_':
				if ((p > s) && !isalp(*(p - 1)) && !isalp(*(p + 1))) {
					my_fputc(*p, stream);
					break;
				} else if ((p == s) || !isalp(*(p - 1))) {
					if (check_valid_mark(p, '_')) {
						col2=1;
						fcol(col_markdash);
						switch (word_h_display_marks) {
						case 1: /* print mark */
							my_fputc(*p, stream);
							break;
						case 2: /* print space */
							my_fputc(' ', stream);
							break;
						default: /* print nothing */
							break;
						}
					} else { /* print normal character */
						my_fputc(*p, stream);
					}
				} else {
					if (!isalp(*( p + 1)) && col2) {
						switch (word_h_display_marks) {
						case 1: /* print mark */
							my_fputc(*p, stream);
							break;
						case 2: /* print space */
							my_fputc(' ', stream);
							break;
						default: /* print nothing */
							break;
						}
						fcol(color);
					} else {
						my_fputc(*p, stream);
					}
				}
				break;

			default: /* p is no mark char */
				my_fputc(*p, stream);
				break;
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
/*		    	 || str[0] == '»' */
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
			} else {
				fcol (col_text);
			}
		}
	}

	if (word_highlight && use_color) {
		color_fputs(str, stdout, color);
	} else {
		my_fputs(str, stdout);
	}
	my_fputs(cCRLF, stdout);
}
#endif
