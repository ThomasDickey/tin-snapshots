/*
 *  Project   : tin - a Usenet reader
 *  Module    : read.c
 *  Author    : Jason Faultless <jason@radar.demon.co.uk>
 *  Created   : 1997-04-10
 *  Updated   : 1997-04-15
 *  Copyright : (c) 1997-98 by Jason Faultless
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"

/*
 * Read oversized lines in blocks of this size
 */
#define CHUNK					256

/*
 * Global error flag. Set if something abnormal happens during file I/O
 */
int tin_errno;

/*
 * local prototypes
 */
static char * tin_read (char *buffer, size_t len, FILE *fp);
#ifdef NNTP_ABLE
	static t_bool wait_for_input (FILE *fd);
#endif

#ifdef NNTP_ABLE
/*
 * Used by the I/O read routine to look for keyboard input
 * Returns TRUE if user aborted with 'q' or 'z' (lynx-style)
 *         FALSE otherwise
 * Exits via tin_done() on irrecoverable errors
 */
static t_bool
wait_for_input (
	FILE *fd)
{
#ifdef VMS
	int ch = ReadChNowait ();

	if (ch == 'q' || ch == 'z' || ch == ESC) {
		if (prompt_yn (cLINES, "Do you want to abort this operation? (y/n): ", FALSE) == 1)
			return (TRUE);
	}
	if (ch == 'Q') {
		if (prompt_yn (cLINES, "Do you want to exit tin immediately ? (y/n): ", FALSE) == 1)
			tin_done (EXIT_ERROR);
	}
#else
	int nfds, ch;
	fd_set readfds;
	struct timeval tv;

	/*
	 * Main loop. Wait for input from keyboard or file or for a timeout.
	 */
	forever {

		FD_ZERO(&readfds);
		FD_SET(STDIN_FILENO, &readfds);
/*		FD_SET(fileno(fd), &readfds);*/

		tv.tv_sec = 0;		/*NNTP_READ_TIMEOUT;*/
		tv.tv_usec = 0;

/*DEBUG_IO((stderr, "waiting on %d and %d...", STDIN_FILENO, fileno(fd)));*/
		if ((nfds = select(STDIN_FILENO+1, &readfds, NULL, NULL, &tv)) == -1) {
			if (errno != EINTR) {
				perror_message("select() failed");
				tin_done(EXIT_ERROR);
			}
		}

		/* No input pending */
		if (nfds == 0) {
/*			fprintf(stderr, "Timeout...Treating as user initiated abort ? \n");*/
/* TODO What do we do here ? Drain ? don't drain ? */
			return(FALSE);
		}

		/*
		 * Something is waiting. See what's cooking...
		 */
		if (nfds > 0) {

			/*
			 * User pressed something. If 'q'uit, then handle this. Process
			 * user input 1st so they get chance to quit on busy (or stalled) reads
			 */
			if (FD_ISSET(STDIN_FILENO, &readfds)) {
DEBUG_IO((stderr, "keybd ready\n"));
				ch = ReadCh();

				if (ch == 'q' || ch == 'z' || ch == ESC) {
					if (prompt_yn (cLINES, "Do you want to abort this operation? (y/n): ", FALSE) == 1)
/* TODO if(cmd_line) this is all cacked when not in curses mode */
						return (TRUE);
				}

				if (ch == 'Q') {
					if (prompt_yn (cLINES, "Do you want to exit tin immediately ? (y/n): ", FALSE) == 1)
						tin_done (EXIT_ERROR);
				}

			}

#if 0
			/*
			 * Our file has something for us to read
			 */
			if (FD_ISSET(fileno(fd), &readfds)) {
DEBUG_IO((stderr, "file ready\n"));
				return(TRUE);
			}
#endif
		}

	}
#endif
	/* NOTREACHED */
	return(FALSE);
}
#endif /* NNTP_ABLE */

/*
 * Support routine to read a fixed size buffer. This does most of the
 * hard work for tin_fgets()
 */
int partial_read;

static char *
tin_read (
	char *buffer,
	size_t len,
	FILE *fp)
{
	int i;
	char *ptr;

	partial_read = FALSE;

#ifdef NNTP_ABLE
	if (fp == (FILE*)nntp_rd_fp)
		if (wait_for_input(fp)) {			/* Check if okay to read */
			info_message("Aborting read, please wait...");
			drain_buffer(fp);
			clear_message();
			tin_errno = TIN_ABORT;
			fflush(stdin);
			return(NULL);
		}

	/*
	 * Initially try and fit into supplied buffer
	 */
	if (fp == (FILE*)nntp_rd_fp)
		ptr = get_server(buffer, len);
	else
#endif /* NNTP_ABLE */
		ptr = fgets (buffer, len, fp);

/* TODO check system errno here ?  */

	if (ptr == 0)						/* End of file ? */
		return(NULL);

	/*
	 * Was this only a partial read ?
	 * We strip trailing \r and \n here and here _only_
	 */
	if (buffer[i = strlen(buffer) - 1] == '\n') {
		if (buffer[i-1] == '\r')
			*(buffer+i-1) = '\0';
		else
			*(buffer+i) = '\0';
	} else
		partial_read = TRUE;


	/*
	 * Do processing of leading . for NNTP case here and here _only_
	 */
#ifdef NNTP_ABLE
	if (fp != (FILE*)nntp_rd_fp)
#endif /* NNTP_ABLE */
		return(buffer);

	if (STRCMPEQ(buffer, "."))		/* end of text */
		return(NULL);

	/* reduce leading .'s */
	return ((buffer[0] == '.') ? (buffer+1) : buffer);
}

/*
 * This is the main routine for reading news data from local spool or NNTP.
 * It can handle arbitrary length lines of data, failed connections and
 * user initiated aborts (where possible)
 *
 * Similar to fgets(), except that \n is prestripped and the returned pointer
 * should always be used, and not the passed buffer.
 * [ The passed buffer will hold partially processed data, and will be
 *   incomplete if the read overran. ]
 *
 * If reading is via a socket to an NNTP server, then the required
 * post-processing of the data will be done such that we look like
 * fgets() to the caller.
 *
 * Trailing \r and \n will be stripped.
 *
 * Dynamic read code based on code by <emcmanus@gr.osf.org>
 *
 * Despite the large amount of code here, the code path for a trivial read
 * is still very short.
 */

char *
tin_fgets (
	char *buffer,
	size_t len,
	FILE *fp)
{
	static char *dynbuf = NULL;

	char *temp;
	int next;

	tin_errno = 0;			/* Clear errors */

	temp = tin_read(buffer, len, fp);

DEBUG_IO((stderr, "tin_fgets (%s)\n", (temp) ? temp : "NULL"));

	if (tin_errno != 0) {
DEBUG_IO((stderr, "Aborted read\n"));
		return(NULL);
	}

	if (!partial_read)		/* We have all the data with no errors. Cool */
		return(temp);

	if (dynbuf != NULL)
		free(dynbuf);				/* Free any previous allocation */

	dynbuf = (char *) my_malloc (1 + CHUNK + len * sizeof(char));
	strncpy(dynbuf, temp, len);
	next = len - 1;

	forever {			/* TODO poss. revamp while(partial_read) */
		temp = tin_read (dynbuf + next, CHUNK, fp); /* If == 0 ?? */

		if (tin_errno != 0) {
			free(dynbuf);
			return(NULL);
		}

		if (!partial_read) {
DEBUG_IO((stderr, "Oversized read !%s!\n", dynbuf));
				return(dynbuf);
		}

		next += CHUNK - 1;

		temp = (char *) my_realloc (dynbuf, (next+CHUNK) * sizeof(char));
		dynbuf = temp;
	}
}

/*
 * We can't just stop reading a socket once we are through with it. This drains out any
 * pending data on the NNTP port
 */
void
drain_buffer (
	FILE *fp)
{
#ifdef NNTP_ABLE
	char buf[NNTP_STRLEN];
	int i=0;

	if (!read_news_via_nntp)
		return;

DEBUG_IO((stderr, "Draining\n"));
	while (tin_read(buf, sizeof(buf), fp) != (char *) 0) {
DEBUG_IO((stderr, "Drain %s\n", buf));
		if (++i % MODULO_COUNT_NUM == 0)
			spin_cursor();
	}

#endif
}

/* It works the same way as fgets except that it converts
 * new line character into ' ' if the first character following it
 * is white space(' ' or '\t'). It's used by rfc1521.c
 * to concatenate multiple line header field into a single line.
 * It also removes leading white spaces in continuation header lines
 * J. Shin
 */
char *
fgets_hdr (
	char *s,
	size_t size,
	FILE *f)
{
	char *s1 = s;
	int c = 0;
	int is_leading_wsp = 0;

	c = fgetc(f);
	*s1 = (int) c;

	while ((size_t) (s1-s) < size-2 && c != EOF) {
		if (*s1 == '\n' || *s1 == '\r') {
			if (!is_leading_wsp) {
				is_leading_wsp = 1;
				c = fgetc(f);
				*(++s1) = (char) c;
			} else {
				c = (int) *s1;
				ungetc(c,f);
				s1--;
				break;
			}
		}
		else if (is_leading_wsp) {
			if (*s1 == ' ' || *s1 == '\t') {
				*(s1-1) = ' '; /* convert newline to space */
				/* remove leading wsp in continuation header lines */
				do {
					c = fgetc(f);
					*s1 = (char) c;
				} while (c != EOF && (*s1 == '\t' || *s1== ' '));
				is_leading_wsp = 0;
				continue;
			} else {
				c = (int) *s1;
				ungetc(c,f);
				s1--;
				break;
			}
		} else {
			c = fgetc(f);
			*(++s1) = (char) c;
		}
	}

	if (c == EOF)
		s1--;

	*(++s1) = '\0';

	return ((s1 == s) ? NULL : s);
}

/* end of read.c */
