/*
 *  Project   : tin - a Usenet reader
 *  Module    : post.c
 *  Author    : I. Lea
 *  Created   : 1991-04-01
 *  Updated   : 1997-12-25
 *  Notes     : mail/post/replyto/followup/repost & cancel articles
 *  Copyright : (c) Copyright 1991-99 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */


#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */
#ifndef TCURSES_H
#	include "tcurses.h"
#endif /* !TCURSES_H */
#ifndef MENUKEYS_H
#	include  "menukeys.h"
#endif /* !MENUKEYS_H */
#ifndef VERSION_H
#	include  "version.h"
#endif /* !VERSION_H */

#ifndef INDEX_DAEMON
#	ifdef USE_CANLOCK
#		define ADD_CAN_KEY(id) { \
			char key[1024]; \
			const char *kptr = (const char *) 0; \
			key[0] = '\0' ; \
			if ((kptr = build_cankey(id, get_secret())) != (const char *) 0) { \
				STRCPY(key, kptr); \
				msg_add_header ("Cancel-Key", key); \
			} \
		}
		/*
		 * only add lock here if we use an external inews
		 * and generate our own Message-IDs (EVIL_INSIDE)
		 * otherwise inews.c adds the canlock (if possible:
		 * i.e EVIL_INSIDE or server passed id on POST or
		 * user supplied ID by hand) for us!
		 */
#		define ADD_CAN_LOCK(id) { \
			if (!tinrc.use_builtin_inews) { \
				char lock[1024]; \
				const char *lptr = (const char *) 0; \
				lock[0] = '\0' ; \
				if ((lptr = build_canlock(id, get_secret())) != (const char *) 0) { \
					STRCPY(lock, lptr); \
					msg_add_header ("Cancel-Lock", lock); \
				} \
			} \
		}
#	else
#		define ADD_CAN_KEY(id)
#		define ADD_CAN_LOCK(id)
#	endif /* USE_CANLOCK */

#	ifdef EVIL_INSIDE
	/* gee! ugly hack - but works */
#		define ADD_MSG_ID_HEADER()	{ \
			char mid[1024]; \
			const char *mptr = (const char *) 0; \
			mid[0] = '\0'; \
			if ((mptr = build_messageid()) != (const char *) 0) { \
				STRCPY(mid, mptr); \
				msg_add_header ("Message-ID", mid); \
				ADD_CAN_LOCK(mid); \
			} \
		}
#	else
#		define ADD_MSG_ID_HEADER()
#	endif /* EVIL_INSIDE */
#else
#	define ADD_MSG_ID_HEADER()
#endif /* !INDEX_DAEMON */

#define	MAX_MSG_HEADERS	20

/* Different posting types for post_loop() */
#define POST_QUICK		0
#define POST_POSTPONED	1
#define POST_NORMAL		2
#define POST_RESPONSE	3
#define POST_REPOST		4

/* When prompting for subject, display no more than 20 characters */
#define DISPLAY_SUBJECT_LEN 20

/* which keys are allowed for posting/sending? */
#ifdef HAVE_PGP
#	ifdef HAVE_ISPELL
#		define TIN_POST_KEYS	"\033egiopyq"
#		define TIN_SEND_KEYS	"\033egiqsy"
#	else
#		define TIN_POST_KEYS	"\033egopyq"
#		define TIN_SEND_KEYS	"\033egqsy"
#	endif /* HAVE_ISPELL */
#else
#	ifdef HAVE_ISPELL
#		define TIN_POST_KEYS	"\033eiopqy"
#		define TIN_SEND_KEYS	"\033eiqsy"
#	else
#		define TIN_POST_KEYS	"\033eopyq"
#		define TIN_SEND_KEYS	"\033eqsy"
#	endif /* HAVE_ISPELL */
#endif /* HAVE_PGP */
#define TIN_EDIT_KEYS	"\033eoq"
#define TIN_EDIT_KEYS_EXT	"\033eqM"
#define TIN_CONT_KEYS	"\033ac"

/* tmpname for responses by mail */
#ifdef VMS
#	define TIN_LETTER	"letter."
#else
#	define TIN_LETTER	".letter"
#endif /* VMS */

struct t_posted *posted;

extern char article[PATH_LEN];			/* Path of the file holding temp. article */
int start_line_offset = 1;		/* used by invoke_editor for line no. */
static char found_newsgroups[HEADER_LEN];

static struct msg_header {
	char *name;
	char *text;
} msg_headers[MAX_MSG_HEADERS];


/*
 * Local prototypes
 */
static char *backup_article_name (char *the_article);
static char prompt_rejected (void);
static char prompt_to_send (const char *subject);
static int msg_add_x_body (FILE *fp_out, char *body);
static int msg_write_headers (FILE *fp);
static int post_loop (int type, struct t_group *psGrp, char ch, const char *posting_msg, int art_type, int offset);
static size_t skip_id (const char *id);
static t_bool check_article_to_be_posted (char *the_article, int art_type);
static t_bool check_for_spamtrap (char *addr);
static t_bool damaged_id (const char *id);
static t_bool fetch_postponed_article(char tmp_file[], char subject[], char newsgroups[]);
static t_bool is_crosspost (char *xref);
static t_bool must_include (char *id);
static t_bool pcCopyArtHeader (int iHeader, char *pcArt, char *result);
static t_bool repair_article (char *result);
static t_bool submit_mail_file (char *file);
static void append_postponed_file (char *file, char *addr);
static void appendid (char **where, const char **what);
static void backup_article (char *the_article);
static void find_reply_to_addr (/*int respnum,*/ char *from_addr, t_bool parse);
static void join_references (char *buffer, char *oldrefs, const char *newref);
static void modify_headers (char *line);
static void msg_add_header (const char *name, const char *text);
static void msg_add_x_headers (char *headers);
static void msg_free_headers (void);
static void msg_init_headers (void);
static void post_postponed_article (int ask, char* subject);
static void postpone_article (char *the_article);
static void setup_check_article_screen (int *init);
static void update_active_after_posting (char *newsgroups);
static void update_posted_info_file (char *group, int action, char *subj);
static void update_posted_msgs_file (char *file, char *addr);
static void yank_to_addr (char *orig, char *addr);
#ifdef FORGERY
	static void make_path_header (char *line);
#endif /* FORGERY */
#if !defined(M_AMIGA) && !defined(INDEX_DAEMON)
	static t_bool insert_from_header (char *infile);
#endif /* !M_AMIGA && !INDEX_DAEMON */
#if defined(EVIL_INSIDE) && !defined(INDEX_DAEMON)
	static const char * build_messageid (void);
#endif /* EVIL_INSIDE && !INDEX_DAEMON */


static char
prompt_to_send (
	const char *subject)
{
	return (prompt_slk_response (iKeyPostSend, TIN_SEND_KEYS, sized_message(txt_quit_edit_send, subject)));
}


static char
prompt_rejected (
	void)
{
/* FIXME (what does this mean?) fix screen pos. */
	Raw (FALSE);
	my_fprintf(stderr, "\n\n%s\n\n", txt_post_error_ask_postpone);
	my_fflush(stderr);
	Raw (TRUE);

	return (prompt_slk_response (iKeyPostEdit, TIN_EDIT_KEYS, txt_quit_edit_postpone));
}


static t_bool
repair_article (
	char *result)
{
	int ch;

	ch = prompt_slk_response (iKeyPostEdit, TIN_EDIT_KEYS_EXT, txt_bad_article);

	*result = ch;
	if (ch == iKeyPostEdit) {
		if (invoke_editor (article, start_line_offset))
			return TRUE;
	} else if (ch == iKeyOptionMenu) {
		(void) change_config_file(NULL);
		return TRUE;
	}
	return FALSE;
}


/*
 * make a backup copy of ~/.article, this is necessary since
 * submit_news_file adds headers, does q-p conversion etc
 *
 * TODO:
 * - why not use BACKUP_FILE_EXT like in misc.c?
 * - using sprintf here is unsafe
 */
static char *
backup_article_name (
	char *the_article)
{
	static char name[PATH_LEN];

	sprintf(name, "%s.bak", the_article);
	return name;
}


static void
backup_article (
	char *the_article)
{
	FILE *in, *out;
	char line[LEN];

	if ((in = fopen(the_article, "r")) == NULL)
		return;

	if ((out = fopen(backup_article_name(the_article), "w")) == NULL) {
		fclose(in);
		return;
	}

	/* Why not use copy_fp() here - it should be more efficient */
	while (fgets(line, (int) sizeof(line), in) != NULL)
		fputs(line, out);

	fclose(in);
	fclose(out);
}


static void
msg_init_headers (
	void)
{
	int i;

	for (i = 0; i < MAX_MSG_HEADERS; i++) {
		msg_headers[i].name = (char *) 0;
		msg_headers[i].text = (char *) 0;
	}
}


static void
msg_free_headers (
	void)
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
	int i;
	t_bool done = FALSE;

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
					for (p = text; *p && (*p == ' ' || *p == '\t'); p++)
						;
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
				for (p = text; *p && (*p == ' ' || *p == '\t'); p++)
					;
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
user_posted_messages (
	void)
{
	FILE *fp;
	char buf[LEN];
	int no_of_lines = 0;
	size_t i = 0, j, k;

	if ((fp = fopen (posted_info_file, "r")) == (FILE *) 0) {
		clear_message ();
		return FALSE;
	}

	while (fgets (buf, (int) sizeof(buf), fp) != (char *) 0)
		no_of_lines++;

	if (!no_of_lines) {
		fclose (fp);
		info_message (txt_no_arts_posted);
		return FALSE;
	}
	rewind (fp);
	posted = (struct t_posted *) my_malloc((no_of_lines + 1) * sizeof(struct t_posted));

	while (fgets (buf, (int) sizeof(buf), fp) != (char *) 0) {
		if (buf[0] == '#' || buf[0] == '\n')
			continue;

		for (j = 0; buf[j] != '|' && buf[j] != '\n'; j++)
			posted[i].date[j] = buf[j];	/* posted date */

		if (buf[j] == '\n') {
			error_message (txt_error_corrupted_file, posted_info_file);
			(void) sleep (1);
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
	posted[i].date[0] = '\0';	/* end-marker for display */
	fclose (fp);

	show_info_page (POST_INFO, 0, txt_post_history_menu);

	if (posted != (struct t_posted *)0) {
		free((char *)posted);
		posted = (struct t_posted *)0;
	}
	return TRUE;
}


static void
update_posted_info_file (
	char *group,
	int action,
	char *subj)
{
	FILE *fp;
	struct tm *pitm;
	time_t epoch;

	if (no_write)
		return;

	if ((fp = fopen (posted_info_file, "a+")) != NULL) {
		(void) time (&epoch);
		pitm = localtime (&epoch);
		fprintf (fp, "%02d-%02d-%02d|%c|%s|%s\n", pitm->tm_mday, pitm->tm_mon + 1, pitm->tm_year % 100, action, group, subj);
		fclose (fp);
	}
}


static void
update_posted_msgs_file (
	char *file,
	char *addr)
{
	FILE *fp_in, *fp_out;
	char buf[LEN];
	time_t epoch;

	if ((fp_in = fopen (file, "r"))  == (FILE *) 0)
		return;

	if (!strfpath (posted_msgs_file, buf, sizeof (buf), homedir, (char *) 0, (char *) 0, (char *) 0))
		strcpy (buf, posted_msgs_file);

	if ((fp_out = fopen (buf, "a+")) != (FILE *) 0) {
		(void) time (&epoch);
		fprintf (fp_out, "From %s %s", addr, ctime (&epoch));
		while (fgets (buf, (int) sizeof(buf), fp_in) != (char *) 0)
			fputs (buf, fp_out);

		print_art_seperator_line (fp_out, FALSE);
		fclose (fp_out);
	}
	fclose (fp_in);
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
 * 10. Check for charset != US-ASCII when using non-7bit-encoding
 * 11. Warn if transfer encoding is base64 or quoted-printable and using
 *     external inews
 * 12. Check that Subject, Newsgroups and if present Followup-To
 *     headers are uniq
 * 13. Display an 'are you sure' message before posting article
 */
static t_bool
check_article_to_be_posted (
	char *the_article,
	int art_type)
{
	FILE *fp;
	char *ngptrs[NGLIMIT], *ftngptrs[NGLIMIT];
	char line[HEADER_LEN], *cp, *cp2;
	char subject[HEADER_LEN];
	int cnt = 0;
	int col, len, i = 0;
	int errors = 0;
	int init = 1;
	int ngcnt = 0, ftngcnt = 0;
	int oldraw;		/* save previous raw state */
	int c;
	int saw_sig_dashes = 0;
	int sig_lines = 0;
	int found_newsgroups_lines = 0;
	int found_subject_lines = 0;
	int found_followup_to_lines = 0;
	t_bool end_of_header = FALSE;
	t_bool got_long_line = FALSE;
	t_bool saw_wrong_sig_dashes = FALSE;
	t_bool mime_7bit = TRUE;
	t_bool mime_usascii = TRUE;
	t_bool contains_8bit = FALSE;
	size_t nglens[NGLIMIT], ftnglens[NGLIMIT];
	struct t_group *psGrp;

	if ((fp = fopen (the_article, "r")) == (FILE *) 0) {
		perror_message (txt_cannot_open, the_article);
		return FALSE;
	}
	oldraw = RawState ();	/* save state */

	while (fgets (line, (int) sizeof(line), fp) != NULL) {
		cnt++;
		len = strlen (line);
		if (len > 0) {
			if (line[len - 1] == '\n')
				line[--len] = 0;
		}
		if ((cnt == 1) && (len == 0)) {
			/* first line, no content => no header, abort */
			setup_check_article_screen (&init);
			StartInverse();
			my_fprintf (stderr, txt_error_header_line_blank);
			my_fflush (stderr);
			EndInverse();
			errors++;
			end_of_header = TRUE;
			break;
		}
		for (cp = line; *cp; cp++) {
			if (!contains_8bit && !isascii(*cp))
				contains_8bit = TRUE;
		}
		if ((len == 0) && (cnt >= 2)) {
			end_of_header = TRUE;
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
			StartInverse();
			my_fprintf (stderr, txt_error_header_line_colon, cnt, line);
			my_fflush (stderr);
			EndInverse();
			errors++;
			continue;
		}
		if (cp[1] != ' ') {
			setup_check_article_screen (&init);
			StartInverse();
			my_fprintf (stderr, txt_error_header_line_space, cnt, line);
			my_fflush (stderr);
			EndInverse();
			errors++;
		}
		if (cp - line == 7 && !strncasecmp (line, "Subject", 7)) {
			found_subject_lines++;
			strncpy (subject, cp+2, cCOLS-6);
			subject[cCOLS-6] = '\0';
		}

#ifndef FORGERY
		if (cp - line == 6 && !strncasecmp (line, "Sender", 6)) {
			StartInverse();
			my_fprintf (stderr, txt_error_sender_in_header_not_allowed, cnt);
			my_fflush (stderr);
			EndInverse();
			errors++;
		}
#endif /* !FORGERY */
		if (cp - line == 8 && !strncasecmp (line, "Approved", 8)) {
			if (tinrc.beginner_level) {
				StartInverse();
				my_fprintf (stderr, txt_error_approved, i);
				my_fflush (stderr);
				EndInverse();
#ifdef HAVE_FASCIST_NEWSADMIN
				errors++;
#endif /* HAVE_FASCIST_NEWSADMIN */
			}
			if (GNKSA_OK != (i = gnksa_check_from (rfc1522_encode (line, FALSE) + (cp - line) + 1))) {
				StartInverse();
				my_fprintf (stderr, txt_error_bad_approved, i);
				my_fprintf (stderr, gnksa_strerror(i), i);
				my_fflush (stderr);
				EndInverse();
#ifndef FORGERY
				errors++;
#endif /* !FORGERY */
			}
		}
		if (cp - line == 4 && !strncasecmp (line, "From", 4)) {
			if (GNKSA_OK != (i = gnksa_check_from (rfc1522_encode (line, FALSE) + (cp - line) + 1))) {
				StartInverse();
				my_fprintf (stderr, txt_error_bad_from, i);
				my_fprintf (stderr, gnksa_strerror(i), i);
				my_fflush (stderr);
				EndInverse();
#ifndef FORGERY
				errors++;
#endif /* !FORGERY */
			}
		}

		if (cp - line == 8 && !strncasecmp (line, "Reply-To", 8)) {
			if (GNKSA_OK != (i = gnksa_check_from (rfc1522_encode (line, FALSE) + (cp - line) + 1))) {
				StartInverse();
				my_fprintf (stderr, txt_error_bad_replyto, i);
				my_fprintf (stderr, gnksa_strerror(i), i);
				my_fflush (stderr);
				EndInverse();
#ifndef FORGERY
				errors++;
#endif /* !FORGERY */
			}
		}
		if (cp - line == 10 && !strncasecmp (line, "Message-Id", 10)) {
			i = gnksa_check_from (cp+1);
			if ((GNKSA_OK != i) && (GNKSA_LOCALPART_MISSING > i)) {
				StartInverse();
				my_fprintf (stderr, txt_error_bad_msgidfqdn, i);
				my_fprintf (stderr, gnksa_strerror(i), i);
				my_fflush (stderr);
				EndInverse();
#ifndef FORGERY
				errors++;
#endif /* !FORGERY */
			}
		}
		if (cp - line == 10 && !strncasecmp (line, "Newsgroups", 10)) {
			found_newsgroups_lines++;
			for (cp = line + 11; *cp == ' '; cp++)
				;
			if (strchr (cp, ' ')) {
				setup_check_article_screen (&init);
				StartInverse();
				my_fprintf (stderr, txt_error_header_line_comma, "Newsgroups");
				my_fflush (stderr);
				EndInverse();
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
						for (i = 0; i < ftngcnt; i++)
							FreeIfNeeded (ftngptrs[i]);
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
				StartInverse();
				my_fprintf (stderr, txt_error_header_line_empty_newsgroups);
				my_fflush (stderr);
				EndInverse();
				errors++;
				continue;
			}
			if ((c = fgetc(fp)) != EOF) {
				ungetc(c, fp);
				if (isspace (c) && c != '\n') {
					setup_check_article_screen (&init);
					StartInverse();
					my_fprintf (stderr, txt_error_header_line_groups_contd, "Newsgroups");
					my_fflush (stderr);
					EndInverse();
					errors++;
					continue;
				}
			}
		}
		if (cp - line == 11 && !strncasecmp (line, "Followup-To", 11)) {
			for (cp = line + 12; *cp == ' '; cp++)
				;
			if (strlen(cp)) /* Followup-To not empty */
				found_followup_to_lines++;
			strip_double_ngs (cp);
			if (strchr (cp, ' ')) {
				setup_check_article_screen (&init);
				StartInverse();
				my_fprintf (stderr, txt_error_header_line_comma, "Followup-To");
				my_fflush (stderr);
				EndInverse();
				errors++;
				continue;
			}
			while (*cp) {
				if (!(cp2 = strchr (cp, ','))) {
					cp2 = cp + strlen (cp);
				} else {
					*cp2++ = '\0';
				}
				if (ftngcnt < NGLIMIT) {
					ftnglens[ftngcnt] = strlen (cp);
					ftngptrs[ftngcnt] = (char *) my_malloc (ftnglens[ftngcnt] + 1);
					if (!ftngptrs[ftngcnt]) {
						/* out of memory? */
						for (i = 0; i < ftngcnt; i++)
							FreeIfNeeded (ftngptrs[i]);
						for (i = 0; i < ngcnt; i++)
							FreeIfNeeded (ngptrs[i]);
						Raw (oldraw);
						return TRUE;
					}
					strcpy (ftngptrs[ftngcnt], cp);
					ftngcnt++;
				}
				cp = cp2;
			}
			if ((c = fgetc(fp)) != EOF) {
				ungetc(c, fp);
				if (isspace(c) && c != '\n') {
					setup_check_article_screen (&init);
					StartInverse();
					my_fprintf (stderr, txt_error_header_line_groups_contd, "Followup-To");
					my_fflush (stderr);
					EndInverse();
					errors++;
					continue;
				}
			}
		}
	}

	if (!found_subject_lines || subject[0] == '\0') {
		setup_check_article_screen (&init);
		StartInverse();
		my_fprintf (stderr, found_subject_lines ? txt_error_header_line_empty_subject : txt_error_header_line_missing_subject);
		my_fflush (stderr);
		EndInverse();
		errors++;
	} else {
		char foo[HEADER_LEN];
		strcpy(foo, subject);
		if (!strtok(foo, " \t")) { /* only blanks in Subject? */
			setup_check_article_screen (&init);
			my_fprintf (stderr, txt_warn_blank_subject);
			my_fflush (stderr);
		}
	}

	if (!found_newsgroups_lines && art_type == GROUP_TYPE_NEWS) {
		setup_check_article_screen (&init);
		StartInverse();
		my_fprintf (stderr, txt_error_header_line_missing_newsgroups);
		my_fflush (stderr);
		EndInverse();
		errors++;
	}

	if (found_newsgroups_lines > 1) {
		setup_check_article_screen (&init);
		StartInverse();
		my_fprintf (stderr, txt_error_header_duplicate, found_newsgroups_lines, "Newsgroups:");
		my_fflush (stderr);
		EndInverse();
		errors++;
	}

	if (found_subject_lines > 1) {
		setup_check_article_screen (&init);
		StartInverse();
		my_fprintf (stderr, txt_error_header_duplicate, found_subject_lines, "Subject:");
		my_fflush (stderr);
		EndInverse();
		errors++;
	}

	if (found_followup_to_lines > 1) {
		setup_check_article_screen (&init);
		StartInverse();
		my_fprintf (stderr, txt_error_header_duplicate, found_followup_to_lines, "Followup-To:");
		my_fflush (stderr);
		EndInverse();
		errors++;
	}

	/*
	 * Check the body of the article for long lines
	 * check if article contains non-7bit-ASCII characters
	 * check if sig is shorter then MAX_SIG_LINES lines
	 */
	while (fgets (line, (int) sizeof(line), fp)) {
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
			if (!contains_8bit && !isascii(*cp))
				contains_8bit = TRUE;
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
		my_fprintf (stderr, txt_warn_wrong_sig_format);
		my_fflush (stderr);
	}
	if (sig_lines > MAX_SIG_LINES) {
		setup_check_article_screen (&init);
		my_fprintf (stderr, txt_warn_sig_too_long, MAX_SIG_LINES);
		my_fflush (stderr);
#ifdef HAVE_FASCIST_NEWSADMIN
		errors++;
#endif /* HAVE_FASCIST_NEWSADMIN */
	}
	if (!end_of_header) {
		setup_check_article_screen (&init);
		StartInverse();
		my_fprintf (stderr, txt_error_header_and_body_not_separate);
		my_fflush (stderr);
		EndInverse();
		errors++;
	}

	/* check for MIME Content-Type and Content-Transfer-Encoding */
	get_mm_charset ();

	if (strcasecmp (tinrc.mm_charset, "US-ASCII"))
		mime_usascii = FALSE;

	if (strcasecmp (txt_mime_encodings[tinrc.post_mime_encoding], "7bit"))
		mime_7bit = FALSE;

	if (contains_8bit && mime_usascii) {
		setup_check_article_screen (&init);
		my_fprintf (stderr, txt_error_header_line_bad_charset);
		my_fflush (stderr);
		errors++;
	}

	if (contains_8bit && mime_7bit) {
		setup_check_article_screen (&init);
		my_fprintf (stderr, txt_error_header_line_bad_encoding);
		my_fflush (stderr);
		errors++;
	}

	/*
	 * Warn when poster is using a non-plain encoding such as quoted-printable
	 * or base64 and external inews because if that external inews appends a
	 * signature it will not be encoded. We might additionally check if there's
	 * a file named ~/.signature and skip the warning if it is not present.
	 */
	if (((tinrc.post_mime_encoding == MIME_ENCODING_QP) ||
			(tinrc.post_mime_encoding == MIME_ENCODING_BASE64)) &&
			!tinrc.use_builtin_inews) {
		setup_check_article_screen (&init);
		my_fprintf (stderr, txt_warn_encoding_and_external_inews);
		my_fflush (stderr);
	}

	if (ngcnt && !errors) {
		/*
		 * Print a note about each newsgroup
		 */
		setup_check_article_screen (&init);
		my_fprintf (stderr, txt_art_newsgroups, subject, ngcnt == 1 ? "" : "s");
		my_fflush (stderr);
		for (i = 0; i < ngcnt; i++) {
			psGrp = psGrpFind (ngptrs[i]);
			if (psGrp) {
				my_fprintf (stderr, "  %s\t %s\n", ngptrs[i], (psGrp->description ? psGrp->description : ""));
				my_fflush (stderr);
			} else {
#ifdef HAVE_FASCIST_NEWSADMIN
				StartInverse();
				errors++;
				my_fprintf (stderr, txt_error_not_valid_newsgroup, ngptrs[i]);
				my_fflush (stderr);
				EndInverse();
#else
				my_fprintf (stderr, (!list_active ? /* did we read the whole active file? */ txt_warn_not_in_newsrc : txt_warn_not_valid_newsgroup), ngptrs[i]);
#endif /* HAVE_FASCIST_NEWSADMIN */
			}
		}
		if (!found_followup_to_lines && ngcnt > 1 && !errors) {
#ifdef HAVE_FASCIST_NEWSADMIN
			StartInverse();
			my_fprintf (stderr, txt_error_missing_followup_to, ngcnt);
			my_fflush (stderr);
			errors++;
			EndInverse();
#else
			my_fprintf (stderr, txt_warn_missing_followup_to, ngcnt);
#endif /* HAVE_FASCIST_NEWSADMIN */
		}

		if (ftngcnt && !errors) {
			if (ftngcnt > 1) {
#ifdef HAVE_FASCIST_NEWSADMIN
				StartInverse();
				my_fprintf (stderr, txt_error_followup_to_several_groups);
				my_fflush (stderr);
				errors++;
				EndInverse();
#else
				my_fprintf (stderr, txt_warn_followup_to_several_groups);
#endif /* HAVE_FASCIST_NEWSADMIN */
			}
			if (!errors) {
				my_fprintf (stderr, txt_followup_newsgroups, ftngcnt == 1 ? "" : "s");
				for (i = 0; i < ftngcnt; i++) {
					psGrp = psGrpFind (ftngptrs[i]);
					if (psGrp) {
						my_fprintf (stderr, "  %s\t %s\n", ftngptrs[i], (psGrp->description ? psGrp->description : ""));
						my_fflush (stderr);
					} else {
						if (STRCMPEQ("poster", ftngptrs[i]))
							my_fprintf (stderr, txt_followup_poster, ftngptrs[i]);
						else {
#ifdef HAVE_FASCIST_NEWSADMIN
							errors++;
							StartInverse ();
							my_fprintf (stderr, txt_error_not_valid_newsgroup, ftngptrs[i]);
							my_fflush (stderr);
							EndInverse ();
#else
							my_fprintf (stderr, (!list_active ? /* did we read the whole active file? */ txt_warn_not_in_newsrc : txt_warn_not_valid_newsgroup), ftngptrs[i]);
#endif /* HAVE_FASCIST_NEWSADMIN */
						}
					}
				}
			}
		}

#ifndef NO_ETIQUETTE
		if (tinrc.beginner_level)
			my_fprintf (stderr, txt_warn_posting_etiquette);
#endif /* !NO_ETIQUETTE */
		my_fflush (stderr);
	}
	fclose (fp);

	Raw (oldraw);		/* restore raw/unraw state */

	/* free memory */
	for (i = 0; i < ngcnt; i++)
		FreeIfNeeded (ngptrs[i]);
	for (i = 0; i < ftngcnt; i++)
		FreeIfNeeded (ftngptrs[i]);

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
		*init = 0;
	}
}


/*
 * edit/present an article, perform spell/PGP etc., operations if required
 * submit the article and perform all necessary backend processing
 */
static int
post_loop(
	int type,				/* type of posting */
	struct t_group *psGrp,
	char ch,				/* default prompt char */
	const char *posting_msg,/* displayed just prior to article submission */
	int art_type,			/* news, mail etc. */
	int offset)				/* editor start offset */
{
	char group[HEADER_LEN];
	char subj[HEADER_LEN];
	int ret_code = POSTED_NONE;
	long artchanged = 0L;		/* artchanged work was not done in post_postponed_article */

	forever {
post_article_loop:
		switch (ch) {
			case iKeyPostEdit:
				/* This was VERY different in repost_article
				 * Code existed to recheck subject and restart editor, but
				 * is not enabled
				 */
				artchanged = file_changed(article);
				if (!invoke_editor (article, offset))
					goto post_article_postponed;
				ret_code = POSTED_REDRAW;

				/* This might be erroneous with posting postponed */
				if (file_size(article) > 0L) {
					if ((artchanged == file_changed(article)) && (prompt_yn (cLINES, txt_prompt_unchanged_art, TRUE) > 0 )) {
						;
					} else {
						while (!check_article_to_be_posted (article, art_type) && repair_article(&ch))
							;
						if (ch == iKeyPostEdit || ch == iKeyOptionMenu)
							break;
					}
				}
				nobreak;	/* FALLTHROUGH */

			case iKeyQuit:
			case iKeyAbort:
				if (tinrc.unlink_article)
					unlink (article);
				clear_message ();
				return ret_code;

#ifdef HAVE_ISPELL
			case iKeyPostIspell:
				invoke_ispell (article, psGrp);
				ret_code = POSTED_REDRAW; /* not all versions did this */
				break;
#endif /* HAVE_ISPELL */

#ifdef HAVE_PGP
			case iKeyPostPGP:
				invoke_pgp_news (article);
				break;
#endif /* HAVE_PGP */

			case iKeyPostPost:
			case iKeyPostPost2:
				wait_message (0, posting_msg);
				backup_article (article);

				/* Functions that didn't handle mail didn't do this */
				if (art_type == GROUP_TYPE_NEWS) {
					if (submit_news_file (article))
						ret_code = POSTED_OK;
				} else {
					if (submit_mail_file (article))
						ret_code = POSTED_OK;
				}

				if (ret_code == POSTED_OK) {
					unlink(backup_article_name(article));
					wait_message (1, txt_art_posted);
					goto post_article_done;
				} else {
					if ((ch = prompt_rejected()) == iKeyPostPostpone)
						postpone_article(backup_article_name(article));
					else if (ch == iKeyPostEdit) {
						rename(backup_article_name(article), article);
						ch = iKeyPostEdit;
						goto post_article_loop;
					} else {
						unlink(backup_article_name(article));
						rename_file (article, dead_article);
						if (tinrc.keep_dead_articles)
							append_file (dead_articles, dead_article);
						wait_message (2, txt_art_rejected, dead_article);
					}
				return ret_code;
				}
			case iKeyPostPostpone:
				postpone_article(article);
				goto post_article_postponed;

			default:
				break;
		}
		if (type != POST_REPOST)
			ch = prompt_slk_response(iKeyPostPost, TIN_POST_KEYS, txt_quit_edit_post);
		else
			/* Superfluous force_command stuff not used in current code */
			ch = (/*force_command ? ch_default :*/ prompt_slk_response (ch, TIN_POST_KEYS, sized_message(txt_quit_edit_xpost, note_h.subj)));
	}

post_article_done:
	if (ret_code == POSTED_OK) {
		if (art_type == GROUP_TYPE_NEWS) {
			if (pcCopyArtHeader (HEADER_NEWSGROUPS, article, group)) {
				update_active_after_posting (group);
				/* In POST_RESPONSE, this was copied from note_h.newsgroups if !followup to poster */
				my_strncpy (tinrc.default_post_newsgroups, group, sizeof (tinrc.default_post_newsgroups));
			}
		}

		if (pcCopyArtHeader (HEADER_SUBJECT, article, subj)) {
			char tag = 'w';
			/*
			 * When crossposting postponed articles we currently do not add
			 * autoselect since we don't know which group the article was
			 * actually in
			 * FIXME: This logic is faithful to the original, but awful
			 */
			if (tinrc.add_posted_to_filter && (type == POST_QUICK || type == POST_POSTPONED || type == POST_NORMAL)) {
				if (type != POST_POSTPONED || (type == POST_POSTPONED && !strchr(group, ',') && (psGrp = psGrpFind(group))))
					quick_filter_select_posted_art (psGrp, subj);
			}

			switch (type) {
				case POST_QUICK:
				case POST_NORMAL:
					tag = 'w';
					break;

				case POST_POSTPONED:
					tag = strncmp(subj, "Re: ", 4) ? 'w' : 'f';
					break;

				case POST_RESPONSE:
					tag = 'f';
					break;

				case POST_REPOST:
					tag = 'x';
					break;
			}

			/* Different logic for followup_to: poster */
			if (type == POST_RESPONSE) {
				if (*note_h.followup && strcmp (note_h.followup, "poster") != 0)
					update_posted_info_file (note_h.followup, tag, subj);
				else
					update_posted_info_file (note_h.newsgroups, tag, subj);
			} else
				/* Repost_article() uses psGrp->name rather than group here, but this is probably better anyway */
				update_posted_info_file (group, tag, subj);

			my_strncpy (tinrc.default_post_subject, subj, sizeof (tinrc.default_post_subject));
		}

		if (tinrc.keep_posted_articles && type != POST_REPOST)
			update_posted_msgs_file (article, userid);
	}

/*	write_config_file (local_config_file); Overkill, I think */

post_article_postponed:
	if (tinrc.unlink_article)
		unlink (article);

	return ret_code;
}


/*
 * Parse the list of newsgroups. For each, check group flag status. If it is
 * possible to post to the group and the user agrees, then keep going. Return
 * pointer to the first group in the list (the posting code needs this)
 * Any one failure => return NULL
 */
static struct t_group *
check_moderated (
	const char *groups,
	int *art_type,
	const char *failmsg)
{
	char *group;
	char newsgroups[HEADER_LEN];
	struct t_group *psretGrp = NULL;
	int vnum = 0, bnum = 0;

	/* Take copy - strtok() modifies its args */
	STRCPY(newsgroups, groups);

	group = strtok (newsgroups, ",");

	do {
		struct t_group *psGrp;

		vnum++; /* number of newsgroups */

		if (!(psGrp = psGrpFind (group))) {
			bnum++; /* number of groups not in active file */
			continue;
		}

		if (!psretGrp)				/* Save ptr to the 1st group */
			psretGrp = psGrp;

		/*
		 * Testing for !attribute here is a useful check for other brokenness
		 * Generally only bogus groups should have no attributes
		 */
		if (!psGrp->attribute || psGrp->bogus) {
			if (psGrp->bogus)
				error_message("%s is bogus", group);
			if (!psGrp->attribute)
				error_message("No attributes for %s", group);
			return NULL;
		}

		if (psGrp->attribute->mailing_list != (char *) 0)
			*art_type = GROUP_TYPE_MAIL;

		if (!can_post && *art_type == GROUP_TYPE_NEWS) {
			info_message (txt_cannot_post);
			return NULL;
		}

		if (psGrp->moderated == 'x' || psGrp->moderated == 'n') {
			error_message(txt_cannot_post_group, psGrp->name);
			return NULL;
		}

		if (psGrp->moderated == 'm') {
			sprintf (mesg, txt_group_is_moderated, group);
			if (prompt_yn (cLINES, mesg, TRUE) != 1) {
/*				Raw (FALSE);*/
				error_message (failmsg);
				return NULL;
			}
		}
	} while ((group = strtok (NULL, ",")) != NULL);

	if (vnum > bnum)
		return psretGrp;
	else {
		error_message (txt_not_in_active_file, group);
		return NULL;
	}
}


/*
 * Build the standard headers used by quick_post_article() and post_article()
 * Return TRUE or FALSE if things went wrong - there seems to be little
 * error checking possible in here
 */
static t_bool
create_normal_article_headers(
	struct t_group *psGrp,
	const char *newsgroups,
	int art_type
	)
{
	FILE *fp;
	char from_name[HEADER_LEN];
	char tmp[HEADER_LEN];

	/* TODO rework without tmp */
	/* combine with other code in tin that does the ... truncation ? */
	/* Get subject for posting article - Limit the display if needed */
	if (strlen(tinrc.default_post_subject) > DISPLAY_SUBJECT_LEN) {
		strncpy(tmp, tinrc.default_post_subject, DISPLAY_SUBJECT_LEN);
		tmp[DISPLAY_SUBJECT_LEN] = '\0';
		strcat(tmp, " ...");
	} else
		strncpy(tmp, tinrc.default_post_subject, sizeof(tmp));

	sprintf (mesg, txt_post_subject, tmp);

	if (!(prompt_string_default (mesg, tinrc.default_post_subject, txt_no_subject, HIST_POST_SUBJECT)))
		return FALSE;

	if ((fp = fopen (article, "w")) == NULL) {
		perror_message (txt_cannot_open, article);
		return FALSE;
	}
	chmod (article, (mode_t)(S_IRUSR|S_IWUSR));

#ifndef INDEX_DAEMON
	get_from_name(from_name, psGrp);
#endif /* !INDEX_DAEMON */
#ifdef FORGERY
	make_path_header (tmp);
	msg_add_header ("Path", tmp);
#endif /* FORGERY */
	msg_add_header ("From", from_name);
	msg_add_header ("Subject", tinrc.default_post_subject);

	if (art_type == GROUP_TYPE_MAIL)
		msg_add_header ("To", psGrp->attribute->mailing_list);
	else {
		msg_add_header ("Newsgroups", newsgroups);
		ADD_MSG_ID_HEADER();
	}

	if (psGrp->attribute->followup_to != (char *) 0 && art_type == GROUP_TYPE_NEWS)
		msg_add_header ("Followup-To", psGrp->attribute->followup_to);
	else {
		if (tinrc.prompt_followupto)
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

	start_line_offset = msg_write_headers (fp) + 1;
	fprintf (fp, "\n");			/* add a newline to keep vi from bitching */
	msg_free_headers ();

	start_line_offset += msg_add_x_body (fp, psGrp->attribute->x_body);

	msg_write_signature (fp, FALSE, &CURR_GROUP);
	fclose (fp);

	return TRUE;
}

#ifndef INDEX_DAEMON
/*
 *  Quick post an article (not a followup)
 */
void
quick_post_article (
	t_bool postponed_only)
{
	char buf[HEADER_LEN];
	int art_type = GROUP_TYPE_NEWS;
	struct t_group *psGrp;

	msg_init_headers ();

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
	 * Get groupname
	 */
	sprintf (buf, txt_post_newsgroups, tinrc.default_post_newsgroups);
	if (!(prompt_string_default (buf, tinrc.default_post_newsgroups, txt_no_newsgroups, HIST_POST_NEWSGROUPS)))
		return;

	/*
	 * Strip double newsgroups
	 */
	strip_double_ngs (tinrc.default_post_newsgroups);

	/*
	 * Check/see if any of the newsgroups are not postable.
	 */
	if ((psGrp = check_moderated (tinrc.default_post_newsgroups, &art_type, txt_exiting)) == NULL)
		return;

	if (!create_normal_article_headers (psGrp, tinrc.default_post_newsgroups, art_type))
		return;

	post_loop(POST_QUICK, psGrp, iKeyPostEdit, txt_posting, art_type, start_line_offset);
}
#endif /* !INDEX_DAEMON */


/*
 *  Post an article that is already written (for postponed articles)
 */
static void
post_postponed_article (
	int ask,
	char *subject)
{
	char buf[LEN];

	if (!can_post) {
		info_message (txt_cannot_post);
		return;
	}

	sprintf(buf, "Posting: %.*s ...", (int)(cCOLS-14), subject);
	post_loop (POST_POSTPONED, NULL, (ask ? iKeyPostEdit : iKeyPostPost), buf, GROUP_TYPE_NEWS, 0);

	return;
}


static void
append_postponed_file (
	char *file,
	char *addr)
{
	FILE *fp_in, *fp_out;
	char buf[LEN];
	time_t epoch;

	if ((fp_in = fopen (file, "r")) == (FILE *) 0)
		return;

	if ((fp_out = fopen (postponed_articles_file, "a+")) != (FILE *) 0) {
		(void) time (&epoch);
		fprintf (fp_out, "From %s %s", addr, ctime (&epoch));
		while (fgets (buf, (int) sizeof(buf), fp_in) != (char *) 0) {
			if (strncmp(buf, "From ", 5) == 0)
				fputc('>', fp_out);
			fputs (buf, fp_out);
		}
		print_art_seperator_line (fp_out, FALSE);
		fclose (fp_out);
	}
	fclose (fp_in);
}


/*
 * count how many articles are in postponed.articles. Essentially,
 * we count '^From ' lines
 */
int
count_postponed_articles (
	void)
{
	FILE *fp = fopen(postponed_articles_file,"r");
	char line[HEADER_LEN];
	int count = 0;

	if (!fp)
		return 0;

	while (fgets(line, (int) sizeof(line), fp)) {
		if (strncmp(line, "From ", 5) == 0)
			count++;
	}
	fclose(fp);
	return count;
}


/*
 * Copy the first postponed article and remove it from the postponed file
 */
static t_bool
fetch_postponed_article (
	char tmp_file[],
	char subject[],
	char newsgroups[])
{
	FILE *in;
	FILE *out;
	FILE *tmp;
	char postponed_tmp[PATH_LEN];
	char line[HEADER_LEN];
	t_bool first_article;
	t_bool prev_line_nl;
	t_bool anything_left;

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

	fgets(line, (int) sizeof(line), in);

	if (strncmp(line, "From ", 5) != 0) {
		fclose(in);
		fclose(out);
		fclose(tmp);
		return FALSE;
	}

	first_article = TRUE;
	prev_line_nl = FALSE;
	anything_left = FALSE;

	/*
	 * we have one minor problem with copying the article, we have added
	 * a newline at the end of the article and we have to remove that,
	 * but we don't know we are on the last line until we read the next
	 * line containing "From "
	 */

	while (fgets(line, (int) sizeof(line), in) != NULL) {
		if (strncmp(line, "From ", 5) == 0)
			first_article = FALSE;
		if (first_article) {
			match_string(line, "Newsgroups: ", newsgroups, HEADER_LEN);
			match_string(line, "Subject: ", subject, HEADER_LEN);

			if (prev_line_nl)
				fputc('\n', out);

			if (strlen(line) && line[strlen(line)-1]=='\n') {
				prev_line_nl = TRUE;
				line[strlen(line)-1] = '\0';
			} else
				prev_line_nl = FALSE;

			fputs(line, out);
		} else {
			fputs(line, tmp);
			anything_left = TRUE;
		}
	}

	fclose(in);
	fclose(out);
	fclose(tmp);

	unlink(postponed_articles_file);

	if (anything_left)
		rename(postponed_tmp, postponed_articles_file);
	else
		unlink(postponed_tmp);

	return TRUE;
}


/* pick up any postponed articles and ask if the user wants to use them */
t_bool
pickup_postponed_articles (
	t_bool ask,
	t_bool all)
{
	char ch = 0;
	char newsgroups[HEADER_LEN];
	char subject[HEADER_LEN];
	char question[HEADER_LEN];
	int count = count_postponed_articles();
	int i;

	if (!count) {
		if (!ask)
			info_message(txt_info_nopostponed);
		return FALSE;
	}

	sprintf(question, txt_prompt_see_postponed, count);

	if (ask && prompt_yn(cLINES, question, TRUE) != 1)
		return FALSE;

	for (i = 0; i < count; i++) {
		if (!fetch_postponed_article(article, subject, newsgroups))
			return TRUE;

		if (!all) {
			ch = prompt_slk_response (iKeyPostponeYes, "\033qyYnA", sized_message(txt_postpone_repost, subject));

			if (ch == iKeyPostponeYesAll)
				all = TRUE;
		}

		/* No else here since all changes in previous if */
		if (all)
			ch = iKeyPostponeYesOverride;

		switch (ch)
		{
			case iKeyPostponeYes:
			case iKeyPostponeYesOverride:
				post_postponed_article(ch == iKeyPostponeYes, subject);
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
postpone_article (
	char *the_article)
{
	wait_message(3, txt_info_do_postpone);
	append_postponed_file(the_article, userid);
}


/*
 * Post an original article (not a followup)
 */
t_bool
post_article (
	const char *group)
{
	int art_type = GROUP_TYPE_NEWS;
	struct t_group *psGrp;
	t_bool redraw_screen = FALSE;

	msg_init_headers ();

	/*
	 * Check that we can post to all the groups we want to
	 */
	if ((psGrp = check_moderated (group, &art_type, "")) == NULL)
		return redraw_screen;

	if (!create_normal_article_headers (psGrp, group, art_type))
		return redraw_screen;

	return (post_loop(POST_NORMAL, psGrp, iKeyPostEdit, txt_posting, art_type, start_line_offset) != POSTED_NONE);
}


/*
 * yeah, right, that's from the same Chris who is telling Jason he's
 * doing obfuscated C :-)
 */
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


/*
 * check given Message-ID for "_-_@" which (should) indicate(s)
 * a Subject: change
 */
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
	int count = 0;

	for (; *xref; xref++)
		if (*xref==':')
			count++;

	return (count>=2) ? TRUE : FALSE;
}


/*
 * Widespread news software like INN's nnrpd restricts the size of several
 * headers, notably the references header, to 512 characters.  Oh well...
 * guess that's what son-of-1036 calls a "desparate last resort" :-/
 * From TIN's point of view, this could be HEADER_LEN.
 */
#define MAXREFSIZE 512

/*
 * TODO - if we have the art[x] that we are following up to, then
 *        get_references(art[x].refptr) will give us the new refs line
 */

static void
join_references (
	char *buffer,
	char *oldrefs,
	const char *newref)
{
	/*
	 * First of all: shortening references is a VERY BAD IDEA.
	 * Nevertheless, current software usually has restrictions in
	 * header length (their programmers seem to misinterpret RFC821
	 * as valid for news, and the command length limit of RFC977
	 * as valid for headers)
	 *
	 * construct a new references line, then trim it if necessary
	 *
	 * do some sanity cleanups: remove damaged ids, make
	 * sure there is space between ids (tabs and commas are stripped)
	 *
	 * note that we're not doing strict son-of-1036 here: we don't
	 * take any precautions to keep the last three message ids, but
	 * it's not very likely that MAXREFSIZE chars can't hold at least
	 * 4 refs
	 */
	char *b, *c, *d;
	const char *e;
	int space;

	b = (char *) my_malloc (strlen (oldrefs) + strlen (newref) + 64);
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

	/*
	 * son-of-1036 says:
	 * Followup agents SHOULD not shorten References  headers.   If
	 * it  is absolutely necessary to shorten the header, as a des-
	 * perate last resort, a followup agent MAY do this by deleting
	 * some  of  the  message IDs.  However, it MUST not delete the
	 * first message ID, the last three message IDs (including that
	 * of  the immediate precursor), or any message ID mentioned in
	 * the body of the followup.  If it is possible  for  the  fol-
	 * lowup agent to determine the Subject content of the articles
	 * identified in the References header, it MUST not delete  the
	 * message  ID of any article where the Subject content changed
	 * (other than by prepending of a back  reference).   The  fol-
	 * lowup  agent MUST not delete any message ID whose local part
	 * ends with "_-_" (underscore (ASCII 95), hyphen  (ASCII  45),
	 * underscore);  followup  agents are urged to use this form to
	 * mark subject changes, and to avoid using it otherwise.
	 * [...]
	 * When a References header is shortened, at least three blanks
	 * SHOULD be left between adjacent message IDs  at  each  point
	 * where  deletions  were  made.  Software preparing new Refer-
	 * ences headers SHOULD preserve multiple blanks in older  Ref-
	 * erences content.
	 */
}


int /* return code is currently ignored! */
post_response (
	char *group,
	int respnum,
	int copy_text,
	t_bool with_headers)
{
	FILE *fp;
	char ch, *ptr;
	char bigbuf[HEADER_LEN];
	char buf[HEADER_LEN];
	char from_name[HEADER_LEN];
	char initials[64];
	int art_type = GROUP_TYPE_NEWS;
	int ret_code = POSTED_NONE;
	struct t_group *psGrp;
#ifdef FORGERY
	char line[HEADER_LEN];
#endif /* FORGERY */

	msg_init_headers ();

	wait_message (0, txt_post_a_followup);

	/*
	 * Remove duplicates in Newsgroups and Followup-To line
	 */
	strip_double_ngs (note_h.newsgroups);
	if (*note_h.followup)
		strip_double_ngs (note_h.followup);

	if (*note_h.followup && STRCMPEQ(note_h.followup, "poster")) {
/*		clear_message (); */
		ch = prompt_slk_response(iKeyPageMail, "\033mpyq", txt_resp_to_poster);
		switch (ch) {
			case iKeyPostPost:
			case iKeyPostPost2:
				break;
			case iKeyQuit:
			case iKeyAbort:
				return ret_code;
			default:
			{
				char save_followup[HEADER_LEN];

				strcpy (save_followup, note_h.followup);
				*note_h.followup = '\0';
				find_reply_to_addr (/*respnum,*/ buf, FALSE);
				mail_to_someone (respnum, buf, TRUE, FALSE, (t_bool *) &ret_code);	/* FIXME: cast for arg5 */
				strcpy (note_h.followup, save_followup);
				return ret_code;
			}
		}
	} else if (*note_h.followup && strcmp (note_h.followup, group) != 0
		   && strcmp (note_h.followup, note_h.newsgroups) != 0) {
		/*
		 * note that comparing newsgroups and followup-to isn't
		 * really correct, since the order of the newsgroups may be
		 * different, but testing that also isn't really worth
		 * it. The main culprit for the duplication is tin <=1.22, BTW.
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

		ch = prompt_slk_response(iKeyPostPost, "\033ipqy", txt_prompt_fup_ignore);
		switch (ch) {
			case iKeyQuit:
			case iKeyAbort:
				return ret_code;

			case iKeyPostIgnore:
				note_h.followup[0]='\0';
				break;

			case iKeyPostPost:
			case iKeyPostPost2:
			default:
				break;
		}
	}

	if ((fp = fopen (article, "w")) == NULL) {
		perror_message (txt_cannot_open, article);
		return ret_code;
	}
	chmod (article, (mode_t)(S_IRUSR|S_IWUSR));

	psGrp = psGrpFind (group);
#ifndef INDEX_DAEMON
	get_from_name(from_name, psGrp);
#endif /* !INDEX_DAEMON */
#ifdef FORGERY
	make_path_header (line);
	msg_add_header ("Path", line);
#endif /* FORGERY */
	msg_add_header ("From", from_name);

	sprintf (bigbuf, "Re: %s", eat_re (note_h.subj, TRUE));
	msg_add_header ("Subject", bigbuf);

	if (psGrp && psGrp->attribute->x_comment_to && *note_h.from)
		msg_add_header ("X-Comment-To", note_h.from);
	if (*note_h.followup && strcmp (note_h.followup, "poster") != 0) {
		msg_add_header ("Newsgroups", note_h.followup);
		if (tinrc.prompt_followupto)
			msg_add_header("Followup-To", (strchr(note_h.followup, ',') != (char *) 0) ? note_h.followup : "");
	} else {
		if (psGrp && psGrp->attribute->mailing_list) {
			msg_add_header ("To", psGrp->attribute->mailing_list);
			art_type = GROUP_TYPE_MAIL;
		} else {
			msg_add_header ("Newsgroups", note_h.newsgroups);
			if (tinrc.prompt_followupto)
				msg_add_header("Followup-To",
				(strchr(note_h.newsgroups, ',') != (char *) 0) ? note_h.newsgroups : "");
			if (psGrp && psGrp->attribute->followup_to != (char *) 0) {
				msg_add_header ("Followup-To", psGrp->attribute->followup_to);
			} else {
				ptr = strchr (note_h.newsgroups, ',');
				if (ptr)
					msg_add_header ("Followup-To", note_h.newsgroups);
			}
		}
	}

	/*
	 * Append to References: line if its already there
	 */
	if (*note_h.references) {
		join_references (bigbuf, note_h.references, note_h.messageid);
		msg_add_header ("References", bigbuf);
	} else
		msg_add_header ("References", note_h.messageid);

	if (psGrp && psGrp->attribute->organization != (char *) 0)
		msg_add_header ("Organization", random_organization(psGrp->attribute->organization));

	if (*reply_to)
		msg_add_header ("Reply-To", reply_to);

	if (art_type != GROUP_TYPE_MAIL) {
		ADD_MSG_ID_HEADER();
		if (*note_h.distrib)
			msg_add_header ("Distribution", note_h.distrib);
		else if (*my_distribution)
			msg_add_header ("Distribution", my_distribution);
	}

	if (*note_h.authorids)
		msg_add_header ("Author-IDs", note_h.authorids);

	msg_add_x_headers (msg_headers_file);
	msg_add_x_headers (psGrp->attribute->x_headers);
	{
		struct t_header_list *pptr;

		for (pptr = note_h.persist; pptr != NULL; pptr = pptr->next)
			msg_add_header(pptr->header, pptr->content);
	}
	start_line_offset = msg_write_headers (fp) + 1;
	msg_free_headers ();
	start_line_offset += msg_add_x_body (fp, psGrp->attribute->x_body);

	if (copy_text) {
		if (arts[respnum].xref && is_crosspost(arts[respnum].xref)) {
			if (strfquote (CURR_GROUP.name, respnum, buf, sizeof (buf), tinrc.xpost_quote_format))
				fprintf (fp, "%s\n", buf);
		} else if (strfquote (group, respnum, buf, sizeof (buf), (psGrp && psGrp->attribute->news_quote_format != (char *) 0) ? psGrp->attribute->news_quote_format : tinrc.news_quote_format))
			fprintf (fp, "%s\n", buf);
		start_line_offset++;

		/*
		 * check if tinrc.xpost_quote_format or tinrc.news_quote_format
		 * is longer than 1 line and correct start_line_offset
		 */
		{
			char *s;

			for (s = buf; *s; s++) {
				if (*s == '\n')
					++start_line_offset;
			}
		}

		fseek (note_fp, (with_headers ? 0L : mark_body), SEEK_SET);

		get_initials (respnum, initials, sizeof (initials));
		copy_body (note_fp, fp,
			   (psGrp && psGrp->attribute->quote_chars != (char *) 0) ? psGrp->attribute->quote_chars : tinrc.quote_chars,
			   initials, with_headers ? TRUE : tinrc.quote_signatures);
	} else
		fprintf (fp, "\n");	/* add a newline to keep vi from bitching */

	msg_write_signature (fp, FALSE, &CURR_GROUP);
	fclose (fp);

	return (post_loop(POST_RESPONSE, psGrp, iKeyPostEdit, txt_posting, art_type, start_line_offset));
}


/*
 * Generates the basic header for a mailed article
 * Returns NULL if article couldn't be created
 * The name of the temp. article file is written to 'filename'
 */
static FILE *
create_mail_headers(
	char *filename,
	const char *suffix,
	const char *to,
	const char *subject)
{
	FILE *fp;

	msg_init_headers ();

	joinpath (filename, homedir, suffix);
#if defined(APPEND_PID) && !defined (VMS)
	sprintf (filename+strlen(filename), ".%d", (int) process_id);
#endif /* APPEND_PID && !VMS */

	if ((fp = fopen (filename, "w")) == NULL) {
		perror_message (txt_cannot_open, filename);
		return NULL;
	}
	chmod (filename, (mode_t)(S_IRUSR|S_IWUSR));

	if (!tinrc.use_mailreader_i) {	/* tin should start editor */

		if (*tinrc.mail_address)
			msg_add_header ("From", tinrc.mail_address);

		msg_add_header ("To", to);
		msg_add_header ("Subject", subject);

		if (tinrc.auto_cc)
			msg_add_header ("Cc", userid);

		if (tinrc.auto_bcc)
			msg_add_header ("Bcc", userid);

		if (*default_organization)
			msg_add_header ("Organization", random_organization(default_organization));

		if (*reply_to)
			msg_add_header ("Reply-To", reply_to);

		msg_add_x_headers (msg_headers_file);
	}
	start_line_offset = msg_write_headers (fp) + 1;
	msg_free_headers ();

	return fp;
}


/*
 * Handle editing/spellcheck/PGP etc., operations on a mail article
 * Submit/abort the article as required and return POSTED_{NONE,REDRAW,OK}
 */
#if 0
int
mail_loop(
	char *filename,			/* Temp. filename being used */
	char ch				/* default prompt char */
	)
{
	int ret = POSTED_NONE;
	long artchanged = 0L;

	forever {
		switch (ch) {
			case iKeyPostEdit:
				artchanged = file_changed(filename);

				if (!(invoke_editor (filename, start_line_offset)))
					return ret;

				ret = POSTED_REDRAW;
				if (((artchanged == file_changed(filename)) && (prompt_yn (cLINES, txt_prompt_unchanged_bug, TRUE) > 0 )) || (file_size(filename) <= 0L)) {
					clear_message();
					return ret;
				}
				if (!pcCopyArtHeader (HEADER_SUBJECT, filename, subject))
					subject[0] = '\0';
				break;

#ifdef HAVE_ISPELL
			case iKeyPostIspell:
				invoke_ispell (filename, 0);
/*				ret = POSTED_REDRAW; TODO needed ? */
				break;
#endif /* HAVE_ISPELL */

#ifdef HAVE_PGP
			case iKeyPostPGP:
				my_strncpy (mail_to, arts[respnum].from, sizeof (mail_to));
				if (pcCopyArtHeader (HEADER_TO, filename, mail_to) && pcCopyArtHeader (HEADER_SUBJECT, filename, subject))
					invoke_pgp_mail (filename, mail_to);
				break;
#endif /* HAVE_PGP */

			case iKeyQuit:
			case iKeyAbort:
				clear_message ();
				return ret;

			case iKeyPostSend:
			case iKeyPostSend2:
/*				my_strncpy (mail_to, arts[respnum].from, sizeof (mail_to));*/
				checknadd_headers (filename);
				if (submit_mail_file (filename)) {
					info_message (txt_mailed, 1, IS_PLURAL(1));
					return POSTED_OK;
				}
				return ret;

			default:
				break;
		}
		ch = prompt_to_send(subject);
	}

	return ret;
}
#endif /* 0 */


/*
 * Return TRUE if screen redraw is needed
 */
t_bool
mail_to_someone (
	int respnum,
	const char *address,
	t_bool mail_to_poster,
	t_bool confirm_to_mail,
	t_bool *mailed_ok)		/* TODO this should be the return code */
{
	FILE *fp;
	char ch = iKeyPostSend;
	char nam[HEADER_LEN];
	char subject[HEADER_LEN];
	char mailreader_subject[PATH_LEN];	/* for calling external mailreader */
	char buf[HEADER_LEN];
	char mail_to[HEADER_LEN];
	char initials[64];
	t_bool redraw_screen = FALSE;

	*mailed_ok = FALSE;
	strcpy (mail_to, address);			/* strfmailer() won't take const arg 3 */
	clear_message ();

	if (mail_to_poster)
		sprintf (subject, "Re: %s\n", eat_re (note_h.subj, TRUE));
	else
		sprintf (subject, "(fwd) %s\n", note_h.subj);

	if ((fp = create_mail_headers(nam, TIN_LETTER, mail_to, subject)) == NULL)
		return redraw_screen;

	if (mail_to_poster) {
		ch = iKeyPostEdit;
		if (strfquote (CURR_GROUP.name, respnum, buf, sizeof (buf), tinrc.mail_quote_format)) {
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
		copy_body (note_fp, fp, tinrc.quote_chars, initials, tinrc.quote_signatures);
	} else {
		rewind (note_fp);
		fprintf (fp, "-- forwarded message --\n");
		copy_fp (note_fp, fp);
		fprintf (fp, "-- end of forwarded message --\n");
	}

	if (!tinrc.use_mailreader_i)
		msg_write_signature (fp, TRUE, &CURR_GROUP);

#ifdef WIN32
	putc ('\0', fp);
#endif /* WIN32 */
	fclose (fp);

	if (tinrc.use_mailreader_i) {	/* user wants to use his own mailreader */
		ch = iKeyAbort;
		redraw_screen = TRUE;
		if (mail_to_poster)
			sprintf (mailreader_subject, "Re: %s", eat_re (note_h.subj, TRUE));
		else
			sprintf (mailreader_subject, "(fwd) %s", note_h.subj);
		strfmailer (mailer, mailreader_subject, mail_to, nam, buf, sizeof (buf), tinrc.mailer_format);
		if (!invoke_cmd (buf))
			error_message (txt_command_failed, buf);		/* TODO - not needed */
	}

	forever {
		if (confirm_to_mail && (!tinrc.use_mailreader_i))
			ch = prompt_to_send(subject);

		switch (ch) {
			case iKeyPostEdit:
				if (!invoke_editor (nam, start_line_offset))
					goto mail_to_someone_done;
				if (!pcCopyArtHeader (HEADER_SUBJECT, nam, subject))
					subject[0] = '\0';
				redraw_screen = TRUE;
				break;

#ifdef HAVE_ISPELL
			case iKeyPostIspell:
				invoke_ispell (nam, 0);
				break;
#endif /* HAVE_ISPELL */

#ifdef HAVE_PGP
			case iKeyPostPGP:
				invoke_pgp_mail (nam, mail_to);
				break;
#endif /* HAVE_PGP */

			case iKeyQuit:
			case iKeyAbort:
				clear_message ();
				goto mail_to_someone_done;

			case iKeyPostSend:
			case iKeyPostSend2:
				/*
				 * Open letter and get the To: line in
				 * case they changed it with the editor
				 */
				*mailed_ok = submit_mail_file (nam);
				goto mail_to_someone_done;

			default:
				break;
		}
		if (mail_to_poster)
			ch = prompt_to_send(subject);
	}

mail_to_someone_done:
	if (tinrc.unlink_article)
		unlink (nam);

	return redraw_screen;
}


t_bool
mail_bug_report (
	void) /* FIXME: return value is always ignored */
{
	FILE *fp;
	const char *domain;
	char ch;
	char buf[LEN], nam[100];
	char mail_to[HEADER_LEN];
	char subject[HEADER_LEN];
	int ret_code = FALSE;
	long artchanged = 0L;
	t_bool is_nntp = FALSE, is_nntp_only;

	wait_message (1, txt_mail_bug_report);

	sprintf (subject, "BUG REPORT %s\n", page_header);

	if ((fp = create_mail_headers(nam, ".bugreport", bug_addr, subject)) == NULL)
		return FALSE;

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

#ifdef NNTP_ONLY
	is_nntp_only = TRUE;
#else
	is_nntp_only = FALSE;
#	ifdef NNTP_ABLE
	is_nntp = TRUE;
#	endif /* NNTP_ABLE */
#endif /* NNTP_ONLY */
#ifdef DOMAIN_NAME
	domain = DOMAIN_NAME;
#else
	domain = "";
#endif /* DOMAIN_NAME */
	fprintf (fp, "\nCFG1: active=%d  arts=%d  reread=%d  longfilenames=%d  setuid=%d\n",
		DEFAULT_ACTIVE_NUM,
		DEFAULT_ARTICLE_NUM,
		tinrc.reread_active_file_secs,
#ifdef HAVE_LONG_FILE_NAMES
		TRUE,
#else
		FALSE,
#endif /* HAVE_LONG_FILE_NAMES */
		(tin_uid == real_uid ? 0 : 1));
	fprintf (fp, "CFG2: nntp=%d  nntp_only=%d  nntp_xover=%d\n",
		is_nntp,
		is_nntp_only,
		xover_supported);
	fprintf (fp, "CFG3: debug=%d domain=[%s]\n",
#ifdef DEBUG
		TRUE,
#else
		FALSE,
#endif /* DEBUG */
		*domain ? domain : "");
	fprintf (fp, "CFG4: threading=%d\n", tinrc.thread_articles);
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

	start_line_offset += 4;

	if (!tinrc.use_mailreader_i)
		msg_write_signature (fp, TRUE, (cur_groupnum == -1) ? NULL : &CURR_GROUP);

#ifdef WIN32
	putc ('\0', fp);
#endif /* WIN32 */
	fclose (fp);

	if (tinrc.use_mailreader_i) {	/* user wants to use his own mailreader */
		ch = iKeyAbort;
		sprintf (subject, "BUG REPORT %s", page_header);
		sprintf (mail_to, "%s", bug_addr);
		strfmailer (mailer, subject, mail_to, nam, buf, sizeof (buf), tinrc.mailer_format);
		if (!invoke_cmd (buf))
			error_message (txt_command_failed, buf);		/* TODO not needed */
	} else
		ch = iKeyPostEdit;

	forever {
		switch (ch) {
			case iKeyPostEdit:
				artchanged = file_changed(nam);
				if (!(invoke_editor (nam, start_line_offset)))
					goto mail_bug_report_done;

				if (((artchanged == file_changed(nam)) && (prompt_yn (cLINES, txt_prompt_unchanged_bug, TRUE) > 0 )) || (file_size(nam) <= 0L)) {
					unlink (nam);
					clear_message ();
					return TRUE;
				} else {
					if (!pcCopyArtHeader (HEADER_SUBJECT, nam, subject))
						subject[0] = '\0';
				}
				break;

#ifdef HAVE_ISPELL
			case iKeyPostIspell:
				invoke_ispell (nam, 0);
				break;
#endif /* HAVE_ISPELL */

#ifdef HAVE_PGP
			case iKeyPostPGP:
				invoke_pgp_mail (nam, mail_to);
				break;
#endif /* HAVE_PGP */

			case iKeyQuit:
			case iKeyAbort:
				clear_message ();
				goto mail_bug_report_done;

			case iKeyPostSend:
			case iKeyPostSend2:
				sprintf (mesg, txt_mail_bug_report_confirm, bug_addr);
				if (prompt_yn (cLINES, mesg, FALSE) == 1) {
					if (submit_mail_file (nam)) {
						ret_code = TRUE;
						info_message (txt_mailed, 1, IS_PLURAL(1));
					}
				}
				goto mail_bug_report_done;

			default:
				break;
		}
		ch = prompt_to_send(subject);
	}

mail_bug_report_done:
	unlink (nam);
	return ret_code;
}


t_bool
mail_to_author (
	char *group,
	int respnum,
	int copy_text,
	t_bool with_headers) /* return value is always ignored */
{
	FILE *fp;
	char ch;
	char buf[LEN];
	char from_addr[HEADER_LEN];
	char nam[100];
	char mail_to[HEADER_LEN];
	char subject[HEADER_LEN];
	char mailreader_subject[PATH_LEN];	/* for calling external mailreader */
	char initials[64];
	t_bool redraw_screen = FALSE;
	t_bool spamtrap_found = FALSE;

	wait_message (0, txt_reply_to_author);

	msg_init_headers ();

	joinpath (nam, homedir, TIN_LETTER);

#if defined(APPEND_PID) && !defined (VMS)
	sprintf (nam+strlen(nam), ".%d", (int) process_id);
#endif /* APPEND_PID && !VMS */

	if ((fp = fopen (nam, "w")) == NULL) {
		perror_message (txt_cannot_open, nam);
		return redraw_screen;
	}
	chmod (nam, (mode_t)(S_IRUSR|S_IWUSR));

	sprintf (subject, "Re: %s\n", eat_re (note_h.subj, TRUE));

	if (!tinrc.use_mailreader_i) {	/* tin should start editor */

		if (*tinrc.mail_address)
			msg_add_header ("From", tinrc.mail_address);

		find_reply_to_addr (/*respnum,*/ from_addr, FALSE);

		msg_add_header ("To", from_addr);
		spamtrap_found = check_for_spamtrap(from_addr);
		msg_add_header ("Subject", subject);

		if (tinrc.auto_cc)
			msg_add_header ("Cc", userid);

		if (tinrc.auto_bcc)
			msg_add_header ("Bcc", userid);

		/*
		 * remove duplicates from Newsgroups header
		 */
		strip_double_ngs (note_h.newsgroups);

		/*
		 * rename Newsgroups: to X-Newsgroups as RFC 822 dosn't define it.
		 */
		msg_add_header ("X-Newsgroups", note_h.newsgroups);

		/*
		 * Write Message-Id as In-Reply-To to the mail
		 */
		msg_add_header ("In-Reply-To", note_h.messageid);

		if (*default_organization)
			msg_add_header ("Organization", random_organization(default_organization));

		if (*reply_to)
			msg_add_header ("Reply-To", reply_to);

		msg_add_x_headers (msg_headers_file);
	}
	start_line_offset = msg_write_headers (fp) + 1;
	msg_free_headers ();

	if (copy_text) {
		if (strfquote (group, respnum, buf, sizeof (buf), tinrc.mail_quote_format)) {
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

		fseek (note_fp, (with_headers ? 0L : mark_body), SEEK_SET);

		get_initials (respnum, initials, sizeof (initials));
		copy_body (note_fp, fp, tinrc.quote_chars, initials, with_headers ? TRUE : tinrc.quote_signatures);
	} else
		fprintf (fp, "\n");	/* add a newline to keep vi from bitching */

	if (!tinrc.use_mailreader_i)
		msg_write_signature (fp, TRUE, &CURR_GROUP);

#ifdef WIN32
	putc ('\0', fp);
#endif /* WIN32 */
	fclose (fp);

	if (spamtrap_found) {
		ch = prompt_slk_response (iKeyPostContinue, TIN_CONT_KEYS, txt_warn_suspicious_mail);
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

	if (tinrc.use_mailreader_i) {	/* user wants to use his own mailreader for reply */
		ch = iKeyAbort;
		sprintf (mailreader_subject, "Re: %s", eat_re (note_h.subj, TRUE));
		find_reply_to_addr (/* respnum, */ mail_to, TRUE);
		strfmailer (mailer, mailreader_subject, mail_to, nam, buf, sizeof (buf), tinrc.mailer_format);
		if (!invoke_cmd (buf))
			error_message (txt_command_failed, buf);		/* TODO not needed */
	} else
		ch = iKeyPostEdit;

	forever {
		switch (ch) {
			case iKeyPostEdit:
				if (!(invoke_editor (nam, start_line_offset)))
					goto mail_to_author_failed;

				if (!pcCopyArtHeader (HEADER_SUBJECT, nam, subject))
					subject[0] = '\0';
				redraw_screen = TRUE;
				break;

#ifdef HAVE_ISPELL
			case iKeyPostIspell:
				invoke_ispell (nam, 0);
				break;
#endif /* HAVE_ISPELL */

#ifdef HAVE_PGP
			case iKeyPostPGP:
				my_strncpy (mail_to, arts[respnum].from, sizeof (mail_to));
				if (pcCopyArtHeader (HEADER_TO, nam, mail_to) && pcCopyArtHeader (HEADER_SUBJECT, nam, subject))
					invoke_pgp_mail (nam, mail_to);
				break;
#endif /* HAVE_PGP */

			case iKeyQuit:
			case iKeyAbort:
				clear_message ();
				goto mail_to_author_failed;

			case iKeyPostSend:
			case iKeyPostSend2:
/*				my_strncpy (mail_to, arts[respnum].from, sizeof (mail_to));*/
				checknadd_headers (nam);
				if (submit_mail_file (nam)) {
					redraw_screen = TRUE;
					info_message (txt_mailed, 1, IS_PLURAL(1));
					goto mail_to_author_done;
				}
				goto mail_to_author_failed;

			default:
				break;
		}
		ch = prompt_to_send(subject);
	}

mail_to_author_done:
	update_posted_info_file (group, 'r', subject);
mail_to_author_failed:
	unlink (nam);
	return redraw_screen;
}


/*
 * compare the given e-mail address with a list of components in $SPAMTRAP
 */
static t_bool
check_for_spamtrap (
	char *addr)
{
	char *env, *ptr;

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
	if ((env = my_strdup(tinrc.spamtrap_warning_addresses)) != (char *) 0) {
		char *tmp = env;
		while (strlen(tmp)) {
			ptr = strchr(tmp, ',');
			if (ptr != NULL)
				*ptr = '\0';
			if (strcasestr(addr, tmp) != NULL) {
				free(env);
				return TRUE;
			}
			tmp += strlen(tmp);
			if (ptr != NULL)
				tmp++;
		}
		free(env);
	}
	return FALSE;
}


/*
 *  Read a file grabbing the value of the specified mail header line
 */
static t_bool
pcCopyArtHeader (
	int iHeader,
	char *pcArt,
	char *result)
{
	FILE *fp;
	char *ptr;
	char buf2[HEADER_LEN];
	const char *p;
	static char header[HEADER_LEN];
	t_bool found = FALSE;
	t_bool was_to = FALSE;

	*header = '\0';

	if ((fp = fopen (pcArt, "r")) == (FILE *) 0) {
		perror_message (txt_cannot_open, pcArt);
		return FALSE;
	}

	while ((ptr = tin_fgets (fp, TRUE)) != (char *) 0) {

		if (*ptr == '\0')
			break;

		switch (iHeader) {
			case HEADER_TO:
				if (STRNCASECMPEQ(ptr, "To: ", 4) || STRNCASECMPEQ(ptr, "Cc: ", 4)) {
					my_strncpy (buf2, &ptr[4], sizeof (buf2));
					yank_to_addr (buf2, header);
					was_to = TRUE;
					found = TRUE;
				} else if (STRNCASECMPEQ(ptr, "Bcc: ", 5)) {
					my_strncpy (buf2, &ptr[5], sizeof (buf2));
					yank_to_addr (buf2, header);
					was_to = TRUE;
					found = TRUE;
				} else if ((*ptr == ' ' || *ptr == '\t') && was_to) {
					yank_to_addr (ptr, header);
					found = TRUE;
				} else
					was_to = FALSE;

				break;

			case HEADER_NEWSGROUPS:
				if (match_string (ptr, "Newsgroups: ", header, sizeof (header)))
					found = TRUE;

				break;

			case HEADER_SUBJECT:
				if (STRNCASECMPEQ(ptr, "Subject: ", 9)) {
					my_strncpy (header, &ptr[9], sizeof (header));
					found = TRUE;
				}
				break;

			default:
				break;
		}
	}
	fclose (fp);

	if (tin_errno != 0)
		return(FALSE);

	if (found) {
		p = ((header[0] == ' ') ? &header[1] : header);
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

	/*
	 * This should show the name of the offending file, but I didn't want to
	 * add unnecessary message-text.
	 */
	error_message (p, pcArt);
	return FALSE;
}


#ifndef INDEX_DAEMON
t_bool
cancel_article (
	struct t_group *group,
	struct t_article *art,
	int respnum)
{
	FILE *fp;
	char ch, ch_default = iKeyPostCancel;
	char option = iKeyPostCancel;
	char option_default = iKeyPostCancel;
	char buf[HEADER_LEN];
	char cancel[HEADER_LEN];
	char from_name[HEADER_LEN];
#ifdef FORGERY
	char line[HEADER_LEN];
	char line2[HEADER_LEN];
	t_bool author = TRUE;
#else
	char user_name[128];
	char full_name[128];
#endif /* FORGERY */
	int init = 1;
	int oldraw;
	t_bool redraw_screen = TRUE;

	msg_init_headers ();

	/*
	 * Check if news / mail / save group
	 */
	if (group->type == GROUP_TYPE_MAIL || group->type == GROUP_TYPE_SAVE) {
#ifndef INDEX_DAEMON
		vGrpDelMailArt (art);
#endif /* !INDEX_DAEMON */
		return FALSE;
	}
#ifndef INDEX_DAEMON
	get_from_name (from_name, group);
#endif /* !INDEX_DAEMON */
#ifdef FORGERY
	make_path_header (line);
#else
#	ifndef INDEX_DAEMON
	get_user_info (user_name, full_name);
#	endif /* !INDEX_DAEMON */
#endif /* FORGERY */
#ifdef DEBUG
	if (debug == 2)
		error_message ("From=[%s]  Cancel=[%s]", art->from, from_name);
#endif /* DEBUG */
	if (!strcasestr (from_name, art->from)) {
#ifdef FORGERY
		author = FALSE;
#else
		wait_message (3, txt_art_cannot_cancel);
		return redraw_screen;
#endif /* FORGERY */
	} else {
		option = prompt_slk_response (option_default, "\033dqs", sized_message(txt_cancel_article, art->subject));

		switch (option) {
			case iKeyPostCancel:
				break;
			case iKeyPostSupersede:
				repost_article (note_h.newsgroups, respnum, TRUE);
				return redraw_screen;
			default:
				return redraw_screen;
		}
	}

	clear_message ();

	joinpath (cancel, homedir, ".cancel");
#ifdef APPEND_PID
	sprintf (cancel+strlen(cancel), ".%d", (int) process_id);
#endif /* APPEND_PID */

	if ((fp = fopen (cancel, "w")) == (FILE *) 0) {
		perror_message (txt_cannot_open, cancel);
		return redraw_screen;
	}
	chmod (cancel, (mode_t)(S_IRUSR|S_IWUSR));

#ifdef FORGERY
	if (!author) {
		sprintf (line2, "cyberspam!%s", line);
		msg_add_header ("Path", line2);
		msg_add_header ("From", note_h.from);
		sprintf (line, "<cancel.%s", note_h.messageid + 1);
		msg_add_header ("Message-ID", line);
		msg_add_header ("X-Cancelled-By", from_name);
/*		msg_add_header ("X-Orig-Subject", note_h.subj); */
	} else {
		msg_add_header ("Path", line);
		if (art->name)
			sprintf (line, "%s <%s>", art->name, art->from);
		else
			sprintf (line, "<%s>", art->from);
		msg_add_header ("From", line);
		ADD_CAN_KEY(note_h.messageid);
	}
#else
	msg_add_header ("From", from_name);
	ADD_MSG_ID_HEADER();
	ADD_CAN_KEY(note_h.messageid);
#endif /* FORGERY */
	sprintf (buf, "cmsg cancel %s", note_h.messageid);
	msg_add_header ("Subject", buf);

	/*
	 * remove duplicates from Newsgroups header
	 */
	strip_double_ngs (note_h.newsgroups);

	msg_add_header ("Newsgroups", note_h.newsgroups);
	if (tinrc.prompt_followupto)
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

	/* some ppl. like X-Headers: in cancels */
	msg_add_x_headers (msg_headers_file);
	msg_add_x_headers (group->attribute->x_headers);

	msg_write_headers (fp);
	msg_free_headers ();

#ifdef FORGERY
	if (author)
		fprintf (fp, txt_article_cancelled);
	else {
		rewind (note_fp);
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
#endif /* FORGERY */
	my_fprintf (stderr, txt_warn_cancel);

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
					error_message (txt_command_failed, cancel);	/* TODO not needed */
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
	/* NOTREACHED */
	return redraw_screen;
}
#endif /* !INDEX_DAEMON */


/*
 * Repost an already existing article to another group (ie. local group)
 */

#define FromSameUser (strcasestr (from_name, arts[respnum].from))

#ifndef FORGERY
#	define NotSuperseding	(!supersede || (supersede && (!FromSameUser)))
#	define Superseding	(supersede && FromSameUser)
#else
#	define NotSuperseding	(!supersede)
#	define Superseding	(supersede)
#endif /* !FORGERY */

#ifndef INDEX_DAEMON
int
repost_article (
	const char *group,
	int respnum,
	t_bool supersede)
{
	FILE *fp;
	char ch;
	char ch_default = iKeyPostPost;
	char buf[HEADER_LEN];
	char from_name[HEADER_LEN];
	char full_name[128];
	char user_name[128];
	int art_type = GROUP_TYPE_NEWS;
	int ret_code = POSTED_NONE;
	struct t_group *psGrp;
	t_bool force_command = FALSE;
#	ifdef FORGERY
	char line[HEADER_LEN];
#	endif /* FORGERY */

	msg_init_headers ();

	/*
	 * remove duplicates from Newsgroups header
	 */
	strip_double_ngs (note_h.newsgroups);

	/*
	 * Check if any of the newsgroups are moderated.
	 */
	if ((psGrp = check_moderated (group, &art_type, txt_art_not_posted)) == NULL)
		return ret_code;

	if ((fp = fopen (article, "w")) == (FILE *) 0) {
		perror_message (txt_cannot_open, article);
		return ret_code;
	}
	chmod (article, (mode_t)(S_IRUSR|S_IWUSR));

	if (supersede) {
		get_user_info (user_name, full_name);
		get_from_name (from_name, psGrp);
#	ifndef FORGERY
		if (FromSameUser) {
			msg_add_header ("From", from_name);
			if (*reply_to)
				msg_add_header ("Reply-To", reply_to);
			ADD_MSG_ID_HEADER();
			ADD_CAN_KEY(note_h.messageid);
#	else
		{
			make_path_header (line);
			msg_add_header ("Path", line);

			msg_add_header ("From", (*note_h.from ? note_h.from : from_name));
			find_reply_to_addr (/* respnum,*/ line, FALSE);
			msg_add_header ("Reply-To", line);
			msg_add_header ("X-Superseded-By", from_name);

			if (*note_h.org)
				msg_add_header ("Organization", note_h.org);

			sprintf (line, "<supersede.%s", note_h.messageid + 1);
			msg_add_header ("Message-ID", line);
			/* ADD_CAN_KEY(note_h.messageid); */ /* should we add key here? */
#	endif /* !FORGERY */
			msg_add_header ("Supersedes", note_h.messageid);

			if (*note_h.followup)
				msg_add_header ("Followup-To", note_h.followup);

			if (*note_h.keywords)
				msg_add_header ("Keywords", note_h.keywords);

			if (*note_h.summary)
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
	ADD_MSG_ID_HEADER();

	if (*note_h.references) {
		join_references (buf, note_h.references, (NotSuperseding ? note_h.messageid : ""));
		msg_add_header ("References", buf);
	}
	if (NotSuperseding) {
		if (psGrp->attribute->organization != (char *) 0)
			msg_add_header ("Organization", random_organization(psGrp->attribute->organization));
		else if (*default_organization)
			msg_add_header ("Organization", random_organization(default_organization));

		if (*reply_to)
			msg_add_header ("Reply-To", reply_to);

		if (*my_distribution)
			msg_add_header ("Distribution", my_distribution);

	}

/* some ppl. like X-Headers: in reposts */
/*	if (Superseding) { */
		/* X-Headers got lost on supersede, readd */
		msg_add_x_headers (msg_headers_file);
		msg_add_x_headers (psGrp->attribute->x_headers);
/*	} */

	start_line_offset = msg_write_headers (fp) + 1;
	msg_free_headers ();

	if (NotSuperseding) {
		fprintf (fp, "[ %-72s ]\n", txt_article_reposted);
		/*
		 * all string lengths are calculated to a maximum line length
		 * of 76 characters, this should look ok (sven@tin.org)
		 */
		if (*note_h.from)
			fprintf (fp, "[ From: %-66s ]\n", note_h.from);
		if (*note_h.subj)
			fprintf (fp, "[ Subject: %-63s ]\n", note_h.subj);
		if (*note_h.newsgroups)
			fprintf (fp, "[ Newsgroups: %-60s ]\n", note_h.newsgroups);
		if (*note_h.messageid)
			fprintf (fp, "[ Message-ID: %-60s ]\n\n", note_h.messageid);
	}
	fseek (note_fp, mark_body, SEEK_SET);
	copy_fp (note_fp, fp);

/* only append signature when NOT superseeding own articles */
	if (NotSuperseding && tinrc.signature_repost)
		msg_write_signature (fp, FALSE, psGrp);

	fclose (fp);

	/* on supersede change default-key */
	/*
	 * FIXME: this is only usefull when entering the editor
	 * after leaving the editor it should be iKeyPostPost
	 */
	if (Superseding) {
		ch_default = iKeyPostEdit;
		force_command = TRUE;
	}

	ch = (force_command ? ch_default : prompt_slk_response (ch_default, TIN_POST_KEYS, sized_message(txt_quit_edit_xpost, note_h.subj)));

	return (post_loop(POST_REPOST, psGrp, ch,
				(Superseding ? txt_superseding_art : txt_repost_an_article),
				art_type, start_line_offset));
}
#endif /* !INDEX_DAEMON */


static void
msg_add_x_headers (
	char *headers)
{
	FILE *fp;
	char *ptr;
	char file[PATH_LEN];
	char line[HEADER_LEN];

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
		} else {
		/*
		 * without this else a "x_headers=name" without a ':' would be
		 * treated as a filename in the current dir - IMHO not very usefull
		 */
			if (!strfpath (headers, file, sizeof (file), homedir, (char *) 0, (char *) 0, (char *) 0))
				strcpy (file, headers);

			if ((fp = fopen (file, "r")) != (FILE *) 0) {
				while (fgets (line, (int) sizeof(line), fp) != (char *) 0) {
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
}


/*
 * Add an x_body attribute to an article if it exists.
 * Can be a piece of text or the name of a file to append
 * Returns the # of lines appended.
 */
static int
msg_add_x_body (
	FILE *fp_out,
	char *body)
{
	FILE *fp;
	char *ptr;
	char file[PATH_LEN];
	char line[HEADER_LEN];
	int wrote = 0;

	if (!body)
		return 0;

	if (body[0] != '/' && body[0] != '~') {
		strncpy (line, body, sizeof(line));
		if ((ptr = strrchr (line, '\n')) != NULL)
			*ptr = '\0';

		fprintf (fp_out, "%s\n", line);
		wrote++;
	} else {
		if (!strfpath (body, file, sizeof (file), homedir, (char *) 0, (char *) 0, (char *) 0))
			strcpy (file, body);

		if ((fp = fopen (file, "r")) != (FILE *) 0) {
			while (fgets (line, (int) sizeof(line), fp) != (char *) 0) {
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
	return wrote;
}


static void
modify_headers (
	char *line)
{
	char *chr;
	char *chr2;
	char buf[HEADER_LEN];

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
	FILE *fp_in, *fp_out;
	char line[HEADER_LEN];
	char outfile[PATH_LEN];
	t_bool gotit = FALSE;

	if ((fp_in = fopen (infile, "r")) != (FILE *) 0) {
#ifdef VMS
		sprintf (outfile, "%s-%d", infile, (int) process_id);
#else
		sprintf (outfile, "%s.%d", infile, (int) process_id);
#endif /* VMS */
		*found_newsgroups = '\0';
		if ((fp_out = fopen (outfile, "w")) != (FILE *) 0) {
			while (fgets (line, (int) sizeof(line), fp_in) != (char *) 0) {
				if (!gotit && line[0] == '\n') {
					if (tinrc.advertising)
#ifdef HAVE_SYS_UTSNAME_H
#	ifdef _AIX
						fprintf (fp_out, "User-Agent: %s/%s-%s (\"%s\") (%s) (%s/%s-%s)\n\n",
							PRODUCT, VERSION, RELEASEDATE, RELEASENAME, OSNAME,
							system_info.sysname, system_info.version, system_info.release);
#	else /* AIX */
						fprintf (fp_out, "User-Agent: %s/%s-%s (\"%s\") (%s) (%s/%s (%s))\n\n",
							PRODUCT, VERSION, RELEASEDATE, RELEASENAME, OSNAME,
							system_info.sysname, system_info.release, system_info.machine);
#	endif /* AIX */
#else
						fprintf (fp_out, "User-Agent: %s/%s-%s (\"%s\") (%s)\n\n",
							PRODUCT, VERSION, RELEASEDATE, RELEASENAME, OSNAME);
#endif /* HAVE_SYS_UTSNAME_H */
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


#if !defined(M_AMIGA) && !defined(INDEX_DAEMON)
static t_bool
insert_from_header (
	char *infile)
{
	FILE *fp_in, *fp_out;
	char from_name[HEADER_LEN];
#if 0 /* unused */
	char full_name[128];
	char user_name[128];
#endif /* 0 */
	char line[HEADER_LEN];
	char outfile[PATH_LEN];
	t_bool from_found = FALSE;
	t_bool in_header = TRUE;

	if ((fp_in = fopen (infile, "r")) != (FILE *) 0) {
#	ifdef VMS
		sprintf (outfile, "%s-%d", infile, (int) process_id);
#	else
		sprintf (outfile, "%s.%d", infile, (int) process_id);
#	endif /* VMS */
		if ((fp_out = fopen (outfile, "w")) != (FILE *) 0) {
#if 0 /* unused */
			get_user_info (user_name, full_name);
#endif /* 0 */
			strcpy (from_name, "From: ");
			get_from_name (from_name + 6, (struct t_group *) 0);

#ifdef DEBUG
			if (debug == 2)
				wait_message (2, "insert_from_header [%s]", from_name + 6);
#endif /* DEBUG */

			/* Check the From: line */
			if (GNKSA_OK != gnksa_check_from(rfc1522_encode(from_name, FALSE) + 6)) { /* error in address */
				error_message (txt_invalid_from, from_name + 6);
				return FALSE;
			}

			while ((fgets (line, (int) sizeof(line), fp_in) != (char *) 0) && in_header) {
				if (!strncasecmp(line, "From:", 5))
					from_found = TRUE;
				if (*line == '\n')
					in_header = FALSE;
			}

			if (!from_found)
				fprintf (fp_out, "%s\n", from_name);

			rewind (fp_in);
			while (fgets (line, (int) sizeof(line), fp_in) != (char *) 0)
				fputs (line, fp_out);

			fclose (fp_out);
			fclose (fp_in);
			rename_file (outfile, infile);

			return TRUE;
		}
	}
	return FALSE;
}
#endif /* !M_AMIGA && !INDEX_DAEMON */


static void
find_reply_to_addr (
	/* int respnum, */	/* we don't need that in the #else part */
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
	t_bool found = FALSE;
	long orig_offset;

	orig_offset = ftell (note_fp);
	rewind (note_fp);

	while (fgets (buf, (int) sizeof(buf), note_fp) != (char *) 0 && !found && buf[0] != '\n') {
		if (STRNCASECMPEQ(buf, "Reply-To: ", 10)) {
			strcpy (from_both, &buf[10]);
			ptr = strchr (from_both, '\n');
			if (ptr != (char *) 0)
				*ptr = '\0';
			parse_from (from_both, from_addr, from_name);
			if (*from_name)
				sprintf (buf, "%s (%s)", from_addr, from_name);
			found = TRUE;
		}
	}

	if (!found) {
		if (arts[respnum].name != (char *) 0 && arts[respnum].name != arts[respnum].from) {
			sprintf (buf, "%s (%s)", arts[respnum].from, arts[respnum].name);
			strcpy (from_addr, buf);
		} else
			sprintf (from_addr, "%s", arts[respnum].from);
	}
	fseek (note_fp, orig_offset, SEEK_SET);
#else
	char *ptr, buf[HEADER_LEN];
	char replyto[HEADER_LEN];
	char from[HEADER_LEN];
	char fullname[HEADER_LEN];
	long orig_offset;
	t_bool found_replyto = FALSE;

	orig_offset = ftell (note_fp);
	rewind (note_fp);

	while (fgets (buf, (int) sizeof (buf), note_fp) != (char *) 0 && buf[0] != '\n') {
		/* FIXME we don't process continuation lines, we should use tin_fgets() here */
		if (STRNCASECMPEQ(buf, "Reply-To: ", 10)) {
			strcpy (replyto, &buf[10]);
			ptr = strchr (replyto, '\n');
			if (ptr != (char *) 0)
				*ptr = '\0';

			found_replyto = TRUE;
		} else if (STRNCASECMPEQ(buf, "From: ", 6)) {
			strcpy (from, &buf[6]);
			ptr = strchr (from, '\n');
			if (ptr != (char *) 0)
				*ptr = '\0';

		}
	}

	/* We do this to save a redundant strcpy when we don't want to parse */

	if (parse) {
#	if 1
		parse_from ((found_replyto ? replyto : from), from_addr, fullname);
#	else
		/* Or should we decode full_addr? */
		parse_from ((found_replyto ? replyto : from), temp, fullname);
		strcpy (full_addr, rfc1522_decode(tmp));
#	endif /* 1 */
	} else
		strcpy (from_addr, rfc1522_decode((found_replyto ? replyto : from)));

	fseek (note_fp, orig_offset, SEEK_SET);
#endif /* 0 */
}


/*
 * If any arts have been posted by the user reread the active
 * file so that they are shown in the unread articles number
 * for each group at the group selection level.
 */
t_bool
reread_active_after_posting (
	void)
{
	int i;
	long lMinOld;
	long lMaxOld;
	struct t_group *psGrp;
	t_bool modified = FALSE;

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
#endif /* DEBUG */
						psGrp->newsrc.num_unread = psGrp->count;
					}
					if (psGrp->xmin != lMinOld || psGrp->xmax != lMaxOld) {
#ifdef DEBUG
						my_printf (cCRLF "Min/Max DIFF grp=[%s] old=[%ld-%ld] new=[%ld-%ld]",
							psGrp->name, lMinOld, lMaxOld, psGrp->xmin, psGrp->xmax);
						my_flush ();
#endif /* DEBUG */

						expand_bitmap (psGrp, 0);
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


static t_bool
submit_mail_file (
	char *file)
{
	char buf[HEADER_LEN];
	char mail_to[HEADER_LEN];
	char subject[HEADER_LEN];
	t_bool mailed = FALSE;

#if !defined(M_AMIGA) && !defined(INDEX_DAEMON)
	if (insert_from_header (file))
#endif /* !M_AMIGA && !INDEX_DAEMON */
	{
		if (pcCopyArtHeader (HEADER_TO, file, mail_to) && pcCopyArtHeader (HEADER_SUBJECT, file, subject)) {
			wait_message (0, txt_mailing_to, mail_to);

			rfc15211522_encode (file, txt_mime_encodings[tinrc.mail_mime_encoding], tinrc.mail_8bit_header, TRUE);

			strfmailer (mailer, subject, mail_to, file, buf, sizeof (buf), tinrc.mailer_format);

#ifdef VMS /* quick hack! M.St. 29.01.98 */
			{
				char *transport = getenv("MAIL$INTERNET_TRANSPORT");
				if (!transport)
					transport = "smtp";
				sprintf (buf, "mail/subject=\"%s\" %s %s%%\"%s\"", subject, file, transport, mail_to);
			}
#endif /* VMS */
			if (invoke_cmd (buf))
				mailed = TRUE;
		}
	}
	return mailed;
}


#ifdef FORGERY
static void
make_path_header (
	char *line)
{
	char full_name[128];
	char user_name[128];

	get_user_info (user_name, full_name);

	sprintf (line, "%s!%s", domain_name, user_name);
	return;
}
#endif /* FORGERY */


static void
yank_to_addr (
	char *orig,
	char *addr)
{
	char *p;
	int open_parens;

	for (p = orig; *p; p++)
		if (((*p) & 0xFF) < ' ')
			*p = ' ';

	while (*addr)
		addr++;

	while (*orig) {
		while (*orig && (*orig == ' ' /* || *orig == '"' */ || *orig == ','))
			orig++;
		*addr++ = ' ';
		while (*orig && (*orig != ' ' && *orig != ',' /* && *orig != '"' */ ))
			*addr++ = *orig++;
		while (*orig && (*orig == ' ' /* || *orig == '"' */ || *orig == ','))
			orig++;
		if (*orig == '(') {
			orig++;
			open_parens = 1;
			while (*orig && open_parens) {
				if (*orig == '(')
					open_parens++;
				if (*orig == ')')
					open_parens--;
				orig++;
			}
			if (*orig == ')')
				orig++;
		}
	}
	*addr = '\0';
}


#if defined(EVIL_INSIDE) && !defined(INDEX_DAEMON)
/*
 * build_messageid()
 * returns *(<Message-ID>)
 */
static const char *
build_messageid (
	void)
{
	int i = 0;
	static char buf[1024]; /* Message-IDs are limited to 998-12+CRLF octets*/
	static unsigned long int seqnum = 0; /* we'd use a counter in tinrc */
#	ifndef FORGERY
	/*
	 * Message ID format as suggested in
	 * draft-ietf-usefor-msg-id-alt-00, 2.1.3
	 * based on login name and FQDN
	 */
	static char buf2[1024];

	strip_name(build_sender(), buf2);
	/* we'd use snprintf here */
	sprintf(buf, "<%lxt%lxi%xn%lx%%%s>", seqnum++, time(0), process_id, (unsigned long int) real_uid, buf2);
#	else
	/*
	 * Message ID format as suggested in
	 * draft-ietf-usefor-msg-id-alt-00, 2.1.1
	 * based on the host's FQDN
	 */
	sprintf(buf, "<%lxt%xi%xn%x@%s>", seqnum++, time(0), getpid(), getuid(), get_fqdn(get_host_name()));
#	endif /* !FORGERY */

	i = gnksa_check_from (buf);
	if ((GNKSA_OK != i) && (GNKSA_LOCALPART_MISSING > i))
		buf[0] = '\0';
	return buf;
}
#endif /* EVIL_INSIDE && !INDEX_DAEMON */


/* TODO: move to canlock.c */
#if defined(USE_CANLOCK) && !defined(INDEX_DAEMON)
/*
 * build_canlock(messageid, secret)
 * returns *(cancel-lock) or NULL
 */
const char *
build_canlock (
	const char * messageid,
	const char * secret)
{
	if ((messageid == (const char *) 0) || (secret == (const char *) 0))
		return ((const char *) 0);
	else
		/*
		 * sha_lock should be
		 * const char * sha_lock (const char *, size_t, const char *, size_t)
		 * but unfortunately is
		 * unsigned char * sha_lock (char *, size_t, char *, size_t)
		 * -> cast as cast can
		 */
		return (const char *) (sha_lock((char *) secret, strlen(secret), (char *) messageid, strlen(messageid)));
}


/*
 * build_cankey(messageid, secret)
 * returns *(cancel-key) or NULL
 */
const char *
build_cankey (
	const char * messageid,
	const char * secret)
{
	if ((messageid == (const char *) 0) || (secret == (const char *) 0))
		return ((const char *) 0);
	else
		/*
		 * sha_key should be
		 * const char * sha_key (const char *, size_t, const char *, size_t)
		 * but unfortunately is
		 * unsigned char * sha_key (char *, size_t, char *, size_t)
		 * -> cast as cast can
		 */
		return (const char *) (sha_key((char *) secret, strlen(secret), (char *) messageid, strlen(messageid)));
}


/*
 * get_secret()
 * returns *(secret) or NULL
 */
#	ifdef VMS
#		define SECRET_FILE "cancelsecret."
#	else
#		define SECRET_FILE ".cancelsecret"
#	endif /* VMS */
char *
get_secret (
	void)
{
	FILE *fp_secret;
	char *ptr;
	char path_secret[PATH_LEN];
	static char cancel_secret[HEADER_LEN];

	cancel_secret[0] = '\0';
	joinpath (path_secret, homedir, SECRET_FILE);
	if ((fp_secret = fopen(path_secret, "r")) == (FILE *) 0) {
		my_fprintf (stderr, txt_cannot_open, path_secret);
		my_fflush (stderr);
		sleep(2);
		/* TODO: prompt for secret manually here? */
		return ((char *) 0);
	} else {
		(void) fread(cancel_secret, HEADER_LEN, 1, fp_secret);
		fclose(fp_secret);
	}

	cancel_secret[HEADER_LEN - 1] = '\0';

	if ((ptr = strchr (cancel_secret, '\n')))
		*ptr='\0';

	return cancel_secret;
}
#endif /* USE_CANLOCK && !INDEX_DAEMON */
