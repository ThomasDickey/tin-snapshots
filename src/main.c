/*
 *  Project   : tin - a Usenet reader
 *  Module    : main.c
 *  Author    : I. Lea & R. Skrenta
 *  Created   : 1991-04-01
 *  Updated   : 1997-12-28
 *  Notes     :
 *  Copyright : (c) Copyright 1991-99 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"
#include	"version.h"
#include	"bugrep.h"

#if defined(M_AMIGA) && defined(__SASC_650)
	extern int _WBArg;
	extern char	**_WBArgv;
	char __stdiowin[] = "con:0/12/640/200/TIN " VERSION;
	char __stdiov37[] = "/AUTO/NOCLOSE";
#else
#	ifdef VMS
	int debug;
#	endif /* VMS */
#endif /* M_AMIGA && __SASC_650 */

signed long int read_newsrc_lines = -1;

static char **cmdargs;
static int num_cmdargs;
static int max_cmdargs;

/*
 * Local prototypes
 */
static int check_for_any_new_news (t_bool CheckAnyUnread, t_bool StartAnyUnread);
static void save_or_mail_new_news (void);
static void read_cmd_line_options (int argc, char *argv[]);
static void update_index_files (void);
static void usage (char *theProgname);
#ifndef INDEX_DAEMON
	static void show_intro_page (void);
#endif /* !INDEX_DAEMON */


/*
 * OK lets start the ball rolling...
 */
int
main (
	int argc,
	char *argv[])
{
	int num_cmd_line_groups;
	int start_groupnum = 0;
	int count;
	t_bool tmp_no_write;

	set_signal_handlers ();

	cmd_line = TRUE;
	debug = 0;	/* debug OFF */

#if defined(M_AMIGA) && defined(__SASC)
	/* Call tzset() here!*/
	_TZ = "GMT0";
	tzset();
	if (argc == 0) /* we are running from the Workbench */
	{
		argc = _WBArgc;
		argv = _WBArgv;
	}
#endif /* M_AMIGA && __SASC */

	base_name (argv[0], tin_progname);
#ifdef VMS
	argv[0] = tin_progname;
#endif /* VMS */

#ifdef NNTP_ONLY
	read_news_via_nntp = TRUE;
#else
	/*
	 * If called as rtin, read news remotely via NNTP
	 */
	if (tin_progname[0] == 'r') {
#		ifdef NNTP_ABLE
			read_news_via_nntp = TRUE;
#		else
			error_message (txt_option_not_enabled, "-DNNTP_ABLE");
			giveup();
#		endif /* NNTP_ABLE */
	}
#endif /* NNTP_ONLY */

	/*
	 * Set up initial array sizes, char *'s: homedir, newsrc, etc.
	 */
	init_alloc ();
	hash_init ();
	init_selfinfo ();
	init_group_hash ();

	/*
	 * Read user local & global config files
	 * These override the compiled in defaults
	 */
	read_config_file (global_config_file, TRUE);
	read_config_file (local_config_file, FALSE);

	/*
	 * Process envargs & command line options
	 * These override the configured in values
	 */
	read_cmd_line_options (argc, argv);

	tmp_no_write = no_write; /* keep no_write */
	no_write = TRUE;		 /* don't allow any writing back during startup */

#if defined(M_UNIX) && !defined(INDEX_DAEMON)
#	ifndef USE_CURSES
	if (INTERACTIVE) {
		if (!get_termcaps ()) {
			error_message (txt_screen_init_failed, tin_progname);
			giveup();
		}
/*		EndInverse ();*/
	}
#	endif /* !USE_CURSES */
#endif /* M_UNIX && !INDEX_DAEMON */

	/*
	 * Init curses emulation
	 */
	if (!InitScreen ()) {
		error_message (txt_screen_init_failed, tin_progname);
		giveup();
	}

	EndInverse ();

	/*
	 * This depends on various things in tinrc
	 */
	setup_screen ();

	if (INTERACTIVE || (batch_mode && verbose))
		wait_message (0, "%s\n", cvers);

#ifndef INDEX_DAEMON
	set_up_private_index_cache ();
#endif /* !INDEX_DAEMON */

	/*
	 * Connect to nntp server?
	 */
	if (read_news_via_nntp && !read_saved_news)
		if (nntp_open () != 0)
			giveup();

	/*
	 * Check if overview indexes contain Xref: lines
	 */
	if (xover_supported)
		xref_supported = overview_xref_support ();

#ifdef DEBUG_NEWSRC
	unlink ("/tmp/BITMAP");
/*	vNewsrcTestHarness ();*/
#endif /* DEBUG_NEWSRC */

	/*
	 * Read input history
	 */
#ifndef INDEX_DAEMON
	if (!batch_mode)
		read_input_history_file ();
#endif /* !INDEX_DAEMON */

	/*
	 * Load the mail & news active files into active[]
	 *
	 * create_save_active_file cannot write to active.save
	 * if no_write == TRUE, so restore original value temporarily
	 */
	no_write = tmp_no_write;
	if (read_saved_news)
		create_save_active_file ();
	no_write = TRUE;

#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
	read_mail_active_file ();
#endif /* !INDEX_DAEMON && HAVE_MH_MAIL_HANDLING */

	/*
	 * Initialise active[] and add new newsgroups to start of my_group[]
	 */
	group_top = 0;
	read_news_active_file ();
#ifdef DEBUG
	debug_print_active();
#endif /* DEBUG */

	/*
	 * Load the local & global group specific attribute files
	 */
#ifndef INDEX_DAEMON
	read_attributes_file (global_attributes_file, TRUE);
	read_attributes_file (local_attributes_file, FALSE);
#endif /* !INDEX_DAEMON */

	/*
	 * Read in users filter preferences file.
	 * This has to be done before quick post
	 * because the filters will be updated.
	 */
#ifndef INDEX_DAEMON
	global_filtered_articles = read_filter_file (global_filter_file, TRUE);
	local_filtered_articles = read_filter_file (local_filter_file, FALSE);
#else
	global_filtered_articles = TRUE;
	local_filtered_articles = TRUE;
#endif /* !INDEX_DAEMON */

#ifdef DEBUG
	debug_print_filters ();
#endif /* DEBUG */

	/*
	 * Quick post an article & exit if -w specified
	 */
#ifndef INDEX_DAEMON
	if (post_article_and_exit || post_postponed_and_exit) {
		quick_post_article (post_postponed_and_exit);
		wait_message (2, txt_exiting);
		tin_done (EXIT_SUCCESS);
	}
#endif /* !INDEX_DAEMON */

	if ((count = count_postponed_articles()))
		wait_message(3, txt_info_postponed, count, IS_PLURAL(count));

	/*
	 * Read text descriptions for mail and/or news groups
	 */
#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
	read_mailgroups_file ();
#endif /* !INDEX_DAEMON && HAVE_MH_MAIL_HANDLING */
	read_newsgroups_file ();

#ifndef INDEX_DAEMON
	if (create_mail_save_dirs ())
		write_config_file (local_config_file);
#endif /* !INDEX_DAEMON */

	/*
	 * Preloads active[] with command line groups. They will follow any
	 * new newsgroups
	 */
	num_cmd_line_groups = read_cmd_line_groups ();

#ifdef INDEX_DAEMON
	vMakeActiveMyGroup ();
#else
	backup_newsrc ();

	/*
	 * Load my_groups[] from the .newsrc file. We append these groups to any
	 * new newsgroups and command line newsgroups already loaded
	 */
	read_newsrc_lines = read_newsrc (newsrc, FALSE);

	no_write = tmp_no_write; /* restore old value */

	/*
	 * We have to show all groups with command line groups
	 */
	if (num_cmd_line_groups)
		tinrc.show_only_unread_groups = FALSE;
	else
		toggle_my_groups (tinrc.show_only_unread_groups, "");
#endif /* INDEX_DAEMON */

	/*
	 * Check/start if any new/unread articles
	 */
	start_groupnum = check_for_any_new_news (check_any_unread, start_any_unread);

	/*
	 * Mail any new articles to specified user
	 * or
	 * Save any new articles to savedir structure for later reading
	 */
	save_or_mail_new_news ();

	/*
	 * Catchup newsrc file (-c option)
	 */
	catchup_newsrc_file ();

	/*
	 * Update index files
	 */
	update_index_files ();

	/*
	 * If first time print welcome screen and auto-subscribe
	 * to groups specified in /usr/lib/news/subscribe locally
	 * or via NNTP if reading news remotely (LIST SUBSCRIBE)
	 */
#ifndef INDEX_DAEMON
	if (created_rcdir && !batch_mode)
		show_intro_page ();

#endif /* !INDEX_DAEMON */
	/*
	 * Work loop
	 */
	selection_page (start_groupnum, num_cmd_line_groups);
	return 0; /* not reached */
}


/*
 * process command line options
 */
#ifndef INDEX_DAEMON
#	ifndef M_AMIGA
#		define OPTIONS "aAcdD:f:G:g:hHI:lm:M:nNop:qQrRs:SuUvVwXzZ"
#	else
#		define OPTIONS "BcdD:f:G:hHI:lm:M:nNop:qQrRs:SuUvVwXzZ"
#	endif /* M_AMIGA */
#else
#	define OPTIONS "dD:f:G:hI:PvV"
#endif /* !INDEX_DAEMON */

static void
read_cmd_line_options (
	int argc,
	char *argv[])
{
	int ch;
	t_bool newsrc_set = FALSE;

	envargs (&argc, &argv, "TINRC");

	while ((ch = getopt (argc, argv, OPTIONS)) != -1) {
		switch (ch) {
#ifndef INDEX_DAEMON
#	ifndef M_AMIGA
			case 'a':
#		ifdef HAVE_COLOR
				use_color = !use_color;
#		else
				error_message (txt_option_not_enabled, "-DHAVE_COLOR");
				giveup();
				/* keep lint quiet: */
				/* NOTREACHED */
#		endif /* HAVE_COLOR */
				break;

			case 'A':
#		ifdef NNTP_ABLE
				force_auth_on_conn_open = TRUE;
#		else
				error_message (txt_option_not_enabled, "-DNNTP_ABLE");
				giveup();
				/* keep lint quiet: */
				/* NOTREACHED */
#		endif /* NNTP_ABLE */
				break;
#	else
			case 'B':
				tin_bbs_mode = TRUE;
				break;
#	endif /* !M_AMIGA */
			case 'c':
				catchup = TRUE;
				batch_mode = TRUE;
				break;

			case 'd':
				show_description = FALSE;
				break;
#else
			case 'd':		/* delete index file before indexing */
				delete_index_file = TRUE;
				break;
#endif /* !INDEX_DAEMON */

			case 'D':		/* debug mode 1=NNTP 2=ALL */
#ifdef DEBUG
				debug = atoi (optarg);
				debug_delete_files ();
#else
				error_message (txt_option_not_enabled, "-DDEBUG");
				giveup();
				/* keep lint quiet: */
				/* NOTREACHED */
#endif /* DEBUG */
				break;

			case 'f':	/* active (tind) / newsrc (tin) file */
#ifdef INDEX_DAEMON
				my_strncpy (news_active_file, optarg, sizeof (news_active_file));
#else
				my_strncpy (newsrc, optarg, sizeof (newsrc));
				newsrc_set = TRUE;
#endif /* INDEX_DAEMON */
				break;

			case 'G':
				tinrc.getart_limit = atoi(optarg);
				if (tinrc.getart_limit != 0)
					tinrc.use_getart_limit = TRUE;
				else
					tinrc.use_getart_limit = FALSE;
				break;

#ifndef INDEX_DAEMON
#	ifndef M_AMIGA
			case 'g':	/* select alternative NNTP-server, implies -r */
#		ifdef NNTP_ABLE
				my_strncpy(cmdline_nntpserver, optarg, sizeof(cmdline_nntpserver));
				read_news_via_nntp = TRUE;
#		else
				error_message (txt_option_not_enabled, "-DNNTP_ABLE");
				giveup();
				/* keep lint quiet: */
				/* NOTREACHED */
#		endif /* NNTP_ABLE */
				break;
#	endif /* !M_AMIGA */

			case 'H':
				show_intro_page ();
				exit (EXIT_SUCCESS);
				/* keep lint quiet: */
				/* FALLTHROUGH */
#endif /* !INDEX_DAEMON */

			case 'I':
#ifndef NNTP_ONLY
				my_strncpy (index_newsdir, optarg, sizeof (index_newsdir));
				my_mkdir (index_newsdir, (mode_t)S_IRWXUGO);
#else
				error_message (txt_option_not_enabled, "-DNNTP_ABLE");
				giveup();
				/* keep lint quiet: */
				/* NOTREACHED */
#endif /* !NNTP_ONLY */
				break;

#ifndef INDEX_DAEMON
			case 'l':
				list_active = TRUE;
				break;

			case 'm':
				my_strncpy (tinrc.maildir, optarg, sizeof (tinrc.maildir));
				break;

			case 'M':	/* mail new news to specified user */
				my_strncpy (mail_news_user, optarg, sizeof (mail_news_user));
				mail_news = TRUE;
				batch_mode = TRUE;
				break;

			case 'n':
				newsrc_active = TRUE;
				break;

			case 'N':	/* mail new news to your posts */
				my_strncpy (mail_news_user, userid, sizeof(userid));
				mail_news = TRUE;
				batch_mode = TRUE;
				break;

			case 'o':	/* post postponed articles & exit */
				post_postponed_and_exit = TRUE;
				break;
#else
			case 'P':	/* stat every art for a through purge */
				purge_index_files = TRUE;
				break;
#endif /* !INDEX_DAEMON */

#ifdef NNTP_ABLE
			case 'p': /* implies -r */
				read_news_via_nntp = TRUE;
				if (atoi(optarg) != 0)
					nntp_tcp_port = (unsigned short) atoi(optarg);
				break;
#endif /* NNTP_ABLE */

#ifndef INDEX_DAEMON
			case 'q':
				check_for_new_newsgroups = FALSE;
				break;

			case 'Q':
				newsrc_active = TRUE;
				check_for_new_newsgroups = FALSE;
				show_description = FALSE;
				break;

			case 'r':	/* read news remotely from default NNTP server */
#	ifdef NNTP_ABLE
				read_news_via_nntp = TRUE;
#	else
				error_message (txt_option_not_enabled, "-DNNTP_ABLE");
				giveup();
				/* keep lint quiet: */
				/* NOTREACHED */
#	endif /* NNTP_ABLE */
				break;

			case 'R':	/* read news saved by -S option */
				read_saved_news = TRUE;
				list_active = TRUE;
				newsrc_active = FALSE;
				check_for_new_newsgroups = FALSE;
				my_strncpy (news_active_file, save_active_file, sizeof (news_active_file));
				break;

			case 's':
				my_strncpy (tinrc.savedir, optarg, sizeof (tinrc.savedir));
				break;

			case 'S':	/* save new news to dir structure */
				save_news = TRUE;
				batch_mode = TRUE;
				break;

			case 'u':	/* update index files */
#	ifndef NNTP_ONLY
				batch_mode = TRUE;
				show_description = FALSE;
#	else
				error_message (txt_option_not_enabled, "-DNNTP_ABLE");
				giveup();
				/* keep lint quiet: */
				/* NOTREACHED */
#	endif /* !NNTP_ONLY */
				break;

			case 'U':	/* update index files in background */
#	ifndef NNTP_ONLY
				update_fork = TRUE;
				batch_mode = TRUE;
#	else
				error_message (txt_option_not_enabled, "-DNNTP_ABLE");
				giveup();
				/* keep lint quiet: */
				/* NOTREACHED */
#	endif /* !NNTP_ONLY */
				break;

#endif /* !INDEX_DAEMON */

			case 'v':	/* verbose mode */
				verbose = TRUE;
				break;

			case 'V':
#if defined(__DATE__) && defined(__TIME__)
				error_message ("Version: %s release %s (\"%s\") %s %s",
					VERSION, RELEASEDATE, RELEASENAME, __DATE__, __TIME__);
#else
				error_message ("Version: %s release %s (\"%s\")",
					VERSION, RELEASEDATE, RELEASENAME);
#endif /* __DATE__  && __TIME__ */
				exit (EXIT_SUCCESS);
				/* keep lint quiet: */
				/* FALLTHROUGH */
#ifndef INDEX_DAEMON
			case 'w':	/* post article & exit */
				post_article_and_exit = TRUE;
				no_write = TRUE;
				newsrc_active = TRUE;
				check_for_new_newsgroups = FALSE;
				break;

			case 'X':	/* don't save ~/.newsrc on exit */
				no_write = TRUE;
				break;

			case 'z':
				start_any_unread = TRUE;
				break;

			case 'Z':
				check_any_unread = TRUE;
				batch_mode = TRUE;
				break;
#endif /* !INDEX_DAEMON */

			case 'h':
			case '?':
			default:
				usage (tin_progname);
				exit (EXIT_SUCCESS);
		}
	}
	cmdargs = argv;
	num_cmdargs = optind;
	max_cmdargs = argc;
	if (!newsrc_set) {
		if (read_news_via_nntp)
			get_newsrcname(newsrc, getserverbyfile(NNTP_SERVER_FILE));
		else {
#if defined(HAVE_SYS_UTSNAME_H) && defined(HAVE_UNAME)
			struct utsname uts;
			(void) uname(&uts);
			get_newsrcname(newsrc, uts.nodename);
#else
			char nodenamebuf[32];
#	if defined(M_AMIGA)
			my_strncpy (nodenamebuf, get_val ("NodeName", "PROBLEM_WITH_NODE_NAME"), sizeof (nodenamebuf));
#	else /* NeXT, Apollo */
			(void) gethostname(nodenamebuf, sizeof(nodenamebuf));
#	endif /* M_AMIGA */
			get_newsrcname(newsrc, nodenamebuf);
#endif /* HAVE_SYS_UTSNAME_H && HAVE_UNAME */
		}
	}

	if (verbose && !batch_mode) {
		wait_message(1, "-v only useful for batch mode operations\n");
		verbose = FALSE;
	}

	if (read_saved_news && batch_mode) {
		wait_message(1, "-R only useful without batch mode operations\n");
		read_saved_news = FALSE;
	}

	/*
	 * Sort out conflicts of options....
	 */
#ifdef NNTP_ABLE
	/*
	 * If we're reading from an NNTP server and we've been asked not to look
	 * for new newsgroups, trust our cached copy of the newsgroups file.
	 */
	if (read_news_via_nntp)
		read_local_newsgroups_file = !check_for_new_newsgroups;
#endif /* NNTP_ABLE */
	/*
	 * If we use neither list_active nor newsrc_active,
	 * we use both of them.
	 */
	if (!list_active && !newsrc_active) {
		list_active = TRUE;
		newsrc_active = TRUE;
	}
}


/*
 * usage
 *
 * FIXME: move strings to lang.c
 */
static void
usage (
	char *theProgname)
{
#ifndef INDEX_DAEMON
	error_message ("A Usenet reader.\n\nUsage: %s [options] [newsgroup[,...]]", theProgname);

#	ifndef M_AMIGA
#		ifdef HAVE_COLOR
			error_message ("  -a       toggle color flag");
#		endif /* HAVE_COLOR */
#		ifdef NNTP_ABLE
			error_message ("  -A       force authentication on connect");
#		endif /* NNTP_ABLE */
#	else
		error_message ("  -B       BBS mode. File operations limited to home directories.");
#	endif /* !M_AMIGA */

	error_message ("  -c       mark all news as read in subscribed newsgroups (batch mode)");
	error_message ("  -d       don't show newsgroup descriptions");

#	ifdef DEBUG
		error_message ("  -D       debug mode 1=NNTP 2=ALL");
#	endif /* DEBUG */

	error_message ("  -f file  subscribed to newsgroups file [default=%s]", newsrc);
	error_message ("  -G limit get only limit articles/group");

#	ifndef M_AMIGA
#		ifdef NNTP_ABLE
			/* FIXME, default should be $NNTPSERVER if set ... */
			error_message ("  -g serv  read news from NNTP server serv [default=%s]", NNTP_DEFAULT_SERVER);
#		endif /* NNTP_ABLE */
#	endif /* !M_AMIGA */

	error_message ("  -h       this help message");
	error_message ("  -H       help information about %s", theProgname);

#	ifndef NNTP_ONLY
		error_message ("  -I dir   news index file directory [default=%s]", index_newsdir);
#	endif /* !NNTP_ONLY */

#	ifdef NNTP_ABLE
		error_message ("  -l       use only LISTGROUP instead of GROUP (-n) command");
#	endif /* NNTP_ABLE */

	error_message ("  -m dir   mailbox directory [default=%s]", tinrc.maildir);
	error_message ("  -M user  mail new news to specified user (batch mode)");

#	ifdef NNTP_ABLE
		error_message ("  -n       only read subscribed .newsrc groups from NNTP server");
#	endif /* NNTP_ABLE */

	error_message ("  -N       mail new news to your posts");
	error_message ("  -o       post all postponed articles and exit");

#	ifdef NNTP_ABLE
		error_message ("  -p port  use port as NNTP port [default=%d]", nntp_tcp_port);
#	endif /* NNTP_ABLE */

	error_message ("  -q       don't check for new newsgroups");

#	ifdef NNTP_ABLE
	error_message ("  -Q       quick start. Same as -nqd");
#	else
	error_message ("  -Q       quick start. Same as -qd");
#	endif /* NNTP_ABLE */

#	ifdef NNTP_ABLE
		if (!read_news_via_nntp)
			error_message ("  -r       read news remotely from default NNTP server");
#	endif /* NNTP_ABLE */

	error_message ("  -R       read news saved by -S option");
	error_message ("  -s dir   save news directory [default=%s]", tinrc.savedir);
	error_message ("  -S       save new news for later reading (batch mode)");

#	ifndef NNTP_ONLY
		error_message ("  -u       update index files (batch mode)");
		error_message ("  -U       update index files in the background while reading news");
#	endif /* !NNTP_ONLY */

	error_message ("  -v       verbose output for batch mode options");
	error_message ("  -V       print version & date information");
	error_message ("  -w       post an article and exit");
	error_message ("  -X       don't save any files on quit");
	error_message ("  -z       start if any unread news");
	error_message ("  -Z       return status indicating if any unread news (batch mode)");

#else /* INDEX_DAEMON */

	error_message ("Index file daemon.\n\nUsage: %s [options] [newsgroups]", theProgname);

	error_message ("  -d       delete index file before indexing articles");

#	ifdef DEBUG
	error_message ("  -D       debug mode 1=NNTP 2=ALL");
#	endif /* DEBUG */

	error_message ("  -f file  active newsgroups file [default=%s]", newsrc);
	error_message ("  -h       this help message");
	error_message ("  -I dir   news index file directory [default=%s]", index_newsdir);
	error_message ("  -P       purge any expired articles from index files");
	error_message ("  -v       verbose output for batch mode options");
	error_message ("  -V       print version & date information");
#endif /* INDEX_DAEMON */

	error_message ("\nMail bug reports/comments to %s", BUG_REPORT_ADDRESS);
}


/*
 * check/start if any new/unread articles
 */
static int
check_for_any_new_news (
	t_bool CheckAnyUnread,
	t_bool StartAnyUnread)
{
	int i = 0;

	if (CheckAnyUnread) {
		i = check_start_save_any_news (CHECK_ANY_NEWS);
		exit (i);
	}

	if (StartAnyUnread) {
		batch_mode = TRUE;			/* Suppress some unwanted on-screen garbage */
		if ((i = check_start_save_any_news (START_ANY_NEWS)) == -1)
			exit (EXIT_SUCCESS);			/* No new/unread news so exit */
		batch_mode = FALSE;
	}

	return (i);
}


/*
 * mail any new articles to specified user
 * or
 * save any new articles to savedir structure for later reading
 */
static void
save_or_mail_new_news (
	void)
{
	if (mail_news || save_news) {
		t_bool i = catchup;
		catchup = FALSE;	/* set catchup to FALSE */
		do_update ();
		catchup = i;		/* set catchup to previous value */
		check_start_save_any_news (mail_news ? MAIL_ANY_NEWS : SAVE_ANY_NEWS);
		tin_done (EXIT_SUCCESS);
	}
}


/*
 * update index files
 */
static void
update_index_files (
	void)
{
	if (batch_mode || update_fork) {
		if (!catchup && (read_news_via_nntp && xover_supported)) {
			error_message (txt_batch_update_unavail, tin_progname);
			tin_done (EXIT_FAILURE);
		}

		cCOLS = 132;				/* set because curses has not started */
#ifdef HAVE_FORK
		if (update_fork) {
			catchup = FALSE;		/* turn off msgs when running forked */
			verbose = FALSE;
			switch ((int) fork ()) {		/* fork child to update indexes in background */
				case -1:			/* error forking */
					perror_message (txt_batch_update_failed);
					break;
				case 0:				/* child process */
					create_index_lock_file (lock_file);
					process_id = getpid ();
#	if defined(BSD) /* FIXME: check for setsid/setpgid/... and remove OS depending ifdefs */
#		if defined(__FreeBSD__) || defined(__NetBSD__)
					setsid();
#		else
#			ifdef __osf__
					setpgid (0, 0);
#			else
					setpgrp (0, process_id);	/* reset process group leader to this process */
#			endif /* __osf__ */
#			ifdef TIOCNOTTY
					{
						int fd;

						if ((fd = open ("/dev/tty", O_RDWR)) >= 0) {
							ioctl (fd, TIOCNOTTY, (char *) NULL);
							close (fd);
						}
					}
#			endif /* TIOCNOTTY */
#		endif /* __FreeBSD__ || __NetBSD__ */
#	else
#		ifdef HAVE_SETPGRP
#				ifdef SETPGRP_VOID
						setpgrp ();
#				else
						setpgrp (0, process_id);
#				endif /* SETPGRP_VOID */
					signal (SIGHUP, SIG_IGN);	/* make immune from process group leader death */
#		endif /* HAVE_SETPGRP */
#	endif /* BSD */
					signal (SIGQUIT, SIG_IGN);	/* stop indexing being interrupted */

					if (nntp_open () != 0)				/* connect server if we are using nntp */
						tin_done (EXIT_SUCCESS);

					tinrc.thread_articles = THREAD_NONE;	/* stop threading to run faster */
					do_update ();
					tin_done (EXIT_SUCCESS);
					break;
				default:						/* parent process*/
					break;
			}
			batch_mode = FALSE;
		} else
#endif /* HAVE_FORK */
		{
			create_index_lock_file (lock_file);
			tinrc.thread_articles = THREAD_NONE;	/* stop threading to run faster */
			do_update ();
			tin_done (EXIT_SUCCESS);
		}
	}
}


/*
 * display page of general info. for first time user.
 */
#ifndef INDEX_DAEMON
static void
show_intro_page (
	void)
{
	char buf[4096]; /* should be enoght */

	if (!cmd_line) {
		ClearScreen ();
		center_line (0, TRUE, cvers);
		Raw (FALSE);
		my_printf("\n");
	}

	sprintf(buf, txt_intro_page, BUG_REPORT_ADDRESS);

	my_fputs (buf, stdout);
	my_flush();

	if (!cmd_line) {
		Raw (TRUE);
		continue_prompt ();
	}
}
#endif /* !INDEX_DAEMON */


/*
 * Wildcard match any newsgroups on the command line. Sort of like a limited
 * yank at startup. Return number of groups that were matched.
 */
int
read_cmd_line_groups (
	void)
{
	int matched = 0;
	int num;
	register int i;

	if (num_cmdargs < max_cmdargs) {
		group_top = skip_newgroups();		/* Reposition after any newgroups */

		for (num = num_cmdargs; num < max_cmdargs; num++) {
			wait_message (0, txt_matching_cmd_line_groups, cmdargs[num]);

			for (i = 0; i < num_active; i++) {
				if (match_group_list (active[i].name, cmdargs[num])) {
					if (my_group_add (active[i].name) != -1)
						matched++;
				}
			}
		}
	}

	return matched;
}

void
giveup(void)
{
	static int nested;

	if (!cmd_line && !nested++) {
		cursoron();
		EndWin ();
		Raw (FALSE);
	}
	exit (EXIT_FAILURE);
}
