/*
 *  Project   : tin - a Usenet reader
 *  Module    : attrib.c
 *  Author    : I. Lea
 *  Created   : 1993-12-01
 *  Updated   : 1997-12-20
 *  Notes     : Group attribute routines
 *  Copyright : (c) Copyright 1991-98 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"

#ifndef INDEX_DAEMON

/*
 * Defines used in setting attributes switch
 */

#define	ATTRIB_MAILDIR			0
#define	ATTRIB_SAVEDIR			1
#define	ATTRIB_SAVEFILE			2
#define	ATTRIB_ORGANIZATION		3
#define	ATTRIB_FROM			4
#define	ATTRIB_SIGFILE			5
#define	ATTRIB_FOLLOWUP_TO		6
#define	ATTRIB_PRINTER			7
#define	ATTRIB_AUTO_SELECT		8
#define	ATTRIB_AUTO_SAVE		9
#define	ATTRIB_BATCH_SAVE		10
#define	ATTRIB_DELETE_TMP_FILES		11
#define	ATTRIB_SHOW_ONLY_UNREAD		12
#define	ATTRIB_THREAD_ARTS		13
#define	ATTRIB_SHOW_AUTHOR		14
#define	ATTRIB_SORT_ART_TYPE		15
#define	ATTRIB_POST_PROC_TYPE		16
#define	ATTRIB_QUICK_KILL_HEADER	17
#define	ATTRIB_QUICK_KILL_SCOPE		18
#define	ATTRIB_QUICK_KILL_EXPIRE	19
#define	ATTRIB_QUICK_KILL_CASE		20
#define	ATTRIB_QUICK_SELECT_HEADER	21
#define	ATTRIB_QUICK_SELECT_SCOPE	22
#define	ATTRIB_QUICK_SELECT_EXPIRE	23
#define	ATTRIB_QUICK_SELECT_CASE	24
#define	ATTRIB_MAILING_LIST		25
#define	ATTRIB_X_HEADERS		26
#define	ATTRIB_X_BODY			27
#define	ATTRIB_AUTO_SAVE_MSG		28
#define	ATTRIB_X_COMMENT_TO		29
#define	ATTRIB_NEWS_QUOTE		30
#define	ATTRIB_QUOTE_CHARS		31

/*
 * Local prototypes
 */
static void set_attrib (struct t_group *psGrp, int type, const char *str, int num);
static void set_attrib_num (int type, char *scope, int num);
static void set_attrib_str (int type, char *scope, char *str);
static void set_default_attributes (struct t_attribute *psAttrib);

/*
 * global attributes
 */

struct t_attribute glob_attributes;

/*
 * Per group attributes
 */

static void
set_default_attributes (
	struct t_attribute *psAttrib)
{
	psAttrib->global = FALSE;	/* global/group specific */
	psAttrib->maildir = default_maildir;
	psAttrib->savedir = default_savedir;
	psAttrib->savefile = (char *) 0;
	psAttrib->sigfile = default_sigfile;
	psAttrib->organization = (default_organization ? default_organization : (char *) 0);
	psAttrib->from = (char *) 0;
	psAttrib->followup_to = (char *) 0;
	psAttrib->printer = default_printer;
	psAttrib->quick_kill_scope = (default_filter_kill_global ? my_strdup("*") : (char *) 0);
	psAttrib->quick_kill_header = default_filter_kill_header;
	psAttrib->quick_kill_case = default_filter_kill_case;
	psAttrib->quick_kill_expire = default_filter_kill_expire;
	psAttrib->quick_select_scope = (default_filter_select_global ? my_strdup("*") : (char *) 0);
	psAttrib->quick_select_header = default_filter_select_header;
	psAttrib->quick_select_case = default_filter_select_case;
	psAttrib->quick_select_expire = default_filter_select_expire;
	psAttrib->mailing_list = (char *) 0;
	psAttrib->x_headers = (char *) 0;
	psAttrib->x_body = (char *) 0;
	psAttrib->show_only_unread = default_show_only_unread;
	psAttrib->thread_arts = default_thread_arts;
	psAttrib->sort_art_type = default_sort_art_type;
	psAttrib->show_author = default_show_author;
	psAttrib->auto_save_msg = default_auto_save_msg;
	psAttrib->auto_save = default_auto_save;
	psAttrib->auto_select = FALSE;
	psAttrib->batch_save = default_batch_save;
	psAttrib->delete_tmp_files = FALSE;
	psAttrib->post_proc_type = default_post_proc_type;
	psAttrib->x_comment_to = FALSE;
	psAttrib->news_quote_format = news_quote_format;
	psAttrib->quote_chars = quote_chars;
}

/*
 *  Load global & local attributes into active[].attribute
 *
 *  attribute.maildir          = STRING
 *  attribute.savedir          = STRING
 *  attribute.savefile	       = STRING
 *  attribute.organization     = STRING
 *  attribute.from             = STRING
 *  attribute.sigfile          = STRING
 *  attribute.followup_to      = STRING
 *  attribute.printer          = STRING
 *  attribute.auto_select      = ON/OFF
 *  attribute.auto_save        = ON/OFF
 *  attribute.batch_save       = ON/OFF
 *  attribute.delete_tmp_files = ON/OFF
 *  attribute.show_only_unread = ON/OFF
 *  attribute.thread_arts      = NUM
 *	  0=none, 1=subj, 2=refs, 3=both
 *  attribute.show_author      = NUM
 *    0=none, 1=name, 2=addr, 3=both
 *  attribute.sort_art_type    = NUM
 *    0=none, 1=subj descend, 2=subj ascend
 *    3=from descend, 4=from ascend
 *    5=date descend, 6=date ascend
 *    7=score descend, 8=score ascend
 *  attribute.post_proc_type   = NUM
 *    0=none, 1=unshar, 2=uudecode
 *    3=uudecode & list zoo (unix) / lha (AmigaDOS) archive
 *    4=uudecode & extract zoo (unix) / lha (AmigaDOS) archive
 *    5=uudecode & list zip archive
 *    6=uudecode & extract zip archive
 *  attribute.quick_kill_header = NUM
 *    0=subj (case sensitive) 1=subj (ignore case)
 *    2=from (case sensitive) 3=from (ignore case)
 *    4=msgid 5=lines
 *  attribute.quick_kill_scope  = STRING
 *  attribute.quick_kill_expire = ON/OFF
 *  attribute.quick_kill_case   = ON/OFF
 *  attribute.quick_select_header = NUM
 *    0=subj (case sensitive) 1=subj (ignore case)
 *    2=from (case sensitive) 3=from (ignore case)
 *    4=msgid 5=lines
 *  attribute.quick_select_scope  = STRING
 *  attribute.quick_select_expire = ON/OFF
 *  attribute.quick_select_case   = ON/OFF
 *  attribute.mailing_list = STRING
 *  attribute.x_headers = STRING
 *  attribute.x_comment_to = ON/OFF
 *  attribute.news_quote_format = STRING
 *  attribute.quote_chars = STRING
 */

#define MATCH_BOOLEAN(pattern, type) \
	if (match_boolean (line, pattern, &flag)) { \
		set_attrib_num (type, scope, flag != FALSE); \
		break; \
	}
#define MATCH_INTEGER(pattern, type, maxlen) \
	if (match_integer (line, pattern, &num, maxlen)) { \
		set_attrib_num (type, scope, num); \
		break; \
	}
	/* FIXME: the code always modifies 'scope' -- is this right? */
#define MATCH_STRING(pattern, type) \
	if (match_string (line, pattern, buf, sizeof (buf))) { \
		set_attrib_str (type, scope, buf); \
		break; \
	}

void
read_attributes_file (
	char	*file,
	t_bool global_file)
{
	char buf[LEN];
	char line[LEN];
	char scope[LEN];
	FILE *fp;
	t_bool flag;
	int num;
	register int i;

	/*
	 * Initialize global attributes (default unless overridden)
	 */
	if (global_file) {
		set_default_attributes (&glob_attributes);
		glob_attributes.global = TRUE;
	}

	if ((fp = fopen (file, "r")) != (FILE *) 0) {
		if (INTERACTIVE)
			wait_message (0, txt_reading_attributes_file, (global_file) ? "global " : "");

		scope[0] = '\0';
		while (fgets (line, (int) sizeof (line), fp) != (char *) 0) {
			if (line[0] == '#' || line[0] == '\n')
				continue;

			switch(tolower((unsigned char)line[0])) {
			case 'a':
				MATCH_BOOLEAN (
					"auto_save=",
					ATTRIB_AUTO_SAVE);
				MATCH_BOOLEAN (
					"auto_save_msg=",
					ATTRIB_AUTO_SAVE_MSG);
				MATCH_BOOLEAN (
					"auto_select=",
					ATTRIB_AUTO_SELECT);
				break;
			case 'b':
				MATCH_BOOLEAN (
					"batch_save=",
					ATTRIB_BATCH_SAVE);
				break;
			case 'd':
				MATCH_BOOLEAN (
					"delete_tmp_files=",
					ATTRIB_DELETE_TMP_FILES);
				break;
			case 'f':
				MATCH_STRING (
					"followup_to=",
					ATTRIB_FOLLOWUP_TO);
				MATCH_STRING (
					"from=",
					ATTRIB_FROM);
				break;
			case 'm':
				MATCH_STRING (
					"maildir=",
					ATTRIB_MAILDIR);
				MATCH_STRING (
					"mailing_list=",
					ATTRIB_MAILING_LIST);
				break;
			case 'n':
				MATCH_STRING (
					"news_quote_format=",
					ATTRIB_NEWS_QUOTE);
				break;
			case 'o':
				MATCH_STRING (
					"organization=",
					ATTRIB_ORGANIZATION);
				break;
			case 'p':
				MATCH_STRING (
					"printer=",
					ATTRIB_PRINTER);
				MATCH_INTEGER (
					"post_proc_type=",
					ATTRIB_POST_PROC_TYPE,
					POST_PROC_UUD_EXT_ZIP);
				break;
			case 'q':
				MATCH_INTEGER (
					"quick_kill_header=",
					ATTRIB_QUICK_KILL_HEADER,
					FILTER_LINES);
				MATCH_STRING (
					"quick_kill_scope=",
					ATTRIB_QUICK_KILL_SCOPE);
				MATCH_BOOLEAN (
					"quick_kill_case=",
					ATTRIB_QUICK_KILL_CASE);
				MATCH_BOOLEAN (
					"quick_kill_expire=",
					ATTRIB_QUICK_KILL_EXPIRE);
				MATCH_INTEGER (
					"quick_select_header=",
					ATTRIB_QUICK_SELECT_HEADER,
					FILTER_LINES);
				MATCH_STRING (
					"quick_select_scope=",
					ATTRIB_QUICK_SELECT_SCOPE);
				MATCH_BOOLEAN (
					"quick_select_case=",
					ATTRIB_QUICK_SELECT_CASE);
				MATCH_BOOLEAN (
					"quick_select_expire=",
					ATTRIB_QUICK_SELECT_EXPIRE);
				if (match_string (line, "quote_chars=", buf, sizeof (buf))) {
					quote_dash_to_space (buf);
					set_attrib_str (ATTRIB_QUOTE_CHARS, scope, buf);
					break;
				}
				break;
			case 's':
				MATCH_STRING (
					"savedir=",
					ATTRIB_SAVEDIR);
				MATCH_STRING (
					"savefile=",
					ATTRIB_SAVEFILE);
				MATCH_STRING (
					"sigfile=",
					ATTRIB_SIGFILE);
				if (match_string (line, "scope=", scope, sizeof (scope)))
					break;

				MATCH_BOOLEAN (
					"show_only_unread=",
					ATTRIB_SHOW_ONLY_UNREAD);
				MATCH_INTEGER (
					"sort_art_type=",
					ATTRIB_SORT_ART_TYPE,
					SORT_BY_SCORE_ASCEND);
				MATCH_INTEGER (
					"show_author=",
					ATTRIB_SHOW_AUTHOR,
					SHOW_FROM_BOTH);
				break;
			case 't':
				MATCH_INTEGER (
					"thread_arts=",
					ATTRIB_THREAD_ARTS,
					THREAD_MAX);
				break;
			case 'x':
				MATCH_STRING (
					"x_headers=",
					ATTRIB_X_HEADERS);
				MATCH_STRING (
					"x_body=",
					ATTRIB_X_BODY);
				MATCH_BOOLEAN (
					"x_comment_to=",
					ATTRIB_X_COMMENT_TO);
				break;
			default:
				break;
			}
		}
		fclose (fp);
	}

	/*
	 * Now setup the rest of the groups to use the default attributes
	 */
	if (!global_file) {
		for (i = 0; i < num_active; i++) {
			if (active[i].attribute == (struct t_attribute *) 0)
				active[i].attribute = &glob_attributes;
		}
	}
/* debug_print_filter_attributes(); */

	if (INTERACTIVE2)
		wait_message (0, "\n");

}


static void
set_attrib_str (
	int type,
	char *scope,
	char *str)
{
	register int i;
	struct t_group *psGrp;

	if (scope != (char *) 0 && *scope != '\0') {
		/*
		 * Does scope refer to 1 or more groups (ie. regex) ?
		 */
		if (!strchr (scope, '*')) {
			psGrp = psGrpFind (scope);
			if (psGrp != (struct t_group *) 0) {
#ifdef DEBUG
				if (debug)
					my_printf ("GROUP=[%s] Type=[%2d] Str=[%s]\n", psGrp->name, type, str);
#endif /* DEBUG */
				set_attrib (psGrp, type, str, -1);
			}
		} else {
			for (i = 0; i < num_active; i++) {
				psGrp = &active[i];
/* TODO can we use match_group_list() here for better effect ? */
				if (GROUP_MATCH (psGrp->name, scope, TRUE)) {
#ifdef DEBUG
					if (debug)
						my_printf ("SCOPE=[%s] Group=[%s] Type=[%2d] Str=[%s]\n", scope, psGrp->name, type, str);
#endif /* DEBUG */
					set_attrib (psGrp, type, str, -1);
				}
			}
		}
	}
}


static void
set_attrib_num (
	int type,
	char *scope,
	int num)
{
	register int i;
	struct t_group *psGrp;

	if (scope != (char *) 0 && *scope != '\0') {
		/*
		 * Does scope refer to 1 or more groups (ie. regex) ?
		 */
		if (!strchr (scope, '*')) {
			psGrp = psGrpFind (scope);
			if (psGrp != (struct t_group *) 0) {
#ifdef DEBUG
				if (debug)
					my_printf ("GROUP=[%s] Type=[%2d] Num=[%d]\n", psGrp->name, type, num);
#endif /* DEBUG */
				set_attrib (psGrp, type, "", num);
			}
		} else {
			for (i = 0; i < num_active; i++) {
				psGrp = &active[i];
/* TODO use match_group_list() here too ? */
				if (GROUP_MATCH (psGrp->name, scope, TRUE)) {
#ifdef DEBUG
					if (debug)
						my_printf ("SCOPE=[%s] Group=[%s] Type=[%2d] Num=[%d]\n", scope, psGrp->name, type, num);
#endif /* DEBUG */
					set_attrib (psGrp, type, "", num);
				}
			}
		}
	}
}


static void
set_attrib (
	struct	t_group	*psGrp,
	int	type,
	const char *str,
	int	num)
{

	/*
	 * Setup attributes for this group
	 */
	if (psGrp->attribute == (struct t_attribute *) 0) {
		psGrp->attribute = (struct t_attribute *) my_malloc (sizeof (struct t_attribute));
		set_default_attributes (psGrp->attribute);
	}

	/*
	 * Now set the attribute for this group
	 */
	switch (type) {
		case ATTRIB_MAILDIR:
			psGrp->attribute->maildir = my_strdup (str);
			break;
		case ATTRIB_SAVEDIR:
			psGrp->attribute->savedir = my_strdup (str);
			break;
		case ATTRIB_SAVEFILE:
			psGrp->attribute->savefile = my_strdup (str);
			break;
		case ATTRIB_ORGANIZATION:
			psGrp->attribute->organization = my_strdup (str);
			break;
		case ATTRIB_FROM:
			psGrp->attribute->from = my_strdup (str);
			break;
		case ATTRIB_SIGFILE:
			psGrp->attribute->sigfile = my_strdup (str);
			break;
		case ATTRIB_FOLLOWUP_TO:
			psGrp->attribute->followup_to = my_strdup (str);
			break;
		case ATTRIB_PRINTER:
			psGrp->attribute->printer = my_strdup (str);
			break;
		case ATTRIB_AUTO_SELECT:
			psGrp->attribute->auto_select = num;
			break;
		case ATTRIB_AUTO_SAVE:
			psGrp->attribute->auto_save = num;
			break;
		case ATTRIB_AUTO_SAVE_MSG:
			psGrp->attribute->auto_save_msg = num;
			break;
		case ATTRIB_BATCH_SAVE:
			psGrp->attribute->batch_save = num;
			break;
		case ATTRIB_DELETE_TMP_FILES:
			psGrp->attribute->delete_tmp_files = num;
			break;
		case ATTRIB_SHOW_ONLY_UNREAD:
			psGrp->attribute->show_only_unread = num;
			break;
		case ATTRIB_THREAD_ARTS:
			psGrp->attribute->thread_arts = num;
			break;
		case ATTRIB_SHOW_AUTHOR:
			psGrp->attribute->show_author = num;
			break;
		case ATTRIB_SORT_ART_TYPE:
			psGrp->attribute->sort_art_type = num;
			break;
		case ATTRIB_POST_PROC_TYPE:
			psGrp->attribute->post_proc_type = num;
			break;
		case ATTRIB_QUICK_KILL_HEADER:
			psGrp->attribute->quick_kill_header = num;
			break;
		case ATTRIB_QUICK_KILL_SCOPE:
			psGrp->attribute->quick_kill_scope = my_strdup (str);
			break;
		case ATTRIB_QUICK_KILL_EXPIRE:
			psGrp->attribute->quick_kill_expire = num;
			break;
		case ATTRIB_QUICK_KILL_CASE:
			psGrp->attribute->quick_kill_case = num;
			break;
		case ATTRIB_QUICK_SELECT_HEADER:
			psGrp->attribute->quick_select_header = num;
			break;
		case ATTRIB_QUICK_SELECT_SCOPE:
			psGrp->attribute->quick_select_scope = my_strdup (str);
			break;
		case ATTRIB_QUICK_SELECT_EXPIRE:
			psGrp->attribute->quick_select_expire = num;
			break;
		case ATTRIB_QUICK_SELECT_CASE:
			psGrp->attribute->quick_select_case = num;
			break;
		case ATTRIB_MAILING_LIST:
			psGrp->attribute->mailing_list = my_strdup (str);
			break;
		case ATTRIB_X_HEADERS:
			psGrp->attribute->x_headers = my_strdup (str);
			break;
		case ATTRIB_X_BODY:
			psGrp->attribute->x_body = my_strdup (str);
			break;
		case ATTRIB_X_COMMENT_TO:
			psGrp->attribute->x_comment_to = num;
			break;
		case ATTRIB_NEWS_QUOTE:
			psGrp->attribute->news_quote_format = my_strdup (str);
			break;
		case ATTRIB_QUOTE_CHARS:
			psGrp->attribute->quote_chars = my_strdup (str);
			break;
		default:
			break;
	}

}


/*
 *  Save the group attributes from active[].attribute to ~/.tin/attributes
 */
void
write_attributes_file (
	char	*file)
{
	FILE *fp;
	char *file_tmp;
#	if 0
	register int i;
	struct t_group *psGrp;
#	endif /* 0 */

	/* alloc memory for tmp-filename */
	if ((file_tmp = (char *) my_malloc (strlen (file)+5)) == NULL) {
		wait_message (2, txt_out_of_memory2);
		return;
	}
	/* generate tmp-filename */
	sprintf (file_tmp, "%s.tmp", file);

	if ((fp = fopen (file_tmp, "w" FOPEN_OPTS)) == (FILE *) 0) {
		error_message (txt_filesystem_full_backup, ATTRIBUTES_FILE);
		free (file_tmp);	/* free memory for tmp-filename */
		return;
	}

	if (!cmd_line && INTERACTIVE)
		wait_message (0, txt_writing_attributes_file);

	/* FIXME - move strings to lang.c */
	fprintf (fp, "# Group attributes file for the TIN newsreader\n#\n");
	fprintf (fp, "#  scope=STRING (ie. alt.sources or *sources*) [mandatory]\n#\n");
	fprintf (fp, "#  maildir=STRING (ie. ~/Mail)\n");
	fprintf (fp, "#  savedir=STRING (ie. ~user/News)\n");
	fprintf (fp, "#  savefile=STRING (ie. =linux)\n");
	fprintf (fp, "#  organization=STRING (if beginning with '/' read from file)\n");
	fprintf (fp, "#  from=STRING (just append wanted From:-line, don't use quotes)\n");
	fprintf (fp, "#  sigfile=STRING (ie. $var/sig)\n");
	fprintf (fp, "#  followup_to=STRING\n");
	fprintf (fp, "#  printer=STRING\n");
	fprintf (fp, "#  auto_select=ON/OFF\n");
	fprintf (fp, "#  auto_save=ON/OFF\n");
	fprintf (fp, "#  auto_save_msg=ON/OFF\n");
	fprintf (fp, "#  batch_save=ON/OFF\n");
	fprintf (fp, "#  delete_tmp_files=ON/OFF\n");
	fprintf (fp, "#  show_only_unread=ON/OFF\n#\n");
	fprintf (fp, "#  thread_arts=NUM\n");
	fprintf (fp, "#    0=none, 1=subj, 2=refs, 3=both\n#\n");
	fprintf (fp, "#  show_author=NUM\n");
	fprintf (fp, "#    0=none, 1=name, 2=addr, 3=both\n#\n");
	fprintf (fp, "#  sort_art_type=NUM\n");
	fprintf (fp, "#    0=none, 1=subj descend, 2=subj ascend,\n");
	fprintf (fp, "#    3=from descend, 4=from ascend,\n");
	fprintf (fp, "#    5=date descend, 6=date ascend\n#\n");
	fprintf (fp, "#    7=score descend, 8=score ascend\n#\n");
	fprintf (fp, "#  post_proc_type=NUM\n");
	fprintf (fp, "#    0=none, 1=unshar, 2=uudecode,\n");
#	ifdef M_AMIGA
		fprintf (fp, "#    3=uudecode & list lha archive,\n");
		fprintf (fp, "#    4=uudecode & extract lha archive\n");
#	else
		fprintf (fp, "#    3=uudecode & list zoo archive,\n");
		fprintf (fp, "#    4=uudecode & extract zoo archive\n");
#	endif /* M_AMIGA */
	fprintf (fp, "#    5=uudecode & list zip archive,\n");
	fprintf (fp, "#    6=uudecode & extract zip archive\n#\n");
	fprintf (fp, "#  mailing_list=STRING (ie. majordomo@list.org)\n");
	fprintf (fp, "#  x_headers=STRING (ie. ~/.tin/extra-headers)\n");
	fprintf (fp, "#  x_body=STRING (ie. ~/.tin/extra-body-text)\n");
	fprintf (fp, "#  quick_kill_scope  = STRING (ie. talk.*)\n");
	fprintf (fp, "#  quick_kill_expire = ON/OFF\n");
	fprintf (fp, "#  quick_kill_case   = ON/OFF\n");
	fprintf (fp, "#  quick_kill_header = NUM\n");
	fprintf (fp, "#    0=subj (case sensitive) 1=subj (ignore case)\n");
	fprintf (fp, "#    2=from (case sensitive) 3=from (ignore case)\n");
	fprintf (fp, "#    4=msgid 5=lines\n#\n");
	fprintf (fp, "#  quick_select_scope  = STRING\n");
	fprintf (fp, "#  quick_select_expire = ON/OFF\n");
	fprintf (fp, "#  quick_select_case   = ON/OFF\n");
	fprintf (fp, "#  quick_select_header = NUM\n");
	fprintf (fp, "#    0=subj (case sensitive) 1=subj (ignore case)\n");
	fprintf (fp, "#    2=from (case sensitive) 3=from (ignore case)\n");
	fprintf (fp, "#    4=msgid 5=lines\n#\n");
	fprintf (fp, "#  x_comment_to=ON/OFF\n");
	fprintf (fp, "#  news_quote_format=STRING\n#\n");
	fprintf (fp, "#  quote_chars=STRING (%%s, %%S for initials)\n#\n");
	fprintf (fp, "# Note that it is best to put general (global scoping)\n");
	fprintf (fp, "# entries first followed by group specific entries.\n#\n");
	fprintf (fp, "############################################################################\n\n");

/*
 * some useful defaults
 */
	fprintf (fp, "# in *sources* set post process type to shar\n");
	fprintf (fp, "scope=*sources*\n");
	fprintf (fp, "post_proc_type=1\n\n");

	fprintf (fp, "# in *binaries* set post process type to uudecode, remove tmp files\n");
	fprintf (fp, "# and set Followup-To: poster\n");
	fprintf (fp, "scope=*binaries*\n");
	fprintf (fp, "post_proc_type=2\n");
	fprintf (fp, "delete_tmp_files=ON\n");
	fprintf (fp, "followup_to=poster\n\n");

#	if 0 /* FIXME */
	for (i = 0; i < num_active; i++) {
		psGrp = &active[i];
		fprintf (fp, "scope=%s\n", psGrp->name);
		fprintf (fp, "maildir=%s\n", psGrp->attribute->maildir);
		fprintf (fp, "savedir=%s\n", psGrp->attribute->savedir);
		fprintf (fp, "savefile=%s\n", psGrp->attribute->savefile);
		fprintf (fp, "organization=%s\n", psGrp->attribute->organization);
		fprintf (fp, "from=%s\n", psGrp->attribute->from);
		fprintf (fp, "sigfile=%s\n", psGrp->attribute->sigfile);
		fprintf (fp, "followup_to=%s\n", psGrp->attribute->followup_to);
		fprintf (fp, "printer=%s\n", psGrp->attribute->printer);
		fprintf (fp, "show_only_unread=%s\n",
			print_boolean (psGrp->attribute->show_only_unread));
		fprintf (fp, "thread_arts=%d\n", psGrp->attribute->thread_arts);
		fprintf (fp, "auto_select=%s\n",
			print_boolean (psGrp->attribute->auto_select));
		fprintf (fp, "auto_save=%s\n",
			print_boolean (psGrp->attribute->auto_save));
		fprintf (fp, "auto_save_msg=%s\n",
			print_boolean (psGrp->attribute->auto_save_msg));
		fprintf (fp, "batch_save=%s\n",
			print_boolean (psGrp->attribute->batch_save));
		fprintf (fp, "delete_tmp_files=%s\n",
			print_boolean (psGrp->attribute->delete_tmp_files));
		fprintf (fp, "sort_art_type=%d\n", psGrp->attribute->sort_art_type);
		fprintf (fp, "show_author=%d\n", psGrp->attribute->show_author);
		fprintf (fp, "post_proc_type=%d\n", psGrp->attribute->post_proc_type);
		fprintf (fp, "quick_kill_scope=%s\n",
			psGrp->attribute->quick_kill_scope);
		fprintf (fp, "quick_kill_case=%s\n",
			print_boolean (psGrp->attribute->quick_kill_case));
		fprintf (fp, "quick_kill_expire=%s\n",
			print_boolean (psGrp->attribute->quick_kill_expire));
		fprintf (fp, "quick_kill_header=%d\n", psGrp->attribute->quick_kill_header);
		fprintf (fp, "quick_select_scope=%s\n",
			psGrp->attribute->quick_select_scope);
		fprintf (fp, "quick_select_case=%s\n",
			print_boolean (psGrp->attribute->quick_select_case));
		fprintf (fp, "quick_select_expire=%s\n",
			print_boolean (psGrp->attribute->quick_select_expire));
		fprintf (fp, "quick_select_header=%d\n\n", psGrp->attribute->quick_select_header);
		fprintf (fp, "mailing_list=%s\n", psGrp->attribute->mailing_list);
		fprintf (fp, "x_headers=%s\n", psGrp->attribute->x_headers);
		fprintf (fp, "x_body=%s\n", psGrp->attribute->x_body);
		fprintf (fp, "x_comment_to=%s\n",
			print_boolean (psGrp->attribute->x_comment_to));
		fprintf (fp, "news_quote_format=%s\n",
			psGrp->attribute->news_quote_format);
		fprintf (fp, "quote_chars=%s\n",
			quote_space_to_dash (psGrp->attribute->quote_chars));
	}
#	endif /* 0 */

	if (ferror (fp) | fclose (fp))
		error_message (txt_filesystem_full, ATTRIBUTES_FILE);
	else {
		rename_file (file_tmp, file);
		chmod (file, (mode_t)(S_IRUSR|S_IWUSR));
	}
	free (file_tmp);	/* free memory for tmp-filename */
}


#	if 0
void
debug_print_filter_attributes (void)
{
	register int i;
	struct t_group *psGrp;

	my_printf("\nBEG ***\n");

	for (i = 0; i < num_active; i++) {
		psGrp = &active[i];
		my_printf ("Grp=[%s] KILL   header=[%d] scope=[%s] case=[%s] expire=[%s]\n",
			psGrp->name, psGrp->attribute->quick_kill_header,
			(psGrp->attribute->quick_kill_scope ?
				psGrp->attribute->quick_kill_scope : ""),
			(psGrp->attribute->quick_kill_case ? "ON" : "OFF"),
			(psGrp->attribute->quick_kill_expire ? "ON" : "OFF"));
		my_printf ("Grp=[%s] SELECT header=[%d] scope=[%s] case=[%s] expire=[%s]\n",
			psGrp->name, psGrp->attribute->quick_select_header,
			(psGrp->attribute->quick_select_scope ?
				psGrp->attribute->quick_select_scope: ""),
			(psGrp->attribute->quick_select_case ? "ON" : "OFF"),
			(psGrp->attribute->quick_select_expire ? "ON" : "OFF"));
	}

	my_printf("END ***\n");
}
#	endif /* 0 */

#endif /* !INDEX_DAEMON */
