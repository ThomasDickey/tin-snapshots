/*
 *  Project   : tin - a Usenet reader
 *  Module    : open.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 21-12-94
 *  Notes     : Routines to make reading news locally (ie. /usr/spool/news)
 *              or via NNTP transparent
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"patchlev.h"

int nntp_codeno = 0;
long head_next;

/* error message from server */
char error_response[NNTP_STRLEN];

#ifdef NNTP_ABLE
int compiled_with_nntp = TRUE;		/* used in mail_bug_report() info */
#else
int compiled_with_nntp = FALSE;
#endif

#ifdef NO_POSTING
int	can_post = FALSE;
#else
int	can_post = TRUE;
#endif

char *nntp_server;

#ifdef NNTP_ABLE
int get_server_nolf=0; /* this will only be set by our own nntplib.c */
#endif

int
nntp_open ()
{
#ifdef NNTP_ABLE
	int ret;
	unsigned short nntp_tcp_port;

	if (read_news_via_nntp) {
		debug_nntp ("nntp_open", "BEGIN");

		nntp_server = getserverbyfile (NNTP_SERVER_FILE);
		nntp_tcp_port = atoi (get_val ("NNTPPORT", NNTP_TCP_PORT));

		if (nntp_server == (char *) 0) {
			error_message (txt_cannot_get_nntp_server_name, "");
			error_message (txt_server_name_in_file_env_var, NNTP_SERVER_FILE);
			return -1;
		}

		if (update == FALSE) {
			if (nntp_tcp_port != 119) {
				sprintf (msg, txt_connecting_port, nntp_server, nntp_tcp_port);
			} else {
				sprintf (msg, txt_connecting, nntp_server);
			}
			wait_message (msg);
		}

		debug_nntp ("nntp_open", nntp_server);

		ret = server_init (nntp_server, NNTP_TCP_NAME, nntp_tcp_port);
		if (update == FALSE && ret != -1 && cmd_line) {
			my_fputc ('\n', stdout);
		}

		debug_nntp_respcode (ret);

		switch (ret) {
		case OK_CANPOST:
#ifndef NO_POSTING
			can_post = TRUE;
#endif
			break;

		case OK_NOPOST:
			can_post = FALSE;
			wait_message(txt_cannot_post);
			fputc ('\n', stdout);
			break;

		case -1:
			error_message (txt_failed_to_connect_to_server, nntp_server);
			return -1;

		default:
			nntp_message (ret);
			return -1;
		}

		/*
		 * Switch INN into NNRP mode with 'mode reader'
		 */
		debug_nntp ("nntp_open", "mode reader");
		put_server ("mode reader");
		ret = get_respcode ();
		switch (ret) {
			case OK_CANPOST:
			case OK_NOPOST:
				break;
			case ERR_ACCESS:
				nntp_message (ret);
				return -1;
			default:
				break;
		}

		/*
		 * Check if NNTP/INN supports XOVER command
		 */
		debug_nntp ("nntp_open", "xover");
		put_server ("xover");
		if (get_respcode () != ERR_COMMAND) {
			xover_supported = TRUE;
		}

		/*
		 * Check if NNTP supports my XINDEX & XUSER commands
		 */
#ifdef HAVE_TIN_NNTP_EXTS
		debug_nntp ("nntp_open", "xuser");
		put_server ("xuser");
		if (get_respcode () != ERR_COMMAND) {
			xuser_supported = TRUE;
		}
#endif	/* HAVE_TIN_NNTP_EXTS */

		/*
		 * Check if NNTP server expects user authorization
		 */
		authorization (nntp_server, userid);
	}
#endif	/* NNTP_ABLE */

	return 0;
}


void
nntp_close ()
{
#ifdef NNTP_ABLE
	if (read_news_via_nntp) {
		debug_nntp ("nntp_close", "END");
		close_server ();
	}
#endif
}

/*
 * Open the mail active file locally
 */

FILE *
open_mail_active_fp (mode)
	char *mode;
{
	return fopen (mail_active_file, mode);
}

/*
 * Open the news active file locally or send the LIST command via NNTP
 */

FILE *
open_news_active_fp ()
{
#ifdef NNTP_ABLE
	int respcode;
#endif

	if (read_news_via_nntp) {
#ifdef NNTP_ABLE
		put_server ("list");
		if ((respcode = get_respcode ()) != OK_GROUPS) {
			debug_nntp ("open_news_active_fp", "NOT_OK");
			error_message ("%s", nntp_respcode (respcode));
			return (FILE *) 0;
		}
		debug_nntp ("open_news_active_fp", "OK");
		return nntp_to_fp ();
#else
		return (FILE *) 0;
#endif
	} else {
		return fopen (news_active_file, "r");
	}
}

/*
 * Open the NEWSLIBDIR/overview.fmt file locally or send the LIST OVERVIEW.FMT
 * command via NNTP
 */

FILE *
open_overview_fmt_fp ()
{
	char line[NNTP_STRLEN];

	if (read_news_via_nntp) {
#ifdef NNTP_ABLE
		if (xover_supported) {
			sprintf (line, "list %s", OVERVIEW_FMT);
			debug_nntp ("open_overview_fmt_fp", line);
			put_server (line);
			if (get_respcode () != OK_GROUPS) {
				debug_nntp ("open_overview_fmt_fp", "NOT_OK");
				return (FILE *) 0;
			}
			debug_nntp ("open_overview_fmt_fp", "OK");
			return nntp_to_fp ();
		} else {
			return (FILE *) 0;
		}
#else
		return (FILE *) 0;
#endif
	} else {
		joinpath (line, libdir, OVERVIEW_FMT);
		return fopen (line, "r");
	}
}

/*
 * Open the ~/.tin/active file locally or send the NEWGROUPS command via NNTP
 *
 * NEWGROUPS 311299 235959
 */

FILE *
open_newgroups_fp (the_index)
	int the_index;
{
#ifdef NNTP_ABLE
	char line[NNTP_STRLEN];
	struct tm *tm;
#endif

	if (read_news_via_nntp) {
#ifdef NNTP_ABLE
		if (the_index == -1) {
			return (FILE *) 0;
		}

		tm = localtime (&newnews[the_index].time);
		sprintf (line, "newgroups %02d%02d%02d %02d%02d%02d",
			tm->tm_year, tm->tm_mon+1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec);

		debug_nntp ("open_newgroups_fp", line);
		put_server (line);

		if (get_respcode () != OK_NEWGROUPS) {
			debug_nntp ("open_newgroups_fp", "NOT_OK");
			return (FILE *) 0;
		}
		debug_nntp ("open_newgroups_fp", "OK");
		return nntp_to_fp ();
#else
		return (FILE *) 0;
#endif
	} else {
		return fopen (active_times_file, "r");
	}
}

/*
 * Open the news motd file locally or on the NNTP server
 *
 * XMOTD 311299 235959 [GMT]
 */

FILE *
open_motd_fp (motd_file_date)
	char *motd_file_date;
{
#if defined(NNTP_ABLE) && defined(HAVE_TIN_NNTP_EXTS)
	char line[NNTP_STRLEN];
#endif

	if (read_news_via_nntp) {
#if defined(NNTP_ABLE) && defined(HAVE_TIN_NNTP_EXTS)
		sprintf (line, "xmotd %s", motd_file_date);
		debug_nntp ("open_motd_fp", line);
		put_server (line);
		if (get_respcode () != OK_XMOTD) {
			debug_nntp ("open_motd_fp", "NOT_OK");
			return (FILE *) 0;
		}
		debug_nntp ("open_motd_fp", "OK");
		return nntp_to_fp ();
#else
		return (FILE *) 0;
#endif
	} else {
		return fopen (motd_file, "r");
	}
}


FILE *
open_subscription_fp ()
{
	if (read_news_via_nntp) {
#ifdef NNTP_ABLE
		put_server ("list subscriptions");
		if (get_respcode () != OK_GROUPS) {
			debug_nntp ("open_subscription_fp", "NOT_OK");
			return (FILE *) 0;
		}
		debug_nntp ("open_subscription_fp", "OK");
		return nntp_to_fp ();
#else
		return (FILE *) 0;
#endif
	} else {
		return fopen (subscriptions_file, "r");
	}
}

/*
 *  Open mail groups description file.
 */

FILE *
open_mailgroups_fp ()
{
	return fopen (mailgroups_file, "r");
}


/*
 * If reading via NNTP the newsgroups file will be saved to ~/.tin/newsgroups
 * so that any subsequent rereads on the active file will not have to waste
 * net bandwidth and the local copy of the newsgroups file can be accessed.
 */

FILE *
open_newsgroups_fp ()
{
	if (read_news_via_nntp) {
#ifdef NNTP_ABLE
		if (read_local_newsgroups_file) {
			debug_nntp ("open_newsgroups_fp", "Using local copy of newsgroups file");
			return fopen (local_newsgroups_file, "r");
		} else {
			put_server ("list newsgroups");
			if (get_respcode () != OK_GROUPS) {
				debug_nntp ("open_newsgroups_fp", "NOT_OK");
				return (FILE *) 0;
			}
			debug_nntp ("open_newsgroups_fp", "OK");
			return nntp_to_fp ();
		}
#else
		return (FILE *) 0;
#endif
	} else {
		return fopen (newsgroups_file, "r");
	}
}

/*
 * Open a group Nov/XOVER file
 */

FILE *
open_xover_fp (psGrp, pcMode, lMin, lMax)
	struct t_group *psGrp;
	char *pcMode;
	long lMin;
	long lMax;
{
	char *pcNovFile;
#ifdef NNTP_ABLE
	char acLine[NNTP_STRLEN];
#endif

	if (read_news_via_nntp && xover_supported &&
	    *pcMode == 'r' && psGrp->type == GROUP_TYPE_NEWS) {
#ifdef NNTP_ABLE
		sprintf (acLine, "xover %ld-%ld", lMin, lMax);
		debug_nntp ("open_xover_fp", acLine);
		put_server (acLine);
		if (get_respcode () != OK_XOVER) {
			debug_nntp ("open_xover_fp", "NOT_OK");
			return (FILE *) 0;
		}
		debug_nntp ("open_xover_fp", "OK");
		return nntp_to_fp ();
#else
		return (FILE *) 0;
#endif
	} else {
		pcNovFile = pcFindNovFile (psGrp, (*pcMode == 'r' ? R_OK : W_OK));

		if (debug) {
			error_message ("READ file=[%s]", pcNovFile);
		}
		if (pcNovFile != (char *) 0) {
			return fopen (pcNovFile, pcMode);
		}
		return (FILE *) 0;
	}
}

/*
 * Stat a mail/news article to see if it still exists
 */

int
stat_article (art, group_path)
	long art;
	char *group_path;
{
/*
#ifdef NNTP_ABLE
	int respcode;
#endif
*/
	char buf[NNTP_STRLEN];
	int i;
	int art_exists = TRUE;
	struct stat sb;

	i = my_group[cur_groupnum];

	if (read_news_via_nntp && active[i].type == GROUP_TYPE_NEWS) {
#ifdef NNTP_ABLE
		sprintf (buf, "stat %ld", art);
		debug_nntp ("stat_article", buf);
		put_server (buf);
		if (/*(respcode = */ get_respcode ()/*)*/ != OK_NOTEXT) {
			art_exists = FALSE;
		}
#endif
	} else {
		joinpath (buf, active[i].spooldir, group_path);
		sprintf (&buf[strlen (buf)], "/%ld", art);

		if (stat (buf, &sb) == -1) {
			art_exists = FALSE;
		}
	}

	return art_exists;
}

char *
open_art_header (art)
	long art;
{
#ifdef NNTP_ABLE
	int safe_nntp_strlen, full, len;
	char *ptr;
#endif
	char buf[NNTP_STRLEN];
	FILE *fp;
	int items = 0;
	static char mem[HEADER_LEN];

	if (read_news_via_nntp && CURR_GROUP.type == GROUP_TYPE_NEWS) {
#ifdef NNTP_ABLE
		/*
		 *  Don't bother requesting if we have not got there yet.
		 *  This is a big win if the group has got holes in it (ie. if 000's
		 *  of articles have expired between active files min & max values).
		 */
		if (art < head_next) {
			return (char *) 0;
		}
		sprintf (buf, "head %ld", art);

		debug_nntp ("open_art_header", buf);

		put_server (buf);
		if (get_respcode () != OK_HEAD) {
			debug_nntp ("open_art_header", "NOT_OK_HEAD - Find NEXT");
			/*
			 *  HEAD failed, try to find NEXT
			 */
			put_server ("next");
			switch (get_server (buf, NNTP_STRLEN)) {
			case -1:
				error_message (txt_connection_to_server_broken, "");
				tin_done (EXIT_NNTP_ERROR);
			case -2:
				tin_done (0);
			}
			if (atoi (buf) == OK_NOTEXT) {
				ptr = buf;
				while (isspace(*ptr)) {
					ptr++;
				}
				while (isdigit(*ptr)) {
					ptr++;
				}
				head_next = atoi (ptr);
			}
			return (char *) 0;
		}
		debug_nntp ("open_art_header", "OK_HEAD");

		full = FALSE;
		safe_nntp_strlen = NNTP_STRLEN - 2;
		len = safe_nntp_strlen;
		ptr = mem;
		forever {
			if (full || len < 32) {
				full = TRUE;
				ptr = buf;
				len = safe_nntp_strlen;
			}
			switch (get_server (ptr, len)) {
			case -1:
				error_message (txt_connection_to_server_broken, "");
				tin_done (EXIT_NNTP_ERROR);
			case -2:
				tin_done (0);
			}
			if (STRCMPEQ(ptr, ".")) {	/* end of text */
				break;
			}
			if (ptr[0] == '.') {	/* reduce leading .'s */
				while (ptr[1]) {
					ptr[0] = ptr[1];
					ptr++;
					len++;
				}
			} else {
				while (*ptr) {
					ptr++;
					len++;
				}
				*ptr++ = '\n';
				*ptr = '\0';
				len++;
			}
		}
#else
		return (char *) 0;
#endif
	} else {
		sprintf (buf, "%ld", art);
		fp = fopen (buf, "r");
		if (fp != (FILE *) 0) {
			items = fread (mem, 1, sizeof (mem)-1, fp);
			fclose (fp);
		}
/*
printf ("Artnum=[%ld] Items=[%d]\n", art, items);
fflush (stdout);
sleep (1);
*/
		if (items == 0) {
			return (char *) 0;
		}
	}
	return mem;
}

/*
 * Open a mail/news article
 */

FILE *
open_art_fp (group_path, art)
	char *group_path;
	long art;
{
	char buf[NNTP_STRLEN];
	int i;
	struct stat sb;
#ifdef NNTP_ABLE
	int respcode;
#endif
	FILE *fp, *old_fp;

	i = my_group[cur_groupnum];

	if (read_news_via_nntp && active[i].type == GROUP_TYPE_NEWS) {
#ifdef NNTP_ABLE
		sprintf (buf, "article %ld", art);
		debug_nntp ("open_art_fp", buf);
		put_server (buf);
		if ((respcode = get_respcode ()) != OK_ARTICLE) {
			if (debug == 2) {
				error_message ("%s", nntp_respcode (respcode));
			}
			debug_nntp ("open_art_fp", "NOT OK");
			return (FILE *) 0;
		}

		debug_nntp ("open_art_fp", "OK");

		return nntp_to_fp ();
#else
		return (FILE *) 0;
#endif
	} else {
		joinpath (buf, active[i].spooldir, group_path);
		sprintf (&buf[strlen (buf)], "/%ld", art);

		if (debug == 2) {
			error_message ("ART=[%s]", buf);
		}

		if (stat (buf, &sb) == -1) {
			note_size = 0;
		} else {
			note_size = sb.st_size;
		}
		fp=rfc1521_decode(old_fp=fopen (buf, "r"));
		if(fp!=old_fp)
		  note_size=0;
		return fp;
	}
}


FILE *
open_xhdr_fp (header, min, max)
	char *header;
	long min;
	long max;
{
	if (read_news_via_nntp) {
#ifdef NNTP_ABLE
		char buf[NNTP_STRLEN];
		sprintf(buf, "xhdr %s %ld-%ld", header, min, max);

		debug_nntp ("open_xhdr_fp", buf);

		put_server (buf);
		if (get_respcode () != OK_HEAD) {
			debug_nntp ("open_xhdr_fp", "NOT_OK_XHDR");
			return (FILE *) 0;
		}

		debug_nntp ("open_xhdr_fp", "OK_XHDR");

		return nntp_to_fp ();
#else
		return (FILE *) 0;
#endif
	} else {
		return (FILE *) 0;
	}
}

/*
 *  Longword comparison routine for the qsort()
 */

int
base_comp (p1, p2)
	t_comptype *p1;
	t_comptype *p2;
{
	long *a = (long *) p1;
	long *b = (long *) p2;

	if (*a < *b)
		return -1;
	if (*a > *b)
		return 1;
	return 0;
}


/*
 * looks like setup_soft_base is not needed... -> strip the code
 * anyone remembering who #if'ed it out? or where it was used?
 */

/*
 *  Read the article numbers existing in arts[] that was filled from nov file.
 *  Returns total number of articles in group.
 */

#if 0
int
setup_soft_base (group)
	struct t_group *group;
{
	for (top_base=0; top_base < top; top_base++) {
		base[top_base] = arts[top_base].artnum;
	}

	if (top_base) {
		group->xmax = base[top_base-1];
		expand_bitmap (group, base[0]);
	}

	return top_base-1;
}
#endif

/*
 *  Read the article numbers existing in a group's spool directory
 *  into base[] and sort them.  top_base is one past top.
 *  Returns total number of articles in group.
 */

int
setup_hard_base (group, group_path)
	struct t_group *group;
	char *group_path;
{
	char buf[NNTP_STRLEN];
#ifdef NNTP_ABLE
	char line[NNTP_STRLEN];
	long start, last, dummy, count;
#endif
	DIR *d;
	DIR_BUF *e;
	long art;
	long total = 0;

	top_base = 0;

	if (read_news_via_nntp && group->type == GROUP_TYPE_NEWS) {
#ifdef NNTP_ABLE
		sprintf (buf, "group %s", group->name);

		debug_nntp ("setup_base", buf);

		put_server (buf);

			switch (get_server (line, NNTP_STRLEN)) {
			case -1:
			error_message (txt_connection_to_server_broken, "");
			tin_done (EXIT_NNTP_ERROR);
			case -2:
				tin_done (0);
		}

		if (atoi (line) != OK_GROUP) {
			debug_nntp ("setup_base", "NOT_OK");
			return total;
		}

		debug_nntp ("setup_base", line);

		sscanf (line,"%ld %ld %ld %ld", &dummy, &count, &start, &last);

		total = count;

		sprintf (buf, "listgroup %s", group->name);
		debug_nntp ("setup_base", buf);
		put_server (buf);
			switch (get_server (line, NNTP_STRLEN)) {
			case -1:
			error_message (txt_connection_to_server_broken, "");
			tin_done (EXIT_NNTP_ERROR);
			case -2:
				tin_done (0);
		}
		if (atoi (line) == OK_GROUP) {
			debug_nntp ("setup_base", line);
			forever {
					switch (get_server (line, NNTP_STRLEN)) {
					case -1:
					error_message (txt_connection_to_server_broken, "");
					tin_done (EXIT_NNTP_ERROR);
					case -2:
						tin_done (0);
				}
				if (STRCMPEQ(line, ".")) {
					debug_nntp ("setup_base", line);
					break;	/* end of text */
				}
				if (top_base >= max_art) {
					expand_art ();
				}
				base[top_base++] = atoi (line);
			}
		} else {
			debug_nntp ("setup_base, listgroup", "NOT_OK");
			if (last - count > start) {
				count = last - start;
			}

			while (start <= last) {
				if (top_base >= max_art) {
					expand_art();
				}
				base[top_base++] = start++;
			}
		}
#endif
	} else {
		joinpath (buf, group->spooldir, group_path);

		if (access (buf, R_OK) != 0) {
			return total;
		}

		d = opendir (buf);
		if (d != NULL) {
			while ((e = readdir (d)) != NULL) {
				art = my_atol (e->d_name, D_NAMLEN(e));
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

#ifdef HAVE_GENERIC_AUTHINFO

/*
 * process authinfo generic.
 * 0 means succeeded.
 * 1 means failed
 */

int
authenticate ()
{
	extern FILE *nntp_rd_fp, *nntp_wr_fp;
	char tmpbuf[NNTP_STRLEN], *authval, *p;
	char *authcmd;
	FILE *fp;
	int builtinauth = 0;
	static int cookiefd = -1;
#ifdef HAVE_PUTENV
	char *new_env;
	static char *old_env = 0;
#endif

	/*
	 * If we have authenticated before, NNTP_AUTH_FDS already
	 * exists, pull out the cookiefd. Just in case we've nested.
	 */
	if (cookiefd == -1 && (authcmd = getenv ("NNTP_AUTH_FDS"))) {
	    sscanf (authcmd, "%*d.%*d.%d", &cookiefd);
	}

	if (cookiefd == -1) {
		char *tempfile = mktemp ("/tmp/tinAXXXXXX");
		fp = fopen (tempfile, "w+");
		if (! fp) {
			error_message ("Can't open %s", tempfile);
			return 1;
		}
		(void) unlink (tempfile);
		cookiefd = fileno (fp);
	}

	strcpy (tmpbuf, "authinfo generic ");
	if (authval = getenv ("NNTPAUTH")) {
		strcat (tmpbuf, authval);
	} else {
		strcat (tmpbuf, "any ");
		strcat (tmpbuf, userid);
		builtinauth = 1;
	}
	put_server (tmpbuf);

#ifdef HAVE_PUTENV
	sprintf (tmpbuf, "NNTP_AUTH_FDS=%d.%d.%d",
		fileno(nntp_rd_fp), fileno(nntp_wr_fp), cookiefd);
	new_env = my_malloc (strlen (tmpbuf)+1);
	strcpy (new_env, tmpbuf);
 	putenv (new_env);
	if (old_env) {
		free (old_env);
	}
	old_env = new_env;
#else
	sprintf (tmpbuf, "%d.%d.%d",
		fileno(nntp_rd_fp), fileno(nntp_wr_fp), cookiefd);
	setenv ("NNTP_AUTH_FDS", tmpbuf, 1);
#endif

	if (! builtinauth) {
		return (system (authval));
	} else {
		get_server (tmpbuf, sizeof(tmpbuf));
		return (atoi (tmpbuf) != OK_AUTH);	/* 0 = okay */
	}
}

#endif	/* HAVE_GENERIC_AUTHINFO */

/*
 *  Get a response code from the server and return it to the caller
 *  Also does AUTHINFO user/pass or GENERIC authorization
 */

int
get_respcode ()
{
#ifdef NNTP_ABLE
	char line[NNTP_STRLEN];
	int respcode;

	switch (get_server (line, NNTP_STRLEN)) {
	case -1:
		error_message (txt_connection_to_server_broken, "");
		tin_done (EXIT_NNTP_ERROR);
	case -2:
		tin_done (0);
	}

	debug_nntp ("get_respcode", line);

	/* save error message from server */
	strcpy(error_response, line);

	respcode = atoi (line);

#ifdef USE_GENAUTH
	if (respcode == ERR_NOAUTH) {
		char savebuf[NNTP_STRLEN];

		strcpy (savebuf, last_put);

		if (authenticate ()) {
			sprintf(line, "%d Authentication failed", ERR_ACCESS);
		} else {
			strcpy (last_put, savebuf);
			put_server (last_put);
			get_server (line, NNTP_STRLEN);
		}
		respcode = atoi (line);
	}
#endif
	return respcode;
#else
	return 0;
#endif
}


int
stuff_nntp (fnam)
	char *fnam;
{
#ifdef NNTP_ABLE
	char line[HEADER_LEN];
	FILE *fp;
/*	int count = 0; */
	struct stat sb;
	int last_line_nolf=0;

	sprintf (fnam, "%stin_nntpXXXXXX", TMPDIR);
	mktemp (fnam);

	if ((fp = fopen (fnam, "w")) == (FILE *) 0) {
		perror_message (txt_stuff_nntp_cannot_open, fnam);
		return FALSE;
	}

	forever {
		switch (get_server (line, sizeof (line)-1)) {
		case -1:
			error_message (txt_connection_to_server_broken, "");
			tin_done (EXIT_NNTP_ERROR);
		case -2:
			tin_done (0);
		}


#ifdef DEBUG
		debug_nntp ("stuff_nntp", line);
#endif
		if (!last_line_nolf && STRCMPEQ(line, ".")) {	/* end of text */
			break;
		}
		if(!get_server_nolf) {
			strcat (line, "\n");
			if (line[0] == '.') {	/* reduce leading .'s */
				fputs (&line[1], fp);
			} else {
				fputs (line, fp);
			}
		} else {
			fputs (line, fp);
		}
/*
		if (++count % 50 == 0) {
			spin_cursor ();
		}
*/
	}
	fclose (fp);

	if (stat (fnam, &sb) < 0) {
		note_size = 0;
	} else {
		note_size = sb.st_size;
	}
#endif
	return TRUE;
}


FILE *
nntp_to_fp ()
{
#ifdef NNTP_ABLE
	char fnam[PATH_LEN];
	FILE *fp;
	FILE *old_fp;

	if (! stuff_nntp (fnam)) {
		debug_nntp ("nntp_to_fp", "! stuff_nntp()");
		return (FILE *) 0;
	}

	if ((fp = rfc1521_decode(old_fp=fopen (fnam, "r"))) == (FILE *) 0) {
		perror_message (txt_nntp_to_fp_cannot_reopen, fnam);
		return (FILE *) 0;
	}
	if(old_fp!=fp)
	  note_size=0;

/*
 * It is impossible to delete an open file on the Amiga or Win32. So we keep a
 * copy of the file name and delete it when finished instead.
 */

#if defined(M_AMIGA) || defined(WIN32)
	log_unlink(fp, fnam);
#else
	unlink (fnam);
#endif
	return fp;
#else
	return (FILE *) 0;
#endif
}

/*
 * Log user info to local file or NNTP logfile
 */

void
log_user ()
{
	char dummy[PATH_LEN];
	char buf[32], *ptr;
	char line[NNTP_STRLEN];
#ifdef	LOG_USER
	char log_file[PATH_LEN];
	FILE *fp;
	long epoch;
#endif
#ifndef M_AMIGA
	get_user_info (dummy, buf);

	if (read_news_via_nntp && xuser_supported) {
		if ((ptr = strchr (buf, ','))) {
			*ptr = '\0';
		}
		sprintf (line, "xuser %s (%s)", myentry->pw_name, buf);

		debug_nntp ("log_user", line);
		put_server (line);
	} else
#endif	/* M_AMIGA */
	{
#ifdef	LOG_USER
		joinpath (log_file, TMPDIR, LOG_USER_FILE);

		if ((fp = fopen (log_file, "a+")) != (FILE *) 0) {
			time (&epoch);
			fprintf (fp, "%s%s: %-32s (%-8s) %s",
				VERSION, RELEASEDATE,
#ifdef M_AMIGA
				get_val ("REALNAME", "Unknown"),
				get_val ("USERNAME", "Unknown"),
#else
				buf,
				myentry->pw_name,
#endif
				ctime (&epoch);
			fclose (fp);
			chmod (log_file, 0666);
		}
#endif	/* LOG_USER */
	}
}

/*
 * NNTP user authorization. Password read from ~/.newsauth
 * The ~/.newsauth authorization file has the format:
 *   nntpserver1 password [user]
 *   nntpserver2 password [user]
 *   etc.
 */

void
authorization (server, authuser)
	char *server;
	char *authuser;
{
	static char already_failed = 0;
	char line[PATH_LEN];
	char line2[PATH_LEN];
	char *authpass;
	char *ptr;
	FILE *fp;
	int ret;

	/*
	 * Check if running via NNTP
	 */
	if (! read_news_via_nntp) {
		return;
	}

	/*
	 * don't try again if failed before
	 */
	if(already_failed) {
		return;
	}

	/*
	 * Lets check if the NNTP supports authorization
	 */
	debug_nntp ("authorization", "authinfo");
	put_server ("authinfo");
	if (get_respcode () == ERR_COMMAND) {
		return;
	}

	joinpath (line, homedir, ".newsauth");

	if ((fp = fopen (line,"r")) == (FILE *) 0)
		return;

	/*
	 * Search through authorization file for correct NNTP server
	 * File has format:  'nntp-server' 'password'
	 * will return authpass != NULL if any match
	 */
	authpass = (char *) 0;
	while (fgets (line, PATH_LEN, fp) != (char *) 0) {

		/*
		 * strip trailing newline character
		 */

		ptr = strchr (line, '\n');
		if(ptr != (char *) 0)
			*ptr = '\0';

		/*
		 * Get server from 1st part of the line
		 */

		ptr = strchr (line, ' ');

		if(ptr == (char *) 0)		/* no passwd, no auth, skip */
			continue;

		*ptr++ = '\0'; 			/* cut of server part */

		if ((strcasecmp(line, server)))
			continue;		/* wrong server, keep on */

		/*
		 * Get password from 2nd part of the line
		 */

		authpass = ptr;
		while(*authpass == ' ')
			authpass++;		/* skip any blanks */

		/*
		 * Get user from 3rd part of the line
		 */

		ptr = authpass;			/* continue searching here */

		if(*authpass == '"') {		/* skip "embedded" password string */
			ptr = strrchr (authpass,'"');
			if((ptr != (char *) 0) && (ptr > authpass)) {
				authpass++;
				*ptr++ = '\0';	/* cut off trailing " */
			} else			/* no matching ", proceede as normal */
				ptr = authpass;
		}

		ptr = strchr (ptr,' ');		/* find next separating blank */

		if(ptr != (char *) 0) {		/* a 3rd argument follows */
			while(*ptr == ' ')	/* skip any blanks */
				*ptr++ = '\0';
			if(*ptr != '\0')	/* if its not just empty */
				authuser = ptr;	/* so will replace default user */
		}

		break;	/* if we end up here, everything seems OK */
	}
	fclose (fp);

	if (authpass == (char *) 0) {
		error_message (txt_nntp_authorization_failed, server);
		return;
	}

	sprintf (line2, "authinfo user %s", authuser);
	put_server (line2);
	ret = get_respcode ();
	if (ret != NEED_AUTHDATA) {
		nntp_message (ret);
		already_failed = 1;
		return;
	}

	sprintf (line2, "authinfo pass %s", authpass);
	put_server (line2);
	ret = get_respcode ();
	if (ret != OK_AUTH) {
		nntp_message (ret);
		already_failed = 1;
		return;
	}

	sprintf (line, txt_authorization_ok, authuser);
	wait_message (line);
	return;
}

void
vGrpGetSubArtInfo ()
{
#ifndef INDEX_DAEMON
	int	iNum;
	long	lMinOld;
	long	lMaxOld;
	struct	t_group *psGrp;

	if ((update && update_fork) || ! update) {
		wait_message (txt_rereading_active_file);
	}

	for (iNum = 0 ; iNum < num_active ; iNum++) {
		psGrp = &active[iNum];
		if (psGrp->subscribed == SUBSCRIBED) {
			lMinOld = psGrp->xmin;
			lMaxOld = psGrp->xmax;
			vGrpGetArtInfo (
				psGrp->spooldir,
				psGrp->name,
				psGrp->type,
				&psGrp->count,
				&psGrp->xmax,
				&psGrp->xmin);
			if (psGrp->newsrc.num_unread > psGrp->count) {
#ifdef DEBUG
	printf ("\r\nUnread WRONG [%d]=%s unread=[%ld] count=[%ld]",
		iNum, psGrp->name, psGrp->newsrc.num_unread, psGrp->count);
	fflush(stdout);
#endif
				psGrp->newsrc.num_unread = psGrp->count;
			}
			if (psGrp->xmin != lMinOld || psGrp->xmax != lMaxOld) {
				expand_bitmap(psGrp,psGrp->xmin);
#ifdef DEBUG
	printf ("\r\nMin/Max DIFF [%d]=%s old=[%ld-%ld] new=[%ld-%ld]",
		iNum, psGrp->name, lMinOld, lMaxOld, psGrp->xmin, psGrp->xmax);
	fflush(stdout);
#endif
			}
			if (iNum % 5 == 0) {
				spin_cursor ();
			}
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
 */

void
vGrpGetArtInfo (pcSpoolDir, pcGrpName, iGrpType, plArtCount, plArtMax, plArtMin)
	char	*pcSpoolDir;
	char	*pcGrpName;
	int		iGrpType;
	long	*plArtCount;
	long	*plArtMax;
	long	*plArtMin;
{
	char	acBuf[NNTP_STRLEN];
#ifdef NNTP_ABLE
	char	acLine[NNTP_STRLEN];
	long	lDummy;
#endif
	DIR		*tDirFile;
	DIR_BUF	*tFile;
	long	lArtMin;
	long	lArtMax;
	long	lArtNum;

	lArtMin = *plArtMin;
	lArtMax = *plArtMax;

	*plArtCount = 0;
	*plArtMax = 0;
	*plArtMin = 1;

	if (read_news_via_nntp && iGrpType == GROUP_TYPE_NEWS) {
#ifdef NNTP_ABLE

		sprintf (acBuf, "group %s", pcGrpName);

		debug_nntp ("vGrpGetArtInfo", acBuf);

		put_server (acBuf);

		switch (get_server (acLine, NNTP_STRLEN)) {
		case -1:
			error_message (txt_connection_to_server_broken, "");
			tin_done (EXIT_NNTP_ERROR);
		case -2:
			tin_done (0);
		}

		if (atoi (acLine) != OK_GROUP) {
			debug_nntp ("NOT_OK", acLine);
			if (atoi (acLine) == ERR_ACCESS) {
				error_message ("\r\n%s", acLine);
				tin_done (EXIT_NNTP_ERROR);
			}
			*plArtMin = lArtMin;
			*plArtMax = lArtMax;
			return;
		}

		debug_nntp ("vGrpGetArtInfo", acLine);

		sscanf (acLine, "%ld %ld %ld %ld", &lDummy, plArtCount, plArtMin, plArtMax);
#else
		*plArtMin = lArtMin;
		*plArtMax = lArtMax;
		return;
#endif
	} else {
#ifndef M_AMIGA
		vMakeGrpPath (pcSpoolDir, pcGrpName, acBuf);

		if (access (acBuf, R_OK) != 0) {
			*plArtMin = lArtMin;
			*plArtMax = lArtMax;
			return;
		}

		tDirFile = opendir (acBuf);
		if (tDirFile != (DIR *) 0) {
			while ((tFile = readdir (tDirFile)) != (DIR_BUF *) 0) {
				lArtNum = lAtol (tFile->d_name, (int) D_NAMLEN(tFile));
				if (lArtNum >= 1) {
					if (lArtNum > *plArtMax) {
						*plArtMax = lArtNum;
						if (*plArtMin == 0) {
							*plArtMin = lArtNum;
						}
					} else if (lArtNum < *plArtMin) {
						*plArtMin = lArtNum;
					}
					(*plArtCount)++;
				}
			}
			closedir (tDirFile);
		}
#else
		if (!lArtMin) *plArtMin = 1;
		*plArtMax = lArtMax;
		*plArtCount = lArtMax - *plArtMin + 1;
#endif
	}

	if (*plArtCount == 0) {
		*plArtMin = lArtMin;
		*plArtMax = lArtMax;
	}
}
