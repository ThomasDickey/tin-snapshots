/*
 *  Project   : tin - a Usenet reader
 *  Module    : open.c
 *  Author    : I. Lea & R. Skrenta
 *  Created   : 1991-04-01
 *  Updated   : 1999-11-29
 *  Notes     : Routines to make reading news locally (ie. /var/spool/news)
 *              or via NNTP transparent
 *  Copyright : (c) Copyright 1991-99 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */


#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */
#ifndef TCURSES_H
#	include "tcurses.h"
#endif /* !TCURSES_H */


/*
 * local prototypes
 */
static int base_comp (t_comptype *p1, t_comptype *p2);
#if 0 /* currently unused */
	static FILE * open_xhdr_fp (char *header, long min, long max);
#endif /* 0 */


long head_next;

#ifdef NO_POSTING
	t_bool can_post = FALSE;
#else
	t_bool can_post = TRUE;
#endif /* NO_POSTING */

char *nntp_server = (char *)0;
static char txt_xover_string[] = "XOVER";
char *txt_xover = txt_xover_string;

/*
 * Open a connection to the NNTP server. Authenticate if necessary or
 * desired, and test if the server supports XOVER.
 * Returns: 0	success
 *        > 0	NNTP error response code
 *        < 0	-errno from system call or similar error
 */
int
nntp_open (
	void)
{
#ifdef NNTP_ABLE
	char *linep;
	char line[NNTP_STRLEN];
	int ret;
	t_bool sec = FALSE;
	static t_bool is_reconnect = FALSE;

	if (!read_news_via_nntp)
		return 0;

#	ifdef DEBUG
	debug_nntp ("nntp_open", "BEGIN");
#	endif /* DEBUG */

	/* do this only once at start-up */
	if (!is_reconnect)
		nntp_server = getserverbyfile (NNTP_SERVER_FILE);

	if (nntp_server == (char *) 0) {
		error_message (txt_cannot_get_nntp_server_name);
		error_message (txt_server_name_in_file_env_var, NNTP_SERVER_FILE);
		return -EHOSTUNREACH;
	}

	if (INTERACTIVE) {
		if (nntp_tcp_port != 119)
			wait_message (0, txt_connecting_port, nntp_server, nntp_tcp_port);
		else
			wait_message (0, txt_connecting, nntp_server);
	}

#	ifdef DEBUG
	debug_nntp ("nntp_open", nntp_server);
#	endif /* DEBUG */

	ret = server_init (nntp_server, NNTP_TCP_NAME, nntp_tcp_port, line);
DEBUG_IO((stderr, "server_init returns %d,%s\n", ret, line));

	if (!batch_mode && ret >= 0 && cmd_line)
		my_fputc ('\n', stdout);

#	ifdef DEBUG
	debug_nntp ("nntp_open", line);
#	endif /* DEBUG */

	switch (ret) {

		/*
		 * ret < 0 : some error from system call
		 * ret > 0 : NNTP response code
		 *
		 * Latest NNTP draft (Aug 1999) says only the following response codes
		 * may be returned:
		 *
		 *   200 (OK_CANPOST) Hello, you can post
		 *   201 (OK_NOPOST) Hello, you can't post
		 *   502 (ERR_ACCESS) Service unavailable
		 *   400 (ERR_GOODBYE) Service temporarily unavailable
		 */

		case OK_CANPOST:
#	ifndef NO_POSTING
			can_post = TRUE;
#	endif /* !NO_POSTING */
			break;

		case OK_NOPOST:
			can_post = FALSE;
			break;

		default:
			if (ret < 0) {
				error_message (txt_failed_to_connect_to_server, nntp_server);
			} else {
				error_message (line);
			}
			return ret;
	}
	if (!is_reconnect) {
		/* remove leading whitespace and save server's initial response */
		linep = line;
		while (isspace((int)*linep))
			linep++;

		STRCPY(bug_nntpserver1, linep);
	}

	/*
	 * Switch INN into NNRP mode with 'mode reader'
	 */

#	ifdef DEBUG
	debug_nntp ("nntp_open", "mode reader");
#	endif /* DEBUG */
DEBUG_IO((stderr, "nntp_command(MODE READER)\n"));
	put_server ("MODE READER");

	/*
	 * According to the latest NNTP draft (Aug 1999), MODE READER may only
	 * return the following response codes:
	 *
	 *   200 (OK_CANPOST) Hello, you can post
	 *   201 (OK_NOPOST) Hello, you can't post
	 *   400 (ERR_GOODBYE) Service temporarily unavailable
	 *   502 (ERR_ACCESS) Service unavailable
	 *
	 * However, there may be old servers out there that do not implement this
	 * command and therefore return ERR_COMMAND (500).
	 */

	ret = get_respcode(line);
	switch (ret) {
		case OK_CANPOST:
#	ifndef NO_POSTING
			can_post = TRUE;
#	endif /* !NO_POSTING */
			sec = TRUE;
			break;

		case OK_NOPOST:
			can_post = FALSE;
			sec = TRUE;
			break;

		case ERR_GOODBYE:
		case ERR_ACCESS:
			error_message (line);
			return ret;

		case ERR_COMMAND:
		default:
			break;

	}

	/*
	 * NOTE: Latest NNTP draft (Aug 1998) states that LIST EXTENSIONS should
	 *       (not SHOULD, however) be used to find out what commands are
	 *       supported.
	 *
	 * TODO: Implement LIST EXTENSIONS here. Get this list before issuing
	 *       authentication because the authentication method required may be
	 *       mentioned in the list of extensions. (For details about
	 *       authentication methods, see draft-newman-nntpext-auth-01.txt.)
	 */

	/*
	 * If the user wants us to authenticate on connection startup, do it now.
	 * Some news servers return "201 no posting" first, but after successful
	 * authentication you get a "200 posting allowed". To find out if we are
	 * allowed to post after authentication issue a "MODE READER" again and
	 * interpret the response code.
	 */

	if (force_auth_on_conn_open) {
#	ifdef DEBUG
		debug_nntp ("nntp_open", "authenticate");
#	endif /* DEBUG */
		authenticate (nntp_server, ERR_NOAUTH, userid, TRUE);
		put_server ("MODE READER");
		ret = get_respcode (line);
		switch (ret) {
			case OK_CANPOST:
#	ifndef NO_POSTING
				can_post = TRUE;
#	endif /* !NO_POSTING */
				sec = TRUE;
				break;

			case OK_NOPOST:
				can_post = FALSE;
				sec = TRUE;
				break;

			case ERR_GOODBYE:
			case ERR_ACCESS:
				error_message (line);
				return ret;

			case ERR_COMMAND:	/* Uh-oh ... now we don't know if posting */
			default:				/* is allowed or not ... so use last 200 */
				break;			/* or 201 response to decide. */

		}
	}

	if (!is_reconnect) {
		/* Inform user if he cannot post */
		if (!can_post)
			wait_message(0, "%s\n", txt_cannot_post);

		/* Remove leading white space and save server's second response */
		linep = line;
		while (isspace((int)*linep))
			linep++;

		STRCPY(bug_nntpserver2, linep);

		/*
		 * Show user last server response line, do some nice formatting if
		 * response is longer than a screen wide.
		 *
		 * TODO: This only breaks the line once, but the response could be
		 * longer than two lines ...
		 */
		{
			char *chr1, *chr2;
			int j;

			j = atoi (get_val ("COLUMNS", "80"));
			chr1 = my_strdup ((sec ? bug_nntpserver2 : bug_nntpserver1));

			if (((int)strlen (chr1)) >= j) {
				chr2 = chr1 + strlen (chr1) - 1;
				while (chr2 - chr1 >= j)
					chr2--;
				while (chr2 > chr1 && *chr2 != ' ')
					chr2--;
				if (chr2 != chr1)
					*chr2 = '\n';
			}

			wait_message (0, "%s\n", chr1);
			free (chr1);
		}
	}

	/*
	 * Check if NNTP supports XOVER or OVER (successor of XOVER as of latest
	 * NNTP Draft (Aug 1999)) command
	 * ie, we _don't_ get an ERR_COMMAND
	 *
	 * TODO: Don't try (X)OVER if listed in LIST EXTENSIONS.
	 */

	if (!nntp_command(txt_xover_string, ERR_COMMAND, NULL)) {
		xover_supported = TRUE;
		txt_xover = txt_xover_string;
		/* TODO issue warning if old index files found ? */
	} else {
		if (!nntp_command(&txt_xover_string[1], ERR_COMMAND, NULL)) {
			xover_supported = TRUE;
			txt_xover = &txt_xover_string[1];
			/* TODO issue warning if old index files found ? */
		} else {
			if (!is_reconnect) {
				wait_message(2, txt_no_xover_support);
			}
		}
	}

#	if 0 /* TODO */
	/* if we're using -n, check for XGTITLE */
	if (newsrc_active && !list_active) { /* -n */
		if (!nntp_command("XGTITLE", ERR_COMMAND, NULL))
			xgtitle_supported = TRUE;
	}
#	endif /* 0 */

	is_reconnect = TRUE;

#endif /* NNTP_ABLE */

	DEBUG_IO((stderr, "nntp_open okay\n"));
	return 0;
}


void
nntp_close (
	void)
{
#ifdef NNTP_ABLE
	if (read_news_via_nntp) {
#	ifdef DEBUG
		debug_nntp ("nntp_close", "END");
#	endif /* DEBUG */
		close_server ();
	}
#endif /* NNTP_ABLE */
}

/*
 *  Get a response code from the server.
 *  Returns:
 *    +ve NNTP return code
 *    -1  on an error or user abort. We don't differentiate.
 *  If 'message' is not NULL, then any trailing text after the response
 *  code is copied into it.
 *  Does not perform authentication if required; use get_respcode()
 *  instead.
 */

int
get_only_respcode (
	char *message)
{
#ifdef NNTP_ABLE
	char *ptr, *end;
	int respcode;

	ptr = tin_fgets (FAKE_NNTP_FP, FALSE);

	if (tin_errno || ptr == NULL)
		return -1;

	respcode = (int) strtol(ptr, &end, 10);
DEBUG_IO((stderr, "get_only_respcode(%d)\n", respcode));

	if ((respcode == ERR_FAULT /* || respcode == ERR_GOODBYE ??? */) &&
	    last_put[0] != '\0') {
		/*
		 * Maybe server timed out.
		 * If so, retrying will force a reconnect.
		 */
#	ifdef DEBUG
		debug_nntp ("get_only_respcode", "timeout");
#	endif /* DEBUG */
		put_server (last_put);
		ptr = tin_fgets (FAKE_NNTP_FP, FALSE);

		if (tin_errno)
			return -1;

		respcode = (int) strtol(ptr, &end, 10);
DEBUG_IO((stderr, "get_only_respcode(%d)\n", respcode));
	}
	if (message != NULL)				/* Pass out the rest of the text */
		strcpy(message, end);

	return respcode;
#else
	return 0;
#endif /* NNTP_ABLE */
}

/*
 *  Get a response code from the server.
 *  Returns:
 *    +ve NNTP return code
 *    -1  on an error
 *  If 'message' is not NULL, then any trailing text after the response
 *	 code is copied into it.
 *  Performs authentication if required and repeats the last command if
 *  necessary after a timeout.
 */

int
get_respcode (
	char *message)
{
#ifdef NNTP_ABLE
	char savebuf[NNTP_STRLEN];
	char *ptr, *end;
	int respcode;

	respcode = get_only_respcode (message);
	if ((respcode == ERR_NOAUTH)       ||
	    (respcode == ERR_NOAUTHSIMPLE) ||
	    (respcode == NEED_AUTHINFO)) {
		/*
		 * Server requires authentication.
		 */
#	ifdef DEBUG
		debug_nntp ("get_respcode", "authentication");
#	endif /* DEBUG */
		strncpy (savebuf, last_put, NNTP_STRLEN);		/* Take copy, as authenticate() will clobber this */

		if (authenticate (nntp_server, respcode, userid, FALSE)) {
			strcpy (last_put, savebuf);

			put_server (last_put);
			ptr = tin_fgets (FAKE_NNTP_FP, FALSE);

			if (tin_errno)
				return -1;

			respcode = (int) strtol(ptr, &end, 10);
			if (message != NULL)				/* Pass out the rest of the text */
				strcpy(message, end);

		} else {
			error_message (txt_auth_failed, ERR_ACCESS);
/*			return -1;*/
			tin_done (EXIT_FAILURE);
		}
	}

	return respcode;
#else
	return 0;
#endif /* NNTP_ABLE */
}

#ifdef NNTP_ABLE
/*
 * Do an NNTP command. Send command to server, and read the reply.
 * If the reply code matches success, then return an open file stream
 * Return NULL if we did not see the response we wanted.
 * If message is not NULL, then the trailing text of the reply string is
 * copied into it for the caller to process.
 */
FILE *
nntp_command (
	const char *command,
	int success,
	char *message)
{
DEBUG_IO((stderr, "nntp_command (%s)\n", command));
#	ifdef DEBUG
	debug_nntp ("nntp command", command);
#	endif /* DEBUG */
	put_server (command);

	if (!bool_equal(dangerous_signal_exit, TRUE))
		if ((/* respcode = */ get_respcode (message)) != success) {
#	ifdef DEBUG
			debug_nntp (command, "NOT_OK");
#	endif /* DEBUG */
			/* error_message ("%s", message); */
			return (FILE *) 0;
		}

#	ifdef DEBUG
	debug_nntp (command, "OK");
#	endif /* DEBUG */
	return FAKE_NNTP_FP;
}
#endif /* NNTP_ABLE */

/*
 * Open the news active file locally or send the LIST command
 */

FILE *
open_news_active_fp (
	void)
{
#ifdef NNTP_ABLE
	if (read_news_via_nntp && !read_saved_news)
		return (nntp_command ("LIST", OK_GROUPS, NULL));
	else
#endif /* NNTP_ABLE */
		return (fopen (news_active_file, "r"));
}

/*
 * Open the NEWSLIBDIR/overview.fmt file locally or send LIST OVERVIEW.FMT
 */

FILE *
open_overview_fmt_fp (
	void)
{
	char line[NNTP_STRLEN];

#ifdef NNTP_ABLE
	if (read_news_via_nntp && !read_saved_news) {
		if (!xover_supported)
			return (FILE *) 0;

		sprintf (line, "LIST %s", OVERVIEW_FMT);
		return (nntp_command (line, OK_GROUPS, NULL));
	} else {
#endif /* NNTP_ABLE */
		joinpath (line, libdir, OVERVIEW_FMT);
		return (fopen (line, "r"));
#ifdef NNTP_ABLE
	}
#endif /* NNTP_ABLE */
}

/*
 * Open the active.times file locally or send the NEWGROUPS command
 *
 * NEWGROUPS yymmdd hhmmss
 */

FILE *
open_newgroups_fp (
	int idx)
{
#ifdef NNTP_ABLE
	char line[NNTP_STRLEN];
	struct tm *ngtm;

	if (read_news_via_nntp && !read_saved_news) {
		if (idx == -1)
			return (FILE *) 0;

		ngtm = localtime (&newnews[idx].time);
	/*
	 * in the current draft NEWGROUPS is allowed to take a 4 digit year
	 * componennt - but even with a 2 digit year componennt it is y2k
	 * compilant... we should switch over to ngtm->tm_year + 1900
	 * after most of the server could handle the new format
	 */
		sprintf (line, "NEWGROUPS %02d%02d%02d %02d%02d%02d",
			ngtm->tm_year % 100, ngtm->tm_mon + 1, ngtm->tm_mday,
			ngtm->tm_hour, ngtm->tm_min, ngtm->tm_sec);

		return (nntp_command (line, OK_NEWGROUPS, NULL));
	} else
#endif /* NNTP_ABLE */
		return (fopen (active_times_file, "r"));
}

/*
 * Get a list of default groups to subscribe to
 */
/* TODO fixme/checkme */
FILE *
open_subscription_fp (
	void)
{
#ifdef NNTP_ABLE
	if (read_news_via_nntp && !read_saved_news)
		return (nntp_command ("LIST SUBSCRIPTIONS", OK_GROUPS, NULL));
	else
#endif /* NNTP_ABLE */
		return (fopen (subscriptions_file, "r"));
}

#ifdef HAVE_MH_MAIL_HANDLING
/*
 * Open the mail active file locally
 */
FILE *
open_mail_active_fp (
	const char *mode)
{
	return fopen (mail_active_file, mode);
}

/*
 *  Open mail groups description file locally
 */
FILE *
open_mailgroups_fp (
	void)
{
	return fopen (mailgroups_file, "r");
}
#endif /* HAVE_MH_MAIL_HANDLING */

/*
 * If reading via NNTP the newsgroups file will be saved to ~/.tin/newsgroups
 * so that any subsequent rereads on the active file will not have to waste
 * net bandwidth and the local copy of the newsgroups file can be accessed.
 */
FILE *
open_newsgroups_fp (
	void)
{
#ifdef NNTP_ABLE
	FILE *result;
	if (read_news_via_nntp && !read_saved_news) {
		if (read_local_newsgroups_file) {
			result = fopen (local_newsgroups_file, "r");
			if (result != NULL) {
#	ifdef DEBUG
				debug_nntp ("open_newsgroups_fp", "Using local copy of newsgroups file");
#	endif /* DEBUG */
				return result;
			}
			read_local_newsgroups_file = FALSE;
		}
#	if 0 /* TODO */
		if (xgtitle_supported && newsrc_active
		    && !list_active
		    && num_active < some_usefull_limit) {
			for (i = 0; i < num_active; i++) {
				sprintf(buff, "XGTITLE %s", active[i].name);
				nntp_command(buff, OK_LIST, NULL));
		} else
#	endif /* 0 */
		return (nntp_command ("LIST NEWSGROUPS", OK_GROUPS, NULL));
	} else
#endif /* NNTP_ABLE */
		return fopen (newsgroups_file, "r");
}

/*
 * Open a group NOV/XOVER file
 */

FILE *
open_xover_fp (
	struct t_group *psGrp,
	const char *pcMode,
	long lMin,
	long lMax)
{
#ifdef NNTP_ABLE
	if (read_news_via_nntp && xover_supported && *pcMode == 'r' && psGrp->type == GROUP_TYPE_NEWS) {
		char acLine[NNTP_STRLEN];

		sprintf (acLine, "%s %ld-%ld", txt_xover, lMin, lMax);
		return(nntp_command (acLine, OK_XOVER, NULL));
	} else {
#endif /* NNTP_ABLE */
		char *pcNovFile;

		pcNovFile = pcFindNovFile (psGrp, (*pcMode == 'r' ? R_OK : W_OK));
#ifdef DEBUG
		if (debug)
			error_message ("READ file=[%s]", pcNovFile);
#endif /* DEBUG */
		if (pcNovFile != (char *) 0)
			return fopen (pcNovFile, pcMode);

		return (FILE *) 0;
#ifdef NNTP_ABLE
	}
#endif /* NNTP_ABLE */
}


/*
 * Stat a mail/news article to see if it still exists
 */
t_bool
stat_article (
	long art,
	char *group_path)
{
	char buf[NNTP_STRLEN];
	int i;

	i = my_group[cur_groupnum];

#ifdef NNTP_ABLE
	if (read_news_via_nntp && active[i].type == GROUP_TYPE_NEWS) {
		sprintf (buf, "STAT %ld", art);
		return(nntp_command (buf, OK_NOTEXT, NULL) != NULL);
	} else
#endif /* NNTP_ABLE */
	{
		struct stat sb;

		joinpath (buf, active[i].spooldir, group_path);
		sprintf (&buf[strlen (buf)], "/%ld", art);

		return (stat (buf, &sb) != -1);
	}
}

FILE *
open_art_header (
	long art)
{
	char buf[NNTP_STRLEN];
#ifdef NNTP_ABLE
	FILE *fp;

	if (read_news_via_nntp && CURR_GROUP.type == GROUP_TYPE_NEWS) {
		/*
		 *  Don't bother requesting if we have not got there yet.
		 *  This is a big win if the group has got holes in it (ie. if 000's
		 *  of articles have expired between active files min & max values).
		 */
		if (art < head_next)
			return (FILE *) 0;

		sprintf (buf, "HEAD %ld", art);
		if ((fp = nntp_command(buf, OK_HEAD, NULL)) != NULL)
			return(fp);

		/*
		 *  HEAD failed, try to find NEXT
		 *	Should return "223 artno message-id more text...."
		 */
		if (nntp_command("NEXT", OK_NOTEXT, buf))
			head_next = atoi (buf);		/* Set next art number */

		return (FILE *) 0;
	} else {
#endif /* NNTP_ABLE */
		sprintf (buf, "%ld", art);
		return(fopen (buf, "r"));
#ifdef NNTP_ABLE
	}
#endif /* NNTP_ABLE */
}

#ifdef NNTP_ABLE
/*
 * Copies an article from the nntp socket and writes it to a temp file.
 * Returns open descriptor to this file. We have to do this in order
 * to do header and MIME parsing, and other functions that use seek()
 */
static FILE *
get_article (
	FILE *art_fp,
	int lines)
{
	FILE *fp;
	char *ptr;
	char tempfile[PATH_LEN];
	int count = 0;
#	if defined(HAVE_FDOPEN) && defined(HAVE_MKSTEMP)
	int fd = -1;
#	endif /* HAVE_FDOPEN && HAVE_MKSTEMP */
	struct stat sb;

	sprintf (tempfile, "%stin_nntpXXXXXX", TMPDIR);
#	if defined(HAVE_FDOPEN) && defined(HAVE_MKSTEMP)
	if ((fd = my_mktemp (tempfile)) == -1) {
		perror_message (txt_cannot_create_uniq_name);
		return (FILE *) 0;
	}
	if ((fp = fdopen (fd, "w")) == (FILE *) 0) {
#	else
	mktemp (tempfile);
	if ((fp = fopen (tempfile, "w")) == (FILE *) 0) {
#	endif /* HAVE_FDOPEN && HAVE_MKSTEMP */
		perror_message (txt_article_cannot_open, tempfile);
		return (FILE *) 0;
	}

	while ((ptr = tin_fgets(art_fp, FALSE)) != NULL) {
		fputs (ptr, fp);
		fputs ("\n", fp);		/* The one case where we do need the \n */
								/* as rfc1521_decode() still expects this */
		/*
		 * Use the default message if one hasn't been supplied
		 * Body search is currently the only function that has a different message
		 */
		if (lines && ++count % MODULO_COUNT_NUM == 0)
			show_progress((*mesg=='\0') ? txt_reading_article : mesg, count, lines);

	}

	if (ferror(fp) || tin_errno) {
		fclose(fp);
		if (!tin_errno) {
			tin_errno = 1;
			error_message(txt_filesystem_full, tempfile);
		}
#	if defined(M_AMIGA) || defined(WIN32)
		log_unlink(fp, tempfile);
#	else
		unlink (tempfile);
#	endif /* M_AMIGA || WIN32 */
		return (FILE *) 0;
	}

	fclose(fp);

	/*
	 * Grab the correct filesize now that it's been closed
	 */
	note_size = ((stat (tempfile, &sb) < 0) ? 0 : sb.st_size);

	if ((fp = fopen (tempfile, "r")) == (FILE *) 0) {	/* Reopen for just reading */
		perror_message (txt_article_cannot_reopen, tempfile);
		return (FILE *) 0;
	}


	/*
	 * It is impossible to delete an open file on the Amiga or Win32. So we keep a
	 * copy of the file name and delete it when finished instead.
	 */
#	if defined(M_AMIGA) || defined(WIN32)
	log_unlink(fp, tempfile);
#	else
	unlink (tempfile);
#	endif /* M_AMIGA || WIN32 */

	return fp;
}
#endif /* NNTP_ABLE */

/*
 * Open a mail/news article. If via NNTP, despool it to local disk. We need the
 * article local for later. Run it through RFC1521 decode.
 * If lines != 0 then show progress meter.
 * Return:
 *		A pointer to the open postprocessed file
 *		NULL pointer if article read fails in some way
 */

FILE *
open_art_fp (
	char *group_path,
	long art,
	int lines,
	t_bool rfc1521decode)
{
	char buf[NNTP_STRLEN];
	struct stat sb;
	FILE *fp, *art_fp;

#ifdef NNTP_ABLE
	if (read_news_via_nntp && CURR_GROUP.type == GROUP_TYPE_NEWS) {
		FILE *nntp_fp;

		sprintf (buf, "ARTICLE %ld", art);
		if ((nntp_fp = nntp_command (buf, OK_ARTICLE, NULL)) == NULL)
			return (FILE *) 0;

		art_fp = get_article (nntp_fp, lines);
	} else {
#endif /* NNTP_ABLE */
		joinpath (buf, CURR_GROUP.spooldir, group_path);
		sprintf (&buf[strlen (buf)], "/%ld", art);
		/*
		 * Get the correct file size. This is done in get_article() for
		 * the NNTP case. TODO - fix this
		 */
		note_size = ((stat (buf, &sb) == -1) ? 0 : sb.st_size);

		art_fp = fopen (buf, "r");
#ifdef NNTP_ABLE
	}
#endif /* NNTP_ABLE */

	/*
	 * Do a bit of 1521 decoding, if appropriate.
	 * If art_fp=NULL, then it returns NULL
	 */
	if (rfc1521decode) {
		fp = rfc1521_decode(art_fp);
		if (fp != art_fp)
			note_size = 0;
	} else
		fp = art_fp;

	return fp;
}



/*
 *  Longword comparison routine for the qsort()
 */
static int
base_comp (
	t_comptype *p1,
	t_comptype *p2)
{
	const long *a = (const long *) p1;
	const long *b = (const long *) p2;

	if (*a < *b)
		return -1;

	if (*a > *b)
		return 1;

	return 0;
}


/*
 * via NNTP:
 *   Issue a LISTGROUP command
 *   Read the article numbers existing in the group into base[]
 *   If the LISTGROUP failed, issue a GROUP command. Use the results to
 *   create a less accurate version of base[]
 *	 This data will already be sorted
 *
 * on local spool:
 *   Read the spool dir to populate base[] as above. Sort it.
 *
 * Grow the arts[] and bitmaps as needed.
 * NB: the output will be sorted on artnum
 *
 * top_base is one past top.
 * Returns total number of articles in group, or -1 on error
 */
long
setup_hard_base (
	struct t_group *group,
	char *group_path)
{
	char buf[NNTP_STRLEN];
	long art;
	long total = 0;

	top_base = 0;

	/*
	 * If reading with NNTP, issue a LISTGROUP
	 */
	if (read_news_via_nntp && group->type == GROUP_TYPE_NEWS) {
#ifdef NNTP_ABLE

#	ifdef BROKEN_LISTGROUP
		/*
		 * Some nntp servers are broken and need an extra GROUP command
		 * (reported by reorx@irc.pl). This affects (old?) versions of
		 * nntpcache and leafnode. Usually this should not be needed.
		 */
		sprintf (buf, "GROUP %s", group->name);
		if (nntp_command(buf, OK_GROUP, NULL) == NULL)
			return(-1);
#	endif /* BROKEN_LISTGROUP */

		/*
		 * See if LISTGROUP works
		 */
		sprintf (buf, "LISTGROUP %s", group->name);
		if (nntp_command(buf, OK_GROUP, NULL) != NULL) {
			char *ptr;

#	ifdef DEBUG
			debug_nntp ("setup_base", buf);
#	endif /* DEBUG */

			while ((ptr = tin_fgets(FAKE_NNTP_FP, FALSE)) != NULL) {
				if (top_base >= max_art)
					expand_art ();

				base[top_base++] = atoi (ptr);
			}

			if (tin_errno)
				return(-1);

		} else {
			long start, last, count;
			char line[NNTP_STRLEN];

			/*
			 * Handle the obscure case that the user aborted before the LISTGROUP
			 * had a chance to respond
			 */
			if (tin_errno)
				return(-1);

			/*
			 * LISTGROUP failed, try a GROUP command instead
			 */
			sprintf (buf, "GROUP %s", group->name);
			if (nntp_command(buf, OK_GROUP, line) == NULL)
				return(-1);

			if (sscanf (line,"%ld %ld %ld", &count, &start, &last) != 3)
				return(-1);

			total = count;

			if (last - count > start)
				count = last - start;

			while (start <= last) {
				if (top_base >= max_art)
					expand_art();
				base[top_base++] = start++;
			}
		}
#endif /* NNTP_ABLE */
	/*
	 * Reading off local spool, read the directory files
	 */
	} else {
		DIR *d;
		DIR_BUF *e;

		joinpath (buf, group->spooldir, group_path);

		if (access (buf, R_OK) != 0) {
			error_message(txt_not_exist);
			return (-1);
		}

		if ((d = opendir (buf)) != NULL) {
			while ((e = readdir (d)) != NULL) {
				art = atol (e->d_name);
				if (art >= 1) {
					total++;
					if (top_base >= max_art)
						expand_art ();
					base[top_base++] = art;
				}
			}
			CLOSEDIR(d);
			qsort ((char *) base, (size_t)top_base, sizeof (long), base_comp);
		}
	}

	if (top_base) {
		if (base[top_base-1] > group->xmax)
			group->xmax = base[top_base-1];
		expand_bitmap (group, base[0]);
	}

	return total;
}


void
vGet1GrpArtInfo (
	struct t_group *grp)
{
	long lMinOld = grp->xmin;
	long lMaxOld = grp->xmax;

	vGrpGetArtInfo (grp->spooldir, grp->name, grp->type, &grp->count, &grp->xmax, &grp->xmin);

	if (grp->newsrc.num_unread > grp->count) {
#ifdef DEBUG
		my_printf (cCRLF "Unread WRONG %s unread=[%ld] count=[%ld]", grp->name, grp->newsrc.num_unread, grp->count);
		my_flush ();
#endif /* DEBUG */
		grp->newsrc.num_unread = grp->count;
	}

	if (grp->xmin != lMinOld || grp->xmax != lMaxOld) {
		expand_bitmap(grp, 0);
#ifdef DEBUG
		my_printf (cCRLF "Min/Max DIFF %s old=[%ld-%ld] new=[%ld-%ld]", grp->name, lMinOld, lMaxOld, grp->xmin, grp->xmax);
		my_flush ();
#endif /* DEBUG */
	}
}


/*
 *  Find the total, max & min articles number for specified group
 *  Use nntp GROUP command or read local spool
 *  Return 0, or -error
 */
int
vGrpGetArtInfo (
	char *pcSpoolDir,
	char *pcGrpName,
	int iGrpType,
	long *plArtCount,
	long *plArtMax,
	long *plArtMin)
{
	DIR *tDirFile;
	DIR_BUF *tFile;
	char acBuf[NNTP_STRLEN];
	long lArtNum;
#ifdef M_AMIGA
	long lArtMin;
	long lArtMax;

	lArtMin = *plArtMin;
	lArtMax = *plArtMax;
#endif /* M_AMIGA */

	if (read_news_via_nntp && iGrpType == GROUP_TYPE_NEWS) {
#ifdef NNTP_ABLE
		char acLine[NNTP_STRLEN];

		sprintf (acBuf, "GROUP %s", pcGrpName);
#	ifdef DEBUG
		debug_nntp ("vGrpGetArtInfo", acBuf);
#	endif /* DEBUG */
		put_server (acBuf);

		switch (get_respcode(acLine)) {

			case OK_GROUP:
				if (sscanf (acLine, "%ld %ld %ld", plArtCount, plArtMin, plArtMax) != 3)
					error_message("Invalid response to GROUP command, %s", acLine);
				break;

			case ERR_NOGROUP:
				*plArtCount = 0;
				*plArtMin = 1;
				*plArtMax = 0;
				return(-ERR_NOGROUP);

			case ERR_ACCESS:
				error_message (cCRLF "%s", acLine);
				tin_done (NNTP_ERROR_EXIT);
				/* keep lint quiet: */
				/* FALLTHROUGH */
			default:
#	ifdef DEBUG
				debug_nntp ("NOT_OK", acLine);
#	endif /* DEBUG */
				return(-1);
		}
#else
		my_fprintf(stderr, "Unreachable ?\n");
		return 0;
#endif /* NNTP_ABLE */
	} else {
#ifdef M_AMIGA
		if (!lArtMin)
			*plArtMin = 1;
		*plArtMax = lArtMax;
		*plArtCount = lArtMax - *plArtMin + 1;
#else
		*plArtCount = 0;
		*plArtMin = 1;
		*plArtMax = 0;

		vMakeGrpPath (pcSpoolDir, pcGrpName, acBuf);

/* TODO - Surely this is spurious, the opendir will fail anyway */
/*		  unless there is some subtle permission check if tin is suid news? */
#	if 0
		if (access (acBuf, R_OK) != 0)
			return(-1);
#	endif /* 0 */

		if ((tDirFile = opendir (acBuf)) != (DIR *) 0) {
			while ((tFile = readdir (tDirFile)) != (DIR_BUF *) 0) {
				lArtNum = atol(tFile->d_name); /* should be '\0' terminated... */
				if (lArtNum >= 1) {
					if (lArtNum > *plArtMax) {
						*plArtMax = lArtNum;
						if (*plArtMin == 0)
							*plArtMin = lArtNum;
					} else if (lArtNum < *plArtMin)
						*plArtMin = lArtNum;
					(*plArtCount)++;
				}
			}
			CLOSEDIR(tDirFile);
		} else
			return(-1);
#endif /* M_AMIGA */
	}

	return 0;
}


/* This will come in useful for filtering on non-overview hdr fields */
#if 0
static FILE *
open_xhdr_fp (
	char *header,
	long min,
	long max)
{
#	ifdef NNTP_ABLE
	if (read_news_via_nntp && !read_saved_news) {
		char buf[NNTP_STRLEN];

		sprintf(buf, "XHDR %s %ld-%ld", header, min, max);
		return(nntp_command(buf, OK_HEAD));
	} else
#	endif /* NNTP_ABLE */
		return (FILE *) 0;		/* Some trick implementation for local spool... */
}
#endif /* 0 */
