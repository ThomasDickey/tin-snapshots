/*
 *  Project   : tin - a Usenet reader
 *  Module    : attrib.c
 *  Author    : I.Lea
 *  Created   : 01-12-93
 *  Updated   : 21-11-94
 *  Notes     : Group attribute routines
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

/*
 * Defines used in setting attributes switch
 */
 
#define	ATTRIB_MAILDIR			0
#define	ATTRIB_SAVEDIR			1
#define	ATTRIB_SAVEFILE			2
#define	ATTRIB_ORGANIZATION		3
#define	ATTRIB_SIGFILE			4
#define	ATTRIB_FOLLOWUP_TO		5
#define	ATTRIB_PRINTER			6
#define	ATTRIB_AUTO_SELECT		7
#define	ATTRIB_AUTO_SAVE		8
#define	ATTRIB_BATCH_SAVE		9
#define	ATTRIB_DELETE_TMP_FILES		10
#define	ATTRIB_SHOW_ONLY_UNREAD		11
#define	ATTRIB_THREAD_ARTS		12
#define	ATTRIB_SHOW_AUTHOR		13
#define	ATTRIB_SORT_ART_TYPE		14
#define	ATTRIB_POST_PROC_TYPE		15
#define	ATTRIB_QUICK_KILL_HEADER	16
#define	ATTRIB_QUICK_KILL_SCOPE		17
#define	ATTRIB_QUICK_KILL_EXPIRE	18
#define	ATTRIB_QUICK_KILL_CASE  	19
#define	ATTRIB_QUICK_SELECT_HEADER	20
#define	ATTRIB_QUICK_SELECT_SCOPE	21
#define	ATTRIB_QUICK_SELECT_EXPIRE	22
#define	ATTRIB_QUICK_SELECT_CASE  	23
#define	ATTRIB_MAILING_LIST		24
#define	ATTRIB_X_HEADERS		25
#define	ATTRIB_X_BODY			26
#define	ATTRIB_AUTO_SAVE_MSG		27
#define ATTRIB_X_COMMENT_TO		28
#define ATTRIB_NEWS_QUOTE		29

/*
 * global attributes	
 */

struct t_attribute glob_attributes;

/*
 * Per group attributes
 */
 
void
set_default_attributes (psAttrib)
	struct t_attribute *psAttrib;
{
#ifndef INDEX_DAEMON

	psAttrib->global = FALSE;	/* global/group specific */
	psAttrib->maildir = default_maildir;
	psAttrib->savedir = default_savedir;
	psAttrib->savefile = (char *) 0;
	psAttrib->sigfile = default_sigfile;
	psAttrib->organization = 
		(default_organization[0] ? default_organization : (char *) 0);
	psAttrib->followup_to = (char *) 0;
	psAttrib->printer = default_printer;
	psAttrib->quick_kill_scope = (char *) 0;
	psAttrib->quick_kill_header = default_filter_kill_header;
	psAttrib->quick_kill_case = default_filter_kill_case;
	psAttrib->quick_kill_expire = default_filter_kill_expire;
	psAttrib->quick_select_scope = (char *) 0;
	psAttrib->quick_select_header = default_filter_select_header;
	psAttrib->quick_select_case = default_filter_select_case;
	psAttrib->quick_select_expire = default_filter_select_expire;
	psAttrib->mailing_list = (char *) 0;
	psAttrib->x_headers = (char *) 0;
	psAttrib->x_body = (char *) 0;
	psAttrib->show_only_unread = default_show_only_unread;
	psAttrib->thread_arts = default_thread_arts;
	psAttrib->sort_art_type = default_sort_art_type;
	psAttrib->show_author  = default_show_author;
	psAttrib->auto_save_msg = default_auto_save_msg;
	psAttrib->auto_save = default_auto_save;
	psAttrib->auto_select = FALSE;
	psAttrib->batch_save = default_batch_save;
	psAttrib->delete_tmp_files = FALSE;
	psAttrib->post_proc_type = default_post_proc_type;
	psAttrib->x_comment_to = FALSE;
	psAttrib->news_quote_format = news_quote_format;

#endif	/* INDEX_DAEMON */
}

/*
 *  Load global & local attributes into active[].attribute
 *
 *  attribute.maildir          = STRING
 *  attribute.savedir          = STRING
 *  attribute.savefile	       = STRING
 *  attribute.organization     = STRING
 *  attribute.sigfile          = STRING
 *  attribute.followup_to      = STRING
 *  attribute.printer          = STRING
 *  attribute.auto_select      = ON/OFF
 *  attribute.auto_save        = ON/OFF
 *  attribute.batch_save       = ON/OFF
 *  attribute.delete_tmp_files = ON/OFF
 *  attribute.show_only_unread = ON/OFF
 *  attribute.thread_arts      = NUM
 *	  0=none, 1=subj, 2=refs
 *  attribute.show_author      = NUM
 *    0=none, 1=name, 2=addr, 3=both
 *  attribute.sort_art_type    = NUM
 *    0=none, 1=subj descend, 2=subj ascend
 *    3=from descend, 4=from ascend
 *    5=date descend, 6=date ascend
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
 */

void
read_attributes_file (file, global_file)
	char	*file;
	int		global_file;
{
#ifndef INDEX_DAEMON

	char buf[LEN];
	char line[LEN];
	char scope[LEN];
	FILE *fp;
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
		if ((update && update_fork) || ! update) {
			if (global_file) {
				wait_message (txt_reading_global_attributes_file);
			} else {
				wait_message (txt_reading_attributes_file);
			}
	}
	
		scope[0] = '\0';
		while (fgets (line, sizeof (line), fp) != (char *) 0) {
			if (line[0] == '#' || line[0] == '\n') {
				continue;
			}
			switch(tolower(line[0])) {
			case 'a':
				if (match_boolean (line, "auto_select=", &num)) {
					set_attrib_num (ATTRIB_AUTO_SELECT, scope, num);
					break;
				}
				if (match_boolean (line, "auto_save=", &num)) {
					set_attrib_num (ATTRIB_AUTO_SAVE, scope, num);
					break;
				}
				if (match_boolean (line, "auto_save_msg=", &num)) {
					set_attrib_num (ATTRIB_AUTO_SAVE_MSG, scope, num);
					break;
				}
				break;
			case 'b':
				if (match_boolean (line, "batch_save=", &num)) {
					set_attrib_num (ATTRIB_BATCH_SAVE, scope, num);
					break;
				}
				break;
			case 'd':
				if (match_boolean (line, "delete_tmp_files=", &num)) {
					set_attrib_num (ATTRIB_DELETE_TMP_FILES, scope, num);
					break;
				}
				break;
			case 'f':
				if (match_string (line, "followup_to=", buf, sizeof (buf))) {
					set_attrib_str (ATTRIB_FOLLOWUP_TO, scope, buf);
					break;
				}
				break;
			case 'm':
				if (match_string (line, "maildir=", buf, sizeof (buf))) {
					set_attrib_str (ATTRIB_MAILDIR, scope, buf);
					break;
				}
				if (match_string (line, "mailing_list=", buf, sizeof (buf))) {
					set_attrib_str (ATTRIB_MAILING_LIST, scope, buf);
					break;
				}
				break;
			case 'n':
				if (match_string (line, "news_quote_format=", buf, sizeof (buf))) {
					set_attrib_str (ATTRIB_NEWS_QUOTE, scope, buf);
					break;
				}
			case 'o':
				if (match_string (line, "organization=", buf, sizeof (buf))) {
					set_attrib_str (ATTRIB_ORGANIZATION, scope, buf);
					break;
				}
				break;
			case 'p':
				if (match_string (line, "printer=", buf, sizeof (buf))) {
					set_attrib_str (ATTRIB_PRINTER, scope, buf);
					break;
				}
				if (match_integer (line, "post_proc_type=", &num, POST_PROC_UUD_EXT_ZIP)) {
					set_attrib_num (ATTRIB_POST_PROC_TYPE, scope, num);
					break;
				}
				break;
			case 'q':
				if (match_integer (line, "quick_kill_header=", &num, FILTER_LINES)) {
					set_attrib_num (ATTRIB_QUICK_KILL_HEADER, scope, num);
					break;
					}
				if (match_string (line, "quick_kill_scope=", buf, sizeof (buf))) {
					set_attrib_str (ATTRIB_QUICK_KILL_SCOPE, scope, buf);
					break;
				}
				if (match_boolean (line, "quick_kill_case=", &num)) {
					set_attrib_num (ATTRIB_QUICK_KILL_CASE, scope, num);
					break;
				}
				if (match_boolean (line, "quick_kill_expire=", &num)) {
					set_attrib_num (ATTRIB_QUICK_KILL_EXPIRE, scope, num);
					break;
				}
				if (match_integer (line, "quick_select_header=", &num, FILTER_LINES)) {
					set_attrib_num (ATTRIB_QUICK_SELECT_HEADER, scope, num);
					break;
				}
				if (match_string (line, "quick_select_scope=", buf, sizeof (buf))) {
					set_attrib_str (ATTRIB_QUICK_SELECT_SCOPE, scope, buf);
					break;
				}
				if (match_boolean (line, "quick_select_case=", &num)) {
					set_attrib_num (ATTRIB_QUICK_SELECT_CASE, scope, num);
					break;
				}
				if (match_boolean (line, "quick_select_expire=", &num)) {
					set_attrib_num (ATTRIB_QUICK_SELECT_EXPIRE, scope, num);
					break;
				}
				break;
			case 's':
				if (match_string (line, "savedir=", buf, sizeof (buf))) {
					set_attrib_str (ATTRIB_SAVEDIR, scope, buf);
					break;
				}
				if (match_string (line, "savefile=", buf, sizeof (buf))) {
					set_attrib_str (ATTRIB_SAVEFILE, scope, buf);
					break;
				}
				if (match_string (line, "sigfile=", buf, sizeof (buf))) {
					set_attrib_str (ATTRIB_SIGFILE, scope, buf);
					break;
				}
				if (match_string (line, "scope=", scope, sizeof (scope))) {
					break;
				}
				if (match_boolean (line, "show_only_unread=", &num)) {
					set_attrib_num (ATTRIB_SHOW_ONLY_UNREAD, scope, num);
					break;
				}
				if (match_integer (line, "sort_art_type=", &num, SORT_BY_DATE_ASCEND)) {
					set_attrib_num (ATTRIB_SORT_ART_TYPE, scope, num);
					break;
				}
				if (match_integer (line, "show_author=", &num, SHOW_FROM_BOTH)) {
					set_attrib_num (ATTRIB_SHOW_AUTHOR, scope, num);
					break;
				}
				break;
			case 't':
				if (match_integer (line, "thread_arts=", &num, THREAD_REFS)) {
					set_attrib_num (ATTRIB_THREAD_ARTS, scope, num);
					break;
				}
				break;
			case 'x':
				if (match_string (line, "x_headers=", buf, sizeof (buf))) {
					set_attrib_str (ATTRIB_X_HEADERS, scope, buf);
					break;
				}
				if (match_string (line, "x_body=", buf, sizeof (buf))) {
					set_attrib_str (ATTRIB_X_BODY, scope, buf);
					break;
				}
				if (match_boolean (line, "x_comment_to=", &num)) {
					set_attrib_num (ATTRIB_X_COMMENT_TO, scope, num);
					break;
				}
				break;
			}
		}
		fclose (fp);
	}
	
	/*
	 * Now setup the rest of the groups to use the default attributes
	 */
	if (! global_file) {
		for (i = 0; i < num_active ; i++) {
			if (active[i].attribute == (struct t_attribute *) 0) {
				active[i].attribute = &glob_attributes;
			}
		}
	}
/* debug_print_filter_attributes(); */

	if ((cmd_line && ! update && ! verbose) || (update && update_fork)) {
		wait_message ("\n");
	}

#endif	/* INDEX_DAEMON */
}


void
set_attrib_str (type, scope, str)
	int		type;
	char	*scope;
	char	*str;
{
#ifndef INDEX_DAEMON
	register int i;
	struct t_group *psGrp;

	if (scope != (char *) 0 && *scope != '\0') {
		/*
		 * Does scope refer to 1 or more groups (ie. regex) ?
		 */
		if (! strchr (scope, '*')) {
			psGrp = psGrpFind (scope);
			if (psGrp != (struct t_group *) 0) { 	
if (debug) {
	printf ("GROUP=[%s] Type=[%2d] Str=[%s]\n", psGrp->name, type, str);
}
				set_attrib (psGrp, type, str, -1);
			}
		} else {
			for (i = 0; i < num_active; i++) {
				psGrp = &active[i];
				if (wildmat (psGrp->name, scope)) {
if (debug) {
	printf ("SCOPE=[%s] Group=[%s] Type=[%2d] Str=[%s]\n", scope, psGrp->name, type, str);
}
					set_attrib (psGrp, type, str, -1);
				}
			}
		}
	}
#endif	/* INDEX_DAEMON */
}


void
set_attrib_num (type, scope, num)
	int		type;
	char	*scope;
	int		num;
{
#ifndef INDEX_DAEMON
	register int i;
	struct t_group *psGrp;

	if (scope != (char *) 0 && *scope != '\0') {
		/*
		 * Does scope refer to 1 or more groups (ie. regex) ?
		 */
		if (! strchr (scope, '*')) {
			psGrp = psGrpFind (scope);
			if (psGrp != (struct t_group *) 0) { 	
if (debug) {
	printf ("GROUP=[%s] Type=[%2d] Num=[%d]\n", psGrp->name, type, num);
}
				set_attrib (psGrp, type, "", num);
			}
		} else {
			for (i = 0; i < num_active; i++) {
				psGrp = &active[i];
				if (wildmat (psGrp->name, scope)) {
if (debug) {
	printf ("SCOPE=[%s] Group=[%s] Type=[%2d] Num=[%d]\n", scope, psGrp->name, type, num);
}
					set_attrib (psGrp, type, "", num);
				}
			}
		}
	}
#endif	/* INDEX_DAEMON */
}

void
set_attrib (psGrp, type, str, num)
	struct	t_group	*psGrp;
	int		type;
	char	*str;
	int		num;
{
#ifndef INDEX_DAEMON

	/*
	 * Setup attributes for this group
	 */
	if (psGrp->attribute == (struct t_attribute *) 0) {
		psGrp->attribute = 
			(struct t_attribute *) my_malloc (sizeof (struct t_attribute));
		set_default_attributes (psGrp->attribute);
	}

	/*
	 * Now set the attribute for this group 
	 */	 	
	switch (type) {
		case ATTRIB_MAILDIR:
			psGrp->attribute->maildir = str_dup (str);
			break;
		case ATTRIB_SAVEDIR:
			psGrp->attribute->savedir = str_dup (str);
			break;
		case ATTRIB_SAVEFILE:
			psGrp->attribute->savefile = str_dup (str);
			break;
		case ATTRIB_ORGANIZATION:
			psGrp->attribute->organization = str_dup (str);
			break;
		case ATTRIB_SIGFILE:
			psGrp->attribute->sigfile = str_dup (str);
			break;
		case ATTRIB_FOLLOWUP_TO:
			psGrp->attribute->followup_to = str_dup (str);
			break;
		case ATTRIB_PRINTER:
			psGrp->attribute->printer = str_dup (str);
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
			psGrp->attribute->quick_kill_scope = str_dup (str);
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
			psGrp->attribute->quick_select_scope = str_dup (str);
			break;
		case ATTRIB_QUICK_SELECT_EXPIRE:
			psGrp->attribute->quick_select_expire = num;
			break;
		case ATTRIB_QUICK_SELECT_CASE:
			psGrp->attribute->quick_select_case = num;
			break;
		case ATTRIB_MAILING_LIST:
			psGrp->attribute->mailing_list = str_dup (str);
			break;
		case ATTRIB_X_HEADERS:
			psGrp->attribute->x_headers = str_dup (str);
			break;
		case ATTRIB_X_BODY:
			psGrp->attribute->x_body = str_dup (str);
			break;
		case ATTRIB_X_COMMENT_TO:
			psGrp->attribute->x_comment_to = num;
			break;
		case ATTRIB_NEWS_QUOTE:
			psGrp->attribute->news_quote_format = str_dup (str);
			break;
		default:
			break;
	}
	
#endif	/* INDEX_DAEMON */
}

/*
 *  Save the group attributes from active[].attribute to ~/.tin/attributes 
 */

void
write_attributes_file (file)
	char	*file;
{
#ifndef INDEX_DAEMON
	FILE *fp;
	register int i;
	struct t_group *psGrp;

#ifdef VMS
	if ((fp = fopen (file, "w", "fop=cif")) == (FILE *) 0) {
#else
	if ((fp = fopen (file, "w")) == (FILE *) 0) {
#endif
		return;
	}

	if (! cmd_line) {
		if ((update && update_fork) || ! update) {
			wait_message (txt_writing_attributes_file);
		}
	}

	fprintf (fp, "# Group attributes file for the TIN newsreader\n#\n");
	fprintf (fp, "#  scope=STRING (ie. alt.sources or *sources*) [mandatory]\n#\n");
	fprintf (fp, "#  maildir=STRING (ie. ~/Mail)\n");
	fprintf (fp, "#  savedir=STRING (ie. ~user/News)\n");
	fprintf (fp, "#  savefile=STRING (ie. =linux)\n");
	fprintf (fp, "#  organization=STRING\n");
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
	fprintf (fp, "#    0=none, 1=subj, 2=refs\n#\n");
	fprintf (fp, "#  show_author=NUM\n");
	fprintf (fp, "#    0=none, 1=name, 2=addr, 3=both\n#\n");
	fprintf (fp, "#  sort_art_type=NUM\n");
	fprintf (fp, "#    0=none, 1=subj descend, 2=subj ascend,\n"); 
	fprintf (fp, "#    3=from descend, 4=from ascend,\n");
	fprintf (fp, "#    5=date descend, 6=date ascend\n#\n");
	fprintf (fp, "#  post_proc_type=NUM\n");
	fprintf (fp, "#    0=none, 1=unshar, 2=uudecode,\n"); 
#ifdef M_AMIGA
	fprintf (fp, "#    3=uudecode & list lha archive,\n"); 
	fprintf (fp, "#    4=uudecode & extract lha archive\n");
#else
	fprintf (fp, "#    3=uudecode & list zoo archive,\n"); 
	fprintf (fp, "#    4=uudecode & extract zoo archive\n");
#endif
	fprintf (fp, "#    5=uudecode & list zip archive,\n"); 
	fprintf (fp, "#    6=uudecode & extract zip archive\n#\n");
	fprintf (fp, "#  mailing_list=STRING (ie. majordomo@list.org)\n");
	fprintf (fp, "#  x_headers=STRING (ie. ~/.tin/extra-headers)\n");
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
	fprintf (fp, "# Note that it is best to put general (global scoping)\n");
	fprintf (fp, "# entries first followed by group specific entries.\n\n");

	for (i = 0 ; i < num_active ; i++) {
		psGrp = &active[i];
		fprintf (fp, "scope=%s\n", psGrp->name);
		fprintf (fp, "maildir=%s\n", psGrp->attribute->maildir);
		fprintf (fp, "savedir=%s\n", psGrp->attribute->savedir);
		fprintf (fp, "savefile=%s\n", psGrp->attribute->savefile);
		fprintf (fp, "organization=%s\n", psGrp->attribute->organization);
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
		fprintf (fp, "news_quote_format=%s\n", psGrp->attribute->news_quote_format);
	}

	fclose (fp);

#endif	/* INDEX_DAEMON */
}


void
debug_print_filter_attributes ()
{
#ifndef INDEX_DAEMON
	register int i;
	struct t_group *psGrp;
		
	printf("\nBEG ***\n");

	for (i = 0; i < num_active ; i++) {
		psGrp = &active[i];
		printf ("Grp=[%s] KILL   header=[%d] scope=[%s] case=[%s] expire=[%s]\n",
			psGrp->name, psGrp->attribute->quick_kill_header,
			(psGrp->attribute->quick_kill_scope ? 
				psGrp->attribute->quick_kill_scope : ""),
			(psGrp->attribute->quick_kill_case ? "ON" : "OFF"),
			(psGrp->attribute->quick_kill_expire ? "ON" : "OFF"));
		printf ("Grp=[%s] SELECT header=[%d] scope=[%s] case=[%s] expire=[%s]\n",
			psGrp->name, psGrp->attribute->quick_select_header,
			(psGrp->attribute->quick_select_scope ? 
				psGrp->attribute->quick_select_scope: ""),
			(psGrp->attribute->quick_select_case ? "ON" : "OFF"),
			(psGrp->attribute->quick_select_expire ? "ON" : "OFF"));
	}
	
	printf("END ***\n");
#endif	/* INDEX_DAEMON */
}

