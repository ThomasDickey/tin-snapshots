/*
 *  Project   : tin - a Usenet reader
 *  Module    : config.c
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 21-12-94
 *  Notes     : Configuration file routines
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

extern int index_point;

static int COL1;
static int COL2;
static int COL3;

/*
 *  read local & global configuration defaults
 */

int 
read_config_file (file, global_file)
	char 	*file;
	int		global_file;
{
	char newnews_info[PATH_LEN];
	char buf[LEN];
	FILE *fp;

	if ((fp = fopen (file, "r")) == (FILE *) 0) {
		return FALSE;
	}	

	if ((update && update_fork) || ! update) {
		if (global_file) {
			wait_message (txt_reading_global_config_file);
		} else {
			wait_message (txt_reading_config_file);
		}
	}

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		if (buf[0] == '#' || buf[0] == '\n') { 
			continue;
		}
		switch(tolower(buf[0])) {
		case 'a':
			if (match_boolean (buf, "auto_save=", &default_auto_save)) {
				break;
			}
			if (match_string (buf, "art_marked_deleted=", buf, sizeof (buf))) {
				art_marked_deleted = buf[0];
				break;
			}
			if (match_string (buf, "art_marked_inrange=", buf, sizeof (buf))) {
				art_marked_inrange = buf[0];
				break;
			}
			if (match_string (buf, "art_marked_return=", buf, sizeof (buf))) {
				art_marked_return = buf[0];
				break;
			}
			if (match_string (buf, "art_marked_selected=", buf, sizeof (buf))) {
				art_marked_selected = buf[0];
				break;
			}
			if (match_string (buf, "art_marked_unread=", buf, sizeof (buf))) {
				art_marked_unread = buf[0];
				break;
			}
#ifdef HAVE_METAMAIL
			if (match_boolean (buf, "ask_for_metamail=", &ask_for_metamail)) {
				break;
			}
#endif
			if (match_boolean (buf, "auto_cc=", &auto_cc)) {
				break;
			}
			if (match_boolean (buf, "auto_bcc=", &auto_bcc)) {
				break;
			}
			if (match_boolean (buf, "auto_list_thread=", &auto_list_thread)) {
				break;
			}
			break;
		case 'b':
			if (match_boolean (buf, "batch_save=", &default_batch_save)) {
				break;
			}
			if (match_boolean (buf, "beginner_level=", &beginner_level)) {
				break;
			}
			break;
		case 'c':
			if (match_boolean (buf, "catchup_read_groups=", &catchup_read_groups)) {
				break;
			}
			if (match_boolean (buf, "confirm_action=", &confirm_action)) {
				break;
			}
			if (match_boolean (buf, "confirm_to_quit=", &confirm_to_quit)) {
				break;
			}
#ifdef HAVE_COLOR
			if (match_integer (buf, "col_back=", &col_back, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_invers=", &col_invers, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_text=", &col_text, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_foot=", &col_foot, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_quote=", &col_quote, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_head=", &col_head, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_subject=", &col_subject, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_response=", &col_response, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_from=", &col_from, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_normal=", &col_normal, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_title=", &col_title, MAX_COLOR)) {
				break;
			}
#endif
			break;
		case 'd':
			if (match_string (buf, "default_editor_format=", default_editor_format, sizeof (default_editor_format))) {
				break;
			}
			if (match_string (buf, "default_mailer_format=", default_mailer_format, sizeof (default_mailer_format))) {
				break;
			}
			if (match_string (buf, "default_savedir=", default_savedir, sizeof (default_savedir))) {
				if (default_savedir[0] == '.' && strlen (default_savedir) == 1) {
					get_cwd (buf);
					my_strncpy (default_savedir, buf, sizeof (default_savedir));
				}
				break;
			}	
			if (match_string (buf, "default_maildir=", default_maildir, sizeof (default_maildir))) {
				break;
			}
			if (match_string (buf, "default_printer=", default_printer, sizeof (default_printer))) {
				break;
			}
			if (match_string (buf, "default_sigfile=", default_sigfile, sizeof (default_sigfile))) {
				break;
			}
			if (match_integer (buf, "default_filter_days=", &default_filter_days, 0)) {
				break;
			}
			if (match_integer (buf, "default_filter_kill_header=", &default_filter_kill_header, 0)) {
				break;
			}
			if (match_boolean (buf, "default_filter_kill_global=", &default_filter_kill_global)) {
				break;
			}
			if (match_boolean (buf, "default_filter_kill_case=", &default_filter_kill_case)) {
				break;
			}
			if (match_boolean (buf, "default_filter_kill_expire=", &default_filter_kill_expire)) {
				break;
			}
			if (match_integer (buf, "default_filter_select_header=", &default_filter_select_header, 0)) {
				break;
			}
			if (match_boolean (buf, "default_filter_select_global=", &default_filter_select_global)) {
				break;
			}
			if (match_boolean (buf, "default_filter_select_case=", &default_filter_select_case)) {
				break;
			}
			if (match_boolean (buf, "default_filter_select_expire=", &default_filter_select_expire)) {
				break;
			}
			if (match_string (buf, "default_save_mode=", buf, sizeof (buf))) {
				default_save_mode = buf[0];
				break;
			}
			if (match_string (buf, "default_author_search=", default_author_search, sizeof (default_author_search))) {
				break;
			}
			if (match_string (buf, "default_goto_group=", default_goto_group, sizeof (default_goto_group))) {
				break;
			}
			if (match_string (buf, "default_group_search=", default_group_search, sizeof (default_group_search))) {
				break;
			}
			if (match_string (buf, "default_subject_search=", default_subject_search, sizeof (default_subject_search))) {
				break;
			}
			if (match_string (buf, "default_art_search=", default_art_search, sizeof (default_art_search))) {
				break;
			}
			if (match_string (buf, "default_repost_group=", default_repost_group, sizeof (default_repost_group))) {
				break;
			}
			if (match_string (buf, "default_mail_address=", default_mail_address, sizeof (default_mail_address))) {
				break;
			}
			if (match_integer (buf, "default_move_group=", &default_move_group, 0)) {
				break;
			}
			if (match_string (buf, "default_pipe_command=", default_pipe_command, sizeof (default_pipe_command))) {
				break;
			}
			if (match_string (buf, "default_post_newsgroups=", default_post_newsgroups, sizeof (default_post_newsgroups))) {
				break;
			}
			if (match_string (buf, "default_post_subject=", default_post_subject, sizeof (default_post_subject))) {
				break;
			}
			if (match_string (buf, "default_regex_pattern=", default_regex_pattern, sizeof (default_regex_pattern))) {
				break;
			}
			if (match_string (buf, "default_range_group=", default_range_group, sizeof (default_range_group))) {
				break;
			}
			if (match_string (buf, "default_range_select=", default_range_select, sizeof (default_range_select))) {
				break;
			}
			if (match_string (buf, "default_range_thread=", default_range_thread, sizeof (default_range_thread))) {
				break;
			}
			if (match_string (buf, "default_save_file=", default_save_file, sizeof (default_save_file))) {
				break;
			}
			if (match_string (buf, "default_select_pattern=", default_select_pattern, sizeof (default_select_pattern))) {
				break;
			}
			if (match_string (buf, "default_shell_command=", default_shell_command, sizeof (default_shell_command))) {
				break;
			}
			if (match_boolean (buf, "draw_arrow=", &draw_arrow_mark)) {
				break;
			}
			break;
		case 'f':
			if (match_boolean (buf, "full_page_scroll=", &full_page_scroll)) {
				break;
			}
			if (match_boolean (buf, "force_screen_redraw=", &force_screen_redraw)) {
				break;
			}
			break;
		case 'g':
			if (match_integer (buf, "groupname_max_length=", &groupname_max_length, 132)) {
				break;
			}
			if (match_boolean (buf, "group_catchup_on_exit=", &group_catchup_on_exit)) {
				break;
			}
			break;
		case 'h':
			if (match_boolean (buf, "highlight_xcommentto=", &highlight_xcommentto)) {
				break;
			}
			break;
		case 'i':
			if (match_boolean (buf, "inverse_okay=", &inverse_okay)) {
				break;
			}
			break;
		case 'k':
			if (match_boolean (buf, "keep_dead_articles=", &keep_dead_articles)) {
				break;
			}
			if (match_boolean (buf, "keep_posted_articles=", &keep_posted_articles)) {
				break;
			}
			break;				
		case 'm':
			if (match_string (buf, "mime_encoding=", mime_encoding, sizeof (mime_encoding))) {
				if (strcasecmp(mime_encoding, "8bit") &&
					strcasecmp(mime_encoding, "base64") &&
					strcasecmp(mime_encoding, "quoted-printable")) {
					strcpy(mime_encoding,"8bit");
				}
				break;
			}
			if (match_string (buf, "motd_file_info=", motd_file_info, sizeof (motd_file_info))) {
				break;
			}
			if (match_boolean (buf, "mark_saved_read=", &mark_saved_read)) {
				break;
			}
			if (match_string (buf, "mail_quote_format=", mail_quote_format, sizeof (mail_quote_format))) {
				break;
			}
			break;
		case 'n':
			if (match_string (buf, "newnews=", newnews_info, sizeof (newnews_info))) {
				load_newnews_info (newnews_info);
				break;
			}
			if (match_string (buf, "news_quote_format=", news_quote_format, sizeof (news_quote_format))) {
				break;
			}
			if (match_boolean (buf, "no_advertising=", &no_advertising)) {
				break;
			}
			break;
		case 'p':
			if (match_boolean (buf, "print_header=", &print_header)) {
				break;
			}
			if (match_boolean (buf, "pos_first_unread=", &pos_first_unread)) {
				break;
			}	
			if (match_integer (buf, "post_process_type=", &default_post_proc_type, POST_PROC_UUD_EXT_ZIP)) {
				proc_ch_default = get_post_proc_type (default_post_proc_type);
				break;
			}
			if (match_string (buf, "post_process_command=", post_proc_command, sizeof(post_proc_command))) {
				break;
			}	
			if (match_boolean (buf, "process_only_unread=", &process_only_unread)) {
				break;
			}
			break;
		case 'q':
			if (match_string (buf, "quote_chars=", quote_chars, sizeof (quote_chars))) {
				quote_dash_to_space (quote_chars);
				break;
			}
			break;
		case 'r':
			if (match_integer (buf, "reread_active_file_secs=", &reread_active_file_secs, 10000)) {
				break;
			}
			break;
		case 's':
			if (match_boolean (buf, "sigdashes=", &sigdashes)) {
				break;
			}
			if (match_boolean (buf, "start_editor_offset=", &start_editor_offset)) {
				break;
			}
			if (match_boolean (buf, "show_only_unread_groups=", &show_only_unread_groups)) {
				break;
			}
			if (match_boolean (buf, "show_only_unread=", &default_show_only_unread)) {
				break;
			}
			if (match_boolean (buf, "show_description=", &show_description)) {
				break;
			}
			if (match_integer (buf, "show_author=", &default_show_author, SHOW_FROM_BOTH)) {
				break;
			}	
			if (match_integer (buf, "sort_article_type=", &default_sort_art_type, SORT_BY_DATE_ASCEND)) {
				break;
			}	
			if (match_boolean (buf, "show_last_line_prev_page=", &show_last_line_prev_page)) {
				break;
			}
			if (match_boolean (buf, "save_to_mmdf_mailbox=", &save_to_mmdf_mailbox)) {
				break;
			}
			if (match_boolean (buf, "strip_blanks=", &strip_blanks)) {
				break;
			}
			if (match_boolean (buf, "show_xcommentto=", &show_xcommentto)) {
				 break;
			}
			if (match_boolean (buf, "space_goto_next_unread=", &space_goto_next_unread)) {
				break;
			}
			break;
		case 't':
			if (match_integer (buf, "thread_articles=", &default_thread_arts, THREAD_REFS)) {
				break;
			}	
			if (match_boolean (buf, "tab_after_X_selection=", &tab_after_X_selection)) {
				break;
			}
			if (match_boolean (buf, "tab_goto_next_unread=", &tab_goto_next_unread)) {
				break;
			}
			if (match_boolean (buf, "thread_catchup_on_exit=", &thread_catchup_on_exit)) {
				break;
			}
			break;
		case 'u':
			if (match_boolean (buf, "unlink_article=", &unlink_article)) {
				break;
			}	
			if (match_boolean (buf, "use_builtin_inews=", &use_builtin_inews)) {
				break;
			}
			if (match_boolean (buf, "use_mouse=", &use_mouse)) {
				break;
			}
#ifdef HAVE_KEYPAD
			if (match_boolean (buf, "use_keypad=", &use_keypad)) {
				break;
			}
#endif
#ifdef HAVE_METAMAIL
			if (match_boolean (buf, "use_metamail=", &use_metamail)) {
				break;
			}
#endif
#ifdef HAVE_COLOR
			if (match_boolean (buf, "use_color=", &use_color_tinrc)) {
				use_color=use_color_tinrc;
				break;
			}
#endif
			break;
		case 'x':
			if (match_string (buf, "xpost_quote_format=", xpost_quote_format, sizeof (xpost_quote_format))) {

				break;
			}
			break;
		}
	}
	fclose (fp);
	
	/* nobody likes to navigate blind */
	if (! draw_arrow_mark && !inverse_okay) {
		draw_arrow_mark = TRUE;
	}

	/* sort out conflicting settings */
	if (! draw_arrow_mark && strip_blanks) {
		strip_blanks = FALSE;
	}

	if ((cmd_line && ! update && ! verbose) || (update && update_fork)) {
		wait_message ("\n");
	}

	return TRUE;		
}


/*
 *  write config defaults to ~/.tin/tinrc
 */

void 
write_config_file (file)
	char	*file;
{
	FILE *fp;
	char *file_tmp;
	int i;
	
	/* alloc memory for tmp-filename */
	if((file_tmp=malloc(strlen(file)+5)) == NULL) {
		wait_message ("Out of memory!");
		return;
	}	
	/* generate tmp-filename */
	strcpy(file_tmp,file);	
	strcat(file_tmp,".tmp");
	
	if ((fp = fopen (file_tmp, "w")) == (FILE *) 0) {
		wait_message (txt_filesystem_full_config_backup);
		return;
	}	
	
	if (! cmd_line) {
		wait_message (txt_saving);
	}
	
	if (! default_editor_format[0]) {
		strcpy (default_editor_format, EDITOR_FORMAT_ON);
	}
	
	fprintf (fp, "# if ON articles/threads with Archive-name: in mail header will\n");
	fprintf (fp, "# be automatically saved with the Archive-name & part/patch no.\n");
	fprintf (fp, "auto_save=%s\n\n", print_boolean (default_auto_save));
	fprintf (fp, "# if ON articles/threads will be saved in batch mode when save -S\n");
	fprintf (fp, "# or mail -M is specified on the command line\n");
	fprintf (fp, "batch_save=%s\n\n", print_boolean (default_batch_save));
	fprintf (fp, "# if ON editor will be started with cursor offset into the file\n");
	fprintf (fp, "# otherwise the cursor will be positioned at the first line\n");
	fprintf (fp, "start_editor_offset=%s\n\n", print_boolean (start_editor_offset));
	fprintf (fp, "# Format of editor line including parameters\n");
	fprintf (fp, "# %%E Editor  %%F Filename  %%N Linenumber\n");
	fprintf (fp, "default_editor_format=%s\n\n", default_editor_format);
	fprintf (fp, "# Format of mailer line including parameters\n");
	fprintf (fp, "# %%M Mailer  %%S Subject  %%T To  %%F Filename  %%U User (AmigaDOS)\n");
	fprintf (fp, "# ie. to use elm as your mailer:    elm -s \"%%S\" \"%%T\" < %%F\n");
	fprintf (fp, "default_mailer_format=%s\n\n", default_mailer_format);
	fprintf (fp, "# if ON mark articles that are saved as read\n");
	fprintf (fp, "mark_saved_read=%s\n\n", print_boolean (mark_saved_read));
	fprintf (fp, "# if ON use inverse video for page headers at different levels\n");
	fprintf (fp, "inverse_okay=%s\n\n", print_boolean (inverse_okay));
	fprintf (fp, "# if ON use -> otherwise highlighted bar for selection\n");
	fprintf (fp, "draw_arrow=%s\n\n", print_boolean (draw_arrow_mark));
#ifdef HAVE_COLOR
	fprintf (fp, "# if ON using ansi-color\n");
	fprintf (fp, "use_color=%s\n\n", (use_color_tinrc ? "ON" : "OFF"));
	fprintf (fp, "# For coloradjust use the following numbers\n");
	fprintf (fp, "# 0-black        1-red         2-green        3-brown\n");
	fprintf (fp, "# 4-blue         5-pink        6-cyan         7-white\n");
	fprintf (fp, "# These are *only* for foreground:\n");
	fprintf (fp, "#  8-gray        9-lightred   10-lightgreen  11-yellow\n");
	fprintf (fp, "# 12-lightblue  13-lightpink  14-lightcyan   15-lightwhite\n\n");
	fprintf (fp, "#Standard-Background-Color\n");
	fprintf (fp, "col_back=%d\n\n", col_back);
	fprintf (fp, "#Color for inverse text\n");
	fprintf (fp, "col_invers=%d\n\n", col_invers);
	fprintf (fp, "#Color of textlines\n");
	fprintf (fp, "col_text=%d\n\n", col_text);
	fprintf (fp, "#Color of footlines (Help,...)\n");
	fprintf (fp, "col_foot=%d\n\n", col_foot);
	fprintf (fp, "#Color of quotelines\n");
	fprintf (fp, "col_quote=%d\n\n", col_quote);
	fprintf (fp, "#Color of headerlines\n");
	fprintf (fp, "col_head=%d\n\n", col_head);
	fprintf (fp, "#Color of article subject\n");
	fprintf (fp, "col_subject=%d\n\n", col_subject);
	fprintf (fp, "#Color of response counter\n");
	fprintf (fp, "col_response=%d\n\n", col_response);
	fprintf (fp, "#Color of sender (From:)\n");
	fprintf (fp, "col_from=%d\n\n", col_from);
	fprintf (fp, "#Standard foreground color\n");
	fprintf (fp, "col_normal=%d\n\n", col_normal);
	fprintf (fp, "#Color of Help/Mail-Sign\n");
	fprintf (fp, "col_title=%d\n\n", col_title);
#endif
	fprintf (fp, "# if ON print all of mail header otherwise Subject: & From: lines\n");
	fprintf (fp, "print_header=%s\n\n", print_boolean (print_header));
	fprintf (fp, "# if ON put cursor at first unread art in group otherwise last art\n");
	fprintf (fp, "pos_first_unread=%s\n\n", print_boolean (pos_first_unread));
	fprintf (fp, "# if ON scroll full page of groups/articles otherwise half a page\n");
	fprintf (fp, "full_page_scroll=%s\n\n", print_boolean (full_page_scroll));
#ifdef HAVE_METAMAIL
 	fprintf (fp, "# if ON metamail can/will be used to display MIME articles\n");
 	fprintf (fp, "use_metamail=%s\n\n", print_boolean (use_metamail));
 	fprintf (fp, "# if ON tin will ask before using metamail to display MIME messages\n");
 	fprintf (fp, "# this only occurs, if use_metamail is also switched ON\n");
 	fprintf (fp, "ask_for_metamail=%s\n\n", print_boolean (ask_for_metamail));
#endif
	fprintf (fp, "# MIME encoding of the body, if necessary. (8bit, base64, quoted-printable)\n");
	fprintf (fp, "mime_encoding=%s\n\n", mime_encoding);
	fprintf (fp, "# if ON ask user if read groups should all be marked read\n");
	fprintf (fp, "catchup_read_groups=%s\n\n", print_boolean (catchup_read_groups));
	fprintf (fp, "# if ON confirm certain commands with y/n before executing\n");
	fprintf (fp, "confirm_action=%s\n\n", print_boolean (confirm_action));
	fprintf (fp, "# if ON confirm with y/n before quitting ('Q' never asks)\n");
	fprintf (fp, "confirm_to_quit=%s\n\n", print_boolean (confirm_to_quit));
	fprintf (fp, "# if ON show group description text after newsgroup name at\n");
	fprintf (fp, "# group selection level\n");
	fprintf (fp, "show_description=%s\n\n", print_boolean (show_description));
	fprintf (fp, "# part of from field to display 0) none 1) address 2) full name 3) both\n");
	fprintf (fp, "show_author=%d\n\n", default_show_author);
	fprintf (fp, "# type of post processing to perform after saving articles.\n");
#ifdef M_AMIGA
	fprintf (fp, "# 0=(none) 1=(unshar) 2=(uudecode) 3=(uudecode & list lha)\n");
	fprintf (fp, "# 4=(uud & extract lha) 5=(uud & list zip) 6=(uud & extract zip)\n");
#else
	fprintf (fp, "# 0=(none) 1=(unshar) 2=(uudecode) 3=(uudecode & list zoo)\n");
	fprintf (fp, "# 4=(uud & extract zoo) 5=(uud & list zip) 6=(uud & extract zip)\n");
#endif
	fprintf (fp, "post_process_type=%d\n\n", default_post_proc_type);
	fprintf (fp, "# if set, command to be run after a successful uudecode\n");
	fprintf (fp, "post_process_command=%s\n\n", post_proc_command);
	fprintf (fp, "# Thread articles on 0=(nothing) 1=(Subject) 2=(References).\n");
	fprintf (fp, "thread_articles=%d\n\n", default_thread_arts);
	fprintf (fp, "# if ON remove ~/.article after posting.\n");
	fprintf (fp, "unlink_article=%s\n\n", print_boolean (unlink_article));
	fprintf (fp, "# if ON keep all failed postings in ~/dead.articles\n");
	fprintf (fp, "keep_dead_articles=%s\n\n", print_boolean (keep_dead_articles));
	fprintf (fp, "# if ON keep all failed postings in ~/Mail/posted\n");
	fprintf (fp, "keep_posted_articles=%s\n\n", print_boolean (keep_posted_articles));
	fprintf (fp, "# if ON show only subscribed to groups that contain unread articles.\n");
	fprintf (fp, "show_only_unread_groups=%s\n\n", print_boolean (show_only_unread_groups));
	fprintf (fp, "# if ON show only new/unread articles otherwise show all.\n");
	fprintf (fp, "show_only_unread=%s\n\n", print_boolean (default_show_only_unread));
	fprintf (fp, "# sort articles by 0=(nothing) 1=(Subject descend) 2=(Subject ascend)\n");
	fprintf (fp, "# 3=(From descend) 4=(From ascend) 5=(Date descend) 6=(Date ascend).\n");
	fprintf (fp, "sort_article_type=%d\n\n", default_sort_art_type);
	fprintf (fp, "# directory where articles/threads are saved\n");
	fprintf (fp, "default_savedir=%s\n\n", default_savedir);
	fprintf (fp, "# (-m) directory where articles/threads are saved in mailbox format\n");	
	fprintf (fp, "default_maildir=%s\n\n", default_maildir);	
	fprintf (fp, "# print program with parameters used to print articles/threads\n");
	fprintf (fp, "default_printer=%s\n\n", default_printer);
	fprintf (fp, "# Signature path (random sigs)/file to be used when posting/replying to messages\n");
	fprintf (fp, "default_sigfile=%s\n\n", default_sigfile);
	fprintf (fp, "# if ON prepend the signature with dashes '\\n-- \\n'\n");
	fprintf (fp, "sigdashes=%s\n\n", print_boolean (sigdashes));
	fprintf (fp, "# turn off advertising in header (X-Newsreader/X-Mailer)\n");
	fprintf (fp, "no_advertising=%s\n\n", print_boolean (no_advertising));
	fprintf (fp, "# time interval in seconds between rereading the active file\n");
	fprintf (fp, "reread_active_file_secs=%d\n\n", reread_active_file_secs);
	fprintf (fp, "# characters used in quoting to followups and replys.\n");
	fprintf (fp, "# '_' is replaced by ' ', %%s is replaced by your initials.\n");
	fprintf (fp, "quote_chars=%s\n\n", quote_space_to_dash (quote_chars));
	fprintf (fp, "# character used to show that an art was deleted (default 'D')\n");
	fprintf (fp, "art_marked_deleted=%c\n\n", art_marked_deleted);
	fprintf (fp, "# character used to show that an art is in a range (default '#')\n");
	fprintf (fp, "art_marked_inrange=%c\n\n", art_marked_inrange);
	fprintf (fp, "# character used to show that an art will return (default '-')\n");
	fprintf (fp, "art_marked_return=%c\n\n", art_marked_return);
	fprintf (fp, "# character used to show that an art was auto-selected (default '*')\n");
	fprintf (fp, "art_marked_selected=%c\n\n", art_marked_selected);
	fprintf (fp, "# character used to show that an art was unread (default '+')\n");
	fprintf (fp, "art_marked_unread=%c\n\n", art_marked_unread);
	fprintf (fp, "# if ON show the last line of the previous page as first line of next page\n");
	fprintf (fp, "show_last_line_prev_page=%s\n\n", print_boolean (show_last_line_prev_page));
	fprintf (fp, "# if ON a TAB command will be automatically done after the X command\n");
	fprintf (fp, "tab_after_X_selection=%s\n\n", print_boolean (tab_after_X_selection));
	fprintf (fp, "# if ON the TAB command will goto next unread article at article viewer level\n");
	fprintf (fp, "tab_goto_next_unread=%s\n\n", print_boolean (tab_goto_next_unread));
	fprintf (fp, "# if ON the SPACE command will goto next unread article at article viewer\n");
	fprintf (fp, "# level when the end of the article is reached (rn-style pager)\n");
	fprintf (fp, "space_goto_next_unread=%s\n\n", print_boolean (space_goto_next_unread));
	fprintf (fp, "# if ON a screen redraw will always be done after certain external commands\n");
	fprintf (fp, "force_screen_redraw=%s\n\n", print_boolean (force_screen_redraw));
	fprintf (fp, "# if ON save mail to a MMDF style mailbox (default is normal mbox format)\n");
	fprintf (fp, "save_to_mmdf_mailbox=%s\n\n", print_boolean (save_to_mmdf_mailbox));
	fprintf (fp, "# if ON use the builtin mini inews otherwise use an external inews program\n");
	fprintf (fp, "use_builtin_inews=%s\n\n", print_boolean (use_builtin_inews));
	fprintf (fp, "# Format of quote line when mailing/posting/followingup an article\n");
	fprintf (fp, "# %%A Address  %%D Date  %%F Addr+Name  %%G Groupname  %%M MessageId  %%N Name\n");
	fprintf (fp, "news_quote_format=%s\n", news_quote_format);
	fprintf (fp, "mail_quote_format=%s\n", mail_quote_format);
	fprintf (fp, "xpost_quote_format=%s\n\n", xpost_quote_format);
	fprintf (fp, "# if ON automatically put your name in the Cc: field when mailing an article\n");
	fprintf (fp, "auto_cc=%s\n\n", print_boolean (auto_cc));
	fprintf (fp, "# if ON automatically put your name in the Bcc: field when mailing an article\n");
	fprintf (fp, "auto_bcc=%s\n\n", print_boolean (auto_bcc));
	fprintf (fp, "# if ON catchup group/thread when leaving with the left arrow key.\n");
	fprintf (fp, "group_catchup_on_exit=%s\n", print_boolean (group_catchup_on_exit));
	fprintf (fp, "thread_catchup_on_exit=%s\n\n", print_boolean (thread_catchup_on_exit));
	fprintf (fp, "# if ON automatically list thread when entering it using right arrow key.\n");
	fprintf (fp, "auto_list_thread=%s\n\n", print_boolean (auto_list_thread));
	fprintf (fp, "# If ON enable mouse key support on xterm terminals\n");
	fprintf (fp, "use_mouse=%s\n\n", print_boolean (use_mouse));
#ifdef HAVE_KEYPAD
	fprintf (fp, "# If ON enable scroll keys on terminals that support it\n");
	fprintf (fp, "use_keypad=%s\n\n", print_boolean (use_keypad));
#endif
	fprintf (fp, "# If ON strip blanks from end of lines to speedup display on slow terminals\n");
	fprintf (fp, "strip_blanks=%s\n\n", print_boolean (strip_blanks));
	fprintf (fp, "# Maximum length of the names of newsgroups displayed\n");
	fprintf (fp, "groupname_max_length=%d\n\n", groupname_max_length);
	fprintf (fp, "# If ON show a mini menu of useful commands at each level\n");
	fprintf (fp, "beginner_level=%s\n\n", print_boolean (beginner_level));
	fprintf (fp, "# If ON only save/print/pipe/mail unread articles (tagged articles excepted)\n");
	fprintf (fp, "process_only_unread=%s\n\n", print_boolean (process_only_unread));
	fprintf (fp, "# If ON, the realname in the X-Comment-To header is displayed\n");
	fprintf (fp, "show_xcommentto=%s\n\n", print_boolean(show_xcommentto));
	fprintf (fp, "# If ON X-Commento-To name is displayed in the upper-right corner,\n");
	fprintf (fp, "# if OFF below the Summary-Header\n");
	fprintf (fp, "highlight_xcommentto=%s\n\n", print_boolean(highlight_xcommentto));
	fprintf (fp, "# Num of days a short term filter will be active\n");
	fprintf (fp, "default_filter_days=%d\n\n", default_filter_days);
	fprintf (fp, "# Defaults for quick (1 key) kill & auto-selection filters\n");
	fprintf (fp, "# header=NUM  0=Subject: 1=From: 2=Message-Id:\n");
	fprintf (fp, "# global=ON/OFF  ON=apply to all groups OFF=apply to current group\n");
	fprintf (fp, "# case=ON/OFF    ON=filter case sensitive OFF=ignore case\n");
	fprintf (fp, "# expire=ON/OFF  ON=limit to default_filter_days OFF=don't ever expire\n");
	fprintf (fp, "default_filter_kill_header=%d\n", default_filter_kill_header);
	fprintf (fp, "default_filter_kill_global=%s\n", print_boolean (default_filter_kill_global));
	fprintf (fp, "default_filter_kill_case=%s\n", print_boolean (default_filter_kill_case));
	fprintf (fp, "default_filter_kill_expire=%s\n", print_boolean (default_filter_kill_expire));
	fprintf (fp, "default_filter_select_header=%d\n", default_filter_select_header);
	fprintf (fp, "default_filter_select_global=%s\n", print_boolean (default_filter_select_global));
	fprintf (fp, "default_filter_select_case=%s\n", print_boolean (default_filter_select_case));
	fprintf (fp, "default_filter_select_expire=%s\n\n", print_boolean (default_filter_select_expire));
	fprintf (fp, "# default action/prompt strings\n");
	fprintf (fp, "default_save_mode=%c\n", default_save_mode);
	fprintf (fp, "default_author_search=%s\n", default_author_search);
	fprintf (fp, "default_goto_group=%s\n", default_goto_group);
	fprintf (fp, "default_group_search=%s\n", default_group_search);
	fprintf (fp, "default_subject_search=%s\n", default_subject_search);
	fprintf (fp, "default_art_search=%s\n", default_art_search);
	fprintf (fp, "default_repost_group=%s\n", default_repost_group);
	fprintf (fp, "default_mail_address=%s\n", default_mail_address);
	fprintf (fp, "default_move_group=%d\n", default_move_group);
	fprintf (fp, "default_pipe_command=%s\n", default_pipe_command);
	fprintf (fp, "default_post_newsgroups=%s\n", default_post_newsgroups);
	fprintf (fp, "default_post_subject=%s\n", default_post_subject);
	fprintf (fp, "default_range_group=%s\n", default_range_group);
	fprintf (fp, "default_range_select=%s\n", default_range_select);
	fprintf (fp, "default_range_thread=%s\n", default_range_thread);
	fprintf (fp, "default_regex_pattern=%s\n", default_regex_pattern);
	fprintf (fp, "default_save_file=%s\n", default_save_file);
	fprintf (fp, "default_select_pattern=%s\n", default_select_pattern);
	fprintf (fp, "default_shell_command=%s\n\n", default_shell_command);
	fprintf (fp, "# news motd file dates from server used for detecting new motd info\n");
	fprintf (fp, "motd_file_info=%s\n\n", motd_file_info);
	fprintf (fp, "# host&time info used for detecting new groups (don't touch)\n");
	if (! num_newnews) {
		fprintf (fp, "newnews=%s %ld\n", new_newnews_host, new_newnews_time);
	} else {
		for (i = 0 ; i < num_newnews ; i++) {
			fprintf (fp, "newnews=%s %ld\n", newnews[i].host, newnews[i].time);
		}
	}
	if (ferror (fp) | fclose (fp)){
		wait_message (txt_filesystem_full_config);
		return;
	} else {
		rename_file(file_tmp, file);
		chmod (file, 0600);
	}
}

/*
 *  options menu so that the user can dynamically change parameters
 */
 
int 
change_config_file (group, filter_at_once)
	struct t_group *group;
	int filter_at_once;
{
	int ch, i;
	int filter_changed = FALSE;
	int orig_show_only_unread;
	int orig_thread_arts;
	int option;
	int ret_code = NO_FILTERING;
	
#ifdef SIGTSTP
	RETSIGTYPE (*susp)(SIG_ARGS);
	
	susp = (RETSIGTYPE (*)(SIG_ARGS)) 0;

	if (do_sigtstp) {
		susp = sigdisp (SIGTSTP, SIG_DFL);
	}
#endif

	COL1 = 0;
	COL2 = ((cCOLS / 3) * 1) + 1;
	COL3 = ((cCOLS / 3) * 2) + 2;

	show_config_menu ();

	set_xclick_off ();
	forever {

#ifdef SIGTSTP
		if (do_sigtstp) {
			sigdisp (SIGTSTP, config_suspend);
		}
#endif
		MoveCursor (cLINES, 0);
		ch = ReadCh ();
		if (ch >= '1' && ch <= '9') {
			option = prompt_num (ch, "Enter option number> ");
		} else {
			if (ch == 'q' || ch == ESC) {
				option = -1;
			} else {
				option = 0;
			}
		}
#ifdef SIGTSTP
		if (do_sigtstp) {
			sigdisp (SIGTSTP, SIG_IGN);
		}
#endif
		switch (option) {
			case 0:
				write_config_file (local_config_file);
				/* FALLTHRU */
			case -1:
				if (filter_changed) {
					if (filter_at_once) {
						global_filtered_articles = read_filter_file (global_filter_file, TRUE);
						local_filtered_articles = read_filter_file (local_filter_file, FALSE);
						if (global_filtered_articles || local_filtered_articles) {
							if (filter_articles (group)) {
								make_threads (group, FALSE);
								find_base (group);
							}
						} else {
							if (unfilter_articles ()) {
								make_threads (group, FALSE);
								find_base (group);
							}
						}
					}
					ret_code = FILTERING;
				}

				clear_note_area ();
#ifdef SIGTSTP
				if (do_sigtstp) {
					sigdisp (SIGTSTP, susp);
				}
#endif
				return ret_code;
			
			case 1:		/* auto save */
				prompt_on_off (INDEX_TOP, COL1, &default_auto_save, 
					txt_help_autosave, txt_opt_autosave);
				break;

			case 2:		/* start editor with line offset */
				prompt_on_off (INDEX_TOP, COL2, &start_editor_offset, 
					txt_help_start_editor_offset, txt_opt_start_editor_offset);
				break;
			
			case 3:		/* mark saved articles read */
				prompt_on_off (INDEX_TOP, COL3, &mark_saved_read, 
					txt_help_mark_saved_read, txt_opt_mark_saved_read);
				break;

			case 4:		/* confirm action */
				prompt_on_off (INDEX_TOP+2, COL1, &confirm_action, 
					txt_help_confirm_action, txt_opt_confirm_action);
				break;

			case 5:		/* draw -> / highlighted bar */
				prompt_on_off (INDEX_TOP+2, COL2, &draw_arrow_mark, 
					txt_help_draw_arrow, txt_opt_draw_arrow);
				if (draw_arrow_mark == FALSE && inverse_okay == FALSE) {
					inverse_okay = TRUE;
				}
				break;

			case 6:		/* print header */
				prompt_on_off (INDEX_TOP+2, COL3, &print_header, 
					txt_help_print_header, txt_opt_print_header);
				break;
			
			case 7:		/* position cursor at first / last unread art */
				prompt_on_off (INDEX_TOP+4, COL1, &pos_first_unread, 
					txt_help_pos_first_unread, txt_opt_pos_first_unread);
				break;

			case 8:		/* scroll half/full page of groups/articles */
				prompt_on_off (INDEX_TOP+4, COL2, &full_page_scroll, 
					txt_help_page_scroll, txt_opt_page_scroll);
				break;

			case 9:		/* catchup read groups when quitting */
				prompt_on_off (INDEX_TOP+4, COL3, &catchup_read_groups, 
					txt_help_catchup_groups, txt_opt_catchup_groups);
				break;

			case 10:	/* thread/unthread all groups except those in ~/.tin/unthreaded */
				orig_thread_arts = default_thread_arts;	

				default_thread_arts = prompt_list (INDEX_TOP+6, COL1,
							default_thread_arts,
							txt_help_thread_arts,
							txt_opt_thread_arts,
							txt_thread,
#ifdef HAVE_REF_THREADING
							THREAD_REFS + 1
#else
							THREAD_REFS
#endif
							);

				/*
				 * If the threading strategy has changed, fix things
				 * so that rethreading will occur
				 */
				if (default_thread_arts != orig_thread_arts && 
				    							group != (struct t_group *) 0) {
					group->attribute->thread_arts = default_thread_arts;
					make_threads (group, TRUE);
					find_base (group);
				}
				clear_message ();
				break;

			case 11:	/* show all arts or just new/unread arts */
				orig_show_only_unread = default_show_only_unread;	
				prompt_on_off (INDEX_TOP+6, COL2, &default_show_only_unread, 
					txt_help_show_only_unread, txt_opt_show_only_unread);
				if (default_show_only_unread != orig_show_only_unread &&
				    group != (struct t_group *) 0) {
					make_threads (group, TRUE);
					find_base (group);
					if (space_mode) {
						for (i = 0; i < top_base; i++) {
							if (new_responses (i)) {
								break;
							}
						}
						if (i < top_base) {
							index_point = i;
						} else {
							index_point = top_base - 1;
						}
					} else {
						index_point = top_base - 1;
					}
				}
				break;

			case 12:	/* show newsgroup description text next to newsgroups */
				prompt_on_off (INDEX_TOP+6, COL3, &show_description, 
					txt_help_show_description, txt_opt_show_description);
				if (show_description) {	/* force reread of newgroups file */
					read_newsgroups_file ();
					clear_message ();
				} else {
					set_groupname_len (FALSE);
				}
				break;

			case 13:		/* show subject & author / subject only */
				default_show_author = prompt_list (INDEX_TOP+8, COL1,
							default_show_author,
							txt_help_show_author,
							txt_opt_show_author,
							txt_show_from,
							SHOW_FROM_BOTH + 1
							);
#if 0
				/* If not changed ... */
				 else {
					set_subj_from_size (cCOLS);
				}
#endif				
				break;

			case 14:
				default_post_proc_type = prompt_list (INDEX_TOP+8, COL2,
							default_post_proc_type,
							txt_help_post_proc_type,
							txt_opt_process_type,
							txt_post_process,
							POST_PROC_UUD_EXT_ZIP + 1
							);
				proc_ch_default = get_post_proc_type (default_post_proc_type);
				break;

			case 15:
				default_sort_art_type = prompt_list (INDEX_TOP+10, COL1,
							default_sort_art_type,
							txt_help_sort_type,
							txt_opt_sort_type,
							txt_sort_type,
							SORT_BY_DATE_ASCEND + 1
							);
				break;

			case 16:
#ifdef M_AMIGA
				{	extern int tin_bbs_mode;
					if (tin_bbs_mode) break;
				}
#endif
				show_menu_help (txt_help_savedir);
				prompt_menu_string (INDEX_TOP+12, COL1 + (int) strlen (txt_opt_savedir), default_savedir);
				expand_rel_abs_pathname (INDEX_TOP+12, COL1 + (int) strlen (txt_opt_savedir), default_savedir);
				break;

			case 17:
#ifdef M_AMIGA
				{	extern int tin_bbs_mode;
					if (tin_bbs_mode) break;
				}
#endif
				show_menu_help (txt_help_maildir);
				prompt_menu_string (INDEX_TOP+14, COL1 + (int) strlen (txt_opt_maildir), default_maildir);
				expand_rel_abs_pathname (INDEX_TOP+14, COL1 + (int) strlen (txt_opt_maildir), default_maildir);
				break;

			case 18:
#ifdef M_AMIGA
				{	extern int tin_bbs_mode;
					if (tin_bbs_mode) break;
				}
#endif
				show_menu_help (txt_help_printer);
				prompt_menu_string (INDEX_TOP+16, COL1 + (int) strlen (txt_opt_printer), default_printer);
				expand_rel_abs_pathname (INDEX_TOP+16, COL1 + (int) strlen (txt_opt_printer), default_printer);
				break;
		}
		show_menu_help (txt_select_config_file_option);
	}
}


void 
show_config_menu ()
{
	ClearScreen ();

	center_line (0, TRUE, txt_options_menu);
	
	MoveCursor (INDEX_TOP, 0);
	printf ("%s%s\r\n\r\n", txt_opt_autosave, print_boolean (default_auto_save));
	printf ("%s%s\r\n\r\n", txt_opt_confirm_action, print_boolean (confirm_action));
	printf ("%s%s\r\n\r\n", txt_opt_pos_first_unread, print_boolean (pos_first_unread));
	printf ("%s%s", txt_opt_thread_arts, txt_thread[default_thread_arts]);

	MoveCursor(INDEX_TOP, COL2);
	printf ("%s%s", txt_opt_start_editor_offset, print_boolean (start_editor_offset));
	MoveCursor(INDEX_TOP+2, COL2);
	printf ("%s%s", txt_opt_draw_arrow, print_boolean (draw_arrow_mark));
	MoveCursor(INDEX_TOP+4, COL2);
	printf ("%s%s", txt_opt_page_scroll, print_boolean (full_page_scroll));
	MoveCursor(INDEX_TOP+6, COL2);
	printf ("%s%s", txt_opt_show_only_unread, print_boolean (default_show_only_unread));

	MoveCursor(INDEX_TOP, COL3);
	printf ("%s%s", txt_opt_mark_saved_read, print_boolean (mark_saved_read));
	MoveCursor(INDEX_TOP+2, COL3);
	printf ("%s%s", txt_opt_print_header, print_boolean (print_header));
	MoveCursor(INDEX_TOP+4, COL3);
	printf ("%s%s", txt_opt_catchup_groups, print_boolean (catchup_read_groups));
	MoveCursor(INDEX_TOP+6, COL3);
	printf ("%s%s", txt_opt_show_description, print_boolean (show_description));

	MoveCursor(INDEX_TOP+8, COL1);
	printf ("%s%s", txt_opt_show_author, txt_show_from[default_show_author]);
	MoveCursor(INDEX_TOP+8, COL2);
	printf ("%s%s\r\n\r\n", txt_opt_process_type, txt_post_process[default_post_proc_type]);
	
	MoveCursor(INDEX_TOP+10, COL1);
	printf ("%s%s\r\n\r\n", txt_opt_sort_type, txt_sort_type[default_sort_art_type]);

#ifdef M_AMIGA
	{	extern int tin_bbs_mode;
		if (!tin_bbs_mode) {
#endif
	printf ("%s%s\r\n\r\n", txt_opt_savedir, default_savedir);
	printf ("%s%s\r\n\r\n", txt_opt_maildir, default_maildir);
	printf ("%s%s\r\n\r\n", txt_opt_printer, default_printer);
#ifdef M_AMIGA
		}
	}
#endif
	fflush(stdout);

	show_menu_help (txt_select_config_file_option);
	MoveCursor (cLINES, 0);
}

/*
 *  expand ~/News to /usr/username/News and print to screen
 */
 
void 
expand_rel_abs_pathname (line, col, str)
	int line;
	int col;
	char *str;
{
	char buf[LEN];
	
	if (str[0] == '~') {
		if (strlen (str) == 1) {
			strcpy (str, homedir);
		} else {
			joinpath (buf, homedir, str+2);
			strcpy (str, buf);
		}
	}
	MoveCursor (line, col);
	CleartoEOLN ();
	my_fputs (str, stdout);
	fflush (stdout);
}

/*
 *  show_menu_help
 */
 
void 
show_menu_help (help_message)
	char *help_message;
{
	 MoveCursor (cLINES-2, 0);
	 CleartoEOLN ();
	 center_line (cLINES-2, FALSE, help_message);
}


int 
match_boolean (line, pat, dst)
	char *line;
	char *pat;
	int *dst;
{
	size_t	patlen = strlen (pat);
	
	if (STRNCASECMPEQ(line, pat, patlen)) {
		*dst = (STRNCASECMPEQ(&line[patlen], "ON", 2) ? TRUE : FALSE);
		return TRUE;
	}
	return FALSE;
}


/*
 * If pat matches the start of line, convert rest of line to an integer, dst
 * If maxlen is set, constrain value to 0 <= dst <= maxlen and return TRUE.
 * If no match is made, return FALSE.
 */
int 
match_integer (line, pat, dst, maxlen)
	char *line;
	char *pat;
	int *dst, maxlen;
{
	size_t	patlen = strlen (pat);

	if (STRNCMPEQ(line, pat, patlen)) {
		*dst = atoi (&line[patlen]);

		if (maxlen)  {
			if ((*dst < 0) || (*dst > maxlen)) {
				fprintf(stderr, "\n%s%d out of range 0 - %d. Reset to 0", pat, *dst, maxlen);
				*dst = 0;
			}
		}

		return TRUE;
	}
	return FALSE;
}


int
match_long (line, pat, dst)
	char *line;
	char *pat;
	long *dst;
{
	size_t	patlen = strlen (pat);
  
	if (STRNCMPEQ(line, pat, patlen)) {
		*dst = atol (&line[patlen]);
		return TRUE;
	}
	return FALSE;
}


int 
match_string (line, pat, dst, dstlen)
	char *line;
	char *pat;
	char *dst;
	size_t dstlen;
{
	char	*ptr;
	size_t	patlen = strlen (pat);

	if (STRNCMPEQ(line, pat, patlen)) {
		strncpy (dst, &line[patlen], dstlen);
		ptr = strrchr (dst, '\n');
		if (ptr != (char *) 0) {
			*ptr = '\0';
		}
		return TRUE;
	}
	return FALSE;
}


char *
print_boolean (value)
	int value;
{
	return (value ? txt_onoff[TRUE] : txt_onoff[FALSE]);
}

/*
 *  convert underlines to spaces in a string
 */

void 
quote_dash_to_space (str)
	char *str;
{
	char *ptr;

	for (ptr = str; *ptr; ptr++ ) {
		if (*ptr == '_') {
			*ptr = ' ';
		}
	} 
}

/*
 *  convert spaces to underlines in a string
 */

char *
quote_space_to_dash (str)
	char *str;
{
	char *ptr, *dst;
	static char buf[PATH_LEN];

	dst = buf;
	for (ptr = str; *ptr; ptr++) {
		if (*ptr == ' ') {
			*dst = '_';
		} else {
			*dst = *ptr;
		}
		dst++;
	}	
	*dst = '\0';

	return buf;
}

