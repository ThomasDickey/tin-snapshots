/*
 *  Project   : tin - a Usenet reader
 *  Module    : search.c
 *  Author    : I. Lea & R. Skrenta
 *  Created   : 1991-04-01
 *  Updated   : 1997-12-27
 *  Notes     :
 *  Copyright : (c) Copyright 1991-98 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"

static char tmpbuf[LEN];

/*
 * local prototypes
 */
static char * get_search_pattern (int forward, const char *fwd_msg, const char *bwd_msg, char *def, int which_hist);
static int search_art_body (char *group_path, struct t_article *art, char *pat);
static int search_thread (int i, char *pattern);


/*
 * last search patterns (from tinrc file)
 */

char default_author_search[LEN];
char default_config_search[LEN];
char default_group_search[LEN];
char default_subject_search[LEN];
char default_art_search[LEN];

/*
 * Obtain the search pattern, save it in the default buffer.
 * Return NULL if no pattern could be found
 */
static char *
get_search_pattern(
	int forward,
	const char *fwd_msg,
	const char *bwd_msg,
	char *def,
	int which_hist)
{
	sprintf (tmpbuf, (forward ? fwd_msg : bwd_msg), def);

	if (!prompt_string_default(tmpbuf, def, txt_no_search_string, which_hist))
		return (NULL);

	wait_message (0, txt_searching);
	stow_cursor();

	if (wildcard) {			/* ie, not wildmat() */
		strcpy(def, quote_wild_whitespace (def));
		return(def);
	}

	/*
	 * A gross hack to simulate substrings with wildmat()
	 */
	sprintf(tmpbuf, "*%s*", def);
	return(tmpbuf);
}


/*
 * Called by group.c & page.c
 */
int
search_author (
	int the_index,
	int current_art,
	int forward)
{
	char *buf;
	char buf2[LEN];
	char group_path[PATH_LEN];
	int i;

	if (!(buf = get_search_pattern(forward, txt_author_search_forwards, txt_author_search_backwards, default_author_search, HIST_AUTHOR_SEARCH)))
		return -1;

	if (!read_news_via_nntp || active[the_index].type != GROUP_TYPE_NEWS)
		make_group_path (active[the_index].name, group_path);

	i = current_art;

	do {
		if (forward) {
			i = next_response (i);
			if (i < 0)
				i = base[0];
		} else {
			if ((i = prev_response (i)) < 0)
				i = find_response(top_base - 1, num_of_responses (top_base - 1));
		}

		if (active[the_index].attribute->show_only_unread && arts[i].status != ART_UNREAD)
			continue;

		if (arts[i].name == (char *) 0)
			strcpy (buf2, arts[i].from);
		else
			sprintf (buf2, "%s <%s>", arts[i].name, arts[i].from);

		if (REGEX_MATCH (buf2, buf, TRUE)) {
			/*
			 * check if article still exists
			 */
			if (stat_article (arts[i].artnum, group_path)) {
				clear_message ();
				return i;
			}
		}
	} while (i != current_art);

	info_message (txt_no_match);
	return -1;
}


/*
 * Called by config.c
 */
int
search_config (
	int forward,
	int current,
	int last)
{
	int n;
	int incr = forward ? 1 : -1;
	int result = current;
	char *buf;

	if (!(buf = get_search_pattern(
				forward,
				txt_search_forwards,
				txt_search_backwards,
				default_group_search,
				HIST_CONFIG_SEARCH
	))) return result;

	current += incr;
	n = current;
	do {
		if (n < 0)
			n = last;
		else if (n > last)
			n = 0;
		if (REGEX_MATCH (option_table[n].option_text, buf, TRUE)) {
			result = n;
			break;
		}
		n += incr;
	} while (n != current);
	clear_message ();
	return result;
}


#ifndef INDEX_DAEMON
/*
 * Called by select.c
 */
void
search_group (
	int forward)
{
	char *buf;
	char buf2[LEN];
	int i;

	if (!group_top) {
		info_message (txt_no_groups);
		return;
	}

	if (!(buf = get_search_pattern(forward, txt_search_forwards, txt_search_backwards, default_group_search, HIST_GROUP_SEARCH)))
		return;

	i = cur_groupnum;

	do {
		if (forward)
			i++;
		else
			i--;

		if (i >= group_top)
			i = 0;
		if (i < 0)
			i = group_top - 1;

		/*
		 * Get the group name & description into buf2
		 */
		if (show_description && active[my_group[i]].description)
			sprintf (buf2, "%s %s", active[my_group[i]].name, active[my_group[i]].description);
		else
			strcpy (buf2, active[my_group[i]].name);

		if (REGEX_MATCH (buf2, buf, TRUE)) {
			move_to_group(i);
			clear_message();
			return;
		}
	} while (i != cur_groupnum);

	info_message (txt_no_match);
}
#endif /* !INDEX_DAEMON */


/*
 * Called by help.c
 */
int
search_help (
	int forward,
	int current,
	int last)
{
	int n;
	int incr = forward ? 1 : -1;
	int result = current;
	char *buf;

	if (!(buf = get_search_pattern(
				forward,
				txt_search_forwards,
				txt_search_backwards,
				default_group_search,
				HIST_HELP_SEARCH
	))) return result;

	current += incr;
	n = current;
	do {
		if (n < 0)
			n = last;
		else if (n > last)
			n = 0;
		if (REGEX_MATCH (info_help[n], buf, TRUE)) {
			result = n;
			break;
		}
		n += incr;
	} while (n != current);
	clear_message ();
	return result;
}

/*
 * Internal function - traverse a thread looking for a subject match
 * Return -1 if not found, or the depth in the thread of the match
 * Performance note: we can compare hashed subjects directly as they
 * point to the same address. This is much cheaper than a REGEX_MATCH()
 */
static int
search_thread(
	int i,
	char *pattern)
{
	int art, depth = 0;
	char *old_subject = 0;

	for (art = i; art >= 0; art = arts[art].thread, ++depth) {

		/*
		 * Speed hack, if the subject remains constant, skip the check
		 */
		if (arts[art].subject == old_subject)
			continue;

		old_subject = arts[art].subject;

		if (REGEX_MATCH(old_subject, pattern, TRUE))
			return(depth);
	}

	return(-1);
}

/*
 * Search a thread for a subject. Reposition cursor & update internal
 * pointers as needed.
 */
#ifndef INDEX_DAEMON
void
search_subject_thread(
	int forward,
	int baseart,
	int offset)
{
	char *buf;
	int i, depth;

	if (!(buf = get_search_pattern(forward, txt_search_forwards, txt_search_backwards, default_subject_search, HIST_GROUP_SEARCH)))
		return;

	/*
	 * Advance to our current position in the thread
	 */
	for (i = 0; i < offset; i++)
		baseart = arts[baseart].thread;

	if ((depth = search_thread(baseart, buf)) == -1) {
		info_message (txt_no_match);
		return;
	}

	move_to_response(depth+offset);
	clear_message();
	return;
}
#endif

/*
 * Search the current group for a subject. Reposition the cursor if needed.
 * If the match happened inside a thread (ie after a subject change within a thread)
 * return the depth in the thread of the matching article. Otherwise -1.
 */
#ifndef INDEX_DAEMON
int
search_subject_group (
	int forward)
{
	char *buf;
	int i, j, depth=0;
	t_bool found = FALSE;

	if (index_point < 0) {
		info_message (txt_no_arts);
		return 0;
	}

	if (!(buf = get_search_pattern(
				forward,
				txt_search_forwards,
				txt_search_backwards,
				default_subject_search,
				HIST_SUBJECT_SEARCH
	))) return -1;

	i = index_point;						/* Search from current position */

	do {
		forward ? i++ : i--;

		if (i >= top_base)
			i = 0;

		if (i < 0)
			i = top_base - 1;

		j = (int) base[i];				/* Get index in arts[] of thread root */

		if (CURR_GROUP.attribute->thread_arts < THREAD_REFS) {
			if (REGEX_MATCH(arts[j].subject, buf, TRUE)) {
				found = TRUE;
				break;
			}
		} else {
			/*
			 * With threading on References, Subject lines can change mid thread.
			 * We must descend the thread in these cases
			 */
			if ((depth = search_thread (j, buf)) != -1) {
				found = TRUE;
				break;
			}
		}

	} while (i != index_point && !found);

	if (!found) {
		info_message (txt_no_match);
		return(-1);
	}

	if (depth != 0) {
		index_point = i;
		return(depth);		/* group.c needs to enter this thread */
	}

	/* Otherwise update the on-screen pointer */
	move_to_thread(i);
	clear_message();
	return(-1);				/* No furthur action needed in group.c */
}
#endif /* !INDEX_DAEMON */

/*
 * page.c (search current article body)
 *	TODO - highlight located text ?
 */
int
search_article (
	int forward)
{
	char buf[LEN];
	char buf2[LEN];
	char *pattern;
	char *p, *q;
	int ctrl_L;
	int i, j;
	int local_note_page = note_page; /* copy current position in article */
	t_bool local_note_end = note_end;

	if (!(pattern = get_search_pattern(
				forward,
				txt_search_forwards,
				txt_search_backwards,
				default_art_search,
				HIST_ART_SEARCH
	))) return FALSE;

	while (!local_note_end) {
		ctrl_L = FALSE;
		note_line = (local_note_page < 1) ? 5 : 3;

		while (note_line < cLINES) {
			if (fgets (buf, (int) sizeof(buf), note_fp) == NULL) {
				local_note_end = TRUE;
				break;
			}
			buf[LEN-1] = '\0';
			/*
			 * Build the string to search in buf2. Preprocess in the following ways:
			 * ignore backspaces
			 * expand pagefeed into a literal '^L'
			 * expand tabs
			 * maps control chars to '^char'
			 */
			for (p = buf, q = buf2;	*p && *p != '\n' && q<&buf2[LEN]; p++) {
				if (*p == '\b' && q > buf2) {
					q--;
				} else if (*p == '\f') {		/* ^L */
					*q++ = '^';
					*q++ = 'L';
					ctrl_L = TRUE;
				} else if (*p == '\t') {
					i = q - buf2;
					j = (i|7) + 1;
					while (i++ < j)
						*q++ = ' ';

				} else if (((*p) & 0xFF) < ' ') {
					*q++ = '^';
					*q++ = ((*p) & 0xFF) + '@';
				} else {
					*q++ = *p;
				}
			}
			*q = '\0';

			if (REGEX_MATCH (buf2, pattern, TRUE)) {
				fseek (note_fp, note_mark[local_note_page], SEEK_SET);
				return TRUE;
			}

			note_line += ((int) strlen(buf2) / cCOLS) + 1;

			if (ctrl_L)
				break;
		}
		if (!local_note_end)
			note_mark[++local_note_page] = ftell (note_fp);
	}

	fseek (note_fp, note_mark[note_page], SEEK_SET);
	info_message (txt_no_match);
	return FALSE;
}


/*
 * Scan the body of an article for a string.
 * used only by search_body()
 * Returns:	TRUE  String found
 *          FALSE Not found
 *          -1	   User aborted the search
 */
static int
search_art_body (
	char *group_path,
	struct t_article *art,
	char *pat)
{
	char *line;
	FILE *fp;

#if 1 /* see also screen.c show_progress ()*/
	if ((fp = open_art_fp (group_path, art->artnum, -art->lines, TRUE)) == (FILE *) 0)
#else
	if ((fp = open_art_fp (group_path, art->artnum, art->lines, TRUE)) == (FILE *) 0)
#endif
		return ((tin_errno != 0) ? -1 : FALSE);

	/*
	 * Skip the header
	 */
	while ((line = tin_fgets (fp, TRUE)) != (char *) 0) {
		if (*line == '\0')
			break;
	}

	if (tin_errno != 0) {
		fclose(fp);
		return(-1);
	}

	/*
	 * Now search the body
	 */
	while ((line = tin_fgets (fp, FALSE)) != (char *) 0) {
		if (REGEX_MATCH (line, pat, TRUE)) {
			fclose (fp);
			return TRUE;
		}
	}

	TIN_FCLOSE (fp);

	if (tin_errno != 0)
		return -1;

	return FALSE;
}

/*
 * Search the bodies of all the articles in current group
 */
int
search_body (
	struct t_group *group,
	int current_art)
{
	char group_path[PATH_LEN];
	char *pat;
	int art_cnt = 0, i, j = 0;

	if (!(pat = get_search_pattern(1, txt_search_body, txt_search_body, default_art_search, HIST_ART_SEARCH)))
		return -1;

	make_group_path (group->name, group_path);

	/*
	 * Count up the articles to be processed for the progress meter
	 */
	if (group->attribute->show_only_unread) {
		for (i = 0; i < top_base; i++)
			art_cnt += new_responses (i);
	} else {
		for (i = 0; i < top; i++) {
			if (!IGNORE_ART(i))
				art_cnt++;
		}
	}

	i = current_art;

	do {
		i = next_response (i);
		if (i < 0)
			i = base[0];

		if (group->attribute->show_only_unread && arts[i].status == ART_READ)
			continue;

		sprintf(mesg, txt_searching_body, ++j, art_cnt);

		switch (search_art_body (group_path, &arts[i], pat)) {
			case TRUE:					/* Found it okay */
				return i;

			case -1:					/* User aborted search */
				return -1;
		}

	} while (i != current_art);

	info_message (txt_no_match);

	return -1;
}
