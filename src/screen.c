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

#include	"tin.h"

extern int errno;

char msg[LEN];
struct t_screen *screen;


void 
info_message (str)
	char *str;
{
	clear_message ();				/* Clear any old messages hanging around */
#ifdef HAVE_COLOR
	fcol(col_message);
#endif
	center_line (cLINES, FALSE, str);	/* center the message at screen bottom */
#ifdef HAVE_COLOR
	fcol(col_normal);
#endif
	if (! cmd_line) {
		MoveCursor (cLINES, 0);
	}
}


void 
wait_message (str)
	char *str;
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
	fflush (stdout);
}


void 
error_message (template, str)
	char *template;
	char *str;
{
	errno = 0;

	clear_message ();	  /* Clear any old messages hanging around */
	
	fprintf (stderr, template, str);
	fflush (stderr);

	if (cmd_line) {
		my_fputc ('\n', stderr);
		fflush (stderr);
	} else {
		MoveCursor (cLINES, 0);
		sleep (3);
	}
}


void 
perror_message (template, str)
	char *template;
	char *str;
{
#ifdef HAVE_SYSERRLIST
#	ifdef M_AMIGA
#		ifndef sys_errlist
			extern char *__sys_errlist[];
#			define sys_errlist	__sys_errlist
#		endif
#	else
#	if !(defined(BSD) && (BSD >= 199306))
		extern char *sys_errlist[];
#	endif
#	endif
#endif

	char str2[512];
	int err;
	
	clear_message ();	  /* Clear any old messages hanging around */

	sprintf (str2, template, str);
	err = errno;
#ifdef HAVE_SYSERRLIST
	fprintf (stderr, "%s: %s", str2, sys_errlist[err]);
#else
	fprintf (stderr, "%s: Error: %i", str2, err);
#endif
	errno = 0;

	if (cmd_line) {
		my_fputc ('\n', stderr);
		fflush (stderr);
	} else {
		MoveCursor (cLINES, 0);
		sleep (3);
	}
}


void 
clear_message ()
{
	if (! cmd_line) {
		MoveCursor (cLINES, 0);
		CleartoEOLN ();
		cursoroff ();
		fflush(stdout);
	}
}


void 
center_line (line, inverse, str)
	int line;
	int inverse;
	char *str;
{
	int pos;

	if (! cmd_line) {
		pos = (cCOLS - (int) strlen (str)) / 2;
		MoveCursor (line, pos);
		if (inverse) {
			StartInverse ();
		}
	}

	my_fputs (str, stdout);
	fflush (stdout);

	if (! cmd_line) {
		if (inverse) {
			EndInverse ();
		}
	}
}


void 
draw_arrow (line)
	int line;
{
	MoveCursor (line, 0);

	if (draw_arrow_mark) {
		my_fputs ("->", stdout);
		fflush (stdout);
	} else {
		StartInverse ();
		my_fputs (screen[line-INDEX_TOP].col, stdout);
		fflush (stdout);
		EndInverse ();
	}
	MoveCursor (cLINES, 0);
}


void 
erase_arrow (line)
	int line;
{
	MoveCursor (line, 0);

	if (draw_arrow_mark) {
		my_fputs ("  ", stdout);
	} else {
		EndInverse ();
		my_fputs (screen[line-INDEX_TOP].col, stdout);
	}
	fflush (stdout);
}


void 
show_title (title)
	char *title;
{	
	int col;
	
	col = (cCOLS - (int) strlen (txt_type_h_for_help))+1;
	if (col) {
		MoveCursor (0, col);
#ifdef HAVE_COLOR
		fcol(col_title);
#endif
		if (mail_check ()) {		/* you have mail message in */
			my_fputs (txt_you_have_mail, stdout);
		} else {
			my_fputs (txt_type_h_for_help, stdout);
		}
#ifdef HAVE_COLOR
		fcol(col_normal);
#endif
	}
	center_line (0, TRUE, title);
}


void 
ring_bell ()
{
	my_fputc ('\007', stdout);
	fflush (stdout);
}


void
spin_cursor ()
{
	static char *buf = "|/-\\|/-\\";
	static int i = 0;
	
	if (i > 7) {
		i = 0;
	}
	printf ("\b%c", buf[i++]);
	fflush (stdout);	
}


/*
 *  Maintain the buffer showing how many items we've processed.  Allow
 *  four digits for each number, with the possibility that it will be
 *  wider.
 */
 
void
show_progress (dst, txt, count, total)
	char *dst;
	char *txt;
	int count;
	int total;
{
	register char *s;

	if (count == 0) {
		s = txt;
		*dst = '\0';
	} else {
		for (s = dst; *s != '\0'; s++) {
			putchar ('\b');
		}
		sprintf (s = dst, "%4d/%-4d", count, total);
	}
	my_fputs (s, stdout);
	fflush (stdout);
}
