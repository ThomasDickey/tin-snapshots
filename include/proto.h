#ifndef PROTO_H
#define PROTO_H 1

/* active.c */
extern int get_active_num P_((void));
extern int resync_active_file P_((void));
extern int parse_active_line P_((char *line, long *max, long *min, char *moderated));
extern void read_news_active_file P_((void));
extern void read_group_times_file P_((void));
extern void write_group_times_file P_((void));
extern void load_newnews_info P_((char *info));
extern void read_motd_file P_((void));
extern void vMakeActiveMyGroup P_((void));

/* actived.c */
extern void create_save_active_file P_((void));
#ifdef INDEX_DAEMON
extern void vCreatePath P_((char *pcPath));
#endif

/* art.c */
extern void find_base P_((struct t_group *group));
extern int index_group P_((struct t_group *group));
extern void make_threads P_((struct t_group *group, int rethread));
extern void sort_arts P_((int sort_art_type));
extern void vWriteNovFile P_((struct t_group *psGrp));
extern char *pcFindNovFile P_((struct t_group *psGrp, int iMode));
extern void do_update P_((void));
extern void set_article P_((struct t_article *art));
extern int input_pending P_((void));
extern char *safe_fgets P_((FILE *fp));

/* attrib.c */
extern void read_attributes_file P_((char *file, int global_file));
extern void write_attributes_file P_((char *file));

/* charset.c */
extern void ConvertIso2Asc P_((char *iso, char *asc, int t));
extern void ConvertTeX2Iso P_((char *from, char *to));
extern int iIsArtTexEncoded P_((long art, char *group_path));
extern void Convert2Printable P_((unsigned char* buf));

/* color.c */
extern void fcol P_((int color));
extern void bcol P_((int color));
extern void print_color P_((char *str, t_bool signature));

/* config.c */
extern int read_config_file P_((char *file, int global_file));
extern void write_config_file P_((char *file));
extern void refresh_config_page P_((int act_option, int force_redraw));
extern int change_config_file P_((struct t_group *group, int filter_at_once));
extern void show_menu_help P_((char *help_message));
extern int match_boolean P_((char *line, char *pat, t_bool *dst));
extern int match_integer P_((char *line, char *pat, int *dst, int maxlen));
extern int match_long P_((char *line, char *pat, long *dst));
extern int match_string P_((char *line, char *pat, char *dst, size_t dstlen));
extern char *print_boolean P_((t_bool value));
extern void quote_dash_to_space P_((char *str));
extern char *quote_space_to_dash P_((char *str));

/* curses.c */
extern void setup_screen P_((void));
extern int InitScreen P_((void));
extern void InitWin P_((void));
extern void EndWin P_((void));
extern void set_keypad_on P_((void));
extern void set_keypad_off P_((void));
extern void ClearScreen P_((void));
extern void MoveCursor P_((int row, int col));
extern void CleartoEOLN P_((void));
extern void CleartoEOS P_((void));
extern void StartInverse P_((void));
extern void EndInverse P_((void));
extern void ToggleInverse P_((void));
extern int RawState P_((void));
extern void Raw P_((int state));
extern int ReadCh P_((void));
extern OUTC_RETTYPE outchar P_((OUTC_ARGS));
extern void xclick P_((int state));
extern void set_xclick_on P_((void));
extern void set_xclick_off P_((void));
extern void cursoron P_((void));
extern void cursoroff P_((void));

/* debug.c */
extern void debug_delete_files P_((void));
extern void debug_nntp P_((char *func, char *line));
extern void debug_nntp_respcode P_((int respcode));
extern void debug_print_arts P_((void));
extern void debug_print_header P_((struct t_article *s));
extern void debug_save_comp P_((void));
extern void debug_print_comment P_((char *comment));
extern void debug_print_active P_((void));
extern void debug_print_bitmap P_((struct t_group *group, struct t_article *art));
extern void debug_print_newsrc P_((struct t_newsrc *NewSrc, FILE *fp));
extern void vDbgPrintMalloc P_((int iIsMalloc, char *pcFile, int iLine, size_t iSize));
extern void debug_print_filter P_((FILE *fp, int num, struct t_filter *the_filter));
extern void debug_print_filters P_((void));

/* envarg.c */
extern void envargs P_((int *Pargc, char ***Pargv, char *envstr));

/* feed.c */
extern void feed_articles P_((int function, int level, struct t_group *group, int respnum));
extern int get_post_proc_type P_((int proc_type));

/* filter.c */
extern struct t_filter *psExpandFilterArray P_((struct t_filter *ptr, int *num));
extern void free_all_filter_arrays P_((void));
extern int read_filter_file P_((char *file, int global_file));
extern int filter_menu P_((int type, struct t_group *group, struct t_article *art));
extern int quick_filter_kill P_((struct t_group *group, struct t_article *art));
extern int quick_filter_select P_((struct t_group *group, struct t_article *art));
extern int quick_filter_select_posted_art P_((struct t_group *group, char *subj));
extern int filter_articles P_((struct t_group *group));
extern int auto_select_articles P_((struct t_group *group));

/* getline.c */
extern char *getline P_((char *prompt, int number_only, char *str, int max_chars, int passwd));

/* group.c */
extern void group_page P_((struct t_group *group));
extern void show_group_page P_((void));
extern void draw_subject_arrow P_((void));
extern void erase_subject_arrow P_((void));
extern void clear_note_area P_((void));
extern int find_new_pos P_((int old_top, long old_artnum, int cur_pos));
extern void mark_screen P_((int level, int screen_row, int screen_col, char *value));
extern void set_subj_from_size P_((int num_cols));
extern void toggle_subject_from P_((void));

/* hashstr.c */
extern char *hash_str P_((char *s));
extern void hash_init P_((void));
extern void hash_reclaim P_((void));

/* help.c */
extern void show_info_page P_((int type, char *help[], char *title));
extern void display_info_page P_((void));
extern void show_mini_help P_((int level));
extern void toggle_mini_help P_((int level));

/* inews.c */
extern void get_host_name P_((char *host_name));
extern void get_user_info P_((char *user_name, char *full_name));
extern void get_from_name P_((char *user_name, char *host_name, char *full_name, char *from_name));
extern void get_domain_name P_((char *inews_domain, char *domain));
extern int submit_news_file P_((char *name, int lines));

/* init.c */
extern void init_selfinfo P_((void));
extern int create_mail_save_dirs P_((void));
extern char *GetFQDN P_((void));
extern char *GetConfigValue P_((char *name));

/* joinpath.c */
extern void joinpath P_((char *result, char *dir, char *file));

/* list.c */
extern void init_group_hash P_((void));
extern unsigned long hash_groupname P_((char *group));
extern int find_group_index P_((char *group));
extern struct t_group *psGrpFind P_((char *pcGrpName));
extern int psGrpAdd P_((char *group));
#if 0
extern struct t_group *psGrpFirst P_((void));
extern struct t_group *psGrpLast P_((void));
extern struct t_group *psGrpNext P_((void));
extern struct t_group *psGrpPrev P_((void));
#endif

/* mail.c */
extern void read_newsgroups_file P_((void));
extern void read_groups_descriptions P_((FILE *fp, FILE *fp_save));
extern void vPrintActiveHead P_((char *pcActiveFile));
extern void vParseGrpLine P_((char *pcLine, char *pcGrpName, long *plArtMax, long *plArtMin, char *pcModerated));
extern void vFindArtMaxMin P_((char *pcGrpPath, long *plArtMax, long *plArtMin));
extern void vPrintGrpLine P_((FILE *hFp, char *pcGrpName, long lArtMax, long lArtMin, char *pcBaseDir));
extern void vMakeGrpPath P_((char *pcBaseDir, char *pcGrpName, char *pcGrpPath));
extern void vMakeGrpName P_((char *pcBaseDir, char *pcGrpName, char *pcGrpPath));
extern int iArtEdit P_((struct t_group *psGrp, struct t_article *psArt));

/* main.c */
extern int main P_((int argc, char *argv[]));
extern int read_cmd_line_groups P_((void));

/* memory.c */
extern void init_alloc P_((void));
extern void expand_art P_((void));
extern void expand_active P_((void));
extern void expand_save P_((void));
extern void expand_newnews P_((void));
extern void init_screen_array P_((int allocate));
extern void free_all_arrays P_((void));
extern void free_art_array P_((void));
extern void free_attributes_array P_((void));
extern void free_active_arrays P_((void));
extern void free_save_array P_((void));
extern void *my_malloc1 P_((char *file, int line, size_t size));
extern void *my_realloc1 P_((char *file, int line, char *p, size_t size));

/* misc.c */
extern void asfail P_((char *file, int line, char *cond));
extern void append_file P_((char *old_filename, char *new_filename));
extern void copy_fp P_((FILE *fp_ip, FILE *fp_op, char *prefix));
extern void copy_body P_((FILE *fp_ip, FILE *fp_op, char *prefix, char *initl));
extern char *get_val P_((char *env, char *def));
extern int invoke_editor P_((char *filename, int lineno));
extern int invoke_ispell P_((char *nam));
extern void shell_escape P_((void));
extern void tin_done P_((int ret));
extern void strip_double_ngs P_((char *ngs_list));
extern int my_mkdir P_((char *path, int mode));
extern int my_chdir P_((char *path));
extern void rename_file P_((char *old_filename, char *new_filename));
extern int invoke_cmd P_((char *nam));
extern void draw_percent_mark P_((long cur_num, long max_num));
extern void set_real_uid_gid P_((void));
extern void set_tin_uid_gid P_((void));
extern void base_name P_((char *dirname, char *program));
extern int mail_check P_((void));
extern void parse_from P_((char *from_line, char *eaddr, char *fname));
extern char *eat_re P_((char *s));
extern int untag_all_articles P_((void));
extern int my_isprint P_((int c));
extern void get_author P_((int thread, struct t_article *art, char *str, int len));
extern void toggle_inverse_video P_((void));
extern void show_inverse_video_status P_((void));
extern int get_arrow_key P_((void));
extern void create_index_lock_file P_((char *the_lock_file));
extern int strfquote P_((char *group, int respnum, char *s, size_t maxsize, char *format));
extern int strfpath P_((char *format, char *str, size_t maxsize, char *the_homedir, char *maildir, char *savedir, char *group));
extern int strfmailer P_((char *the_mailer, char *subject, char *to, char *filename, char *s, size_t maxsize, char *format));
extern int get_initials P_((int respnum, char *s, int maxsize));
extern void get_cwd P_((char *buf));
extern void make_group_path P_((char *name, char *path));
extern void cleanup_tmp_files P_((void));
extern void make_post_process_cmd P_((char *cmd, char *dir, char *file));
extern int stat_file P_((char *file));
extern void vPrintBugAddress P_((void));
extern int iCopyFile P_((char *pcSrcFile, char *pcDstFile));
extern int peek_char P_((FILE *fp));
#ifdef LOCAL_CHARSET
extern void buffer_to_local P_((char *b));
extern void buffer_to_network P_((char *b));
#endif

/* newsrc.c */
extern void read_newsrc P_((char *newsrc_file, int allgroups));
extern void vWriteNewsrc P_((void));
extern void backup_newsrc P_((void));
extern void subscribe P_((struct t_group *group, int sub_state));
extern void reset_newsrc P_((void));
extern void grp_mark_read P_((struct t_group *group, struct t_article *psArt));
extern void grp_mark_unread P_((struct t_group *group));
extern void thd_mark_read P_((struct t_group *group, long thread));
extern void thd_mark_unread P_((struct t_group *group, long thread));
extern void parse_unread_arts P_((struct t_group *group));
extern int pos_group_in_newsrc P_((struct t_group *group, int pos));
extern void catchup_newsrc_file P_((char *newsrc_file));
extern void expand_bitmap P_((struct t_group *group, long min));
extern void art_mark_read P_((struct t_group *group, struct t_article *art));
extern void art_mark_unread P_((struct t_group *group, struct t_article *art));
extern void art_mark_will_return P_((struct t_group *group, struct t_article *art));
extern void vSetDefaultBitmap P_((struct t_group *group));
#ifdef DEBUG_NEWSRC
extern void vNewsrcTestHarness P_((void));
#endif

/* nntplib.c */
extern char *getserverbyfile P_((char *file));
extern int server_init P_((char *machine, char *service, int port));
extern int get_tcp_socket P_((char *machine, char *service, unsigned port));
/*extern int handle_server_response P_((int response, char *nntpserver));*/ /* not used */
extern void u_put_server P_((char *string));
extern void put_server P_((char *string));
extern int get_server P_((char *string, int size));
extern void close_server P_((void));
extern char *nntp_respcode P_((int respcode));
extern int nntp_message P_((int respcode));

/* nrctbl.c */
extern void get_nntpserver P_((char *nntpserver_name, char *nick_name));
extern int get_newsrcname P_((char *newsrc_name, char *nntpserver_name));

/* open.c */
extern int nntp_open P_((void));
extern void nntp_close P_((void));
extern FILE *open_mail_active_fp P_((char *mode));
extern FILE *open_news_active_fp P_((void));
extern FILE *open_overview_fmt_fp P_((void));
extern FILE *open_newgroups_fp P_((int the_index));
extern FILE *open_motd_fp P_((char *motd_file_date));
extern FILE *open_subscription_fp P_((void));
extern FILE *open_mailgroups_fp P_((void));
extern FILE *open_newsgroups_fp P_((void));
extern FILE *open_xover_fp P_((struct t_group *psGrp, char *pcMode, long lMin, long lMax));
extern int stat_article P_((long art, char *group_path));
extern char *open_art_header P_((long art));
extern FILE *open_art_fp P_((char *group_path, long art));
extern int setup_hard_base P_((struct t_group *group, char *group_path));
extern int get_respcode P_((void));
extern int stuff_nntp P_((char *fnam));
extern FILE *nntp_to_fp P_((void));
extern void vGrpGetSubArtInfo P_((void));
extern int vGrpGetArtInfo P_((char *pcSpoolDir, char *pcGrpName, int iGrpType, long *plArtCount, long *plArtMax, long *plArtMin));

/* page.c */
extern int show_page P_((struct t_group *group, char *group_path, int respnum, int *threadnum));
extern void redraw_page P_((char *group, int respnum));
extern void show_note_page P_((char *group, int respnum));
extern int art_open P_((long art, char *group_path));
extern void art_close P_((void));
extern void yank_to_addr P_((char *orig, char *addr));
extern int match_header P_((char *buf, char *pat, char *body, char *nodec_body, size_t len));

/* parsdate.y */
extern int GetTimeInfo P_((TIMEINFO *Now));
extern time_t parsedate P_((char *p, TIMEINFO *now));

/* pgp.c */
extern void invoke_pgp_mail P_((char *nam, char *mail_to));
extern void invoke_pgp_news P_((char *the_article));
extern int pgp_check_article P_((void));

/* post.c */
extern t_bool cancel_article P_((struct t_group *group, struct t_article *art, int respnum));
extern int mail_bug_report P_((void));
extern int mail_to_author P_((char *group, int respnum, int copy_text));
extern int mail_to_someone P_((int respnum, char *address, int mail_to_poster, int confirm_to_mail, int *mailed_ok));
extern int post_article P_((char *group, int *posted_flag));
extern int post_response P_((char *group, int respnum, int copy_text));
extern int repost_article P_((char *group, struct t_article *art, int respnum, int supersede));
extern int reread_active_after_posting P_((void));
extern t_bool user_posted_messages P_((void));
extern void checknadd_headers P_((char *infile, int lines));
extern void quick_post_article P_((void));

/* prompt.c */
extern int prompt_num P_((int ch, char *prompt));
extern int prompt_string P_((char *prompt, char *buf));
extern int prompt_menu_string P_((int line, int col, char *var));
extern int prompt_yn P_((int line, char *prompt, int default_answer));
extern int prompt_yn2 P_((int line, char *prompt, int default_answer));
extern int prompt_list P_((int row, int col, int var, char *help_text, char *prompt_text, char *list[], int size));
extern void prompt_on_off P_((int row, int col, t_bool *var, char *help_text, char *prompt_text));
extern int prompt_option_string P_((int option));
extern int prompt_option_num P_((int option));
extern int prompt_option_char P_((int option));
extern void continue_prompt P_((void));

/* refs.c */
extern char *get_references P_((struct t_msgid *refptr));
extern void free_msgids P_((void));
extern void clear_art_ptrs P_((void));
extern void thread_by_reference P_((void));
extern void collate_subjects P_((void));
extern void build_references P_((struct t_group *group));

/* rfc1521.c */
extern unsigned char bin2hex P_((unsigned int x));
extern FILE *rfc1521_decode P_((FILE *file));
extern void rfc1521_encode P_((char *line, FILE *f, int e));
/* added for EUC-KR/JP/CN support by Jungshik Shin */
extern void rfc1557_encode P_((char *line, FILE *f, int e));
extern void rfc1468_encode P_((char *line, FILE *f, int e));
extern void rfc1922_encode P_((char *line, FILE *f, int e));

/* rfc1522.c */
extern int mmdecode P_((char *what, int encoding, int delimiter, char *where, char *charset));
extern void get_mm_charset P_((void));
extern char *rfc1522_decode P_((char *s));
extern char *rfc1522_encode P_((char *s));
extern void rfc15211522_encode P_((char *filename, char *mime_encoding, t_bool allow_8bit_header));

/* save.c */
extern int check_start_save_any_news P_((int check_start_save));
extern int save_art_to_file P_((int respnum, int indexnum, int the_mailbox, char *filename));
extern int save_thread_to_file P_((int is_mailbox, char *group_path));
extern int save_regex_arts P_((int is_mailbox, char *group_path));
extern int create_path P_((char *path));
extern void add_to_save_list P_((int the_index, struct t_article *the_article, int is_mailbox, int archive_save, char *path));
extern void sort_save_list P_((void));
extern int save_comp P_((t_comptype *p1, t_comptype *p2));
extern char *save_filename P_((int i));
extern int post_process_files P_((int proc_type_ch, t_bool auto_delete));
extern void delete_processed_files P_((t_bool auto_delete));
extern void print_art_seperator_line P_((FILE *fp, int the_mailbox));

/* screen.c */
extern void info_message P_((char *str));
extern void wait_message P_((char *str));
extern void error_message P_((char *template, char *str));
extern void perror_message P_((char *template, char *str));
extern void clear_message P_((void));
extern void center_line P_((int line, int inverse, char *str));
extern void draw_arrow P_((int line));
extern void erase_arrow P_((int line));
extern void show_title P_((char *title));
extern void ring_bell P_((void));
extern void spin_cursor P_((void));
extern void show_progress P_((char *dst, char *txt, int count, int total));

/* search.c */
extern int search_author P_((int the_index, int current_art, int forward));
extern void search_group P_((int forward));
extern void search_subject P_((int forward));
extern int search_article P_((int forward));
extern int search_body P_((struct t_group *group, int current_art));

/* select.c */
extern void selection_index P_((int start_groupnum, int num_cmd_line_groups));
extern void group_selection_page P_((void));
extern void erase_group_arrow P_((void));
extern void draw_group_arrow P_((void));
extern int choose_new_group P_((void));
extern int add_my_group P_((char *group, int add));
extern void set_groupname_len P_((int all_groups));
extern void toggle_my_groups P_((t_bool only_unread_groups, char *group));
extern void strip_line P_((char *line, size_t len));
extern int iSetRange P_((int iLevel, int iNumMin, int iNumMax, int iNumCur));

/* sigfile.c */
extern void msg_write_signature P_((FILE *fp, int flag));

/* signal.c */
extern RETSIGTYPE (*sigdisp P_((int sig, RETSIGTYPE (*func)(SIG_ARGS)))) P_((SIG_ARGS));
extern void set_signal_handlers P_((void));
extern void set_alarm_signal P_((void));
extern void set_alarm_clock_on P_((void));
extern void set_alarm_clock_off P_((void));
extern void signal_handler P_((SIG_ARGS));
extern int set_win_size P_((int *num_lines, int *num_cols));
extern void set_signals_art P_((void));
extern void set_signals_config P_((void));
extern void set_signals_group P_((void));
extern void set_signals_help P_((void));
extern void set_signals_page P_((void));
extern void set_signals_select P_((void));
extern void set_signals_thread P_((void));
extern void art_suspend P_((SIG_ARGS));
extern void main_suspend P_((SIG_ARGS));
extern void select_suspend P_((SIG_ARGS));
extern void group_suspend P_((SIG_ARGS));
extern void help_suspend P_((SIG_ARGS));
extern void page_suspend P_((SIG_ARGS));
extern void thread_suspend P_((SIG_ARGS));
extern void config_suspend P_((SIG_ARGS));
extern void art_resize P_((SIG_ARGS));
extern void config_resize P_((SIG_ARGS));
extern void main_resize P_((SIG_ARGS));
extern void select_resize P_((SIG_ARGS));
extern void group_resize P_((SIG_ARGS));
extern void help_resize P_((SIG_ARGS));
extern void page_resize P_((SIG_ARGS));
extern void thread_resize P_((SIG_ARGS));

/* strftime.c */
extern size_t my_strftime P_((char *s, size_t maxsize, char *format, struct tm *timeptr));

/* string.c */
extern char *tin_itoa P_((int value, int digits));
extern char *my_strdup P_((char *str));
extern char *strcasestr P_((char *haystack, char *needle));
extern int mystrcat P_((char **t, char *s));
extern void my_strncpy P_((char *p, /* const */ char *q, int n));
extern void modifiedstrncpy P_((char *target, char *source, size_t size, int decode));
extern void strcpynl P_((char *to, char *from));
extern void str_lwr P_((char *src, char *dst));

#ifndef HAVE_STRCASECMP
extern int strcasecmp P_((/* const */ char *p, /* const */ char *q));
#endif
#ifndef HAVE_STRNCASECMP
extern int strncasecmp P_((/* const */ char *p, /* const */ char *q, size_t n));
#endif
#ifndef HAVE_ATOL
extern long atol P_((char *s));
#endif
#ifndef HAVE_STRPBRK
extern char *strpbrk P_((char *str1, char *str2));
#endif
#ifndef HAVE_STRSTR
extern char *strstr P_((char *text, char *pattern));
#endif
#ifndef HAVE_STRTOL
extern long strtol P_((/* const */ char *str, char **ptr, int use_base));
#endif

/* thread.c */
extern int show_thread P_((struct t_group *group, char *group_path, int respnum));
extern void show_thread_page P_((void));
extern int new_responses P_((int thread));
extern int which_thread P_((int n));
extern int which_response P_((int n));
extern int num_of_responses P_((int n));
extern int stat_thread P_((int n, struct t_art_stat *sbuf));
extern int next_response P_((int n));
extern int next_thread P_((int n));
extern int prev_response P_((int n));
extern int choose_response P_((int i, int n));
extern int next_unread P_((int n));
extern int prev_unread P_((int n));

/* wildmat.c */
extern int wildmat P_((char *text, char *p));

/* xref.c */
extern int overview_xref_support P_((void));
extern void art_mark_xref_read P_((struct t_article *art));
extern void NSETRNG1 P_((t_bitmap *bitmap, long low, long high));
extern void NSETRNG0 P_((t_bitmap *bitmap, long low, long high));

#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
	/* mail.c */
	extern void read_mail_active_file P_((void));
	extern void write_mail_active_file P_((void));
	extern void read_mailgroups_file P_((void));
	extern void vGrpDelMailArt P_((struct t_article *psArt));
	extern void vGrpDelMailArts P_((struct t_group *psGrp));

	/* newsrc.c */
	extern void art_mark_deleted P_((struct t_article *art));
	extern void art_mark_undeleted P_((struct t_article *art));
#endif /* !INDEX_DAEMON && HAVE_MH_MAIL_HANDLING */

#ifdef HAVE_COLOR
	/* misc.c */
	extern void toggle_color P_((void));
	extern void show_color_status P_((void));
#endif /* HAVE_COLOR */

#endif /* PROTO_H */
