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

#include	"version.h"
#include	"tin.h"
#include	"tincfg.h"
#include	"tcurses.h"
#include	"menukeys.h"

#define LAST_OPTION_PAGE ((LAST_OPT - 1) / option_lines_per_page)

static int match_list ( char *line, constext *pat, constext *const *table, size_t tablelen, int *dst);
static void expand_rel_abs_pathname (int line, int col, char *str);
static void print_any_option (int the_option);
static void show_config_page (int page_no);
static char **ulBuildArgv(char *cmd, int *new_argc);

enum state { IGNORE, CHECK, UPGRADE };

/*
 * If we don't find a tin-1.3 unoff tag line at the top of the rc file,
 * give the user some upgrade guidance and silently update variables where
 * necessary. We use a simple state mechanism, starting with CHECK for the
 * 1st line, then switch to UPGRADE or IGNORE accordingly.
 */
static int
check_upgrade(
	char *buf)
{
	if (strncmp(buf, "# tin-unoff configuration file V" TINRC_VERSION, 35) == 0)
		return(IGNORE);
	else {
		my_fprintf(stderr, "\n\nYou are upgrading to tin %s from an earlier version.\n", VERSION);
		my_fprintf(stderr, "Some values in your configuration file have changed\n");
		my_fprintf(stderr, "Read WHATSNEW, etc.....\n\n");
		my_fprintf(stderr, txt_return_key);
		ReadCh();
		return(UPGRADE);
	}
}

/*
 *  read local & global configuration defaults
 */

int
read_config_file (
	char	*file,
	int	global_file)
{
	char	newnews_info[PATH_LEN];
	char	buf[LEN];
	int	upgrade = CHECK;
	FILE	*fp;

	if ((fp = fopen (file, "r")) == (FILE *) 0)
		return FALSE;

	if (SHOW_UPDATE) {
		if (global_file)
			wait_message (txt_reading_global_config_file);
		else
			wait_message (txt_reading_config_file);
	}

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		if (buf[0] == '#' || buf[0] == '\n') {
			if (upgrade == CHECK)
				upgrade = check_upgrade(buf);
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
			if (match_boolean (buf, "alternative_handling=", &alternative_handling)) {
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
			if (match_integer (buf, "col_invers_bg=", &col_invers_bg, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_invers_fg=", &col_invers_fg, MAX_COLOR)) {
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
			if (match_integer (buf, "col_newsheaders=", &col_newsheaders, MAX_COLOR)) {
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
			if (match_integer (buf, "col_markstar=", &col_markstar, MAX_COLOR)) {
				break;
			}
			if (match_integer (buf, "col_markdash=", &col_markdash, MAX_COLOR)) {
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
				joinpath (posted_msgs_file, default_maildir, POSTED_FILE);
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
			if (match_integer (buf, "default_filter_kill_header=", &default_filter_kill_header, FILTER_LINES)) {
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
			if (match_integer (buf, "default_filter_select_header=", &default_filter_select_header, FILTER_LINES)) {
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
			if (match_boolean (buf, "display_mime_header_asis=", &display_mime_header_asis)) {
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

		case 'l':
#ifdef LOCAL_CHARSET
			if (match_boolean (buf, "local_charset=", &use_local_charset)) {
				break;
			}
#endif
			break;

		case 'm':
			if (match_list (buf, "mail_mime_encoding=", txt_mime_types, NUM_MIME_TYPES, &mail_mime_encoding)) {
				break;
			}
			/* option to toggle 8bit char. in header of mail message */
			if (match_boolean (buf, "mail_8bit_header=", &mail_8bit_header)) {
				if (strcasecmp(txt_mime_types[mail_mime_encoding], txt_8bit))
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
			if (match_string (buf, "mail_address=", mail_address, sizeof (mail_address))) {
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
			/* pick which news headers to display */
			if (match_string(buf, "news_headers_to_display=", news_headers_to_display, sizeof (news_headers_to_display))) {
				news_headers_to_display_array = ulBuildArgv(news_headers_to_display, &num_headers_to_display);
				break;
			}
			/* pick which news headers to NOT display -- swp */
			if (match_string(buf, "news_headers_to_not_display=", news_headers_to_not_display, sizeof (news_headers_to_not_display))) {
				news_headers_to_not_display_array = ulBuildArgv(news_headers_to_not_display, &num_headers_to_not_display);
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
			if (match_list (buf, "post_mime_encoding=", txt_mime_types, NUM_MIME_TYPES, &post_mime_encoding)) {
				break;
			}
			/* option to toggle 8bit char. in header of news message */
			if (match_boolean (buf, "post_8bit_header=", &post_8bit_header)) {
				if (strcasecmp(txt_mime_types[post_mime_encoding], txt_8bit))
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
			if (match_boolean (buf, "prompt_followupto=", &prompt_followupto)) {
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
			if (match_integer (buf, "strip_bogus=", &strip_bogus, BOGUS_ASK)) {
				break;
			}
			if (match_boolean (buf, "strip_newsrc=", &strip_newsrc)) {
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
				/* Upgrade changes YES/NO to integer, fix it ! */
				if (upgrade == UPGRADE)
					default_thread_arts = THREAD_MAX;
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

		case 'w':
#ifdef HAVE_COLOR
			if (match_boolean (buf, "word_highlight=", &word_highlight_tinrc)) {
				word_highlight=word_highlight_tinrc;
				break;
			}
			if (match_integer (buf, "word_h_display_marks=", &word_h_display_marks, MAX_MARK)) {
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
	
	/*
	 * sort out conflicting settings
	 */
	 
	/* nobody likes to navigate blind */
	if (!(draw_arrow_mark || inverse_okay)) {
		draw_arrow_mark = TRUE;
	}
	/* with invers video bar strip tailing blanks looks ugly */
	if (!draw_arrow_mark && strip_blanks) {
		strip_blanks = FALSE;
	}
	
	if ((cmd_line && !(update || verbose)) || (update && update_fork)) {
		wait_message ("\n");
	}

	return TRUE;
}


/*
 *  write config defaults to ~/.tin/tinrc
 */

void
write_config_file (
	char	*file)
{
	FILE *fp;
	char *file_tmp;
	int i;

	/* alloc memory for tmp-filename */
	if ((file_tmp = (char *) malloc (strlen (file)+5)) == NULL) {
		wait_message (txt_out_of_memory2);
		return;
	}
	/* generate tmp-filename */
	strcpy (file_tmp, file);
	strcat (file_tmp, ".tmp");

	if ((fp = fopen (file_tmp, "w")) == (FILE *) 0) {
		error_message (txt_filesystem_full_backup, CONFIG_FILE);
		/* free memory for tmp-filename */
		free (file_tmp);
		return;
	}

	if (!cmd_line) {
		wait_message (txt_saving);
	}

	if (!default_editor_format[0]) {
		strcpy (default_editor_format, EDITOR_FORMAT_ON);
	}
	fprintf (fp, txt_tinrc_header, TINRC_VERSION, progname, VERSION, RELEASEDATE);

	fprintf (fp, txt_tinrc_default_savedir);
	fprintf (fp, "default_savedir=%s\n\n", default_savedir);

	fprintf (fp, txt_tinrc_auto_save);
	fprintf (fp, "auto_save=%s\n\n", print_boolean (default_auto_save));

	fprintf (fp, txt_tinrc_mark_saved_read);
	fprintf (fp, "mark_saved_read=%s\n\n", print_boolean (mark_saved_read));

	fprintf (fp, txt_tinrc_post_process_type);
	fprintf (fp, "post_process_type=%d\n\n", default_post_proc_type);

	fprintf (fp, txt_tinrc_post_process_command);
	fprintf (fp, "post_process_command=%s\n\n", post_proc_command);

	fprintf (fp, txt_tinrc_process_only_unread);
	fprintf (fp, "process_only_unread=%s\n\n", print_boolean (process_only_unread));

	fprintf (fp, txt_tinrc_prompt_followupto);
	fprintf (fp, "prompt_followupto=%s\n\n", print_boolean (prompt_followupto));

	fprintf (fp, txt_tinrc_confirm_action);
	fprintf (fp, "confirm_action=%s\n\n", print_boolean (confirm_action));

	fprintf (fp, txt_tinrc_confirm_to_quit);
	fprintf (fp, "confirm_to_quit=%s\n\n", print_boolean (confirm_to_quit));

	fprintf (fp, txt_tinrc_draw_arrow);
	fprintf (fp, "draw_arrow=%s\n\n", print_boolean (draw_arrow_mark));

	fprintf (fp, txt_tinrc_inverse_okay);
	fprintf (fp, "inverse_okay=%s\n\n", print_boolean (inverse_okay));

	fprintf (fp, txt_tinrc_pos_first_unread);
	fprintf (fp, "pos_first_unread=%s\n\n", print_boolean (pos_first_unread));

	fprintf (fp, txt_tinrc_show_only_unread);
	fprintf (fp, "show_only_unread=%s\n\n", print_boolean (default_show_only_unread));

	fprintf (fp, txt_tinrc_show_only_unread_groups);
	fprintf (fp, "show_only_unread_groups=%s\n\n", print_boolean (show_only_unread_groups));

	fprintf (fp, txt_tinrc_tab_goto_next_unread);
	fprintf (fp, "tab_goto_next_unread=%s\n\n", print_boolean (tab_goto_next_unread));

	fprintf (fp, txt_tinrc_space_goto_next_unread);
	fprintf (fp, "space_goto_next_unread=%s\n\n", print_boolean (space_goto_next_unread));

	fprintf (fp, txt_tinrc_tab_after_X_selection);
	fprintf (fp, "tab_after_X_selection=%s\n\n", print_boolean (tab_after_X_selection));

	fprintf (fp, txt_tinrc_full_page_scroll);
	fprintf (fp, "full_page_scroll=%s\n\n", print_boolean (full_page_scroll));

	fprintf (fp, txt_tinrc_show_last_line_prev_page);
	fprintf (fp, "show_last_line_prev_page=%s\n\n", print_boolean (show_last_line_prev_page));

	fprintf (fp, txt_tinrc_catchup_read_groups);
	fprintf (fp, "catchup_read_groups=%s\n\n", print_boolean (catchup_read_groups));

	fprintf (fp, txt_tinrc_group_catchup_on_exit);
	fprintf (fp, "group_catchup_on_exit=%s\n", print_boolean (group_catchup_on_exit));
	fprintf (fp, "thread_catchup_on_exit=%s\n\n", print_boolean (thread_catchup_on_exit));

	fprintf (fp, txt_tinrc_thread_articles);
	fprintf (fp, "thread_articles=%d\n\n", default_thread_arts);

	fprintf (fp, txt_tinrc_show_description);
	fprintf (fp, "show_description=%s\n\n", print_boolean (show_description));

	fprintf (fp, txt_tinrc_show_author);
	fprintf (fp, "show_author=%d\n\n", default_show_author);

	fprintf (fp, txt_tinrc_news_headers_to_display);
	fprintf (fp, "news_headers_to_display=");
	for (i=0; i<num_headers_to_display; i++) {
		fprintf (fp, " %s",news_headers_to_display_array[i]);
	}
	fprintf (fp, "\n\n");

	fprintf (fp, txt_tinrc_news_headers_to_not_display);
	fprintf (fp, "news_headers_to_not_display=");
	for (i=0; i<num_headers_to_not_display; i++) {
		fprintf (fp, " %s",news_headers_to_not_display_array[i]);
	}
	fprintf (fp, "\n\n");

	fprintf (fp, txt_tinrc_sort_article_type);
	fprintf (fp, "sort_article_type=%d\n\n", default_sort_art_type);

	fprintf (fp, txt_tinrc_default_maildir);
	fprintf (fp, "default_maildir=%s\n\n", default_maildir);

	fprintf (fp, txt_tinrc_save_to_mmdf_mailbox);
	fprintf (fp, "save_to_mmdf_mailbox=%s\n\n", print_boolean (save_to_mmdf_mailbox));

	fprintf (fp, txt_tinrc_show_xcommentto);
	fprintf (fp, "show_xcommentto=%s\n\n", print_boolean(show_xcommentto));

	fprintf (fp, txt_tinrc_highlight_xcommentto);
	fprintf (fp, "highlight_xcommentto=%s\n\n", print_boolean(highlight_xcommentto));

	fprintf (fp, txt_tinrc_print_header);
	fprintf (fp, "print_header=%s\n\n", print_boolean (print_header));

	fprintf (fp, txt_tinrc_default_printer);
	fprintf (fp, "default_printer=%s\n\n", default_printer);

	fprintf (fp, txt_tinrc_batch_save);
	fprintf (fp, "batch_save=%s\n\n", print_boolean (default_batch_save));

	fprintf (fp, txt_tinrc_start_editor_offset);
	fprintf (fp, "start_editor_offset=%s\n\n", print_boolean (start_editor_offset));

	fprintf (fp, txt_tinrc_default_editor_format);
	fprintf (fp, "default_editor_format=%s\n\n", default_editor_format);

	fprintf (fp, txt_tinrc_default_mailer_format);
	fprintf (fp, "default_mailer_format=%s\n\n", default_mailer_format);

	fprintf (fp, txt_tinrc_use_mailreader_i);
	fprintf (fp, "use_mailreader_i=%s\n\n", print_boolean (use_mailreader_i));

	fprintf (fp, txt_tinrc_show_lines);
	fprintf (fp, "show_lines=%s\n\n", print_boolean(show_lines));

	fprintf (fp, txt_tinrc_unlink_article);
	fprintf (fp, "unlink_article=%s\n\n", print_boolean (unlink_article));

#ifdef M_UNIX
	fprintf (fp, txt_tinrc_keep_dead_articles);
	fprintf (fp, "keep_dead_articles=%s\n\n", print_boolean (keep_dead_articles));
#endif

	fprintf (fp, txt_tinrc_keep_posted_articles);
	fprintf (fp, "keep_posted_articles=%s\n\n", print_boolean (keep_posted_articles));

	fprintf (fp, txt_tinrc_default_sigfile);
	fprintf (fp, "default_sigfile=%s\n\n", default_sigfile);

	fprintf (fp, txt_tinrc_sigdashes);
	fprintf (fp, "sigdashes=%s\n\n", print_boolean (sigdashes));

	fprintf (fp, txt_tinrc_no_advertising);
	fprintf (fp, "no_advertising=%s\n\n", print_boolean (no_advertising));

	fprintf (fp, txt_tinrc_reread_active_file_secs);
	fprintf (fp, "reread_active_file_secs=%d\n\n", reread_active_file_secs);

	fprintf (fp, txt_tinrc_quote_chars);
	fprintf (fp, "quote_chars=%s\n\n", quote_space_to_dash (quote_chars));

	fprintf (fp, txt_tinrc_news_quote_format);
	fprintf (fp, "news_quote_format=%s\n", news_quote_format);
	fprintf (fp, "mail_quote_format=%s\n", mail_quote_format);
	fprintf (fp, "xpost_quote_format=%s\n\n", xpost_quote_format);

	fprintf (fp, txt_tinrc_auto_cc);
	fprintf (fp, "auto_cc=%s\n\n", print_boolean (auto_cc));

	fprintf (fp, txt_tinrc_auto_bcc);
	fprintf (fp, "auto_bcc=%s\n\n", print_boolean (auto_bcc));

	fprintf (fp, txt_tinrc_art_marked_deleted);
	fprintf (fp, "art_marked_deleted=%c\n\n", art_marked_deleted);

	fprintf (fp, txt_tinrc_art_marked_inrange);
	fprintf (fp, "art_marked_inrange=%c\n\n", art_marked_inrange);

	fprintf (fp, txt_tinrc_art_marked_return);
	fprintf (fp, "art_marked_return=%c\n\n", art_marked_return);

	fprintf (fp, txt_tinrc_art_marked_selected);
	fprintf (fp, "art_marked_selected=%c\n\n", art_marked_selected);

	fprintf (fp, txt_tinrc_art_marked_unread);
	fprintf (fp, "art_marked_unread=%c\n\n", art_marked_unread);

	fprintf (fp, txt_tinrc_force_screen_redraw);
	fprintf (fp, "force_screen_redraw=%s\n\n", print_boolean (force_screen_redraw));

	fprintf (fp, txt_tinrc_use_builtin_inews);
	fprintf (fp, "use_builtin_inews=%s\n\n", print_boolean (use_builtin_inews));

	fprintf (fp, txt_tinrc_auto_list_thread);
	fprintf (fp, "auto_list_thread=%s\n\n", print_boolean (auto_list_thread));

	fprintf (fp, txt_tinrc_use_mouse);
	fprintf (fp, "use_mouse=%s\n\n", print_boolean (use_mouse));

	fprintf (fp, txt_tinrc_strip_blanks);
	fprintf (fp, "strip_blanks=%s\n\n", print_boolean (strip_blanks));

	fprintf (fp, txt_tinrc_groupname_max_length);
	fprintf (fp, "groupname_max_length=%d\n\n", groupname_max_length);

	fprintf (fp, txt_tinrc_beginner_level);
	fprintf (fp, "beginner_level=%s\n\n", print_boolean (beginner_level));

	fprintf (fp, txt_tinrc_default_filter_days);
	fprintf (fp, "default_filter_days=%d\n\n", default_filter_days);

#ifdef HAVE_COLOR
	fprintf (fp, txt_tinrc_use_color);
	fprintf (fp, "use_color=%s\n\n", print_boolean (use_color_tinrc));

	fprintf (fp, txt_tinrc_colors);

	fprintf (fp, txt_tinrc_col_normal);
	fprintf (fp, "col_normal=%d\n\n", col_normal);

	fprintf (fp, txt_tinrc_col_back);
	fprintf (fp, "col_back=%d\n\n", col_back);

	fprintf (fp, txt_tinrc_col_invers_bg);
	fprintf (fp, "col_invers_bg=%d\n\n", col_invers_bg);

	fprintf (fp, txt_tinrc_col_invers_fg);
	fprintf (fp, "col_invers_fg=%d\n\n", col_invers_fg);

	fprintf (fp, txt_tinrc_col_text);
	fprintf (fp, "col_text=%d\n\n", col_text);

	fprintf (fp, txt_tinrc_col_minihelp);
	fprintf (fp, "col_minihelp=%d\n\n", col_minihelp);

	fprintf (fp, txt_tinrc_col_help);
	fprintf (fp, "col_help=%d\n\n", col_help);

	fprintf (fp, txt_tinrc_col_message);
	fprintf (fp, "col_message=%d\n\n", col_message);

	fprintf (fp, txt_tinrc_col_quote);
	fprintf (fp, "col_quote=%d\n\n", col_quote);

	fprintf (fp, txt_tinrc_col_head);
	fprintf (fp, "col_head=%d\n\n", col_head);

	fprintf (fp, txt_tinrc_col_newsheaders);
	fprintf (fp, "col_newsheaders=%d\n\n", col_newsheaders);

	fprintf (fp, txt_tinrc_col_subject);
	fprintf (fp, "col_subject=%d\n\n", col_subject);

	fprintf (fp, txt_tinrc_col_response);
	fprintf (fp, "col_response=%d\n\n", col_response);

	fprintf (fp, txt_tinrc_col_from);
	fprintf (fp, "col_from=%d\n\n", col_from);

	fprintf (fp, txt_tinrc_col_title);
	fprintf (fp, "col_title=%d\n\n", col_title);

	fprintf (fp, txt_tinrc_col_signature);
	fprintf (fp, "col_signature=%d\n\n", col_signature);

	fprintf (fp, txt_tinrc_word_highlight);
	fprintf (fp, "word_highlight=%s\n\n", print_boolean (word_highlight_tinrc));

	fprintf (fp, txt_tinrc_word_h_display_marks);
	fprintf (fp, "word_h_display_marks=%d\n\n",word_h_display_marks);

	fprintf (fp, txt_tinrc_col_markstar);
	fprintf (fp, "col_markstar=%d\n", col_markstar);
	fprintf (fp, "col_markdash=%d\n\n", col_markdash);

#endif

	if (*mail_address) {
		fprintf (fp, txt_tinrc_mail_address);
		fprintf (fp, "mail_address=%s\n\n",mail_address);
	}

	fprintf (fp, txt_tinrc_mm_charset);
	fprintf (fp, "mm_charset=%s\n\n", mm_charset);

#ifdef LOCAL_CHARSET
	fprintf (fp, txt_tinrc_local_charset);
	fprintf (fp, "local_charset=%s\n\n", print_boolean(use_local_charset));
#endif

	fprintf (fp, txt_tinrc_post_mime_encoding);
	fprintf (fp, "post_mime_encoding=%s\n", txt_mime_types[post_mime_encoding]);
	fprintf (fp, "mail_mime_encoding=%s\n\n", txt_mime_types[mail_mime_encoding]);

	fprintf (fp, txt_tinrc_post_8bit_header);
	fprintf (fp, "post_8bit_header=%s\n\n", print_boolean(post_8bit_header));

	fprintf (fp, txt_tinrc_mail_8bit_header);
	fprintf (fp, "mail_8bit_header=%s\n\n", print_boolean(mail_8bit_header));

	fprintf (fp, txt_tinrc_display_mime_header_asis);
	fprintf (fp, "display_mime_header_asis=%s\n\n", print_boolean(display_mime_header_asis));

#ifdef HAVE_METAMAIL
 	fprintf (fp, txt_tinrc_use_metamail);
 	fprintf (fp, "use_metamail=%s\n\n", print_boolean (use_metamail));

 	fprintf (fp, txt_tinrc_ask_for_metamail);
 	fprintf (fp, "ask_for_metamail=%s\n\n", print_boolean (ask_for_metamail));
#endif

#ifdef HAVE_KEYPAD
	fprintf (fp, txt_tinrc_use_keypad);
	fprintf (fp, "use_keypad=%s\n\n", print_boolean (use_keypad));
#endif

 	fprintf (fp, txt_tinrc_alternative_handling);
	fprintf (fp, "alternative_handling=%s\n\n", print_boolean (alternative_handling));
 	fprintf (fp, txt_tinrc_strip_newsrc);
	fprintf (fp, "strip_newsrc=%s\n\n", print_boolean (strip_newsrc));
 	fprintf (fp, txt_tinrc_strip_bogus);
	fprintf (fp, "strip_bogus=%d\n\n", strip_bogus);

	fprintf (fp, txt_tinrc_filter);
	fprintf (fp, "default_filter_kill_header=%d\n", default_filter_kill_header);
	fprintf (fp, "default_filter_kill_global=%s\n", print_boolean (default_filter_kill_global));
	fprintf (fp, "default_filter_kill_case=%s\n", print_boolean (default_filter_kill_case));
	fprintf (fp, "default_filter_kill_expire=%s\n", print_boolean (default_filter_kill_expire));
	fprintf (fp, "default_filter_select_header=%d\n", default_filter_select_header);
	fprintf (fp, "default_filter_select_global=%s\n", print_boolean (default_filter_select_global));
	fprintf (fp, "default_filter_select_case=%s\n", print_boolean (default_filter_select_case));
	fprintf (fp, "default_filter_select_expire=%s\n\n", print_boolean (default_filter_select_expire));

	fprintf (fp, txt_tinrc_defaults);
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

	fprintf (fp, txt_tinrc_motd_file_info);
	fprintf (fp, "motd_file_info=%s\n\n", motd_file_info);

	fprintf (fp, txt_tinrc_newnews);
	if (!num_newnews) {
		fprintf (fp, "newnews=%s %ld\n", new_newnews_host, new_newnews_time);
	} else {
		for (i = 0 ; i < num_newnews ; i++) {
			fprintf (fp, "newnews=%s %ld\n", newnews[i].host, newnews[i].time);
		}
	}
	if (ferror (fp) | fclose (fp)){
		error_message (txt_filesystem_full, CONFIG_FILE);
		/* free memory for tmp-filename */
		free (file_tmp);
		return;
	} else {
		rename_file (file_tmp, file);
		chmod (file, (S_IRUSR|S_IWUSR));
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
 * displayed on the screen, which is different from the option number
 * in the option_table array (the former one is one greater since counting
 * starts with one instead of zero).
 */

static void
print_option (
	enum option_enum the_option)
{
	print_any_option((int)the_option);
}

static void
print_any_option (
	int act_option)
{
	my_printf("%3d. %s ", act_option, option_table[act_option - 1].option_text);
	switch (option_table[act_option - 1].var_type) {
		case OPT_ON_OFF:
			my_printf("%s ", print_boolean(*OPT_ON_OFF_list[option_table[act_option - 1].var_index]));
			break;
		case OPT_LIST:
			my_printf("%s", option_table[act_option - 1].opt_list[*(option_table[act_option - 1].variable)]);
			break;
		case OPT_STRING:
			my_printf("%-.*s", cCOLS - (int) strlen(option_table[act_option - 1].option_text) - OPT_ARG_COLUMN - 3, OPT_STRING_list[option_table[act_option - 1].var_index]);
			break;
		case OPT_NUM:
			my_printf("%d", *(option_table[act_option - 1].variable));
			break;
		case OPT_CHAR:
			my_printf("%c", *OPT_CHAR_list[option_table[act_option - 1].var_index]);
			break;
		default:
			break;
	}
}

static void
highlight_option (
	int option)
{
	MoveCursor (INDEX_TOP + (option - 1) % option_lines_per_page, 0);
	my_fputs ("->", stdout);
	my_flush();
	stow_cursor();
}

static void
unhighlight_option (
	int option)
{
	MoveCursor (INDEX_TOP + (option - 1) % option_lines_per_page, 0);
	my_fputs ("  ", stdout);
	my_flush();
}

/*
 * Refresh the config page which holds the actual option. If act_option is
 * zero fall back on the last given option; if even that is not present,
 * use 1 (first option) as fall back value. Note that act_option is the
 * number shown on the screen, not the index in option_table (which is
 * one smaller). Set force_redraw to TRUE if you want to enforce a refresh,
 * and to FALSE if you want to refresh the screen only when necessary
 * (needed by signal.c: config_resize(); the resizing could result in
 * desired_page == actual_option_page even if there are now more/less
 * options on the screen than before).
 */

void
refresh_config_page (
	int act_option,
	int force_redraw)
{
	static int last_option = 0;
	int desired_page;

	if (act_option == 0) {	/* Yes, could be done shorter. */
		if (last_option != 0) {
			act_option = last_option;
		}
		else {
			act_option = 1;
		}
	}
	
	/* determine on which page act_option would be */
	desired_page = (int) (act_option - 1) / option_lines_per_page;

	if ((desired_page != actual_option_page) || force_redraw)
	{
		show_config_page (desired_page);
		actual_option_page = desired_page;
	}
	last_option = act_option;
}

/*
 *  options menu so that the user can dynamically change parameters
 */

int
change_config_file (
	struct t_group *group,
	int filter_at_once)	/* not used */
{
	int ch, i;
	int change_option = FALSE;
	t_bool original_on_off_value;
	int original_list_value;
	int option, old_option;
	int ret_code = NO_FILTERING;
	int mime_type = 0;

	set_signals_config ();
	
	actual_option_page = -1;
	option = 1;

	set_xclick_off ();
	forever {

	 	refresh_config_page (option, FALSE);
	 	highlight_option (option);

		stow_cursor();
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
						ch = iKeyUp;
						break;

					case KEYMAP_DOWN:
						ch = iKeyDown;
						break;

					case KEYMAP_HOME:
						ch = iKeyFirstPage;
						break;

					case KEYMAP_END:
						ch = iKeyLastPage;
						break;

					case KEYMAP_PAGE_UP:
						ch = iKeyPageUp;
						break;

					case KEYMAP_PAGE_DOWN:
						ch = iKeyPageDown;
						break;
#ifndef WIN32
					default:
						break;
				} /* switch (get_arrow_key ()) */
				break;
#endif
			default:
				break;
		}	/* switch (ch) */

		switch (ch) {
			case iKeyQuit:
				write_config_file (local_config_file);
				/* FALLTHRU */
			case iKeyConfigNoSave:
				clear_note_area ();
				return ret_code;

			case iKeyUp:
			case iKeyUp2:
				unhighlight_option (option);
				option--;
				if (option < 1)
					option = LAST_OPT;
				refresh_config_page (option, FALSE);
				highlight_option (option);
				break;

			case iKeyDown:
			case iKeyDown2:
				unhighlight_option (option);
				option++;
				if (option > LAST_OPT)
					option = 1;
				refresh_config_page (option, FALSE);
				highlight_option (option);
				break;

			case iKeyFirstPage:
			case iKeyConfigFirstPage:
				unhighlight_option (option);
				option = 1;
				refresh_config_page (option, FALSE);
				highlight_option (option);
				break;

			case iKeyLastPage:
			case iKeyConfigLastPage:
				unhighlight_option (option);
				option = LAST_OPT;
				refresh_config_page (option, FALSE);
				highlight_option (option);
				break;

			case iKeyPageUp:
			case iKeyPageUp2:
			case iKeyPageUp3:
				unhighlight_option (option);
				option -= option_lines_per_page;
				if (option < 1)
					option = LAST_OPT;
				refresh_config_page (option, FALSE);
				highlight_option (option);
				break;

			case iKeyPageDown:
			case iKeyPageDown2:
			case iKeyPageDown3:
				unhighlight_option (option);
				option += option_lines_per_page;
				if (option > LAST_OPT) {
					if (LAST_OPTION_PAGE == actual_option_page) {
						option = 1;
					} else {
						option = LAST_OPT;
					}
				}
				refresh_config_page (option, FALSE);
				highlight_option (option);
				break;

			case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				unhighlight_option (option);
				old_option = option;
				option = prompt_num (ch, txt_enter_option_num);
				if (option < 1 || option > LAST_OPT) {
					option = old_option;
					break;
				}
				refresh_config_page (option, FALSE);
				/* FALLTHROUGH */

			case iKeyConfigSelect:
			case iKeyConfigSelect2:
				change_option = TRUE;
				break;
			default:
				break;
		} /* switch (ch) */

		if (change_option) {
			switch (option_table[option - 1].var_type) {
				case OPT_ON_OFF:
					original_on_off_value = *OPT_ON_OFF_list[option_table[option - 1].var_index];
					prompt_on_off (INDEX_TOP + (option - 1) % option_lines_per_page,
						OPT_ARG_COLUMN,
						OPT_ON_OFF_list[option_table[option - 1].var_index],
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
								if ((int)OPT_INVERSE_OKAY > first_option_on_screen
								 && (int)OPT_INVERSE_OKAY < first_option_on_screen + option_lines_per_page) {
									MoveCursor (INDEX_TOP + (OPT_INVERSE_OKAY - 1) % option_lines_per_page, 3);
									print_option (OPT_INVERSE_OKAY);
								}
							}
							break;

						/* draw inversed screen header lines */
						/* draw inversed group/article/option line if draw_arrow_mark is OFF */
 	   					case OPT_INVERSE_OKAY:
							unhighlight_option (option);
							if (draw_arrow_mark == FALSE && inverse_okay == FALSE) {
								draw_arrow_mark = TRUE;	/* we don't want to navigate blindly */
								if ((int)OPT_DRAW_ARROW_MARK > first_option_on_screen
								 && (int)OPT_DRAW_ARROW_MARK <= first_option_on_screen + option_lines_per_page + 1) {
									MoveCursor (INDEX_TOP + (OPT_DRAW_ARROW_MARK - 1) % option_lines_per_page, 3);
									print_option (OPT_DRAW_ARROW_MARK);
								}
							}
							break;

						case OPT_MAIL_8BIT_HEADER:
							if (strcasecmp(txt_mime_types[mail_mime_encoding], txt_8bit)) {
								mail_8bit_header = FALSE;
								MoveCursor (INDEX_TOP + (OPT_MAIL_8BIT_HEADER - 1) % option_lines_per_page, 3);
								print_option (OPT_MAIL_8BIT_HEADER);
							}
							break;

						case OPT_POST_8BIT_HEADER:
							if (strcasecmp(txt_mime_types[post_mime_encoding], txt_8bit)) {
								post_8bit_header = FALSE;
								MoveCursor (INDEX_TOP + (OPT_POST_8BIT_HEADER - 1) % option_lines_per_page, 3);
								print_option (OPT_POST_8BIT_HEADER);
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
						case OPT_USE_COLOR_TINRC:
							use_color = use_color_tinrc;
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
                                                 * case OPT_DISPLAY_MIME_HEADER_ASIS:
#ifdef HAVE_KEYPAD
						 * case OPT_USE_KEYPAD:
#endif
#ifdef HAVE_METAMAIL
						 * case OPT_ASK_FOR_METAMAIL:	case OPT_USE_METAMAIL:
#endif
#ifdef M_UNIX
						 * case OPT_KEEP_DEAD_ARTICLES:
#endif
#ifdef HAVE_COLOR
						 * case OPT_WORD_HIGHLIGHT_TINRC:
						 * case OPT_WORD_H_DISPLAY_MARKS:
#endif
						 * 	break;
						 */
						default:
							break;
					} /* switch (option) */
					break;

				case OPT_LIST:
					original_list_value = *(option_table[option - 1].variable);
					*(option_table[option - 1].variable) = prompt_list (INDEX_TOP + (option - 1) % option_lines_per_page,
								OPT_ARG_COLUMN,
								*(option_table[option - 1].variable), /*default_post_proc_type,*/
								option_table[option - 1].help_text,
								option_table[option - 1].option_text,
								option_table[option - 1].opt_list,
								option_table[option - 1].opt_count
								);

					/*
					 * some options need further action to take effect
					 */
					switch (option) {

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

						case OPT_DEFAULT_SHOW_AUTHOR:
							if (group != (struct t_group *) 0) {
								group->attribute->show_author = default_show_author;
							}
							break;

						/*
						 * the following don't need any further action
#ifdef HAVE_COLOR
						 *
						 * case OPT_COL_BACK:		case OPT_COL_FROM:
						 * case OPT_COL_HEAD:		case OPT_COL_HELP:
						 * case OPT_COL_INVERS:		case OPT_COL_MESSAGE:
						 * case OPT_COL_MINIHELP:	case OPT_COL_NORMAL:
						 * case OPT_COL_QUOTE:		case OPT_COL_RESPONSE:
						 * case OPT_COL_SIGNATURE:	case OPT_COL_SUBJECT:
						 * case OPT_COL_TEXT:		case OPT_COL_TITLE:
						 * case OPT_COL_MARKSTAR:
						 * case OPT_COL_MARKDASH:
						 *
#endif
						 *
						 * case OPT_DEFAULT_SORT_ART_TYPE:
						 *	break;
						 */
						 
						default:
							break;
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
						case OPT_MAIL_ADDRESS:
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
								OPT_STRING_list[option_table[option - 1].var_index]
								);
							break;

						case OPT_MAIL_MIME_ENCODING:
						case OPT_POST_MIME_ENCODING:
							mime_type = *(option_table[option - 1].variable);
							mime_type = prompt_list (INDEX_TOP + (option - 1) % option_lines_per_page,
										OPT_ARG_COLUMN,
										mime_type,
										option_table[option - 1].help_text,
										option_table[option - 1].option_text,
										option_table[option - 1].opt_list,
										option_table[option - 1].opt_count
										);
							*(option_table[option - 1].variable) = mime_type;

							/* do not use 8 bit headers if mime encoding is not 8bit; ask J. Shin why */
							if (strcasecmp(txt_mime_types[mime_type], txt_8bit)) {
								if (option == (int)OPT_POST_MIME_ENCODING) {
									post_8bit_header = FALSE;
									if (((int)OPT_POST_8BIT_HEADER > first_option_on_screen)
									 && ((int)OPT_POST_8BIT_HEADER <= first_option_on_screen + option_lines_per_page + 1)) {
										MoveCursor (INDEX_TOP + (OPT_POST_8BIT_HEADER - 1) % option_lines_per_page, 3);
										print_option (OPT_POST_8BIT_HEADER);
									}
								} else {
									mail_8bit_header = FALSE;
									if (((int)OPT_MAIL_8BIT_HEADER > first_option_on_screen)
									 && ((int)OPT_POST_8BIT_HEADER <= first_option_on_screen + option_lines_per_page + 1)) {
										MoveCursor (INDEX_TOP + (OPT_MAIL_8BIT_HEADER - 1) % option_lines_per_page, 3);
										print_option (OPT_MAIL_8BIT_HEADER);
									}
								}
							}
							break;

						default:
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

						default:
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

						default:
							break;
					} /* switch (option) */
					break;

				default:
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
expand_rel_abs_pathname (
	int line,
	int col,
	char *str)
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
	my_flush();
}

/*
 *  show_menu_help
 */

void
show_menu_help (
	const char *help_message)
{
	 MoveCursor (cLINES-2, 0);
	 CleartoEOLN ();
	 center_line (cLINES-2, FALSE, help_message);
}


int
match_boolean (
	char *line,
	const char *pat,
	t_bool *dst)
{
	size_t	patlen = strlen (pat);

	if (STRNCASECMPEQ(line, pat, patlen)) {
		*dst = (t_bool) (STRNCASECMPEQ(&line[patlen], "ON", 2) ? TRUE : FALSE);
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
match_integer (
	char *line,
	const char *pat,
	int *dst,
	int maxlen)
{
	size_t	patlen = strlen (pat);

	if (STRNCMPEQ(line, pat, patlen)) {
		*dst = atoi (&line[patlen]);

		if (maxlen)  {
			if ((*dst < 0) || (*dst > maxlen)) {
				my_fprintf(stderr, txt_value_out_of_range, pat, *dst, maxlen);
				*dst = 0;
			}
		}

		return TRUE;
	}
	return FALSE;
}


int
match_long (
	char *line,
	const char *pat,
	long *dst)
{
	size_t	patlen = strlen (pat);

	if (STRNCMPEQ(line, pat, patlen)) {
		*dst = atol (&line[patlen]);
		return TRUE;
	}
	return FALSE;
}

/* If the 'pat' keyword matches, lookup & return an index into the table */
static int
match_list (
	char *line,
	constext *pat,
	constext *const *table,
	size_t tablelen,
	int *dst)
{
	size_t	patlen = strlen (pat);
	size_t	n;
	char	temp[LEN];

	if (STRNCMPEQ(line, pat, patlen)) {
		line += patlen;
		*dst = 0;	/* default, if no match */
		for (n = 0; n < tablelen; n++) {
			if (match_string(line, table[n], temp, sizeof(temp))) {
				*dst = (int)n;
				break;
			}
		}
		return TRUE;
	}
	return FALSE;
}

int
match_string (
	char *line,
	const char *pat,
	char *dst,
	size_t dstlen)
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


const char *
print_boolean (
	t_bool value)
{
	return txt_onoff[value != FALSE];
}

/*
 *  convert underlines to spaces in a string
 */

void
quote_dash_to_space (
	char *str)
{
	char *ptr;

	for (ptr = str; *ptr; ptr++) {
		if (*ptr == '_') {
			*ptr = ' ';
		}
	}
}

/*
 *  convert spaces to underlines in a string
 */

char *
quote_space_to_dash (
	char *str)
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

static void
show_config_page (
	int page_no)
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
		print_option (first_option_on_screen + i + 1);
	}

	show_menu_help (txt_select_config_file_option);
	my_flush();
	stow_cursor();
}


/*
 * Written by: Brad Viviano and Scott Powers (bcv & swp)
 *
 * Takes a 1d string and turns it into a 2d array of strings.
 *
 * Watch out for the frees! You must free(*argv) and then free(argv)! NOTHING
 *   ELSE!! Do _NOT_ free the individual args of argv.
 */
static char **
ulBuildArgv(
	char *cmd,
	int *new_argc) 
{
	char **new_argv=NULL;
	char *buf=NULL, *tmp=NULL;
	int i=0;

	if (!cmd && !*cmd) {
		*new_argc = 0;
		return (NULL);
	}

	for(tmp=cmd; isspace (*tmp); tmp++);
	buf = my_strdup(tmp);
	if (!buf) {
		*new_argc = 0;
		return (NULL);
	}

	tmp=buf;

	new_argv = (char **) calloc (1, sizeof (char *));
	if (!new_argv) {
		free (buf);
		*new_argc = 0;
		return (NULL);
	}

	new_argv[0]=NULL;
	while (*tmp) {
		if (!isspace(*tmp)) { /*found the begining of a word*/
			new_argv[i]=tmp;
				for (; *tmp && !isspace(*tmp); tmp++);
				if (*tmp) {
					*tmp = '\0';
					tmp++;
				}
			i++;
			new_argv = (char **) realloc (new_argv, ((i+1) * sizeof (char *)));
			new_argv[i] = NULL;
		} else {
			tmp++;
		}
	}
	*new_argc = i;
	return (new_argv);
}
