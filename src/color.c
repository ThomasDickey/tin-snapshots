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
#ifdef HAVE_COLOR

static int current_fcol = 7;
static int current_bcol = 0;

/*
 * Local prototypes
 */
static int isalp P_((int c));
static int check_valid_mark P_((const char *s, int c));
static int color_fputs P_((const char *s, FILE *stream, int color));


/* setting foregroundcolor */
void
fcol (color)
	int color;
{
	if (use_color) {
		if (color >= 0 && color <= 15) {
			int bold = (color >> 3);
			printf("\033[%d;%dm", bold, ((color & 7) + 30));
			if (!bold)
				bcol(current_bcol);
			current_fcol = color;
		}
	}
}

/* setting backgroundcolor */
void
bcol (color)
	int color;
{
	if (use_color) {
		if (color >= 0 && color <= 7) {
			printf("\033[%dm", (color + 40));
			current_bcol = color;
		}
	}
}

static int
isalp (c)
	int c;
{
	if (isalnum(c)) {
		return (1);
	}
	switch (c) {
		case '.':
		case ':':
		case '!':
		case '\"':		/* for emacs: " */
		case '\'':
		case ',':
		case '(':
		case ')':
		case '{':
		case '}':
		case '[':
		case ']':
		case '=':
		case '<':
		case '>':
		case '$':
		case '%':
		case '&':
		case '/':
		case '?':
#ifdef NOLOCALE
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
			return (1);

		default:
			return (0);
	}
}

static int
check_valid_mark (s, c)
	const char *s;
	int c;
{
	const char *p;

	for (p=s+1; p < (s+strlen(s)); p++) {
		if (!isalp(*p) && *(p + 1) == c) {
			return (0);
		} else {
			if (!isalp(*(p + 1)) && *p == c) {
				return (1);
			}
		}
	}
	return (0);
}

static int
color_fputs (s, stream, color)
	const char *s;
	FILE *stream;
	int color;
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
					} 
				}
				break;

			default: /* p is no mark char */
				my_fputc(*p, stream);
				break;
		}
	}
	return (1);
}

void
print_color (str)
	char *str;
{
	int color = col_text;

	if (use_color) {
		if (str[0] == '>'
		    || str[0] == '|'
		    || str[0] == ']'
/*		    || str[0] == '»' */
		    || (str[0] == ':' && str[1] != '-')
		    || (str[1] == '>' && str[0] != '-')
		    || (str[2] == '>' && str[1] != '-')
		    || (str[3] == '>' && str[2] != '-')
		    || (str[0] == ' ' && str[1] == ':' && str[2] != '-')) {
			fcol (col_quote);
			color = col_quote;
		} else {
			fcol (col_text);
		}
	}

	if (word_highlight && use_color) {
		color_fputs(str, stdout, color);
	} else {
		my_fputs(str, stdout);
	}
	my_fputs("\r\n", stdout);
}
#endif
