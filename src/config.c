/*
 *  Project   : tin - a Usenet reader
 *  Module    : config.c
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 21-12-94, 15-08-96
 *  Notes     : Configuration file routines
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"patchlev.h"
#include	"tin.h"
#include	"conf.h"
#include	"menukeys.h"

static void expand_rel_abs_pathname P_((int line, int col, char *str));
static void highlight_option P_((int option));
static void print_option P_((int act_option));
static void refresh_config_page P_((int act_option));
static void unhighlight_option P_((int option));

/*
 *  read local & global configuration defaults
 */

int
read_config_file (file, global_file)
	char	*file;
	int	global_file;
{
	char	newnews_info[PATH_LEN];
	char	buf[LEN];
	FILE	*fp;

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
			if (match_integer (buf, "col_minihelp=", &col_minihelp, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_help=", &col_help, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_message=", &col_message, MAX_COLOR)) {
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
			if (match_integer (buf, "col_signature=", &col_signature, MAX_COLOR)) {
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
#ifdef M_UNIX
			if (match_boolean (buf, "keep_dead_articles=", &keep_dead_articles)) {
				break;
			}
#endif
			if (match_boolean (buf, "keep_posted_articles=", &keep_posted_articles)) {
				break;
			}
			break;
		case 'm':
			if (match_string (buf, "mail_mime_encoding=", mail_mime_encoding, sizeof (mail_mime_encoding))) {
				if (strcasecmp(mail_mime_encoding, "8bit") &&
					strcasecmp(mail_mime_encoding, "base64") &&
					strcasecmp(mail_mime_encoding, "7bit") &&  /* For CJK charsets(EUC-CN/JP/KR and others */
					strcasecmp(mail_mime_encoding, "quoted-printable")) {
					strcpy(mail_mime_encoding,"8bit");
				}
				break;
			}
			/* option to toggle 8bit char. in header of mail message */
			if (match_boolean (buf, "mail_8bit_header=", &mail_8bit_header)) {
				if (strcasecmp(mail_mime_encoding, "8bit"))
					mail_8bit_header=FALSE;
				break;
			}
			if (match_string (buf, "mm_charset=", mm_charset, sizeof (mm_charset))) {
				break;
			}
			if (match_string (buf, "motd_file_info=", motd_file_info, sizeof (motd_file_info))) {
				break;
			}
			if (match_boolean (buf, "mark_saved_read=", &mark_saved_read)) {
				break;
			}
#ifdef FORGERY
			if (match_string (buf, "mail_address=", mail_address, sizeof (mail_address))) {
				break;
			}
#endif
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
			if (match_string (buf, "post_mime_encoding=", post_mime_encoding, sizeof (post_mime_encoding))) {
				if (strcasecmp(post_mime_encoding, "8bit") &&
					strcasecmp(post_mime_encoding, "base64") &&
					strcasecmp(post_mime_encoding, "7bit") && /* perhaps necessary for EUC-JP/CN */
					strcasecmp(post_mime_encoding, "quoted-printable")) {
					strcpy(post_mime_encoding,"8bit");
				}
				break;
			}
/* option to toggle 8bit char. in header of news message */
			if (match_boolean (buf, "post_8bit_header=", &post_8bit_header)) {
				if (strcasecmp(post_mime_encoding, "8bit"))
					post_8bit_header=FALSE;
				break;
			}
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
			if (match_boolean (buf, "show_lines=" , &show_lines)) {
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
			if (match_integer (buf, "thread_articles=", &default_thread_arts, THREAD_MAX)) {
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
			if (match_boolean (buf, "use_mailreader_i=", &use_mailreader_i)) {
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
		default:
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
	if ((file_tmp = (char *) malloc (strlen (file)+5)) == NULL) {
		wait_message ("Out of memory!");
		return;
	}
	/* generate tmp-filename */
	strcpy (file_tmp, file);
	strcat (file_tmp, ".tmp");

	if ((fp = fopen (file_tmp, "w")) == (FILE *) 0) {
		wait_message (txt_filesystem_full_config_backup);
		/* free memory for tmp-filename */
		free (file_tmp);
		return;
	}

	if (! cmd_line) {
		wait_message (txt_saving);
	}

	if (! default_editor_format[0]) {
		strcpy (default_editor_format, EDITOR_FORMAT_ON);
	}
	fprintf (fp, "# %s %s %s configuration file\n#\n", progname, VERSION, RELEASEDATE);
	fprintf (fp, "# This file was automatically saved by tin\n#\n");
	fprintf (fp, "# Do not edit while tin is running, since all your changes to this file\n");
	fprintf (fp, "# would be overwritten when you leave tin.\n#\n");
	fprintf (fp, "############################################################################\n\n");

	fprintf (fp, "# directory where articles/threads are saved\n");
	fprintf (fp, "default_savedir=%s\n\n", default_savedir);

	fprintf (fp, "# if ON articles/threads with Archive-name: in mail header will\n");
	fprintf (fp, "# be automatically saved with the Archive-name & part/patch no.\n");
	fprintf (fp, "auto_save=%s\n\n", print_boolean (default_auto_save));

	fprintf (fp, "# if ON mark articles that are saved as read\n");
	fprintf (fp, "mark_saved_read=%s\n\n", print_boolean (mark_saved_read));

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

	fprintf (fp, "# If ON only save/print/pipe/mail unread articles (tagged articles excepted)\n");
	fprintf (fp, "process_only_unread=%s\n\n", print_boolean (process_only_unread));

	fprintf (fp, "# if ON confirm certain commands with y/n before executing\n");
	fprintf (fp, "confirm_action=%s\n\n", print_boolean (confirm_action));

	fprintf (fp, "# if ON confirm with y/n before quitting ('Q' never asks)\n");
	fprintf (fp, "confirm_to_quit=%s\n\n", print_boolean (confirm_to_quit));

	fprintf (fp, "# if ON use -> otherwise highlighted bar for selection\n");
	fprintf (fp, "draw_arrow=%s\n\n", print_boolean (draw_arrow_mark));

	fprintf (fp, "# if ON use inverse video for page headers at different levels\n");
	fprintf (fp, "inverse_okay=%s\n\n", print_boolean (inverse_okay));

	fprintf (fp, "# if ON put cursor at first unread art in group otherwise last art\n");
	fprintf (fp, "pos_first_unread=%s\n\n", print_boolean (pos_first_unread));

	fprintf (fp, "# if ON show only new/unread articles otherwise show all.\n");
	fprintf (fp, "show_only_unread=%s\n\n", print_boolean (default_show_only_unread));

	fprintf (fp, "# if ON show only subscribed to groups that contain unread articles.\n");
	fprintf (fp, "show_only_unread_groups=%s\n\n", print_boolean (show_only_unread_groups));

	fprintf (fp, "# if ON the TAB command will goto next unread article at article viewer level\n");
	fprintf (fp, "tab_goto_next_unread=%s\n\n", print_boolean (tab_goto_next_unread));

	fprintf (fp, "# if ON the SPACE command will goto next unread article at article viewer\n");
	fprintf (fp, "# level when the end of the article is reached (rn-style pager)\n");
	fprintf (fp, "space_goto_next_unread=%s\n\n", print_boolean (space_goto_next_unread));

	fprintf (fp, "# if ON a TAB command will be automatically done after the X command\n");
	fprintf (fp, "tab_after_X_selection=%s\n\n", print_boolean (tab_after_X_selection));

	fprintf (fp, "# if ON scroll full page of groups/articles otherwise half a page\n");
	fprintf (fp, "full_page_scroll=%s\n\n", print_boolean (full_page_scroll));

	fprintf (fp, "# if ON show the last line of the previous page as first line of next page\n");
	fprintf (fp, "show_last_line_prev_page=%s\n\n", print_boolean (show_last_line_prev_page));

	fprintf (fp, "# if ON ask user if read groups should all be marked read\n");
	fprintf (fp, "catchup_read_groups=%s\n\n", print_boolean (catchup_read_groups));

	fprintf (fp, "# if ON catchup group/thread when leaving with the left arrow key.\n");
	fprintf (fp, "group_catchup_on_exit=%s\n", print_boolean (group_catchup_on_exit));
	fprintf (fp, "thread_catchup_on_exit=%s\n\n", print_boolean (thread_catchup_on_exit));

	fprintf (fp, "# Thread articles on 0=(nothing) 1=(Subject) 2=(References) 3=(Both).\n");
	fprintf (fp, "thread_articles=%d\n\n", default_thread_arts);

	fprintf (fp, "# if ON show group description text after newsgroup name at\n");
	fprintf (fp, "# group selection level\n");
	fprintf (fp, "show_description=%s\n\n", print_boolean (show_description));

	fprintf (fp, "# part of from field to display 0) none 1) address 2) full name 3) both\n");
	fprintf (fp, "show_author=%d\n\n", default_show_author);

	fprintf (fp, "# sort articles by 0=(nothing) 1=(Subject descend) 2=(Subject ascend)\n");
	fprintf (fp, "# 3=(From descend) 4=(From ascend) 5=(Date descend) 6=(Date ascend).\n");
	fprintf (fp, "sort_article_type=%d\n\n", default_sort_art_type);

	fprintf (fp, "# (-m) directory where articles/threads are saved in mailbox format\n");
	fprintf (fp, "default_maildir=%s\n\n", default_maildir);

	fprintf (fp, "# if ON save mail to a MMDF style mailbox (default is normal mbox format)\n");
	fprintf (fp, "save_to_mmdf_mailbox=%s\n\n", print_boolean (save_to_mmdf_mailbox));

	fprintf (fp, "# If ON, the realname in the X-Comment-To header is displayed\n");
	fprintf (fp, "show_xcommentto=%s\n\n", print_boolean(show_xcommentto));

	fprintf (fp, "# If ON X-Commento-To name is displayed in the upper-right corner,\n");
	fprintf (fp, "# if OFF below the Summary-Header\n");
	fprintf (fp, "highlight_xcommentto=%s\n\n", print_boolean(highlight_xcommentto));

	fprintf (fp, "# if ON print all of mail header otherwise Subject: & From: lines\n");
	fprintf (fp, "print_header=%s\n\n", print_boolean (print_header));

	fprintf (fp, "# print program with parameters used to print articles/threads\n");
	fprintf (fp, "default_printer=%s\n\n", default_printer);

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
	fprintf (fp, "# ie. elm interactive          :    elm -i %%F -s \"%%S\" \"%%T\"\n");
	fprintf (fp, "default_mailer_format=%s\n\n", default_mailer_format);

	fprintf (fp, "# interactive mailreader: if ON mailreader will be invoked earlier for\n");
	fprintf (fp, "# reply so you can use more of its features (eg. MIME, pgp, ...)\n");
	fprintf (fp, "# this option has to suit default_mailer_format\n");
	fprintf (fp, "use_mailreader_i=%s\n\n", print_boolean (use_mailreader_i));

	fprintf (fp, "# show number of lines of first unread article in thread listing (ON/OFF)\n");
	fprintf (fp, "show_lines=%s\n\n", print_boolean(show_lines));

	fprintf (fp, "# if ON remove ~/.article after posting.\n");
	fprintf (fp, "unlink_article=%s\n\n", print_boolean (unlink_article));

#ifdef M_UNIX
	fprintf (fp, "# if ON keep all failed postings in ~/dead.articles\n");
	fprintf (fp, "keep_dead_articles=%s\n\n", print_boolean (keep_dead_articles));
#endif

	fprintf (fp, "# if ON keep all postings in ~/Mail/posted\n");
	fprintf (fp, "keep_posted_articles=%s\n\n", print_boolean (keep_posted_articles));

	fprintf (fp, "# Signature path (random sigs)/file to be used when posting/replying\n");
	fprintf (fp, "# default_sigfile=file       appends file as signature\n");
	fprintf (fp, "# default_sigfile=! command  executes external command to generate a signature\n");
	fprintf (fp, "# default_sigfile=--none     don't append a signature\n");
	fprintf (fp, "default_sigfile=%s\n\n", default_sigfile);

	fprintf (fp, "# if ON prepend the signature with dashes '\\n-- \\n'\n");
	fprintf (fp, "sigdashes=%s\n\n", print_boolean (sigdashes));

	fprintf (fp, "# turn off advertising in header (X-Newsreader/X-Mailer)\n");
	fprintf (fp, "no_advertising=%s\n\n", print_boolean (no_advertising));

	fprintf (fp, "# time interval in seconds between rereading the active file\n");
	fprintf (fp, "reread_active_file_secs=%d\n\n", reread_active_file_secs);

	fprintf (fp, "# characters used in quoting to followups and replys.\n");
	fprintf (fp, "# '_' is replaced by ' ', %%s, %%S are replaced by author's initials.\n");
	fprintf (fp, "quote_chars=%s\n\n", quote_space_to_dash (quote_chars));

	fprintf (fp, "# Format of quote line when mailing/posting/followingup an article\n");
	fprintf (fp, "# %%A Address    %%D Date   %%F Addr+Name   %%G Groupname   %%M MessageId\n");
	fprintf (fp, "# %%N Full Name  %%C First Name\n");
	fprintf (fp, "news_quote_format=%s\n", news_quote_format);
	fprintf (fp, "mail_quote_format=%s\n", mail_quote_format);
	fprintf (fp, "xpost_quote_format=%s\n\n", xpost_quote_format);

	fprintf (fp, "# if ON automatically put your name in the Cc: field when mailing an article\n");
	fprintf (fp, "auto_cc=%s\n\n", print_boolean (auto_cc));

	fprintf (fp, "# if ON automatically put your name in the Bcc: field when mailing an article\n");
	fprintf (fp, "auto_bcc=%s\n\n", print_boolean (auto_bcc));

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

	fprintf (fp, "# if ON a screen redraw will always be done after certain external commands\n");
	fprintf (fp, "force_screen_redraw=%s\n\n", print_boolean (force_screen_redraw));

	fprintf (fp, "# if ON use the builtin mini inews otherwise use an external inews program\n");
	fprintf (fp, "use_builtin_inews=%s\n\n", print_boolean (use_builtin_inews));

	fprintf (fp, "# if ON automatically list thread when entering it using right arrow key.\n");
	fprintf (fp, "auto_list_thread=%s\n\n", print_boolean (auto_list_thread));

	fprintf (fp, "# If ON enable mouse key support on xterm terminals\n");
	fprintf (fp, "use_mouse=%s\n\n", print_boolean (use_mouse));

	fprintf (fp, "# If ON strip blanks from end of lines to speedup display on slow terminals\n");
	fprintf (fp, "strip_blanks=%s\n\n", print_boolean (strip_blanks));

	fprintf (fp, "# Maximum length of the names of newsgroups displayed\n");
	fprintf (fp, "groupname_max_length=%d\n\n", groupname_max_length);

	fprintf (fp, "# If ON show a mini menu of useful commands at each level\n");
	fprintf (fp, "beginner_level=%s\n\n", print_boolean (beginner_level));

	fprintf (fp, "# Num of days a short term filter will be active\n");
	fprintf (fp, "default_filter_days=%d\n\n", default_filter_days);

#ifdef HAVE_COLOR
	fprintf (fp, "# if ON using ansi-color\n");
	fprintf (fp, "use_color=%s\n\n", print_boolean (use_color_tinrc));

	fprintf (fp, "# For coloradjust use the following numbers\n");
	fprintf (fp, "#  0-black       1-red         2-green        3-brown\n");
	fprintf (fp, "#  4-blue        5-pink        6-cyan         7-white\n");
	fprintf (fp, "# These are *only* for foreground:\n");
	fprintf (fp, "#  8-gray        9-lightred   10-lightgreen  11-yellow\n");
	fprintf (fp, "# 12-lightblue  13-lightpink  14-lightcyan   15-lightwhite\n\n");

	fprintf (fp, "# Standard foreground color\n");
	fprintf (fp, "col_normal=%d\n\n", col_normal);

	fprintf (fp, "# Standard-Background-Color\n");
	fprintf (fp, "col_back=%d\n\n", col_back);

	fprintf (fp, "# Color for inverse text\n");
	fprintf (fp, "col_invers=%d\n\n", col_invers);

	fprintf (fp, "# Color of textlines\n");
	fprintf (fp, "col_text=%d\n\n", col_text);

	fprintf (fp, "# Color of mini help menu\n");
	fprintf (fp, "col_minihelp=%d\n\n", col_minihelp);

	fprintf (fp, "# Color of help pages\n");
	fprintf (fp, "col_help=%d\n\n", col_help);

	fprintf (fp, "# Color of messages in last line\n");
	fprintf (fp, "col_message=%d\n\n", col_message);

	fprintf (fp, "# Color of quotelines\n");
	fprintf (fp, "col_quote=%d\n\n", col_quote);

	fprintf (fp, "# Color of headerlines\n");
	fprintf (fp, "col_head=%d\n\n", col_head);

	fprintf (fp, "# Color of article subject\n");
	fprintf (fp, "col_subject=%d\n\n", col_subject);

	fprintf (fp, "# Color of response counter\n");
	fprintf (fp, "col_response=%d\n\n", col_response);

	fprintf (fp, "# Color of sender (From:)\n");
	fprintf (fp, "col_from=%d\n\n", col_from);

	fprintf (fp, "# Color of Help/Mail-Sign\n");
	fprintf (fp, "col_title=%d\n\n", col_title);

	fprintf (fp, "# Color of signature\n");
	fprintf (fp, "col_signature=%d\n\n", col_signature);
#endif

#ifdef FORGERY
	if (*mail_address) {
		fprintf (fp, "# user's mail address, if not username@host\n");
		fprintf (fp, "mail_address=%s\n\n",mail_address);
	}
#endif

	fprintf (fp, "# charset supported locally  which is also used for MIME header and\n");
	fprintf (fp, "# Content-Type header unless news and mail need to be encoded in other\n");
	fprintf (fp, "# charsets as in ISO-2022-KR encoding of EUC-KR in mail message.\n");
	fprintf (fp, "# If not set, the value of the environment variable MM_CHARSET is used.\n");
	fprintf (fp, "# Set to US-ASCII or compile time default if neither of them is defined.\n");
	fprintf (fp, "# If MIME_STRICT_CHARSET is defined at compile-time, charset other than\n");
	fprintf (fp, "# mm_charset is considered not displayable and represented as '?'.\n");
	fprintf (fp, "mm_charset=%s\n\n", mm_charset);

	fprintf (fp, "# MIME encoding (8bit, base64, quoted-printable, 7bit) of the body\n");
	fprintf (fp, "# for mails and posts, if necessary. QP is efficient for most European\n");
	fprintf (fp, "# character sets (ISO-8859-X) with small fraction of non-US-ASCII chars,\n");
	fprintf (fp, "# while Base64 is more efficient for most 8bit East Asian charsets.\n");
	fprintf (fp, "# For EUC-KR, 7bit encoding specifies that EUC charsets be converted\n");
	fprintf (fp, "# to corresponding ISO-2022-KR. The same may be true of EUC-JP/CN.\n");
	fprintf (fp, "# For other charsets used in Japan and China, it seems more complicated.\n");
	fprintf (fp, "# Korean users should set post_mime_encoding to 8bit and mail_mime_encoding\n");
	fprintf (fp, "# to 7bit. With mm_charset to EUC-KR, post_mime_encoding set to 7bit does\n");
	fprintf (fp, "# NOT lead to encoding of EUC-KR into ISO-2022-KR in news-postings since\n");
	fprintf (fp, "# it's never meant to be used for Usenet news. Perhaps, it's not the case\n");
	fprintf (fp, "# for EUC-JP and EUC-CN.\n");
	fprintf (fp, "# Handling of Chinese and Japanese characters is not yet implemented.\n");
	fprintf (fp, "post_mime_encoding=%s\n", post_mime_encoding);
	fprintf (fp, "mail_mime_encoding=%s\n\n", mail_mime_encoding);

	fprintf (fp, "# if ON, 8bit characters in news posting is NOT encoded.\n");
	fprintf (fp, "# default is OFF. Thus 8bit character is encoded by default.\n");
	fprintf (fp, "# 8bit chars in header is encoded regardless of the value of this\n");
	fprintf (fp, "# parameter unless post_mime_encoding is 8bit as well. \n");
	fprintf (fp, "post_8bit_header=%s\n\n", print_boolean(post_8bit_header));

	fprintf (fp, "# if ON, 8bit characters in mail message is NOT encoded.\n");
	fprintf (fp, "# default is OFF. Thus 8bit character is encoded by default.\n");
	fprintf (fp, "# 8bit chars in header is encoded regardless of the value of this parameter\n");
	fprintf (fp, "# unless mail_mime_encoding is 8bit as well. Note that RFC 1552/1651/1652\n");
	fprintf (fp, "# prohibit 8bit characters in mail header so that you are advised NOT to\n");
	fprintf (fp, "# turn it ON unless you have some compelling reason as is the case of\n");
	fprintf (fp, "# Korean users with localized sendmail.\n");
	fprintf (fp, "mail_8bit_header=%s\n\n", print_boolean(mail_8bit_header));

#ifdef HAVE_METAMAIL
 	fprintf (fp, "# if ON metamail can/will be used to display MIME articles\n");
 	fprintf (fp, "use_metamail=%s\n\n", print_boolean (use_metamail));

 	fprintf (fp, "# if ON tin will ask before using metamail to display MIME messages\n");
 	fprintf (fp, "# this only occurs, if use_metamail is also switched ON\n");
 	fprintf (fp, "ask_for_metamail=%s\n\n", print_boolean (ask_for_metamail));
#endif

#ifdef HAVE_KEYPAD
	fprintf (fp, "# If ON enable scroll keys on terminals that support it\n");
	fprintf (fp, "use_keypad=%s\n\n", print_boolean (use_keypad));
#endif

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

	fprintf (fp, "# host & time info used for detecting new groups (don't touch)\n");
	if (! num_newnews) {
		fprintf (fp, "newnews=%s %ld\n", new_newnews_host, new_newnews_time);
	} else {
		for (i = 0 ; i < num_newnews ; i++) {
			fprintf (fp, "newnews=%s %ld\n", newnews[i].host, newnews[i].time);
		}
	}
	if (ferror (fp) | fclose (fp)){
		wait_message (txt_filesystem_full_config);
		/* free memory for tmp-filename */
		free (file_tmp);
		return;
	} else {
		rename_file (file_tmp, file);
		chmod (file, 0600);
		/* free memory for tmp-filename */
		free (file_tmp);
	}
}

int first_option_on_screen;
/*
 * FIXME put in tin.h (or find a better solution). See also: prompt.c
 */
#define option_lines_per_page (cLINES - INDEX_TOP - 3)

int actual_option_page = 0;

/*
 * Display option in a line containing option number, explaining text,
 * and option value. Note that "act_option" needs to be the option number
 * in the option_table array, which is different from the option number
 * displayed to the user (the latter one is one greater since counting
 * starts with one instead of zero).
 * FIXME make consistent with other functions using the option number
 *       shown on the screen.
 */

static void
print_option (act_option)
	int act_option;
{
	printf("%3d. %s ", act_option + 1, option_table[act_option].option_text);
	switch (option_table[act_option].var_type) {
		case OPT_ON_OFF:
			printf("%s ", print_boolean(*((int *)option_table[act_option].variable)));
			break;
		case OPT_LIST:
			printf("%s", option_table[act_option].opt_list[*((int *)option_table[act_option].variable)]);
			break;
		case OPT_STRING:
			printf("%-.*s", cCOLS - (int) strlen((char *) option_table[act_option].option_text) - OPT_ARG_COLUMN - 3, (char *) option_table[act_option].variable);
			break;
		case OPT_NUM:
			printf("%d", *((int *) option_table[act_option].variable));
			break;
		case OPT_CHAR:
			/* grrr... who the heck defined art_marked_* as int? */
			printf("%c", *((int *) option_table[act_option].variable));
		break;
	}
}

static void
highlight_option (option)
	int option;
{
	MoveCursor (INDEX_TOP + (option - 1) % option_lines_per_page, 0);
	my_fputs ("->", stdout);
	fflush (stdout);
	MoveCursor (cLINES, 0);
}

static void
unhighlight_option (option)
	int option;
{
	MoveCursor (INDEX_TOP + (option - 1) % option_lines_per_page, 0);
	my_fputs ("  ", stdout);
	fflush (stdout);
}

static void
refresh_config_page (act_option)
	int act_option;
{
	int desired_page;

	/* determine on which page act_option would be */
	desired_page = (int) (act_option - 1) / option_lines_per_page;

	if (desired_page != actual_option_page)
	{
		show_config_page (desired_page);
		actual_option_page = desired_page;
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
/*	int filter_changed = FALSE; */
	int change_option = FALSE;
	int original_on_off_value, original_list_value;
	int option, old_option;
	int ret_code = NO_FILTERING;
	int mime_type = 0;

#ifdef SIGTSTP
	RETSIGTYPE (*susp)(SIG_ARGS);

	susp = (RETSIGTYPE (*)(SIG_ARGS)) 0;

	if (do_sigtstp) {
		susp = sigdisp (SIGTSTP, SIG_DFL);
	}
#endif

	actual_option_page = -1;
	option = 1;

	set_xclick_off ();
	forever {

#ifdef SIGTSTP
		if (do_sigtstp) {
			sigdisp (SIGTSTP, config_suspend);
		}
#endif
	 	refresh_config_page (option);
	 	highlight_option (option);

		MoveCursor (cLINES, 0);
		ch = ReadCh ();

		/*
		 * convert arrow key codes to "normal" codes
		 */
		switch (ch) {

#ifndef WIN32
			case ESC:	/* common arrow keys */
#	ifdef HAVE_KEY_PREFIX
			case KEY_PREFIX:
#	endif
				switch (get_arrow_key ()) {
#endif
					case KEYMAP_UP:
						ch = iKeyConfigUp;
						break;

					case KEYMAP_DOWN:
						ch = iKeyConfigDown;
						break;

					case KEYMAP_HOME:
						ch = iKeyConfigHome;
						break;

					case KEYMAP_END:
						ch = iKeyConfigEnd;
						break;

					case KEYMAP_PAGE_UP:
						ch = iKeyConfigPageUp;
						break;

					case KEYMAP_PAGE_DOWN:
						ch = iKeyConfigPageDown;
						break;
#ifndef WIN32
				} /* switch (get_arrow_key ()) */
				break;
#endif
		}	/* switch (ch) */

		switch (ch) {
			case iKeyQuit:
				write_config_file (local_config_file);
				/* FALLTHRU */
			case iKeyConfigNoSave:
/* FIXME who did this? what for? filter_changed is not changed ever */
/*
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
*/
				clear_note_area ();
#ifdef SIGTSTP
				if (do_sigtstp) {
					sigdisp (SIGTSTP, susp);
				}
#endif
				return ret_code;

			case iKeyConfigUp:
				unhighlight_option (option);
				option--;
				if (option < 1)
					option = LAST_OPT;
				refresh_config_page (option);
				highlight_option (option);
				break;

			case iKeyConfigDown:
				unhighlight_option (option);
				option++;
				if (option > LAST_OPT)
					option = 1;
				refresh_config_page (option);
				highlight_option (option);
				break;

			case iKeyConfigHome:
				unhighlight_option (option);
				option = 1;
				refresh_config_page (option);
				highlight_option (option);
				break;

			case iKeyConfigEnd:
				unhighlight_option (option);
				option = LAST_OPT;
				refresh_config_page (option);
				highlight_option (option);
				break;

			case iKeyConfigPageUp:
				unhighlight_option (option);
				option -= option_lines_per_page;
				if (option < 1)
					option = LAST_OPT;
				refresh_config_page (option);
				highlight_option (option);
				break;

			case iKeyConfigPageDown:
				unhighlight_option (option);
				option += option_lines_per_page;
				if (option > LAST_OPT)
					option = 1;
				refresh_config_page (option);
				highlight_option (option);
				break;

			case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				unhighlight_option (option);
				old_option = option;
				option = prompt_num (ch, "Enter option number> ");
				if (option < 1 || option > LAST_OPT) {
					option = old_option;
					break;
				}
				refresh_config_page (option);
				/* FALLTHROUGH */

			case iKeyConfigSelect:
			case iKeyConfigSelect2:
				change_option = TRUE;
				break;
		} /* switch (ch) */

		if (change_option) {
			switch (option_table[option - 1].var_type) {
				case OPT_ON_OFF:
					original_on_off_value = *((int *) option_table[option - 1].variable);
					prompt_on_off (INDEX_TOP + (option - 1) % option_lines_per_page,
						OPT_ARG_COLUMN, option_table[option - 1].variable,
						option_table[option - 1].help_text,
						option_table[option - 1].option_text
						);
					/*
					 * some options need further action to take effect
					 */
	    				switch (option) {
	    					/* show mini help menu */
		    				case OPT_BEGINNER_LEVEL:
		    					if (beginner_level != original_on_off_value)
								(void) set_win_size (&cLINES, &cCOLS);
							break;

						/* show all arts or just new/unread arts */
						case OPT_DEFAULT_SHOW_ONLY_UNREAD:
							if (default_show_only_unread != original_on_off_value && group != (struct t_group *) 0) {
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

						/* draw -> / highlighted bar */
						case OPT_DRAW_ARROW_MARK:
							unhighlight_option (option);
							if (draw_arrow_mark == FALSE && inverse_okay == FALSE) {
								inverse_okay = TRUE;
								if (OPT_INVERSE_OKAY > first_option_on_screen && OPT_INVERSE_OKAY < first_option_on_screen + option_lines_per_page) {
									MoveCursor (INDEX_TOP + (OPT_INVERSE_OKAY - 1) % option_lines_per_page, 3);
									print_option (OPT_INVERSE_OKAY - 1);
								}
							}
							break;

						/* draw inversed screen header lines */
						/* draw inversed group/article/option line if draw_arrow_mark is OFF */
 	   					case OPT_INVERSE_OKAY:
							unhighlight_option (option);
							if (draw_arrow_mark == FALSE && inverse_okay == FALSE) {
								draw_arrow_mark = TRUE;	/* we don't want to navigate blindly */
								if (OPT_DRAW_ARROW_MARK > first_option_on_screen && OPT_DRAW_ARROW_MARK <= first_option_on_screen + option_lines_per_page + 1) {
									MoveCursor (INDEX_TOP + (OPT_DRAW_ARROW_MARK - 1) % option_lines_per_page, 3);
									print_option (OPT_DRAW_ARROW_MARK - 1);
								}
							}
							break;

						case OPT_MAIL_8BIT_HEADER:
							if (strcasecmp(mail_mime_encoding, "8bit")) {
								mail_8bit_header = FALSE;
								MoveCursor (INDEX_TOP + (OPT_MAIL_8BIT_HEADER - 1) % option_lines_per_page, 3);
								print_option (OPT_MAIL_8BIT_HEADER - 1);
							}
							break;

						case OPT_POST_8BIT_HEADER:
							if (strcasecmp(post_mime_encoding, "8bit")) {
								post_8bit_header = FALSE;
								MoveCursor (INDEX_TOP + (OPT_POST_8BIT_HEADER - 1) % option_lines_per_page, 3);
								print_option (OPT_POST_8BIT_HEADER - 1);
							}
							break;

						/* show newsgroup description text next to newsgroups */
						case OPT_SHOW_DESCRIPTION:
							if (show_description) {	/* force reread of newgroups file */
								read_newsgroups_file ();
								clear_message ();
							} else {
								set_groupname_len (FALSE);
							}
							break;

#ifdef HAVE_COLOR
						/* use ANSI color */
						case OPT_USE_COLOR:
							use_color = use_color_tinrc;
							break;
#endif

						/*
						 * the following do not need further action (if I'm right)
						 *
						 * case OPT_AUTO_BCC:			case OPT_AUTO_CC:
						 * case OPT_AUTO_LIST_THREAD:		case OPT_CATCHUP_READ_GROUPS:
						 * case OPT_CONFIRM_ACTION:		case OPT_CONFIRM_TO_QUIT:
						 * case OPT_DEFAULT_AUTO_SAVE:		case OPT_DEFAULT_BATCH_SAVE:
						 * case OPT_FORCE_SCREEN_REDRAW:	case OPT_FULL_PAGE_SCROLL:
						 * case OPT_GROUP_CATCHUP_ON_EXIT:	case OPT_HIGHLIGHT_XCOMMENTTO:
						 * case OPT_KEEP_POSTED_ARTICLES:	case OPT_MARK_SAVED_READ:
						 * case OPT_NO_ADVERTISING:		case OPT_POS_FIRST_UNREAD:
						 * case OPT_PRINT_HEADER:		case OPT_PROCESS_ONLY_UNREAD:
						 * case OPT_SAVE_TO_MMDF_MAILBOX:	case OPT_SHOW_LINES:
						 * case OPT_SHOW_LAST_LINE_PREV_PAGE:	case OPT_SHOW_ONLY_UNREAD_GROUPS:
						 * case OPT_SHOW_XCOMMENTTO:		case OPT_SIGDASHES:
						 * case OPT_SPACE_GOTO_NEXT_UNREAD:	case OPT_START_EDITOR_OFFSET:
						 * case OPT_STRIP_BLANKS:		case OPT_TAB_AFTER_X_SELECTION:
						 * case OPT_TAB_GOTO_NEXT_UNREAD:	case OPT_THREAD_CATCHUP_ON_EXIT:
						 * case OPT_UNLINK_ARTICLE:		case OPT_USE_BUILTIN_INEWS:
						 * case OPT_USE_MAILREADER_I:		case OPT_USE_MOUSE:
#ifdef HAVE_KEYPAD
						 * case OPT_USE_KEYPAD:
#endif
#ifdef HAVE_METAMAIL
						 * case OPT_ASK_FOR_METAMAIL:	case OPT_USE_METAMAIL:
#endif
#ifdef M_UNIX
						 * case OPT_KEEP_DEAD_ARTICLES:
#endif
						 * 	break;
						 */
					} /* switch (option) */
					break;

				case OPT_LIST:
					original_list_value = *((int *) option_table[option - 1].variable);
					*((int *) option_table[option - 1].variable) = prompt_list (INDEX_TOP + (option - 1) % option_lines_per_page,
								OPT_ARG_COLUMN,
								*((int *) option_table[option - 1].variable), /*default_post_proc_type,*/
								option_table[option - 1].help_text,
								option_table[option - 1].option_text,
								option_table[option - 1].opt_list,
								option_table[option - 1].opt_count
								);

					/*
					 * some options need further action to take effect
					 */
					switch (option) {
						/* threading strategy for groups except those in ~/.tin/unthreaded */
						case OPT_DEFAULT_THREAD_ARTS:
							/*
							 * If the threading strategy has changed, fix things
							 * so that rethreading will occur
							 */
							if (default_thread_arts != original_list_value && group != (struct t_group *) 0) {
								group->attribute->thread_arts = default_thread_arts;
								make_threads (group, TRUE);
								find_base (group);
							}
							clear_message ();
							break;

						case OPT_DEFAULT_POST_PROC_TYPE:
							proc_ch_default = get_post_proc_type (default_post_proc_type);
							break;

			    			/*
	    					 * the following don't need any further action (if i'm right)
	    					 *
#ifdef HAVE_COLOR
						 * case OPT_COL_BACK:		case OPT_COL_FROM:
						 * case OPT_COL_HEAD:		case OPT_COL_HELP:
						 * case OPT_COL_INVERS:		case OPT_COL_MESSAGE:
						 * case OPT_COL_MINIHELP:	case OPT_COL_NORMAL:
						 * case OPT_COL_QUOTE:		case OPT_COL_RESPONSE:
						 * case OPT_COL_SIGNATURE:	case OPT_COL_SUBJECT:
						 * case OPT_COL_TEXT:		case OPT_COL_TITLE:
#endif
						 * case OPT_DEFAULT_SHOW_AUTHOR:	case OPT_DEFAULT_SORT_ART_TYPE:
						 *	break;
						 */

					} /* switch (option) */
					break;

				case OPT_STRING:
					switch (option) {
						case OPT_DEFAULT_EDITOR_FORMAT:
						case OPT_DEFAULT_MAILER_FORMAT:
						case OPT_MM_CHARSET:
						case OPT_MAIL_QUOTE_FORMAT:
						case OPT_NEWS_QUOTE_FORMAT:
						case OPT_QUOTE_CHARS:
						case OPT_XPOST_QUOTE_FORMAT:
#ifdef FORGERY
						case OPT_MAIL_ADDRESS:
#endif
							prompt_option_string (option);
							break;

						case OPT_DEFAULT_MAILDIR:
						case OPT_DEFAULT_PRINTER:
						case OPT_DEFAULT_SAVEDIR:
						case OPT_DEFAULT_SIGFILE:
#ifdef M_AMIGA
							if (tin_bbs_mode) break;
#endif
							prompt_option_string (option);
							expand_rel_abs_pathname (INDEX_TOP + (option - 1) % option_lines_per_page,
								OPT_ARG_COLUMN + (int) strlen (option_table[option - 1].option_text),
								option_table[option - 1].variable
								);
							break;

						/*
						 * special case: mime encoding options. The encoding
						 * is stored as a string, but we want to select it
						 * from a predefined list.
						 */
						case OPT_MAIL_MIME_ENCODING:
						case OPT_POST_MIME_ENCODING:
							for (i=0; i<4; i++) {
								if (! strcasecmp (option_table[option - 1].variable, txt_mime_types[i])) {
									mime_type = i;
								}
							}
							mime_type = prompt_list (INDEX_TOP + (option - 1) % option_lines_per_page,
										OPT_ARG_COLUMN,
										mime_type,
										option_table[option - 1].help_text,
										option_table[option - 1].option_text,
										option_table[option - 1].opt_list,
										option_table[option - 1].opt_count
										);
							strcpy (option_table[option - 1].variable, txt_mime_types[mime_type]);

							/* do not use 8 bit headers if mime encoding is not 8bit; ask J. Shin why */
							if (strcasecmp(txt_mime_types[mime_type], "8bit")) {
								if (option == OPT_POST_MIME_ENCODING) {
									post_8bit_header = FALSE;
									if ((OPT_POST_8BIT_HEADER > first_option_on_screen) &&
											(OPT_POST_8BIT_HEADER <= first_option_on_screen + option_lines_per_page + 1)) {
										MoveCursor (INDEX_TOP + (OPT_POST_8BIT_HEADER - 1) % option_lines_per_page, 3);
										print_option (OPT_POST_8BIT_HEADER - 1);
									}
								} else {
									mail_8bit_header = FALSE;
									if ((OPT_MAIL_8BIT_HEADER > first_option_on_screen) &&
											(OPT_POST_8BIT_HEADER <= first_option_on_screen + option_lines_per_page + 1)) {
										MoveCursor (INDEX_TOP + (OPT_MAIL_8BIT_HEADER - 1) % option_lines_per_page, 3);
										print_option (OPT_MAIL_8BIT_HEADER - 1);
									}
								}
							}
							break;

					} /* switch (option) */
					break;

				case OPT_NUM:
					switch (option) {
						case OPT_REREAD_ACTIVE_FILE_SECS:
						case OPT_GROUPNAME_MAX_LENGTH:
						case OPT_DEFAULT_FILTER_DAYS:
							prompt_option_num (option);
							break;
					} /* switch (option) */
					break;

				case OPT_CHAR:
					switch (option) {
						case OPT_ART_MARKED_DELETED:
						case OPT_ART_MARKED_INRANGE:
						case OPT_ART_MARKED_RETURN:
						case OPT_ART_MARKED_SELECTED:
						case OPT_ART_MARKED_UNREAD:
							prompt_option_char (option);
							break;
					} /* switch (option) */
					break;

			} /* switch (option_table[option - 1].var_type) */
			change_option = FALSE;
			show_menu_help (txt_select_config_file_option);
		} /* if (change_option) */
	} /* forever */
} /* change_config_file */

/*
 *  expand ~/News to /usr/username/News and print to screen
 */

static void
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
	sprintf (&buf[0], "%-.*s", cCOLS - col - 1, str);
	MoveCursor (line, col);
	CleartoEOLN ();
	my_fputs (&buf[0], stdout);
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
				fprintf(stderr, "\n%s%d out of range (0 - %d). Reset to 0", pat, *dst, maxlen);
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
	return (value ? txt_onoff[(int) TRUE] : txt_onoff[(int) FALSE]);
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

/*
 * display current configuration page
 * page numbering starts with zero; argument page_no is expected to be valid
 */

void
show_config_page (page_no)
	int page_no;
{
	int i, lines_to_print = option_lines_per_page;

	ClearScreen ();
	center_line (0, TRUE, txt_options_menu);

	first_option_on_screen = page_no * option_lines_per_page;
	/*
	 * on last page, there need not be option_lines_per_page options
	 */
	if (first_option_on_screen + option_lines_per_page > LAST_OPT)
		lines_to_print = LAST_OPT - first_option_on_screen;

	for (i = 0;i < lines_to_print;i++)
	{
		MoveCursor (INDEX_TOP + i, 3);
		print_option (first_option_on_screen + i);
	}
	fflush (stdout);

	show_menu_help (txt_select_config_file_option);
	MoveCursor (cLINES, 0);
}
