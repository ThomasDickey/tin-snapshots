/*
 *  Project   : tin - a Usenet reader
 *  Module    : search.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 05-06-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"

static char tmpbuf[LEN];

/*
 * last search patterns (from tinrc file)
 */

char default_author_search[LEN];
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
	char *def)
{
	char pattern[LEN];

	clear_message ();

	if (forward)
		sprintf (tmpbuf, fwd_msg, def);
	else
		sprintf (tmpbuf, bwd_msg, def);

	if (!prompt_string (tmpbuf, pattern))
		return NULL;

	if (pattern[0] != '\0') {
		strcpy (def, pattern);
	} else {
		if (def[0]) {
			strcpy (pattern, def);
		} else {
			info_message (txt_no_search_string);
			return NULL;
		}
	}

	wait_message (txt_searching);

	if (wildcard)			/* ie, not wildmat() */
		return(def);
		
	/*
	 * A gross hack to simulate substrings with wildmat()
	 */
	sprintf(tmpbuf, "*%s*", pattern);
	return(tmpbuf);
}

/*
 *  Called by group.c & page.c
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

	if (!(buf = get_search_pattern(
				forward,
				txt_author_search_forwards,
				txt_author_search_backwards,
				default_author_search
	))) return -1;

    if (read_news_via_nntp && active[the_index].type == GROUP_TYPE_NEWS)
		make_group_path (active[the_index].name, group_path);

	i = current_art;

	do {
		if (forward) {
			i = next_response (i);
			if (i < 0)
				i = base[0];
		} else {
			i = prev_response (i);
			if (i < 0)
				i = base[top_base - 1] +
					num_of_responses (top_base - 1);
		}

		if (active[the_index].attribute->show_only_unread &&
		    arts[i].status != ART_UNREAD) {
			continue;
		}

		if (arts[i].name == (char *) 0) {
			strcpy (buf2, arts[i].from);
		} else {
			sprintf (buf2, "%s <%s>", arts[i].name, arts[i].from);
		}

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

	info_message ((msg[0] == '\0') ? txt_no_match : msg);
	return -1;
}

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

	if (!(buf = get_search_pattern(
				forward,
				txt_search_forwards,
				txt_search_backwards,
				default_group_search
	))) return;

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
		if (show_description && active[my_group[i]].description) {
			sprintf (buf2, "%s %s", active[my_group[i]].name,
				active[my_group[i]].description);
		} else {
			strcpy (buf2, active[my_group[i]].name);
		}

		if (REGEX_MATCH (buf2, buf, TRUE)) {
			HpGlitch(erase_group_arrow ());
			if (i >= first_group_on_screen && i < last_group_on_screen) {
				clear_message ();
				erase_group_arrow ();
				cur_groupnum = i;
				draw_group_arrow ();
			} else {
				cur_groupnum = i;
				show_selection_page ();
			}
			return;
		}
	} while (i != cur_groupnum);

	info_message ((msg[0] == '\0') ? txt_no_match : msg);
}

/*
 * Called by group.c
 * Search for a Subject line in the current group
 */
void
search_subject (
	int forward)
{
	char *buf;
	int i, j;
	int found = FALSE;

	if (index_point < 0) {
		info_message (txt_no_arts);
		return;
	}

	if (!(buf = get_search_pattern(
				forward,
				txt_search_forwards,
				txt_search_backwards,
				default_subject_search
	))) return;

	i = index_point;						/* Search from current position */

	do {
		(forward) ? i++ : i--;

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
			 * We must descend the rest of the thread in these cases
			 * TODO - optimise when subject is constant (use ptr into hash ?)
			 */
			int art;

			for (art = j ; art >= 0 ; art = arts[art].thread) {

				if (REGEX_MATCH(arts[art].subject, buf, TRUE)) {
					found = TRUE;
					break;
				}
			}
		}

	} while (i != index_point && !found);

	if (found) {
		HpGlitch(erase_subject_arrow());

		if (i >= first_subj_on_screen && i < last_subj_on_screen) {
			clear_message ();
			erase_subject_arrow ();
			index_point = i;
			draw_subject_arrow ();
		} else {
			index_point = i;
			show_group_page ();
		}
	} else
		info_message ((msg[0] == '\0') ? txt_no_match : msg);
}

/*
 *  page.c (search current article body)
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
	int orig_note_end;
	int orig_note_page;

	if (!(pattern = get_search_pattern(
				forward,
				txt_search_forwards,
				txt_search_backwards,
				default_art_search
	))) return FALSE;
	
	/*
	 *  save current position in article
	 */
	orig_note_end = note_end;
	orig_note_page = note_page;

	while (!note_end) {
		note_line = 1;
		ctrl_L = FALSE;

		if (note_page == 0) {
			note_line += 4;
		} else {
			note_line += 2;
		}
		while (note_line < cLINES) {
			if (fgets (buf, sizeof buf, note_fp) == NULL) {
				note_end = TRUE;
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

					while (i++ < j) {
						*q++ = ' ';
					}
				} else if (((*p) & 0xFF) < ' ') {
					*q++ = '^';
					*q++ = ((*p) & 0xFF) + '@';
				} else {
					*q++ = *p;
				}
			}
			*q = '\0';

			if (REGEX_MATCH (buf2, pattern, TRUE)) {
				fseek (note_fp, note_mark[note_page], 0);
				return TRUE;
			}

			note_line += ((int) strlen(buf2) / cCOLS) + 1;

			if (ctrl_L) {
				break;
			}
		}
		if (!note_end) {
			note_mark[++note_page] = ftell (note_fp);
		}
	}

	note_end = orig_note_end;
	note_page = orig_note_page;
	fseek (note_fp, note_mark[note_page], 0);
	info_message ((msg[0] == '\0') ? txt_no_match : msg);
	return FALSE;
}


/*
 * Search article body. Used only by search_body()
 */
static int
search_art_body (
	char *group_path,
	struct t_article *art,
	char *pat)
{
	char buf[LEN];
	FILE *fp;

	fp = open_art_fp (group_path, art->artnum);

	if (fp == (FILE *) 0) {
		return FALSE;
	}

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		if (buf[0] == '\n') {
			break;
		}
	}

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		if (REGEX_MATCH (buf, pat, TRUE)) {
			fclose (fp);
			return TRUE;
		}
	}

	fclose (fp);
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
	char *buf;
	char buf2[LEN];
	char group_path[PATH_LEN];
	char pat[LEN];
	char temp[20];
	int aborted = FALSE;
	int art_cnt = 0, i;
	int count = 0;

	if (!(buf = get_search_pattern(
				1,
				txt_search_body,
				txt_search_body,
				default_art_search
	))) return -1;

	make_group_path (group->name, group_path);

	if (group->attribute->show_only_unread) {
		for (i = 0 ; i < top_base ; i++) {
			art_cnt += new_responses (i);
		}
	} else {
		for (i = 0 ; i < top ; i++) {
			if (!IGNORE_ART(i)) {
				art_cnt++;
			}
		}
	}

	i = current_art;

	do {
		i = next_response (i);
		if (i < 0) {
			i = base[0];
		}

		if (group->attribute->show_only_unread && arts[i].status == ART_READ) {
			continue;
		}

		if (search_art_body (group_path, &arts[i], pat)) {
			return i;
		}

		if (count % MODULO_COUNT_NUM == 0) {
			if (input_pending ()) {
				if (read (STDIN_FILENO, buf2, sizeof (buf2)-1)) {
					if (buf2[0] == ESC || buf2[0] == 'q' || buf2[0] == 'Q') {
						if (prompt_yn (cLINES, txt_abort_searching, TRUE) == 1) {
							aborted = TRUE;
							break;
						} else {
							show_progress(temp, txt_searching_body, 0, art_cnt);
						}
					}
				}
			}
			show_progress(temp, txt_searching_body, count, art_cnt);
		}
		count++;
	} while (i != current_art);

	if (!aborted) {
		info_message ((msg[0] == '\0') ? txt_no_match : msg);
	}

	return -1;
}
