/*
 *  Project   : tin - a Usenet reader
 *  Module    : nntpvms.c
 *  Author    : S.Barber & I.Lea
 *  Created   : 10-04-97
 *  Notes     : VMS specific NNTP client routines, see nntplib.c
 *  Copyright : (c) Copyright 1991-94 by Stan Barber & Iain Lea
 *              Permission is hereby granted to copy, reproduce, redistribute
 *              or otherwise use this software  as long as: there is no
 *              monetary  profit  gained  specifically  from the use or
 *              reproduction or this software, it is not  sold, rented,
 *              traded or otherwise marketed, and this copyright notice
 *              is included prominently in any copy made.
 */
#ifdef VMS

int sockt_rd = -1, sockt_wr = -1;

/*
 * u_put_server -- send data to the server. Do not flush output.
 */

#ifdef NNTP_ABLE
void
u_put_server(
					const char *string)
{
	netwrite(sockt_wr, string, strlen(string));
}

#endif

/*
 * put_server -- send a line of text to the server, terminating it
 * with CR and LF, as per ARPA standard.
 *
 * Parameters: "string" is the string to be sent to the
 *       server.
 *
 * Returns: Nothing.
 *
 * Side effects:  Talks to the server.
 *       Closes connection if things are not right.
 *
 * Note: This routine flushes the buffer each time
 *       it is called.  For large transmissions
 *       (i.e., posting news) don't use it.  Instead,
 *       do the fprintf's yourself, and then a final
 *       fflush.
 */

#ifdef NNTP_ABLE
void
put_server(
				 const char *string)
{
	int respno;
	static time_t time_last;
	time_t time_now;

	/*
	 *  Check how idle we have been, if too idle send a STAT to check
	 */
	time(&time_now);

	if (sockt_wr == -1 || (time_last != 0 && time_last + NNTP_IDLE_RETRY_SECS < time_now)) {
		respno = 0;
		if (sockt_wr >= 0) {
			netwrite(sockt_wr, "stat\r\n", 6);
			respno = get_respcode();
		}
		if (respno != OK_NOTEXT && respno != ERR_NCING && respno != ERR_NOCRNT) {
			/*
			 *  STAT was not happy, close the connection
			 *  it will reopen on next get_server
			 */
			if (sockt_wr >= 0)
				netclose(sockt_wr);
			sockt_wr = sockt_rd = -1;

			strcpy(last_put, string);
			time_last = 0;
			return;
		}
	}
	time_last = time_now;

	{
		char line[256];

		sprintf(line, "%s\r\n", string);
		netwrite(sockt_wr, line, strlen(line));
	}

	strcpy(last_put, string);
}

#endif /* NNTP_ABLE */

/*
 * get_server -- return the next line of text from the server.  Strips
 * CR's and LF's.
 *
 * Parameters: "string" has the buffer space for the line received.
 *       "size" is maximum size of the buffer to read.
 *
 * Returns: NULL on end of stream, or a line of data
 *
 * Side effects:  Talks to server, changes contents of "string".
 *       Reopens connection when necessary and requested.
 *       Exits via tin_done() if fatal error occurs.
 */

#ifdef NNTP_ABLE
char *
get_server(char *string, int size)
{
	char buf[NNTP_STRLEN];
	register char *cp;

#ifdef USE_SFGETS
	while (Sfgets(string, size, sockt_rd) == NULL) {
#else
	char *p;

	while ((p = Srdline(sockt_rd)) == NULL) {
#endif
		if (errno != EINTR) {
			netclose(sockt_rd);
			ring_bell();
			if (!prompt_yn(cLINES, txt_reconnect_to_news_server, TRUE)) {
				tin_done(EXIT_NNTP_ERROR);
			}
			clear_message();
			strcpy(buf, last_put);
			if (nntp_open() == 0) {
				if (glob_group != (char *) 0) {
					sprintf(last_put, "group %s", glob_group);
					put_server(last_put);
#ifdef USE_SFGETS
					Sfgets(last_put, NNTP_STRLEN, sockt_rd);
#else
					p = Srdline(sockt_rd);
					strncpy(last_put, p, NNTP_STRLEN);
#endif
				}
			} else {
				tin_done(EXIT_NNTP_ERROR);
			}
			put_server(buf);
		}
	}
#ifndef USE_SFGETS
	memcpy(string, p, SIOLINELEN(sockt_rd) + 1);
#endif
	if ((cp = strchr(string, '\r')) != NULL) {
		*cp = '\0';
	} else if ((cp = strchr(string, '\n')) != NULL) {
		*cp = '\0';
	}
	return (0);
}
#endif /* NNTP_ABLE */

/*
 * close_server -- close the connection to the server, after sending
 *    the "quit" command.
 *
 * Parameters: None.
 *
 * Returns: Nothing.
 *
 * Side effects:  Closes the connection with the server.
 *       You can't use "put_server" or "get_server"
 *       after this routine is called.
 */

#ifdef NNTP_ABLE
void
close_server(void)
{
	if (sockt_rd < 0 || sockt_wr < 0)
		return;

	put_server("QUIT");
	(void) get_server(nntp_line, sizeof (nntp_line));

	netclose(sockt_rd);
	s_end();
	sockt_rd = sockt_wr = -1;
}
#endif /* NNTP_ABLE */

#endif /* VMS */
