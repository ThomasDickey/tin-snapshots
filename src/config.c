/*
 *  Project   : tin - a Usenet reader
 *  Module    : config.c
 *  Author    : I. Lea
 *  Created   : 1991-04-01
 *  Updated   : 1999-11-04
 *  Notes     : Configuration file routines
 *  Copyright : (c) Copyright 1991-99 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef VERSION_H
#	include	"version.h"
#endif /* !VERSION_H */
#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */
#ifndef TINTBL_H
#	include	"tincfg.h"
#endif /* !TINTBL_H */
#ifndef TCURSES_H
#	include "tcurses.h"
#endif /* !TCURSES_H */
#ifndef MENUKEYS_H
#	include	"menukeys.h"
#endif /* !MENUKEYS_H */

static void expand_rel_abs_pathname (int line, int col, char *str);
static void show_config_page (void);

#ifdef HAVE_COLOR
	static t_bool match_color (char *line, const char *pat, int *dst, int maxlen);
#endif /* HAVE_COLOR */

enum state { IGNORE, CHECK, UPGRADE };


/* FIXME: see doc/TODO and comments in the code */
/*
 * If we don't find a matching version tag line at the top of the rc file,
 * give the user some upgrade guidance and silently update variables where
 * necessary. We use a simple state mechanism, starting with CHECK for the
 * 1st line, then switch to UPGRADE or IGNORE accordingly.
 */
static int
check_upgrade (
	char *buf)
{
	char foo[60];
	char bar[120]; /* should be enought; snprintf() would be handy */

	my_strncpy(foo, txt_tinrc_header, strlen(foo)-1);
	sprintf(bar, foo, PRODUCT, TINRC_VERSION);

	if (strncmp(buf, bar, MIN(strlen(bar),strlen(buf))) == 0)
		return(IGNORE);
	else {
		error_message (txt_warn_update, VERSION);
		error_message (txt_return_key);
		ReadCh();
		return(UPGRADE);
	}
}

/*
 *  read local & global configuration defaults
 */

t_bool
read_config_file (
	char *file,
	t_bool global_file) /* return value is always ignored */
{
	FILE *fp;
	char newnews_info[PATH_LEN];
	char buf[LEN];
	int upgrade = CHECK;

	if ((fp = fopen (file, "r")) == (FILE *) 0)
		return FALSE;

	if (INTERACTIVE)
		wait_message (0, txt_reading_config_file, (global_file) ? "global " : "");

	while (fgets (buf, (int) sizeof (buf), fp) != (char *) 0) {
		if (buf[0] == '#' || buf[0] == '\n') {
			if (upgrade == CHECK)
				upgrade = check_upgrade(buf);
			continue;
		}

		switch(tolower((unsigned char)buf[0])) {
		case 'a':
			if (match_boolean (buf, "add_posted_to_filter=", &tinrc.add_posted_to_filter))
				break;

			if (match_boolean (buf, "advertising=", &tinrc.advertising))
				break;

			if (match_boolean (buf, "auto_save=", &tinrc.auto_save))
				break;

			if (match_string (buf, "art_marked_deleted=", buf, sizeof (buf))) {
				tinrc.art_marked_deleted = buf[0];
				break;
			}

			if (match_string (buf, "art_marked_inrange=", buf, sizeof (buf))) {
				tinrc.art_marked_inrange = buf[0];
				break;
			}

			if (match_string (buf, "art_marked_return=", buf, sizeof (buf))) {
				tinrc.art_marked_return = buf[0];
				break;
			}

			if (match_string (buf, "art_marked_selected=", buf, sizeof (buf))) {
				tinrc.art_marked_selected = buf[0];
				break;
			}

			if (match_string (buf, "art_marked_unread=", buf, sizeof (buf))) {
				tinrc.art_marked_unread = buf[0];
				break;
			}

#ifdef HAVE_METAMAIL
			if (match_boolean (buf, "ask_for_metamail=", &tinrc.ask_for_metamail))
				break;
#endif /* HAVE_METAMAIL */

			if (match_boolean (buf, "auto_cc=", &tinrc.auto_cc))
				break;

			if (match_boolean (buf, "auto_bcc=", &tinrc.auto_bcc))
				break;

			if (match_boolean (buf, "auto_list_thread=", &tinrc.auto_list_thread))
				break;

			if (match_boolean (buf, "auto_reconnect=", &tinrc.auto_reconnect))
				break;

			if (match_boolean (buf, "alternative_handling=", &tinrc.alternative_handling))
				break;

			break;

		case 'b':
			if (match_boolean (buf, "batch_save=", &tinrc.batch_save))
				break;

			if (match_boolean (buf, "beginner_level=", &tinrc.beginner_level))
				break;

			break;

		case 'c':
			if (match_boolean (buf, "cache_overview_files=", &tinrc.cache_overview_files))
				break;

			if (match_boolean (buf, "catchup_read_groups=", &tinrc.catchup_read_groups))
				break;

			if (match_boolean (buf, "confirm_action=", &tinrc.confirm_action))
				break;

			if (match_boolean (buf, "confirm_to_quit=", &tinrc.confirm_to_quit))
				break;

#ifdef HAVE_COLOR
			if (match_color (buf, "col_back=", &tinrc.col_back, MAX_COLOR))
				break;

			if (match_color (buf, "col_invers_bg=", &tinrc.col_invers_bg, MAX_COLOR))
				break;

			if (match_color (buf, "col_invers_fg=", &tinrc.col_invers_fg, MAX_COLOR))
				break;

			if (match_color (buf, "col_text=", &tinrc.col_text, MAX_COLOR))
				break;

			if (match_color (buf, "col_minihelp=", &tinrc.col_minihelp, MAX_COLOR))
				break;

			if (match_color (buf, "col_help=", &tinrc.col_help, MAX_COLOR))
				break;

			if (match_color (buf, "col_message=", &tinrc.col_message, MAX_COLOR))
				break;

			if (match_color (buf, "col_quote=", &tinrc.col_quote, MAX_COLOR))
				break;

			if (match_color (buf, "col_quote2=", &tinrc.col_quote2, MAX_COLOR))
				break;

			if (match_color (buf, "col_quote3=", &tinrc.col_quote3, MAX_COLOR))
				break;

			if (match_color (buf, "col_head=", &tinrc.col_head, MAX_COLOR))
				break;

			if (match_color (buf, "col_newsheaders=", &tinrc.col_newsheaders, MAX_COLOR))
				break;

			if (match_color (buf, "col_subject=", &tinrc.col_subject, MAX_COLOR))
				break;

			if (match_color (buf, "col_response=", &tinrc.col_response, MAX_COLOR))
				break;

			if (match_color (buf, "col_from=", &tinrc.col_from, MAX_COLOR))
				break;

			if (match_color (buf, "col_normal=", &tinrc.col_normal, MAX_COLOR))
				break;

			if (match_color (buf, "col_title=", &tinrc.col_title, MAX_COLOR))
				break;

			if (match_color (buf, "col_signature=", &tinrc.col_signature, MAX_COLOR))
				break;

			if (match_color (buf, "col_markstar=", &tinrc.col_markstar, MAX_COLOR))
				break;

			if (match_color (buf, "col_markdash=", &tinrc.col_markdash, MAX_COLOR))
				break;
#endif /* HAVE_COLOR */

			break;

		case 'd':
			if (match_string (buf, "default_editor_format=", tinrc.editor_format, sizeof (tinrc.editor_format)))
				break;

			if (match_string (buf, "default_mailer_format=", tinrc.mailer_format, sizeof (tinrc.mailer_format)))
				break;

			if (match_string (buf, "default_savedir=", tinrc.savedir, sizeof (tinrc.savedir))) {
				if (tinrc.savedir[0] == '.' && strlen (tinrc.savedir) == 1) {
					get_cwd (buf);
					my_strncpy (tinrc.savedir, buf, sizeof (tinrc.savedir));
				}
				break;
			}

			if (match_string (buf, "default_maildir=", tinrc.maildir, sizeof (tinrc.maildir))) {
				joinpath (posted_msgs_file, tinrc.maildir, POSTED_FILE);
				break;
			}

#ifndef DISABLE_PRINTING
			if (match_string (buf, "default_printer=", tinrc.printer, sizeof (tinrc.printer)))
				break;
#endif /* !DISABLE_PRINTING */

			if (match_string (buf, "default_sigfile=", tinrc.sigfile, sizeof (tinrc.sigfile)))
				break;

			if (match_integer (buf, "default_filter_days=", &tinrc.filter_days, 0))
				break;

			if (match_integer (buf, "default_filter_kill_header=", &tinrc.default_filter_kill_header, FILTER_LINES))
				break;

			if (match_boolean (buf, "default_filter_kill_global=", &tinrc.default_filter_kill_global))
				break;

			if (match_boolean (buf, "default_filter_kill_case=", &tinrc.default_filter_kill_case)) {
				/* ON=false, OFF=true */
				tinrc.default_filter_kill_case = !tinrc.default_filter_kill_case;
				break;
			}

			if (match_boolean (buf, "default_filter_kill_expire=", &tinrc.default_filter_kill_expire))
				break;

			if (match_integer (buf, "default_filter_select_header=", &tinrc.default_filter_select_header, FILTER_LINES))
				break;

			if (match_boolean (buf, "default_filter_select_global=", &tinrc.default_filter_select_global))
				break;

			if (match_boolean (buf, "default_filter_select_case=", &tinrc.default_filter_select_case)) {
				/* ON=false, OFF=true */
				tinrc.default_filter_select_case = !tinrc.default_filter_select_case;
				break;
			}

			if (match_boolean (buf, "default_filter_select_expire=", &tinrc.default_filter_select_expire))
				break;

			if (match_string (buf, "default_save_mode=", buf, sizeof (buf))) {
				tinrc.default_save_mode = buf[0];
				break;
			}

			if (match_string (buf, "default_author_search=", tinrc.default_search_author, sizeof (tinrc.default_search_author)))
				break;

			if (match_string (buf, "default_goto_group=", tinrc.default_goto_group, sizeof (tinrc.default_goto_group)))
				break;

			if (match_string (buf, "default_config_search=", tinrc.default_search_config, sizeof (tinrc.default_search_config)))
				break;

			if (match_string (buf, "default_group_search=", tinrc.default_search_group, sizeof (tinrc.default_search_group)))
				break;

			if (match_string (buf, "default_subject_search=", tinrc.default_search_subject, sizeof (tinrc.default_search_subject)))
				break;

			if (match_string (buf, "default_art_search=", tinrc.default_search_art, sizeof (tinrc.default_search_art)))
				break;

			if (match_string (buf, "default_repost_group=", tinrc.default_repost_group, sizeof (tinrc.default_repost_group)))
				break;

			if (match_string (buf, "default_mail_address=", tinrc.default_mail_address, sizeof (tinrc.default_mail_address)))
				break;

			if (match_integer (buf, "default_move_group=", &tinrc.default_move_group, 0))
				break;

#ifndef DONT_HAVE_PIPING
			if (match_string (buf, "default_pipe_command=", tinrc.default_pipe_command, sizeof (tinrc.default_pipe_command)))
				break;
#endif /* DONT_HAVE_PIPING */

			if (match_string (buf, "default_post_newsgroups=", tinrc.default_post_newsgroups, sizeof (tinrc.default_post_newsgroups)))
				break;

			if (match_string (buf, "default_post_subject=", tinrc.default_post_subject, sizeof (tinrc.default_post_subject)))
				break;

			if (match_string (buf, "default_regex_pattern=", tinrc.default_regex_pattern, sizeof (tinrc.default_regex_pattern)))
				break;

			if (match_string (buf, "default_range_group=", tinrc.default_range_group, sizeof (tinrc.default_range_group)))
				break;

			if (match_string (buf, "default_range_select=", tinrc.default_range_select, sizeof (tinrc.default_range_select)))
				break;

			if (match_string (buf, "default_range_thread=", tinrc.default_range_thread, sizeof (tinrc.default_range_thread)))
				break;

			if (match_string (buf, "default_save_file=", tinrc.default_save_file, sizeof (tinrc.default_save_file)))
				break;

			if (match_string (buf, "default_select_pattern=", tinrc.default_select_pattern, sizeof (tinrc.default_select_pattern)))
				break;

			if (match_string (buf, "default_shell_command=", tinrc.default_shell_command, sizeof (tinrc.default_shell_command)))
				break;

			if (match_boolean (buf, "display_mime_header_asis=", &tinrc.display_mime_header_asis))
				break;

			if (match_boolean (buf, "display_mime_allheader_asis=", &tinrc.display_mime_allheader_asis))
				break;

			if (match_boolean (buf, "draw_arrow=", &tinrc.draw_arrow))
				break;

			break;

		case 'f':
			if (match_boolean (buf, "full_page_scroll=", &tinrc.full_page_scroll))
				break;

			if (match_boolean (buf, "force_screen_redraw=", &tinrc.force_screen_redraw))
				break;

			break;

		case 'g':
			if (match_integer (buf, "getart_limit=", &tinrc.getart_limit, 0))
				break;

			if (match_integer (buf, "groupname_max_length=", &tinrc.groupname_max_length, 132))
				break;

			if (match_boolean (buf, "group_catchup_on_exit=", &tinrc.group_catchup_on_exit))
				break;

			break;

		case 'i':
			if (match_boolean (buf, "info_in_last_line=", &tinrc.info_in_last_line))
				break;

			if (match_boolean (buf, "inverse_okay=", &tinrc.inverse_okay))
				break;

			break;

		case 'k':
			if (match_boolean (buf, "keep_dead_articles=", &tinrc.keep_dead_articles))
				break;

			if (match_boolean (buf, "keep_posted_articles=", &tinrc.keep_posted_articles))
				break;

			if (match_integer (buf, "kill_level=", &tinrc.kill_level, KILL_NOTHREAD))
				break;

			break;

		case 'l':
#ifdef LOCAL_CHARSET
			if (match_boolean (buf, "local_charset=", &use_local_charset))
				break;
#endif /* LOCAL_CHARSET */

			break;

		case 'm':
			if (match_list (buf, "mail_mime_encoding=", txt_mime_encodings, NUM_MIME_ENCODINGS, &tinrc.mail_mime_encoding))
				break;

			/* option to toggle 8bit char. in header of mail message */
			if (match_boolean (buf, "mail_8bit_header=", &tinrc.mail_8bit_header)) {
				if (strcasecmp(txt_mime_encodings[tinrc.mail_mime_encoding], txt_8bit))
					tinrc.mail_8bit_header = FALSE;
				break;
			}

			if (match_string (buf, "mm_charset=", tinrc.mm_charset, sizeof (tinrc.mm_charset)))
				break;

			if (match_boolean (buf, "mark_saved_read=", &tinrc.mark_saved_read))
				break;

			if (match_string (buf, "mail_address=", tinrc.mail_address, sizeof (tinrc.mail_address)))
				break;

			if (match_string (buf, "mail_quote_format=", tinrc.mail_quote_format, sizeof (tinrc.mail_quote_format)))
				break;

			break;

		case 'n':
			if (match_string (buf, "newnews=", newnews_info, sizeof (newnews_info))) {
				load_newnews_info (newnews_info);
				break;
			}

			/* pick which news headers to display */
			if (match_string (buf, "news_headers_to_display=", tinrc.news_headers_to_display, sizeof (tinrc.news_headers_to_display))) {
				news_headers_to_display_array = ulBuildArgv(tinrc.news_headers_to_display, &num_headers_to_display);
				break;
			}

			/* pick which news headers to NOT display */
			if (match_string (buf, "news_headers_to_not_display=", tinrc.news_headers_to_not_display, sizeof (tinrc.news_headers_to_not_display))) {
				news_headers_to_not_display_array = ulBuildArgv(tinrc.news_headers_to_not_display, &num_headers_to_not_display);
				break;
			}

			if (match_string (buf, "news_quote_format=", tinrc.news_quote_format, sizeof (tinrc.news_quote_format)))
				break;

			break;

		case 'p':
			if (match_list (buf, "post_mime_encoding=", txt_mime_encodings, NUM_MIME_ENCODINGS, &tinrc.post_mime_encoding))
				break;

			/* option to toggle 8bit char. in header of news message */
			if (match_boolean (buf, "post_8bit_header=", &tinrc.post_8bit_header)) {
				if (strcasecmp(txt_mime_encodings[tinrc.post_mime_encoding], txt_8bit))
					tinrc.post_8bit_header = FALSE;
				break;
			}

#ifndef DISABLE_PRINTING
			if (match_boolean (buf, "print_header=", &tinrc.print_header))
				break;
#endif /* !DISABLE_PRINTING */

			if (match_boolean (buf, "pos_first_unread=", &tinrc.pos_first_unread))
				break;

			if (match_integer (buf, "post_process_type=", &tinrc.post_process, POST_PROC_UUD_EXT_ZIP)) {
				proc_ch_default = get_post_proc_type (tinrc.post_process);
				break;
			}

			if (match_string (buf, "post_process_command=", tinrc.post_process_command, sizeof(tinrc.post_process_command)))
				break;

			if (match_boolean (buf, "process_only_unread=", &tinrc.process_only_unread))
				break;

			if (match_boolean (buf, "prompt_followupto=", &tinrc.prompt_followupto))
				break;

			if (match_boolean (buf, "pgdn_goto_next=", &tinrc.pgdn_goto_next))
				break;

			break;

		case 'q':
			if (match_string (buf, "quote_chars=", tinrc.quote_chars, sizeof (tinrc.quote_chars))) {
				quote_dash_to_space (tinrc.quote_chars);
				break;
			}

			if (match_boolean (buf, "quote_empty_lines=", &tinrc.quote_empty_lines))
				break;

#ifdef HAVE_COLOR
			if (match_string (buf, "quote_regex=", tinrc.quote_regex, sizeof (tinrc.quote_regex)))
				break;

			if (match_string (buf, "quote_regex2=", tinrc.quote_regex2, sizeof (tinrc.quote_regex2)))
				break;

			if (match_string (buf, "quote_regex3=", tinrc.quote_regex3, sizeof (tinrc.quote_regex3)))
				break;
#endif /* HAVE_COLOR */

			if (match_boolean (buf, "quote_signatures=", &tinrc.quote_signatures))
				break;

			break;

		case 'r':
			if (match_integer (buf, "reread_active_file_secs=", &tinrc.reread_active_file_secs, 10000))
				break;

			break;

		case 's':
			if (match_integer (buf, "show_author=", &tinrc.show_author, SHOW_FROM_BOTH))
				break;

			if (match_boolean (buf, "show_description=", &tinrc.show_description)) {
				show_description = tinrc.show_description;
				break;
			}

			if (match_boolean (buf, "show_only_unread=", &tinrc.show_only_unread_arts))
				break;

			if (match_boolean (buf, "show_only_unread_groups=", &tinrc.show_only_unread_groups))
				break;

			if (match_boolean (buf, "sigdashes=", &tinrc.sigdashes))
				break;

			if (match_boolean (buf, "signature_repost=", &tinrc.signature_repost))
				break;

			if (match_string (buf, "spamtrap_warning_addresses=", tinrc.spamtrap_warning_addresses, sizeof (tinrc.spamtrap_warning_addresses)))
				break;

			if (match_boolean (buf, "start_editor_offset=", &tinrc.start_editor_offset))
				break;

			if (match_integer (buf, "sort_article_type=", &tinrc.sort_article_type, SORT_BY_SCORE_ASCEND))
				break;

			if (match_boolean (buf, "show_last_line_prev_page=", &tinrc.show_last_line_prev_page))
				break;

			if (match_boolean (buf, "show_lines=" , &tinrc.show_lines))
				break;

			if (match_boolean (buf, "show_score=" , &tinrc.show_score))
				break;

			if (match_boolean (buf, "show_signatures=", &tinrc.show_signatures))
				break;

			if (match_boolean (buf, "save_to_mmdf_mailbox=", &tinrc.save_to_mmdf_mailbox))
				break;

			if (match_boolean (buf, "strip_blanks=", &tinrc.strip_blanks))
				break;

			if (match_integer (buf, "strip_bogus=", &tinrc.strip_bogus, BOGUS_ASK))
				break;

			if (match_boolean (buf, "strip_newsrc=", &tinrc.strip_newsrc))
				break;

			if (match_boolean (buf, "show_xcommentto=", &tinrc.show_xcommentto))
				break;

			/* Regexp used to strip "Re: "s and similar */
			if (match_string (buf, "strip_re_regex=", tinrc.strip_re_regex, sizeof (tinrc.strip_re_regex)))
				break;

			if (match_string (buf, "strip_was_regex=", tinrc.strip_was_regex, sizeof (tinrc.strip_was_regex)))
				break;

			if (match_boolean (buf, "space_goto_next_unread=", &tinrc.space_goto_next_unread))
				break;

			break;

		case 't':
			if (match_integer (buf, "thread_articles=", &tinrc.thread_articles, THREAD_MAX)) {
				/* Upgrade changes YES/NO to integer, fix it ! */
				if (upgrade == UPGRADE)
					tinrc.thread_articles = THREAD_MAX;
				break;
			}

			if (match_boolean (buf, "tab_after_X_selection=", &tinrc.tab_after_X_selection))
				break;

			if (match_boolean (buf, "tab_goto_next_unread=", &tinrc.tab_goto_next_unread))
				break;

			if (match_boolean (buf, "thread_catchup_on_exit=", &tinrc.thread_catchup_on_exit))
				break;

			break;

		case 'u':
			if (match_boolean (buf, "unlink_article=", &tinrc.unlink_article))
				break;

#if defined(NNTP_ABLE) || defined(NNTP_ONLY)
			if (match_boolean (buf, "use_builtin_inews=", &tinrc.use_builtin_inews))
				break;
#endif /* NNTP_ABLE || NNTP_ONLY */

			if (match_boolean (buf, "use_getart_limit=", &tinrc.use_getart_limit))
				break;

			if (match_boolean (buf, "use_mailreader_i=", &tinrc.use_mailreader_i))
				break;

			if (match_boolean (buf, "use_mouse=", &tinrc.use_mouse))
				break;

#ifdef HAVE_KEYPAD
			if (match_boolean (buf, "use_keypad=", &tinrc.use_keypad))
				break;
#endif /* HAVE_KEYPAD */

#ifdef HAVE_METAMAIL
			if (match_boolean (buf, "use_metamail=", &tinrc.use_metamail))
				break;
#endif /* HAVE_METAMAIL */

#ifdef HAVE_COLOR
			if (match_boolean (buf, "use_color=", &tinrc.use_color)) {
				use_color = tinrc.use_color;
				break;
			}
#endif /* HAVE_COLOR */

			break;

		case 'w':
			if (match_integer (buf, "wildcard=", &tinrc.wildcard, 2)) {
				wildcard_func = (tinrc.wildcard) ? match_regex : wildmat;
				break;
			}

#ifdef HAVE_COLOR
			if (match_boolean (buf, "word_highlight=", &tinrc.word_highlight)) {
				word_highlight = tinrc.word_highlight;
				break;
			}

			if (match_integer (buf, "word_h_display_marks=", &tinrc.word_h_display_marks, MAX_MARK))
				break;
#endif /* HAVE_COLOR */

			break;

		case 'x':
			if (match_string (buf, "xpost_quote_format=", tinrc.xpost_quote_format, sizeof (tinrc.xpost_quote_format)))
				break;

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
	if (!(tinrc.draw_arrow || tinrc.inverse_okay))
		tinrc.draw_arrow = TRUE;

#if 0
	if (INTERACTIVE)
		wait_message (0, "\n");
#endif /* 0 */

	return TRUE;
}


/*
 *  write config defaults to ~/.tin/tinrc
 */
void
write_config_file (
	char *file)
{
	FILE *fp;
	char *file_tmp;
	int i;

	if (no_write && file_size (file) != -1)
		return;

	/* alloc memory for tmp-filename */
	file_tmp = (char *) my_malloc (strlen (file)+5);

	/* generate tmp-filename */
	sprintf (file_tmp, "%s.tmp", file);

	if ((fp = fopen (file_tmp, "w")) == (FILE *) 0) {
		error_message (txt_filesystem_full_backup, CONFIG_FILE);
		/* free memory for tmp-filename */
		free (file_tmp);
		return;
	}

	if (!cmd_line)
		wait_message (0, txt_saving);

	if (!*tinrc.editor_format)
		strcpy (tinrc.editor_format, TIN_EDITOR_FMT_ON);

	fprintf (fp, txt_tinrc_header, PRODUCT, TINRC_VERSION, tin_progname, VERSION, RELEASEDATE, RELEASENAME);

	fprintf (fp, txt_savedir.tinrc);
	fprintf (fp, "default_savedir=%s\n\n", tinrc.savedir);

	fprintf (fp, txt_auto_save.tinrc);
	fprintf (fp, "auto_save=%s\n\n", print_boolean (tinrc.auto_save));

	fprintf (fp, txt_mark_saved_read.tinrc);
	fprintf (fp, "mark_saved_read=%s\n\n", print_boolean (tinrc.mark_saved_read));

	fprintf (fp, txt_post_process.tinrc);
	fprintf (fp, "post_process_type=%d\n\n", tinrc.post_process);

	fprintf (fp, txt_tinrc_post_process_command);
	fprintf (fp, "post_process_command=%s\n\n", tinrc.post_process_command);

	fprintf (fp, txt_process_only_unread.tinrc);
	fprintf (fp, "process_only_unread=%s\n\n", print_boolean (tinrc.process_only_unread));

	fprintf (fp, txt_prompt_followupto.tinrc);
	fprintf (fp, "prompt_followupto=%s\n\n", print_boolean (tinrc.prompt_followupto));

	fprintf (fp, txt_confirm_action.tinrc);
	fprintf (fp, "confirm_action=%s\n\n", print_boolean (tinrc.confirm_action));

	fprintf (fp, txt_confirm_to_quit.tinrc);
	fprintf (fp, "confirm_to_quit=%s\n\n", print_boolean (tinrc.confirm_to_quit));

	fprintf (fp, txt_auto_reconnect.tinrc);
	fprintf (fp, "auto_reconnect=%s\n\n", print_boolean (tinrc.auto_reconnect));

	fprintf (fp, txt_draw_arrow.tinrc);
	fprintf (fp, "draw_arrow=%s\n\n", print_boolean (tinrc.draw_arrow));

	fprintf (fp, txt_inverse_okay.tinrc);
	fprintf (fp, "inverse_okay=%s\n\n", print_boolean (tinrc.inverse_okay));

	fprintf (fp, txt_pos_first_unread.tinrc);
	fprintf (fp, "pos_first_unread=%s\n\n", print_boolean (tinrc.pos_first_unread));

	fprintf (fp, txt_show_only_unread_arts.tinrc);
	fprintf (fp, "show_only_unread=%s\n\n", print_boolean (tinrc.show_only_unread_arts));

	fprintf (fp, txt_show_only_unread_groups.tinrc);
	fprintf (fp, "show_only_unread_groups=%s\n\n", print_boolean (tinrc.show_only_unread_groups));

	fprintf (fp, txt_kill_level.tinrc);
	fprintf (fp, "kill_level=%d\n\n", tinrc.kill_level);

	fprintf (fp, txt_tab_goto_next_unread.tinrc);
	fprintf (fp, "tab_goto_next_unread=%s\n\n", print_boolean (tinrc.tab_goto_next_unread));

	fprintf (fp, txt_space_goto_next_unread.tinrc);
	fprintf (fp, "space_goto_next_unread=%s\n\n", print_boolean (tinrc.space_goto_next_unread));

	fprintf (fp, txt_pgdn_goto_next.tinrc);
	fprintf (fp, "pgdn_goto_next=%s\n\n", print_boolean (tinrc.pgdn_goto_next));

	fprintf (fp, txt_tab_after_X_selection.tinrc);
	fprintf (fp, "tab_after_X_selection=%s\n\n", print_boolean (tinrc.tab_after_X_selection));

	fprintf (fp, txt_full_page_scroll.tinrc);
	fprintf (fp, "full_page_scroll=%s\n\n", print_boolean (tinrc.full_page_scroll));

	fprintf (fp, txt_show_last_line_prev_page.tinrc);
	fprintf (fp, "show_last_line_prev_page=%s\n\n", print_boolean (tinrc.show_last_line_prev_page));

	fprintf (fp, txt_catchup_read_groups.tinrc);
	fprintf (fp, "catchup_read_groups=%s\n\n", print_boolean (tinrc.catchup_read_groups));

	fprintf (fp, txt_group_catchup_on_exit.tinrc);
	fprintf (fp, "group_catchup_on_exit=%s\n", print_boolean (tinrc.group_catchup_on_exit));
	fprintf (fp, "thread_catchup_on_exit=%s\n\n", print_boolean (tinrc.thread_catchup_on_exit));

	fprintf (fp, txt_thread_articles.tinrc);
	fprintf (fp, "thread_articles=%d\n\n", tinrc.thread_articles);

	fprintf (fp, txt_show_description.tinrc);
	fprintf (fp, "show_description=%s\n\n", print_boolean (tinrc.show_description));

	fprintf (fp, txt_show_author.tinrc);
	fprintf (fp, "show_author=%d\n\n", tinrc.show_author);

	fprintf (fp, txt_news_headers_to_display.tinrc);
	fprintf (fp, "news_headers_to_display=");
	for (i=0; i<num_headers_to_display; i++)
		fprintf (fp, "%s ", news_headers_to_display_array[i]);
	fprintf (fp, "\n\n");

	fprintf (fp, txt_news_headers_to_not_display.tinrc);
	fprintf (fp, "news_headers_to_not_display=");
	for (i=0; i<num_headers_to_not_display; i++)
		fprintf (fp, "%s ", news_headers_to_not_display_array[i]);
	fprintf (fp, "\n\n");

	fprintf (fp, txt_tinrc_info_in_last_line);
	fprintf (fp, "info_in_last_line=%s\n\n", print_boolean(tinrc.info_in_last_line));

	fprintf (fp, txt_sort_article_type.tinrc);
	fprintf (fp, "sort_article_type=%d\n\n", tinrc.sort_article_type);

	fprintf (fp, txt_maildir.tinrc);
	fprintf (fp, "default_maildir=%s\n\n", tinrc.maildir);

	fprintf (fp, txt_save_to_mmdf_mailbox.tinrc);
	fprintf (fp, "save_to_mmdf_mailbox=%s\n\n", print_boolean (tinrc.save_to_mmdf_mailbox));

	fprintf (fp, txt_show_xcommentto.tinrc);
	fprintf (fp, "show_xcommentto=%s\n\n", print_boolean(tinrc.show_xcommentto));

#ifndef DISABLE_PRINTING
	fprintf (fp, txt_print_header.tinrc);
	fprintf (fp, "print_header=%s\n\n", print_boolean (tinrc.print_header));

	fprintf (fp, txt_printer.tinrc);
	fprintf (fp, "default_printer=%s\n\n", tinrc.printer);
#endif /* !DISABLE_PRINTING */

	fprintf (fp, txt_batch_save.tinrc);
	fprintf (fp, "batch_save=%s\n\n", print_boolean (tinrc.batch_save));

	fprintf (fp, txt_start_editor_offset.tinrc);
	fprintf (fp, "start_editor_offset=%s\n\n", print_boolean (tinrc.start_editor_offset));

	fprintf (fp, txt_editor_format.tinrc);
	fprintf (fp, "default_editor_format=%s\n\n", tinrc.editor_format);

	fprintf (fp, txt_mailer_format.tinrc);
	fprintf (fp, "default_mailer_format=%s\n\n", tinrc.mailer_format);

	fprintf (fp, txt_use_mailreader_i.tinrc);
	fprintf (fp, "use_mailreader_i=%s\n\n", print_boolean (tinrc.use_mailreader_i));

	fprintf (fp, txt_show_lines.tinrc);
	fprintf (fp, "show_lines=%s\n\n", print_boolean(tinrc.show_lines));

	fprintf (fp, txt_show_score.tinrc);
	fprintf (fp, "show_score=%s\n\n", print_boolean(tinrc.show_score));

	fprintf (fp, txt_unlink_article.tinrc);
	fprintf (fp, "unlink_article=%s\n\n", print_boolean (tinrc.unlink_article));

	fprintf (fp, txt_keep_dead_articles.tinrc);
	fprintf (fp, "keep_dead_articles=%s\n\n", print_boolean (tinrc.keep_dead_articles));

	fprintf (fp, txt_keep_posted_articles.tinrc);
	fprintf (fp, "keep_posted_articles=%s\n\n", print_boolean (tinrc.keep_posted_articles));

	fprintf (fp, txt_add_posted_to_filter.tinrc);
	fprintf (fp, "add_posted_to_filter=%s\n\n", print_boolean (tinrc.add_posted_to_filter));

	fprintf (fp, txt_sigfile.tinrc);
	fprintf (fp, "default_sigfile=%s\n\n", tinrc.sigfile);

	fprintf (fp, txt_sigdashes.tinrc);
	fprintf (fp, "sigdashes=%s\n\n", print_boolean (tinrc.sigdashes));

	fprintf (fp, txt_signature_repost.tinrc);
	fprintf (fp, "signature_repost=%s\n\n", print_boolean (tinrc.signature_repost));

	fprintf (fp, txt_spamtrap_warning_addresses.tinrc);
	fprintf (fp, "spamtrap_warning_addresses=%s\n\n", tinrc.spamtrap_warning_addresses);

	fprintf (fp, txt_advertising.tinrc);
	fprintf (fp, "advertising=%s\n\n", print_boolean (tinrc.advertising));

	fprintf (fp, txt_reread_active_file_secs.tinrc);
	fprintf (fp, "reread_active_file_secs=%d\n\n", tinrc.reread_active_file_secs);

	fprintf (fp, txt_quote_chars.tinrc);
	fprintf (fp, "quote_chars=%s\n\n", quote_space_to_dash (tinrc.quote_chars));

	fprintf (fp, txt_quote_empty_lines.tinrc);
	fprintf (fp, "quote_empty_lines=%s\n\n", print_boolean(tinrc.quote_empty_lines));

#ifdef HAVE_COLOR
	fprintf (fp, txt_quote_regex.tinrc);
	fprintf (fp, "quote_regex=%s\n\n", tinrc.quote_regex);
	fprintf (fp, txt_quote_regex2.tinrc);
	fprintf (fp, "quote_regex2=%s\n\n", tinrc.quote_regex2);
	fprintf (fp, txt_quote_regex3.tinrc);
	fprintf (fp, "quote_regex3=%s\n\n", tinrc.quote_regex3);
#endif /* HAVE_COLOR */

	fprintf (fp, txt_strip_re_regex.tinrc);
	fprintf (fp, "strip_re_regex=%s\n\n", tinrc.strip_re_regex);
	fprintf (fp, txt_strip_was_regex.tinrc);
	fprintf (fp, "strip_was_regex=%s\n\n", tinrc.strip_was_regex);

	fprintf (fp, txt_quote_signatures.tinrc);
	fprintf (fp, "quote_signatures=%s\n\n", print_boolean(tinrc.quote_signatures));

	fprintf (fp, txt_show_signatures.tinrc);
	fprintf (fp, "show_signatures=%s\n\n", print_boolean(tinrc.show_signatures));

	fprintf (fp, txt_news_quote_format.tinrc);
	fprintf (fp, "news_quote_format=%s\n", tinrc.news_quote_format);
	fprintf (fp, "mail_quote_format=%s\n", tinrc.mail_quote_format);
	fprintf (fp, "xpost_quote_format=%s\n\n", tinrc.xpost_quote_format);

	fprintf (fp, txt_auto_cc.tinrc);
	fprintf (fp, "auto_cc=%s\n\n", print_boolean (tinrc.auto_cc));

	fprintf (fp, txt_auto_bcc.tinrc);
	fprintf (fp, "auto_bcc=%s\n\n", print_boolean (tinrc.auto_bcc));

	fprintf (fp, txt_art_marked_deleted.tinrc);
	fprintf (fp, "art_marked_deleted=%c\n\n", tinrc.art_marked_deleted);

	fprintf (fp, txt_art_marked_inrange.tinrc);
	fprintf (fp, "art_marked_inrange=%c\n\n", tinrc.art_marked_inrange);

	fprintf (fp, txt_art_marked_return.tinrc);
	fprintf (fp, "art_marked_return=%c\n\n", tinrc.art_marked_return);

	fprintf (fp, txt_art_marked_selected.tinrc);
	fprintf (fp, "art_marked_selected=%c\n\n", tinrc.art_marked_selected);

	fprintf (fp, txt_art_marked_unread.tinrc);
	fprintf (fp, "art_marked_unread=%c\n\n", tinrc.art_marked_unread);

	fprintf (fp, txt_force_screen_redraw.tinrc);
	fprintf (fp, "force_screen_redraw=%s\n\n", print_boolean (tinrc.force_screen_redraw));

#if defined(NNTP_ABLE) || defined(NNTP_ONLY)
	fprintf (fp, txt_use_builtin_inews.tinrc);
	fprintf (fp, "use_builtin_inews=%s\n\n", print_boolean (tinrc.use_builtin_inews));
#endif /* NNTP_ABLE || NNTP_ONLY */

	fprintf (fp, txt_auto_list_thread.tinrc);
	fprintf (fp, "auto_list_thread=%s\n\n", print_boolean (tinrc.auto_list_thread));

	fprintf (fp, txt_use_mouse.tinrc);
	fprintf (fp, "use_mouse=%s\n\n", print_boolean (tinrc.use_mouse));

	fprintf (fp, txt_strip_blanks.tinrc);
	fprintf (fp, "strip_blanks=%s\n\n", print_boolean (tinrc.strip_blanks));

	fprintf (fp, txt_groupname_max_length.tinrc);
	fprintf (fp, "groupname_max_length=%d\n\n", tinrc.groupname_max_length);

	fprintf (fp, txt_beginner_level.tinrc);
	fprintf (fp, "beginner_level=%s\n\n", print_boolean (tinrc.beginner_level));

	fprintf (fp, txt_filter_days.tinrc);
	fprintf (fp, "default_filter_days=%d\n\n", tinrc.filter_days);

	fprintf (fp, txt_cache_overview_files.tinrc);
	fprintf (fp, "cache_overview_files=%s\n\n", print_boolean (tinrc.cache_overview_files));

	fprintf (fp, txt_use_getart_limit.tinrc);
	fprintf (fp, "use_getart_limit=%s\n\n", print_boolean (tinrc.use_getart_limit));

	fprintf (fp, txt_getart_limit.tinrc);
	fprintf (fp, "getart_limit=%d\n\n", tinrc.getart_limit);

#ifdef HAVE_COLOR
	fprintf (fp, txt_use_color.tinrc);
	fprintf (fp, "use_color=%s\n\n", print_boolean (tinrc.use_color));

	fprintf (fp, txt_tinrc_colors);

	fprintf (fp, txt_col_normal.tinrc);
	fprintf (fp, "col_normal=%d\n\n", tinrc.col_normal);

	fprintf (fp, txt_col_back.tinrc);
	fprintf (fp, "col_back=%d\n\n", tinrc.col_back);

	fprintf (fp, txt_col_invers_bg.tinrc);
	fprintf (fp, "col_invers_bg=%d\n\n", tinrc.col_invers_bg);

	fprintf (fp, txt_col_invers_fg.tinrc);
	fprintf (fp, "col_invers_fg=%d\n\n", tinrc.col_invers_fg);

	fprintf (fp, txt_col_text.tinrc);
	fprintf (fp, "col_text=%d\n\n", tinrc.col_text);

	fprintf (fp, txt_col_minihelp.tinrc);
	fprintf (fp, "col_minihelp=%d\n\n", tinrc.col_minihelp);

	fprintf (fp, txt_col_help.tinrc);
	fprintf (fp, "col_help=%d\n\n", tinrc.col_help);

	fprintf (fp, txt_col_message.tinrc);
	fprintf (fp, "col_message=%d\n\n", tinrc.col_message);

	fprintf (fp, txt_col_quote.tinrc);
	fprintf (fp, "col_quote=%d\n\n", tinrc.col_quote);

	fprintf (fp, txt_col_quote2.tinrc);
	fprintf (fp, "col_quote2=%d\n\n", tinrc.col_quote2);

	fprintf (fp, txt_col_quote3.tinrc);
	fprintf (fp, "col_quote3=%d\n\n", tinrc.col_quote3);

	fprintf (fp, txt_col_head.tinrc);
	fprintf (fp, "col_head=%d\n\n", tinrc.col_head);

	fprintf (fp, txt_col_newsheaders.tinrc);
	fprintf (fp, "col_newsheaders=%d\n\n", tinrc.col_newsheaders);

	fprintf (fp, txt_col_subject.tinrc);
	fprintf (fp, "col_subject=%d\n\n", tinrc.col_subject);

	fprintf (fp, txt_col_response.tinrc);
	fprintf (fp, "col_response=%d\n\n", tinrc.col_response);

	fprintf (fp, txt_col_from.tinrc);
	fprintf (fp, "col_from=%d\n\n", tinrc.col_from);

	fprintf (fp, txt_col_title.tinrc);
	fprintf (fp, "col_title=%d\n\n", tinrc.col_title);

	fprintf (fp, txt_col_signature.tinrc);
	fprintf (fp, "col_signature=%d\n\n", tinrc.col_signature);

	fprintf (fp, txt_word_highlight.tinrc);
	fprintf (fp, "word_highlight=%s\n\n", print_boolean (tinrc.word_highlight));

	fprintf (fp, txt_word_h_display_marks.tinrc);
	fprintf (fp, "word_h_display_marks=%d\n\n", tinrc.word_h_display_marks);

	fprintf (fp, txt_col_markstar.tinrc);
	fprintf (fp, "col_markstar=%d\n", tinrc.col_markstar);
	fprintf (fp, "col_markdash=%d\n\n", tinrc.col_markdash);
#endif /* HAVE_COLOR */

	fprintf (fp, txt_mail_address.tinrc);
	fprintf (fp, "mail_address=%s\n\n", tinrc.mail_address);

	fprintf (fp, txt_mm_charset.tinrc);
	fprintf (fp, "mm_charset=%s\n\n", tinrc.mm_charset);

/* Not on Option Menu */
#ifdef LOCAL_CHARSET
	fprintf (fp, txt_tinrc_local_charset);
	fprintf (fp, "local_charset=%s\n\n", print_boolean(use_local_charset));
#endif /* LOCAL_CHARSET */

	fprintf (fp, txt_post_mime_encoding.tinrc);
	fprintf (fp, "post_mime_encoding=%s\n", txt_mime_encodings[tinrc.post_mime_encoding]);
	fprintf (fp, "mail_mime_encoding=%s\n\n", txt_mime_encodings[tinrc.mail_mime_encoding]);

	fprintf (fp, txt_post_8bit_header.tinrc);
	fprintf (fp, "post_8bit_header=%s\n\n", print_boolean(tinrc.post_8bit_header));

	fprintf (fp, txt_mail_8bit_header.tinrc);
	fprintf (fp, "mail_8bit_header=%s\n\n", print_boolean(tinrc.mail_8bit_header));

	fprintf (fp, txt_display_mime_header_asis.tinrc);
	fprintf (fp, "display_mime_header_asis=%s\n\n", print_boolean(tinrc.display_mime_header_asis));

	fprintf (fp, txt_display_mime_allheader_asis.tinrc);
	fprintf (fp, "display_mime_allheader_asis=%s\n\n", print_boolean(tinrc.display_mime_allheader_asis));

#ifdef HAVE_METAMAIL
	fprintf (fp, txt_use_metamail.tinrc);
	fprintf (fp, "use_metamail=%s\n\n", print_boolean (tinrc.use_metamail));

	fprintf (fp, txt_ask_for_metamail.tinrc);
	fprintf (fp, "ask_for_metamail=%s\n\n", print_boolean (tinrc.ask_for_metamail));
#endif /* HAVE_METAMAIL */

#ifdef HAVE_KEYPAD
	fprintf (fp, txt_use_keypad.tinrc);
	fprintf (fp, "use_keypad=%s\n\n", print_boolean (tinrc.use_keypad));
#endif /* HAVE_KEYPAD */

	fprintf (fp, txt_alternative_handling.tinrc);
	fprintf (fp, "alternative_handling=%s\n\n", print_boolean (tinrc.alternative_handling));

	fprintf (fp, txt_strip_newsrc.tinrc);
	fprintf (fp, "strip_newsrc=%s\n\n", print_boolean (tinrc.strip_newsrc));

	fprintf (fp, txt_strip_bogus.tinrc);
	fprintf (fp, "strip_bogus=%d\n\n", tinrc.strip_bogus);

	fprintf (fp, txt_wildcard.tinrc);
	fprintf (fp, "wildcard=%d\n\n", tinrc.wildcard);

	fprintf (fp, txt_tinrc_filter);
	fprintf (fp, "default_filter_kill_header=%d\n", tinrc.default_filter_kill_header);
	fprintf (fp, "default_filter_kill_global=%s\n", print_boolean (tinrc.default_filter_kill_global));
	/* ON=false, OFF=true */
	fprintf (fp, "default_filter_kill_case=%s\n", print_boolean (!tinrc.default_filter_kill_case));
	fprintf (fp, "default_filter_kill_expire=%s\n", print_boolean (tinrc.default_filter_kill_expire));
	fprintf (fp, "default_filter_select_header=%d\n", tinrc.default_filter_select_header);
	fprintf (fp, "default_filter_select_global=%s\n", print_boolean (tinrc.default_filter_select_global));
	/* ON=false, OFF=true */
	fprintf (fp, "default_filter_select_case=%s\n", print_boolean (!tinrc.default_filter_select_case));
	fprintf (fp, "default_filter_select_expire=%s\n\n", print_boolean (tinrc.default_filter_select_expire));

	fprintf (fp, txt_tinrc_defaults);
	fprintf (fp, "default_save_mode=%c\n", tinrc.default_save_mode);
	fprintf (fp, "default_author_search=%s\n", tinrc.default_search_author);
	fprintf (fp, "default_goto_group=%s\n", tinrc.default_goto_group);
	fprintf (fp, "default_config_search=%s\n", tinrc.default_search_config);
	fprintf (fp, "default_group_search=%s\n", tinrc.default_search_group);
	fprintf (fp, "default_subject_search=%s\n", tinrc.default_search_subject);
	fprintf (fp, "default_art_search=%s\n", tinrc.default_search_art);
	fprintf (fp, "default_repost_group=%s\n", tinrc.default_repost_group);
	fprintf (fp, "default_mail_address=%s\n", tinrc.default_mail_address);
	fprintf (fp, "default_move_group=%d\n", tinrc.default_move_group);
#ifndef DONT_HAVE_PIPING
	fprintf (fp, "default_pipe_command=%s\n", tinrc.default_pipe_command);
#endif /* DONT_HAVE_PIPING */
	fprintf (fp, "default_post_newsgroups=%s\n", tinrc.default_post_newsgroups);
	fprintf (fp, "default_post_subject=%s\n", tinrc.default_post_subject);
	fprintf (fp, "default_range_group=%s\n", tinrc.default_range_group);
	fprintf (fp, "default_range_select=%s\n", tinrc.default_range_select);
	fprintf (fp, "default_range_thread=%s\n", tinrc.default_range_thread);
	fprintf (fp, "default_regex_pattern=%s\n", tinrc.default_regex_pattern);
	fprintf (fp, "default_save_file=%s\n", tinrc.default_save_file);
	fprintf (fp, "default_select_pattern=%s\n", tinrc.default_select_pattern);
	fprintf (fp, "default_shell_command=%s\n\n", tinrc.default_shell_command);

	fprintf (fp, txt_tinrc_newnews);
	for (i = 0; i < num_newnews; i++)
		fprintf (fp, "newnews=%s %lu\n", newnews[i].host, (unsigned long int) newnews[i].time);

	if (ferror (fp) || fclose (fp))
		error_message (txt_filesystem_full, CONFIG_FILE);
	else {
		rename_file (file_tmp, file);
		chmod (file, (mode_t)(S_IRUSR|S_IWUSR));
	}
	/* free memory for tmp-filename */
	free (file_tmp);
}

#define option_lines_per_page (cLINES - INDEX_TOP - 3)

static int first_option_on_screen;
static int actual_top_option = 0;


static void
print_any_option (
	int act_option)
{
	constext **list;

	my_printf("%3d. %s ", act_option+1, option_table[act_option].txt->opt);
	switch (option_table[act_option].var_type) {
		case OPT_ON_OFF:
			my_printf("%s ", print_boolean(*OPT_ON_OFF_list[option_table[act_option].var_index]));
			break;
		case OPT_LIST:
			list = option_table[act_option].opt_list;
			my_printf("%s", list[*(option_table[act_option].variable) + ((strcasecmp(list[0], txt_default) == 0) ? 1 : 0)]);
			break;
		case OPT_STRING:
			my_printf("%-.*s", cCOLS - (int) strlen(option_table[act_option].txt->opt) - OPT_ARG_COLUMN - 3, OPT_STRING_list[option_table[act_option].var_index]);
			break;
		case OPT_NUM:
			my_printf("%d", *(option_table[act_option].variable));
			break;
		case OPT_CHAR:
			my_printf("%c", *OPT_CHAR_list[option_table[act_option].var_index]);
			break;
		default:
			break;
	}
#ifdef USE_CURSES
	clrtoeol();
#endif /* USE_CURSES */
}


static void
print_option (
	enum option_enum the_option)
{
	print_any_option((int)the_option);
}


static t_bool
OptionOnPage (
	int option)
{
	if ((option >= first_option_on_screen) && (option < first_option_on_screen + option_lines_per_page))
		return TRUE;
	return FALSE;
}

#define TopOfPage(option) option_lines_per_page \
			* ((option) / option_lines_per_page)

#define OptionInPage(option)	((option) - first_option_on_screen)
#define OptionIndex(option)	(OptionInPage(option) % option_lines_per_page)


int
option_row (
	int option)
{
	return (INDEX_TOP + OptionIndex(option));
}


static void
RepaintOption (
	int option)
{
	if (OptionOnPage(option)) {
		MoveCursor (option_row(option), 3);
		print_any_option (option);
	}
}


#ifdef USE_CURSES
static void DoScroll (
	int jump)
{
	int y, x;
	getyx(stdscr, y, x);
	move(INDEX_TOP, 0);
	setscrreg(INDEX_TOP, INDEX_TOP + option_lines_per_page - 1);
	scrl(jump);
	setscrreg(0, LINES-1);
}
#endif /* USE_CURSES */


static void
highlight_option (
	int option)
{
	if (!OptionOnPage(option)) {
#ifdef USE_CURSES
		if (option > 0 && OptionOnPage(option-1)) {
			DoScroll(1);
			first_option_on_screen++;
		} else if (option < LAST_OPT && OptionOnPage(option+1)) {
			DoScroll(-1);
			first_option_on_screen--;
		} else
#endif /* USE_CURSES */
		{
			first_option_on_screen = TopOfPage(option);
			ClearScreen();
		}
	}

	refresh_config_page (option);
	MoveCursor (option_row(option), 0);
	my_fputs ("->", stdout);
	my_flush();
	stow_cursor();
}


static void
unhighlight_option (
	int option)
{
	MoveCursor (option_row(option), 0);
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
 * first_option_on_page == actual_top_option even if there are now more/less
 * options on the screen than before).
 */
void
refresh_config_page (
	int act_option)
{
	static int last_option = 0;
	t_bool force_redraw = FALSE;

	signal_context = cConfig;

	if (act_option < 0) {
		force_redraw = TRUE;
		act_option = last_option;
		ClearScreen ();
	}

	if ((first_option_on_screen != actual_top_option) || force_redraw)
	{
		show_config_page ();
		actual_top_option = first_option_on_screen;
	}
	last_option = act_option;
}

/*
 *  options menu so that the user can dynamically change parameters
 */

int
change_config_file (
	struct t_group *group)
{
	int ch = 0;
	int original_list_value;
	int option, old_option;
	int ret_code = NO_FILTERING;
	int mime_encoding = MIME_ENCODING_7BIT;
	t_bool change_option = FALSE;
	t_bool original_on_off_value;


	actual_top_option = -1;
	option = 0;

	ClearScreen ();
	set_xclick_off ();
	forever {

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
#	endif /* HAVE_KEY_PREFIX */
				switch (get_arrow_key (ch)) {
#endif /* !WIN32 */
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
#endif /* !WIN32 */
			default:
				break;
		}	/* switch (ch) */

		switch (ch) {
			case iKeyQuit:
				write_config_file (local_config_file);
				nobreak; /* FALLTHROUGH */
			case iKeyConfigNoSave:
				clear_note_area ();
				return ret_code;

			case iKeyUp:
			case iKeyUp2:
				unhighlight_option (option);
				if (--option < 0)
					option = LAST_OPT;
				highlight_option (option);
				break;

			case iKeyDown:
			case iKeyDown2:
				unhighlight_option (option);
				if (++option > LAST_OPT)
					option = 0;
				highlight_option (option);
				break;

			case iKeyFirstPage:
			case iKeyConfigFirstPage:
				unhighlight_option (option);
				option = 0;
				highlight_option (option);
				break;

			case iKeyLastPage:
			case iKeyConfigLastPage:
				unhighlight_option (option);
				option = LAST_OPT;
				highlight_option (option);
				break;

			case iKeyPageUp:
			case iKeyPageUp2:
			case iKeyPageUp3:
				unhighlight_option (option);
				if (OptionInPage(option)) {
					option = first_option_on_screen;
				} else if (!first_option_on_screen) {
					option = LAST_OPT;
					first_option_on_screen = TopOfPage(option);
					ClearScreen ();
					show_config_page ();
				} else if ((option -= option_lines_per_page) < 0) {
					option = 0;
					first_option_on_screen = 0;
				} else {
					first_option_on_screen -= option_lines_per_page;
					ClearScreen ();
					show_config_page ();
				}
				highlight_option (option);
				break;

			case iKeyPageDown:
			case iKeyPageDown2:
			case iKeyPageDown3:
				unhighlight_option (option);
				first_option_on_screen += option_lines_per_page;
				if (first_option_on_screen > LAST_OPT)
					first_option_on_screen = 0;

				option = first_option_on_screen;
				ClearScreen ();
				show_config_page ();
				highlight_option (option);
				break;

			case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				unhighlight_option (option);
				old_option = option;
				option = prompt_num (ch, txt_enter_option_num) - 1;
				if (option < 0 || option > LAST_OPT) {
					option = old_option;
					break;
				}
				highlight_option (option);
				break;

			case iKeySearchSubjF:
			case iKeySearchSubjB:
				old_option = option;
				option = search_config(ch == iKeySearchSubjF, option, LAST_OPT);
				if (option != old_option) {
					unhighlight_option (old_option);
					highlight_option (option);
				}
				break;

			case iKeyConfigSelect:
			case iKeyConfigSelect2:
				change_option = TRUE;
				break;

			case iKeyRedrawScr:	/* redraw screen */
				my_retouch ();
				set_xclick_off ();
				ClearScreen ();
				show_config_page ();
				highlight_option (option);
				break;

			default:
				break;
		} /* switch (ch) */

		if (change_option) {
			switch (option_table[option].var_type) {
				case OPT_ON_OFF:
					original_on_off_value = *OPT_ON_OFF_list[option_table[option].var_index];
					prompt_on_off (option_row(option),
						OPT_ARG_COLUMN,
						OPT_ON_OFF_list[option_table[option].var_index],
						option_table[option].txt->help,
						option_table[option].txt->opt
						);
					/*
					 * some options need further action to take effect
					 */
					switch (option) {
						/* show mini help menu */
						case OPT_BEGINNER_LEVEL:
							if (!bool_equal(tinrc.beginner_level, original_on_off_value))
								(void) set_win_size (&cLINES, &cCOLS);
							break;

						/* show all arts or just new/unread arts */
						case OPT_SHOW_ONLY_UNREAD_ARTS:
							if (!bool_equal(tinrc.show_only_unread_arts, original_on_off_value) && group != (struct t_group *) 0) {
								make_threads (group, TRUE);
								pos_first_unread_thread();
							}
							break;

						/* draw -> / highlighted bar */
						case OPT_DRAW_ARROW:
							unhighlight_option (option);
							if (!tinrc.draw_arrow && !tinrc.inverse_okay) {
								tinrc.inverse_okay = TRUE;
								RepaintOption(OPT_INVERSE_OKAY);
							}
							break;

						/* draw inversed screen header lines */
						/* draw inversed group/article/option line if draw_arrow is OFF */
						case OPT_INVERSE_OKAY:
							unhighlight_option (option);
							if (!tinrc.draw_arrow && !tinrc.inverse_okay) {
								tinrc.draw_arrow = TRUE;	/* we don't want to navigate blindly */
								RepaintOption(OPT_DRAW_ARROW);
							}
							break;

						case OPT_MAIL_8BIT_HEADER:
							if (strcasecmp(txt_mime_encodings[tinrc.mail_mime_encoding], txt_8bit)) {
								tinrc.mail_8bit_header = FALSE;
								MoveCursor (option_row(OPT_MAIL_8BIT_HEADER), 3);
								print_option (OPT_MAIL_8BIT_HEADER);
							}
							break;

						case OPT_POST_8BIT_HEADER:
							if (strcasecmp(txt_mime_encodings[tinrc.post_mime_encoding], txt_8bit)) {
								tinrc.post_8bit_header = FALSE;
								MoveCursor (option_row(OPT_POST_8BIT_HEADER), 3);
								print_option (OPT_POST_8BIT_HEADER);
							}
							break;

						/* show newsgroup description text next to newsgroups */
						case OPT_SHOW_DESCRIPTION:
							show_description = tinrc.show_description;
							if (show_description) {			/* force reread of newgroups file */
								read_newsgroups_file ();
								clear_message ();
							} else
								set_groupname_len (FALSE);

							break;

#ifdef HAVE_COLOR
						/* use ANSI color */
						case OPT_USE_COLOR:
#	ifdef USE_CURSES
							if (!has_colors())
								use_color = FALSE;
							else
#	endif /* USE_CURSES */
								use_color = tinrc.use_color;
							break;
#endif /* HAVE_COLOR */

						/*
						 * the following do not need further action (if I'm right)
						 *
						 * case OPT_AUTO_BCC:
						 * case OPT_AUTO_CC:
						 * case OPT_AUTO_LIST_THREAD:
						 * case OPT_AUTO_RECONNECT:
						 * case OPT_CATCHUP_READ_GROUPS:
						 * case OPT_CONFIRM_ACTION:
						 * case OPT_CONFIRM_TO_QUIT:
						 * case OPT_AUTO_SAVE:
						 * case OPT_BATCH_SAVE:
						 * case OPT_FORCE_SCREEN_REDRAW:
						 * case OPT_FULL_PAGE_SCROLL:
						 * case OPT_GROUP_CATCHUP_ON_EXIT:
						 * case OPT_KEEP_POSTED_ARTICLES:
						 * case OPT_MARK_SAVED_READ:
						 * case OPT_NO_ADVERTISING:
						 * case OPT_POS_FIRST_UNREAD:
						 * case OPT_PRINT_HEADER:
						 * case OPT_PROCESS_ONLY_UNREAD:
						 * case OPT_SAVE_TO_MMDF_MAILBOX:
						 * case OPT_SHOW_LINES:
						 * case OPT_SHOW_SCORE:
						 * case OPT_SHOW_LAST_LINE_PREV_PAGE:
						 * case OPT_SHOW_ONLY_UNREAD_GROUPS:
						 * case OPT_SHOW_XCOMMENTTO:
						 * case OPT_SIGDASHES:
						 * case OPT_SPACE_GOTO_NEXT_UNREAD:
						 * case OPT_START_EDITOR_OFFSET:
						 * case OPT_STRIP_BLANKS:
						 * case OPT_TAB_AFTER_X_SELECTION:
						 * case OPT_TAB_GOTO_NEXT_UNREAD:
						 * case OPT_THREAD_CATCHUP_ON_EXIT:
						 * case OPT_UNLINK_ARTICLE:
#if defined(NNTP_ABLE) || defined(NNTP_ONLY)
						 * case OPT_USE_BUILTIN_INEWS:
#endif
						 * case OPT_USE_MAILREADER_I:
						 * case OPT_USE_MOUSE:
						 * case OPT_DISPLAY_MIME_HEADER_ASIS:
						 * case OPT_DISPLAY_MIME_ALLHEADER_ASIS:
#ifdef HAVE_KEYPAD
						 * case OPT_USE_KEYPAD:
#endif
#ifdef HAVE_METAMAIL
						 * case OPT_ASK_FOR_METAMAIL:
						 * case OPT_USE_METAMAIL:
#endif
						 * case OPT_KEEP_DEAD_ARTICLES:
#ifdef HAVE_COLOR
						 * case OPT_WORD_HIGHLIGHT_TINRC:
#endif
						 */

						default:
							break;
					} /* switch (option) */
					break;

				case OPT_LIST:
					original_list_value = *(option_table[option].variable);
					*(option_table[option].variable) = prompt_list (option_row(option),
								OPT_ARG_COLUMN,
								*(option_table[option].variable), /* post_process */
								option_table[option].txt->help,
								option_table[option].txt->opt,
								option_table[option].opt_list,
								option_table[option].opt_count
								);

					/*
					 * some options need further action to take effect
					 */
					switch (option) {

						case OPT_THREAD_ARTICLES:
							/*
							 * If the threading strategy has changed, fix things
							 * so that rethreading will occur
							 */
							if (tinrc.thread_articles != original_list_value && group != (struct t_group *) 0) {
								group->attribute->thread_arts = tinrc.thread_articles;
								make_threads (group, TRUE);
							}
							clear_message ();
							break;

						case OPT_POST_PROCESS:
							proc_ch_default = get_post_proc_type (tinrc.post_process);
							break;

						case OPT_SHOW_AUTHOR:
							if (group != (struct t_group *) 0)
								group->attribute->show_author = tinrc.show_author;
							break;

						case OPT_WILDCARD:
							wildcard_func = (tinrc.wildcard) ? match_regex : wildmat;
							break;

						/*
						 * the following don't need any further action
#ifdef HAVE_COLOR
						 *
						 * case OPT_COL_BACK:
						 * case OPT_COL_FROM:
						 * case OPT_COL_HEAD:
						 * case OPT_COL_HELP:
						 * case OPT_COL_INVERS:
						 * case OPT_COL_MESSAGE:
						 * case OPT_COL_MINIHELP:
						 * case OPT_COL_NORMAL:
						 * case OPT_COL_QUOTE:
						 * case OPT_COL_QUOTE2:
						 * case OPT_COL_QUOTE3:
						 * case OPT_COL_RESPONSE:
						 * case OPT_COL_SIGNATURE:
						 * case OPT_COL_SUBJECT:
						 * case OPT_COL_TEXT:
						 * case OPT_COL_TITLE:
						 * case OPT_COL_MARKSTAR:
						 * case OPT_COL_MARKDASH:
						 * case OPT_WORD_H_DISPLAY_MARKS:
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
						case OPT_EDITOR_FORMAT:
						case OPT_MAILER_FORMAT:
						case OPT_MM_CHARSET:
						case OPT_MAIL_QUOTE_FORMAT:
						case OPT_NEWS_QUOTE_FORMAT:
						case OPT_QUOTE_CHARS:
						case OPT_XPOST_QUOTE_FORMAT:
						case OPT_MAIL_ADDRESS:
						case OPT_SPAMTRAP_WARNING_ADDRESSES:
							prompt_option_string (option);
							break;

						case OPT_NEWS_HEADERS_TO_DISPLAY:
							prompt_option_string (option);
							free (*news_headers_to_display_array);
							free (news_headers_to_display_array);
							news_headers_to_display_array = ulBuildArgv(tinrc.news_headers_to_display, &num_headers_to_display);
							break;

						case OPT_NEWS_HEADERS_TO_NOT_DISPLAY:
							prompt_option_string (option);
							free (*news_headers_to_not_display_array);
							free (news_headers_to_not_display_array);
							news_headers_to_not_display_array = ulBuildArgv(tinrc.news_headers_to_not_display, &num_headers_to_not_display);
							break;

#ifndef DISABLE_PRINTING
						case OPT_PRINTER:
#endif /* !DISABLE_PRINTING */
						case OPT_MAILDIR:
						case OPT_SAVEDIR:
						case OPT_SIGFILE:
#ifdef M_AMIGA
							if (tin_bbs_mode)
								break;
#endif /* M_AMIGA */
							prompt_option_string (option);
							expand_rel_abs_pathname (option_row(option),
								OPT_ARG_COLUMN + (int) strlen (option_table[option].txt->opt),
								OPT_STRING_list[option_table[option].var_index]
								);
							joinpath (posted_msgs_file, tinrc.maildir, POSTED_FILE);
							break;

						case OPT_MAIL_MIME_ENCODING:
						case OPT_POST_MIME_ENCODING:
							mime_encoding = *(option_table[option].variable);
							mime_encoding = prompt_list (option_row(option),
										OPT_ARG_COLUMN,
										mime_encoding,
										option_table[option].txt->help,
										option_table[option].txt->opt,
										option_table[option].opt_list,
										option_table[option].opt_count
										);
							*(option_table[option].variable) = mime_encoding;

							/* do not use 8 bit headers if mime encoding is not 8bit; ask J. Shin why */
							if (strcasecmp(txt_mime_encodings[mime_encoding], txt_8bit)) {
								if (option == (int)OPT_POST_MIME_ENCODING) {
									tinrc.post_8bit_header = FALSE;
									RepaintOption(OPT_POST_8BIT_HEADER);
								} else {
									tinrc.mail_8bit_header = FALSE;
									RepaintOption(OPT_MAIL_8BIT_HEADER);
								}
							}
							break;

#ifdef HAVE_COLOR
						case OPT_QUOTE_REGEX:
							prompt_option_string (option);
							FreeIfNeeded(quote_regex.re);
							FreeIfNeeded(quote_regex.extra);
							if (!strlen(tinrc.quote_regex))
								STRCPY(tinrc.quote_regex, DEFAULT_QUOTE_REGEX);
							compile_regex (tinrc.quote_regex, &quote_regex, PCRE_CASELESS);
							break;

						case OPT_QUOTE_REGEX2:
							prompt_option_string (option);
							FreeIfNeeded(quote_regex2.re);
							FreeIfNeeded(quote_regex2.extra);
							if (!strlen(tinrc.quote_regex2))
								STRCPY(tinrc.quote_regex2, DEFAULT_QUOTE_REGEX2);
							compile_regex (tinrc.quote_regex2, &quote_regex2, PCRE_CASELESS);
							break;

						case OPT_QUOTE_REGEX3:
							prompt_option_string (option);
							FreeIfNeeded(quote_regex3.re);
							FreeIfNeeded(quote_regex3.extra);
							if (!strlen(tinrc.quote_regex3))
								STRCPY(tinrc.quote_regex3, DEFAULT_QUOTE_REGEX3);
							compile_regex (tinrc.quote_regex3, &quote_regex3, PCRE_CASELESS);
							break;
#endif /* HAVE_COLOR */
						case OPT_STRIP_RE_REGEX:
							prompt_option_string (option);
							FreeIfNeeded(strip_re_regex.re);
							FreeIfNeeded(strip_re_regex.extra);
							if (!strlen(tinrc.strip_re_regex))
								STRCPY(tinrc.strip_re_regex, DEFAULT_STRIP_RE_REGEX);
							compile_regex (tinrc.strip_re_regex, &strip_re_regex, PCRE_ANCHORED);
							break;

						case OPT_STRIP_WAS_REGEX:
							prompt_option_string (option);
							FreeIfNeeded(strip_was_regex.re);
							FreeIfNeeded(strip_was_regex.extra);
							if (!strlen(tinrc.strip_was_regex))
								STRCPY(tinrc.strip_was_regex, DEFAULT_STRIP_WAS_REGEX);
							compile_regex (tinrc.strip_was_regex, &strip_was_regex, 0);
							break;

						default:
							break;
					} /* switch (option) */
					break;

				case OPT_NUM:
					switch (option) {
						case OPT_REREAD_ACTIVE_FILE_SECS:
						case OPT_GETART_LIMIT:
						case OPT_GROUPNAME_MAX_LENGTH:
						case OPT_FILTER_DAYS:
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
			} /* switch (option_table[option].var_type) */
			change_option = FALSE;
			show_menu_help (txt_select_config_file_option);
		} /* if (change_option) */
	} /* forever */
	/* NOTREACHED */
	return ret_code;
}


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
		if (strlen (str) == 1)
			strcpy (str, homedir);
		else {
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


t_bool
match_boolean (
	char *line,
	const char *pat,
	t_bool *dst)
{
	size_t patlen = strlen (pat);

	if (STRNCASECMPEQ(line, pat, patlen)) {
		*dst = (t_bool) (STRNCASECMPEQ(&line[patlen], "ON", 2) ? TRUE : FALSE);
		return TRUE;
	}
	return FALSE;
}


#ifdef HAVE_COLOR
static t_bool
match_color (
	char *line,
	const char *pat,
	int *dst,
	int maxlen)
{
	int n;
	size_t patlen = strlen (pat);

	if (STRNCMPEQ(line, pat, patlen)) {
		t_bool found = FALSE;
		for (n = 0; n < MAX_COLOR+1; n++) {
			if (!strcasecmp(&line[patlen], txt_colors[n])) {
				found = TRUE;
				*dst = n;
				if (*dst > maxlen)
					*dst = -1;
			}
		}

		if (!found)
			*dst = atoi (&line[patlen]);

		if (maxlen) {
			if ((*dst < -1) || (*dst > maxlen)) {
				my_fprintf(stderr, txt_value_out_of_range, pat, *dst, maxlen);
				*dst = 0;
			}
		}
		return TRUE;
	}
	return FALSE;
}
#endif /* HAVE_COLOR */


/*
 * If pat matches the start of line, convert rest of line to an integer, dst
 * If maxlen is set, constrain value to 0 <= dst <= maxlen and return TRUE.
 * If no match is made, return FALSE.
 */
t_bool
match_integer (
	char *line,
	const char *pat,
	int *dst,
	int maxlen)
{
	size_t patlen = strlen (pat);

	if (STRNCMPEQ(line, pat, patlen)) {
		*dst = atoi (&line[patlen]);

		if (maxlen) {
			if ((*dst < 0) || (*dst > maxlen)) {
				my_fprintf(stderr, txt_value_out_of_range, pat, *dst, maxlen);
				*dst = 0;
			}
		}
		return TRUE;
	}
	return FALSE;
}


t_bool
match_long (
	char *line,
	const char *pat,
	long *dst)
{
	size_t patlen = strlen (pat);

	if (STRNCMPEQ(line, pat, patlen)) {
		*dst = atol (&line[patlen]);
		return TRUE;
	}
	return FALSE;
}


/* If the 'pat' keyword matches, lookup & return an index into the table */
t_bool
match_list (
	char *line,
	constext *pat,
	constext *const *table,
	size_t tablelen,
	int *dst)
{
	size_t patlen = strlen (pat);
	size_t n;
	char temp[LEN];

	if (STRNCMPEQ(line, pat, patlen)) {
		line += patlen;
		*dst = 0;	/* default, if no match */
		for (n = 0; n < tablelen; n++) {
			if (match_string (line, table[n], temp, sizeof(temp))) {
				*dst = (int)n;
				break;
			}
		}
		return TRUE;
	}
	return FALSE;
}


t_bool
match_string (
	char *line,
	const char *pat,
	char *dst,
	size_t dstlen)
{
	char *ptr;
	size_t patlen = strlen (pat);

	if (STRNCMPEQ(line, pat, patlen)) {
		strncpy (dst, &line[patlen], dstlen);
		ptr = strrchr (dst, '\n');
		if (ptr != (char *) 0)
			*ptr = '\0';

		return TRUE;
	}
	return FALSE;
}


const char *
print_boolean (
	t_bool value)
{
	return txt_onoff[value != FALSE ? 1 : 0];
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
		if (*ptr == '_')
			*ptr = ' ';
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
		if (*ptr == ' ')
			*dst = '_';
		else
			*dst = *ptr;
		dst++;
	}
	*dst = '\0';

	return buf;
}


/*
 * display current configuration page
 */
static void
show_config_page (
	void)
{
	int i, lines_to_print = option_lines_per_page;

	center_line (0, TRUE, txt_options_menu);

	/*
	 * on last page, there need not be option_lines_per_page options
	 */
	if (first_option_on_screen + option_lines_per_page > LAST_OPT)
		lines_to_print = LAST_OPT + 1 - first_option_on_screen;

	for (i = 0; i < lines_to_print;i++) {
		MoveCursor (INDEX_TOP + i, 3);
		print_any_option (first_option_on_screen + i);
	}
	CleartoEOS ();

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
char **
ulBuildArgv (
	char *cmd,
	int *new_argc)
{
	char **new_argv = NULL;
	char *buf, *tmp;
	int i = 0;

	if (!cmd && !*cmd) {
		*new_argc = 0;
		return (NULL);
	}

	for (tmp = cmd; isspace ((int)*tmp); tmp++)
		;

	buf = my_strdup(tmp);
	if (!buf) {
		*new_argc = 0;
		return (NULL);
	}

	new_argv = (char **) calloc (1, sizeof (char *));
	if (!new_argv) {
		free (buf);
		*new_argc = 0;
		return (NULL);
	}

	tmp = buf;
	new_argv[0] = NULL;

	while (*tmp) {
		if (!isspace((int)*tmp)) { /*found the begining of a word*/
			new_argv[i] = tmp;
			for (; *tmp && !isspace((int)*tmp); tmp++)
				;
			if (*tmp) {
				*tmp = '\0';
				tmp++;
			}
			i++;
			new_argv = (char **) realloc (new_argv, ((i+1) * sizeof (char *)));
			new_argv[i] = NULL;
		} else
			tmp++;
	}
	*new_argc = i;
	return (new_argv);
}
