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

#ifdef apollo
extern char *getlogin  P_((void));
extern int access  P_((char *, int));
extern int bcopy  P_((char *, char *, int));
extern int bzero  P_((char *, int));
extern int chdir  P_((char *));
extern int chmod  P_((char *, int));
extern int dup  P_((int));
extern int gethostname  P_((char *, int));
extern int ioctl  P_((int, unsigned long, void *));
extern int mkdir  P_((char *, int));
extern int setregid  P_((int, int));
extern int setreuid  P_((int, int));
extern int umask  P_((int));
extern int wait  P_((int *));
#endif

#if defined(DEBUG) && (!defined(__GNUC__) && !defined(__DECC))

extern unsigned int alarm  P_((unsigned int seconds));
extern int close  P_((int fildes));
extern int fork  P_((void));
#if !defined (WIN32)
extern int getpid  P_((void));
#if !defined(DGUX) && !defined(M_AMIGA)
extern unsigned short getuid  P_((void));
extern unsigned short getegid  P_((void));
extern unsigned short geteuid  P_((void));
#endif /* DGUX */
#endif /* !WIN32 */
extern int kill  P_((int pid, int sig));
#if defined(sun) && defined(HAVE_STDLIB_H)
extern int bcopy  P_((char *p1, char *p2, int n));
extern void bzero  P_((char *p, int n));
extern int fclose  P_((FILE *fp));
extern int fflush  P_((FILE *fp));
extern int fprintf  P_((FILE *fp, const char *fmt, ...));
extern int fputc  P_((int c, FILE *fp));
extern int fputs  P_((const char *s, FILE *fp));
extern size_t fread  P_((void *p, size_t size, size_t n, FILE *s));
extern int fseek  P_((FILE *p, long off, int n));
extern size_t fwrite  P_((void *p, size_t size, size_t n, FILE *s));
extern int gethostname  P_((char *name, int namelen));
extern char * getwd  P_((char *));
extern int ioctl  P_((int fd, unsigned long req, void *p));
extern void * memset  P_((void *p, int c, size_t n));
extern int open  P_((const char *path, int oflag, ...));
extern void perror  P_((const char *s));
extern int printf  P_((const char *fmt, ...));
extern int puts  P_((const char *s));
extern void rewind  P_((FILE *s));
extern int select  P_((int width, fd_set *r, fd_set *w, fd_set *e, struct timeval *t));
extern int socket  P_((int domain, int type, int protocol));
extern int sscanf  P_((const char *s, const char *fmt, ...));
extern long strtol  P_((const char *s, char **d, int base));
extern int system  P_((char *s));
extern time_t time  P_((time_t *p));
extern int tolower  P_((int c));
extern int toupper  P_((int c));
extern int wait  P_((int *s));
#else
#	ifndef M_AMIGA
extern int open  P_((char *path, int oflag));
#	endif
#endif
extern int pclose  P_((FILE *stream));
#if !defined (linux) && !defined (WIN32)
extern int tgetent  P_((char *bp, char *name));
extern int tputs  P_((register char *cp, int count, int (*outc)(int)));
extern int getopt  P_((int argc, char **argv, char *optstring));
#	if defined(sun) && defined(HAVE_STDLIB_H)
extern char *getcwd  P_((char *buf, size_t size));
extern int link  P_((const char *path1, const char *path2));
/*extern int read  P_((int fildes, void *buf, unsigned int nbyte)); */
extern int setgid  P_((uid_t gid));
extern int setegid  P_((uid_t gid));
extern int setuid  P_((uid_t uid));
extern int seteuid  P_((uid_t uid));
extern int unlink  P_((const char *path));
#	else
extern char *getcwd  P_((char *buf, int size));
extern int link  P_((char *path1, char *path2));
/*extern int read  P_((int fildes, char *buf, unsigned int nbyte)); */
#ifndef M_AMIGA
extern int setgid  P_((int gid));
extern int setuid  P_((int uid));
#endif
extern int unlink  P_((char *path));
#	endif
#endif
#if !defined(sony) && !defined(linux) && !defined(M_AMIGA) && !defined(WIN32)
#	ifndef DGUX
extern unsigned short getgid  P_((void));
#	endif /* DGUX */
#	ifdef BSD
extern void setpgrp  P_((int pid, int pgrp));
#	else
extern void setpgrp  P_((void));
#	endif
extern unsigned int sleep  P_((unsigned int seconds));
#endif

#endif

#ifdef DECL_BCOPY
extern int	bcopy P_((char *, char *, int));
#endif
#ifdef DECL_BZERO
extern void bzero P_((char *, int));
#endif
#ifdef DECL_FCLOSE
extern int fclose P_((FILE *));
#endif
#ifdef DECL_FFLUSH
extern int fflush P_((FILE *));
#endif
#ifdef DECL_FPRINTF
extern int fprintf P_((FILE *, const char *, ...));
#endif
#ifdef DECL_FPUTC
extern int fputc  P_((int c, FILE *fp));
#endif
#ifdef DECL_FPUTS
extern int fputs  P_((const char *s, FILE *fp));
#endif
#ifdef DECL_FREAD
extern size_t fread  P_((void *p, size_t size, size_t n, FILE *s));
#endif
#ifdef DECL_FSEEK
extern int fseek  P_((FILE *p, long off, int n));
#endif
#ifdef DECL_FWRITE
extern size_t fwrite  P_((void *p, size_t size, size_t n, FILE *s));
#endif
#ifdef DECL_GETHOSTNAME
extern int gethostname  P_((char *, int));
#endif
#ifdef DECL_GETOPT
extern int getopt  P_((int argc, char **argv, char *optstring));
#endif
#ifdef DECL_IOCTL
extern int ioctl  P_((int, unsigned long, void *));
#endif
#ifdef DECL_MEMSET
extern void * memset  P_((void *p, int c, size_t n));
#endif
#ifdef DECL_MKTEMP
extern char * mktemp P_((char *));
#endif
#ifdef DECL_PCLOSE
extern int pclose P_((FILE *));
#endif
#ifdef DECL_PERROR
extern void perror P_((const char *s));
#endif
#ifdef DECL_PRINTF
extern int printf P_((const char *fmt, ...));
#endif
#ifdef DECL_REWIND
extern void rewind  P_((FILE *s));
#endif
#ifdef DECL_SELECT
extern int select  P_((int width, fd_set *r, fd_set *w, fd_set *e, struct timeval *t));
#endif
#ifdef DECL_SETEGID
extern int setegid  P_((uid_t gid));
#endif
#ifdef DECL_SETEUID
extern int seteuid  P_((uid_t uid));
#endif
#ifdef DECL_SETPGRP
/* FIXME: setpgrp */
#endif
#ifdef DECL_SOCKET
extern int socket  P_((int domain, int type, int protocol));
#endif
#ifdef DECL_SSCANF
extern int sscanf  P_((const char *s, const char *fmt, ...));
#endif
#ifdef DECL_STRFTIME
extern int strftime P_((char *, int, char *, struct tm *));
#endif
#ifdef DECL_STRTOL
extern long strtol P_((const char *s, char **d, int base));
#endif
#ifdef DECL_SYSTEM
extern int system  P_((char *s));
#endif
#ifdef DECL_TIME
extern time_t time  P_((time_t *p));
#endif
#ifdef DECL_TOLOWER
extern int tolower  P_((int c));
#endif
#ifdef DECL_TOUPPER
extern int toupper  P_((int c));
#endif
#ifdef DECL_TPUTS
extern int tputs  P_((char *, int count, int (*outc)(int)));
#endif
#ifdef DECL__FLSBUF
extern int _flsbuf P_((int, FILE *));
#endif

#if !__STDC__ || defined(DECL_GETENV)
extern char *getenv  P_((char *));
#endif

extern int optind;
extern char *optarg;

/*
 * Local variables
 */
 
extern char *help_group[];
extern char *help_page[];
extern char *help_select[];
extern char *help_thread[];
extern char active_times_file[PATH_LEN];
extern char add_addr[LEN];
extern char article[PATH_LEN];
extern char bug_addr[LEN];
extern char ch_post_process[];
extern char cmd_line_printer[PATH_LEN];
extern char cvers[LEN];
extern char dead_article[PATH_LEN];
extern char dead_articles[PATH_LEN];
extern char default_art_search[LEN];
extern char default_author_search[LEN];
extern char default_editor_format[PATH_LEN];
extern char default_mail_address[LEN];
extern char default_mailer_format[PATH_LEN];
extern char default_organization[PATH_LEN];
extern char default_pipe_command[LEN];
extern char default_post_newsgroups[PATH_LEN];
extern char default_post_subject[PATH_LEN];
extern char default_range_group[PATH_LEN];
extern char default_range_select[PATH_LEN];
extern char default_range_thread[PATH_LEN];
extern char default_regex_pattern[LEN];
extern char default_repost_group[LEN];
extern char default_save_file[PATH_LEN];
extern char default_select_pattern[LEN];
extern char default_shell_command[LEN];
extern char default_goto_group[LEN];
extern char default_group_search[LEN];
extern char default_maildir[PATH_LEN];
extern char default_savedir[PATH_LEN];
extern char default_sigfile[PATH_LEN];
extern char default_signature[PATH_LEN];
extern char default_subject_search[LEN];
extern char global_attributes_file[PATH_LEN];
extern char global_config_file[PATH_LEN];
extern char global_filter_file[PATH_LEN];
extern char group_times_file[PATH_LEN];
extern char homedir[PATH_LEN];
extern char index_maildir[PATH_LEN];
extern char index_newsdir[PATH_LEN];
extern char index_savedir[PATH_LEN];
extern char libdir[PATH_LEN];
extern char lock_file[PATH_LEN];
extern char local_attributes_file[PATH_LEN];
extern char local_config_file[PATH_LEN];
extern char local_filter_file[PATH_LEN];
extern char local_newsgroups_file[PATH_LEN];
extern char mail_active_file[PATH_LEN];
extern char mail_quote_format[PATH_LEN];
extern char news_active_file[PATH_LEN];
extern char news_quote_format[PATH_LEN];
extern char xpost_quote_format[PATH_LEN];
extern char mail_news_user[LEN];
extern char mailbox[PATH_LEN];
extern char mailer[PATH_LEN];
extern char mime_encoding[LEN];
extern char motd_file[PATH_LEN];
extern char motd_file_info[PATH_LEN];
extern char msg[LEN];
extern char msg_headers_file[PATH_LEN];
extern char my_distribution[LEN];
extern char new_newnews_host[PATH_LEN];
extern char mailgroups_file[PATH_LEN];
extern char newsgroups_file[PATH_LEN];
extern char newnewsrc[PATH_LEN];
extern char newsrc[PATH_LEN];
extern char *nntp_server;
extern char page_header[LEN];
extern char *pgpopts;
extern char pgp_data[PATH_LEN];
extern char posted_info_file[PATH_LEN];
extern char posted_msgs_file[PATH_LEN];
extern char post_proc_command[PATH_LEN];
extern char default_printer[LEN];
extern char novrootdir[PATH_LEN];
extern char proc_ch_default;
extern char progname[PATH_LEN];
extern char quote_chars[PATH_LEN];
extern char rcdir[PATH_LEN];
extern char redirect_output[LEN];
extern char reply_to[LEN];
extern char save_active_file[PATH_LEN];
extern char spooldir[PATH_LEN];
extern char subscriptions_file[PATH_LEN];
extern char userid[PATH_LEN];
#ifdef M_OS2
extern char TMPDIR[PATH_LEN];
#endif

extern const char base64_alphabet[64];


extern constext txt_1_resp[];
extern constext txt_abort_indexing[];
extern constext txt_abort_searching[];
extern constext txt_active_file_is_empty[];
extern constext txt_added_groups[];
extern constext txt_append_overwrite_quit[];
extern constext txt_autosubscribing_groups[];
extern constext txt_lines[];
extern constext txt_art_deleted[];
extern constext txt_art_cannot_delete[];
extern constext txt_art_marked_as_unread[];
extern constext txt_art_not_saved[];
extern constext txt_art_pager_com[];
extern constext txt_art_posted[];
extern constext txt_art_rejected[];
extern constext txt_art_saved_to[];
extern constext txt_art_thread_regex_tag[];
extern constext txt_art_unavailable[];
extern constext txt_author_search_backwards[];
extern constext txt_author_search_forwards[];
extern constext txt_bad_active_file[];
extern constext txt_bad_article[];
extern constext txt_bad_command[];
extern constext txt_begin_of_art[];
extern constext txt_delete_article[];
extern constext txt_deleting[];
extern constext txt_catchup_all_read_groups[];
extern constext txt_cannot_find_base_art[];
extern constext txt_cannot_get_nntp_server_name[];
extern constext txt_cannot_open[];
extern constext txt_cannot_open_active_file[];
extern constext txt_cannot_open_art[];
extern constext txt_cannot_post[];
extern constext txt_cannot_change_spooldir[];
extern constext txt_cannot_write_index[];
extern constext txt_cannot_stat_group[];
extern constext txt_cannot_stat_index_file[];
extern constext txt_catchup_despite_tags[];
extern constext txt_catchup_update_info[];
extern constext txt_changing_spooldir_to[];
extern constext txt_check_article[];
extern constext txt_checking_active_file[];
extern constext txt_checking_for_news[];
extern constext txt_checksum_of_file[];
extern constext txt_color_off[];
extern constext txt_color_on[];
extern constext txt_command_failed[];
extern constext txt_command_failed_s[];
extern constext txt_connecting_port[];
extern constext txt_connecting[];
extern constext txt_reconnect_to_news_server[];
extern constext txt_connection_to_server_broken[];
extern constext txt_continue[];
extern constext txt_copyright_notice[];
extern constext txt_creating_newsrc[];
extern constext txt_del_group_in_newsrc[];
extern constext txt_delete_bogus_group[];
extern constext txt_deleting_art[];
extern constext txt_deleting_from_newsrc[];
extern constext txt_delete_processed_files[];
extern constext txt_end_of_arts[];
extern constext txt_end_of_groups[];
extern constext txt_end_of_thread[];
extern constext txt_env_var_not_found[];
extern constext txt_enter_range[];
extern constext txt_error_header_line_blank[];
extern constext txt_error_header_line_colon[];
extern constext txt_error_header_line_space[];
extern constext txt_error_header_line_empty_newsgroups[];
extern constext txt_error_header_line_missing_newsgroups[];
extern constext txt_error_header_line_missing_subject[];
extern constext txt_error_header_line_missing_target[];
extern constext txt_warn_art_line_too_long[];
extern constext txt_warn_posting_etiquette[];
extern constext txt_error_header_and_body_not_seperate[];
extern constext txt_error_from_in_header_not_allowed[];
extern constext txt_art_newsgroups[];
extern constext txt_warn_not_valid_newsgroup[];
extern constext txt_error_not_valid_newsgroup[];
extern constext txt_error_header_line_comma[];
extern constext txt_warn_missing_followup_to[];
extern constext txt_error_missing_followup_to[];
extern constext txt_warn_followup_to_several_groups[];
extern constext txt_error_followup_to_several_groups[];
extern constext txt_extracting_archive[];
extern constext txt_extracting_shar[];
extern constext txt_failed_to_connect_to_server[];
extern constext txt_feed_pattern[];
extern constext txt_filesystem_full[];
extern constext txt_filesystem_full_backup[];
extern constext txt_filesystem_full_config[];
extern constext txt_filesystem_full_config_backup[];
extern constext txt_filter_global_rules[];
extern constext txt_filter_local_rules[];
extern constext txt_group[];
extern constext txt_group_deleted[];
extern constext txt_group_is_moderated[];
extern constext txt_group_select_com[];
extern constext txt_select_pattern[];
extern constext txt_skipping_newsgroups[];
extern constext txt_spooldir_com[];
extern constext txt_group_selection[];
extern constext txt_group_undeleted[];
extern char txt_help_bug_report[LEN];
extern constext txt_help_4[];
#ifdef CASE_PROBLEM
extern constext txt_help_BIGD[];
extern constext txt_help_BIGI[];
extern constext txt_help_BIGK[];
extern constext txt_help_BIGM[];
extern constext txt_help_BIGS[];
extern constext txt_help_BIGT[];
extern constext txt_help_BIGU[];
extern constext txt_help_BIGW[];
extern constext txt_help_BIGX[];
#else
extern constext txt_help_D[];
extern constext txt_help_I[];
extern constext txt_help_K[];
extern constext txt_help_M[];
extern constext txt_help_S[];
extern constext txt_help_T[];
extern constext txt_help_U[];
extern constext txt_help_W[];
extern constext txt_help_X[];
#endif
extern constext txt_help_a[];
extern constext txt_help_autosave[];
extern constext txt_help_b[];
#ifdef CASE_PROBLEM
extern constext txt_help_BIGB[];
#else
extern constext txt_help_B[];
#endif
extern constext txt_help_bug[];
extern constext txt_help_c[];
extern constext txt_help_cC[];
extern constext txt_help_ck[];
extern constext txt_help_cr[];
extern constext txt_help_catchup_groups[];
extern constext txt_help_confirm_action[];
extern constext txt_help_colon[];
extern constext txt_help_ctrl_a[];
extern constext txt_help_quick_kill[];
extern constext txt_help_quick_select[];
extern constext txt_help_ctrl_d[];
extern constext txt_help_ctrl_f[];
extern constext txt_help_ctrl_g[];
extern constext txt_help_ctrl_h[];
extern constext txt_help_ctrl_k[];
extern constext txt_help_ctrl_l[];
#ifdef HAVE_COLOR
extern constext txt_help_color[];
#endif
extern constext txt_help_d[];
extern constext txt_help_dash[];
extern constext txt_help_draw_arrow[];
extern constext txt_help_e[];
extern constext txt_help_esc[];
extern constext txt_help_equal[];
extern constext txt_help_g[];
extern constext txt_help_g_4[];
extern constext txt_help_g_c[];
extern constext txt_help_g_d[];
extern constext txt_help_g_cr[];
extern constext txt_help_g_ctrl_r[];
extern constext txt_help_g_l[];
extern constext txt_help_g_q[];
extern constext txt_help_g_r[];
extern constext txt_help_g_search[];
extern constext txt_help_g_tab[];
extern constext txt_help_g_y[];
extern constext txt_help_g_z[];
extern constext txt_help_h[];
extern constext txt_help_g_hash[];
extern constext txt_help_hash[];
extern constext txt_help_i[];
extern constext txt_help_i_4[];
extern constext txt_help_i_coma[];
extern constext txt_help_i_cr[];
extern constext txt_help_i_dot[];
extern constext txt_help_i_n[];
extern constext txt_help_i_p[];
extern constext txt_help_i_search[];
extern constext txt_help_i_star[];
extern constext txt_help_i_tab[];
extern constext txt_help_i_tilda[];
extern constext txt_help_j[];
extern constext txt_help_l[];
extern constext txt_help_m[];
extern constext txt_help_maildir[];
extern constext txt_help_mark_saved_read[];
extern constext txt_help_n[];
extern constext txt_help_o[];
extern constext txt_help_p_0[];
extern constext txt_help_p_4[];
extern constext txt_help_p_coma[];
extern constext txt_help_p_cr[];
extern constext txt_help_p_ctrl_r[];
extern constext txt_help_p_d[];
extern constext txt_help_p_dot[];
extern constext txt_help_p_f[];
extern constext txt_help_p_g[];
extern constext txt_help_p_k[];
extern constext txt_help_p_m[];
extern constext txt_help_p_n[];
extern constext txt_help_p_p[];
extern constext txt_help_p_r[];
extern constext txt_help_p_s[];
#ifdef CASE_PROBLEM
extern constext txt_help_p_BIGS[];
#else
extern constext txt_help_p_S[];
#endif
extern constext txt_help_p_search[];
extern constext txt_help_p_star[];
extern constext txt_help_p_tab[];
extern constext txt_help_p_tilda[];
extern constext txt_help_p_z[];
extern constext txt_help_page_scroll[];
extern constext txt_help_pipe[];
extern constext txt_help_plus[];
extern constext txt_help_pos_first_unread[];
extern constext txt_help_post_proc_type[];
extern constext txt_help_print_header[];
extern constext txt_help_printer[];
extern constext txt_help_q[];
extern constext txt_help_r[];
extern constext txt_help_s[];
extern constext txt_help_savedir[];
extern constext txt_help_sel_c[];
extern constext txt_help_semicolon[];
#ifndef NO_SHELL_ESCAPE
extern constext txt_help_shell[];
#endif
extern constext txt_help_show_author[];
extern constext txt_help_show_description[];
extern constext txt_help_show_only_unread[];
extern constext txt_help_sort_type[];
extern constext txt_help_start_editor_offset[];
extern constext txt_help_t[];
extern constext txt_help_tex[];
extern constext txt_help_t_0[];
extern constext txt_help_t_4[];
#ifdef CASE_PROBLEM
extern constext txt_help_t_BIGK[];
#else
extern constext txt_help_t_K[];
#endif
extern constext txt_help_t_cr[];
extern constext txt_help_t_tab[];
extern constext txt_help_thread[];
extern constext txt_help_thread_arts[];
extern constext txt_help_u[];
extern constext txt_help_v[];
extern constext txt_help_w[];
extern constext txt_help_x[];
extern constext txt_help_y[];
extern constext txt_help_s_i[];
extern constext txt_help_g_t_p_i[];
extern constext txt_hit_any_key[];
extern constext txt_cmdline_hit_any_key[];
extern constext txt_hit_space_for_more[];
extern constext txt_index_page_com[];
extern constext txt_ispell_define_not_compiled[];
extern constext txt_inverse_off[];
extern constext txt_inverse_on[];
extern constext txt_filtering_arts[];
extern constext txt_last_resp[];
extern constext txt_listing_archive[];
extern constext txt_mail_art_to[];
extern constext txt_mail_bug_report[];
extern constext txt_mail_bug_report_confirm[];
extern constext txt_mail_quote[];
extern constext txt_mailed[];
extern constext txt_mailing_to[];
extern constext txt_mark_all_read[];
extern constext txt_mark_thread_read[];
extern constext txt_mark_group_read[];
extern constext txt_matching_cmd_line_groups[];
extern constext txt_mini_select_1[];
extern constext txt_mini_select_2[];
extern constext txt_mini_select_3[];
extern constext txt_mini_spooldir_1[];
extern constext txt_mini_group_1[];
extern constext txt_mini_group_2[];
extern constext txt_mini_group_3[];
extern constext txt_mini_thread_1[];
extern constext txt_mini_thread_2[];
extern constext txt_mini_page_1[];
extern constext txt_mini_page_2[];
extern constext txt_mini_page_3[];
extern constext txt_more[];
extern constext txt_more_percent[];
extern constext txt_moving[];
extern constext txt_news_quote[];
extern constext txt_newsgroup[];
extern constext txt_newsgroup_position[];
extern constext txt_next_resp[];
extern constext txt_nntp_authorization_failed[];
extern constext txt_nntp_to_fd_cannot_reopen[];
extern constext txt_nntp_to_fp_cannot_reopen[];
extern constext txt_no[];
extern constext txt_no_arts[];
extern constext txt_no_arts_posted[];
extern constext txt_no_command[];
extern constext txt_no_filename[];
extern constext txt_no_group[];
extern constext txt_no_groups[];
extern constext txt_no_groups_to_delete[];
extern constext txt_no_groups_to_read[];
extern constext txt_no_groups_to_yank_in[];
extern constext txt_no_index_file[];
extern constext txt_no_last_message[];
extern constext txt_no_mail_address[];
extern constext txt_no_match[];
extern constext txt_no_more_groups[];
extern constext txt_no_newsgroups[];
extern constext txt_no_next_unread_art[];
extern constext txt_no_prev_group[];
extern constext txt_no_prev_unread_art[];
extern constext txt_no_quick_newsgroups[];
extern constext txt_no_quick_subject[];
extern constext txt_no_resp[];
extern constext txt_no_responses[];
extern constext txt_no_resps_in_thread[];
extern constext txt_no_search_string[];
extern constext txt_no_spooldirs[];
extern constext txt_no_tagged_arts_to_save[];
extern constext txt_no_subject[];
extern constext txt_not_active_newsfeed[];
extern constext txt_not_in_active_file[];
extern constext txt_opt_autosave[];
extern constext txt_opt_catchup_groups[];
extern constext txt_opt_confirm_action[];
extern constext txt_opt_draw_arrow[];
extern constext txt_opt_maildir[];
extern constext txt_opt_mark_saved_read[];
extern constext txt_opt_page_scroll[];
extern constext txt_opt_pos_first_unread[];
extern constext txt_opt_post_process[];
extern constext txt_opt_print_header[];
extern constext txt_opt_printer[];
extern constext txt_opt_process_type[];
extern constext txt_opt_savedir[];
extern constext txt_opt_show_author[];
extern constext txt_opt_show_description[];
extern constext txt_opt_show_only_unread[];
extern constext txt_opt_sort_type[];
extern constext txt_opt_start_editor_offset[];
extern constext txt_opt_thread_arts[];
extern constext txt_option_not_enabled[];
extern constext txt_options_menu[];
extern constext txt_out_of_memory[];
extern constext txt_pgp_article[];
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
extern constext *txt_onoff[];
extern constext *txt_post_process[];
extern constext txt_post_process_type[];
extern constext txt_post_processing[];
extern constext txt_post_processing_failed[];
extern constext txt_post_processing_finished[];
extern constext txt_post_subject[];
extern constext txt_posting[];
extern constext txt_processing_xrefs[];
extern constext txt_purge[];
extern constext txt_printed[];
extern constext txt_printing[];
extern constext txt_quick_filter_kill[];
extern constext txt_quick_filter_select[];
extern constext txt_quit[];
extern constext txt_quit_delete[];
extern constext txt_quit_edit_post[];
extern constext txt_quit_edit_xpost[];
extern constext txt_quit_edit_save_killfile[];
extern constext txt_quit_edit_ispell_send[];
extern constext txt_quit_edit_send[];
extern constext txt_quit_despite_tags[];
extern constext txt_read_resp[];
extern constext txt_reading_all_arts[];
extern constext txt_reading_new_arts[];
extern constext txt_reading_all_groups[];
extern constext txt_reading_global_filter_file[];
extern constext txt_reading_filter_file[];
extern constext txt_reading_new_groups[];
extern constext txt_reading_article[];
extern constext txt_reading_news_active_file[];
extern constext txt_reading_mail_active_file[];
extern constext txt_reading_global_attributes_file[];
extern constext txt_reading_attributes_file[];
extern constext txt_reading_global_config_file[];
extern constext txt_reading_config_file[];
extern constext txt_reading_mailgroups_file[];
extern constext txt_reading_newsrc[];
extern constext txt_reading_newsgroups_file[];
extern constext txt_rereading_active_file[];
extern constext txt_reconnecting[];
extern constext txt_rejected_by_nntpserver[];
extern constext txt_rename_error[];
extern constext txt_reply_to_author[];
extern constext txt_repost_an_article[];
extern constext txt_repost_group[];
extern constext txt_reset_newsrc[];
extern constext txt_resizing_window[];
extern constext txt_resp_redirect[];
extern constext txt_resp_to_poster[];
extern constext txt_resp_x_of_n[];
extern constext txt_s_at_s[];
extern constext txt_save_filename[];
extern constext txt_save_pattern[];
extern constext txt_saved[];
extern constext txt_saved_pattern_to[];
extern constext txt_saved_to_mailbox[];
extern constext txt_saving[];
extern constext txt_screen_init_failed[];
extern constext txt_search_backwards[];
extern constext txt_search_forwards[];
extern constext txt_search_body[];
extern constext txt_searching[];
extern constext txt_searching_body[];
extern constext txt_select_art[];
extern constext txt_select_group[];
extern constext txt_select_config_file_option[];
extern constext txt_select_spooldir[];
extern constext txt_select_thread[];
extern constext txt_server_name_in_file_env_var[];
extern constext txt_shell_escape[];
extern constext *txt_show_from[];
extern constext txt_spooldir_selection[];
extern constext *txt_sort_type[];
extern constext txt_spooldir_server_error_1[];
extern constext txt_spooldir_server_error_2[];
extern constext txt_spooldirs_not_supported[];
extern constext txt_stuff_nntp_cannot_open[];
extern constext txt_subscribe_pattern[];
extern constext txt_subscribe_to_new_group[];
extern constext txt_subscribed_num_groups[];
extern constext txt_subscribed_to[];
extern constext txt_subscribing_to[];
extern constext txt_subscribing[];
extern constext txt_suspended_message[];
extern constext txt_tagged_art[];
extern constext txt_tagged_thread[];
extern constext txt_testing_archive[];
extern constext txt_there_is_no_news[];
extern constext *txt_thread[];
extern constext txt_threading_arts[];
extern constext txt_thread_com[];
extern constext txt_thread_marked_as_unread[];
extern constext txt_thread_not_saved[];
extern constext txt_thread_page[];
extern constext txt_thread_resp_page[];
extern constext txt_thread_saved_to[];
extern constext txt_thread_saved_to_many[];
extern constext txt_thread_x_of_n[];
extern constext txt_thread_marked_as_selected[];
extern constext txt_thread_marked_as_deselected[];
extern constext txt_toggled_rot13[];
extern constext txt_toggled_tex2iso_on[];
extern constext txt_toggled_tex2iso_off[];
extern constext txt_type_h_for_help[];
extern constext txt_unfiltering_arts[];
extern constext txt_unsubscribe_pattern[];
extern constext txt_unsubscribed_num_groups[];
extern constext txt_unsubscribed_to[];
extern constext txt_unsubscribing[];
extern constext txt_untagged_art[];
extern constext txt_untagged_thread[];
extern constext txt_unthreading_arts[];
extern constext txt_use_mime[];
extern constext txt_uudecoding[];
extern constext txt_writing_attributes_file[];
extern constext txt_writing_index_file[];
extern constext txt_xpost_quote[];
extern constext txt_x_resp[];
extern constext txt_yanking_all_groups[];
extern constext txt_yanking_sub_groups[];
extern constext txt_yes[];
extern constext txt_you_have_mail[];
extern constext txt_filter_text_type[];
extern constext txt_help_filter_subj[];
extern constext txt_help_filter_from[];
extern constext txt_help_filter_msgid[];
extern constext txt_help_filter_text[];
extern constext txt_help_filter_text_type[];
extern constext txt_kill_menu[];
extern constext txt_kill_subj[];
extern constext txt_kill_from[];
extern constext txt_kill_msgid[];
extern constext txt_kill_text[]; 
extern constext txt_kill_scope[];
extern constext txt_kill_lines[];
extern constext txt_kill_time[];
extern constext txt_help_kill_scope[];
extern constext txt_quit_edit_save_kill[];
extern constext txt_select_menu[];
extern constext txt_select_subj[];
extern constext txt_select_from[];
extern constext txt_select_msgid[];
extern constext txt_select_text[];
extern constext txt_select_scope[];
extern constext txt_select_lines[];
extern constext txt_select_time[];
extern constext txt_help_select_scope[];
extern constext txt_help_filter_lines[];
extern constext txt_help_filter_time[];
extern constext txt_quit_edit_save_select[];
extern constext txt_all_groups[];
extern constext txt_scope_group_only[];
extern constext txt_time_default_days[];
extern constext txt_unlimited_time[];
extern constext txt_subj_line_only[];
extern constext txt_subj_line_only_case[];
extern constext txt_from_line_only[];
extern constext txt_from_line_only_case[];
extern constext txt_msgid_line_only[];

extern int art_marked_deleted;
extern int art_marked_inrange;
extern int art_marked_return;
extern int art_marked_selected;
extern int art_marked_unread;
extern int auto_cc;
extern int auto_bcc;
extern int auto_list_thread;
extern int _hp_glitch;
extern int cCOLS, cLINES;
extern int MORE_POS;
extern int NOTESLINES;
extern int RIGHT_POS;
extern int *my_group;
extern int old_active_file_size;
extern int beginner_level;
extern int can_post;
extern int catchup;
extern int catchup_read_groups;
extern int check_for_new_newsgroups;
extern int cmd_line;
extern int compiled_with_nntp;
extern int confirm_action;
extern int confirm_to_quit;
extern int count_articles;
extern int created_rcdir;
extern int cur_active_size;
extern int cur_groupnum;
extern int debug;
extern int default_auto_save;
extern int default_auto_save_msg;
extern int default_batch_save;
extern int default_filter_days;
extern int default_filter_kill_header;
extern int default_filter_kill_global;
extern int default_filter_kill_expire;
extern int default_filter_kill_case;
extern int default_filter_select_header;
extern int default_filter_select_global;
extern int default_filter_select_expire;
extern int default_filter_select_case;
extern int default_move_group;
extern int default_post_proc_type;
extern int default_save_mode;
extern int default_show_author;
extern int default_show_only_unread;
extern int default_sort_art_type;
extern int default_thread_arts;
extern int delete_index_file;
#ifdef SIGTSTP
extern int do_sigtstp;
#endif
extern int draw_arrow_mark;
extern int force_screen_redraw;
extern int full_page_scroll;
extern int global_filtered_articles;
extern int group_catchup_on_exit;
extern int groupname_max_length;
extern int group_hash[TABLE_SIZE];
extern int group_top;
extern int groupname_len;
extern int index_file_killed;
extern int inverse_okay;
extern int iso2asc_supported;
extern int local_filtered_articles;
extern int local_index;
extern int mail_news;
extern int mark_saved_read;
extern int max_active;
extern int max_newnews;
extern int max_art;
extern int max_from;
extern int max_subj;
extern int max_kill;
extern int max_save;
extern int max_spooldir;
extern int newsrc_active;
extern int nntp_codeno;
extern int no_advertising;
extern int num_active;
extern int num_newnews;
extern int num_kill;
extern int num_of_selected_arts;
extern int num_of_killed_arts;
extern int num_of_tagged_arts;
extern int num_save;
extern int num_spooldir;
extern int post_article_and_exit;
extern int pos_first_unread;
extern int print_header;
extern int process_only_unread;
extern int purge_index_files;
extern int process_id;
extern int read_local_newsgroups_file;
extern int read_news_via_nntp;
extern int read_saved_news;
extern int real_gid;
extern int real_uid;
extern int real_umask;
extern int reread_active_file;
extern int save_news;
extern int save_to_mmdf_mailbox;
extern int show_author;
extern int show_xcommentto;
extern int highlight_xcommentto;
extern int show_description;
extern int show_last_line_prev_page;
extern int show_only_unread_groups;
extern int sigdashes;
extern int space_mode;
extern int start_editor_offset;
extern int start_line_offset;
extern int strip_blanks;
extern int system_status;
extern int tex2iso_supported;
extern int tin_gid;
extern int tin_uid;
extern int tab_after_X_selection;
extern int tab_goto_next_unread;
extern int space_goto_next_unread;
extern int thread_catchup_on_exit;
extern int top;
extern int top_base;
extern int unlink_article;
extern int update;
extern int use_builtin_inews;
extern int verbose;
extern int update_fork;
extern int use_keypad;
#ifdef HAVE_METAMAIL
extern int use_metamail;
extern int ask_for_metamail;
#endif
#ifdef HAVE_COLOR
extern int use_color;
extern int use_color_tinrc;
extern int col_back;
extern int col_invers;
extern int col_text;
extern int col_foot;
extern int col_quote;
extern int col_head;
extern int col_subject;
extern int col_response;
extern int col_from;
extern int col_normal;
extern int col_title;
#endif
extern int use_mouse;
extern int check_any_unread;
extern int start_any_unread;
extern int xref_supported;
extern int xover_supported;
extern int xuser_supported;
extern int xmouse;
/*
extern int xcut_and_paste;
*/
extern int xcol;
extern int xrow;
extern int reread_active_file_secs;

/* keep all dead articles in dead.articles */
extern int keep_dead_articles;

/* keep all posted articles in ~/Mail/posted */
extern int keep_posted_articles;

extern long *base;
extern time_t new_newnews_time;

extern struct passwd *myentry;
extern struct t_article *arts;
extern struct t_attribute glob_attributes;
extern struct t_filters glob_filter;
extern struct t_group *active;
extern struct t_newnews *newnews;
extern struct t_posted *posted;
extern struct t_save *save;
extern struct t_spooldir *spooldirs;
extern struct t_screen *screen;

#undef P_
