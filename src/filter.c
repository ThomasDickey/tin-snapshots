/*
 *  Project   : tin - a Usenet reader
 *  Module    : filter.c
 *  Author    : I.Lea
 *  Created   : 28-12-92
 *  Updated   : 21-12-94
 *  Notes     : Filter articles. Kill & auto selection are supported.
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"
#include	"menukeys.h"

#define IS_READ(i)		(arts[i].status == ART_READ)
#define IS_KILLED(i)		(arts[i].killed == TRUE)
#define IS_SELECTED(i)	(arts[i].selected == TRUE)

#define	SET_FILTER(grp,i,j)	\
	if (ptr[j].type == FILTER_KILL) { \
		if (arts[i].killed == FALSE && arts[i].selected == FALSE) { \
			arts[i].killed = TRUE; \
			art_mark_read (grp, &arts[i]); \
		} \
	} else { \
		if (arts[i].selected == FALSE) { \
			arts[i].selected = TRUE; \
			arts[i].killed = FALSE; \
			num_of_selected_arts++; \
		} \
	}

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
static char *pcChkRegexStr (char *pcStr);
static int get_choice (int x, const char *help, const char *prompt, const char *opt1, const char *opt2, const char *opt3, const char *opt4, const char *opt5);
static int iAddFilterRule (struct t_group *psGrp, struct t_article *psArt, struct t_filter_rule *psRule);
static int unfilter_articles (void);
static int set_filter_scope (struct t_group *group);
static void free_filter_array (struct t_filters *ptr);
static void free_filter_item (struct t_filter *ptr);
#ifndef INDEX_DAEMON
	static void vSetFilter (struct t_filter *psFilter);
	static void vWriteFilterArray (FILE *fp, int global, struct t_filters *ptr, long theTime);
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

	if (*num == 1) {	/* allocate */
		new = (struct t_filter *) my_malloc (block);
	} else {	/* reallocate */
		new = (struct t_filter *) my_realloc ((char *) ptr, block);
	}

	return new;
}


#ifndef INDEX_DAEMON
static void
vSetFilter (
	struct t_filter *psFilter)
{
	if (psFilter != (struct t_filter *) 0)
	{
		psFilter->type = FILTER_SELECT;
		psFilter->scope = (char *) 0;
		psFilter->inscope = TRUE;
		psFilter->icase = FALSE;
		psFilter->subj = (char *) 0;
		psFilter->from = (char *) 0;
		psFilter->msgid = (char *) 0;
		psFilter->lines_cmp = FILTER_LINES_NO;
		psFilter->lines_num = 0;
		psFilter->xref = (char *) 0;
		psFilter->xref_max=0;
		psFilter->xref_score_cnt=0;
		psFilter->time = 0L;
	}
}
#endif /* INDEX_DAEMON */


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

static void
free_filter_array (
	struct t_filters *ptr)
{
	register int i;

	if (ptr != (struct t_filters *) 0) {
		for (i = 0 ; i < ptr->num ; i++) {
			free_filter_item(ptr->filter + i);
		}

		if (ptr->filter != (struct t_filter *) 0) {
			free ((char *) ptr->filter);
			ptr->filter = (struct t_filter *) 0;
		}

		ptr->num = 0;
		ptr->max = 0;
	}
}


void
free_all_filter_arrays (void)
{
	register int i;

	/*
	 * Global filter array
	 */
	free_filter_array (&glob_filter);

	/*
	 * Local filter arrays
	 */
	for (i = 0 ; i < num_active ; i++) {
		free_filter_array (active[i].grps_filter);
	}
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
	char group[HEADER_LEN];
	char scope[HEADER_LEN];
	char subj[HEADER_LEN];
	char from[HEADER_LEN];
	char msgid[HEADER_LEN];
	char lines[HEADER_LEN];
	char xref[HEADER_LEN];
	char xref_score[HEADER_LEN];
	char *s;
	int expired = FALSE;
	int expired_time = FALSE;
	int global = TRUE;
	int i = 0;
	int icase = 0, type = -1;
	int xref_max = 0;
	int xref_score_cnt=0;
	int xref_score_value=0;
	time_t current_secs = 0L;
	long secs = 0L;
	struct t_group *psGrp;

	if ((fp = fopen (file, "r")) == (FILE *) 0) {
		return FALSE;
	}

	if (SHOW_UPDATE) {
		if (global_file) {
			wait_message (txt_reading_global_filter_file);
		} else {
			wait_message (txt_reading_filter_file);
		}
	}

	time (&current_secs);

	/*
	 * Reset all filter arrays if doing a reread of the active file
	 */
	if (global_file) {
		free_all_filter_arrays ();
	}

	psGrp = (struct t_group *) 0;
	arr_ptr = (struct t_filter *) 0;

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		if (*buf == '#' || *buf == '\n') {
			continue;
		}
		switch(tolower((unsigned char)buf[0])) {
		case 'c':
			if (match_integer (buf+1, "ase=", &icase, 1)) {
				if (arr_ptr && !expired_time) {
					arr_ptr[i].icase = icase;
				}
				break;
			}
			break;
		case 'f':
			if (match_string (buf+1, "rom=", from, sizeof (from))) {
				if (arr_ptr && !expired_time) {
					arr_ptr[i].from = my_strdup (from);
				}
				break;
			}
			break;
		case 'g':
			if (match_string (buf+1, "roup=", group, sizeof (group))) {
				psGrp = psGrpFind (group);
				if (psGrp != (struct t_group *) 0) {	/* switch to group filter */
					global = FALSE;
					if (psGrp->grps_filter == (struct t_filters *) 0) {
if (debug) {
	my_printf ("Allocating grp_filter for group=[%s]\n", psGrp->name);
	my_flush ();
}
						psGrp->grps_filter =
							(struct t_filters *) my_malloc (sizeof (struct t_filters));
						psGrp->grps_filter->num = 0;
						psGrp->grps_filter->max = 0;
						psGrp->grps_filter->filter = (struct t_filter *) 0;
					}
				}
				subj[0] = '\0';
				from[0] = '\0';
				msgid[0] = '\0';
				lines[0] = '\0';
				secs = 0L;
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
				}
				break;
			}
			break;
		case 's':
			if (match_string (buf+1, "cope=", scope, sizeof (scope))) {
if (debug) {
	my_printf ("scope=[%s] num=[%d]\n", scope, glob_filter.num);
	my_flush ();
}
				global = TRUE;
				arr_num = &glob_filter.num;
				arr_max = &glob_filter.max;
				if (*arr_num >= (*arr_max - 1)) {
					glob_filter.filter = psExpandFilterArray (
						glob_filter.filter, arr_max);
				}
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
				if (arr_ptr && !expired_time) {
					arr_ptr[i].subj = my_strdup (subj);
				}
if (debug) {
	if (global) {
		my_printf ("6. buf=[%s]  Gsubj=[%s]\n", arr_ptr[i].subj, glob_filter.filter[i].subj);
		my_flush ();
	} else {
		my_printf ("6. buf=[%s]  Lsubj=[%s]\n", arr_ptr[i].subj, psGrp->grps_filter->filter[i].subj);
		my_flush ();
	}
}
				break;
			}
			break;
		case 't':
			if (match_integer (buf+1, "ype=", &type, 1)) {
if (debug) {
	my_printf ("type=[%d][%s]\n", type, (type == 0 ? "KILL" : "SELECT"));
	my_flush ();
}
				if (psGrp && !global) {
					arr_num = &psGrp->grps_filter->num;
					arr_max = &psGrp->grps_filter->max;
					if (*arr_num >= (*arr_max - 1)) {
						psGrp->grps_filter->filter = psExpandFilterArray (
							psGrp->grps_filter->filter, arr_max);
					}
					arr_ptr = psGrp->grps_filter->filter;
					i = *arr_num;
					(*arr_num)++;
					vSetFilter (&arr_ptr[i]);
					expired_time = FALSE;
if (debug) {
	my_printf ("grp_filter num=[%d] max=[%d]\n", *arr_num, *arr_max);
	my_flush ();
}
					subj[0] = '\0';
					from[0] = '\0';
					msgid[0] = '\0';
					lines[0] = '\0';
					secs = 0L;
				}
				if (arr_ptr) {
					arr_ptr[i].type = type;
				}
				break;
			}
	 		if (match_long (buf+1, "ime=", &secs)) {
				if (arr_ptr && !expired_time) {
					arr_ptr[i].time = secs;
					if (secs && current_secs > secs) {
if (debug) {
	my_printf ("EXPIRED  secs=[%ld]  current_secs=[%ld]\n", secs, current_secs);
	my_flush ();
}
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
				if (arr_ptr && ! expired_time) {
					arr_ptr[i].xref = my_strdup (xref);
				}
				break;
			}
			if (match_integer (buf+1, "ref_max=", &xref_max, 1000)) {
				if (arr_ptr && ! expired_time) {
					arr_ptr[i].xref_max = xref_max;
				}
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

	if (expired) {
		vWriteFilterFile (file);
	}

	if (cmd_line) {
		printf ("\r\n");
	}

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
	int i;
	time_t lCurTime;
	struct t_filters *psFilter;

	if ((hFp = fopen (pcFile, "w")) == (FILE *) 0) {
		return;
	}

	/*
	 * Get current time for checking rules against expire times
	 */
	time (&lCurTime);

	/* FIXME: move to lang.c ! */
	fprintf (hFp, "# Global & local filter file for the TIN newsreader\n#\n");
	fprintf (hFp, "# Global format:\n");
	fprintf (hFp, "#   scope=STRING      Newsgroups (e.g. comp.*)    [mandatory]\n");
	fprintf (hFp, "#   type=NUM          0=kill 1=auto-select (hot) [mandatory]\n");
	fprintf (hFp, "#   case=NUM          Compare=0 / ignore=1 case when filtering\n");
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

	fprintf (hFp, "# Local format:\n");
	fprintf (hFp, "#   group=STRING      Newsgroup (e.g. alt.flame)  [mandatory]\n");
	fprintf (hFp, "#   type=NUM          0=kill 1=auto-select (hot) [mandatory]\n");
	fprintf (hFp, "#   case=NUM          Compare=0 / ignore=1 case when filtering\n");
	fprintf (hFp, "#   subj=STRING       Subject: line\n");
	fprintf (hFp, "#   from=STRING       From: line\n");
	fprintf (hFp, "#   msgid=STRING      Message-ID: line\n");
	fprintf (hFp, "#   lines=NUM         Lines: line\n");
	fprintf (hFp, "#   xref_max=NUM      as above\n");
	fprintf (hFp, "#   xref_score=NUM,PATTERN\n");
	fprintf (hFp, "#   xref=PATTERN\n");
	fprintf (hFp, "#   time=NUM          Filter period in days\n#\n");
	fprintf (hFp, "#   type=...\n\n");
	fflush (hFp);

	/*
	 * Save global filters
	 */
	vWriteFilterArray (hFp, TRUE, &glob_filter, lCurTime);

	/*
	 * Save local filters for each group
	 */
	for (i = 0 ; i < num_active ; i++) {
		psFilter = active[i].grps_filter;
		if (psFilter != (struct t_filters *) 0 && psFilter->num) {
/*
my_printf ("Group=[%s]" cCRLF, active[i].name);
my_flush ();
*/
			fprintf (hFp, "\ngroup=%s\n", active[i].name);
			vWriteFilterArray (hFp, FALSE, psFilter, lCurTime);
		}
	}

	fclose (hFp);
	chmod (pcFile, (S_IRUSR|S_IWUSR));
}

#endif/* !INDEX_DAEMON */

/* do NOT sort the filterfile, cause sorting should be left to the user,
   so he could do a kill and then a mark hot (usefull in binaries groups
   to eleminate all postings smaller than f.e. 10-lines)

scope=*binar*
type=0
case=1
lines=<10
scope=*binar*
type=1
case=1
subj=*xv*

should work - but didn't with changing order of filterfile
*/

#ifndef INDEX_DAEMON
static void
vWriteFilterArray (
	FILE *fp,
	int global,
	struct t_filters *ptr,
	long theTime)
{
	register int i;
	int j;

	if (ptr == (struct t_filters *) 0) {
		return;
	}

	for (i = 0 ; i < ptr->num ; i++) {

/* my_printf ("WRITE i=[%d] subj=[%s] from=[%s]\n", i, (ptr->filter[i].subj ? ptr->filter[i].subj : ""), (ptr->filter[i].from ? ptr->filter[i].from : "")); */

		if (theTime && ptr->filter[i].time) {
			if (theTime > ptr->filter[i].time) {
				continue;
			}
		}

		if (global) {
/*
my_printf ("Scope=[%s]" cCRLF,
(ptr->filter[i].scope != (char *) 0 ? ptr->filter[i].scope : "*"));
my_flush ();
*/
				fprintf (fp, "scope=%s\n",
					(ptr->filter[i].scope != (char *) 0 ? ptr->filter[i].scope : "*"));
			}
/*
my_printf ("PtrType=[%d] FilType=[%d]" cCRLF, ptr->filter[i].type, write_filter_type);
my_flush ();
*/
			fprintf (fp, "type=%d\n", ptr->filter[i].type);
			fprintf (fp, "case=%d\n", ptr->filter[i].icase);

			if (ptr->filter[i].subj != (char *) 0) {
				fprintf (fp, "subj=%s\n", ptr->filter[i].subj);
			}
			if (ptr->filter[i].from != (char *) 0) {
				fprintf (fp, "from=%s\n", ptr->filter[i].from);
			}
			if (ptr->filter[i].msgid != (char *) 0) {
				fprintf (fp, "msgid=%s\n", ptr->filter[i].msgid);
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
			if (ptr->filter[i].xref != NULL) {
				fprintf (fp, "xref=%s\n", ptr->filter[i].xref);
			}
			if (ptr->filter[i].xref_max > 0) {
				fprintf (fp, "xref_max=%d\n", ptr->filter[i].xref_max);
				
				for(j=0;j<ptr->filter[i].xref_score_cnt;j++) {
				  fprintf (fp, "xref_score=%d%s%s\n", ptr->filter[i].xref_scores[j], ptr->filter[i].xref_score_strings[j] ? "," : "", ptr->filter[i].xref_score_strings[j]);
				}
			}
			if (ptr->filter[i].time) {
				fprintf (fp, "time=%ld\n", ptr->filter[i].time);
		}
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

	if (help) {
		show_menu_help (help);
	}

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
	char *ptr;
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
	rule.global = FALSE;
	rule.lines_cmp = FILTER_LINES_NO;
	rule.lines_num = 0;
	rule.from_ok = FALSE;
	rule.lines_ok = FALSE;
	rule.msgid_ok = FALSE;
	rule.subj_ok = FALSE;
	rule.type = type;
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

	len = cCOLS - 30;

	sprintf (text_time, txt_time_default_days, default_filter_days);
	sprintf (text_subj, ptr_filter_subj, len, len, art->subject);
	if (art->name != (char *) 0) {
		sprintf (buf, "%s (%s)", art->from, art->name);
	} else {
		strcpy (buf, art->from);
	}
	sprintf (text_from, ptr_filter_from, len, len, buf);
	sprintf (text_msgid, ptr_filter_msgid, len, len, MSGID(art));

	ClearScreen ();

	center_line (0, TRUE, ptr_filter_menu);

	MoveCursor (INDEX_TOP, 0);
	my_printf ("%s" cCRLF cCRLF, ptr_filter_text);
	my_printf ("%s" cCRLF cCRLF cCRLF, txt_filter_text_type);
	my_printf ("%s" cCRLF cCRLF, text_subj);
	my_printf ("%s" cCRLF cCRLF, text_from);
	my_printf ("%s" cCRLF cCRLF cCRLF, text_msgid);
	my_printf ("%s" cCRLF cCRLF, ptr_filter_lines);
	my_printf ("%s" cCRLF cCRLF cCRLF, ptr_filter_time);
	my_printf ("%s%s", ptr_filter_scope, group->name);
	my_flush ();

	show_menu_help (txt_help_filter_text);

	if (!prompt_menu_string (INDEX_TOP, (int) strlen (ptr_filter_text), rule.text)) {
		return FALSE;
	}

	if (rule.text[0]) {
		i = get_choice (INDEX_TOP+2, txt_help_filter_text_type,
			       txt_filter_text_type,
			       txt_subj_line_only_case,
			       txt_subj_line_only,
			       txt_from_line_only_case,
			       txt_from_line_only,
			       txt_msgid_line_only);
		if (i == -1) {
			return FALSE;
		}
		rule.counter = i;
	}

	if (!rule.text[0]) {
		/*
		 * Subject:
		 */
		i = get_choice (INDEX_TOP+5, txt_help_filter_subj,
			        text_subj, txt_yes, txt_no,
				(char *)0, (char *)0, (char *)0);
		if (i == -1) {
			return FALSE;
		} else {
			rule.subj_ok = (i ? FALSE : TRUE);
		}
		/*
		 * From:
		 */
		if (rule.subj_ok) {
			i = get_choice (INDEX_TOP+7, txt_help_filter_from,
			        text_from, txt_no, txt_yes,
				(char *)0, (char *)0, (char *)0);
		} else {
			i = get_choice (INDEX_TOP+7, txt_help_filter_from,
			        text_from, txt_yes, txt_no,
				(char *)0, (char *)0, (char *)0);
		}
		if (i == -1) {
			return FALSE;
		} else {
			if (rule.subj_ok) {
				rule.from_ok = (i ? TRUE : FALSE);
			} else {
				rule.from_ok = (i ? FALSE : TRUE);
			}
		}
		/*
		 * Message-Id:
		 */
		if (rule.subj_ok || rule.from_ok) {
			i = get_choice (INDEX_TOP+9, txt_help_filter_msgid,
			        text_msgid, txt_no, txt_yes,
				(char *)0, (char *)0, (char *)0);
		} else {
			i = get_choice (INDEX_TOP+9, txt_help_filter_msgid,
			        text_msgid, txt_yes, txt_no,
				(char *)0, (char *)0, (char *)0);
		}
		if (i == -1) {
			return FALSE;
		} else {
			if (rule.subj_ok || rule.from_ok) {
				rule.msgid_ok = (i ? TRUE : FALSE);
			} else {
				rule.msgid_ok = (i ? FALSE : TRUE);
			}
		}
	}

	/*
	 * Lines:
	 */
	show_menu_help (txt_help_filter_lines);

	buf[0] = '\0';
	if (!prompt_menu_string (INDEX_TOP+12, (int) strlen (ptr_filter_lines), buf)) {
		return FALSE;
	}

	/*
	 * Get the < > sign if any for the lines rule
	 */
	ptr = buf;
	while (ptr && *ptr == ' ') {
		ptr++;
	}
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
	if (rule.lines_cmp != FILTER_LINES_NO && rule.lines_num >= 0) {
		rule.lines_ok = TRUE;
	}

	/*
	 * Expire time
	 */
	strcpy(double_time, "2x ");
	strcat(double_time, text_time);
	strcpy(quat_time, "4x ");
	strcat(quat_time, text_time);
	i = get_choice (INDEX_TOP+14, txt_help_filter_time,
			ptr_filter_time, txt_unlimited_time, text_time,
			double_time, quat_time, (char *)0);
	if (i == -1) {
		return FALSE;
	}
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
			} else {
				argv[3][0] = '\0';
			}
		} else {
			argv[2][0] = '\0';
		}

		i = get_choice (INDEX_TOP+17, ptr_filter_help_scope,
			       ptr_filter_scope,
			       (argv[0][0] ? argv[0] : (char *)0),
			       (argv[1][0] ? argv[1] : (char *)0),
			       (argv[2][0] ? argv[2] : (char *)0),
			       (argv[3][0] ? argv[3] : (char *)0),
			       (char *)0);
		if (i == -1) {
			return FALSE;
		}
		rule.global = (i == 0 ? FALSE : TRUE);
		if (rule.global) {
			strcpy (rule.scope, (i == 1 ? "*" : argv[i]));
		}
	} else {
		return FALSE;
	}

	forever {
		do {
			sprintf (msg, "%s%c", ptr_filter_quit_edit_save, ch_default);
			wait_message (msg);
			MoveCursor (cLINES, (int) strlen (ptr_filter_quit_edit_save));
			if ((ch = ReadCh ()) == '\r' || ch == '\n')
				ch = ch_default;
		} while (!strchr ("eqs\033", ch));
		switch (ch) {
		case iKeyFilterEdit:
			start_line_offset = 22;
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

	if (debug) {
		sprintf (msg, "KILL header=[%d] scope=[%s] expire=[%s] case=[%d]",
			group->attribute->quick_kill_header,
			(group->attribute->quick_kill_scope ?
				group->attribute->quick_kill_scope : ""),
			(group->attribute->quick_kill_expire ? "ON" : "OFF"),
			group->attribute->quick_kill_case);
		error_message ("%s", msg);
	}

	header = group->attribute->quick_kill_header;

	/*
	 * Setup rules
	 */
	rule.counter = 0;
	rule.scope[0] = '\0';
	if (group->attribute->quick_kill_scope) {
		strcpy (rule.scope, group->attribute->quick_kill_scope);
	}
	rule.global = (strchr (rule.scope, '*') ? TRUE : FALSE);
	rule.lines_cmp = FILTER_LINES_NO;
	rule.lines_num = 0;
	rule.lines_ok = (header == FILTER_LINES ? TRUE : FALSE);
	rule.msgid_ok = (header == FILTER_MSGID ? TRUE : FALSE);
	if (header == FILTER_FROM_CASE_SENSITIVE ||
	    header == FILTER_FROM_CASE_IGNORE) {
		rule.from_ok = TRUE;
	} else {
		rule.from_ok = FALSE;
	}
	if (header == FILTER_SUBJ_CASE_SENSITIVE ||
	    header == FILTER_SUBJ_CASE_IGNORE) {
		rule.subj_ok = TRUE;
	} else {
		rule.subj_ok = FALSE;
	}
	rule.text[0] = '\0';
	rule.type = FILTER_KILL;
	rule.icase = group->attribute->quick_kill_case;
	rule.expire_time = group->attribute->quick_kill_expire;
	rule.check_string = TRUE;

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

	if (debug) {
		sprintf (msg, "SELECT header=[%d] scope=[%s] expire=[%s] case=[%d]",
			group->attribute->quick_select_header,
			(group->attribute->quick_select_scope ?
				group->attribute->quick_select_scope : ""),
			(group->attribute->quick_select_expire ? "ON" : "OFF"),
			group->attribute->quick_select_case);
		error_message ("%s", msg);
	}

	header = group->attribute->quick_select_header;

	/*
	 * Setup rules
	 */
	rule.counter = 0;
	rule.scope[0] = '\0';
	if (group->attribute->quick_select_scope) {
		strcpy (rule.scope, group->attribute->quick_select_scope);
	}
	rule.global = (strchr (rule.scope, '*') ? TRUE : FALSE);
	rule.lines_cmp = FILTER_LINES_NO;
	rule.lines_num = 0;
	rule.lines_ok = (header == FILTER_LINES ? TRUE : FALSE);
	rule.msgid_ok = (header == FILTER_MSGID ? TRUE : FALSE);
	if (header == FILTER_FROM_CASE_SENSITIVE ||
	    header == FILTER_FROM_CASE_IGNORE) {
		rule.from_ok = TRUE;
	} else {
		rule.from_ok = FALSE;
	}
	if (header == FILTER_SUBJ_CASE_SENSITIVE ||
	    header == FILTER_SUBJ_CASE_IGNORE) {
		rule.subj_ok = TRUE;
	} else {
		rule.subj_ok = FALSE;
	}
	rule.text[0] = '\0';
	rule.type = FILTER_SELECT;
	rule.icase = group->attribute->quick_select_case;
	rule.expire_time = group->attribute->quick_select_expire;
	rule.check_string = TRUE;

	filtered = iAddFilterRule (group, art, &rule);

	return filtered;
}

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
		rule.global = FALSE;
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

		filtered = iAddFilterRule (group, &art, &rule);

		FreeIfNeeded(art.subject);

	}

	return filtered;
}

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

	if (psRule->global) {
		plNum = &glob_filter.num;
		plMax = &glob_filter.max;
		if (*plNum >= *plMax) {
			 glob_filter.filter = psExpandFilterArray (glob_filter.filter, plMax);
		}
		psPtr = glob_filter.filter;
	} else {
		if (psGrp->grps_filter == (struct t_filters *) 0) {
			psGrp->grps_filter =
				(struct t_filters *) my_malloc (sizeof (struct t_filters));
			psGrp->grps_filter->num = 0;
			psGrp->grps_filter->max = 0;
			psGrp->grps_filter->filter = (struct t_filter *) 0;
		}
		plNum = &psGrp->grps_filter->num;
		plMax = &psGrp->grps_filter->max;
		if (*plNum >= *plMax) {
			psGrp->grps_filter->filter  = psExpandFilterArray (psGrp->grps_filter->filter, plMax);
		}
		psPtr = psGrp->grps_filter->filter;
	}

	psPtr[*plNum].type = psRule->type;
	psPtr[*plNum].icase = FALSE;
	psPtr[*plNum].inscope = TRUE;
	psPtr[*plNum].scope = (char *) 0;
	psPtr[*plNum].subj = (char *) 0;
	psPtr[*plNum].from = (char *) 0;
	psPtr[*plNum].msgid = (char *) 0;
	psPtr[*plNum].lines_cmp = psRule->lines_cmp;
	psPtr[*plNum].lines_num = psRule->lines_num;
	psPtr[*plNum].xref = (char *) 0;
	psPtr[*plNum].xref_max = 0;
	psPtr[*plNum].xref_score_cnt = 0;

	if (psRule->global && (psRule->scope[0] != '*' && psRule->scope[1] != '\0')) {
		psPtr[*plNum].scope = my_strdup (psRule->scope);
	}

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
		sprintf (acBuf, "*%s*", psRule->text);
		if (psRule->check_string) {
			strcpy (acBuf, pcChkRegexStr (acBuf));
		}
		switch (psRule->counter) {
			case FILTER_SUBJ_CASE_IGNORE:
			case FILTER_SUBJ_CASE_SENSITIVE:
				psPtr[*plNum].subj = my_strdup (acBuf);
				if (psRule->counter == FILTER_SUBJ_CASE_IGNORE) {
					psPtr[*plNum].icase = TRUE;
				}
				break;
			case FILTER_FROM_CASE_IGNORE:
			case FILTER_FROM_CASE_SENSITIVE:
				psPtr[*plNum].from = my_strdup (acBuf);
				if (psRule->counter == FILTER_FROM_CASE_IGNORE) {
					psPtr[*plNum].icase = TRUE;
				}
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
			sprintf (acBuf, "*%s*", quote_wild(psArt->subject));
			if (psRule->check_string) {
				strcpy (acBuf, pcChkRegexStr (acBuf));
			}
			psPtr[*plNum].subj = my_strdup (acBuf);
		}
		if (psRule->from_ok) {
			if (psArt->name != (char *) 0) {
				sprintf (acBuf, "*%s (%s)*", psArt->from, psArt->name);
			} else {
				sprintf (acBuf, "*%s*", psArt->from);
			}
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
		if (debug) {
			sprintf (msg, "Glob=[%s] inscope=[%s] scope=[%s] typ=[%d] case=[%d] subj=[%s] from=[%s] msgid=[%s] line=[%d %d] time=[%ld]",
				(psRule->global ? "TRUE" : "FALSE"),
				(psPtr[*plNum-1].inscope ? "TRUE" : "FALSE"),
				(psRule->scope ? psRule->scope : ""),
				psPtr[*plNum-1].type, psPtr[*plNum-1].icase,
				(psPtr[*plNum-1].subj ? psPtr[*plNum-1].subj : ""),
				(psPtr[*plNum-1].from ? psPtr[*plNum-1].from : ""),
				(psPtr[*plNum-1].msgid ? psPtr[*plNum-1].msgid : ""),
				psPtr[*plNum-1].lines_cmp, psPtr[*plNum-1].lines_num,
				psPtr[*plNum-1].time);
			wait_message (msg);
			sleep (2);
		}
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
	struct t_filter *ptr;

	num_of_killed_arts = 0;
	num_of_selected_arts = 0;

	/*
	 * check if there are any global and/or local filter rules
	 */
	if (group->glob_filter->num == 0 &&
	    group->grps_filter == (struct t_filters *) 0) {
		return filtered;
	}

	/*
	 * Apply global filter first if it has any entries
	 */
	if (group->glob_filter->num) {
		/*
		 * Check if any scope rules are active for this group
		 * ie. group=comp.os.linux.help  scope=comp.os.linux.*
		 */
		inscope = set_filter_scope (group);
		if (!cmd_line) {
			sprintf (buf, txt_filter_global_rules, inscope, group->glob_filter->num);
			wait_message (buf);
		}
		num = group->glob_filter->num;
		ptr = group->glob_filter->filter;
		global_filter = TRUE;
	} else {
		if (!cmd_line) {
			sprintf (buf, txt_filter_local_rules, group->grps_filter->num);
			wait_message (buf);
		}
		num = group->grps_filter->num;
		ptr = group->grps_filter->filter;
		global_filter = FALSE;
	}

local_filter:	/* jumps back from end of for() loop to help speed */

	/*
	 * loop thru all arts applying global & local filtering rules
	 */
	for (i=0 ; i < top ; i++) {
/* TODO - this seems to be the crux of the kill_level stuff */
		if (IS_READ(i) || IS_SELECTED(i)) {
			continue;
		}

		for (j=0 ; j < num ; j++) {
			if (ptr[j].inscope) {
				/*
				 * Filter on Subject: line
				 */
				if (ptr[j].subj != (char *) 0) {
					if (REGEX_MATCH (arts[i].subject, ptr[j].subj, ptr[j].icase)) {
						SET_FILTER(group, i, j);
					}
				}
				/*
				 * Filter on From: line
				 */
				if (ptr[j].from != (char *) 0) {
					if (arts[i].name != (char *) 0) {
						sprintf (buf, "%s (%s)", arts[i].from, arts[i].name);
					} else {
						strcpy (buf, arts[i].from);
					}
					if (REGEX_MATCH (buf, ptr[j].from, ptr[j].icase)) {
						SET_FILTER(group, i, j);
					}
				}

				/*
				 * Filter on Message-ID: line
				 * Apply to Message-ID: & References: lines
				 * Case is important here
				 */
				if (ptr[j].msgid != (char *) 0) {

					struct t_article *art = &arts[i];
					char *refs;
/* TODO This is now rubbish - we can rewrite filtering on MsgID & Refs
 * TODO to traverse the references tree in *1* pass for the whole group
 * TODO Simply find the root art and SET_FILTER(every child)
 */
					if (REGEX_MATCH (REFS(art, refs), ptr[j].msgid, FALSE) ||
					    REGEX_MATCH (MSGID(art), ptr[j].msgid, FALSE)) {
						SET_FILTER(group, i, j);
					}
					
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
sprintf (msg, "FILTERED Lines arts[%d] == [%d]", arts[i].lines, ptr[j].lines_num);
wait_message (msg); sleep (1);
*/
								SET_FILTER(group, i, j);
							}
							break;
						case FILTER_LINES_LT:
							if (arts[i].lines < ptr[j].lines_num) {
/*
sprintf (msg, "FILTERED Lines arts[%d] < [%d]", arts[i].lines, ptr[j].lines_num);
wait_message (msg); sleep (1);
*/
								SET_FILTER(group, i, j);
							}
							break;
						case FILTER_LINES_GT:
							if (arts[i].lines > ptr[j].lines_num) {
/*
sprintf (msg, "FILTERED Lines arts[%d] > [%d]", arts[i].lines, ptr[j].lines_num);
wait_message (msg); sleep (1);
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
				if(arts[i].xref && *arts[i].xref!='\0'
#if 0 /* WHY?! */
				  /* don't do anything in *.test groups */
				   && strlen(group->name)>4 &&
				   strcmp(group->name+strlen(group->name)-5, ".test")!=0
#endif
				   ) {

				  if (ptr[j].xref_max > 0 || ptr[j].xref != (char*)0) {
				    char *s,*e;
				    int group_count;

				    s=arts[i].xref;

				    while(*s && !isspace(*s)) s++;
				    while(*s && isspace(*s)) s++;

				    group_count=0;

				    while(*s) {
				      e=s;
				      while(*e && *e!=':' && !isspace(*e)) e++;

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
					/* don't kill when we are actually in that group */
					/* Group names shouldn't be case sensitive in any case. Whatever */
					if(ptr[j].type != FILTER_KILL || strcmp(group->name, buf)!=0) {
					  if (REGEX_MATCH (buf, ptr[j].xref, ptr[j].icase)) {
					    group_count=-1;
					    break;
					  }
					}
				      }
  
				      s=e;
				      while(*s && !isspace(*s)) s++;
				      while(*s && isspace(*s)) s++;
				    }
				    if(group_count==-1 ||
				       group_count>ptr[j].xref_max) {
				      SET_FILTER(group, i, j);
				    }
				  }
				}

				if (IS_KILLED(i) || !filtered) {
					filtered = TRUE;
				}
			}
		}
	}

	/*
	 * Apply local filter only if no global filter
	 *
	 * goto was used here to stop the function call
	 * overhead for each and every article.
	 */
	if (group->glob_filter->num && global_filter &&
	    group->grps_filter != (struct t_filters *) 0 &&
	    group->grps_filter->num) {
		num = group->grps_filter->num;
		ptr = group->grps_filter->filter;
		global_filter = FALSE;
		if (!cmd_line) {
			sprintf (buf, txt_filter_local_rules, group->grps_filter->num);
			wait_message (buf);
		}
		goto local_filter;	/* begin of for() */
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

int
auto_select_articles (
	struct t_group *group)
{
	filter_articles (group);
	return (TRUE);
}


/* TODO can we use match_group_list() for better defining the scope= rule ? */
static int
set_filter_scope (
	struct t_group *group)
{
	int i, num, inscope;
	struct t_filter *ptr;

	num = group->glob_filter->num;
	ptr = group->glob_filter->filter;
	inscope = num;

	for (i = 0; i < num; i++) {
		ptr[i].inscope = TRUE;
		if (ptr[i].scope != (char *) 0) {
			if (!GROUP_MATCH (group->name, ptr[i].scope, TRUE)) {
				ptr[i].inscope = FALSE;
				inscope--;
			}
		}
	}

	return inscope;
}

static char *
pcChkRegexStr (
	char *pcStr)
{
	char *pcSrcPtr;
	char *pcDstPtr;
	static char acBuf[LEN];

	pcSrcPtr = pcStr;
	pcDstPtr = acBuf;

	while (*pcSrcPtr) {
		if (*pcSrcPtr == '[' || *pcSrcPtr == ']') {
			*pcDstPtr = '\\';
			pcDstPtr++;
		}
		*pcDstPtr = *pcSrcPtr;
		pcDstPtr++;
		pcSrcPtr++;
	}
	*pcDstPtr = '\0';

	return acBuf;
}
