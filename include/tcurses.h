/*
 *  Project   : tin - a Usenet reader
 *  Module    : tcurses.h
 *  Author    : Thomas Dickey
 *  Created   : 02.03.97
 *  Updated   : 02.03.97
 *  Notes     : #include files, #defines & struct's
 *
 *  Copyright 1997 by Thomas Dickey
 *		You may  freely  copy or  redistribute	this software,
 *		so  long as there is no profit made from its use, sale
 *		trade or  reproduction.  You may not change this copy-
 *		right notice, and it must be included in any copy made
 */

#ifndef TCURSES_H
#define TCURSES_H 1

#if USE_CURSES

#ifdef HAVE_NCURSES_H
#include <ncurses.h>
#else
#include <curses.h>
#endif

#if USE_TRACE
#if HAVE_NOMACROS_H
#include <nomacros.h>
#endif
#endif

#define cCRLF				"\n"
#define my_flush()			my_fflush(stdout)
#define ClearScreen()			my_erase()
#define CleartoEOLN()			clrtoeol()
#define CleartoEOS()			clrtobot()

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
#endif
	;
extern void my_printf(const char *fmt, ...)
#ifdef __GNUC__
	__attribute__((format(printf,1,2)))
#endif
	;
extern void my_retouch(void);
extern void refresh_color(void);

#else	/* !USE_CURSES */

#define cCRLF				"\r\n"

#define	my_fputc(ch, stream)		fputc (ch, stream)
#define	my_fputs(str, stream)		fputs (str, stream)

#define my_printf			printf
#define my_fprintf			fprintf
#define my_flush()			fflush(stdout)
#define my_fflush(stream)		fflush(stream)
#define my_retouch()			ClearScreen()

#define HpGlitch(func)			if (_hp_glitch) func

#endif	/* USE_CURSES/!USE_CURSES */

extern void my_dummy(void);

#endif /* !TCURSES_H */
