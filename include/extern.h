/*
 *  Project   : tin - a Usenet reader
 *  Module    : extern.h
 *  Author    : I. Lea
 *  Created   : 1991-04-01
 *  Updated   : 1998-07-13
 *  Notes     :
 *  Copyright : (c) Copyright 1991-99 by Iain Lea
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

#ifdef USE_TERMINFO
#define tigetnum(s) tgetnum(s)
#endif

/*
 * The prototypes bracketed by DECL_xxxx ifdef's are used to get moderately
 * clean compiles on systems with pre-ANSI/POSIX headers when compiler warnings
 * are enabled.  (Not all of the functions are ANSI or POSIX).
 */
#ifdef DECL__FLSBUF
	extern int _flsbuf (int, FILE *);
#endif /* DECL__FLSBUF */
#ifdef DECL_ATOI
	extern int atoi (char *);
#endif /* DECL_ATOI */
#ifdef DECL_ATOL
	extern long atol (char *);
#endif /* DECL_ATOL */
#ifndef HAVE_MEMCPY
#	ifdef DECL_BCOPY
		extern int bcopy (char *, char *, int);
#	endif /* DECL_BCOPY */
#endif /* !HAVE_MEMCPY */
#if 0 /* trouble on Linux/gcc 3.1 */
#ifdef DECL_BZERO /* FD_ZERO() might use this */
	extern void bzero (char *, int);
#endif /* DECL_BZERO */
#endif /* 0 */
#ifdef DECL_CALLOC
	extern void *calloc (size_t, size_t);
#endif /* DECL_CALLOC */
#ifdef DECL_FCLOSE
	extern int fclose (FILE *);
#endif /* DECL_FCLOSE */
#ifdef DECL_FDOPEN
	extern FILE *fdopen (int, const char *);
#endif /* DECL_FDOPEN */
#ifdef DECL_FFLUSH
	extern int fflush (FILE *);
#endif /* DECL_FFLUSH */
#ifdef DECL_FGETC
	extern int fgetc (FILE *);
#endif /* DECL_FGETC */
#if defined(DECL_FILENO) && !defined(fileno)
	extern int fileno (FILE *);
#endif /* DECL_FILENO && !fileno */
#ifdef DECL_FPRINTF
	extern int fprintf (FILE *, const char *, ...);
#endif /* DECL_FPRINTF */
#ifdef DECL_FPUTC
	extern int fputc (int, FILE *);
#endif /* DECL_FPUTC */
#ifdef DECL_FPUTS
	extern int fputs (const char *, FILE *);
#endif /* DECL_FPUTS */
#ifdef DECL_FREAD
	extern size_t fread (void *, size_t, size_t, FILE *);
#endif /* DECL_FREAD */
#ifdef DECL_FREE
	extern void free (void *);
#endif /* DECL_FREE */
#ifdef DECL_FSEEK
	extern int fseek (FILE *, long, int);
#endif /* DECL_FSEEK */
#ifdef DECL_FWRITE
	extern size_t fwrite (void *, size_t, size_t, FILE *);
#endif /* DECL_FWRITE */
#ifdef DECL_GETCWD
	extern char *getcwd (char *, size_t);
#endif /* DECL_GETCWD */
#ifdef DECL_GETENV
	extern char *getenv (const char *);
#endif /* DECL_GETENV */
#ifdef DECL_GETHOSTBYNAME
	extern struct hostent *gethostbyname (const char *);
#endif /* DECL_GETHOSTBYNAME */
#ifdef DECL_GETHOSTNAME
	extern int gethostname (char *, int);
#endif /* DECL_GETHOSTNAME */
#ifdef DECL_GETLOGIN
	extern char *getlogin (void);
#endif /* DECL_GETLOGIN */
#ifdef DECL_GETOPT
	extern int getopt (int, char * const*, const char *);
#endif /* DECL_GETOPT */
#ifdef DECL_GETPASS
	extern char *getpass(const char *);
#endif /* DECL_GETPASS */
#ifdef DECL_GETPWNAM
	extern struct passwd *getpwnam (const char *);
#endif /* DECL_GETPWNAM */
#ifdef DECL_GETSERVBYNAME
	extern struct servent *getservbyname (const char *, const char *);
#endif /* DECL_GETSERVBYNAME */
#ifdef DECL_INET_ADDR
	extern unsigned long inet_addr (const char *);
#endif /* DECL_INET_ADDR */
#ifdef DECL_IOCTL
	extern int ioctl (int, int, void *);
#endif /* DECL_IOCTL */
#if defined(DECL_ISASCII) && !defined(isascii)
	extern int isascii (int);
#endif /* DECL_ISASCII && !isascii */
#ifdef DECL_MALLOC
	extern void *malloc (size_t);
#endif /* DECL_MALLOC */

#if 0 /* this breaks egcs 2.91.66/gcc 2.95.1 on SuSE 6.2 */
#	ifdef DECL_MEMSET
		extern void *memset (void *, int, size_t);
#	endif /* DECL_MEMSET */
#endif /* 0 */

#ifdef DECL_MKSTEMP
	extern int mktemp (char *);
#endif /* DECL_MKSTEMP */
#ifdef DECL_MKTEMP
	extern char *mktemp (char *);
#endif /* DECL_MKTEMP */
#ifdef DECL_PCLOSE
	extern int pclose (FILE *);
#endif /* DECL_PCLOSE */
#ifdef DECL_PERROR
	extern void perror (const char *);
#endif /* DECL_PERROR */
#ifdef DECL_POPEN
	extern FILE *popen (const char *, const char *);
#endif /* DECL_POPEN */
#ifdef DECL_PRINTF
	extern int printf (const char *, ...);
#endif /* DECL_PRINTF */
#ifdef DECL_PUTENV
	extern int putenv (const char *);
#endif /* DECL_PUTENV */
#ifdef DECL_QSORT
	extern void qsort (void *, size_t, size_t, int (*)(t_comptype*, t_comptype*));
#endif /* DECL_QSORT */
#ifdef DECL_REALLOC
	extern void *realloc (void *, size_t);
#endif /* DECL_REALLOC */
#ifdef DECL_RENAME
	extern int rename (const char *, const char *);
#endif /* DECL_RENAME */
#ifdef DECL_REWIND
	extern void rewind (FILE *);
#endif /* DECL_REWIND */
#ifdef DECL_SELECT
	extern int select (int, fd_set *, fd_set *, fd_set *, struct timeval *);
#endif /* DECL_SELECT */
#ifdef DECL_SETEGID
	extern int setegid (gid_t);
#endif /* DECL_SETEGID */
#ifndef HAVE_PUTENV
#	ifdef DECL_SETENV
		extern int setenv (const char *, const char *, int);
#	endif /* DECL_SETENV */
#endif /* !HAVE_PUTENV */
#ifdef DECL_SETEUID
	extern int seteuid (uid_t);
#endif /* DECL_SETEUID */
#ifdef DECL_SETPGRP
#	ifdef SETPGRP_VOID
		extern pid_t setpgrp (void);
#	else
		extern int setpgrp (int, int);
#	endif /* SETPGRP_VOID */
#endif /* DECL_SETPGRP */
#ifndef HAVE_SETEGID
#	ifdef DECL_SETREGID
		extern int setregid (gid_t, gid_t);
#	endif /* DECL_SETREGID */
#endif /* !HAVE_SETEGID */
#ifndef HAVE_SETEUID
#	ifdef DECL_SETREUID
		extern int setreuid (uid_t, uid_t);
#	endif /* DECL_SETREUID */
#endif /* !HAVE_SETEUID */
#ifdef DECL_SOCKET
	extern int socket (int, int, int);
#endif /* DECL_SOCKET */
#ifdef DECL_SSCANF
	extern int sscanf (const char *, const char *, ...);
#endif /* DECL_SSCANF */
#ifdef DECL_STRCASECMP
	extern int strcasecmp (const char *, const char *);
#endif /* DECL_STRCASECMP */
#ifdef DECL_STRCHR
	extern char *strchr (const char *, int);
	extern char *strrchr (const char *, int);
#endif /* DECL_STRCHR */
#ifdef DECL_STRFTIME
	extern int strftime (char *, int, char *, struct tm *);
#endif /* DECL_STRFTIME */
#ifdef DECL_STRNCASECMP
	extern int strncasecmp (const char *, const char *, size_t);
#endif /* DECL_STRNCASECMP */
#ifdef DECL_STRTOL
	extern long strtol (const char *, char **, int);
#endif /* DECL_STRTOL */
#ifdef DECL_SYSTEM
	extern int system (char *);
#endif /* DECL_SYSTEM */
#ifdef DECL_TEMPNAM
	extern char *tempnam (const char *, const char *);
#endif /* DECL_TEMPNAM */
#ifdef DECL_TGETENT
	extern int tgetent (char *, char *);
#endif /* DECL_TGETENT */
#ifdef DECL_TGETFLAG
	extern int tgetflag (char *);
#endif /* DECL_TGETFLAG */
#if defined(DECL_TGETNUM) || defined(DECL_TIGETNUM)
	extern int tgetnum (char *);
#endif /* DECL_TGETNUM */
#ifdef DECL_TGETSTR
	extern char *tgetstr (char *, char **);
#endif /* DECL_TGETSTR */
#ifdef DECL_TGOTO
	extern char *tgoto (char *, int, int);
#endif /* DECL_TGOTO */
#ifdef DECL_TIME
	extern time_t time (time_t *);
#endif /* DECL_TIME */
#if defined(DECL_TOLOWER) && !defined(tolower)
	extern int tolower (int);
#endif /* DECL_TOLOWER && !tolower */
#if defined(DECL_TOUPPER) && !defined(toupper)
	extern int toupper (int);
#endif /* DECL_TOUPPER && !toupper */
#ifdef DECL_TPUTS
	extern int tputs (char *, int, OutcPtr);
#endif /* DECL_TPUTS */
#ifdef DECL_UNGETC
	extern int ungetc (int, FILE *);
#endif /* DECL_UNGETC */
#ifdef DECL_USLEEP
	extern void usleep(unsigned long);
#endif /* DECL_USLEEP */
#ifdef DECL_VSPRINTF
	extern int vsprintf(char *, char *, va_list);
#endif /* DECL_VSPRINTF */

extern int optind;
extern char *optarg;

/*
 * Local variables
 */

/* This fixes ambiguities on platforms that don't distinguish extern case */
#ifdef CASE_PROBLEM
#	define txt_help_B	txt_help_BIGB
#	define txt_help_D	txt_help_BIGD
#	define txt_help_I	txt_help_BIGI
#	define txt_help_K	txt_help_BIGK
#	define txt_help_L	txt_help_BIGL
#	define txt_help_M	txt_help_BIGM
#	define txt_help_Q	txt_help_BIGQ
#	define txt_help_S	txt_help_BIGS
#	define txt_help_T	txt_help_BIGT
#	define txt_help_U	txt_help_BIGU
#	define txt_help_W	txt_help_BIGW
#	define txt_help_X	txt_help_BIGX
#	define txt_help_p_S	txt_help_p_BIGS
#	define txt_help_t_K	txt_help_t_BIGK
#	define txt_help_thd_C	txt_help_thd_BIGC
#endif /* CASE_PROBLEM */

extern FILE *note_fp;				/* body of current article */
extern char **news_headers_to_display_array;
extern char **news_headers_to_not_display_array;
extern char *OPT_CHAR_list[];
extern char *OPT_STRING_list[];
extern char *glob_group;
extern char *nntp_server;
extern char active_times_file[PATH_LEN];
extern char bug_addr[LEN];
extern char bug_nntpserver1[PATH_LEN];
extern char bug_nntpserver2[PATH_LEN];
extern char ch_post_process[];
extern char cmdline_nntpserver[PATH_LEN];
extern char cvers[LEN];
extern char dead_article[PATH_LEN];
extern char dead_articles[PATH_LEN];
extern char default_organization[PATH_LEN];
extern char default_signature[PATH_LEN];
extern char global_attributes_file[PATH_LEN];
extern char global_config_file[PATH_LEN];
extern char homedir[PATH_LEN];
extern char index_maildir[PATH_LEN];
extern char index_newsdir[PATH_LEN];
extern char index_savedir[PATH_LEN];
extern char inewsdir[PATH_LEN];
extern char last_put[];
extern char libdir[PATH_LEN];
extern char local_attributes_file[PATH_LEN];
extern char local_config_file[PATH_LEN];
extern char filter_file[PATH_LEN];
extern char local_input_history_file[PATH_LEN];
extern char local_newsgroups_file[PATH_LEN];
extern char local_newsrctable_file[PATH_LEN];
extern char lock_file[PATH_LEN];
extern char mail_active_file[PATH_LEN];
extern char mail_news_user[LEN];
extern char mailbox[PATH_LEN];
extern char mailer[PATH_LEN];
extern char mailgroups_file[PATH_LEN];
extern char mesg[LEN];
extern char msg_headers_file[PATH_LEN];
extern char my_distribution[LEN];
extern char newnewsrc[PATH_LEN];
extern char news_active_file[PATH_LEN];
extern char newsgroups_file[PATH_LEN];
extern char newsrc[PATH_LEN];
extern char novrootdir[PATH_LEN];
extern char novfilename[PATH_LEN];
extern char page_header[LEN];
extern char posted_info_file[PATH_LEN];
extern char posted_msgs_file[PATH_LEN];
extern char postponed_articles_file[PATH_LEN];
extern char proc_ch_default;
extern char tin_progname[PATH_LEN];
extern char rcdir[PATH_LEN];
extern char reply_to[LEN];
extern char save_active_file[PATH_LEN];
extern char spooldir[PATH_LEN];
extern char subscriptions_file[PATH_LEN];
extern char txt_help_bug_report[LEN];
extern char userid[PATH_LEN];

extern char domain_name[];
extern char host_name[];

extern const char base64_alphabet[64];
extern const char **info_help;
extern constext *help_group[];
extern constext *help_page[];
extern constext *help_select[];
extern constext *help_thread[];
extern constext *txt_colors[];
extern constext *txt_kill_level_type[];
extern constext *txt_marks[];
extern constext *txt_mime_encodings[NUM_MIME_ENCODINGS];
extern constext *txt_onoff[];
extern constext *txt_post_process_type[];
extern constext *txt_show_from[];
extern constext *txt_sort_type[];
extern constext *txt_strip_bogus_type[];
extern constext *txt_thread[];
extern constext *txt_wildcard_type[];
extern constext txt_1_resp[];
extern constext txt_7bit[];
extern constext txt_8bit[];
extern constext txt_active_file_is_empty[];
extern constext txt_added_groups[];
extern constext txt_all_groups[];
extern constext txt_append_overwrite_quit[];
extern constext txt_art_cancel[];
extern constext txt_art_cannot_cancel[];
extern constext txt_art_newsgroups[];
extern constext txt_art_not_posted[];
extern constext txt_art_not_saved[];
extern constext txt_art_pager_com[];
extern constext txt_art_parent_killed[];
extern constext txt_art_parent_none[];
extern constext txt_art_parent_unavail[];
extern constext txt_art_posted[];
extern constext txt_art_rejected[];
extern constext txt_art_saved_to[];
extern constext txt_art_thread_regex_tag[];
extern constext txt_art_unavailable[];
extern constext txt_article_cancelled[];
extern constext txt_article_cannot_open[];
extern constext txt_article_cannot_reopen[];
extern constext txt_article_reposted[];
extern constext txt_at_s[];
extern constext txt_auth_failed[];
extern constext txt_auth_pass[];
extern constext txt_auth_user[];
extern constext txt_auth_needed[];
extern constext txt_author_search_backwards[];
extern constext txt_author_search_forwards[];
extern constext txt_authorization_fail[];
extern constext txt_authorization_ok[];
extern constext txt_autoselecting_articles[];
extern constext txt_autosubscribed[];
extern constext txt_autosubscribing_groups[];
extern constext txt_bad_active_file[];
extern constext txt_bad_article[];
extern constext txt_bad_command[];
extern constext txt_base64[];
extern constext txt_batch_update_unavail[];
extern constext txt_batch_update_failed[];
extern constext txt_begin_of_art[];
extern constext txt_caching_disabled[];
extern constext txt_cancel_article[];
extern constext txt_cancelling_art[];
extern constext txt_cannot_create[];
extern constext txt_cannot_create_uniq_name[];
extern constext txt_cannot_find_base_art[];
extern constext txt_cannot_get_nntp_server_name[];
extern constext txt_cannot_get_term[];
extern constext txt_cannot_get_term_entry[];
extern constext txt_cannot_open[];

#if defined(NNTP_ABLE) || defined(NNTP_ONLY)
	extern constext txt_cannot_open_active_file[];
#endif /* NNTP_ABLE || NNTP_ONLY */

extern constext txt_cannot_post[];
extern constext txt_cannot_post_group[];
extern constext txt_cannot_write_index[];
extern constext txt_cannot_write_to_directory[];
extern constext txt_catchup_all_read_groups[];
extern constext txt_catchup_despite_tags[];
extern constext txt_catchup_update_info[];
extern constext txt_check_article[];
extern constext txt_checking_for_news[];
extern constext txt_checking_new_groups[];
extern constext txt_checksum_of_file[];
extern constext txt_choose_post_process_type[];
extern constext txt_color_off[];
extern constext txt_color_on[];
extern constext txt_command_failed[];
extern constext txt_connecting[];
extern constext txt_connecting_port[];
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
extern constext txt_enter_next_thread[];
extern constext txt_enter_option_num[];
extern constext txt_enter_range[];
extern constext txt_error_approved[];
extern constext txt_error_asfail[];
extern constext txt_error_bad_approved[];
extern constext txt_error_bad_from[];
extern constext txt_error_bad_replyto[];
extern constext txt_error_bad_msgidfqdn[];
extern constext txt_error_copy_fp[];
extern constext txt_error_corrupted_file[];
extern constext txt_error_gnksa_internal[];
extern constext txt_error_gnksa_langle[];
extern constext txt_error_gnksa_lparen[];
extern constext txt_error_gnksa_rparen[];
extern constext txt_error_gnksa_atsign[];
extern constext txt_error_gnksa_sgl_domain[];
extern constext txt_error_gnksa_inv_domain[];
extern constext txt_error_gnksa_ill_domain[];
extern constext txt_error_gnksa_unk_domain[];
extern constext txt_error_gnksa_fqdn[];
extern constext txt_error_gnksa_zero[];
extern constext txt_error_gnksa_length[];
extern constext txt_error_gnksa_hyphen[];
extern constext txt_error_gnksa_begnum[];
extern constext txt_error_gnksa_bad_lit[];
extern constext txt_error_gnksa_local_lit[];
extern constext txt_error_gnksa_rbracket[];
extern constext txt_error_gnksa_lp_missing[];
extern constext txt_error_gnksa_lp_invalid[];
extern constext txt_error_gnksa_lp_zero[];
extern constext txt_error_gnksa_rn_unq[];
extern constext txt_error_gnksa_rn_qtd[];
extern constext txt_error_gnksa_rn_enc[];
extern constext txt_error_gnksa_rn_encsyn[];
extern constext txt_error_gnksa_rn_paren[];
extern constext txt_error_gnksa_rn_invalid[];
extern constext txt_error_header_and_body_not_separate[];
extern constext txt_error_header_duplicate[];
extern constext txt_error_header_line_bad_charset[];
extern constext txt_error_header_line_bad_encoding[];
extern constext txt_error_header_line_blank[];
extern constext txt_error_header_line_colon[];
extern constext txt_error_header_line_comma[];
extern constext txt_error_header_line_empty_subject[];
extern constext txt_error_header_line_empty_newsgroups[];
extern constext txt_error_header_line_groups_contd[];
extern constext txt_error_header_line_missing_newsgroups[];
extern constext txt_error_header_line_missing_subject[];
extern constext txt_error_header_line_missing_target[];
extern constext txt_error_header_line_space[];
extern constext txt_error_no_domain_name[];
extern constext txt_error_no_enter_permission[];
extern constext txt_error_no_from[];
extern constext txt_error_no_read_permission[];
extern constext txt_error_no_such_file[];
extern constext txt_error_no_write_permission[];
extern constext txt_error_sender_in_header_not_allowed[];
extern constext txt_exiting[];
extern constext txt_external_mail_done[];
extern constext txt_extracting_archive[];
extern constext txt_extracting_shar[];
extern constext txt_failed_to_connect_to_server[];
extern constext txt_feed_pattern[];
extern constext txt_filesystem_full[];
extern constext txt_filesystem_full_backup[];
extern constext txt_filter_file[];
extern constext txt_filter_global_rules[];
extern constext txt_filter_score[];
extern constext txt_filter_score_help[];
extern constext txt_filter_text_type[];
extern constext txt_followup_newsgroups[];
extern constext txt_followup_poster[];
extern constext txt_from_line_only[];
extern constext txt_from_line_only_case[];
extern constext txt_full[];
extern constext txt_group[];
extern constext txt_group_is_moderated[];
extern constext txt_group_select_com[];
extern constext txt_group_selection[];
extern constext txt_help_disp[];
extern constext txt_help_disp_[];
extern constext txt_help_navi[];
extern constext txt_help_navi_[];
extern constext txt_help_misc[];
extern constext txt_help_misc_[];
extern constext txt_help_ops[];
extern constext txt_help_ops_[];
extern constext txt_help_B[];
extern constext txt_help_D[];
extern constext txt_help_I[];
extern constext txt_help_K[];
extern constext txt_help_L[];
extern constext txt_help_M[];
extern constext txt_help_Q[];
extern constext txt_help_S[];
extern constext txt_help_T[];
extern constext txt_help_U[];
extern constext txt_help_W[];
extern constext txt_help_X[];
extern constext txt_help_a[];
extern constext txt_help_b[];
extern constext txt_help_bug[];
extern constext txt_help_cC[];
extern constext txt_help_c[];
extern constext txt_help_colon[];
extern constext txt_help_ctrl_a[];
extern constext txt_help_ctrl_d[];
extern constext txt_help_ctrl_f[];
extern constext txt_help_ctrl_h[];
extern constext txt_help_ctrl_k[];
extern constext txt_help_ctrl_l[];
extern constext txt_help_ctrl_n[];
extern constext txt_help_ctrl_o[];
extern constext txt_help_ctrl_t[];
extern constext txt_help_d[];
extern constext txt_help_dash[];
extern constext txt_help_e[];
extern constext txt_help_equal[];
extern constext txt_help_esc[];
extern constext txt_help_filter_from[];
extern constext txt_help_filter_lines[];
extern constext txt_help_filter_msgid[];
extern constext txt_help_filter_subj[];
extern constext txt_help_filter_text[];
extern constext txt_help_filter_text_type[];
extern constext txt_help_filter_time[];
extern constext txt_help_g[];
extern constext txt_help_g_caret_dollar[];
extern constext txt_help_g_cr[];
extern constext txt_help_g_G[];
extern constext txt_help_g_ctrl_r[];
extern constext txt_help_g_d[];
extern constext txt_help_g_hash[];
extern constext txt_help_g_num[];
extern constext txt_help_g_q[];
extern constext txt_help_g_r[];
extern constext txt_help_g_search[];
extern constext txt_help_g_T[];
extern constext txt_help_g_tab[];
extern constext txt_help_g_x[];
extern constext txt_help_g_y[];
extern constext txt_help_g_z[];
extern constext txt_help_h[];
extern constext txt_help_hash[];
extern constext txt_help_i[];
extern constext txt_help_i_caret_dollar[];
extern constext txt_help_i_at[];
extern constext txt_help_i_cr[];
extern constext txt_help_i_dot[];
extern constext txt_help_i_n[];
extern constext txt_help_i_num[];
extern constext txt_help_i_p[];
extern constext txt_help_i_search[];
extern constext txt_help_i_star[];
extern constext txt_help_i_tab[];
extern constext txt_help_i_tilda[];
extern constext txt_help_j[];
extern constext txt_help_kill_scope[];
extern constext txt_help_l[];
extern constext txt_help_m[];
extern constext txt_help_n[];

#ifndef DISABLE_PRINTING
	extern constext txt_help_o[];
#endif /* !DISABLE_PRINTING */

extern constext txt_help_p_S[];
extern constext txt_help_p_caret_dollar[];
extern constext txt_help_p_at[];
extern constext txt_help_p_cr[];
extern constext txt_help_p_ctrl_e[];
extern constext txt_help_p_ctrl_w[];
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

#ifndef DONT_HAVE_PIPING
	extern constext txt_help_pipe[];
#endif /* !DONT_HAVE_PIPING */

extern constext txt_help_plus[];

extern constext txt_help_q[];
extern constext txt_help_quick_kill[];
extern constext txt_help_quick_select[];

extern constext txt_help_r[];
extern constext txt_help_s[];
extern constext txt_help_s_i[];
extern constext txt_help_sel_c[];
extern constext txt_help_select_scope[];
extern constext txt_help_semicolon[];
extern constext txt_help_t[];
extern constext txt_help_t_caret_dollar[];
extern constext txt_help_t_cr[];
extern constext txt_help_t_num[];
extern constext txt_help_thd_c[];
extern constext txt_help_thd_C[];
extern constext txt_help_thd_K[];
extern constext txt_help_tex[];
extern constext txt_help_thread[];
extern constext txt_help_u[];
extern constext txt_help_v[];
extern constext txt_help_w[];
extern constext txt_help_x[];
extern constext txt_help_y[];
extern constext txt_hit_space_for_more[];
extern constext txt_index_page_com[];
extern constext txt_info_add_kill[];
extern constext txt_info_add_select[];
extern constext txt_info_do_postpone[];
extern constext txt_info_nopostponed[];
extern constext txt_info_not_subscribed[];
extern constext txt_info_no_write[];
extern constext txt_info_postponed[];
extern constext txt_info_x_conversion_note[];
extern constext txt_invalid_from[];
extern constext txt_invalid_sender[];
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
extern constext txt_last[];
extern constext txt_last_resp[];

#ifdef HAVE_LIBUU
	extern constext txt_libuu_saved[];
	extern constext txt_libuu_success[];
	extern constext txt_libuu_error_decode[];
	extern constext txt_libuu_error_missing[];
	extern constext txt_libuu_error_no_begin[];
	extern constext txt_libuu_error_no_end[];
	extern constext txt_libuu_error_no_data[];
	extern constext txt_libuu_error_unknown[];
#endif /* HAVE_LIBUU */

extern constext txt_lines[];
extern constext txt_listing_archive[];
extern constext txt_mail[];
extern constext txt_mail_art_to[];
extern constext txt_mail_bug_report[];
extern constext txt_mail_bug_report_confirm[];
extern constext txt_mailed[];
extern constext txt_mailing_to[];
extern constext txt_mark_arts_read[];
extern constext txt_mark_group_read[];
extern constext txt_mark_thread_read[];
extern constext txt_marked_as_unread[];
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
extern constext txt_msg_headers_file[];
extern constext txt_msgid_line_only[];
extern constext txt_newsgroup[];
extern constext txt_newsgroup_position[];
extern constext txt_newsrc_again[];
extern constext txt_newsrc_nogroups[];
extern constext txt_newsrc_saved[];
extern constext txt_next_resp[];
extern constext txt_nntp_authorization_failed[];
#ifdef NNTP_ABLE
	extern constext txt_nntp_ok_goodbye[];
#endif /* NNTP_ABLE */
extern constext txt_no[];
extern constext txt_no_arts[];
extern constext txt_no_arts_posted[];

#ifndef DONT_HAVE_PIPING
	extern constext txt_no_command[];
#endif /* !DONT_HAVE_PIPING */

extern constext txt_no_description[];
extern constext txt_no_filename[];
extern constext txt_no_group[];
extern constext txt_no_groups[];
extern constext txt_no_groups_to_read[];
extern constext txt_no_groups_to_yank_in[];
extern constext txt_no_last_message[];
extern constext txt_no_mail_address[];
extern constext txt_no_match[];
extern constext txt_no_more_groups[];
extern constext txt_no_newsgroups[];
extern constext txt_no_next_unread_art[];
extern constext txt_no_prev_group[];
extern constext txt_no_prev_unread_art[];
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
extern constext txt_no_xover_support[];
extern constext txt_not_exist[];
extern constext txt_not_in_active_file[];
extern constext txt_nrctbl_create[];
extern constext txt_nrctbl_default[];
extern constext txt_nrctbl_info[];
extern constext txt_only[];

extern constext txt_option_not_enabled[];
extern constext txt_options_menu[];
extern constext txt_out_of_memory[];
extern constext txt_pcre_error_at[];
extern constext txt_pcre_error_num[];
extern constext txt_pcre_error_text[];
extern constext txt_plural[];
extern constext txt_post_a_followup[];
extern constext txt_post_error_ask_postpone[];
extern constext txt_post_history_menu[];
extern constext txt_post_newsgroups[];
extern constext txt_post_processing[];
extern constext txt_post_processing_failed[];
extern constext txt_post_processing_finished[];
extern constext txt_post_subject[];
extern constext txt_posted_info_file[];
extern constext txt_posting[];
extern constext txt_postpone_repost[];

#ifndef DISABLE_PRINTING
	extern constext txt_print[];
	extern constext txt_printed[];
	extern constext txt_printing[];
#endif /* !DISABLE_PRINTING */

extern constext txt_prompt_fup_ignore[];
extern constext txt_prompt_unchanged_art[];
extern constext txt_prompt_unchanged_bug[];
extern constext txt_prompt_see_postponed[];
extern constext txt_quick_filter_kill[];
extern constext txt_quick_filter_select[];
extern constext txt_quit[];
extern constext txt_quit_cancel[];
extern constext txt_quit_despite_tags[];
extern constext txt_quit_edit_post[];
extern constext txt_quit_edit_postpone[];
extern constext txt_quit_edit_save_kill[];
extern constext txt_quit_edit_save_select[];
extern constext txt_quit_edit_send[];
extern constext txt_quit_edit_xpost[];
extern constext txt_quit_no_write[];
extern constext txt_quoted_printable[];
extern constext txt_range_invalid[];
extern constext txt_read_abort[];
extern constext txt_read_exit[];
extern constext txt_read_resp[];
extern constext txt_reading_article[];
extern constext txt_reading_arts[];
extern constext txt_reading_attributes_file[];
extern constext txt_reading_config_file[];
extern constext txt_reading_filter_file[];
extern constext txt_reading_groups[];
extern constext txt_reading_input_history_file[];
extern constext txt_reading_news_active_file[];
extern constext txt_reading_news_newsrc_file[];
extern constext txt_reading_newsgroups_file[];
extern constext txt_reading_newsrc[];
extern constext txt_reconnect_to_news_server[];
extern constext txt_remove_bogus[];
extern constext txt_rename_error[];
extern constext txt_reply_to_author[];
extern constext txt_repost[];
extern constext txt_repost_an_article[];
extern constext txt_repost_group[];
extern constext txt_reset_newsrc[];
extern constext txt_resp_redirect[];
extern constext txt_resp_to_poster[];
extern constext txt_resp_x_of_n[];
extern constext txt_return_key[];
extern constext txt_save[];
extern constext txt_save_config[];
extern constext txt_save_filename[];
extern constext txt_saved[];
extern constext txt_saved_arts[];
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
extern constext txt_servers_active[];

#ifndef NO_SHELL_ESCAPE
	extern constext txt_shell_escape[];
#endif /* !NO_SHELL_ESCAPE */

extern constext txt_skipping_newgroups[];
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
extern constext txt_thread_not_saved[];
extern constext txt_thread_page[];
extern constext txt_thread_resp_page[];
extern constext txt_thread_saved_to_many[];
extern constext txt_thread_x_of_n[];
extern constext txt_threading_arts[];
extern constext txt_time_default_days[];

extern constext txt_tinrc_defaults[];
extern constext txt_tinrc_filter[];
extern constext txt_tinrc_header[];
extern constext txt_tinrc_info_in_last_line[];
extern constext txt_tinrc_newnews[];
extern constext txt_tinrc_post_process_command[];

extern constext txt_toggled_rot13[];
extern constext txt_toggled_tex2iso[];
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
extern constext txt_warn_blank_subject[];
extern constext txt_warn_cancel[];
extern constext txt_warn_encoding_and_external_inews[];

#ifndef HAVE_FASCIST_NEWSADMIN
	extern constext txt_warn_followup_to_several_groups[];
	extern constext txt_warn_missing_followup_to[];
	extern constext txt_warn_not_in_newsrc[];
	extern constext txt_warn_not_valid_newsgroup[];
#endif /* !HAVE_FASCIST_NEWSADMIN */

extern constext txt_warn_newsrc[];
extern constext txt_warn_multiple_sigs[];
extern constext txt_warn_sig_too_long[];
extern constext txt_warn_suspicious_mail[];
extern constext txt_warn_update[];
extern constext txt_warn_wrong_sig_format[];
extern constext txt_warn_xref_not_supported[];
extern constext txt_writing_attributes_file[];
extern constext txt_x_resp[];
extern constext txt_yanking_all_groups[];
extern constext txt_yanking_sub_groups[];
extern constext txt_yes[];
extern constext txt_you_have_mail[];

extern int *my_group;
extern int MORE_POS;
extern int NOTESLINES;
extern int RIGHT_POS;
extern int _hp_glitch;
extern int cCOLS;
extern int cLINES;
extern int cur_groupnum;
extern int debug;
extern int glob_respnum;
extern int group_hash[TABLE_SIZE];
extern int group_top;
extern int groupname_len;
extern int index_point;
extern int iso2asc_supported;
extern int last_resp;
extern int max_active;
extern int max_art;
extern int max_from;
extern int max_newnews;
extern int max_save;
extern int num_headers_to_display;
extern int num_headers_to_not_display;
extern int max_subj;
extern int need_resize;
extern int note_line;
extern int note_page;				/* what page we're on */
extern int num_active;
extern int num_newnews;
extern int num_of_killed_arts;
extern int num_of_selected_arts;
extern int num_of_tagged_arts;
extern int num_save;
extern int signal_context;
extern int system_status;
extern int this_resp;
extern int thread_basenote;
extern int tin_errno;
extern int top;
extern int top_base;
extern int xcol;
extern int xmouse;
extern int xrow;

extern long *base;
extern long head_next;
extern long note_mark[MAX_PAGES];	/* ftells on beginnings of pages */
extern long mark_body;					/* ftell on beginning of body */
extern long note_size;

extern signed long int read_newsrc_lines;

extern gid_t real_gid;
extern gid_t tin_gid;
extern mode_t real_umask;
extern pid_t process_id;
extern uid_t real_uid;
extern uid_t tin_uid;

#ifdef HAVE_COLOR
	extern struct regex_cache quote_regex;
	extern struct regex_cache quote_regex2;
	extern struct regex_cache quote_regex3;
#endif /* HAVE_COLOR */

extern struct regex_cache strip_re_regex;
extern struct regex_cache strip_was_regex;

extern struct t_article *arts;
extern struct t_config tinrc;
extern struct t_filters glob_filter;
extern struct t_group *active;
extern struct t_header note_h;
extern struct t_newnews *newnews;
extern struct t_option option_table[];
extern struct t_posted *posted;
extern struct t_save *save;
extern t_bool can_post;
extern t_bool catchup;
extern t_bool check_any_unread;
extern t_bool check_for_new_newsgroups;
extern t_bool cmd_line;
extern t_bool created_rcdir;
extern t_bool dangerous_signal_exit; /* TRUE if SIGHUP, SIGTERM, SIGUSR1 */
#ifdef INDEX_DAEMON
	extern t_bool delete_index_file;
#endif /* INDEX_DAEMON */
extern t_bool disable_gnksa_domain_check;
extern t_bool disable_sender;
extern t_bool do_rfc1521_decoding;
extern t_bool force_reread_active_file;
extern t_bool got_sig_pipe;
extern t_bool in_headers; /* colorful headers */
extern t_bool filtered_articles;
extern t_bool local_index;
extern t_bool mail_news;
extern t_bool list_active;
extern t_bool newsrc_active;
extern t_bool note_end;					/* end of article ? */
extern t_bool no_write;
extern t_bool post_article_and_exit;
extern t_bool post_postponed_and_exit;
extern t_bool purge_index_files;
extern t_bool read_local_newsgroups_file;
extern t_bool read_news_via_nntp;
extern t_bool read_saved_news;
extern t_bool reconnected_in_last_get_server;
extern t_bool reread_active_for_posted_arts;
extern t_bool save_news;
extern t_bool show_description;
extern t_bool show_subject;
extern t_bool space_mode;
extern t_bool start_any_unread;
extern t_bool tex2iso_supported;
extern t_bool batch_mode;
extern t_bool update_fork;
extern t_bool verbose;
extern t_bool xover_supported;
extern t_bool xref_supported;

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
#define HIST_SAVE_FILE		13
#define HIST_SELECT_PATTERN	14
#define HIST_SHELL_COMMAND	15
#define HIST_SUBJECT_SEARCH	16
#define HIST_CONFIG_SEARCH	17
#define HIST_HELP_SEARCH	18
#define HIST_MAXNUM		18	/* must always be the same as the highest HIST_ value except HIST_NONE */
#define HIST_NONE		(HIST_MAXNUM+1)

extern int hist_last[HIST_MAXNUM+1];
extern int hist_pos[HIST_MAXNUM+1];
extern char *input_history[HIST_MAXNUM+1][HIST_SIZE+1];


/* defines for GNKSA checking */
/* success/undefined failure */
#define GNKSA_OK			0
#define GNKSA_INTERNAL_ERROR		1
/* general syntax */
#define GNKSA_LANGLE_MISSING		100
#define GNKSA_LPAREN_MISSING		101
#define GNKSA_RPAREN_MISSING		102
#define GNKSA_ATSIGN_MISSING		103
/* FQDN checks */
#define GNKSA_SINGLE_DOMAIN		200
#define GNKSA_INVALID_DOMAIN		201
#define GNKSA_ILLEGAL_DOMAIN		202
#define GNKSA_UNKNOWN_DOMAIN		203
#define GNKSA_INVALID_FQDN_CHAR		204
#define GNKSA_ZERO_LENGTH_LABEL		205
#define GNKSA_ILLEGAL_LABEL_LENGTH	206
#define GNKSA_ILLEGAL_LABEL_HYPHEN	207
#define GNKSA_ILLEGAL_LABEL_BEGNUM	208
#define GNKSA_BAD_DOMAIN_LITERAL	209
#define GNKSA_LOCAL_DOMAIN_LITERAL	210
#define GNKSA_RBRACKET_MISSING		211
/* localpart checks */
#define GNKSA_LOCALPART_MISSING		300
#define GNKSA_INVALID_LOCALPART		301
#define GNKSA_ZERO_LENGTH_LOCAL_WORD	302
/* realname checks */
#define GNKSA_ILLEGAL_UNQUOTED_CHAR	400
#define GNKSA_ILLEGAL_QUOTED_CHAR	401
#define GNKSA_ILLEGAL_ENCODED_CHAR	402
#define GNKSA_BAD_ENCODE_SYNTAX		403
#define GNKSA_ILLEGAL_PAREN_CHAR		404
#define GNKSA_INVALID_REALNAME		405

/* address types */
#define GNKSA_ADDRTYPE_ROUTE	0
#define GNKSA_ADDRTYPE_OLDSTYLE	1

#ifndef DONT_HAVE_PIPING
	extern constext txt_pipe[];
	extern constext txt_pipe_to_command[];
	extern constext txt_piping[];
#else
#	ifdef VMS /* M.St. 15.01.98 */
		extern constext txt_pipe_to_command[];
		extern constext txt_piping[];
#	endif /* VMS */
	extern constext txt_piping_not_enabled[];
#endif /* !DONT_HAVE_PIPING */

#ifdef FORGERY
	extern constext txt_warn_cancel_forgery[];
#endif /* FORGERY */

#ifdef HAVE_COLOR
	extern constext txt_help__[];
	extern constext txt_help_color[];
	extern constext txt_tinrc_colors[];
	extern constext txt_toggled_high[];
	extern int default_bcol;
	extern int default_fcol;
	extern t_bool use_color;
	extern t_bool word_highlight;
#	ifdef USE_CURSES
		extern constext txt_no_colorterm[];
#	endif /* USE_CURSES */
#endif /* HAVE_COLOR */

#ifdef HAVE_FASCIST_NEWSADMIN
	extern constext txt_error_followup_to_several_groups[];
	extern constext txt_error_missing_followup_to[];
	extern constext txt_error_not_valid_newsgroup[];
#endif /* HAVE_FASCIST_NEWSADMIN */

#ifdef HAVE_METAMAIL
	extern constext txt_use_mime[];
#endif /* HAVE_METAMAIL */

#if defined(HAVE_METAMAIL) && !defined(INDEX_DAEMON)
	extern constext txt_error_metamail_failed[];
#endif /* HAVE_METAMAIL && !INDEX_DAEMON */

#ifdef HAVE_PGP
	extern char pgp_data[PATH_LEN];
	extern const char *pgpopts;
	extern constext txt_help_ctrl_g[];
	extern constext txt_pgp_add[];
	extern constext txt_pgp_mail[];
	extern constext txt_pgp_news[];
	extern constext txt_pgp_not_avail[];
	extern constext txt_pgp_nothing[];
#endif /* HAVE_PGP */

#ifdef HAVE_SYS_UTSNAME_H
	extern struct utsname system_info;
#endif /* HAVE_SYS_UTSNAME_H */

#ifdef INDEX_DAEMON
	extern char group_times_file[PATH_LEN];
	extern constext txt_cannot_stat_group[];
	extern constext txt_cannot_stat_index_file[];
#else
	extern constext txt_art_deleted[];
	extern constext txt_art_undeleted[];
	extern constext txt_intro_page[];
	extern constext txt_processing_mail_arts[];
	extern constext txt_processing_saved_arts[];
#endif /* INDEX_DAEMON */

#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
	extern constext txt_reading_mail_active_file[];
	extern constext txt_reading_mailgroups_file[];
#endif /* !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING) */

#ifdef LOCAL_CHARSET
	extern constext txt_tinrc_local_charset[];
	extern t_bool use_local_charset;
#endif /* LOCAL_CHARSET */

#ifdef M_AMIGA
	extern constext txt_env_var_not_found[];
	extern t_bool tin_bbs_mode;
#endif /* M_AMIGA */

#ifdef M_OS2
	extern char TMPDIR[PATH_LEN];
#endif /* M_OS2 */

#ifndef NO_ETIQUETTE
	extern constext txt_warn_posting_etiquette[];
#endif /* NO_ETIQUETTE */

#ifndef NO_SHELL_ESCAPE
	extern constext txt_help_shell[];
#endif /* NO_SHELL_ESCAPE */

#if !defined(USE_CURSES)
	extern struct t_screen *screen;
#endif /* !USE_CURSES */

#ifdef NNTP_ABLE
	extern unsigned short nntp_tcp_port;
	extern t_bool force_auth_on_conn_open;
#endif /* NNTP_ABLE */

extern struct opttxt txt_beginner_level;
extern struct opttxt txt_show_description;
extern struct opttxt txt_show_author;
extern struct opttxt txt_draw_arrow;
extern struct opttxt txt_inverse_okay;
extern struct opttxt txt_thread_articles;
extern struct opttxt txt_sort_article_type;
extern struct opttxt txt_pos_first_unread;
extern struct opttxt txt_show_only_unread_arts;
extern struct opttxt txt_show_only_unread_groups;
extern struct opttxt txt_kill_level;
extern struct opttxt txt_tab_goto_next_unread;
extern struct opttxt txt_space_goto_next_unread;
extern struct opttxt txt_pgdn_goto_next;
extern struct opttxt txt_auto_list_thread;
extern struct opttxt txt_art_marked_deleted;
extern struct opttxt txt_art_marked_inrange;
extern struct opttxt txt_art_marked_return;
extern struct opttxt txt_art_marked_selected;
extern struct opttxt txt_art_marked_unread;
extern struct opttxt txt_groupname_max_length;
extern struct opttxt txt_show_lines;
extern struct opttxt txt_show_score;
extern struct opttxt txt_full_page_scroll;
extern struct opttxt txt_show_last_line_prev_page;
extern struct opttxt txt_show_signatures;
extern struct opttxt txt_news_headers_to_display;
extern struct opttxt txt_news_headers_to_not_display;
extern struct opttxt txt_show_xcommentto;
extern struct opttxt txt_display_mime_header_asis;
extern struct opttxt txt_display_mime_allheader_asis;
extern struct opttxt txt_alternative_handling;
#ifdef HAVE_COLOR
	extern struct opttxt txt_quote_regex;
	extern struct opttxt txt_quote_regex2;
	extern struct opttxt txt_quote_regex3;
#endif	/* HAVE_COLOR */
extern struct opttxt txt_strip_re_regex;
extern struct opttxt txt_strip_was_regex;
#ifdef HAVE_METAMAIL
	extern struct opttxt txt_use_metamail;
	extern struct opttxt txt_ask_for_metamail;
#endif	/* HAVE_METAMAIL */
extern struct opttxt txt_catchup_read_groups;
extern struct opttxt txt_group_catchup_on_exit;
extern struct opttxt txt_thread_catchup_on_exit;
extern struct opttxt txt_confirm_action;
extern struct opttxt txt_confirm_to_quit;
extern struct opttxt txt_use_mouse;
#ifdef HAVE_KEYPAD
	extern struct opttxt txt_use_keypad;
#endif	/* HAVE_KEYPAD */
extern struct opttxt txt_use_getart_limit;
extern struct opttxt txt_getart_limit;
#ifdef HAVE_COLOR
	extern struct opttxt txt_use_color;
	extern struct opttxt txt_col_normal;
	extern struct opttxt txt_col_back;
	extern struct opttxt txt_col_invers_bg;
	extern struct opttxt txt_col_invers_fg;
	extern struct opttxt txt_col_text;
	extern struct opttxt txt_col_minihelp;
	extern struct opttxt txt_col_help;
	extern struct opttxt txt_col_message;
	extern struct opttxt txt_col_quote;
	extern struct opttxt txt_col_quote2;
	extern struct opttxt txt_col_quote3;
	extern struct opttxt txt_col_head;
	extern struct opttxt txt_col_newsheaders;
	extern struct opttxt txt_col_subject;
	extern struct opttxt txt_col_response;
	extern struct opttxt txt_col_from;
	extern struct opttxt txt_col_title;
	extern struct opttxt txt_col_signature;
	extern struct opttxt txt_word_highlight;
	extern struct opttxt txt_word_h_display_marks;
	extern struct opttxt txt_col_markstar;
	extern struct opttxt txt_col_markdash;
#endif	/* HAVE_COLOR */
extern struct opttxt txt_mail_address;
extern struct opttxt txt_prompt_followupto;
extern struct opttxt txt_sigfile;
extern struct opttxt txt_sigdashes;
extern struct opttxt txt_signature_repost;
extern struct opttxt txt_quote_chars;
extern struct opttxt txt_quote_empty_lines;
extern struct opttxt txt_quote_signatures;
extern struct opttxt txt_news_quote_format;
extern struct opttxt txt_xpost_quote_format;
extern struct opttxt txt_mail_quote_format;
extern struct opttxt txt_advertising;
extern struct opttxt txt_mm_charset;
extern struct opttxt txt_post_mime_encoding;
extern struct opttxt txt_post_8bit_header;
extern struct opttxt txt_mail_mime_encoding;
extern struct opttxt txt_mail_8bit_header;
extern struct opttxt txt_strip_blanks;
extern struct opttxt txt_auto_cc;
extern struct opttxt txt_auto_bcc;
extern struct opttxt txt_spamtrap_warning_addresses;
extern struct opttxt txt_filter_days;
extern struct opttxt txt_add_posted_to_filter;
extern struct opttxt txt_maildir;
extern struct opttxt txt_save_to_mmdf_mailbox;
extern struct opttxt txt_batch_save;
extern struct opttxt txt_savedir;
extern struct opttxt txt_auto_save;
extern struct opttxt txt_mark_saved_read;
extern struct opttxt txt_post_process;
extern struct opttxt txt_process_only_unread;
extern struct opttxt txt_print_header;
extern struct opttxt txt_printer;
extern struct opttxt txt_wildcard;
extern struct opttxt txt_force_screen_redraw;
extern struct opttxt txt_start_editor_offset;
extern struct opttxt txt_editor_format;
extern struct opttxt txt_use_builtin_inews;
extern struct opttxt txt_mailer_format;
extern struct opttxt txt_use_mailreader_i;
extern struct opttxt txt_unlink_article;
extern struct opttxt txt_keep_posted_articles;
extern struct opttxt txt_keep_dead_articles;
extern struct opttxt txt_strip_newsrc;
extern struct opttxt txt_strip_bogus;
extern struct opttxt txt_reread_active_file_secs;
extern struct opttxt txt_auto_reconnect;
extern struct opttxt txt_cache_overview_files;

#endif /* !EXTERN_H */
