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

#ifndef EXTERN_H
#define EXTERN_H 1

/*
 * Library prototypes
 */

/*
 * The prototypes bracketed by DECL_xxxx ifdef's are used to get moderately
 * clean compiles on systems with pre-ANSI/POSIX headers when compiler warnings
 * are enabled.  (Not all of the functions are ANSI or POSIX).
 */
#ifdef DECL_ATOI
extern int atoi (char *);
#endif
#ifdef DECL_ATOL
extern int atol (char *);
#endif
#ifdef DECL_BCOPY
extern int bcopy (char *, char *, int);
#endif
#ifdef DECL_BZERO
extern void bzero (char *, int);
#endif
#ifdef DECL_CALLOC
extern void * calloc (size_t, size_t);
#endif
#ifdef DECL_FCLOSE
extern int fclose (FILE *);
#endif
#ifdef DECL_FDOPEN
extern FILE *fdopen (int, const char *);
#endif
#ifdef DECL_FFLUSH
extern int fflush (FILE *);
#endif
#if defined(DECL_FILENO) && !defined(fileno)
extern int fileno (FILE *);
#endif
#ifdef DECL_FPRINTF
extern int fprintf (FILE *, const char *, ...);
#endif
#ifdef DECL_FPUTC
extern int fputc (int, FILE *);
#endif
#ifdef DECL_FPUTS
extern int fputs (const char *, FILE *);
#endif
#ifdef DECL_FREAD
extern size_t fread (void *, size_t, size_t, FILE *);
#endif
#ifdef DECL_FREE
extern void free (void *);
#endif
#ifdef DECL_FSEEK
extern int fseek (FILE *, long, int);
#endif
#ifdef DECL_FWRITE
extern size_t fwrite (void *, size_t, size_t, FILE *);
#endif
#ifdef DECL_GETCWD
extern char *getcwd (char *, size_t);
#endif
#ifdef DECL_GETHOSTBYNAME
extern struct hostent *gethostbyname (const char *);
#endif
#ifdef DECL_GETHOSTNAME
extern int gethostname (char *, int);
#endif
#ifdef DECL_GETLOGIN
extern char *getlogin (void);
#endif
#ifdef DECL_GETOPT
extern int getopt (int, char * const*, const char *);
#endif
#ifdef DECL_GETPASS
char *getpass(const char *);
#endif
#ifdef DECL_GETPWNAM
extern struct passwd *getpwnam (const char *);
#endif
#ifdef DECL_GETSERVBYNAME
extern struct servent *getservbyname (const char *, const char *);
#endif
#ifdef DECL_INET_ADDR
extern unsigned long inet_addr (const char *);
#endif
#ifdef DECL_INET_NTOA
extern char *inet_ntoa (struct in_addr);
#endif
#ifdef DECL_IOCTL
extern int ioctl (int, unsigned long, void *);
#endif
#ifdef DECL_MALLOC
extern void * malloc (size_t);
#endif
#ifdef DECL_MEMSET
extern void * memset (void *, int, size_t);
#endif
#ifdef DECL_MKTEMP
extern char * mktemp (char *);
#endif
#ifdef DECL_PCLOSE
extern int pclose (FILE *);
#endif
#ifdef DECL_PERROR
extern void perror (const char *);
#endif
#ifdef DECL_POPEN
extern FILE * popen (const char *, const char *);
#endif
#ifdef DECL_PRINTF
extern int printf (const char *, ...);
#endif
#ifdef DECL_PUTENV
extern int putenv (const char *);
#endif
#ifdef DECL_QSORT
extern void qsort (void *, size_t, size_t, int (*)(t_comptype*, t_comptype*));
#endif
#ifdef DECL_REALLOC
extern void * realloc (void *, size_t);
#endif
#ifdef DECL_REWIND
extern void rewind (FILE *);
#endif
#ifdef DECL_SELECT
extern int select (int, fd_set *, fd_set *, fd_set *, struct timeval *);
#endif
#ifdef DECL_SETEGID
extern int setegid (gid_t);
#endif
#ifdef DECL_SETEUID
extern int seteuid (uid_t);
#endif
#ifdef DECL_SETPGRP
#if SETPGRP_VOID
extern pid_t setpgrp (void);
#else
extern int setpgrp (int, int);
#endif
#endif /* DECL_SETPGRP */
#ifdef DECL_SETREGID
extern int setregid (gid_t, gid_t);
#endif
#ifdef DECL_SETREUID
extern int setreuid (uid_t, uid_t);
#endif
#ifdef DECL_SOCKET
extern int socket (int, int, int);
#endif
#ifdef DECL_SSCANF
extern int sscanf (const char *, const char *, ...);
#endif
#ifdef DECL_STRCASECMP
extern int strcasecmp (const char *, const char *);
#endif
#ifdef DECL_STRCHR
extern char *strchr (const char *, int);
extern char *strrchr (const char *, int);
#endif
#ifdef DECL_STRFTIME
extern int strftime (char *, int, char *, struct tm *);
#endif
#ifdef DECL_STRNCASECMP
extern int strncasecmp (const char *, const char *, size_t);
#endif
#ifdef DECL_STRTOL
extern long strtol (const char *, char **, int);
#endif
#ifdef DECL_SYSTEM
extern int system (char *);
#endif
#ifdef DECL_TEMPNAM
extern char *tempnam (const char *, const char *);
#endif
#ifdef DECL_TGETENT
extern int tgetent (char *, char *);
#endif
#ifdef DECL_TGETFLAG
extern int tgetflag (char *);
#endif
#ifdef DECL_TGETNUM
extern int tgetnum (char *);
#endif
#ifdef DECL_TGETSTR
extern char *tgetstr (char *, char **);
#endif
#ifdef DECL_TGOTO
extern char *tgoto (char *, int, int);
#endif
#ifdef DECL_TIME
extern time_t time (time_t *);
#endif
#if defined(DECL_TOLOWER) && !defined(tolower)
extern int tolower (int);
#endif
#if defined(DECL_TOUPPER) && !defined(toupper)
extern int toupper (int);
#endif
#ifdef DECL_TPUTS
extern int tputs (char *, int, OutcPtr);
#endif
#ifdef DECL_USLEEP
void usleep(unsigned long);
#endif
#ifdef DECL__FLSBUF
extern int _flsbuf (int, FILE *);
#endif

#ifdef DECL_GETENV
extern char *getenv (const char *);
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
extern TCP *nntp_rd_fp, *nntp_wr_fp;
extern char *OPT_CHAR_list[];
extern char *OPT_STRING_list[];
extern char *glob_art_group;
extern char *glob_group;
extern char *glob_page_group;
extern char *nntp_server;
extern char active_times_file[PATH_LEN];
extern char add_addr[LEN];
extern char art_marked_deleted;
extern char art_marked_inrange;
extern char art_marked_return;
extern char art_marked_selected;
extern char art_marked_unread;
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
extern char news_headers_to_display[LEN];
extern char news_headers_to_not_display[LEN];
extern char **news_headers_to_display_array;
extern char **news_headers_to_not_display_array;
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
extern char homedir[PATH_LEN];
extern char index_maildir[PATH_LEN];
extern char index_newsdir[PATH_LEN];
extern char index_savedir[PATH_LEN];
extern char last_put[];
extern char libdir[PATH_LEN];
extern char local_attributes_file[PATH_LEN];
extern char local_config_file[PATH_LEN];
extern char local_filter_file[PATH_LEN];
extern char local_input_history_file[PATH_LEN];
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
extern char note_h_contentenc[HEADER_LEN];	/* Content-Transfer-Encoding: */
extern char note_h_contenttype[HEADER_LEN];	/* Content-Type: */
extern char note_h_date[HEADER_LEN];			/* Date:	*/
extern char note_h_distrib[HEADER_LEN];		/* Distribution: */
extern char note_h_followup[HEADER_LEN];		/* Followup-To: */
extern char note_h_from[HEADER_LEN];			/* From: */
extern char note_h_keywords[HEADER_LEN];		/* Keywords: */
extern char note_h_messageid[HEADER_LEN];		/* Message-ID:	*/
extern char note_h_mimeversion[HEADER_LEN];	/* Mime-Version: */
extern char note_h_newsgroups[HEADER_LEN];	/* Newsgroups:	*/
extern char note_h_org[HEADER_LEN];				/* Organization: */
extern char note_h_path[HEADER_LEN];			/* Path:	*/
extern char note_h_references[HEADER_LEN];	/* References:	*/
extern char note_h_subj[HEADER_LEN];			/* Subject:	*/
extern char note_h_summary[HEADER_LEN];		/* Summary: */
extern char novrootdir[PATH_LEN];
extern char page_header[LEN];
extern char post_proc_command[PATH_LEN];
extern char posted_info_file[PATH_LEN];
extern char posted_msgs_file[PATH_LEN];
extern char postponed_articles_file[PATH_LEN];
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

extern char domain_name[];
extern char host_name[];

extern const char base64_alphabet[64];
extern constext *help_group[];
extern constext *help_page[];
extern constext *help_select[];
extern constext *help_thread[];
extern constext *txt_colors[];
extern constext *txt_marks[];
extern constext *txt_mime_types[NUM_MIME_TYPES];
extern constext *txt_onoff[];
extern constext *txt_post_process[];
extern constext *txt_show_from[];
extern constext *txt_sort_type[];
extern constext *txt_strip_bogus[];
extern constext *txt_thread[];
extern constext *txt_wildcard[];
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
extern constext txt_art_parent_none[];
extern constext txt_art_parent_unavail[];
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
extern constext txt_authorization_fail[];
extern constext txt_autosubscribing_groups[];
extern constext txt_autoselecting_articles[];
extern constext txt_bad_active_file[];
extern constext txt_bad_article[];
extern constext txt_bad_command[];
extern constext txt_base64[];
extern constext txt_begin_of_art[];
extern constext txt_caching_disabled[];
extern constext txt_cancel_article[];
extern constext txt_cancelling_art[];
extern constext txt_cannot_create_uniq_name[];
extern constext txt_cannot_find_base_art[];
extern constext txt_cannot_get_nntp_server_name[];
extern constext txt_cannot_get_term[];
extern constext txt_cannot_get_term_entry[];
extern constext txt_cannot_open[];
extern constext txt_cannot_open_active_file[];
extern constext txt_cannot_post[];
extern constext txt_cannot_write_index[];
extern constext txt_cannot_write_to_directory[];
extern constext txt_catchup_all_read_groups[];
extern constext txt_catchup_despite_tags[];
extern constext txt_catchup_update_info[];
extern constext txt_check_article[];
extern constext txt_checking_new_groups[];
extern constext txt_checking_for_news[];
extern constext txt_checksum_of_file[];
extern constext txt_return_key[];
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
extern constext txt_default[];
extern constext txt_delete_processed_files[];
extern constext txt_deleting[];
extern constext txt_end_of_arts[];
extern constext txt_end_of_groups[];
extern constext txt_end_of_thread[];
extern constext txt_enter_message_id[];
extern constext txt_enter_option_num[];
extern constext txt_enter_range[];
extern constext txt_error_sender_in_header_not_allowed[];
extern constext txt_error_header_and_body_not_separate[];
extern constext txt_error_header_line_blank[];
extern constext txt_error_header_line_colon[];
extern constext txt_error_header_line_comma[];
extern constext txt_error_header_line_empty_newsgroups[];
extern constext txt_error_header_line_groups_contd[];
extern constext txt_error_header_line_missing_newsgroups[];
extern constext txt_error_header_line_missing_subject[];
extern constext txt_error_header_line_missing_target[];
extern constext txt_error_header_line_space[];
extern constext txt_error_no_enter_permission[];
extern constext txt_error_no_read_permission[];
extern constext txt_error_no_such_file[];
extern constext txt_error_no_write_permission[];
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
extern constext txt_help_L[];
extern constext txt_help_M[];
extern constext txt_help_S[];
extern constext txt_help_T[];
extern constext txt_help_U[];
extern constext txt_help_W[];
extern constext txt_help_X[];
extern constext txt_help_a[];
extern constext txt_help_alternative_handling[];
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
extern constext txt_help_cache_overview_files[];
extern constext txt_help_catchup_read_groups[];
extern constext txt_help_ck[];
extern constext txt_help_colon[];
extern constext txt_help_confirm_action[];
extern constext txt_help_confirm_to_quit[];
extern constext txt_help_ctrl_a[];
extern constext txt_help_ctrl_d[];
extern constext txt_help_ctrl_f[];
extern constext txt_help_ctrl_h[];
extern constext txt_help_ctrl_k[];
extern constext txt_help_ctrl_l[];
extern constext txt_help_d[];
extern constext txt_help_dash[];
extern constext txt_help_display_mime_header_asis[];
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
extern constext txt_help_g_caret_dollar[];
extern constext txt_help_g_cr[];
extern constext txt_help_g_ctrl_r[];
extern constext txt_help_g_d[];
extern constext txt_help_g_hash[];
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
extern constext txt_help_p_ctrl_e[];
extern constext txt_help_p_ctrl_w[];
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
extern constext txt_help_p_u[];
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
extern constext txt_help_prompt_followupto[];
extern constext txt_help_q[];
extern constext txt_help_quick_kill[];
extern constext txt_help_quick_select[];
extern constext txt_help_quote_chars[];
extern constext txt_help_quote_empty_lines[];
extern constext txt_help_quote_signatures[];
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
extern constext txt_help_show_signatures[];
extern constext txt_help_show_xcommentto[];
extern constext txt_help_sigdashes[];
extern constext txt_help_signature_repost[];
extern constext txt_help_sigfile[];
extern constext txt_help_sort_art_type[];
extern constext txt_help_space_goto_next_unread[];
extern constext txt_help_start_editor_offset[];
extern constext txt_help_strip_blanks[];
extern constext txt_help_strip_bogus[];
extern constext txt_help_strip_newsrc[];
extern constext txt_help_t[];
extern constext txt_help_t_caret_dollar[];
extern constext txt_help_t_cr[];
extern constext txt_help_t_num[];
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
extern constext txt_help_wildcard[];
extern constext txt_help_x[];
extern constext txt_help_xpost_quote_format[];
extern constext txt_help_y[];
extern constext txt_hit_space_for_more[];
extern constext txt_index_page_com[];
extern constext txt_info_add_kill[];
extern constext txt_info_add_select[];
extern constext txt_info_do_postpone[];
extern constext txt_info_postponed[];
extern constext txt_info_nopostponed[];
extern constext txt_invalid_from[];
extern constext txt_inverse_off[];
extern constext txt_inverse_on[];
extern constext txt_is_tex_ecoded[];
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
extern constext txt_opt_alternative_handling[];
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
extern constext txt_opt_cache_overview_files[];
extern constext txt_opt_catchup_read_groups[];
extern constext txt_opt_confirm_action[];
extern constext txt_opt_confirm_to_quit[];
extern constext txt_opt_display_mime_header_asis[];
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
extern constext txt_opt_prompt_followupto[];
extern constext txt_opt_quote_chars[];
extern constext txt_opt_quote_empty_lines[];
extern constext txt_opt_quote_signatures[];
extern constext txt_opt_reread_active_file_secs[];
extern constext txt_opt_save_to_mmdf_mailbox[];
extern constext txt_opt_savedir[];
extern constext txt_opt_show_author[];
extern constext txt_opt_show_description[];
extern constext txt_opt_show_last_line_prev_page[];
extern constext txt_opt_show_lines[];
extern constext txt_opt_show_only_unread[];
extern constext txt_opt_show_only_unread_groups[];
extern constext txt_opt_show_signatures[];
extern constext txt_opt_show_xcommentto[];
extern constext txt_opt_sigdashes[];
extern constext txt_opt_signature_repost[];
extern constext txt_opt_sigfile[];
extern constext txt_opt_sort_art_type[];
extern constext txt_opt_space_goto_next_unread[];
extern constext txt_opt_start_editor_offset[];
extern constext txt_opt_strip_blanks[];
extern constext txt_opt_strip_bogus[];
extern constext txt_opt_strip_newsrc[];
extern constext txt_opt_tab_after_X_selection[];
extern constext txt_opt_tab_goto_next_unread[];
extern constext txt_opt_thread_arts[];
extern constext txt_opt_thread_catchup_on_exit[];
extern constext txt_opt_unlink_article[];
extern constext txt_opt_use_builtin_inews[];
extern constext txt_opt_use_color_tinrc[];
extern constext txt_opt_use_mailreader_i[];
extern constext txt_opt_use_mouse[];
extern constext txt_opt_wildcard[];
extern constext txt_opt_xpost_quote_format[];
extern constext txt_option_not_enabled[];
extern constext txt_options_menu[];
extern constext txt_out_of_memory2[];
extern constext txt_out_of_memory[];
extern constext txt_plural[];
extern constext txt_post_a_followup[];
extern constext txt_post_an_article[];
extern constext txt_post_error_ask_postpone[];
extern constext txt_post_history_menu[];
extern constext txt_post_newsgroup[];
extern constext txt_post_newsgroups[];
extern constext txt_post_process_type[];
extern constext txt_post_processing[];
extern constext txt_post_processing_failed[];
extern constext txt_post_processing_finished[];
extern constext txt_post_subject[];
extern constext txt_posting[];
extern constext txt_postpone_repost[];
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
extern constext txt_quit_edit_postpone[];
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
extern constext txt_reading_input_history_file[];
extern constext txt_reading_new_arts[];
extern constext txt_reading_new_groups[];
extern constext txt_reading_news_active_file[];
extern constext txt_reading_news_newsrc_file[];
extern constext txt_reading_newsgroups_file[];
extern constext txt_reading_newsrc[];
extern constext txt_reconnect_to_news_server[];
extern constext txt_remove_bogus[];
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
extern constext txt_save_config[];
extern constext txt_save_filename[];
extern constext txt_saved[];
extern constext txt_saved_nothing[];
extern constext txt_saved_pattern_to[];
extern constext txt_saved_to_mailbox[];
extern constext txt_saving[];
extern constext txt_screen_init_failed[];
extern constext txt_screen_too_small[];
extern constext txt_screen_too_small_exiting[];
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
extern constext txt_skipping_newgroups[];
extern constext txt_stuff_nntp_cannot_open[];
extern constext txt_subj_line_only[];
extern constext txt_subj_line_only_case[];
extern constext txt_subscribe_pattern[];
extern constext txt_subscribed_num_groups[];
extern constext txt_subscribed_to[];
extern constext txt_subscribing[];
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
extern constext txt_tinrc_alternative_handling[];
extern constext txt_tinrc_art_marked_deleted[];
extern constext txt_tinrc_art_marked_inrange[];
extern constext txt_tinrc_art_marked_return[];
extern constext txt_tinrc_art_marked_selected[];
extern constext txt_tinrc_art_marked_unread[];
extern constext txt_tinrc_auto_bcc[];
extern constext txt_tinrc_auto_cc[];
extern constext txt_tinrc_auto_list_thread[];
extern constext txt_tinrc_auto_save[];
extern constext txt_tinrc_batch_save[];
extern constext txt_tinrc_beginner_level[];
extern constext txt_tinrc_cache_overview_files[];
extern constext txt_tinrc_catchup_read_groups[];
extern constext txt_tinrc_confirm_action[];
extern constext txt_tinrc_confirm_to_quit[];
extern constext txt_tinrc_default_editor_format[];
extern constext txt_tinrc_default_filter_days[];
extern constext txt_tinrc_default_maildir[];
extern constext txt_tinrc_default_mailer_format[];
extern constext txt_tinrc_default_printer[];
extern constext txt_tinrc_default_savedir[];
extern constext txt_tinrc_default_sigfile[];
extern constext txt_tinrc_defaults[];
extern constext txt_tinrc_display_mime_header_asis[];
extern constext txt_tinrc_draw_arrow[];
extern constext txt_tinrc_filter[];
extern constext txt_tinrc_force_screen_redraw[];
extern constext txt_tinrc_full_page_scroll[];
extern constext txt_tinrc_group_catchup_on_exit[];
extern constext txt_tinrc_groupname_max_length[];
extern constext txt_tinrc_header[];
extern constext txt_tinrc_highlight_xcommentto[];
extern constext txt_tinrc_inverse_okay[];
extern constext txt_tinrc_keep_posted_articles[];
extern constext txt_tinrc_mail_8bit_header[];
extern constext txt_tinrc_mail_address[];
extern constext txt_tinrc_mark_saved_read[];
extern constext txt_tinrc_mm_charset[];
extern constext txt_tinrc_motd_file_info[];
extern constext txt_tinrc_newnews[];
extern constext txt_tinrc_news_quote_format[];
extern constext txt_tinrc_no_advertising[];
extern constext txt_tinrc_pos_first_unread[];
extern constext txt_tinrc_post_8bit_header[];
extern constext txt_tinrc_post_mime_encoding[];
extern constext txt_tinrc_post_process_command[];
extern constext txt_tinrc_post_process_type[];
extern constext txt_tinrc_print_header[];
extern constext txt_tinrc_process_only_unread[];
extern constext txt_tinrc_prompt_followupto[];
extern constext txt_tinrc_quote_chars[];
extern constext txt_tinrc_quote_empty_lines[];
extern constext txt_tinrc_quote_signatures[];
extern constext txt_tinrc_reread_active_file_secs[];
extern constext txt_tinrc_save_to_mmdf_mailbox[];
extern constext txt_tinrc_news_headers_to_display[];
extern constext txt_tinrc_news_headers_to_not_display[];
extern constext txt_tinrc_show_author[];
extern constext txt_tinrc_show_description[];
extern constext txt_tinrc_show_last_line_prev_page[];
extern constext txt_tinrc_show_lines[];
extern constext txt_tinrc_show_only_unread[];
extern constext txt_tinrc_show_only_unread_groups[];
extern constext txt_tinrc_show_signatures[];
extern constext txt_tinrc_show_xcommentto[];
extern constext txt_tinrc_sigdashes[];
extern constext txt_tinrc_signature_repost[];
extern constext txt_tinrc_sort_article_type[];
extern constext txt_tinrc_space_goto_next_unread[];
extern constext txt_tinrc_start_editor_offset[];
extern constext txt_tinrc_strip_blanks[];
extern constext txt_tinrc_strip_bogus[];
extern constext txt_tinrc_strip_newsrc[];
extern constext txt_tinrc_tab_after_X_selection[];
extern constext txt_tinrc_tab_goto_next_unread[];
extern constext txt_tinrc_thread_articles[];
extern constext txt_tinrc_unlink_article[];
extern constext txt_tinrc_use_builtin_inews[];
extern constext txt_tinrc_use_mailreader_i[];
extern constext txt_tinrc_use_mouse[];
extern constext txt_tinrc_wildcard[];
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
extern constext txt_uudecoding[];
extern constext txt_value_out_of_range[];
extern constext txt_warn_art_line_too_long[];
extern constext txt_warn_cancel[];
extern constext txt_warn_difficulties[];
extern constext txt_warn_followup_to_several_groups[];
extern constext txt_warn_missing_followup_to[];
extern constext txt_warn_multiple_sigs[];
extern constext txt_warn_not_in_newsrc[];
extern constext txt_warn_not_valid_newsgroup[];
extern constext txt_warn_posting_etiquette[];
extern constext txt_warn_sig_too_long[];
extern constext txt_warn_suspicious_mail[];
extern constext txt_warn_update[];
extern constext txt_warn_wrong_sig_format[];
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
extern int cCOLS;
extern int cLINES;
extern int can_post;
extern int cmd_line;
extern int cur_groupnum;
extern int debug;
extern int default_auto_save_msg;
extern int default_filter_days;
extern int default_filter_kill_header;
extern int default_filter_select_header;
extern int default_move_group;
extern int default_post_proc_type;
extern int default_save_mode;
extern int default_show_author;
extern int default_sort_art_type;
extern int default_thread_arts;
extern int first_group_on_screen;
extern int first_subj_on_screen;
extern int glob_respnum;
extern int global_filtered_articles;
extern int group_hash[TABLE_SIZE];
extern int group_top;
extern int groupname_len;
extern int groupname_max_length;
extern int in_headers; /* colorful headers */
extern int index_point;
extern int iso2asc_supported;
extern int last_group_on_screen;
extern int last_resp;
extern int last_subj_on_screen;
extern int local_filtered_articles;
extern int local_index;
extern int mail_mime_encoding;
extern int max_active;
extern int max_art;
extern int max_from;
extern int max_newnews;
extern int max_save;
extern int max_spooldir;
extern int num_headers_to_display;
extern int num_headers_to_not_display;
extern int max_subj;
extern int note_end;					/* end of article ? */
extern int note_line;
extern int note_page;				/* what page we're on */
extern int num_active;
extern int num_newnews;
extern int num_of_killed_arts;
extern int num_of_selected_arts;
extern int num_of_tagged_arts;
extern int num_save;
extern int num_spooldir;
extern int post_mime_encoding;
extern int process_id;
extern int real_gid;
extern int real_uid;
extern int real_umask;
extern int reread_active_file;
extern int reread_active_file_secs;
extern int reread_active_for_posted_arts;
extern int show_subject;
extern int space_mode;
extern int start_line_offset;
extern int strip_bogus;
extern int system_status;
extern int tex2iso_supported;
extern int this_resp;
extern int thread_basenote;
extern int tin_gid;
extern int tin_uid;
extern int top;
extern int top_base;
extern int wildcard;
extern int xcol;
extern int xmouse;
extern int xrow;

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
extern struct t_spooldir *spooldirs;

extern t_bool alternative_handling;
extern t_bool auto_bcc;
extern t_bool auto_cc;
extern t_bool auto_list_thread;
extern t_bool beginner_level;
extern t_bool cache_overview_files;
extern t_bool catchup;
extern t_bool catchup_read_groups;
extern t_bool check_any_unread;
extern t_bool check_for_new_newsgroups;
extern t_bool confirm_action;
extern t_bool confirm_to_quit;
extern t_bool count_articles;
extern t_bool created_rcdir;
extern t_bool default_auto_save;
extern t_bool default_batch_save;
extern t_bool default_filter_kill_case;
extern t_bool default_filter_kill_expire;
extern t_bool default_filter_kill_global;
extern t_bool default_filter_select_case;
extern t_bool default_filter_select_expire;
extern t_bool default_filter_select_global;
extern t_bool default_show_only_unread;
extern t_bool delete_index_file;
extern t_bool display_mime_header_asis;
extern t_bool draw_arrow_mark;
extern t_bool force_screen_redraw;
extern t_bool full_page_scroll;
extern t_bool got_sig_pipe;
extern t_bool group_catchup_on_exit;
extern t_bool highlight_xcommentto;
extern t_bool inverse_okay;
extern t_bool keep_dead_articles;	/* keep all dead articles in dead.articles */
extern t_bool keep_posted_articles;	/* keep all posted articles in ~/Mail/posted */
extern t_bool mail_8bit_header;
extern t_bool mail_news;
extern t_bool mail_news_to_posted;
extern t_bool mark_saved_read;
extern t_bool newsrc_active;
extern t_bool no_advertising;
extern t_bool pos_first_unread;
extern t_bool post_8bit_header;
extern t_bool post_article_and_exit;
extern t_bool post_postponed_and_exit;
extern t_bool print_header;
extern t_bool process_only_unread;
extern t_bool prompt_followupto;
extern t_bool purge_index_files;
extern t_bool quote_empty_lines;
extern t_bool quote_signatures;
extern t_bool read_local_newsgroups_file;
extern t_bool read_news_via_nntp;
extern t_bool read_saved_news;
extern t_bool save_news;
extern t_bool save_to_mmdf_mailbox;
extern t_bool show_description;
extern t_bool show_last_line_prev_page;
extern t_bool show_lines;
extern t_bool show_only_unread_groups;
extern t_bool show_signatures;
extern t_bool show_xcommentto;
extern t_bool sigdashes;
extern t_bool signature_repost;
extern t_bool space_goto_next_unread;
extern t_bool start_any_unread;
extern t_bool start_editor_offset;
extern t_bool strip_blanks;
extern t_bool strip_newsrc;
extern t_bool tab_after_X_selection;
extern t_bool tab_goto_next_unread;
extern t_bool thread_catchup_on_exit;
extern t_bool unlink_article;
extern t_bool update;
extern t_bool update_fork;
extern t_bool use_builtin_inews;
extern t_bool use_keypad;
extern t_bool use_mailreader_i;
extern t_bool use_mouse;
extern t_bool verbose;
extern t_bool xover_supported;
extern t_bool xref_supported;

extern time_t new_newnews_time;

#define HIST_SIZE		15
#define HIST_OTHER		 0
#define HIST_ART_SEARCH		 1
#define HIST_AUTHOR_SEARCH	 2
#define HIST_GOTO_GROUP		 3
#define HIST_GROUP_SEARCH	 4
#define HIST_MAIL_ADDRESS	 5
#define HIST_MESSAGE_ID		 6
#define HIST_MOVE_GROUP		 7
#define HIST_PIPE_COMMAND	 8
#define HIST_POST_NEWSGROUPS	 9
#define HIST_POST_SUBJECT	10
#define HIST_REGEX_PATTERN	11
#define HIST_REPOST_GROUP	12
#define HIST_SAVE_FILE	 	13
#define HIST_SELECT_PATTERN	14
#define HIST_SHELL_COMMAND	15
#define HIST_SUBJECT_SEARCH	16
#define HIST_MAXNUM		17	/* must always be the same as the highest HIST_ value! */

extern int hist_last[HIST_MAXNUM+1];
extern int hist_pos[HIST_MAXNUM+1];
extern char *input_history[HIST_MAXNUM+1][HIST_SIZE+1];

extern char mail_address[LEN];
extern constext txt_help_mail_address[];
extern constext txt_opt_mail_address[];

/* This fixes ambiguities on platforms that don't distinguish extern case */
#ifdef CASE_PROBLEM
#define txt_help_B   txt_help_BIGB
#define txt_help_D   txt_help_BIGD
#define txt_help_I   txt_help_BIGI
#define txt_help_K   txt_help_BIGK
#define txt_help_L   txt_help_BIGL
#define txt_help_M   txt_help_BIGM
#define txt_help_S   txt_help_BIGS
#define txt_help_T   txt_help_BIGT
#define txt_help_U   txt_help_BIGU
#define txt_help_W   txt_help_BIGW
#define txt_help_X   txt_help_BIGX
#define txt_help_p_S txt_help_p_BIGS
#define txt_help_t_K txt_help_t_BIGK
#endif

#ifndef DONT_HAVE_PIPING
	extern constext txt_pipe_to_command[];
	extern constext txt_piping[];
#else
	extern constext txt_piping_not_enabled[];
#endif /* DONT_HAVE_PIPING */

#ifdef FORGERY
	extern constext txt_warn_cancel_forgery[];
#endif /* FORGERY */

#ifdef HAVE_COLOR
	extern constext txt_help__[];
	extern constext txt_help_col_back[];
	extern constext txt_help_col_from[];
	extern constext txt_help_col_head[];
	extern constext txt_help_col_help[];
	extern constext txt_help_col_invers_bg[];
	extern constext txt_help_col_invers_fg[];
	extern constext txt_help_col_markdash[];
	extern constext txt_help_col_markstar[];
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
	extern constext txt_help_word_h_display_marks[];
	extern constext txt_help_word_highlight_tinrc[];
	extern constext txt_opt_col_back[];
	extern constext txt_opt_col_from[];
	extern constext txt_opt_col_head[];
	extern constext txt_opt_col_help[];
	extern constext txt_opt_col_invers_bg[];
	extern constext txt_opt_col_invers_fg[];
	extern constext txt_opt_col_markdash[];
	extern constext txt_opt_col_markstar[];
	extern constext txt_opt_col_message[];
	extern constext txt_opt_col_minihelp[];
	extern constext txt_opt_col_normal[];
	extern constext txt_opt_col_quote[];
	extern constext txt_opt_col_response[];
	extern constext txt_opt_col_signature[];
	extern constext txt_opt_col_subject[];
	extern constext txt_opt_col_text[];
	extern constext txt_opt_col_title[];
	extern constext txt_opt_word_h_display_marks[];
	extern constext txt_opt_word_highlight_tinrc[];
	extern constext txt_tinrc_col_back[];
	extern constext txt_tinrc_col_from[];
	extern constext txt_tinrc_col_head[];
	extern constext txt_tinrc_col_newsheaders[];
	extern constext txt_tinrc_col_help[];
	extern constext txt_tinrc_col_invers_bg[];
	extern constext txt_tinrc_col_invers_fg[];
	extern constext txt_tinrc_col_markstar[];
	extern constext txt_tinrc_col_message[];
	extern constext txt_tinrc_col_minihelp[];
	extern constext txt_tinrc_col_normal[];
	extern constext txt_tinrc_col_response[];
	extern constext txt_tinrc_col_quote[];
	extern constext txt_tinrc_col_signature[];
	extern constext txt_tinrc_col_subject[];
	extern constext txt_tinrc_col_text[];
	extern constext txt_tinrc_col_title[];
	extern constext txt_tinrc_colors[];
	extern constext txt_tinrc_use_color[];
	extern constext txt_tinrc_word_h_display_marks[];
	extern constext txt_tinrc_word_highlight[];
	extern constext txt_toggled_high_off[];
	extern constext txt_toggled_high_on[];
	extern int col_back;
	extern int col_from;
	extern int col_head;
	extern int col_newsheaders;
	extern int col_help;
	extern int col_invers_bg;
	extern int col_invers_fg;
	extern int col_markdash;
	extern int col_markstar;
	extern int col_message;
	extern int col_minihelp;
	extern int col_normal;
	extern int col_quote;
	extern int col_response;
	extern int col_signature;
	extern int col_subject;
	extern int col_text;
	extern int col_title;
	extern int default_bcol;
	extern int default_fcol;
	extern int use_color;
	extern int word_h_display_marks;
	extern int word_highlight;
	extern t_bool use_color_tinrc;
	extern t_bool word_highlight_tinrc;
#	if 0 /* unused */
	extern constext txt_help_col_newsheaders[];
	extern constext txt_tinrc_col_markdash[];
#	endif /* 0 */
#endif /* HAVE_COLOR */

#ifdef HAVE_FASCIST_NEWSADMIN
	extern constext txt_error_followup_to_several_groups[];
	extern constext txt_error_missing_followup_to[];
	extern constext txt_error_not_valid_newsgroup[];
#endif /* HAVE_FASCIST_NEWSADMIN */

#ifdef HAVE_KEYPAD
	extern constext txt_help_use_keypad[];
	extern constext txt_opt_use_keypad[];
	extern constext txt_tinrc_use_keypad[];
#endif /* HAVE_KEYPAD */

#ifdef HAVE_METAMAIL
	extern constext txt_help_ask_for_metamail[];
	extern constext txt_help_use_metamail[];
	extern constext txt_opt_ask_for_metamail[];
	extern constext txt_opt_use_metamail[];
	extern constext txt_tinrc_ask_for_metamail[];
	extern constext txt_tinrc_use_metamail[];
	extern constext txt_use_mime[];
	extern t_bool ask_for_metamail;
	extern t_bool use_metamail;
#endif /* HAVE_METAMAIL */

#ifdef HAVE_PGP
	extern char pgp_data[PATH_LEN];
	extern const char *pgpopts;
	extern constext txt_help_ctrl_g[];
	extern constext txt_pgp_mail[];
	extern constext txt_pgp_news[];
	extern constext txt_pgp_not_avail[];
	extern constext txt_pgp_nothing[];
#endif /* HAVE_PGP */

#ifdef HAVE_SYS_UTSNAME_H
	extern struct utsname system_info;
#endif /* HAVE_SYS_UTSNAME_H */

#ifdef HAVE_TIN_NNTP_EXTS
	extern t_bool xuser_supported;
#endif /* HAVE_TIN_NNTP_EXTS */

#ifdef INDEX_DAEMON
	extern char group_times_file[PATH_LEN];
	extern constext txt_cannot_stat_group[];
	extern constext txt_cannot_stat_index_file[];
#endif /* INDEX_DAEMON */

#if !defined(INDEX_DAEMON) && !defined(ACTIVE_DAEMON)
	extern constext txt_intro_page[];
#endif /* !INDEX_DAEMON && !ACTIVE_DAEMON */

#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
	extern constext txt_reading_mail_active_file[];
	extern constext txt_reading_mailgroups_file[];
#endif /* !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING) */

#ifdef LOCAL_CHARSET
	extern constext txt_tinrc_local_charset[];
	extern int use_local_charset;
#endif /* LOCAL_CHARSET */

#ifdef M_AMIGA
	extern constext txt_env_var_not_found[];
	extern int tin_bbs_mode;
#endif /* M_AMIGA */

#ifdef M_OS2
	extern char TMPDIR[PATH_LEN];
#endif /* M_OS2 */

#ifdef M_UNIX
	extern constext txt_help_keep_dead_articles[];
	extern constext txt_opt_keep_dead_articles[];
	extern constext txt_tinrc_keep_dead_articles[];
#endif /* M_UNIX */

#ifdef NNTP_ABLE
	extern int get_server_nolf;
#endif /* NNTP_ABLE */

#ifndef NO_SHELL_ESCAPE
	extern constext txt_help_shell[];
#endif /* NO_SHELL_ESCAPE */

#if !defined(USE_CURSES)
	extern struct t_screen *screen;
#endif /* !USE_CURSES */

#if 0 /* unused */
extern constext txt_help_cr[];
extern constext txt_help_g_c[];
extern constext txt_help_g_l[];
extern constext txt_help_space_toggles[];
extern constext txt_help_t_K[];
extern constext txt_help_t_tab[];
extern constext txt_mini_spooldir_1[];
extern constext txt_opt_col_newsheaders[];
extern constext txt_tinrc_mail_mime_encoding[];
extern constext txt_tinrc_mail_quote_format[];
extern constext txt_tinrc_thread_catchup_on_exit[];
extern constext txt_tinrc_xpost_quote_format[];
extern constext txt_subscribe_to_new_group[];
extern constext txt_subscribing_to[];
#endif /* 0 */

#endif /* !EXTERN_H */
