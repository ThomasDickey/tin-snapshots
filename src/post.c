/*
 *  Project   : tin - a Usenet reader
 *  Module    : post.c
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 22-08-95
 *  Notes     : mail/post/replyto/followup/repost & delete articles
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"menukeys.h"
#include	"patchlev.h"

#define	PRINT_LF()	{Raw (FALSE); my_fputc ('\n', stdout); fflush (stdout); Raw (TRUE);}

#define	MAX_MSG_HEADERS	20

/* which keys are allowed for posting/sending? */
#ifdef	HAVE_PGP
#	ifdef	HAVE_ISPELL
#		define POST_KEYS	"\033egipq"
#		define SEND_KEYS	"\033egiqs"
#	else
#		define POST_KEYS	"\033egpq"
#		define SEND_KEYS	"\033egqs"
#	endif
#else
#	ifdef   HAVE_ISPELL
#		define POST_KEYS	"\033eipq"
#		define SEND_KEYS	"\033eiqs"
#	else
#		define POST_KEYS	"\033epq"
#		define SEND_KEYS	"\033eqs"
#	endif
#endif
#define EDIT_KEYS	"\033eq"

char found_newsgroups[HEADER_LEN];

int unlink_article = TRUE;
int keep_dead_articles = TRUE;
int keep_posted_articles = TRUE;
int reread_active_for_posted_arts = FALSE;
struct t_posted *posted;

struct msg_header
{
	char	*name;
	char	*text;
} msg_headers[MAX_MSG_HEADERS];

void
msg_init_headers ()
{
	int	i;

	for (i = 0; i < MAX_MSG_HEADERS; i++)
	{
		msg_headers[i].name = (char *) 0;
		msg_headers[i].text = (char *) 0;
	}
}

void
msg_free_headers ()
{
	int	i;

	for (i = 0; i < MAX_MSG_HEADERS; i++)
	{
		if (msg_headers[i].name) {
			free (msg_headers[i].name);
			msg_headers[i].name = (char *) 0;
		}
		if (msg_headers[i].text) {
			free (msg_headers[i].text);
			msg_headers[i].text = (char *) 0;
		}
	}
}

void
msg_add_header (name, text)
	char	*name;
	char	*text;
{
	char	*ptr;
	char	*new_name = (char *) 0;
	char	*new_text = (char *) 0;
	int		done = FALSE;
	int		i;

	if (name) {
		/*
		 * Remove : if one is attached to name
		 */
		new_name = str_dup (name);
		ptr = strchr (new_name, ':');
		if (ptr) {
			*ptr = '\0';
		}

		/*
		 * Check if header already exists and if update text
		 */
		for (i = 0; i < MAX_MSG_HEADERS && msg_headers[i].name; i++)
		{
			if (STRCMPEQ (msg_headers[i].name, new_name)) {
				if (msg_headers[i].text) {
					free (msg_headers[i].text);
					msg_headers[i].text = (char *) 0;
				}
				if (text) {
					for (ptr = text; *ptr && (*ptr == ' ' || *ptr == '\t'); ptr++) {
						;
					}
					new_text = str_dup (ptr);
					ptr = strchr (new_text, '\n');
					if (ptr) {
						*ptr = '\0';
					}
					msg_headers[i].text = str_dup (new_text);
				}
				done = TRUE;
			}
		}


		/*
		 * if header does not exist then add it
		 */
		if (! done && ! msg_headers[i].name) {
			msg_headers[i].name = str_dup (new_name);
			if (text) {
				for (ptr = text; *ptr && (*ptr == ' ' || *ptr == '\t'); ptr++) {
					;
				}
				new_text = str_dup (ptr);
				ptr = strchr (new_text, '\n');
				if (ptr) {
					*ptr = '\0';
				}
				msg_headers[i].text = str_dup (new_text);
			}
		}

		if (new_name) {
			free (new_name);
		}
		if (new_text) {
			free (new_text);
		}
	}
}

int
msg_write_headers (fp)
	FILE	*fp;
{
	int	i;
	int	wrote = 1;

	for (i = 0; i < MAX_MSG_HEADERS; i++) {
		if (msg_headers[i].name) {
			fprintf (fp, "%s: %s\n",
				msg_headers[i].name,
				(msg_headers[i].text ? msg_headers[i].text : ""));
			wrote++;
		}
	}
	fputc ('\n', fp);

	return wrote;
}


int
user_posted_messages ()
{
	char buf[LEN];
	FILE *fp;
	int i = 0, j, k;
	int no_of_lines = 0;

	if ((fp = fopen (posted_info_file, "r")) == (FILE *) 0) {
		clear_message ();
		return FALSE;
	} else {
		while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
			no_of_lines++;
		}
		if (! no_of_lines) {
			fclose (fp);
			info_message (txt_no_arts_posted);
			return FALSE;
		}
		rewind (fp);
		posted = (struct t_posted *) my_malloc ((no_of_lines+1) * sizeof (struct t_posted));
		while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
			if (buf[0] == '#' || buf[0] == '\n') {
				continue;
			}
			for (j = 0 ; buf[j] != '|' && buf[j] != '\n' ; j++) {
				posted[i].date[j] = buf[j];		/* posted date */
			}
			if (buf[j] == '\n') {
				error_message ("Corrupted file %s", posted_info_file);
				sleep (1);
				fclose (fp);
				clear_message ();
				return FALSE;
			}
			posted[i].date[j++] = '\0';
			posted[i].action = buf[j];
			j += 2;
			for (k = j, j = 0 ; buf[k] != '|' && buf[k] != ',' ; k++, j++) {
				if (j < sizeof (posted[i].group)) {
					posted[i].group[j] = buf[k];
				}
			}
			if (buf[k] == ',') {
				while (buf[k] != '|' && buf[k] != '\n') {
					k++;
				}
				posted[i].group[j++] = ',';
				posted[i].group[j++] = '.';
				posted[i].group[j++] = '.';
				posted[i].group[j++] = '.';
			}
			posted[i].group[j] = '\0';
			k++;
			for (j = k, k = 0 ; buf[j] != '\n' ; j++, k++) {
				if (k < sizeof (posted[i].subj)) {
					posted[i].subj[k] = buf[j];
				}
			}
			posted[i].subj[k] = '\0';
			i++;
		}
		fclose (fp);

		show_info_page (POST_INFO, (char **) 0, txt_post_history_menu);
		if (posted != (struct t_posted *) 0) {
			free ((char *) posted);
			posted = (struct t_posted *) 0;
		}
		return TRUE;
	}
}


void
update_posted_info_file (group, action, subj)
	char *group;
	int action;
	char *subj;
{
	char buf[LEN];
	char tmp_post[LEN];
	FILE *fp, *tmp_fp;
	time_t epoch;
	struct tm *tm;

	sprintf (tmp_post, "%s.%d", posted_info_file, process_id);

	if ((tmp_fp = fopen (tmp_post, "w")) != NULL) {
		time (&epoch);
		tm = localtime (&epoch);
		fprintf (tmp_fp, "%02d-%02d-%02d|%c|%s|%s\n",
			tm->tm_mday, tm->tm_mon+1, tm->tm_year,
			action, group, subj);
		fclose (tmp_fp);
	}

	if ((tmp_fp = fopen (tmp_post, "a+")) != NULL) {
		int fMove = 0;
		if ((fp = fopen (posted_info_file, "r")) != NULL) {
			fMove = 1;
			while (fgets (buf, sizeof buf, fp) != NULL) {
				fprintf (tmp_fp, "%s", buf);
			}
			fclose (fp);
		}
		fclose (tmp_fp);
		/* Win32 -- can't move a file which is open */
		if (fMove)
			rename_file (tmp_post, posted_info_file);
	}
}

void
update_posted_msgs_file (file, addr)
	char *file;
	char *addr;
{
	char buf[LEN];
	FILE *fp_in, *fp_out;
	time_t epoch;

	fp_in = fopen (file, "r");
	if (fp_in != (FILE *) 0) {
		if (! strfpath (posted_msgs_file, buf, sizeof (buf),
		    homedir, (char *) 0, (char *) 0, (char *) 0)) {
			strcpy (buf, posted_msgs_file);
		}
		fp_out = fopen (buf, "a+");
		if (fp_out != (FILE *) 0) {
		 	time (&epoch);
		 	fprintf (fp_out, "From %s %s", addr, ctime (&epoch));
			while (fgets (buf, sizeof buf, fp_in) != (char *) 0) {
				fputs (buf, fp_out);
			}
			print_art_seperator_line (fp_out, FALSE);
			fclose (fp_out);
		}
		fclose (fp_in);
	}
}

/*
 * Check the article file for correct header syntax and if there
 * is a blank between the header information and the text.
 *
 * 1.  Subject header present
 * 2.  Newsgroups header present
 * 3.  Space after every colon in header
 * 4.  Colon in every header line
 * 5.  Newsgroups line has no spaces, only comma separated
 * 6.  List of newsgroups is presented to user with description
 * 7.  Lines in body that are to long causes a warning to be printed
 * 8.  Group(s) must be listed in the active file
 * 9.  No From: header allowed (limit accidental forging) and
 *     rejection by inn servers
 * 10  Display an 'are you sure' message before posting article
 */

int
check_article_to_be_posted (the_article, art_type, lines)
	char *the_article;
	int  art_type;
	int  *lines;
{
	char *ngptrs[NGLIMIT];
	char line[HEADER_LEN], *cp, *cp2;
	FILE *fp;
	int cnt = 0;
	int col, len, i;
	int end_of_header = FALSE;
	int errors = 0;
	int found_newsgroups_line = FALSE;
	int found_subject_line = FALSE;
	int found_followup_to = FALSE;
	int found_followup_to_several_groups = FALSE;
	int got_long_line = FALSE;
	int init = TRUE;
	int ngcnt = 0;
	size_t nglens[NGLIMIT];
	int oldraw;	/* save previous raw state */
	struct t_group *psGrp;

	if ((fp = fopen (the_article, "r")) == (FILE *) 0) {
		perror_message (txt_cannot_open, the_article);
		return FALSE;
	}

	oldraw = RawState();	/* save state */

	while (fgets (line, sizeof (line), fp) != NULL) {
		cnt++;
		len = strlen (line);
		if (len > 0) {
			if (line[len - 1] == '\n') {
				line[--len] = 0;
			}
		}
		if ((cnt == 1) && (len == 0)) {
			setup_check_article_screen (&init);
			fprintf (stderr, txt_error_header_line_blank);
			fflush (stderr);
			errors++;
			end_of_header = TRUE;
			break;
		}
		if ((len == 0) && (cnt >= 2)) {
			end_of_header = TRUE;
			*lines = 0;
			break;
		}
		/*
		 * ignore continuation lines - they start with white space
		 */
		if ((line[0] == ' ' || line[0] == '\t') && (cnt != 1)) {
			continue;
		}
		cp = strchr (line, ':');
		if (cp == (char *) 0) {
			setup_check_article_screen (&init);
			fprintf (stderr, txt_error_header_line_colon, cnt, line);
			fflush (stderr);
			errors++;
			continue;
		}
		if (cp[1] != ' ') {
			setup_check_article_screen (&init);
			fprintf (stderr, txt_error_header_line_space, cnt, line);
			fflush (stderr);
			errors++;
		}
		if (cp - line == 7 && ! strncasecmp (line, "Subject", 7)) {
			found_subject_line = TRUE;
		}
#ifndef FORGERY
		if (cp - line == 4 && ! strncasecmp (line, "From", 4)) {
			fprintf (stderr, txt_error_from_in_header_not_allowed, cnt);
			fflush (stderr);
			errors++;
		}
#endif
		if (cp - line == 10 && ! strncasecmp (line, "Newsgroups", 10)) {
			found_newsgroups_line = TRUE;
			for (cp = line + 11; *cp == ' '; cp++) {
				;
			}
			if (strchr (cp, ' ')) {
				setup_check_article_screen (&init);
				fprintf (stderr, txt_error_header_line_comma);
				fflush (stderr);
				errors++;
				continue;
			}
			strip_double_ngs (cp);
			while (*cp) {
				if (! (cp2 = strchr (cp, ','))) {
					cp2 = cp + strlen (cp);
				} else {
					*cp2++ = '\0';
				}
				if (ngcnt < NGLIMIT) {
					nglens[ngcnt] = strlen (cp);
					ngptrs[ngcnt] = my_malloc (nglens[ngcnt]+1);
					if (! ngptrs[ngcnt]) {
						for (i = 0; i < ngcnt; i++) {
							if (ngptrs[i]) {
								free (ngptrs[i]);
							}
						}
						Raw (oldraw);
						return TRUE;
					}
					strcpy (ngptrs[ngcnt], cp);
					ngcnt++;
				}
				cp = cp2;
			}
			if (! ngcnt) {
				setup_check_article_screen (&init);
				fprintf (stderr, txt_error_header_line_empty_newsgroups);
				fflush (stderr);
				errors++;
				continue;
			}
		}
		if (cp - line == 11 && ! strncasecmp (line, "Followup-To", 11)) {
			found_followup_to = 1;
			if (strchr(cp, ',')) found_followup_to_several_groups = 1;
		}
	}

	if (! found_subject_line) {
		setup_check_article_screen (&init);
		fprintf (stderr, txt_error_header_line_missing_subject);
		fflush (stderr);
		errors++;
	}

	if (! found_newsgroups_line && art_type == GROUP_TYPE_NEWS) {
		setup_check_article_screen (&init);
		fprintf (stderr, txt_error_header_line_missing_newsgroups);
		fflush (stderr);
		errors++;
	}

	/*
	 * Check the body of the article for long lines
	 */
	while (fgets (line, sizeof (line), fp)) {
		(*lines)++;
		cnt++;
		cp = strrchr (line, '\n');
		if (cp != (char *) 0) {
			*cp = '\0';
		}
		col = 0;
		for (cp = line; *cp; cp++) {
			if (*cp == '\t') {
				col += 8 - (col%8);
			} else {
				col++;
			}
		}
		if (col > MAX_COL && !got_long_line) {
			setup_check_article_screen (&init);
			fprintf (stderr, txt_warn_art_line_too_long, MAX_COL, cnt, line);
			fflush (stderr);
			got_long_line = TRUE;
		}
	}
	if (! end_of_header) {
		setup_check_article_screen (&init);
		fprintf (stderr, txt_error_header_and_body_not_seperate);
		fflush (stderr);
		errors++;
	}

	if (ngcnt && errors == 0) {
		/*
		 * Print a note about each newsgroup
		 */
		setup_check_article_screen (&init);
		fprintf (stderr, txt_art_newsgroups, ngcnt == 1 ? "" : "s");
		fflush (stderr);
		for (i = 0; i < ngcnt; i++) {
			psGrp = psGrpFind (ngptrs[i]);
			if (psGrp) {
				fprintf (stderr, "  %s\t%s\n", ngptrs[i],
					 (psGrp->description ? psGrp->description : ""));
				fflush (stderr);
			} else {
#ifdef HAVE_FASCIST_NEWSADMIN
				fprintf (stderr, txt_error_not_valid_newsgroup, ngptrs[i]);
				errors++;
#else
				fprintf (stderr, txt_warn_not_valid_newsgroup, ngptrs[i]);
#endif
			}
			free (ngptrs[i]);
		}
		if (!found_followup_to && ngcnt > 1 && !errors) {
#ifdef HAVE_FASCIST_NEWSADMIN
			fprintf(stderr, txt_error_missing_followup_to, ngcnt);
			errors++;
#else
			fprintf(stderr, txt_warn_missing_followup_to, ngcnt);
#endif
		}
		if (found_followup_to_several_groups && !errors) {
#ifdef HAVE_FASCIST_NEWSADMIN
			fprintf(stderr, txt_error_followup_to_several_groups);
			errors++;
#else
			fprintf(stderr, txt_warn_followup_to_several_groups);
#endif
		}
#ifndef NO_ETIQUETTE
		fprintf (stderr, txt_warn_posting_etiquette);
#endif
		fflush (stderr);
	}
	fclose (fp);

	Raw (oldraw);	/* restore raw/unraw state */

	return (errors ? FALSE : TRUE);
}


void
setup_check_article_screen (init)
	int *init;
{
	if (*init) {
		ClearScreen ();
		center_line (0, TRUE, txt_check_article);
		MoveCursor (INDEX_TOP, 0);
		Raw(FALSE);
		*init = FALSE;
	}
}

/*
 *  Quick post an article (not a followup)
 */

void
quick_post_article ()
{
	FILE	*fp;
	char	ch, *ptr;
	char	ch_default = iKeyPostPost;
	char	group[HEADER_LEN];
	char	subj[HEADER_LEN];
	char	buf[HEADER_LEN], tmp[HEADER_LEN];
	int	art_type = GROUP_TYPE_NEWS;
	int	done = FALSE;
	int	lines;
	struct	t_group *psGrp;
#ifdef FORGERY
	char 	from_name[HEADER_LEN];
	char 	line[HEADER_LEN];
#endif

	msg_init_headers ();

	if (! can_post) {
		info_message (txt_cannot_post);
		return;
	}

	setup_screen ();
	InitScreen ();
	ClearScreen ();

	/*
	 * Get groupname & subject for posting article.
	 * If multiple newsgroups test all to see if any are moderated.
	 */
	sprintf (buf, txt_post_newsgroups, default_post_newsgroups);

	if (! prompt_string (buf, group)) {
		fprintf (stderr, "%s\n", txt_no_quick_newsgroups);
		return;
	}

	if (strlen (group)) {
		my_strncpy (default_post_newsgroups, group,
			sizeof (default_post_newsgroups));
	} else {
		if (default_post_newsgroups[0]) {
			my_strncpy (group, default_post_newsgroups, sizeof (group));
		} else {
			fprintf (stderr, "%s\n", txt_no_quick_newsgroups);
			return;
		}
	}

	/*
	 * Strip double newsgroups
	 */
	strip_double_ngs(group);

	/*
	 * Check if any of the newsgroups are moderated.
	 */
	strcpy (tmp, group);
	while (! done) {
		strcpy (buf, tmp);
		ptr = strchr (buf, ',');
		if (ptr != (char *) 0) {
			strcpy (tmp, ptr+1);
			*ptr = '\0';
		} else {
			done = TRUE;
		}
		psGrp = psGrpFind (buf);

		if (debug == 2) {
			sprintf (msg, "Group=[%s]", buf);
			wait_message (msg);
		}

		if (! psGrp) {
			Raw(FALSE);
			fprintf (stderr, "\nGroup %s not found in active file. Exiting...\n", buf);
			return;
		}
		if (psGrp->moderated == 'm') {
			sprintf (msg, txt_group_is_moderated, buf);
			if (prompt_yn (cLINES, msg, TRUE) != 1) {
				Raw(FALSE);
				fprintf (stderr, "\nExiting...\n");
				return;
			}
		}
	}

	PRINT_LF();
	sprintf (buf, txt_post_subject, default_post_subject);

	if (! prompt_string (buf, subj)) {
		Raw (FALSE);
		fprintf (stderr, "%s\n", txt_no_quick_subject);
		return;
	}

	if (strlen (subj)) {
		my_strncpy (default_post_subject, subj,
			sizeof (default_post_subject));
	} else {
		if (default_post_subject[0]) {
			my_strncpy (subj, default_post_subject, sizeof (subj));
		} else {
			Raw (FALSE);
			fprintf (stderr, "%s\n", txt_no_quick_subject);
			return;
		}
	}

	PRINT_LF();

	if ((fp = fopen (article, "w")) == NULL) {
		Raw (FALSE);
		perror_message (txt_cannot_open, article);
		return;
	}
	chmod (article, 0600);

/* FIXME so that group only contains 1 group when finding an index number */
/* Does this count? */
	strcpy (buf, group);
	ptr = strchr (buf, ',');
	if (ptr) {
		*ptr = '\0';
	}
	psGrp = psGrpFind (group);

#ifdef FORGERY
	make_path_header (line, from_name);
	msg_add_header ("Path", line);
	msg_add_header ("From", from_name);
#endif
	msg_add_header ("Subject", subj);
	msg_add_header ("Newsgroups", group);
	if (psGrp && psGrp->attribute->organization != (char *) 0) {
		msg_add_header ("Organization", psGrp->attribute->organization);
	}
	if (*reply_to) {
		msg_add_header ("Reply-To", reply_to);
	}
	if (psGrp && psGrp->attribute->followup_to != (char *) 0) {
		msg_add_header ("Followup-To", psGrp->attribute->followup_to);
	}
	if (*my_distribution) {
		msg_add_header ("Distribution", my_distribution);
	}
	msg_add_x_headers (msg_headers_file);
	if (psGrp) {
		msg_add_x_headers (psGrp->attribute->x_headers);
	}

	msg_add_header ("Summary", "");
	msg_add_header ("Keywords", "");

	start_line_offset = msg_write_headers (fp);
	fprintf(fp, "\n"); /* add a newline to keep vi from bitching */
	start_line_offset += 2;
	msg_free_headers ();
	if (psGrp) {
		start_line_offset += msg_add_x_body (fp, psGrp->attribute->x_body);
	}
	msg_write_signature (fp, FALSE);
	fclose (fp);

	ch = iKeyPostEdit;
	forever {
		switch (ch) {
		case iKeyPostEdit:
			invoke_editor (article, start_line_offset);
			while (! check_article_to_be_posted (article, art_type, &lines)) {
				do {
					sprintf (msg, "%s%c", txt_bad_article, iKeyPostEdit);
					wait_message (msg);
					MoveCursor (cLINES, (int) strlen (txt_bad_article));
					if ((ch = (char) ReadCh ()) == '\r' || ch == '\n')
						ch = iKeyPostEdit;
				} while (! strchr (EDIT_KEYS, ch));
				if (ch == iKeyPostEdit) {
					invoke_editor (article, start_line_offset);
				} else {
					break;
				}
			}
			if (ch == iKeyPostEdit) {
				break;
			}

		case iKeyPostQuit:
		case iKeyPostQuit2:
			if (unlink_article)
				unlink (article);
			clear_message ();
			return;

#ifdef HAVE_ISPELL
		case iKeyPostIspell:
			invoke_ispell (article);
			break;
#endif

#ifdef HAVE_PGP
		case iKeyPostPGP:
		        invoke_pgp_news (article);
			break;
#endif

		case iKeyPostPost:
			wait_message (txt_posting);
			if (submit_news_file (article, lines)) {
				Raw (FALSE);
				info_message (txt_art_posted);
				goto post_article_done;
			} else {
				rename_file (article, dead_article);
#ifdef M_UNIX
				if (keep_dead_articles)
					append_file (dead_articles, dead_article);
#endif
				Raw (FALSE);
				error_message (txt_art_rejected, dead_article);
				ReadCh();
				return;
			}
		}

		do {
			sprintf (msg, "%s%c", txt_quit_edit_post, ch_default);
			wait_message (msg);
			MoveCursor (cLINES, (int) strlen (txt_quit_edit_post));
			if ((ch = (char) ReadCh ()) == '\r' || ch == '\n')
				ch = ch_default;
		} while (! strchr (POST_KEYS, ch));
	}

post_article_done:
	if (pcCopyArtHeader (HEADER_NEWSGROUPS, article, group)) {
		update_active_after_posting (group);

		if (pcCopyArtHeader (HEADER_SUBJECT, article, subj)) {
			quick_filter_select_posted_art (psGrp, subj);

			update_posted_info_file (group, 'w', subj);
		} else {
			subj[0] = '\0';
		}
	} else {
		group[0] = '\0';
	}

	if (keep_posted_articles) {
	if (psGrp->attribute->auto_save_msg) {
		update_posted_msgs_file (article, userid);
	}
	}

	if (unlink_article) {
		unlink (article);
	}

	if (group[0] != '\0')
		my_strncpy (default_post_newsgroups, group, sizeof (default_post_newsgroups));
	if (subj[0] != '\0')
		my_strncpy (default_post_subject, subj, sizeof (default_post_subject));

	write_config_file (local_config_file);

	return;
}

/*
 *  Post an original article (not a followup)
 */

int
post_article (group, posted_flag)
	char	*group;
	int	*posted_flag;
{
	FILE	*fp;
	char	ch;
	char	ch_default = iKeyPostPost;
	char	subj[HEADER_LEN];
	char	buf[HEADER_LEN];
	int	art_type = GROUP_TYPE_NEWS;
	int	lines;
	int	redraw_screen = FALSE;
	struct	t_group *psGrp;
#ifdef FORGERY
	char	from_name[HEADER_LEN];
	char	line[HEADER_LEN];
#endif

	msg_init_headers ();

	psGrp = psGrpFind (group);
	if (! psGrp) {
		error_message (txt_not_in_active_file, group);
		return redraw_screen;
	}
	if (psGrp->attribute->mailing_list != (char *) 0) {
		art_type = GROUP_TYPE_MAIL;
	}

	if (! can_post && art_type == GROUP_TYPE_NEWS) {
		info_message (txt_cannot_post);
		return redraw_screen;
	}

	*posted_flag = FALSE;

	if (psGrp->moderated == 'm') {
		sprintf (msg, txt_group_is_moderated, group);
		if (prompt_yn (cLINES, msg, TRUE) != 1) {
			clear_message ();
			return redraw_screen;
		}
	}

	sprintf (msg, txt_post_subject, default_post_subject);

	if (! prompt_string (msg, subj)) {
		clear_message ();
		return redraw_screen;
	}

	if (strlen (subj)) {
		my_strncpy (default_post_subject, subj,
			sizeof (default_post_subject));
	} else {
		if (default_post_subject[0]) {
			my_strncpy (subj, default_post_subject, sizeof (subj));
		} else {
			info_message (txt_no_subject);
			return redraw_screen;
		}
	}

	wait_message (txt_post_an_article);

	if ((fp = fopen (article, "w")) == (FILE *) 0) {
		perror_message (txt_cannot_open, article);
		return redraw_screen;
	}
	chmod (article, 0600);

#ifdef FORGERY
	make_path_header (line, from_name);
	msg_add_header ("Path", line);
	msg_add_header ("From", from_name);
#endif
	msg_add_header ("Subject", subj);
	if (art_type == GROUP_TYPE_MAIL) {
		msg_add_header ("To", psGrp->attribute->mailing_list);
	} else {
		msg_add_header ("Newsgroups", group);
	}
	if (psGrp->attribute->organization != (char *) 0) {
		msg_add_header ("Organization", psGrp->attribute->organization);
	}
	if (*reply_to) {
		msg_add_header ("Reply-To", reply_to);
	}
	if (psGrp->attribute->followup_to != (char *) 0 && art_type == GROUP_TYPE_NEWS) {
		msg_add_header ("Followup-To", psGrp->attribute->followup_to);
	}
	if (*my_distribution && art_type == GROUP_TYPE_NEWS) {
		msg_add_header ("Distribution", my_distribution);
	}
	msg_add_x_headers (msg_headers_file);
	msg_add_x_headers (psGrp->attribute->x_headers);

	msg_add_header ("Summary", "");
	msg_add_header ("Keywords", "");

	start_line_offset = msg_write_headers (fp);
        fprintf(fp, "\n"); /* add a newline to keep vi from bitching */
	start_line_offset++;
	msg_free_headers ();
	lines = msg_add_x_body (fp, psGrp->attribute->x_body);
	start_line_offset += lines;
	msg_write_signature (fp, FALSE);
	fclose (fp);

	ch = iKeyPostEdit;
	forever {
		switch (ch) {
		case iKeyPostEdit:
			invoke_editor (article, start_line_offset);
			while (! check_article_to_be_posted (article, art_type, &lines)) {
				do {
					sprintf (msg, "%s%c", txt_bad_article, iKeyPostEdit);
					wait_message (msg);
					MoveCursor (cLINES, (int) strlen (txt_bad_article));
					if ((ch = (char) ReadCh ()) == '\r' || ch == '\n')
						ch = iKeyPostEdit;
				} while (! strchr (EDIT_KEYS, ch));
				if (ch == iKeyPostEdit) {
					invoke_editor (article, start_line_offset);
				} else {
					break;
				}
			}
			redraw_screen = TRUE;
			if (ch == iKeyPostEdit) {
				break;
			}

		case iKeyPostQuit:
		case iKeyPostQuit2:
			if (unlink_article)
				unlink (article);
			clear_message ();
			return redraw_screen;

#ifdef HAVE_ISPELL
		case iKeyPostIspell:
			invoke_ispell (article);
			break;
#endif

#ifdef HAVE_PGP
		case iKeyPostPGP:
		        invoke_pgp_news (article);
			break;
#endif

		case iKeyPostPost:
			wait_message (txt_posting);
			if (art_type == GROUP_TYPE_NEWS) {
				if (submit_news_file (article, lines)) {
					*posted_flag = TRUE;
				}
			} else {
				if (submit_mail_file (article)) {
					*posted_flag = TRUE;
				}
			}
			if (*posted_flag) {
				info_message (txt_art_posted);
				sleep(1);
				goto post_article_done;
			} else {
				rename_file (article, dead_article);
#ifdef M_UNIX
				if (keep_dead_articles)
					append_file (dead_articles, dead_article);
#endif
				sprintf (buf, txt_art_rejected, dead_article);
				info_message (buf);
				ReadCh();
				return redraw_screen;
			}
		}

		do {
			sprintf (msg, "%s%c", txt_quit_edit_post, ch_default);
			wait_message (msg);
			MoveCursor (cLINES, (int) strlen (txt_quit_edit_post));
			if ((ch = (char) ReadCh ()) == '\r' || ch == '\n')
				ch = ch_default;
		} while (! strchr (POST_KEYS, ch));
	}

post_article_done:
	if (*posted_flag) {
		if (art_type == GROUP_TYPE_NEWS) {
			if (pcCopyArtHeader (HEADER_NEWSGROUPS, article, buf))
				update_active_after_posting (buf);
		}
		if (pcCopyArtHeader (HEADER_SUBJECT, article, subj)) {
			quick_filter_select_posted_art (psGrp, subj);
			update_posted_info_file (group, 'w', subj);
		}
		if (keep_posted_articles) {
		if (psGrp->attribute->auto_save_msg) {
			update_posted_msgs_file (article, userid);
		}
	}
	}

	if (unlink_article) {
		unlink (article);
	}

	my_strncpy (default_post_newsgroups, group, sizeof (default_post_newsgroups));
	my_strncpy (default_post_subject, subj, sizeof (default_post_subject));

	return redraw_screen;
}

/* local prototyles */
static void appendid P_((char **where, char **what));
static int must_include P_((char *id));
static void skip_id P_((char **id));
static int damaged_id P_((char *id));


/* yeah, right, that's from the same Chris who is telling Jason he's
   doing obfuscated C :-) */
static void appendid(where, what)
	char **where;
	char **what;
{
	char *oldpos;
	oldpos = *where;
	while (**what && **what!='<') (*what)++;
	if (**what) {
		while (**what && **what != '>'
		   && !isspace(**what)) *(*where)++ = *(*what)++;
		if (**what!='>') *where=oldpos;
		else {
			(*what)++;
			*(*where)++='>';
		}
	}
}


static int must_include(id)
	char *id;
{
	while (*id && *id!='<') id++;
	while (*id && *id!='>') {
		if (*++id!='_') continue;
		if (*++id!='-') continue;
		if (*++id!='_') continue;
		if (*++id=='@') return 1;
	}
	return 0;
}


static void skip_id(id)
	char **id;
{
	while (**id && isspace(**id)) (*id)++;
	if (**id) {
		while (**id && !isspace(**id)) (*id)++;
	}
}


static int damaged_id(id)
	char *id;
{
	while (*id && isspace(*id)) id++;
	if (*id!='<') return 1;
	while (*id && *id!='>') id++;
	if (*id!='>') return 1;
	return 0;
}


/* Widespread news software like INN's nnrpd restricts the size of several
   headers, notably the references header, to 512 characters.  Oh well...
   guess that's what son-of-1036 calls a "desparate last ressort" :-/
   From TIN's point of view, this could be HEADER_LEN. */
#define MAXREFSIZE 512

/* TODO - if we have the art[x] that we are following up to, then
 *        get_references(art[x].refptr) will give us the new refs line
 */

void
join_references (buffer, oldrefs, newref)
	char *buffer;
	char *oldrefs;
	char *newref;
{
	/* First of all: shortening references is a VERY BAD IDEA.
	   Nevertheless, current software usually has restrictions in
	   header length (their programmers seem to misinterpret RFC821
	   as valid for news, and the command length limit of RFC977
	   as valid for headers) */
	/* construct a new references line, then trim it if necessary */
	/* do some sanity cleanups: remove damaged ids, make
	   sure there is space between ids (tabs and commas are stripped) */
	/* note that we're not doing strict son-of-1036 here: we don't
	   take any precautions to keep the last three message ids, but
	   it's not very likely that MAXREFSIZE chars can't hold at least
	   4 refs */
	char *b,*c,*d;
	int space;
	b=(char *)malloc(strlen(oldrefs)+strlen(newref)+64);
	c=b;
	d=oldrefs;
	space=0;
	while (*d) {
		if (*d==' ') {
			space++, *c++=' ', d++; /* keep existing spaces */
			continue;
		}
		else if (*d!='<') { /* strip everything besides spaces and */
			d++;	    /* message-ids */
			continue;
		}
		if (damaged_id(d)) { /* remove damaged message ids and mark
					the gap if that's not already done */
			skip_id(&d);
			while (space<3) {
				space++, *c++=' ';
			}
			continue;
		}
		if (!space) *c+=' ';
		else space=0;
		appendid(&c,&d);
	}
	while (space) c--,space--; /* remove superfluous space at the end */
	*c++=' ';
	appendid(&c,&newref);
	*c=0;

	/* now see if we need to remove ids */
	while (strlen(b)>MAXREFSIZE-14) { /* 14 = strlen("References: ")+2 */
		c=b;
		skip_id(&c); /* keep the first one */
		while (*c && must_include(c)) skip_id(&c); /* skip those marked
							      with _-_ */
		d=c;
		skip_id(&c); /* ditch one */
		*d++=' '; *d++=' '; *d++=' '; /* and mark this appropriately */
		while (*c==' ') c++;
		strcpy(d,c);
	}

	strcpy(buffer,b);
	free(b);
	return;

	/* son-of-1036 says:
          Followup agents SHOULD not shorten References  headers.   If
          it  is absolutely necessary to shorten the header, as a des-
          perate last resort, a followup agent MAY do this by deleting
          some  of  the  message IDs.  However, it MUST not delete the
          first message ID, the last three message IDs (including that
          of  the immediate precursor), or any message ID mentioned in
          the body of the followup.  If it is possible  for  the  fol-
          lowup agent to determine the Subject content of the articles
          identified in the References header, it MUST not delete  the
          message  ID of any article where the Subject content changed
          (other than by prepending of a back  reference).   The  fol-
          lowup  agent MUST not delete any message ID whose local part
          ends with "_-_" (underscore (ASCII 95), hyphen  (ASCII  45),
          underscore);  followup  agents are urged to use this form to
          mark subject changes, and to avoid using it otherwise.
	  [...]
          When a References header is shortened, at least three blanks
          SHOULD be left between adjacent message IDs  at  each  point
          where  deletions  were  made.  Software preparing new Refer-
          ences headers SHOULD preserve multiple blanks in older  Ref-
          erences content.
	*/
}


int
post_response (group, respnum, copy_text)
	char *group;
	int respnum;
	int copy_text;
{
	FILE *fp;
	char ch, *ptr;
	char ch_default = iKeyPostPost;
	char bigbuf[HEADER_LEN];
	char buf[HEADER_LEN];
	int art_type = GROUP_TYPE_NEWS;
	int lines;
	int ret_code = POSTED_NONE;
	struct t_group *psGrp;
	char	initials[64];
#ifdef FORGERY
	char	from_name[HEADER_LEN];
	char	line[HEADER_LEN];
#endif

	msg_init_headers ();

	wait_message (txt_post_a_followup);

	/*
	 *  Remove duplicates in Newsgroups and Followup-To line
	 */
	strip_double_ngs(note_h_newsgroups);
	if (*note_h_followup) {
		strip_double_ngs(note_h_followup);
	}

	if (*note_h_followup && STRCMPEQ(note_h_followup, "poster")) {
		clear_message ();
		sprintf (msg, "%s%c", txt_resp_to_poster, iKeyPageMail);
		wait_message (msg);
		MoveCursor (cLINES, (int) strlen (txt_resp_to_poster));
		do {
			if ((ch = (char) ReadCh ()) == '\r' || ch == '\n')
				ch = iKeyPageMail;
		} while (! strchr ("\033mpq", ch));
		switch (ch) {
		case iKeyPostPost:
			goto ignore_followup_to_poster;
		case iKeyPostQuit:
		case iKeyPostQuit2:
			return ret_code;
		}
		{
			char save_followup[HEADER_LEN];
			strcpy(save_followup, note_h_followup);
			*note_h_followup = '\0';
			find_reply_to_addr (respnum, buf);
			mail_to_someone (respnum, buf, TRUE, FALSE, &ret_code);
			strcpy(note_h_followup, save_followup);
			return ret_code;
		}
	} else if (*note_h_followup && strcmp (note_h_followup, group) != 0) {
		MoveCursor (cLINES/2, 0);
		CleartoEOS ();
		center_line ((cLINES/2)+2, TRUE, txt_resp_redirect);
		MoveCursor ((cLINES/2)+4, 0);

		my_fputs ("    ", stdout);
		ptr = note_h_followup;
		while (*ptr) {
			if (*ptr != ',') {
				my_fputc (*ptr, stdout);
			} else {
				my_fputs ("\r\n    ", stdout);
			}
			ptr++;
		}
		fflush (stdout);

		if (prompt_yn (cLINES, txt_continue, TRUE) != 1) {
			return ret_code;
		}
	}
ignore_followup_to_poster:
	if ((fp = fopen (article, "w")) == NULL) {
		perror_message (txt_cannot_open, article);
		return ret_code;
	}
	chmod (article, 0600);

#ifdef FORGERY
	make_path_header (line, from_name);
	msg_add_header ("Path", line);
	msg_add_header ("From", from_name);
#endif

	psGrp = psGrpFind (group);

	sprintf (bigbuf, "Re: %s", eat_re (note_h_subj));
	msg_add_header ("Subject", bigbuf);

	if (psGrp && psGrp->attribute-> x_comment_to && *note_h_from) {
		msg_add_header ("X-Comment-To", note_h_from);
	}
	if (*note_h_followup && strcmp (note_h_followup, "poster") != 0) {
		msg_add_header ("Newsgroups", note_h_followup);
	} else {
		if (psGrp && psGrp->attribute->mailing_list) {
			msg_add_header ("To", psGrp->attribute->mailing_list);
			art_type = GROUP_TYPE_MAIL;
		} else {
			msg_add_header ("Newsgroups", note_h_newsgroups);
			if (psGrp && psGrp->attribute->followup_to != (char *) 0) {
				msg_add_header ("Followup-To", psGrp->attribute->followup_to);
			} else {
				ptr = (char *) strchr (note_h_newsgroups, ',');
				if (ptr) {
					msg_add_header ("Followup-To", note_h_newsgroups);
				}
			}
		}
	}

	/*
	 * Append to References: line if its already there
	 */
	if (art_type != GROUP_TYPE_MAIL) {
		if (note_h_references[0]) {
			join_references (bigbuf, note_h_references, note_h_messageid);
			msg_add_header ("References", bigbuf);
		} else {
			msg_add_header ("References", note_h_messageid);
		}
	}

	if (psGrp && psGrp->attribute->organization != (char *) 0) {
		msg_add_header ("Organization", psGrp->attribute->organization);
	}
	if (*reply_to) {
		msg_add_header ("Reply-To", reply_to);
	}
	if (art_type != GROUP_TYPE_MAIL) {
		if (*note_h_distrib) {
			msg_add_header ("Distribution", note_h_distrib);
		} else {
			msg_add_header ("Distribution", my_distribution);
		}
	}
	msg_add_x_headers (msg_headers_file);
	msg_add_x_headers (psGrp->attribute->x_headers);

	start_line_offset = msg_write_headers (fp);
	start_line_offset++;
	msg_free_headers ();
	lines = msg_add_x_body (fp, psGrp->attribute->x_body);
	start_line_offset += lines;

	if (copy_text) {
		if (arts[respnum].xref) {
			if (strfquote (CURR_GROUP.name, respnum, buf, sizeof (buf),
														xpost_quote_format)) {
			    	fprintf (fp, "%s\n", buf);
			}
		} else if (strfquote (group, respnum, buf, sizeof (buf),
		    (psGrp && psGrp->attribute->news_quote_format != (char *) 0) ? psGrp->attribute->news_quote_format : news_quote_format)) {
			fprintf (fp, "%s\n", buf);
			}
		start_line_offset++;

		/*
		 * check if xpost_quote_format or news_quote_format
		 * is longer than 1 line and correct start_line_offset
		 */
		{ char *s;
			for (s = buf; *s; s++) {
				if (*s == '\n') ++start_line_offset;
			}
		}

		fseek (note_fp, note_mark[0], 0);
		get_initials(respnum, initials, sizeof (initials));
		copy_body (note_fp, fp,
			(psGrp && psGrp->attribute->quote_chars != (char *) 0) ? psGrp->attribute->quote_chars : quote_chars,
			initials);
	} else {
		fprintf(fp, "\n"); /* add a newline to keep vi from bitching */
	}

	msg_write_signature (fp, FALSE);
	fclose (fp);

	ch = iKeyPostEdit;
	forever {
		switch (ch) {
		case iKeyPostEdit:
			invoke_editor (article, start_line_offset);
			while (! check_article_to_be_posted (article, art_type, &lines)) {
				do {
					sprintf (msg, "%s%c", txt_bad_article, iKeyPostEdit);
					wait_message (msg);
					MoveCursor (cLINES, (int) strlen (txt_bad_article));
					if ((ch = (char) ReadCh ()) == '\r' || ch == '\n')
						ch = iKeyPostEdit;
				} while (! strchr (EDIT_KEYS, ch));
				if (ch == iKeyPostEdit) {
					invoke_editor (article, start_line_offset);
				} else {
					break;
				}
			}
			ret_code = POSTED_REDRAW;
			if (ch == iKeyPostEdit) {
				break;
			}

		case iKeyPostQuit:
		case iKeyPostQuit2:
			if (unlink_article)
				unlink (article);
			clear_message ();
			return ret_code;

#ifdef HAVE_ISPELL
		case iKeyPostIspell:
			invoke_ispell (article);
			ret_code = POSTED_REDRAW;
			break;
#endif

#ifdef HAVE_PGP
		case iKeyPostPGP:
		        invoke_pgp_news (article);
			break;
#endif

		case iKeyPostPost:
			wait_message (txt_posting);
			if (art_type == GROUP_TYPE_NEWS) {
				if (submit_news_file (article, lines)) {
					ret_code = POSTED_OK;
				}
			} else {
				if (submit_mail_file (article)) {
					ret_code = POSTED_OK;
				}
			}
			if (ret_code == POSTED_OK) {
				info_message (txt_art_posted);
				goto post_response_done;
			} else {
				rename_file (article, dead_article);
#ifdef M_UNIX
				if (keep_dead_articles)
					append_file (dead_articles, dead_article);
#endif
				sprintf (buf, txt_art_rejected, dead_article);
				info_message (buf);
				ReadCh();
				return ret_code;
			}
		}

		do {
			sprintf (msg, "%s%c", txt_quit_edit_post, ch_default);
			wait_message (msg);
			MoveCursor(cLINES, (int) strlen (txt_quit_edit_post));
			if ((ch = (char) ReadCh()) == '\r' || ch == '\n')
				ch = ch_default;
		} while (! strchr (POST_KEYS, ch));
	}

post_response_done:
	if (ret_code == POSTED_OK) {
		if (art_type == GROUP_TYPE_NEWS) {
			if (pcCopyArtHeader (HEADER_NEWSGROUPS, article, buf))
				update_active_after_posting (buf);
		}

		if (*note_h_followup && strcmp(note_h_followup, "poster") != 0) {
			if (pcCopyArtHeader (HEADER_SUBJECT, article, buf))
				update_posted_info_file (note_h_followup, 'f', buf);
		} else if (pcCopyArtHeader (HEADER_SUBJECT, article, buf)) {
			update_posted_info_file (note_h_newsgroups, 'f', buf);
			my_strncpy (default_post_newsgroups, note_h_newsgroups,
				sizeof (default_post_newsgroups));
		}
		if (keep_posted_articles) {
		if (psGrp->attribute->auto_save_msg) {
			update_posted_msgs_file (article, userid);
		}
	}
	}

	my_strncpy (default_post_subject, buf, sizeof (default_post_subject));

	if (unlink_article) {
		unlink (article);
	}

	return ret_code;
}


int
mail_to_someone (respnum, address, mail_to_poster, confirm_to_mail, mailed_ok)
	int respnum;
	char *address;
	int mail_to_poster;
	int confirm_to_mail;
	int *mailed_ok;
{
	char nam[HEADER_LEN];
	char subject[HEADER_LEN];
	char ch = iKeyPostSend;
	char ch_default = iKeyPostSend;
	char buf[HEADER_LEN];
	char mail_to[HEADER_LEN];
	char initials[64];
	FILE *fp;
	int redraw_screen = FALSE;

	msg_init_headers ();

	strcpy (mail_to, address);
	clear_message ();

#ifdef VMS
	joinpath (nam, homedir, "letter.");
#else
	joinpath (nam, homedir, ".letter");
#endif
	if ((fp = fopen (nam, "w")) == NULL) {
		perror_message (txt_cannot_open, nam);
		return redraw_screen;
	}
	chmod (nam, 0600);

	msg_add_header ("To", mail_to);

	if (mail_to_poster) {
		sprintf (subject, "Re: %s\n", eat_re (note_h_subj));
		msg_add_header ("Subject", subject);
	} else {
		sprintf (subject, "(fwd) %s\n", note_h_subj);
		msg_add_header ("Subject", subject);
	}

	if (auto_cc) {
		msg_add_header ("Cc", userid);
	}
	if (auto_bcc) {
		msg_add_header ("Bcc", userid);
	}

	/*
	 * remove duplicates from Newsgroups header
	 */
	strip_double_ngs(note_h_newsgroups);

	msg_add_header ("Newsgroups", note_h_newsgroups);

	if (*default_organization) {
		msg_add_header ("Organization", default_organization);
	}
	if (*reply_to) {
		msg_add_header ("Reply-To", reply_to);
	}
	msg_add_x_headers (msg_headers_file);

	start_line_offset = msg_write_headers (fp);
	start_line_offset++;
	msg_free_headers ();

	if (mail_to_poster) {
		ch = iKeyPostEdit;
		if (strfquote (CURR_GROUP.name, respnum, buf, sizeof (buf),
														mail_quote_format)) {
			fprintf (fp, "%s\n", buf);
			start_line_offset++;
			{ char *s;
				for (s = buf; *s; s++) {
					if (*s == '\n') ++start_line_offset;
				}
			}
		}
		fseek (note_fp, note_mark[0], 0);
		get_initials(respnum, initials, sizeof (initials));
		copy_body (note_fp, fp, quote_chars, initials);
	} else {
		fseek (note_fp, 0L, 0);
		fprintf(fp, "-------- forwarded-message -------------->\n");
		copy_fp (note_fp, fp, "");
		fprintf(fp, "<------- end-of-forwarded-message --------\n");
	}

	msg_write_signature (fp, TRUE);
#ifdef WIN32
	putc('\0', fp);
#endif
	fclose (fp);

	forever {
		if (confirm_to_mail) {
			do {
				sprintf (msg, "%s [%.*s]: %c", txt_quit_edit_send,
					cCOLS-36, note_h_subj, ch_default);
				wait_message (msg);
				MoveCursor (cLINES, (int) (strlen (msg)-1));
				if ((ch = (char) ReadCh ()) == '\r' || ch == '\n')
					ch = ch_default;
			} while (! strchr (SEND_KEYS, ch));
		}
		switch (ch) {
			case iKeyPostEdit:
				invoke_editor (nam, start_line_offset);
				redraw_screen = TRUE;
				break;

#ifdef HAVE_ISPELL
			case iKeyPostIspell:
				invoke_ispell (nam);
				break;
#endif

#ifdef HAVE_PGP
		        case iKeyPostPGP:
			        invoke_pgp_mail (nam, mail_to);
				break;
#endif

			case iKeyPostQuit:
			case iKeyPostQuit2:
				unlink (nam);
				clear_message ();
				*mailed_ok = FALSE;
				return redraw_screen;

			case iKeyPostSend:
				/*
				 *  Open letter and get the To:  line in
				 *  case they changed it with the editor
				 */
				*mailed_ok = submit_mail_file (nam);
				if (*mailed_ok) {
					goto mail_to_someone_done;
				} else {
					break;
				}
		}
		if (mail_to_poster) {
			do {
				sprintf (msg, "%s [Re: %.*s]: %c", txt_quit_edit_send,
					cCOLS-36, eat_re (note_h_subj), ch_default);
				wait_message (msg);
				MoveCursor (cLINES, (int) (strlen (msg)-1));
				if ((ch = (char) ReadCh ()) == '\r' || ch == '\n')
					ch = ch_default;
			} while (! strchr (SEND_KEYS, ch));
		}
	}

mail_to_someone_done:
	unlink (nam);

	return redraw_screen;
}


int
mail_bug_report ()
{
	char buf[LEN], nam[100];
	char *gateway;
	char *domain;
	char ch, ch_default = iKeyPostSend;
	char mail_to[HEADER_LEN];
	char subject[HEADER_LEN];
	FILE *fp;
#ifdef HAVE_UNAME
	FILE *fp_uname;
#endif
	int is_debug;
	int is_longfiles;
	int is_nntp;
	int is_nntp_only;
	int uname_ok = FALSE;

	msg_init_headers ();

	wait_message (txt_mail_bug_report);

	joinpath (nam, homedir, ".bugreport");
	if ((fp = fopen (nam, "w")) == NULL) {
		perror_message (txt_cannot_open, nam);
		return FALSE;
	}
	chmod(nam, 0600);

	sprintf (buf, "%s%s", bug_addr, add_addr);
	msg_add_header ("To", buf);

	sprintf (subject, "BUG REPORT %s\n", page_header);
	msg_add_header ("Subject", subject);

	if (auto_cc) {
		msg_add_header ("Cc", userid);
	}
	if (auto_bcc) {
		msg_add_header ("Bcc", userid);
	}

	if (*default_organization) {
		msg_add_header ("Organization", default_organization);
	}
	if (*reply_to) {
		msg_add_header ("Reply-To", reply_to);
	}
	msg_add_x_headers (msg_headers_file);

	start_line_offset = msg_write_headers (fp);
	start_line_offset++;
	msg_free_headers ();

#if HAVE_UNAME || HAVE_HOSTNAME
#if HAVE_UNAME
	(void) sprintf(buf, "%s -a", PATH_UNAME);
#else
	(void) strcpy(buf, PATH_HOSTNAME);
#endif
	if ((fp_uname = popen (buf, "r")) != NULL) {
		while (fgets (buf, sizeof (buf), fp_uname) != NULL) {
			fprintf (fp, "BOX1: %s", buf);
			start_line_offset += 2;
			uname_ok = TRUE;
		}
		pclose (fp_uname);
	}
#endif	/* HAVE_UNAME */

	if (! uname_ok) {
		fprintf (fp, "Please enter the following information:\n");
		fprintf (fp, "BOX1: Machine+OS:\n");
	}
#ifdef HAVE_LONG_FILE_NAMES
	is_longfiles = TRUE;
#else
	is_longfiles = FALSE;
#endif
#ifdef NNTP_ABLE
	is_nntp = TRUE;
#else
	is_nntp = FALSE;
#endif
#ifdef NNTP_ONLY
	is_nntp_only = TRUE;
#else
	is_nntp_only = FALSE;
#endif
#ifdef DEBUG
	is_debug = TRUE;
#else
	is_debug = FALSE;
#endif
#ifdef INEWS_MAIL_GATEWAY
	gateway = INEWS_MAIL_GATEWAY;
#else
	gateway = (char *) 0;
#endif
#ifdef INEWS_MAIL_DOMAIN
	domain = INEWS_MAIL_DOMAIN;
#else
	domain = (char *) 0;
#endif
	fprintf (fp, "\nCFG1: active=%d  arts=%d  reread=%d  longfilenames=%d  setuid=%d\n",
		DEFAULT_ACTIVE_NUM,
		DEFAULT_ARTICLE_NUM,
		reread_active_file_secs,
		is_longfiles,
		(tin_uid == real_uid ? 0 : 1));
	fprintf (fp, "CFG2: nntp=%d  nntp_only=%d  nntp_xover=%d\n",
		is_nntp,
		is_nntp_only,
		xover_supported);
	fprintf (fp, "CFG3: debug=%d gateway=[%s] domain=[%s]\n",
		is_debug,
		(gateway ? gateway : ""),
		(domain ? domain : ""));

	start_line_offset += 5;

	fprintf (fp, "\nPlease enter bug report/gripe/comment:\n");

	msg_write_signature (fp, TRUE);
#ifdef WIN32
	putc('\0', fp);
#endif
	fclose (fp);

	ch = iKeyPostEdit;
	forever {
		switch (ch) {
			case iKeyPostEdit:
				invoke_editor (nam, start_line_offset);
				break;

#ifdef HAVE_ISPELL
			case iKeyPostIspell:
				invoke_ispell (nam);
				break;
#endif

#ifdef HAVE_PGP
		        case iKeyPostPGP:
			        invoke_pgp_mail (nam, mail_to);
				break;
#endif

			case iKeyPostQuit:
			case iKeyPostQuit2:
				unlink (nam);
				clear_message ();
				return TRUE;

			case iKeyPostSend:
				sprintf (msg, txt_mail_bug_report_confirm, bug_addr, add_addr);
				if (prompt_yn (cLINES, msg, FALSE) == 1) {
					if (pcCopyArtHeader (HEADER_TO, nam, mail_to)
					 && pcCopyArtHeader (HEADER_SUBJECT, nam, subject)) {
						sprintf (msg, txt_mailing_to, mail_to);
						wait_message (msg);
						rfc15211522_encode(nam);
						strfmailer (mailer, subject, mail_to, nam,
							buf, sizeof (buf), default_mailer_format);
						if (invoke_cmd (buf)) {
							sprintf (msg, txt_mailed, 1);
							info_message (msg);
							goto mail_bug_report_done;
						} else {
							error_message (txt_command_failed_s, buf);
						}
  					}
					break;	/* an error occurred */
				} else {
					goto mail_bug_report_done;
				}
		}
		do {
			sprintf (msg, "%s: %c", txt_quit_edit_send, ch_default);
			wait_message (msg);
			MoveCursor (cLINES, (int) strlen (msg)-1);
			if ((ch = (char) ReadCh ()) == '\r' || ch == '\n')
				ch = ch_default;
			} while (! strchr (SEND_KEYS, ch));
	}

mail_bug_report_done:
	unlink (nam);

	return TRUE;
}


int
mail_to_author (group, respnum, copy_text)
	char *group;
	int respnum;
	int copy_text;
{
	char buf[LEN];
	char from_addr[HEADER_LEN];
	char nam[100];
	char mail_to[HEADER_LEN];
	char subject[HEADER_LEN];
	char initials[64];
	char ch, ch_default = iKeyPostSend;
	FILE *fp;
	int lines = 0;
	int redraw_screen = FALSE;

	msg_init_headers ();

	wait_message (txt_reply_to_author);

#ifdef VMS
	joinpath (nam, homedir, "letter.");
#else
	joinpath (nam, homedir, ".letter");
#endif
	if ((fp = fopen (nam, "w")) == NULL) {
		perror_message (txt_cannot_open, nam);
		return redraw_screen;
	}
	chmod (nam, 0600);

	find_reply_to_addr (respnum, from_addr);

	msg_add_header ("To", from_addr);
	sprintf (subject, "Re: %s\n", eat_re (note_h_subj));
	msg_add_header ("Subject", subject);

	if (auto_cc) {
		msg_add_header ("Cc", userid);
	}
	if (auto_bcc) {
		msg_add_header ("Bcc", userid);
	}

	/*
	 * remove duplicates from Newsgroups header
	 */
	strip_double_ngs(note_h_newsgroups);

	msg_add_header ("Newsgroups", note_h_newsgroups);
	if (*default_organization) {
		msg_add_header ("Organization", default_organization);
	}
	if (*reply_to) {
		msg_add_header ("Reply-To", reply_to);
	}

	start_line_offset = msg_write_headers (fp);
	start_line_offset++;
	msg_free_headers ();

	if (copy_text) {
		if (strfquote (group, respnum, buf, sizeof (buf), mail_quote_format)) {
			fprintf (fp, "%s\n", buf);
			start_line_offset++;
			{ char *s;
				for (s = buf; *s; s++) {
					if (*s == '\n') ++start_line_offset;
				}
			}
		}
		fseek (note_fp, note_mark[0], 0);
		get_initials(respnum, initials, sizeof (initials));
		copy_body (note_fp, fp, quote_chars, initials);
	} else {
		fprintf(fp, "\n"); /* add a newline to keep vi from bitching */
	}

	msg_write_signature (fp, TRUE);
#ifdef WIN32
	putc('\0', fp);
#endif
	fclose (fp);

	ch = iKeyPostEdit;
	forever {
		switch (ch) {
		case iKeyPostEdit:
			invoke_editor (nam, start_line_offset);
			redraw_screen = TRUE;
			break;

#ifdef HAVE_ISPELL
		case iKeyPostIspell:
			invoke_ispell (nam);
			break;
#endif

#ifdef HAVE_PGP
		case iKeyPostPGP:
		        my_strncpy (mail_to, arts[respnum].from, sizeof (mail_to));
			if (pcCopyArtHeader (HEADER_TO, nam, mail_to)
			 && pcCopyArtHeader (HEADER_SUBJECT, nam, subject))
			        invoke_pgp_mail (nam, mail_to);
			break;
#endif

		case iKeyPostQuit:
		case iKeyPostQuit2:
			unlink (nam);
			clear_message ();
			return redraw_screen;

		case iKeyPostSend:
			my_strncpy (mail_to, arts[respnum].from, sizeof (mail_to));
			if (pcCopyArtHeader (HEADER_TO, nam, mail_to)
			 && pcCopyArtHeader (HEADER_SUBJECT, nam, subject)) {
				sprintf (msg, txt_mailing_to, mail_to);
				wait_message (msg);
				checknadd_headers (nam, lines);
				rfc15211522_encode(nam);
				strfmailer (mailer, subject, mail_to, nam,
					buf, sizeof (buf), default_mailer_format);
				if (invoke_cmd (buf)) {
					sprintf (msg, txt_mailed, 1);
					info_message (msg);
					goto mail_to_author_done;
				} else {
 					error_message (txt_command_failed_s, buf);
				}
			}
			break;	/* an error occurred */
		}

		do {
			sprintf (msg, "%s: %c", txt_quit_edit_send, ch_default);
			wait_message (msg);
			MoveCursor (cLINES, (int) strlen (msg)-1);
			if ((ch = (char) ReadCh ()) == '\r' || ch == '\n')
				ch = ch_default;
			} while (! strchr (SEND_KEYS, ch));
	}

mail_to_author_done:
	update_posted_info_file (group, 'r', subject);
/* Commented until decided if this should be done here or be mailer (ie. elm)
	if (psGrp->attribute->auto_save_msg) {
		update_posted_msgs_file (article, ?);
	}
*/
	unlink (nam);

	return redraw_screen;
}

/*
 *  Read a file grabbing the value of the specified mail header line
 */

int
pcCopyArtHeader (iHeader, pcArt, result)
	int iHeader;
	char *pcArt;
	char *result;
{
	char buf[HEADER_LEN];
	char buf2[HEADER_LEN];
	char *p;
	FILE *fp;
	int found = FALSE;
	int was_to = FALSE;
	static char header[HEADER_LEN];

	*header = '\0';

	if ((fp = fopen (pcArt, "r")) == (FILE *) 0) {
		perror_message (txt_cannot_open, pcArt);
		return FALSE;
	}

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		p = strrchr (buf, '\n');
		if (p != (char *) 0) {
			*p = '\0';
		}

		if (*buf == '\0')
			break;

		switch (iHeader) {
			case HEADER_TO:
				if (STRNCMPEQ(buf, "To: ", 4) || STRNCMPEQ(buf, "Cc: ", 4)) {
					my_strncpy (buf2, &buf[4], sizeof (buf2));
					yank_to_addr (buf2, header);
					was_to = TRUE;
					found = TRUE;
				} else if (STRNCMPEQ(buf, "Bcc: ", 5)) {
					my_strncpy (buf2, &buf[5], sizeof (buf2));
					yank_to_addr (buf2, header);
					was_to = TRUE;
					found = TRUE;
				} else if ((*buf == ' ' || *buf == '\t') && was_to) {
					yank_to_addr (buf, header);
					found = TRUE;
				} else {
					was_to = FALSE;
				}
				break;
			case HEADER_NEWSGROUPS:
				if (match_string (buf, "Newsgroups: ", header, sizeof (header))) {
					found = TRUE;
				}
				break;
			case HEADER_SUBJECT:
				if (STRNCMPEQ(buf, "Subject: ", 9)) {
					my_strncpy (header, &buf[9], sizeof (header));
					found = TRUE;
				}
				break;
		}
	}
	fclose (fp);

	if (found) {
		if (header[0] == ' ') {
			p = &header[1];
		} else {
			p = header;
		}
		(void)strcpy(result, rfc1522_decode(p));
		return TRUE;
	}

	switch (iHeader) {
		case HEADER_TO:
			p = txt_error_header_line_missing_target;
			break;
		case HEADER_NEWSGROUPS:
			p = txt_error_header_line_missing_newsgroups;
			break;
		case HEADER_SUBJECT:
			p = txt_error_header_line_missing_subject;
			break;
		default:
			p = "?";
			break;
	}

	/* This should show the name of the offending file, but I didn't want to
	 * add unnecessary message-text.
	 */
	error_message (p, pcArt);
	return FALSE;
}


int
delete_article (group, art, respnum)
	struct t_group *group;
	struct t_article *art;
	int respnum;
{
	char ch, ch_default = iKeyPostDelete;
	char buf[HEADER_LEN];
	char delete[HEADER_LEN];
	char from_name[HEADER_LEN];
#ifdef FORGERY
	char line[HEADER_LEN];
	char line2[HEADER_LEN];
	char author = TRUE;
#else
	char host_name[PATH_LEN];
	char user_name[128];
	char full_name[128];
#endif
	FILE *fp;
	int redraw_screen = FALSE;
	int init = TRUE;
	int oldraw;
	char option = iKeyPostDelete;
	char option_default = iKeyPostDelete;

	msg_init_headers ();

	/*
	 * Check if news / mail / save group
	 */
	if (group->type == GROUP_TYPE_MAIL || group->type == GROUP_TYPE_SAVE) {
#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
		vGrpDelMailArt (art);
#endif	/* !INDEX_DAEMON && HAVE_MH_MAIL_HANDLING */
		return FALSE;
	}

#ifdef FORGERY
	make_path_header (line, from_name);
#else
	get_host_name (host_name);
	get_user_info (user_name, full_name);
	get_from_name (user_name, host_name, full_name, from_name);
#endif

	if (debug == 2) {
		sprintf (msg, "From=[%s]  Cancel=[%s]", art->from, from_name);
		error_message (msg, "");
	}

	if (! str_str (from_name, art->from, strlen (art->from))) {
#ifdef FORGERY
		author = FALSE;
#else
		info_message (txt_art_cannot_delete);
		return redraw_screen;
#endif
	} else {
		do {
			sprintf (msg, txt_delete_article, art->subject, option_default);
			wait_message (msg);
			MoveCursor (cLINES, (int) (strlen (msg)-1));
			if ((option = (char) ReadCh ()) == '\r' || option == '\n')
			option = option_default;
		} while (! strchr ("\033dqs", option));

		switch (option) {
			case iKeyPostDelete:
				break;
			case iKeyPostSupersede:
				repost_article (group->name, art, respnum, TRUE);
				return(0);
			default:
				return(0);
		}
	}

	clear_message ();

	joinpath (delete, homedir, ".cancel");
	if ((fp = fopen (delete, "w")) == (FILE *) 0) {
		perror_message (txt_cannot_open, delete);
		return redraw_screen;
	}
	chmod (delete, 0600);

#ifdef FORGERY
	if (! author) {
		sprintf (line2, "cyberspam!%s", line);
		msg_add_header ("Path", line2);
	} else
		msg_add_header ("Path", line);

	sprintf (line, "%s (%s)", art->from, art->name);
	msg_add_header ("From", line);

	if (! author) {
		sprintf (line, "<cancel.%s", note_h_messageid+1);
		msg_add_header ("Message-ID", line);
		msg_add_header ("X-Cancelled-By", from_name);
	}
#endif

	sprintf (buf, "cmsg cancel %s", note_h_messageid);
	msg_add_header ("Subject", buf);

	/*
	 * remove duplicates from Newsgroups header
	 */
	strip_double_ngs(note_h_newsgroups);

	msg_add_header ("Newsgroups", note_h_newsgroups);
	sprintf (buf, "cancel %s", note_h_messageid);
	msg_add_header ("Control", buf);
	if (group->moderated == 'm') {
		msg_add_header ("Approved", from_name);
	}
	if (*default_organization) {
		msg_add_header ("Organization", default_organization);
	}
	if (*note_h_distrib) {
		msg_add_header ("Distribution", note_h_distrib);
	} else {
		msg_add_header ("Distribution", my_distribution);
	}
	msg_write_headers (fp);
	msg_free_headers ();

	fprintf (fp, txt_article_cancelled, VERSION, RELEASEDATE);
#ifdef FORGERY
	if (! author) {
		fputc ('\n', fp);
		fseek (note_fp, 0L, 0);
		copy_fp (note_fp, fp, "");
	}
	fclose (fp);
	invoke_editor (delete, start_line_offset);
	redraw_screen = TRUE;
#else
	fclose (fp);
#endif /* FORGERY */

	oldraw = RawState();
	setup_check_article_screen(&init);

#ifdef FORGERY
	if (!author) {
		fprintf(stderr, txt_warn_cancel_forgery);
		fprintf(stderr, "From: %s\n", note_h_from);
	} else {
		fprintf(stderr, txt_warn_cancel);
	}
#else
	fprintf(stderr, txt_warn_cancel);
#endif /* FORGERY */

	fprintf(stderr, "Subject: %s\n", note_h_subj);
	fprintf(stderr, "Date: %s\n", note_h_date);
	fprintf(stderr, "Message-ID: %s\n", note_h_messageid);
	fprintf(stderr, "Newsgroups: %s\n", note_h_newsgroups);
	Raw(oldraw);

	forever {
		do {
			sprintf (msg, "%s [%.*s]: %c", txt_quit_delete,
				cCOLS-30, note_h_subj, ch_default);
			wait_message (msg);
			MoveCursor (cLINES, (int) strlen (msg)-1);
			if ((ch = (char) ReadCh ()) == '\r' || ch == '\n')
				ch = ch_default;
		} while (! strchr ("\033deq", ch));

		switch (ch) {
			case iKeyPostEdit:
				invoke_editor (delete, start_line_offset);
				break;

			case iKeyPostDelete:
				wait_message (txt_deleting_art);
				if (submit_news_file (delete, 0)) {
					info_message (txt_art_deleted);
					if (pcCopyArtHeader (HEADER_SUBJECT, delete, buf))
						update_posted_info_file (group->name, iKeyPostDelete, buf);
					unlink (delete);
					return redraw_screen;
				} else {
					error_message (txt_command_failed_s, delete);
					break;
				}

			case iKeyPostQuit:
			case iKeyPostQuit2:
				unlink (delete);
				clear_message ();
				return redraw_screen;
		}
	}
}

/*
 * Repost an already existing article to another group (ie. local group)
 */

int
repost_article (group, art, respnum, supersede)
	char *group;
	struct t_article *art;
	int respnum;
	int supersede;
{
	char 	buf[HEADER_LEN];
	char 	tmp[HEADER_LEN];
	int	done 		= FALSE;
	char 	ch;
	char 	ch_default 	= iKeyPostPost;
	FILE 	*fp;
	int 	ret_code 	= POSTED_NONE;
 	struct t_group 	*psGrp;
 	char 	*ptr;
	char	line[HEADER_LEN];
	char	from_name[HEADER_LEN];
#ifndef FORGERY
	char host_name[PATH_LEN];
	char user_name[128];
	char full_name[128];
#endif
                                
	msg_init_headers ();

	/*
	 * remove duplicates from Newsgroups header
	 */
	strip_double_ngs(note_h_newsgroups);

	/*
	 * Check if any of the newsgroups are moderated.
	 */
	strcpy (tmp, group);
	while (! done) {
		strcpy (buf, tmp);
		ptr = strchr (buf, ',');
		if (ptr != (char *) 0) {
			strcpy (tmp, ptr+1);
			*ptr = '\0';
		} else {
			done = TRUE;
		}
		psGrp = psGrpFind (buf);

		if (debug == 2) {
			sprintf (msg, "Group=[%s]", buf);
			wait_message (msg);
		}

		if (! psGrp) {
	 		error_message (txt_not_in_active_file, buf);
			return POSTED_NONE;
		}
		if (psGrp->moderated == 'm') {
			sprintf (msg, txt_group_is_moderated, buf);
			if (prompt_yn (cLINES, msg, TRUE) != 1) {
				info_message(txt_art_not_posted);
				sleep(3);
				return POSTED_NONE;
			}
		}
	}



/* FIXME so that group only contains 1 group when finding an index number */
/* Does this count? */
	strcpy (buf, group);
	ptr = strchr (buf, ',');
	if (ptr) {
		*ptr = '\0';
	}

 	psGrp = psGrpFind (buf);
 	if (! psGrp) {
 		error_message (txt_not_in_active_file, buf);
 		return ret_code;
 	}

	if ((fp = fopen (article, "w")) == (FILE *) 0) {
		perror_message (txt_cannot_open, article);
		return ret_code;
	}
	chmod (article, 0600);

	if (supersede) {
#ifndef FORGERY
		get_host_name (host_name);
		get_user_info (user_name, full_name);
		get_from_name (user_name, host_name, full_name, from_name);

		if (str_str (from_name, arts[respnum].from, strlen (arts[respnum].from))) {
#else
		make_path_header (line, from_name);
		msg_add_header ("Path", line);
		sprintf (line, "%s (%s)", arts[respnum].from, art->name);
		msg_add_header ("From", line);
		msg_add_header ("X-Superseded-By", from_name);
		if (note_h_org[0])
			msg_add_header ("Organization", note_h_org);
		sprintf (line, "<supersede.%s", note_h_messageid+1);
		msg_add_header ("Message-ID", line);
#endif
		msg_add_header ("Supersedes", note_h_messageid);
		if (note_h_followup[0])
			msg_add_header ("Followup-To", note_h_followup);
		find_reply_to_addr (respnum, line);
		msg_add_header ("Reply-To", line);
		if (note_h_keywords[0])
			msg_add_header ("Keywords", note_h_keywords);
		if (note_h_summary[0])
			msg_add_header ("Summary", note_h_summary);
		if (*note_h_distrib)
			msg_add_header ("Distribution", note_h_distrib);
#ifndef FORGERY
		}
#endif
	}

	msg_add_header ("Subject", note_h_subj);
	msg_add_header ("Newsgroups", group);

	if (note_h_references[0]) {
		/*
		 * calling join_references prevents repost_article
		 * to fail if References: contains a double space
		 * between 2 msgids - what it does not do is
		 * adding the msgid of the original article to the
		 * References header - is this needed?
		 */
#if 0
		msg_add_header ("References", note_h_references);
#else
		join_references (buf, note_h_references, "");
		msg_add_header ("References", buf);
#endif
		}

#ifndef FORGERY
	if (!supersede || (supersede && (!(str_str (from_name, arts[respnum].from, strlen (arts[respnum].from)))))) {
#else
	if (! supersede) {
#endif
		if (psGrp->attribute->organization != (char *) 0) {
			msg_add_header ("Organization", psGrp->attribute->organization);
		}
		if (*reply_to) {
			msg_add_header ("Reply-To", reply_to);
		}
		if (*note_h_distrib) {
			msg_add_header ("Distribution", note_h_distrib);
		} else {
			msg_add_header ("Distribution", my_distribution);
		}
	}
  	
	start_line_offset = msg_write_headers (fp);
	start_line_offset++;
	msg_free_headers ();

#ifndef FORGERY
	if (!supersede || (supersede && (!(str_str (from_name, arts[respnum].from, strlen (arts[respnum].from)))))) {
#else
	if (! supersede) {
#endif
		fprintf (fp, txt_article_reposted1, note_h_newsgroups);
		if (art->name) {
			fprintf (fp, txt_article_reposted2a, art->name, art->from);
		} else {
			fprintf (fp, txt_article_reposted2b, art->from);
		}
		fprintf (fp, "\n[ Posted on %s ]\n\n", note_h_date);
	}

	fseek (note_fp, note_mark[0], 0);
	copy_fp (note_fp, fp, "");

	msg_write_signature (fp, FALSE);
	fclose (fp);
	
	/* on supersede change default-key */
#ifndef FORGERY
	if (supersede && (str_str (from_name, arts[respnum].from, strlen (arts[respnum].from))))
#else
	if (supersede)
#endif
		ch_default=iKeyPostEdit;
	forever {
		do {
			sprintf (msg, txt_quit_edit_xpost,
				cCOLS-(strlen (txt_quit_edit_xpost)-1),
				note_h_subj, ch_default);
			wait_message (msg);
			MoveCursor (cLINES, (int) strlen (msg)-1);
                        if ((ch = (char) ReadCh ()) == '\r' || ch == '\n')
				ch = ch_default;
		} while (! strchr(POST_KEYS, ch));

		switch (ch) {
		case iKeyPostEdit:
			invoke_editor (article, start_line_offset);
			ret_code = POSTED_REDRAW;
			ch_default = iKeyPostPost;
			break;

#ifdef HAVE_ISPELL
		case iKeyPostIspell:
			invoke_ispell (article);
			break;
#endif

#ifdef HAVE_PGP
		case iKeyPostPGP:
		        invoke_pgp_news (article);
			break;
#endif

 		case iKeyPostQuit:
 		case iKeyPostQuit2:
			if (unlink_article)
				unlink (article);
			clear_message ();
			return ret_code;

 		case iKeyPostPost:
#ifndef FORGERY
		if (supersede && (str_str (from_name, arts[respnum].from, strlen (arts[respnum].from)))) {
#else
		if (supersede) {
#endif
				wait_message (txt_superseding_art);
			} else
				wait_message (txt_repost_an_article);

			if (submit_news_file (article, 0)) {
				info_message (txt_art_posted);
				ret_code = POSTED_OK;
				goto repost_done;
			} else {
				rename_file (article, dead_article);
#ifdef M_UNIX
				if (keep_dead_articles)
					append_file (dead_articles, dead_article);
#endif
				sprintf (buf, txt_art_rejected, dead_article);
				info_message (buf);
				sleep (3);
				return ret_code;
			}
		}
	}

repost_done:
	if (pcCopyArtHeader (HEADER_NEWSGROUPS, article, buf)) {
		update_active_after_posting (buf);

		if (pcCopyArtHeader (HEADER_SUBJECT, article, buf))
			update_posted_info_file (psGrp->name, 'x', buf);
	}

	if (unlink_article) {
		unlink (article);
	}

	return ret_code;
}

void
msg_add_x_headers (headers)
	char	*headers;
{
	char	*ptr;
	char	file[PATH_LEN];
	char	line[HEADER_LEN];
	FILE	*fp;

	if (headers) {
		if (headers[0] != '/' && headers[0] != '~') {
			strcpy (line, headers);
			ptr = strchr (line, ':');
			if (ptr) {
				*ptr = '\0';
				ptr++;
				if (*ptr == ' ' || *ptr == '\t') {
					msg_add_header (line, ptr);
					return;
				}
			}
		}
		if (! strfpath (headers, file, sizeof (file),
		    homedir, (char *) 0, (char *) 0, (char *) 0)) {
			strcpy (file, headers);
		}
		if ((fp = fopen (file, "r")) != (FILE *) 0) {
			while (fgets (line, sizeof (line), fp) != (char *) 0) {
				if (line[0] != '\n' && line[0] != '#') {
					ptr = strchr (line, ':');
					if (ptr) {
						*ptr = '\0';
						ptr++;
					}
					msg_add_header (line, ptr);
				}
			}
			fclose (fp);
		}
	}
}


int
msg_add_x_body (fp_out, body)
  	FILE *fp_out;
	char *body;
{
	char	*ptr;
	char	file[PATH_LEN];
	char	line[HEADER_LEN];
	FILE	*fp;
	int		wrote = 0;

	if (body) {
		if (body[0] != '/' && body[0] != '~') {
			strcpy (line, body);
			ptr = strrchr (line, '\n');
			if (ptr) {
				*ptr = '\0';
			}
			fprintf (fp_out, "%s\n", line);
			wrote++;
		} else {
			if (! strfpath (body, file, sizeof (file),
			    homedir, (char *) 0, (char *) 0, (char *) 0)) {
				strcpy (file, body);
			}
			if ((fp = fopen (file, "r")) != (FILE *) 0) {
				while (fgets (line, sizeof (line), fp) != (char *) 0) {
					fputs (line, fp_out);
					wrote++;
				}
				fclose (fp);
			}
		}
		if (wrote > 1) {
			fputc ('\n', fp_out);
			wrote++;
		}
	}

	return wrote;
}

void
modify_headers (line)
	char *line;
{

	char 	buf[HEADER_LEN];
	char	*chr;
	char	*chr2;

	if (strncasecmp(line, "Newsgroups: ", 12) == 0) {
		chr = strpbrk(line, "\t ");
		while ((strchr("\t ", *chr)) != (char *) 0) {
			chr++;
		}
		chr2 = strchr(chr, '\n');
		*chr2 = '\0';
		strip_double_ngs(chr);
		strcpy(found_newsgroups, chr);
		sprintf(line, "Newsgroups: %s\n", found_newsgroups);
	}
	if (strncasecmp(line, "Followup-To: ", 11) ==  0) {
		chr = strpbrk(line, "\t ");
		while ((strchr("\t ", *chr)) != (char *) 0) {
			chr++;
		}
		chr2 = strchr(chr, '\n');
		*chr2 = '\0';
		strip_double_ngs(chr);
		strcpy(buf, chr);
		if ((*found_newsgroups == '\0') || (strcasecmp(found_newsgroups, buf))) {
			sprintf(line, "Followup-To: %s\n", buf);
		} else {
			*line = '\0';
		}
	}
}


void
checknadd_headers (infile, lines)
	char *infile;
	int   lines;
{
	char line[HEADER_LEN];
	char outfile[PATH_LEN];
	FILE *fp_in, *fp_out;
	int gotit = FALSE;

	if ((fp_in = fopen (infile, "r")) != (FILE *) 0) {
#ifdef VMS
		sprintf (outfile, "%s-%d", infile, process_id);
#else
		sprintf (outfile, "%s.%d", infile, process_id);
#endif
		*found_newsgroups = '\0';
		if ((fp_out = fopen (outfile, "w")) != (FILE *) 0) {
			while (fgets (line, sizeof (line), fp_in) != (char *) 0) {
				if (! gotit && line[0] == '\n') {
					if (lines) {
						fprintf (fp_out, "Lines: %d\n", lines);
					}
					if (!no_advertising) {
						if (CURR_GROUP.type == GROUP_TYPE_MAIL) {
							fprintf (fp_out, "X-Mailer: TIN [%s %s release %s]\n\n",
								OS, VERSION, RELEASEDATE);
						} else {
							fprintf (fp_out, "X-Newsreader: TIN [%s %s release %s]\n\n",
								OS, VERSION, RELEASEDATE);
						}
					}
					else {
						fprintf(fp_out, "\n");
					}
					gotit = TRUE;
				} else {
					if (!gotit) {
						modify_headers(line);
					}
					fputs (line, fp_out);
				}
			}
			fclose (fp_out);
			fclose (fp_in);
			rename_file (outfile, infile);
		}
	}
}

#ifndef M_AMIGA
int
insert_from_header (infile)
  	char *infile;
{
	char	*ptr;
	char	from_name[HEADER_LEN];
	char	host_name[PATH_LEN];
	char	full_name[128];
	char	user_name[128];
	char	line[HEADER_LEN];
	char	outfile[PATH_LEN];
	FILE	*fp_in, *fp_out;

	if ((fp_in = fopen (infile, "r")) != (FILE *) 0) {
#ifdef VMS
		sprintf (outfile, "%s-%d", infile, process_id);
#else
		sprintf (outfile, "%s.%d", infile, process_id);
#endif
		if ((fp_out = fopen (outfile, "w")) != (FILE *) 0) {
			get_host_name (host_name);
			get_user_info (user_name, full_name);
			get_from_name (user_name, host_name, full_name, from_name);

			/*
			 * Check that at least one '.' comes after the '@' in the From: line
			 */
			ptr = strchr (from_name, '@');
			if (ptr != (char *) 0) {
				ptr = strchr (ptr, '.');
				if (ptr == (char *) 0) {
					error_message (txt_invalid_from1, from_name);
					return FALSE;
				}
			}

			/*
			 * Check that domain is not of type  host.subdomain.domain
			 */
			ptr = str_str (from_name, "subdomain.domain", 16);
			if (ptr != (char *) 0) {
				error_message (txt_invalid_from2, from_name);
				return FALSE;
			}

			fprintf (fp_out, "From: %s\n", from_name);

			while (fgets (line, sizeof (line), fp_in) != (char *) 0) {
				fputs (line, fp_out);
			}
			fclose (fp_out);
			fclose (fp_in);
			rename_file (outfile, infile);

			return TRUE;
		}
	}

	return FALSE;
}
#endif

void
find_reply_to_addr (respnum, from_addr)
	int respnum;
	char *from_addr;
{
	char *ptr, buf[HEADER_LEN];
	char from_both[HEADER_LEN];
	char from_name[HEADER_LEN];
	int found = FALSE;
	long orig_offset;

	orig_offset = ftell (note_fp);
	fseek (note_fp, 0L, 0);

	while (fgets (buf, sizeof (buf), note_fp) != (char *) 0 &&
		found == FALSE && buf[0] != '\n') {
		if (STRNCMPEQ(buf, "Reply-To: ", 10)) {
			strcpy (from_both, &buf[10]);
			ptr = strchr (from_both, '\n');
			if (ptr != (char *) 0) {
				*ptr = '\0';
			}
			parse_from (from_both, from_addr, from_name);
			if (from_name[0]) {
				sprintf (buf, "%s%s (%s)", from_addr, add_addr, from_name);
			} else {
				strcat (from_addr, add_addr);
			}
			found = TRUE;
		}
	}

	if (! found) {
		if (arts[respnum].name != (char *) 0 &&
		    arts[respnum].name != arts[respnum].from) {
			sprintf (buf, "%s%s (%s)",
				 arts[respnum].from, add_addr,
				 arts[respnum].name);
			strcpy (from_addr, buf);
		} else {
			sprintf (from_addr, "%s%s",
				 arts[respnum].from, add_addr);
		}
	}

	fseek (note_fp, orig_offset, 0);
}

/*
 * If any arts have been posted by the user reread the active
 * file so that they are shown in the unread articles number
 * for each group at the group selection level.
 */

int
reread_active_after_posting ()
{
	char acBuf[PATH_LEN];
	int i, modified = FALSE;
	long lMinOld;
	long lMaxOld;
	struct t_group *psGrp;

	if (reread_active_for_posted_arts) {
		reread_active_for_posted_arts = FALSE;

		for (i = 0; i < num_active; i++) {
			psGrp = &active[i];
			if (psGrp->subscribed == SUBSCRIBED && psGrp->art_was_posted) {
				psGrp->art_was_posted = FALSE;

				if (psGrp != (struct t_group *) 0) {
					sprintf (acBuf, "Rereading %s...", psGrp->name);
					wait_message (acBuf);
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
	printf ("\r\nUnread WRONG grp=[%s] unread=[%ld] count=[%ld]",
		psGrp->name, psGrp->newsrc.num_unread, psGrp->count);
	fflush(stdout);
#endif
						psGrp->newsrc.num_unread = psGrp->count;
					}
					if (psGrp->xmin != lMinOld || psGrp->xmax != lMaxOld) {
#ifdef DEBUG
	printf ("\r\nMin/Max DIFF grp=[%s] old=[%ld-%ld] new=[%ld-%ld]",
		psGrp->name, lMinOld, lMaxOld, psGrp->xmin, psGrp->xmax);
	fflush(stdout);
#endif
/*
sprintf (acBuf, "EXPAND Min/Max DIFF grp=[%s] old=[%ld-%ld] new=[%ld-%ld]",
	psGrp->name, lMinOld, lMaxOld, psGrp->xmin, psGrp->xmax);
error_message (acBuf, "");
*/
						expand_bitmap (psGrp, psGrp->xmin);
						modified = TRUE;
					}
					clear_message ();
				}
			}
		}
	}

	return modified;
}

/*
 * If posting was successful parse the Newgroups; line and set a flag in each
 * posted to newsgroups for later processing to update num of unread articles
 */

void
update_active_after_posting (newsgroups)
	char *newsgroups;
{
	char *src, *dst;
	char group[HEADER_LEN];
	struct t_group *psGrp;

	/*
	 * remove duplicates from Newsgroups header
	 */
	strip_double_ngs(note_h_newsgroups);

	strcat (newsgroups, "\n");
	src = newsgroups;
	dst = group;

	while (*src) {
		if (*src != ' ') {
			*dst = *src;
		}
		src++;
		if (*dst == ',' || *dst == '\n') {
			*dst = '\0';
			psGrp = psGrpFind (group);
			if (psGrp != (struct t_group *) 0 && psGrp->subscribed == SUBSCRIBED) {
/*
error_message ("Updating %s...", group);
*/
				reread_active_for_posted_arts = TRUE;
				psGrp->art_was_posted = TRUE;
/*
clear_message ();
*/
			}
			dst = group;
		} else {
			dst++;
		}
	}
}

int
submit_mail_file (file)
	char	*file;
{
	char buf[HEADER_LEN];
	char mail_to[HEADER_LEN];
	char subject[HEADER_LEN];
	int mailed = FALSE;

#ifndef M_AMIGA
	if (insert_from_header (file))
#endif
	{
		if (pcCopyArtHeader (HEADER_TO, file, mail_to)
	 	&& pcCopyArtHeader (HEADER_SUBJECT, file, subject)) {

			sprintf (buf, txt_mailing_to, mail_to);
			wait_message (buf);

			rfc15211522_encode(file);

			strfmailer (mailer, subject, mail_to, file,
				buf, sizeof (buf), default_mailer_format);

/* error_message ("Mail=[%s]", buf); */

			if (invoke_cmd (buf)) {
				mailed = TRUE;
			} else {
				error_message (txt_command_failed_s, buf);
			}
		}
	}

	return mailed;
}

#ifdef FORGERY
void
make_path_header (line, from_name)
	char    *line, *from_name;
{
	char    domain_name[PATH_LEN];
	char    host_name[PATH_LEN];
	char    full_name[128];
	char    user_name[128];

	get_host_name (host_name);
	get_user_info (user_name, full_name);
	get_from_name (user_name, host_name, full_name, from_name);

#if defined(INEWS_MAIL_GATEWAY) || defined(INEWS_MAIL_DOMAIN)
	if (*(INEWS_MAIL_GATEWAY)) {
		strcpy (line, user_name);
	} else if (*(INEWS_MAIL_DOMAIN)) {
		strcpy (line, INEWS_MAIL_DOMAIN);
		get_domain_name (line, domain_name);
		if (*domain_name == '.') {
			sprintf (line, "%s%s!%s", host_name, domain_name, user_name);
		} else {
			/* mail mark@garden.equinox.gen.nz if you think
			 * host_name should be domain_name here...
			 */
			sprintf (line, "%s!%s", host_name, user_name);
		}
	} else {
		sprintf (line, "%s!%s", host_name, user_name);
	}
#else
	sprintf (line, "%s!%s", host_name, user_name);
#endif
	return;
}
#endif /* FORGERY */
