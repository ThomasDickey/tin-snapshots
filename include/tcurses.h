/*
 *  Project   : tin - a Usenet reader
 *  Module    : tcurses.h
 *  Author    : Thomas Dickey <dickey@clark.net>
 *  Created   : 1997-03-02
 *  Updated   : 1997-03-02
 *  Notes     : curses #include files, #defines & struct's
 *  Copyright : (c) Copyright 1997 by Thomas Dickey
 *	             You may  freely  copy or  redistribute  this software,
 *	             so  long as there is no profit made from its use, sale
 *	             trade or  reproduction.  You may not change this copy-
 *	             right notice, and it must be included in any copy made
 */

#ifndef TCURSES_H
#define TCURSES_H 1


#if defined(USE_CURSES) || defined(NEED_CURSES_H)
#	ifdef HAVE_NCURSES_H
#		include <ncurses.h>
#	else
#		undef TRUE
#		undef FALSE
#		include <curses.h>
#		ifndef FALSE
#			define FALSE	0
#		endif /* !FALSE */
#		ifndef TRUE
#			define TRUE	(!FALSE)
#		endif /* !TRUE */
#	endif /* HAVE_NCURSES_H */
#endif /* USE_CURSES || NEED_CURSES_H */

#ifdef USE_CURSES

#ifdef USE_TRACE
#	ifdef HAVE_NOMACROS_H
#		include <nomacros.h>
#	endif /* HAVE_NOMACROS_H */
#endif /* USE_TRACE */

#if 0	/* FIXME: this has prototypes, but opens up new problems! */
#ifdef HAVE_TERM_H
#	include <term.h>
#endif /* HAVE_TERM_H */
#endif /* 0 */

#define cCRLF				"\n"
#define my_flush()			my_fflush(stdout)
#define ClearScreen()			my_erase()
#define CleartoEOLN()			clrtoeol()
#define CleartoEOS()			clrtobot()
#define WriteLine(row,buffer)		write_line(row,buffer)

#define HpGlitch(func)			/*nothing*/

extern int cmdReadCh (void);

extern char *screen_contents(int row, int col, char *buffer);
extern void MoveCursor(int row,int col);
extern void my_erase(void);
extern void my_fflush(FILE *stream);
extern void my_fputc(int ch, FILE *stream);
extern void my_fputs(const char *str, FILE *stream);
extern void my_fprintf(FILE *stream, const char *fmt, ...)
#ifdef __GNUC__
	__attribute__((format(printf,2,3)))
#endif /* __GNUC__ */
	;
extern void my_printf(const char *fmt, ...)
#ifdef __GNUC__
	__attribute__((format(printf,1,2)))
#endif /* __GNUC__ */
	;
extern void my_retouch(void);
extern void refresh_color(void);
extern void write_line(int row, char *buffer);

#else	/* !USE_CURSES */

#ifdef HAVE_TERMCAP_H
#	include <termcap.h>
#endif /* HAVE_TERMCAP_H */

#define cCRLF				"\r\n"

#define my_fputc(ch, stream)		fputc (ch, stream)
#define my_fputs(str, stream)		fputs (str, stream)

#define my_printf			printf
#define my_fprintf			fprintf
#define my_flush()			fflush(stdout)
#define my_fflush(stream)		fflush(stream)
#define my_retouch()			ClearScreen()
#define WriteLine(row,buffer)		/*nothing*/

#define HpGlitch(func)			if (_hp_glitch) func

#endif /* USE_CURSES/!USE_CURSES */

extern void my_dummy(void);

#endif /* !TCURSES_H */
