/*
 *  Project   : tin - a Usenet reader
 *  Module    : trace.c
 *  Author    : Thomas Dickey <dickey@clark.net>
 *  Created   : 22.03.1997
 *  Updated   : 31.12.1997
 *  Notes     : debugging support via TRACE macro.
 *  Copyright : (c) Copyright 1997-98 by Thomas Dickey
 *	             You may  freely  copy or  redistribute  this software,
 *	             so  long as there is no profit made from its use, sale
 *	             trade or  reproduction.  You may not change this copy-
 *	             right notice, and it must be included in any copy made
 */

#include "tin.h"
#include "tcurses.h"
#include "trace.h"

void
_tracef(const char *fmt, ...)
{
	static	FILE	*fp;
	va_list ap;

	if (!fp)
		fp = fopen("trace.out", "w");
	if (!fp)
		abort();

	va_start(ap,fmt);
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
_tracechar(int ch)
{
	static char result[2];
	result[0] = ch;
	return result;
}
