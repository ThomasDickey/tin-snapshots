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
#include	"version.h"

#ifdef NNTP_ABLE
static int authorization (char *server, char *authuser);
#endif

long head_next;

/* error message from server */
char error_response[NNTP_STRLEN];

#ifdef NNTP_ABLE
int compiled_with_nntp = TRUE;		/* used in mail_bug_report() info */
#else
int compiled_with_nntp = FALSE;
#endif

#ifdef NO_POSTING
int can_post = FALSE;
#else
int can_post = TRUE;
#endif

char *nntp_server = (char *)0;

#ifdef NNTP_ABLE
int get_server_nolf=0; /* this will only be set by our own nntplib.c */
#endif

/* fixme - return different values for different errors
** i.e.: -1 = connection refused
**       -2 = connection timed out
**
** this is needed because get_server often fails to reconnect
** to the server on a first try...
*/

int
nntp_open (void)
{
#ifdef NNTP_ABLE
	int ret;
	static unsigned short nntp_tcp_port;

	if (read_news_via_nntp) {
		debug_nntp ("nntp_open", "BEGIN");

		if (nntp_server == (char *) 0) {
		/* do this only once at start-up */
			nntp_server = getserverbyfile (NNTP_SERVER_FILE);
			nntp_tcp_port = atoi (get_val ("NNTPPORT", NNTP_TCP_PORT));
		}
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

	}
#endif	/* NNTP_ABLE */

	return 0;
}


void
nntp_close (void)
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
#ifdef HAVE_MAIL_MH_HANDLING
FILE *
open_mail_active_fp (
	char *mode)
{
	return fopen (mail_active_file, mode);
}
#endif

/*
 * Open the news active file locally or send the LIST command via NNTP
 */

FILE *
open_news_active_fp (void)
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
open_overview_fmt_fp (void)
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
open_newgroups_fp (
	int the_index)
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
open_motd_fp (
	char *motd_file_date)
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
open_subscription_fp (void)
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
#ifdef HAVE_MAIL_MH_HANDLING
FILE *
open_mailgroups_fp (void)
{
	return fopen (mailgroups_file, "r");
}
#endif

/*
 * If reading via NNTP, allow the special case where the user's telling us to
 * read the groups in .newsrc file, and to suppress the check for new groups.
 * This won't detect the flag for moderated newsgroups, but will greatly speed
 * up reading news over a slow line.
 *
 * see: read_groups_descriptions(), which reads this data.
 */
#ifdef NNTP_ABLE
static FILE *
extract_groups_from_newsrc(void)
{
	FILE *ifp, *ofp;
	char buf[LEN];
	char *p;

	if ((ifp = fopen(newsrc, "r")) != 0) {
		if ((ofp = fopen(local_newsgroups_file, "w")) != 0) {
			while (fgets (buf, sizeof (buf), ifp) != 0) {
				for (p = buf; *p && !isspace(*p); p++)
					;
				p[-1] = '\n';
				p[0]  = '\0';
				fputs(buf, ofp);
			}
			fclose(ifp);
			fclose(ofp);
			read_local_newsgroups_file = TRUE;
			return fopen (local_newsgroups_file, "r");
		}
		fclose(ifp);
	}
	return 0;
}
#endif

/*
 * If reading via NNTP the newsgroups file will be saved to ~/.tin/newsgroups
 * so that any subsequent rereads on the active file will not have to waste
 * net bandwidth and the local copy of the newsgroups file can be accessed.
 */

FILE *
open_newsgroups_fp (void)
{
	if (read_news_via_nntp) {
#ifdef NNTP_ABLE
		if (read_local_newsgroups_file) {
			debug_nntp ("open_newsgroups_fp", "Using local copy of newsgroups file");
			return fopen (local_newsgroups_file, "r");
		} else if (newsrc_active && !check_for_new_newsgroups) {
			debug_nntp ("open_newsgroups_fp", "Using info in .newsrc");
			return extract_groups_from_newsrc();
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
open_xover_fp (
	struct t_group *psGrp,
	const char *pcMode,
	long lMin,
	long lMax)
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
stat_article (
	long art,
	char *group_path)
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
		if (get_respcode () != OK_NOTEXT) {
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
open_art_header (
	long art)
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
			default:
				break;
			}
			if (atoi (buf) == OK_NOTEXT) {
				ptr = buf;
				while (isspace((unsigned char)*ptr)) {
					ptr++;
				}
				while (isdigit((unsigned char)*ptr)) {
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
			default:
				break;
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
open_art_fp (
	char *group_path,
	long art)
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
		if (fp!=old_fp)
		  note_size=0;
		return fp;
	}
}

#if 0
static FILE *
open_xhdr_fp (
	char *header,
	long min,
	long max)
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
setup_hard_base (
	struct t_group *group,
	char *group_path)
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
			default:
				break;
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
			default:
				break;
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
					default:
						break;
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

/*
 * process authinfo generic.
 * 0 means succeeded.
 * 1 means failed
 */

static int
authenticate (void)
{
#ifdef NNTP_ABLE /* former: HAVE_GENERIC_AUTHINFO */
	char tmpbuf[NNTP_STRLEN];
	char authval[NNTP_STRLEN];
	char *authcmd;
	FILE *fp;
	t_bool builtinauth = FALSE;
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
		char tempfile[BUFSIZ];
		
		sprintf (tempfile, "%stin_AXXXXXX", TMPDIR);		
		if (!mktemp(tempfile)) {
			error_message ("Can't create unique tempfile-name","");
			return 1;
		} else {
			fp = fopen (tempfile, "w+");
			if (!fp) {
				error_message ("Can't open %s", tempfile);
				return 1;
			}
		}
		(void) unlink (tempfile);
		cookiefd = fileno (fp);
	}

	strcpy (tmpbuf, "authinfo generic ");
	strcpy (authval, get_val("NNTPAUTH", ""));
	if (strlen(authval)) {
		strcat (tmpbuf, authval);
	} else {
		strcat (tmpbuf, "any ");
		strcat (tmpbuf, userid);
		builtinauth = TRUE;
	}
	put_server (tmpbuf);

#ifdef HAVE_PUTENV
	sprintf (tmpbuf, "NNTP_AUTH_FDS=%d.%d.%d",
		fileno(nntp_rd_fp), fileno(nntp_wr_fp), cookiefd);
	new_env = my_malloc (strlen (tmpbuf)+1);
	strcpy (new_env, tmpbuf);
 	putenv (new_env);
	FreeIfNeeded(old_env);
	old_env = new_env;
#else
	sprintf (tmpbuf, "%d.%d.%d",
		fileno(nntp_rd_fp), fileno(nntp_wr_fp), cookiefd);
	setenv ("NNTP_AUTH_FDS", tmpbuf, 1);
#endif

	if (!builtinauth) {
		return (system (authval));
	} else {
		get_server (tmpbuf, sizeof(tmpbuf));
		return (atoi (tmpbuf) != OK_AUTH);	/* 0 = okay */
	}
#else
	return 1;	/* authentication "failed" */
#endif	/* NNTP_ABLE; former: HAVE_GENERIC_AUTHINFO */
}

/*
 *  Get a response code from the server and return it to the caller
 *  Also does AUTHINFO user/pass or GENERIC authorization
 */

int
get_respcode (void)
{
#ifdef NNTP_ABLE
	char line[NNTP_STRLEN];
	char savebuf[NNTP_STRLEN];
	int respcode;

	switch (get_server (line, NNTP_STRLEN)) {
		case -1:
			error_message (txt_connection_to_server_broken, "");
			tin_done (EXIT_NNTP_ERROR);
		case -2:
			tin_done (0);
		default:
			break;
	}

	debug_nntp ("get_respcode", line);

	respcode = atoi (line);

	if ((respcode == ERR_NOAUTH) || (respcode == NEED_AUTHINFO)) {
		/*
		 * Server requires authentication.
		 */
		strcpy (savebuf, last_put);
		/*
		 * First, try generic authentication; if this fails, try
		 * AUTHINFO USER/PASS.
		 */
		if (authenticate () && !authorization (nntp_server, userid)) {
			sprintf (line, txt_auth_failed, ERR_ACCESS);
		} else {
			strcpy (last_put, savebuf);
			put_server (last_put);
			get_server (line, NNTP_STRLEN);
		}
		respcode = atoi (line);
	}
	return respcode;
#else
	return 0;
#endif
}


int
stuff_nntp (
	char *fnam)
{
#ifdef NNTP_ABLE
	char line[HEADER_LEN];
	FILE *fp;
#ifdef SLOW_OPEN_GROUP
	int count = 0;
#endif
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
		default:
			break;
		}


#ifdef DEBUG
		debug_nntp ("stuff_nntp", line);
#endif
		if (!last_line_nolf && STRCMPEQ(line, ".")) {	/* end of text */
			break;
		}
		if (!get_server_nolf) {
			strcat (line, "\n");
			if (line[0] == '.') {	/* reduce leading .'s */
				fputs (&line[1], fp);
			} else {
				fputs (line, fp);
			}
		} else {
			fputs (line, fp);
		}
#ifdef SLOW_OPEN_GROUP
		if (++count % 50 == 0)
			spin_cursor ();
#endif
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
nntp_to_fp (void)
{
#ifdef NNTP_ABLE
	char fnam[PATH_LEN];
	FILE *fp;
	FILE *old_fp;

	if (!stuff_nntp (fnam)) {
		debug_nntp ("nntp_to_fp", "!stuff_nntp()");
		return (FILE *) 0;
	}

	if ((fp = rfc1521_decode(old_fp=fopen (fnam, "r"))) == (FILE *) 0) {
		perror_message (txt_nntp_to_fp_cannot_reopen, fnam);
		return (FILE *) 0;
	}
	if (old_fp!=fp)
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

#ifdef NNTP_ABLE
/*
 * NNTP user authorization. Password read from ~/.newsauth
 * The ~/.newsauth authorization file has the format:
 *   nntpserver1 password [user]
 *   nntpserver2 password [user]
 *   etc.
 */

static int
authorization (
	char *server,
	char *authuser)
{
	char line[PATH_LEN];
	char line2[PATH_LEN];
	char authusername[PATH_LEN];
	char authpassword[PATH_LEN];
	char *authpass;
	char *ptr;
	FILE *fp;
	int ret;

	debug_nntp ("authorization", "authinfo");

	joinpath (line, homedir, ".newsauth");
	authpass = (char *) 0;
	
	if ((fp = fopen (line,"r")) == (FILE *) 0) {
		/*
		 * if no .newsauth-file given, fall back on console input
		 */
		clear_message ();
		if ((ptr = getline (txt_auth_user_needed, FALSE, authuser, PATH_LEN, FALSE)) == (char *) 0)
			return FALSE;
		strcpy (authusername, ptr);
		authuser = &authusername[0];
		clear_message ();
/*
** we should use getpass here
*/
		if ((ptr = getline (txt_auth_pass_needed, FALSE, (char *) 0, 0, TRUE)) == (char *) 0)
			return FALSE;
		strcpy (authpassword, ptr);
		authpass = &authpassword[0];
	} else {
		/*
		 * Search through authorization file for correct NNTP server
		 * File has format:  'nntp-server' 'password'
		 * will return authpass != NULL if any match
		 */
		while (fgets (line, PATH_LEN, fp) != (char *) 0) {

			/*
			 * strip trailing newline character
			 */

			ptr = strchr (line, '\n');
			if (ptr != (char *) 0)
				*ptr = '\0';

			/*
			 * Get server from 1st part of the line
			 */

			ptr = strchr (line, ' ');

			if (ptr == (char *) 0)		/* no passwd, no auth, skip */
				continue;

			*ptr++ = '\0'; 			/* cut of server part */

			if ((strcasecmp(line, server)))
				continue;		/* wrong server, keep on */

			/*
			 * Get password from 2nd part of the line
			 */

			authpass = ptr;
			while (*authpass == ' ')
				authpass++;		/* skip any blanks */

			/*
			 * Get user from 3rd part of the line
			 */

			ptr = authpass;			/* continue searching here */

			if (*authpass == '"') {		/* skip "embedded" password string */
				ptr = strrchr (authpass,'"');
				if ((ptr != (char *) 0) && (ptr > authpass)) {
					authpass++;
					*ptr++ = '\0';	/* cut off trailing " */
				} else			/* no matching ", proceede as normal */
					ptr = authpass;
			}

			ptr = strchr (ptr,' ');		/* find next separating blank */

			if (ptr != (char *) 0) {		/* a 3rd argument follows */
				while(*ptr == ' ')	/* skip any blanks */
					*ptr++ = '\0';
				if (*ptr != '\0')	/* if its not just empty */
					authuser = ptr;	/* so will replace default user */
			}

			break;	/* if we end up here, everything seems OK */
		}
		fclose (fp);
	}
	
	if (authpass == (char *) 0) {
		error_message (txt_nntp_authorization_failed, server);
		return FALSE;
	}

	sprintf (line2, "authinfo user %s", authuser);
	put_server (line2);
	get_server (line2, PATH_LEN);
	ret = atoi (line2);
	if (ret != NEED_AUTHDATA) {
		if (ret == OK_AUTH) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	sprintf (line2, "authinfo pass %s", authpass);
	put_server (line2);
	get_server (line, PATH_LEN);
	ret = atoi (line);
	if (ret != OK_AUTH) {
		return FALSE;
	}

	sprintf (line, txt_authorization_ok, authuser);
	wait_message (line);
	return TRUE;
}
#endif /* NNTP_ABLE */

void
vGrpGetSubArtInfo (void)
{
#ifndef INDEX_DAEMON
	int	iNum;
	long	lMinOld;
	long	lMaxOld;
	struct	t_group *psGrp;

	if (SHOW_UPDATE)
		wait_message (txt_rereading_active_file);

	for (iNum = 0 ; iNum < num_active ; iNum++) {
		psGrp = &active[iNum];
		if (psGrp->subscribed) {
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
				expand_bitmap(psGrp, psGrp->xmin);
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
 *  Use nntp GROUP command or read local spool
 *  Return 0, or -1 on error
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
	long	lArtMin;
	long	lArtMax;
	long	lArtNum;

	lArtMin = *plArtMin;
	lArtMax = *plArtMax;

	if (read_news_via_nntp && iGrpType == GROUP_TYPE_NEWS) {
#ifdef NNTP_ABLE
		char	acLine[NNTP_STRLEN];
		long	lDummy;

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

		debug_nntp ("vGrpGetArtInfo", acLine);

		switch (atoi(acLine)) {

			case OK_GROUP:
				if (sscanf (acLine, "%ld %ld %ld %ld", &lDummy, plArtCount, plArtMin, plArtMax) != 4)
					error_message("Trashed response to GROUP command, %s", acLine);
				break;

			case ERR_NOGROUP:
				*plArtCount = 0;
				*plArtMin = 1;
				*plArtMax = 0;
				return(-ERR_NOGROUP);

			case ERR_ACCESS:
				error_message ("\r\n%s", acLine);
				tin_done (EXIT_NNTP_ERROR);

			default:
				debug_nntp ("NOT_OK", acLine);
				return(-1);
		}
#else
		fprintf(stderr, "Unreachable ?\n");
		return(0);
#endif	/* #ifdef NNTP_ABLE */
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
#if 0
		if (access (acBuf, R_OK) != 0)
			return(-1);
#endif

		if ((tDirFile = opendir (acBuf)) != (DIR *) 0) {
			while ((tFile = readdir (tDirFile)) != (DIR_BUF *) 0) {
				lArtNum = atol (tFile->d_name/*, (int) D_NAMLEN(tFile)*/); /* should be '\0' terminated... */
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
#endif	/* #ifdef M_AMIGA */
	}

	return(0);
}
