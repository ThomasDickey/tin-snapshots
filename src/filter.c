/*
 *  Project   : tin - a Usenet reader
 *  Module    : filter.c
 *  Author    : I. Lea
 *  Created   : 1992-12-28
 *  Updated   : 2003-01-31
 *  Notes     : Filter articles. Kill & auto selection are supported.
 *  Copyright : (c) Copyright 1991-99 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 *
 *              update for scoring (c) 1997 by Oliver B. Warzecha
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

#define IS_READ(i)	(arts[i].status == ART_READ)
#define IS_KILLED(i)	(arts[i].killed)

/*
 * SET_FILTER in group grp, current article arts[i], with rule ptr[j]
 *
 * filtering is now done this way:
 * a. set score for all articles and rules
 * b. check each article if the score is above or below the limit
 */

#define	SET_FILTER(grp, i, j)	\
	if (ptr[j].score > 0) { \
		arts[i].score = (SCORE_MAX - ptr[j].score >= arts[i].score) ? \
		(arts[i].score + ptr[j].score) : SCORE_MAX ; \
	} else { \
		arts[i].score = (-SCORE_MAX - ptr[j].score <= arts[i].score) ? \
		(arts[i].score + ptr[j].score) : -SCORE_MAX ; }

/*
 * SET_FILTER is now somewhat shorter, as the real filtering is done
 * at the end of filter_articles()
 */

/*
 * global filter array
 */
struct t_filters glob_filter = { 0, 0, (struct t_filter *) 0 };

#ifndef INDEX_DAEMON
	static int *arr_max;
	static int *arr_num;
	static struct t_filter *arr_ptr;
#endif /* !INDEX_DAEMON */

/*
 * Local prototypes
 */
static int get_choice (int x, const char *help, const char *prompt, const char *opt1, const char *opt2, const char *opt3, const char *opt4, const char *opt5);
static int unfilter_articles (void);
static int set_filter_scope (struct t_group *group);
static struct t_filter * psExpandFilterArray (struct t_filter *ptr, int *num);
static t_bool bAddFilterRule (struct t_group *psGrp, struct t_article *psArt, struct t_filter_rule *psRule);
static void free_filter_array (struct t_filters *ptr);
static void free_filter_item (struct t_filter *ptr);
#ifndef INDEX_DAEMON
	static void vSetFilter (struct t_filter *psFilter);
	static void vWriteFilterArray (FILE *fp, struct t_filters *ptr, time_t theTime);
	static void vWriteFilterFile (char *pcFile);
#endif /* !INDEX_DAEMON */


static struct t_filter *
psExpandFilterArray (
	struct t_filter *ptr,
	int *num)
{
	size_t block;
	struct t_filter *result;

	(*num)++;

	block = *num * sizeof (struct t_filter);

	if (*num == 1)	/* allocate */
		result = (struct t_filter *) my_malloc (block);
	else	/* reallocate */
		result = (struct t_filter *) my_realloc ((char *) ptr, block);

	return result;
}


/*
 * Looks for a matching filter hit (wildmat or pcre regex) in the supplied string
 * If the cache is not yet initialised, compile and optimise the regex
 * Return TRUE if we hit the rule
 */
static t_bool
test_regex(
	const char *string,
	char *regex,
	t_bool nocase,
	struct regex_cache *cache)
{
	const char *regex_errmsg = 0;
	int regex_errpos;

	if (!tinrc.wildcard) {
		if (wildmat (string, regex, nocase))
			return TRUE;
	} else {
		if (!cache->re) {
			if ((cache->re = pcre_compile(regex, PCRE_EXTENDED | ((nocase) ? PCRE_CASELESS : 0),
												&regex_errmsg, &regex_errpos, NULL)) == NULL)
				sprintf (mesg, txt_pcre_error_at, regex_errmsg, regex_errpos);
			else {
				cache->extra = pcre_study(cache->re, 0, &regex_errmsg);
				if (regex_errmsg != NULL)
					sprintf (mesg, txt_pcre_error_text, regex_errmsg);
			}
		}
		if (cache->re) {
			regex_errpos = pcre_exec(cache->re, cache->extra, string, strlen(string), 0, 0, NULL, 0);
			if (regex_errpos >= 0)
				return TRUE;
			else if (regex_errpos != PCRE_ERROR_NOMATCH)
				sprintf (mesg, txt_pcre_error_num, regex_errpos);
		}
	}

	return FALSE;
}


/*
 * vSetFilter() initialises a struct t_filter with default values
 */
#ifndef INDEX_DAEMON
static void
vSetFilter (
	struct t_filter *psFilter)
{
	if (psFilter != (struct t_filter *) 0) {
		psFilter->type = FILTER_SELECT;
		psFilter->scope = (char *) 0;
		psFilter->inscope = TRUE;
		psFilter->icase = FALSE;
		psFilter->fullref = FILTER_MSGID;
		psFilter->subj = (char *) 0;
		psFilter->from = (char *) 0;
		psFilter->msgid = (char *) 0;
		psFilter->lines_cmp = FILTER_LINES_NO;
		psFilter->lines_num = 0;
		psFilter->gnksa_cmp = FILTER_LINES_NO;
		psFilter->gnksa_num = 0;
		psFilter->score = 0;
		psFilter->xref = (char *) 0;
		psFilter->xref_max = 0;
		psFilter->xref_score_cnt = 0;
		psFilter->time = (time_t) 0;
		psFilter->next = (struct t_filter *) 0;
	}
}
#endif /* !INDEX_DAEMON */


/*
 * free_filter_item() frees all filter data (char *)
 */
static void
free_filter_item (
	struct t_filter *ptr)
{
	FreeAndNull(ptr->scope);
	FreeAndNull(ptr->subj);
	FreeAndNull(ptr->from);
	FreeAndNull(ptr->msgid);
	FreeAndNull(ptr->xref);
}


/*
 * free_filter_array() frees t_filter structs t_filters contains pointers to
 */
static void
free_filter_array (
	struct t_filters *ptr)
{
	register int i;

	if (ptr != (struct t_filters *) 0) {
		for (i = 0; i < ptr->num; i++)
			free_filter_item(ptr->filter + i);

		if (ptr->filter != (struct t_filter *) 0) {
			free ((char *) ptr->filter);
			ptr->filter = (struct t_filter *) 0;
		}

		ptr->num = 0;
		ptr->max = 0;
	}
}


void
free_all_filter_arrays (void) /* FIXME: use free_filter_array() instead */
{
	/*
	 * Global filter array
	 */
	free_filter_array (&glob_filter);
}

/*
 *  read ~/.tin/filter file contents into filter array
 */

#ifndef INDEX_DAEMON
t_bool
read_filter_file (
	char *file)
{
	FILE *fp;
	char *s;
	char buf[HEADER_LEN];
	char scope[HEADER_LEN];
	char subj[HEADER_LEN];
	char from[HEADER_LEN];
	char msgid[HEADER_LEN];
	char lines[HEADER_LEN];
	char gnksa[HEADER_LEN];
	char xref[HEADER_LEN];
	char xref_score[HEADER_LEN];
	char scbuf[PATH_LEN];
	int i = 0;
	int icase = 0, type = -1;
	int score = 0;
	int xref_max = 0;
	int xref_score_cnt = 0;
	int xref_score_value = 0;
	long secs = 0L;
	struct t_group *psGrp;
	t_bool expired = FALSE;
	t_bool expired_time = FALSE;
	t_bool global = TRUE;
	time_t current_secs = (time_t) 0;

	if ((fp = fopen (file, "r")) == (FILE *) 0)
		return FALSE;

	if (INTERACTIVE)
		wait_message (0, txt_reading_filter_file);

	(void) time (&current_secs);

	/*
	 * Reset all filter arrays if doing a reread of the active file
	 */
	free_all_filter_arrays ();

	psGrp = (struct t_group *) 0;
	arr_ptr = (struct t_filter *) 0;

	while (fgets (buf, (int) sizeof (buf), fp) != (char *) 0) {
		if (*buf == '#' || *buf == '\n')
			continue;

		switch(tolower((unsigned char)buf[0])) {
		case 'c':
			if (match_integer (buf+1, "ase=", &icase, 1)) {
				if (arr_ptr && !expired_time)
					arr_ptr[i].icase = (unsigned) icase;

				break;
			}
			break;
		case 'f':
			if (match_string (buf+1, "rom=", from, sizeof (from))) {
				if (arr_ptr && !expired_time)
					arr_ptr[i].from = my_strdup (from);

				break;
			}
			break;
/*
 * TODO: This is a quick hack for now, the "group" code is just a copy
 *       of the "scope" code.
 */
		case 'g':
			if (match_string (buf+1, "roup=", scope, sizeof (scope))) {
#ifdef DEBUG
if (debug) {
	my_printf ("scope=[%s] num=[%d]\n", scope, glob_filter.num);
	my_flush ();
}
#endif /* DEBUG */
				arr_num = &glob_filter.num;
				arr_max = &glob_filter.max;
				if (*arr_num >= (*arr_max - 1))
					glob_filter.filter = psExpandFilterArray (glob_filter.filter, arr_max);
				arr_ptr = glob_filter.filter;
				i = *arr_num;
				(*arr_num)++;
				vSetFilter (&arr_ptr[i]);
				expired_time = FALSE;
				arr_ptr[i].scope = my_strdup (scope);
				subj[0] = '\0';
				from[0] = '\0';
				msgid[0] = '\0';
				lines[0] = '\0';
				xref[0] = '\0';
				icase = 0;
				secs = 0L;
				psGrp = (struct t_group *) 0;		/* fudge for out of order rules */
				break;
			}
			if (match_string (buf+1, "nksa=", gnksa, sizeof (gnksa))) {
				if (arr_ptr && !expired_time) {
					if (gnksa[0] == '<') {
						arr_ptr[i].gnksa_cmp = FILTER_LINES_LT;
						arr_ptr[i].gnksa_num = atoi (&gnksa[1]);
					} else if (gnksa[0] == '>') {
						arr_ptr[i].gnksa_cmp = FILTER_LINES_GT;
						arr_ptr[i].gnksa_num = atoi (&gnksa[1]);
					} else {
						arr_ptr[i].gnksa_cmp = FILTER_LINES_EQ;
						arr_ptr[i].gnksa_num = atoi (gnksa);
					}
				}
				break;
			}
		break;

		case 'l':
			if (match_string (buf+1, "ines=", lines, sizeof (lines))) {
				if (arr_ptr && !expired_time) {
					if (lines[0] == '<') {
						arr_ptr[i].lines_cmp = FILTER_LINES_LT;
						arr_ptr[i].lines_num = atoi (&lines[1]);
					} else if (lines[0] == '>') {
						arr_ptr[i].lines_cmp = FILTER_LINES_GT;
						arr_ptr[i].lines_num = atoi (&lines[1]);
					} else {
						arr_ptr[i].lines_cmp = FILTER_LINES_EQ;
						arr_ptr[i].lines_num = atoi (lines);
					}
				}
				break;
			}
			break;

		case 'm':
			if (match_string (buf+1, "sgid=", msgid, sizeof (msgid))) {
				if (arr_ptr) {
					arr_ptr[i].msgid = my_strdup (msgid);
					arr_ptr[i].fullref = FILTER_MSGID;
				}
				break;
			}
			if (match_string (buf+1, "sgid_last=", msgid, sizeof (msgid))) {
				if (arr_ptr) {
					arr_ptr[i].msgid = my_strdup (msgid);
					arr_ptr[i].fullref = FILTER_MSGID_LAST;
				}
				break;
			}
			if (match_string (buf+1, "sgid_only=", msgid, sizeof (msgid))) {
				if (arr_ptr) {
					arr_ptr[i].msgid = my_strdup (msgid);
					arr_ptr[i].fullref = FILTER_MSGID_ONLY;
				}
				break;
			}
			break;

		case 'r':
			if (match_string (buf+1, "efs_only=", msgid, sizeof (msgid))) {
				if (arr_ptr) {
					arr_ptr[i].msgid = my_strdup (msgid);
					arr_ptr[i].fullref = FILTER_REFS_ONLY;
				}
				break;
			}
			break;

		case 's':
			if (match_string (buf+1, "cope=", scope, sizeof (scope))) {
#ifdef DEBUG
if (debug) {
	my_printf ("scope=[%s] num=[%d]\n", scope, glob_filter.num);
	my_flush ();
}
#endif /* DEBUG */
				global = TRUE;
				arr_num = &glob_filter.num;
				arr_max = &glob_filter.max;
				if (*arr_num >= (*arr_max - 1))
					glob_filter.filter = psExpandFilterArray (glob_filter.filter, arr_max);
				arr_ptr = glob_filter.filter;
				i = *arr_num;
				(*arr_num)++;
				vSetFilter (&arr_ptr[i]);
				expired_time = FALSE;
				arr_ptr[i].scope = my_strdup (scope);
				subj[0] = '\0';
				from[0] = '\0';
				msgid[0] = '\0';
				lines[0] = '\0';
				xref[0] = '\0';
				icase = 0;
				secs = 0L;
				psGrp = (struct t_group *) 0;	/* fudge for out of order rules */
				break;
			}
			if (match_string (buf+1, "ubj=", subj, sizeof (subj))) {
				if (arr_ptr && !expired_time)
					arr_ptr[i].subj = my_strdup (subj);

#ifdef DEBUG
if (debug) {
	if (global) {
		my_printf ("6. buf=[%s]  Gsubj=[%s]\n", arr_ptr[i].subj, glob_filter.filter[i].subj);
		my_flush ();
	}
} /* FIXME: Only global filters now */
#endif /* DEBUG */
				break;
			}

			/*
			 * read score for rule
			 */
			if (match_string (buf+1, "core=", scbuf, PATH_LEN)) {
				score = atoi(scbuf);
#ifdef DEBUG
if (debug) {
	my_printf ("score=[%d]\n", score);
	my_flush();
}
#endif /* DEBUG */
				if (arr_ptr && !expired_time) {
					if (score > SCORE_MAX)
						score = SCORE_MAX;
					else if (score < -SCORE_MAX)
						score = -SCORE_MAX;
					else if (!score) {
						if (!strncmp(scbuf, "kill", 4))
							score = SCORE_KILL;
						else
							score = (!strncmp(scbuf, "hot", 3) ? SCORE_SELECT : SCORE_DEFAULT);
					}
					if ((arr_ptr[i].type == FILTER_KILL && score > 0) || (arr_ptr[i].type == FILTER_SELECT && score < 0))
						score = -score;
					arr_ptr[i].score = score;
				}
				break;
			}
			break;

		case 't':
			if (match_integer (buf+1, "ype=", &type, 1)) {
#ifdef DEBUG
if (debug) {
	my_printf ("type=[%d][%s]\n", type, (!type ? "KILL" : "SELECT"));
	my_flush ();
}
#endif /* DEBUG */
				if (arr_ptr) {
					arr_ptr[i].type = (unsigned) type;
					arr_ptr[i].score = (!type ? SCORE_KILL : SCORE_SELECT);
				}
				break;
			}
			if (match_long (buf+1, "ime=", &secs)) {
				if (arr_ptr && !expired_time) {
					arr_ptr[i].time = (time_t) secs;
					if (secs && current_secs > (time_t) secs) {
#ifdef DEBUG
if (debug) {
	my_printf ("EXPIRED  secs=[%lu]  current_secs=[%lu]\n", (unsigned long int) secs, (unsigned long int) current_secs);
	my_flush ();
}
#endif /* DEBUG */
						(*arr_num)--;
						expired_time = TRUE;
						expired = TRUE;
					}
				}
				break;
			}
			break;

		case 'x':
			if (match_string (buf+1, "ref=", xref, sizeof (xref))) {
				if (arr_ptr && ! expired_time)
					arr_ptr[i].xref = my_strdup (xref);

				break;
			}
			if (match_integer (buf+1, "ref_max=", &xref_max, 1000)) {
				if (arr_ptr && ! expired_time)
					arr_ptr[i].xref_max = xref_max;

				break;
			}
			if (match_string (buf+1, "ref_score=", xref_score, sizeof(xref_score))) {
				if (arr_ptr && !expired_time) {
					if (xref_score_cnt < 10) {
						if (isdigit((int)xref_score[0])) {
							xref_score_value = atoi(xref_score);
							if ((s = strchr(xref_score, ',')))
								s++;
							arr_ptr[i].xref_scores[xref_score_cnt] = xref_score_value;
							arr_ptr[i].xref_score_strings[xref_score_cnt] = (s != 0 ? my_strdup(s) : 0);
							arr_ptr[i].xref_score_cnt++;
							xref_score_cnt++;
						}
					}
				}
				break;
			}
			break;

		default:
			break;
		}
	}
	fclose (fp);

	if (expired)
		vWriteFilterFile (file);

	if (cmd_line)
		printf ("\r\n");

	if (INTERACTIVE)
		clear_message();

	return TRUE;
}

/*
 * write filter strings to ~/.tin/filter
 */

static void
vWriteFilterFile (
	char *pcFile)
{
	FILE *hFp;

	if (no_write)
		return;

	if ((hFp = fopen (pcFile, "w")) == (FILE *) 0)
		return;

	fprintf (hFp, txt_filter_file, tinrc.filter_days);
	fflush (hFp);

	/*
	 * Save global filters
	 */
	vWriteFilterArray (hFp, &glob_filter, time(NULL));

	fclose (hFp);
	chmod (pcFile, (mode_t)(S_IRUSR|S_IWUSR));
}
#endif/* !INDEX_DAEMON */

#ifndef INDEX_DAEMON
static void
vWriteFilterArray (
	FILE *fp,
	struct t_filters *ptr,
	time_t theTime)
{
	register int i;
	int j;

	if (ptr == (struct t_filters *) 0)
		return;

	for (i = 0; i < ptr->num; i++) {

/* my_printf ("WRITE i=[%d] subj=[%s] from=[%s]\n", i, (ptr->filter[i].subj ? ptr->filter[i].subj : ""), (ptr->filter[i].from ? ptr->filter[i].from : "")); */

		if (theTime && ptr->filter[i].time) {
			if (theTime > ptr->filter[i].time)
				continue;
		}
/*
my_printf ("Scope=[%s]" cCRLF, (ptr->filter[i].scope != (char *) 0 ? ptr->filter[i].scope : "*"));
my_flush ();
*/
		fprintf (fp, "group=%s\n", (ptr->filter[i].scope != (char *) 0 ? ptr->filter[i].scope : "*"));

/*
my_printf ("PtrType=[%d] FilType=[%d]" cCRLF, ptr->filter[i].type, write_filter_type);
my_flush ();
*/
		fprintf (fp, "type=%u\n", ptr->filter[i].type);
		fprintf (fp, "case=%u\n", ptr->filter[i].icase);
		switch (ptr->filter[i].score) {
			case SCORE_KILL:
				fprintf (fp, "score=kill\n");
				break;
			case SCORE_SELECT:
				fprintf (fp, "score=hot\n");
				break;
			default:
				fprintf (fp, "score=%d\n", ptr->filter[i].score);
				break;
		}
		if (ptr->filter[i].subj != (char *) 0)
			fprintf (fp, "subj=%s\n", ptr->filter[i].subj);

		if (ptr->filter[i].from != (char *) 0)
			fprintf (fp, "from=%s\n", ptr->filter[i].from);

		if (ptr->filter[i].msgid != (char *) 0) {
			switch (ptr->filter[i].fullref) {
				case FILTER_MSGID:
					fprintf (fp, "msgid=%s\n", ptr->filter[i].msgid);
					break;
				case FILTER_MSGID_LAST:
					fprintf (fp, "msgid_last=%s\n", ptr->filter[i].msgid);
					break;
				case FILTER_MSGID_ONLY:
					fprintf (fp, "msgid_only=%s\n", ptr->filter[i].msgid);
					break;
				case FILTER_REFS_ONLY:
					fprintf (fp, "refs_only=%s\n", ptr->filter[i].msgid);
					break;
				default:
					break;
			}
		}

		if (ptr->filter[i].lines_cmp != FILTER_LINES_NO) {
			switch (ptr->filter[i].lines_cmp) {
				case FILTER_LINES_EQ:
					fprintf (fp, "lines=%d\n", ptr->filter[i].lines_num);
					break;
				case FILTER_LINES_LT:
					fprintf (fp, "lines=<%d\n", ptr->filter[i].lines_num);
					break;
				case FILTER_LINES_GT:
					fprintf (fp, "lines=>%d\n", ptr->filter[i].lines_num);
					break;
				default:
					break;
			}
		}

		if (ptr->filter[i].gnksa_cmp != FILTER_LINES_NO) {
			switch (ptr->filter[i].gnksa_cmp) {
				case FILTER_LINES_EQ:
					fprintf (fp, "gnksa=%d\n", ptr->filter[i].gnksa_num);
					break;
				case FILTER_LINES_LT:
					fprintf (fp, "gnksa=<%d\n", ptr->filter[i].gnksa_num);
					break;
				case FILTER_LINES_GT:
					fprintf (fp, "gnksa=>%d\n", ptr->filter[i].gnksa_num);
					break;
				default:
					break;
			}
		}

		if (ptr->filter[i].xref != NULL)
			fprintf (fp, "xref=%s\n", ptr->filter[i].xref);

		if (ptr->filter[i].xref_max > 0) {
			fprintf (fp, "xref_max=%d\n", ptr->filter[i].xref_max);

			for(j=0;j<ptr->filter[i].xref_score_cnt;j++)
				fprintf (fp, "xref_score=%d%s%s\n", ptr->filter[i].xref_scores[j], ptr->filter[i].xref_score_strings[j] ? "," : "", ptr->filter[i].xref_score_strings[j]);
		}
		if (ptr->filter[i].time)
			fprintf (fp, "time=%lu\n", (unsigned long int) ptr->filter[i].time);

		fprintf (fp, "#####\n"); /* makes filter file more readable */
	}

	fflush (fp);
}
#endif /* !INDEX_DAEMON */


static int
get_choice (
	int x,
	const char *help,
	const char *prompt,
	const char *opt1,
	const char *opt2,
	const char *opt3,
	const char *opt4,
	const char *opt5)
{
	const char *argv[5];
	int ch, n = 0, i = 0;

	if (opt1)
		argv[n++] = opt1;
	if (opt2)
		argv[n++] = opt2;
	if (opt3)
		argv[n++] = opt3;
	if (opt4)
		argv[n++] = opt4;
	if (opt5)
		argv[n++] = opt5;
	assert(n > 0);

	if (help)
		show_menu_help (help);

	do {
		MoveCursor(x, (int) strlen (prompt));
		my_fputs (argv[i], stdout);
		my_flush ();
		CleartoEOLN ();
		if ((ch = ReadCh ()) != ' ')
			continue;
		if (++i == n)
			i = 0;
	} while (ch != '\n' && ch != '\r' && ch != ESC);

	if (ch == ESC)
		return (-1);

	return (i);
}

/*
 * Interactive filter menu so that the user can dynamically enter parameters.
 * Can be configured for kill or auto-selection screens.
 */

t_bool
filter_menu (
	int type,
	struct t_group *group,
	struct t_article *art)
{
	const char *ptr_filter_from;
	const char *ptr_filter_lines;
	const char *ptr_filter_menu;
	const char *ptr_filter_msgid;
	const char *ptr_filter_scope;
	const char *ptr_filter_subj;
	const char *ptr_filter_text;
	const char *ptr_filter_time;
	const char *ptr_filter_help_scope;
	const char *ptr_filter_quit_edit_save;
	const char *ptr_filter_score;
	char *ptr;
	char argv[4][PATH_LEN];
	char buf[LEN];
	char text_from[PATH_LEN];
	char text_msgid[PATH_LEN];
	char text_subj[PATH_LEN];
	char text_time[PATH_LEN];
	char double_time[PATH_LEN];
	char quat_time[PATH_LEN];
	char ch_default = iKeyFilterSave;
	int ch, i, len;
	struct t_filter_rule rule;

	rule.text[0] = '\0';
	rule.scope[0] = '\0';
	rule.counter = 0;
	rule.lines_cmp = FILTER_LINES_NO;
	rule.lines_num = 0;
	rule.from_ok = FALSE;
	rule.lines_ok = FALSE;
	rule.msgid_ok = FALSE;
	rule.fullref = FILTER_MSGID;
	rule.subj_ok = FALSE;
	rule.icase = FALSE;
	rule.type = type;
	rule.score = 0;
	rule.expire_time = FALSE;
	rule.check_string = FALSE;

	/*
	 * setup correct text for user selected menu
	 */

	if (type == FILTER_KILL) {
		ptr_filter_from = txt_kill_from;
		ptr_filter_lines = txt_kill_lines;
		ptr_filter_menu = txt_kill_menu;
		ptr_filter_msgid = txt_kill_msgid;
		ptr_filter_scope = txt_kill_scope;
		ptr_filter_subj = txt_kill_subj;
		ptr_filter_text = txt_kill_text;
		ptr_filter_time = txt_kill_time;
		ptr_filter_help_scope = txt_help_kill_scope;
		ptr_filter_quit_edit_save = txt_quit_edit_save_kill;
	} else {
		ptr_filter_from = txt_select_from;
		ptr_filter_lines = txt_select_lines;
		ptr_filter_menu = txt_select_menu;
		ptr_filter_msgid = txt_select_msgid;
		ptr_filter_scope = txt_select_scope;
		ptr_filter_subj = txt_select_subj;
		ptr_filter_text = txt_select_text;
		ptr_filter_time = txt_select_time;
		ptr_filter_help_scope = txt_help_select_scope;
		ptr_filter_quit_edit_save = txt_quit_edit_save_select;
	}

	ptr_filter_score = txt_filter_score;

	len = cCOLS - 30;

	sprintf (text_time, txt_time_default_days, tinrc.filter_days);
	sprintf (text_subj, ptr_filter_subj, len, len, art->subject);

	strcpy (buf, art->from);

	sprintf (text_from, ptr_filter_from, len, len, buf);
	sprintf (text_msgid, ptr_filter_msgid, len-4, len-4, MSGID(art));

	ClearScreen ();

	center_line (0, TRUE, ptr_filter_menu);

	MoveCursor (INDEX_TOP, 0);
	my_printf ("%s" cCRLF, ptr_filter_text);
	my_printf ("%s" cCRLF cCRLF, txt_filter_text_type);
	my_printf ("%s" cCRLF , text_subj);
	my_printf ("%s" cCRLF , text_from);
	my_printf ("%s" cCRLF cCRLF, text_msgid);
	my_printf ("%s" cCRLF , ptr_filter_lines);
	my_printf ("%s" cCRLF , ptr_filter_score);
	my_printf ("%s" cCRLF cCRLF, ptr_filter_time);
	my_printf ("%s%s", ptr_filter_scope, group->name);
	my_flush ();

	show_menu_help (txt_help_filter_text);

	if (!prompt_menu_string (INDEX_TOP, (int) strlen (ptr_filter_text), rule.text))
		return FALSE;

	if (*rule.text) {
		i = get_choice (INDEX_TOP+1, txt_help_filter_text_type,
			       txt_filter_text_type,
			       txt_subj_line_only_case,
			       txt_subj_line_only,
			       txt_from_line_only_case,
			       txt_from_line_only,
			       txt_msgid_line_only);
		if (i == -1)
			return FALSE;

		rule.counter = i;
		switch (i) {
			case FILTER_SUBJ_CASE_IGNORE:
			case FILTER_FROM_CASE_IGNORE:
				rule.icase = TRUE;
				break;

			case FILTER_SUBJ_CASE_SENSITIVE:
			case FILTER_FROM_CASE_SENSITIVE:
			case FILTER_MSGID:
			case FILTER_MSGID_LAST:
			case FILTER_MSGID_ONLY:
				/* rule.icase is FALSE already, no assignment necessary */
				break;

			default: /* should not happen */
				/* CONSTANTCONDITION */
				assert(0 != 0);
				break;
		}
	}

	if (!*rule.text) {
		rule.check_string = TRUE;
		/*
		 * Subject:
		 */
		i = get_choice (INDEX_TOP+3, txt_help_filter_subj, text_subj, txt_yes, txt_no, (char *)0, (char *)0, (char *)0);

		if (i == -1)
			return FALSE;
		else
			rule.subj_ok = (i == 0);

		/*
		 * From:
		 */
		i = get_choice (INDEX_TOP+4, txt_help_filter_from, text_from, (rule.subj_ok ? txt_no : txt_yes), (rule.subj_ok ? txt_yes : txt_no), (char *)0, (char *)0, (char *)0);

		if (i == -1)
			return FALSE;
		else
			rule.from_ok = rule.subj_ok ? (i != 0) : (i == 0);

		/*
		 * Message-Id:
		 */
		if (rule.subj_ok || rule.from_ok)
			i = get_choice (INDEX_TOP+5, txt_help_filter_msgid, text_msgid, txt_no, txt_full, txt_last, txt_only, (char *)0);
		else
			i = get_choice (INDEX_TOP+5, txt_help_filter_msgid, text_msgid, txt_full, txt_last, txt_only, txt_no, (char *)0);

		if (i == -1)
			return FALSE;
		else {
			switch ((rule.subj_ok || rule.from_ok) ? i : i+1) {
				case 0:
				case 4:
					rule.msgid_ok = FALSE;
					rule.fullref  = FILTER_MSGID;
					break;

				case 1:
					rule.msgid_ok = TRUE;
					rule.fullref  = FILTER_MSGID;
					break;

				case 2:
					rule.msgid_ok = TRUE;
					rule.fullref  = FILTER_MSGID_LAST;
					break;

				case 3:
					rule.msgid_ok = TRUE;
					rule.fullref  = FILTER_MSGID_ONLY;
					break;

				default: /* should not happen */
					/* CONSTANTCONDITION */
					assert(0 != 0);
					break;
			}
		}

	}

	/*
	 * Lines:
	 */
	show_menu_help (txt_help_filter_lines);

	buf[0] = '\0';

	if (!prompt_menu_string (INDEX_TOP+7, (int) strlen (ptr_filter_lines), buf))
		return FALSE;

	/*
	 * Get the < > sign if any for the lines rule
	 */
	ptr = buf;
	while (ptr && *ptr == ' ')
		ptr++;

	if (ptr && *ptr == '>') {
		rule.lines_cmp = FILTER_LINES_GT;
		ptr++;
	} else if (ptr && *ptr == '<') {
		rule.lines_cmp = FILTER_LINES_LT;
		ptr++;
	} else if (ptr && *ptr == '=') {
		rule.lines_cmp = FILTER_LINES_EQ;
		ptr++;
	}
	rule.lines_num = atoi (ptr);

	if (rule.lines_cmp != FILTER_LINES_NO && rule.lines_num >= 0)
		rule.lines_ok = TRUE;

	/*
	 * Scoring value
	 */
	buf[0] = '\0';
	show_menu_help(txt_filter_score_help); /* FIXME: a sprintf() is necessary here */

	if (!prompt_menu_string(INDEX_TOP+8, (int)strlen(ptr_filter_score), buf))
		return FALSE;

	rule.score = atoi(buf);
	/*
	 * assure we are in range
	 */
	if (rule.score <= 0)
		rule.score = SCORE_DEFAULT;
	else if (rule.score > SCORE_MAX)
		rule.score = SCORE_MAX;

	if (type == FILTER_KILL)
		rule.score = -rule.score;

	/*
	 * Expire time
	 */
	sprintf (double_time, "2x %s", text_time);
	sprintf (quat_time, "4x %s", text_time);
	i = get_choice (INDEX_TOP+9, txt_help_filter_time, ptr_filter_time, txt_unlimited_time, text_time, double_time, quat_time, (char *)0);

	if (i == -1)
		return FALSE;

	rule.expire_time = i;

	/*
	 * Scope
	 */
	if (*rule.text || rule.subj_ok || rule.from_ok || rule.msgid_ok || rule.lines_ok) {
		strcpy (argv[0], group->name);
		strcpy (argv[1], txt_all_groups);
		strcpy (argv[2], group->name);
		ptr = strrchr (argv[2], '.');
		if (ptr != (char *) 0) {
			ptr++;
			*(ptr++) = '*';
			*ptr = '\0';
			strcpy (argv[3], argv[2]);
			argv[3][strlen(argv[3])-2] = '\0';
			ptr = strrchr (argv[3], '.');
			if (ptr != (char *) 0) {
				ptr++;
				*(ptr++) = '*';
				*ptr = '\0';
			} else
				argv[3][0] = '\0';

		} else
			argv[2][0] = '\0';

		i = get_choice (INDEX_TOP+11, ptr_filter_help_scope,
			       ptr_filter_scope,
			       (argv[0][0] ? argv[0] : (char *)0),
			       (argv[1][0] ? argv[1] : (char *)0),
			       (argv[2][0] ? argv[2] : (char *)0),
			       (argv[3][0] ? argv[3] : (char *)0),
			       (char *)0);

		if (i == -1)
			return FALSE;

		strcpy (rule.scope, ((i == 1) ? "*" : argv[i]));
	} else
		return FALSE;

	forever {
		ch = prompt_slk_response(ch_default, "eqs\033", "%s", ptr_filter_quit_edit_save);
		switch (ch) {

		case iKeyFilterEdit:
			bAddFilterRule (group, art, &rule); /* save the rule */
			if (!invoke_editor (filter_file, 25)) /* FIXME: is 25 correct offset? */
				return FALSE;
			unfilter_articles ();
#ifndef INDEX_DAEMON
			(void) read_filter_file (filter_file);
#endif /* !INDEX_DAEMON */
			return TRUE;
			/* keep lint quiet: */
			/* FALLTHROUGH */

		case iKeyQuit:
		case iKeyAbort:
			return FALSE;
			/* keep lint quiet: */
			/* FALLTHROUGH */

		case iKeyFilterSave:
			/*
			 * Add the filter rule and save it to the filter file
			 */
			return (bAddFilterRule (group, art, &rule));
			/* keep lint quiet: */
			/* FALLTHROUGH */

		default:
			break;
		}
	}
	/* NOTREACHED */
	return FALSE;
}


/*
 * Quick command to add an auto-select / kill filter to specified groups filter
 */
t_bool
quick_filter (
	int type,
	struct t_group *group,
	struct t_article *art)
{
	char *scope;
	int header, expire, icase;
	struct t_filter_rule rule;

	if (type == FILTER_KILL) {
		header = group->attribute->quick_kill_header;
		expire = group->attribute->quick_kill_expire;
		icase = group->attribute->quick_kill_case;
		scope = group->attribute->quick_kill_scope;
	} else {
		header = group->attribute->quick_select_header;
		expire = group->attribute->quick_select_expire;
		icase = group->attribute->quick_select_case;
		scope = group->attribute->quick_select_scope;
	}

#ifdef DEBUG
	if (debug)
		error_message ("%s header=[%d] scope=[%s] expire=[%s] case=[%d]",
			(type == FILTER_KILL) ? "KILL" : "SELECT",
			header, (scope ? scope : ""), (expire ? "ON" : "OFF"), icase);
#endif /* DEBUG */

	/*
	 * Setup rules
	 */
	rule.counter = 0;
	rule.scope[0] = '\0';
	if (scope)
		strcpy (rule.scope, scope);

	rule.lines_cmp = FILTER_LINES_NO;
	rule.lines_num = 0;
	rule.lines_ok = (header == FILTER_LINES);
	rule.msgid_ok = (header == FILTER_MSGID) || (header == FILTER_MSGID_LAST);
	rule.fullref = header; /* value is directly used to select correct filter type */
	rule.from_ok = (header == FILTER_FROM_CASE_SENSITIVE || header == FILTER_FROM_CASE_IGNORE);
	rule.subj_ok = (header == FILTER_SUBJ_CASE_SENSITIVE || header == FILTER_SUBJ_CASE_IGNORE);

	rule.text[0] = '\0';
	rule.type = type;
	rule.icase = icase;
	rule.expire_time = expire;
	rule.check_string = TRUE;
	rule.score = (type == FILTER_KILL) ? SCORE_KILL : SCORE_SELECT;

	return (bAddFilterRule (group, art, &rule));
}

/*
 * Quick command to add an auto-select filter to the article that user
 * has just posted. Selects on Subject: line with limited expire time.
 * Don't process if MAILGROUP.
 */
t_bool
quick_filter_select_posted_art (
	struct t_group *group,
	char *subj)	/* return value is always ignored */
{
	t_bool filtered = FALSE;

	if (group->type == GROUP_TYPE_NEWS) {
		struct t_article art;
		struct t_filter_rule rule;

#ifdef __cplusplus /* keep C++ quiet */
		rule.scope[0] = '\0';
#endif /* __cplusplus */

		if (strlen(group->name) > (sizeof(rule.scope) -1)) /* groupname to long? */
			return FALSE;

		/*
		 * Setup dummy article with posted articles subject
		 */
		set_article (&art);
		art.subject = my_strdup (subj);

		/*
		 * Setup rules
		 */
		rule.counter = 0;
		rule.lines_cmp = FILTER_LINES_NO;
		rule.lines_num = 0;
		rule.from_ok = FALSE;
		rule.lines_ok = FALSE;
		rule.msgid_ok = FALSE;
		rule.fullref = FILTER_MSGID;
		rule.subj_ok = TRUE;
		rule.text[0] = '\0';
		rule.type = FILTER_SELECT;
		rule.icase = FALSE;
		rule.expire_time = TRUE;
		rule.check_string = TRUE;
		rule.score = SCORE_SELECT;

		strcpy(rule.scope, group->name);

		filtered = bAddFilterRule (group, &art, &rule);

		FreeIfNeeded(art.subject);

	}

	return filtered;
}

/*
 * API to add filter rule to the local or global filter array
 */
static t_bool
bAddFilterRule (
	struct t_group *psGrp,
	struct t_article *psArt,
	struct t_filter_rule *psRule)
{
	char acBuf[PATH_LEN];
	char sbuf[(sizeof(acBuf)/2)]; /* half as big as acBuf so quote_wild(sbuf) fits into acBuf */
	int *plNum, *plMax;
	t_bool bFiltered = FALSE;
	time_t lCurTime;
	struct t_filter *psPtr;

	plNum = &glob_filter.num;
	plMax = &glob_filter.max;
	if (*plNum >= *plMax)
		glob_filter.filter = psExpandFilterArray (glob_filter.filter, plMax);

	psPtr = glob_filter.filter;

	psPtr[*plNum].type = psRule->type;
	psPtr[*plNum].icase = FALSE;
	psPtr[*plNum].inscope = TRUE;
	psPtr[*plNum].fullref = FILTER_MSGID;
	psPtr[*plNum].scope = (char *) 0;
	psPtr[*plNum].subj = (char *) 0;
	psPtr[*plNum].from = (char *) 0;
	psPtr[*plNum].msgid = (char *) 0;
	psPtr[*plNum].lines_cmp = psRule->lines_cmp;
	psPtr[*plNum].lines_num = psRule->lines_num;
	psPtr[*plNum].gnksa_cmp = FILTER_LINES_NO;
	psPtr[*plNum].gnksa_num = 0;
	psPtr[*plNum].score = psRule->score;
	psPtr[*plNum].xref = (char *) 0;
	psPtr[*plNum].xref_max = 0;
	psPtr[*plNum].xref_score_cnt = 0;

	if (psRule->scope[0] == '\0') /* replace empty scope with current group name */
		psPtr[*plNum].scope = my_strdup (psGrp->name);
	else {
		if ((psRule->scope[0] != '*') && (psRule->scope[1] != '\0')) /* copy non-global scope */
			psPtr[*plNum].scope = my_strdup (psRule->scope);
	}

	(void) time (&lCurTime);
	switch(psRule->expire_time)
	{
		case 1:
			psPtr[*plNum].time = lCurTime + (time_t) (tinrc.filter_days * 86400);	/*  86400 = 60 * 60 * 24 */
			break;
		case 2:
			psPtr[*plNum].time = lCurTime + (time_t) (tinrc.filter_days * 172800);	/* 172800 = 60 * 60 * 24 * 2 */
			break;
		case 3:
			psPtr[*plNum].time = lCurTime + (time_t) (tinrc.filter_days * 345600);	/* 345600 = 60 * 60 * 24 * 4 */
			break;
		default:
			psPtr[*plNum].time = (time_t) 0;
			break;
	}

	psPtr[*plNum].icase = psRule->icase;
	if (*psRule->text) {
		sprintf (acBuf, REGEX_FMT, quote_wild_whitespace(psRule->text));

		switch (psRule->counter) {
			case FILTER_SUBJ_CASE_IGNORE:
			case FILTER_SUBJ_CASE_SENSITIVE:
				psPtr[*plNum].subj = my_strdup (acBuf);
				break;

			case FILTER_FROM_CASE_IGNORE:
			case FILTER_FROM_CASE_SENSITIVE:
				psPtr[*plNum].from = my_strdup (acBuf);
				break;

			case FILTER_MSGID:
			case FILTER_MSGID_LAST:
			case FILTER_MSGID_ONLY:
			case FILTER_REFS_ONLY:
				psPtr[*plNum].msgid = my_strdup (acBuf);
				psPtr[*plNum].fullref = psRule->counter;
				break;

			default: /* should not happen */
				/* CONSTANTCONDITION */
				assert(0 != 0);
				break;
		}
		bFiltered = TRUE;
		(*plNum)++;
	} else {
		/*
		 * STRCPY() truncates subject/from/message-id so it fits
		 * into acBuf even after quote_wild()
		 */
		if (psRule->subj_ok) {
			STRCPY(sbuf, psArt->subject);
			sprintf (acBuf, REGEX_FMT, (psRule->check_string ? quote_wild (sbuf) : sbuf));
			psPtr[*plNum].subj = my_strdup (acBuf);
		}
		if (psRule->from_ok) {
			STRCPY(sbuf, psArt->from);
			sprintf (acBuf, REGEX_FMT, quote_wild (sbuf));
			psPtr[*plNum].from = my_strdup (acBuf);
		}
		/*
		 * message-ids should be quoted
		 */
		if (psRule->msgid_ok) {
			STRCPY(sbuf, MSGID(psArt));
			sprintf (acBuf, REGEX_FMT, quote_wild (sbuf));
			psPtr[*plNum].msgid = my_strdup (acBuf);
			psPtr[*plNum].fullref = psRule->fullref;
		}
		if (psRule->subj_ok || psRule->from_ok || psRule->msgid_ok || psRule->lines_ok) {
			bFiltered = TRUE;
			(*plNum)++;
		}
	}

	if (bFiltered) {
#ifdef DEBUG
		if (debug)
			wait_message (2, "inscope=[%s] scope=[%s] typ=[%d] case=[%d] subj=[%s] from=[%s] msgid=[%s] fullref=[%d] line=[%d %d] time=[%lu]",
				bool_unparse(psPtr[*plNum-1].inscope),
				(psRule->scope ? psRule->scope : ""),
				psPtr[*plNum-1].type, psPtr[*plNum-1].icase,
				(psPtr[*plNum-1].subj ? psPtr[*plNum-1].subj : ""),
				(psPtr[*plNum-1].from ? psPtr[*plNum-1].from : ""),
				(psPtr[*plNum-1].msgid ? psPtr[*plNum-1].msgid : ""),
				psPtr[*plNum-1].fullref, psPtr[*plNum-1].lines_cmp,
				psPtr[*plNum-1].lines_num, (unsigned long int) psPtr[*plNum-1].time);
#endif /* DEBUG */

#ifndef INDEX_DAEMON
		vWriteFilterFile (filter_file);
#endif /* !INDEX_DAEMON */
	}

	return bFiltered;
}


/*
 * We assume that any articles which are tagged as killed are also
 * tagged as being read BECAUSE they were killed. So, we retag
 * them as being unread.
 */
static int
unfilter_articles (void) /* return value is always ignored */
{
	int unkilled = 0;
	register int i;

	for (i = 0; i < top; i++) {
		if (IS_KILLED(i)) {
			arts[i].killed = FALSE;
			arts[i].status = ART_UNREAD;
			arts[i].score = 0;
			unkilled++;
		}
	}
	num_of_killed_arts = 0;

	return unkilled;
}


/*
 * Filter any articles in specified group.
 * Apply global filter rules followed by group filter rules.
 * In global rules check if scope field set to determine if
 * filter applys to current group.
 */
t_bool
filter_articles (
	struct t_group *group)
{
	char buf[LEN];
	int num, inscope;
/*	int score; */
	register int i, j, k;
	struct t_filter *ptr; /*, *curr; */
	struct regex_cache *regex_cache_subj = NULL;
	struct regex_cache *regex_cache_from = NULL;
	struct regex_cache *regex_cache_msgid = NULL;
	struct regex_cache *regex_cache_xref = NULL;
	t_bool filtered = FALSE;

	num_of_killed_arts = 0;
	num_of_selected_arts = 0;

	/*
	 * check if there are any global filter rules
	 */
	if (group->glob_filter->num == 0)
		return filtered;

	/*
	 * Apply global filter rules first if there are any entries
	 */
	/*
	 * Check if any scope rules are active for this group
	 * ie. group=comp.os.linux.help  scope=comp.os.linux.*
	 */
	inscope = set_filter_scope (group);
	if (!batch_mode /* !cmd_line */)
		wait_message (0, txt_filter_global_rules, inscope, group->glob_filter->num);
	num = group->glob_filter->num;
	ptr = group->glob_filter->filter;

	/*
	 * set up cache tables for all types of filter rules
	 * (only for regexp matching)
	 */
	if (tinrc.wildcard) {
		size_t msiz;

		msiz = sizeof(struct regex_cache) * num;
		regex_cache_subj = (struct regex_cache *) my_malloc(msiz);
		regex_cache_from = (struct regex_cache *) my_malloc(msiz);
		regex_cache_msgid = (struct regex_cache *) my_malloc(msiz);
		regex_cache_xref = (struct regex_cache *) my_malloc(msiz);
		for (j = 0; j < num; j++) {
			regex_cache_subj[j].re = NULL;
			regex_cache_subj[j].extra = NULL;
			regex_cache_from[j].re = NULL;
			regex_cache_from[j].extra = NULL;
			regex_cache_msgid[j].re = NULL;
			regex_cache_msgid[j].extra = NULL;
			regex_cache_xref[j].re = NULL;
			regex_cache_xref[j].extra = NULL;
		}
	}

	/*
	 * loop thru all arts applying global & local filtering rules
	 */
	for (i = 0; i < top; i++) {
		arts[i].score = 0;

		if (tinrc.kill_level == KILL_READ && IS_READ(i)) /* skip only when the article is read */
			continue;

		mesg[0] = '\0';				/* Clear system message field */

		for (j = 0; j < num; j++) {
			if (ptr[j].inscope) {
				/*
				 * Filter on Subject: line
				 */
				if (ptr[j].subj != (char *) 0) {
					if (test_regex (arts[i].subject, ptr[j].subj, ptr[j].icase, &regex_cache_subj[j])) {
						SET_FILTER(group, i, j);
					}
				}

				/*
				 * Filter on From: line
				 */
				if (ptr[j].from != (char *) 0) {
					if (arts[i].name != (char *) 0)
						sprintf (buf, "%s (%s)", arts[i].from, arts[i].name);
					else
						strcpy (buf, arts[i].from);
					if (test_regex (buf, ptr[j].from, ptr[j].icase, &regex_cache_from[j])) {
						SET_FILTER(group, i, j);
					}
				}

				/*
				 * Filter on Message-ID: line
				 * Apply to Message-ID: & References: lines or
				 * Message-ID: & last entry from References: line
				 * Case is important here
				 */
				if (ptr[j].msgid != (char *) 0) {

					struct t_article *art = &arts[i];
					char *refs = NULL;
					const char *myrefs = NULL;
					const char *mymsgid = NULL;

/*
 * TODO nice idea del'd; better apply one rule on all fitting
 * TODO articles, so we can switch to an appropriate algorithm
 * TODO for each kind of rule, including the deleted one.
 */
					/* myrefs does not need to be freed */

					/* use full references header or just the last entry ? */
					switch (ptr[j].fullref) {
						case FILTER_MSGID:
							myrefs = REFS(art, refs);
							mymsgid = MSGID(art);
							break;
						case FILTER_MSGID_LAST:
							myrefs = (art->refptr->parent) ? art->refptr->parent->txt : "";
							mymsgid = MSGID(art);
							break;
						case FILTER_MSGID_ONLY:
							myrefs = "";
							mymsgid = MSGID(art);
							break;
						case FILTER_REFS_ONLY:
							myrefs = REFS(art, refs);
							mymsgid = "";
							break;
						default: /* should not happen */
							/* CONSTANTCONDITION */
							assert(0 != 0);
							break;
					}

					if (test_regex (myrefs, ptr[j].msgid, FALSE, &regex_cache_msgid[j])) {
						SET_FILTER(group, i, j);
					} else if (test_regex (mymsgid, ptr[j].msgid, FALSE, &regex_cache_msgid[j])) {
						SET_FILTER(group, i, j);
					}
					FreeIfNeeded(refs);
				}
				/*
				 * Filter on Lines: line
				 */
				if ((ptr[j].lines_cmp != FILTER_LINES_NO) && (arts[i].lines >= 0)) {
					switch (ptr[j].lines_cmp) {
						case FILTER_LINES_EQ:
							if (arts[i].lines == ptr[j].lines_num) {
/*
wait_message (1, "FILTERED Lines arts[%d] == [%d]", arts[i].lines, ptr[j].lines_num);
*/
								SET_FILTER(group, i, j);
							}
							break;
						case FILTER_LINES_LT:
							if (arts[i].lines < ptr[j].lines_num) {
/*
wait_message (1, "FILTERED Lines arts[%d] < [%d]", arts[i].lines, ptr[j].lines_num);
*/
								SET_FILTER(group, i, j);
							}
							break;
						case FILTER_LINES_GT:
							if (arts[i].lines > ptr[j].lines_num) {
/*
wait_message (1, "FILTERED Lines arts[%d] > [%d]", arts[i].lines, ptr[j].lines_num);
*/
								SET_FILTER(group, i, j);
							}
							break;
						default:
							break;
					}
				}

				/*
				 * Filter on GNKSA code
				 */
				if ((ptr[j].gnksa_cmp != FILTER_LINES_NO) && (arts[i].gnksa_code >= 0)) {
					switch (ptr[j].gnksa_cmp) {
						case FILTER_LINES_EQ:
							if (arts[i].gnksa_code == ptr[j].gnksa_num) {
								SET_FILTER(group, i, j);
							}
							break;
						case FILTER_LINES_LT:
							if (arts[i].gnksa_code < ptr[j].gnksa_num) {
								SET_FILTER(group, i, j);
							}
							break;
						case FILTER_LINES_GT:
							if (arts[i].gnksa_code > ptr[j].gnksa_num) {
								SET_FILTER(group, i, j);
							}
							break;
						default:
							break;
					}
				}

				/*
				 * Filter on Xref: lines
				 */
				if (arts[i].xref && *arts[i].xref != '\0') {
					if (ptr[j].xref_max > 0 || ptr[j].xref != (char*)0) {
						char *s, *e;
						int group_count;

						s = arts[i].xref;

						while(*s && !isspace((int)*s))
							s++;
						while(*s && isspace((int)*s))
							s++;

						group_count=0;

						while(*s) {
							e = s;
							while (*e && *e != ':' && !isspace((int)*e))
								e++;

							if (ptr[j].xref_max > 0) {
								strncpy(buf, s, e-s);
								buf[e-s] = '\0';
								for (k = 0; k < ptr[j].xref_score_cnt; k++) {
									if (GROUP_MATCH(buf, ptr[j].xref_score_strings[k], TRUE)) {
										group_count += ptr[j].xref_scores[k];
										break;
									}
								}
								if (k == ptr[j].xref_score_cnt)
									group_count++;
							}
							if (ptr[j].xref != (char*)0) {
								strncpy(buf, s, e-s);
								buf[e-s] = '\0';
								/* don't filter when we are actually in that group */
								/* Group names shouldn't be case sensitive in any case. Whatever */
								if (ptr[j].type != FILTER_KILL || strcmp(group->name, buf) != 0) {
									if (test_regex (buf, ptr[j].xref, ptr[j].icase, &regex_cache_xref[j]))
										group_count = -1;
								}
							}
							s=e;
							while(*s && !isspace((int)*s))
								s++;
							while(*s && isspace((int)*s))
								s++;
						}
						if (group_count == -1 || group_count>ptr[j].xref_max) {
							SET_FILTER(group, i, j);
						}
					}
				}
			}
		}
	}

	if (mesg[0] != '\0')
		error_message (mesg);

	/*
	 * throw away the contents of all regex_caches
	 */
	if (tinrc.wildcard) {
		for (j = 0; j < num; j++) {
			FreeIfNeeded(regex_cache_subj[j].re);
			FreeIfNeeded(regex_cache_subj[j].extra);
			FreeIfNeeded(regex_cache_from[j].re);
			FreeIfNeeded(regex_cache_from[j].extra);
			FreeIfNeeded(regex_cache_msgid[j].re);
			FreeIfNeeded(regex_cache_msgid[j].extra);
			FreeIfNeeded(regex_cache_xref[j].re);
			FreeIfNeeded(regex_cache_xref[j].extra);
		}
		free(regex_cache_subj);
		free(regex_cache_from);
		free(regex_cache_msgid);
		free(regex_cache_xref);
	}

	/*
	 * now entering the main filter loop:
	 * all articles have scored, so do kill & select
	 */

	for (i = 0; i < top; i++) {
		if (arts[i].score <= SCORE_LIM_KILL) {
			arts[i].killed = TRUE;
			num_of_killed_arts++;
			filtered = TRUE;
			art_mark_read (group, &arts[i]);
		} else if (arts[i].score >= SCORE_LIM_SEL) {
			arts[i].selected = TRUE;
			num_of_selected_arts++;
		}
	}
	return filtered;
}

static int
set_filter_scope (
	struct t_group *group)
{
	int i, num, inscope;
	struct t_filter *ptr, *prev;

	num = group->glob_filter->num;
	ptr = group->glob_filter->filter;
	inscope = num;
	prev = ptr;

	for (i = 0; i < num; i++) {
		ptr[i].inscope = TRUE;
		ptr[i].next = (struct t_filter*) 0;
		if (ptr[i].scope != (char *) 0) {
			if (!match_group_list (group->name, ptr[i].scope)) {
				ptr[i].inscope = FALSE;
				inscope--;
			}
		}
		if (i != 0 && ptr[i].inscope)
			prev = prev->next = &ptr[i];
	}
	return inscope;
}
