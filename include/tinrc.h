/*
 * These are the tin defaults read from the tinrc file
 * If you change this, ensure you change the initial values in init.c
 *
 * FIXME: most default_* could/should be stored in the .inputhistory
 *        and could be nuked if tin comes with a prefilled .inputhistory
 *        which is installed automatically if no .inputhistory is found.
 */
struct t_config {
	/*
	 * Chars used to show article status
	 */
	char art_marked_deleted;
	char art_marked_inrange;
	char art_marked_return;
	char art_marked_selected;
	char art_marked_unread;
	char default_editor_format[PATH_LEN];	/* editor + parameters  %E +%N %F */
	char default_goto_group[LEN];		/* default for the 'g' command */
	char default_mail_address[LEN];
	char default_mailer_format[PATH_LEN];	/* mailer + parameters  %M %S %T %F */
#ifndef DONT_HAVE_PIPING
	char default_pipe_command[LEN];
#endif /* DONT_HAVE_PIPING */
	char default_post_newsgroups[PATH_LEN];	/* default newsgroups to post to */
	char default_post_subject[PATH_LEN];	/* default subject when posting */
#ifndef DISABLE_PRINTING
	char default_printer[LEN];			/* printer program specified from tinrc */
#endif /* !DISABLE_PRINTING */
	char default_range_group[PATH_LEN];
	char default_range_select[PATH_LEN];
	char default_range_thread[PATH_LEN];
	char default_regex_pattern[LEN];
	char default_repost_group[LEN];		/* default group to repost to */
	char default_save_file[PATH_LEN];
	char default_search_art[LEN];		/* default when searching in article */
	char default_search_author[LEN];	/* default when searching for author */
	char default_search_config[LEN];	/* default when searching config menu */
	char default_search_group[LEN];		/* default when searching select screen */
	char default_search_subject[LEN];	/* default when searching by subject */
	char default_select_pattern[LEN];
	char default_shell_command[LEN];
	char default_sigfile[PATH_LEN];
	char mail_quote_format[PATH_LEN];
	char maildir[PATH_LEN];				/* mailbox dir where = saves are stored */
	char mail_address[LEN];				/* user's mail address */
	char mm_charset[LEN];				/* MIME charset: moved from rfc1522.c */
	char news_headers_to_display[LEN];	/* which headers to display */
	char news_headers_to_not_display[LEN];	/* which headers to not display */
	char news_quote_format[PATH_LEN];
	char post_process_command[PATH_LEN];/* Post processing command */
	char quote_chars[PATH_LEN];			/* quote chars for posting/mails ": " */
	char savedir[PATH_LEN];				/* directory to save articles to */
	char spamtrap_warning_addresses[LEN];
	char xpost_quote_format[PATH_LEN];
	int default_filter_days;			/* num of days an article filter can be active */
	int default_filter_kill_header;
	int default_filter_select_header;
	int default_move_group;
	int default_save_mode;				/* Append/Overwrite existing file when saving */
	int getart_limit;					/* number of article to get */
	int groupname_max_length;			/* max len of group names to display on screen */
	int mail_mime_encoding;
	int post_mime_encoding;
	int post_process_type;				/* type of post processing to be performed */
	int reread_active_file_secs;		/* reread active file interval in seconds */
	int show_author;					/* show_author value from 'M' menu in tinrc */
	int sort_article_type;				/* method used to sort arts[] */
	int strip_bogus;
	int thread_articles;				/* threading system for viewing articles */
	int wildcard;						/* 0=wildmat, 1=regex */
#ifdef HAVE_COLOR
	int col_back;						/* standard background color */
	int col_from;						/* color of sender (From:) */
	int col_head;						/* color of headerlines */
	int col_help;						/* color of help pages */
	int col_invers_bg;					/* color of inverse text (background) */
	int col_invers_fg;					/* color of inverse text (foreground) */
	int col_minihelp;					/* color of mini help menu*/
	int col_normal;						/* standard foreground color */
	int col_markdash;					/* text highlighting with _underdashes_ */
	int col_markstar;					/* text highlighting with *stars* */
	int col_message;					/* color of message lines at bottom */
	int col_newsheaders;				/* color of actual news header fields */
	int col_quote;						/* color of quotelines */
	int col_response;					/* color of respone counter */
	int col_signature;					/* color of signature */
	int col_subject;					/* color of article subject */
	int col_text;						/* color of textlines*/
	int col_title;						/* color of Help/Mail-Sign */
	int word_h_display_marks;			/* display * or _ when highlighting or space or nothing*/
	t_bool word_highlight;				/* like word_highlight but stored in tinrc */
	t_bool use_color;					/* like use_color but stored in tinrc */
#endif /* HAVE_COLOR */
	t_bool add_posted_to_filter;
	t_bool advertising;
	t_bool alternative_handling;
	t_bool auto_bcc;					/* add your name to bcc automatically */
	t_bool auto_cc;						/* add your name to cc automatically */
	t_bool auto_list_thread;			/* list thread when entering it using right arrow */
	t_bool auto_reconnect;				/* automatically reconnect to news server */
	t_bool auto_save;					/* save thread with name from Archive-name: field */
	t_bool batch_save;					/* save arts if -M/-S command line switch specified */
	t_bool beginner_level;				/* beginner level (shows mini help a la elm) */
	t_bool cache_overview_files;		/* create local index files for NNTP overview files */
	t_bool catchup_read_groups;			/* ask if read groups are to be marked read */
	t_bool confirm_action;
	t_bool confirm_to_quit;
	t_bool display_mime_allheader_asis;	/* rfc 1522/2047 all heades (^H) will not be decoded by default */
	t_bool display_mime_header_asis;	/* rfc 1522/2047 news_headers_to_display will be decoded by default */
	t_bool draw_arrow_mark;				/* draw -> or highlighted bar */
	t_bool force_screen_redraw;			/* force screen redraw after external (shell) commands */
	t_bool full_page_scroll;			/* page half/full screen of articles/groups */
	t_bool group_catchup_on_exit;		/* catchup group with left arrow key or not */
	t_bool info_in_last_line;
	t_bool inverse_okay;
	t_bool keep_dead_articles;			/* keep all dead articles in dead.articles */
	t_bool keep_posted_articles;		/* keep all posted articles in ~/Mail/posted */
	t_bool mail_8bit_header;			/* allow 8bit chars. in header of mail message */
	t_bool mark_saved_read;				/* mark saved article/thread as read */
	t_bool pgdn_goto_next;
	t_bool pos_first_unread;			/* position cursor at first/last unread article */
	t_bool post_8bit_header;			/* allow 8bit chars. in header when posting to newsgroup */
#ifndef DISABLE_PRINTING
	t_bool print_header;				/* print all of mail header or just Subject: & From lines */
#endif /* !DISABLE_PRINTING */
	t_bool process_only_unread;			/* save/print//mail/pipe unread/all articles */
	t_bool prompt_followupto;			/* display empty Followup-To header in editor */
	t_bool quote_empty_lines;			/* quote empty lines, too */
	t_bool quote_signatures;			/* quote signatures */
	t_bool save_to_mmdf_mailbox;		/* save mail to MMDF/mbox format mailbox */
	t_bool show_description;
	t_bool show_last_line_prev_page;	/* set TRUE to see last line of prev page (ala nn) */
	t_bool show_lines;
	t_bool show_only_unread_arts;		/* show only new/unread arts or all arts */
	t_bool show_only_unread_groups;		/* set TRUE to see only subscribed groups with new news */
	t_bool show_score;
	t_bool show_signatures;				/* show signatures when displaying articles */
	t_bool show_xcommentto;				/* set TRUE to show X-Comment-To in upper right corner */
	t_bool sigdashes;					/* set TRUE to prepend every signature with dashes */
	t_bool signature_repost;			/* set TRUE to add signature when reposting articles */
	t_bool space_goto_next_unread;
	t_bool start_editor_offset;
	t_bool strip_blanks;
	t_bool strip_newsrc;
	t_bool tab_after_X_selection;		/* set TRUE if you want auto TAB after X */
	t_bool tab_goto_next_unread;
	t_bool thread_catchup_on_exit;		/* catchup thread with left arrow key or not */
	t_bool unlink_article;
	t_bool use_builtin_inews;
	t_bool use_getart_limit;
	t_bool use_mailreader_i;			/* invoke user's mailreader earlier to use more of its features (i = interactive) */
	t_bool use_mouse;					/* enables/disables mouse support under xterm */
#ifdef HAVE_KEYPAD
	t_bool use_keypad;
#endif /* HAVE_KEYPAD */
#ifdef HAVE_METAMAIL
	t_bool ask_for_metamail;			/* enables/disables the metamail query if a MIME message is going to be displayed */
	t_bool use_metamail;				/* enables/disables metamail on MIME messages */
#endif /* HAVE_METAMAIL */
	t_bool default_filter_kill_case;
	t_bool default_filter_kill_expire;
	t_bool default_filter_kill_global;
	t_bool default_filter_select_case;
	t_bool default_filter_select_expire;
	t_bool default_filter_select_global;
};
