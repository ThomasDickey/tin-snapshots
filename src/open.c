/*
 *  Project   : tin - a Usenet reader
 *  Module    : open.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 21-12-94, 05-04-97
 *  Notes     : Routines to make reading news locally (ie. /var/spool/news)
 *              or via NNTP transparent
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"

#if 0
	static FILE * open_xhdr_fp (char *header, long min, long max);
#endif /* 0 */

static int base_comp (t_comptype *p1, t_comptype *p2);

long head_next;

#ifdef NO_POSTING
	int can_post = FALSE;
#else
	int can_post = TRUE;
#endif

char *nntp_server = (char *)0;

/*
 * Open a connection to the NNTP server
 * Returns:   0	success
 *	         > 0	NNTP error response code
 *          < 0	-errno from system call or similar error
 */

int
nntp_open (void)
{
#ifdef NNTP_ABLE
	char *linep;
	char line[NNTP_STRLEN];
	int ret;
	t_bool sec = FALSE;
	static t_bool is_reconnect = FALSE;
	static unsigned short nntp_tcp_port;

	if (!read_news_via_nntp)
		return 0;

#ifdef DEBUG
	debug_nntp ("nntp_open", "BEGIN");
#endif

	/* do this only once at start-up */
	if (!is_reconnect) {
		nntp_server = getserverbyfile (NNTP_SERVER_FILE);
		nntp_tcp_port = (unsigned short) atoi (get_val ("NNTPPORT", NNTP_TCP_PORT));
	}

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

#ifdef DEBUG
	debug_nntp ("nntp_open", nntp_server);
#endif

	ret = server_init (nntp_server, NNTP_TCP_NAME, nntp_tcp_port, line);
DEBUG_IO((stderr, "server_init returns %d,%s\n", ret, line));

	if (!batch_mode && ret >= 0 && cmd_line)
		my_fputc ('\n', stdout);

#ifdef DEBUG
	debug_nntp ("nntp_open", line);
#endif

	/* Latest NNTP draft says 205 Authentication required could be returned here */
	switch (ret) {

		case OK_CANPOST:
#ifndef NO_POSTING
			can_post = TRUE;
#endif
			break;

		case OK_NOPOST:
			can_post = FALSE;
			break;

		default:
			if (ret < 0) {
				error_message (txt_failed_to_connect_to_server, nntp_server);
				return -ret;
			}

			error_message (line);
			return ret;
	}
	if (!is_reconnect) {
		linep = line;
		while (isspace(*linep))
			linep++;
		strncpy(bug_nntpserver1, linep, sizeof(bug_nntpserver1)-1);
		bug_nntpserver1[sizeof(bug_nntpserver1)-1] = '\0';
	}

	/*
	 * Switch INN into NNRP mode with 'mode reader'
	 */

#ifdef DEBUG
	debug_nntp ("nntp_open", "mode reader");
#endif
DEBUG_IO((stderr, "nntp_command(MODE READER)\n"));
	put_server ("MODE READER");
	switch (get_respcode(line)) {
		case OK_CANPOST:
			sec = TRUE;
			break;
			
		case OK_NOPOST:
			can_post = FALSE;
			sec = TRUE;
			break;

		case ERR_ACCESS:
			error_message (line);
			return ret;

		case ERR_COMMAND:
		default:
			break;
 
	}

   if (!is_reconnect) {
		if (!can_post)
			wait_message(0, "%s\n", txt_cannot_post);

		linep = line;
		while (isspace(*linep))
			linep++;
		strncpy(bug_nntpserver2, linep, sizeof(bug_nntpserver2)-1);
		bug_nntpserver2[sizeof(bug_nntpserver2)-1] = '\0';

		if (sec)
			wait_message(0, "%s\n", bug_nntpserver2);
		else
			wait_message(0, "%s\n", bug_nntpserver1);

		is_reconnect = TRUE;
	}

	/*
	 * Check if NNTP supports XOVER command
	 * ie, we _don't_ get an ERR_COMMAND
	 */
	if (!nntp_command("XOVER", ERR_COMMAND, NULL))
		xover_supported = TRUE;
		/* TODO issue warning if old index files found ? */
	else {
		if (!is_reconnect)
			wait_message(2, "Your server does not support the NNTP XOVER command.\nTin will use local index files instead.\n");

#if 0	/* It seems this breaks the M$ newsserver */
		/*
		 * Try to authenticate. If XOVER exists, then authentication will already
		 * have been forced by XOVER
		 */
#ifdef DEBUG
		debug_nntp ("nntp_open", "authenticate");
#endif /* DEBUG */
		authenticate (nntp_server, userid, TRUE);
#endif /* 0 */
	}

#endif	/* NNTP_ABLE */

	/*
	 * Check if NNTP supports my XINDEX & XUSER commands
	 * ie, we _don't_ get an ERR_COMMAND
	 */
#ifdef HAVE_TIN_NNTP_EXTS
	if (!nntp_command("XUSER", ERR_COMMAND, NULL))
		xuser_supported = TRUE;
#endif	/* HAVE_TIN_NNTP_EXTS */

DEBUG_IO((stderr, "nntp_open okay\n"));
	return 0;
}


void
nntp_close (void)
{
#ifdef NNTP_ABLE
	if (read_news_via_nntp) {
#ifdef DEBUG
		debug_nntp ("nntp_close", "END");
#endif
		close_server ();
	}
#endif
}

/*
 *  Get a response code from the server.
 * 	Returns:
 *		+ve NNTP return code
 * 		-1  on an error
 *  If 'message' is not NULL, then any trailing text after the response
 *	code is copied into it.
 *  Performs authentication if required and repeats the last command if
 *  necessary after a timeout.
 */

int
get_respcode (
	char *message)
{
#ifdef NNTP_ABLE
	char line[NNTP_STRLEN];
	char savebuf[NNTP_STRLEN];
	char *ptr, *end;
	int respcode;

	ptr = tin_fgets (line, NNTP_STRLEN, nntp_rd_fp);

	if (tin_errno != 0 || ptr == NULL)
		return(-1);

	respcode = (int) strtol(ptr, &end, 10);
DEBUG_IO((stderr, "get_respcode(%d)\n", respcode));
	if ((respcode == ERR_NOAUTH) || (respcode == NEED_AUTHINFO)) {
		/*
		 * Server requires authentication.
		 */
#ifdef DEBUG
		debug_nntp ("get_respcode", "authentication");
#endif
		strcpy (savebuf, last_put);		/* Take copy, as authenticate() will clobber this */

		if (authenticate (nntp_server, userid, FALSE)) {
			strcpy (last_put, savebuf);

			put_server (last_put);
			ptr = tin_fgets (line, NNTP_STRLEN, nntp_rd_fp);

			if (tin_errno != 0)
				return(-1);

		} else {
			error_message(txt_auth_failed, ERR_ACCESS);
			return(-1);
		}
	}

	if (message != NULL)				/* Pass out the rest of the text */
		strcpy(message, end);

	return respcode;
#else
	return 0;
#endif
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
/*	int respcode; */
DEBUG_IO((stderr, "nntp_command (%s)\n", command));
#ifdef DEBUG
	debug_nntp ("nntp command", command);
#endif
	put_server (command);

	if ((/* respcode = */ get_respcode (message)) != success) {
#ifdef DEBUG
		debug_nntp (command, "NOT_OK");
#endif
/*		error_message ("%s", message);*/
		return (FILE *) 0;
	}

#ifdef DEBUG
	debug_nntp (command, "OK");
#endif
	return nntp_rd_fp;
}
#endif

/*
 * Open the news active file locally or send the LIST command
 */

FILE *
open_news_active_fp (void)
{
#ifdef NNTP_ABLE
	if (read_news_via_nntp)
		return (nntp_command ("LIST", OK_GROUPS, NULL));
	else
#endif
		return (fopen (news_active_file, "r"));
}

/*
 * Open the NEWSLIBDIR/overview.fmt file locally or send LIST OVERVIEW.FMT
 */

FILE *
open_overview_fmt_fp (void)
{
	char line[NNTP_STRLEN];

#ifdef NNTP_ABLE
	if (read_news_via_nntp) {
		if (!xover_supported)
			return (FILE *) 0;

		sprintf (line, "LIST %s", OVERVIEW_FMT);
		return (nntp_command (line, OK_GROUPS, NULL));
	} else {
#endif
		joinpath (line, libdir, OVERVIEW_FMT);
		return (fopen (line, "r"));
#ifdef NNTP_ABLE
	}
#endif
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
	struct tm *tm;

	if (read_news_via_nntp) {
		if (idx == -1)
			return (FILE *) 0;

		tm = localtime (&newnews[idx].time);
		sprintf (line, "NEWGROUPS %02d%02d%02d %02d%02d%02d",
			tm->tm_year, tm->tm_mon+1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec);

		return (nntp_command (line, OK_NEWGROUPS, NULL));
	} else
#endif
		return (fopen (active_times_file, "r"));
}

/*
 * If we have the tin NNTP extensions, open the news motd file
 *
 * XMOTD 311299 235959 [GMT]
 */
/* TODO testme */
#ifdef HAVE_TIN_NNTP_EXTS
FILE *
open_motd_fp (
	char *motd_file_date)
{
#if defined(NNTP_ABLE)
	if (read_news_via_nntp) {

		char line[NNTP_STRLEN];

		sprintf (line, "XMOTD %s", motd_file_date);
		return (nntp_command (line, OK_XMOTD));
	} else
#endif
		return (fopen (motd_file, "r"));
}
#endif


/*
 * Get a list of default groups to subscribe to
 */
/* TODO fixme/checkme */
FILE *
open_subscription_fp (void)
{
#ifdef NNTP_ABLE
	if (read_news_via_nntp)
		return (nntp_command ("LIST SUBSCRIPTIONS", OK_GROUPS, NULL));
	else
#endif
		return (fopen (subscriptions_file, "r"));
}

#ifdef HAVE_MH_MAIL_HANDLING
/*
 * Open the mail active file locally
 */
FILE *
open_mail_active_fp (
	char *mode)
{
	return fopen (mail_active_file, mode);
}

/*
 *  Open mail groups description file locally
 */
FILE *
open_mailgroups_fp (void)
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
open_newsgroups_fp (void)
{
#ifdef NNTP_ABLE
	if (read_news_via_nntp) {
		if (read_local_newsgroups_file) {
# ifdef DEBUG
			debug_nntp ("open_newsgroups_fp", "Using local copy of newsgroups file");
# endif /* DEBUG */
			return fopen (local_newsgroups_file, "r");
		} else
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

		sprintf (acLine, "XOVER %ld-%ld", lMin, lMax);
		return(nntp_command (acLine, OK_XOVER, NULL));
	} else {
#endif /* NNTP_ABLE */
		char *pcNovFile;

		pcNovFile = pcFindNovFile (psGrp, (*pcMode == 'r' ? R_OK : W_OK));
#ifdef DEBUG
		if (debug)
			error_message ("READ file=[%s]", pcNovFile);
#endif
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

int
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
	} else {
#endif /* NNTP_ABLE */
		struct stat sb;

		joinpath (buf, active[i].spooldir, group_path);
		sprintf (&buf[strlen (buf)], "/%ld", art);

		return (stat (buf, &sb) != -1);
#ifdef NNTP_ABLE
	}
#endif /* NNTP_ABLE */
}

FILE *
open_art_header (
	long art)
{
	char buf[NNTP_STRLEN];
	FILE *fp;

#ifdef NNTP_ABLE
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
	int lines)
{
	char tempfile[PATH_LEN];
	char line[HEADER_LEN];
	char *ptr;
	FILE *fp;
	struct stat sb;
#ifdef SHOW_PROGRESS
	int count = 0;
#endif

	sprintf (tempfile, "%stin_nntpXXXXXX", TMPDIR);
	mktemp (tempfile);

	if ((fp = fopen (tempfile, "w")) == (FILE *) 0) {
		perror_message (txt_article_cannot_open, tempfile);
		return ((FILE *) 0);
	}

	while ((ptr = tin_fgets(line, sizeof(line), nntp_rd_fp)) != NULL) {
		fputs (ptr, fp);
		fputs ("\n", fp);		/* The one case where we do need the \n */
								/* as rfc1521_decode() still expects this */
		/*
		 * Use the default message if one hasn't been supplied
		 * Body search is currently the only function that has a different message
		 */
#ifdef SHOW_PROGRESS
		if (lines && ++count % MODULO_COUNT_NUM == 0)
			show_progress((*msg=='\0') ? txt_reading_article : msg, count, lines);
#endif
	}

	if (tin_errno != 0) {
		fclose(fp);
		goto error;
	}

	if (ferror(fp)) {
		fclose(fp);
		tin_errno = 1;
		error_message(txt_filesystem_full, tempfile);
		goto error;
	}

	fclose(fp);

	/*
	 * Grab the correct filesize now that it's been closed
	 */
	if (stat (tempfile, &sb) < 0)
		note_size = 0;
	else
		note_size = sb.st_size;

	if ((fp = fopen (tempfile, "r")) == (FILE *) 0) {	/* Reopen for just reading */
		perror_message (txt_nntp_to_fp_cannot_reopen, tempfile);
		return (FILE *) 0;
	}


	/*
	 * It is impossible to delete an open file on the Amiga or Win32. So we keep a
	 * copy of the file name and delete it when finished instead.
	 */
#if defined(M_AMIGA) || defined(WIN32)
	log_unlink(fp, tempfile);
#else
error:
	unlink (tempfile);
#endif

	if (tin_errno == 0)
		return (fp);
	else
		return ((FILE *) 0);
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
	int lines)
{
	char buf[NNTP_STRLEN];
	int i;
	struct stat sb;
	FILE *fp, *art_fp;

	i = my_group[cur_groupnum];

#ifdef NNTP_ABLE
	if (read_news_via_nntp && active[i].type == GROUP_TYPE_NEWS) {

		sprintf (buf, "ARTICLE %ld", art);
		if (nntp_command (buf, OK_ARTICLE, NULL) == NULL)
			return (FILE *) 0;

		art_fp = get_article (lines);
	} else {
#endif /* NNTP_ABLE */
		joinpath (buf, active[i].spooldir, group_path);
		sprintf (&buf[strlen (buf)], "/%ld", art);

		/* Get the correct file size. This is done in get_article() for
		 * the NNTP case. TODO - fix this
		 */
		if (stat (buf, &sb) == -1)
			note_size = 0;
		else
			note_size = sb.st_size;

		art_fp = fopen (buf, "r");
#ifdef NNTP_ABLE
	}
#endif

	/*
	 * Do a bit of 1521 decoding. If art_fp=NULL, then it returns NULL
	 */
	fp = rfc1521_decode(art_fp);
	if (fp != art_fp)
		note_size = 0;

	return fp;
}

/* This will come in useful for filtering on non-overview hdr fields */
#if 0
static FILE *
open_xhdr_fp (
	char *header,
	long min,
	long max)
{
#ifdef NNTP_ABLE
	if (read_news_via_nntp) {
		char buf[NNTP_STRLEN];

		sprintf(buf, "XHDR %s %ld-%ld", header, min, max);
		return(nntp_command(buf, OK_HEAD));
	} else
#endif
		return (FILE *) 0;		/* Some trick implementation for local spool... */
}
#endif /* 0 */


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

int
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

#if 0
		/*
		 * Some nntp servers are broken and need an extra GROUP command
		 * (reported by reorx@irc.pl). This affects (old?) versions of
		 * nntpcache and leafnode. Usually this should not be needed.
		 */
		sprintf (buf, "GROUP %s", group->name);
		if (nntp_command(buf, OK_GROUP, line) == NULL)
			return(-1);
#endif /* 0*/

		/*
		 * See if LISTGROUP works
		 */
		sprintf (buf, "LISTGROUP %s", group->name);
		if (nntp_command(buf, OK_GROUP, NULL) != NULL) {
			char *ptr;

#ifdef DEBUG
			debug_nntp ("setup_base", buf);
#endif

			while ((ptr = tin_fgets(buf, sizeof(buf), nntp_rd_fp)) != NULL)  {
				if (top_base >= max_art)
					expand_art ();

				base[top_base++] = atoi (ptr);
			}

			if (tin_errno != 0)
				return(-1);

		} else {
			long start, last, count;
			char line[NNTP_STRLEN];

			/*
			 * Handle the obscure case that the user aborted before the LISTGROUP
			 * had a chance to respond
			 */
			if (tin_errno != 0)
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
#endif
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
				art = atol (e->d_name/*, D_NAMLEN(e)*/); /*e->d_name should be '\0' terminated... */
				if (art >= 1) {
					total++;
					if (top_base >= max_art)
						expand_art ();
					base[top_base++] = art;
				}
			}
			closedir (d);
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
vGet1GrpArtInfo(struct t_group *grp)
{
	long	lMinOld = grp->xmin;
	long	lMaxOld = grp->xmax;

	vGrpGetArtInfo (
		grp->spooldir,
		grp->name,
		grp->type,
		&grp->count,
		&grp->xmax,
		&grp->xmin);

	if (grp->newsrc.num_unread > grp->count) {
#ifdef DEBUG
		my_printf (cCRLF "Unread WRONG %s unread=[%ld] count=[%ld]", grp->name, grp->newsrc.num_unread, grp->count);
		my_flush ();
#endif
		grp->newsrc.num_unread = grp->count;
	}

	if (grp->xmin != lMinOld || grp->xmax != lMaxOld) {
		expand_bitmap(grp, grp->xmin);
#ifdef DEBUG
		my_printf (cCRLF "Min/Max DIFF %s old=[%ld-%ld] new=[%ld-%ld]", grp->name, lMinOld, lMaxOld, grp->xmin, grp->xmax);
		my_flush ();
#endif
	}
}

void
vGrpGetSubArtInfo (void)
{
#ifndef INDEX_DAEMON
	int	iNum;
	struct	t_group *psGrp;

	if (INTERACTIVE)
		wait_message (0, txt_rereading_active_file);

	for (iNum = 0 ; iNum < num_active ; iNum++) {
		psGrp = &active[iNum];

		if (psGrp->subscribed) {
			vGet1GrpArtInfo(psGrp);

#ifdef SHOW_PROGRESS
			if (iNum % 5 == 0)
				spin_cursor ();
#endif
		}
	}
	if (cmd_line) {
		printf ("\r\n");
		fflush (stdout);
	}
#endif
}

/*
 *  Find the total, max & min articles number for specified group
 *  Use nntp GROUP command or read local spool
 *  Return 0, or -error
 */
int
vGrpGetArtInfo (
	char	*pcSpoolDir,
	char	*pcGrpName,
	int	iGrpType,
	long	*plArtCount,
	long	*plArtMax,
	long	*plArtMin)
{
	char	acBuf[NNTP_STRLEN];
	DIR		*tDirFile;
	DIR_BUF	*tFile;
	long	lArtNum;
#ifdef M_AMIGA
	long	lArtMin;
	long	lArtMax;

	lArtMin = *plArtMin;
	lArtMax = *plArtMax;
#endif

	if (read_news_via_nntp && iGrpType == GROUP_TYPE_NEWS) {
#ifdef NNTP_ABLE
		char	acLine[NNTP_STRLEN];

		sprintf (acBuf, "GROUP %s", pcGrpName);
#ifdef DEBUG
		debug_nntp ("vGrpGetArtInfo", acBuf);
#endif
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
				tin_done (EXIT_NNTP_ERROR);
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
		return(0);
#endif	/* NNTP_ABLE */
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
					} else if (lArtNum < *plArtMin) {
						*plArtMin = lArtNum;
					}
					(*plArtCount)++;
				}
			}
			closedir (tDirFile);
		} else {
			return(-1);
		}
#endif /* M_AMIGA */
	}

	return(0);
}
