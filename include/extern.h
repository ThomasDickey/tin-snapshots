/*
 *  Project   : tin - a Usenet reader
 *  Module    : extern.h
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 26-04-95
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

/*
 * Library prototypes
 */

#if __STDC__ || defined(__cplusplus)
#define P_(s) s
#else
#define P_(s) ()
#endif

/*
 * The prototypes bracketed by DECL_xxxx ifdef's are used to get moderately
 * clean compiles on systems with pre-ANSI/POSIX headers when compiler warnings
 * are enabled.  (Not all of the functions are ANSI or POSIX).
 */
#ifdef DECL_BCOPY
extern int bcopy P_((char *, char *, int));
#endif
#ifdef DECL_BZERO
extern void bzero P_((char *, int));
#endif
#ifdef DECL_FCLOSE
extern int fclose P_((FILE *));
#endif
#ifdef DECL_FDOPEN
extern FILE *fdopen P_((int, const char *));
#endif
#ifdef DECL_FFLUSH
extern int fflush P_((FILE *));
#endif
#ifdef DECL_FPRINTF
extern int fprintf P_((FILE *, const char *, ...));
#endif
#ifdef DECL_FPUTC
extern int fputc P_((int, FILE *));
#endif
#ifdef DECL_FPUTS
extern int fputs P_((const char *, FILE *));
#endif
#ifdef DECL_FREAD
extern size_t fread P_((void *, size_t, size_t, FILE *));
#endif
#ifdef DECL_FSEEK
extern int fseek P_((FILE *, long, int));
#endif
#ifdef DECL_FWRITE
extern size_t fwrite P_((void *, size_t, size_t, FILE *));
#endif
#ifdef DECL_GETCWD
extern char *getcwd P_((char *, size_t));
#endif
#ifdef DECL_GETHOSTNAME
extern int gethostname P_((char *, int));
#endif
#ifdef DECL_GETLOGIN
extern char *getlogin P_((void));
#endif
#ifdef DECL_GETOPT
extern int getopt P_((int, char **, char *));
#endif
#ifdef DECL_GETHOSTBYNAME
extern struct hostent *gethostbyname P_((const char *));
#endif
#ifdef DECL_GETPWNAM
extern struct passwd *getpwnam P_((const char *));
#endif
#ifdef DECL_GETSERVBYNAME
extern struct servent *getservbyname P_((const char *, const char *));
#endif
#ifdef DECL_INET_ADDR
extern unsigned long inet_addr P_((const char *));
#endif
#ifdef DECL_INET_NTOA
extern char *inet_ntoa P_((struct in_addr));
#endif
#ifdef DECL_IOCTL
extern int ioctl P_((int, unsigned long, void *));
#endif
#ifdef DECL_MEMSET
extern void * memset P_((void *, int, size_t));
#endif
#ifdef DECL_MKTEMP
extern char * mktemp P_((char *));
#endif
#ifdef DECL_PCLOSE
extern int pclose P_((FILE *));
#endif
#ifdef DECL_PERROR
extern void perror P_((const char *));
#endif
#ifdef DECL_POPEN
extern FILE * popen P_((const char *, const char *));
#endif
#ifdef DECL_PRINTF
extern int printf P_((const char *, ...));
#endif
#ifdef DECL_REWIND
extern void rewind P_((FILE *));
#endif
#ifdef DECL_SELECT
extern int select P_((int, fd_set *, fd_set *, fd_set *, struct timeval *));
#endif
#ifdef DECL_SETEGID
extern int setegid P_((gid_t));
#endif
#ifdef DECL_SETEUID
extern int seteuid P_((uid_t));
#endif
#ifdef DECL_SETPGRP
#if SETPGRP_VOID
extern pid_t setpgrp P_((void));
#else
extern int setpgrp P_((int, int));
#endif
#endif /* DECL_SETPGRP */
#ifdef DECL_SETREGID
extern int setregid P_((gid_t, gid_t));
#endif
#ifdef DECL_SETREUID
extern int setreuid P_((uid_t, uid_t));
#endif
#ifdef DECL_SOCKET
extern int socket P_((int, int, int));
#endif
#ifdef DECL_SSCANF
extern int sscanf P_((const char *, const char *, ...));
#endif
#ifdef DECL_STRCASECMP
extern int strcasecmp P_((const char *, const char *));
#endif
#ifdef DECL_STRCHR
extern char *strchr P_((const char *, int));
extern char *strrchr P_((const char *, int));
#endif
#ifdef DECL_STRFTIME
extern int strftime P_((char *, int, char *, struct tm *));
#endif
#ifdef DECL_STRNCASECMP
extern int strncasecmp P_((const char *, const char *, size_t));
#endif
#ifdef DECL_STRTOL
extern long strtol P_((const char *, char **, int));
#endif
#ifdef DECL_SYSTEM
extern int system P_((char *));
#endif
#ifdef DECL_TGETENT
extern int tgetent P_((char *, char *));
#endif
#ifdef DECL_TGETFLAG
extern int tgetflag P_((char *));
#endif
#ifdef DECL_TGETNUM
extern int tgetnum P_((char *));
#endif
#ifdef DECL_TGETSTR
extern char *tgetstr P_((char *, char **));
#endif
#ifdef DECL_TGOTO
extern char *tgoto P_((char *, int, int));
#endif
#ifdef DECL_TIME
extern time_t time P_((time_t *));
#endif
#ifdef DECL_TOLOWER
extern int tolower P_((int));
#endif
#ifdef DECL_TOUPPER
extern int toupper P_((int));
#endif
#ifdef DECL_TPUTS
extern int tputs P_((char *, int, int (*)(int)));
#endif
#ifdef DECL__FLSBUF
extern int _flsbuf P_((int, FILE *));
#endif

#if !__STDC__ || defined(DECL_GETENV)
extern char *getenv P_((char *));
#endif

#ifdef DECL_ERRNO
extern int errno;
#endif

extern int optind;
extern char *optarg;

/*
 * Local variables
 */

extern FILE *note_fp;				/* body of current article */
extern char *glob_art_group;
extern char *glob_group;
extern char *glob_page_group;
extern char *help_group[];
extern char *help_page[];
extern char *help_select[];
extern char *help_thread[];
extern char *nntp_server;
extern char *pgpopts;
extern char active_times_file[PATH_LEN];
extern char add_addr[LEN];
extern char article[PATH_LEN];
extern char bug_addr[LEN];
extern char ch_post_process[];
extern char cmd_line_printer[PATH_LEN];
extern char cmdline_nntpserver[PATH_LEN];
extern char cvers[LEN];
extern char dead_article[PATH_LEN];
extern char dead_articles[PATH_LEN];
extern char default_art_search[LEN];
extern char default_author_search[LEN];
extern char default_editor_format[PATH_LEN];
extern char default_goto_group[LEN];
extern char default_group_search[LEN];
extern char default_mail_address[LEN];
extern char default_maildir[PATH_LEN];
extern char default_mailer_format[PATH_LEN];
extern char default_organization[PATH_LEN];
extern char default_pipe_command[LEN];
extern char default_post_newsgroups[PATH_LEN];
extern char default_post_subject[PATH_LEN];
extern char default_printer[LEN];
extern char default_range_group[PATH_LEN];
extern char default_range_select[PATH_LEN];
extern char default_range_thread[PATH_LEN];
extern char default_regex_pattern[LEN];
extern char default_repost_group[LEN];
extern char default_save_file[PATH_LEN];
extern char default_savedir[PATH_LEN];
extern char default_select_pattern[LEN];
extern char default_shell_command[LEN];
extern char default_sigfile[PATH_LEN];
extern char default_signature[PATH_LEN];
extern char default_subject_search[LEN];
extern char error_response[];
extern char global_attributes_file[PATH_LEN];
extern char global_config_file[PATH_LEN];
extern char global_filter_file[PATH_LEN];
extern char group_times_file[PATH_LEN];
extern char homedir[PATH_LEN];
extern char index_maildir[PATH_LEN];
extern char index_newsdir[PATH_LEN];
extern char index_savedir[PATH_LEN];
extern char last_put[];
extern char libdir[PATH_LEN];
extern char local_attributes_file[PATH_LEN];
extern char local_config_file[PATH_LEN];
extern char local_filter_file[PATH_LEN];
extern char local_newsgroups_file[PATH_LEN];
extern char local_newsrctable_file[PATH_LEN];
extern char lock_file[PATH_LEN];
extern char mail_active_file[PATH_LEN];
extern char mail_news_user[LEN];
extern char mail_quote_format[PATH_LEN];
extern char mailbox[PATH_LEN];
extern char mailer[PATH_LEN];
extern char mailgroups_file[PATH_LEN];
extern char mm_charset[LEN];
extern char motd_file[PATH_LEN];
extern char motd_file_info[PATH_LEN];
extern char msg[LEN];
extern char msg_headers_file[PATH_LEN];
extern char my_distribution[LEN];
extern char new_newnews_host[PATH_LEN];
extern char newnewsrc[PATH_LEN];
extern char news_active_file[PATH_LEN];
extern char news_quote_format[PATH_LEN];
extern char newsgroups_file[PATH_LEN];
extern char newsrc[PATH_LEN];
extern char note_h_contentenc[HEADER_LEN];		/* Content-Transfer-Encoding: */
extern char note_h_contenttype[HEADER_LEN];		/* Content-Type: */
extern char note_h_date[HEADER_LEN];			/* Date:	*/
extern char note_h_distrib[HEADER_LEN];			/* Distribution: */
extern char note_h_followup[HEADER_LEN];		/* Followup-To: */
extern char note_h_from[HEADER_LEN];			/* From: */
extern char note_h_keywords[HEADER_LEN];		/* Keywords: */
extern char note_h_messageid[HEADER_LEN];		/* Message-ID:	*/
extern char note_h_mimeversion[HEADER_LEN];		/* Mime-Version: */
extern char note_h_newsgroups[HEADER_LEN];		/* Newsgroups:	*/
extern char note_h_org[HEADER_LEN];			/* Organization: */
extern char note_h_path[HEADER_LEN];			/* Path:	*/
extern char note_h_references[HEADER_LEN];		/* References:	*/
extern char note_h_subj[HEADER_LEN];			/* Subject:	*/
extern char note_h_summary[HEADER_LEN];			/* Summary: */
extern char novrootdir[PATH_LEN];
extern char page_header[LEN];
extern char pgp_data[PATH_LEN];
extern char post_proc_command[PATH_LEN];
extern char posted_info_file[PATH_LEN];
extern char posted_msgs_file[PATH_LEN];
extern char proc_ch_default;
extern char progname[PATH_LEN];
extern char quote_chars[PATH_LEN];
extern char rcdir[PATH_LEN];
extern char reply_to[LEN];
extern char save_active_file[PATH_LEN];
extern char spooldir[PATH_LEN];
extern char subscriptions_file[PATH_LEN];
extern char txt_help_bug_report[LEN];
extern char userid[PATH_LEN];
extern char xpost_quote_format[PATH_LEN];
extern const char base64_alphabet[64];
extern constext *txt_colors[];
extern constext *txt_mime_types[NUM_MIME_TYPES];
extern constext *txt_onoff[];
extern constext *txt_post_process[];
extern constext *txt_show_from[];
extern constext *txt_sort_type[];
extern constext *txt_thread[];
extern constext txt_1_resp[];
extern constext txt_7bit[];
extern constext txt_8bit[];
extern constext txt_abort_indexing[];
extern constext txt_abort_searching[];
extern constext txt_active_file_is_empty[];
extern constext txt_added_groups[];
extern constext txt_all_groups[];
extern constext txt_append_overwrite_quit[];
extern constext txt_art_cancel[];
extern constext txt_art_cannot_cancel[];
extern constext txt_art_marked_as_unread[];
extern constext txt_art_newsgroups[];
extern constext txt_art_not_posted[];
extern constext txt_art_not_saved[];
extern constext txt_art_pager_com[];
extern constext txt_art_posted[];
extern constext txt_art_rejected[];
extern constext txt_art_saved_to[];
extern constext txt_art_thread_regex_tag[];
extern constext txt_art_unavailable[];
extern constext txt_article_cancelled[];
extern constext txt_article_reposted1[];
extern constext txt_article_reposted2a[];
extern constext txt_article_reposted2b[];
extern constext txt_auth_failed[];
extern constext txt_auth_pass_needed[];
extern constext txt_auth_user_needed[];
extern constext txt_author_search_backwards[];
extern constext txt_author_search_forwards[];
extern constext txt_authorization_ok[];
extern constext txt_autosubscribing_groups[];
extern constext txt_bad_active_file[];
extern constext txt_bad_article[];
extern constext txt_bad_command[];
extern constext txt_base64[];
extern constext txt_begin_of_art[];
extern constext txt_cancel_article[];
extern constext txt_cancelling_art[];
extern constext txt_cannot_find_base_art[];
extern constext txt_cannot_get_nntp_server_name[];
extern constext txt_cannot_get_term[];
extern constext txt_cannot_get_term_entry[];
extern constext txt_cannot_open[];
extern constext txt_cannot_open_active_file[];
extern constext txt_cannot_post[];
extern constext txt_cannot_stat_group[];
extern constext txt_cannot_stat_index_file[];
extern constext txt_cannot_write_index[];
extern constext txt_catchup_all_read_groups[];
extern constext txt_catchup_despite_tags[];
extern constext txt_catchup_update_info[];
extern constext txt_check_article[];
extern constext txt_checking_active_file[];
extern constext txt_checking_for_news[];
extern constext txt_checksum_of_file[];
extern constext txt_cmdline_hit_any_key[];
extern constext txt_color_off[];
extern constext txt_color_on[];
extern constext txt_command_failed_s[];
extern constext txt_connecting[];
extern constext txt_connecting_port[];
extern constext txt_connection_to_server_broken[];
extern constext txt_continue[];
extern constext txt_copyright_notice[];
extern constext txt_creating_active[];
extern constext txt_creating_newsrc[];
extern constext txt_delete_processed_files[];
extern constext txt_deleting[];
extern constext txt_end_of_arts[];
extern constext txt_end_of_groups[];
extern constext txt_end_of_thread[];
extern constext txt_enter_option_num[];
extern constext txt_enter_range[];
extern constext txt_env_var_not_found[];
extern constext txt_error_followup_to_several_groups[];
extern constext txt_error_from_in_header_not_allowed[];
extern constext txt_error_header_and_body_not_separate[];
extern constext txt_error_header_line_blank[];
extern constext txt_error_header_line_colon[];
extern constext txt_error_header_line_comma[];
extern constext txt_error_header_line_empty_newsgroups[];
extern constext txt_error_header_line_missing_newsgroups[];
extern constext txt_error_header_line_missing_subject[];
extern constext txt_error_header_line_missing_target[];
extern constext txt_error_header_line_space[];
extern constext txt_error_missing_followup_to[];
extern constext txt_error_no_enter_permission[];
extern constext txt_error_no_read_permission[];
extern constext txt_error_no_such_file[];
extern constext txt_error_no_write_permission[];
extern constext txt_error_not_valid_newsgroup[];
extern constext txt_external_mail_done[];
extern constext txt_extracting_archive[];
extern constext txt_extracting_shar[];
extern constext txt_failed_to_connect_to_server[];
extern constext txt_feed_pattern[];
extern constext txt_filesystem_full[];
extern constext txt_filesystem_full_backup[];
extern constext txt_filter_global_rules[];
extern constext txt_filter_local_rules[];
extern constext txt_filter_text_type[];
extern constext txt_from_line_only[];
extern constext txt_from_line_only_case[];
extern constext txt_group[];
extern constext txt_group_is_moderated[];
extern constext txt_group_select_com[];
extern constext txt_group_selection[];
extern constext txt_help_B[];
extern constext txt_help_D[];
extern constext txt_help_I[];
extern constext txt_help_K[];
extern constext txt_help_M[];
extern constext txt_help_S[];
extern constext txt_help_T[];
extern constext txt_help_U[];
extern constext txt_help_W[];
extern constext txt_help_X[];
extern constext txt_help_a[];
extern constext txt_help_art_marked_deleted[];
extern constext txt_help_art_marked_inrange[];
extern constext txt_help_art_marked_return[];
extern constext txt_help_art_marked_selected[];
extern constext txt_help_art_marked_unread[];
extern constext txt_help_auto_bcc[];
extern constext txt_help_auto_cc[];
extern constext txt_help_auto_list_thread[];
extern constext txt_help_auto_save[];
extern constext txt_help_b[];
extern constext txt_help_batch_save[];
extern constext txt_help_beginner_level[];
extern constext txt_help_bug[];
extern constext txt_help_cC[];
extern constext txt_help_c[];
extern constext txt_help_catchup_read_groups[];
extern constext txt_help_ck[];
extern constext txt_help_colon[];
extern constext txt_help_confirm_action[];
extern constext txt_help_confirm_to_quit[];
extern constext txt_help_cr[];
extern constext txt_help_ctrl_a[];
extern constext txt_help_ctrl_d[];
extern constext txt_help_ctrl_f[];
extern constext txt_help_ctrl_g[];
extern constext txt_help_ctrl_h[];
extern constext txt_help_ctrl_k[];
extern constext txt_help_ctrl_l[];
extern constext txt_help_d[];
extern constext txt_help_dash[];
extern constext txt_help_draw_arrow_mark[];
extern constext txt_help_e[];
extern constext txt_help_editor_format[];
extern constext txt_help_equal[];
extern constext txt_help_esc[];
extern constext txt_help_filter_days[];
extern constext txt_help_filter_from[];
extern constext txt_help_filter_lines[];
extern constext txt_help_filter_msgid[];
extern constext txt_help_filter_subj[];
extern constext txt_help_filter_text[];
extern constext txt_help_filter_text_type[];
extern constext txt_help_filter_time[];
extern constext txt_help_force_screen_redraw[];
extern constext txt_help_full_page_scroll[];
extern constext txt_help_g[];
extern constext txt_help_g_c[];
extern constext txt_help_g_caret_dollar[];
extern constext txt_help_g_cr[];
extern constext txt_help_g_ctrl_r[];
extern constext txt_help_g_d[];
extern constext txt_help_g_hash[];
extern constext txt_help_g_l[];
extern constext txt_help_g_num[];
extern constext txt_help_g_q[];
extern constext txt_help_g_r[];
extern constext txt_help_g_search[];
extern constext txt_help_g_t_p_i[];
extern constext txt_help_g_tab[];
extern constext txt_help_g_x[];
extern constext txt_help_g_y[];
extern constext txt_help_g_z[];
extern constext txt_help_group_catchup_on_exit[];
extern constext txt_help_groupname_max_length[];
extern constext txt_help_h[];
extern constext txt_help_hash[];
extern constext txt_help_highlight_xcommentto[];
extern constext txt_help_i[];
extern constext txt_help_i_caret_dollar[];
extern constext txt_help_i_coma[];
extern constext txt_help_i_cr[];
extern constext txt_help_i_dot[];
extern constext txt_help_i_n[];
extern constext txt_help_i_num[];
extern constext txt_help_i_p[];
extern constext txt_help_i_search[];
extern constext txt_help_i_star[];
extern constext txt_help_i_tab[];
extern constext txt_help_i_tilda[];
extern constext txt_help_inverse_okay[];
extern constext txt_help_j[];
extern constext txt_help_keep_posted_articles[];
extern constext txt_help_kill_scope[];
extern constext txt_help_l[];
extern constext txt_help_m[];
extern constext txt_help_mail_8bit_header[];
extern constext txt_help_mail_mime_encoding[];
extern constext txt_help_mail_quote_format[];
extern constext txt_help_maildir[];
extern constext txt_help_mailer_format[];
extern constext txt_help_mark_saved_read[];
extern constext txt_help_mm_charset[];
extern constext txt_help_n[];
extern constext txt_help_news_quote_format[];
extern constext txt_help_no_advertising[];
extern constext txt_help_o[];
extern constext txt_help_p_S[];
extern constext txt_help_p_caret_dollar[];
extern constext txt_help_p_coma[];
extern constext txt_help_p_cr[];
extern constext txt_help_p_d[];
extern constext txt_help_p_dot[];
extern constext txt_help_p_f[];
extern constext txt_help_p_g[];
extern constext txt_help_p_k[];
extern constext txt_help_p_m[];
extern constext txt_help_p_n[];
extern constext txt_help_p_num[];
extern constext txt_help_p_p[];
extern constext txt_help_p_r[];
extern constext txt_help_p_s[];
extern constext txt_help_p_search[];
extern constext txt_help_p_star[];
extern constext txt_help_p_tab[];
extern constext txt_help_p_tilda[];
extern constext txt_help_p_z[];
extern constext txt_help_pipe[];
extern constext txt_help_plus[];
extern constext txt_help_pos_first_unread[];
extern constext txt_help_post_8bit_header[];
extern constext txt_help_post_mime_encoding[];
extern constext txt_help_post_proc_type[];
extern constext txt_help_print_header[];
extern constext txt_help_printer[];
extern constext txt_help_process_only_unread[];
extern constext txt_help_q[];
extern constext txt_help_quick_kill[];
extern constext txt_help_quick_select[];
extern constext txt_help_quote_chars[];
extern constext txt_help_r[];
extern constext txt_help_reread_active_file_secs[];
extern constext txt_help_s[];
extern constext txt_help_s_i[];
extern constext txt_help_save_to_mmdf_mailbox[];
extern constext txt_help_savedir[];
extern constext txt_help_sel_c[];
extern constext txt_help_select_scope[];
extern constext txt_help_semicolon[];
extern constext txt_help_show_author[];
extern constext txt_help_show_description[];
extern constext txt_help_show_last_line_prev_page[];
extern constext txt_help_show_lines[];
extern constext txt_help_show_only_unread[];
extern constext txt_help_show_only_unread_groups[];
extern constext txt_help_show_xcommentto[];
extern constext txt_help_sigdashes[];
extern constext txt_help_sigfile[];
extern constext txt_help_sort_art_type[];
extern constext txt_help_space_goto_next_unread[];
extern constext txt_help_space_toggles[];
extern constext txt_help_start_editor_offset[];
extern constext txt_help_strip_blanks[];
extern constext txt_help_t[];
extern constext txt_help_t_K[];
extern constext txt_help_t_caret_dollar[];
extern constext txt_help_t_cr[];
extern constext txt_help_t_num[];
extern constext txt_help_t_tab[];
extern constext txt_help_tab_after_X_selection[];
extern constext txt_help_tab_goto_next_unread[];
extern constext txt_help_tex[];
extern constext txt_help_thread[];
extern constext txt_help_thread_arts[];
extern constext txt_help_thread_catchup_on_exit[];
extern constext txt_help_u[];
extern constext txt_help_unlink_article[];
extern constext txt_help_use_builtin_inews[];
extern constext txt_help_use_color_tinrc[];
extern constext txt_help_use_mailreader_i[];
extern constext txt_help_use_mouse[];
extern constext txt_help_v[];
extern constext txt_help_w[];
extern constext txt_help_x[];
extern constext txt_help_xpost_quote_format[];
extern constext txt_help_y[];
extern constext txt_hit_any_key[];
extern constext txt_hit_space_for_more[];
extern constext txt_index_page_com[];
extern constext txt_invalid_from1[];
extern constext txt_invalid_from2[];
extern constext txt_inverse_off[];
extern constext txt_inverse_on[];
extern constext txt_kill_from[];
extern constext txt_kill_lines[];
extern constext txt_kill_menu[];
extern constext txt_kill_msgid[];
extern constext txt_kill_scope[];
extern constext txt_kill_subj[];
extern constext txt_kill_text[];
extern constext txt_kill_time[];
extern constext txt_last_resp[];
extern constext txt_lines[];
extern constext txt_listing_archive[];
extern constext txt_mail_art_to[];
extern constext txt_mail_bug_report[];
extern constext txt_mail_bug_report_confirm[];
extern constext txt_mail_quote[];
extern constext txt_mailed[];
extern constext txt_mailing_to[];
extern constext txt_mark_all_read[];
extern constext txt_mark_group_read[];
extern constext txt_mark_thread_read[];
extern constext txt_matching_cmd_line_groups[];
extern constext txt_mini_group_1[];
extern constext txt_mini_group_2[];
extern constext txt_mini_group_3[];
extern constext txt_mini_page_1[];
extern constext txt_mini_page_2[];
extern constext txt_mini_page_3[];
extern constext txt_mini_select_1[];
extern constext txt_mini_select_2[];
extern constext txt_mini_select_3[];
extern constext txt_mini_spooldir_1[];
extern constext txt_mini_thread_1[];
extern constext txt_mini_thread_2[];
extern constext txt_more[];
extern constext txt_moving[];
extern constext txt_msgid_line_only[];
extern constext txt_news_quote[];
extern constext txt_newsgroup[];
extern constext txt_newsgroup_position[];
extern constext txt_next_resp[];
extern constext txt_nntp_authorization_failed[];
extern constext txt_nntp_to_fp_cannot_reopen[];
extern constext txt_no[];
extern constext txt_no_arts[];
extern constext txt_no_arts_posted[];
extern constext txt_no_command[];
extern constext txt_no_filename[];
extern constext txt_no_group[];
extern constext txt_no_groups[];
extern constext txt_no_groups_to_read[];
extern constext txt_no_groups_to_yank_in[];
extern constext txt_no_last_message[];
extern constext txt_no_mail_address[];
extern constext txt_no_match[];
extern constext txt_no_more_groups[];
extern constext txt_no_next_unread_art[];
extern constext txt_no_prev_group[];
extern constext txt_no_prev_unread_art[];
extern constext txt_no_quick_newsgroups[];
extern constext txt_no_quick_subject[];
extern constext txt_no_resp[];
extern constext txt_no_responses[];
extern constext txt_no_resps_in_thread[];
extern constext txt_no_search_string[];
extern constext txt_no_subject[];
extern constext txt_no_tagged_arts_to_save[];
extern constext txt_no_term_clear_eol[];
extern constext txt_no_term_clear_eos[];
extern constext txt_no_term_clearscreen[];
extern constext txt_no_term_cursor_motion[];
extern constext txt_no_term_set[];
extern constext txt_not_exist[];
extern constext txt_not_in_active_file[];
extern constext txt_nrctbl_create[];
extern constext txt_nrctbl_default[];
extern constext txt_opt_art_marked_deleted[];
extern constext txt_opt_art_marked_inrange[];
extern constext txt_opt_art_marked_return[];
extern constext txt_opt_art_marked_selected[];
extern constext txt_opt_art_marked_unread[];
extern constext txt_opt_auto_bcc[];
extern constext txt_opt_auto_cc[];
extern constext txt_opt_auto_list_thread[];
extern constext txt_opt_auto_save[];
extern constext txt_opt_batch_save[];
extern constext txt_opt_beginner_level[];
extern constext txt_opt_catchup_read_groups[];
extern constext txt_opt_confirm_action[];
extern constext txt_opt_confirm_to_quit[];
extern constext txt_opt_draw_arrow_mark[];
extern constext txt_opt_editor_format[];
extern constext txt_opt_filter_days[];
extern constext txt_opt_force_screen_redraw[];
extern constext txt_opt_full_page_scroll[];
extern constext txt_opt_group_catchup_on_exit[];
extern constext txt_opt_groupname_max_length[];
extern constext txt_opt_highlight_xcommentto[];
extern constext txt_opt_inverse_okay[];
extern constext txt_opt_keep_posted_articles[];
extern constext txt_opt_mail_8bit_header[];
extern constext txt_opt_mail_mime_encoding[];
extern constext txt_opt_mail_quote_format[];
extern constext txt_opt_maildir[];
extern constext txt_opt_mailer_format[];
extern constext txt_opt_mark_saved_read[];
extern constext txt_opt_mm_charset[];
extern constext txt_opt_news_quote_format[];
extern constext txt_opt_no_advertising[];
extern constext txt_opt_pos_first_unread[];
extern constext txt_opt_post_8bit_header[];
extern constext txt_opt_post_mime_encoding[];
extern constext txt_opt_post_proc_type[];
extern constext txt_opt_print_header[];
extern constext txt_opt_printer[];
extern constext txt_opt_process_only_unread[];
extern constext txt_opt_quote_chars[];
extern constext txt_opt_reread_active_file_secs[];
extern constext txt_opt_save_to_mmdf_mailbox[];
extern constext txt_opt_savedir[];
extern constext txt_opt_show_author[];
extern constext txt_opt_show_description[];
extern constext txt_opt_show_last_line_prev_page[];
extern constext txt_opt_show_lines[];
extern constext txt_opt_show_only_unread[];
extern constext txt_opt_show_only_unread_groups[];
extern constext txt_opt_show_xcommentto[];
extern constext txt_opt_sigdashes[];
extern constext txt_opt_sigfile[];
extern constext txt_opt_sort_art_type[];
extern constext txt_opt_space_goto_next_unread[];
extern constext txt_opt_start_editor_offset[];
extern constext txt_opt_strip_blanks[];
extern constext txt_opt_tab_after_X_selection[];
extern constext txt_opt_tab_goto_next_unread[];
extern constext txt_opt_thread_arts[];
extern constext txt_opt_thread_catchup_on_exit[];
extern constext txt_opt_unlink_article[];
extern constext txt_opt_use_builtin_inews[];
extern constext txt_opt_use_color_tinrc[];
extern constext txt_opt_use_mailreader_i[];
extern constext txt_opt_use_mouse[];
extern constext txt_opt_xpost_quote_format[];
extern constext txt_option_not_enabled[];
extern constext txt_options_menu[];
extern constext txt_out_of_memory2[];
extern constext txt_out_of_memory[];
extern constext txt_pgp_mail[];
extern constext txt_pgp_news[];
extern constext txt_pgp_not_avail[];
extern constext txt_pgp_nothing[];
extern constext txt_pipe_to_command[];
extern constext txt_piping[];
extern constext txt_piping_not_enabled[];
extern constext txt_plural[];
extern constext txt_post_a_followup[];
extern constext txt_post_an_article[];
extern constext txt_post_history_menu[];
extern constext txt_post_newsgroup[];
extern constext txt_post_newsgroups[];
extern constext txt_post_process_type[];
extern constext txt_post_processing[];
extern constext txt_post_processing_failed[];
extern constext txt_post_processing_finished[];
extern constext txt_post_subject[];
extern constext txt_posting[];
extern constext txt_printed[];
extern constext txt_printing[];
extern constext txt_quick_filter_kill[];
extern constext txt_quick_filter_select[];
extern constext txt_quit[];
extern constext txt_quit_cancel[];
extern constext txt_quit_despite_tags[];
extern constext txt_quit_edit_post[];
extern constext txt_quit_edit_save_kill[];
extern constext txt_quit_edit_save_select[];
extern constext txt_quit_edit_send[];
extern constext txt_quit_edit_xpost[];
extern constext txt_quit_no_write[];
extern constext txt_quoted_printable[];
extern constext txt_read_resp[];
extern constext txt_reading_all_arts[];
extern constext txt_reading_all_groups[];
extern constext txt_reading_article[];
extern constext txt_reading_attributes_file[];
extern constext txt_reading_config_file[];
extern constext txt_reading_filter_file[];
extern constext txt_reading_global_attributes_file[];
extern constext txt_reading_global_config_file[];
extern constext txt_reading_global_filter_file[];
extern constext txt_reading_mail_active_file[];
extern constext txt_reading_mailgroups_file[];
extern constext txt_reading_new_arts[];
extern constext txt_reading_new_groups[];
extern constext txt_reading_news_active_file[];
extern constext txt_reading_news_newsrc_file[];
extern constext txt_reading_newsgroups_file[];
extern constext txt_reading_newsrc[];
extern constext txt_reconnect_to_news_server[];
extern constext txt_rename_error[];
extern constext txt_reply_to_author[];
extern constext txt_repost_an_article[];
extern constext txt_repost_group[];
extern constext txt_rereading_active_file[];
extern constext txt_reset_newsrc[];
extern constext txt_resp_redirect[];
extern constext txt_resp_to_poster[];
extern constext txt_resp_x_of_n[];
extern constext txt_s_at_s[];
extern constext txt_save_filename[];
extern constext txt_saved[];
extern constext txt_saved_nothing[];
extern constext txt_saved_pattern_to[];
extern constext txt_saved_to_mailbox[];
extern constext txt_saving[];
extern constext txt_screen_init_failed[];
extern constext txt_search_backwards[];
extern constext txt_search_body[];
extern constext txt_search_forwards[];
extern constext txt_searching[];
extern constext txt_searching_body[];
extern constext txt_select_art[];
extern constext txt_select_config_file_option[];
extern constext txt_select_from[];
extern constext txt_select_group[];
extern constext txt_select_lines[];
extern constext txt_select_menu[];
extern constext txt_select_msgid[];
extern constext txt_select_pattern[];
extern constext txt_select_scope[];
extern constext txt_select_subj[];
extern constext txt_select_text[];
extern constext txt_select_thread[];
extern constext txt_select_time[];
extern constext txt_server_name_in_file_env_var[];
extern constext txt_shell_escape[];
extern constext txt_skipping_newsgroups[];
extern constext txt_stuff_nntp_cannot_open[];
extern constext txt_subj_line_only[];
extern constext txt_subj_line_only_case[];
extern constext txt_subscribe_pattern[];
extern constext txt_subscribe_to_new_group[];
extern constext txt_subscribed_num_groups[];
extern constext txt_subscribed_to[];
extern constext txt_subscribing[];
extern constext txt_subscribing_to[];
extern constext txt_supersede_article[];
extern constext txt_supersede_group[];
extern constext txt_superseding_art[];
extern constext txt_suspended_message[];
extern constext txt_tagged_art[];
extern constext txt_tagged_thread[];
extern constext txt_testing_archive[];
extern constext txt_there_is_no_news[];
extern constext txt_thread_com[];
extern constext txt_thread_marked_as_deselected[];
extern constext txt_thread_marked_as_selected[];
extern constext txt_thread_marked_as_unread[];
extern constext txt_thread_not_saved[];
extern constext txt_thread_page[];
extern constext txt_thread_resp_page[];
extern constext txt_thread_saved_to_many[];
extern constext txt_thread_x_of_n[];
extern constext txt_threading_arts[];
extern constext txt_time_default_days[];
extern constext txt_toggled_rot13[];
extern constext txt_toggled_tex2iso_off[];
extern constext txt_toggled_tex2iso_on[];
extern constext txt_type_h_for_help[];
extern constext txt_unlimited_time[];
extern constext txt_unsubscribe_pattern[];
extern constext txt_unsubscribed_num_groups[];
extern constext txt_unsubscribed_to[];
extern constext txt_unsubscribing[];
extern constext txt_untagged_art[];
extern constext txt_untagged_thread[];
extern constext txt_unthreading_arts[];
extern constext txt_use_mime[];
extern constext txt_uudecoding[];
extern constext txt_value_out_of_range[];
extern constext txt_warn_art_line_too_long[];
extern constext txt_warn_cancel[];
extern constext txt_warn_cancel_forgery[];
extern constext txt_warn_difficulties[];
extern constext txt_warn_followup_to_several_groups[];
extern constext txt_warn_missing_followup_to[];
extern constext txt_warn_not_valid_newsgroup[];
extern constext txt_warn_posting_etiquette[];
extern constext txt_writing_attributes_file[];
extern constext txt_x_resp[];
extern constext txt_xpost_quote[];
extern constext txt_yanking_all_groups[];
extern constext txt_yanking_sub_groups[];
extern constext txt_yes[];
extern constext txt_you_have_mail[];
extern int *my_group;
extern int MORE_POS;
extern int NOTESLINES;
extern int RIGHT_POS;
extern int _hp_glitch;
extern int actual_option_page;
extern int art_marked_deleted;
extern int art_marked_inrange;
extern int art_marked_return;
extern int art_marked_selected;
extern int art_marked_unread;
extern int auto_bcc;
extern int auto_cc;
extern int auto_list_thread;
extern int beginner_level;
extern int cCOLS;
extern int cLINES;
extern int can_post;
extern int catchup;
extern int catchup_read_groups;
extern int check_any_unread;
extern int check_for_new_newsgroups;
extern int cmd_line;
extern int compiled_with_nntp;
extern int confirm_action;
extern int confirm_to_quit;
extern int count_articles;
extern int created_rcdir;
extern int cur_groupnum;
extern int debug;
extern int default_auto_save;
extern int default_auto_save_msg;
extern int default_batch_save;
extern int default_filter_days;
extern int default_filter_kill_case;
extern int default_filter_kill_expire;
extern int default_filter_kill_global;
extern int default_filter_kill_header;
extern int default_filter_select_case;
extern int default_filter_select_expire;
extern int default_filter_select_global;
extern int default_filter_select_header;
extern int default_move_group;
extern int default_post_proc_type;
extern int default_save_mode;
extern int default_show_author;
extern int default_show_only_unread;
extern int default_sort_art_type;
extern int default_thread_arts;
extern int delete_index_file;
extern int draw_arrow_mark;
extern int first_group_on_screen;
extern int first_subj_on_screen;
extern int force_screen_redraw;
extern int full_page_scroll;
extern int glob_respnum;
extern int global_filtered_articles;
extern int got_sig_pipe;
extern int group_catchup_on_exit;
extern int group_hash[TABLE_SIZE];
extern int group_top;
extern int groupname_len;
extern int groupname_max_length;
extern int highlight_xcommentto;
extern int index_point;
extern int inverse_okay;
extern int iso2asc_supported;
extern int keep_dead_articles; /* keep all dead articles in dead.articles */
extern int keep_posted_articles; /* keep all posted articles in ~/Mail/posted */
extern int last_group_on_screen;
extern int last_resp;
extern int last_subj_on_screen;
extern int local_filtered_articles;
extern int local_index;
extern int mail_8bit_header;
extern int mail_mime_encoding;
extern int mail_news;
extern int mark_saved_read;
extern int max_active;
extern int max_art;
extern int max_from;
extern int max_kill;
extern int max_newnews;
extern int max_save;
extern int max_spooldir;
extern int max_subj;
extern int newsrc_active;
extern int nntp_codeno;
extern int no_advertising;
extern int note_end;				/* end of article ? */
extern int note_line;
extern int note_page;				/* what page we're on */
extern int num_active;
extern int num_kill;
extern int num_newnews;
extern int num_of_killed_arts;
extern int num_of_selected_arts;
extern int num_of_tagged_arts;
extern int num_save;
extern int num_spooldir;
extern int pos_first_unread;
extern int post_8bit_header;
extern int post_article_and_exit;
extern int post_mime_encoding;
extern int print_header;
extern int process_id;
extern int process_only_unread;
extern int purge_index_files;
extern int read_local_newsgroups_file;
extern int read_news_via_nntp;
extern int read_saved_news;
extern int real_gid;
extern int real_uid;
extern int real_umask;
extern int reread_active_file;
extern int reread_active_file_secs;
extern int reread_active_for_posted_arts;
extern int save_news;
extern int save_to_mmdf_mailbox;
extern int show_author;
extern int show_description;
extern int show_last_line_prev_page;
extern int show_lines;
extern int show_only_unread_groups;
extern int show_subject;
extern int show_xcommentto;
extern int sigdashes;
extern int space_goto_next_unread;
extern int space_mode;
extern int start_any_unread;
extern int start_editor_offset;
extern int start_line_offset;
extern int strip_blanks;
extern int system_status;
extern int tab_after_X_selection;
extern int tab_goto_next_unread;
extern int tex2iso_supported;
extern int this_resp;
extern int thread_basenote;
extern int thread_catchup_on_exit;
extern int tin_gid;
extern int tin_uid;
extern int top;
extern int top_base;
extern int unlink_article;
extern int update;
extern int update_fork;
extern int use_builtin_inews;
extern int use_keypad;
extern int use_local_charset;
extern int use_mailreader_i;
extern int use_mouse;
extern int verbose;
extern int xcol;
extern int xmouse;
extern int xover_supported;
extern int xref_supported;
extern int xrow;
extern int xuser_supported;
extern long *base;
extern long head_next;
extern long note_mark[MAX_PAGES];	/* ftells on beginnings of pages */
extern long note_size;
extern struct passwd *myentry;
extern struct t_article *arts;
extern struct t_attribute glob_attributes;
extern struct t_filters glob_filter;
extern struct t_group *active;
extern struct t_newnews *newnews;
extern struct t_option option_table[];
extern struct t_posted *posted;
extern struct t_save *save;
extern struct t_screen *screen;
extern struct t_spooldir *spooldirs;
extern time_t new_newnews_time;

/* This fixes ambiguities on platforms that don't distinguish extern case */
#ifdef CASE_PROBLEM
#define txt_help_B   txt_help_BIGB
#define txt_help_D   txt_help_BIGD
#define txt_help_I   txt_help_BIGI
#define txt_help_K   txt_help_BIGK
#define txt_help_M   txt_help_BIGM
#define txt_help_S   txt_help_BIGS
#define txt_help_T   txt_help_BIGT
#define txt_help_U   txt_help_BIGU
#define txt_help_W   txt_help_BIGW
#define txt_help_X   txt_help_BIGX
#define txt_help_p_S txt_help_p_BIGS
#define txt_help_t_K txt_help_t_BIGK
#endif

#ifdef FORGERY
extern char mail_address[LEN];
extern constext txt_help_mail_address[];
extern constext txt_opt_mail_address[];
#endif

#ifdef HAVE_COLOR
extern constext txt_help_col_back[];
extern constext txt_help_col_from[];
extern constext txt_help_col_head[];
extern constext txt_help_col_help[];
extern constext txt_help_col_invers[];
extern constext txt_help_col_message[];
extern constext txt_help_col_minihelp[];
extern constext txt_help_col_normal[];
extern constext txt_help_col_quote[];
extern constext txt_help_col_response[];
extern constext txt_help_col_signature[];
extern constext txt_help_col_subject[];
extern constext txt_help_col_text[];
extern constext txt_help_col_title[];
extern constext txt_help_color[];
extern constext txt_opt_col_back[];
extern constext txt_opt_col_from[];
extern constext txt_opt_col_head[];
extern constext txt_opt_col_help[];
extern constext txt_opt_col_invers[];
extern constext txt_opt_col_message[];
extern constext txt_opt_col_minihelp[];
extern constext txt_opt_col_normal[];
extern constext txt_opt_col_quote[];
extern constext txt_opt_col_response[];
extern constext txt_opt_col_signature[];
extern constext txt_opt_col_subject[];
extern constext txt_opt_col_text[];
extern constext txt_opt_col_title[];
extern int col_back;
extern int col_from;
extern int col_head;
extern int col_help;
extern int col_invers;
extern int col_message;
extern int col_minihelp;
extern int col_normal;
extern int col_quote;
extern int col_response;
extern int col_signature;
extern int col_subject;
extern int col_text;
extern int col_title;
extern int use_color;
extern int use_color_tinrc;
#endif

#ifdef HAVE_KEYPAD
extern constext txt_help_use_keypad[];
extern constext txt_opt_use_keypad[];
#endif

#ifdef HAVE_METAMAIL
extern constext txt_help_ask_for_metamail[];
extern constext txt_help_use_metamail[];
extern constext txt_opt_ask_for_metamail[];
extern constext txt_opt_use_metamail[];
extern int ask_for_metamail;
extern int use_metamail;
#endif

#ifdef M_AMIGA
extern int tin_bbs_mode;
#endif

#ifdef M_OS2
extern char TMPDIR[PATH_LEN];
#endif

#ifdef M_UNIX
extern constext txt_help_keep_dead_articles[];
extern constext txt_opt_keep_dead_articles[];
#endif

#ifdef NNTP_ABLE
extern int  get_server_nolf; /* from open.c */
#endif

#ifndef NO_SHELL_ESCAPE
extern constext txt_help_shell[];
#endif

#ifdef SIGTSTP
extern int do_sigtstp;
#endif

#undef P_
