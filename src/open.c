/*
 *  Project   : tin - a Usenet reader
 *  Module    : open.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 21-12-94, 05-04-97
 *  Notes     : Routines to make reading news locally (ie. /usr/spool/news)
 *              or via NNTP transparent
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"

#ifdef NNTP_ABLE
static FILE *nntp_to_fp (void);
static FILE *stuff_nntp (char *fnam);
#endif

long head_next;

/* error message from server */
char error_response[NNTP_STRLEN];

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

		/* do this only once at start-up */
		if (nntp_server == (char *) 0) {
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

		/*
		 * Try to authenticate
		 */
		debug_nntp ("nntp_open", "authenticate");
		authenticate (nntp_server, userid, TRUE);

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
 *  Open mail groups description file.
 */
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
my_printf ("Artnum=[%ld] Items=[%d]\n", art, items);
my_flush ();
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
	FILE *fp, *old_fp;
#ifdef NNTP_ABLE
	int respcode;
#endif

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

		old_fp = nntp_to_fp ();
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
		old_fp=fopen (buf, "r");
	}

	fp=rfc1521_decode(old_fp);
	if (fp!=old_fp)
	  note_size=0;
	return fp;
}

/* This could well come in useful for filtering on non-overview hdr fields */
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
#ifdef NNTP_ABLE
	char line[NNTP_STRLEN];
	long start, last, dummy, count;
#endif
	long art;
	long total = 0;

	top_base = 0;

	/*
	 * If reading with NNTP, issue a LISTGROUP
	 */
	if (read_news_via_nntp && group->type == GROUP_TYPE_NEWS) {
#ifdef NNTP_ABLE

#if 0
/* leafnode and nntpcache (old versions only?) need an extra GROUP
 * before LISTGROUP works propperly - this is a bug in leafnode/nntpcache
 * NOT in tin.
 */
		sprintf (buf, "group %s", group->name);
		put_server (buf);
#endif /* 0*/

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

		/*
		 * LISTGROUP worked
		 */
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
				if (STRCMPEQ(line, ".")) {				/* end of text */
					debug_nntp ("setup_base", line);
					break;
				}
				if (top_base >= max_art) {
					expand_art ();
				}
				base[top_base++] = atoi (line);
			}
		/*
		 * LISTGROUP failed, try a GROUP command instead
		 */
		} else {
			debug_nntp ("setup_base, listgroup", "NOT_OK");
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
				return (-1);
			}

			debug_nntp ("setup_base", line);

			sscanf (line,"%ld %ld %ld %ld", &dummy, &count, &start, &last);

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
			error_message(txt_not_exist, "");
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

/*
 *  Get a response code from the server and return it to the caller.
 *  Also does authentication if requested and repeats the last command.
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
		debug_nntp ("get_respcode", "authentication");
		strcpy (savebuf, last_put);
		if (!authenticate (nntp_server, userid, FALSE)) {
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

#ifdef NNTP_ABLE
/*
 * Copies data from the nntp socket and writes it to a temp file
 * returns open descriptor to this file and writes in 'fnam'
 * the filename we used
 */
static FILE *
stuff_nntp (
	char *fnam)
{
	char line[HEADER_LEN];
	FILE *fp;
	struct stat sb;
#ifdef SHOW_PROGRESS
	int count = 0;
#endif

	sprintf (fnam, "%stin_nntpXXXXXX", TMPDIR);
	mktemp (fnam);

	if ((fp = fopen (fnam, "w")) == (FILE *) 0) {
		perror_message (txt_stuff_nntp_cannot_open, fnam);
		return ((FILE *) 0);
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
		if (STRCMPEQ(line, "."))		/* end of text */
			break;

		if (!get_server_nolf) {
			strcat (line, "\n");

			if (line[0] == '.')					/* reduce leading .'s */
				fputs (&line[1], fp);
			else
				fputs (line, fp);

		} else {
			fputs (line, fp);
		}
#ifdef SHOW_PROGRESS
		if (++count % 50 == 0)
			spin_cursor ();
#endif
	}

	fclose(fp);

	if (stat (fnam, &sb) < 0)
		note_size = 0;
	else
		note_size = sb.st_size;

	if ((fp = fopen (fnam, "r")) == (FILE *) 0) {	/* Reopen for writing. */
		perror_message (txt_nntp_to_fp_cannot_reopen, fnam);
		return (FILE *) 0;
	}

	return (fp);
}
#endif


#ifdef NNTP_ABLE
static FILE *
nntp_to_fp (void)
{
	char fnam[PATH_LEN];
	FILE *fp;

	if ((fp = stuff_nntp (fnam)) == (FILE *) 0) {
		debug_nntp ("nntp_to_fp", "!stuff_nntp()");
		return (FILE *) 0;
	}

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
}
#endif	/* NNTP_ABLE */

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
				my_printf (cCRLF "Unread WRONG [%d]=%s unread=[%ld] count=[%ld]",
					iNum, psGrp->name, psGrp->newsrc.num_unread, psGrp->count);
				my_flush ();
#endif
				psGrp->newsrc.num_unread = psGrp->count;
			}
			if (psGrp->xmin != lMinOld || psGrp->xmax != lMaxOld) {
				expand_bitmap(psGrp, psGrp->xmin);
#ifdef DEBUG
	my_printf (cCRLF "Min/Max DIFF [%d]=%s old=[%ld-%ld] new=[%ld-%ld]",
		iNum, psGrp->name, lMinOld, lMaxOld, psGrp->xmin, psGrp->xmax);
	my_flush ();
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
				error_message (cCRLF "%s", acLine);
				tin_done (EXIT_NNTP_ERROR);

			default:
				debug_nntp ("NOT_OK", acLine);
				return(-1);
		}
#else
		my_fprintf(stderr, "Unreachable ?\n");
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
/*		  unless there is some subtle permission check if tin is suid news? */
#if 0
		if (access (acBuf, R_OK) != 0)
			return(-1);
#endif

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
#endif	/* #ifdef M_AMIGA */
	}

	return(0);
}
