/*
 *  Project   : tin - a Usenet reader
 *  Module    : post.c
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 22-08-95
 *  Notes     : mail/post/replyto/followup/repost & cancel articles
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"
#include	"menukeys.h"
#include	"version.h"

#define	PRINT_LF()	{Raw (FALSE); my_fputc ('\n', stdout); my_flush (); Raw (TRUE);}

#define	MAX_MSG_HEADERS	20

/* which keys are allowed for posting/sending? */
#ifdef HAVE_PGP
#	ifdef HAVE_ISPELL
#		define POST_KEYS	"\033egiopyq"
#		define SEND_KEYS	"\033egiqsy"
#	else
#		define POST_KEYS	"\033egopyq"
#		define SEND_KEYS	"\033egqsy"
#	endif
#else
#	ifdef HAVE_ISPELL
#		define POST_KEYS	"\033eiopqy"
#		define SEND_KEYS	"\033eiqsy"
#	else
#		define POST_KEYS	"\033eopyq"
#		define SEND_KEYS	"\033eqsy"
#	endif
#endif
#define EDIT_KEYS	"\033eoq"
#define CONT_KEYS	"\033ac"

char found_newsgroups[HEADER_LEN];

t_bool reread_active_for_posted_arts = TRUE;
t_bool unlink_article = TRUE;
t_bool keep_dead_articles = TRUE;
t_bool keep_posted_articles = TRUE;

struct t_posted *posted;

struct msg_header {
	char *name;
	char *text;
} msg_headers[MAX_MSG_HEADERS];

/*
 * Local prototypes
 */
static char *backup_article_name (char *the_article);
static char prompt_rejected (void);
static char prompt_to_continue (void);
static char prompt_to_edit (void);
static char prompt_to_send (const char *subject);
static int fetch_postponed_article(char tmp_file[], char subject[], char newsgroups[]);
static int msg_add_x_body (FILE *fp_out, char *body);
static int msg_write_headers (FILE *fp);
static int pcCopyArtHeader (int iHeader, char *pcArt, char *result);
static int submit_mail_file (char *file);
static size_t skip_id (const char *id);
static t_bool check_article_to_be_posted (char *the_article, int art_type, int *lines);
static t_bool check_for_spamtrap (char *addr);
static t_bool damaged_id (const char *id);
static t_bool is_crosspost (char *xref);
static t_bool must_include (char *id);
static t_bool repair_article (char *result);
static void append_postponed_file (char *file, char *addr);
static void appendid (char **where, const char **what);
static void backup_article (char *the_article);
static void find_reply_to_addr (int respnum, char *from_addr, t_bool parse);
static void join_references (char *buffer, char *oldrefs, const char *newref);
static void modify_headers (char *line);
static void msg_add_header (const char *name, const char *text);
static void msg_add_x_headers (char *headers);
static void msg_free_headers (void);
static void msg_init_headers (void);
static void post_existing_article (int ask, char* subject);
static void postpone_article (char *the_article);
static void setup_check_article_screen (int *init);
static void update_active_after_posting (char *newsgroups);
static void update_posted_info_file (char *group, int action, char *subj);
static void update_posted_msgs_file (char *file, char *addr);
#ifdef FORGERY
	static void make_path_header (char *line, char *from_name);
#endif /* FORGERY */
#ifndef M_AMIGA
	static t_bool insert_from_header (char *infile);
#endif /* !M_AMIGA */


static char
prompt_to_edit(void)
{
	return (prompt_slk_response(iKeyPostPost, POST_KEYS, txt_quit_edit_post));
}

static char
prompt_to_send(
	const char *subject)
{
	return (prompt_slk_response (iKeyPostSend, SEND_KEYS, sized_message(txt_quit_edit_send, subject)));
}

static char
prompt_to_continue(void)
{
	return (prompt_slk_response (iKeyPostContinue, CONT_KEYS, txt_warn_suspicious_mail));
}

static char
prompt_rejected(void)
{
/* fix screen pos. ?? */
	Raw (FALSE);
	my_fprintf(stderr, "\n\n%s\n\n", txt_post_error_ask_postpone);
	my_fflush(stderr);
	Raw (TRUE);

	return (prompt_slk_response (iKeyPostEdit, EDIT_KEYS, txt_quit_edit_postpone));
}

static t_bool
repair_article(
	char *result)
{
	int ch;

	ch = prompt_slk_response (iKeyPostEdit, EDIT_KEYS, txt_bad_article);

	*result = ch;
	if (ch == iKeyPostEdit) {
		invoke_editor (article, start_line_offset);
		return TRUE;
	}
	return FALSE;
}

/* make a backup copy of ~/.article, this is necessary since
 * submit_news_file adds headers, does q-p conversion etc 
 */

static char *
backup_article_name(
	char *the_article)
{
	static char name[PATH_LEN];

	strcpy(name, the_article);
	strcat(name, ".bak");
	return name;
}

static void
backup_article(
	char *the_article)
{
	char line[LEN];
	FILE *in, *out;

	if ((in = fopen(the_article, "r")) == NULL)
		return;

	if ((out = fopen(backup_article_name(the_article), "w")) == NULL) {
		fclose(in);
		return;
	}

	while( fgets(line, sizeof(line), in) != NULL)
		fputs(line, out);

	fclose(in);
	fclose(out);
}

static void
msg_init_headers (void)
{
	int i;

	for (i = 0; i < MAX_MSG_HEADERS; i++) {
		msg_headers[i].name = (char *) 0;
		msg_headers[i].text = (char *) 0;
	}
}

static void
msg_free_headers (void)
{
	int i;

	for (i = 0; i < MAX_MSG_HEADERS; i++) {
		FreeAndNull (msg_headers[i].name);
		FreeAndNull (msg_headers[i].text);
	}
}

static void
msg_add_header (
	const char *name,
	const char *text)
{
	const char *p;
	char *ptr;
	char *new_name = (char *) 0;
	char *new_text = (char *) 0;
	int done = FALSE;
	int i;

	if (name) {
		/*
		 * Remove : if one is attached to name
		 */
		new_name = my_strdup (name);
		ptr = strchr (new_name, ':');
		if (ptr)
			*ptr = '\0';

		/*
		 * Check if header already exists and if update text
		 */
		for (i = 0; i < MAX_MSG_HEADERS && msg_headers[i].name; i++) {
			if (STRCMPEQ(msg_headers[i].name, new_name)) {
				FreeAndNull (msg_headers[i].text);
				if (text) {
					for (p = text; *p && (*p == ' ' || *p == '\t'); p++) {
						;
					}
					new_text = my_strdup (p);
					ptr = strchr (new_text, '\n');
					if (ptr)
						*ptr = '\0';

					msg_headers[i].text = my_strdup (new_text);
				}
				done = TRUE;
			}
		}

		/*
		 * if header does not exist then add it
		 */
		if (!(done || msg_headers[i].name)) {
			msg_headers[i].name = my_strdup (new_name);
			if (text) {
				for (p = text; *p && (*p == ' ' || *p == '\t'); p++) {
					;
				}
				new_text = my_strdup (p);
				ptr = strchr (new_text, '\n');
				if (ptr)
					*ptr = '\0';

				msg_headers[i].text = my_strdup (new_text);
			}
		}
		FreeIfNeeded (new_name);
		FreeIfNeeded (new_text);
	}
}

static int
msg_write_headers (
	FILE *fp)
{
	int i;
	int wrote = 1;

	for (i = 0; i < MAX_MSG_HEADERS; i++) {
		if (msg_headers[i].name) {
			fprintf (fp, "%s: %s\n", msg_headers[i].name, (msg_headers[i].text ? msg_headers[i].text : ""));
			wrote++;
		}
	}
	fputc ('\n', fp);

	return wrote;
}

t_bool
user_posted_messages (void)
{
	char buf[LEN];
	FILE *fp;
	size_t i = 0, j, k;
	int no_of_lines = 0;

	if ((fp = fopen (posted_info_file, "r")) == (FILE *) 0) {
		clear_message ();
		return FALSE;
	} else {
		while (fgets (buf, sizeof (buf), fp) != (char *) 0)
			no_of_lines++;

		if (!no_of_lines) {
			fclose (fp);
			info_message (txt_no_arts_posted);
			return FALSE;
		}
		rewind (fp);
		posted = (struct t_posted *) my_malloc ((no_of_lines + 1) * sizeof (struct t_posted));

		while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
			if (buf[0] == '#' || buf[0] == '\n')
				continue;

			for (j = 0; buf[j] != '|' && buf[j] != '\n'; j++)
				posted[i].date[j] = buf[j];	/* posted date */

			if (buf[j] == '\n') {
				error_message ("Corrupted file %s", posted_info_file);
				sleep (1);
				fclose (fp);
				clear_message ();
				return FALSE;
			}
			posted[i].date[j++] = '\0';
			posted[i+1].date[0] = '\0';
			posted[i].action = buf[j];
			j += 2;
			for (k = j, j = 0; buf[k] != '|' && buf[k] != ','; k++, j++) {
				if (j < sizeof (posted[i].group))
					posted[i].group[j] = buf[k];
			}
			if (buf[k] == ',') {
				while (buf[k] != '|' && buf[k] != '\n')
					k++;

				posted[i].group[j++] = ',';
				posted[i].group[j++] = '.';
				posted[i].group[j++] = '.';
				posted[i].group[j++] = '.';
			}
			posted[i].group[j] = '\0';
			k++;
			for (j = k, k = 0; buf[j] != '\n'; j++, k++) {
				if (k < sizeof (posted[i].subj))
					posted[i].subj[k] = buf[j];
			}
			posted[i].subj[k] = '\0';
			i++;
		}
		fclose (fp);

		show_info_page (POST_INFO, 0, txt_post_history_menu);

		if (posted != (struct t_posted *)0) {
			free((char *)posted);
			posted = (struct t_posted *)0;
		}
		return TRUE;
	}
}

static void
update_posted_info_file (
	char *group,
	int action,
	char *subj)
{
	FILE *fp;
	time_t epoch;
	struct tm *tm;

	if ((fp = fopen (posted_info_file, "a+")) != NULL) {
		time (&epoch);
		tm = localtime (&epoch);
		fprintf (fp, "%02d-%02d-%02d|%c|%s|%s\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year, action, group, subj);
		fclose (fp);
	}
}

static void
update_posted_msgs_file (
	char *file,
	char *addr)
{
	char buf[LEN];
	FILE *fp_in, *fp_out;
	time_t epoch;

	fp_in = fopen (file, "r");
	if (fp_in != (FILE *) 0) {
		if (!strfpath (posted_msgs_file, buf, sizeof (buf), homedir, (char *) 0, (char *) 0, (char *) 0))
			strcpy (buf, posted_msgs_file);

		fp_out = fopen (buf, "a+");
		if (fp_out != (FILE *) 0) {
			time (&epoch);
			fprintf (fp_out, "From %s %s", addr, ctime (&epoch));
			while (fgets (buf, sizeof buf, fp_in) != (char *) 0)
				fputs (buf, fp_out);

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
 * 9.  No Sender: header allowed (limit forging) and rejection by
 *     inn servers
 * 10. Display an 'are you sure' message before posting article
 */

static t_bool
check_article_to_be_posted (
	char *the_article,
	int art_type,
	int *lines)
{
	FILE *fp;
	char *ngptrs[NGLIMIT];
	char line[HEADER_LEN], *cp, *cp2;
	int cnt = 0;
	int col, len, i;
	int errors = 0;
	int init = TRUE;
	int ngcnt = 0;
	int oldraw;		/* save previous raw state */
	int c;
	int saw_sig_dashes = 0;
	int sig_lines = 0;
	size_t nglens[NGLIMIT];
	t_bool end_of_header = FALSE;
	t_bool found_newsgroups_line = FALSE;
	t_bool found_subject_line = FALSE;
	t_bool found_followup_to = FALSE;
	t_bool found_followup_to_several_groups = FALSE;
	t_bool got_long_line = FALSE;
	t_bool saw_wrong_sig_dashes = FALSE;
	struct t_group *psGrp;

	if ((fp = fopen (the_article, "r")) == (FILE *) 0) {
		perror_message (txt_cannot_open, the_article);
		return FALSE;
	}
	oldraw = RawState ();	/* save state */

	while (fgets (line, sizeof (line), fp) != NULL) {
		cnt++;
		len = strlen (line);
		if (len > 0) {
			if (line[len - 1] == '\n')
				line[--len] = 0;
		}
		if ((cnt == 1) && (len == 0)) {
			setup_check_article_screen (&init);
			my_fprintf (stderr, txt_error_header_line_blank);
			my_fflush (stderr);
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
		if ((line[0] == ' ' || line[0] == '\t') && (cnt != 1))
			continue;

		cp = strchr (line, ':');
		if (cp == (char *) 0) {
			setup_check_article_screen (&init);
			my_fprintf (stderr, txt_error_header_line_colon, cnt, line);
			my_fflush (stderr);
			errors++;
			continue;
		}
		if (cp[1] != ' ') {
			setup_check_article_screen (&init);
			my_fprintf (stderr, txt_error_header_line_space, cnt, line);
			my_fflush (stderr);
			errors++;
		}
		if (cp - line == 7 && !strncasecmp (line, "Subject", 7))
			found_subject_line = TRUE;

#ifndef FORGERY
		if (cp - line == 6 && !strncasecmp (line, "Sender", 6)) {
			my_fprintf (stderr, txt_error_sender_in_header_not_allowed, cnt);
			my_fflush (stderr);
			errors++;
		}
#endif
		if (cp - line == 10 && !strncasecmp (line, "Newsgroups", 10)) {
			found_newsgroups_line = TRUE;
			for (cp = line + 11; *cp == ' '; cp++) {
				;
			}
			if (strchr (cp, ' ')) {
				setup_check_article_screen (&init);
				my_fprintf (stderr, txt_error_header_line_comma, "Newsgroups");
				my_fflush (stderr);
				errors++;
				continue;
			}
			strip_double_ngs (cp);
			while (*cp) {
				if (!(cp2 = strchr (cp, ',')))
					cp2 = cp + strlen (cp);
				else
					*cp2++ = '\0';
				if (ngcnt < NGLIMIT) {
					nglens[ngcnt] = strlen (cp);
					ngptrs[ngcnt] = (char *) my_malloc (nglens[ngcnt] + 1);
					if (!ngptrs[ngcnt]) {
						for (i = 0; i < ngcnt; i++)
							FreeIfNeeded (ngptrs[i]);
						Raw (oldraw);
						return TRUE;
					}
					strcpy (ngptrs[ngcnt], cp);
					ngcnt++;
				}
				cp = cp2;
			}
			if (!ngcnt) {
				setup_check_article_screen (&init);
				my_fprintf (stderr, txt_error_header_line_empty_newsgroups);
				my_fflush (stderr);
				errors++;
				continue;
			}
			if((c = fgetc(fp)) != EOF) {
				ungetc(c, fp);
				if (isspace (c) && c != '\n') {
					setup_check_article_screen (&init);
					my_fprintf (stderr, txt_error_header_line_groups_contd, "Newsgroups");
					my_fflush (stderr);
					errors++;
					continue;
				}
			}
		}
		if (cp - line == 11 && !strncasecmp (line, "Followup-To", 11)) {
			found_followup_to = TRUE;
			for (cp = line + 12; *cp == ' '; cp++) {
				;
			}
			if (strchr (cp, ','))
				found_followup_to_several_groups = TRUE;
			if (strchr (cp, ' ')) {
				setup_check_article_screen (&init);
				my_fprintf (stderr, txt_error_header_line_comma, "Followup-To");
				my_fflush (stderr);
				errors++;
				continue;
			}
			if((c = fgetc(fp)) != EOF) {
				ungetc(c, fp);
				if (isspace(c) && c != '\n' ) {
					setup_check_article_screen (&init);
					my_fprintf (stderr, txt_error_header_line_groups_contd, "Followup-To");
					my_fflush (stderr);
					errors++;
					continue;
				}
			}
		}
	}

	if (!found_subject_line) {
		setup_check_article_screen (&init);
		my_fprintf (stderr, txt_error_header_line_missing_subject);
		my_fflush (stderr);
		errors++;
	}
	if (!found_newsgroups_line && art_type == GROUP_TYPE_NEWS) {
		setup_check_article_screen (&init);
		my_fprintf (stderr, txt_error_header_line_missing_newsgroups);
		my_fflush (stderr);
		errors++;
	}
	/*
	 * Check the body of the article for long lines
	 * check if sig is shorter then 5 lines
	 */
	while (fgets (line, sizeof (line), fp)) {
		(*lines)++;
		cnt++;
		cp = strrchr (line, '\n');
		if (cp != (char *) 0)
			*cp = '\0';

		if (saw_sig_dashes || saw_wrong_sig_dashes)
			sig_lines++;

		if (!strcmp(line, "-- ")) {
			saw_wrong_sig_dashes=FALSE;
			saw_sig_dashes++;
			sig_lines = 0;
		}
		if (!strcmp(line, "--") && !saw_sig_dashes) {
			saw_wrong_sig_dashes=TRUE;
			sig_lines = 0;
		}
		col = 0;
		for (cp = line; *cp; cp++) {
			if (*cp == '\t')
				col += 8 - (col % 8);
			else
				col++;
		}
		if (col > MAX_COL && !got_long_line) {
			setup_check_article_screen (&init);
			my_fprintf (stderr, txt_warn_art_line_too_long, MAX_COL, cnt, line);
			my_fflush (stderr);
			got_long_line = TRUE;
		}
	}
	if (saw_sig_dashes >= 2) {
		setup_check_article_screen (&init);
		my_fprintf (stderr, txt_warn_multiple_sigs, saw_sig_dashes);
		my_fflush (stderr);
	}
	if (saw_wrong_sig_dashes) {
		setup_check_article_screen (&init);
		my_fprintf (stderr, txt_warn_wrong_sig_format );
		my_fflush (stderr);
	}
	if (sig_lines > MAX_SIG_LINES) {
		setup_check_article_screen (&init);
		my_fprintf (stderr, txt_warn_sig_too_long, MAX_SIG_LINES );
		my_fflush (stderr);
#ifdef HAVE_FASCIST_NEWSADMIN
		errors++;
#endif
	}
	if (!end_of_header) {
		setup_check_article_screen (&init);
		my_fprintf (stderr, txt_error_header_and_body_not_separate);
		my_fflush (stderr);
		errors++;
	}
	if (ngcnt && errors == 0) {
		/*
		 * Print a note about each newsgroup
		 */
		setup_check_article_screen (&init);
		my_fprintf (stderr, txt_art_newsgroups, ngcnt == 1 ? "" : "s");
		my_fflush (stderr);
		for (i = 0; i < ngcnt; i++) {
			psGrp = psGrpFind (ngptrs[i]);
			if (psGrp) {
				my_fprintf (stderr, "  %s\t%s\n", ngptrs[i], (psGrp->description ? psGrp->description : ""));
				my_fflush (stderr);
			} else {
#ifdef HAVE_FASCIST_NEWSADMIN
				my_fprintf (stderr, txt_error_not_valid_newsgroup, ngptrs[i]);
				errors++;
#else
				my_fprintf (stderr, (newsrc_active ? /* did we read the whole active file? */ txt_warn_not_in_newsrc : txt_warn_not_valid_newsgroup ), ngptrs[i]);
#endif
			}
			free (ngptrs[i]);
		}
		if (!found_followup_to && ngcnt > 1 && !errors) {
#ifdef HAVE_FASCIST_NEWSADMIN
			my_fprintf (stderr, txt_error_missing_followup_to, ngcnt);
			errors++;
#else
			my_fprintf (stderr, txt_warn_missing_followup_to, ngcnt);
#endif
		}
		if (found_followup_to_several_groups && !errors) {
#ifdef HAVE_FASCIST_NEWSADMIN
			my_fprintf (stderr, txt_error_followup_to_several_groups);
			errors++;
#else
			my_fprintf (stderr, txt_warn_followup_to_several_groups);
#endif
		}
#ifndef NO_ETIQUETTE
		my_fprintf (stderr, txt_warn_posting_etiquette);
#endif
		my_fflush (stderr);
	}
	fclose (fp);

	Raw (oldraw);		/* restore raw/unraw state */

	return (errors ? FALSE : TRUE);
}

static void
setup_check_article_screen (
	int *init)
{
	if (*init) {
		ClearScreen ();
		center_line (0, TRUE, txt_check_article);
		MoveCursor (INDEX_TOP, 0);
		Raw (FALSE);
		*init = FALSE;
	}
}

/*
 *  Quick post an article (not a followup)
 */

void
quick_post_article (
	t_bool postponed_only)
{
	FILE *fp;
	char ch, *ptr;
	char group[HEADER_LEN];
	char subj[HEADER_LEN];
	char buf[HEADER_LEN], tmp[HEADER_LEN];
	char from_name[HEADER_LEN];
	int art_type = GROUP_TYPE_NEWS;
	int lines;
	t_bool done = FALSE;
	struct t_group *psGrp;

#ifdef FORGERY
	char line[HEADER_LEN];
#endif

	msg_init_headers ();

	if (!can_post) {
		info_message (txt_cannot_post);
		return;
	}

	setup_screen ();
	InitScreen ();
	ClearScreen ();

	/*
	 * check for postponed articles first
	 * first param is whether to ask the user if they want to do it or not.
	 * it's opposite to the command line switch.
	 * second param is whether to assume yes to all which is the same as
	 * the command line switch.
	 */

	if (pickup_postponed_articles(!postponed_only, postponed_only) || postponed_only)
		return;

	/*
	 * Get groupname & subject for posting article.
	 * If multiple newsgroups test all to see if any are moderated.
	 */
	sprintf (buf, txt_post_newsgroups, default_post_newsgroups);

	if (!prompt_string (buf, group, HIST_POST_NEWSGROUPS)) {
		my_fprintf (stderr, "%s\n", txt_no_quick_newsgroups);
		return;
	}
	if (strlen (group))
		my_strncpy (default_post_newsgroups, group, sizeof (default_post_newsgroups));
	else {
		if (default_post_newsgroups[0])
			my_strncpy (group, default_post_newsgroups, sizeof (group));
		else {
			my_fprintf (stderr, "%s\n", txt_no_quick_newsgroups);
			return;
		}
	}

	/*
	 * Strip double newsgroups
	 */
	strip_double_ngs (group);

	/*
	 * Check if any of the newsgroups are moderated.
	 */
	strcpy (tmp, group);
	while (!done) {
		strcpy (buf, tmp);
		ptr = strchr (buf, ',');
		if (ptr != (char *) 0) {
			strcpy (tmp, ptr + 1);
			*ptr = '\0';
		} else
			done = TRUE;

		psGrp = psGrpFind (buf);
#ifdef DEBUG
		if (debug == 2)
			wait_message (1, "Group=[%s]", buf);
#endif
		if (!psGrp) {
			Raw (FALSE);
			my_fprintf (stderr, "\nGroup %s not found in active file. Exiting...\n", buf);
			return;
		}
		if (psGrp->moderated == 'x' || psGrp->moderated == 'n') {
			error_message(txt_cannot_post_group, psGrp->name);
			return;
		}

		if (psGrp->moderated == 'm') {
			sprintf (msg, txt_group_is_moderated, buf);
			if (prompt_yn (cLINES, msg, TRUE) != 1) {
				Raw (FALSE);
				error_message("Exiting...");
				return;
			}
		}
	}

	PRINT_LF();

	/* Only display leading characters of subject */
	if (strlen(default_post_subject) > DISPLAY_SUBJECT_LEN) {
		strncpy(tmp, default_post_subject, DISPLAY_SUBJECT_LEN);
		tmp[DISPLAY_SUBJECT_LEN] = '\0';
		strcat(tmp, " ...");
	} else
		strncpy(tmp, default_post_subject, sizeof(tmp));

	sprintf (buf, txt_post_subject, tmp);
	if (!prompt_string (buf, subj, HIST_POST_SUBJECT)) {
		Raw (FALSE);
		my_fprintf (stderr, "%s\n", txt_no_quick_subject);
		return;
	}
	if (strlen (subj))
		my_strncpy (default_post_subject, subj, sizeof (default_post_subject));
	else {
		if (default_post_subject[0])
			my_strncpy (subj, default_post_subject, sizeof (subj));
		else {
			Raw (FALSE);
			my_fprintf (stderr, "%s\n", txt_no_quick_subject);
			return;
		}
	}

	PRINT_LF();

	if ((fp = fopen (article, "w")) == NULL) {
		Raw (FALSE);
		perror_message (txt_cannot_open, article);
		return;
	}
	chmod (article, (S_IRUSR|S_IWUSR));

/* FIXME so that group only contains 1 group when finding an index number */
/* Does this count? */
	strcpy (buf, group);
	ptr = strchr (buf, ',');

	if (ptr)
		*ptr = '\0';

	psGrp = psGrpFind (group);
	get_from_name(from_name, psGrp);
#ifdef FORGERY
	make_path_header (line, from_name);
	msg_add_header ("Path", line);
#endif
	msg_add_header ("From", from_name);
	msg_add_header ("Subject", subj);
	msg_add_header ("Newsgroups", group);
	if (psGrp && psGrp->attribute->followup_to != (char *) 0)
		msg_add_header ("Followup-To", psGrp->attribute->followup_to);
	else {
		if (prompt_followupto)
			msg_add_header("Followup-To", "");
	}
	if (*reply_to)
		msg_add_header ("Reply-To", reply_to);

	if (psGrp && psGrp->attribute->organization != (char *) 0)
		msg_add_header ("Organization", random_organization(psGrp->attribute->organization));

	if (*my_distribution)
		msg_add_header ("Distribution", my_distribution);

	msg_add_header ("Summary", "");
	msg_add_header ("Keywords", "");
	msg_add_x_headers (msg_headers_file);

	if (psGrp)
		msg_add_x_headers (psGrp->attribute->x_headers);

	start_line_offset = msg_write_headers (fp);
	fprintf (fp, "\n");	/* add a newline to keep vi from bitching */
	start_line_offset += 2;
	msg_free_headers ();

	if (psGrp)
		start_line_offset += msg_add_x_body (fp, psGrp->attribute->x_body);

	msg_write_signature (fp, FALSE, &active[my_group[cur_groupnum]]);
	fclose (fp);

	ch = iKeyPostEdit;
	forever {
quick_post_article_loop:
		switch (ch) {
			case iKeyPostEdit:
				invoke_editor (article, start_line_offset);
				while (!check_article_to_be_posted (article, art_type, &lines) && repair_article(&ch))
					;
				if (ch == iKeyPostEdit)
					break;

			/* FALLTHROUGH */
			case iKeyQuit:
			case iKeyAbort:
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
			case iKeyPostPost2:
				wait_message (0, txt_posting);
				backup_article (article);
				if (submit_news_file (article)) {
					unlink(backup_article_name(article));
					Raw (FALSE);
					info_message (txt_art_posted);
					goto post_article_done;
				} else {
					ch = prompt_rejected();
					if (ch==iKeyPostPostpone)
						postpone_article(backup_article_name(article));
					else if (ch==iKeyPostEdit) {
						rename(backup_article_name(article), article);
						ch = iKeyPostEdit;
						goto quick_post_article_loop;
					} else {
						unlink(backup_article_name(article));
						rename_file (article, dead_article);
#ifdef M_UNIX
						if (keep_dead_articles)
							append_file (dead_articles, dead_article);
#endif
						Raw (FALSE);
						error_message (txt_art_rejected, dead_article);
						ReadCh ();
					}
				return;
				}
			case iKeyPostPostpone:
			  postpone_article(article);
			  goto post_article_postponed;

			default:
				break;
		}
		ch = prompt_to_edit();
	}

post_article_done:
	if (pcCopyArtHeader (HEADER_NEWSGROUPS, article, group)) {
		update_active_after_posting (group);

		if (pcCopyArtHeader (HEADER_SUBJECT, article, subj)) {

#if 0 /* this sucks! (urs) */
			quick_filter_select_posted_art (psGrp, subj);
#endif

			update_posted_info_file (group, 'w', subj);
		} else
			subj[0] = '\0';
	} else
		group[0] = '\0';

	if (keep_posted_articles)
		update_posted_msgs_file (article, userid);

	if (group[0] != '\0')
		my_strncpy (default_post_newsgroups, group, sizeof (default_post_newsgroups));

	if (subj[0] != '\0')
		my_strncpy (default_post_subject, subj, sizeof (default_post_subject));

	write_config_file (local_config_file);

post_article_postponed:
	if (unlink_article)
		unlink (article);

	return;
}

/*
 *  Post an article that is already written (for postponed articles)
 */

static void
post_existing_article (
	int ask,
	char* subject)
{
	char ch;
	char group[HEADER_LEN];
	char subj[HEADER_LEN];
	char prompt[HEADER_LEN];
	int art_type = GROUP_TYPE_NEWS;
	int lines;
	struct t_group *psGrp;

	if (!can_post) {
		info_message (txt_cannot_post);
		return;
	}

	if (ask)
		ch = iKeyPostEdit;
	else
		ch = iKeyPostPost;
	forever {
post_existing_article_loop:
		switch (ch) {
			case iKeyPostEdit:
				invoke_editor (article, 0);
				while (!check_article_to_be_posted (article, art_type, &lines) && repair_article(&ch))
					;
				if (ch == iKeyPostEdit)
					break;

			/* FALLTHROUGH */
			case iKeyQuit:
			case iKeyAbort:
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
			case iKeyPostPost2:
/* TODO - can't we use some generic routine for trimming with '... ? */
				sprintf(prompt, "Posting: %s", subject);
				if ((int)strlen(prompt) > cCOLS-5)
					prompt[cCOLS-5] = 0;
				strcat(prompt, " ...");
				wait_message (0, prompt);

				backup_article(article);

				if(!strchr(group, ','))
					psGrp=psGrpFind(group);
				else
					psGrp=NULL;

				if (submit_news_file (article)) {
					unlink(backup_article_name(article));
					Raw(FALSE);
					info_message (txt_art_posted);
					goto post_article_done;
				} else {
					ch = prompt_rejected();
					if (ch==iKeyPostPostpone)
						postpone_article(backup_article_name(article));
					else if (ch==iKeyPostEdit) {
						rename(backup_article_name(article), article);
						ch = iKeyPostEdit;
						goto post_existing_article_loop;
					} else {
						unlink(backup_article_name(article));
						rename_file (article, dead_article);
#ifdef M_UNIX
						if (keep_dead_articles)
							append_file (dead_articles, dead_article);
#endif
						Raw (FALSE);
						error_message (txt_art_rejected, dead_article);
						ReadCh ();
					}
					return;
				}
			case iKeyPostPostpone:
				postpone_article(article);
				goto post_article_postponed;

			default:
				break;
		}
		ch = prompt_to_edit();
	}

post_article_done:
	if (pcCopyArtHeader (HEADER_NEWSGROUPS, article, group)) {
		update_active_after_posting (group);

		/* Hmmm? What's the difference between subject and subj?  */
		if (pcCopyArtHeader (HEADER_SUBJECT, article, subj)) {
		   /* we currently do not add autoselect for
		    * crossposted postponed articles, since we don't
		    * know in which group the article was actually in
		    */
#if 0 /* this sucks! (urs) */
			if (!strchr(group, ',') && (psGrp=psGrpFind(group)))
				quick_filter_select_posted_art (psGrp, subj);
#endif

			update_posted_info_file (group, strncmp(subj, "Re: ", 4) ? 'w' : 'f', subj);
		} else
			subj[0] = '\0';
	} else
		group[0] = '\0';

	if (keep_posted_articles)
		update_posted_msgs_file (article, userid);

	if (group[0] != '\0')
		my_strncpy (default_post_newsgroups, group, sizeof (default_post_newsgroups));

	if (subj[0] != '\0')
		my_strncpy (default_post_subject, subj, sizeof (default_post_subject));

	write_config_file (local_config_file);

post_article_postponed:
	if (unlink_article)
		unlink (article);

	return;
}


static void
append_postponed_file (
	char *file,
	char *addr)
{
	char buf[LEN];
	FILE *fp_in, *fp_out;
	time_t epoch;

	fp_in = fopen (file, "r");
	if (fp_in != (FILE *) 0) {
		fp_out = fopen (postponed_articles_file, "a+");
		if (fp_out != (FILE *) 0) {
			time (&epoch);
			fprintf (fp_out, "From %s %s", addr, ctime (&epoch));
			while (fgets (buf, sizeof buf, fp_in) != (char *) 0) {
				if(strncmp(buf, "From ", 5)==0)
				  fputc('>', fp_out);
				fputs (buf, fp_out);
			}
			print_art_seperator_line (fp_out, FALSE);
			fclose (fp_out);
		}
		fclose (fp_in);
	}
}

/* count how any articles are in postponed.articles. Essentially, we
 * count '^From ' lines */

int count_postponed_articles(void)
{
	FILE *fp=fopen(postponed_articles_file,"r");
	int count=0;
	char line[HEADER_LEN];

	if (!fp)
		return 0;

	while (fgets(line, sizeof(line), fp)) {
		if (strncmp(line, "From ", 5)==0)
			count++;
	}
	fclose(fp);
	return count;
}

/* Copy the first postponed article and remove it from the postponed file */

static int
fetch_postponed_article(
	char tmp_file[],
	char subject[],
	char newsgroups[])
{
	FILE *in;
	FILE *out;
	FILE *tmp;
	char postponed_tmp[PATH_LEN];
	char line[HEADER_LEN];
	int first_article;
	int prev_line_nl;
	int anything_left;

	strcpy(postponed_tmp, postponed_articles_file);
	strcat(postponed_tmp, "_");
	in = fopen(postponed_articles_file, "r");
	out = fopen(tmp_file, "w");
	tmp = fopen(postponed_tmp, "w");
	
	if (in==NULL || out==NULL || tmp==NULL) {
		if (in)
			fclose(in);
		if (out)
			fclose(out);
		if (tmp)
			fclose(tmp);
		return FALSE;
	}

	fgets(line, sizeof(line), in);

	if(strncmp(line, "From ", 5)!=0) {
		fclose(in);
		fclose(out);
		fclose(tmp);
		return FALSE;
	}

	first_article = TRUE;
	prev_line_nl = FALSE;
	anything_left = FALSE;

	/* we have one minor problem with copying the article, we have added
	 * a newline at the end of the article and we have to remove that,
	 * but we don't know we are on the last line until we read the next
	 * line containing "From "
	 */
	
	while(fgets(line, sizeof(line), in)!=NULL) {
		if(strncmp(line, "From ", 5)==0)
			first_article=FALSE;
		if(first_article) {
			match_string(line, "Newsgroups: ", newsgroups, HEADER_LEN);
			match_string(line, "Subject: ", subject, HEADER_LEN);
			
			if(prev_line_nl)
				fputc('\n', out);

			if(strlen(line) && line[strlen(line)-1]=='\n') {
				prev_line_nl=TRUE;
				line[strlen(line)-1]='\0';
			} else
				prev_line_nl=FALSE;

			fputs(line, out);
		} else {
			fputs(line, tmp);
			anything_left=1;
		}
	}

	fclose(in);
	fclose(out);
	fclose(tmp);

	unlink(postponed_articles_file);

	if(anything_left)
		rename(postponed_tmp, postponed_articles_file);
	else
		unlink(postponed_tmp);

	return TRUE;
}


/* pick up any postponed article and ask if the user want to use it */

t_bool
pickup_postponed_articles(
	t_bool ask,
	t_bool all)
{
	char newsgroups[HEADER_LEN];
	char subject[HEADER_LEN];
	char question[HEADER_LEN];
	char ch = 0;
	int count = count_postponed_articles();
	int i;

	if (count==0) {
		if (!ask)
			info_message(txt_info_nopostponed);
		return FALSE;
	}

	sprintf(question, "do you want to see postponed articles (%d)? ", count);

	if (ask && prompt_yn(cLINES, question, TRUE) != 1)
		return FALSE;

	for (i = 0; i < count; i++) {
		if (!fetch_postponed_article(article, subject, newsgroups))
			return TRUE;

		if (!all) {
			ch = prompt_slk_response (iKeyPostponeYes, "\033qyYnA", sized_message(txt_postpone_repost, subject));

			if (ch == iKeyPostponeYesAll)
				all = 1;
		}

		/* No else here since all changes in previous if */
		if (all)
			ch = iKeyPostponeYesOverride;

		switch (ch)
		{
			case iKeyPostponeYes:
			case iKeyPostponeYesOverride:
				post_existing_article(ch == iKeyPostponeYes, subject);
				Raw(TRUE);
				break;
			case iKeyPostponeNo:
			case iKeyQuit:
			case iKeyAbort:
				append_postponed_file(article, userid);
				unlink(article);
				if (ch != iKeyPostponeNo)
					return TRUE;
		}
	}
	return TRUE;
}


static void
postpone_article(
	char *the_article)
{
	wait_message(3, txt_info_do_postpone);
	append_postponed_file(the_article, userid);
}

/*
 *  Post an original article (not a followup)
 */

int
post_article (
	char *group,
	int *posted_flag)
{
	FILE *fp;
	char ch;
	char subj[HEADER_LEN];
	char buf[HEADER_LEN], tmp[HEADER_LEN];
	char from_name[HEADER_LEN];
	int art_type = GROUP_TYPE_NEWS;
	int lines;
	int redraw_screen = FALSE;
	struct t_group *psGrp;
#ifdef FORGERY
	char line[HEADER_LEN];
#endif

	msg_init_headers ();

	psGrp = psGrpFind (group);
	if (!psGrp) {
		error_message (txt_not_in_active_file, group);
		return redraw_screen;
	}
	if (psGrp->attribute->mailing_list != (char *) 0)
		art_type = GROUP_TYPE_MAIL;

	if (!can_post && art_type == GROUP_TYPE_NEWS) {
		info_message (txt_cannot_post);
		return redraw_screen;
	}
	*posted_flag = FALSE;

	if (psGrp->moderated == 'x' || psGrp->moderated == 'n') {
		error_message(txt_cannot_post_group, psGrp->name);
		return redraw_screen;
	}

	if (psGrp->moderated == 'm') {
		sprintf (msg, txt_group_is_moderated, group);
		if (prompt_yn (cLINES, msg, TRUE) != 1) {
			clear_message ();
			return redraw_screen;
		}
	}
	/* Only display leading characters of subject */
	if (strlen(default_post_subject) > DISPLAY_SUBJECT_LEN) {
		strncpy(tmp, default_post_subject, DISPLAY_SUBJECT_LEN);
		tmp[DISPLAY_SUBJECT_LEN] = '\0';
		strcat(tmp, " ...");
	} else
		strncpy(tmp, default_post_subject, sizeof(tmp));

	sprintf (msg, txt_post_subject, tmp);

	if (!prompt_string (msg, subj, HIST_POST_SUBJECT)) {
		clear_message ();
		return redraw_screen;
	}

	if (strlen (subj))
		my_strncpy (default_post_subject, subj, sizeof (default_post_subject));
	else {
		if (default_post_subject[0])
			my_strncpy (subj, default_post_subject, sizeof (subj));
		else {
			info_message (txt_no_subject);
			return redraw_screen;
		}
	}

	wait_message (0, txt_post_an_article);

	if ((fp = fopen (article, "w")) == (FILE *) 0) {
		perror_message (txt_cannot_open, article);
		return redraw_screen;
	}
	chmod (article, (S_IRUSR|S_IWUSR));

	get_from_name(from_name, psGrp);
#ifdef FORGERY
	make_path_header (line, from_name);
	msg_add_header ("Path", line);
#endif
	msg_add_header ("From", from_name);
	msg_add_header ("Subject", subj);

	if (art_type == GROUP_TYPE_MAIL)
		msg_add_header ("To", psGrp->attribute->mailing_list);
	else
		msg_add_header ("Newsgroups", group);

	if (psGrp->attribute->followup_to != (char *) 0 && art_type == GROUP_TYPE_NEWS)
		msg_add_header ("Followup-To", psGrp->attribute->followup_to);
	else {
		if (prompt_followupto)
			msg_add_header("Followup-To", "");
	}

	if (*reply_to)
		msg_add_header ("Reply-To", reply_to);

	if (psGrp->attribute->organization != (char *) 0)
		msg_add_header ("Organization", random_organization(psGrp->attribute->organization));

	if (*my_distribution && art_type == GROUP_TYPE_NEWS)
		msg_add_header ("Distribution", my_distribution);

	msg_add_header ("Summary", "");
	msg_add_header ("Keywords", "");

	msg_add_x_headers (msg_headers_file);
	msg_add_x_headers (psGrp->attribute->x_headers);

	start_line_offset = msg_write_headers (fp);
	fprintf (fp, "\n");	/* add a newline to keep vi from bitching */
	start_line_offset++;
	msg_free_headers ();
	lines = msg_add_x_body (fp, psGrp->attribute->x_body);
	start_line_offset += lines;
	msg_write_signature (fp, FALSE, &active[my_group[cur_groupnum]]);
	fclose (fp);

	ch = iKeyPostEdit;
	forever {
post_article_loop:
		switch (ch) {
			case iKeyPostEdit:
				invoke_editor (article, start_line_offset);
				while (!check_article_to_be_posted (article, art_type, &lines) && repair_article(&ch))
					;
				redraw_screen = TRUE;
				if (ch == iKeyPostEdit)
					break;

			/* FALLTHROUGH */
			case iKeyQuit:
			case iKeyAbort:
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
			case iKeyPostPost2:
				wait_message (0, txt_posting);
				backup_article(article);
				if (art_type == GROUP_TYPE_NEWS) {
					if (submit_news_file (article))
						*posted_flag = TRUE;
				} else {
					if (submit_mail_file (article))
						*posted_flag = TRUE;
				}
				if (*posted_flag) {
					unlink(backup_article_name(article));
					info_message (txt_art_posted);
					sleep (1);
					goto post_article_done;
				} else {
					ch = prompt_rejected();
					if (ch==iKeyPostPostpone)
						postpone_article(backup_article_name(article));
					else if (ch==iKeyPostEdit) {
						rename(backup_article_name(article), article);
						ch = iKeyPostEdit;
						goto post_article_loop;
				  	} else {
					    unlink(backup_article_name(article));
					    rename_file (article, dead_article);
#ifdef M_UNIX
					    if (keep_dead_articles)
					      append_file (dead_articles, dead_article);
#endif
	 				    info_message (txt_art_rejected, dead_article);
					    ReadCh ();
					}
				 	return redraw_screen;
				}
			case iKeyPostPostpone:
			  postpone_article(article);
			  goto post_article_postponed;
			default:
				break;
		}
		ch = prompt_to_edit();
	}

post_article_done:
	if (*posted_flag) {
		if (art_type == GROUP_TYPE_NEWS) {
			if (pcCopyArtHeader (HEADER_NEWSGROUPS, article, buf))
				update_active_after_posting (buf);
		}
		if (pcCopyArtHeader (HEADER_SUBJECT, article, subj)) {

#if 0 /* this sucks! (urs) */
			quick_filter_select_posted_art (psGrp, subj);
#endif

			update_posted_info_file (group, 'w', subj);
		}
		if (keep_posted_articles)
			update_posted_msgs_file (article, userid);
	}
	my_strncpy (default_post_newsgroups, group, sizeof (default_post_newsgroups));
	my_strncpy (default_post_subject, subj, sizeof (default_post_subject));

post_article_postponed:
	if (unlink_article)
		unlink (article);

	return redraw_screen;
}


/* yeah, right, that's from the same Chris who is telling Jason he's
   doing obfuscated C :-) */
static void
appendid (
	char **where,
	const char **what)
{
	char *oldpos;

	oldpos = *where;
	while (**what && **what != '<')
		(*what)++;
	if (**what) {
		while (**what && **what != '>' && !isspace ((unsigned char)**what))
			*(*where)++ = *(*what)++;
		if (**what != '>')
			*where = oldpos;
		else {
			(*what)++;
			*(*where)++ = '>';
		}
	}
}

static t_bool
must_include (
	char *id)
{
	while (*id && *id != '<')
		id++;
	while (*id && *id != '>') {
		if (*++id != '_')
			continue;
		if (*++id != '-')
			continue;
		if (*++id != '_')
			continue;
		if (*++id == '@')
			return TRUE;
	}
	return FALSE;
}

static size_t
skip_id (
	const char *id)
{
	size_t skipped = 0;
	while (id[skipped] && isspace ((unsigned char)id[skipped]))
		skipped++;
	if (id[skipped]) {
		while (id[skipped] && !isspace ((unsigned char)id[skipped]))
			skipped++;
	}
	return skipped;
}

static t_bool
damaged_id (
	const char *id)
{
	while (*id && isspace ((unsigned char)*id))
		id++;
	if (*id != '<')
		return 1;
	while (*id && *id != '>')
		id++;
	if (*id != '>')
		return TRUE;
	return FALSE;
}

/*
 * A real crossposting test had to run on Newsgroups but we only have Xref in
 * t_article, so we use this.
 */
static t_bool
is_crosspost (
	char *xref)
{
	int count=0;
	for ( ; *xref; xref++)
		if (*xref==':')
			count++;
	return (count>=2) ? TRUE : FALSE;
}

/* Widespread news software like INN's nnrpd restricts the size of several
   headers, notably the references header, to 512 characters.  Oh well...
   guess that's what son-of-1036 calls a "desparate last resort" :-/
   From TIN's point of view, this could be HEADER_LEN. */
#define MAXREFSIZE 512

/* TODO - if we have the art[x] that we are following up to, then
 *        get_references(art[x].refptr) will give us the new refs line
 */

static void
join_references (
	char *buffer,
	char *oldrefs,
	const char *newref)
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
	char *b, *c, *d;
	const char *e;
	int space;

	b = (char *) malloc (strlen (oldrefs) + strlen (newref) + 64);
	c = b;
	e = oldrefs;
	space = 0;
	while (*e) {
		if (*e == ' ') {
			space++, *c++ = ' ', e++;	/* keep existing spaces */
			continue;
		} else if (*e != '<') {		/* strip everything besides spaces and */
			e++;	/* message-ids */
			continue;
		}
		if (damaged_id (e)) {	/* remove damaged message ids and mark
					   the gap if that's not already done */
			e += skip_id (e);
			while (space < 3)
				space++, *c++ = ' ';

			continue;
		}
		if (!space)
			*c++ = ' ';
		else
			space = 0;
		appendid (&c, &e);
	}
	while (space)
		c--, space--;	/* remove superfluous space at the end */
	*c++ = ' ';
	appendid (&c, &newref);
	*c = 0;

	/* now see if we need to remove ids */
	while (strlen (b) > MAXREFSIZE - 14) {	/* 14 = strlen("References: ")+2 */
		c = b;
		c += skip_id (c);	/* keep the first one */
		while (*c && must_include (c))
			c += skip_id (c); /* skip those marked with _-_ */
		d = c;
		c += skip_id (c);	/* ditch one */
		*d++ = ' ';
		*d++ = ' ';
		*d++ = ' ';	/* and mark this appropriately */
		while (*c == ' ')
			c++;
		strcpy (d, c);
	}

	strcpy (buffer, b);
	free (b);
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

int /* return code is currently ignored! */
post_response (
	char *group,
	int respnum,
	int copy_text,
	int with_headers)
{
	FILE *fp;
	char ch, *ptr;
	char bigbuf[HEADER_LEN];
	char buf[HEADER_LEN];
	char from_name[HEADER_LEN];
	char initials[64];
	int art_type = GROUP_TYPE_NEWS;
	int lines;
	int ret_code = POSTED_NONE;
	struct t_group *psGrp;

#ifdef FORGERY
	char line[HEADER_LEN];
#endif

	msg_init_headers ();

	wait_message (0, txt_post_a_followup);

	/*
	 *  Remove duplicates in Newsgroups and Followup-To line
	 */
	strip_double_ngs (note_h.newsgroups);
	if (*note_h.followup)
		strip_double_ngs (note_h.followup);

	if (*note_h.followup && STRCMPEQ(note_h.followup, "poster")) {
/*		clear_message ();*/
		ch = prompt_slk_response(iKeyPageMail, "\033mpyq", txt_resp_to_poster);
		switch (ch) {
			case iKeyPostPost:
			case iKeyPostPost2:
				goto ignore_followup_to_poster;
			case iKeyQuit:
			case iKeyAbort:
				return ret_code;
			default:
				break;
		}
		{
			char save_followup[HEADER_LEN];

			strcpy (save_followup, note_h.followup);
			*note_h.followup = '\0';
			find_reply_to_addr (respnum, buf, FALSE);
			mail_to_someone (respnum, buf, TRUE, FALSE, &ret_code);
			strcpy (note_h.followup, save_followup);
			return ret_code;
		}
	} else if (*note_h.followup && strcmp (note_h.followup, group) != 0
		   && strcmp (note_h.followup, note_h.newsgroups) != 0) {
		/* note that comparing newsgroups and followup-to isn't
		   really correct, since the order of the newsgroups may be
		   different, but testing that also isn't really worth
		   it. The main culprit for the duplication is tin <=1.22, BTW.
		 */
		MoveCursor (cLINES / 2, 0);
		CleartoEOS();
		center_line ((cLINES / 2) + 2, TRUE, txt_resp_redirect);
		MoveCursor ((cLINES / 2) + 4, 0);

		my_fputs ("    ", stdout);
		ptr = note_h.followup;
		while (*ptr) {
			if (*ptr != ',')
				my_fputc (*ptr, stdout);
			else
				my_fputs (cCRLF "    ", stdout);

			ptr++;
		}
		my_flush ();

		if (prompt_yn (cLINES, txt_continue, TRUE) != 1)
			return ret_code;
	}
ignore_followup_to_poster:
	if ((fp = fopen (article, "w")) == NULL) {
		perror_message (txt_cannot_open, article);
		return ret_code;
	}
	chmod (article, (S_IRUSR|S_IWUSR));

	psGrp = psGrpFind (group);
	get_from_name(from_name, psGrp);
#ifdef FORGERY
	make_path_header (line, from_name);
	msg_add_header ("Path", line);
#endif
	msg_add_header ("From", from_name);

	sprintf (bigbuf, "Re: %s", eat_re (note_h.subj, TRUE));
	msg_add_header ("Subject", bigbuf);

	if (psGrp && psGrp->attribute->x_comment_to && *note_h.from) {
		msg_add_header ("X-Comment-To", note_h.from);
	}
	if (*note_h.followup && strcmp (note_h.followup, "poster") != 0) {
		msg_add_header ("Newsgroups", note_h.followup);
		if (prompt_followupto)
			msg_add_header("Followup-To", (strchr(note_h.followup, ',') != (char *) 0) ? note_h.followup : "");
	} else {
		if (psGrp && psGrp->attribute->mailing_list) {
			msg_add_header ("To", psGrp->attribute->mailing_list);
			art_type = GROUP_TYPE_MAIL;
		} else {
			msg_add_header ("Newsgroups", note_h.newsgroups);
			if (prompt_followupto)
				msg_add_header("Followup-To",
	  			  (strchr(note_h.newsgroups, ',') != (char *) 0) ?
				    note_h.newsgroups : "");
			if (psGrp && psGrp->attribute->followup_to != (char *) 0) {
				msg_add_header ("Followup-To", psGrp->attribute->followup_to);
			} else {
				ptr = strchr (note_h.newsgroups, ',');
				if (ptr) {
					msg_add_header ("Followup-To", note_h.newsgroups);
				}
			}
		}
	}

	/*
	 * Append to References: line if its already there
	 */
	if (note_h.references[0]) {
		join_references (bigbuf, note_h.references, note_h.messageid);
		msg_add_header ("References", bigbuf);
	} else
		msg_add_header ("References", note_h.messageid);

	if (psGrp && psGrp->attribute->organization != (char *) 0)
		msg_add_header ("Organization", random_organization(psGrp->attribute->organization));

	if (*reply_to)
		msg_add_header ("Reply-To", reply_to);

	if (art_type != GROUP_TYPE_MAIL) {
		if (*note_h.distrib)
			msg_add_header ("Distribution", note_h.distrib);
		else if (*my_distribution)
			msg_add_header ("Distribution", my_distribution);
	}
	msg_add_x_headers (msg_headers_file);
	msg_add_x_headers (psGrp->attribute->x_headers);

	start_line_offset = msg_write_headers (fp);
	start_line_offset++;
	msg_free_headers ();
	lines = msg_add_x_body (fp, psGrp->attribute->x_body);
	start_line_offset += lines;

	if (copy_text) {
		if (arts[respnum].xref && is_crosspost(arts[respnum].xref)) {
			if (strfquote (CURR_GROUP.name, respnum, buf, sizeof (buf), xpost_quote_format))
				fprintf (fp, "%s\n", buf);
		} else if (strfquote (group, respnum, buf, sizeof (buf), (psGrp && psGrp->attribute->news_quote_format != (char *) 0) ? psGrp->attribute->news_quote_format : news_quote_format))
			fprintf (fp, "%s\n", buf);
		start_line_offset++;

		/*
		 * check if xpost_quote_format or news_quote_format
		 * is longer than 1 line and correct start_line_offset
		 */
		{
			char *s;

			for (s = buf; *s; s++) {
				if (*s == '\n')
					++start_line_offset;
			}
		}

		if (with_headers)
			fseek (note_fp, 0L, SEEK_SET);
		else
			fseek (note_fp, mark_body, SEEK_SET);
		get_initials (respnum, initials, sizeof (initials));
		copy_body (note_fp, fp,
			   (psGrp && psGrp->attribute->quote_chars != (char *) 0) ? psGrp->attribute->quote_chars : quote_chars,
			   initials, with_headers ? TRUE : quote_signatures);
	} else
		fprintf (fp, "\n");	/* add a newline to keep vi from bitching */

	msg_write_signature (fp, FALSE, &active[my_group[cur_groupnum]]);
	fclose (fp);

	ch = iKeyPostEdit;
	forever {
post_response_loop:
		switch (ch) {
			case iKeyPostEdit:
				invoke_editor (article, start_line_offset);
				while (!check_article_to_be_posted (article, art_type, &lines) && repair_article(&ch))
					;
				ret_code = POSTED_REDRAW;
				if (ch == iKeyPostEdit)
					break;
			/* FALLTHROUGH */
			case iKeyQuit:
			case iKeyAbort:
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
			case iKeyPostPost2:
				wait_message (0, txt_posting);
				backup_article(article);
				if (art_type == GROUP_TYPE_NEWS) {
					if (submit_news_file (article))
						ret_code = POSTED_OK;
				} else {
					if (submit_mail_file (article))
						ret_code = POSTED_OK;
				}
				if (ret_code == POSTED_OK) {
					info_message (txt_art_posted);
					unlink(backup_article_name(article));
					goto post_response_done;
				} else {
					ch = prompt_rejected();
					if (ch==iKeyPostPostpone) {
						postpone_article(backup_article_name(article));
					} else if (ch==iKeyPostEdit) {
						rename(backup_article_name(article), article);
						ch = iKeyPostEdit;
						goto post_response_loop;
					} else {
					    unlink(backup_article_name(article));
					    rename_file (article, dead_article);
#ifdef M_UNIX
					    if (keep_dead_articles)
					      append_file (dead_articles, dead_article);
#endif
					    info_message (txt_art_rejected, dead_article);
					    ReadCh ();
					}
					return ret_code;
				}
			case iKeyPostPostpone:
			  postpone_article(article);
			  goto post_response_postponed;

			default:
				break;
		}
		ch = prompt_to_edit();
	}

post_response_done:
	if (ret_code == POSTED_OK) {
		if (art_type == GROUP_TYPE_NEWS) {
			if (pcCopyArtHeader (HEADER_NEWSGROUPS, article, buf))
				update_active_after_posting (buf);
		}
		if (*note_h.followup && strcmp (note_h.followup, "poster") != 0) {
			if (pcCopyArtHeader (HEADER_SUBJECT, article, buf))
				update_posted_info_file (note_h.followup, 'f', buf);
		} else if (pcCopyArtHeader (HEADER_SUBJECT, article, buf)) {
			update_posted_info_file (note_h.newsgroups, 'f', buf);
			my_strncpy (default_post_newsgroups, note_h.newsgroups,
				    sizeof (default_post_newsgroups));
		}
		if (keep_posted_articles)
			update_posted_msgs_file (article, userid);
	}
	my_strncpy (default_post_subject, buf, sizeof (default_post_subject));

post_response_postponed:
	if (unlink_article)
		unlink (article);

	return ret_code;
}

int
mail_to_someone (
	int respnum,
	char *address,
	int mail_to_poster,
	int confirm_to_mail,
	int *mailed_ok)
{
	char nam[HEADER_LEN];
	char subject[HEADER_LEN];
	char mailreader_subject[PATH_LEN];	/* for calling external mailreader */
	char ch = iKeyPostSend;
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
#	ifdef APPEND_PID
		sprintf (nam+strlen(nam), ".%d", process_id);
#	endif /* APPEND_PID */
#endif /* VMS */
	if ((fp = fopen (nam, "w")) == NULL) {
		perror_message (txt_cannot_open, nam);
		return redraw_screen;
	}
	chmod (nam, (S_IRUSR|S_IWUSR));

	sprintf (subject, "(fwd) %s\n", note_h.subj);

	if (!use_mailreader_i) {	/* tin should start editor */

		msg_add_header ("To", mail_to);

		if (mail_to_poster) {
			sprintf (subject, "Re: %s\n", eat_re (note_h.subj, TRUE));
			msg_add_header ("Subject", subject);
		} else
			msg_add_header ("Subject", subject);

		if (auto_cc)
			msg_add_header ("Cc", userid);

		if (auto_bcc)
			msg_add_header ("Bcc", userid);
		/*
		 * remove duplicates from Newsgroups header
		 */
		strip_double_ngs (note_h.newsgroups);

		msg_add_header ("Newsgroups", note_h.newsgroups);

		if (*default_organization)
			msg_add_header ("Organization", random_organization(default_organization));

		if (*reply_to)
			msg_add_header ("Reply-To", reply_to);

		msg_add_x_headers (msg_headers_file);
	}
	start_line_offset = msg_write_headers (fp);
	start_line_offset++;
	msg_free_headers ();

	if (mail_to_poster) {
		ch = iKeyPostEdit;
		if (strfquote (CURR_GROUP.name, respnum, buf, sizeof (buf),
			       mail_quote_format)) {
			fprintf (fp, "%s\n", buf);
			start_line_offset++;
			{
				char *s;

				for (s = buf; *s; s++) {
					if (*s == '\n')
						++start_line_offset;
				}
			}
		}
		fseek (note_fp, mark_body, SEEK_SET);
		get_initials (respnum, initials, sizeof (initials));
		copy_body (note_fp, fp, quote_chars, initials, quote_signatures);
	} else {
		fseek (note_fp, 0L, SEEK_SET);
		fprintf (fp, "-- forwarded message --\n");
		copy_fp (note_fp, fp);
		fprintf (fp, "-- end of forwarded message --\n");
	}

	if (!use_mailreader_i)
		msg_write_signature (fp, TRUE, &active[my_group[cur_groupnum]]);

#ifdef WIN32
	putc ('\0', fp);
#endif
	fclose (fp);

	if (use_mailreader_i) {	/* user wants to use his own mailreader */
		ch = iKeyAbort;
		redraw_screen = TRUE;
		sprintf (mailreader_subject, "Re: %s", eat_re (note_h.subj, TRUE));
		strfmailer (mailer, mailreader_subject, mail_to, nam, buf, sizeof (buf), default_mailer_format);
		if (!invoke_cmd (buf))
			error_message (txt_command_failed_s, buf);
	}
	forever {
		if (confirm_to_mail && (!use_mailreader_i))
			ch = prompt_to_send(subject);

		switch (ch) {
			case iKeyPostEdit:
				invoke_editor (nam, start_line_offset);
				if (!pcCopyArtHeader (HEADER_SUBJECT, nam, subject))
					subject[0] = '\0';
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

			case iKeyQuit:
			case iKeyAbort:
				if (unlink_article)
					unlink (nam);
				clear_message ();
				*mailed_ok = FALSE;
				return redraw_screen;

			case iKeyPostSend:
			case iKeyPostSend2:
				/*
				 *  Open letter and get the To:  line in
				 *  case they changed it with the editor
				 */
				*mailed_ok = submit_mail_file (nam);
				if (*mailed_ok)
					goto mail_to_someone_done;
				else
					break;

			default:
				break;
		}
		if (mail_to_poster)
			ch = prompt_to_send(subject);
	}

mail_to_someone_done:
	if (unlink_article)
		unlink (nam);

	return redraw_screen;
}

int
mail_bug_report (void) /* return value is always ignored */
{
	char buf[LEN], nam[100];
	const char *domain;
	char ch;
	char mail_to[HEADER_LEN];
	char subject[HEADER_LEN];
	FILE *fp;
	t_bool is_longfiles, is_nntp, is_nntp_only, is_debug;

	msg_init_headers ();

	wait_message (2, txt_mail_bug_report);

	joinpath (nam, homedir, ".bugreport");
#ifdef APPEND_PID
	sprintf (nam+strlen(nam), ".%d", process_id);
#endif /* APPEND_PID */

	if ((fp = fopen (nam, "w")) == NULL) {
		perror_message (txt_cannot_open, nam);
		return FALSE;
	}
	chmod (nam, (S_IRUSR|S_IWUSR));

	if (!use_mailreader_i) {	/* tin should start editor */
		sprintf (buf, "%s%s", bug_addr, add_addr);
		msg_add_header ("To", buf);

		sprintf (subject, "BUG REPORT %s\n", page_header);
		msg_add_header ("Subject", subject);

		if (auto_cc)
			msg_add_header ("Cc", userid);

		if (auto_bcc)
			msg_add_header ("Bcc", userid);

		if (*default_organization)
			msg_add_header ("Organization", random_organization(default_organization));

		if (*reply_to)
			msg_add_header ("Reply-To", reply_to);

		msg_add_x_headers (msg_headers_file);
	}
	start_line_offset = msg_write_headers (fp);
	start_line_offset++;
	msg_free_headers ();

#ifdef HAVE_SYS_UTSNAME_H
#	ifdef _AIX
	fprintf (fp, "BOX1: %s %s.%s", system_info.sysname, system_info.version, system_info.release);
#	else
	fprintf (fp, "BOX1: %s %s %s", system_info.machine, system_info.sysname, system_info.release);
#	endif /* AIX */
	start_line_offset += 2;
#else
	fprintf (fp, "Please enter the following information:\n");
	fprintf (fp, "BOX1: Machine+OS:\n");
#endif /* HAVE_SYS_UTSNAME_H */

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
#ifdef DOMAIN_NAME
	domain = DOMAIN_NAME;
#else
	domain = 0;
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
	fprintf (fp, "CFG3: debug=%d domain=[%s]\n",
		 is_debug,
		 (domain ? domain : ""));
	fprintf (fp, "CFG4: threading=%d\n", default_thread_arts);
	start_line_offset++;

	if (*bug_nntpserver1) {
		fprintf(fp, "NNTP1: %s\n", bug_nntpserver1);
		start_line_offset++;
	}
	if (*bug_nntpserver2) {
		fprintf(fp, "NNTP2: %s\n", bug_nntpserver2);
		start_line_offset++;
	}

	fprintf (fp, "\nPlease enter bug report/gripe/comment:\n");

	start_line_offset += 6;

	if (!use_mailreader_i)
		msg_write_signature (fp, TRUE, &active[my_group[cur_groupnum]]);

#ifdef WIN32
	putc ('\0', fp);
#endif
	fclose (fp);

	if (use_mailreader_i) {	/* user wants to use his own mailreader */
		ch = iKeyAbort;
		sprintf (subject, "BUG REPORT %s", page_header);
		sprintf (mail_to, "%s%s", bug_addr, add_addr);
		strfmailer (mailer, subject, mail_to, nam, buf, sizeof (buf), default_mailer_format);
		if (!invoke_cmd (buf))
			error_message (txt_command_failed_s, buf);
	} else
		ch = iKeyPostEdit;

	forever {
		switch (ch) {
			case iKeyPostEdit:
				invoke_editor (nam, start_line_offset);
				if (!pcCopyArtHeader (HEADER_SUBJECT, nam, subject))
					subject[0] = '\0';
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

			case iKeyQuit:
			case iKeyAbort:
				unlink (nam);
				clear_message ();
				return TRUE;

			case iKeyPostSend:
			case iKeyPostSend2:
				sprintf (msg, txt_mail_bug_report_confirm, bug_addr, add_addr);
				if (prompt_yn (cLINES, msg, FALSE) == 1) {
					if (pcCopyArtHeader (HEADER_TO, nam, mail_to) && pcCopyArtHeader (HEADER_SUBJECT, nam, subject)) {
						wait_message (0, txt_mailing_to, mail_to);
						rfc15211522_encode (nam, txt_mime_types[mail_mime_encoding], mail_8bit_header, TRUE);
						strfmailer (mailer, subject, mail_to, nam, buf, sizeof (buf), default_mailer_format);
						if (invoke_cmd (buf)) {
							info_message (txt_mailed, 1, IS_PLURAL(1));
							goto mail_bug_report_done;
						} else
							error_message (txt_command_failed_s, buf);
					}
					break;	/* an error occurred */
				} else
					goto mail_bug_report_done;

			default:
				break;
		}
		ch = prompt_to_send(subject);
	}

mail_bug_report_done:
	unlink (nam);

	return TRUE;
}

int
mail_to_author (
	char *group,
	int respnum,
	int copy_text,
	int with_headers) /* return value is always ignored */
{
	char buf[LEN];
	char from_addr[HEADER_LEN];
	char nam[100];
	char mail_to[HEADER_LEN];
	char subject[HEADER_LEN];
	char bigbuf[HEADER_LEN];
	char mailreader_subject[PATH_LEN];	/* for calling external mailreader */
	char initials[64];
	char ch;
	FILE *fp;
	int redraw_screen = FALSE;
	t_bool spamtrap_found = FALSE;

	msg_init_headers ();

	wait_message (0, txt_reply_to_author);

#ifdef VMS
	joinpath (nam, homedir, "letter.");
#else
	joinpath (nam, homedir, ".letter");
#	ifdef APPEND_PID
		sprintf (nam+strlen(nam), ".%d", process_id);
#	endif /* APPEND_PID */
#endif /* VMS */
	if ((fp = fopen (nam, "w")) == NULL) {
		perror_message (txt_cannot_open, nam);
		return redraw_screen;
	}
	chmod (nam, (S_IRUSR|S_IWUSR));

	sprintf (subject, "Re: %s\n", eat_re (note_h.subj, TRUE));

	if (!use_mailreader_i) {	/* tin should start editor */
		find_reply_to_addr (respnum, from_addr, FALSE);

		msg_add_header ("To", from_addr);
		spamtrap_found = check_for_spamtrap(from_addr);
		msg_add_header ("Subject", subject);

		if (auto_cc)
			msg_add_header ("Cc", userid);

		if (auto_bcc)
			msg_add_header ("Bcc", userid);

		/*
		 * remove duplicates from Newsgroups header
		 */
		strip_double_ngs (note_h.newsgroups);

		msg_add_header ("Newsgroups", note_h.newsgroups);

		/*
		 * Write References and Message-Id as In-Reply-To to the mail
		 */
		if (note_h.references[0]) {
			join_references (bigbuf, note_h.references, note_h.messageid);
			msg_add_header ("In-Reply-To", bigbuf);
		} else
			msg_add_header ("In-Reply-To", note_h.messageid);

		if (*default_organization)
			msg_add_header ("Organization", random_organization(default_organization));

		if (*reply_to)
			msg_add_header ("Reply-To", reply_to);

		msg_add_x_headers (msg_headers_file);
	}
	start_line_offset = msg_write_headers (fp);
	start_line_offset++;
	msg_free_headers ();

	if (copy_text) {
		if (strfquote (group, respnum, buf, sizeof (buf), mail_quote_format)) {
			fprintf (fp, "%s\n", buf);
			start_line_offset++;
			{
				char *s;

				for (s = buf; *s; s++) {
					if (*s == '\n')
						++start_line_offset;
				}
			}
		}
		if (with_headers)
			fseek (note_fp, 0L, SEEK_SET);
		else
			fseek (note_fp, mark_body, SEEK_SET);
		get_initials (respnum, initials, sizeof (initials));
		copy_body (note_fp, fp, quote_chars, initials,
		    with_headers ? TRUE : quote_signatures);
	} else
		fprintf (fp, "\n");	/* add a newline to keep vi from bitching */

	if (!use_mailreader_i)
		msg_write_signature (fp, TRUE, &active[my_group[cur_groupnum]]);

#ifdef WIN32
	putc ('\0', fp);
#endif
	fclose (fp);

	if (spamtrap_found) {
		ch = prompt_to_continue();
		switch (ch) {
			case iKeyPostAbort:
			case iKeyAbort:
				unlink(nam);
				clear_message();
				return redraw_screen;

			case iKeyPostContinue:
				break;
			/* the user wants to continue anyway, so we do nothing special here */
			default:
				break;
		}
	}

	if (use_mailreader_i) {	/* user wants to use his own mailreader for reply */
		ch = iKeyAbort;
		sprintf (mailreader_subject, "Re: %s", eat_re (note_h.subj, TRUE));
		find_reply_to_addr (respnum, mail_to, TRUE);
		strfmailer (mailer, mailreader_subject, mail_to, nam, buf, sizeof (buf), default_mailer_format);
		if (!invoke_cmd (buf))
			error_message (txt_command_failed_s, buf);
	} else
		ch = iKeyPostEdit;

	forever {
		switch (ch) {
			case iKeyPostEdit:
				invoke_editor (nam, start_line_offset);
				if (!pcCopyArtHeader (HEADER_SUBJECT, nam, subject))
					subject[0] = '\0';
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
				if (pcCopyArtHeader (HEADER_TO, nam, mail_to) && pcCopyArtHeader (HEADER_SUBJECT, nam, subject))
					invoke_pgp_mail (nam, mail_to);
				break;
#endif

			case iKeyQuit:
			case iKeyAbort:
				unlink (nam);
				clear_message ();
				return redraw_screen;

			case iKeyPostSend:
			case iKeyPostSend2:
				my_strncpy (mail_to, arts[respnum].from, sizeof (mail_to));
				if (pcCopyArtHeader (HEADER_TO, nam, mail_to) && pcCopyArtHeader (HEADER_SUBJECT, nam, subject)) {
					t_bool ismail=TRUE;
					wait_message (0, txt_mailing_to, mail_to);
					checknadd_headers (nam);
					rfc15211522_encode (nam, txt_mime_types[mail_mime_encoding], mail_8bit_header,ismail);
					strfmailer (mailer, subject, mail_to, nam, buf, sizeof (buf), default_mailer_format);
					if (invoke_cmd (buf)) {
						info_message (txt_mailed, 1, IS_PLURAL(1));
						goto mail_to_author_done;
					} else
						error_message (txt_command_failed_s, buf);
				}
				break;	/* an error occurred */
			default:
				break;
		}
		ch = prompt_to_send(subject);
	}

mail_to_author_done:
	update_posted_info_file (group, 'r', subject);
	unlink (nam);

	return redraw_screen;
}

/*
 * compare the given e-mail address with a list of components in $SPAMTRAP
 */

static t_bool
check_for_spamtrap(char *addr)
{
	char *env;
	char *ptr;

	if ((env = getenv("SPAMTRAP")) != (char *) 0) {
		while (strlen(env)) {
			ptr = strchr(env, ',');
			if (ptr != NULL)
				*ptr = '\0';
			if (strcasestr(addr, env) != NULL)
				return TRUE;
			env += strlen(env);
			if (ptr != NULL)
				env++;
		}
	}
	return FALSE;
}

/*
 *  Read a file grabbing the value of the specified mail header line
 */

static int
pcCopyArtHeader (
	int iHeader,
	char *pcArt,
	char *result)
{
	FILE *fp;
	char buf[HEADER_LEN];
	char buf2[HEADER_LEN];
	char *q;
	const char *p;
	int found = FALSE;
	int was_to = FALSE;
	int c;
	static char header[HEADER_LEN];

	*header = '\0';

	if ((fp = fopen (pcArt, "r")) == (FILE *) 0) {
		perror_message (txt_cannot_open, pcArt);
		return FALSE;
	}
	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		q = strrchr (buf, '\n');
		if (q != 0)
			*q = '\0';

		if (*buf == '\0')
			break;

		/* check for continued header */
		while ((c = peek_char (fp)) != EOF && isspace (c) && c != '\n' && strlen (buf) < sizeof (buf) - 1) {
			if (strlen (buf) > 0 && buf[strlen (buf) - 1] == '\n')
				buf[strlen (buf) - 1] = '\0';

			fgets (buf + strlen (buf), sizeof buf - strlen (buf), fp);
		}

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
				} else
					was_to = FALSE;

				break;

			case HEADER_NEWSGROUPS:
				if (match_string (buf, "Newsgroups: ", header, sizeof (header)))
					found = TRUE;

				break;

			case HEADER_SUBJECT:
				if (STRNCMPEQ(buf, "Subject: ", 9)) {
					my_strncpy (header, &buf[9], sizeof (header));
					found = TRUE;
				}
				break;

			default:
				break;
		}
	}
	fclose (fp);

	if (found) {
		if (header[0] == ' ')
			p = &header[1];
		else
			p = header;

		(void) strcpy (result, rfc1522_decode (p));
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

t_bool
cancel_article (
	struct t_group *group,
	struct t_article *art,
	int respnum)
{
	char ch, ch_default = iKeyPostCancel;
	char buf[HEADER_LEN];
	char cancel[HEADER_LEN];
	char from_name[HEADER_LEN];
#ifdef FORGERY
	char line[HEADER_LEN];
	char line2[HEADER_LEN];
	char author = TRUE;
#else
	char user_name[128];
	char full_name[128];
#endif
	FILE *fp;
	char option = iKeyPostCancel;
	char option_default = iKeyPostCancel;
	int init = TRUE;
	int oldraw;
	t_bool redraw_screen = TRUE;

	msg_init_headers ();

	/*
	 * Check if news / mail / save group
	 */
	if (group->type == GROUP_TYPE_MAIL || group->type == GROUP_TYPE_SAVE) {
#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
		vGrpDelMailArt (art);
#endif /* !INDEX_DAEMON && HAVE_MH_MAIL_HANDLING */
		return FALSE;
	}

	get_from_name (from_name, group);
#ifdef FORGERY
	make_path_header (line, from_name);
#else
	get_user_info (user_name, full_name);
#endif
#ifdef DEBUG
	if (debug == 2)
		error_message ("From=[%s]  Cancel=[%s]", art->from, from_name);
#endif
	if (!strstr (from_name, art->from)) {
#ifdef FORGERY
		author = FALSE;
#else
		wait_message (3, txt_art_cannot_cancel);
		return redraw_screen;
#endif
	} else {
		option = prompt_slk_response (option_default, "\033dqs", sized_message(txt_cancel_article, art->subject));

		switch (option) {
			case iKeyPostCancel:
				break;
			case iKeyPostSupersede:
				repost_article (note_h.newsgroups, art, respnum, TRUE);
				return redraw_screen;
			default:
				return redraw_screen;
		}
	}

	clear_message ();

	joinpath (cancel, homedir, ".cancel");
#ifdef APPEND_PID
	sprintf (cancel+strlen(cancel), ".%d", process_id);
#endif /* APPEND_PID */

	if ((fp = fopen (cancel, "w")) == (FILE *) 0) {
		perror_message (txt_cannot_open, cancel);
		return redraw_screen;
	}
	chmod (cancel, (S_IRUSR|S_IWUSR));

#ifdef FORGERY
	if (!author) {
		sprintf (line2, "cyberspam!%s", line);
		msg_add_header ("Path", line2);
	} else
		msg_add_header ("Path", line);

	if (art->name)
		sprintf (line, "%s <%s>", art->name, art->from);
	else
		sprintf (line, "<%s>", art->from);

	msg_add_header ("From", line);
	if (!author) {
		sprintf (line, "<cancel.%s", note_h.messageid + 1);
		msg_add_header ("Message-ID", line);
		msg_add_header ("X-Cancelled-By", from_name);
	}
#else
	msg_add_header ("From", from_name);
#endif
	sprintf (buf, "cmsg cancel %s", note_h.messageid);
	msg_add_header ("Subject", buf);

	/*
	 * remove duplicates from Newsgroups header
	 */
	strip_double_ngs (note_h.newsgroups);

	msg_add_header ("Newsgroups", note_h.newsgroups);
	if (prompt_followupto)
		msg_add_header("Followup-To", "");
	sprintf (buf, "cancel %s", note_h.messageid);
	msg_add_header ("Control", buf);

	if (group->moderated == 'm')
		msg_add_header ("Approved", from_name);

	if (group && group->attribute->organization != (char *) 0)
		msg_add_header ("Organization", random_organization(group->attribute->organization));

	if (*note_h.distrib)
		msg_add_header ("Distribution", note_h.distrib);
	else if (*my_distribution)
		msg_add_header ("Distribution", my_distribution);

	msg_write_headers (fp);
	msg_free_headers ();

#ifdef FORGERY
	if (author)
		fprintf (fp, txt_article_cancelled);
	else {
		fputc ('\n', fp);
		fseek (note_fp, 0L, SEEK_SET);
		copy_fp (note_fp, fp);
	}
	fclose (fp);
	invoke_editor (cancel, start_line_offset);
	redraw_screen = TRUE;
#else
	fprintf (fp, txt_article_cancelled);
	fclose (fp);
#endif /* FORGERY */

	oldraw = RawState ();
	setup_check_article_screen (&init);

#ifdef FORGERY
	if (!author) {
		my_fprintf (stderr, txt_warn_cancel_forgery);
		my_fprintf (stderr, "From: %s\n", note_h.from);
	} else
		my_fprintf (stderr, txt_warn_cancel);
#else
	my_fprintf (stderr, txt_warn_cancel);
#endif /* FORGERY */

	my_fprintf (stderr, "Subject: %s\n", note_h.subj);
	my_fprintf (stderr, "Date: %s\n", note_h.date);
	my_fprintf (stderr, "Message-ID: %s\n", note_h.messageid);
	my_fprintf (stderr, "Newsgroups: %s\n", note_h.newsgroups);
	Raw (oldraw);

	forever {
		ch = prompt_slk_response(ch_default, "\033deq", sized_message(txt_quit_cancel, note_h.subj));
		switch (ch) {
			case iKeyPostEdit:
				invoke_editor (cancel, start_line_offset);
				break;

			case iKeyPostCancel:
				wait_message (1, txt_cancelling_art);
				if (submit_news_file (cancel)) {
					info_message (txt_art_cancel);
					if (pcCopyArtHeader (HEADER_SUBJECT, cancel, buf))
						update_posted_info_file (group->name, iKeyPostCancel, buf);
					unlink (cancel);
					return redraw_screen;
				} else {
					error_message (txt_command_failed_s, cancel);
					break;
				}

			case iKeyQuit:
			case iKeyAbort:
				unlink (cancel);
				clear_message ();
				return redraw_screen;
			default:
				break;
		}
	}
}

/*
 * Repost an already existing article to another group (ie. local group)
 */

#define FromSameUser ((strstr (from_name, arts[respnum].from)) != 0)

#ifndef FORGERY
#	define NotSuperseding (!supersede || (supersede && (!FromSameUser)))
#	define Superseding    (supersede && FromSameUser)
#else
#	define NotSuperseding (!supersede)
#	define Superseding    (supersede)
#endif

int
repost_article (
	char *group,
	struct t_article *art,
	int respnum,
	int supersede)
{
	FILE *fp;
	char *ptr;
	char buf[HEADER_LEN];
	char tmp[HEADER_LEN];
	char from_name[HEADER_LEN];
	char user_name[128];
	char full_name[128];
	char ch;
	char ch_default = iKeyPostPost;
	t_bool done = FALSE;
	int force_command = FALSE;
	int ret_code = POSTED_NONE;
	struct t_group *psGrp, *tmpGrp = NULL;
#ifdef FORGERY
	char line[HEADER_LEN];
#endif

	msg_init_headers ();

	/*
	 * remove duplicates from Newsgroups header
	 */
	strip_double_ngs (note_h.newsgroups);

	/*
	 * Check if any of the newsgroups are moderated.
	 */
	strcpy (tmp, group);
	while (!done) {
		strcpy (buf, tmp);
		ptr = strchr (buf, ',');
		if (ptr != (char *) 0) {
			strcpy (tmp, ptr + 1);
			*ptr = '\0';
		} else
			done = TRUE;

		psGrp = psGrpFind (buf);

		if (psGrp && !tmpGrp)
			tmpGrp = psGrp;
#ifdef DEBUG
		if (debug == 2)
			wait_message (1, "Group=[%s]", buf);
#endif
		if (psGrp && psGrp->moderated == 'm') {
			sprintf (msg, txt_group_is_moderated, buf);
			if (prompt_yn (cLINES, msg, TRUE) != 1) {
				wait_message (3, txt_art_not_posted);
				return POSTED_NONE;
			}
		}
	}

	if (!tmpGrp) {
		error_message (txt_not_in_active_file, group);
		return POSTED_NONE;
	}


/* FIXME so that group only contains 1 group when finding an index number */
/* Does this count? */
	psGrp = tmpGrp;

	if ((fp = fopen (article, "w")) == (FILE *) 0) {
		perror_message (txt_cannot_open, article);
		return ret_code;
	}
	chmod (article, (S_IRUSR|S_IWUSR));

	if (supersede) {
		get_user_info (user_name, full_name);
		get_from_name (from_name, psGrp);
#ifndef FORGERY
		if (FromSameUser) {
			msg_add_header ("From", from_name);
			if (*reply_to)
				msg_add_header ("Reply-To", reply_to);
#else
		{
			make_path_header (line, from_name);
			msg_add_header ("Path", line);

			if (art->name)
				sprintf (line, "%s <%s>", art->name, arts[respnum].from);
			else
				sprintf (line, "<%s>", arts[respnum].from);

			msg_add_header ("From", line);
			find_reply_to_addr (respnum, line, FALSE);
			msg_add_header ("Reply-To", line);
			msg_add_header ("X-Superseded-By", from_name);

			if (note_h.org[0])
				msg_add_header ("Organization", note_h.org);

			sprintf (line, "<supersede.%s", note_h.messageid + 1);
			msg_add_header ("Message-ID", line);
#endif
			msg_add_header ("Supersedes", note_h.messageid);

			if (note_h.followup[0])
				msg_add_header ("Followup-To", note_h.followup);

			if (note_h.keywords[0])
				msg_add_header ("Keywords", note_h.keywords);

			if (note_h.summary[0])
				msg_add_header ("Summary", note_h.summary);

			if (*note_h.distrib)
				msg_add_header ("Distribution", note_h.distrib);
		}
	} else { /* !supersede */
		get_user_info (user_name, full_name);
		get_from_name (from_name, psGrp);
		msg_add_header ("From", from_name);
		if (*reply_to)
			msg_add_header ("Reply-To", reply_to);
	}
	msg_add_header ("Subject", note_h.subj);
	msg_add_header ("Newsgroups", group);

	if (note_h.references[0]) {
		/*
		 * calling join_references prevents repost_article
		 * to fail if References: contains a double space
		 * between 2 msgids - what it does not do is
		 * adding the msgid of the original article to the
		 * References header - is this needed?
		 */
		join_references (buf, note_h.references, "");
		msg_add_header ("References", buf);
	}
	if (NotSuperseding) {
		if (psGrp->attribute->organization != (char *) 0)
			msg_add_header ("Organization", random_organization(psGrp->attribute->organization));
		else if (*default_organization)
			msg_add_header ("Organization", random_organization(default_organization));

		if (*reply_to)
			msg_add_header ("Reply-To", reply_to);

		if (*note_h.distrib)
			msg_add_header ("Distribution", note_h.distrib);
		else if (*my_distribution)
			msg_add_header ("Distribution", my_distribution);

	}
	start_line_offset = msg_write_headers (fp);
	start_line_offset++;
	msg_free_headers ();

	if (NotSuperseding) {
		fprintf (fp, txt_article_reposted1, note_h.newsgroups);

		if (art->name)
			fprintf (fp, txt_article_reposted2a, art->name, art->from);
		else
			fprintf (fp, txt_article_reposted2b, art->from);

		fprintf (fp, "\n[ Posted on %s ]\n\n", note_h.date);
	}
	fseek (note_fp, mark_body, SEEK_SET);
	copy_fp (note_fp, fp);

/* only append signature when NOT superseeding own articles */
	if (NotSuperseding && signature_repost)
		msg_write_signature (fp, FALSE, psGrp);

	fclose (fp);

	/* on supersede change default-key */
	if (Superseding) {
		ch_default = iKeyPostEdit;
		force_command = TRUE;
	}
	forever {
repost_article_loop:
		if (!force_command)
			ch = prompt_slk_response (ch_default, POST_KEYS, sized_message(txt_quit_edit_xpost, note_h.subj));
		else
			ch = ch_default;

		switch (ch) {
			case iKeyPostEdit:
				invoke_editor (article, start_line_offset);
				force_command = FALSE;
				if (!pcCopyArtHeader (HEADER_SUBJECT, article, note_h.subj)) {
					note_h.subj[0] = '\0';		/* no subject line, re-enter editor */
					ch_default = iKeyPostEdit;
					/*
					 * To enforce re-entering the editor immediately when no
					 * subject is given, uncomment the following line
					 */
					 /* force_command = TRUE */
				} else {
					ret_code = POSTED_REDRAW;
					ch_default = iKeyPostPost;
				}
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

			case iKeyQuit:
			case iKeyAbort:
				if (unlink_article)
					unlink (article);
				clear_message ();
				return ret_code;

			case iKeyPostPost:
			case iKeyPostPost2:
				if (Superseding)
					wait_message (2, txt_superseding_art);
				else
					wait_message (2, txt_repost_an_article);

				backup_article(article);

				if (submit_news_file (article)) {
					info_message (txt_art_posted);
					unlink(backup_article_name(article));
					ret_code = POSTED_OK;
					goto repost_done;
				} else {
					ch = prompt_rejected();
					if (ch==iKeyPostPostpone)
						postpone_article(backup_article_name(article));
					else if (ch==iKeyPostEdit) {
						rename(backup_article_name(article), article);
						ch = iKeyPostEdit;
						goto repost_article_loop;
					} else {
						unlink(backup_article_name(article));
						rename_file (article, dead_article);
#ifdef M_UNIX
						if (keep_dead_articles)
							append_file (dead_articles, dead_article);
#endif
						wait_message (3, txt_art_rejected, dead_article);
					}
				return ret_code;
			}
		case iKeyPostPostpone:
			postpone_article(article);
			goto repost_postponed;
		default:
			break;
		}
	}

repost_done:
		if (pcCopyArtHeader (HEADER_NEWSGROUPS, article, buf)) {
			update_active_after_posting (buf);

			if (pcCopyArtHeader (HEADER_SUBJECT, article, buf))
				update_posted_info_file (psGrp->name , 'x', buf);
		}
repost_postponed:
		if (unlink_article)
			unlink (article);

	return ret_code;
}

static void
msg_add_x_headers (
	char *headers)
{
	char *ptr;
	char file[PATH_LEN];
	char line[HEADER_LEN];
	FILE *fp;

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
		if (!strfpath (headers, file, sizeof (file), homedir, (char *) 0, (char *) 0, (char *) 0))
			strcpy (file, headers);

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

static int
msg_add_x_body (
	FILE *fp_out,
	char *body)
{
	char *ptr;
	char file[PATH_LEN];
	char line[HEADER_LEN];
	FILE *fp;
	int wrote = 0;

	if (body) {
		if (body[0] != '/' && body[0] != '~') {
			strcpy (line, body);
			ptr = strrchr (line, '\n');
			if (ptr)
				*ptr = '\0';

			fprintf (fp_out, "%s\n", line);
			wrote++;
		} else {
			if (!strfpath (body, file, sizeof (file), homedir, (char *) 0, (char *) 0, (char *) 0))
				strcpy (file, body);

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

static void
modify_headers (
	char *line)
{
	char buf[HEADER_LEN];
	char *chr;
	char *chr2;

	if (strncasecmp (line, "Newsgroups: ", 12) == 0) {
		chr = strpbrk (line, "\t ");
		while ((strchr ("\t ", *chr)) != (char *) 0)
			chr++;

		chr2 = strchr (chr, '\n');
		*chr2 = '\0';
		strip_double_ngs (chr);
		strcpy (found_newsgroups, chr);
		sprintf (line, "Newsgroups: %s\n", found_newsgroups);
	}
	if (strncasecmp (line, "Followup-To: ", 11) == 0) {
		chr = strpbrk (line, "\t ");
		while ((strchr ("\t ", *chr)) != (char *) 0)
			chr++;

		chr2 = strchr (chr, '\n');
		*chr2 = '\0';
		strip_double_ngs (chr);
		strcpy (buf, chr);
		if ((*found_newsgroups == '\0') || (strcasecmp (found_newsgroups, buf)))
			sprintf (line, "Followup-To: %s\n", buf);
		else
			*line = '\0';
	}
}

void
checknadd_headers (
	char *infile)
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
				if (!gotit && line[0] == '\n') {
					if (advertising)
#ifdef HAVE_SYS_UTSNAME_H
#ifdef _AIX
						fprintf (fp_out, "User-Agent: %s/%s-%s (%s) (%s/%s-%s)\n\n",
							PRODUCT, VERSION, RELEASEDATE, OS,
							system_info.sysname, system_info.version, system_info.release);
#else /* AIX */
						fprintf (fp_out, "User-Agent: %s/%s-%s (%s) (%s/%s (%s))\n\n",
							PRODUCT, VERSION, RELEASEDATE, OS,
							system_info.sysname, system_info.release, system_info.machine);
#endif /* AIX */
#else
						fprintf (fp_out, "User-Agent: %s/%s-%s (%s)\n\n",
							PRODUCT, VERSION, RELEASEDATE, OS);
#endif
					else
						fprintf (fp_out, "\n");

					gotit = TRUE;
				} else {
					if (!gotit)
						modify_headers (line);

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
static t_bool
insert_from_header (
	char *infile)
{
	char *ptr;
	char from_name[HEADER_LEN];
	char full_name[128];
	char user_name[128];
	char line[HEADER_LEN];
	char outfile[PATH_LEN];
	FILE *fp_in, *fp_out;
	t_bool from_found = FALSE;
	t_bool in_header = TRUE;

	if ((fp_in = fopen (infile, "r")) != (FILE *) 0) {
#ifdef VMS
		sprintf (outfile, "%s-%d", infile, process_id);
#else
		sprintf (outfile, "%s.%d", infile, process_id);
#endif /* VMS */
		if ((fp_out = fopen (outfile, "w")) != (FILE *) 0) {
			get_user_info (user_name, full_name);
			get_from_name (from_name, (struct t_group *) 0);

			/*
			 * Check that at least one '.' comes after the '@' in the From: line
			 */
			ptr = strchr (from_name, '@');
			if (ptr != (char *) 0) {
				ptr = strchr (ptr, '.');
				if (ptr == (char *) 0) {
					error_message (txt_invalid_from, from_name);
					return FALSE;
				}
			}

			while ((fgets (line, sizeof (line), fp_in) != (char *) 0) && in_header) {
				if (!strncasecmp(line, "From:", 5))
					from_found = TRUE;
				if (*line == '\n')
				   in_header = FALSE;
			}

			if (!from_found)
				fprintf (fp_out, "From: %s\n", from_name);

			fseek(fp_in, 0L, SEEK_SET);
			while (fgets (line, sizeof (line), fp_in) != (char *) 0)
				fputs (line, fp_out);

			fclose (fp_out);
			fclose (fp_in);
			rename_file (outfile, infile);

			return TRUE;
		}
	}
	return FALSE;
}
#endif /* !M_AMIGA */

static void
find_reply_to_addr (
	int respnum,	/* we don't need that in the #else part */
	char *from_addr,
	t_bool parse)
{
#if 0
	/*
	 * This works fine, but since parse_from() has bugs, it seems to be better
	 * to use the other routine...
	 */

	char *ptr, buf[HEADER_LEN];
	char from_both[HEADER_LEN];
	char from_name[HEADER_LEN];
	int found = FALSE;
	long orig_offset;

	orig_offset = ftell (note_fp);
	fseek (note_fp, 0L, SEEK_SET);

	while (fgets (buf, sizeof (buf), note_fp) != (char *) 0 &&
	       !found && buf[0] != '\n') {
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

	if (!found) {
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
	fseek (note_fp, orig_offset, SEEK_SET);
#else
	char *ptr, buf[HEADER_LEN];
	char replyto[HEADER_LEN];
	char from[HEADER_LEN];
	char temp[HEADER_LEN];
	char fullname[HEADER_LEN];
	char* dest;
	int found_replyto = FALSE;
	long orig_offset;

	orig_offset = ftell (note_fp);
	fseek (note_fp, 0L, SEEK_SET);

	while (fgets (buf, sizeof (buf), note_fp) != (char *) 0 && buf[0] != '\n') {
		/* not quite correct, since we don't process continuation
		   lines, but that is unlikely */
		if (STRNCMPEQ(buf, "Reply-To: ", 10)) {
			strcpy (replyto, &buf[10]);
			ptr = strchr (replyto, '\n');
			if (ptr != (char *) 0)
				*ptr = '\0';

			found_replyto = TRUE;
		} else if (STRNCMPEQ(buf, "From: ", 6)) {
			strcpy (from, &buf[6]);
			ptr = strchr (from, '\n');
			if (ptr != (char *) 0)
				*ptr = '\0';

		}
	}

	/* We do this to save a redundant strcpy when we don't want to parse */

	if (parse)
		dest = temp;
	else
		dest = from_addr;

	if (found_replyto)
		strcpy (dest, rfc1522_decode(replyto));
	else
		strcpy (dest, rfc1522_decode(from));

	if (parse)
		parse_from (temp, from_addr, fullname);

	fseek (note_fp, orig_offset, SEEK_SET);
#endif
}

/*
 * If any arts have been posted by the user reread the active
 * file so that they are shown in the unread articles number
 * for each group at the group selection level.
 */

int
reread_active_after_posting (void)
{
	int i, modified = FALSE;
	long lMinOld;
	long lMaxOld;
	struct t_group *psGrp;

	if (reread_active_for_posted_arts) {
		reread_active_for_posted_arts = FALSE;

		for (i = 0; i < num_active; i++) {
			psGrp = &active[i];
			if (psGrp->subscribed && psGrp->art_was_posted) {
				psGrp->art_was_posted = FALSE;

				if (psGrp != (struct t_group *) 0) {
					wait_message (0, "Rereading %s...", psGrp->name);
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
						my_printf (cCRLF "Unread WRONG grp=[%s] unread=[%ld] count=[%ld]",
							psGrp->name, psGrp->newsrc.num_unread, psGrp->count);
						my_flush ();
#endif
						psGrp->newsrc.num_unread = psGrp->count;
					}
					if (psGrp->xmin != lMinOld || psGrp->xmax != lMaxOld) {
#ifdef DEBUG
						my_printf (cCRLF "Min/Max DIFF grp=[%s] old=[%ld-%ld] new=[%ld-%ld]",
							psGrp->name, lMinOld, lMaxOld, psGrp->xmin, psGrp->xmax);
						my_flush ();
#endif

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

static void
update_active_after_posting (
	char *newsgroups)
{
	char *src, *dst;
	char group[HEADER_LEN];
	struct t_group *psGrp;

	/*
	 * remove duplicates from Newsgroups header
	 */
	strip_double_ngs (note_h.newsgroups);

	src = newsgroups;
	dst = group;

	while (*src) {
		if (*src != ' ')
			*dst = *src;

		src++;
		if (*dst == ',' || *dst == '\0') {
			*dst = '\0';
			psGrp = psGrpFind (group);
			if (psGrp != (struct t_group *) 0 && psGrp->subscribed) {
				reread_active_for_posted_arts = TRUE;
				psGrp->art_was_posted = TRUE;
			}
			dst = group;
		} else
			dst++;
	}
}

static int
submit_mail_file (
	char *file)
{
	char buf[HEADER_LEN];
	char mail_to[HEADER_LEN];
	char subject[HEADER_LEN];
	int mailed = FALSE;

#ifndef M_AMIGA
	if (insert_from_header (file))
#endif
	{
		if (pcCopyArtHeader (HEADER_TO, file, mail_to) && pcCopyArtHeader (HEADER_SUBJECT, file, subject)) {
			t_bool ismail=TRUE;
			wait_message (0, txt_mailing_to, mail_to);

			rfc15211522_encode (file, txt_mime_types[mail_mime_encoding], mail_8bit_header,ismail);

			strfmailer (mailer, subject, mail_to, file,
				  buf, sizeof (buf), default_mailer_format);

			if (invoke_cmd (buf))
				mailed = TRUE;
			else
				error_message (txt_command_failed_s, buf);
		}
	}
	return mailed;
}

#ifdef FORGERY
static void
make_path_header (
	char *line, char *from_name)
{
	char full_name[128];
	char user_name[128];

	get_user_info (user_name, full_name);

	sprintf (line, "%s!%s", domain_name, user_name);
	return;
}
#endif /* FORGERY */
