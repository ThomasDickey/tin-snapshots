/*
 *  Project   : tin - a Usenet reader
 *  Module    : conf.h
 *  Author    : D.Nimmich
 *  Created   : 04-08-96
 *  Updated   :
 *  Notes     : #defines and structs for config.c
 *  Copyright : (c) Copyright 1996 by Dirk Nimmich
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

/*
 * Option numbers.
 * NOTE: Do not change the order of these unless you change the corresponding
 * order in option_table[].
 */

enum option_enum {
	OPT_VERY_FIRST = 0,
	OPT_DEFAULT_SAVEDIR,
	OPT_DEFAULT_AUTO_SAVE,
	OPT_MARK_SAVED_READ,
	OPT_DEFAULT_POST_PROC_TYPE,
	OPT_PROCESS_ONLY_UNREAD,
	OPT_CONFIRM_ACTION,
	OPT_CONFIRM_TO_QUIT,
	OPT_DRAW_ARROW_MARK,
	OPT_INVERSE_OKAY,
	OPT_POS_FIRST_UNREAD,
	OPT_DEFAULT_SHOW_ONLY_UNREAD,
	OPT_SHOW_ONLY_UNREAD_GROUPS,
	OPT_TAB_GOTO_NEXT_UNREAD,
	OPT_SPACE_GOTO_NEXT_UNREAD,
	OPT_TAB_AFTER_X_SELECTION,
	OPT_FULL_PAGE_SCROLL,
	OPT_SHOW_LAST_LINE_PREV_PAGE,
	OPT_CATCHUP_READ_GROUPS,
	OPT_GROUP_CATCHUP_ON_EXIT,
	OPT_THREAD_CATCHUP_ON_EXIT,
	OPT_DEFAULT_THREAD_ARTS,
	OPT_SHOW_DESCRIPTION,
	OPT_DEFAULT_SHOW_AUTHOR,
	OPT_DEFAULT_SORT_ART_TYPE,
	OPT_DEFAULT_MAILDIR,
	OPT_SAVE_TO_MMDF_MAILBOX,
	OPT_SHOW_XCOMMENTTO,
	OPT_HIGHLIGHT_XCOMMENTTO,
	OPT_PRINT_HEADER,
	OPT_DEFAULT_PRINTER,
	OPT_DEFAULT_BATCH_SAVE,
	OPT_START_EDITOR_OFFSET,
	OPT_DEFAULT_EDITOR_FORMAT,
	OPT_DEFAULT_MAILER_FORMAT,
	OPT_USE_MAILREADER_I,
	OPT_SHOW_LINES,
	OPT_UNLINK_ARTICLE,
	OPT_KEEP_POSTED_ARTICLES,
#ifdef M_UNIX
	OPT_KEEP_DEAD_ARTICLES,
#endif
	OPT_DEFAULT_SIGFILE,
	OPT_SIGDASHES,
	OPT_NO_ADVERTISING,
	OPT_REREAD_ACTIVE_FILE_SECS,
	OPT_QUOTE_CHARS,
	OPT_NEWS_QUOTE_FORMAT,
	OPT_XPOST_QUOTE_FORMAT,
	OPT_MAIL_QUOTE_FORMAT,
	OPT_AUTO_CC,
	OPT_AUTO_BCC,
	OPT_ART_MARKED_DELETED,
	OPT_ART_MARKED_INRANGE,
	OPT_ART_MARKED_RETURN,
	OPT_ART_MARKED_SELECTED,
	OPT_ART_MARKED_UNREAD,
	OPT_FORCE_SCREEN_REDRAW,
	OPT_USE_BUILTIN_INEWS,
	OPT_AUTO_LIST_THREAD,
	OPT_USE_MOUSE,
	OPT_STRIP_BLANKS,
	OPT_GROUPNAME_MAX_LENGTH,
	OPT_BEGINNER_LEVEL,
	OPT_DEFAULT_FILTER_DAYS,
#ifdef HAVE_COLOR
	OPT_USE_COLOR,
	OPT_COL_NORMAL,
	OPT_COL_BACK,
	OPT_COL_INVERS,
	OPT_COL_TEXT,
	OPT_COL_MINIHELP,
	OPT_COL_HELP,
	OPT_COL_MESSAGE,
	OPT_COL_QUOTE,
	OPT_COL_HEAD,
	OPT_COL_SUBJECT,
	OPT_COL_RESPONSE,
	OPT_COL_FROM,
	OPT_COL_TITLE,
	OPT_COL_SIGNATURE,
#endif
#ifdef FORGERY
	OPT_MAIL_ADDRESS,
#endif
	OPT_MM_CHARSET,
	OPT_POST_MIME_ENCODING,
	OPT_POST_8BIT_HEADER,
	OPT_MAIL_MIME_ENCODING,
	OPT_MAIL_8BIT_HEADER,
#ifdef HAVE_METAMAIL
	OPT_USE_METAMAIL,
	OPT_ASK_FOR_METAMAIL,
#endif
#ifdef HAVE_KEYPAD
	OPT_USE_KEYPAD,
#endif
	VERY_LAST_OPT };

#define LAST_OPT VERY_LAST_OPT - 1
#define OPT_ARG_COLUMN	9

#define OPT_ON_OFF	1
#define OPT_LIST	2
#define OPT_STRING	3
#define OPT_NUM		4
#define OPT_CHAR	5

/*
 * build access table
 */
struct t_option option_table[]=
{ {(void *) default_savedir, OPT_STRING, NULL, 0, txt_opt_savedir, txt_help_savedir },
  {(void *) &default_auto_save, OPT_ON_OFF, NULL, 0, txt_opt_autosave, txt_help_autosave },
  {(void *) &mark_saved_read, OPT_ON_OFF, NULL, 0, txt_opt_mark_saved_read, txt_help_mark_saved_read },
  {(void *) &default_post_proc_type, OPT_LIST, txt_post_process, POST_PROC_UUD_EXT_ZIP + 1, txt_opt_process_type, txt_help_post_proc_type },
  {(void *) &process_only_unread, OPT_ON_OFF, NULL, 0, txt_opt_process_unread, txt_help_process_unread },
  {(void *) &confirm_action, OPT_ON_OFF, NULL, 0, txt_opt_confirm_action, txt_help_confirm_action },
  {(void *) &confirm_to_quit, OPT_ON_OFF, NULL, 0, txt_opt_confirm_to_quit, txt_help_confirm_to_quit },
  {(void *) &draw_arrow_mark, OPT_ON_OFF, NULL, 0, txt_opt_draw_arrow, txt_help_draw_arrow },
  {(void *) &inverse_okay, OPT_ON_OFF, NULL, 0, txt_opt_inverse_okay, txt_help_inverse_okay },
  {(void *) &pos_first_unread, OPT_ON_OFF, NULL, 0, txt_opt_pos_first_unread, txt_help_pos_first_unread },
  {(void *) &default_show_only_unread, OPT_ON_OFF, NULL, 0, txt_opt_show_only_unread, txt_help_show_only_unread },
  {(void *) &show_only_unread_groups, OPT_ON_OFF, NULL, 0, txt_opt_show_only_unread_groups, txt_help_show_only_unread_groups },
  {(void *) &tab_goto_next_unread, OPT_ON_OFF, NULL, 0, txt_opt_tab_goto_next_unread, txt_help_tab_goto_next_unread },
  {(void *) &space_goto_next_unread, OPT_ON_OFF, NULL, 0, txt_opt_space_goto_next_unread, txt_help_space_goto_next_unread },
  {(void *) &tab_after_X_selection, OPT_ON_OFF, NULL, 0, txt_opt_tab_after_X_selection, txt_help_tab_after_X_selection },
  {(void *) &full_page_scroll, OPT_ON_OFF, NULL, 0, txt_opt_page_scroll, txt_help_page_scroll },
  {(void *) &show_last_line_prev_page, OPT_ON_OFF, NULL, 0, txt_opt_show_last_line_prev_page, txt_help_show_last_line_prev_page },
  {(void *) &catchup_read_groups, OPT_ON_OFF, NULL, 0, txt_opt_catchup_groups, txt_help_catchup_groups },
  {(void *) &group_catchup_on_exit, OPT_ON_OFF, NULL, 0, txt_opt_group_catchup_on_exit, txt_help_group_catchup_on_exit },
  {(void *) &thread_catchup_on_exit, OPT_ON_OFF, NULL, 0, txt_opt_thread_catchup_on_exit, txt_help_thread_catchup_on_exit },
  {(void *) &default_thread_arts, OPT_LIST, txt_thread, THREAD_MAX + 1, txt_opt_thread_arts, txt_help_thread_arts },
  {(void *) &show_description, OPT_ON_OFF, NULL, 0, txt_opt_show_description, txt_help_show_description },
  {(void *) &default_show_author, OPT_LIST, txt_show_from, SHOW_FROM_BOTH + 1, txt_opt_show_author, txt_help_show_author },
  {(void *) &default_sort_art_type, OPT_LIST, txt_sort_type, SORT_BY_DATE_ASCEND + 1, txt_opt_sort_type, txt_help_sort_type },
  {(void *) default_maildir, OPT_STRING, NULL, 0, txt_opt_maildir, txt_help_maildir },
  {(void *) &save_to_mmdf_mailbox, OPT_ON_OFF, NULL, 0, txt_opt_save_to_mmdf_mailbox, txt_help_save_to_mmdf_mailbox },
  {(void *) &show_xcommentto, OPT_ON_OFF, NULL, 0, txt_opt_show_xcommentto, txt_help_show_xcommentto },
  {(void *) &highlight_xcommentto, OPT_ON_OFF, NULL, 0, txt_opt_highlight_xcommentto, txt_help_highlight_xcommentto },
  {(void *) &print_header, OPT_ON_OFF, NULL, 0, txt_opt_print_header, txt_help_print_header },
  {(void *) default_printer, OPT_STRING, NULL, 0, txt_opt_printer, txt_help_printer },
  {(void *) &default_batch_save, OPT_ON_OFF, NULL, 0, txt_opt_batch_save, txt_help_batch_save },
  {(void *) &start_editor_offset, OPT_ON_OFF, NULL, 0, txt_opt_start_editor_offset, txt_help_start_editor_offset },
  {(void *) default_editor_format, OPT_STRING, NULL, 0, txt_opt_editor_format, txt_help_editor_format },
  {(void *) default_mailer_format, OPT_STRING, NULL, 0, txt_opt_mailer_format, txt_help_mailer_format },
  {(void *) &use_mailreader_i, OPT_ON_OFF, NULL, 0, txt_opt_use_mailreader, txt_help_use_mailreader },
  {(void *) &show_lines, OPT_ON_OFF, NULL, 0, txt_opt_show_lines, txt_help_show_lines },
  {(void *) &unlink_article, OPT_ON_OFF, NULL, 0, txt_opt_unlink_article, txt_help_unlink_article },
  {(void *) &keep_posted_articles, OPT_ON_OFF, NULL, 0, txt_opt_keep_posted_articles, txt_help_keep_posted_articles },
#ifdef M_UNIX
  {(void *) &keep_dead_articles, OPT_ON_OFF, NULL, 0, txt_opt_keep_dead_articles, txt_help_keep_dead_articles },
#endif
  {(void *) default_sigfile, OPT_STRING, NULL, 0, txt_opt_sigfile, txt_help_sigfile },
  {(void *) &sigdashes, OPT_ON_OFF, NULL, 0, txt_opt_sigdashes, txt_help_sigdashes },
  {(void *) &no_advertising, OPT_ON_OFF, NULL, 0, txt_opt_no_advertising, txt_help_no_advertising },
  {(void *) &reread_active_file_secs, OPT_NUM, NULL, 0, txt_opt_reread_active_file_secs, txt_help_reread_active_file_secs },
  {(void *) quote_chars, OPT_STRING, NULL, 0, txt_opt_quote_chars, txt_help_quote_chars },
  {(void *) news_quote_format, OPT_STRING, NULL, 0, txt_opt_news_quote_format, txt_help_news_quote_format },
  {(void *) xpost_quote_format, OPT_STRING, NULL, 0, txt_opt_xpost_quote_format, txt_help_xpost_quote_format },
  {(void *) mail_quote_format, OPT_STRING, NULL, 0, txt_opt_mail_quote_format, txt_help_mail_quote_format },
  {(void *) &auto_cc, OPT_ON_OFF, NULL, 0, txt_opt_auto_cc, txt_help_auto_cc },
  {(void *) &auto_bcc, OPT_ON_OFF, NULL, 0, txt_opt_auto_bcc, txt_help_auto_bcc },
  {(void *) &art_marked_deleted, OPT_CHAR, NULL, 0, txt_opt_art_marked_deleted, txt_help_art_marked_deleted },
  {(void *) &art_marked_inrange, OPT_CHAR, NULL, 0, txt_opt_art_marked_inrange, txt_help_art_marked_inrange },
  {(void *) &art_marked_return, OPT_CHAR, NULL, 0, txt_opt_art_marked_return, txt_help_art_marked_return },
  {(void *) &art_marked_selected, OPT_CHAR, NULL, 0, txt_opt_art_marked_selected, txt_help_art_marked_selected },
  {(void *) &art_marked_unread, OPT_CHAR, NULL, 0, txt_opt_art_marked_unread, txt_help_art_marked_unread },
  {(void *) &force_screen_redraw, OPT_ON_OFF, NULL, 0, txt_opt_force_screen_redraw, txt_help_force_screen_redraw },
  {(void *) &use_builtin_inews, OPT_ON_OFF, NULL, 0, txt_opt_use_builtin_inews, txt_help_use_builtin_inews },
  {(void *) &auto_list_thread, OPT_ON_OFF, NULL, 0, txt_opt_auto_list_thread, txt_help_auto_list_thread },
  {(void *) &use_mouse, OPT_ON_OFF, NULL, 0, txt_opt_use_mouse, txt_help_use_mouse },
  {(void *) &strip_blanks, OPT_ON_OFF, NULL, 0, txt_opt_strip_blanks, txt_help_strip_blanks },
  {(void *) &groupname_max_length, OPT_NUM, NULL, 0, txt_opt_groupname_length, txt_help_groupname_length },
  {(void *) &beginner_level, OPT_ON_OFF, NULL, 0, txt_opt_beginner_level, txt_help_beginner_level },
  {(void *) &default_filter_days, OPT_NUM, NULL, 0, txt_opt_filter_days, txt_help_filter_days }
#ifdef HAVE_COLOR
 ,{(void *) &use_color_tinrc, OPT_ON_OFF, NULL, 0, txt_opt_use_color, txt_help_use_color },
  {(void *) &col_normal, OPT_LIST, txt_colors, MAX_COLOR + 1, txt_opt_col_normal, txt_help_col_normal },
  {(void *) &col_back, OPT_LIST, txt_colors, MAX_BACKCOLOR + 1, txt_opt_col_back, txt_help_col_back },
  {(void *) &col_invers, OPT_LIST, txt_colors, MAX_COLOR + 1, txt_opt_col_invers, txt_help_col_invers },
  {(void *) &col_text, OPT_LIST, txt_colors, MAX_COLOR + 1, txt_opt_col_text, txt_help_col_text },
  {(void *) &col_minihelp, OPT_LIST, txt_colors, MAX_COLOR + 1, txt_opt_col_minihelp, txt_help_col_minihelp },
  {(void *) &col_help, OPT_LIST, txt_colors, MAX_COLOR + 1, txt_opt_col_help, txt_help_col_help },
  {(void *) &col_message, OPT_LIST, txt_colors, MAX_COLOR + 1, txt_opt_col_message, txt_help_col_message },
  {(void *) &col_quote, OPT_LIST, txt_colors, MAX_COLOR + 1, txt_opt_col_quote, txt_help_col_quote },
  {(void *) &col_head, OPT_LIST, txt_colors, MAX_COLOR + 1, txt_opt_col_head, txt_help_col_head },
  {(void *) &col_subject, OPT_LIST, txt_colors, MAX_COLOR + 1, txt_opt_col_subject, txt_help_col_subject },
  {(void *) &col_response, OPT_LIST, txt_colors, MAX_COLOR + 1, txt_opt_col_response, txt_help_col_response },
  {(void *) &col_from, OPT_LIST, txt_colors, MAX_COLOR + 1, txt_opt_col_from, txt_help_col_from },
  {(void *) &col_title, OPT_LIST, txt_colors, MAX_COLOR + 1, txt_opt_col_title, txt_help_col_title },
  {(void *) &col_signature, OPT_LIST, txt_colors, MAX_COLOR + 1, txt_opt_col_signature, txt_help_col_signature }
#endif
#ifdef FORGERY
 ,{(void *) mail_address, OPT_STRING, NULL, 0, txt_opt_mail_address, txt_help_mail_address }
#endif
 ,{(void *) mm_charset, OPT_STRING, NULL, 0, txt_opt_mm_charset, txt_help_mm_charset },
  {(void *) post_mime_encoding, OPT_STRING, txt_mime_types, 4, txt_opt_post_mime_encoding, txt_help_post_mime_encoding },
  {(void *) &post_8bit_header, OPT_ON_OFF, NULL, 0, txt_opt_post_8bit_header, txt_help_post_8bit_header },
  {(void *) mail_mime_encoding, OPT_STRING, txt_mime_types, 4, txt_opt_mail_mime_encoding, txt_help_mail_mime_encoding },
  {(void *) &mail_8bit_header, OPT_ON_OFF, NULL, 0, txt_opt_mail_8bit_header, txt_help_mail_8bit_header }
#ifdef HAVE_METAMAIL
 ,{(void *) &use_metamail, OPT_ON_OFF, NULL, 0, txt_opt_use_metamail, txt_help_use_metamail },
  {(void *) &ask_for_metamail, OPT_ON_OFF, NULL, 0, txt_opt_ask_for_metamail, txt_help_ask_for_metamail }
#endif
#ifdef HAVE_KEYPAD
 ,{(void *) &use_keypad, OPT_ON_OFF, NULL, 0, txt_opt_use_keypad, txt_help_use_keypad }
#endif
};
