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
	if (!use_color) {
		return;
	}
	printf("\033[%d;%dm", (color >> 3), ((color & 7) + 30));
}

/* setting backgroundcolor */
void
bcol (color)
	int color;
{
	if (!use_color) {
		return;
	}
	printf("\033[%dm", (color + 40));
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
		case '�':
		case '�':
		case '�':
		case '�':
		case '�':
		case '�':
		case '�':
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
						}
						if (word_h_display_marks) {
							my_fputc(*p, stream);
						}
				} else {
					if (word_h_display_marks) {
						my_fputc(*p, stream);
					}
					if (!isalp(*(p + 1)) && col1) {
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
					}
					if (word_h_display_marks) {
						my_fputc(*p, stream);
					}
				} else {
					if (word_h_display_marks) {
						my_fputc(*p, stream);
					}
					if (!isalp(*( p + 1)) && col2) {
						fcol(color);
					} 
				}
				break;

			default:
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
/*		    || str[0] == '�' */
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
