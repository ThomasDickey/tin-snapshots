/*
 *  Project   : tin - a Usenet reader
 *  Module    : select.c
 *  Author    : I. Lea & R. Skrenta
 *  Created   : 1991-04-01
 *  Updated   : 1994-12-21
 *  Notes     :
 *  Copyright : (c) Copyright 1991-98 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"
#include	"menukeys.h"

char default_goto_group[LEN];
int default_move_group;
int cur_groupnum = 0;
int first_group_on_screen;
int last_group_on_screen;
/*
 * Oddly named variable - seems to dictate whether to position on first
 * unread item or not. Used internally unlike 'pos_first_unread', which
 * is a tinrc variable.
 */
t_bool space_mode;

/*
 * Local prototypes
 */
#ifndef INDEX_DAEMON
	static int continual_key (int ch, int ch1);
	static int reposition_group (struct t_group *group, int default_num);
	static t_bool pos_next_unread_group (t_bool redraw);
	static t_bool read_groups (void);
	static void catchup_group (struct t_group *group, t_bool goto_next_unread_group);
	static void move_to_group (int n);
	static void prompt_group_num (int ch);
	static void subscribe_pattern (const char *prompt, const char *message, const char *result, t_bool state);
	static void yank_active_file (void);
#endif /* !INDEX_DAEMON */

static t_bool bParseRange (char *pcRange, int iNumMin, int iNumMax, int iNumCur, int *piRngMin, int *piRngMax);
static void vDelRange (int iLevel, int iNumMax);
static void erase_group_arrow (void);


/*
 *  TRUE, if we should check whether it's time to reread the active file
 *  after this keypress.
 */
static int
continual_key (
	int ch,
	int ch1)
{
	switch(ch) {
#ifndef NO_SHELL_ESCAPE
		case iKeyShellEscape:
#endif
		case iKeyLookupMessage:
		case iKeyOptionMenu:
		case iKeyQuit:
		case iKeyPostponed:
		case iKeySelectResetNewsrc:
		case iKeySelectQuit2:
		case iKeySelectBugReport:
		case iKeyDisplayPostHist:
		case iKeySelectQuitNoWrite:
		case iKeySelectSyncWithActive:
		case iKeySelectHelp:
		case iKeySelectPost:
			return FALSE;

#ifndef WIN32
		case ESC:
#ifdef HAVE_KEY_PREFIX
		case KEY_PREFIX:
#endif
			switch(ch1) {
#endif /* WIN32 */
				case KEYMAP_LEFT:
					return FALSE;
#ifndef WIN32
			}
			/* FALLTHROUGH */
#endif /* WIN32 */

		default:
			return TRUE;
	}
}

void
selection_page (
	int start_groupnum,
	int num_cmd_line_groups)
{
#ifndef INDEX_DAEMON

	char buf[LEN];
	char post_group[LEN];
	int i, n, ch, ch1 = 0;
	int INDEX_BOTTOM;
	int posted_flag;
	t_bool yank_in_active_file = TRUE;

	cur_groupnum = start_groupnum;

#ifdef READ_CHAR_HACK
	setbuf (stdin, 0);
#endif

	ClearScreen();
	set_groupname_len (FALSE);	/* find longest subscribed to groupname */

	/*
	 * If user specified only 1 cmd line groupname (eg. tin -r alt.sources)
	 * then go there immediately.
	 */
	if (num_cmd_line_groups == 1)
		goto select_read_group;

	show_selection_page ();	/* display group selection page */

	forever {
		if (!resync_active_file () && reread_active_after_posting ())	/* reread active file if necessary */
			show_selection_page ();
		set_xclick_on ();
		ch = ReadCh ();
#ifndef WIN32
		switch(ch) {
			case ESC:	/* (ESC) common arrow keys */
#ifdef HAVE_KEY_PREFIX
			case KEY_PREFIX:
#endif
				ch1 = get_arrow_key (ch);
		}
#endif /* WIN32 */

		if (continual_key (ch, ch1))
			(void) resync_active_file ();

		if (ch > '0' && ch <= '9') {
			prompt_group_num (ch);
			continue;
		}
		switch (ch) {
#ifndef WIN32
			case ESC:	/* (ESC) common arrow keys */
#ifdef HAVE_KEY_PREFIX
			case KEY_PREFIX:
#endif
				switch (ch1) {
#endif /* WIN32 */
					case KEYMAP_UP:
						goto select_up;

					case KEYMAP_DOWN:
						goto select_down;

					case KEYMAP_LEFT:
						goto select_done;

					case KEYMAP_RIGHT:
						goto select_read_group;

					case KEYMAP_PAGE_UP:
						goto select_page_up;

					case KEYMAP_PAGE_DOWN:
						goto select_page_down;

					case KEYMAP_HOME:
						goto top_of_list;

					case KEYMAP_END:
						goto end_of_list;
#ifndef WIN32
					case KEYMAP_MOUSE:
						INDEX_BOTTOM = INDEX_TOP+last_group_on_screen-first_group_on_screen;
						switch (xmouse)
						{
							case MOUSE_BUTTON_1:
							case MOUSE_BUTTON_3:
								if (xrow < INDEX_TOP || xrow >= INDEX_BOTTOM)
									goto select_page_down;

								erase_group_arrow ();
								cur_groupnum = xrow-INDEX_TOP+first_group_on_screen;
								draw_group_arrow ();
								if (xmouse == MOUSE_BUTTON_1)
									goto select_read_group;
								break;

							case MOUSE_BUTTON_2:
								if (xrow < INDEX_TOP || xrow >= INDEX_BOTTOM)
									goto select_page_up;

								goto select_done;

							default:
								break;
						}
						break;
					default:
						break;
				}
				break;
#endif /* !WIN32 */

#ifndef NO_SHELL_ESCAPE
			case iKeyShellEscape:
				shell_escape ();
				show_selection_page ();
				break;
#endif

			case iKeyFirstPage:	/* show first page of groups */
top_of_list:
				if (group_top)
					move_to_group(0);
				break;

			case iKeyLastPage:	/* show last page of groups */
end_of_list:
				if (group_top)
					move_to_group(group_top - 1);
				break;

			case iKeySetRange:	/* set range */
				if (bSetRange (SELECT_LEVEL, 1, group_top, cur_groupnum+1))
					show_selection_page ();
				break;

			case iKeySearchSubjF:	/* search forward */
			case iKeySearchSubjB:	/* search backward */
				i = (ch == iKeySearchSubjF);

				if ((i = search_active (i)) != -1) {
					move_to_group (i);
					clear_message ();
				}
				break;

			case iKeySelectReadGrp:	/* go into group */
			case iKeySelectReadGrp2:
select_read_group:
				if (!group_top) {
					info_message (txt_no_groups);
					break;
				}

				if (CURR_GROUP.bogus) {
					info_message (txt_not_exist);
					break;
				}

				n = my_group[cur_groupnum];
				if (active[n].xmax > 0 && (active[n].xmin <= active[n].xmax)) {
					if (!read_groups())
						goto select_done;
				} else
					info_message (txt_no_arts);
				break;

			case iKeyPageDown3:
				if (!space_goto_next_unread)
					goto select_page_down;
				/* FALLTHROUGH */
			case iKeySelectEnterNextUnreadGrp:	/* enter next group containing unread articles */
			case iKeySelectEnterNextUnreadGrp2:
				if (pos_next_unread_group (FALSE)) {
					if (!read_groups ())
						goto select_done;		/* User quit */
				}

				break;							/* Nothing more to do at the moment */

			case iKeyPageDown:		/* page down */
			case iKeyPageDown2:
select_page_down:
				if (group_top)
					move_to_group (page_down (cur_groupnum, group_top));
				break;

			case iKeySelectRedrawScr:		/* redraw */
				my_retouch ();
				set_xclick_off ();
				show_selection_page ();
				break;

			case iKeyDown:		/* line down */
			case iKeyDown2:
select_down:
				if (group_top)
					move_to_group((cur_groupnum + 1 >= group_top) ? 0 : (cur_groupnum + 1));
				break;

			case iKeyUp:		/* line up */
			case iKeyUp2:
select_up:
				if (group_top)
					move_to_group ((cur_groupnum == 0) ? (group_top - 1) : (cur_groupnum - 1));
				break;

			case iKeySelectResetNewsrc:	/* reset .newsrc */
				if (prompt_yn (cLINES, txt_reset_newsrc, FALSE) == 1) {
					reset_newsrc ();
					yank_active_file ();
					cur_groupnum = 0;
					show_selection_page ();
				}
				break;

			case iKeyPageUp:		/* page up */
			case iKeyPageUp2:
			case iKeyPageUp3:
select_page_up:
				if (group_top)
					move_to_group (page_up (cur_groupnum, group_top));
				break;

			case iKeySelectCatchup:	/* catchup - mark all articles as read */
			case iKeySelectCatchupNextUnread:	/* and goto next unread group */
				if (!group_top)
					break;
				catchup_group (&CURR_GROUP, (ch == iKeySelectCatchupNextUnread));
				break;

			case iKeySelectToggleSubjDisplay:	/* toggle newsgroup descriptions */
				show_description = !show_description;
				if (show_description)
					read_newsgroups_file ();
				set_groupname_len (FALSE);
				show_selection_page ();
				break;

			case iKeySelectGoto:	/* prompt for a new group name */
				n = choose_new_group ();
				if (n >= 0) {
					erase_group_arrow ();
					cur_groupnum = n;
					set_groupname_len (FALSE);
					if (cur_groupnum < first_group_on_screen ||
						cur_groupnum >= last_group_on_screen ||
						cur_groupnum != n) {
						show_selection_page ();
					} else {
						clear_message ();
						draw_group_arrow();
					}
				}
				break;

			case iKeySelectHelp:	/* help */
				show_info_page (HELP_INFO, help_select, txt_group_select_com);
				show_selection_page ();
				break;

			case iKeySelectToggleHelpDisplay:	/* toggle mini help menu */
				toggle_mini_help (SELECT_LEVEL);
				show_selection_page ();
				break;

			case iKeySelectToggleInverseVideo:	/* toggle inverse video */
				toggle_inverse_video ();
				show_selection_page ();
				show_inverse_video_status ();
				break;

#ifdef HAVE_COLOR
			case iKeySelectToggleColor:		/* toggle color */
				if (toggle_color ()) {
					show_selection_page ();
					show_color_status ();
				}
				break;
#endif

			case iKeyToggleInfoLastLine:	/* display group description */
				info_in_last_line = !info_in_last_line;
				show_selection_page ();
				break;

			case iKeySelectMoveGrp:	/* reposition group within group list */
				if (!CURR_GROUP.subscribed) {
					wait_message(0, txt_info_not_subscribed);
					break;
				}

				n = cur_groupnum;
				cur_groupnum = reposition_group (&active[my_group[n]], n);
				HpGlitch(erase_group_arrow ());
				if (cur_groupnum < first_group_on_screen ||
					cur_groupnum >= last_group_on_screen ||
					cur_groupnum != n) {
					show_selection_page ();
				} else {
					i = cur_groupnum;
					cur_groupnum = n;
					erase_group_arrow ();
					cur_groupnum = i;
					clear_message ();
					draw_group_arrow ();
				}
				break;

			case iKeyOptionMenu:	/* option menu */
				(void) change_config_file(NULL);
				set_signals_select ();	/* just to be sure */
				free_attributes_array ();
				read_attributes_file (global_attributes_file, TRUE);
				read_attributes_file (local_attributes_file, FALSE);
				show_selection_page ();
				break;

			case iKeySelectNextUnreadGrp:	/* goto next unread group */
				pos_next_unread_group (TRUE);
				break;

			case iKeyQuit:	/* quit */
select_done:
				if (!confirm_to_quit || prompt_yn (cLINES, txt_quit, TRUE) == 1) {
					if (!no_write)
						write_config_file (local_config_file);
					tin_done (EXIT_SUCCESS);	/* Tin END */
				}
				if (!no_write && prompt_yn (cLINES, txt_save_config, TRUE) == 1) {
					write_config_file (local_config_file);
					vWriteNewsrc ();
				}
				show_selection_page ();
				break;

			case iKeySelectQuit2:	/* quit, no ask */
				if (!no_write)
					write_config_file (local_config_file);
				tin_done (EXIT_SUCCESS);
				break;

			case iKeySelectQuitNoWrite:	/* quit, but don't save configuration */
				if (prompt_yn (cLINES, txt_quit_no_write, TRUE) == 1)
					tin_done (EXIT_SUCCESS);
				show_selection_page ();
				break;

			case iKeySelectToggleReadDisplay:
				/*
				 * If in show_only_unread_groups mode toggle
				 * all subscribed to groups and only groups
				 * that contain unread articles
				 */
				show_only_unread_groups = !show_only_unread_groups;
				wait_message (0, txt_reading_groups, (show_only_unread_groups) ? "unread" : "all");

				toggle_my_groups (show_only_unread_groups, "");
				set_groupname_len (FALSE);
				show_selection_page ();
				break;

			case iKeySelectBugReport:	/* bug/gripe/comment mailed to author */
				mail_bug_report ();
				ClearScreen ();
				show_selection_page ();
				break;

			case iKeySelectSubscribe:	/* subscribe to current group */
				if (!group_top)
					break;
				if (!CURR_GROUP.subscribed && !CURR_GROUP.bogus) {
					subscribe (&CURR_GROUP, SUBSCRIBED);
					show_selection_page();
					info_message (txt_subscribed_to, CURR_GROUP.name);
				}
				break;

			case iKeySelectSubscribePat:	/* subscribe to groups matching pattern */
				subscribe_pattern (txt_subscribe_pattern, txt_subscribing, txt_subscribed_num_groups, TRUE);
				break;

			case iKeySelectUnsubscribe:	/* unsubscribe to current group */
				if (!group_top)
					break;

				if (CURR_GROUP.subscribed) {
					mark_screen (SELECT_LEVEL, cur_groupnum - first_group_on_screen,
											2, CURR_GROUP.newgroup ? "N" : "u");
					subscribe (&CURR_GROUP, UNSUBSCRIBED);
					info_message(txt_unsubscribed_to, CURR_GROUP.name);
					move_to_group (cur_groupnum + 1);
				} else if (CURR_GROUP.bogus && strip_bogus == BOGUS_ASK && !no_write) {
					/* Bogus groups aren't subscribed to avoid confusion */
					sprintf (buf, txt_remove_bogus, CURR_GROUP.name);
					vWriteNewsrc();									/* save current newsrc */
					delete_group(CURR_GROUP.name);					/* remove bogus group */
					read_newsrc(newsrc, TRUE);						/* reload newsrc */
					toggle_my_groups (show_only_unread_groups, ""); /* keep current display-state */
					show_selection_page();							/* reddraw screen */
					info_message (buf);
				}
				break;

			case iKeySelectUnsubscribePat:	/* unsubscribe to groups matching pattern */
				subscribe_pattern (txt_unsubscribe_pattern,
								txt_unsubscribing, txt_unsubscribed_num_groups, FALSE);
				break;

			case iKeyVersion:	/* show tin version */
				info_message (cvers);
				break;

			case iKeySelectPost:	/* post a basenote */
				if (can_post) {
					if (!group_top) {
						if (!prompt_string (txt_post_newsgroup, buf, HIST_POST_NEWSGROUPS))
							break;
						if (buf[0] == '\0')
							break;
						strcpy (post_group, buf);
						if (find_group_index (post_group) == -1) {
							error_message (txt_not_in_active_file, post_group);
							break;
						}
					} else
						strcpy (post_group, CURR_GROUP.name);
					if (post_article (post_group, &posted_flag))
						show_selection_page ();
				} else
					info_message(txt_cannot_post);
				break;

			case iKeyPostponed:
			case iKeyPostponed2:	/* post postponed article */
				if (can_post) {
					if (pickup_postponed_articles(FALSE, FALSE))
						show_selection_page ();
				} else
					info_message(txt_cannot_post);
				break;

			case iKeyDisplayPostHist:	/* display messages posted by user */
				if (user_posted_messages ())
					show_selection_page ();
				break;

			case iKeySelectYankActive:	/* pull in rest of groups from active */
				if (yank_in_active_file) {
					wait_message (0, txt_yanking_all_groups);
					n = group_top;
					if (group_top)
						strcpy (buf, CURR_GROUP.name);
					/*
					 * Reset counter and read in all the groups in active[]
					 */
					group_top = 0;
					for (i = 0; i < num_active; i++)
						my_group[group_top++] = i;

					/*
					 * If there are now more groups than before, we did yank something
					 */
					if (n < group_top) {
						if (n)	/* Keep us positioned on the group we were before */
							cur_groupnum = my_group_add (buf);
						set_groupname_len (yank_in_active_file);
						show_selection_page ();
						info_message (txt_added_groups, group_top - n, (group_top - n) == 1 ? "" : txt_plural);
					} else
						info_message (txt_no_groups_to_yank_in);
					yank_in_active_file = FALSE;
				} else {												/* Yank out */
					wait_message (0, txt_yanking_sub_groups);

					n = group_top;
					if (group_top)
						strcpy (buf, CURR_GROUP.name);

					toggle_my_groups(show_only_unread_groups, "");
					HpGlitch(erase_group_arrow ());

					cur_groupnum = -1;
					if (n)  /* Keep us positioned on the group we were before */
						cur_groupnum = add_my_group (buf, FALSE);
					if (cur_groupnum == -1)
						cur_groupnum = group_top - 1;

					set_groupname_len (yank_in_active_file);
					show_selection_page ();
					yank_in_active_file = TRUE;
				}
				break;

			case iKeySelectSyncWithActive:	/* yank active file to see if any new news */
				yank_active_file ();
				break;

			case iKeySelectMarkGrpUnread:
			case iKeySelectMarkGrpUnread2:	/* mark group unread */
				if (!group_top)
					break;
				grp_mark_unread (&CURR_GROUP);
				if (CURR_GROUP.newsrc.num_unread)
					strcpy (mesg, tin_ltoa(CURR_GROUP.newsrc.num_unread, 5));
				else
					strcpy (mesg, "     ");
				mark_screen (SELECT_LEVEL, cur_groupnum - first_group_on_screen, 9, mesg);
				break;

			default:
				info_message(txt_bad_command);
		}
	}

#endif /* !INDEX_DAEMON */
}


void
show_selection_page (void)
{
#ifndef INDEX_DAEMON

	char buf[LEN];
	char tmp[10];
	char subs;
	int i, j, n;
	int blank_len;
	char active_name[255];
	char group_descript[255];

	set_signals_select ();

	MoveCursor (0, 0);		/* top left corner */
	CleartoEOLN ();

	if (read_news_via_nntp)
		sprintf (buf, "%s (%s  %d%s)", txt_group_selection, nntp_server, group_top, (show_only_unread_groups ? " R" : ""));
	else
		sprintf (buf, "%s (%d%s)", txt_group_selection, group_top, (show_only_unread_groups ? " R" : ""));

	show_title (buf);
	MoveCursor (1, 0);
	CleartoEOLN ();
	MoveCursor (INDEX_TOP, 0);

	if (cur_groupnum < 0)
		cur_groupnum = 0;

	if (cur_groupnum >= group_top)
		cur_groupnum = group_top - 1;

	set_first_screen_item (cur_groupnum, group_top, &first_group_on_screen, &last_group_on_screen);

	blank_len = (cCOLS - (groupname_len + SELECT_MISC_COLS)) + (show_description ? 2 : 4);

	for (j = 0, i = first_group_on_screen; i < last_group_on_screen; i++, j++) {
#ifdef USE_CURSES
		char sptr[BUFSIZ];
#else
		char *sptr = screen[j].col;
#endif
		if (active[my_group[i]].inrange)
			strcpy (tmp, "    #");
		else if (active[my_group[i]].newsrc.num_unread) {
			strcpy (tmp, tin_ltoa(active[my_group[i]].newsrc.num_unread, 5));
		} else
			strcpy (tmp, "     ");

		n = my_group[i];

		/*
		 * Display a flag for this group if needed
		 * . Bogus groups are dumped immediately
		 * . Normal subscribed groups may be
		 *   ' ' normal, 'X' not postable, 'M' moderated, '=' renamed
		 * . Newgroups are 'N'
		 * . Unsubscribed groups are 'u'
		 */
		if (active[n].bogus)					/* Group is not in active list */
			subs = 'D';
		else if (active[n].subscribed)			/* Important that this preceeds Newgroup */
			subs = group_flag (active[n].moderated);
		else
			subs = ((active[n].newgroup) ? 'N' : 'u'); /* New (but unsubscribed) group or unsubscribed group */

		strncpy(active_name, active[n].name, groupname_len);
		active_name[groupname_len+1] = '\0';
		if (blank_len > 254)
			blank_len = 254;
		/* copy of active[n].description fix some malloc bugs kg */
		strncpy(group_descript, active[n].description ? active[n].description : " ", blank_len);
		group_descript[blank_len+1] = '\0';

		if (show_description) {
			if (active[n].description)
				sprintf (sptr, "  %c %s %s  %-*.*s  %-*.*s" cCRLF,
				         subs, tin_ltoa(i+1, 4), tmp,
				         groupname_len, groupname_len, active_name,
				         blank_len, blank_len, group_descript);
			else
				sprintf (sptr, "  %c %s %s  %-*.*s  " cCRLF,
				         subs, tin_ltoa(i+1, 4), tmp,
				         (groupname_len+blank_len),
				         (groupname_len+blank_len), active[n].name);
		} else {
			if (draw_arrow_mark)
				sprintf (sptr, "  %c %s %s  %-*.*s" cCRLF, subs, tin_ltoa(i+1, 4), tmp, groupname_len, groupname_len, active_name);
			else
				sprintf (sptr, "  %c %s %s  %-*.*s%*s" cCRLF, subs, tin_ltoa(i+1, 4), tmp, groupname_len, groupname_len, active_name, blank_len, " ");
		}
		if (strip_blanks) {
			strip_line (sptr);
			strcat (sptr, cCRLF);
		}
		CleartoEOLN ();
		my_fputs (sptr, stdout);
	}

	CleartoEOS ();
	show_mini_help (SELECT_LEVEL);

	if (group_top <= 0) {
		info_message (txt_no_groups);
		return;
	} else if (last_group_on_screen == group_top)
		info_message (txt_end_of_groups);

	draw_group_arrow ();

#endif /* !INDEX_DAEMON */
}


#ifndef INDEX_DAEMON
static void
prompt_group_num (
	int ch)
{
	int num;

	clear_message ();

	if ((num = prompt_num (ch, txt_select_group)) == -1) {
		clear_message ();
		return;
	}
	num--;		/* index from 0 (internal) vs. 1 (user) */

	if (num < 0)
		num = 0;

	if (num >= group_top)
		num = group_top - 1;

	move_to_group (num);
}
#endif /* !INDEX_DAEMON */


static void
erase_group_arrow (void)
{
	erase_arrow (INDEX_TOP + (cur_groupnum-first_group_on_screen));
}


void
draw_group_arrow (void)
{
	draw_arrow (INDEX_TOP + (cur_groupnum-first_group_on_screen));
	if (!group_top)
		info_message (txt_no_groups);
	else if (info_in_last_line)
		info_message ("%s", CURR_GROUP.description ? CURR_GROUP.description : txt_no_description);
}


#ifndef INDEX_DAEMON
static void
yank_active_file (void)
{
	force_reread_active_file = TRUE;
	resync_active_file ();
}
#endif /* !INDEX_DAEMON */


int
choose_new_group (void)
{
	char buf[LEN];
	char *p;
	int idx;

	sprintf (mesg, txt_newsgroup, default_goto_group);

	if (!prompt_string (mesg, buf, HIST_GOTO_GROUP))
		return -1;

	if (strlen (buf))
		strcpy (default_goto_group, buf);
	else {
		if (*default_goto_group)
			strcpy (buf, default_goto_group);
		else
			return -1;
	}

	/*
	 * Skip leading whitespace, ignore blank strings
	 */
	for (p = buf; *p && (*p == ' ' || *p == '\t'); p++)
		continue;

	if (*p == '\0')
		return -1;

	clear_message ();

	idx = my_group_add (p);
	if (idx == -1)
		info_message (txt_not_in_active_file, p);

	return idx;
}

/*
 * Return new value for group_top, skipping any new newsgroups that have been
 * found
 */
int
skip_newgroups(void)
{
	int i=0;

	if (group_top) {
		while (i < group_top && active[my_group[i]].newgroup)
			i++;
	}

	return(i);
}

/*
 *  Find a group in the users selection list, my_group[]
 *  If 'add' is TRUE, then add the supplied group
 *  Return the index into my_group[] if group is added or was already
 *  there.  Return -1 if group is not in active[]
 */
int
add_my_group (
	char *group,
	t_bool add)
{
	int i, j;

	if ((i = find_group_index (group)) < 0)
		return -1;

	for (j = 0; j < group_top; j++) {
		if (my_group[j] == i)
			return j;
	}

	if (add) {
		my_group[group_top++] = i;
		return (group_top - 1);
	}

	return -1;
}

#ifndef INDEX_DAEMON
static int
reposition_group (
	struct t_group *group,
	int default_num)
{
	char buf[LEN];
	char pos[LEN];
	int pos_num, newgroups;

	sprintf (buf, txt_newsgroup_position, group->name,
		(default_move_group ? default_move_group : default_num+1));

	if (!prompt_string (buf, pos, HIST_MOVE_GROUP))
		return default_num;

	if (strlen (pos))
		pos_num = ((pos[0] == '$') ? group_top: atoi (pos));
	else {
		if (default_move_group)
			pos_num = default_move_group;
		else
			return default_num;
	}

	if (pos_num > group_top)
		pos_num = group_top;
	else if (pos_num <= 0)
		pos_num = 1;

	newgroups = skip_newgroups();

	/*
	 * Can't move into newgroups, they aren't in .newsrc
	 */
	if (pos_num <= newgroups) {
		error_message(txt_skipping_newgroups);
		return(default_num);
	}

	wait_message (0, txt_moving, group->name);

	/*
	 * seems to have the side effect of rearranging
	 * my_groups, so show_only_unread_groups has to be updated
	 */
	show_only_unread_groups = FALSE;

	/*
	 * New newgroups aren't in .newsrc so we need to offset to
	 * get the right position
	 */
	if (pos_group_in_newsrc (group, pos_num - newgroups)) {
		read_newsrc (newsrc, TRUE);
		default_move_group = pos_num;
		return (pos_num-1);
	} else {
		default_move_group = default_num + 1;
		return (default_num);
	}
}


static void
catchup_group (
	struct t_group *group,
	t_bool goto_next_unread_group)
{
	if (!confirm_action || prompt_yn (cLINES, sized_message(txt_mark_group_read, group->name), TRUE) == 1) {
		grp_mark_read (group, NULL);
		mark_screen (SELECT_LEVEL, cur_groupnum - first_group_on_screen, 9, "     ");

		move_to_group (cur_groupnum + 1);

		if (goto_next_unread_group)
			pos_next_unread_group (TRUE);
	}
}


/*
 * Set cur_groupnum to next group with unread arts
 * If the current group has unread arts, it will be counted. This is important !
 * If redraw is set, update the selection menu appropriately
 * Return FALSE if no groups left to read
 *        TRUE  at all other times
 */
static t_bool
pos_next_unread_group (
	t_bool redraw)
{
	int i;
	t_bool all_groups_read = TRUE;

	for (i = cur_groupnum; i < group_top; i++) {
		if (UNREAD_GROUP (i)) {
			all_groups_read = FALSE;
			break;
		}
	}

	if (all_groups_read) {
		for (i = 0; i < cur_groupnum; i++) {
			if (UNREAD_GROUP (i)) {
				all_groups_read = FALSE;
				break;
			}
		}
	}

	if (all_groups_read) {
		info_message (txt_no_groups_to_read);
		return FALSE;
	}

	if (redraw)
		move_to_group (i);
	else
		cur_groupnum = i;

	return TRUE;
}


/*
 * This is the main loop that cycles through, reading groups.
 * We keep going until we return to the selection screen
 * (return TRUE) or quit (return FALSE)
 */
static t_bool
read_groups()
{
	t_bool done = FALSE;

	space_mode = pos_first_unread;

	clear_message ();

	forever {

		if (done)
			break;

		switch (group_page (&CURR_GROUP)) {

			case GRP_QUIT:
				return FALSE;

			case GRP_NEXT:
				if (cur_groupnum + 1 < group_top)
					cur_groupnum++;

				done = TRUE;
				break;

			case GRP_NEXTUNREAD:
				if (!pos_next_unread_group (FALSE))
					done = TRUE;
				break;

			case GRP_RETURN:
			default:
				done = TRUE;
				break;
		}
	}

	if (!reread_active_file ())
		show_selection_page ();

	return TRUE;
}
#endif /* !INDEX_DAEMON */


/*
 *  Calculate max length of groupname field for group selection level.
 *  If all_groups is TRUE check all groups in active file otherwise
 *  just subscribed to groups.
 */
void
set_groupname_len (
	t_bool all_groups)
{
	int len;
	register int i;

	groupname_len = 0;

	if (all_groups) {
		for (i = 0; i < num_active; i++) {
			if ((len = strlen (active[i].name)) > groupname_len)
				groupname_len = len;
		}
	} else {
		for (i = 0; i < group_top; i++) {
			if ((len = strlen (active[my_group[i]].name)) > groupname_len)
				groupname_len = len;
		}
	}

	if (groupname_len >= (cCOLS - SELECT_MISC_COLS)) {
		groupname_len = cCOLS - SELECT_MISC_COLS - 1;
		if (groupname_len < 0)
			groupname_len = 0;
	}

	/*
	 * If newsgroups descriptions are ON then cut off groupnames
	 * to specified max. length otherwise display full length
	 */
	if (show_description && groupname_len > groupname_max_length)
		groupname_len = groupname_max_length;
}


/*
 * Toggle my_group[] between all groups / only unread groups
 * We make a special case for Newgroups (always appear, at the top)
 * and Bogus groups if strip_bogus = BOGUS_ASK
 */
void
toggle_my_groups (
	t_bool only_unread_groups,
	const char *group)
{
#ifndef INDEX_DAEMON
	FILE *fp;
	char buf[NEWSRC_LINE];
	char old_curr_group[PATH_LEN];
	char *ptr;
	int active_idx = 0;
	int group_num = cur_groupnum;
	register int i;

	if ((fp = fopen (newsrc, "r")) == (FILE *) 0)
		return;

	/*
	 * Save current or next group with unread arts for later use
	 */
	old_curr_group[0] = '\0';

	if (group_top) {
		if (!only_unread_groups || reread_active_file ()) {

			if (group[0] != '\0') {
				if ((i = find_group_index (group)) >= 0)
					active_idx = i;
			} else
				active_idx = my_group[group_num];

			my_strncpy (old_curr_group, active[active_idx].name, sizeof (old_curr_group));
		} else {
			for (i = group_num; i < group_top; i++) {
				if (active[my_group[i]].newsrc.num_unread) {
					my_strncpy (old_curr_group, active[my_group[i]].name, sizeof (old_curr_group));
					break;
				}
			}
		}
	}

	group_top = skip_newgroups();			/* Reposition after any newgroups */

	while (fgets (buf, (int) sizeof(buf), fp) != (char *) 0) {
		if ((ptr = strchr (buf, SUBSCRIBED)) != (char *) 0) {
			*ptr = '\0';

			if ((i = find_group_index (buf)) < 0)
				continue;

			if (only_unread_groups) {
				if (active[i].newsrc.num_unread || (active[i].bogus && strip_bogus == BOGUS_ASK))
					my_group_add (buf);
			} else
				my_group_add (buf);
		}
	}
	fclose (fp);

	/*
	 * Try and reposition on same or next group before toggling
	 */
	if ((cur_groupnum = my_group_find(old_curr_group)) == -1)
		cur_groupnum = 0;

#endif	/* !INDEX_DAEMON */
}


#ifndef INDEX_DAEMON
/*
 * Subscribe or unsubscribe from a list of groups. List can be full list as supported
 * by match_group_list()
 */
static void
subscribe_pattern (
	const char *prompt,
	const char *message,
	const char *result,
	t_bool state)
{
	char buf[LEN];
	int i, subscribe_num;

	if (!num_active)
		return;

	if (!prompt_string (prompt, buf, HIST_OTHER) || !*buf) {
		clear_message ();
		return;
	}

	wait_message (0, message);

	/* TODO - so why precisely do we need these 2 separate passes ? */

	for (subscribe_num = 0, i = 0; i < group_top; i++) {
		if (match_group_list (active[my_group[i]].name, buf)) {
			if (active[my_group[i]].subscribed != (state != FALSE)) {
				spin_cursor ();
				subscribe (&active[my_group[i]], SUB_CHAR(state));
				subscribe_num++;
			}
		}
	}

	if (num_active > group_top) {
		for (i = 0; i < num_active; i++) {
			if (match_group_list (active[i].name, buf)) {
				if (active[i].subscribed != (state != FALSE)) {
					spin_cursor ();
					/* If found and group is not subscribed add it to end of my_group[]. */
					subscribe (&active[i], SUB_CHAR(state));
					if (state) {
						my_group_add (active[i].name);
/* TODO grp_mark_unread() or something needs to do a GrpGetArtInfo to get initial count right */
						grp_mark_unread (&active[i]);
					}
					subscribe_num++;
				}
			}
		}
	}

	if (subscribe_num) {
		toggle_my_groups (show_only_unread_groups, "");
		set_groupname_len (FALSE);
		show_selection_page ();
		info_message (result, subscribe_num);
	} else
		info_message (txt_no_match);
}
#endif /* !INDEX_DAEMON */


/*
 * Strip trailing blanks, \r and \n
 */
void
strip_line (
	char *line)
{
	char *ptr = line + strlen(line) - 1;

	while ((ptr >= line) && (*ptr == ' ' || *ptr == '\r' || *ptr == '\n'))
		ptr--;

	*++ptr = '\0';
}


/*
 * Allows user to specify an group/article range that a followup
 * command will operate on (eg. catchup articles 1-56) # 1-56 K
 *
 * Allowed syntax is 0123456789-.$ (blanks are ignored):
 *   1-23    mark grp/art 1 thru 23
 *   1-.     mark grp/art 1 thru current
 *   1-$     mark grp/art 1 thru last
 *   .-$     mark grp/art current thru last
 */
t_bool
bSetRange (
	int iLevel,
	int iNumMin,
	int iNumMax,
	int iNumCur)
{
	char *pcPtr;
	char acRng[PATH_LEN];
	int iIndex;
	int iNum;
	int iRngMin;
	int iRngMax;
	t_bool bRetCode = FALSE;

	switch (iLevel)
	{
		case SELECT_LEVEL:
			pcPtr = default_range_select;
			break;
		case GROUP_LEVEL:
			pcPtr = default_range_group;
			break;
		case THREAD_LEVEL:
			pcPtr = default_range_thread;
			break;
		default:
			return bRetCode;
	}
/*
	error_message ("Min=[%d] Max=[%d] Cur=[%d] DefRng=[%s]",
		iNumMin, iNumMax, iNumCur, pcPtr);
*/
	sprintf (mesg, txt_enter_range, pcPtr);

	if (!prompt_string (mesg, acRng, HIST_OTHER))
		return bRetCode;

	if (strlen (acRng))
		strcpy (pcPtr, acRng);
	else {
		if (*pcPtr)
			strcpy (acRng, pcPtr);
		else
			return bRetCode;
	}

	/*
	 * Parse range string
	 */
	if (!bParseRange (acRng, iNumMin, iNumMax, iNumCur, &iRngMin, &iRngMax))
		/* FIXME -> lang.c */
		info_message ("Invalid range - valid are '0-9.$' eg. 1-$");
	else {
/*
		info_message ("DefRng=[%s] NewRng=[%s] Min=[%d] Max=[%d]", pcPtr, acRng, iRngMin, iRngMax);
*/
		bRetCode = TRUE;
		switch (iLevel)
		{
			case SELECT_LEVEL:
				vDelRange (iLevel, iNumMax);
				for (iIndex = iRngMin-1; iIndex < iRngMax; iIndex++)
					active[my_group[iIndex]].inrange = TRUE;
				break;
			case GROUP_LEVEL:
				vDelRange (iLevel, iNumMax);
				for (iIndex = iRngMin-1; iIndex < iRngMax; iIndex++) {
					for (iNum = (int) base[iIndex]; iNum != -1; iNum = arts[iNum].thread)
						arts[iNum].inrange = TRUE;
				}
				break;
			case THREAD_LEVEL:
				vDelRange (iLevel, group_top);
				for (iNum = 0, iIndex = base[thread_basenote]; iIndex >= 0; iIndex = arts[iIndex].thread, iNum++) {
					if (iNum >= iRngMin && iNum <= iRngMax)
						arts[iIndex].inrange = TRUE;
				}
				break;
			default:
				bRetCode = FALSE;
				break;
		}
	}
	return bRetCode;
}


static t_bool
bParseRange (
	char *pcRange,
	int iNumMin,
	int iNumMax,
	int iNumCur,
	int *piRngMin,
	int *piRngMax)
{
	char *pcPtr;
	t_bool bRetCode = FALSE;
	t_bool bSetMax = FALSE;
	t_bool bDone = FALSE;

	pcPtr = pcRange;
	*piRngMin = -1;
	*piRngMax = -1;

	while (*pcPtr && !bDone) {
		if (*pcPtr >= '0' && *pcPtr <= '9') {
			if (bSetMax) {
				*piRngMax = atoi (pcPtr);
				bDone = TRUE;
			} else
				*piRngMin = atoi (pcPtr);
			while (*pcPtr >= '0' && *pcPtr <= '9')
				pcPtr++;
		} else {
			switch (*pcPtr) {
				case '-':
					bSetMax = TRUE;
					break;
				case '.':
					if (bSetMax) {
						*piRngMax = iNumCur;
						bDone = TRUE;
					} else
						*piRngMin = iNumCur;
					break;
				case '$':
					if (bSetMax) {
						*piRngMax = iNumMax;
						bDone = TRUE;
					}
					break;
				default:
					break;
			}
			pcPtr++;
		}
	}

	if (*piRngMin >= iNumMin && *piRngMax > iNumMin && *piRngMax <= iNumMax)
		bRetCode = TRUE;

	return bRetCode;
}


static void
vDelRange (
	int iLevel,
	int iNumMax)
{
	int iIndex;

	switch (iLevel)
	{
		case SELECT_LEVEL:
			for (iIndex = 0; iIndex < iNumMax-1; iIndex++)
				active[iIndex].inrange = FALSE;
			break;
		case GROUP_LEVEL:
			for (iIndex = 0; iIndex < iNumMax-1; iIndex++) {
				int iNum;

				for (iNum = (int) base[iIndex]; iNum != -1; iNum = arts[iNum].thread)
					arts[iNum].inrange = FALSE;
			}
			break;
		case THREAD_LEVEL:
			for (iIndex = 0; iIndex < iNumMax-1; iIndex++)
				arts[iIndex].inrange = FALSE;
/*
			for (iIndex = base[thread_basenote]; iIndex >= 0; iIndex = arts[iIndex].thread)
				arts[iIndex].inrange = FALSE;
*/
			break;
		default:
			break;
	}
}


#ifndef INDEX_DAEMON
/*
 * Move the on-screen pointer & internal variable to the given group
 */
static void
move_to_group(
	int n)
{
	if (cur_groupnum == n)
		return;

	HpGlitch(erase_group_arrow ());
	erase_group_arrow ();
	cur_groupnum = n;
	clear_message ();

	if (n >= first_group_on_screen && n < last_group_on_screen)
		draw_group_arrow ();
	else
		show_selection_page ();

	if (CURR_GROUP.aliasedto) /* FIXME -> lang.c */
		info_message ("Please use %.100s instead", CURR_GROUP.aliasedto);
}
#endif /* !INDEX_DAEMON */
