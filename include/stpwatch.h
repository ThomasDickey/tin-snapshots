/*
 *  Project   : tin - a Usenet reader
 *  Module    : stpwatch.h
 *  Author    : I.Lea
 *  Created   : 03-08-93
 *  Updated   : 03-08-93
 *  Notes     : Simple stopwatch routines for timing code using timeb struct
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifdef PROFILE

#include <sys/timeb.h>

char msg_tb[1024];
char tmp_tb[1024];
struct timeb beg_tb;
struct timeb end_tb;

#define LSECS 700000000

#define BegStopWatch(msg) \
	{ \
		strcpy (msg_tb, msg); \
		ftime (&beg_tb); \
	}
	
#define EndStopWatch() \
	{ \
		ftime (&end_tb); \
	}

#define PrintStopWatch() \
	{ \
		sprintf (tmp_tb, "%s: Beg=[%ld.%d] End=[%ld.%d] Elapsed=[%ld]", \
			msg_tb, beg_tb.time, beg_tb.millitm, \
			end_tb.time, end_tb.millitm, \
			(((end_tb.time - LSECS) * 1000) + end_tb.millitm) - \
			(((beg_tb.time - LSECS) * 1000) + beg_tb.millitm)); \
		error_message (tmp_tb, ""); \
	}

#else

#define BegStopWatch(msg)
#define EndStopWatch()
#define PrintStopWatch()
	
#endif	/* PROFILE */
