#ifndef PROTO_H
#	define PROTO_H 1

/* active.c */
extern int get_active_num (void);
extern int group_flag (int ch);
extern int match_group_list (char *group, char *group_list);
extern int parse_active_line (char *line, long *max, long *min, char *moderated);
extern int process_bogus(char *name);
extern int resync_active_file (void);
extern void create_save_active_file (void);
extern void load_newnews_info (char *info);
extern void read_news_active_file (void);
#ifdef INDEX_DAEMON
	extern void read_group_times_file (void);
	extern void vMakeActiveMyGroup (void);
	extern void write_group_times_file (void);
#endif /* INDEX_DAEMON */

/* art.c */
extern char *pcFindNovFile (struct t_group *psGrp, int iMode);
extern int index_group (struct t_group *group);
extern void do_update (void);
extern void find_base (struct t_group *group);
extern void make_threads (struct t_group *group, int rethread);
extern void set_article (struct t_article *art);
extern void sort_arts (int sort_art_type);
extern void vWriteNovFile (struct t_group *psGrp);

/* attrib.c */
extern void read_attributes_file (char *file, int global_file);
extern void write_attributes_file (char *file);

/* auth.c */
#if !defined (INDEX_DAEMON) && defined (NNTP_ABLE)
	extern t_bool authenticate (char *server, char *user, t_bool startup);
#endif /* !INDEX_DAEMON && NNTP_ABLE */

/* charset.c */
extern int iIsArtTexEncoded (long art, char *group_path);
extern void Convert2Printable (char* buf);
extern void ConvertBody2Printable (char* buf);
extern void ConvertIso2Asc (char *iso, char *asc, int t);
extern void ConvertTeX2Iso (char *from, char *to);

/* color.c */
extern void bcol (int color);
extern void fcol (int color);
extern void print_color (char *str, t_bool signature);

/* config.c */
extern char *quote_space_to_dash (char *str);
extern const char *print_boolean (t_bool value);
extern int change_config_file (struct t_group *group);
extern int match_boolean (char *line, const char *pat, t_bool *dst);
extern int match_integer (char *line, const char *pat, int *dst, int maxlen);
extern int match_long (char *line, const char *pat, long *dst);
extern int match_string (char *line, const char *pat, char *dst, size_t dstlen);
extern int option_row(int option);
extern int read_config_file (char *file, int global_file);
extern void quote_dash_to_space (char *str);
extern void refresh_config_page (int act_option);
extern void show_menu_help (const char *help_message);
extern char **ulBuildArgv(char *cmd, int *new_argc);
extern void write_config_file (char *file);

/* curses.c */
extern OUTC_RETTYPE outchar (OUTC_ARGS);
extern int InitScreen (void);
extern int RawState (void);
extern int ReadCh (void);
extern int SetupScreen (void);
extern void ClearScreen (void);
extern void CleartoEOLN (void);
extern void CleartoEOS (void);
extern void EndInverse (void);
extern void EndWin (void);
extern void InitWin (void);
extern void MoveCursor (int row, int col);
extern void Raw (int state);
extern void StartInverse (void);
extern void ToggleInverse (void);
extern void cursoroff (void);
extern void cursoron (void);
extern void set_keypad_off (void);
extern void set_keypad_on (void);
extern void set_xclick_off (void);
extern void set_xclick_on (void);
extern void setup_screen (void);

/* debug.c */
#ifdef DEBUG
	extern void debug_delete_files (void);
	extern void debug_nntp (const char *func, const char *line);
	extern void debug_print_active (void);
	extern void debug_print_arts (void);
	extern void debug_print_filters (void);
	extern void debug_print_header (struct t_article *s);
	extern void debug_save_comp (void);
	extern void vDbgPrintMalloc (int iIsMalloc, const char *pcFile, int iLine, size_t iSize);
#endif /* DEBUG */
#ifdef DEBUG_NEWSRC
	extern void debug_print_newsrc (struct t_newsrc *NewSrc, FILE *fp);
#endif /* DEBUG_NEWSRC */
#if defined(DEBUG) || defined (DEBUG_NEWSRC)
	extern void debug_print_bitmap (struct t_group *group, struct t_article *art);
	extern void debug_print_comment (const char *comment);
#endif

/* envarg.c */
extern void envargs (int *Pargc, char ***Pargv, const char *envstr);

/* feed.c */
extern char get_post_proc_type (int proc_type);
extern void feed_articles (int function, int level, struct t_group *group, int respnum);

/* filter.c */
extern int auto_select_articles (struct t_group *group);
extern int filter_articles (struct t_group *group);
extern int filter_menu (int type, struct t_group *group, struct t_article *art);
extern int quick_filter_kill (struct t_group *group, struct t_article *art);
extern int quick_filter_select (struct t_group *group, struct t_article *art);
extern struct t_filter *psExpandFilterArray (struct t_filter *ptr, int *num);
extern void free_all_filter_arrays (void);
#ifndef INDEX_DAEMON
	extern int read_filter_file (char *file, t_bool global_file);
#endif /* INDEX_DAEMON */
extern int quick_filter_select_posted_art (struct t_group *group, char *subj);

/* getline.c */
extern char *getline (const char *prompt, int number_only, char *str, int max_chars, int which_hist);

/* group.c */
extern int find_new_pos (int old_top, long old_artnum, int cur_pos);
extern void clear_note_area (void);
extern void decr_tagged (int tag);
extern void group_page (struct t_group *group);
extern void mark_screen (int level, int screen_row, int screen_col, const char *value);
extern void set_subj_from_size (int num_cols);
extern void show_group_page (void);
extern void toggle_subject_from (void);
extern void move_to_thread (int n);

/* hashstr.c */
extern char *hash_str (const char *s);
extern void hash_init (void);
extern void hash_reclaim (void);

/* help.c */
extern void display_info_page (t_bool first);
extern void show_info_page (int type, const char *help[], const char *title);
extern void show_mini_help (int level);
extern void toggle_mini_help (int level);

/* header.c */
extern const char *get_domain_name (void);
extern const char *get_fqdn (const char *host);
extern const char *get_host_name (void);
#ifndef FORGERY
	extern char *build_sender (void);
#endif /* !FORGERY */

/* inews.c */
extern int submit_news_file (char *name);
extern void get_from_name (char *from_name, struct t_group *thisgrp);
extern void get_user_info (char *user_name, char *full_name);

/* init.c */
extern char *GetConfigValue (const char *name);
extern int create_mail_save_dirs (void);
extern t_bool (*wildcard_func)(const char *str, char *patt, t_bool icase);		/* Wildcard matching function */
extern void init_selfinfo (void);
#ifdef HAVE_COLOR
	extern void postinit_colors (void);
#endif /* HAVE_COLOR */
#ifndef INDEX_DAEMON
	void set_up_private_index_cache (void);
#endif /* INDEX_DAEMON */

/* joinpath.c */
extern void joinpath (char *result, const char *dir, const char *file);

/* list.c */
extern int find_group_index (const char *group);
extern struct t_group *psGrpAdd (char *group);
extern struct t_group *psGrpFind (char *pcGrpName);
extern unsigned long hash_groupname (const char *group);
extern void init_group_hash (void);
extern char *random_organization(char *in_org);
#if 0
	extern struct t_group *psGrpFirst (void);
	extern struct t_group *psGrpLast (void);
	extern struct t_group *psGrpNext (void);
	extern struct t_group *psGrpPrev (void);
#endif

/* mail.c */
extern int iArtEdit (struct t_group *psGrp, struct t_article *psArt);
extern void read_newsgroups_file (void);
extern void vFindArtMaxMin (char *pcGrpPath, long *plArtMax, long *plArtMin);
extern void vMakeGrpName (char *pcBaseDir, char *pcGrpName, char *pcGrpPath);
extern void vMakeGrpPath (char *pcBaseDir, char *pcGrpName, char *pcGrpPath);
extern void vPrintActiveHead (char *pcActiveFile);
extern void vPrintGrpLine (FILE *hFp, char *pcGrpName, long lArtMax, long lArtMin, char *pcBaseDir);
#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
	extern void read_mail_active_file (void);
	extern void read_mailgroups_file (void);
	extern void vGrpDelMailArt (struct t_article *psArt);
	extern void vGrpDelMailArts (struct t_group *psGrp);
	extern void write_mail_active_file (void);
#endif /* !INDEX_DAEMON && HAVE_MH_MAIL_HANDLING */

/* main.c */
extern int main (int argc, char *argv[]);
extern int read_cmd_line_groups (void);

/* memory.c */
extern void expand_active (void);
extern void expand_art (void);
extern void expand_newnews (void);
extern void expand_save (void);
extern void init_alloc (void);
extern void init_screen_array (int allocate);
extern void free_all_arrays (void);
extern void free_art_array (void);
extern void free_attributes_array (void);
extern void free_save_array (void);
extern void *my_malloc1 (const char *file, int line, size_t size);
extern void *my_realloc1 (const char *file, int line, char *p, size_t size);

/* misc.c */
extern char *eat_re (char *s, t_bool eat_was);
extern char *quote_wild(char *str);
extern char *quote_wild_whitespace(char *str);
extern const char *get_val (const char *env, const char *def);
extern int get_arrow_key (int prech);
extern int get_initials (int respnum, char *s, int maxsize);
extern int iCopyFile (char *pcSrcFile, char *pcDstFile);
extern int invoke_cmd (char *nam);
extern int invoke_editor (char *filename, int lineno);
extern int mail_check (void);
extern int my_chdir (char *path);
extern int my_isprint (int c);
extern int my_mkdir (char *path, mode_t mode);
extern int peek_char (FILE *fp);
extern int stat_file (char *file);
extern int strfmailer (char *the_mailer, char *subject, char *to, char *filename, char *s, size_t maxsize, char *format);
extern int strfpath (char *format, char *str, size_t maxsize, char *the_homedir, char *maildir, char *savedir, char *group);
extern int strfquote (char *group, int respnum, char *s, size_t maxsize, char *format);
extern int untag_all_articles (void);
extern void asfail (const char *file, int line, const char *cond);
extern void base_name (char *dirname, char *program);
extern void cleanup_tmp_files (void);
extern void copy_body (FILE *fp_ip, FILE *fp_op, char *prefix, char *initl, t_bool with_sig);
extern void copy_fp (FILE *fp_ip, FILE *fp_op);
extern void create_index_lock_file (char *the_lock_file);
extern void draw_percent_mark (long cur_num, long max_num);
extern void get_author (int thread, struct t_article *art, char *str, size_t len);
extern void get_cwd (char *buf);
extern void make_group_path (char *name, char *path);
extern void parse_from (char *from_line, char *eaddr, char *fname);
extern void read_input_history_file (void);
extern void rename_file (char *old_filename, char *new_filename);
extern void set_real_uid_gid (void);
extern void set_tin_uid_gid (void);
extern void show_inverse_video_status (void);
extern void strip_double_ngs (char *ngs_list);
extern void tin_done (int ret);
extern void toggle_inverse_video (void);
extern void vPrintBugAddress (void);
#ifdef LOCAL_CHARSET
	extern void buffer_to_local (char *b);
	extern void buffer_to_network (char *b);
#endif /* LOCAL_CHARSET */
#ifdef HAVE_COLOR
	extern t_bool toggle_color (void);
	extern void show_color_status (void);
#endif /* HAVE_COLOR */
#ifdef HAVE_ISPELL
	extern int invoke_ispell (char *nam);
#endif /* HAVE_ISPELL */
#ifdef M_UNIX
	extern void append_file (char *old_filename, char *new_filename);
#else
	extern void make_post_process_cmd (char *cmd, char *dir, char *file);
#endif /* M_UNIX */
#ifndef	NO_SHELL_ESCAPE
	extern void shell_escape (void);
#endif /* NO_SHELL_ESCAPE */

/* newsrc.c */
extern int pos_group_in_newsrc (struct t_group *group, int pos);
extern void art_mark_read (struct t_group *group, struct t_article *art);
extern void art_mark_unread (struct t_group *group, struct t_article *art);
extern void art_mark_will_return (struct t_group *group, struct t_article *art);
extern void backup_newsrc (void);
extern void catchup_newsrc_file (void);
extern void delete_group (char *group);
extern void expand_bitmap (struct t_group *group, long min);
extern void grp_mark_read (struct t_group *group, struct t_article *psArt);
extern void grp_mark_unread (struct t_group *group);
extern void parse_unread_arts (struct t_group *group);
extern void read_newsrc (char *newsrc_file, int allgroups);
extern void reset_newsrc (void);
extern void subscribe (struct t_group *group, int sub_state);
extern void thd_mark_read (struct t_group *group, long thread);
extern void thd_mark_unread (struct t_group *group, long thread);
extern void vSetDefaultBitmap (struct t_group *group);
extern void vWriteNewsrc (void);
#ifdef DEBUG_NEWSRC
	extern void vNewsrcTestHarness (void);
#endif /* DEBUG_NEWSRC */
#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
	extern void art_mark_deleted (struct t_article *art);
	extern void art_mark_undeleted (struct t_article *art);
#endif /* !INDEX_DAEMON && HAVE_MH_MAIL_HANDLING */


/* nntplib.c */
extern char *getserverbyfile (const char *file);
extern char *get_server (char *string, int size);
#ifdef DEBUG
extern const char *nntp_respcode (int respcode);
#endif
extern int get_tcp_socket (char *machine, char *service, unsigned port);
extern int server_init (char *machine, const char *service, int port, char *text);
extern void close_server (void);
extern void put_server (const char *string);
extern void u_put_server (const char *string);

/* nrctbl.c */
extern int get_newsrcname (char *newsrc_name, const char *nntpserver_name);
extern void get_nntpserver (char *nntpserver_name, char *nick_name);

/* open.c */
extern FILE *nntp_command(const char *, int, char *);
extern FILE *open_art_fp (char *group_path, long art, int lines);
extern FILE *open_newgroups_fp (int the_index);
extern FILE *open_news_active_fp (void);
extern FILE *open_newsgroups_fp (void);
extern FILE *open_overview_fmt_fp (void);
extern FILE *open_subscription_fp (void);
extern FILE *open_xover_fp (struct t_group *psGrp, const char *pcMode, long lMin, long lMax);
extern FILE *open_art_header (long art);
extern int get_respcode (char *);
extern int nntp_open (void);
extern int setup_hard_base (struct t_group *group, char *group_path);
extern int stat_article (long art, char *group_path);
extern void vGet1GrpArtInfo(struct t_group *grp);
extern int vGrpGetArtInfo (char *pcSpoolDir, char *pcGrpName, int iGrpType, long *plArtCount, long *plArtMax, long *plArtMin);
extern void nntp_close (void);
extern void vGrpGetSubArtInfo (void);
#ifdef HAVE_MH_MAIL_HANDLING
	extern FILE *open_mail_active_fp (const char *mode);
	extern FILE *open_mailgroups_fp (void);
#endif /* HAVE_MH_MAIL_HANDLING */

/* page.c */
extern int art_open (struct t_article *art, char *group_path);
extern int match_header (char *buf, const char *pat, char *body, char *nodec_body, size_t len);
extern void art_close (void);
extern void redraw_page (char *group, int respnum);
extern void show_note_page (char *group, int respnum);
extern void yank_to_addr (char *orig, char *addr);
#ifndef INDEX_DAEMON
	extern int show_page (struct t_group *group, char *group_path, int respnum, int *threadnum);
#endif /* INDEX_DAEMON */

/* parsdate.y */
extern int GetTimeInfo (TIMEINFO *Now);
extern time_t parsedate (char *p, TIMEINFO *now);

/* pgp.c */
#ifdef HAVE_PGP
	extern int pgp_check_article (void);
	extern void invoke_pgp_mail (char *nam, char *mail_to);
	extern void invoke_pgp_news (char *the_article);
#endif

/* post.c */
extern int count_postponed_articles (void);
extern int mail_bug_report (void);
extern int mail_to_author (char *group, int respnum, int copy_text, int with_headers);
extern int mail_to_someone (int respnum, char *address, int mail_to_poster, int confirm_to_mail, int *mailed_ok);
extern int post_article (char *group, int *posted_flag);
extern int post_response (char *group, int respnum, int copy_text, int with_headers);
extern int repost_article (char *group, struct t_article *art, int respnum, int supersede);
extern int reread_active_after_posting (void);
extern t_bool cancel_article (struct t_group *group, struct t_article *art, int respnum);
extern t_bool pickup_postponed_articles (t_bool ask, t_bool all);
extern t_bool user_posted_messages (void);
extern void checknadd_headers (char *infile);
extern void quick_post_article (t_bool postponed_only);

/* prompt.c */
extern char *sized_message (const char *format, const char *subject);
extern int prompt_list (int row, int col, int var, constext *help_text, constext *prompt_text, constext *list[], int size);
extern int prompt_menu_string (int line, int col, char *var);
extern int prompt_num (int ch, const char *prompt);
extern int prompt_option_char (int option);
extern int prompt_option_num (int option);
extern int prompt_option_string (int option);
extern int prompt_slk_response (int ch_default, const char *responses, const char *fmt, ...);
extern int prompt_string (const char *prompt, char *buf, int which_hist);
extern int prompt_yn (int line, const char *prompt, t_bool default_answer);
extern void continue_prompt (void);
extern void prompt_on_off (int row, int col, t_bool *var, constext *help_text, constext *prompt_text);

/* read.c */
extern char *fgets_hdr (char *s, size_t size, FILE *f);
extern char *tin_fgets (char *buffer, size_t len, FILE *fp);
extern void drain_buffer (FILE *fp);

/* refs.c */
extern char *get_references (struct t_msgid *refptr);
extern struct t_msgid *find_msgid (char *msgid);
extern void build_references (struct t_group *group);
extern void clear_art_ptrs (void);
extern void collate_subjects (void);
extern void free_msgids (void);
extern void thread_by_reference (void);

/* regex.c */
extern t_bool match_regex (const char *string, char *pattern, t_bool icase);

/* rfc1521.c */
extern FILE *rfc1521_decode (FILE *file);
extern void rfc1468_encode (char *line, FILE *f, int e);
extern void rfc1521_encode (char *line, FILE *f, int e);
extern void rfc1557_encode (char *line, FILE *f, int e);
extern void rfc1922_encode (char *line, FILE *f, int e);

/* rfc1522.c */
extern char *rfc1522_decode (const char *s);
extern char *rfc1522_encode (char *s,t_bool ismail);
extern int mmdecode (const char *what, int encoding, int delimiter, char *where, const char *charset);
extern void get_mm_charset (void);
extern void rfc15211522_encode (char *filename, constext *mime_encoding, t_bool allow_8bit_header,t_bool ismail);

/* save.c */
extern char *save_filename (int i);
extern int check_start_save_any_news (int check_start_save);
extern int create_path (char *path);
extern int post_process_files (int proc_type_ch, t_bool auto_delete);
extern int save_art_to_file (int respnum, int indexnum, int the_mailbox, const char *filename);
extern int save_comp (t_comptype *p1, t_comptype *p2);
#ifndef INDEX_DAEMON
	extern int save_regex_arts (int is_mailbox, char *group_path);
	extern int save_thread_to_file (int is_mailbox, char *group_path);
#endif
extern void add_to_save_list (int the_index, struct t_article *the_article, int is_mailbox, int archive_save, char *path);
extern void delete_processed_files (t_bool auto_delete);
extern void print_art_seperator_line (FILE *fp, int the_mailbox);
extern void sort_save_list (void);

/* screen.c */
extern void center_line (int line, int inverse, const char *str);
extern void clear_message (void);
extern void draw_arrow (int line);
extern void erase_arrow (int line);
extern void error_message (const char *fmt, ...);
extern void info_message (const char *fmt, ...);
extern void perror_message (const char *fmt, ...);
extern void ring_bell (void);
extern void show_progress (const char *txt, int count, int total);
extern void show_title (char *title);
extern void spin_cursor (void);
extern void stow_cursor (void);
extern void wait_message (int delay, const char *fmt, ...);

/* search.c */
extern int search_article (int forward);
extern int search_author (int the_index, int current_art, int forward);
extern int search_body (struct t_group *group, int current_art);
extern int search_config (int forward, int current, int last);
extern int search_help (int forward, int current, int last);
extern int search_subject_group (int forward);
extern void search_group (int forward);
extern void search_subject_thread (int forward, int baseart, int offset);

/* select.c */
extern int add_my_group (char *group, t_bool add);
extern int choose_new_group (void);
extern int iSetRange (int iLevel, int iNumMin, int iNumMax, int iNumCur);
extern int skip_newgroups (void);
extern void draw_group_arrow (void);
extern void selection_index (int start_groupnum, int num_cmd_line_groups);
extern void set_groupname_len (int all_groups);
extern void show_selection_page (void);
extern void strip_line (char *line);
extern void toggle_my_groups (t_bool only_unread_groups, const char *group);
extern void move_to_group (int n);

/* sigfile.c */
extern void msg_write_signature (FILE *fp, int flag, struct t_group *thisgroup);

/* signal.c */
extern RETSIGTYPE (*sigdisp (int sig, RETSIGTYPE (*func)(SIG_ARGS))) (SIG_ARGS);
extern int set_win_size (int *num_lines, int *num_cols);
extern void _CDECL signal_handler (SIG_ARGS);
extern void handle_resize (int repaint);
extern void set_alarm_clock_off (void);
extern void set_alarm_clock_on (void);
extern void set_alarm_signal (void);
extern void set_signal_catcher (int flag);
extern void set_signal_handlers (void);
extern void set_signals_art (void);
extern void set_signals_config (void);
extern void set_signals_group (void);
extern void set_signals_help (void);
extern void set_signals_page (void);
extern void set_signals_select (void);
extern void set_signals_thread (void);

/* strftime.c */
extern size_t my_strftime (char *s, size_t maxsize, const char *format, struct tm *timeptr);

/* string.c */
extern char *my_strdup (const char *str);
extern char *str_trim (char *string);
extern char *strcasestr (char *haystack, const char *needle);
extern char *tin_itoa (int value, int digits);
extern int sh_format(char *dst, size_t len, const char *fmt, ...);
extern size_t mystrcat (char **t, const char *s);
extern void modifiedstrncpy (char *target, const char *source, size_t size, int decode);
extern void my_strncpy (char *p, const char *q, size_t n);
extern void str_lwr (char *dst, const char *src);
extern void strcpynl (char *to, const char *from);
#ifndef HAVE_STRPBRK
	extern char *strpbrk (char *str1, char *str2);
#endif /* HAVE_STRPBRK */
#ifndef HAVE_STRSTR
	extern char *strstr (char *text, char *pattern);
#endif /* HAVE_STRSTR */
#ifndef HAVE_STRCASECMP
	extern int strcasecmp (const char *p, const char *q);
#endif /* HAVE STRCASECMP */
#ifndef HAVE_STRNCASECMP
	extern int strncasecmp (const char *p, const char *q, size_t n);
#endif /* HAVE_STRNCASECMP */
#ifndef HAVE_ATOI
	extern int atoi (const char *s);
#endif /* HAVE_ATOI */
#ifndef HAVE_ATOL
	extern long atol (const char *s);
#endif /* HAVE_ATOL */
#ifndef HAVE_STRTOL
	extern long strtol (const char *str, char **ptr, int use_base);
#endif /* HAVE STRTOL */

/* thread.c */
extern int find_response (int i, int n);
extern int new_responses (int thread);
extern int next_response (int n);
extern int next_thread (int n);
extern int next_unread (int n);
extern int num_of_responses (int n);
extern int prev_response (int n);
extern int prev_unread (int n);
extern int show_thread (struct t_group *group, char *group_path, int respnum, int thread_depth);
extern int stat_thread (int n, struct t_art_stat *sbuf);
extern int which_response (int n);
extern int which_thread (int n);
extern void move_to_response (int n);
extern void show_thread_page (void);

/* wildmat.c */
extern t_bool wildmat (const char *text, char *p, t_bool icase);

/* xref.c */
extern int overview_xref_support (void);
extern void NSETRNG0 (t_bitmap *bitmap, long low, long high);
extern void NSETRNG1 (t_bitmap *bitmap, long low, long high);
extern void art_mark_xref_read (struct t_article *art);

#endif /* PROTO_H */
