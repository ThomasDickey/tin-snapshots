/*
 *  Project   : tin - a Usenet reader
 *  Module    : trace.c
 *  Author    : Thomas Dickey <dickey@clark.net>
 *  Created   : 1997-03-22
 *  Updated   : 1997-12-31
 *  Notes     : debugging support via TRACE macro.
 *  Copyright : (c) Copyright 1997-99 by Thomas Dickey
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
#ifndef included_trace_h
#	include "trace.h"
#endif /* !included_trace_h */

void
_tracef(
	const char *fmt,
	...)
{
	static	FILE	*fp;
	va_list ap;

	if (!fp)
		fp = fopen("trace.out", "w");
	if (!fp)
		abort();

	va_start(ap, fmt);
	if (fmt != 0) {
		vfprintf(fp, fmt, ap);
		fputc('\n', fp);
		(void)fflush(fp);
	} else {
		(void)fclose(fp);
		(void)fflush(stdout);
		(void)fflush(stderr);
	}
	va_end(ap);
}

char *
_tracechar(
	int ch)
{
	static char result[2];
	result[0] = ch;
	return result;
}
