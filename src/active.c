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
#include	"tcurses.h"
#include	"menukeys.h"

char new_newnews_host[PATH_LEN];
int reread_active_file = FALSE;
time_t new_newnews_time;			/* FIXME: never set */

/*
 * Local prototypes
 */
static int find_newnews_index (char *cur_newnews_host);
static int parse_newsrc_active_line (char *buf, long *count, long *max, long *min, char *moderated);
static void check_for_any_new_groups (void);
static void subscribe_new_group (char *group, char *autosubscribe, char *autounsubscribe);
static void active_add (struct t_group *ptr, long count, long max, long min, const char *moderated);

/*
 *  Get default array size for active[] from environment (AmigaDOS)
 *  or just return the standard default.
 */

int
get_active_num (void)
{
#ifdef ENV_VAR_GROUPS
	char *ptr;
	int num;

	if ((ptr = getenv (ENV_VAR_GROUPS)) != (char *) 0)
		return ((num = atoi (ptr)) ? num : DEFAULT_ACTIVE_NUM);
#endif
	return DEFAULT_ACTIVE_NUM;
}

/*
 *  Resync active file when SIGALRM signal received that
 *  is triggered by alarm (reread_active_file_secs) call.
 */

int
resync_active_file (void)
{
	char old_group[HEADER_LEN];
	int reread = FALSE;
	int command_line;

	if (reread_active_file) {
		reread_active_for_posted_arts = FALSE;
		reread = TRUE;

		if (cur_groupnum >= 0 && group_top)
			strcpy (old_group, CURR_GROUP.name);
		else
			old_group[0] = '\0';

		vWriteNewsrc ();
#if 0
		/* 1.3beta behaviour, check the on-spool counts 
		 * it's slow but it works!
		 */
		vGrpGetSubArtInfo ();
#else
		/* original 1.2 behaviour, reload the active[] array and all its
		 * dependants.
		 * it's much faster, but sometimes articles aren't displayed!
		 */
		free_active_arrays ();
		max_active = get_active_num ();
		expand_active ();

		init_group_hash();
#	if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
		read_mail_active_file ();
#	endif
		group_top = 0;
		read_news_active_file ();

		read_attributes_file (local_attributes_file, FALSE);

#	if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
		read_mailgroups_file ();
#	endif
		read_newsgroups_file ();
		/* end of former 1.2 behaviour */
#endif

		command_line = read_cmd_line_groups ();

		read_newsrc (newsrc, command_line ? 0 : 1);
		if (command_line)		/* Can't show only unread groups with cmd line groups */
			show_only_unread_groups = FALSE;
		else
			toggle_my_groups (show_only_unread_groups, old_group);

		set_groupname_len (FALSE);
		set_alarm_signal ();
		show_selection_page ();
	}

	return reread;
}

/* List of allowed seperator chars in active file */
#define ACTIVE_SEP				" \n"

/*
 * Populate a slot in the active[] array
 * TODO: 1) Have a preinitialised default slot and block assign it for speed
 * TODO: 2) Lump count/max/min/moderat into a t_active, big patch but much cleaner throughout tin
 */
static void
active_add(
	struct t_group *ptr,
	long count,
	long max,
	long min,
	const char *moderated)
{
	/* name - pre-initialised when group is made */
	ptr->description = (char *) 0;
	/* spool - see below */
	ptr->moderated = moderated[0];
	if (moderated[0] == '=')
		wait_message(3, "\nNote: Any posts to %s should be sent to %s\n", ptr->name, moderated+1);

	ptr->count = count;
	ptr->xmax = max;
	ptr->xmin = min;
	/* type - see below */
	ptr->inrange = FALSE;
	ptr->read_during_session = FALSE;
	ptr->art_was_posted = FALSE;
	ptr->subscribed = FALSE;			/* not in my_group[] yet */
	ptr->newgroup = FALSE;
	ptr->bogus = FALSE;
	ptr->next = -1;						/* hash chain */
	ptr->newsrc.xbitmap = (t_bitmap *) 0;
	ptr->attribute = (struct t_attribute *) 0;
	ptr->glob_filter = &glob_filter;
	vSetDefaultBitmap (ptr);
#ifdef INDEX_DAEMON
	ptr->last_updated_time = (time_t) 0;
#endif

#ifdef WIN32				/* Paths are in form - x:\a\b\c */
	if (strchr(moderated, '\\'))
#else
	if (moderated[0] == '/')
#endif
	{
		ptr->type = GROUP_TYPE_SAVE;		/* What is this ? */
		ptr->spooldir = my_strdup(moderated);
	} else {
		ptr->type = GROUP_TYPE_NEWS;
		ptr->spooldir = spooldir;		/* another global - sigh */
	}
}

/*
 * Decide how to handle a bogus groupname.
 * If we process them interactively, create an empty active[] for this
 * group and mark it bogus for display in the group selection page
 * Otherwise, bogus groups are dealt with when newsrc is written.
 */
int
process_bogus(
	char *name) /* return value is always ignored */
{
	struct t_group *ptr;

	if (strip_bogus != BOGUS_ASK)
		return(0);

	if ((ptr = psGrpAdd(name)) == NULL)
		return(0);

	active_add(ptr, 0L, 1L, 0L, "n");
	ptr->bogus = TRUE;						/* Mark it bogus */

	if (my_group_add(name) < 0)
		return(1);							/* Return code is ignored */

	return(0);								/* Nothing was printed yet */
}

/*
 * Parse line from news or mail active files
 */
int
parse_active_line (
	char *line,
	long *max,
	long *min,
	char *moderated)
{
	char *p, *q, *r;

	if (line[0] == '#' || line[0] == '\0')
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

	return(TRUE);
}


/*
 * Parse a line from the .newsrc file
 * Returns TRUE or FALSE accordingly
 * Use vGrpGetArtInfo() to obtain min/max/count for the group
 * We can't know the 'moderator' status and always return 'y'
 */
static int
parse_newsrc_active_line (
	char *buf,
	long *count,
	long *max,
	long *min,
	char *moderated)
{
	char	*ptr;

	ptr = strpbrk (buf, ":!");

	if (!ptr || *ptr != SUBSCRIBED)		/* Invalid line or unsubscribed */
		return(FALSE);

	*ptr = '\0';					/* Now buf is the group name */

	if (vGrpGetArtInfo (spooldir, buf, GROUP_TYPE_NEWS, count, max, min) != 0)
		return(FALSE);

	strcpy (moderated, "y");

	return(TRUE);
}

/*
 * Load the active file into active[]
 * Check and preload any new newgroups into my_group[]
 */

void
read_news_active_file (void)
{
	FILE *fp = 0;
	char buf[HEADER_LEN];
	char moderated[PATH_LEN];
	char *ptr;
	struct t_group *grpptr;
	long count = -1L, min = 1, max = 0;

	/*
	 * Ignore -n if no .newsrc can be found
	 */
	if (newsrc_active && ((fp = fopen (newsrc, "r")) == (FILE *) 0))
		newsrc_active = FALSE;

	if (INTERACTIVE)
		wait_message (0, newsrc_active ? txt_reading_news_newsrc_file : txt_reading_news_active_file);

	if (!newsrc_active) {
		if ((fp = open_news_active_fp ()) == (FILE *) 0) {

			if (cmd_line)
				my_fputc ('\n', stderr);

#ifdef NNTP_ABLE
				error_message (txt_cannot_open_active_file, news_active_file, progname);
#else
				error_message (txt_cannot_open, news_active_file);
#endif

			tin_done (EXIT_ERROR);
		}
	}
/* TODO should we move default count,max,min here as in v1.3, these should really be explicitly set */
	while ((ptr = tin_fgets (buf, sizeof(buf), fp)) != (char *)0) {

		if (newsrc_active) {
			if (!parse_newsrc_active_line (ptr, &count, &max, &min, moderated))
				continue;
		} else {
			if (!parse_active_line (ptr, &max, &min, moderated))
				continue;
		}

		/*
		 * Load group into group hash table
		 * Error => duplicate group
		 */
		if ((grpptr = psGrpAdd(ptr)) == NULL)
			continue;

		/*
		 * Load the new group in active[]
		 */
		active_add(grpptr, count, max, min, moderated);

#ifdef SHOW_PROGRESS		/* Spin the arrow as we read the active file */
		if (num_active % ((newsrc_active) ? 5 : MODULO_COUNT_NUM) == 0)
			spin_cursor ();
#endif
	}

	if (newsrc_active)
		fclose(fp);
	else
		TIN_FCLOSE (fp);

	/*
	 *  Exit if active file wasn't read or is empty
	 */
	if (tin_errno != 0 || !num_active) {
		error_message (txt_active_file_is_empty, news_active_file);
		tin_done (EXIT_ERROR);
	}

	if (INTERACTIVE2)
		wait_message (0, "\n");

	check_for_any_new_groups ();
}

/*
 * Check for any newly created newsgroups.
 *
 * If reading news locally check the NEWSLIBDIR/active.times file.
 * Format:   Groupname Seconds Creator
 *
 * If reading news via NNTP issue a NEWGROUPS command.
 * Format:   (as active file) Groupname Maxart Minart moderated
 */

static void
check_for_any_new_groups (void)
{
	char *autosubscribe, *autounsubscribe;
	char *ptr, buf[NNTP_STRLEN];
	char old_newnews_host[PATH_LEN];
	int newnews_index = -1;
	FILE *fp = (FILE *) 0;
	time_t the_newnews_time = (time_t) 0;
	time_t old_newnews_time;

	if (!check_for_new_newsgroups || !INTERACTIVE)
		return;

	wait_message (0, txt_checking_new_groups);

	time (&the_newnews_time);

	if (read_news_via_nntp)
		strcpy (new_newnews_host, nntp_server);
	else
		strcpy (new_newnews_host, "local");	/* What if nntp server called local ? */

	/*
	 * find out if we have read news from here before otherwise -1
	 */
	newnews_index = find_newnews_index (new_newnews_host);

	if (newnews_index >= 0) {
		strcpy (old_newnews_host, newnews[newnews_index].host);
		old_newnews_time = newnews[newnews_index].time;
	} else {
		strcpy (old_newnews_host, "UNKNOWN");
		old_newnews_time = (time_t) 0;
	}

#ifdef DEBUG
	if (debug == 2) {
		error_message("Newnews old=[%ld]  new=[%ld]", old_newnews_time, the_newnews_time);
		sleep (2);
	}
#endif

	if ((fp = open_newgroups_fp (newnews_index)) != (FILE *) 0) {

		/*
		 * Need these later. They list user-defined groups to be
		 * automatically subscribed or unsubscribed.
		 */
		autosubscribe = getenv ("AUTOSUBSCRIBE");
		autounsubscribe = getenv ("AUTOUNSUBSCRIBE");

		while (tin_fgets (buf, sizeof (buf), fp) != (char *) 0) {

			/*
			 * Split the group name off and subscribe. If we're reading local,
			 * we must check the creation date manually
			 */
			if ((ptr = strchr (buf, ' ')) != (char *) 0) {

				if (!read_news_via_nntp && ((time_t) atol (ptr) < old_newnews_time || old_newnews_time == (time_t) 0))
					continue;

				*ptr = '\0';
			}

			subscribe_new_group(buf, autosubscribe, autounsubscribe);
		}

		TIN_FCLOSE (fp);
		if (tin_errno != 0)
			return;				/* Don't update the time if we quit */
	}

	/*
	 * Update or create the in-memory 'last time newgroups checked' array
	 */
	if (newnews_index >= 0)
		newnews[newnews_index].time = the_newnews_time;
	else {
		sprintf (buf, "%s %ld", new_newnews_host, the_newnews_time);
		load_newnews_info (buf);
	}
}

/*
 * Subscribe to a new news group:
 * Handle the AUTOSUBSCRIBE/AUTOUNSUBSCRIBE env vars
 * They hold a wildcard list of groups that should be automatically
 * (un)subscribed when a new group is found
 * If a group is autounsubscribed, completely ignore it
 * If a group is autosubscribed, subscribe to it
 * Otherwise, mark it as New for inclusion in selection screen
 */
static void
subscribe_new_group (
	char *group,
	char *autosubscribe,
	char *autounsubscribe)
{
	int idx;
	struct t_group *ptr;

	/*
	 * If we explicitly don't auto subscribe to this group, then don't bother going on
	 */
	if ((autounsubscribe != (char *) 0) && match_group_list (group, autounsubscribe))
		return;

	/*
	 * Try to add the group to our selection list. If this fails, we're
	 * probably using -n, so we fake an entry with no counts. The count will
	 * be properly updated when we enter the group
	 */
	if ((idx = my_group_add(group)) < 0) {
		if (!newsrc_active)
			my_fprintf(stderr, "subscribe_new_group: group not in active[] && !newsrc_active\n");

		if ((ptr = psGrpAdd(group)) != NULL)
			active_add(ptr, 0L, 1L, 0L, "n");

		if ((idx = my_group_add(group)) < 0)
			return;
	}

	if ((autosubscribe != (char *) 0) && match_group_list (group, autosubscribe)) {
		my_printf("\nAutosubscribed to %s", group);

		subscribe (&active[my_group[idx]], SUBSCRIBED);
		/*
		 * Bad kluge to stop group later appearing in New newsgroups. This
		 * effectively loses the group, and it will be reread by read_newsrc()
		 */
		group_top--;
	} else
		active[my_group[idx]].newgroup = TRUE;
}

/*
 * See if group is a member of group_list, returning a boolean.
 * group_list is a comma separated list of newsgroups, ! implies NOT
 * The same degree of wildcarding as used elsewhere in tin is allowed
 */
int
match_group_list (
	char *group,
	char *group_list)
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

		if (separator != (char *) 0)
			group_len = separator-group_list;
		else
			group_len = list_len;

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
		 * case-insensitive wildcard match
		 */
		if (GROUP_MATCH(group, pattern, TRUE))
			accept = !negate;	/* matched!*/

		/*
		 * now examine next entry if any
		 */
		if ((char) 0 != group_list[group_len])
			group_len++;	/* skip the separator */

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

#ifdef INDEX_DAEMON
void
read_group_times_file (void)
{
	char *p, *q;
	char buf[HEADER_LEN];
	char group[HEADER_LEN];
	FILE *fp;
	time_t updated_time;
	struct t_group *psGrp;

	if ((fp = fopen (group_times_file, "r")) == (FILE *) 0)
		return;

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		/*
		 * read the group name
		 */
		for (p = buf, q = group ; *p && *p != ' ' && *p != '\t' ; p++, q++)
			*q = *p;

		*q = '\0';

		/*
		 * read the last updated time
		 */
		updated_time = (time_t) atol (p);

		/*
		 * find the group in active[] and set updated time
		 */
		psGrp = psGrpFind (group);

		if (psGrp)
			psGrp->last_updated_time = updated_time;

#ifdef DEBUG
if (debug == 2)
	my_printf ("group=[%-40.40s]  [%ld]\n", psGrp->name, psGrp->last_updated_time);
#endif

	}
	fclose (fp);
}

/*
 *  Save the last updated time for each group to ~/.tin/group.times
 */

void
write_group_times_file (void)
{
	FILE *fp;
	register int i;

	if ((fp = fopen (group_times_file, "w")) == (FILE *) 0)
		return;

	for (i = 0 ; i < num_active ; i++)
		fprintf (fp, "%s %ld\n", active[i].name, active[i].last_updated_time);

	fclose (fp);
}
#endif	/* INDEX_DAEMON */

/*
 * Add an entry to the in-memory newnews[] array (The times newgroups were last
 * checked for on each news server)
 * If this is first time we've been called, zero out the array.
 * Growing the array if needed
 */
void
load_newnews_info (
	char *info)
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

	/* TODO: Surely this is a waste of time, we strdup() this 10 lines lower down */
	my_strncpy (buf, info, sizeof (buf));

	if ((ptr = strchr (buf, ' ')) != (char *) 0) {
		the_time = (time_t) atol (ptr);
		*ptr = '\0';
		if (num_newnews >= max_newnews)
			expand_newnews ();

		newnews[num_newnews].host = my_strdup (buf);
		newnews[num_newnews].time = the_time;
#ifdef DEBUG
		if (debug == 2)
			error_message("ACTIVE host=[%s] time=[%ld]", newnews[num_newnews].host, newnews[num_newnews].time);
#endif
		num_newnews++;
	}
}

/*
 * Return the index of cur_newnews_host in newnews[] or -1 if not found
 */
static int
find_newnews_index (
	char *cur_newnews_host)
{
	int i;

	for (i = 0 ; i < num_newnews ; i++) {
		if (STRCMPEQ(cur_newnews_host, newnews[i].host))
			return(i);
	}

	return (-1);
}

/*
 * check for message of the day (motd) file
 *
 * If reading news locally stat() the active file to get its
 * mtime otherwise do a XMOTD command to the NNTP server
 */

void
read_motd_file (void)
{
#ifndef INDEX_DAEMON
	char motd_file_date[32];
	time_t new_motd_date = (time_t) 0;
	time_t old_motd_date = (time_t) 0;
	struct stat sb;
	struct tm *tm;
#ifdef HAVE_TIN_NNTP_EXTS
	char buf[NNTP_STRLEN];
	char *line;
	FILE *fp;
	int lineno = 0;
#endif /* HAVE_TIN_NNTP_EXTS */

	if (!INTERACTIVE)
		return;

	old_motd_date = (time_t) atol (motd_file_info);

	/*
	 * reading news locally (local) or via NNTP (server name)
	 */
	if (read_news_via_nntp) {
		if (!old_motd_date)
			strcpy (motd_file_date, "920101 000000");
		else {
			tm = localtime (&old_motd_date);
			sprintf (motd_file_date, "%02d%02d%02d %02d%02d%02d",
				tm->tm_year, tm->tm_mon+1, tm->tm_mday,
				tm->tm_hour, tm->tm_min, tm->tm_sec);
		}
		time (&new_motd_date);
	} else {
		if (stat (motd_file, &sb) >=0)
			new_motd_date = sb.st_mtime;
	}

	if (old_motd_date && new_motd_date > old_motd_date) {
#ifdef HAVE_TIN_NNTP_EXTS
		if ((fp = open_motd_fp (motd_file_date)) != (FILE *) 0) {
			while ((line = tin_fgets (buf, sizeof (buf), fp)) != 0) {
				my_printf ("%s", line);
				lineno++;
			}
			TIN_FCLOSE (fp);

/* TODO probably needs testing */
			if (lineno) {
				wait_message (0, txt_return_key);
				Raw (TRUE);
				ReadCh ();
				Raw (FALSE);
				wait_message (0, "\n");
			}
		}
#endif
	}

	/*
	 * update motd tinrc entry with new date
	 */
	sprintf (motd_file_info, "%ld", new_motd_date);

#endif	/* INDEX_DAEMON */
}

/*
 * Get a single status char from the moderated field. Used on selection screen
 * and in header of group screen
 */
int
group_flag(int ch)
{
	switch (ch) {
		case 'm':
			return 'M';
		case 'x':
		case 'n':
			return 'X';
		case '=':
			return '=';
		default:
			return ' ';
	}
}

#ifdef INDEX_DAEMON
void
vMakeActiveMyGroup (void)
{
	register int iNum;

	group_top = 0;

	for (iNum = 0; iNum < num_active; iNum++)
		my_group[group_top++] = iNum;
}
#endif
