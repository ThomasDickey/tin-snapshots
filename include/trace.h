/*
 *  Project   : tin - a Usenet reader
 *  Module    : trace.h
 *  Author    : Thomas Dickey
 *  Created   : 22.03.97
 *  Updated   : 22.03.97
 *  Notes     : Interface of trace.c
 *
 *  Copyright 1997 by Thomas Dickey
 *		You may  freely  copy or  redistribute	this software,
 *		so  long as there is no profit made from its use, sale
 *		trade or  reproduction.  You may not change this copy-
 *		right notice, and it must be included in any copy made
 */
#ifndef	included_trace_h
#define	included_trace_h

#ifdef NCURSES_VERSION

extern char * _nc_visbuf(const char *s);

#else

#define _nc_visbuf(s) s

extern	char *_tracechar(int c);

extern	void	_tracef ( const char *, ... )
#if __GNUC__
	__attribute__ ((format(printf,1,2)))
#endif
	;
#endif

#ifdef USE_TRACE
#define TRACE(p) _tracef p;
#else
#define TRACE(p) /* nothing */
#endif

#endif	/* included_trace_h */