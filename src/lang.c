/*
 *  Project   : tin - a Usenet reader
 *  Module    : lang.c
 *  Author    : I. Lea
 *  Created   : 1991-04-01
 *  Updated   : 1998-10-17
 *  Notes     :
 *  Copyright : (c) Copyright 1991-99 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"
#include "tcurses.h"	/* to define cCRLF */

constext txt_1_resp[] = "1 Response" cCRLF;
constext txt_7bit[] = "7bit";
constext txt_8bit[] = "8bit";
constext txt_active_file_is_empty[] = "\n%s contains no newsgroups. Exiting.";
constext txt_added_groups[] = "Added %d group%s";
constext txt_append_overwrite_quit[] = "File %s exists. a)ppend, o)verwrite, q)uit: ";
constext txt_art_cancel[] = "Article cancelled (deleted).";
constext txt_art_cannot_cancel[] = "Article cannot be cancelled (deleted).";
constext txt_art_newsgroups[] = "\nYour article:\n  \"%s\"\nwill be posted to the following newsgroup%s:\n";
constext txt_art_not_posted[] = "Article not posted!";
constext txt_art_not_saved[] = "Article not saved";
constext txt_art_pager_com[] = "Article Level Commands (page %d of %d)";
constext txt_art_posted[] = "Article posted";
constext txt_art_rejected[] = "Article rejected (saved to %s)";
constext txt_art_saved_to[] = "Article saved to %s";
constext txt_art_thread_regex_tag[] = " a)rticle, t)hread, h)ot, p)attern, T)agged articles, q)uit: ";
constext txt_art_unavailable[] = "Article unavailable";
constext txt_art_parent_none[] = "Article has no parent";
constext txt_art_parent_killed[] = "Parent article has been killed";
constext txt_art_parent_unavail[] = "Parent article is unavailable";
constext txt_article_cancelled[] = "Article cancelled by author\n";
constext txt_article_cannot_open[] = "get_article: can't open %s: ";
constext txt_article_cannot_reopen[] = "get_article: can't reopen %s: ";
constext txt_article_reposted[] = "This is a repost of the following article:";
constext txt_auth_failed[] = "%d Authentication failed";
constext txt_auth_needed[] = "Server expects authentication.\n";
constext txt_auth_pass[] = "    Please enter password: ";
constext txt_auth_user[] = "    Please enter username: ";
constext txt_author_search_backwards[] = "Author search backwards [%s]> ";
constext txt_author_search_forwards[] = "Author search forwards [%s]> ";
constext txt_authorization_ok[] = "Authorized for user: %s\n";
constext txt_authorization_fail[] = "Authorization failed for user: %s\n";
constext txt_autosubscribed[] = "\nAutosubscribed to %s";
constext txt_autosubscribing_groups[] = "Autosubscribing groups...\n";
constext txt_autoselecting_articles[] = "Autoselecting articles (use 'X' to see all unread) ...";
constext txt_bad_active_file[] = "Active file corrupt - %s";
constext txt_bad_article[] = "Article to be posted resulted in errors/warnings. q)uit, M)enu, e)dit: ";
constext txt_bad_command[] = "Bad command. Type 'h' for help.";
constext txt_base64[] = "base64";
constext txt_batch_update_unavail[] = "%s: Updating of index files not supported";
constext txt_batch_update_failed[] = "Failed to start background indexing process";
constext txt_begin_of_art[] = "*** Beginning of article ***";
constext txt_caching_disabled[] = "Overview caching not supported; Tin is setuid";
constext txt_cannot_create_uniq_name[] = "Can't create unique tempfile-name";
constext txt_cannot_find_base_art[] = "Can't find base article %d";
constext txt_cannot_get_nntp_server_name[] = "Cannot find NNTP server name";
constext txt_cannot_get_term[] = "%s: Can't get TERM variable\n";
constext txt_cannot_get_term_entry[] = "%s: Can't get entry for TERM\n";
constext txt_cannot_open[] = "Can't open %s";
#if defined(NNTP_ABLE) || defined(NNTP_ONLY)
	constext txt_cannot_open_active_file[] = "Can't open %s. Try %s -r to read news via NNTP.\n";
#endif /* NNTP_ABLE || NNTP_ONLY */
constext txt_cannot_post[] = "*** Posting not allowed ***";
constext txt_cannot_post_group[] = "Posting is not allowed to %s";
constext txt_cannot_write_index[] = "Can't write index %s";
constext txt_cannot_write_to_directory[] = "%s is a directory";
constext txt_catchup_all_read_groups[] = "Catchup all groups entered during this session? (y/n): ";
constext txt_catchup_update_info[] = "%s %d group%s in %lu seconds\n";
constext txt_check_article[] = "Check Prepared Article";
constext txt_checking_new_groups[] = "Checking for new groups...";
constext txt_checking_for_news[] = "Checking for news...";
constext txt_checksum_of_file[] = cCRLF "Checksum of %s" cCRLF;
constext txt_return_key[] = "Press <RETURN> to continue...";
constext txt_color_off[] = "ANSI color disabled";
constext txt_color_on[] = "ANSI color enabled";
constext txt_command_failed_s[] = "Command failed: %s\n";
constext txt_connecting[] = "Connecting to %s...";
constext txt_connecting_port[] = "Connecting to %s:%d...";
constext txt_continue[] = "Continue? (y/n): ";

#ifdef M_AMIGA
	constext txt_copyright_notice[] = "%s (c) Copyright 1991-99 Iain Lea & Mark Tomlinson.";
#endif /* M_AMIGA */

#ifdef M_OS2
	constext txt_copyright_notice[] = "%s (c) Copyright 1991-99 Iain Lea & Andreas Wrede.";
#endif /* M_OS2 */

#ifdef M_UNIX
	constext txt_copyright_notice[] = "%s (c) Copyright 1991-99 Iain Lea.";
#endif /* M_UNIX */

#ifdef VMS
	constext txt_copyright_notice[] = "%s (c) Copyright 1991-99 Iain Lea & Tod McQuillin & other.";
#endif /* VMS */

#ifdef WIN32
	constext txt_copyright_notice[] = "%s (c) Copyright 1991-99 Iain Lea & Nigel Ellis.";
#endif /* WIN32 */

constext txt_cancel_article[] = "Cancel (delete) or supersede (overwrite) article [%.*s]? (d/s/q): ";
constext txt_cancelling_art[] = "Cancelling article...";
constext txt_creating_active[] = "Creating active file for saved groups...\n";
constext txt_creating_newsrc[] = "Creating newsrc file...\n";
constext txt_delete_processed_files[] = "Delete saved files that have been post processed? (y/n): ";
constext txt_deleting[] = "Deleting...";
constext txt_end_of_arts[] = "*** End of articles ***";
constext txt_end_of_groups[] = "*** End of groups ***";
constext txt_end_of_thread[] = "*** End of thread ***";
constext txt_enter_message_id[] = "Enter Message-ID to goto> ";
constext txt_enter_next_thread[] = " and enter next unread thread";
constext txt_enter_option_num[] = "Enter option number> ";
constext txt_enter_range[] = "Enter range [%s]> ";
constext txt_error_approved[] = "\nWarning: Approved: header used.\n";
constext txt_error_asfail[] = "%s: assertion failure: %s (%d): %s\n";
constext txt_error_bad_approved[] = "\nError: Bad address in Approved: header.\n";
constext txt_error_bad_from[] = "\nError: Bad address in From: header.\n";
constext txt_error_bad_replyto[] = "\nError: Bad address in Reply-To: header.\n";
constext txt_error_bad_msgidfqdn[] = "\nError: Bad FQDN in Message-ID: header.\n";
constext txt_error_gnksa_internal[] = "Internal error in GNKSA routine - send bug report.\n";
constext txt_error_gnksa_langle[] = "Left angle bracket missing in route address.\n";
constext txt_error_gnksa_lparen[] = "Left parenthese missing in old-style address.\n";
constext txt_error_gnksa_rparen[] = "Right parenthese missing in old-style address.\n";
constext txt_error_gnksa_atsign[] = "At-sign missing in mail address.\n";
constext txt_error_gnksa_sgl_domain[] = "Single component FQDN is not allowed. Add your domain.\n";
constext txt_error_gnksa_inv_domain[] = "Invalid domain. Send bug report if your top level domain really exists.\nUse .invalid as top level domain for munged addresses.\n";
constext txt_error_gnksa_ill_domain[] = "Illegal domain. Send bug report if your top level domain really exists.\nUse .invalid as top level domain for munged addresses.\n";
constext txt_error_gnksa_unk_domain[] = "Unknown domain. Send bug report if your top level domain really exists.\nUse .invalid as top level domain for munged addresses.\n";
constext txt_error_gnksa_fqdn[] = "Illegal character in FQDN.\n";
constext txt_error_gnksa_zero[] = "Zero length FQDN component not allowed.\n";
constext txt_error_gnksa_length[] = "FQDN component exceeds maximum allowed length (63 chars).\n";
constext txt_error_gnksa_hyphen[] = "FQDN component may not start or end with hyphen.\n";
constext txt_error_gnksa_begnum[] = "FQDN component may not start with digit.\n";
constext txt_error_gnksa_bad_lit[] = "Domain literal has impossible numeric value.\n";
constext txt_error_gnksa_local_lit[] = "Domain literal is for private use only and not allowed for global use.\n";
constext txt_error_gnksa_rbracket[] = "Right bracket missing in domain literal.\n";
constext txt_error_gnksa_lp_missing[] = "Missing localpart of mail address.\n";
constext txt_error_gnksa_lp_invalid[] = "Illegal character in localpart of mail address.\n";
constext txt_error_gnksa_lp_zero[] = "Zero length localpart component not allowed.\n";
constext txt_error_gnksa_rn_unq[] = "Illegal character in realname.\nUnquoted words may not contain '!()<>@,;:\\.[]' in route addresses.\n";
constext txt_error_gnksa_rn_qtd[] = "Illegal character in realname.\nQuoted words may not contain '()<>\\'.\n";
constext txt_error_gnksa_rn_enc[] = "Illegal character in realname.\nEncoded words may not contain '!()<>@,;:\"\\.[]/=' in parameter.\n";
constext txt_error_gnksa_rn_encsyn[] = "Bad syntax in encoded word used in realname.\n";
constext txt_error_gnksa_rn_paren[] = "Illegal character in realname.\nUnquoted words may not contain '()<>\\' in oldstyle addresses.\n";
constext txt_error_gnksa_rn_invalid[] = "Illegal character in realname.\nControl characters and unencoded 8bit characters > 127 are not allowed.\n";
constext txt_error_header_and_body_not_separate[] = "\nError: No blank line found after header.\n";
constext txt_error_header_line_bad_charset[] = "\n\
Error: Posting contains non-ASCII characters but MM_CHARSET is set to\n\
       US-ASCII  - please change this setting to a suitable value for\n\
       your language  using the  M)enu of configurable  options or by\n\
       editing tinrc.\n";
constext txt_error_header_line_bad_encoding[] = "\n\
Error: Posting contains  non-ASCII characters  but the  MIME encoding\n\
       for news  messages  is set  to \"7bit\"  -  please change this\n\
       setting to \"8bit\" or \"quoted-printable\" depending  on what\n\
       is more common  in your part  of the world.  This can  be done\n\
       using the M)enu of configurable options or by editing tinrc.\n";
constext txt_error_header_line_blank[] = "\nError: Article starts with blank line instead of header\n";
constext txt_error_header_line_colon[] = "\nError: Header on line %d does not have a colon after the header name:\n%s\n";
constext txt_error_header_line_comma[] = "\n\
Error: the \"%s:\" line has spaces  in it that MUST be removed.\n\
       The only allowable  space is the one  separating the colon (:)\n\
       from  the  contents.  Use a  comma  (,)  to separate  multiple\n\
       newsgroup names.\n";
constext txt_error_header_line_empty_subject[] = "\nError: the \"Subject:\" line is empty.\n";
constext txt_error_header_line_empty_newsgroups[] = "\nError: the \"Newsgroups:\" line lists no newsgroups.\n";
constext txt_error_header_line_groups_contd[] ="\n\
Error: The \"%s:\" line is  continued in  the next line.  Since\n\
       the line  may not  contain  whitespace,  this is  not allowed.\n\
       Please write all newsgroups into a single line.\n";
constext txt_error_header_line_missing_newsgroups[] = "\nError: the \"Newsgroups:\" line is missing from the article header.\n";
constext txt_error_header_line_missing_subject[] = "\nError: the \"Subject:\" line is missing from the article header.\n";
constext txt_error_header_line_missing_target[] = "\nError: the \"To:\" line is missing from the article header.\n";
constext txt_error_header_line_space[] = "\nError: Header on line %d does not have a space after the colon:\n%s\n";
constext txt_error_no_domain_name[] ="Can't get a (fully-qualified) domain-name!\n";
constext txt_error_no_enter_permission[] = "No permissions to go into %s\n";
constext txt_error_no_from[] = "\nError: From: line missing.\n";
constext txt_error_no_read_permission[] = "No read permissions for %s\n";
constext txt_error_no_such_file[] = "File %s does not exist\n";
constext txt_error_no_write_permission[] = "No write permissions for %s\n";
constext txt_error_sender_in_header_not_allowed[] = "\nError on line %d: \"Sender:\" header not allowed (it will be added for you)\n";
#if defined(HAVE_METAMAIL) && !defined(INDEX_DAEMON)
	constext txt_error_metamail_failed[] = "metamail failed: %s";
#endif /* defined(HAVE_METAMAIL) && !defined(INDEX_DAEMON) */
constext txt_external_mail_done[] = "leaving external mail-reader";
constext txt_extracting_archive[] = cCRLF cCRLF "Extracting %s archive..." cCRLF;
constext txt_extracting_shar[] =cCRLF "Extracting %s..." cCRLF;
constext txt_failed_to_connect_to_server[] = "Failed to connect to NNTP server %s. Exiting...";
constext txt_filesystem_full[] = "Error writing %s file. Filesystem full? File reset to previous state.";
constext txt_filesystem_full_backup[] = "Error making backup of %s file. Filesystem full?";
constext txt_filter_global_rules[] = "Filtering global rules (%d/%d)...";
constext txt_warn_update[] = "\n\nYou are upgrading to tin %s from an earlier version.\n\
Some values in your configuration file have changed\nRead WHATSNEW, etc...\n";
constext txt_feed_pattern[] = "Enter wildcard pattern [%s]> ";
constext txt_subscribe_pattern[] = "Enter wildcard subscribe pattern> ";
constext txt_superseding_art[] = "Superseding article...";
constext txt_unsubscribe_pattern[] = "Enter wildcard unsubscribe pattern> ";

#if defined(HAVE_POLL) || defined(HAVE_SELECT)
	constext txt_group[] = "Group %.*s ('q' to quit)... ";
#else
	constext txt_group[] = "Group %*s... ";
#endif /* HAVE_POLL || HAVE_SELECT */

constext txt_group_is_moderated[] = "Group %s is moderated. Continue? (y/n): ";
constext txt_group_select_com[] = "Top Level Commands (page %d of %d)";
constext txt_group_selection[] = "Group Selection";

#ifndef NO_SHELL_ESCAPE
	constext txt_help_shell[] = "!\t  shell escape" cCRLF;
#endif /* !NO_SHELL_ESCAPE */

#ifdef CASE_PROBLEM
	constext txt_help_BIGB[] = "B\t  search all articles for a given string (this may take some time)" cCRLF "\t  (searches are case-insensitive and wrap around to all articles)" cCRLF;
	constext txt_help_BIGD[] = "D\t  cancel (delete) current article; must have been posted by you" cCRLF;
	constext txt_help_BIGI[] = "I\t  toggle inverse video" cCRLF;
	constext txt_help_BIGK[] = "K\t  mark current thread as read" cCRLF;
	constext txt_help_BIGL[] = "L\t  goto article chosen by Message-ID" cCRLF;
	constext txt_help_BIGM[] = "M\t  menu of configurable options" cCRLF;
	constext txt_help_BIGS[] = "S U\t  subscribe (U = unsubscribe) to groups that match pattern" cCRLF;
	constext txt_help_BIGT[] = "T\t  return to group selection level" cCRLF;
	constext txt_help_BIGU[] = "U\t  untag all tagged threads" cCRLF;
	constext txt_help_BIGW[] = "W\t  list articles posted by you (from posted file)" cCRLF;
	constext txt_help_BIGX[] = "X\t  toggle display of all/selected articles" cCRLF;
#else
	constext txt_help_B[] = "B\t  search all articles for a given string (this may take some time)" cCRLF "\t  (searches are case-insensitive and wrap around to all articles)" cCRLF;
	constext txt_help_D[] = "D\t  cancel (delete) current article; must have been posted by you" cCRLF;
	constext txt_help_I[] = "I\t  toggle inverse video" cCRLF;
	constext txt_help_K[] = "K\t  mark current thread as read" cCRLF;
	constext txt_help_L[] = "L\t  goto article chosen by Message-ID" cCRLF;
	constext txt_help_M[] = "M\t  menu of configurable options" cCRLF;
	constext txt_help_S[] = "S U\t  subscribe (U = unsubscribe) to groups that match pattern" cCRLF;
	constext txt_help_T[] = "T\t  return to group selection level" cCRLF;
	constext txt_help_U[] = "U\t  untag all tagged threads" cCRLF;
	constext txt_help_W[] = "W\t  list articles posted by you (from posted file)" cCRLF;
	constext txt_help_X[] = "X\t  toggle display of all/selected articles" cCRLF;
#endif /* CASE_PROBLEM */

constext txt_help_a[] = "a A\t  search for articles by author (a = forward, A = backward)" cCRLF;
constext txt_help_add_posted_to_filter[] = "Add subject of posted articles to filter. <SPACE> toggles & <CR> sets.";
constext txt_help_alternative_handling[] = "Do you want to enable automatic handling of multipart/alternative articles?";
constext txt_help_art_marked_deleted[] = "Enter character to indicate deleted articles. <CR> sets, <ESC> cancels.";
constext txt_help_art_marked_inrange[] = "Enter character to indicate articles in range. <CR> sets, <ESC> cancels.";
constext txt_help_art_marked_return[] = "Enter character to indicate that article will return. <CR> sets, <ESC> cancels.";
constext txt_help_art_marked_selected[] = "Enter character to indicate selected articles. <CR> sets, <ESC> cancels.";
constext txt_help_art_marked_unread[] = "Enter character to indicate unread articles. <CR> sets, <ESC> cancels.";

#ifdef HAVE_METAMAIL
	constext txt_help_ask_for_metamail[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
#endif /* HAVE_METAMAIL */

constext txt_help_auto_bcc[] = "Send you a blind carbon copy automatically. <SPACE> toggles & <CR> sets.";
constext txt_help_auto_cc[] = "Send you a carbon copy automatically. <SPACE> toggles & <CR> sets.";
constext txt_help_auto_list_thread[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_auto_reconnect[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_auto_save[] = "Auto save article/thread by Archive-name: header. <SPACE> toggles & <CR> sets.";
constext txt_help_b[] = "<SPACE> b down (b = up) one page" cCRLF;
constext txt_help_batch_save[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_beginner_level[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_bug[] = "R\t  report bug or comment via mail to %s" cCRLF;
constext txt_help_cC[] = "C\t  mark all articles as read and enter next group with unread articles" cCRLF;
constext txt_help_c[] = "c\t  mark all articles as read and return to group selection menu" cCRLF;
constext txt_help_cache_overview_files[] = "Create local copies of NNTP overview files. <SPACE> toggles & <CR> sets.";
constext txt_help_catchup_read_groups[] = "Ask to mark groups read when quitting. <SPACE> toggles & <CR> sets.";
constext txt_help_getart_limit[] = "Enter maximum number of article to get. <CR> sets.";
constext txt_help_sel_c[] = "c C\t  mark all articles in chosen group read (C = and enter next unread group)" cCRLF;
constext txt_help_thd_c[] = "c\t  mark thread as read and return to group index page" cCRLF;
constext txt_help_thd_C[] = "C\t  mark thread as read and enter next unread thread or group" cCRLF;
constext txt_help_thd_K[] = "K\t  mark article as read and move cursor to next unread article" cCRLF;
constext txt_help_use_getart_limit[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";

#ifdef HAVE_COLOR
	constext txt_help_color[] = "&\t  toggle color" cCRLF;
	constext txt_help_col_back[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_from[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_head[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_help[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_invers_fg[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_invers_bg[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_message[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_minihelp[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_newsheaders[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_normal[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_quote[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_response[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_signature[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_subject[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_text[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_title[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_markstar[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_col_markdash[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_word_highlight[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
	constext txt_help_word_h_display_marks[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
#endif /* HAVE_COLOR */

constext txt_help_colon[] = ":\t  skip next block of included text" cCRLF;
constext txt_help_confirm_action[] = "Ask for command confirmation. <SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_confirm_to_quit[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_ctrl_a[] = "^A\t  auto-select (hot) an article via a menu" cCRLF;
constext txt_help_ctrl_d[] = "^D ^U\t  down (^U = up) one page" cCRLF;
constext txt_help_ctrl_f[] = "^F ^B\t  down (^B = up) one page" cCRLF;

#ifdef HAVE_PGP
	constext txt_help_ctrl_g[] = "^G\t  perform PGP operations on article" cCRLF;
#endif /* HAVE_PGP */

constext txt_help_ctrl_h[] = "^H\t  show article headers" cCRLF;
constext txt_help_ctrl_k[] = "^K\t  kill an article via a menu" cCRLF;
constext txt_help_ctrl_l[] = "^L\t  redraw page" cCRLF;
constext txt_help_d[] = "d\t  cycle the display of authors email address, real name, both or neither" cCRLF;
constext txt_help_dash[] = "-\t  display last article viewed" cCRLF;
constext txt_help_display_mime_header_asis[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_display_mime_allheader_asis[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_draw_arrow_mark[] = "Draw -> or highlighted bar for selection. <SPACE> toggles & <CR> sets.";
constext txt_help_e[] =  "e\t  edit article (mail-groups only)" cCRLF;
constext txt_help_editor_format[] = "Enter %E for editor, %F for filename, %N for line-number, <CR> to set.";
constext txt_help_equal[] = "=\t  select threads that match user specified pattern" cCRLF;
constext txt_help_esc[] = "<ESC>\t  escape from command prompt" cCRLF;
constext txt_help_filter_days[] = "Enter default number of days a filter entry will be valid. <CR> sets.";
constext txt_help_force_screen_redraw[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_g[] = "g\t  choose group by name" cCRLF;
constext txt_help_g_caret_dollar[] = "^ $\t  choose first ($ = last) group in list" cCRLF;
constext txt_help_g_cr[] = "<CR>\t  read chosen group" cCRLF;
constext txt_help_g_G[] = "G\t  toggle limit number of articles to get, and reload" cCRLF;
constext txt_help_g_ctrl_r[] = "^R\t  reset .newsrc (all available articles in groups marked unread)" cCRLF;
constext txt_help_g_d[] = "d\t  toggle display of group name only or group name plus description" cCRLF;
constext txt_help_g_hash[] = "#\t  choose range of groups to be affected by next command" cCRLF;
constext txt_help_g_num[] = "0 - 9\t  choose group by number" cCRLF;
constext txt_help_g_q[] = "q Q\t  quit" cCRLF;
constext txt_help_g_r[] = "r\t  toggle display to show all/unread subscribed groups" cCRLF;
constext txt_help_g_search[] = "/ ?\t  choose group by group name string forward (? = backward) search" cCRLF "\t  (all searches are case-insensitive and wrap around group list)" cCRLF;
constext txt_help_g_tab[] = "n <TAB>\t  enter next group with unread news" cCRLF;
constext txt_help_g_x[] = "X\t  quit without saving (changed) configuration" cCRLF;
constext txt_help_g_y[] = "y\t  toggle display to show all/subscribed groups" cCRLF;
constext txt_help_g_z[] = "z\t  mark all articles in chosen group unread" cCRLF;
constext txt_help_group_catchup_on_exit[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_groupname_max_length[] = "Enter maximum length of newsgroup names displayed. <CR> sets.";
constext txt_help_h[] = "h H\t  get help (H = toggle mini help menu display)" cCRLF;
constext txt_help_hash[] = "#\t  choose range of articles to be affected by next command" cCRLF;
constext txt_help_i[] = "q\t  return to group index page" cCRLF;
constext txt_help_i_caret_dollar[] = "^ $\t  choose first ($ = last) thread in list" cCRLF;
constext txt_help_i_coma[] = "@\t  toggle all selections (all articles)" cCRLF;
constext txt_help_i_cr[] = "<CR>\t  read chosen article" cCRLF;
constext txt_help_i_dot[] = ".\t  toggle selection of thread" cCRLF;
constext txt_help_i_n[] = "n p\t  display next (p = previous) group" cCRLF;
constext txt_help_i_num[] = "0 - 9\t  choose thread by number" cCRLF;
constext txt_help_i_p[] = "N P\t  display next (P = previous) unread article" cCRLF;
constext txt_help_i_search[] = "/ ?\t  search for articles by Subject line (/ = forward, ? = backward)" cCRLF;
constext txt_help_i_star[] = "*\t  select thread" cCRLF;
constext txt_help_i_tab[] = "<TAB>\t  display next unread article" cCRLF;
constext txt_help_i_tilda[] ="~\t  undo all selections (all articles)" cCRLF;
constext txt_help_inverse_okay[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_j[] = "j k\t  down (k = up) one line" cCRLF;
constext txt_help_quick_kill[] = "]\t  quickly kill an article using defaults" cCRLF;
constext txt_help_quick_select[] = "[\t  quickly auto-select (hot) an article using defaults" cCRLF;
constext txt_help_keep_dead_articles[] = "Keep all failed articles in ~/dead.articles. <SPACE> toggles & <CR> sets.";
constext txt_help_keep_posted_articles[] = "Keep all posted articles in ~/Mail/posted. <SPACE> toggles & <CR> sets.";
constext txt_help_l[] = "l\t  list articles within current thread (bring up Thread sub-menu)" cCRLF;
constext txt_help_m[] = "m\t  move chosen group within list" cCRLF;
constext txt_help_mail_8bit_header[] = "Don't change unless you know what you are doing. <ESC> cancels.";
constext txt_help_mail_mime_encoding[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_mail_address[] = "Enter default mail address. <CR> sets.";
constext txt_help_mail_quote_format[] = "%A Addr %D Date %F Addr+Name %G Groupname %M Message-ID %N Name %C First Name";
constext txt_help_maildir[] = "The directory where articles/threads are to be saved in mailbox format.";
constext txt_help_mailer_format[] = "Enter %M for mailer, %S for subject, %T for to, %F for filename, <CR> to set.";
constext txt_help_mark_saved_read[] = "Mark saved articles/threads as read. <SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_mm_charset[] = "Enter charset name for MIME (e.g. US-ASCII, ISO-8859-1, EUC-KR), <CR> to set.";
constext txt_help_n[] = "N\t  choose next group with unread news" cCRLF;
constext txt_help_news_quote_format[] = "%A Addr %D Date %F Addr+Name %G Groupname %M Message-Id %N Name %C First Name";
constext txt_help_news_headers_to_display[] = "Space separated list of header fields";
constext txt_help_news_headers_to_not_display[] = "Space separated list of header fields";
constext txt_help_advertising[] = "If ON, include User-Agent: header. <SPACE> toggles & <CR> sets.";
constext txt_help_o[] = "o\t  output article/thread/hot/pattern/tagged articles to printer" cCRLF;
constext txt_help_p_caret_dollar[] = "^ $\t  display first ($ = last) page of article" cCRLF;
constext txt_help_p_coma[] = "@\t  reverse article selections" cCRLF;
constext txt_help_p_ctrl_e[] = "^E\t  reply through mail to author quoting complete headers" cCRLF;
constext txt_help_p_ctrl_w[] = "^W\t  post followup to current article quoting complete headers" cCRLF;
constext txt_help_p_cr[] = "<CR>\t  display first article in next thread" cCRLF;
constext txt_help_p_d[] = "d\t  decode article; toggle rotate-13 for current article" cCRLF;
constext txt_help_p_dot[] = ".\t  toggle article selection" cCRLF;
constext txt_help_p_f[] = "f F\t  post followup (F = don't copy text) to current article" cCRLF;
constext txt_help_p_g[] = "g G\t  display first (G = last) page of article" cCRLF;
constext txt_help_p_k[] = "k K\t  mark article (K = thread) as read and advance to next unread" cCRLF;
constext txt_help_p_m[] = "m\t  mail article/thread/hot/pattern/tagged articles to someone" cCRLF;
constext txt_help_p_n[] = "n N\t  display next (N = next unread) article" cCRLF;
constext txt_help_p_num[] = "0 - 9\t  display article by number in current thread" cCRLF;
constext txt_help_p_p[] = "p P\t  display previous (P = previous unread) article" cCRLF;
constext txt_help_p_r[] = "r R\t  reply through mail (R = don't copy text) to author" cCRLF;
constext txt_help_p_s[] = "s\t  save article/thread/hot/pattern/tagged articles to file" cCRLF;

#ifdef CASE_PROBLEM
	constext txt_help_p_BIGS[] = "S\t  save tagged articles automatically without user prompts" cCRLF;
#else
	constext txt_help_p_S[] = "S\t  save tagged articles automatically without user prompts" cCRLF;
#endif /* CASE_PROBLEM */

constext txt_help_full_page_scroll[] = "Scroll half/full page of groups/articles. <SPACE> toggles & <CR> sets.";
constext txt_help_p_search[] = "/\t  search forward within this article" cCRLF;
constext txt_help_p_star[] = "*\t  select article" cCRLF;
constext txt_help_p_tab[] = "<TAB>\t  display next unread article" cCRLF;
constext txt_help_p_u[] = "u\t  goto the article that this one followed up" cCRLF;
constext txt_help_p_tilda[] = "~\t  undo all selections in thread" cCRLF;
constext txt_help_p_z[] = "z Z\t  mark article (Z = thread) as unread" cCRLF;
constext txt_help_pipe[] = "|\t  pipe article/thread/hot/pattern/tagged articles into command" cCRLF;
constext txt_help_plus[] = "+\t  select group (make \"hot\")" cCRLF;
constext txt_help_pos_first_unread[] = "Put cursor at first/last unread art in groups. <SPACE> toggles & <CR> sets.";
constext txt_help_post_8bit_header[] = "Don't change unless you know what you are doing. <ESC> cancels.";
constext txt_help_post_mime_encoding[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_post_process_type[] = "Post process (ie. unshar) saved article/thread. <SPACE> toggles & <CR> sets.";
constext txt_help_print_header[] = "Print all or just part of header. <SPACE> toggles & <CR> sets.";
constext txt_help_printer[] = "The printer program with options that is to be used to print articles/threads.";
constext txt_help_process_only_unread[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_prompt_followupto[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_q[] = "Q\t  quit" cCRLF;
constext txt_help_quote_chars[] = "Enter quotation marks, %s or %S for author's initials.";
constext txt_help_quote_empty_lines[] = "Quote empty lines. <SPACE> toggles & <CR> sets.";
constext txt_help_quote_signatures[] = "Quote signatures. <SPACE> toggles & <CR> sets.";
constext txt_help_r[] = "r\t  toggle display of all/unread articles" cCRLF;
constext txt_help_reread_active_file_secs[] = "Enter number of seconds until active file will be reread. <CR> sets.";
constext txt_help_s[] = "s u\t  subscribe (u = unsubscribe) to chosen group" cCRLF;
constext txt_help_save_to_mmdf_mailbox[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_savedir[] = "The directory where you want articles/threads saved.";
constext txt_help_semicolon[] = ";\t  select threads if at least one unread article is selected" cCRLF;
constext txt_help_show_author[] = "Show Subject & From (author) fields in group menu. <SPACE> toggles & <CR> sets.";
constext txt_help_show_description[] = "Show short description for each newsgroup. <SPACE> toggles & <CR> sets.";
constext txt_help_show_last_line_prev_page[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_show_lines[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_show_score[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_show_only_unread_arts[] = "Show all articles or only unread articles. <SPACE> toggles & <CR> sets.";
constext txt_help_show_only_unread_groups[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_show_signatures[] = "Display signatures. <SPACE> toggles & <CR> sets.";
constext txt_help_show_xcommentto[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_sigdashes[] = "Prepend signature with \"-- \" on own line. <SPACE> toggles & <CR> sets.";
constext txt_help_signature_repost[] = "Add signature when reposting articles. <SPACE> toggles & <CR> sets.";
constext txt_help_sigfile[] = "Enter path/! command/--none to create your default signature. <CR> sets.";
constext txt_help_sort_article_type[] = "Sort articles by Subject, From, Date or Score. <SPACE> toggles & <CR> sets.";
constext txt_help_space_goto_next_unread[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_spamtrap_warning_addresses[] = "Enter address elements about which you want to be warned. <CR> sets.";
constext txt_help_pgdn_goto_next[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_start_editor_offset[] = "Start editor with line offset. <SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_strip_blanks[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_strip_bogus[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_strip_newsrc[] = "Do you want to strip unsubscribed groups from .newsrc";
constext txt_help_t[] = "t\t  tag current article for reposting/mailing/piping/printing/saving" cCRLF;
constext txt_help_tab_after_X_selection[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_tab_goto_next_unread[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_tex[] = "\"\t  toggle german TeX style decoding for current article" cCRLF;
constext txt_help_t_caret_dollar[] = "^ $\t  choose first ($ = last) article in list" cCRLF;
constext txt_help_t_cr[] = "<CR>\t  read chosen article" cCRLF;
constext txt_help_t_num[] = "0 - 9\t  choose article by number" cCRLF;
constext txt_help_thread[] = "< >\t  display first (> = last) article in current thread" cCRLF;
constext txt_help_thread_articles[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_thread_catchup_on_exit[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_u[] = "u\t  cycle through threading options available" cCRLF;
constext txt_help_unlink_article[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";

#if defined(NNTP_ABLE) || defined(NNTP_ONLY)
	constext txt_help_use_builtin_inews[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
#endif /* NNTP_ABLE || NNTP_ONLY */

#ifdef HAVE_COLOR
	constext txt_help_use_color[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
#endif /* HAVE_COLOR */

#ifdef HAVE_KEYPAD
	constext txt_help_use_keypad[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
#endif /* HAVE_KEYPAD */

constext txt_help_use_mailreader_i[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";

#ifdef HAVE_METAMAIL
	constext txt_help_use_metamail[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
#endif /* HAVE_METAMAIL */

constext txt_help_g_t_p_i[] = "i\t  toggle info message in last line (subject/description)" cCRLF;
constext txt_help_s_i[] = "i\t  show description of chosen newsgroup" cCRLF;
constext txt_help_use_mouse[] = "<SPACE> toggles, <CR> sets, <ESC> cancels.";
constext txt_help_v[] = "v\t  show version information" cCRLF;
constext txt_help_w[] = "w\t  post (write) article to current group" cCRLF;
constext txt_help_wildcard[] = "WILDMAT for normal wildcards, REGEX for full regular expression matching.";
constext txt_help_x[] = "x\t  repost chosen article to another group" cCRLF;
constext txt_help_xpost_quote_format[] = "%A Addr %D Date %F Addr+Name %G Groupname %M Message-ID %N Name %C First Name";
constext txt_help_y[] = "Y\t  yank in active file to see any new news" cCRLF;

#ifdef HAVE_COLOR
	constext txt_help__[] = "_\t  toggle word highlighting on/off" cCRLF;
#endif /* HAVE_COLOR */

constext txt_hit_space_for_more[] = "PgDn,End,<SPACE>,^D - page down. PgUp,Home,b,^U - page up. <CR>,q - quit";
constext txt_index_page_com[] = "Group Level Commands (page %d of %d)";
constext txt_info_add_kill[] = "Kill filter added";
constext txt_info_add_select[] = "Auto-selection filter added";
constext txt_info_do_postpone[] = "Storing article for later posting";
constext txt_info_postponed[] = "%d postponed article%s, reuse with ^O...\n";
constext txt_info_nopostponed[] = "*** No postponed articles ***";
constext txt_info_not_subscribed[] = "You are not subscribed to this group";
constext txt_info_no_write[] = "Operation disabled in no-overwrite mode";
constext txt_is_tex_ecoded[]= "TeX2Iso encoded article";

#ifndef INDEX_DAEMON
	constext txt_intro_page[] = "\nWelcome to tin, a full screen threaded Netnews reader. It can read news locally\n\
(ie. <spool>/news) or remotely (-r option)  from a NNTP (Network News Transport\n\
Protocol) server. tin -h lists the available command line options.\n\n\
Tin  has four newsreading levels, the newsgroup selection page, the group index\n\
page, the thread listing page and the article viewer. Help is available at each\n\
level by pressing the 'h' command.\n\n\
Move up/down by using the terminal arrow keys or 'j' and 'k'.  Use PgUp/PgDn or\n\
Ctrl-U and Ctrl-D to page up/down. Enter a newsgroup by pressing RETURN/TAB.\n\n\
Articles, threads, tagged articles or articles matching a pattern can be mailed\n\
('m' command), printed ('o' command), saved ('s' command), piped ('|' command).\n\
Use  the 'w' command  to post a  news article,  the 'f'/'F' commands  to post a\n\
follow-up to  an existing  news article  and the 'r'/'R' commands  to reply via\n\
mail to an existing news article author.  The 'M' command allows the operation\n\
of tin to be configured via a menu.\n\n\
For more information read the manual page, README, INSTALL, TODO and FTP files.\n\
Please send bug-reports/comments to %s with the 'R' command.\n";
#endif /* !INDEX_DAEMON */

constext txt_invalid_from[] = "Invalid  From: %s  line. Read the INSTALL file again.";
constext txt_inverse_off[] = "Inverse video disabled";
constext txt_inverse_on[] = "Inverse video enabled";
constext txt_last_resp[] = "-- Last response --";
constext txt_lines[] = "Lines %s  ";
constext txt_listing_archive[] = cCRLF cCRLF "Listing %s archive..." cCRLF;
constext txt_mail[] = "Mail";
constext txt_mail_art_to[] = "Mail article(s) to [%.*s]> ";
constext txt_mail_bug_report[] = "Mail bug report...";
constext txt_mail_bug_report_confirm[] = "Mail BUG REPORT to %s? (y/n): ";
constext txt_mailed[] = "-- %d Article%s mailed --";
constext txt_mailing_to[] = "Mailing to %s...";
constext txt_marked_as_unread[] = "%s marked as unread";
constext txt_mark_arts_read[] = "Mark all articles as read%s? (y/n): ";
constext txt_mark_group_read[] = "Mark group %.*s as read? (y/n): ";
constext txt_mark_thread_read[] = "Mark thread as read%s? (y/n): ";
constext txt_matching_cmd_line_groups[] = "Matching %s groups...";
constext txt_mini_group_1[] = "<n>=set current to n, TAB=next unread, /=search pattern, ^K)ill/select,";
constext txt_mini_group_2[] = "a)uthor search, c)atchup, j=line down, k=line up, K=mark read, l)ist thread,";
constext txt_mini_group_3[] = "|=pipe, m)ail, o=print, q)uit, r=toggle all/unread, s)ave, t)ag, w=post";
constext txt_mini_page_1[] = "<n>=set current to n, TAB=next unread, /=search pattern, ^K)ill/select,";
constext txt_mini_page_2[] = "a)uthor search, B)ody search, c)atchup, f)ollowup, K=mark read,";
constext txt_mini_page_3[] = "|=pipe, m)ail, o=print, q)uit, r)eply mail, s)ave, t)ag, w=post";
constext txt_mini_select_1[] = "<n>=set current to n, TAB=next unread, /=search pattern, c)atchup,";
constext txt_mini_select_2[] = "g)oto, j=line down, k=line up, h)elp, m)ove, q)uit, r=toggle all/unread,";
constext txt_mini_select_3[] = "s)ubscribe, S)ub pattern, u)nsubscribe, U)nsub pattern, y)ank in/out";
constext txt_mini_thread_1[] = "<n>=set current to n, TAB=next unread, c)atchup, d)isplay toggle,";
constext txt_mini_thread_2[] = "h)elp, j=line down, k=line up, q)uit, t)ag, z=mark unread";
constext txt_more[] = "--More--";
constext txt_moving[] = "Moving %s...";
constext txt_msg_headers_file[] = "# Add extra header lines to mail/news messages.\n# ie.  Organization: Bits & Bobs Inc.\n\n";
constext txt_newsgroup[] = "Goto newsgroup [%s]> ";
constext txt_newsgroup_position[] = "Position %s in group list (1,2,..,$) [%d]> ";
constext txt_newsrc_again[] = "Try and save newsrc file again? (y/n): ";
constext txt_newsrc_nogroups[] = "Warning: No newsgroups were written to your newsrc file. Save aborted.";
constext txt_newsrc_saved[] = "newsrc file saved successfully.\n";
constext txt_next_resp[] = "-- Next response --";
constext txt_nntp_authorization_failed[] = "NNTP authorization password not found for %s";
constext txt_no[] = "No ";
constext txt_no_arts[] = "*** No articles ***";
constext txt_no_arts_posted[] = "No articles have been posted";
constext txt_no_command[] = "No command";
#ifdef HAVE_COLOR
#	ifdef USE_CURSES
		constext txt_no_colorterm[] = "Terminal does not support color";
#	endif /* USE_CURSES */
#endif /* HAVE_COLOR */
constext txt_no_description[] = "*** No description ***";
constext txt_no_filename[] = "No filename";
constext txt_no_group[] = "No group";
constext txt_no_groups[] = "*** No groups ***";
constext txt_no_groups_to_read[] = "No more groups to read";
constext txt_no_groups_to_yank_in[] = "No more groups to yank in";
constext txt_no_last_message[] = "No last message";
constext txt_no_mail_address[] = "No mail address";
constext txt_no_match[] = "No match";
constext txt_no_more_groups[] = "No more groups";
constext txt_no_next_unread_art[] = "No next unread article";
constext txt_no_prev_group[] = "No previous group";
constext txt_no_prev_unread_art[] = "No previous unread article";
constext txt_no_quick_newsgroups[] = "\nNo newsgroups. Exiting...";
constext txt_no_quick_subject[] = "\nNo subject. Exiting...";
constext txt_no_resp[] = "No responses" cCRLF;
constext txt_no_responses[] = "No responses";
constext txt_no_resps_in_thread[] = "No responses to list in current thread";
constext txt_no_search_string[] = "No search string";
constext txt_no_subject[] = "No subject";
constext txt_no_tagged_arts_to_save[] = "No articles tagged for saving";
constext txt_no_term_clear_eol[] = "%s: Terminal must have clear to end-of-line (ce)\n";
constext txt_no_term_clear_eos[] = "%s: Terminal must have clear to end-of-screen (cd)\n";
constext txt_no_term_clearscreen[] = "%s: Terminal must have clearscreen (cl) capability\n";
constext txt_no_term_cursor_motion[] = "%s: Terminal must have cursor motion (cm)\n";
constext txt_no_term_set[] = "%s: TERM variable must be set to use screen capabilities\n";
constext txt_not_exist[] = "Newsgroup does not exist on this server";
constext txt_not_in_active_file[] = "Group %s not found in active file";
constext txt_nrctbl_create[] = "c)reate it, use a)lternative name, use d)efault .newsrc, q)uit tin: ";
constext txt_nrctbl_default[] = "use a)lternative name, use d)efault .newsrc, q)uit tin: ";

constext txt_nrctbl_info[] = "# NNTP-server -> newrc translation table and NNTP-server\n\
# shortname list for %s %s\n#\n# the format of this file is\n\
#   <FQDN of NNTP-server> <newsrc file> <shortname> ...\n#\n\
# if <newsrc file> is given without path, $HOME is assumed as its location\n\
#\n# examples:\n#   news.tin.org  .newsrc-tin.org  tinorg\n\
#   news.ka.nu    /tmp/nrc-nu      kanu    nu\n#\n";

constext txt_opt_add_posted_to_filter[] = "Add posted articles to filter      : ";
constext txt_opt_alternative_handling[] = "Skip multipart/alternative parts   : ";
constext txt_opt_art_marked_deleted[] = "Character to show deleted articles : ";
constext txt_opt_art_marked_inrange[] = "Character to show inrange articles : ";
constext txt_opt_art_marked_return[] = "Character to show returning arts   : ";
constext txt_opt_art_marked_selected[] = "Character to show selected articles: ";
constext txt_opt_art_marked_unread[] = "Character to show unread articles  : ";

#ifdef HAVE_METAMAIL
	constext txt_opt_ask_for_metamail[] = "Ask before using metamail          : ";
#endif /* HAVE_METAMAIL */

constext txt_opt_auto_bcc[] = "Send you a blind cc automatically  : ";
constext txt_opt_auto_cc[] = "Send you a cc automatically        : ";
constext txt_opt_auto_list_thread[] = "List thread using right arrow key  : ";
constext txt_opt_auto_reconnect[] = "Reconnect to server automatically  : ";
constext txt_opt_auto_save[] = "Use Archive-name: header for save  : ";
constext txt_opt_batch_save[] = "Save articles in batch mode (-S)   : ";
constext txt_opt_beginner_level[] = "Show mini menu & posting etiquette : ";
constext txt_opt_cache_overview_files[] = "Cache NNTP overview files locally  : ";
constext txt_opt_catchup_read_groups[] = "Catchup read groups when quitting  : ";
constext txt_opt_use_getart_limit[] = "Use getart_limit                   : ";
constext txt_opt_getart_limit[] = "Number of articles to get          : ";

#ifdef HAVE_COLOR
	constext txt_opt_col_back[] = "Standard background color          : ";
	constext txt_opt_col_from[] = "Color of sender (From:)            : ";
	constext txt_opt_col_head[] = "Color of article header lines      : ";
	constext txt_opt_col_help[] = "Color of help text                 : ";
	constext txt_opt_col_invers_fg[] = "Color for inverse text (foreground): ";
	constext txt_opt_col_invers_bg[] = "Color for inverse text (background): ";
	constext txt_opt_col_message[] = "Color of status messages           : ";
	constext txt_opt_col_minihelp[] = "Color of mini help menu            : ";
	constext txt_opt_col_newsheaders[] = "Color of actual news header fields : ";
	constext txt_opt_col_normal[] = "Standard foreground color          : ";
	constext txt_opt_col_quote[] = "Color of quoted lines              : ";
	constext txt_opt_col_response[] = "Color of response counter          : ";
	constext txt_opt_col_signature[] = "Color of signatures                : ";
	constext txt_opt_col_subject[] = "Color of article subject lines     : ";
	constext txt_opt_col_text[] = "Color of text lines                : ";
	constext txt_opt_col_title[] = "Color of help/mail sign            : ";
	constext txt_opt_word_highlight[] = "Word highlighting in message body  : ";
	constext txt_opt_word_h_display_marks[] = "What to display instead of mark    : ";
	constext txt_opt_col_markstar[] = "Color of highlighting with *stars* : ";
	constext txt_opt_col_markdash[] = "Color of highlighting with _dash_  : ";
#endif /* HAVE_COLOR */

constext txt_opt_confirm_action[] = "Confirm commands before executing  : ";
constext txt_opt_confirm_to_quit[] = "Confirm before quitting            : ";
constext txt_opt_display_mime_header_asis[] = "Show RFC 2047 header undecoded     : ";
constext txt_opt_display_mime_allheader_asis[] = "Show RFC 2047 ^H-header undecoded  : ";
constext txt_opt_draw_arrow_mark[] = "Draw -> instead of highlighted bar : ";
constext txt_opt_editor_format[] = "Invocation of your editor          : ";
constext txt_opt_filter_days[] = "No. of days a filter entry is valid: ";
constext txt_opt_force_screen_redraw[] = "Force redraw after certain commands: ";
constext txt_opt_group_catchup_on_exit[] = "Catchup group using left key       : ";
constext txt_opt_groupname_max_length[] = "Max. length of group names shown   : ";
constext txt_opt_inverse_okay[] = "Use inverse video for page headers : ";
constext txt_opt_keep_dead_articles[] = "Keep failed arts in ~/dead.articles: ";
constext txt_opt_keep_posted_articles[] = "Keep posted arts in ~/Mail/posted  : ";
constext txt_opt_mail_8bit_header[] = "Use 8bit characters in mail headers: ";
constext txt_opt_mail_address[] = "Mail address                       : ";
constext txt_opt_full_page_scroll[] = "Scroll full page (OFF=half page)   : ";
constext txt_opt_mail_mime_encoding[] = "MIME encoding in mail messages     : ";
constext txt_opt_mail_quote_format[] = "Quote line when mailing            : ";
constext txt_opt_maildir[] = "Mail directory                     : ";
constext txt_opt_mailer_format[] = "Invocation of your mail command    : ";
constext txt_opt_mark_saved_read[] = "Mark saved articles/threads as read: ";
constext txt_opt_mm_charset[] = "MM_CHARSET                         : ";
constext txt_opt_news_headers_to_display[] = "Display these header fields (or *) : ";
constext txt_opt_news_headers_to_not_display[] = "Do not display these header fields : ";
constext txt_opt_news_quote_format[] = "Quote line when following up       : ";
constext txt_opt_advertising[] = "Insert 'User-Agent:'-header        : ";
constext txt_opt_pos_first_unread[] = "Goto first unread article in group : ";
constext txt_opt_post_8bit_header[] = "Use 8bit characters in news headers: ";
constext txt_opt_post_mime_encoding[] = "MIME encoding in news messages     : ";
constext txt_opt_post_process_type[] = "Post process saved art/thread with : ";
constext txt_opt_print_header[] = "Print all headers when printing    : ";
constext txt_opt_printer[] = "Printer program with options       : ";
constext txt_opt_process_only_unread[] = "Process only unread articles       : ";
constext txt_opt_prompt_followupto[] = "Show empty Followup-To in editor   : ";
constext txt_opt_quote_chars[] = "Characters used as quote-marks     : ";
constext txt_opt_quote_empty_lines[] = "Quote empty lines                  : ";
constext txt_opt_quote_signatures[] = "Quote signatures                   : ";
constext txt_opt_reread_active_file_secs[] = "Interval in secs to reread active  : ";
constext txt_opt_save_to_mmdf_mailbox[] = "Save mail in MMDF style            : ";
constext txt_opt_savedir[] = "Directory to save arts/threads in  : ";
constext txt_opt_show_author[] = "In group menu, show author by      : ";
constext txt_opt_show_description[] = "Show description of each newsgroup : ";
constext txt_opt_show_last_line_prev_page[] = "Show last line of previous page    : ";
constext txt_opt_show_lines[] = "Show no. of lines in thread listing: ";
constext txt_opt_show_score[] = "Show score of article in listing   : ";
constext txt_opt_show_only_unread_arts[] = "Show only unread articles          : ";
constext txt_opt_show_only_unread_groups[] = "Show only groups with unread arts  : ";
constext txt_opt_show_signatures[] = "Display signatures                 : ";
constext txt_opt_show_xcommentto[] = "Display X-Comment-To: header       : ";
constext txt_opt_sigdashes[] = "Prepend signature with \"-- \"       : ";
constext txt_opt_signature_repost[] = "Add signature when reposting       : ";
constext txt_opt_sigfile[] = "Create signature from path/command : ";
constext txt_opt_sort_article_type[] = "Sort article by                    : ";
constext txt_opt_space_goto_next_unread[] = "Space goes to next unread article  : ";
constext txt_opt_spamtrap_warning_addresses[] = "Spamtrap warning address parts     : ";
constext txt_opt_pgdn_goto_next[] = "PgDn goes to next article at EOF   : ";
constext txt_opt_start_editor_offset[] = "Start editor with line offset      : ";
constext txt_opt_strip_blanks[] = "Strip blanks of end of lines       : ";
constext txt_opt_strip_bogus[] = "Remove bogus groups from newsrc    : ";
constext txt_opt_strip_newsrc[] = "No unsubscribed groups in newsrc   : ";
constext txt_opt_tab_after_X_selection[] = "Do tab after X automatically       : ";
constext txt_opt_tab_goto_next_unread[] = "Tab goes to next unread article    : ";
constext txt_opt_thread_articles[] = "Thread articles by                 : ";
constext txt_opt_thread_catchup_on_exit[] = "Catchup thread by using left key   : ";
constext txt_opt_unlink_article[] = "Remove ~/.article after posting    : ";

#if defined(NNTP_ABLE) || defined(NNTP_ONLY)
	constext txt_opt_use_builtin_inews[] = "Use builtin inews                  : ";
#endif /* NNTP_ABLE || NNTP_ONLY */

#ifdef HAVE_COLOR
	constext txt_opt_use_color[] = "Use ANSI color                     : ";
#endif /* HAVE_COLOR */

#ifdef HAVE_KEYPAD
	constext txt_opt_use_keypad[] = "Use scroll keys on keypad          : ";
#endif /* HAVE_KEYPAD */

constext txt_opt_use_mailreader_i[] = "Use interactive mail reader        : ";

#ifdef HAVE_METAMAIL
	constext txt_opt_use_metamail[] = "Use metamail upon MIME articles    : ";
#endif /* HAVE_METAMAIL */

constext txt_opt_use_mouse[] =	"Use mouse in xterm                 : ";
constext txt_opt_wildcard[] =	"Wildcard matching                  : ";
constext txt_opt_xpost_quote_format[] = "Quote line when cross-posting      : ";
constext txt_option_not_enabled[] = "Option not enabled. Recompile with %s.";
constext txt_options_menu[] = "Options Menu";
constext txt_out_of_memory[] = "%s: memory exhausted trying to allocate %d bytes in file %s line %d";

constext txt_pcre_error_at[] = "Error in regex: %s at pos. %d";
constext txt_pcre_error_num[] = "Error in regex: pcre internal error %d";
constext txt_pcre_error_text[] = "Error in regex: study - pcre internal error %s";

#ifdef HAVE_PGP
	constext txt_pgp_mail[] = "e)ncrypt, s)ign, b)oth, q)uit: ";
	constext txt_pgp_news[] = "s)ign, i) sign & include public key, q)uit: ";
	constext txt_pgp_not_avail[] = "PGP has not been set up for this account.";
	constext txt_pgp_nothing[] = "Article not signed and no public keys found.";
#endif /* HAVE_PGP */

#ifndef DONT_HAVE_PIPING
	constext txt_pipe[] = "Pipe";
	constext txt_pipe_to_command[] = "Pipe to command [%.*s]> ";
	constext txt_piping[] = "Piping...";
#else
#	ifdef VMS
		constext txt_pipe_to_command[] = "";
		constext txt_piping[] = "";
#	endif /* VMS */
	constext txt_piping_not_enabled[] = "Piping not enabled.";
#endif /* !DONT_HAVE_PIPING */

constext txt_plural[] = "s";
constext txt_posted_info_file[] = "# Summary of mailed/posted messages viewable by 'W' command from within tin.\n";
constext txt_print[] = "Print";
constext txt_repost[] = "Repost";
constext txt_save[] = "Save";
constext txt_screen_too_small[] = "%s: screen is too small\n";
constext txt_screen_too_small_exiting[] = "screen is too small, tin is exiting\n";
constext txt_tinrc_add_posted_to_filter[] = "# If ON add posted articles to filter for highlighting follow-ups\n";
constext txt_tinrc_alternative_handling[] = "# If ON strip multipart/alternative messages automatically\n";
constext txt_tinrc_art_marked_deleted[] = "# Character used to show that an art was deleted (default 'D')\n";
constext txt_tinrc_art_marked_inrange[] = "# Character used to show that an art is in a range (default '#')\n";
constext txt_tinrc_art_marked_return[] = "# Character used to show that an art will return (default '-')\n";
constext txt_tinrc_art_marked_selected[] = "# Character used to show that an art was auto-selected (default '*')\n";
constext txt_tinrc_art_marked_unread[] = "# Character used to show that an art was unread (default '+')\n";
#ifdef HAVE_METAMAIL
	constext txt_tinrc_ask_for_metamail[] = "# If ON tin will ask before using metamail to display MIME messages\n\
# this only occurs, if use_metamail is also switched ON\n";
#endif /* HAVE_METAMAIL */
constext txt_tinrc_auto_bcc[] = "# If ON automatically put your name in the Bcc: field when mailing an article\n";
constext txt_tinrc_auto_cc[] = "# If ON automatically put your name in the Cc: field when mailing an article\n";
constext txt_tinrc_auto_list_thread[] = "# If ON automatically list thread when entering it using right arrow key.\n";
constext txt_tinrc_auto_reconnect[] = "# If ON automatically reconnect to NNTP server if the connection is broken\n";
constext txt_tinrc_auto_save[] = "# If ON articles/threads with Archive-name: in mail header will\n\
# be automatically saved with the Archive-name & part/patch no.\n";
constext txt_tinrc_batch_save[] = "# If ON articles/threads will be saved in batch mode when save -S\n\
# or mail -M is specified on the command line\n";
constext txt_tinrc_beginner_level[] = "# If ON show a mini menu of useful commands at each level\n\
# and posting etiquette after composing a article\n";
constext txt_tinrc_cache_overview_files[] = "# If ON, create local copies of NNTP overview files.\n";
constext txt_tinrc_catchup_read_groups[] = "# If ON ask user if read groups should all be marked read\n";
constext txt_tinrc_use_getart_limit[] = "# If ON limit the number of articles to get\n";
constext txt_tinrc_getart_limit[] = "# Number of articles to get (0=no limit)\n";
#ifdef HAVE_COLOR
	constext txt_tinrc_col_back[] = "# Standard-Background-Color\n";
	constext txt_tinrc_col_from[] = "# Color of sender (From:)\n";
	constext txt_tinrc_col_head[] = "# Color of header-lines\n";
	constext txt_tinrc_col_newsheaders[] = "# Color of actual news header fields\n";
	constext txt_tinrc_col_help[] = "# Color of help pages\n";
	constext txt_tinrc_col_invers_bg[] = "# Color of background for inverse text\n";
	constext txt_tinrc_col_invers_fg[] = "# Color of foreground for inverse text\n";
	constext txt_tinrc_col_markstar[] = "# Color of word highlighting. There are two possibilities for\n\
# in Articles: *stars* and _underdashes_\n";
	constext txt_tinrc_col_message[] = "# Color of messages in last line\n";
	constext txt_tinrc_col_minihelp[] = "# Color of mini help menu\n";
	constext txt_tinrc_col_normal[] = "# Standard foreground color\n";
	constext txt_tinrc_col_response[] = "# Color of response counter\n";
	constext txt_tinrc_col_quote[] = "# Color of quote-lines\n";
	constext txt_tinrc_col_signature[] = "# Color of signature\n";
	constext txt_tinrc_col_subject[] = "# Color of article subject\n";
	constext txt_tinrc_col_text[] = "# Color of text-lines\n";
	constext txt_tinrc_col_title[] = "# Color of Help/Mail-Sign\n";
	constext txt_tinrc_colors[] = "# For color-adjust use the following numbers\n\
#  0-black       1-red         2-green        3-brown\n\
#  4-blue        5-pink        6-cyan         7-white\n\
# These are *only* for foreground:\n\
#  8-gray        9-lightred   10-lightgreen  11-yellow\n\
# 12-lightblue  13-lightpink  14-lightcyan   15-lightwhite\n\
# A '-1' is interpreted as default (foreground normally is white, and\n\
# background black)\n\n";
#endif /* HAVE_COLOR */
constext txt_tinrc_confirm_action[] = "# If ON confirm certain commands with y/n before executing\n";
constext txt_tinrc_confirm_to_quit[] = "# If ON confirm with y/n before quitting ('Q' never asks)\n";
constext txt_tinrc_default_editor_format[] = "# Format of editor line including parameters\n\
# %%E Editor  %%F Filename  %%N Linenumber\n";
constext txt_tinrc_default_filter_days[] = "# Num of days a short term filter will be active\n";
constext txt_tinrc_maildir[] = "# (-m) directory where articles/threads are saved in mailbox format\n";
constext txt_tinrc_default_mailer_format[] = "# Format of mailer line including parameters\n\
# %%M Mailer  %%S Subject  %%T To  %%F Filename  %%U User (AmigaDOS)\n\
# ie. to use elm as your mailer:    elm -s \"%%S\" \"%%T\" < %%F\n\
# ie. elm interactive          :    elm -i %%F -s \"%%S\" \"%%T\"\n";
constext txt_tinrc_default_printer[] = "# Print program with parameters used to print articles/threads\n";
constext txt_tinrc_savedir[] = "# Directory where articles/threads are saved\n";
constext txt_tinrc_default_sigfile[] = "# Signature path (random sigs)/file to be used when posting/replying\n\
# default_sigfile=file       appends file as signature\n\
# default_sigfile=!command   executes external command to generate a signature\n\
# default_sigfile=--none     don't append a signature\n";
constext txt_tinrc_defaults[] = "# Default action/prompt strings\n";
constext txt_tinrc_display_mime_header_asis[] = "# If ON, RFC 1522 (now RFC 2047)-style header fields are displayed\n\
# without decoding. No effect on header displayed at the top of\n\
# each page in article mode and summary mode (they are always decoded).\n\
# default is OFF so that RFC 1522 (RFC 2047)-style headers (designated in\n\
# news_header_to_display) will be decoded back to 8bit when displayed.\n";
constext txt_tinrc_display_mime_allheader_asis[] = "# Similar to display_mime_header_asis, but works on CTRL-h (show all headers)\n\
# instead of news_headers_to_display\n";
constext txt_tinrc_draw_arrow[] = "# If ON use -> otherwise highlighted bar for selection\n";
constext txt_tinrc_filter[] = "# Defaults for quick (1 key) kill & auto-selection filters\n\
# header=NUM  0,1=Subject: 2,3=From: 4=Message-Id: & full References: line\n\
#             5=Message-Id: & last References: entry only\n\
#             6=Message-Id: entry only 7=Lines:\n\
# global=ON/OFF  ON=apply to all groups OFF=apply to current group\n\
# case=ON/OFF    ON=filter case sensitive OFF=ignore case\n\
# expire=ON/OFF  ON=limit to default_filter_days OFF=don't ever expire\n";
constext txt_tinrc_force_screen_redraw[] = "# If ON a screen redraw will always be done after certain external commands\n";
constext txt_tinrc_full_page_scroll[] = "# If ON scroll full page of groups/articles otherwise half a page\n";
constext txt_tinrc_group_catchup_on_exit[] = "# If ON catchup group/thread when leaving with the left arrow key.\n";
constext txt_tinrc_groupname_max_length[] = "# Maximum length of the names of newsgroups displayed\n";
constext txt_tinrc_header[] = "# tin-unoff configuration file V%s\n\
# This file was automatically saved by %s %s %s (\"%s\")\n#\n\
# Do not edit while tin is running, since all your changes to this file\n\
# will be overwritten when you leave tin.\n#\n\
############################################################################\n\n";
constext txt_tinrc_info_in_last_line[] = "# If ON use print current subject or newsgroup description in the last line\n";
constext txt_tinrc_inverse_okay[] = "# If ON use inverse video for page headers at different levels\n";
constext txt_tinrc_keep_dead_articles[] = "# If ON keep all failed postings in ~/dead.articles\n";
constext txt_tinrc_keep_posted_articles[] = "# If ON keep all postings in ~/Mail/posted\n";
#ifdef LOCAL_CHARSET
	constext txt_tinrc_local_charset[] = "# Whether or not to automatically convert to a local charset that is\n\
# different from the one defined in mm_charset. Currently only NeXTstep is\n\
# supported. Set to OFF when logged in from a iso-8859-1 environment.\n";
#endif /* LOCAL_CHARSET */
constext txt_tinrc_mail_8bit_header[] = "# If ON, 8bit characters in mail message is NOT encoded.\n\
# default is OFF. Thus 8bit character is encoded by default.\n\
# 8bit chars in header is encoded regardless of the value of this parameter\n\
# unless mail_mime_encoding is 8bit as well. Note that RFC 1552/1651/1652\n\
# prohibit 8bit characters in mail header so that you are advised NOT to\n\
# turn it ON unless you have some compelling reason.\n";
constext txt_tinrc_mail_address[] = "# User's mail address, if not username@host\n";
constext txt_tinrc_mark_saved_read[] = "# If ON mark articles that are saved as read\n";
constext txt_tinrc_mm_charset[] = "# Charset supported locally which is also used for MIME header and\n\
# Content-Type header unless news and mail need to be encoded in other\n\
# charsets as in case of EUC-KR for Korean which needs to be converted to\n\
# ISO-2022-KR in mail message.\n\
# If not set, the value of the environment variable MM_CHARSET is used.\n\
# Set to US-ASCII or compile time default if neither of them is defined.\n\
# If MIME_STRICT_CHARSET is defined at compile-time, charset other than\n\
# mm_charset is considered not displayable and represented as '?'.\n";
constext txt_tinrc_newnews[] = "# Host & time info used for detecting new groups (don't touch)\n";
constext txt_tinrc_news_quote_format[] = "# Format of quote line when mailing/posting/following-up an article\n\
# %%A Address    %%D Date   %%F Addr+Name   %%G Groupname   %%M Message-Id\n\
# %%N Full Name  %%C First Name\n";
constext txt_tinrc_advertising[] = "# If ON include advertising User-Agent: header\n";
constext txt_tinrc_pos_first_unread[] = "# If ON put cursor at first unread art in group otherwise last art\n";
constext txt_tinrc_post_8bit_header[] = "# If ON, 8bit characters in news posting is NOT encoded.\n\
# default is OFF. Thus 8bit character is encoded by default.\n\
# 8bit chars in header is encoded regardless of the value of this\n\
# parameter unless post_mime_encoding is 8bit as well.\n";
constext txt_tinrc_post_mime_encoding[] = "# MIME encoding (8bit, base64, quoted-printable, 7bit) of the body\n\
# for mails and posts, if necessary. QP is efficient for most European\n\
# character sets (ISO-8859-X) with small fraction of non-US-ASCII chars,\n\
# while Base64 is more efficient for most 8bit East Asian,Greek,and Russian\n\
# charsets with a lot of 8bit characters.\n\
# For EUC-KR, 7bit encoding specifies that EUC charsets be converted\n\
# to corresponding ISO-2022-KR. The same may be true of EUC-JP/CN.\n\
# Korean users should set post_mime_encoding to 8bit and mail_mime_encoding\n\
# to 7bit. With mm_charset to EUC-KR, post_mime_encoding set to 7bit does\n\
# NOT lead to conversion of EUC-KR into ISO-2022-KR in news-postings since\n\
# it's never meant to be used for Usenet news. Japanese always use\n\
# ISO-2022-JP for both news and mail. Automatic conversion of EUC-JP and\n\
# other 8bit Japanese encodings into ISO-2022-JP(even if 7bit is chosen)\n\
# is NOT yet implemented.(it may not be necessary at all as Japanese\n\
# terminal emulators and editors appear to have native support of\n\
# ISO-2022-JP). In case of Chinese, the situation seems to be more\n\
# complicated(different newsgroups and hierarchies for Chinese use different\n\
# MIME charsets and encodings) and no special handling is yet implemented.\n\
# Summing up 7bit does NOT have any effect on MIME charset other than EUC-KR\n";
constext txt_tinrc_post_process_command[] = "# If set, command to be run after a successful uudecode\n";
#ifdef M_AMIGA
	constext txt_tinrc_post_process_type[] = "# Type of post processing to perform after saving articles.\n\
# 0=(none) 1=(unshar) 2=(uudecode) 3=(uudecode & list lha)\n\
# 4=(uud & extract lha) 5=(uud & list zip) 6=(uud & extract zip)\n";
#else
	constext txt_tinrc_post_process_type[] = "# Type of post processing to perform after saving articles.\n\
# 0=(none) 1=(unshar) 2=(uudecode) 3=(uudecode & list zoo)\n\
# 4=(uud & extract zoo) 5=(uud & list zip) 6=(uud & extract zip)\n";
#endif /* M_AMIGA */
constext txt_tinrc_print_header[] = "# If ON print all of article header otherwise just the important lines\n";
constext txt_tinrc_process_only_unread[] = "# If ON only save/print/pipe/mail unread articles (tagged articles excepted)\n";
constext txt_tinrc_prompt_followupto[] = "# If ON show empty Followup-To header when editing an article\n";
constext txt_tinrc_quote_chars[] = "# Characters used in quoting to followups and replys.\n\
# '_' is replaced by ' ', %%s, %%S are replaced by author's initials.\n";
constext txt_tinrc_quote_empty_lines[] = "# If ON quote empty lines, too\n";
constext txt_tinrc_quote_signatures[] = "# If ON quote signatures, too\n";
constext txt_tinrc_reread_active_file_secs[] = "# Time interval in seconds between rereading the active file\n";
constext txt_tinrc_save_to_mmdf_mailbox[] = "# If ON save mail to a MMDF style mailbox (default is normal mbox format)\n";
constext txt_tinrc_spamtrap_warning_addresses[] = "# A comma-delimeted list of address-parts you want to be\n\
# about when trying to reply by email.\n";
constext txt_tinrc_news_headers_to_display[] = "# Which news headers you wish to see. If you want to see _all_ the headers,\n\
# place an '*' as this value. This is the only way a wildcard can be used.\n\
# If you enter 'X-' as the value, you will see all headers beginning with\n\
# 'X-' (like X-Alan or X-Pape). You can list more than one by delimiting with\n\
# spaces. Not defining anything turns off this option.\n";
constext txt_tinrc_news_headers_to_not_display[] = "# Same as 'news_headers_to_display' except it denotes the opposite.\n\
# An example of using both options might be if you thought X- headers were\n\
# A Good Thing(tm), but thought Alan and Pape were miscreants...well then you\n\
# would do something like this:\n\
# news_headers_to_display=X-\n\
# news_headers_to_not_display=X-Alan X-Pape\n\
# Not defining anything turns off this option.\n";
constext txt_tinrc_show_author[] = "# Part of from field to display 0) none 1) address 2) full name 3) both\n";
constext txt_tinrc_show_description[] = "# If ON show group description text after newsgroup name at\n\
# group selection level\n";
constext txt_tinrc_show_last_line_prev_page[] = "# If ON show the last line of the previous page as first line of next page\n";
constext txt_tinrc_show_lines[] = "# Show number of lines of first unread article in thread listing (ON/OFF)\n";
constext txt_tinrc_show_score[] = "# Show score of article/thread in listing (ON/OFF)\n";
constext txt_tinrc_show_only_unread_arts[] = "# If ON show only new/unread articles otherwise show all.\n";
constext txt_tinrc_show_only_unread_groups[] = "# If ON show only subscribed to groups that contain unread articles.\n";
constext txt_tinrc_show_signatures[] = "# If OFF don't show signatures when displaying articles\n";
constext txt_tinrc_show_xcommentto[] = "# If ON, the realname in the X-Comment-To header is displayed in the\n\
# upper-right corner. You can use news_headers_to_display=X-Comment-To\n\
# instead of this\n";
constext txt_tinrc_sigdashes[] = "# If ON prepend the signature with dashes '\\n-- \\n'\n";
constext txt_tinrc_signature_repost[] = "# If ON add signature to reposted articles\n";
constext txt_tinrc_sort_article_type[] = "# Sort articles by 0=(nothing) 1=(Subject descend) 2=(Subject ascend)\n\
# 3=(From descend) 4=(From ascend) 5=(Date descend) 6=(Date ascend)\n\
# 7=(Score descend) 8=(Score ascend).\n";
constext txt_tinrc_space_goto_next_unread[] = "# If ON the SPACE command will goto next unread article at article viewer\n\
# level when the end of the article is reached (rn-style pager)\n";
constext txt_tinrc_pgdn_goto_next[] = "# If ON the PGDN or DOWN command will goto next article when pressed\n\
# at end of message\n";
constext txt_tinrc_start_editor_offset[] = "# If ON editor will be started with cursor offset into the file\n\
# otherwise the cursor will be positioned at the first line\n";
constext txt_tinrc_strip_blanks[] = "# If ON strip blanks from end of lines to speedup display on slow terminals\n";
constext txt_tinrc_strip_bogus[] = "# What to do with bogus groups in newsrc file\n# 0=(Keep) 1=(Remove) 2=(Highlight with D on selection screen).\n";
constext txt_tinrc_strip_newsrc[] = "# If ON strip unsubscribed groups from newsrc\n";
constext txt_tinrc_tab_after_X_selection[] = "# If ON a TAB command will be automatically done after the X command\n";
constext txt_tinrc_tab_goto_next_unread[] = "# If ON the TAB command will goto next unread article at article viewer level\n";
constext txt_tinrc_thread_articles[] = "# Thread articles on 0=(nothing) 1=(Subject) 2=(References) 3=(Both).\n";
constext txt_tinrc_unlink_article[] = "# If ON remove ~/.article after posting.\n";
#if defined(NNTP_ABLE) || defined(NNTP_ONLY)
	constext txt_tinrc_use_builtin_inews[] = "# If ON use the builtin mini inews for posting via NNTP\n# otherwise use an external inews program\n";
#endif /* NNTP_ABLE || NNTP_ONLY */
#ifdef HAVE_COLOR
	constext txt_tinrc_use_color[] = "# If ON using ANSI-color\n";
#endif /* HAVE_COLOR */
#ifdef HAVE_KEYPAD
	constext txt_tinrc_use_keypad[] = "# If ON enable scroll keys on terminals that support it\n";
#endif /* HAVE_KEYPAD */
constext txt_tinrc_use_mailreader_i[] = "# Interactive mailreader: if ON mailreader will be invoked earlier for\n\
# reply so you can use more of its features (eg. MIME, pgp, ...)\n\
# this option has to suit default_mailer_format\n";
#ifdef HAVE_METAMAIL
	constext txt_tinrc_use_metamail[] = "# If ON metamail can/will be used to display MIME articles\n";
#endif /* HAVE_METAMAIL */
constext txt_tinrc_use_mouse[] = "# If ON enable mouse key support on xterm terminals\n";
constext txt_tinrc_wildcard[] = "# Wildcard matching 0=(wildmat) 1=(regex)\n";
#ifdef HAVE_COLOR
	constext txt_tinrc_word_h_display_marks[] = "# Should the leading and ending stars and dashes also be displayed,\n\
# even when they are highlighting marks?\n\
# 0 - no    1 - yes, display mark    2 - print a space instead\n";
	constext txt_tinrc_word_highlight[] = "# Enable word highlighting?\n";
#endif /* HAVE_COLOR */

/*
 * Special value used to override option-list behavior
 */
constext txt_default[] = "Default";

/*
 * OFF ~ FALSE, ON ~ TRUE
 */
constext *txt_onoff[] = { "OFF", "ON" };

/*
 * NB: All the following arrays must match corresponding ordering in tin.h
 * Threading types
 */
constext *txt_thread[] = { "None", "Subject", "References", "Both Subject and References" };

/*
 * Whether to use wildmat() or regexec() for matching strings
 */
constext *txt_wildcard[] = { "WILDMAT", "REGEX" };

/*
 * How the From: line is displayed.
 */
constext *txt_show_from[] = { "None", "Address", "Full Name", "Address and Name" };

#ifdef HAVE_COLOR
	/*
	 * Which colors can be used.
	 */
	constext *txt_colors[] = {
	txt_default,
	"Black",      "Red",        "Green",       "Brown",
	"Blue",       "Pink",       "Cyan",        "White",
	"Gray",       "Light Red",  "Light Green", "Yellow",
	"Light Blue", "Light Pink", "Light Cyan",  "Light White" };

	/*
	 * Which mark types can be used.
	 */
	constext *txt_marks[] = { "Nothing", "Mark", "Space" };
#endif /* HAVE_COLOR */

/*
 * MIME-Content-Transfer-Encodings.
 */
constext *txt_mime_encodings[] = { txt_8bit, txt_base64, txt_quoted_printable, txt_7bit };

/*
 * Array of possible post processing descriptions and short-keys
 * This must match the ordering of the defines in tin.h
 */
constext *txt_post_process[] = {
		"None",
		"Shell archive",
		"Uudecode",
#ifdef M_AMIGA
		"Uudecode & list lharc archive",
		"Uudecode & extract lharc archive",
#else
		"Uudecode & list zoo archive",
		"Uudecode & extract zoo archive",
#endif /* M_AMIGA */
		"Uudecode & list zip archive",
		"Uudecode & extract zip archive"
};

char ch_post_process[] = { 'n', 's', 'u', 'l', 'e', 'L', 'E' };

constext *txt_sort_type[] = {
		"Nothing",
		"Subject: field (descending)",
		"Subject: field (ascending)",
		"From: field (descending)",
		"From: field (ascending)",
		"Date: field (descending)",
		"Date: field (ascending)",
		"Score (descending)",
		"Score (ascending)"
};

/* Ways of handling bogus groups */
constext *txt_strip_bogus[] = {
		"Always Keep",
		"Always Remove",
		"Mark with D on selection screen"
};

#ifdef M_AMIGA
	constext txt_post_process_type[] = "Process n)one, s)har, u)ud, q)uit: ";
#else
	constext txt_post_process_type[] = "Process n)one, s)har, u)ud, l)ist zoo, e)xt zoo, L)ist zip, E)xt zip, q)uit: ";
#endif /* M_AMIGA */

constext txt_post_a_followup[] = "Post a followup...";
constext txt_post_an_article[] = "Post an article...";
constext txt_post_error_ask_postpone[]=
"An error has occurred while posting the article. If you think that this\n\
error is temporary or otherwise correctable, you can postpone the article\n\
and pick it up again with ^O later.\n";
constext txt_post_history_menu[] = "Posted articles history (page %d of %d)";
constext txt_post_newsgroup[] = "Post newsgroup> ";
constext txt_post_newsgroups[] = "Post to newsgroup(s) [%s]> ";
constext txt_post_processing[] = "Post processing...";
constext txt_post_processing_failed[] = "Post processing failed";
constext txt_post_processing_finished[] = "-- post processing completed --";
constext txt_post_subject[] = "Post subject [%s]> ";
constext txt_posting[] = "Posting article...";
constext txt_postpone_repost[] = "Post postponed articles [%.*s]? (y/Y/A/n/q): ";
constext txt_printed[] = "%d Article%s printed";
constext txt_printing[] = "Printing...";
constext txt_quick_filter_kill[] = "Add quick kill filter (y/n): ";
constext txt_quick_filter_select[] = "Add quick selection filter (y/n): ";
constext txt_quit[] = "Do you really want to quit? (y/n): ";
constext txt_quit_cancel[] = "e)dit cancel message, q)uit, d)elete [%.*s]: ";
constext txt_quit_no_write[] = "Do you really want to quit without saving your configuration? (y/n): ";

#ifdef HAVE_PGP
#	ifdef HAVE_ISPELL
		constext txt_quit_edit_post[] = "q)uit, e)dit, i)spell, g) pgp, p)ost, p(o)stpone: ";
		constext txt_quit_edit_send[] = "q)uit, e)dit, i)spell, g) pgp, s)end [%.*s]: ";
		constext txt_quit_edit_xpost[] = "q)uit, e)dit, i)spell, g) pgp, p)ost, p(o)stpone [%.*s]: ";
#	else
	constext txt_quit_edit_post[] = "q)uit, e)dit, g) pgp, p)ost, p(o)stpone: ";
	constext txt_quit_edit_send[] = "q)uit, e)dit, g) pgp, s)end [%.*s]: ";
	constext txt_quit_edit_xpost[] = "q)uit, e)dit, g) pgp, p)ost, p(o)stpone [%.*s]: ";
#	endif /* HAVE_ISPELL */
#else
#	ifdef HAVE_ISPELL
		constext txt_quit_edit_post[] = "q)uit, e)dit, i)spell, p)ost, p(o)stpone: ";
		constext txt_quit_edit_send[] = "q)uit, e)dit, i)spell, s)end [%.*s]: ";
		constext txt_quit_edit_xpost[] = "q)uit, e)dit, i)spell, p)ost, p(o)stpone [%.*s]: ";
#	else
		constext txt_quit_edit_post[] = "q)uit, e)dit, p)ost, p(o)stpone: ";
		constext txt_quit_edit_send[] = "q)uit, e)dit, s)end [%.*s]: ";
		constext txt_quit_edit_xpost[] = "q)uit, e)dit, p)ost, p(o)stpone [%.*s]: ";
#	endif /* HAVE_ISPELL */
#endif /* HAVE_PGP */
constext txt_quit_edit_postpone[] = "q)uit, e)dit, p(o)stpone: ";
constext txt_catchup_despite_tags[] = "You have tagged articles in this group - catchup anyway? (y/n): ";
constext txt_quit_despite_tags[] = "You have tagged articles in this group - quit anyway? (y/n): ";
constext txt_quoted_printable[] = "quoted-printable";
constext txt_read_abort[] = "Do you want to abort this operation? (y/n): ";
constext txt_read_exit[] = "Do you want to exit tin immediately ? (y/n): ";
constext txt_read_resp[] = "Read response> ";
constext txt_reading_article[] = "Reading ('q' to quit)... ";
constext txt_reading_arts[] = "Reading %sarticles...";
constext txt_reading_attributes_file[] = "Reading %sattributes file...";
constext txt_reading_config_file[] = "Reading %sconfig file...";
constext txt_reading_filter_file[] = "Reading %sfilter file...";
constext txt_reading_input_history_file[] = "Reading input history file...";
#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
	constext txt_reading_mail_active_file[] = "Reading mail active file... ";
	constext txt_reading_mailgroups_file[] = "Reading mailgroups file... ";
#endif /* !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING) */
constext txt_reading_groups[] = "Reading %s groups...";
constext txt_reading_news_active_file[] = "Reading groups from active file... ";
constext txt_reading_news_newsrc_file[] = "Reading groups from newsrc file... ";
constext txt_reading_newsgroups_file[] = "Reading newsgroups file... ";
constext txt_reading_newsrc[] = "Reading newsrc file...";
constext txt_reconnect_to_news_server[] = "Connection to news server has timed out. Reconnect? (y/n): ";
constext txt_remove_bogus[] = "Bogus group %s removed.";
constext txt_rename_error[] = "Error: rename %s to %s";
constext txt_reply_to_author[] = "Reply to author...";
constext txt_repost_an_article[] = "Reposting article...";
constext txt_repost_group[] = "Repost article(s) to group(s) [%s]> ";
constext txt_reset_newsrc[] = "Reset newsrc? (y/n): ";
constext txt_resp_redirect[] = "Responses have been directed to the following newsgroups";
constext txt_resp_to_poster[] = "Responses have been directed to poster. Mail/Post/Quit (m/p/q): ";
constext txt_resp_x_of_n[] = "RespNo %3d of %3d" cCRLF;
constext txt_at_s[] = " at %s";
constext txt_save_config[] = "Save configuration before continuing? (y/n): ";
constext txt_save_filename[] = "Save filename [%s]> ";
constext txt_saved[] = "-- %d Article%s saved --";
constext txt_saved_nothing[] = "-- No unread articles: nothing saved! --";
constext txt_saved_pattern_to[] = "-- Saved pattern to %s - %s --";
constext txt_saved_to_mailbox[] = "-- Saved to mailbox %s --";
constext txt_saving[] = "Saving...";
constext txt_screen_init_failed[] = "%s: Screen initialization failed";
constext txt_search_backwards[] = "Search backwards [%s]> ";
constext txt_search_body[] = "Search body [%s]> ";
constext txt_search_forwards[] = "Search forwards [%s]> ";
constext txt_searching[] = "Searching...";
constext txt_searching_body[] = "Searching article %d of %d ('q' to abort)...";
constext txt_select_art[] = "Select article> ";
constext txt_select_config_file_option[] = "Select option number before text or use arrow keys and <CR>. 'q' to quit.";
constext txt_select_group[] = "Select group> ";
constext txt_select_pattern[] = "Enter selection pattern [%s]> ";
constext txt_select_thread[] = "Select thread > ";
constext txt_server_name_in_file_env_var[] = "Put the server name in the file %s,\nor set the environment variable NNTPSERVER";
constext txt_shell_escape[] = "Enter shell command [%s]> ";
constext txt_skipping_newgroups[] = "Cannot move into new newsgroups. Subscribe first...";
constext txt_subscribed_num_groups[] = "subscribed to %d groups";
constext txt_subscribed_to[] = "Subscribed to %s";
constext txt_subscribing[] = "Subscribing... ";
constext txt_supersede_article[] = "Repost or supersede article(s) [%.*s]? (r/s/q): ";
constext txt_supersede_group[] = "Supersede article(s) to group(s) [%s]> ";
constext txt_suspended_message[] = "\nStopped. Type 'fg' to restart TIN\n";
constext txt_tagged_art[] = "Tagged article";
constext txt_tagged_thread[] = "Tagged thread";
constext txt_testing_archive[] = cCRLF cCRLF "Testing %s archive..." cCRLF;
constext txt_there_is_no_news[] = "There is no news\n";
constext txt_thread_com[] = "Thread Level Commands (page %d of %d)";
constext txt_thread_marked_as_deselected[] = "Thread deselected";
constext txt_thread_marked_as_selected[] = "Thread selected";
constext txt_thread_not_saved[] = "Thread not saved";
constext txt_thread_page[] = "T %d of %d (p %d), %s:  %s";
constext txt_thread_resp_page[] = "T %d of %d, R %d/%d (p %d), %s:  %s";
constext txt_thread_saved_to_many[] = "Thread saved to %s - %s";
constext txt_thread_x_of_n[] = "%sThread %4s of %4s" cCRLF;
constext txt_threading_arts[] = "Threading articles...";
#ifdef HAVE_COLOR
	constext txt_toggled_high[] = "Toggled word highlighting %s";
#endif /* HAVE_COLOR */
constext txt_toggled_rot13[] = "Toggled rot13 encoding";
constext txt_toggled_tex2iso[] = "Toggled german TeX encoding %s";
constext txt_type_h_for_help[] = "           h=help\n";
constext txt_unsubscribed_num_groups[] = "unsubscribed from %d groups";
constext txt_unsubscribed_to[] = "Unsubscribed from %s";
constext txt_unsubscribing[] = "Unsubscribing... ";
constext txt_untagged_art[] = "Untagged article";
constext txt_untagged_thread[] = "Untagged thread";
constext txt_unthreading_arts[] = "Unthreading articles...";
#ifdef HAVE_METAMAIL
	constext txt_use_mime[] = "Use MIME display program for this message? (y/n): ";
#endif /* HAVE_METAMAIL */
constext txt_uudecoding[] = "Uudecoding %s";
constext txt_value_out_of_range[] = "\n%s%d out of range (0 - %d). Reset to 0";
constext txt_warn_art_line_too_long[] = "\nWarning: posting exceeds %d columns. Line %d is the first long one:\n%-100s\n";
constext txt_warn_blank_subject[] = "\nWarning: \"Subject:\" contains only whitespaces.\n";
constext txt_warn_cancel[] = "Read carefully!\n\n\
  You are about to cancel an article seemingly written by you. This will wipe\n\
  the article from most  news servers  throughout the world,  but there is no\n\
  guarantee that it will work.\n\nThis is the article you are about to cancel:\n\n";
#ifdef FORGERY
	constext txt_warn_cancel_forgery[] = "Read carefully!\n\n\
  You are about to cancel an article seemingly not written by you.  This will\n\
  wipe the article from lots of news servers throughout the world;\n\
  Usenet's majority  considers this  rather inappropriate,  to say the least.\n\
  Only press 'd'  if you are  absolutely positive  that you are ready to take\n\
  the rap.\n\nThis is the article you are about to cancel:\n\n";
#endif /* FORGERY */
constext txt_warn_followup_to_several_groups[] = "\nWarning: Followup-To set to more than one newsgroup!\n";
constext txt_warn_missing_followup_to[] = "\nWarning: cross-posting to %d newsgroups and no Followup-To line!\n";
constext txt_warn_not_in_newsrc[] = "\nWarning: \"%s\" is not in your newsrc, it may be invalid at this site!\n";
constext txt_warn_not_valid_newsgroup[] = "\nWarning: \"%s\" is not a valid newsgroup at this site!\n";
constext txt_warn_suspicious_mail[] = "Warning: this mail address may contain a spamtrap. Continue or Abort? (c/a) ";
#ifndef NO_ETIQUETTE
	constext txt_warn_posting_etiquette[] = "\n\
  If your article contains quoted text  please take some time to pare it down\n\
  to just the  key points to which you are  responding,  or people will think\n\
  you are a dweeb!  Many people have the habit of skipping any article  whose\n\
  first page is largely  quoted material.  Format your article to fit in less\n\
  then 80 chars,  since that's the conventional size  (72 is a good choice as\n\
  it allows quoting without exceeding the limit).  If your lines are too long\n\
  they'll wrap  around  ugly and  people won't  read what you  write.  If you\n\
  aren't  careful  and considerate  in  formatting  your posting, people  are\n\
  likely to ignore it completely.  It's a crowded net out there.\n";
#endif /* !NO_ETIQUETTE */
constext txt_warn_sig_too_long[] ="\n\
Warning: Your signature  is longer than %d lines.  Since signatures usually do\n\
         not  transport any  useful information,  they should be as  short as\n\
         possible.\n";
constext txt_warn_multiple_sigs[] ="\nWarning: Found %d '-- \\n' lines, this may confuse some people.\n";
constext txt_warn_wrong_sig_format[] = "\nWarning: Signatures should start with '-- \\n' not with '--\\n'.\n";
constext txt_writing_attributes_file[] = "Writing attributes file...";
constext txt_x_resp[] = "%d Responses" cCRLF;
constext txt_yanking_all_groups[] = "Yanking in all groups...";
constext txt_yanking_sub_groups[] = "Yanking in subscribed to groups...";
constext txt_yes[] = "Yes";
constext txt_you_have_mail[] = "    You have mail\n";
constext txt_all_groups[] = "All groups";
constext txt_filter_text_type[] = "Apply pattern to    : ";
constext txt_from_line_only[] = "From: line (ignore case)        ";
constext txt_from_line_only_case[] = "From: line (case sensitive)     ";
constext txt_help_filter_from[] = "From: line to add to filter file. <SPACE> toggles & <CR> sets.";
constext txt_help_filter_lines[] = "Linecount of articles to be filtered. < for less, > for more, = for equal.";
constext txt_help_filter_msgid[] = "Message-Id: line to add to filter file. <SPACE> toggles & <CR> sets.";
constext txt_help_filter_subj[] = "Subject: line to add to filter file. <SPACE> toggles & <CR> sets.";
constext txt_help_filter_text[] = "Enter text pattern to filter if Subject: & From: lines are not what you want.";
constext txt_help_filter_text_type[] = "Select where text pattern should be applied. <SPACE> toggles & <CR> sets.";
constext txt_help_filter_time[] = "Expiration time in days for the entered filter. <SPACE> toggles & <CR> sets.";
constext txt_help_kill_scope[] = "Apply kill only to current group or all groups. <SPACE> toggles & <CR> sets.";
constext txt_help_select_scope[] = "Apply select to current group or all groups. <SPACE> toggles & <CR> sets.";
constext txt_kill_from[] = "Kill From:     [%-*.*s] (y/n): ";
constext txt_kill_lines[] = "Kill Lines: (</>num): ";
constext txt_kill_menu[] = "Kill Article Menu";
constext txt_kill_msgid[] = "Kill Msg-Id:   [%-*.*s] (f/l/o/n): ";
constext txt_kill_scope[] = "Kill pattern scope  : ";
constext txt_kill_subj[] = "Kill Subject:  [%-*.*s] (y/n): ";
constext txt_kill_text[] = "Kill text pattern   : ";
constext txt_kill_time[] = "Kill time in days   : ";
constext txt_msgid_line_only[] = "Message-Id: line";
constext txt_quit_edit_save_kill[] = "q)uit e)dit s)ave kill description: ";
constext txt_quit_edit_save_select[] = "q)uit e)dit s)ave select description: ";
constext txt_select_from[] = "Select From    [%-*.*s] (y/n): ";
constext txt_select_lines[] = "Select Lines: (</>num): ";
constext txt_select_menu[] = "Auto-select Article Menu";
constext txt_select_msgid[] = "Select Msg-Id  [%-*.*s] (f/l/o/n): ";
constext txt_select_scope[] = "Select pattern scope: ";
constext txt_select_subj[] = "Select Subject [%-*.*s] (y/n): ";
constext txt_select_text[] = "Select text pattern : ";
constext txt_select_time[] = "Select time in days   : ";
constext txt_subj_line_only[] = "Subject: line (ignore case)     ";
constext txt_subj_line_only_case[] = "Subject: line (case sensitive)  ";
constext txt_time_default_days[] = "%d days";
constext txt_unlimited_time[] = "Unlimited";
constext txt_full[] = "Full";
constext txt_last[] = "Last";
constext txt_only[] = "Only";
constext txt_filter_file[] = "# Global & local filter file for the TIN newsreader\n#\n\
# Global format:\n\
#   group=STRING      Newsgroups list (e.g. comp.*,!*sources*)    [mandatory]\n\
#   type=NUM          0=kill 1=auto-select (hot) [mandatory]\n\
#   case=NUM          Compare=0 / ignore=1 case when filtering\n\
#   score=NUM         Score to give (e.g. 70)\n\
#   subj=STRING       Subject: line (e.g. How to be a wizard)\n\
#   from=STRING       From: line (e.g. *Craig Shergold*)\n\
#   msgid=STRING      Message-ID: line (e.g. <123@ether.net>) with full references\n\
#   msgid_last=STRING Message-ID: line (e.g. <123@ether.net>) with last reference only\n\
#   msgid_only=STRING Message-ID: line (e.g. <123@ether.net>) without references\n\
#   refs_only=STRING  References: line (e.g. <123@ether.net>) without Message-Id:\n\
#   lines=[<>]?NUM    Lines: line\n\
#   gnksa=[<>]?NUM    GNKSA parse_from() return code\n\
# either:\n\
#   xref_max=NUM      Maximum score (e.g. 5)\n\
#   xref_score=NUM,PATTERN score for pattern (e.g 0,*.answers)\n\
#   ...\n\
# or:\n\
#   xref=PATTERN      Kill pattern (e.g. alt.flame*)\n\
#   \n\
#   time=NUM          Filter period in days (default %d)\n#\n";
constext txt_filter_score[] = "Enter score for rule (default=100): ";
constext txt_filter_score_help[] = "Enter the score weight (range 0 < score <= 10000)";

#ifdef INDEX_DAEMON
	constext txt_cannot_stat_group[] = "Can't stat group %s";
	constext txt_cannot_stat_index_file[] = "Can't stat %s index %s\n";
#else
	constext txt_art_deleted[] = "Article deleted.";
	constext txt_art_undeleted[] = "Article undeleted.";
	constext txt_processing_mail_arts[] = "Processing mail messages marked for deletion.";
	constext txt_processing_saved_arts[] = "Processing saved articles marked for deletion.";
#endif /* INDEX_DAEMON */

#ifdef M_AMIGA
	constext txt_env_var_not_found[] = "Environment variable %s not found. Set and retry...";
#endif /* M_AMIGA */

#ifdef HAVE_FASCIST_NEWSADMIN
	constext txt_error_followup_to_several_groups[] = "\nError: Followup-To set to more than one newsgroup!\n";
	constext txt_error_missing_followup_to[] = "\nError: cross-posting to %d newsgroups and no Followup-To line!\n";
	constext txt_error_not_valid_newsgroup[] = "\nError: \"%s\" is not a valid newsgroup!\n";
#endif /* HAVE_FASCIST_NEWSADMIN */

#ifdef XHDR_XREF
	constext txt_warn_xref_not_supported[] = "Your server does not have Xref: in its XOVER information.\n\
Tin will try to use XHDR XREF instead (slows down things a bit).\n";
#else
	constext txt_warn_xref_not_supported[] = "Your server does not have Xref: in its XOVER information.\n";
#endif /* XHDR_XREF */
