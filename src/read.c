/*
 *  Project   : tin - a Usenet reader
 *  Module    : read.c
 *  Author    : Jason Faultless <jason@radar.tele2.co.uk>
 *  Created   : 1997-04-10
 *  Updated   : 1998-07-04
 *  Copyright : (c) 1997-99 by Jason Faultless
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */

/*
 * The initial and expansion sizes to use for allocating read data
 */
#define INIT					512
#define CHUNK					256

/*
 * Global error flag. Set if something abnormal happens during file I/O
 */
int tin_errno;

/* How many chars we read at last tin_read() */
static int offset = 0;

/*
 * local prototypes
 */
static char * tin_read (char *buffer, size_t len, FILE *fp, t_bool header);
#ifdef NNTP_ABLE
	static t_bool wait_for_input (void /*FILE *fd*/);
#endif /* NNTP_ABLE */


#ifdef NNTP_ABLE
/*
 * Used by the I/O read routine to look for keyboard input
 * Returns TRUE if user aborted with 'q' or 'z' (lynx-style)
 *         FALSE otherwise
 */
static t_bool
wait_for_input (void /*FILE *fd*/)
{
#	ifndef HAVE_SELECT
#		ifdef VMS
	int ch = ReadChNowait ();

	if (ch == 'q' || ch == 'z' || ch == ESC) {
		if (prompt_yn (cLINES, txt_read_abort, FALSE) == 1)
			return TRUE;
	}
	if (ch == 'Q') {
		if (prompt_yn (cLINES, txt_read_exit, FALSE) == 1)
			tin_done (EXIT_SUCCESS);
	}
#		endif /* VMS */
#	else
	int nfds, ch;
	fd_set readfds;
	struct timeval tv;

	/*
	 * Main loop. Wait for input from keyboard or file or for a timeout.
	 */
	forever {

		FD_ZERO(&readfds);
		FD_SET(STDIN_FILENO, &readfds);
/*		FD_SET(fileno(NEED_REAL_NNTP_FD_HERE), &readfds);*/

		tv.tv_sec = 0;		/*NNTP_READ_TIMEOUT;*/
		tv.tv_usec = 0;

/*DEBUG_IO((stderr, "waiting on %d and %d...", STDIN_FILENO, fileno(fd)));*/
		if ((nfds = select(STDIN_FILENO+1, &readfds, NULL, NULL, &tv)) == -1) {
			if (errno != EINTR) {
				perror_message("select() failed");
				giveup();
			} else
				return FALSE;
		}

		/* No input pending */
		if (nfds == 0)
			return FALSE;

		/*
		 * Something is waiting. See what's cooking...
		 */
		if (nfds > 0) {

			/*
			 * User pressed something. If 'q'uit, then handle this. Process
			 * user input 1st so they get chance to quit on busy (or stalled) reads
			 */
			if (FD_ISSET(STDIN_FILENO, &readfds)) {
				if ((ch = ReadCh()) == EOF)
					return FALSE;

				if (ch == 'q' || ch == 'z' || ch == ESC) {
					if (prompt_yn (cLINES, txt_read_abort, FALSE) == 1)
						return TRUE;
				}

				if (ch == 'Q') {
					if (prompt_yn (cLINES, txt_read_exit, FALSE) == 1)
						tin_done (EXIT_SUCCESS);
				}

			}

#		if 0
			/*
			 * Our file has something for us to read
			 */
			if (FD_ISSET(fileno(NEED_NNTP_FD_HERE), &readfds))
				return TRUE;
#		endif /* 0 */
		}

	}
#	endif /* !HAVE_SELECT */
#	ifdef M_AMIGA
	return FALSE;
/* work in progress */
/*	return (WaitForChar(Input(), 0) == DOSTRUE) ? TRUE : FALSE; */
#	endif /* M_AMIGA */
/*
 * FIXME, insert !HAVE_SELECT code here - NB: we already have VMS specific code
 *        at the top of this function
 */
	return FALSE;
}
#endif /* NNTP_ABLE */


/*
 * Support routine to read a fixed size buffer. This does most of the
 * hard work for tin_fgets()
 */
static t_bool partial_read;


static char *
tin_read (
	char *buffer,
	size_t len,
	FILE *fp,
	t_bool header)
{
	char *ptr;
	signed char c;
	int i;
#ifdef NNTP_ABLE
	t_bool check_dot_only_line;

	/*
	 * We have to check '.' line when reading via NNTP and
	 * reading first line.
	 */
	check_dot_only_line = (header && fp == FAKE_NNTP_FP && partial_read == FALSE);
#endif /* NNTP_ABLE */

	partial_read = FALSE;

#ifdef NNTP_ABLE
	if (wait_for_input(/*fp*/)) {			/* Check if okay to read */
		info_message("Aborting read, please wait...");
		drain_buffer(fp);
		clear_message();
		tin_errno = TIN_ABORT;
		fflush(stdin);
		return(NULL);
	}

	errno = 0;		/* To check errno after read, clear it here */

	/*
	 * Initially try and fit into supplied buffer
	 */
	if (fp == FAKE_NNTP_FP)
		ptr = get_server(buffer, len);
	else
		ptr = fgets (buffer, len, fp);
#else
	errno = 0;		/* To check errno after read, clear it here */

	ptr = fgets (buffer, len, fp);
#endif /* NNTP_ABLE */

/* TODO develop this next line ? */
#ifdef DEBUG
	if (errno)
		fprintf(stderr, "errno in tin_read %d\n", errno);
#endif /* DEBUG */

	if (ptr == 0)	/* End of data ? */
		return(NULL);

	/*
	 * Was this only a partial read ?
	 * We strip trailing \r and \n here and here _only_
	 * 'offset' is the # of chars which we read now
	 */
	i = strlen(buffer);
	if (i >= 1 && buffer[i - 1] == '\n') {

		if (i >= 2 && buffer[i - 2] == '\r') {
			buffer[i - 2] = '\0';
			offset = i -= 2;
		} else {
			buffer[i - 1] = '\0';
			offset = --i;
		}

		/*
		 * If we're looking for continuation headers, check for whitespace.
		 * If we find some, mark this as a partial read and skip it ready for
		 * the next read. Push a space to effectively condense leading whitespace
		 * to a single ' '
		 */
#ifdef NNTP_ABLE
		if (check_dot_only_line && i == 1 && buffer[0] == '.') { /* EMPTY */
			/* Find a terminator, don't check next line. */
		} else
#endif /* NNTP_ABLE */
		{
			if (header) {
				if (!i) { /* EMPTY */
					/* Find a header separator, don't check next line. */
				} else {
					while ((c = fgetc (get_nntp_fp(fp))) == ' ' || c == '\t')
						partial_read = TRUE;

					/* Push back the 1st char after the now-skipped whitespace */
					if (c != EOF) {
						ungetc(c, get_nntp_fp(fp));
						/* TODO - is this portable ? Push back a single ' ' to compress the white-space */
						if (partial_read)
							ungetc(' ', get_nntp_fp(fp));
					}
				}
			}
		}
	} else {
		partial_read = TRUE;
		offset = i;
	}

	return(buffer);
}


/*
 * This is the main routine for reading news data from local spool or NNTP.
 * It can handle arbitrary length lines of data, failed connections and
 * user initiated aborts (where possible)
 *
 * We simply request data from an fd and data is read up to the next \n
 * Any trailing \r and \n will be stripped.
 * If fp is FAKE_NNTP_FP, then we are reading via a socket to an NNTP server. The
 * required post-processing of the data will be done such that we look like
 * a local read to the calling function.
 *
 * Header lines:
 *   If header is TRUE, then we assume we're reading a news article header.
 *   In some cases, article headers are split over multiple lines. The rule
 *   is that if the next line starts with \t or ' ', then it will be included
 *   as part of the current line. We condense leading whitespace to a single ' '
 *
 * Dynamic read code based on code by <emcmanus@gr.osf.org>
 *
 * Caveat: We try to keep the code path for a trivial read as short as
 * possible.
 */
char *
tin_fgets (
	FILE *fp,
	t_bool header)
{
	static char *dynbuf = NULL;
	static int size = 0;

	char *temp, *ptr;
	int next;

	tin_errno = 0;					/* Clear errors */
	partial_read = FALSE;

#if 1
	/* Allocate initial buffer */
	if (dynbuf == NULL) {
		dynbuf = (char *) my_malloc (INIT * sizeof(char));
		size = INIT;
	}
	/* Otherwise reuse last buffer */
	/* TODO: Should we free too large buffer? */
#else
	if (dynbuf != NULL)
		free(dynbuf);				/* Free any previous allocation */

	dynbuf = (char *) my_malloc (INIT * sizeof(char));
	size = INIT;
#endif /* 1 */

	if ((ptr = tin_read(dynbuf, size, fp, header)) == NULL)
		return ptr;

	if (tin_errno != 0) {
		DEBUG_IO((stderr, "Aborted read\n"));
		return(NULL);
	}

	next = offset;

	while (partial_read) {
		if (next + CHUNK > size)
			size = next + CHUNK;
		temp = (char *) my_realloc (dynbuf, size * sizeof(char));
		dynbuf = temp;
		temp = tin_read (dynbuf + next, size - next, fp, header); /* What if == 0 ?? */
		next += offset;

		if (tin_errno != 0)
			return(NULL);
	}

	/*
	 * Do processing of leading . for NNTP
	 * This only occurs at the start of lines
	 * At this point, dynbuf won't be NULL
	 */
#ifdef NNTP_ABLE
	if (fp == FAKE_NNTP_FP) {
		if (dynbuf[0] == '.') {			/* reduce leading .'s */
			if (dynbuf[1] == '\0') {
				DEBUG_IO((stderr, "tin_fgets (NULL)\n"));
				return (NULL);
			}
			DEBUG_IO((stderr, "tin_fgets (%s)\n", dynbuf+1));
			return (dynbuf+1);
		}
	}
#endif /* NNTP_ABLE */

DEBUG_IO((stderr, "tin_fgets (%s)\n", (dynbuf) ? dynbuf : "NULL"));

	return (dynbuf);
}


/*
 * We can't just stop reading a socket once we are through with it. This drains out any
 * pending data on the NNTP port
 */
#ifdef NNTP_ABLE
void
drain_buffer (
	FILE *fp)
{
	int i = 0;

	if (fp != FAKE_NNTP_FP)
		return;

DEBUG_IO((stderr, "Draining\n"));
	while (tin_fgets(fp, FALSE) != (char *) 0) {
		if (++i % MODULO_COUNT_NUM == 0)
			spin_cursor();
	}
}
#endif /* NNTP_ABLE */

/* end of read.c */
