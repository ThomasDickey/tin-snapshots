#if __STDC__ || defined(__cplusplus)
#define P_(s) s
#else
#define P_(s) ()
#endif

/* ./active.c */
extern void init_group_hash P_((void));
extern int cmp_group_p P_((t_comptype *group1, t_comptype *group2));
extern int cmp_notify_p P_((t_comptype *notify1, t_comptype *notify2));
extern int get_active_num P_((void));
extern int resync_active_file P_((void));
extern int find_group_index P_((char *group));
extern int parse_active_line P_((char *line, long *max, long *min, char *moderated));
extern int parse_newsrc_active_line P_((FILE *fp, char *group, long *count, long *max, long *min, char *moderated));
extern void read_news_active_file P_((void));
extern void backup_active P_((int create));
extern void check_for_any_new_groups P_((void));
extern void prompt_subscribe_group P_((char *group, char *autosubscribe, char *autounsubscribe));
extern int match_group_list P_((char *group, char *group_list));
extern void read_group_times_file P_((void));
extern void write_group_times_file P_((void));
extern void load_newnews_info P_((char *info));
extern int find_newnews_index P_((char *cur_newnews_host));
extern void read_motd_file P_((void));
extern char *my_strpbrk P_((char *str1, char *str2));
extern void vMakeActiveMyGroup P_((void));
/* ./actived.c */
extern void create_save_active_file P_((void));
extern void vInitVariables P_((void));
extern void vReadCmdLineOptions P_((int iNumArgs, char *pacArgs[]));
extern void vUpdateActiveFile P_((char *pcActiveFile, char *pcDir));
extern void vPrintUsage P_((char *pcProgName));
extern void vMakeGrpList P_((char *pcActiveFile, char *pcBaseDir, char *pcGrpPath));
extern void vAppendGrpLine P_((char *pcActiveFile, char *pcGrpPath, long lArtMax, long lArtMin, char *pcBaseDir));
/* ./amiga.c */
/* ./amigatcp.c */
/* ./art.c */
extern void find_base P_((struct t_group *group));
extern int index_group P_((struct t_group *group));
extern void make_threads P_((struct t_group *group, int rethread));
extern void sort_arts P_((int sort_art_type));
extern int parse_headers P_((char *buf, struct t_article *h));
extern int iReadNovFile P_((struct t_group *group, long min, long max, int *expired));
extern void vWriteNovFile P_((struct t_group *psGrp));
extern char *pcFindNovFile P_((struct t_group *psGrp, int iMode));
extern void do_update P_((void));
extern void set_article P_((struct t_article *art));
extern int input_pending P_((void));
extern int valid_artnum P_((long art));
/* ./attrib.c */
extern void set_default_attributes P_((struct t_attribute *psAttrib));
extern void read_attributes_file P_((char *file, int global_file));
extern void set_attrib_str P_((int type, char *scope, char *str));
extern void set_attrib_num P_((int type, char *scope, int num));
extern void set_attrib P_((struct t_group *psGrp, int type, char *str, int num));
extern void write_attributes_file P_((char *file));
extern void debug_print_filter_attributes P_((void));
/* ./charset.c */
extern void ConvertIso2Asc P_((unsigned char *iso, unsigned char *asc, int t));
extern void ConvertTeX2Iso P_((unsigned char *from, unsigned char *to));
extern int iIsArtTexEncoded P_((long art, unsigned char *group_path));
/* ./color.c */
extern void fcol P_((int color));
extern void bcol P_((int color));
extern void print_color P_((char *str));
/* ./config.c */
extern int read_config_file P_((char *file, int global_file));
extern void write_config_file P_((char *file));
extern int change_config_file P_((struct t_group *group, int filter_at_once));
extern void show_config_menu P_((void));
extern void expand_rel_abs_pathname P_((int line, int col, char *str));
extern void show_menu_help P_((char *help_message));
extern int match_boolean P_((char *line, char *pat, int *dst));
extern int match_integer P_((char *line, char *pat, int *dst));
extern int match_long P_((char *line, char *pat, long *dst));
extern int match_string P_((char *line, char *pat, char *dst, size_t dstlen));
extern char *print_boolean P_((int value));
extern void quote_dash_to_space P_((char *str));
extern char *quote_space_to_dash P_((char *str));
/* ./curses.c */
extern void setup_screen P_((void));
extern int InitScreen P_((void));
extern void ScreenSize P_((int *num_lines, int *num_columns));
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
extern int outchar P_((int c));
extern void xclick P_((int state));
extern void set_xclick_on P_((void));
extern void set_xclick_off P_((void));
extern void cursoron P_((void));
extern void cursoroff P_((void));
/* ./debug.c */
extern void debug_delete_files P_((void));
extern void debug_nntp P_((char *func, char *line));
extern void debug_nntp_respcode P_((int respcode));
extern void debug_print_arts P_((void));
extern void debug_print_header P_((struct t_article *s));
extern void debug_save_comp P_((void));
extern void debug_print_comment P_((char *comment));
extern void debug_print_base P_((void));
extern void debug_print_active P_((void));
extern void debug_print_attributes P_((struct t_attribute *attr, FILE *fp));
extern void debug_print_bitmap P_((struct t_group *group, struct t_article *art));
extern void debug_print_newsrc P_((struct t_newsrc *newsrc, FILE *fp));
extern void vDbgPrintMalloc P_((int iIsMalloc, char *pcFile, int iLine, size_t iSize));
extern void debug_print_filter P_((FILE *fp, int num, struct t_filter *filter));
extern void debug_print_filters P_((void));
extern void debug_print_active_hash P_((void));
extern void debug_print_group_hash P_((void));
/* ./envarg.c */
extern int count_args P_((char *s));
extern void envargs P_((int *Pargc, char ***Pargv, char *envstr));
/* ./feed.c */
extern void feed_articles P_((int function, int level, struct t_group *group, int respnum));
extern int print_file P_((char *command, int respnum, int count));
extern int get_post_proc_type P_((int proc_type));
extern int does_article_exist P_((int function, long artnum, char *path));
/* ./filter.c */
extern struct t_filter *psExpandFilterArray P_((struct t_filter *ptr, int *num));
extern void vSetFilter P_((struct t_filter *psFilter));
extern void free_filter_item P_((struct t_filter *ptr));
extern void free_filter_array P_((struct t_filters *ptr));
extern void free_all_filter_arrays P_((void));
extern int read_filter_file P_((char *file, int global_file));
extern void vWriteFilterFile P_((char *pcFile));
extern void vWriteFilterArray P_((FILE *fp, int global, struct t_filters *ptr, long time));
extern int filter_menu P_((int type, struct t_group *group, struct t_article *art));
extern int quick_filter_kill P_((struct t_group *group, struct t_article *art));
extern int quick_filter_select P_((struct t_group *group, struct t_article *art));
extern int quick_filter_select_posted_art P_((struct t_group *group, char *subj));
extern int iAddFilterRule P_((struct t_group *psGrp, struct t_article *psArt, struct t_filter_rule *psRule));
extern int unfilter_articles P_((void));
extern int filter_articles P_((struct t_group *group));
extern int auto_select_articles P_((struct t_group *group));
extern int set_filter_scope P_((struct t_group *group));
extern char *pcChkRegexStr P_((char *pcStr));
/* ./getline.c */
extern char *getline P_((char *prompt, int number_only, char *str));
/* ./group.c */
extern void decr_tagged P_((int tag));
extern void group_page P_((struct t_group *group));
extern void show_group_page P_((void));
extern void update_group_page P_((void));
extern void draw_subject_arrow P_((void));
extern void erase_subject_arrow P_((void));
extern int prompt_subject_num P_((int ch, char *group));
extern void clear_note_area P_((void));
extern int find_new_pos P_((int old_top, long old_artnum, int cur_pos));
extern void mark_screen P_((int level, int screen_row, int screen_col, char *value));
extern void set_subj_from_size P_((int num_cols));
extern void toggle_subject_from P_((void));
extern void show_group_title P_((int clear_title));
/* ./hashstr.c */
extern char *hash_str P_((char *s));
extern void hash_init P_((void));
extern void hash_reclaim P_((void));
/* ./help.c */
extern void show_info_page P_((int type, char *help[], char *title));
extern void display_info_page P_((void));
extern void show_mini_help P_((int level));
extern void toggle_mini_help P_((int level));
/* ./inews.c */
extern int submit_inews P_((char *name));
extern void get_host_name P_((char *host_name));
extern void get_user_info P_((char *user_name, char *full_name));
extern void get_from_name P_((char *user_name, char *host_name, char *full_name, char *from_name));
extern void get_domain_name P_((char *inews_domain, char *domain));
extern int submit_news_file P_((char *name, int lines));
/* ./init.c */
extern void init_selfinfo P_((void));
extern int create_mail_save_dirs P_((void));
extern char *GetFQDN P_((void));
extern char *GetConfigValue P_((char *name));
/* ./lang.c */
/* ./list.c */
extern struct t_group *psGrpFind P_((char *pcGrpName));
extern struct t_group *psGrpFirst P_((void));
extern struct t_group *psGrpLast P_((void));
extern struct t_group *psGrpNext P_((void));
extern struct t_group *psGrpPrev P_((void));
extern void vGrpTest P_((void));
/* ./mail.c */
extern void read_mail_active_file P_((void));
extern void write_mail_active_file P_((void));
extern void read_mailgroups_file P_((void));
extern void read_newsgroups_file P_((void));
extern void read_groups_descriptions P_((FILE *fp, FILE *fp_save));
extern void vPrintActiveHead P_((char *pcActiveFile));
extern void vParseGrpLine P_((char *pcLine, char *pcGrpName, long *plArtMax, long *plArtMin, char *pcModerated));
extern void vFindArtMaxMin P_((char *pcGrpPath, long *plArtMax, long *plArtMin));
extern void vPrintGrpLine P_((FILE *hFp, char *pcGrpName, long lArtMax, long lArtMin, char *pcBaseDir));
extern long lAtol P_((char *pcStr, int iNum));
extern void vMakeGrpPath P_((char *pcBaseDir, char *pcGrpName, char *pcGrpPath));
extern void vMakeGrpName P_((char *pcBaseDir, char *pcGrpName, char *pcGrpPath));
extern void vGrpDelMailArt P_((struct t_group *psGrp, struct t_article *psArt));
extern void vGrpDelMailArts P_((struct t_group *psGrp));
extern int iArtEdit P_((struct t_group *psGrp, struct t_article *psArt));
/* ./main.c */
extern void main P_((int argc, char *argv[]));
extern void read_cmd_line_options P_((int argc, char *argv[]));
extern void usage P_((char *progname));
extern int check_for_any_new_news P_((int check_any_unread, int start_any_unread));
extern void save_or_mail_new_news P_((void));
extern void update_index_files P_((void));
extern void show_intro_page P_((void));
extern int read_cmd_line_groups P_((void));
/* ./memory.c */
extern void init_alloc P_((void));
extern void expand_art P_((void));
extern void expand_active P_((void));
extern void expand_save P_((void));
extern void expand_spooldirs P_((void));
extern void expand_newnews P_((void));
extern void init_screen_array P_((int allocate));
extern void free_all_arrays P_((void));
extern void free_art_array P_((void));
extern void free_attributes_array P_((void));
extern void free_active_arrays P_((void));
extern void free_save_array P_((void));
extern void free_spooldirs_array P_((void));
extern void free_newnews_array P_((void));
extern char *my_malloc1 P_((char *file, int line, size_t size));
extern char *my_realloc1 P_((char *file, int line, char *p, size_t size));
/* ./misc.c */
extern void asfail P_((char *file, int line, char *cond));
extern void copy_fp P_((FILE *fp_ip, FILE *fp_op, char *prefix));
extern char *get_val P_((char *env, char *def));
extern int invoke_editor P_((char *filename, int lineno));
extern int invoke_ispell P_((char *nam));
extern void shell_escape P_((void));
extern void tin_done P_((int ret));
extern long my_strtol P_((char *str, char **ptr, int base));
extern int my_mkdir P_((char *path, int mode));
extern int my_chdir P_((char *path));
extern unsigned long hash_groupname P_((char *group));
extern void rename_file P_((char *old_filename, char *new_filename));
extern void append_file P_((char *old_filename, char *new_filename));
extern char *str_dup P_((char *str));
extern int invoke_cmd P_((char *nam));
extern void draw_percent_mark P_((long cur_num, long max_num));
extern void set_real_uid_gid P_((void));
extern void set_tin_uid_gid P_((void));
extern void base_name P_((char *dirname, char *program));
extern int mail_check P_((void));
extern void parse_from P_((char *from_line, char *eaddr, char *fname));
extern char *parse_references P_((char *r));
extern long my_atol P_((char *s, int n));
extern int my_stricmp P_((char *p, char *q));
extern int my_strnicmp P_((char *p, char *q, size_t n));
extern char *eat_re P_((char *s));
extern long hash_s P_((char *s));
extern void my_strncpy P_((char *p, char *q, int n));
extern int untag_all_articles P_((void));
extern char *str_str P_((char *text, char *pattern, size_t patlen));
extern void get_author P_((int thread, struct t_article *art, char *str));
extern void toggle_inverse_video P_((void));
extern int get_arrow_key P_((void));
extern void create_index_lock_file P_((char *lock_file));
extern int strfquote P_((char *group, int respnum, char *s, int maxsize, char *format));
extern int strfeditor P_((char *editor, int linenum, char *filename, char *s, int maxsize, char *format));
extern int strfpath P_((char *format, char *str, int maxsize, char *homedir, char *maildir, char *savedir, char *group));
extern int strfmailer P_((char *mailer, char *subject, char *to, char *filename, char *s, int maxsize, char *format));
extern void get_cwd P_((char *buf));
extern void make_group_path P_((char *name, char *path));
extern void cleanup_tmp_files P_((void));
extern void make_post_process_cmd P_((char *cmd, char *dir, char *file));
extern int stat_file P_((char *file));
extern void vPrintBugAddress P_((void));
extern int iCopyFile P_((char *pcSrcFile, char *pcDstFile));
/* ./msmail.c */
/* ./newsrc.c */
extern void read_newsrc P_((char *newsrc_file, int allgroups));
extern void vWriteNewsrc P_((void));
extern void create_newsrc P_((char *newsrc_file));
extern void auto_subscribe_groups P_((char *newsrc_file));
extern void backup_newsrc P_((void));
extern void subscribe P_((struct t_group *group, int sub_state));
extern void reset_newsrc P_((void));
extern void delete_group P_((char *group));
extern void grp_mark_read P_((struct t_group *group, struct t_article *psArt));
extern void grp_mark_unread P_((struct t_group *group));
extern void thd_mark_read P_((struct t_group *group, long thread));
extern void thd_mark_unread P_((struct t_group *group, long thread));
extern void parse_bitmap_seq P_((struct t_group *group, char *seq));
extern char *pcParseSubSeq P_((struct t_group *psGrp, char *pcSeq, long *plLow, long *plHigh, int *piSum));
extern char *pcParseGetSeq P_((char *pcSeq, long *plLow, long *plHigh));
extern void parse_unread_arts P_((struct t_group *group));
extern void print_bitmap_seq P_((FILE *fp, struct t_group *group));
extern int pos_group_in_newsrc P_((struct t_group *group, int pos));
extern void catchup_newsrc_file P_((char *newsrc_file));
extern char *pcParseNewsrcLine P_((char *line, char *grp, int *sub));
extern void expand_bitmap P_((struct t_group *group, long min));
extern void art_mark_read P_((struct t_group *group, struct t_article *art));
extern void art_mark_unread P_((struct t_group *group, struct t_article *art));
extern void art_mark_will_return P_((struct t_group *group, struct t_article *art));
extern void art_mark_deleted P_((struct t_group *group, struct t_article *art));
extern void art_mark_undeleted P_((struct t_group *group, struct t_article *art));
extern void vSetDefaultBitmap P_((struct t_group *group));
extern char *getaline P_((FILE *fp));
/* ./nntplib.c */
extern char *getserverbyfile P_((char *file));
extern int server_init P_((char *machine, char *service, int port));
extern int get_tcp_socket P_((char *machine, char *service, int port));
extern int handle_server_response P_((int response, char *nntpserver));
extern void u_put_server P_((char *string));
extern void put_server P_((char *string));
extern int get_server P_((char *string, int size));
extern void close_server P_((void));
extern char *nntp_respcode P_((int respcode));
extern int nntp_message P_((int respcode));
/* ./nntpw32.c */
/* ./open.c */
extern int nntp_open P_((void));
extern void nntp_close P_((void));
extern FILE *open_mail_active_fp P_((char *mode));
extern FILE *open_news_active_fp P_((void));
extern FILE *open_overview_fmt_fp P_((void));
extern FILE *open_newgroups_fp P_((int index));
extern FILE *open_motd_fp P_((char *motd_file_date));
extern FILE *open_subscription_fp P_((void));
extern FILE *open_mailgroups_fp P_((void));
extern FILE *open_newsgroups_fp P_((void));
extern FILE *open_xover_fp P_((struct t_group *psGrp, char *pcMode, long lMin, long lMax));
extern int stat_article P_((long art, char *group_path));
extern char *open_art_header P_((long art));
extern FILE *open_art_fp P_((char *group_path, long art));
extern FILE *open_xhdr_fp P_((char *header, long min, long max));
extern int base_comp P_((t_comptype *p1, t_comptype *p2));
extern int setup_hard_base P_((struct t_group *group, char *group_path));
extern int get_respcode P_((void));
extern int stuff_nntp P_((char *fnam));
extern FILE *nntp_to_fp P_((void));
extern void log_user P_((void));
extern void authorization P_((char *server, char *authuser));
extern void vGrpGetSubArtInfo P_((void));
extern void vGrpGetArtInfo P_((char *pcSpoolDir, char *pcGrpName, int iGrpType, long *plArtCount, long *plArtMax, long *plArtMin));
/* ./os_2.c */
/* ./page.c */
extern int show_page P_((struct t_group *group, char *group_path, int respnum, int *threadnum));
extern void redraw_page P_((char *group, int respnum));
extern void show_note_page P_((char *group, int respnum));
extern void show_mime_article P_((FILE *fp, struct t_article *art));
extern void show_first_header P_((int respnum, char *group));
extern void show_cont_header P_((int respnum));
extern int art_open P_((long art, char *group_path));
extern void art_close P_((void));
extern int prompt_response P_((int ch, int respnum));
extern void yank_to_addr P_((char *orig, char *addr));
extern int show_last_page P_((void));
extern int match_header P_((char *buf, char *pat, char *body, size_t len));
/* ./parsdate.y */
extern int GetTimeInfo P_((TIMEINFO *Now));
extern time_t parsedate P_((char *p, TIMEINFO *now));
/* ./pgp.c */
extern void invoke_pgp_mail P_((char *nam, char *mail_to));
extern void invoke_pgp_news P_((char *article));
extern int pgp_check_article P_((void));
/* ./post.c */
extern void msg_init_headers P_((void));
extern void msg_free_headers P_((void));
extern void msg_add_header P_((char *name, char *text));
extern int msg_write_headers P_((FILE *fp));
extern int user_posted_messages P_((void));
extern void update_posted_info_file P_((char *group, int action, char *subj));
extern void update_posted_msgs_file P_((char *file, char *addr));
extern int check_article_to_be_posted P_((char *article, int art_type, int *lines));
extern void setup_check_article_screen P_((int *init));
extern void quick_post_article P_((void));
extern int post_article P_((char *group, int *posted));
extern int post_response P_((char *group, int respnum, int copy_text));
extern int mail_to_someone P_((int respnum, char *address, int mail_to_poster, int confirm_to_mail, int *mailed_ok));
extern int mail_bug_report P_((void));
extern int mail_to_author P_((char *group, int respnum, int copy_text));
extern int pcCopyArtHeader P_((int iHeader, char *pcArt, char *result));
extern int delete_article P_((struct t_group *group, struct t_article *art));
extern int repost_article P_((char *group, struct t_article *art, int respnum));
extern void msg_add_x_headers P_((char *headers));
extern int msg_add_x_body P_((FILE *fp_out, char *body));
extern void insert_x_headers P_((char *infile, int lines));
extern int insert_from_header P_((char *infile));
extern void find_reply_to_addr P_((int respnum, char *from_addr));
extern int reread_active_after_posting P_((void));
extern void update_active_after_posting P_((char *newsgroups));
extern int submit_mail_file P_((char *file));
#ifdef FORGERY
extern void make_path_header P_((char *line, char *from_name));
#endif
/* ./prompt.c */
extern int prompt_num P_((int ch, char *prompt));
extern int prompt_string P_((char *prompt, char *buf));
extern int prompt_menu_string P_((int line, int col, char *var));
extern int prompt_yn P_((int line, char *prompt, int default_answer));
extern void prompt_on_off P_((int row, int col, int *var, char *help_text, char *prompt_text));
extern int prompt_list P_((int row, int col, int var, char *help_text, char *prompt_text, char **list, int));
extern void continue_prompt P_((void));
/* ./save.c */
extern int check_start_save_any_news P_((int check_start_save));
extern int save_art_to_file P_((int respnum, int indexnum, int mailbox, char *filename));
extern int save_thread_to_file P_((int is_mailbox, char *group_path));
extern int save_regex_arts P_((int is_mailbox, char *group_path));
extern int create_path P_((char *path));
extern int create_sub_dir P_((int i));
extern void add_to_save_list P_((int index, struct t_article *article, int is_mailbox, int archive_save, char *path));
extern void sort_save_list P_((void));
extern int save_comp P_((t_comptype *p1, t_comptype *p2));
extern char *save_filename P_((int i));
extern char *get_first_savefile P_((void));
extern char *get_last_savefile P_((void));
extern int post_process_files P_((int proc_type_ch, int auto_delete));
extern void post_process_uud P_((int pp, int auto_delete));
extern void uudecode_file P_((int pp, char *file_out_dir, char *file_out));
extern void post_process_sh P_((int auto_delete));
extern char *get_archive_file P_((char *dir));
extern void delete_processed_files P_((int auto_delete));
extern void print_art_seperator_line P_((FILE *fp, int mailbox));
/* ./screen.c */
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
/* ./search.c */
extern int search_author P_((int index, int current_art, int forward));
extern void search_group P_((int forward));
extern void search_subject P_((int forward));
extern int search_article P_((int forward));
extern int search_body P_((struct t_group *group, int current_art));
extern int search_art_body P_((char *group_path, struct t_article *art, char *pat, size_t len));
extern void str_lwr P_((char *src, char *dst));
/* ./select.c */
extern void selection_index P_((int start_groupnum, int num_cmd_line_groups));
extern void group_selection_page P_((void));
extern int prompt_group_num P_((int ch));
extern void erase_group_arrow P_((void));
extern void draw_group_arrow P_((void));
extern void yank_active_file P_((void));
extern int choose_new_group P_((void));
extern int add_my_group P_((char *group, int add));
extern int reposition_group P_((struct t_group *group, int default_num));
extern void catchup_group P_((struct t_group *group, int goto_next_unread_group));
extern int next_unread_group P_((int enter_group));
extern void set_groupname_len P_((int all_groups));
extern void toggle_my_groups P_((int only_unread_groups, char *group));
extern void goto_next_group_on_screen P_((void));
extern void strip_line P_((char *line, size_t len));
extern int iSetRange P_((int iLevel, int iNumMin, int iNumMax, int iNumCur));
extern int iParseRange P_((char *pcRange, int iNumMin, int iNumMax, int iNumCur, int *piRngMin, int *piRngMax));
extern void vDelRange P_((int iLevel, int iNumMax));
/* ./sigfile.c */
extern void msg_write_signature P_((FILE *fp, int flag));
extern FILE *open_random_sig P_((char *sigdir));
extern int thrashdir P_((char *sigdir));
/* ./signal.c */
#ifdef M_AMIGA
/* This is to work around a compiler bug in SAS-C 6.51 */
extern RETSIGTYPE (*sigdisp P_((int /* sig */, RETSIGTYPE (*func)(SIG_ARGS)))) P_((SIG_ARGS));
#else
extern RETSIGTYPE (*sigdisp P_((int sig, RETSIGTYPE (*func)(SIG_ARGS)))) P_((SIG_ARGS));
#endif
extern void set_signal_handlers P_((void));
extern void set_alarm_signal P_((void));
extern void set_alarm_clock_on P_((void));
extern void set_alarm_clock_off P_((void));
extern void signal_handler P_((SIG_ARGS));
extern int set_win_size P_((int *num_lines, int *num_cols));
extern void set_signals_art P_((void));
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
extern void main_resize P_((SIG_ARGS));
extern void select_resize P_((SIG_ARGS));
extern void group_resize P_((SIG_ARGS));
extern void help_resize P_((SIG_ARGS));
extern void page_resize P_((SIG_ARGS));
extern void thread_resize P_((SIG_ARGS));
/* ./strftime.c */
extern size_t my_strftime P_((char *s, size_t maxsize, char *format, struct tm *timeptr));
/* ./thread.c */
extern int show_thread P_((struct t_group *group, char *group_path, int respnum));
extern void show_thread_page P_((void));
extern void update_thread_page P_((void));
extern void draw_thread_arrow P_((void));
extern void erase_thread_arrow P_((void));
extern int prompt_thread_num P_((int ch));
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
/* ./wildmat.c */
extern int wildmat P_((char *text, char *p));
/* ./win32.c */
/* ./win32tcp.c */
/* ./xref.c */
extern int overview_xref_support P_((void));
extern void art_mark_xref_read P_((struct t_article *art));
extern void NSETRNG1 P_((t_bitmap *bitmap, long low, long high));
extern void NSETRNG0 P_((t_bitmap *bitmap, long low, long high));

#undef P_
