/*
 *  Project   : tin - a Usenet reader
 *  Module    : main.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 22-12-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"version.h"

#if defined(M_AMIGA) && defined(__SASC_650)
extern int	_WBArg;
extern char	**_WBArgv;
char		__stdiowin[] = "con:0/12/640/200/TIN " VERSION;
char		__stdiov37[] = "/AUTO/NOCLOSE";
#endif

static char **cmdargs;
static int num_cmdargs;
static int max_cmdargs;

/*
 *  OK lets start the ball rolling...
 */

int
main (argc, argv)
	int argc;
	char *argv[];
{
	int num_cmd_line_groups = 0;
	int start_groupnum = 0;

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
#endif

	set_signal_handlers ();

	base_name (argv[0], progname);
#ifdef VMS
    argv[0] = progname;
#endif

#ifdef NNTP_ONLY
	read_news_via_nntp = TRUE;
#else
	/*
	 *  rtin/cdtin so read news remotely via NNTP
	 */
	if (progname[0] == 'r' || (progname[0] == 'c' && progname[1] == 'd')) {
#		ifdef NNTP_ABLE
			read_news_via_nntp = TRUE;
#		else
			error_message (txt_option_not_enabled, "-DNNTP_ABLE");
			exit (EXIT_ERROR);
#		endif
	}
#endif /* NNTP_ONLY */

	/*
	 *  Set up initial array sizes, char *'s: homedir, newsrc, etc.
	 */
	init_alloc ();
	hash_init ();
	init_selfinfo ();
	init_group_hash ();

	if (update_fork || (update && verbose) || !update) {
		error_message (cvers, "");
	}

	/*
	 *  Read user local & global config files
	 */
	read_config_file (global_config_file, TRUE);
	read_config_file (local_config_file, FALSE);

	/*
	 *  Process envargs & command line options
	 */
	read_cmd_line_options (argc, argv);

	if (newsrc_active && !read_news_via_nntp) {
		printf("Reading off local spool: -n ignored\n");
		newsrc_active = FALSE;
	}

	/*
	 *  Connect to nntp server?
	 */
	if (nntp_open () == -1) {
		exit (EXIT_ERROR);
	}

	/*
	 * Check if overview indexes contain Xref: lines
	 */
	if (xover_supported) {
		xref_supported = overview_xref_support ();
	}

#ifdef DEBUG_NEWSRC
	unlink ("/tmp/BITMAP");
	vNewsrcTestHarness ();
#endif

	/*
	 *  Read message of the day file from newsadmin
	 */
	read_motd_file ();

#ifdef WIN32
	/*
	 * Init curses emulation
	 */
	if (!InitScreen ()) {
		error_message (txt_screen_init_failed, progname);
		exit (EXIT_ERROR);
	}
#endif

	/*
	 *  Load the mail & news active files into active[]
	 */
	if (read_saved_news) {
		create_save_active_file ();
	}
#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
	read_mail_active_file ();
#endif
	read_news_active_file ();
	debug_print_active();

	/*
	 *  Load the local & global group specific attribute files
	 */
	read_attributes_file (global_attributes_file, TRUE);
	read_attributes_file (local_attributes_file, FALSE);

	/*
	 *  Read in users filter preferences file
	 *  This has to be done before quick post
	 *  because the filters will be updated!!![eb]
	 */
	global_filtered_articles = read_filter_file (global_filter_file, TRUE);
	local_filtered_articles = read_filter_file (local_filter_file, FALSE);
	debug_print_filters ();

	/*
	 *  Quick post an article & exit if -w specified
	 */
	if (post_article_and_exit) {
		global_filtered_articles = read_filter_file (global_filter_file, TRUE);
		local_filtered_articles = read_filter_file (local_filter_file, FALSE);
		debug_print_filters ();
		quick_post_article ();
		tin_done (EXIT_OK);
	}

	/*
	 *  Read text descriptions for mail & news groups from
	 *  ~/.tin/mailgroups & NEWSLIBDIR/newsgroups respectively
	 */
#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
	read_mailgroups_file ();
#endif
	read_newsgroups_file ();

	if (create_mail_save_dirs ()) {
		write_config_file (local_config_file);
	}

	num_cmd_line_groups = read_cmd_line_groups ();

#ifdef INDEX_DAEMON
	vMakeActiveMyGroup ();
#else
	backup_newsrc ();
	read_newsrc (newsrc, num_cmd_line_groups ? 0 : 1);
	if (!num_cmd_line_groups) {
		toggle_my_groups (show_only_unread_groups, "");
	}
#endif

	if (count_articles && !newsrc_active) {
	/*
	** what is count_articles good for?
	** if running in batch-mode (-Z) -v gives a summary!
	*/
		vGrpGetSubArtInfo ();
	}

	/*
	 *  Check/start if any new/unread articles
	 */
	start_groupnum = check_for_any_new_news (check_any_unread, start_any_unread);

	/*
	 *  Mail any new articles to specified user
	 *  or
	 *  Save any new articles to savedir structure for later reading
	 */
	save_or_mail_new_news ();

	/*
	 *  Catchup newsrc file (-c option)
	 */
	catchup_newsrc_file (newsrc);

	/*
	 *  Update index files
	 */
	update_index_files ();

#ifndef WIN32
	/*
	 * Init curses emulation
	 */
	if (!InitScreen ()) {
		error_message (txt_screen_init_failed, progname);
		exit (EXIT_ERROR);
	}
#endif

	/*
	 *  Set up screen and switch to raw mode
	 */
	setup_screen ();

	/*
	 *  If first time print welcome screen and auto-subscribe
	 *  to groups specified in /usr/lib/news/subscribe locally
	 *  or via NNTP if reading news remotely (LIST SUBSCRIBE)
	 */
	if (created_rcdir && !update) {
		show_intro_page ();
	}

	/*
	 *  Work loop
	 */
	selection_index (start_groupnum, num_cmd_line_groups);
	return(0); /* not reached */
}

/*
 * process command line options
 */

#ifndef INDEX_DAEMON
#	ifndef M_AMIGA
#		define OPTIONS "acCD:f:g:hHI:m:M:nN:qrRs:SuUvVwzZ"
#	else /* M_AMIGA */ /* may need some work */
#		define OPTIONS "BcCD:f:hHI:m:M:nN:qrRs:SuUvVwzZ"
#	endif
#else /* INDEX_DAEMON */
#	define OPTIONS "dD:f:hI:PvV"
#endif

void
read_cmd_line_options (argc, argv)
	int argc;
	char *argv[];
{
	int ch;
	int newsrc_set = 0;

	envargs (&argc, &argv, "TINRC");

	while ((ch = getopt (argc, argv, OPTIONS)) != EOF) {
		switch (ch) {
#ifndef INDEX_DAEMON
#	ifndef M_AMIGA
			case 'a':
#		ifdef HAVE_COLOR
				use_color = !use_color;
#		else
				error_message (txt_option_not_enabled, "-DHAVE_COLOR");
				exit (EXIT_ERROR);
#		endif
				break;
#	else
			case 'B':
				tin_bbs_mode = TRUE;
				break;
#	endif /* !M_AMIGA */
			case 'c':
				catchup = TRUE;
				break;

/* what is it good for? */
			case 'C':
				count_articles = TRUE;
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
				exit (EXIT_ERROR);
#endif
				break;

			case 'f':	/* active (tind) / newsrc (tin) file */
#ifdef INDEX_DAEMON
				my_strncpy (news_active_file, optarg, sizeof (news_active_file));
#else
				my_strncpy (newsrc, optarg, sizeof (newsrc));
				newsrc_set = 1;
#endif
				break;

#ifndef INDEX_DAEMON
#	ifndef M_AMIGA
			case 'g':	/* select alternative NNTP-server, implies -r */
#		ifdef NNTP_ABLE
				my_strncpy(cmdline_nntpserver, optarg, sizeof(cmdline_nntpserver));
				read_news_via_nntp = TRUE;
#		else
				error_message (txt_option_not_enabled, "-DNNTP_ABLE");
				exit (EXIT_ERROR);
#		endif
				break;
#	endif /* !M_AMIGA */

			case 'H':
				show_intro_page ();
				exit (EXIT_OK);

#endif /* !INDEX_DAEMON */

			case 'I':
#ifndef NNTP_ONLY
				my_strncpy (index_newsdir, optarg, sizeof (index_newsdir));
				my_mkdir (index_newsdir, 0777);
#else
				error_message (txt_option_not_enabled, "-DNNTP_ABLE");
				exit (EXIT_ERROR);
#endif
				break;

#ifndef INDEX_DAEMON
			case 'm':
				my_strncpy (default_maildir, optarg, sizeof (default_maildir));
				break;

			case 'M':	/* mail new news to specified user */
				my_strncpy (mail_news_user, optarg, sizeof (mail_news_user));
				mail_news = TRUE;
				update = TRUE;
				break;

			case 'n':
#	ifdef NNTP_ABLE
				newsrc_active = TRUE;
#	else
				error_message (txt_option_not_enabled, "-DNNTP_ABLE");
				exit (EXIT_ERROR);
#	endif
				break;

			case 'N':	/* mail new news to your posts */
				mail_news_to_posted = TRUE;
				break;
#else
			case 'P':	/* stat every art for a through purge */
				purge_index_files = TRUE;
				break;
#endif /* !INDEX_DAEMON */

#ifndef INDEX_DAEMON
			case 'q':
				check_for_new_newsgroups = FALSE;
				break;

			case 'r':	/* read news remotely from default NNTP server */
#	ifdef NNTP_ABLE
				read_news_via_nntp = TRUE;
#	else
				error_message (txt_option_not_enabled, "-DNNTP_ABLE");
				exit (EXIT_ERROR);
#	endif
				break;

			case 'R':	/* read news saved by -S option */
				read_saved_news = TRUE;
				my_strncpy (news_active_file, save_active_file, sizeof (news_active_file));
				break;

			case 's':
				my_strncpy (default_savedir, optarg, sizeof (default_savedir));
				break;

			case 'S':	/* save new news to dir structure */
				save_news = TRUE;
				update = TRUE;
				break;

			case 'u':	/* update index files */
#	ifndef NNTP_ONLY
				update = TRUE;
				show_description = FALSE;
#	else
				error_message (txt_option_not_enabled, "-DNNTP_ABLE");
				exit (EXIT_ERROR);
#	endif
				break;

			case 'U':	/* update index files in background */
#	ifndef NNTP_ONLY
				update_fork = TRUE;
				update = TRUE;
#	else
				error_message (txt_option_not_enabled, "-DNNTP_ABLE");
				exit (EXIT_ERROR);
#	endif
				break;

#endif /* !INDEX_DAEMON */

			case 'v':	/* verbose mode */
				verbose = TRUE;
				break;

			case 'V':
#if defined(__DATE__) && defined(__TIME__)
				sprintf (msg, "Version: %s release %s  %s %s",
					VERSION, RELEASEDATE, __DATE__, __TIME__);
#else
				sprintf (msg, "Version: %s release %s",
					VERSION, RELEASEDATE);
#endif
				error_message (msg, "");
				exit (EXIT_OK);

#ifndef INDEX_DAEMON
			case 'w':	/* post article & exit */
				post_article_and_exit = TRUE;
				break;

			case 'z':
				start_any_unread = TRUE;
				update = TRUE;
				break;

			case 'Z':
				check_any_unread = TRUE;
				update = TRUE;
				break;
#endif /* !INDEX_DAEMON */

			case 'h':
			case '?':
			default:
				usage (progname);
				exit (EXIT_ERROR);
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
			(int) uname(&uts);			
			get_newsrcname(newsrc,uts.nodename);
#else	/* NeXT, Apollo */
			char nodenamebuf[32];
			(int) gethostname(nodenamebuf, sizeof(nodenamebuf));
			get_newsrcname(newsrc,nodenamebuf);
#endif
		}
	}

}

/*
 * usage
 */

void
usage (theProgname)
	char *theProgname;
{
#ifndef INDEX_DAEMON
	error_message ("%s A Usenet reader.\n", cvers);
	error_message ("Usage: %s [options] [newsgroups]", theProgname);

#	ifndef M_AMIGA
#		ifdef HAVE_COLOR
			error_message ("  -a       toggle color flag","");
#		endif /* HAVE_COLOR */

#	else
		error_message ("  -B       BBS mode. File operations limited to home directories.","");
#	endif /* !M_AMIGA */

	error_message ("  -c       mark all news as read in subscribed newsgroups (batch mode)", "");

/* what is it good for? */
	error_message ("  -C       count unread articles", "");

#	ifdef DEBUG
	error_message ("  -D       debug mode 1=NNTP 2=ALL", "");
#	endif

	error_message ("  -f file  subscribed to newsgroups file [default=%s]", newsrc);

#	ifndef M_AMIGA
#		ifdef NNTP_ABLE
			error_message ("  -g serv  read news from NNTP server serv", "");
#		endif /* NNTP_ABLE */
#	endif /* !M_AMIGA */

	error_message ("  -h       this help message", "");
	error_message ("  -H       help information about %s", theProgname);

#	ifndef NNTP_ONLY
	error_message ("  -I dir   news index file directory [default=%s]", index_newsdir);
#	endif /* NNTP_ONLY */	

	error_message ("  -m dir   mailbox directory [default=%s]", default_maildir);
	error_message ("  -M user  mail new news to specified user (batch mode)", "");

#	ifdef NNTP_ABLE
		error_message ("  -n       only read subscribed .newsrc groups from NNTP server", "");
#	endif /* NNTP_ABLE */

	error_message ("  -N       mail new news to your posts", "");
	error_message ("  -q       quick start by not checking for new newsgroups", "");

#	ifdef NNTP_ABLE
		if (!read_news_via_nntp) {
			error_message ("  -r       read news remotely from default NNTP server", "");
		}
#	endif /* NNTP_ABLE */

	error_message ("  -R       read news saved by -S option", "");
	error_message ("  -s dir   save news directory [default=%s]", default_savedir);
	error_message ("  -S       save new news for later reading (batch mode)", "");

#	ifndef NNTP_ONLY
		error_message ("  -u       update index files (batch mode)", "");
		error_message ("  -U       update index files in the background while reading news", "");
#	endif /* NNTP_ONLY */

	error_message ("  -v       verbose output for batch mode options", "");
	error_message ("  -V       print version & date information", "");
	error_message ("  -w       post an article and exit", "");
	error_message ("  -z       start if any unread news", "");
	error_message ("  -Z       return status indicating if any unread news (batch mode)", "");

#else /* INDEX_DAEMON */
	error_message ("%s Tin index file daemon.\n", cvers);
	error_message ("Usage: %s [options] [newsgroups]", theProgname);	
	error_message ("  -d       delete index file before indexing articles", "");

#	ifdef DEBUG
	error_message ("  -D       debug mode 1=NNTP 2=ALL", "");
#	endif

	error_message ("  -f file  active newsgroups file [default=%s]", newsrc);
	error_message ("  -h       this help message", "");
	error_message ("  -I dir   news index file directory [default=%s]", index_newsdir);
	error_message ("  -P       purge any expired articles from index files", "");
	error_message ("  -v       verbose output for batch mode options", "");
	error_message ("  -V       print version & date information", "");
#endif /* INDEX_DAEMON */

	error_message ("\nMail bug reports/comments to %s", BUG_REPORT_ADDRESS);
}

/*
 *  check/start if any new/unread articles
 */

int
check_for_any_new_news (CheckAnyUnread, StartAnyUnread)
	int CheckAnyUnread;
	int StartAnyUnread;
{
	int i = 0;

	if (CheckAnyUnread) {
		i = check_start_save_any_news (CHECK_ANY_NEWS);
		exit (i);
	}

	if (StartAnyUnread) {
		i = check_start_save_any_news (START_ANY_NEWS);
		if (i == -1) {		/* no new/unread news so exit */
			exit (EXIT_OK);
		}
		update = FALSE;
	}

	return (i);
}

/*
 *  mail any new articles to specified user
 *  or
 *  save any new articles to savedir structure for later reading
 */

void
save_or_mail_new_news ()
{
	int i;

	if (mail_news || save_news) {
		i = catchup;			/* set catchup to FALSE */
		catchup = FALSE;
		do_update ();
		catchup = i;			/* set catchup to previous value */
		if (mail_news) {
			check_start_save_any_news (MAIL_ANY_NEWS);
		} else {
			check_start_save_any_news (SAVE_ANY_NEWS);
		}
		tin_done (EXIT_OK);
	}
}

/*
 *  update index files
 */

void
update_index_files ()
{
	if (update || update_fork) {
		if (!catchup && (read_news_via_nntp && xover_supported)) {
			error_message ("%s: Updating of index files not supported", progname);
			tin_done (EXIT_ERROR);
		}

		cCOLS = 132;				/* set because curses has not started */
#ifdef HAVE_FORK
		if (update_fork) {
			catchup = FALSE;		/* turn off msgs when running forked */
			verbose = FALSE;
			switch ((int) fork ()) {		/* fork child to update indexes in background */
				case -1:			/* error forking */
					perror_message ("Failed to start background indexing process", "");
					break;
				case 0:				/* child process */
					create_index_lock_file (lock_file);
					process_id = getpid ();
#ifdef BSD
#if defined(__FreeBSD__) || defined(__NetBSD__)
					setsid();
#else
#	ifdef __osf__
					setpgid (0, 0);
#	else
					setpgrp (0, process_id);	/* reset process group leader to this process */
#	endif
#	ifdef TIOCNOTTY
					{
						int fd;

						if ((fd = open ("/dev/tty", O_RDWR)) >= 0) {
							ioctl (fd, TIOCNOTTY, (char *) NULL);
							close (fd);
						}
					}
#	endif
#endif
#else
#	if HAVE_SETPGRP
					setpgrp ();
					signal (SIGHUP, SIG_IGN);	/* make immune from process group leader death */
#	endif
#endif
					signal (SIGQUIT, SIG_IGN);	/* stop indexing being interrupted */
					signal (SIGALRM, SIG_IGN);	/* stop indexing resyning active file */
					nntp_open ();				/* connect server if we are using nntp */
					default_thread_arts = THREAD_NONE;	/* stop threading to run faster */
					do_update ();
					tin_done (EXIT_OK);
					break;
				default:						/* parent process*/
					break;
			}
			update = FALSE;
		} else
#endif	/* HAVE_FORK */
		{
			create_index_lock_file (lock_file);
			default_thread_arts = THREAD_NONE;	/* stop threading to run faster */
			do_update ();
			tin_done (EXIT_OK);
		}
	}

}

/*
 *  display page of general info. for first time user.
 */

void
show_intro_page ()
{
	if (cmd_line) {
		wait_message (cvers);
	} else {
		ClearScreen ();
		center_line (0, TRUE, cvers);
		Raw (FALSE);
	}

	printf ("\n\nWelcome to tin, a full screen threaded Netnews reader. It can read news locally\n");
	printf ("(ie. <spool>/news) or remotely (-r option) from a NNTP  (Network News Transport\n");
	printf ("Protocol) server. tin -h lists the available command line options.\n\n");

	printf ("Tin has five  newsreading levels,  the newsgroup  selection page,  the spooldir\n");
	printf ("selection page,  the group index page,  the thread listing page and the article\n");
	printf ("viewer. Help is available at each level by pressing the 'h' command.\n\n");

	printf ("Move up/down by using the terminal arrow keys or 'j' and 'k'.  Use PgUp/PgDn or\n");
	printf ("Ctrl-U and Ctrl-D to page up/down. Enter a newsgroup by pressing RETURN/TAB.\n\n");

	printf ("Articles, threads, tagged articles or articles matching a pattern can be mailed\n");
	printf ("('m' command), printed ('o' command), saved ('s' command), piped ('|' command).\n");
	printf ("Use the 'w' command  to post  a news  article,  the 'f'/'F' commands to  post a\n");
	printf ("follow-up  to  an existing  news article and the 'r'/'R' commands to  reply via\n");
	printf ("mail to an existing news articles author.  The 'M' command allows the operation\n");
	printf ("of tin to be configured via a menu.\n\n");

	printf ("For more information read the manual page, README, INSTALL, TODO and FTP files.\n");
	printf ("Please send bug reports/comments to the programs author with the 'R' command.\n");
	fflush (stdout);

	if (!cmd_line) {
		Raw (TRUE);
		continue_prompt ();
	}
}


int
read_cmd_line_groups ()
{
	char buf[PATH_LEN];
	int matched = 0;
	int num;
	register int i;

	if (num_cmdargs < max_cmdargs) {
		group_top = 0;

		for (num = num_cmdargs ; num < max_cmdargs ; num++) {
			sprintf (buf, txt_matching_cmd_line_groups, cmdargs[num]);
			wait_message (buf);

			for (i = 0 ; i < num_active ; i++) {
				if (wildmat (active[i].name, cmdargs[num])) {
					if (add_my_group (active[i].name, 1) != -1) {
						active[i].subscribed = SUBSCRIBED;
						matched++;
					}
				}
			}
		}
	}

	return matched;
}
