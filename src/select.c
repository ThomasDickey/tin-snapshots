/*
 *  Project   : tin - a Usenet reader
 *  Module    : select.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 21-12-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"menukeys.h"

extern int index_point;
extern int thread_basenote;

char default_goto_group[LEN];
int default_move_group;
int cur_groupnum = 0;
int first_group_on_screen;
int last_group_on_screen;
int space_mode;
int yank_in_active_file = TRUE;


void
selection_index (start_groupnum, num_cmd_line_groups)
	int start_groupnum;
	int num_cmd_line_groups;
{
#ifndef INDEX_DAEMON

	char buf[LEN];
	char post_group[LEN];
	int ch, i, n;
	int INDEX_BOTTOM;
	int posted_flag;
	int scroll_lines;
	int subscribe_num;
	
	cur_groupnum = start_groupnum;
	
#ifdef READ_CHAR_HACK
	setbuf (stdin, 0);
#endif

	ClearScreen();
	set_groupname_len (FALSE);	/* find longest subscribed to groupname */
	group_selection_page ();	/* display group selection page */
	set_alarm_signal ();		/* set alarm signal for resync_active_file () */

	/*
	 * If user specified on cmd line only 1 groupname (ie. tin alt.sources) 
	 * then enter the group immediately.  Don't scream at the use of a goto.
	 */
	if (num_cmd_line_groups == 1) {
		goto select_read_group;
	}

	forever {
		if (! resync_active_file ()) {	/* reread active file if alarm set */
			if (reread_active_after_posting ()) {
				group_selection_page ();
			}
		}
		set_xclick_on ();
		ch = ReadCh ();
		if (ch != iKeySelectQuit && ch != iKeySelectQuit2) {
			(void) resync_active_file ();
		}

		if (ch > '0' && ch <= '9') {
			(void) prompt_group_num (ch);
			continue;
		}
		switch (ch) {
#ifndef WIN32
			case ESC:	/* (ESC) common arrow keys */
#ifdef HAVE_KEY_PREFIX
			case KEY_PREFIX:
#endif
				switch (get_arrow_key ()) {
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
								if (xrow < INDEX_TOP || xrow >= INDEX_BOTTOM) {
									goto select_page_down;
								}
								erase_group_arrow ();
								cur_groupnum = xrow-INDEX_TOP+first_group_on_screen;
								draw_group_arrow ();
								if (xmouse == MOUSE_BUTTON_1)
									goto select_read_group;
								break;
							case MOUSE_BUTTON_2:
								if (xrow < INDEX_TOP || xrow >= INDEX_BOTTOM) {
									goto select_page_up;
								}
								goto select_done;
								/* break; */
						}
						break;
				}
				break;
#endif /* ! WIN32 */

#ifndef NO_SHELL_ESCAPE
			case iKeySelectShell:
				shell_escape ();
				group_selection_page ();
				break;
#endif

			case iKeySelectFirstPage:	/* show first page of groups */
top_of_list:
				if (cur_groupnum != 0) {
					if (0 < first_group_on_screen) {
						erase_group_arrow ();
						cur_groupnum = 0;
						group_selection_page ();
					} else {
						erase_group_arrow ();
						cur_groupnum = 0;
						draw_group_arrow ();
					}
				}
				break;
					
			case iKeySelectLastPage:	/* show last page of groups */
end_of_list:
				if (cur_groupnum != group_top - 1) {
					if (group_top - 1 > last_group_on_screen) {
						erase_group_arrow();
						cur_groupnum = group_top - 1;
						group_selection_page();
					} else {
						erase_group_arrow();
						cur_groupnum = group_top - 1;
						draw_group_arrow();
					}
				}
			    break;

			case iKeySelectSetRange:	/* set range */
				if (iSetRange (SELECT_LEVEL, 1, group_top, cur_groupnum+1)) {
						group_selection_page ();
				}
				break;

			case iKeySelectFSearchSubj:	/* search forward */
			case iKeySelectBSearchSubj:	/* search backward */
				i = (ch == iKeySelectFSearchSubj);
				search_group (i);
				break;

			case iKeySelectReadGrp:	/* go into group */
			case iKeySelectReadGrp2:
select_read_group:
				if (group_top == 0) {
					info_message (txt_no_groups);
					break;
				}
				
				n = my_group[cur_groupnum];
				if (active[n].xmin <= active[n].xmax) {
					space_mode = pos_first_unread;
					clear_message ();
					do {
						index_point = GRP_UNINDEXED;
						group_page (&CURR_GROUP);
					} while (index_point == GRP_GOTONEXT || index_point == GRP_CONTINUE);
					if (index_point == GRP_QUIT) {
						goto select_done;
					}
#ifndef DONT_REREAD_ACTIVE_FILE					
					if (! reread_active_file)
#endif
						group_selection_page ();
				} else {
					info_message (txt_no_arts);
				}
				break;

			case iKeySelectEnterNextUnreadGrp:	/* enter next group containing unread articles */
			case iKeySelectEnterNextUnreadGrp2:
				if (next_unread_group (TRUE) == GRP_QUIT) {
					goto select_done;
				}
				break;

			case iKeySelectPageDown:		/* page down */
			case iKeySelectPageDown2:
			case iKeySelectPageDown3:
select_page_down:
				if (! group_top) {
					break;
				}
				if (cur_groupnum == group_top - 1) {
					if (0 < first_group_on_screen) {
						erase_group_arrow();
						cur_groupnum = 0;
						group_selection_page();
					} else {
						erase_group_arrow();
						cur_groupnum = 0;
						draw_group_arrow();
					}
					break;
				}
				erase_group_arrow ();
				scroll_lines = (full_page_scroll ? NOTESLINES : NOTESLINES / 2);
				cur_groupnum = ((cur_groupnum + scroll_lines) / scroll_lines) * scroll_lines;
				if (cur_groupnum >= group_top) {
					cur_groupnum = (group_top / scroll_lines) * scroll_lines;
					if (cur_groupnum < group_top - 1) {
						cur_groupnum = group_top - 1;
					}
				}

				if (cur_groupnum <= first_group_on_screen
				||  cur_groupnum >= last_group_on_screen)
					group_selection_page ();
				else
					draw_group_arrow ();
				break;

			case iKeySelectRedrawScr:		/* redraw */
				ClearScreen ();
				set_xclick_off ();
				group_selection_page ();
				break;

			case iKeySelectDown:		/* line down */
			case iKeySelectDown2:
select_down:
				if (! group_top) {
					break;
				}
				if (cur_groupnum + 1 >= group_top) {
					if (0 < first_group_on_screen) {
						erase_group_arrow();
						cur_groupnum = 0;
						group_selection_page();
					} else {
						erase_group_arrow();
						cur_groupnum = 0;
						draw_group_arrow();
					}
					break;
				}
				if (cur_groupnum + 1 >= last_group_on_screen) {
					erase_group_arrow();
					cur_groupnum++;
					group_selection_page();
				} else {
					erase_group_arrow();
					cur_groupnum++;
					draw_group_arrow();
				}
				break;

			case iKeySelectUp:		/* line up */
			case iKeySelectUp2:
select_up:
				if (! group_top) {
					break;
				}
				if (cur_groupnum == 0) {
					if (_hp_glitch) {
						erase_group_arrow ();
					}
					if (group_top > last_group_on_screen) {
						cur_groupnum = group_top - 1;
						group_selection_page ();
					} else {
						erase_group_arrow ();
						cur_groupnum = group_top - 1;
						draw_group_arrow ();
					}
					break;
				}
				if (_hp_glitch) {
					erase_group_arrow ();
				}
				if (cur_groupnum <= first_group_on_screen) {
					cur_groupnum--;
					group_selection_page ();
				} else {
					erase_group_arrow ();
					cur_groupnum--;
					draw_group_arrow ();
				}
				break;

			case iKeySelectResetNewsrc:	/* reset .newsrc */
			    if (prompt_yn (cLINES, txt_reset_newsrc, FALSE) == 1) {
					reset_newsrc ();
					cur_groupnum = 0;
					group_selection_page ();
			    }
			    break;

			case iKeySelectPageUp:		/* page up */
			case iKeySelectPageUp2:
			case iKeySelectPageUp3:
select_page_up:
				if (! group_top) {
					break;
				}
				if (cur_groupnum == 0) {
					if (_hp_glitch) {
						erase_group_arrow ();
					}
					if (group_top > last_group_on_screen) {
						cur_groupnum = group_top - 1;
						group_selection_page ();
					} else {
						erase_group_arrow ();
						cur_groupnum = group_top - 1;
						draw_group_arrow ();
					}
					break;
				}
				erase_group_arrow ();
				scroll_lines = (full_page_scroll ? NOTESLINES : NOTESLINES / 2);
				if ((n = cur_groupnum % scroll_lines) > 0) {
					cur_groupnum = cur_groupnum - n;
				} else {
					cur_groupnum = ((cur_groupnum - scroll_lines) / scroll_lines) * scroll_lines;
				}
				if (cur_groupnum < 0) {
					cur_groupnum = 0;
				}
				if (cur_groupnum < first_group_on_screen
				||  cur_groupnum >= last_group_on_screen)
					group_selection_page ();
				else
					draw_group_arrow ();
				break;

			case iKeySelectCatchup:	/* catchup - mark all articles as read */
			case iKeySelectCatchupGotoNext:	/* and goto next unread group */
				if (group_top == 0) {
					break;
				}
				catchup_group (&CURR_GROUP, (ch == iKeySelectCatchupGotoNext));
			    break;

			case iKeySelectToggleSubjDisplay:	/* toggle newsgroup descriptions */
				show_description = !show_description;
				if (show_description) {
					read_newsgroups_file ();
				}
				set_groupname_len (FALSE);
				group_selection_page ();
				break;

			case iKeySelectGoto:	/* prompt for a new group name */
				n = choose_new_group ();
				if (n >= 0) {
					erase_group_arrow ();
					/* why differentiate between subscribed
					   and unsubscribed groups here?
					   auto-subscribe does not seem to be
					   intuitive! */
/*					if (active[my_group[n]].subscribed != SUBSCRIBED) {
						subscribe (&active[my_group[n]], SUBSCRIBED);
						cur_groupnum = reposition_group (
							&active[my_group[n]], (n ? n : cur_groupnum));
						toggle_my_groups (show_only_unread_groups, "");
					} else  { */
						cur_groupnum = n;
/*					} */
					set_groupname_len (FALSE);				
					if (cur_groupnum < first_group_on_screen ||
						cur_groupnum >= last_group_on_screen ||
						cur_groupnum != n) {
						group_selection_page();
					} else {
						clear_message ();
						draw_group_arrow();
					}
				}
				break;

			case iKeySelectHelp:	/* help */
				show_info_page (HELP_INFO, help_select, txt_group_select_com);
				group_selection_page ();
				break;

			case iKeySelectToggleHelpDisplay:	/* toggle mini help menu */
				toggle_mini_help (SELECT_LEVEL);
				group_selection_page ();
				break;

			case iKeySelectToggleInverseVideo:	/* toggle inverse video */
				toggle_inverse_video ();
				group_selection_page ();
				show_inverse_video_status ();
				break;

#ifdef HAVE_COLOR
			case iKeySelectToggleColor:		/* toggle color */
				toggle_color ();
				group_selection_page ();
				show_color_status ();
				break;
#endif

			case iKeySelectDisplayGroupInfo:	/* display group description */
				if (group_top == 0) {
					info_message (txt_no_groups);
				} else {
					info_message (
						(CURR_GROUP.description ?  CURR_GROUP.description :
					 	"*** No description ***"));
				}
				break;

			case iKeySelectMoveGrp:	/* reposition group within group list */
				if (CURR_GROUP.subscribed == SUBSCRIBED) {
					n = cur_groupnum;
					cur_groupnum = reposition_group (&active[my_group[n]], n);
					if (_hp_glitch) {
						erase_group_arrow ();
					}
					if (cur_groupnum < first_group_on_screen ||
						cur_groupnum >= last_group_on_screen ||
						cur_groupnum != n) {
						group_selection_page ();
					} else {
						i = cur_groupnum;
						cur_groupnum = n;
						erase_group_arrow ();
						cur_groupnum = i;
						clear_message ();
						draw_group_arrow ();
					}
				}
				break;

			case iKeySelectOptionsMenu:	/* options menu */
				set_alarm_clock_off ();
				change_config_file (NULL, TRUE);
				free_attributes_array ();
				read_attributes_file (global_attributes_file, TRUE);
				read_attributes_file (local_attributes_file, FALSE);
				group_selection_page ();
				set_alarm_clock_on ();
				break;

			case iKeySelectNextUnreadGrp:	/* goto next unread group */
				if (next_unread_group (FALSE) == GRP_QUIT) {
					goto select_done;
				}
				break;

			case iKeySelectQuit:	/* quit */
select_done:
				if (! confirm_to_quit || prompt_yn (cLINES, txt_quit, TRUE) == 1) {
					write_config_file (local_config_file);
					tin_done (EXIT_OK);
				}
				group_selection_page ();
				break;

			case iKeySelectQuit2:	/* quit, no ask */
				write_config_file (local_config_file);
				tin_done (EXIT_OK);
                                break;
                                
			case iKeySelectQuitNoWrite:	/* quit, but don't save configuration */
				if (prompt_yn (cLINES, txt_quit_no_write, TRUE)==1) {
					tin_done (EXIT_OK);
				}
				group_selection_page ();
				break;
				
			case iKeySelectToggleReadDisplay:
	 			/* 
	 			 * If in show_only_unread_groups mode toggle
	 			 * all subscribed to groups and only  groups
	 			 * that contain unread articles
	 			 *
	 			 * Disabled when started with cmdline groups
	 			 */
	 			if (! read_cmd_line_groups ()) { 
	 				show_only_unread_groups = !show_only_unread_groups;
					if (show_only_unread_groups) {
						wait_message (txt_reading_new_groups);
					} else {
						wait_message (txt_reading_all_groups);
					}
					toggle_my_groups (show_only_unread_groups, "");
					set_groupname_len (FALSE);
					group_selection_page ();
				}
				break;

			case iKeySelectBugReport:	/* bug/gripe/comment mailed to author */
				mail_bug_report ();
				ClearScreen ();
				group_selection_page ();
				break;

			case iKeySelectSubscribe:	/* subscribe to current group */
				if (group_top == 0) {
					break;
				}
				if (CURR_GROUP.subscribed != SUBSCRIBED) {
					mark_screen (SELECT_LEVEL, cur_groupnum - first_group_on_screen, 2, " ");
					subscribe (&CURR_GROUP, SUBSCRIBED);
					sprintf (buf, txt_subscribed_to, CURR_GROUP.name);
					info_message (buf);
				}
			    break;

			case iKeySelectSubscribePat:	/* subscribe to groups matching pattern */
				/* If no groups in active[] then break otherwise loop thru looking
				   for matches. If found and group is not subscribed add it to end
				   of my_group[]. */
				if (num_active == 0) {
					break;
				}
				if (prompt_string (txt_subscribe_pattern, buf) && buf[0]) {
					wait_message (txt_subscribing);
					for (subscribe_num=0, i=0 ; i < group_top ; i++) {
						if (wildmat (active[my_group[i]].name, buf)) {
			   		 		if (active[my_group[i]].subscribed != SUBSCRIBED) {
								spin_cursor ();
								subscribe (&active[my_group[i]], SUBSCRIBED);
								subscribe_num++;
							}
						}
					}
					if (num_active > group_top) {
						for (i=0 ; i < num_active ; i++) {
							if (wildmat (active[i].name, buf)) {
				   		 		if (active[i].subscribed != SUBSCRIBED) {
									spin_cursor ();
									subscribe (&active[i], SUBSCRIBED);
									(void) add_my_group (active[i].name, 1);
									grp_mark_unread (&active[i]);
									subscribe_num++;
								}
							}
						}
					}
					if (subscribe_num) {
						toggle_my_groups (show_only_unread_groups, "");
						set_groupname_len (FALSE);
						group_selection_page ();	
						sprintf (buf, txt_subscribed_num_groups, subscribe_num);
						info_message (buf);
					} else {
						info_message (txt_no_match);
					}
				} else {
					clear_message ();
				}
			    break;

			case iKeySelectUnsubscribe:	/* unsubscribe to current group */
				if (group_top == 0) {
					break;
				}
				if (CURR_GROUP.subscribed == SUBSCRIBED) {
					mark_screen (SELECT_LEVEL, cur_groupnum - first_group_on_screen, 2, "u");
					subscribe (&CURR_GROUP, '!');
					sprintf(buf, txt_unsubscribed_to, CURR_GROUP.name);
					info_message(buf);
				}
				goto_next_group_on_screen ();
				break;

			case iKeySelectUnsubscribePat:	/* unsubscribe to groups matching pattern */
				if (num_active == 0) {
					break;
				}
				if (prompt_string (txt_unsubscribe_pattern, buf) && buf[0]) {	
					wait_message (txt_unsubscribing);
					for (subscribe_num=0, i=0 ; i < group_top ; i++) {		
						if (wildmat (active[my_group[i]].name, buf)) {
			   		 		if (active[my_group[i]].subscribed == SUBSCRIBED) {
								spin_cursor ();
								subscribe (&active[my_group[i]], UNSUBSCRIBED);
								subscribe_num++;
							}
						}
					}
					if (num_active > group_top) {
						for (i=0 ; i < num_active ; i++) {
							if (wildmat (active[i].name, buf)) {
				   		 		if (active[i].subscribed == SUBSCRIBED) {
									spin_cursor ();
									subscribe (&active[i], UNSUBSCRIBED);
									subscribe_num++;
								}
							}
						}
					}
					if (subscribe_num) {
						toggle_my_groups (show_only_unread_groups, "");
						set_groupname_len (FALSE);
						group_selection_page ();	
						sprintf (buf, txt_unsubscribed_num_groups, subscribe_num);
						info_message (buf);
					} else {
						info_message (txt_no_match);
					}
				} else {
					clear_message ();
				}
			    break;

			case iKeySelectVersion:	/* show tin version */
				info_message (cvers);
				break;

			case iKeySelectPost:	/* post a basenote */
				if (can_post) {
					if (group_top == 0) {
						if (! prompt_string (txt_post_newsgroup, buf)) 
							break;
						if (buf[0] == '\0')
							break;
						strcpy (post_group, buf);
						if (find_group_index (post_group) == -1) {
							error_message (txt_not_in_active_file, post_group);
							break;
						}
					} else {
						strcpy (post_group, CURR_GROUP.name);
					}
					if (post_article (post_group, &posted_flag)) {
						group_selection_page ();
					}
				}
				break;

			case iKeySelectPostHist:	/* display messages posted by user */
				if (user_posted_messages ()) {
					group_selection_page ();
				}
				break;

			case iKeySelectYankActive:	/* pull in rest of groups from active */
				if (yank_in_active_file) {
					wait_message (txt_yanking_all_groups);
					set_alarm_clock_off ();
					n = group_top;
					if (group_top) {
						strcpy (buf, CURR_GROUP.name);
					}
					vWriteNewsrc ();
					group_top = 0;
					for (i = 0; i < num_active; i++) {
						(void) add_my_group (active[i].name, 1);
					}
					if (n < group_top) {
						if (n) {
							cur_groupnum = add_my_group (buf, 1);
						}
						sprintf (buf, txt_added_groups, group_top - n,
							group_top - n == 1 ? "" : txt_plural);
						set_groupname_len (yank_in_active_file);
						group_selection_page ();
						info_message (buf);
					} else {
					    info_message (txt_no_groups_to_yank_in);
					}
					yank_in_active_file = FALSE;
				} else {
					wait_message (txt_yanking_sub_groups);
					vWriteNewsrc ();
					read_newsrc (newsrc, 1);
					if (_hp_glitch) {
						erase_group_arrow ();
					}					
					cur_groupnum = group_top - 1;
					set_groupname_len (yank_in_active_file);
					group_selection_page ();
					yank_in_active_file = TRUE;
					set_alarm_clock_on ();
				}
				break;

			case iKeySelectSyncWithActive:	/* yank active file to see if any new news */
				yank_active_file ();
				break;

			case iKeySelectMarkGrpUnread:	/* mark group unread */
				if (group_top == 0) {
					break;
				}
				grp_mark_unread (&CURR_GROUP);
				if (CURR_GROUP.newsrc.num_unread) {
					sprintf (msg, "%5ld", CURR_GROUP.newsrc.num_unread);
				} else {
					strcpy (msg, "     ");
				}
				mark_screen (SELECT_LEVEL, cur_groupnum - first_group_on_screen, 9, msg);
				break;

			default:
			    info_message(txt_bad_command);
		}
	}

#endif /* INDEX_DAEMON */
}


void 
group_selection_page ()
{
#ifndef INDEX_DAEMON

	char buf[LEN];
	char new[10];
	char subs;
	int i, j, n;
	int blank_len;

	set_signals_select ();

	MoveCursor (0, 0);		/* top left corner */
	CleartoEOLN ();

	if (read_news_via_nntp) {
		sprintf (buf, "%s (%s  %d%s)", txt_group_selection, 
			nntp_server, group_top, (show_only_unread_groups ? " R" : ""));
	} else {
		sprintf (buf, "%s (%d%s)", txt_group_selection, group_top,
			(show_only_unread_groups ? " R" : ""));
	}	
	show_title (buf);

	MoveCursor (1, 0);
	CleartoEOLN ();
	MoveCursor (INDEX_TOP, 0);

	if (cur_groupnum >= group_top) {
		cur_groupnum = group_top - 1;
	}
	if (cur_groupnum < 0) {
		cur_groupnum = 0;
	}
 
	if (NOTESLINES <= 0) {
		first_group_on_screen = 0;
	} else {
		first_group_on_screen = (cur_groupnum / NOTESLINES) * NOTESLINES;
		if (first_group_on_screen < 0) {
			first_group_on_screen = 0;
		}
	}

	last_group_on_screen = first_group_on_screen + NOTESLINES;

	if (last_group_on_screen >= group_top) {
		last_group_on_screen = group_top;
		first_group_on_screen = (cur_groupnum / NOTESLINES) * NOTESLINES;

		if (first_group_on_screen == last_group_on_screen ||
			first_group_on_screen < 0) {
			if (first_group_on_screen < 0) {
				first_group_on_screen = 0;
			} else {
				first_group_on_screen = last_group_on_screen - NOTESLINES;	
			}
		}	
	}

	if (group_top == 0) {
		first_group_on_screen = 0;
		last_group_on_screen = 0;
	}

	if (show_description) {
		blank_len = (cCOLS - (groupname_len + SELECT_MISC_COLS)) + 2;
	} else {
		blank_len = (cCOLS - (groupname_len + SELECT_MISC_COLS)) + 4;
	}
	
	for (j=0, i=first_group_on_screen; i < last_group_on_screen; i++, j++) {
		if (active[my_group[i]].inrange) {
				strcpy (new, "    #");
		} else if (active[my_group[i]].newsrc.num_unread) {
				sprintf (new, "%5ld", active[my_group[i]].newsrc.num_unread);
		} else {
				strcpy (new, "     ");
		}
		
		n = my_group[i];
		if (active[n].subscribed == SUBSCRIBED) {
			subs = ' ';
		} else {
			subs = 'u';	/* u next to unsubscribed groups */
		}

		if (show_description) {		
			if (draw_arrow_mark) {
				sprintf (screen[j].col, "  %c %4d %s  %-*.*s  %-*.*s\r\n",
				       subs, i+1, new, groupname_len, groupname_len, 
				       active[n].name, blank_len, blank_len, 
				       (active[n].description ? active[n].description : " "));
			} else {
				sprintf (screen[j].col, "  %c %4d %s  %-*.*s  %-*.*s\r\n",
					subs, i+1, new, groupname_len, groupname_len, 
					active[n].name, blank_len, blank_len, 
				       (active[n].description ? active[n].description : " "));
			}
		} else {
			if (draw_arrow_mark) {
				sprintf (screen[j].col, "  %c %4d %s  %-*.*s\r\n",
					subs, i+1, new, groupname_len, groupname_len, active[n].name);
			} else {
				sprintf (screen[j].col, "  %c %4d %s  %-*.*s%*s\r\n",
					subs, i+1, new, groupname_len, groupname_len, active[n].name,
					blank_len, " ");
			}
		}
		if (strip_blanks) {
			strip_line (screen[j].col, strlen (screen[j].col));
			strcat (screen[j].col, "\r\n");
		}
 		CleartoEOLN ();
 		my_fputs (screen[j].col, stdout);
	}

	CleartoEOS ();
	show_mini_help (SELECT_LEVEL);

	if (group_top <= 0) {
		info_message (txt_no_groups);
		return;
	} else if (last_group_on_screen == group_top) {
		info_message (txt_end_of_groups);
	}
	
	draw_group_arrow ();

#endif /* INDEX_DAEMON */
}


int 
prompt_group_num (ch)
	int ch;
{
	int num;

	clear_message ();

	if ((num = prompt_num (ch, txt_select_group)) == -1) {
		clear_message ();
		return FALSE;
	}
	num--;		/* index from 0 (internal) vs. 1 (user) */

	if (num < 0) {
		num = 0;
	}
	if (num >= group_top) {
		num = group_top - 1;
	}

	if (num >= first_group_on_screen
	&&  num < last_group_on_screen) {
		erase_group_arrow ();
		cur_groupnum = num;
		draw_group_arrow ();
	} else {
		erase_group_arrow ();
		cur_groupnum = num;
		group_selection_page ();
	}

	return TRUE;
}


void 
erase_group_arrow ()
{
	erase_arrow (INDEX_TOP + (cur_groupnum-first_group_on_screen));
}


void 
draw_group_arrow ()
{
	draw_arrow (INDEX_TOP + (cur_groupnum-first_group_on_screen));
}


void 
yank_active_file ()
{
	reread_active_file = TRUE;
	resync_active_file ();
}


int 
choose_new_group ()
{
	char buf[LEN];
	char *p;
	int idx;

	if (! group_top && show_only_unread_groups) {
		return -1;
	}
	
	sprintf (msg, txt_newsgroup, default_goto_group);

	if (! prompt_string (msg, buf)) {
		return -1;
	}
	
	if (strlen (buf)) {
		strcpy (default_goto_group, buf);
	} else {
		if (default_goto_group[0]) {
			strcpy (buf, default_goto_group);
		} else {
			return -1;
		}
	}

	for (p = buf; *p && (*p == ' ' || *p == '\t'); p++) {
		continue;
	}
	if (*p == '\0') {
		return -1;
	}

	clear_message ();

	idx = add_my_group (p, 1);
	if (idx == -1) {
		sprintf (msg, txt_not_in_active_file, p);
		info_message (msg);
	}

	return idx;
}

/*
 *  Add a group to the users selection list (my_group[])
 *  Return the index of my_group[] if group is added or was already
 *  there.  Return -1 if named group is not in active[].
 */

int 
add_my_group (group, add)
	char *group;
	int add;
{
	int i,j;

	i = find_group_index (group);
	if (i >= 0) {
		for (j = 0; j < group_top; j++) {
			if (my_group[j] == i) {
				return j;
			}
		}
		if (add) {
			my_group[group_top++] = i;
			return (group_top - 1);
		}
	}
	return -1;
}

int 
reposition_group (group, default_num)
	struct t_group *group;
	int default_num;
{
	char buf[LEN];
	char pos[LEN];
	int pos_num;

	sprintf (buf, txt_newsgroup_position, group->name, 
		(default_move_group ? default_move_group : default_num+1));
	
	if (! prompt_string (buf, pos)) {
		return default_num;
	}	

	if (strlen (pos)) {
		if (pos[0] == '$') {
			pos_num = group_top;
		} else {
			pos_num = atoi (pos);
		}	
	} else {
		if (default_move_group) {
			pos_num = default_move_group;
		} else {
			return default_num;
		}
	}
		
	if (pos_num > group_top) {
		pos_num = group_top;
	} else if (pos_num <= 0) {
		pos_num = 1;
	}

	sprintf (buf, txt_moving, group->name);
	wait_message (buf);

	/* seems to have the side effect of rearranging
	   my_groups, so show_only_unread_groups has to be updated */
	show_only_unread_groups = FALSE;	
	if (pos_group_in_newsrc (group, pos_num)) {
		read_newsrc (newsrc, 1);
		default_move_group = pos_num;
		return (pos_num-1);
	} else {
		default_move_group = default_num + 1;
		return (default_num);
	}
}


void 
catchup_group (group, goto_next_unread_group)
	struct t_group *group;
	int goto_next_unread_group;
{	
	sprintf (msg, txt_mark_group_read, group->name);

	if (! confirm_action || prompt_yn (cLINES, msg, TRUE) == 1) {
		grp_mark_read (group, NULL);

		mark_screen (SELECT_LEVEL, cur_groupnum - first_group_on_screen, 
			9, "     ");

		goto_next_group_on_screen ();
		
		if (goto_next_unread_group) {
			next_unread_group (FALSE);	
		}
	}
}


int 
next_unread_group (enter_group)
	int enter_group;
{
	int i, all_groups_read = TRUE;

	for (i = cur_groupnum ; i < group_top ; i++) {
		if (active[my_group[i]].newsrc.num_unread != 0) {
			all_groups_read = FALSE;
			erase_group_arrow ();
			break;
		}
	}

	if (all_groups_read) {
		for (i = 0 ; i < cur_groupnum ; i++) {
			if (active[my_group[i]].newsrc.num_unread != 0) {
				all_groups_read = FALSE;
				break;
			}
		}
	}
	
	if (all_groups_read) {
		info_message (txt_no_groups_to_read);
		return GRP_UNINDEXED;
	}

	if (i != cur_groupnum) {
		erase_group_arrow ();
	}
	cur_groupnum = i;
	if (cur_groupnum < first_group_on_screen ||
	    cur_groupnum >= last_group_on_screen) {
		group_selection_page ();
	} else {
		draw_group_arrow ();
	}
	space_mode = pos_first_unread;

	if (enter_group) {
		clear_message ();
		do {
			index_point = GRP_UNINDEXED;
			group_page (&CURR_GROUP);
		} while (index_point == GRP_GOTONEXT || index_point == GRP_CONTINUE);
		if (index_point == GRP_QUIT) {
			return GRP_QUIT;
		}
		group_selection_page ();
	}

	return GRP_CONTINUE;
}

/*
 *  Calculate max length of groupname field for group selection level.
 *  If all_group is TRUE check all groups in active file otherwise
 *  just subscribed to groups.
 */
 
void 
set_groupname_len (all_groups)
	int all_groups;
{
	int len;
	register int i;

	groupname_len = 0;
	
	if (all_groups) {
		for (i = 0 ; i < num_active ; i++) {
			len = strlen (active[i].name);
			if (len > groupname_len) {
				groupname_len = len;
			}
		}
	} else {
		for (i = 0 ; i < group_top ; i++) {
			len = strlen (active[my_group[i]].name);
			if (len > groupname_len) {
				groupname_len = len;
			}
		}
	}

	if (groupname_len >= (cCOLS - SELECT_MISC_COLS)) {
		groupname_len = cCOLS - SELECT_MISC_COLS - 1;
		if (groupname_len < 0) {
			groupname_len = 0;
		}	
	}
	
	/*
	 * If newsgroups descriptions are ON then cut off groupnames
	 * to specified max. length otherwise display full length
	 */
	if (show_description && groupname_len > groupname_max_length) {
		groupname_len = groupname_max_length;
	}
}


void 
toggle_my_groups (only_unread_groups, group)
	int only_unread_groups;
	char *group;
{
#ifndef INDEX_DAEMON
	char buf[NEWSRC_LINE];
	char old_curr_group[PATH_LEN];
	char *ptr;
	FILE *fp;
	int active_idx = 0;
	int group_num = cur_groupnum;
	register int i, j;

	if ((fp = fopen (newsrc, "r")) != (FILE *) 0) {
		/*
		 * Save current or next group with unread arts for later use
		 */
		old_curr_group[0] = '\0';

		if (group_top) {
			if (! only_unread_groups || reread_active_file) {
				if (strlen (group)) {
					if ((i = find_group_index (group)) >= 0) {
						active_idx = i;
					}
				} else {
					active_idx = my_group[group_num];
				}
				my_strncpy (old_curr_group, active[active_idx].name, 
					sizeof (old_curr_group));
			} else {
				for (i = group_num ; i < group_top ; i++) {
					if (active[my_group[i]].newsrc.num_unread) {
						my_strncpy (old_curr_group, active[my_group[i]].name, 
							sizeof (old_curr_group));
						break;
					}
				}
			}
		}

		group_top = 0;
		while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
			ptr = (char *) strchr (buf, ':');
			if (ptr != (char *) 0) {
				*ptr = '\0';
				if ((i = find_group_index (buf)) >= 0) {
					if (only_unread_groups) {					
						if (active[i].newsrc.num_unread) {
							my_group[group_top] = i;
							group_top++;
						}
					} else {
						my_group[group_top] = i;
						group_top++;
					}	
				}
			}
		}		
		fclose (fp);

		/*
		 * Try and reposition on same or next group before toggling 
		 */
		cur_groupnum = 0;
		
		i = find_group_index (old_curr_group);
		if (i >= 0) {
			for (j = 0 ; j < group_top ; j++) {
				if (my_group[j] == i) {
					cur_groupnum = j;
					break;
				}
			}
		}
	}
#endif	/* INDEX_DAEMON */
}


void
goto_next_group_on_screen ()
{
	if (_hp_glitch) {
		erase_group_arrow ();
	}

	if (cur_groupnum+1 < last_group_on_screen) {
		erase_group_arrow ();
		cur_groupnum++;
		draw_group_arrow ();
	} else {
		cur_groupnum++;
		group_selection_page ();
	}
}

/*
 * Strip trailing blanks
 */

void 
strip_line (line, len)
	char *line;
	size_t len;
{
	char *ptr = line + (len - 1);
	
	while ((ptr >= line) && (*ptr == ' ' || *ptr == '\r' || *ptr == '\n')) {
		ptr--;
	}	
	*++ptr = '\0';
}

/*
 * Allows user to specify an group/article range that a followup
 * command will operate on (ie. catchup articles 1-56) # 1-56 K
 *
 * Allowed syntax is 0123456789-.$ (blanks are ignored):
 *   1-23    mark grp/art 1 thru 23
 *   1-.     mark grp/art 1 thru current
 *   1-$     mark grp/art 1 thru last
 *   .-$     mark grp/art current thru last
 */
 
int
iSetRange (iLevel, iNumMin, iNumMax, iNumCur)
	int iLevel;
	int iNumMin;
	int iNumMax;
	int iNumCur;
{
	char *pcPtr;
	char acRng[PATH_LEN];
	int iIndex;
	int iNum;
	int iRetCode = FALSE;
	int iRngMin;
	int iRngMax;

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
			pcPtr = "";
	}
/*
	sprintf (acRng, "Min=[%d] Max=[%d] Cur=[%d] DefRng=[%s]",
		iNumMin, iNumMax, iNumCur, pcPtr);
	error_message (acRng, "");
*/	
	sprintf (msg, txt_enter_range, pcPtr);

	if (! prompt_string (msg, acRng)) {
		return iRetCode;
	}
	
	if (strlen (acRng)) {
		strcpy (pcPtr, acRng);
	} else {
		if (*pcPtr) {
			strcpy (acRng, pcPtr);
		} else {
			return iRetCode;
		}
	}
	
	/*
	 * Parse range string
	 */
	if (! iParseRange (acRng, iNumMin, iNumMax, iNumCur, &iRngMin, &iRngMax)) {
		info_message ("Invalid range - valid are '0-9.$' ie. 1-$");	
	} else {
/*
		sprintf (msg, "DefRng=[%s] NewRng=[%s] Min=[%d] Max=[%d]", 
			pcPtr, acRng, iRngMin, iRngMax);
		info_message (msg);
*/
		iRetCode = TRUE;
		switch (iLevel)
		{
			case SELECT_LEVEL:
				vDelRange (iLevel, iNumMax);
				for (iIndex = iRngMin-1; iIndex < iRngMax; iIndex++) {
					active[my_group[iIndex]].inrange = TRUE;
				}
				break;
			case GROUP_LEVEL:
				vDelRange (iLevel, iNumMax);
				for (iIndex = iRngMin-1; iIndex < iRngMax; iIndex++) {
					for (iNum = (int) base[iIndex] ; iNum != -1 ; iNum = arts[iNum].thread) {
						arts[iNum].inrange = TRUE;
					}
				}
				break;
			case THREAD_LEVEL:
				vDelRange (iLevel, group_top);
				for (iNum = 0, iIndex = base[thread_basenote]; iIndex >= 0 ; iIndex = arts[iIndex].thread, iNum++) {
					if (iNum >= iRngMin && iNum <= iRngMax) {
						arts[iIndex].inrange = TRUE;
					}
				}
				break;
			default:
				iRetCode = FALSE;
				break;
		}
	}

	return iRetCode;
}

int
iParseRange (pcRange, iNumMin, iNumMax, iNumCur, piRngMin, piRngMax)
	char *pcRange;
	int iNumMin;
	int iNumMax;
	int iNumCur;
	int *piRngMin;
	int *piRngMax;
{
	char *pcPtr;
	int iDone = FALSE;
	int iRetCode = FALSE;
	int iSetMax = FALSE;

	pcPtr = pcRange;
	*piRngMin = -1;
	*piRngMax = -1;
	
	while (*pcPtr && ! iDone) {
		if (*pcPtr >= '0' && *pcPtr <= '9') {
			if (iSetMax) {
	    			*piRngMax = atoi (pcPtr);
				iDone = TRUE;
	    		} else {
	    			*piRngMin = atoi (pcPtr);
	    		}
			while (*pcPtr >= '0' && *pcPtr <= '9') {
				pcPtr++;
			}
		} else {
			switch (*pcPtr) {
				case '-':
					iSetMax = TRUE;
					break;
			    	case '.':
					if (iSetMax) {
			    			*piRngMax = iNumCur;
						iDone = TRUE;
			    		} else {
			    			*piRngMin = iNumCur;
			    		}
					break;
			    	case '$':
					if (iSetMax) {
			    			*piRngMax = iNumMax;
						iDone = TRUE;
			    		}
					break;
				default:
					break;
			}
			pcPtr++;
		}
	}
	
	if (*piRngMin >= iNumMin && *piRngMax > iNumMin && *piRngMax <= iNumMax) {
		iRetCode = TRUE;
	}
	
	return iRetCode;
}

void
vDelRange (iLevel, iNumMax)
	int iLevel;
	int iNumMax;
{
	int iIndex;
	
	switch (iLevel)
	{
		case SELECT_LEVEL:
			for (iIndex = 0; iIndex < iNumMax-1; iIndex++) {
				active[iIndex].inrange = FALSE;
			}
			break;
		case GROUP_LEVEL:
			for (iIndex = 0; iIndex < iNumMax-1; iIndex++) {
				arts[iIndex].inrange = FALSE;
			}
			break;
		case THREAD_LEVEL:
			for (iIndex = 0; iIndex < iNumMax-1; iIndex++) {
				arts[iIndex].inrange = FALSE;
			}
/*
			for (iIndex = base[thread_basenote]; iIndex >= 0 ; iIndex = arts[iIndex].thread) {
				arts[iIndex].inrange = FALSE;
				}
*/
			break;
		default:
			break;
	}
}

