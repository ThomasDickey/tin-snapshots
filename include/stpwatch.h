/*
 *  Project   : tin - a Usenet reader
 *  Module    : stpwatch.h
 *  Author    : I. Lea
 *  Created   : 1993-08-03
 *  Updated   : 1997-04-05
 *  Notes     : Simple stopwatch routines for timing code using timeb
 *	             or gettimeofday structs
 *  Copyright : (c) Copyright 1991-99 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef STPWATCH_H
#	define STPWATCH_H 1

#	ifdef PROFILE

#		if defined(HAVE_SYS_TIMEB_H) && defined(HAVE_FTIME)
#			include <sys/timeb.h>

char msg_tb[LEN];
char tmp_tb[LEN];
struct timeb beg_tb;
struct timeb end_tb;

#			define LSECS 700000000

#			define BegStopWatch(msg)	{strcpy (msg_tb, msg); ftime (&beg_tb);}

#			define EndStopWatch()		{ftime (&end_tb);}

#			define PrintStopWatch()	{sprintf (tmp_tb, "%s: Beg=[%ld.%d] End=[%ld.%d] Elapsed=[%ld]", \
				 msg_tb, beg_tb.time, beg_tb.millitm, \
				 end_tb.time, end_tb.millitm, \
				 (((end_tb.time - LSECS) * 1000) + end_tb.millitm) - \
				 (((beg_tb.time - LSECS) * 1000) + beg_tb.millitm)); \
				 error_message (tmp_tb, "");}

#		else	/* HAVE_SYS_TIMEB_H && HAVE_FTIME */

#		ifdef	HAVE_SYS_TIME_H
#			include <sys/time.h>

char msg_tb[LEN], tmp_tb[LEN];
struct timeval beg_tb, end_tb;
float d_time;

#			define BegStopWatch(msg)	{strcpy (msg_tb, msg); \
				 (void) gettimeofday (&beg_tb, NULL);}

#			define EndStopWatch()		{(void) gettimeofday (&end_tb, NULL); \
				if ((end_tb.tv_usec -= beg_tb.tv_usec) < 0) { \
					end_tb.tv_sec--; \
					end_tb.tv_usec += 1000000; \
				 } \
				 end_tb.tv_sec -= beg_tb.tv_sec; \
				 d_time = (end_tb.tv_sec*1000.0 + ((float)end_tb.tv_usec)/1000.0);}

#			define PrintStopWatch()	{sprintf (tmp_tb, "StopWatch(%s): %6.3f ms", msg_tb, d_time); \
				 error_message (tmp_tb, "");}

#		endif /* HAVE_SYS_TIME_H */
#	endif /* HAVE_SYS_TIMEB_H && HAVE_FTIME */

#	else	/* PROFILE */

#		define BegStopWatch(msg)
#		define EndStopWatch()
#		define PrintStopWatch()

#	endif /* PROFILE */
#endif /* !STPWATCH_H */
