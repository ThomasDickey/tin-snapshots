#ifndef PROTO_H
#define PROTO_H 1

/* active.c */
extern int get_active_num (void);
extern int resync_active_file (void);
extern int process_bogus(char *name);
extern int parse_active_line (char *line, long *max, long *min, char *moderated);
extern void read_news_active_file (void);
extern int match_group_list (char *group, char *group_list);
extern void read_group_times_file (void);
extern void write_group_times_file (void);
extern void load_newnews_info (char *info);
extern void read_motd_file (void);
#ifdef INDEX_DAEMON
extern void vMakeActiveMyGroup (void);
#endif

/* actived.c */
extern void create_save_active_file (void);
#ifdef INDEX_DAEMON
extern void vCreatePath (char *pcPath);
#endif

/* art.c */
extern void find_base (struct t_group *group);
extern int index_group (struct t_group *group);
extern void make_threads (struct t_group *group, int rethread);
extern void sort_arts (int sort_art_type);
extern void vWriteNovFile (struct t_group *psGrp);
extern char *pcFindNovFile (struct t_group *psGrp, int iMode);
extern void do_update (void);
extern void set_article (struct t_article *art);
extern int input_pending (void);
extern char *safe_fgets (FILE *fp);

/* attrib.c */
extern void read_attributes_file (char *file, int global_file);
extern void write_attributes_file (char *file);

/* charset.c */
extern void ConvertIso2Asc (char *iso, char *asc, int t);
extern void ConvertTeX2Iso (char *from, char *to);
extern int iIsArtTexEncoded (long art, char *group_path);
extern void Convert2Printable (unsigned char* buf);

/* color.c */
extern void fcol (int color);
extern void bcol (int color);
extern void print_color (char *str, t_bool signature);

/* config.c */
extern int read_config_file (char *file, int global_file);
extern void write_config_file (char *file);
extern void refresh_config_page (int act_option, int force_redraw);
extern int change_config_file (struct t_group *group, int filter_at_once);
extern void show_menu_help (const char *help_message);
extern int match_boolean (char *line, const char *pat, t_bool *dst);
extern int match_integer (char *line, const char *pat, int *dst, int maxlen);
extern int match_long (char *line, const char *pat, long *dst);
extern int match_string (char *line, const char *pat, char *dst, size_t dstlen);
extern const char *print_boolean (t_bool value);
extern void quote_dash_to_space (char *str);
extern char *quote_space_to_dash (char *str);

/* curses.c */
extern void setup_screen (void);
extern int InitScreen (void);
extern void InitWin (void);
extern void EndWin (void);
extern void set_keypad_on (void);
extern void set_keypad_off (void);
extern void ClearScreen (void);
extern void MoveCursor (int row, int col);
extern void CleartoEOLN (void);
extern void CleartoEOS (void);
extern void StartInverse (void);
extern void EndInverse (void);
extern void ToggleInverse (void);
extern int RawState (void);
extern void Raw (int state);
extern int ReadCh (void);
extern OUTC_RETTYPE outchar (OUTC_ARGS);
extern void xclick (int state);
extern void set_xclick_on (void);
extern void set_xclick_off (void);
extern void cursoron (void);
extern void cursoroff (void);

/* debug.c */
extern void debug_delete_files (void);
extern void debug_nntp (const char *func, const char *line);
extern void debug_nntp_respcode (int respcode);
extern void debug_print_arts (void);
extern void debug_print_header (struct t_article *s);
extern void debug_save_comp (void);
extern void debug_print_comment (const char *comment);
extern void debug_print_active (void);
extern void debug_print_bitmap (struct t_group *group, struct t_article *art);
#ifdef DEBUG_NEWSRC
extern void debug_print_newsrc (struct t_newsrc *NewSrc, FILE *fp);
#endif
#ifdef DEBUG
extern void vDbgPrintMalloc (int iIsMalloc, const char *pcFile, int iLine, size_t iSize);
#endif
extern void debug_print_filters (void);

/* envarg.c */
extern void envargs (int *Pargc, char ***Pargv, const char *envstr);

/* feed.c */
extern void feed_articles (int function, int level, struct t_group *group, int respnum);
extern int get_post_proc_type (int proc_type);

/* filter.c */
extern struct t_filter *psExpandFilterArray (struct t_filter *ptr, int *num);
extern void free_all_filter_arrays (void);
extern int read_filter_file (char *file, int global_file);
extern int filter_menu (int type, struct t_group *group, struct t_article *art);
extern int quick_filter_kill (struct t_group *group, struct t_article *art);
extern int quick_filter_select (struct t_group *group, struct t_article *art);
extern int quick_filter_select_posted_art (struct t_group *group, char *subj);
extern int filter_articles (struct t_group *group);
extern int auto_select_articles (struct t_group *group);

/* getline.c */
extern char *getline (const char *prompt, int number_only, char *str, int max_chars, int passwd);

/* group.c */
extern void decr_tagged (int tag);
extern void group_page (struct t_group *group);
extern void show_group_page (void);
extern void draw_subject_arrow (void);
extern void erase_subject_arrow (void);
extern void clear_note_area (void);
extern int find_new_pos (int old_top, long old_artnum, int cur_pos);
extern void mark_screen (int level, int screen_row, int screen_col, const char *value);
extern void set_subj_from_size (int num_cols);
extern void toggle_subject_from (void);

/* hashstr.c */
extern char *hash_str (const char *s);
extern void hash_init (void);
extern void hash_reclaim (void);

/* help.c */
extern void show_info_page (int type, const char *help[], const char *title);
extern void display_info_page (void);
extern void show_mini_help (int level);
extern void toggle_mini_help (int level);

/* header.c */
extern char *get_domain_name (void);
extern char *get_host_name (void);
extern const char *get_fqdn (const char *host);

/* inews.c */
extern void get_user_info (char *user_name, char *full_name);
extern void get_from_name (char *user_name, char *full_name, char *from_name);
extern int submit_news_file (char *name, int lines);

/* init.c */
extern void init_selfinfo (void);
extern int create_mail_save_dirs (void);
/*extern char *GetFQDN (void);*/
extern char *GetConfigValue (const char *name);

/* joinpath.c */
extern void joinpath (char *result, const char *dir, const char *file);

/* list.c */
extern void init_group_hash (void);
extern unsigned long hash_groupname (const char *group);
extern int find_group_index (const char *group);
extern struct t_group *psGrpFind (char *pcGrpName);
extern struct t_group *psGrpAdd (char *group);
#if 0
extern struct t_group *psGrpFirst (void);
extern struct t_group *psGrpLast (void);
extern struct t_group *psGrpNext (void);
extern struct t_group *psGrpPrev (void);
#endif

/* mail.c */
extern void read_newsgroups_file (void);
extern void read_groups_descriptions (FILE *fp, FILE *fp_save);
extern void vPrintActiveHead (char *pcActiveFile);
extern void vParseGrpLine (char *pcLine, char *pcGrpName, long *plArtMax, long *plArtMin, char *pcModerated);
extern void vFindArtMaxMin (char *pcGrpPath, long *plArtMax, long *plArtMin);
extern void vPrintGrpLine (FILE *hFp, char *pcGrpName, long lArtMax, long lArtMin, char *pcBaseDir);
extern void vMakeGrpPath (char *pcBaseDir, char *pcGrpName, char *pcGrpPath);
extern void vMakeGrpName (char *pcBaseDir, char *pcGrpName, char *pcGrpPath);
extern int iArtEdit (struct t_group *psGrp, struct t_article *psArt);

/* main.c */
extern int main (int argc, char *argv[]);
extern int read_cmd_line_groups (void);

/* memory.c */
extern void init_alloc (void);
extern void expand_art (void);
extern void expand_active (void);
extern void expand_save (void);
extern void expand_newnews (void);
extern void init_screen_array (int allocate);
extern void free_all_arrays (void);
extern void free_art_array (void);
extern void free_attributes_array (void);
extern void free_active_arrays (void);
extern void free_save_array (void);
extern void *my_malloc1 (const char *file, int line, size_t size);
extern void *my_realloc1 (const char *file, int line, char *p, size_t size);

/* misc.c */
extern void asfail (const char *file, int line, const char *cond);
extern void append_file (char *old_filename, char *new_filename);
extern void copy_fp (FILE *fp_ip, FILE *fp_op, const char *prefix);
extern void copy_body (FILE *fp_ip, FILE *fp_op, char *prefix, char *initl);
extern const char *get_val (const char *env, const char *def);
extern int invoke_editor (char *filename, int lineno);
extern int invoke_ispell (char *nam);
extern void shell_escape (void);
extern void tin_done (int ret);
extern void strip_double_ngs (char *ngs_list);
extern int my_mkdir (char *path, int mode);
extern int my_chdir (char *path);
extern void rename_file (char *old_filename, char *new_filename);
extern int invoke_cmd (char *nam);
extern void draw_percent_mark (long cur_num, long max_num);
extern void set_real_uid_gid (void);
extern void set_tin_uid_gid (void);
extern void base_name (char *dirname, char *program);
extern int mail_check (void);
extern void parse_from (char *from_line, char *eaddr, char *fname);
extern char *eat_re (char *s);
extern int untag_all_articles (void);
extern int my_isprint (int c);
extern void get_author (int thread, struct t_article *art, char *str, int len);
extern void toggle_inverse_video (void);
extern void show_inverse_video_status (void);
extern int get_arrow_key (void);
extern void create_index_lock_file (char *the_lock_file);
extern int strfquote (char *group, int respnum, char *s, size_t maxsize, char *format);
extern int strfpath (char *format, char *str, size_t maxsize, char *the_homedir, char *maildir, char *savedir, char *group);
extern int strfmailer (char *the_mailer, char *subject, char *to, char *filename, char *s, size_t maxsize, char *format);
extern int get_initials (int respnum, char *s, int maxsize);
extern void get_cwd (char *buf);
extern void make_group_path (char *name, char *path);
extern void cleanup_tmp_files (void);
extern void make_post_process_cmd (char *cmd, char *dir, char *file);
extern int stat_file (char *file);
extern void vPrintBugAddress (void);
extern int iCopyFile (char *pcSrcFile, char *pcDstFile);
extern int peek_char (FILE *fp);
#ifdef LOCAL_CHARSET
extern void buffer_to_local (char *b);
extern void buffer_to_network (char *b);
#endif

/* newsrc.c */
extern void read_newsrc (char *newsrc_file, int allgroups);
extern void vWriteNewsrc (void);
extern void backup_newsrc (void);
extern void subscribe (struct t_group *group, int sub_state);
extern void reset_newsrc (void);
extern void delete_group (char *group);
extern void grp_mark_read (struct t_group *group, struct t_article *psArt);
extern void grp_mark_unread (struct t_group *group);
extern void thd_mark_read (struct t_group *group, long thread);
extern void thd_mark_unread (struct t_group *group, long thread);
extern void parse_unread_arts (struct t_group *group);
extern int pos_group_in_newsrc (struct t_group *group, int pos);
extern void catchup_newsrc_file (char *newsrc_file);
extern void expand_bitmap (struct t_group *group, long min);
extern void art_mark_read (struct t_group *group, struct t_article *art);
extern void art_mark_unread (struct t_group *group, struct t_article *art);
extern void art_mark_will_return (struct t_group *group, struct t_article *art);
extern void vSetDefaultBitmap (struct t_group *group);
#ifdef DEBUG_NEWSRC
extern void vNewsrcTestHarness (void);
#endif

/* nntplib.c */
extern char *getserverbyfile (const char *file);
extern int server_init (char *machine, const char *service, int port);
extern int get_tcp_socket (const char *machine, const char *service, unsigned port);
extern void u_put_server (const char *string);
extern void put_server (const char *string);
extern int get_server (char *string, int size);
extern void close_server (void);
extern const char *nntp_respcode (int respcode);
extern int nntp_message (int respcode);

/* nrctbl.c */
extern void get_nntpserver (char *nntpserver_name, char *nick_name);
extern int get_newsrcname (char *newsrc_name, const char *nntpserver_name);

/* open.c */
extern int nntp_open (void);
extern void nntp_close (void);
extern FILE *open_mail_active_fp (char *mode);
extern FILE *open_news_active_fp (void);
extern FILE *open_overview_fmt_fp (void);
extern FILE *open_newgroups_fp (int the_index);
extern FILE *open_motd_fp (char *motd_file_date);
extern FILE *open_subscription_fp (void);
extern FILE *open_mailgroups_fp (void);
extern FILE *open_newsgroups_fp (void);
extern FILE *open_xover_fp (struct t_group *psGrp, const char *pcMode, long lMin, long lMax);
extern int stat_article (long art, char *group_path);
extern char *open_art_header (long art);
extern FILE *open_art_fp (char *group_path, long art);
extern int setup_hard_base (struct t_group *group, char *group_path);
extern int get_respcode (void);
extern void vGrpGetSubArtInfo (void);
extern int vGrpGetArtInfo (char *pcSpoolDir, char *pcGrpName, int iGrpType, long *plArtCount, long *plArtMax, long *plArtMin);

/* page.c */
extern int show_page (struct t_group *group, char *group_path, int respnum, int *threadnum);
extern void redraw_page (char *group, int respnum);
extern void show_note_page (char *group, int respnum);
extern int art_open (long art, char *group_path);
extern void art_close (void);
extern void yank_to_addr (char *orig, char *addr);
extern int match_header (char *buf, const char *pat, char *body, char *nodec_body, size_t len);

/* parsdate.y */
extern int GetTimeInfo (TIMEINFO *Now);
extern time_t parsedate (char *p, TIMEINFO *now);

/* pgp.c */
extern void invoke_pgp_mail (char *nam, char *mail_to);
extern void invoke_pgp_news (char *the_article);
extern int pgp_check_article (void);

/* post.c */
extern t_bool cancel_article (struct t_group *group, struct t_article *art, int respnum);
extern int mail_bug_report (void);
extern int mail_to_author (char *group, int respnum, int copy_text, int with_headers);
extern int mail_to_someone (int respnum, char *address, int mail_to_poster, int confirm_to_mail, int *mailed_ok);
extern int post_article (char *group, int *posted_flag);
extern int post_response (char *group, int respnum, int copy_text, int with_headers);
extern int repost_article (char *group, struct t_article *art, int respnum, int supersede);
extern int reread_active_after_posting (void);
extern t_bool user_posted_messages (void);
extern void checknadd_headers (char *infile, int lines);
extern void quick_post_article (int postponed_only);
extern int count_postponed_articles (void);
extern int pickup_postponed_articles (int ask, int all);

/* prompt.c */
extern int prompt_num (int ch, const char *prompt);
extern int prompt_string (const char *prompt, char *buf);
extern int prompt_menu_string (int line, int col, char *var);
extern int prompt_yn (int line, const char *prompt, int default_answer);
extern int prompt_yn2 (int line, const char *prompt, int default_answer);
extern int prompt_list (int row, int col, int var, constext *help_text, constext *prompt_text, constext *list[], int size);
extern void prompt_on_off (int row, int col, t_bool *var, constext *help_text, constext *prompt_text);
extern int prompt_option_string (int option);
extern int prompt_option_num (int option);
extern int prompt_option_char (int option);
extern void continue_prompt (void);

/* refs.c */
extern char *get_references (struct t_msgid *refptr);
extern void free_msgids (void);
extern void clear_art_ptrs (void);
extern void thread_by_reference (void);
extern void collate_subjects (void);
extern void build_references (struct t_group *group);

/* rfc1521.c */
extern unsigned char bin2hex (unsigned int x);
extern FILE *rfc1521_decode (FILE *file);
extern void rfc1521_encode (char *line, FILE *f, int e);
/* added for EUC-KR/JP/CN support by Jungshik Shin */
extern void rfc1557_encode (char *line, FILE *f, int e);
extern void rfc1468_encode (char *line, FILE *f, int e);
extern void rfc1922_encode (char *line, FILE *f, int e);

/* rfc1522.c */
extern int mmdecode (const char *what, int encoding, int delimiter, char *where, const char *charset);
extern void get_mm_charset (void);
extern char *rfc1522_decode (const char *s);
extern char *rfc1522_encode (char *s,t_bool ismail);
extern void rfc15211522_encode (char *filename, constext *mime_encoding, t_bool allow_8bit_header,t_bool ismail);

/* save.c */
extern int check_start_save_any_news (int check_start_save);
extern int save_art_to_file (int respnum, int indexnum, int the_mailbox, const char *filename);
extern int save_thread_to_file (int is_mailbox, char *group_path);
extern int save_regex_arts (int is_mailbox, char *group_path);
extern int create_path (char *path);
extern void add_to_save_list (int the_index, struct t_article *the_article, int is_mailbox, int archive_save, char *path);
extern void sort_save_list (void);
extern int save_comp (t_comptype *p1, t_comptype *p2);
extern char *save_filename (int i);
extern int post_process_files (int proc_type_ch, t_bool auto_delete);
extern void delete_processed_files (t_bool auto_delete);
extern void print_art_seperator_line (FILE *fp, int the_mailbox);

/* screen.c */
extern void stow_cursor (void);
extern void info_message (const char *str);
extern void wait_message (const char *str);
extern void error_message (const char *template, const char *str);
extern void perror_message (const char *template, const char *str);
extern void clear_message (void);
extern void center_line (int line, int inverse, const char *str);
extern void draw_arrow (int line);
extern void erase_arrow (int line);
extern void show_title (char *title);
extern void ring_bell (void);
extern void spin_cursor (void);
extern void show_progress (char *dst, const char *txt, int count, int total);

/* search.c */
extern int search_author (int the_index, int current_art, int forward);
extern void search_group (int forward);
extern void search_subject (int forward);
extern int search_article (int forward);
extern int search_body (struct t_group *group, int current_art);

/* select.c */
extern void selection_index (int start_groupnum, int num_cmd_line_groups);
extern void show_selection_page (void);
extern void erase_group_arrow (void);
extern void draw_group_arrow (void);
extern int choose_new_group (void);
extern int skip_newgroups (void);
extern int add_my_group (char *group, int add);
extern void set_groupname_len (int all_groups);
extern void toggle_my_groups (t_bool only_unread_groups, const char *group);
extern void strip_line (char *line);
extern int iSetRange (int iLevel, int iNumMin, int iNumMax, int iNumCur);

/* sigfile.c */
extern void msg_write_signature (FILE *fp, int flag);

/* signal.c */
extern RETSIGTYPE (*sigdisp (int sig, RETSIGTYPE (*func)(SIG_ARGS))) (SIG_ARGS);
extern void set_signal_handlers (void);
extern void set_alarm_signal (void);
extern void set_alarm_clock_on (void);
extern void set_alarm_clock_off (void);
extern void signal_handler (SIG_ARGS);
extern int set_win_size (int *num_lines, int *num_cols);
extern void set_signals_art (void);
extern void set_signals_config (void);
extern void set_signals_group (void);
extern void set_signals_help (void);
extern void set_signals_page (void);
extern void set_signals_select (void);
extern void set_signals_thread (void);
extern void art_suspend (SIG_ARGS);
extern void main_suspend (SIG_ARGS);
extern void select_suspend (SIG_ARGS);
extern void group_suspend (SIG_ARGS);
extern void help_suspend (SIG_ARGS);
extern void page_suspend (SIG_ARGS);
extern void thread_suspend (SIG_ARGS);
extern void config_suspend (SIG_ARGS);
extern void art_resize (SIG_ARGS);
extern void config_resize (SIG_ARGS);
extern void main_resize (SIG_ARGS);
extern void select_resize (SIG_ARGS);
extern void group_resize (SIG_ARGS);
extern void help_resize (SIG_ARGS);
extern void page_resize (SIG_ARGS);
extern void thread_resize (SIG_ARGS);

/* strftime.c */
extern size_t my_strftime (char *s, size_t maxsize, const char *format, struct tm *timeptr);

/* string.c */
extern char *tin_itoa (int value, int digits);
extern char *my_strdup (const char *str);
extern char *strcasestr (char *haystack, const char *needle);
extern int mystrcat (char **t, const char *s);
extern void my_strncpy (char *p, const char *q, int n);
extern void modifiedstrncpy (char *target, const char *source, size_t size, int decode);
extern void strcpynl (char *to, const char *from);
extern void str_lwr (const char *src, char *dst);

#ifndef HAVE_STRCASECMP
extern int strcasecmp (const char *p, const char *q);
#endif
#ifndef HAVE_STRNCASECMP
extern int strncasecmp (const char *p, const char *q, size_t n);
#endif
#ifndef HAVE_ATOI
extern int atoi (const char *s);
#endif
#ifndef HAVE_ATOL
extern long atol (const char *s);
#endif
#ifndef HAVE_STRPBRK
extern char *strpbrk (char *str1, char *str2);
#endif
#ifndef HAVE_STRSTR
extern char *strstr (char *text, char *pattern);
#endif
#ifndef HAVE_STRTOL
extern long strtol (const char *str, char **ptr, int use_base);
#endif

/* thread.c */
extern int show_thread (struct t_group *group, char *group_path, int respnum);
extern void show_thread_page (void);
extern int new_responses (int thread);
extern int which_thread (int n);
extern int which_response (int n);
extern int num_of_responses (int n);
extern int stat_thread (int n, struct t_art_stat *sbuf);
extern int next_response (int n);
extern int next_thread (int n);
extern int prev_response (int n);
extern int choose_response (int i, int n);
extern int next_unread (int n);
extern int prev_unread (int n);

/* wildmat.c */
extern int wildmat (const char *text, char *p);

/* xref.c */
extern int overview_xref_support (void);
extern void art_mark_xref_read (struct t_article *art);
extern void NSETRNG1 (t_bitmap *bitmap, long low, long high);
extern void NSETRNG0 (t_bitmap *bitmap, long low, long high);

#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
	/* mail.c */
	extern void read_mail_active_file (void);
	extern void write_mail_active_file (void);
	extern void read_mailgroups_file (void);
	extern void vGrpDelMailArt (struct t_article *psArt);
	extern void vGrpDelMailArts (struct t_group *psGrp);

	/* newsrc.c */
	extern void art_mark_deleted (struct t_article *art);
	extern void art_mark_undeleted (struct t_article *art);
#endif /* !INDEX_DAEMON && HAVE_MH_MAIL_HANDLING */

#ifdef HAVE_COLOR
	/* misc.c */
	extern void toggle_color (void);
	extern void show_color_status (void);
#endif /* HAVE_COLOR */

#endif /* PROTO_H */
