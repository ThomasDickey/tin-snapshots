/*
 *  Project   : tin - a Usenet reader
 *  Module    : active.c
 *  Author    : I.Lea
 *  Created   : 16-02-92
 *  Updated   : 20-08-95
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"menukeys.h"

char new_newnews_host[PATH_LEN];
int reread_active_file = FALSE;
int newnews_index = -1;
time_t new_newnews_time;			/* FIXME: never set */

/*
** Local prototypes
*/
static int find_newnews_index P_((char *cur_newnews_host));
static int match_group_list P_((char *group, char *group_list));
static int parse_newsrc_active_line P_((char *group, long *count, long *max, long *min, char *moderated));
static void check_for_any_new_groups P_((void));
static void prompt_subscribe_group P_((char *group, char *autosubscribe, char *autounsubscribe));

#if 0
static int cmp_group_p P_((t_comptype *group1, t_comptype *group2));
static int cmp_notify_p P_((t_comptype *notify1, t_comptype *notify2));
static void backup_active P_((int create));
#endif /* 0 */


#if 0 /* never used */
/*
 *  Compare two pointers to "group_t" structures - used in qsort.
 */

int
cmp_group_p (group1, group2)
	t_comptype *group1;
	t_comptype *group2;
{
	return (strcmp ((*(group_p *) group1)->name, (*(group_p *) group2)->name));
}

/*
 *  Compare two pointers to "notify_t" structures - used in qsort.
 */

int
cmp_notify_p (notify1, notify2)
	t_comptype *notify1;
	t_comptype *notify2;
{
	return (strcmp ((*(notify_p *) notify1)->name, (*(notify_p *) notify2)->name));
}
#endif /* 0 */

/*
 *  Get default array size for active[] from environment (AmigaDOS)
 *  or just return the standard default.
 */

int
get_active_num ()
{
#ifdef ENV_VAR_GROUPS
	char *ptr;
	int num;

	ptr = getenv (ENV_VAR_GROUPS);
	if (ptr != (char *) 0) {
		num = atoi (ptr);
		return (num ? num : DEFAULT_ACTIVE_NUM);
	}
#endif
	return DEFAULT_ACTIVE_NUM;
}

/*
 *  Resync active file when SIGALRM signal received that
 *  is triggered by alarm (reread_active_file_secs) call.
 */

int
resync_active_file ()
{
	char old_group[HEADER_LEN];
	int reread = FALSE;
	int command_line;

	if (reread_active_file) {
		reread_active_for_posted_arts = FALSE;
		reread = TRUE;

		if (cur_groupnum >= 0 && group_top) {
			strcpy (old_group, CURR_GROUP.name);
		} else {
			old_group[0] = '\0';
		}
		vWriteNewsrc ();
		vGrpGetSubArtInfo ();
#if 0
		free_active_arrays ();
		max_active = get_active_num ();
		expand_active ();

#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
		read_mail_active_file ();
#endif
		read_news_active_file ();
		read_attributes_file ();
#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
		read_mailgroups_file ();
#endif
		read_newsgroups_file ();
#endif /* #if 0 */
		command_line = read_cmd_line_groups ();
		read_newsrc (newsrc, command_line ? 0 : 1);
		if (!command_line) {
			toggle_my_groups (show_only_unread_groups, old_group);
		}
		set_groupname_len (FALSE);
		set_alarm_signal ();
		group_selection_page ();
	}

	return reread;
}

/* List of allowed seperator chars in active file */
#define ACTIVE_SEP				" \n"

/*
 * Parse line from news or mail active files
 */
int
parse_active_line (line, max, min, moderated)
	char *line;
	long *max;
	long *min;
	char *moderated;
{
	char *p, *q, *r;

	if (line[0] == '#' || line[0] == '\n')
		return(FALSE);

	strtok(line, ACTIVE_SEP);		/* skip group name */
	p = strtok((char *)0, ACTIVE_SEP);	/* group max count */
	q = strtok((char *)0, ACTIVE_SEP);	/* group min count */
	r = strtok((char *)0, ACTIVE_SEP);	/* mod status or path to mailgroup */

	if (!p || !q || !r) {
		error_message (txt_bad_active_file, line);
		return(FALSE);
	}

	*max = atol (p);
	*min = atol (q);
	strcpy(moderated, r);

	/* TODO - if local, test if spooldir exists so we bounce bad groups ? */
	/* Currently done in vGGAI() */
	return(TRUE);
}


/*
 * Parse a line from the .newsrc file
 * Returns TRUE or FALSE accordingly
 * Use vGrpGetArtInfo() to obtain min/max/count for the group
 * We can't know the 'moderator' status and always return 'n'
 */
static int
parse_newsrc_active_line (buf, count, max, min, moderated)
	char *buf;
	long *count;
	long *max;
	long *min;
	char *moderated;
{
	char	*ptr;

	ptr = tin_strpbrk (buf, ":!");

	if (!ptr || *ptr != ':')		/* Invalid line or unsubscribed */
		return(FALSE);

	*ptr = '\0';					/* Now buf is the group name */

	if (vGrpGetArtInfo (spooldir, buf, GROUP_TYPE_NEWS, count, max, min) != 0)
		return(FALSE);

#if 0
fprintf(stderr, "vGGAI OUT, cnt=%ld, min=%ld, max=%ld\n", *count, *max, *min);
#endif

	strcpy (moderated, "n");

	return(TRUE);
}

/*
 *  Load the news active file into active[]
 *  [ No longer done: used to create copy of active ~/.tin/active ]
 */

void
read_news_active_file ()
{
	FILE *fp = 0;
	char buf[HEADER_LEN];
	char moderated[PATH_LEN];
	long count = -1L, min = 1, max = 0;

	if (newsrc_active && ((fp = fopen (newsrc, "r")) == (FILE *) 0))
		newsrc_active = FALSE;

	if (SHOW_UPDATE) {
		wait_message (newsrc_active ? txt_reading_news_newsrc_file :
						txt_reading_news_active_file);
	}

	if (!newsrc_active) {
		if ((fp = open_news_active_fp ()) == (FILE *) 0) {

			if (cmd_line)
				my_fputc ('\n', stderr);

			if (compiled_with_nntp) {
				sprintf (msg, txt_cannot_open_active_file, news_active_file, progname);
				error_message (msg, "");
			} else {
				error_message (txt_cannot_open, news_active_file);
			}

			tin_done (EXIT_ERROR);
		}
	}

	strcpy (moderated, "y");			/* Assume moderated as default */

	forever {
		if (fgets (buf, sizeof(buf), fp) == (char *)0)
			break;

		if (newsrc_active) {
			if (!parse_newsrc_active_line (buf, &count, &max, &min, moderated))
				continue;
		} else {
			if (!parse_active_line (buf, &max, &min, moderated))
				continue;
		}

		/*
		 * Don't load group into active[] from active file if
		 * 'x'  junked group
		 * '='  aliased group
		 */
		if (moderated[0] == 'x' || moderated[0] == '=')
			continue;

		/*
		 * Load group into group hash table
		 * Error => duplicate group
		 */
		if (psGrpAdd(buf) != 0)
			goto read_news_active_continue;

		/*
		 * Load group info.
		 */
#ifdef WIN32
		/*
		 * Paths are in form - x:\a\b\c
		 */
		if (strchr(moderated, '\\')) {
#else
		if (moderated[0] == '/') {
#endif
			active[num_active].type = GROUP_TYPE_SAVE;
			active[num_active].spooldir = my_strdup(moderated);
		} else {
			active[num_active].type = GROUP_TYPE_NEWS;
			active[num_active].spooldir = spooldir;
		}
		active[num_active].name = my_strdup (buf);
		active[num_active].description = (char *) 0;
		active[num_active].count = count;
		active[num_active].xmax = max;
		active[num_active].xmin = min;
		active[num_active].moderated = moderated[0];
		active[num_active].next = -1;			/* hash chaining */
		active[num_active].inrange = FALSE;
		active[num_active].read_during_session = FALSE;
		active[num_active].art_was_posted = FALSE;
		active[num_active].subscribed = UNSUBSCRIBED;	/* not in my_group[] yet */
		active[num_active].newsrc.xbitmap = (t_bitmap *) 0;
		active[num_active].attribute = (struct t_attribute *) 0;
		active[num_active].glob_filter = &glob_filter;
		active[num_active].grps_filter = (struct t_filters *) 0;
		vSetDefaultBitmap (&active[num_active]);
#ifdef INDEX_DAEMON
		active[num_active].last_updated_time = (time_t) 0;
#endif
		if (num_active % 100 == 0 && !update)
			spin_cursor ();

		num_active++;
read_news_active_continue:;
	}
	fclose (fp);

	/*
	 *  exit if active file is empty
	 */
	if (!num_active) {
		error_message (txt_active_file_is_empty, news_active_file);
		tin_done (EXIT_ERROR);
	}

	if ((cmd_line && !(update || verbose)) || (update && update_fork)) {
		wait_message ("\n");
	}

	check_for_any_new_groups ();
}

#if 0
/*
 *  create ~/.tin/active if it does not exist (local news only)
 */
void
backup_active (create)
	int create;
{
	char buf[PATH_LEN];
	FILE *fp;
	register int i;
	struct stat sb;

	joinpath (buf, rcdir, "active");

	if (create) {
		if (stat (buf, &sb) != -1) {
			return;
		}
	}

#ifdef VMS
	if ((fp = fopen (buf, "w", "fop=cif")) != (FILE *) 0) {
#else
	if ((fp = fopen (buf, "w")) != (FILE *) 0) {
#endif
		for (i = 0; i < num_active ; i++) {	/* for each group */
			fprintf (fp, "%s\n", active[i].name);
		}
		fclose (fp);
		chmod (buf, (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH));
	}
}
#endif /* 0 */

/*
 * Check for any newly created newsgroups.
 *
 * If reading news locally check the NEWSLIBDIR/active.times file. Format:
 *   Groupname Seconds Creator
 *
 * If reading news via NNTP issue a NEWGROUPS command.
 */

static void
check_for_any_new_groups ()
{
	char *autosubscribe, *autounsubscribe;
	char *ptr, buf[NNTP_STRLEN];
	char old_newnews_host[PATH_LEN];
	FILE *fp = (FILE *) 0;
	time_t the_newnews_time = (time_t) 0;
	time_t old_newnews_time;
	time_t creation_time = (time_t) 0;

	if ((!check_for_new_newsgroups || (update && !update_fork))) {
		return;
	}

	wait_message (txt_checking_active_file);

	time (&the_newnews_time);

	/*
	 * find out if we have read news from here before otherwise -1
	 */
	if (read_news_via_nntp) {
		strcpy (new_newnews_host, nntp_server);
	} else {
		strcpy (new_newnews_host, "local");
	}
	newnews_index = find_newnews_index (new_newnews_host);

	if (newnews_index >= 0) {
		strcpy (old_newnews_host, newnews[newnews_index].host);
		old_newnews_time = newnews[newnews_index].time;
	} else {
		strcpy (old_newnews_host, "UNKNOWN");
		old_newnews_time = (time_t) 0;
	}

/*
	if (!read_news_via_nntp && newnews_index >= 0) {
		new_active_size = the_newnews_time;
		old_active_size = new_newnews_size[active_index].attribute;
		if (the_newnews_time <= old_active_size) {
			goto notify_groups_done;
		}
	}
*/

	if (debug == 2) {
		sprintf (msg, "Newnews old=[%ld]  new=[%ld]",
			old_newnews_time, the_newnews_time);
		error_message (msg, "");
		sleep (2);
	}


	if ((fp = open_newgroups_fp (newnews_index)) == (FILE *) 0) {
		goto notify_groups_done;
	}

	/*
	 * Check if there are user-set groups to be
	 * automatically subscribed or unsubscribed.
	 */
	autosubscribe = getenv ("AUTOSUBSCRIBE");
	autounsubscribe = getenv ("AUTOUNSUBSCRIBE");

	Raw (TRUE);

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		ptr = strchr (buf, '\n');
		if (ptr != (char *) 0) {
			*ptr = '\0';
		}
		if (read_news_via_nntp) {
			if (buf[0] == '.') {
				break;
			} else {
				ptr = strchr (buf, ' ');
				if (ptr != (char *) 0) {
					*ptr = '\0';
				}
			}
		} else {
			ptr = strchr (buf, ' ');
			if (ptr != (char *) 0) {
				creation_time = (time_t) atol (ptr);
				*ptr = '\0';
			}
			if (creation_time < old_newnews_time ||
			    old_newnews_time == (time_t) 0) {
				continue;
			}
		}
		prompt_subscribe_group (buf, autosubscribe, autounsubscribe);
		CleartoEOLN ();
	}

	if (cmd_line) {
		Raw (FALSE);
	}

notify_groups_done:
/*
	if (cmd_line) {
		printf ("\r\n");
		fflush (stdout);
	}
*/
	if (fp != (FILE *) 0) {
		fclose (fp);
	}

	/*
	 * update attribute field/create new entry with new size/date
	 */
	if (newnews_index >= 0) {
		newnews[newnews_index].time = the_newnews_time;
	} else {
		sprintf (buf, "%s %ld", new_newnews_host, the_newnews_time);
		load_newnews_info (buf);
	}
}

/*
 * prompt user if new group should be subscribed to
 */

static void
prompt_subscribe_group (group, autosubscribe, autounsubscribe)
	char *group;
	char *autosubscribe;
	char *autounsubscribe;
{
	int ch, ch_default = iKeyActiveNo;
	int idx;
	static int subscribe_rest = FALSE;
	static int unsubscribe_rest = FALSE;

	/*
	 * First automatically sub/unsub from groups specified in
	 * the env. variables AUTOSUBSCRIBE & AUTOUNSUBSCRIBE
	 */
	if ((autosubscribe != (char *) 0) &&
	    match_group_list (group, autosubscribe)) {
		idx = add_my_group (group, 1);
		if (idx >= 0) {
			subscribe (&active[my_group[idx]], SUBSCRIBED);
		}
		return;
	} else if ((autounsubscribe != (char *) 0) &&
		match_group_list (group, autounsubscribe)) {
		/* ignore this group */
		return;
	}

	/*
	 * If Y/N (capitol Y/N) was pressed then automatically sub/unsub
	 * from the rest of the new groups.
	 */
	if (subscribe_rest) {
		idx = add_my_group (group, 1);
		if (idx >= 0) {
			subscribe (&active[my_group[idx]], SUBSCRIBED);
			if (cmd_line) {
				printf ("\r\n");
				fflush (stdout);
				CleartoEOLN ();
			} else {
				clear_message ();
			}
			printf (txt_subscribing_to, group);
		}
		return;
	} else {
		if (unsubscribe_rest) {
			return; /* ignore this group */
		}
	}

	do {
		if (cmd_line) {
			printf ("\r\n");
			fflush (stdout);
			CleartoEOLN ();
		} else {
			clear_message ();
		}
		printf (txt_subscribe_to_new_group, group, ch_default);
		fflush (stdout);
		ch = ReadCh ();
		if (ch == '\n' || ch == '\r') {
			ch = ch_default;
		}
	} while (!strchr ("NnYy", ch));

	my_fputc (ch, stdout);
	fflush (stdout);

	switch (ch) {
		case iKeyActiveYes:
		case iKeyActiveAll:
			idx = add_my_group (group, 1);
			if (idx >= 0) {
				subscribe (&active[my_group[idx]], SUBSCRIBED);
			}
			if (ch == iKeyActiveAll) {
				subscribe_rest = TRUE;
			}
			break;
		case iKeyActiveNone:
			unsubscribe_rest = TRUE;
			if (cmd_line) {
				printf ("\r\n");
				fflush (stdout);
				CleartoEOLN ();
			} else {
				clear_message ();
			}
			printf (txt_skipping_newsgroups);
			break;
		case iKeyActiveNo:
		default:
			break;
	}
}


static int
match_group_list (group, group_list)
	char *group;
	char *group_list;
{
	char *separator;
	char pattern[HEADER_LEN];
	int accept, negate, list_len;
	size_t group_len;

	accept = FALSE;
	list_len = strlen (group_list);
	/*
	 * walk through comma-separated entries in list
	 */
	while (list_len > 0) {
		/*
		 * find end/length of this entry
		 */
		separator = strchr (group_list, ',');
		if (separator != (char *) 0) {
			group_len = separator-group_list;
		} else {
			group_len = list_len;
		}
		if ((negate = ('!' == *group_list))) {
			/*
			 * a '!' before the pattern inverts sense of match
			 */
			group_list++;
			group_len--;
			list_len--;
		}
		/*
		 * copy out the entry and terminate it properly
		 */
		strncpy (pattern, group_list, group_len);
		pattern[group_len] = (char) 0;
		/*
		 * regexp match
		 */
		if (STR_MATCH(group, pattern)) {
			accept = !negate;	/* matched!*/
		}
		/*
		 * now examine next entry if any
		 */
		if ((char) 0 != group_list[group_len]) {
			group_len++;	/* skip the separator */
		}
		group_list += group_len;
		list_len -= group_len;
	}
	return (accept);
}

/*
 *  Load the last updated time for each group in the active file so that
 *  tind is more efficient and only has to stat the group dir and compare
 *  the last changed time with the time read from the ~/.tin/group.times
 *  file to determine if the group needs updating.
 *
 *  alt.sources 71234589
 *  comp.archives 71234890
 */

void
read_group_times_file ()
{
#ifdef INDEX_DAEMON

	char *p, *q;
	char buf[HEADER_LEN];
	char group[HEADER_LEN];
	FILE *fp;
	time_t updated_time;
	struct t_group *psGrp;

	if ((fp = fopen (group_times_file, "r")) == (FILE *) 0) {
		return;
	}

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		/*
		 * read the group name
		 */
		for (p = buf, q = group ; *p && *p != ' ' && *p != '\t' ; p++, q++) {
			*q = *p;
		}
		*q = '\0';

		/*
		 * read the last updated time
		 */
		updated_time = (time_t) atol (p);

		/*
		 * find the group in active[] and set updated time
		 */
		psGrp = psGrpFind (group);

		if (psGrp) {
			psGrp->last_updated_time = updated_time;
		}

if (debug == 2) {
	printf ("group=[%-40.40s]  [%ld]\n", psGrp->name, psGrp->last_updated_time);
}
	}
	fclose (fp);

#endif	/* INDEX_DAEMON */
}

/*
 *  Save the last updated time for each group to ~/.tin/group.times
 */

void
write_group_times_file ()
{
#ifdef INDEX_DAEMON

	char buf[HEADER_LEN];
	char group[HEADER_LEN];
	FILE *fp;
	register int i;

	if ((fp = fopen (group_times_file, "w")) == (FILE *) 0) {
		return;
	}

	for (i = 0 ; i < num_active ; i++) {
		fprintf (fp, "%s %ld\n", active[i].name, active[i].last_updated_time);
	}
	fclose (fp);

#endif	/* INDEX_DAEMON */
}


void
load_newnews_info (info)
	char *info;
{
	char *ptr;
	char buf[NNTP_STRLEN];
	int i;
	time_t the_time;

	/*
	 * initialize newnews[] if no entries
	 */
	if (!num_newnews) {
		for (i = 0 ; i < max_newnews ; i++) {
			newnews[i].host = (char *) 0;
			newnews[i].time = (time_t) 0;
		}
	}

	my_strncpy (buf, info, sizeof (buf));

	ptr = strchr (buf, ' ');
	if (ptr != (char *) 0) {
		the_time = (time_t) atol (ptr);
		*ptr = '\0';
		if (num_newnews >= max_newnews) {
			expand_newnews ();
		}
		newnews[num_newnews].host = my_strdup (buf);
		newnews[num_newnews].time = the_time;
		if (debug == 2) {
			sprintf (buf, "ACTIVE host=[%s] time=[%ld]",
				newnews[num_newnews].host,
				newnews[num_newnews].time);
			error_message ("%s", buf);
		}
		num_newnews++;
	}
}


static int
find_newnews_index (cur_newnews_host)
	char *cur_newnews_host;
{
	int i, found = FALSE;

	for (i = 0 ; i < num_newnews ; i++) {
		if (STRCMPEQ(cur_newnews_host, newnews[i].host)) {
			found = TRUE;
			break;
		}
  	}

	return (found ? i : -1);
}

/*
 *  check for message of the day (motd) file
 *
 * If reading news locally stat() the active file to get its
 * mtime otherwise do a XMOTD command to the NNTP server
 */

void
read_motd_file ()
{
#ifndef INDEX_DAEMON

	char buf[NNTP_STRLEN];
	char motd_file_date[32];
	FILE *fp;
	int lineno = 0;
	time_t new_motd_date = (time_t) 0;
	time_t old_motd_date = (time_t) 0;
	struct stat sb;
	struct tm *tm;

	if (update && !update_fork) {
		return;
	}

	old_motd_date = (time_t) atol (motd_file_info);

	/*
	 * reading news locally (local) or via NNTP (server name)
	 */
	if (read_news_via_nntp) {
		if (!old_motd_date) {
			strcpy (motd_file_date, "920101 000000");
		} else {
			tm = localtime (&old_motd_date);
			sprintf (motd_file_date, "%02d%02d%02d %02d%02d%02d",
				tm->tm_year, tm->tm_mon+1, tm->tm_mday,
				tm->tm_hour, tm->tm_min, tm->tm_sec);
		}
		time (&new_motd_date);
	} else {
		if (stat (motd_file, &sb) >=0) {
			new_motd_date = sb.st_mtime;
		}
	}

	if (old_motd_date && new_motd_date <= old_motd_date) {
		goto read_motd_done;
	}

	if ((fp = open_motd_fp (motd_file_date)) != (FILE *) 0) {
		while (fgets (buf, sizeof (buf), fp) != 0) {
			if (buf[0] == '.') {
				break;
			}
			printf ("%s", buf);
			lineno++;
		}
		fclose (fp);

		if (lineno) {
			wait_message (txt_cmdline_hit_any_key);
			Raw (TRUE);
			ReadCh ();
			Raw (FALSE);
			wait_message ("\n");
		}
	}

read_motd_done:

	/*
	 * update motd tinrc entry with new date
	 */
	sprintf (motd_file_info, "%ld", new_motd_date);

#endif	/* INDEX_DAEMON */
}

void
vMakeActiveMyGroup ()
{
	register int iNum;

	group_top = 0;

	for (iNum = 0; iNum < num_active; iNum++) {
		my_group[group_top++] = iNum;
	}
}
