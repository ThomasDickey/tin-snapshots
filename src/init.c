/*
 *  Project   : tin - a Usenet reader
 *  Module    : init.c
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 21-12-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"menukeys.h"
#include	"patchlev.h"

char active_times_file[PATH_LEN];
char global_attributes_file[PATH_LEN];
char local_attributes_file[PATH_LEN];
char add_addr[LEN];			/* address to add to rR reply to author with mail */
char article[PATH_LEN];			/* ~/.article file */
char bug_addr[LEN];			/* address to add send bug reports to */
char cmd_line_printer[PATH_LEN];	/* printer program specified on cmd line */
char cmdline_nntpserver[PATH_LEN];
char cvers[LEN];
char dead_article[PATH_LEN];		/* ~/dead.article file */
char dead_articles[PATH_LEN];		/* ~/dead.articles file */
char default_editor_format[PATH_LEN];	/* editor + parameters  %E +%N %F */
char default_mailer_format[PATH_LEN];	/* mailer + parameters  %M %S %T %F */
char default_maildir[PATH_LEN];		/* mailbox dir where = saves are stored */
char default_organization[PATH_LEN];	/* Organization: */
char default_post_newsgroups[PATH_LEN];
char default_post_subject[PATH_LEN];
char default_printer[LEN];		/* printer program specified from tinrc */
char default_range_group[PATH_LEN];
char default_range_select[PATH_LEN];
char default_range_thread[PATH_LEN];
char default_select_pattern[LEN];
char default_sigfile[PATH_LEN];
char default_signature[PATH_LEN];
char default_shell_command[LEN];	/* offers user default choice */
char default_savedir[PATH_LEN];		/* directory to save articles to */
char global_config_file[PATH_LEN];
char global_filter_file[PATH_LEN];
char group_times_file[PATH_LEN];
char homedir[PATH_LEN];
char index_maildir[PATH_LEN];
char index_newsdir[PATH_LEN];
char index_savedir[PATH_LEN];
char libdir[PATH_LEN];			/* directory where news config files are (ie. active) */
char lock_file[PATH_LEN];		/* contains name of index lock file */
char local_config_file[PATH_LEN];
char local_filter_file[PATH_LEN];
char local_newsgroups_file[PATH_LEN];	/* local copy of NNTP newsgroups file */
char local_newsrctable_file[PATH_LEN];
char mail_news_user[LEN];		/* mail new news to this user address */
char mail_quote_format[PATH_LEN];
char mail_active_file[PATH_LEN];
#ifdef FORGERY
char mail_address[LEN];			/* user's mail address */
#endif
char mailbox[PATH_LEN];			/* system mailbox for each user */
char mailer[PATH_LEN];			/* mail program */
char mime_encoding[LEN]="8bit";
char motd_file[PATH_LEN];		/* news motd file for newsadmin purposes */
char motd_file_info[PATH_LEN];		/* date of last time news motd file read */
char my_distribution[LEN];		/* Distribution: */
char news_active_file[PATH_LEN];
char news_quote_format[PATH_LEN];
char mailgroups_file[PATH_LEN];
char newsgroups_file[PATH_LEN];
char newsrc[PATH_LEN];
char newnewsrc[PATH_LEN];
char novrootdir[PATH_LEN];		/* root directory of nov index files */
char page_header[LEN];			/* page header of pgm name and version */
char post_proc_command[PATH_LEN];	/* Post processing command */
char posted_info_file[PATH_LEN];
char posted_msgs_file[PATH_LEN];
char progname[PATH_LEN];		/* program name */
char quote_chars[PATH_LEN];		/* quote chars for posting/mails ": " */
char rcdir[PATH_LEN];
#ifdef VMS
char rcdir_asfile[PATH_LEN];		/* rcdir expressed as dev:[dir]tin.dir, for stat() */
#endif
char reply_to[LEN];			/* Reply-To: address */
char save_active_file[PATH_LEN];
char spooldir[PATH_LEN];		/* directory where news is */
char subscriptions_file[PATH_LEN];
char msg_headers_file[PATH_LEN];	/* $HOME/.tin/headers */
char txt_help_bug_report[LEN];		/* address to add send bug reports to */
char userid[PATH_LEN];
char xpost_quote_format[PATH_LEN];
#ifdef M_OS2
char TMPDIR[PATH_LEN];
#endif

int art_marked_deleted;
int art_marked_inrange;
int art_marked_return;
int art_marked_selected;
int art_marked_unread;
int auto_list_thread;			/* list thread when entering it using right arrow */	
int NOTESLINES;				/* set in set_win_size () */
int RIGHT_POS;				/* set in set_win_size () */
int MORE_POS;				/* set in set_win_size () */
int confirm_action;
int confirm_to_quit;
int max_subj = 0;
int max_from = 0;
int group_top;				/* one past top of my_group */
int groupname_len = 0;			/* one past top of my_group */
int catchup = FALSE;			/* mark all arts read in all subscribed groups */
int update_fork = FALSE;		/* update index files by forked tin -u */
int verbose = FALSE;			/* update index files only mode */
int start_line_offset = 1;		/* used by invoke_editor for line no. */
int read_news_via_nntp = FALSE;		/* read news locally or via NNTP */
int local_index;			/* do private indexing? */
int read_saved_news = FALSE;		/* tin -R read saved news from tin -S */
int real_gid;
int real_uid;
int real_umask;
int show_description;
int strip_blanks;
int sigdashes;				/* set TRUE to prepend every signature with dashes */
int start_editor_offset;
int tin_uid;
int tin_gid;
int top = 0;
int top_base;
int check_any_unread = FALSE;
int start_any_unread = FALSE;

int beginner_level;			/* beginner level (shows mini help a la elm) */
int catchup_read_groups;		/* ask if read groups are to be marked read */
int cmd_line;				/* batch / interactive mode */
int check_for_new_newsgroups;		/* don't check for new newsgroups */
int count_articles;			/* count articles on spooldir or via GROUP cmd */
int created_rcdir;			/* checks if first time tin is started */
int default_auto_save;			/* save thread with name from Archive-name: field */
int default_auto_save_msg;		/* save posted message to ~/Mail/posted */
int default_batch_save;			/* save arts if -M/-S command line switch specified */
int default_filter_days;		/* num of days an article  filter can be active */
int default_save_mode;			/* Append/Overwrite existing file when saving */
int default_show_author;		/* show_author value from 'M' menu in tinrc */
int default_show_only_unread;		/* show only new/unread arts or all arts */
int default_sort_art_type;		/* sort arts[] array by subject,from or date field */
int default_thread_arts;		/* threading system for viewing articles */
int delete_index_file;			/* delete index file before indexing (tind only) */
int draw_arrow_mark;			/* draw -> or highlighted bar */
int force_screen_redraw;		/* force screen redraw after external (shell) commands */
int full_page_scroll;			/* page half/full screen of articles/groups */
int group_catchup_on_exit;		/* catchup group with left arrow key or not */
int thread_catchup_on_exit;		/* catchup thread with left arrow key or not */
int groupname_max_length;		/* max len of group names to display on screen */
int use_keypad;				/* enables/disables scroll keys on supported terminals */
#ifdef HAVE_METAMAIL
int use_metamail;			/* enables/disables metamail on MIME messages */
int ask_for_metamail;			/* enables/disables the metamail query if a MIME message is going to be displayed */
#endif
#ifdef HAVE_COLOR
int use_color;				/* enables/disables ansi-color support under linux-console and color-xterm */
int use_color_tinrc;			/* like use_color but stored in tinrc */
int col_back;				/* standard bacground color */
int col_invers;				/* color of inverse text */
int col_text;				/* color of textlines*/
int col_minihelp;			/* color of mini help menu*/
int col_help;				/* color of help pages */
int col_message;			/* color of message lines at bottom */
int col_quote;				/* color of quotelines */
int col_head;				/* color of headerlines */
int col_subject;			/* color of article subject */
int col_response;			/* color of respone counter */
int col_from;				/* color of sender (From:) */
int col_normal;				/* standard foreground color */
int col_title;				/* color of Help/Mail-Sign */
int col_signature;			/* color of signature */
#endif
int use_mouse;				/* enables/disables mouse support under xterm */
int auto_cc;				/* add your name to cc automatically */
int auto_bcc;				/* add your name to bcc automatically */
int global_filtered_articles;		/* globally killed / auto-selected articles */
int local_filtered_articles;		/* locally killed / auto-selected articles */
int iso2asc_supported;			/* Convert ISO-Latin1 to Ascii */
int mark_saved_read;			/* mark saved article/thread as read */
int newsrc_active;
int num_of_selected_arts;
int num_of_killed_arts;
int num_of_tagged_arts;
int process_id;
int pos_first_unread;			/* position cursor at first/last unread article */
int default_post_proc_type;		/* type of post processing to be performed */
int post_article_and_exit;		/* quick post of an article then exit (elm like) */
int print_header;			/* print all of mail header or just Subject: & From lines */
int purge_index_files;			/* stat all articles to see if they still exist */
int default_filter_kill_header;
int default_filter_kill_global;
int default_filter_kill_case;
int default_filter_kill_expire;
int default_filter_select_header;
int default_filter_select_global;
int default_filter_select_case;
int default_filter_select_expire;
int process_only_unread;		/* save/print//mail/pipe unread/all articles */
int read_local_newsgroups_file;		/* read newsgroups file locally or via NNTP */
int mail_news;				/* mail all arts to specified user */
int save_news;				/* save all arts to savedir structure */
int save_to_mmdf_mailbox;		/* save mail to MMDF/mbox format mailbox */
int show_author;
int show_lines;
int show_last_line_prev_page;		/* set TRUE to see last line of prev page (ala nn) */
int show_only_unread_groups;		/* set TRUE to see only subscribed groups with new news */
int show_xcommentto;			/* set TRUE to show X-Comment-To-Header */
int highlight_xcommentto;		/* set TRUE to show X-Comment-To inverse in the upper right corner */
int system_status;
int tab_after_X_selection;		/* set TRUE if you want auto TAB after X */
int tab_goto_next_unread;
int space_goto_next_unread;
int tex2iso_supported;			/* Convert german style TeX to ISO-Latin1 */
int update;				/* update index files only mode */
int use_builtin_inews;
int xref_supported = TRUE;
int xover_supported = FALSE;
int xuser_supported = FALSE;
int no_advertising = FALSE;
int xmouse, xrow, xcol;			/* xterm button pressing information */
int reread_active_file_secs;		/* reread active file interval in seconds */

#ifndef M_AMIGA
struct passwd *myentry;
struct passwd pwdentry;
#endif

/*
 * Get users home directory, userid, and a bunch of other stuff!
 */

void init_selfinfo ()
{
	char nam[LEN];
	char *ptr;
	FILE *fp;
	struct stat sb;

	process_id = getpid ();
#if defined(M_AMIGA) || defined(M_OS2)
	tin_uid = tin_gid = 0;
	real_uid = real_gid = (getenv ("TIND") ? 1 : 0);
#else
	tin_uid = geteuid ();
	tin_gid = getegid ();
	real_uid = getuid ();
	real_gid = getgid ();

	real_umask = umask (0);
	umask (real_umask);
#endif	/* M_AMIGA */
	
#ifdef HAVE_SETLOCALE
	setlocale (LC_ALL, "");
#endif
		
#ifdef M_AMIGA
	if ((ptr = getenv ("USERNAME")) != (char *) 0) {
		my_strncpy (userid, ptr, sizeof (userid));
	} else {
		error_message (txt_env_var_not_found, "USERNAME");
		tin_done (EXIT_ERROR);
	}
	if ((ptr = getenv ("HOME")) != (char *) 0) {
		my_strncpy (homedir, ptr, sizeof (homedir));
	} else {
		error_message (txt_env_var_not_found, "HOME");
		tin_done (EXIT_ERROR);
	}
#else
	myentry = (struct passwd *) 0;
	if (((ptr = getlogin ()) != (char *) 0) && strlen (ptr)) {
		myentry = getpwnam (ptr);
	}
	if (myentry == (struct passwd *) 0) {	
		myentry = getpwuid (getuid ());
	}
	if (myentry != (struct passwd *) 0) {	
		memcpy (&pwdentry, myentry, sizeof (struct passwd));
		myentry = &pwdentry;
	}
#if defined(M_OS2) || defined(WIN32)
	if (myentry == (struct passwd *) 0) {
		fprintf (stderr, "Environment variable USER not set.\n");
		exit (1);
	}
#endif

#ifdef M_OS2
	strcpy (TMPDIR, get_val ("TMP", "/tmp/"));
	if ((TMPDIR[strlen(TMPDIR)-1] != '/') && (TMPDIR[strlen(TMPDIR)-1] != '\\')) {
		strcat(TMPDIR,"/");
	}
#endif
#ifdef WIN32
	strcpy (TMPDIR, get_val ("TMP", "\\tmp\\"));
	if (TMPDIR[strlen(TMPDIR)-1] != '\\') {
		strcat(TMPDIR,"\\");
	}
#endif
	strcpy (userid, myentry->pw_name);

	if ((ptr = getenv ("TIN_HOMEDIR")) != (char *) 0) {
		my_strncpy (homedir, ptr, sizeof (homedir));
	} else if ((ptr = getenv ("HOME")) != (char *) 0) {
		my_strncpy (homedir, ptr, sizeof (homedir));
	} else if (! myentry) {
		strcpy (homedir, "/tmp");
	} else {
		my_strncpy (homedir, myentry->pw_dir, sizeof (homedir));
	}
#endif	/* M_AMIGA */

	/*
	 * we're setuid, so index in /usr/spool/news even if user root
	 * This is quite essential if non local index files are 
	 * to be updated during the night from crontab by root.
	 */
	if (tin_uid != real_uid) {
		local_index = FALSE;
		set_real_uid_gid ();
	} else {	/* index in users home directory ~/.tin/.index */
		local_index = TRUE;
	}


	art_marked_deleted = ART_MARK_DELETED;
	art_marked_inrange = MARK_INRANGE;
	art_marked_return = ART_MARK_RETURN;
	art_marked_selected = ART_MARK_SELECTED;
	art_marked_unread = ART_MARK_UNREAD;
	auto_cc = FALSE;
	auto_bcc = FALSE;
	auto_list_thread = TRUE;
	beginner_level = TRUE;
	catchup_read_groups = FALSE;
	cmdline_nntpserver[0] = '\0';
	confirm_action = TRUE;
	confirm_to_quit = TRUE;
	count_articles = FALSE;
	created_rcdir = FALSE;
#ifdef USE_INVERSE_HACK
	inverse_okay = FALSE;
	draw_arrow_mark = TRUE;
#else
	inverse_okay = TRUE;
	draw_arrow_mark = FALSE;
#endif
	default_auto_save = TRUE;
	default_auto_save_msg = TRUE;
	default_batch_save = FALSE;
	default_filter_days = DEFAULT_FILTER_DAYS;
	default_filter_kill_header = FILTER_SUBJ_CASE_SENSITIVE;
	default_filter_kill_global = TRUE;
	default_filter_kill_case = FALSE;
	default_filter_kill_expire = FALSE;
	default_filter_select_header = FILTER_SUBJ_CASE_SENSITIVE;
	default_filter_select_global = TRUE;
	default_filter_select_case = FALSE;
	default_filter_select_expire = FALSE;
	default_move_group = 0;
	default_post_proc_type = POST_PROC_NONE;
	default_save_mode = iKeySaveAppendFile;
	default_show_author = SHOW_FROM_NAME;
	default_show_only_unread = TRUE;
	default_sort_art_type = SORT_BY_DATE_ASCEND;
	default_thread_arts = THREAD_SUBJ;
	delete_index_file = FALSE;
	force_screen_redraw = FALSE;
	full_page_scroll = TRUE;
	group_catchup_on_exit = TRUE;
	groupname_max_length = 32;
	global_filtered_articles = FALSE;
	local_filtered_articles = FALSE;
	iso2asc_supported = atoi (get_val ("ISO2ASC", DEFAULT_ISO2ASC));
	mark_saved_read = TRUE;
	newsrc_active = FALSE;
	num_of_selected_arts = 0;
	num_of_killed_arts = 0;
	num_of_tagged_arts = 0;
	pos_first_unread = TRUE;
	post_article_and_exit = FALSE;
	print_header = FALSE;
	process_only_unread = TRUE;
	purge_index_files = FALSE;
	read_local_newsgroups_file = FALSE;
	reread_active_file = TRUE;
	reread_active_file_secs = REREAD_ACTIVE_FILE_SECS;
	save_news = FALSE;
#ifdef HAVE_MMDF_MAILER
	save_to_mmdf_mailbox = TRUE;
#else
	save_to_mmdf_mailbox = FALSE;
#endif
	show_last_line_prev_page = FALSE;
	show_lines = TRUE;
	show_description = TRUE;
	show_only_unread_groups = FALSE;
	show_xcommentto = FALSE;
	highlight_xcommentto = FALSE;
	sigdashes = TRUE;
	strip_blanks = TRUE;
#ifdef M_UNIX
	start_editor_offset = TRUE;
#else	
	start_editor_offset = FALSE;
#endif
	tab_after_X_selection = FALSE;
	tab_goto_next_unread = TRUE;
	space_goto_next_unread = FALSE;
	tex2iso_supported = atoi (get_val ("TEX2ISO", "0"));
	thread_catchup_on_exit = TRUE;
#ifdef INDEX_DAEMON
	check_for_new_newsgroups = FALSE;
	update = TRUE;
#else
	check_for_new_newsgroups = TRUE;
	update = FALSE;
#endif
	use_builtin_inews = TRUE;
	use_keypad = FALSE;
	use_mouse = FALSE; /* default changed to FALSE (eb) */
#ifdef HAVE_METAMAIL
#	ifdef M_AMIGA
		/* for all those AmigaElm users ... ;-) */
		use_metamail = (getenv ("NoMetaMail") != NULL) ? TRUE : FALSE;
#	else
		use_metamail = TRUE;
#	endif
	ask_for_metamail = TRUE;
#endif
#ifdef HAVE_COLOR
	use_color = FALSE;
	use_color_tinrc = FALSE;
	col_back = 0;
	col_invers = 4;
	col_text = 7;
	col_minihelp = 3;
	col_help = 7;
	col_message = 6;
	col_quote = 2;
	col_head = 2;
	col_subject = 6;
	col_response = 2;
	col_from = 2;
	col_normal = 7;
	col_title = 4;
	col_signature = 4;
#endif
	index_maildir[0] = '\0';
	index_newsdir[0] = '\0';
	index_savedir[0] = '\0';
	newsrc[0] = '\0';
	
	strncpy (mail_quote_format, txt_mail_quote, sizeof (mail_quote_format));
	strncpy (news_quote_format, txt_news_quote, sizeof (news_quote_format));
	strncpy (xpost_quote_format, txt_xpost_quote, sizeof (xpost_quote_format));

	sprintf (page_header, "%s %s release %s [%s%s%s]", 
		progname, VERSION, RELEASEDATE, OS,
		(tex2iso_supported ? " TeX2ISO" : ""),
		(iso2asc_supported >= 0 ? " ISO2ASC" : ""));
	sprintf (cvers, txt_copyright_notice, page_header);

	cmd_line_printer[0] = '\0';
	default_art_search[0] = '\0';
	default_author_search[0] = '\0';
	default_editor_format[0] = '\0';
	default_goto_group[0] = '\0';
	default_group_search[0] = '\0';
	default_mail_address[0] = '\0';
	default_mailer_format[0] = '\0';
	default_organization[0] = '\0';
	default_pipe_command[0] = '\0';
	default_post_newsgroups[0] = '\0';
	default_post_subject[0] = '\0';
	strcpy (default_range_group, "1-.");
	strcpy (default_range_select, "1-.");
	strcpy (default_range_thread, "0-.");
	default_regex_pattern[0] = '\0';
	default_repost_group[0] = '\0';
	strcpy (default_save_file, "savefile.tin");
	default_select_pattern[0] = '\0';
	default_shell_command[0] = '\0';
	default_subject_search[0] = '\0';
	post_proc_command[0] = '\0';
	proc_ch_default = 'n';

	/* 
	 * Amiga uses assigns which end in a ':' and won't work with a '/'
	 * tacked on after them: e.g. we want UULIB:active, and not 
	 * UULIB:/active. For this reason I have changed the sprintf calls
	 * to joinpath. This is defined to sprintf(result,"%s/%s",dir,file)
	 * on all UNIX systems.
	 */

	strcpy (libdir, get_val ("TIN_LIBDIR", LIBDIR));
	strcpy (novrootdir, get_val ("TIN_NOVROOTDIR", NOVROOTDIR));
	strcpy (spooldir, get_val ("TIN_SPOOLDIR", SPOOLDIR));
#ifdef VMS
	joindir (rcdir, homedir, RCDIR); /* we're naming a directory here */
	joinpath (rcdir_asfile, homedir, RCDIR);	/* for stat() */
	strcat(rcdir_asfile, ".DIR");
	if (stat (rcdir_asfile, &sb) == -1) {
#else
	joinpath (rcdir, homedir, RCDIR);
	if (stat (rcdir, &sb) == -1) {
#endif
		created_rcdir = TRUE;
		my_mkdir (rcdir, 0755);
	}

	strcpy (bug_addr, BUG_REPORT_ADDRESS);
#if defined(M_UNIX) || defined (M_AMIGA) || defined(VMS)
	strcpy (default_mailer_format, MAILER_FORMAT);
#else
	strcpy (default_mailer_format, mailer);
#endif
	strcpy (default_printer, DEFAULT_PRINTER);
#ifdef M_AMIGA
	if (tin_bbs_mode) {
		strcpy(default_printer, DEFAULT_BBS_PRINTER);
	}
#endif
	strcpy (mailer, get_val (ENV_VAR_MAILER, DEFAULT_MAILER));
	strcpy (quote_chars, DEFAULT_COMMENT);

	joinpath (active_times_file, libdir, ACTIVE_TIMES_FILE);
#ifdef VMS
	joinpath (article, homedir, "article.");
#else
	joinpath (article, homedir, ".article");
#endif
	joinpath (dead_article, homedir, "dead.article");
	joinpath (dead_articles, homedir, "dead.articles");

#ifdef VMS
	joindir (default_maildir, "~", DEFAULT_MAILDIR);
	joindir (default_savedir, "~", DEFAULT_SAVEDIR);
#else
	joinpath (default_maildir, "~", DEFAULT_MAILDIR);
	joinpath (default_savedir, "~", DEFAULT_SAVEDIR);
#endif
	joinpath (default_sigfile, "~", ".Sig");
	joinpath (default_signature, homedir, ".signature");
	joinpath (global_attributes_file, libdir, ATTRIBUTES_FILE);
	joinpath (global_config_file, libdir, CONFIG_FILE);
	joinpath (global_filter_file, libdir, FILTER_FILE);
	if (! index_newsdir[0]) {
#ifdef VMS
		joindir (index_newsdir, get_val ("TIN_INDEX_NEWSDIR", rcdir), INDEX_NEWSDIR);
#else
		joinpath (index_newsdir, get_val ("TIN_INDEX_NEWSDIR", rcdir), INDEX_NEWSDIR);
#endif
	}
	if (stat (index_newsdir, &sb) == -1) {
		my_mkdir (index_newsdir, 0777);
	}
#ifdef VMS
	joindir (index_maildir, get_val ("TIN_INDEX_MAILDIR", rcdir), INDEX_MAILDIR);
#else
	joinpath (index_maildir, get_val ("TIN_INDEX_MAILDIR", rcdir), INDEX_MAILDIR);
#endif
	if (stat (index_maildir, &sb) == -1) {
		my_mkdir (index_maildir, 0777);
	}
	joinpath (index_savedir, get_val ("TIN_INDEX_SAVEDIR", rcdir), INDEX_SAVEDIR);
	if (stat (index_savedir, &sb) == -1) {
		my_mkdir (index_savedir, 0777);
	}
	joinpath (local_attributes_file, rcdir, ATTRIBUTES_FILE);
	joinpath (local_config_file, rcdir, CONFIG_FILE);
	joinpath (local_filter_file, rcdir, FILTER_FILE);
	joinpath (local_newsrctable_file, rcdir, NEWSRCTABLE_FILE);
	joinpath (local_newsgroups_file, rcdir, NEWSGROUPS_FILE);
	joinpath (mail_active_file, rcdir, ACTIVE_MAIL_FILE);
#ifdef VMS
	joinpath (mailbox, DEFAULT_MAILBOX, "MAIL.TXT");
#else
	joinpath (mailbox, DEFAULT_MAILBOX, userid);
#endif
	joinpath (msg_headers_file, rcdir, MSG_HEADERS_FILE);
	joinpath (mailgroups_file, rcdir, MAILGROUPS_FILE);
	joinpath (motd_file, libdir, MOTD_FILE);
	joinpath (msg_headers_file, rcdir, MSG_HEADERS_FILE);
	joinpath (news_active_file, libdir, get_val ("TIN_ACTIVEFILE", ACTIVE_FILE));
	joinpath (newsgroups_file, libdir, NEWSGROUPS_FILE);
#ifdef WIN32
	joinpath (newsrc, rcdir, NEWSRC_FILE);
	joinpath (newnewsrc, rcdir, NEWNEWSRC_FILE);
#else
	joinpath (newsrc, homedir, NEWSRC_FILE);
	joinpath (newnewsrc, homedir, NEWNEWSRC_FILE);
#endif
	joinpath (posted_info_file, rcdir, POSTED_FILE);
	joinpath (posted_msgs_file, default_maildir, POSTED_FILE);
	joinpath (save_active_file, rcdir, ACTIVE_SAVE_FILE);
	joinpath (subscriptions_file, libdir, SUBSCRIPTIONS_FILE);

#ifdef INDEX_DAEMON
	joinpath  (lock_file, TMPDIR, LOCK_FILE);
	strcpy (newsrc, news_active_file);	/* default so all groups are indexed */
	joinpath (group_times_file, rcdir, GROUP_TIMES_FILE);
	joinpath (index_newsdir, get_val ("TIN_INDEX_NEWSDIR", novrootdir), INDEX_NEWSDIR);

	if (stat (index_newsdir, &sb) == -1) {
		my_mkdir (index_newsdir, 0755);
	}
#else
#	ifdef HAVE_LONG_FILE_NAMES
		sprintf (lock_file, "%stin.%s.LCK", TMPDIR, userid);
#	else
		sprintf (lock_file, "%s%s.LCK", TMPDIR, userid);
#endif

#ifdef VMS
	if (stat (rcdir_asfile, &sb) == -1) {
#else
	if (stat (rcdir, &sb) == -1) {
#endif
		created_rcdir = TRUE;
		my_mkdir (rcdir, 0755);
	}
	if (tin_uid != real_uid) {
		joinpath (index_newsdir, get_val ("TIN_INDEX_NEWSDIR", spooldir), INDEX_NEWSDIR);
		set_tin_uid_gid ();
		if (stat (index_newsdir, &sb) == -1) {
			my_mkdir (index_newsdir, 0777);
		}
		set_real_uid_gid ();
	} else if (stat (index_newsdir, &sb) == -1) {
		my_mkdir (index_newsdir, 0777);
	}
	if (stat (posted_info_file, &sb) == -1) {
		if ((fp = fopen (posted_info_file, "w")) != (FILE *) 0) {
			fprintf (fp, "# Summary of mailed/posted messages viewable by 'W' command from within tin.\n");
			fclose (fp);
		}
	}
	if (stat (msg_headers_file, &sb) == -1) {
		if ((fp = fopen (msg_headers_file, "w")) != (FILE *) 0) {
			fprintf (fp, "# Add extra header lines to mail/news messages.\n");
			fprintf (fp, "# ie.  Organization: Bits & Bobs Inc.\n\n");
			fclose (fp);
		}
	}
	if (stat (local_attributes_file, &sb) == -1) {
		write_attributes_file (local_attributes_file);
	}

#endif /* INDEX_DAEMON */

	if (stat (news_active_file, &sb) >= 0)
		goto got_active;

	/*
	 *  I hate forgetting to define LIBDIR correctly.  Guess a couple
	 *  of the likely places if it's not where LIBDIR says it is.
	 */
	strcpy (news_active_file, "/usr/lib/news/active");
	if (stat (news_active_file, &sb) >= 0)
		goto got_active;

	strcpy (news_active_file, "/usr/local/lib/news/active");
	if (stat (news_active_file, &sb) >= 0)
		goto got_active;

	strcpy (news_active_file, "/usr/public/lib/news/active");
	if (stat (news_active_file, &sb) >= 0)
		goto got_active;

	/*
	 *  Oh well. Revert to what LIBDIR says it is to produce a useful
	 *  error message when read_news_active_file () fails later.
	 */
	joinpath (news_active_file, libdir, ACTIVE_FILE);

got_active:

	/*
	 *  Get organization name
	 */	
	ptr = GetConfigValue (_CONF_ORGANIZATION);
	if (ptr != (char *) 0) {
		my_strncpy (default_organization, ptr, sizeof (default_organization));
	}
	
	/*
	 *  check enviroment for REPLYTO
	 */
	reply_to[0] = '\0';
	if ((ptr = getenv ("REPLYTO")) != (char *) 0) {
		my_strncpy (reply_to, ptr, sizeof (reply_to));
	}

	/*
	 *  check enviroment for DISTRIBUTION
	 */
	my_distribution[0] = '\0';
	if ((ptr = getenv ("DISTRIBUTION")) != (char *) 0) {
		my_strncpy (my_distribution, ptr, sizeof (my_distribution));
	}

	/*
	 *  check enviroment for ADD_ADDRESS
 	 */
	add_addr[0] = '\0';
	if ((ptr = getenv ("ADD_ADDRESS")) != (char *) 0) {
		my_strncpy (add_addr, ptr, sizeof (add_addr));
		goto got_add_addr;
	}

	joinpath (nam, rcdir, "add_address");
	if ((fp = fopen (nam, "r")) != (FILE *) 0) {
		if (fgets (add_addr, sizeof (add_addr), fp) != (char *) 0) {
			ptr = strrchr (add_addr, '\n');
			if (ptr != (char *) 0) {
				*ptr = '\0';
			}
		}
		fclose (fp);
	}

got_add_addr:;

	/*
	 *  check enviroment for BUG_ADDRESS
	 */
	if ((ptr = getenv ("BUG_ADDRESS")) != (char *) 0) {
		my_strncpy (bug_addr, ptr, sizeof (bug_addr));
		goto got_bug_addr;
	}

	joinpath (nam, rcdir, "bug_address");
	if ((fp = fopen (nam, "r")) != (FILE *) 0) {
		if (fgets (bug_addr, sizeof (bug_addr), fp) != (char *) 0) {
			ptr = strrchr (bug_addr, '\n');
			if (ptr != (char *) 0) {
				*ptr = '\0';
			}
		}
		fclose (fp);
	}

got_bug_addr:;
	sprintf (txt_help_bug_report, txt_help_bug, bug_addr);

#ifdef HAVE_PGP
	pgpopts = get_val("PGPOPTS", "");
	if ((ptr = getenv("PGPPATH")) != (char *) 0)
		strcpy (pgp_data, ptr);
	else
		joinpath (pgp_data, homedir, ".pgp");
#endif
}

/*
 * Create default mail & save directories if they do not exist
 */
 
int create_mail_save_dirs ()
{
	int created = FALSE;
#ifndef INDEX_DAEMON
	char path[PATH_LEN];
	struct stat sb;

	if (! strfpath (default_maildir, path, sizeof (path), 
	    homedir, (char *) 0, (char *) 0, (char *) 0)) {
		joinpath (path, homedir, DEFAULT_MAILDIR);
	}
	if (stat (path, &sb) == -1) {
		my_mkdir (path, 0755);
		created = TRUE;
	}

	if (! strfpath (default_savedir, path, sizeof (path), 
	    homedir, (char *) 0, (char *) 0, (char *) 0)) {
		joinpath (path, homedir, DEFAULT_SAVEDIR);
	}
	if (stat (path, &sb) == -1) {
		my_mkdir (path, 0755);
		created = TRUE;
	}

#endif	/* INDEX_DAEMON */
	
	return (created);
}


#ifndef USE_INN_NNTPLIB

char *
GetFQDN ()
{
	static char *fqdn = (char *) 0;

	return fqdn;
}


char *
GetConfigValue (name)
	char *name;
{
	char *ptr;
	char path[PATH_LEN];
	FILE *fp;
	static char conf_fromhost[PATH_LEN];
	static char conf_org[PATH_LEN];
	static char conf_server[PATH_LEN];
	char *conf_value = (char *) 0;

	if (STRCMPEQ(_CONF_FROMHOST, name)) {
		conf_fromhost[0] = '\0';
	} else if (STRCMPEQ(_CONF_SERVER, name)) {
		conf_server[0] = '\0';
#ifdef NNTP_DEFAULT_SERVER
		if (*(NNTP_DEFAULT_SERVER)) {
			strcpy (conf_server, NNTP_DEFAULT_SERVER);
			conf_value = conf_server;
		}
#endif	/* NNTP_DEFAULT_SERVER */		
	} else if (STRCMPEQ(_CONF_ORGANIZATION, name)) {
		conf_org[0] = '\0';
		/*
		 *  check enviroment for ORGANIZATION / NEWSORG
		 */
#ifdef apollo
		if ((ptr = getenv ("NEWSORG")) != (char *) 0) {
#else	
		if ((ptr = getenv ("ORGANIZATION")) != (char *) 0) {
#endif
			my_strncpy (conf_org, ptr, sizeof (conf_org));
			goto got_org;
		}

		/*
		 *  check LIBDIR/organization for system wide organization
		 */
		joinpath (path, libdir, "organization");
		fp = fopen (path, "r");

#ifndef M_AMIGA
		if (fp == (FILE *) 0) {
			sprintf (path, "/usr/lib/news/organization");
			fp = fopen (path, "r");
		}

		if (fp == (FILE *) 0) {
			sprintf (path, "/usr/local/lib/news/organization");
			fp = fopen (path, "r");
		}

		if (fp == (FILE *) 0) {
			sprintf (path, "/usr/public/lib/news/organization");
			fp = fopen (path, "r");
		}

		if (fp == (FILE *) 0) {
			sprintf (path, "/etc/organization");
			fp = fopen (path, "r");
		}
#endif	/* M_AMIGA */

		if (fp != (FILE *) 0) {
			if (fgets (conf_org, sizeof (conf_org), fp) != (char *) 0) {
				ptr = strrchr (conf_org, '\n');
				if (ptr != (char *) 0) {
					*ptr = '\0';
				}
			}
			fclose (fp);
		}
got_org:	/* goto */				
		conf_value = conf_org;
	}
	
	return conf_value;
}

#endif	/* USE_INN_NNTPLIB */
