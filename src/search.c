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

/*
 * last search patterns
 */

char default_author_search[LEN];
char default_group_search[LEN];
char default_subject_search[LEN];
char default_art_search[LEN];


/*
 *  group.c & page.c
 */

int
search_author (the_index, current_art, forward)
	int the_index;
	int current_art;
	int forward;
{
	char buf[LEN];
	char buf2[LEN];
	char group_path[PATH_LEN];
	int i;

	clear_message ();

	if (forward) {
		sprintf (buf2, txt_author_search_forwards, default_author_search);
	} else {
		sprintf (buf2, txt_author_search_backwards, default_author_search);
	}

	if (!prompt_string (buf2, buf)) {
		return -1;
	}

	if (strlen (buf)) {
		strcpy (default_author_search, buf);
	} else {
		if (default_author_search[0]) {
			strcpy (buf, default_author_search);
		} else {
			info_message (txt_no_search_string);
			return -1;
		}
	}

	wait_message (txt_searching);

	make_group_path (active[the_index].name, group_path);

	str_lwr (default_author_search, buf);

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
			str_lwr (arts[i].from, buf2);
		} else {
			sprintf (msg, "%s <%s>", arts[i].name, arts[i].from);
			str_lwr (msg, buf2);
		}

		if (strstr (buf2, buf) != 0) {
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
 * select.c
 */

void
search_group (forward)
	int forward;
{
	char buf[LEN];
	char buf2[LEN];
	int i;

	if (!group_top) {
		info_message (txt_no_groups);
		return;
	}

	clear_message ();

	if (forward) {
		sprintf (buf2, txt_search_forwards, default_group_search);
	} else {
		sprintf (buf2, txt_search_backwards, default_group_search);
	}

	if (!prompt_string (buf2, buf)) {
		return;
	}

	if (strlen (buf)) {
		strcpy (default_group_search, buf);
	} else {
		if (default_group_search[0]) {
			strcpy (buf, default_group_search);
		} else {
			info_message (txt_no_search_string);
			return;
		}
	}

	wait_message (txt_searching);

	str_lwr (default_group_search, buf);

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

		if (show_description && active[my_group[i]].description) {
			sprintf (buf2, "%s %s", active[my_group[i]].name,
				active[my_group[i]].description);
		} else {
			strcpy (buf2, active[my_group[i]].name);
		}
		str_lwr (buf2, buf2);

		if (strstr (buf2, buf) != 0) {
			if (_hp_glitch) {
				erase_group_arrow ();
			}
			if (i >= first_group_on_screen
			&&  i < last_group_on_screen) {
				clear_message ();
				erase_group_arrow ();
				cur_groupnum = i;
				draw_group_arrow ();
			} else {
				cur_groupnum = i;
				group_selection_page ();
			}
			return;
		}
	} while (i != cur_groupnum);

	info_message (txt_no_match);
}

/*
 * group.c
 * Search for a Subject line in the current group
 */
void
search_subject (forward)
	int forward;
{
	char buf[LEN];
	char buf2[LEN];
	int i, j;
	int found = FALSE;

	if (index_point < 0) {
		info_message (txt_no_arts);
		return;
	}

	clear_message ();

	if (forward)
		sprintf (buf2, txt_search_forwards, default_subject_search);
	else
		sprintf (buf2, txt_search_backwards, default_subject_search);

	if (!prompt_string (buf2, buf))		/* Get search string from user */
		return;

	if (strlen (buf)) {						/* See if to use the default */
		strcpy (default_subject_search, buf);
	} else {
		if (default_subject_search[0]) {
			strcpy (buf, default_subject_search);
		} else {
			info_message (txt_no_search_string);
			return;
		}
	}

	wait_message (txt_searching);
	str_lwr (default_subject_search, buf);

	i = index_point;						/* Search from current position */

	do {
		(forward) ? i++ : i--;

		if (i >= top_base)
			i = 0;

		if (i < 0)
			i = top_base - 1;

		j = (int) base[i];				/* Get index in arts[] of thread root */

		/*
		 * With threading on References, Subject lines can change mid thread.
		 * We must descend the rest of the thread in these cases
		 * TODO - optimise when subject is constant (use ptr into hash ?)
		 */
#ifdef HAVE_REF_THREADING
		if (CURR_GROUP.attribute->thread_arts < THREAD_REFS) {
			str_lwr (arts[j].subject, buf2);
			if (strstr(buf2, buf) != 0) {
				found = TRUE;
				break;
			}
		} else {
			int art;

			for (art = j ; art >= 0 ; art = arts[art].thread) {

				str_lwr (arts[art].subject, buf2);
				if (strstr(buf2, buf) != 0) {
					found = TRUE;
					goto found_something;		/* I know... I know !!*/
				}
			}
		}
#else
		str_lwr (arts[j].subject, buf2);
		if (strstr(buf2, buf) != 0) {
			found = TRUE;
			break;
		}
#endif

	} while (i != index_point);

found_something:
	if (found) {
		if (_hp_glitch)
			erase_subject_arrow ();

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
		info_message (txt_no_match);
}

/*
 *  page.c (search article body)
 */

int
search_article (forward)
	int forward;
{
	char buf[LEN];
	char buf2[LEN];
	char string[LEN];
	char pattern[LEN];
	char *p, *q;
	int ctrl_L;
	int i, j;
	int orig_note_end;
	int orig_note_page;

	clear_message ();

	if (forward) {
		sprintf (buf2, txt_search_forwards, default_art_search);
	} else {
		sprintf (buf2, txt_search_backwards, default_art_search);
	}

	if (!prompt_string (buf2, buf)) {
		return FALSE;
	}

	if (strlen (buf)) {
		strcpy (default_art_search, buf);
	} else {
		if (default_art_search[0]) {
			strcpy (buf, default_art_search);
		} else {
			info_message (txt_no_search_string);
			return FALSE;
		}
	}

	wait_message (txt_searching);

	str_lwr (default_art_search, pattern);

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

			str_lwr (buf2, string);

			if (strstr (string, pattern) != 0) {
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
	info_message (txt_no_match);
	return FALSE;
}


int
search_body (group, current_art)
	struct t_group *group;
	int current_art;
{
	char buf2[LEN];
	char group_path[PATH_LEN];
	char pat[LEN];
	char temp[20];
	int aborted = FALSE;
	int art_cnt = 0, i;
	int count = 0;

	clear_message ();

	sprintf (buf2, txt_search_body, default_art_search);

	if (!prompt_string (buf2, pat)) {
		return -1;
	}

	if (strlen (pat)) {
		strcpy (default_art_search, pat);
	} else {
		if (default_art_search[0]) {
			strcpy (pat, default_art_search);
		} else {
			info_message (txt_no_search_string);
			return -1;
		}
	}

	make_group_path (group->name, group_path);
	str_lwr (default_art_search, pat);

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
		info_message (txt_no_match);
	}

	return -1;
}


int
search_art_body (group_path, art, pat)
	char *group_path;
	struct t_article *art;
	char *pat;
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
		str_lwr (buf, buf);
		if (strstr (buf, pat)) {
			fclose (fp);
			return TRUE;
		}
	}

	fclose (fp);
	return FALSE;
}


void
str_lwr (src, dst)
	char *src;
	char *dst;
{
	while (*src) {
		*dst++ = (char)tolower((int)*src);
		src++;
	}
	*dst = '\0';
}
