/*
 *  Project   : tin - a Usenet reader
 *  Module    : active.c
 *  Author    : I. Lea
 *  Created   : 1992-02-16
 *  Updated   : 1998-05-02
 *  Notes     :
 *  Copyright : (c) Copyright 1991-99 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef TIN_H
#	include	"tin.h"
#endif /* !TIN_H */
#ifndef TCURSES_H
#	include	"tcurses.h"
#endif /* !TCURSES_H */

/*
 * List of allowed seperator chars in active file
 * unsed in parse_active_line()
 */
#define ACTIVE_SEP		" \n"

t_bool force_reread_active_file = FALSE;

static char acSaveActiveFile[PATH_LEN];
static time_t active_timestamp;	/* time active file read (local) */

#ifdef NNTP_ABLE
#	define NUM_SIMULTANEOUS_GROUP_COMMAND 50
#endif /* NNTP_ABLE */


/*
 * Local prototypes
 */
static int find_newnews_index (char *cur_newnews_host);
static void active_add (struct t_group *ptr, long count, long max, long min, const char *moderated);
static void check_for_any_new_groups (void);
static void read_active_file (void);
static void read_newsrc_active_file (void);
static void set_active_timestamp (void);
static void subscribe_new_group (char *group, char *autosubscribe, char *autounsubscribe);
static void vAppendGrpLine (char *pcActiveFile, char *pcGrpPath, long lArtMax, long lArtMin, char *pcBaseDir);
static void vInitVariables (void);
static void vMakeGrpList (char *pcActiveFile, char *pcBaseDir, char *pcGrpPath);


/*
 *  Get default array size for active[] from environment (AmigaDOS)
 *  or just return the standard default.
 */
int
get_active_num (
	void)
{
#ifdef ENV_VAR_GROUPS /* M_AMIGA && M_OS2 && WIN32 only */
	char *ptr;
	int num;

	if ((ptr = getenv (ENV_VAR_GROUPS)) != (char *) 0)
		return ((num = atoi (ptr)) ? num : DEFAULT_ACTIVE_NUM);
#endif /* ENV_VAR_GROUPS */
	return DEFAULT_ACTIVE_NUM;
}


static void
set_active_timestamp (
	void)
{
	(void) time (&active_timestamp);
	force_reread_active_file = FALSE;
}


t_bool
reread_active_file (
	void)
{
	return (force_reread_active_file || (tinrc.reread_active_file_secs != 0 &&
		(int)(time(NULL) - active_timestamp) >= tinrc.reread_active_file_secs));
}


/*
 * Resync active file when reread_active_file_secs have passed or
 * force_reread_actve_file is set.
 */
t_bool
resync_active_file (
	void)
{
	char old_group[HEADER_LEN];
	t_bool command_line = FALSE;

	if (!reread_active_file ())
		return FALSE;

	reread_active_for_posted_arts = FALSE;

	if (cur_groupnum >= 0 && group_top)
		strcpy (old_group, CURR_GROUP.name);
	else
		old_group[0] = '\0';

	vWriteNewsrc ();
	read_news_active_file ();

#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
	read_mail_active_file ();
#endif /* !INDEX_DAEMON && HAVE_MH_MAIL_HANDLING */

	if (read_cmd_line_groups ())
		command_line = TRUE;

	read_newsrc (newsrc, bool_not(command_line));

	if (command_line)		/* Can't show only unread groups with cmd line groups */
		tinrc.show_only_unread_groups = FALSE;
	else
		toggle_my_groups (tinrc.show_only_unread_groups, old_group);

	set_groupname_len (FALSE);
	show_selection_page ();

	return TRUE;
}


/*
 * Populate a slot in the active[] array
 * TODO: 1) Have a preinitialised default slot and block assign it for speed
 * TODO: 2) Lump count/max/min/moderat into a t_active, big patch but much cleaner throughout tin
 */
static void
active_add (
	struct t_group *ptr,
	long count,
	long max,
	long min,
	const char *moderated)
{
	/* name - pre-initialised when group is made */
	ptr->aliasedto = ((moderated[0] == '=') ? my_strdup(moderated+1) : (char *) 0);
	ptr->description = (char *) 0;
	/* spool - see below */
	ptr->moderated = moderated[0];
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
#endif /* INDEX_DAEMON */

#ifdef WIN32				/* Paths are in form - x:\a\b\c */
	if (strchr(moderated, '\\'))
#else
	if (moderated[0] == '/')
#endif /* WIN32 */
	{
		ptr->type = GROUP_TYPE_SAVE;
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
t_bool
process_bogus (
	char *name) /* return value is always ignored */
{
	struct t_group *ptr;

	if (read_saved_news)
		return FALSE;

	if ((ptr = psGrpAdd(name)) == NULL)
		return FALSE;

	active_add (ptr, 0L, 1L, 0L, "n");
	ptr->bogus = TRUE;		/* Mark it bogus */

	if (my_group_add(name) < 0)
		return TRUE;

	return FALSE;		/* Nothing was printed yet */
}


/*
 * Parse line from news or mail active files
 */
t_bool
parse_active_line (
	char *line,
	long *max,
	long *min,
	char *moderated)
{
	char *p, *q, *r;

	if (line[0] == '#' || line[0] == '\0')
		return FALSE;

	(void) strtok(line, ACTIVE_SEP);		/* skip group name */
	p = strtok((char *)0, ACTIVE_SEP);	/* group max count */
	q = strtok((char *)0, ACTIVE_SEP);	/* group min count */
	r = strtok((char *)0, ACTIVE_SEP);	/* mod status or path to mailgroup */

	if (!p || !q || !r) {
		error_message (txt_bad_active_file, line);
		return FALSE;
	}

	*max = atol (p);
	*min = atol (q);
	strcpy(moderated, r);

	return TRUE;
}


/*
 * Load the active information into active[] by counting the min/max/count
 * for each news group.
 * Parse a line from the .newsrc file
 * Send GROUP command to NNTP server directly to keep window.
 * We can't know the 'moderator' status and always return 'y'
 * But we don't change if the 'moderator' status is alredy checked by
 * read_active_file()
 */
static void
read_newsrc_active_file (
	void)
{
	FILE *fp = (FILE *) 0;
	char *ptr;
	char moderated[PATH_LEN];
	int window = 0;
	long count = -1L, min = 1L, max = 0L;
	long processed = 0L;
	static char ngname[NNTP_STRLEN];
	struct t_group *grpptr;
#ifdef NNTP_ABLE
	char *ngnames[NUM_SIMULTANEOUS_GROUP_COMMAND];
	int index_i = 0;
	int index_o = 0;
#endif /* NNTP_ABLE */

	/*
	 * return immediately if no .newsrc can be found or .newsrc is empty
	 * when function asked to use .newsrc
	 */
	if ((fp = fopen (newsrc, "r")) == (FILE *) 0)
		return;

	if (file_size(newsrc) <= 0) {
		fclose(fp);
		return;
	}

	if (INTERACTIVE)
		wait_message (0, txt_reading_news_newsrc_file);

	while ((ptr = tin_fgets (fp, FALSE)) != (char *)0 || window != 0) {
		if (ptr) {
			char *p = strpbrk (ptr, ":!");

			if (!p || *p != SUBSCRIBED)	/* Invalid line or unsubscribed */
				continue;
			*p = '\0';			/* Now ptr is the group name */

			/*
			 * 128 should be enough for a groupname, >256 and we overflow buffers
			 * later on
			 */
			strncpy(ngname, ptr, 128);
			ptr = ngname;
		}

		if (read_news_via_nntp) {
#ifdef NNTP_ABLE
			char acBuf[NNTP_STRLEN];
			char acLine[NNTP_STRLEN];
			if (window < NUM_SIMULTANEOUS_GROUP_COMMAND && ptr) {
				ngnames[index_i] = my_strdup(ptr);
				sprintf (acBuf, "GROUP %s", ngnames[index_i]);
#	ifdef DEBUG
				debug_nntp ("read_newsrc_active_file", acBuf);
#	endif /* DEBUG */
				put_server (acBuf);
				index_i = (index_i + 1) % NUM_SIMULTANEOUS_GROUP_COMMAND;
				window++;
			}
			if (window == NUM_SIMULTANEOUS_GROUP_COMMAND || ptr == NULL) {
				int respcode = get_respcode(acLine);

				if (reconnected_in_last_get_server) {
					/*
					 * If tin reconnected, last output is resended to server.
					 * So received data is for ngnames[last window_i].
					 * We resend all buffered command except for last window_i.
					 * And rotate buffer to use data received.
					 */
					int i;
					int j = index_o;
					for (i = 0; i < window - 1; i++) {
						sprintf (acBuf, "GROUP %s", ngnames[j]);
#	ifdef DEBUG
						debug_nntp ("read_newsrc_active_file", acBuf);
#	endif /* DEBUG */
						put_server (acBuf);
						j = (j + 1) % NUM_SIMULTANEOUS_GROUP_COMMAND;
					}
					if (--index_o < 0)
						index_o = NUM_SIMULTANEOUS_GROUP_COMMAND - 1;
					if (--index_i < 0)
						index_i = NUM_SIMULTANEOUS_GROUP_COMMAND - 1;
					if (index_i != index_o)
						ngnames[index_o] = ngnames[index_i];
				}

				switch (respcode) {

					case OK_GROUP:
						{
							char fmt[20];

							sprintf(fmt, "%%ld %%ld %%ld %%%ds", NNTP_STRLEN);

							if (sscanf (acLine, fmt, &count, &min, &max, ngname) != 4)
								error_message("Invalid response to GROUP command, %s", acLine); /* FIXME: -> lang.c*/
							if (strcmp(ngname, ngnames[index_o]) != 0)
								error_message("Wrong newsgroup name in response of GROUP command, %s for %s", acLine, ngnames[index_o]); /* FIXME: -> lang.c */
							ptr = ngname;
							free(ngnames[index_o]);
							index_o = (index_o + 1) % NUM_SIMULTANEOUS_GROUP_COMMAND;
							window--;
							break;
						}

					case ERR_NOGROUP:
						free(ngnames[index_o]);
						index_o = (index_o + 1) % NUM_SIMULTANEOUS_GROUP_COMMAND;
						window--;
						continue;

					case ERR_ACCESS:
						error_message (cCRLF "%s", acLine);
						tin_done (NNTP_ERROR_EXIT);
						/* keep lint quiet: */
						/* FALLTHROUGH */
					default:
#	ifdef DEBUG
						debug_nntp ("NOT_OK", acLine);
#	endif /* DEBUG */
						free(ngnames[index_o]);
						index_o = (index_o + 1) % NUM_SIMULTANEOUS_GROUP_COMMAND;
						window--;
						continue;
				}
			} else
				continue;
#endif /* NNTP_ABLE */
		} else {
			if (vGrpGetArtInfo (spooldir, ptr, GROUP_TYPE_NEWS, &count, &max, &min))
				continue;
		}

		strcpy (moderated, "y");

		if (++processed % 5 == 0)
			spin_cursor ();

		/*
		 * Load group into group hash table
		 * NULL means group already present, so we just fixup the counters
		 * This call may implicitly ++num_active
		 */
		if ((grpptr = psGrpAdd(ptr)) == NULL) {
			t_bool changed = FALSE;

			if ((grpptr = psGrpFind(ptr)) == NULL)
				continue;

			if (max > grpptr->xmax) {
				grpptr->xmax = max;
				changed = TRUE;
			}
			if (min > grpptr->xmin) {
				grpptr->xmin = min;
				changed = TRUE;
			}
			if (changed) {
				grpptr->count = count;
				expand_bitmap(grpptr, 0); /* expand_bitmap(grpptr,grpptr->xmin) should be enought*/
			}
			continue;
		}

		/*
		 * Load the new group in active[]
		 */
		active_add (grpptr, count, max, min, moderated);
	}

	fclose(fp);

	/*
	 *  Exit if active file wasn't read correctly or is empty
	 */
	if (tin_errno || !num_active) {
		error_message (txt_active_file_is_empty, (read_news_via_nntp ? txt_servers_active : news_active_file));
		tin_done (EXIT_FAILURE);
	}

	if (INTERACTIVE)
		my_fputs("\n", stdout);
}


/*
 * Load the active file into active[]
 */
static void
read_active_file (
	void)
{
	FILE *fp = (FILE *) 0;
	char *ptr;
	char moderated[PATH_LEN];
	long count = -1L, min = 1L, max = 0L;
	long processed = 0L;
	struct t_group *grpptr;

	if (INTERACTIVE)
		wait_message (0, txt_reading_news_active_file);

	if ((fp = open_news_active_fp ()) == (FILE *) 0) {

		if (cmd_line)
			my_fputc ('\n', stderr);

#if defined(NNTP_ABLE) || defined(NNTP_ONLY)
		if (read_news_via_nntp)
#endif /* NNTP_ABLE || NNTP_ONLY */
			error_message (txt_cannot_open, news_active_file);
#if defined(NNTP_ABLE) || defined(NNTP_ONLY)
		else
			error_message (txt_cannot_open_active_file, news_active_file, tin_progname);
#endif /* NNTP_ABLE || NNTP_ONLY */

		tin_done (EXIT_FAILURE);
	}

	while ((ptr = tin_fgets (fp, FALSE)) != (char *)0) {
		if (!parse_active_line (ptr, &max, &min, moderated))
			continue;

		if (++processed % MODULO_COUNT_NUM == 0)
			spin_cursor ();

		/*
		 * Load group into group hash table
		 * NULL means group already present, so we just fixup the counters
		 * This call may implicitly ++num_active
		 */
		if ((grpptr = psGrpAdd(ptr)) == NULL) {

			if ((grpptr = psGrpFind(ptr)) == NULL)
				continue;

			if (max > grpptr->xmax) {
				grpptr->xmax = max;
				grpptr->count = count;
			}

			if (min > grpptr->xmin) {
				grpptr->xmin = min;
				grpptr->count = count;
			}

			continue;
		}

		/*
		 * Load the new group in active[]
		 */
		active_add (grpptr, count, max, min, moderated);
	}

	TIN_FCLOSE (fp);

	/*
	 *  Exit if active file wasn't read correctly or is empty
	 */
	if (tin_errno || !num_active) {
		error_message (txt_active_file_is_empty, (read_news_via_nntp ? txt_servers_active : news_active_file));
		tin_done (EXIT_FAILURE);
	}

	if (INTERACTIVE)
		my_fputs("\n", stdout);
}


/*
 * Load the active file into active[]
 * Check and preload any new newgroups into my_group[]
 */
void
read_news_active_file (
	void)
{
	FILE *fp = 0;

	/*
	 * Ignore -n if no .newsrc can be found or .newsrc is empty
	 */
	if (newsrc_active) {
		if ((fp = fopen (newsrc, "r")) == (FILE *) 0) {
			list_active = TRUE;
			newsrc_active = FALSE;
		} else
			fclose(fp);

		if (file_size(newsrc) <= 0) {
			list_active = TRUE;
			newsrc_active = FALSE;
		}
	}

	/* Read an active file if it is allowed */
	if (list_active)
		read_active_file ();

	/* Read .newsrc and check each group */
	if (newsrc_active)
		read_newsrc_active_file ();

	set_active_timestamp ();

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
check_for_any_new_groups (
	void)
{
	FILE *fp;
	char *autosubscribe, *autounsubscribe;
	char *ptr, *line, buf[NNTP_STRLEN];
	char old_newnews_host[PATH_LEN];
	char new_newnews_host[PATH_LEN];
	int newnews_index;
	time_t old_newnews_time;
	time_t new_newnews_time;

	if (!check_for_new_newsgroups || !INTERACTIVE)
		return;

	wait_message (0, txt_checking_new_groups);
	(void) time (&new_newnews_time);
	strcpy (new_newnews_host, (read_news_via_nntp ? nntp_server : "local")); /* What if nntp server called local ? */

	/*
	 * find out if we have read news from here before otherwise -1
	 */
	if ((newnews_index = find_newnews_index (new_newnews_host)) >= 0) {
		strcpy (old_newnews_host, newnews[newnews_index].host);
		old_newnews_time = newnews[newnews_index].time;
	} else {
		strcpy (old_newnews_host, "UNKNOWN");
		old_newnews_time = (time_t) 0;
	}

#ifdef DEBUG
	if (debug == 2) {
		error_message("Newnews old=[%lu]  new=[%lu]", (unsigned long int) old_newnews_time, (unsigned long int) new_newnews_time);
		(void) sleep (2);
	}
#endif /* DEBUG */

	if ((fp = open_newgroups_fp (newnews_index)) != (FILE *) 0) {

		/*
		 * Need these later. They list user-defined groups to be
		 * automatically subscribed or unsubscribed.
		 */
		autosubscribe = getenv ("AUTOSUBSCRIBE");
		autounsubscribe = getenv ("AUTOUNSUBSCRIBE");

		while ((line = tin_fgets (fp, FALSE)) != (char *) 0) {

			/*
			 * Split the group name off and subscribe. If we're reading local,
			 * we must check the creation date manually
			 */
			if ((ptr = strchr (line, ' ')) != (char *) 0) {
				if (!read_news_via_nntp && ((time_t) atol (ptr) < old_newnews_time || old_newnews_time == (time_t) 0))
					continue;

				*ptr = '\0';
			}

			subscribe_new_group (line, autosubscribe, autounsubscribe);
		}

		TIN_FCLOSE (fp);

		free_attributes_array ();
		read_attributes_file (global_attributes_file, TRUE);
		read_attributes_file (local_attributes_file, FALSE);

		if (tin_errno)
			return;				/* Don't update the time if we quit */
	}

	/*
	 * Update (if already existing) or create (if new) the in-memory
	 * 'last time newgroups checked' slot for this server. It will be written
	 * out as part of tinrc.
	 */
	if (newnews_index >= 0)
		newnews[newnews_index].time = new_newnews_time;
	else {
		sprintf (buf, "%s %lu", new_newnews_host, (unsigned long int) new_newnews_time);
		load_newnews_info (buf);
	}
	my_fputc('\n',stdout);
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
	 * be properly updated when we enter the group. Otherwise there is some
	 * mismatch in the active.times data and we ignore the newgroup.
	 */
	if ((idx = my_group_add(group)) < 0) {
		if (list_active) {
/*			my_fprintf(stderr, "subscribe_new_group: %s not in active[] && list_active\n", group); */
			return;
		}

		if ((ptr = psGrpAdd(group)) != NULL)
			active_add(ptr, 0L, 1L, 0L, "y");

		if ((idx = my_group_add(group)) < 0)
			return;
	}

	if (!no_write && (autosubscribe != (char *) 0) && match_group_list (group, autosubscribe)) {
		my_printf (txt_autosubscribed, group);

		subscribe (&active[my_group[idx]], SUBSCRIBED);
		/*
		 * Bad kluge to stop group later appearing in New newsgroups. This
		 * effectively loses the group, and it has now been subscribed to and
		 * so will be reread later by read_newsrc()
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
t_bool
match_group_list (
	char *group,
	char *group_list)
{
	char *separator;
	char pattern[HEADER_LEN];
	size_t group_len, list_len;
	t_bool accept, negate;

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
		group_len = ((separator == (char *) 0) ? list_len : (size_t)(separator - group_list));

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
		pattern[group_len] = '\0';
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
 * Add or update an entry to the in-memory newnews[] array (The times newgroups
 * were last checked for a particular news server)
 * If this is first time we've been called, zero out the array.
 *
 * Side effects:
 *   'info' is modified. Caller should not depend on it.
 */
void
load_newnews_info (
	char *info)
{
	char *ptr;
	int i;
	time_t new_time;

	/*
	 * initialize newnews[] if no entries
	 */
	if (!num_newnews) {
		for (i = 0; i < max_newnews; i++) {
			newnews[i].host = (char *) 0;
			newnews[i].time = (time_t) 0;
		}
	}

	/*
	 * Split 'info' into hostname and time
	 */
	if ((ptr = strchr (info, ' ')) == (char *) 0)
		return;

	new_time = (time_t) atol (ptr);
	*ptr = '\0';

	/*
	 * If this is a new host entry, set it up
	 */
	if ((i = find_newnews_index (info)) == -1) {
		i = num_newnews++;

		if (i >= max_newnews)
			expand_newnews ();
		newnews[i].host = my_strdup (info);
	}

	newnews[i].time = new_time;

#ifdef DEBUG
	if (debug == 2)
		error_message("ACTIVE host=[%s] time=[%lu]", newnews[i].host, (unsigned long int) newnews[i].time);
#endif /* DEBUG */
}


/*
 * Return the index of cur_newnews_host in newnews[] or -1 if not found
 */
static int
find_newnews_index (
	char *cur_newnews_host)
{
	int i;

	for (i = 0; i < num_newnews; i++) {
		if (STRCMPEQ(cur_newnews_host, newnews[i].host))
			return(i);
	}

	return -1;
}


/*
 * Get a single status char from the moderated field. Used on selection screen
 * and in header of group screen
 */
char
group_flag (
	int ch)
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


/* ex actived.c functions */
void
create_save_active_file (
	void)
{
	char acGrpPath[PATH_LEN];

	vInitVariables ();

	if (no_write && file_size (acSaveActiveFile) != -1)
		return;

	my_printf (txt_creating_active);

	vPrintActiveHead (acSaveActiveFile);
	strcpy (acGrpPath, tinrc.savedir);
	vMakeGrpList (acSaveActiveFile, tinrc.savedir, acGrpPath);
}


static void
vInitVariables (
	void)
{
	char *pcPtr;
	char acTempActiveFile[PATH_LEN];
	char acMailActiveFile[PATH_LEN];
	char acMailDir[PATH_LEN];
	char acHomeDir[PATH_LEN];
	char acSaveDir[PATH_LEN];
#ifndef M_AMIGA
	struct passwd *psPwd;
	struct passwd sPwd;

	psPwd = (struct passwd *) 0;
	if (((pcPtr = getlogin ()) != (char *) 0) && strlen (pcPtr))
		psPwd = getpwnam (pcPtr);

	if (psPwd == (struct passwd *) 0)
		psPwd = getpwuid (getuid ());

	if (psPwd != (struct passwd *) 0) {
		memcpy (&sPwd, psPwd, sizeof (struct passwd));
		psPwd = &sPwd;
	}
#endif /* !M_AMIGA */

	if ((pcPtr = getenv ("TIN_HOMEDIR")) != (char *) 0) {
		strcpy (acHomeDir, pcPtr);
	} else if ((pcPtr = getenv ("HOME")) != (char *) 0) {
		strcpy (acHomeDir, pcPtr);
#ifndef M_AMIGA
	} else if (psPwd != (struct passwd *) 0) {
		strcpy (acHomeDir, psPwd->pw_dir);
	} else
		strcpy (acHomeDir, "/tmp");
#else
	} else
		strcpy (acHomeDir, "T:");
#endif /* !M_AMIGA */

#ifdef WIN32
#	define DOTTINDIR "tin"
#else
#	define DOTTINDIR ".tin"
#endif /* WIN32 */
	sprintf (acTempActiveFile, "%s/%s/%ld.tmp", acHomeDir, DOTTINDIR, (long) getpid ());
	sprintf (acMailActiveFile, "%s/%s/%s", acHomeDir, DOTTINDIR, ACTIVE_MAIL_FILE);
	sprintf (acSaveActiveFile, "%s/%s/%s", acHomeDir, DOTTINDIR, ACTIVE_SAVE_FILE);
	sprintf (acMailDir, "%s/Mail", acHomeDir);
	sprintf (acSaveDir, "%s/News", acHomeDir);
}


static void
vMakeGrpList (
	char *pcActiveFile,
	char *pcBaseDir,
	char *pcGrpPath)
{
	DIR *tDirFile;
	DIR_BUF *tFile;
	char *pcPtr;
	char acFile[PATH_LEN];
	char acPath[PATH_LEN];
	long lArtMax;
	long lArtMin;
	struct stat sStatInfo;
	t_bool bIsDir;

	if (access (pcGrpPath, R_OK))
		return;

	tDirFile = opendir (pcGrpPath);

	if (tDirFile != (DIR *) 0) {
		bIsDir = FALSE;
		while ((tFile = readdir (tDirFile)) != (DIR_BUF *) 0) {
			STRCPY(acFile, tFile->d_name);
			sprintf (acPath, "%s/%s", pcGrpPath, acFile);

			if (!(acFile[0] == '.' && acFile[1] == '\0') &&
				!(acFile[0] == '.' && acFile[1] == '.' && acFile[2] == '\0')) {
				if (stat (acPath, &sStatInfo) != -1) {
					if (S_ISDIR(sStatInfo.st_mode))
						bIsDir = TRUE;
				}
			}
			if (bIsDir) {
				bIsDir = FALSE;
				strcpy (pcGrpPath, acPath);

				vMakeGrpList (pcActiveFile, pcBaseDir, pcGrpPath);
				vFindArtMaxMin (pcGrpPath, &lArtMax, &lArtMin);
				vAppendGrpLine (pcActiveFile, pcGrpPath, lArtMax, lArtMin, pcBaseDir);

				pcPtr = strrchr (pcGrpPath, '/');
				if (pcPtr != (char *) 0)
					*pcPtr = '\0';
			}
		}
		CLOSEDIR(tDirFile);
	}
}


static void
vAppendGrpLine (
	char *pcActiveFile,
	char *pcGrpPath,
	long lArtMax,
	long lArtMin,
	char *pcBaseDir)
{
	FILE *hFp;
	char acGrpName[PATH_LEN];

	if (lArtMax == 0 && lArtMin == 1)
		return;

	if ((hFp = fopen (pcActiveFile, "a+")) != (FILE *) 0) {
		vMakeGrpName (pcBaseDir, acGrpName, pcGrpPath);
		my_printf ("Appending=[%s %ld %ld %s]\n", acGrpName, lArtMax, lArtMin, pcBaseDir);
		vPrintGrpLine (hFp, acGrpName, lArtMax, lArtMin, pcBaseDir);
		fclose (hFp);
	}
}


#ifdef INDEX_DAEMON
void
vMakeActiveMyGroup (
	void)
{
	register int iNum;

	group_top = 0;

	for (iNum = 0; iNum < num_active; iNum++)
		my_group[group_top++] = iNum;
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
read_group_times_file (
	void)
{
	FILE *fp;
	char *p, *q;
	char buf[HEADER_LEN];
	char group[HEADER_LEN];
	time_t updated_time;
	struct t_group *psGrp;

	if ((fp = fopen (group_times_file, "r")) == (FILE *) 0)
		return;

	while (fgets (buf, (int) sizeof (buf), fp) != (char *) 0) {
		/*
		 * read the group name
		 */
		for (p = buf, q = group; *p && *p != ' ' && *p != '\t'; p++, q++)
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

#	ifdef DEBUG
		if (debug == 2)
			my_printf ("group=[%-40.40s]  [%lu]\n", psGrp->name, (unsigned long int) psGrp->last_updated_time);
#	endif /* DEBUG */

	}
	fclose (fp);
}


/*
 *  Save the last updated time for each group to ~/.tin/group.times
 */
void
write_group_times_file (
	void)
{
	FILE *fp;
	register int i;

	if ((fp = fopen (group_times_file, "w")) == (FILE *) 0)
		return;

	for (i = 0; i < num_active; i++)
		fprintf (fp, "%s %lu\n", active[i].name, (unsigned long int) active[i].last_updated_time);

	fclose (fp);
}
#endif /* INDEX_DAEMON */
