/*
 *  Project   : tin - a Usenet reader
 *  Module    : filter.c
 *  Author    : I.Lea
 *  Created   : 28-12-92
 *  Updated   : 07-10-97
 *  Notes     : Filter articles. Kill & auto selection are supported.
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 *
 *              update for scoring (c) 1997 by Oliver B. Warzecha
 */

#include	"tin.h"
#include	"tcurses.h"
#include	"menukeys.h"

#define IS_READ(i)	(arts[i].status == ART_READ)
#define IS_KILLED(i)	(arts[i].killed)
#define IS_SELECTED(i)	(arts[i].selected)

/* SET_FILTER in group grp, current article arts[i], with rule ptr[j] */
/* 
 * filtering is now done this way:
 * a. set score for all articles and rules
 * b. check each article if the score is above or below the limit
 */

#define	SET_FILTER(grp,i,j)	\
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

int *arr_max;
int *arr_num;
struct t_filter *arr_ptr;
struct t_filters glob_filter = { 0, 0, (struct t_filter *) 0 };

/*
** Local prototypes
*/
static int get_choice (int x, const char *help, const char *prompt, const char *opt1, const char *opt2, const char *opt3, const char *opt4, const char *opt5);
static int iAddFilterRule (struct t_group *psGrp, struct t_article *psArt, struct t_filter_rule *psRule);
static int unfilter_articles (void);
static int set_filter_scope (struct t_group *group);
static void free_filter_array (struct t_filters *ptr);
static void free_filter_item (struct t_filter *ptr);
#ifndef INDEX_DAEMON
	static void vSetFilter (struct t_filter *psFilter);
	static void vWriteFilterArray (FILE *fp, struct t_filters *ptr, long theTime);
	static void vWriteFilterFile (char *pcFile);
#endif


struct t_filter *
psExpandFilterArray (
	struct t_filter *ptr,
	int *num)
{
	size_t block;
	struct t_filter *new;

	(*num)++;

	block = *num * sizeof (struct t_filter);

	if (*num == 1)	/* allocate */
		new = (struct t_filter *) my_malloc (block);
	else	/* reallocate */
		new = (struct t_filter *) my_realloc ((char *) ptr, block);

	return new;
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
		psFilter->subj = (char *) 0;
		psFilter->from = (char *) 0;
		psFilter->msgid = (char *) 0;
		psFilter->lines_cmp = FILTER_LINES_NO;
		psFilter->lines_num = 0;
		psFilter->score = 0;
		psFilter->xref = (char *) 0;
		psFilter->xref_max = 0;
		psFilter->xref_score_cnt = 0;
		psFilter->time = 0L;
		psFilter->next = (struct t_filter *) 0;
	}
}
#endif /* INDEX_DAEMON */

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
		for (i = 0 ; i < ptr->num ; i++)
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
int
read_filter_file (
	char	*file,
	t_bool global_file)
{
	FILE *fp;
	char buf[HEADER_LEN];
	char scope[HEADER_LEN];
	char subj[HEADER_LEN];
	char from[HEADER_LEN];
	char msgid[HEADER_LEN];
	char lines[HEADER_LEN];
	char xref[HEADER_LEN];
	char xref_score[HEADER_LEN];
	char scbuf[PATH_LEN];
	char *s;
	int expired = FALSE;
	int expired_time = FALSE;
	int global = TRUE;
	int i = 0;
	int icase = 0, type = -1;
	int score = 0;
	int xref_max = 0;
	int xref_score_cnt = 0;
	int xref_score_value = 0;
	time_t current_secs = 0L;
	long secs = 0L;
	struct t_group *psGrp;

	if ((fp = fopen (file, "r")) == (FILE *) 0)
		return FALSE;

	if (INTERACTIVE)
			wait_message (0, txt_reading_filter_file, (global ? "global ": "" ));

	time (&current_secs);

	/*
	 * Reset all filter arrays if doing a reread of the active file
	 */
	if (global_file)
		free_all_filter_arrays ();

	psGrp = (struct t_group *) 0;
	arr_ptr = (struct t_filter *) 0;

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		if (*buf == '#' || *buf == '\n')
			continue;

		switch(tolower((unsigned char)buf[0])) {
		case 'c':
			if (match_integer (buf+1, "ase=", &icase, 1)) {
				if (arr_ptr && !expired_time)
					arr_ptr[i].icase = icase;

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
#endif
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
				psGrp = (struct t_group *) 0;   /* fudge for out of order rules */
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
				if (arr_ptr)
					arr_ptr[i].msgid = my_strdup (msgid);
			}
			break;
		case 's':
			if (match_string (buf+1, "cope=", scope, sizeof (scope))) {
#ifdef DEBUG
if (debug) {
	my_printf ("scope=[%s] num=[%d]\n", scope, glob_filter.num);
	my_flush ();
}
#endif
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
#endif
				break;
			}
			/*
			 *  read score for rule
			 */

			if (match_string (buf+1, "core=", scbuf, PATH_LEN)) {
				score = atoi(scbuf);
#ifdef DEBUG
if (debug) {
	my_printf ("score=[%d]\n", score);
	my_flush();
}
#endif
				if (arr_ptr && !expired_time) {
					if (score > SCORE_MAX)
						score = SCORE_MAX;
					else if (score < -SCORE_MAX)
						score = -SCORE_MAX;
					else if (score == 0) {
						if (strncmp(scbuf, "kill", 4) == 0)
							score = SCORE_KILL;
						else if (strncmp(scbuf, "hot", 3) == 0)
							score = SCORE_SELECT;
						else
							score = SCORE_DEFAULT;
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
	my_printf ("type=[%d][%s]\n", type, ((type == 0) ? "KILL" : "SELECT"));
	my_flush ();
}
#endif
				if (arr_ptr) {
					arr_ptr[i].type = type;
					arr_ptr[i].score = (type == 0) ? SCORE_KILL : SCORE_SELECT;
				}
				break;
			}
	 		if (match_long (buf+1, "ime=", &secs)) {
				if (arr_ptr && !expired_time) {
					arr_ptr[i].time = secs;
					if (secs && current_secs > secs) {
#ifdef DEBUG
if (debug) {
	my_printf ("EXPIRED  secs=[%ld]  current_secs=[%ld]\n", secs, current_secs);
	my_flush ();
}
#endif
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
						if (isdigit(xref_score[0])) {
							xref_score_value = atoi(xref_score);
							if ((s = strchr(xref_score, ',')))
								s++;
							arr_ptr[i].xref_scores[xref_score_cnt] = xref_score_value;
							arr_ptr[i].xref_score_strings[xref_score_cnt] = (s != NULL ? my_strdup(s) : NULL);
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

	return TRUE;
}

/*
 *  write filter strings to ~/.tin/filter
 */

static void
vWriteFilterFile (
	char *pcFile)
{
	FILE *hFp;
/*	int i; */
	time_t lCurTime;
/*	struct t_filters *psFilter; */

	if ((hFp = fopen (pcFile, "w")) == (FILE *) 0)
		return;

	/*
	 * Get current time for checking rules against expire times
	 */
	time (&lCurTime);

	/* FIXME: move to lang.c ! */
	fprintf (hFp, "# Global & local filter file for the TIN newsreader\n#\n");
	fprintf (hFp, "# Global format:\n");
	fprintf (hFp, "#   group=STRING      Newsgroups list (e.g. comp.*,!*sources*)    [mandatory]\n");
	fprintf (hFp, "#   type=NUM          0=kill 1=auto-select (hot) [mandatory]\n");
	fprintf (hFp, "#   case=NUM          Compare=0 / ignore=1 case when filtering\n");
	fprintf (hFp, "#   score=NUM         Score to give (e.g. 70)\n");
	fprintf (hFp, "#   subj=STRING       Subject: line (e.g. How to be a wizard)\n");
	fprintf (hFp, "#   from=STRING       From: line (e.g. *Craig Shergold*)\n");
	fprintf (hFp, "#   msgid=STRING      Message-ID: line (e.g. <123@ether.net>)\n");
	fprintf (hFp, "#   lines=NUM         Lines: line\n");
	fprintf (hFp, "#   either:\n");
	fprintf (hFp, "#   xref_max=NUM      Maximum score (e.g. 5)\n");
	fprintf (hFp, "#   xref_score=NUM,PATTERN score for pattern (e.g 0,*.answers)\n");
	fprintf (hFp, "#   ...\n");
	fprintf (hFp, "#   or:\n");
	fprintf (hFp, "#   xref=PATTERN      Kill pattern (e.g. alt.flame*)\n");
	fprintf (hFp, "#   \n");
	fprintf (hFp, "#   time=NUM          Filter period in days (default %d)\n#\n", default_filter_days);
	fflush (hFp);

	/*
	 * Save global filters
	 */
	vWriteFilterArray (hFp, &glob_filter, lCurTime);

	fclose (hFp);
	chmod (pcFile, (S_IRUSR|S_IWUSR));
}

#endif/* !INDEX_DAEMON */

#ifndef INDEX_DAEMON
static void
vWriteFilterArray (
	FILE *fp,
	struct t_filters *ptr,
	long theTime)
{
	register int i;
	int j;

	if (ptr == (struct t_filters *) 0)
		return;

	for (i = 0 ; i < ptr->num ; i++) {

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
		fprintf (fp, "type=%d\n", ptr->filter[i].type);
		fprintf (fp, "case=%d\n", ptr->filter[i].icase);
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

		if (ptr->filter[i].msgid != (char *) 0)
			fprintf (fp, "msgid=%s\n", ptr->filter[i].msgid);

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
		if (ptr->filter[i].xref != NULL)
			fprintf (fp, "xref=%s\n", ptr->filter[i].xref);

		if (ptr->filter[i].xref_max > 0) {
			fprintf (fp, "xref_max=%d\n", ptr->filter[i].xref_max);

			for(j=0;j<ptr->filter[i].xref_score_cnt;j++)
				fprintf (fp, "xref_score=%d%s%s\n", ptr->filter[i].xref_scores[j], ptr->filter[i].xref_score_strings[j] ? "," : "", ptr->filter[i].xref_score_strings[j]);
		}
		if (ptr->filter[i].time)
			fprintf (fp, "time=%ld\n", ptr->filter[i].time);

		fprintf (fp, "#####\n"); /* makes filter file more readable */
	}

	fflush (fp);
}
#endif /* INDEX_DAEMON */


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
	int ch, n = 0, i = 0;
	const char *argv[5];

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
 *  Interactive filter menu so that the user can dynamically enter parameters.
 *  Can be configured for kill or auto-selection screens.
 */

int
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
	int filtered = TRUE;
	struct t_filter_rule rule;

	rule.text[0] = '\0';
	rule.scope[0] = '\0';
	rule.counter = 0;
	rule.lines_cmp = FILTER_LINES_NO;
	rule.lines_num = 0;
	rule.from_ok = FALSE;
	rule.lines_ok = FALSE;
	rule.msgid_ok = FALSE;
	rule.subj_ok = FALSE;
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

	/*
	 *  TODO: move all scoring text to lang.c
	 */

	ptr_filter_score = "Enter score for rule (default=100): ";

	len = cCOLS - 30;

	sprintf (text_time, txt_time_default_days, default_filter_days);
	sprintf (text_subj, ptr_filter_subj, len, len, art->subject);

	if (art->name != (char *) 0)
		sprintf (buf, "%s (%s)", art->from, art->name);
	else
		strcpy (buf, art->from);

	sprintf (text_from, ptr_filter_from, len, len, buf);
	sprintf (text_msgid, ptr_filter_msgid, len, len, MSGID(art));

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

	if (rule.text[0]) {
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
	}

	if (!rule.text[0]) {
		rule.check_string = TRUE;
		/*
		 * Subject:
		 */
		i = get_choice (INDEX_TOP+3, txt_help_filter_subj, text_subj, txt_yes, txt_no, (char *)0, (char *)0, (char *)0);

		if (i == -1)
			return FALSE;
		else
			rule.subj_ok = (i == FALSE);

		/*
		 * From:
		 */
		if (rule.subj_ok)
			i = get_choice (INDEX_TOP+4, txt_help_filter_from, text_from, txt_no, txt_yes, (char *)0, (char *)0, (char *)0);
		else
			i = get_choice (INDEX_TOP+4, txt_help_filter_from, text_from, txt_yes, txt_no, (char *)0, (char *)0, (char *)0);

		if (i == -1)
			return FALSE;
		else
			rule.from_ok = (rule.subj_ok) ? (i != FALSE) : (i == FALSE);

		/*
		 * Message-Id:
		 */
		if (rule.subj_ok || rule.from_ok)
			i = get_choice (INDEX_TOP+5, txt_help_filter_msgid, text_msgid, txt_no, txt_yes, (char *)0, (char *)0, (char *)0);
		else 
			i = get_choice (INDEX_TOP+5, txt_help_filter_msgid, text_msgid, txt_yes, txt_no, (char *)0, (char *)0, (char *)0);

		if (i == -1)
			return FALSE;
		else
			rule.msgid_ok = (rule.subj_ok || rule.from_ok) ? (i != FALSE) : (i == FALSE);

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
	show_menu_help("Enter the score weight (range 0 < score <= 10000)"); /* FIXME: a sprintf() is necessary here */

	if (!prompt_menu_string(INDEX_TOP+8, (int)strlen(ptr_filter_score), buf))
		return FALSE;

	rule.score = atoi(buf);
	/*
	 *  assure we are in range
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
	strcpy(double_time, "2x ");
	strcat(double_time, text_time);
	strcpy(quat_time, "4x ");
	strcat(quat_time, text_time);
	i = get_choice (INDEX_TOP+9, txt_help_filter_time, ptr_filter_time, txt_unlimited_time, text_time, double_time, quat_time, (char *)0);

	if (i == -1)
		return FALSE;

	rule.expire_time = i;

	/*
	 * Scope
	 */
	if (rule.text[0] || rule.subj_ok || rule.from_ok || rule.msgid_ok || rule.lines_ok) {
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

		strcpy (rule.scope, ((i != FALSE) ? "*" : argv[i]));
	} else
		return FALSE;

	forever {
		ch = prompt_slk_response(ch_default, "eqs\033", ptr_filter_quit_edit_save);
		switch (ch) {
/*
 *  what is this? when you edit the filter_file the rule is not saved?
 */

		case iKeyFilterEdit:
			start_line_offset = 22; /* FIXME: check it out */
			invoke_editor (local_filter_file, start_line_offset);
			unfilter_articles ();
#ifndef INDEX_DAEMON
			(void) read_filter_file (local_filter_file, FALSE);
#endif /* INDEX_DAEMON */
			filtered = TRUE;
			return (filtered);
			/* keep lint quiet: */
			/* FALLTHROUGH */

		case iKeyQuit:
		case iKeyAbort:
			filtered = FALSE;
			return (filtered);
			/* keep lint quiet: */
			/* FALLTHROUGH */

		case iKeyFilterSave:
			/*
			 * Add the filter rule and save it to the filter file
			 */
			filtered = iAddFilterRule (group, art, &rule);
			return (filtered);
			/* keep lint quiet: */
			/* FALLTHROUGH */

		default:
			break;
		}
	}
	/* NOTREACHED */
}

/*
 *  Quick command to add a kill filter to specified groups filter
 */

int
quick_filter_kill (
	struct t_group *group,
	struct t_article *art)
{
	int header;
	int filtered;
	struct t_filter_rule rule;

#ifdef DEBUG
	if (debug)
		error_message ("KILL header=[%d] scope=[%s] expire=[%s] case=[%d]",
			group->attribute->quick_kill_header,
			(group->attribute->quick_kill_scope ?
				group->attribute->quick_kill_scope : ""),
			(group->attribute->quick_kill_expire ? "ON" : "OFF"),
			group->attribute->quick_kill_case);
#endif

	header = group->attribute->quick_kill_header;

	/*
	 * Setup rules
	 */
	rule.counter = 0;
	rule.scope[0] = '\0';
	if (group->attribute->quick_kill_scope)
		strcpy (rule.scope, group->attribute->quick_kill_scope);

	rule.lines_cmp = FILTER_LINES_NO;
	rule.lines_num = 0;
	rule.lines_ok = (header == FILTER_LINES);
	rule.msgid_ok = (header == FILTER_MSGID);

	if (header == FILTER_FROM_CASE_SENSITIVE || header == FILTER_FROM_CASE_IGNORE)
		rule.from_ok = TRUE;
	else
		rule.from_ok = FALSE;

	if (header == FILTER_SUBJ_CASE_SENSITIVE || header == FILTER_SUBJ_CASE_IGNORE)
		rule.subj_ok = TRUE;
	else
		rule.subj_ok = FALSE;

	rule.text[0] = '\0';
	rule.type = FILTER_KILL;
	rule.icase = group->attribute->quick_kill_case;
	rule.expire_time = group->attribute->quick_kill_expire;
	rule.check_string = TRUE;
	rule.score = SCORE_KILL;

	filtered = iAddFilterRule (group, art, &rule);

	return (filtered);
}

/*
 *  Quick command to add an auto-select filter to specified groups filter
 */

int
quick_filter_select (
	struct t_group *group,
	struct t_article *art)
{
	int header;
	int filtered;
	struct t_filter_rule rule;

#ifdef DEBUG
	if (debug)
		error_message ("SELECT header=[%d] scope=[%s] expire=[%s] case=[%d]",
			group->attribute->quick_select_header,
			(group->attribute->quick_select_scope ?
				group->attribute->quick_select_scope : ""),
			(group->attribute->quick_select_expire ? "ON" : "OFF"),
			group->attribute->quick_select_case);
#endif

	header = group->attribute->quick_select_header;

	/*
	 * Setup rules
	 */
	rule.counter = 0;
	rule.scope[0] = '\0';
	if (group->attribute->quick_select_scope)
		strcpy (rule.scope, group->attribute->quick_select_scope);

	rule.lines_cmp = FILTER_LINES_NO;
	rule.lines_num = 0;
	rule.lines_ok = (header == FILTER_LINES);
	rule.msgid_ok = (header == FILTER_MSGID);

	if (header == FILTER_FROM_CASE_SENSITIVE || header == FILTER_FROM_CASE_IGNORE)
		rule.from_ok = TRUE;
	else
		rule.from_ok = FALSE;

	if (header == FILTER_SUBJ_CASE_SENSITIVE || header == FILTER_SUBJ_CASE_IGNORE)
		rule.subj_ok = TRUE;
	else
		rule.subj_ok = FALSE;

	rule.text[0] = '\0';
	rule.type = FILTER_SELECT;
	rule.icase = group->attribute->quick_select_case;
	rule.expire_time = group->attribute->quick_select_expire;
	rule.check_string = TRUE;
	rule.score = SCORE_SELECT;

	filtered = iAddFilterRule (group, art, &rule);

	return filtered;
}

#if 0 /* this sucks! (urs) */
/*
 *  Quick command to add an auto-select filter to the article that user
 *  has just posted. Selects on Subject: line with limited expire time.
 *  Don't precess if MAILGROUP.
 */
int
quick_filter_select_posted_art (
	struct t_group *group,
	char *subj)	/* return value is always ignored */
{
	int filtered = FALSE;
	struct t_article art;
	struct t_filter_rule rule;

	if (group->type == GROUP_TYPE_NEWS) {
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
		rule.subj_ok = TRUE;
		rule.text[0] = '\0';
		rule.scope[0] = '\0';
		rule.type = FILTER_SELECT;
		rule.icase = FALSE;
		rule.expire_time = TRUE;
		rule.check_string = TRUE;
		rule.score = SCORE_SELECT;

		filtered = iAddFilterRule (group, &art, &rule);

		FreeIfNeeded(art.subject);

	}

	return filtered;
}
#endif /* 0 */

/*
 * API to add filter rule to the local or global filter array
 */

static int
iAddFilterRule (
	struct t_group *psGrp,
	struct t_article *psArt,
	struct t_filter_rule *psRule)
{
	char acBuf[PATH_LEN];
	int iFiltered = FALSE;
	int *plNum, *plMax;
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
	psPtr[*plNum].scope = (char *) 0;
	psPtr[*plNum].subj = (char *) 0;
	psPtr[*plNum].from = (char *) 0;
	psPtr[*plNum].msgid = (char *) 0;
	psPtr[*plNum].lines_cmp = psRule->lines_cmp;
	psPtr[*plNum].lines_num = psRule->lines_num;
	psPtr[*plNum].score = psRule->score;
	psPtr[*plNum].xref = (char *) 0;
	psPtr[*plNum].xref_max = 0;
	psPtr[*plNum].xref_score_cnt = 0;

	if (psRule->scope[0] != '*' && psRule->scope[1] != '\0')
		psPtr[*plNum].scope = my_strdup (psRule->scope);

	time (&lCurTime);
	switch(psRule->expire_time)
	{
		case 1:
			psPtr[*plNum].time = lCurTime + (default_filter_days * 86400);
			break;
		case 2:
			psPtr[*plNum].time = lCurTime + (default_filter_days * 86400*2);
			break;
		case 3:
			psPtr[*plNum].time = lCurTime + (default_filter_days * 86400*4);
			break;
		default:
			psPtr[*plNum].time = 0L;
			break;
	}

	if (psRule->text[0]) {
		if (psRule->check_string)
			sprintf (acBuf, "*%s*", quote_wild(psRule->text));
		else
			sprintf (acBuf, "*%s*", psRule->text);

		switch (psRule->counter) {
			case FILTER_SUBJ_CASE_IGNORE:
			case FILTER_SUBJ_CASE_SENSITIVE:
				psPtr[*plNum].subj = my_strdup (acBuf);
				if (psRule->counter == FILTER_SUBJ_CASE_IGNORE)
					psPtr[*plNum].icase = TRUE;

				break;
			case FILTER_FROM_CASE_IGNORE:
			case FILTER_FROM_CASE_SENSITIVE:
				psPtr[*plNum].from = my_strdup (acBuf);
				if (psRule->counter == FILTER_FROM_CASE_IGNORE)
					psPtr[*plNum].icase = TRUE;

				break;
			case FILTER_MSGID:
				psPtr[*plNum].msgid = my_strdup (acBuf);
				break;
			default:
				break;
		}
		iFiltered = TRUE;
		(*plNum)++;
	} else {
		if (psRule->subj_ok) {
			if (psRule->check_string)
				sprintf (acBuf, "*%s*", quote_wild (psArt->subject));
			else
				sprintf (acBuf, "*%s*", psArt->subject);

			psPtr[*plNum].subj = my_strdup (acBuf);
		}
		if (psRule->from_ok) {
			if (psArt->name != (char *) 0)
				sprintf (acBuf, "*%s (%s)*", psArt->from, psArt->name);
			else
				sprintf (acBuf, "*%s*", psArt->from);

			psPtr[*plNum].from = my_strdup (acBuf);
		}
		if (psRule->msgid_ok) {
			sprintf (acBuf, "*%s*", MSGID(psArt));
			psPtr[*plNum].msgid = my_strdup (acBuf);
		}
		if (psRule->subj_ok || psRule->from_ok ||
		    psRule->msgid_ok || psRule->lines_ok) {
			iFiltered = TRUE;
			(*plNum)++;
		}
	}

	if (iFiltered) {
#ifdef DEBUG
		if (debug)
			wait_message (2, "inscope=[%s] scope=[%s] typ=[%d] case=[%d] subj=[%s] from=[%s] msgid=[%s] line=[%d %d] time=[%ld]",
				(psPtr[*plNum-1].inscope ? "TRUE" : "FALSE"),
				(psRule->scope ? psRule->scope : ""),
				psPtr[*plNum-1].type, psPtr[*plNum-1].icase,
				(psPtr[*plNum-1].subj ? psPtr[*plNum-1].subj : ""),
				(psPtr[*plNum-1].from ? psPtr[*plNum-1].from : ""),
				(psPtr[*plNum-1].msgid ? psPtr[*plNum-1].msgid : ""),
				psPtr[*plNum-1].lines_cmp, psPtr[*plNum-1].lines_num,
				psPtr[*plNum-1].time);
#endif

#ifndef INDEX_DAEMON
		vWriteFilterFile (local_filter_file);
#endif /* !INDEX_DAEMON */
	}

	return iFiltered;
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

	for (i=0 ; i < top ; i++) {
		if (arts[i].killed) {
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

int
filter_articles (
	struct t_group *group)
{
	char buf[LEN];
	int filtered = FALSE;
	int num, inscope;
	int global_filter;
	register int i, j, k;
	struct t_filter *ptr; /*, *curr; */
/*	int score; */

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
	if (group->glob_filter->num) {
		/*
		 * Check if any scope rules are active for this group
		 * ie. group=comp.os.linux.help  scope=comp.os.linux.*
		 */
		inscope = set_filter_scope (group);
		if (!cmd_line)
			wait_message (0, txt_filter_global_rules, inscope, group->glob_filter->num);

		num = group->glob_filter->num;
		ptr = group->glob_filter->filter;
		global_filter = TRUE;
	}

	/*
	 * loop thru all arts applying global & local filtering rules
	 */
	for (i=0 ; i < top ; i++) {
		if (IS_READ(i)) /* skip only when the article is read */
			continue;

		arts[i].score = 0;
		for (j=0 ; j < num ; j++) {
			if (ptr[j].inscope) {
				/*
				 * Filter on Subject: line
				 */
				if (ptr[j].subj != (char *) 0) {
					if (REGEX_MATCH (arts[i].subject, ptr[j].subj, ptr[j].icase))
						SET_FILTER(group, i, j);
				}
				/*
				 * Filter on From: line
				 */
				if (ptr[j].from != (char *) 0) {
					if (arts[i].name != (char *) 0)
						sprintf (buf, "%s (%s)", arts[i].from, arts[i].name);
					else
						strcpy (buf, arts[i].from);

					if (REGEX_MATCH (buf, ptr[j].from, ptr[j].icase))
						SET_FILTER(group, i, j);
				}

				/*
				 * Filter on Message-ID: line
				 * Apply to Message-ID: & References: lines
				 * Case is important here
				 */
				if (ptr[j].msgid != (char *) 0) {

					struct t_article *art = &arts[i];
					char *refs;
/* TODO nice idea del'd; better apply one rule on all fitting
 * TODO articles, so we can switch to an appropriate algorithm
 * TODO for each kind of rule, including the deleted one.
 */
					if (REGEX_MATCH (REFS(art, refs), ptr[j].msgid, FALSE) || REGEX_MATCH (MSGID(art), ptr[j].msgid, FALSE))
						SET_FILTER(group, i, j);

					FreeIfNeeded(refs);

				}
				/*
				 * Filter on Lines: line
				 */
				if (ptr[j].lines_cmp != FILTER_LINES_NO && arts[i].lines>=0) {
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
				 * Filter on Xref: lines
				 */
				if(arts[i].xref && *arts[i].xref!='\0') {
					if (ptr[j].xref_max > 0 || ptr[j].xref != (char*)0) {
						char *s,*e;
						int group_count;

						s=arts[i].xref;

						while(*s && !isspace(*s))
							s++;
						while(*s && isspace(*s))
							s++;

						group_count=0;

						while(*s) {
							e=s;
							while (*e && *e!=':' && !isspace(*e))
								e++;

							if(ptr[j].xref_max > 0) {
								strncpy(buf, s, e-s);
								buf[e-s]='\0';
								for(k=0;k<ptr[j].xref_score_cnt;k++) {
									if(GROUP_MATCH(buf, ptr[j].xref_score_strings[k], TRUE)) {
										group_count+=ptr[j].xref_scores[k];
										break;
									}
								}
								if(k==ptr[j].xref_score_cnt)
									group_count++;
							}
							if(ptr[j].xref != (char*)0) {
								strncpy(buf, s, e-s);
								buf[e-s]='\0';
								/* don't filter when we are actually in that group */
								/* Group names shouldn't be case sensitive in any case. Whatever */
								if(ptr[j].type != FILTER_KILL || strcmp(group->name, buf)!=0) {
									if (REGEX_MATCH (buf, ptr[j].xref, ptr[j].icase)) {
										group_count=-1;
										break;
									}
								}
							}
							s=e;
							while(*s && !isspace(*s))
								s++;
							while(*s && isspace(*s))
								s++;
						}
						if(group_count==-1 || group_count>ptr[j].xref_max)
							SET_FILTER(group, i, j);
					}
				}
			}
		}
	}

	/*
	 * now entering the main filter loop:
	 * all articles have scored, so do kill & select
	 */

	for ( i=0; i<top; i++) {
		if (arts[i].score <= SCORE_LIM_KILL) {
			arts[i].killed = TRUE;
			filtered = TRUE;
			art_mark_read (group, &arts[i]);
		} else if (arts[i].score >= SCORE_LIM_SEL) {
			arts[i].selected = TRUE;
			num_of_selected_arts++;
		}
	}
	return filtered;
}

/*
 * Auto select articles.
 * WARNING - this routine is presently a kludge. It calls
 * filter_articles() which also kills articles. It also always returns
 * true in order to fake out the display code (cause it doesn't know
 * if any articles were actually selected)
 * The correct way to do this is to modify filter_articles() to take
 * another arg to specify whether killing, auto-selecting, or both is to be
 * done, rename it to something else, and then have a new filter_articles()
 * and auto_select_articles() call this new routine with the appropriate
 * arguments.
 */

/* 
 * If filter_articles() is really score_articles() now then this
 * function is unnecessary anyway
 * TODO: replace calls to this routine by calls to filter_articles()
 */

int
auto_select_articles (
	struct t_group *group)
{
	filter_articles (group);
	return (TRUE);
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
