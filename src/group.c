/*
 *  Project   : tin - a Usenet reader
 *  Module    : group.c
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

#define MARK_OFFSET	8
#define CURR_GROUP	(active[my_group[cur_groupnum]])

/* what we do here is bizarre */
/*
#ifndef ART_ADJUST	
#define ART_ADJUST(n)	(CURR_GROUP.attribute->show_only_unread \
				? ((n) > 1 ? (n) : 0) \
				: ((n) > 0 ? (n) - 1 : 0))
#endif
*/

#define INDEX2SNUM(i)	((i) % NOTESLINES)
#define SNUM2LNUM(i)	(INDEX_TOP + (i))
#define INDEX2LNUM(i)	(SNUM2LNUM(INDEX2SNUM(i)))

extern char proc_ch_type;	/* feed.c */
extern int last_resp;		/* page.c */
extern int this_resp;		/* page.c */
extern int note_page;		/* page.c */

char *glob_group;
int index_point;
int first_subj_on_screen;
int last_subj_on_screen;
static int len_from;
static int len_subj;
static char *spaces = "XXXX";

static int line_is_tagged P_((int n));
static void show_tagged_lines P_((void));
static int bld_sline P_((int i));
static int draw_sline P_((int i, int full));


static int
line_is_tagged (n)
int n;
{
	int	code = 0;
	if (CURR_GROUP.attribute->thread_arts) {
		register int i;
		for (i = n; i >= 0; i = arts[i].thread) {
			if (arts[i].tagged > code)
				code = arts[i].tagged, code;
		}
	} else {
		code = arts[n].tagged;
	}
	return code;
}

static void
show_tagged_lines ()
{
	register int i;
	for (i = first_subj_on_screen; i < last_subj_on_screen; ++i) {
		if ((i != index_point) && line_is_tagged(base[i])) {
			bld_sline (i);
			draw_sline (i, FALSE);
		}
	}
}

void 
decr_tagged (tag)
	int tag;
{
	int i, j;

	for (i = 0; i < top_base; ++i) {
		for (j = (int) base[i] ; j != -1 ; j = arts[j].thread)
			if (arts[j].tagged > tag)
				--arts[j].tagged;
	}
}


void 
group_page (group)
	struct t_group *group;
{
#ifndef INDEX_DAEMON

	char group_path[LEN];
 	char buf[128];
 	char pat[128];
	int ch;
	int dummy = 0;
	int flag, i;
	int n;
	int filter_state;
	int old_top = 0;
	int old_selected_arts;
	int posted;
	int scroll_lines;
	long old_artnum = 0L;
	int xflag = 0;
 	struct t_art_stat sbuf;
	int old_group_top;

	/*
	 * Set the group attributes 
	 */
	group->read_during_session = TRUE;
	show_author = group->attribute->show_author;

 	proc_ch_default = get_post_proc_type (group->attribute->post_proc_type);

	glob_group = group->name;
	num_of_tagged_arts = 0;
	
	make_group_path (group->name, group_path);

	last_resp = -1;
	this_resp = -1;
	
	/* 
	 * update index file. quit group level if user aborts indexing
	 */
	if (! index_group (group)) {
		return;
	}	
	
	if (space_mode) {
		for (i = 0; i < top_base; i++) {
			if (new_responses (i)) {
				break;
			}
		}
		if (i < top_base) {
			index_point = i;
		} else {
			index_point = top_base - 1;
		}
	} else {
		index_point = top_base - 1;
	}
	
	set_subj_from_size (cCOLS);
	clear_note_area ();

	if (group->attribute->auto_select) {
		error_message ("Auto selecting articles (use 'X' to see all unread)...", "");
		goto do_auto_select_arts;	/* 'X' command */
	}
	
	show_group_page ();

debug_print_comment ("group.c: before while(1) loop...");
debug_print_bitmap (group, NULL);

	forever {
		set_xclick_on ();
		ch = ReadCh ();

		if (ch > '0' && ch <= '9') {	/* 0 goes to basenote */
			(void) prompt_subject_num (ch, group->name);
			continue;
		} 
		switch (ch) {
#ifndef WIN32
			case ESC:	/* common arrow keys */ 	
#ifdef HAVE_KEY_PREFIX
			case KEY_PREFIX:
#endif
				switch (get_arrow_key ()) {
#endif /* WIN32 */
				case KEYMAP_UP:
					goto group_up;

				case KEYMAP_DOWN:
					goto group_down;

				case KEYMAP_LEFT:
					if (group_catchup_on_exit) {
						goto group_catchup;
					} else {
						goto group_done;
					}

				case KEYMAP_RIGHT:
					if (auto_list_thread && index_point >= 0 &&
					    num_of_responses (index_point)) {
						goto group_list_thread;
					} else {
						goto group_read_basenote;
					}

				case KEYMAP_PAGE_UP:
					goto group_page_up;

				case KEYMAP_PAGE_DOWN:
					goto group_page_down;

				case KEYMAP_HOME:
					goto top_of_list;
					
				case KEYMAP_END:
					goto end_of_list;
#ifndef WIN32
				case KEYMAP_MOUSE:
					switch (xmouse)
					{
						case MOUSE_BUTTON_1:
						case MOUSE_BUTTON_3:
							if (xrow < INDEX2LNUM(first_subj_on_screen) ||
								xrow > INDEX2LNUM(last_subj_on_screen-1)) {
								goto group_page_down;
							}
							erase_subject_arrow ();
							index_point = xrow-INDEX2LNUM(first_subj_on_screen)+first_subj_on_screen;
							draw_subject_arrow ();
							if (xmouse == MOUSE_BUTTON_1) {
								if (auto_list_thread &&
								    num_of_responses (index_point)) {
									goto group_list_thread;
								} else {
									goto group_tab_pressed;
								}
							}
							break;
						case MOUSE_BUTTON_2:
							if (xrow < INDEX2LNUM(first_subj_on_screen) ||
								xrow > INDEX2LNUM(last_subj_on_screen-1)) {
								goto group_page_up;
							}
							if (group_catchup_on_exit) {
								goto group_catchup;
							} else {
								goto group_done;
							}
							/* break; */
					}
				}				
				break;
#endif /* WIN32 */

#ifndef NO_SHELL_ESCAPE
			case iKeyGroupShell:
				shell_escape ();
				show_group_page ();
				break;
#endif

			case iKeyGroupFirstPage: /*show first page of threads */
top_of_list:
				if (! top_base) {
					break;
				}			
				if (index_point != 0) {
					if (0 < first_subj_on_screen) {
						erase_subject_arrow ();
						index_point = 0;
						show_group_page ();
					} else {
						erase_subject_arrow ();
						index_point = 0;
						draw_subject_arrow ();
					}
				}
				break;

			case iKeyGroupLastPage:	/* show last page of threads */
end_of_list:
				if (! top_base) {
					break;
				}			
				if (index_point != top_base - 1) {
					if (top_base - 1 > last_subj_on_screen) {
						erase_subject_arrow ();
						index_point = top_base - 1;
						show_group_page ();
					} else {
						erase_subject_arrow ();
						index_point = top_base - 1;
						draw_subject_arrow ();
					}
				}
				break;
				
			case iKeyGroupLastViewed:	/* go to last viewed article */
				if (this_resp < 0) {
					info_message (txt_no_last_message);
					break;
				}
				index_point = show_page (group, group_path, this_resp, &dummy);
				if (index_point == GRP_NOREDRAW) {
					index_point = which_thread (this_resp);
					clear_message ();
				} else {
					if (index_point < 0) {
						space_mode = (index_point == GRP_CONTINUE);
						goto group_done;
					}
					clear_note_area ();
					show_group_page ();
				}
				break;

			case iKeyGroupPipe:	/* pipe article/thread/tagged arts to command */
				if (index_point >= 0) {
					feed_articles (FEED_PIPE, GROUP_LEVEL, 
						&CURR_GROUP, (int) base[index_point]);
				}
				break;

			case iKeyGroupSetRange:	/* set range */
				if (iSetRange (GROUP_LEVEL, 1, top_base, index_point+1)) {
					show_group_page ();
				}
				break;

			case iKeyGroupFSearchSubj:	/* forward/backward search */
			case iKeyGroupBSearchSubj:
				search_subject ((ch == iKeyGroupFSearchSubj));
				break;

			case iKeyGroupFSearchBody:	/* search article body */
				if (index_point < 0) {
					info_message (txt_no_arts);
					break;
				}

				n = search_body (group, (int) base[index_point]);
				if (n != -1) {
					index_point = show_page (group, group_path, n, &dummy);
					if (index_point < 0) {
						space_mode = FALSE;
						goto group_done;
					}
					show_group_page ();
				}
				break;

			case iKeyGroupReadBasenote:
			case iKeyGroupReadBasenote2:	/* read current basenote */
group_read_basenote:
				if (index_point < 0) {
					info_message(txt_no_arts);
					break;
				}
				i = (int) base[index_point];
				index_point = show_page (group, group_path, i, &dummy);
				if (index_point == GRP_NOREDRAW) {
					index_point = which_thread (i);
				} else if (index_point < 0) {
					space_mode = (index_point == GRP_CONTINUE);
/*
printf ("point=[%d] filtered_art=[%d]", index_point, filtered_articles);
sleep(3);
*/
					if (index_point == GRP_CONTINUE) {
						goto group_tab_pressed;
					} else {
						goto group_done;
					}
				}
				clear_note_area ();
				show_group_page ();
				break;

			case iKeyGroupNextUnreadArtOrGrp:	/* goto next unread article/group */
group_tab_pressed:		
 				space_mode = TRUE;
				if (index_point < 0) {
					n =  -1;
				} else {
					n = next_unread ((int) base[index_point]);
				}	
				if (index_point < 0 || n < 0) {
					for (i = cur_groupnum+1 ; i < group_top ; i++) {
						if (active[my_group[i]].newsrc.num_unread > 0) {
							break;
						}
					}		
					if (i >= group_top) {
						goto group_done;
					}
					cur_groupnum = i;
					index_point = GRP_GOTONEXT;
					goto group_done;
				}
				index_point = show_page (group, group_path, n, &dummy);
				if (index_point == GRP_NOREDRAW || 
					index_point == GRP_CONTINUE) {
					index_point = which_thread (n);
					goto group_tab_pressed;	/* repeat TAB */
				} else {
					if (index_point < 0) {
						goto group_done;
					}	
					clear_note_area ();
					show_group_page ();
				}
				break;
	
			case iKeyGroupPageDown:		/* page down */
			case iKeyGroupPageDown2:
			case iKeyGroupPageDown3:	/* vi style */
group_page_down:
				if (! top_base) {
					break;
				}
				if (index_point == top_base - 1) {
					if (0 < first_subj_on_screen) {
						erase_subject_arrow ();
						index_point = 0;
						show_group_page ();
					} else {
						erase_subject_arrow ();
						index_point = 0;
						draw_subject_arrow ();
					}
					break;
				}
				erase_subject_arrow ();
				scroll_lines = (full_page_scroll ? NOTESLINES : NOTESLINES / 2);
				index_point = ((index_point + scroll_lines) / scroll_lines) * scroll_lines;
				if (index_point >= top_base) {
					index_point = (top_base / scroll_lines) * scroll_lines;
					if (index_point < top_base - 1) {
						index_point = top_base - 1;
					}
				}
				if (index_point < first_subj_on_screen
				|| index_point >= last_subj_on_screen)
					show_group_page ();
				else
					draw_subject_arrow ();
				break;

			case iKeyGroupAutosel:		/* auto-select article menu */
 				if (index_point < 0) {
 					info_message (txt_no_arts);
					break;
				}
				old_top = top;
				n = (int) base[index_point];
				old_artnum = arts[n].artnum;
				if (filter_menu (FILTER_SELECT, group, &arts[n])) {
					if (filter_articles (group)) {
						make_threads (group, FALSE);
						find_base (group);
						index_point = find_new_pos (old_top, old_artnum, index_point);
					}
				}
				show_group_page ();
				break;

			case iKeyGroupKill:		/* kill article menu */
 				if (index_point < 0) {
 					info_message (txt_no_arts);
					break;
				}
				old_top = top;
				n = (int) base[index_point];
				old_artnum = arts[n].artnum;
				if (filter_menu (FILTER_KILL, group, &arts[n])) {
					if (filter_articles (group)) {
						make_threads (group, FALSE);
						find_base (group);
						index_point = find_new_pos (old_top, old_artnum, index_point);
					}
				}
				show_group_page ();
				break;

			case iKeyGroupQuickAutosel:		/* quickly auto-select article */
 				if (index_point < 0) {
 					info_message (txt_no_arts);
					break;
				}
				if (! confirm_action || prompt_yn (cLINES, txt_quick_filter_select, TRUE) == 1) {
					old_top = top;
					n = (int) base[index_point];
					old_artnum = arts[n].artnum;
					if (quick_filter_select (group, &arts[n])) {
						info_message ("Auto-selection filter added");
						if (filter_articles (group)) {
							make_threads (group, FALSE);
							find_base (group);
							index_point = find_new_pos (old_top, old_artnum, index_point);
						}
					}
					show_group_page ();
				}
				break;

			case iKeyGroupQuickKill:		/* quickly kill article */
 				if (index_point < 0) {
 					info_message (txt_no_arts);
					break;
				}
				if (! confirm_action || prompt_yn (cLINES, txt_quick_filter_kill, TRUE) == 1) {
					old_top = top;
					n = (int) base[index_point];
					old_artnum = arts[n].artnum;
					if (quick_filter_kill (group, &arts[n])) {
						info_message ("Kill filter added");
						if (filter_articles (group)) {
							make_threads (group, FALSE);
							find_base (group);
							index_point = find_new_pos (old_top, old_artnum, index_point);
						}
					}
					show_group_page ();
				}
				break;

			case iKeyGroupRedrawScr:	/* redraw screen */
			case iKeyGroupRedrawScr2:
			case iKeyGroupRedrawScr3:
				ClearScreen ();
				set_xclick_off ();
				show_group_page ();
				break;
				
			case iKeyGroupDown:
			case iKeyGroupDown2:		/* line down */
group_down:
				if (! top_base) {
					break;
				}
				if (index_point + 1 >= top_base) {
					if (_hp_glitch) {
						erase_subject_arrow ();
					}
					if (0 < first_subj_on_screen) {
						index_point = 0;
						show_group_page ();
					} else {
						erase_subject_arrow ();
						index_point = 0;
						draw_subject_arrow ();
					}
					break;
				}
				if (index_point + 1 >= last_subj_on_screen) {
					erase_subject_arrow();
					index_point++;
					show_group_page ();
				} else {
					erase_subject_arrow ();
					index_point++;
					draw_subject_arrow ();
				}
				break;

			case iKeyGroupUp:
			case iKeyGroupUp2:		/* line up */
group_up:
				if (! top_base) {
					break;
				}	
				if (index_point == 0) {
					if (_hp_glitch) {
						erase_subject_arrow ();
					}
					if (top_base > last_subj_on_screen) {
						index_point = top_base - 1;
						show_group_page ();
					} else {
						erase_subject_arrow ();
						index_point = top_base - 1;
						draw_subject_arrow ();
					}
					break;
				}
				if (_hp_glitch) {
					erase_subject_arrow ();
				}
				if (index_point <= first_subj_on_screen) {
					index_point--;
					show_group_page ();
				} else {
					erase_subject_arrow ();
					index_point--;
					draw_subject_arrow ();
				}
				break;

			case iKeyGroupPageUp:		/* page up */
			case iKeyGroupPageUp2:
			case iKeyGroupPageUp3:		/* vi style */
group_page_up:
				if (! top_base) {
					break;
				}
				if (index_point == 0) {
					if (_hp_glitch) {
						erase_subject_arrow ();
					}
					if (top_base > last_subj_on_screen) {
						index_point = top_base - 1;
						show_group_page ();
					} else {
						erase_subject_arrow ();
						index_point = top_base - 1;
						draw_subject_arrow ();
					}
					break;
				}
				clear_message ();
				erase_subject_arrow ();
				scroll_lines = (full_page_scroll ? NOTESLINES : NOTESLINES / 2);
				if ((n = index_point % scroll_lines) > 0) {
					index_point = index_point - n;
				} else {
					index_point = ((index_point - scroll_lines) / scroll_lines) * scroll_lines;
				}
				if (index_point < 0) {
					index_point = 0;
				}
				if (index_point < first_subj_on_screen
				|| index_point >= last_subj_on_screen)
					show_group_page ();
				else
					draw_subject_arrow ();
				break;

			case iKeyGroupFSearchAuth:	/* author search forward */
			case iKeyGroupBSearchAuth:	/* author search backward */
				if (index_point < 0) {
					info_message (txt_no_arts);
					break;
				}

				i = (ch == iKeyGroupFSearchAuth);

				n = search_author (my_group[cur_groupnum], (int) base[index_point], i);
				if (n < 0)
					break;

				index_point = show_page (group, group_path, n, &dummy);
				if (index_point != GRP_NOREDRAW) {
					if (index_point < 0) {
						space_mode = FALSE;
						goto group_done;
					}
					clear_note_area ();
					show_group_page ();
				}
				break;

 			case iKeyGroupCatchup:	/* catchup - mark all articles as read */
 			case iKeyGroupCatchupGotoNext:	/* catchup - and goto next unread group */
group_catchup:
				{	int yn = 1;
					if (num_of_tagged_arts && prompt_yn (cLINES, txt_catchup_despite_tags, 'y') != 1) {
						break;
					}
				    if (! CURR_GROUP.newsrc.num_unread || 
					    ! confirm_action || (yn = prompt_yn (cLINES, txt_mark_all_read, TRUE)) == 1) {
						grp_mark_read (&CURR_GROUP, arts);
					}
					if (ch == iKeyGroupCatchupGotoNext) {
						if (yn == 1) {
							goto group_tab_pressed;
						}
					} else {
						if (yn != -1) {
							if (cur_groupnum + 1 < group_top) {
								cur_groupnum++;
							}
							goto group_done;
						}
					}
				}
			    break;

			case iKeyGroupToggleSubjDisplay:	/* toggle display of subject & subj/author */
				toggle_subject_from ();
				show_group_page ();
				break;

			case iKeyGroupGoto:	/* choose a new group by name */
				old_group_top = group_top;
				n = choose_new_group ();
				if (n >= 0 && n != cur_groupnum) {
					/* 
					 * if we added a group, set the length as appropriate 
					 * for the group selection display
					 */
					if (old_group_top != group_top)
						set_groupname_len(FALSE);
					cur_groupnum = n;
					index_point = GRP_GOTONEXT;
					goto group_done;
				}
				break;

			case iKeyGroupHelp:	/* help */
				show_info_page (HELP_INFO, help_group, txt_index_page_com);
				show_group_page ();
				break;

			case iKeyGroupToggleHelpDisplay:	/* toggle mini help menu */
				toggle_mini_help (GROUP_LEVEL);
				show_group_page();
				break;

			case iKeyGroupToggleInverseVideo:	/* toggle inverse video */
				toggle_inverse_video ();
				show_group_page ();
				break;

			case iKeyGroupKillThd:	/* mark rest of thread as read */
				if (index_point < 0) {
					info_message (txt_no_next_unread_art);
					break;
				}
				old_selected_arts = num_of_selected_arts;
				thd_mark_read (&CURR_GROUP, base[index_point]);
				if (num_of_selected_arts != old_selected_arts) {
					show_group_title (TRUE);
				}
				bld_sline (index_point);
				draw_sline (index_point, FALSE);

				n = next_unread (next_response ((int) base[index_point]));
				if (n < 0) {
					draw_subject_arrow ();
					info_message (txt_no_next_unread_art);
					break;
				}

				if ((n = which_thread (n)) < 0) {
					error_message ("Internal error: K which_thread < 0", "");
					break;
				}

				if (/* n < first_subj_on_screen || */ n >= last_subj_on_screen) {
					if (_hp_glitch)
						erase_subject_arrow ();
					index_point = n;
					show_group_page ();
				} else if (n > index_point) {
					erase_subject_arrow ();
					index_point = n;
					draw_subject_arrow ();
				}
				break;

			case iKeyGroupListThd:	/* list articles within current thread */
group_list_thread:
				if (index_point < 0) {
					info_message (txt_no_arts);
					break;
				}
 				space_mode = TRUE;
				n = show_thread (group, group_path, (int) base[index_point]);
				if (n == GRP_QUIT) {
					index_point = n;
					space_mode = FALSE;
					goto group_done;
				} else if (n == GRP_KILLED) {
					index_point = 0;
				} else if (index_point < 0) {
					space_mode = FALSE;
					goto group_done;
				}
				clear_note_area ();
				show_group_page ();
				break;	

			case iKeyGroupMail:	/* mail article to somebody */
				if (index_point >= 0) {
					feed_articles (FEED_MAIL, GROUP_LEVEL, 
						&CURR_GROUP, (int) base[index_point]);
				}
				break;

			case iKeyGroupOptionMenu:	/* options menu */
				if (top_base > 0) {
					old_top = top;
					n = (int) base[index_point];
					old_artnum = arts[n].artnum;
				}
				n = default_sort_art_type;
				filter_state = change_config_file (group, TRUE);
				if (filter_state == NO_FILTERING && n != default_sort_art_type) {
					make_threads (&CURR_GROUP, TRUE);
					find_base (&CURR_GROUP);
				}
				set_subj_from_size (cCOLS);
				index_point = find_new_pos (old_top, old_artnum, index_point);
				show_group_page ();
			    break;

			case iKeyGroupNextGroup:	/* goto next group */
				clear_message ();
				if (cur_groupnum + 1 >= group_top)
					info_message (txt_no_more_groups);
				else {
					cur_groupnum++;
					index_point = GRP_GOTONEXT;
					space_mode = pos_first_unread;
					goto group_done;
				}
				break;

			case iKeyGroupNextUnreadArt:	/* goto next unread article */
				if (index_point < 0) {
					info_message(txt_no_next_unread_art);
					break;
				}

				n = next_unread ((int) base[index_point]);
				if (n == -1) {
					info_message (txt_no_next_unread_art);
				} else {
					index_point = show_page (group, group_path, n, &dummy);
					if (index_point != GRP_NOREDRAW) {
						if (index_point < 0) {
							space_mode = pos_first_unread;
							goto group_done;
						}
						clear_note_area ();
						show_group_page ();
					}
				}
				break;

			case iKeyGroupPrint:	/* output art/thread/tagged arts to printer */
				if (index_point >= 0) {
					feed_articles (FEED_PRINT, GROUP_LEVEL, 
						&CURR_GROUP, (int) base[index_point]);
				}
				break;

			case iKeyGroupPrevGroup:	/* previous group */
				clear_message();
				if (cur_groupnum <= 0)
					info_message(txt_no_prev_group);
				else {
					cur_groupnum--;
					index_point = GRP_GOTONEXT;
					space_mode = pos_first_unread;
					goto group_done;
				}
				break;

			case iKeyGroupPrevUnreadArt:	/* go to previous unread article */
				if (index_point < 0) {
				    info_message(txt_no_prev_unread_art);
				    break;
				}
				n = prev_response ((int) base[index_point]);
				n = prev_unread (n);
				if (n == -1) {
				    info_message(txt_no_prev_unread_art);
				} else {
					index_point = show_page (group, group_path, n, &dummy);
					if (index_point != GRP_NOREDRAW) {
						if (index_point < 0) {
							space_mode = pos_first_unread;
							goto group_done;
						}
						clear_note_area ();
						show_group_page ();
					}
				}
				break;

			case iKeyGroupQuit:	/* return to group selection page */
/*			case iKeyGroupQuit2: */
				if (num_of_tagged_arts && prompt_yn (cLINES, txt_quit_despite_tags, 'y') != 1) {
					break;
				}
				goto group_done;

			case iKeyGroupQuitTin:		/* quit */
				if (num_of_tagged_arts && prompt_yn (cLINES, txt_quit_despite_tags, 'y') != 1) {
					break;
				}
				index_point = GRP_QUIT;
				space_mode = FALSE;
				goto group_done;

	 		case iKeyGroupToggleReadDisplay:		
	 			/* 
	 			 * If in show_only_unread mode or there  are
	 			 * unread articles we know this thread  will
	 			 * exist after toggle. Otherwise we find the
	 			 * next closest 
	 			 */
 				if (CURR_GROUP.attribute->show_only_unread) {
					wait_message (txt_reading_all_arts);
 				} else {
					wait_message (txt_reading_new_arts);
 				} 
 				i = -1;
 				if (index_point >= 0) {
 					if (CURR_GROUP.attribute->show_only_unread || 
 					    new_responses (index_point)) {
 						i = base[index_point];
 					} else if ((n = prev_unread ((int)base[index_point])) >= 0) {
 						i = n;
 					} else if ((n = next_unread ((int)base[index_point])) >= 0) {
 						i = n;
 					}	
 				}
 				CURR_GROUP.attribute->show_only_unread = 
 					!CURR_GROUP.attribute->show_only_unread;
 				auto_select_articles (&CURR_GROUP);
 				find_base (&CURR_GROUP);
 				if (i >= 0 && (n = which_thread (i)) >= 0)
 					index_point = n;
 				else if (top_base > 0)
 					index_point = top_base - 1;
 				show_group_page ();
 				break;

			case iKeyGroupBugReport:	/* bug/gripe/comment mailed to author */
				mail_bug_report ();
				ClearScreen ();
				show_group_page ();
				break;

			case iKeyGroupSave:	/* save articles with prompting */
				if (index_point >= 0) {
					feed_articles (FEED_SAVE, GROUP_LEVEL, 
						&CURR_GROUP, (int) base[index_point]);
				}
				break;
			
			case iKeyGroupSaveTagged:	/* save tagged articles without prompting */
				if (index_point >= 0) {
					if (num_of_tagged_arts) {
						feed_articles (FEED_SAVE_TAGGED, GROUP_LEVEL, 
							&CURR_GROUP, (int) base[index_point]);
					} else {
						info_message (txt_no_tagged_arts_to_save);
					}
				}
				break;

			case iKeyGroupTag:	/* tag/untag art for mailing/piping/printing/saving */
				if (index_point >= 0) {
					int tagged = TRUE;
					n = (int) base[index_point];
					if (CURR_GROUP.attribute->thread_arts) {
						int i;
						/*
						 * Unlike 'line_is_tagged()', this loop looks for any
						 * article in the thread that isn't already tagged.
						 */
						for (i = n; i != -1 && tagged; i = arts[i].thread) {
							if (! arts[i].tagged)
								tagged = FALSE;
						}
						if (tagged) {
							/* 
							 * Here we repeat the tagged test in both blocks
							 * to leave the choice of tagged/untagged
							 * determination politic in the previous lines. 
							 */
							info_message (txt_untagged_thread);
							for (i = n; i != -1; i = arts[i].thread) {
								if (arts[i].tagged) {
									tagged = TRUE;
									decr_tagged (arts[i].tagged);
									arts[i].tagged = 0;
									--num_of_tagged_arts;
								}
							}
						} else {
							info_message (txt_tagged_thread);
							for (i = n; i != -1; i = arts[i].thread) {
								if (! arts[i].tagged)
									arts[i].tagged = ++num_of_tagged_arts;
							}
						}
					} else {
						tagged = FALSE;
						if (arts[n].tagged) {
							info_message (txt_untagged_art);
							tagged = TRUE;
							decr_tagged (arts[n].tagged);
							arts[n].tagged = 0;
							--num_of_tagged_arts;
						} else {
							info_message (txt_tagged_art);
							arts[n].tagged = ++num_of_tagged_arts;
						}
					}
					bld_sline (index_point);
					draw_sline (index_point, FALSE);
					if (tagged)
						show_tagged_lines ();
					if (index_point + 1 < top_base)
						goto group_down;
					draw_subject_arrow ();
				}
				break;

			/*
			 * If we have ref threading, cycle through the threading
			 * types, otherwise act as a straight toggle
			 */
			case iKeyGroupToggleThreading:
 				if (index_point >= 0) {
					CURR_GROUP.attribute->thread_arts = 
#ifdef REF_THREADING
						++CURR_GROUP.attribute->thread_arts % (THREAD_REFS + 1);
#else
						!CURR_GROUP.attribute->thread_arts;
#endif
					make_threads (&CURR_GROUP, TRUE);
					find_base (&CURR_GROUP);
					show_group_page ();
				}
				break;

			case iKeyGroupUntag:	/* untag all articles */
 				if (index_point >= 0) {
					untag_all_articles ();
					update_group_page ();
				}
				break;

			case iKeyGroupVersion:
				info_message (cvers);
				break;

			case iKeyGroupPost:	/* post an article */
				if (post_article (group->name, &posted)) {
					show_group_page ();
				}
				break;

			case iKeyGroupDisplayPostHist:	/* display messages posted by user */
				if (user_posted_messages ()) {
					show_group_page ();
				}
				break;

			case iKeyGroupRepost:	/* repost current article */
				if (index_point >= 0) {
					feed_articles (FEED_REPOST, GROUP_LEVEL, 
						&CURR_GROUP, (int) base[index_point]);
				}
				break;

			case iKeyGroupMarkArtUnread:	/* mark article as unread */
 				if (index_point < 0) {
 					info_message (txt_no_arts);
					break;
				}
 				art_mark_will_return (&CURR_GROUP, &arts[base[index_point]]);
 				/* art_mark_unread (&CURR_GROUP, &arts[base[index_point]]); */
				show_group_title (TRUE);
				bld_sline(index_point);
				draw_sline(index_point, FALSE);
				draw_subject_arrow();
				info_message (txt_art_marked_as_unread);
				break;

			case iKeyGroupMarkThdUnread:	/* mark thread as unread */
 				if (index_point < 0) {
 					info_message (txt_no_arts);
					break;
				}
				thd_mark_unread (&CURR_GROUP, base[index_point]);
				show_group_title (TRUE);
				bld_sline(index_point);
				draw_sline(index_point, FALSE);
				draw_subject_arrow();
				info_message (txt_thread_marked_as_unread);
				break;

			case iKeyGroupSelThd:	/* mark thread as selected */
			case iKeyGroupToggleThdSel:	/* toggle thread */
 				if (index_point < 0) {
 					info_message (txt_no_arts);
					break;
				}
				
				flag = 1;
				if (ch == iKeyGroupToggleThdSel) {
					stat_thread(index_point, &sbuf);
					if (sbuf.selected_unread == sbuf.unread)
						flag = 0;
				}
				n = 0;
				for (i = (int) base[index_point] ; i != -1 ; i = arts[i].thread) {
					arts[i].selected = flag;
					++n;
				}
				assert (n > 0);
				bld_sline(index_point);
				draw_sline(index_point, FALSE);
/* #if 0 */
				info_message ( flag
					      ? txt_thread_marked_as_selected
					      : txt_thread_marked_as_deselected);
/* #endif */
				if (index_point + 1 < top_base)
					goto group_down;
				draw_subject_arrow ();
				break;

			case iKeyGroupReverseSel:	/* reverse selections */
				for (i=0; i < top; i++) {
					arts[i].selected = !arts[i].selected;
				}
				update_group_page ();
				break;

  			case iKeyGroupUndoSel:	/* undo selections */
 				for (i=0; i < top; i++) {
  					arts[i].selected = FALSE;
 					arts[i].zombie = FALSE;
 				}
 				xflag = 0;
  				update_group_page ();
  				break;
  
 			case iKeyGroupSelPattern:	/* select matching patterns */
 				sprintf (msg, txt_select_pattern, default_select_pattern);
 				if (! prompt_string (msg, buf)) {
 					break;
 				}	
 				if (buf[0] == '\0') {
 					if (default_select_pattern[0] == '\0') {
 						info_message ("No previous expression");
 						break;
 					}
					sprintf (pat, "*%s*", default_select_pattern);
 				} else if (STRCMPEQ(buf, "*")) { /* all */
 					strcpy (pat, buf);
 					strcpy (default_select_pattern, pat);
 				} else {
 					str_lwr (buf, buf);
 					strcpy (default_select_pattern, buf);
 					sprintf (pat, "*%s*", default_select_pattern);
 				}
 
 				flag = 0;
 				for (n=0; n < top_base; n++) {
 					char sub[LEN];
 					str_lwr (arts[base[n]].subject, sub);
 					if (! wildmat (sub, pat)) {
 						continue;
 					}
 					for (i = (int) base[n] ; i != -1 ; i = arts[i].thread) {
 						arts[i].selected = TRUE;
 					}
 					bld_sline(n);
 					flag++;
 				}
 				if (flag) {
 					update_group_page ();
 				}
 				break;

			case iKeyGroupSelThdIfUnreadSelected:	/* select all unread arts in thread hot if 1 is hot */
				for (n=0; n < top_base; n++) {
					stat_thread(n, &sbuf);
					if (! sbuf.selected_unread || sbuf.selected_unread == sbuf.unread) {
						continue;
					}
					for (i = (int) base[n] ; i != -1 ; i = arts[i].thread) {
						arts[i].selected = 1;
					}
				}
				/* no screen update needed */
				break;

			case iKeyGroupMarkUnselArtRead:	/* mark read all unselected arts */
do_auto_select_arts:
				if (xflag) {
					goto undo_auto_select_arts;
				}
				for (i=0; i < top; ++i) {
					if (arts[i].status == ART_UNREAD && arts[i].selected != 1) {
						debug_print_comment ("group.c: X command");
						art_mark_read (&CURR_GROUP, &arts[i]);
						arts[i].zombie = TRUE;
					}
				}
				if (CURR_GROUP.attribute->show_only_unread) {
					find_base (&CURR_GROUP);
				}
				xflag = TRUE;
				index_point = 0;
 				show_group_page ();
				break;

			case iKeyGroupDoAutoSel:	/* perform auto-selection on group */
				if (auto_select_articles (&CURR_GROUP)) {
					update_group_page ();
				}
/*				break; */
undo_auto_select_arts:
				for (i=0; i<top; ++i) {
					if (arts[i].status == ART_READ && arts[i].zombie) {
						debug_print_comment ("group.c: + command");
						art_mark_unread (&CURR_GROUP, &arts[i]);
						arts[i].zombie = FALSE;
					}
				}
				if (CURR_GROUP.attribute->show_only_unread) {
					find_base (&CURR_GROUP);
				}
				xflag = FALSE;
				index_point = 0;	/* do we want this ? */
 				show_group_page ();
				break;

			case iKeyGroupDisplaySubject:
				info_message(arts[(int)base[index_point]].subject);
				break;

			default:
			    info_message (txt_bad_command);
		}
	}

group_done:
	set_xclick_off ();
	if (index_point == GRP_QUIT) {
		write_config_file (local_config_file);
		tin_done (EXIT_OK);
	}	
	clear_note_area ();

	vGrpDelMailArts (&CURR_GROUP);

#endif /* INDEX_DAEMON */
}


void 
show_group_page ()
{
#ifndef INDEX_DAEMON
	int i;

	set_signals_group ();
	
	MoveCursor (0, 0);
	CleartoEOLN ();

	show_group_title (FALSE);

	MoveCursor (1, 0);
	CleartoEOLN ();
	MoveCursor (INDEX_TOP, 0);

	if (index_point >= top_base) {
		index_point = top_base - 1;
	}

	if (NOTESLINES <= 0) {
		first_subj_on_screen = 0;
	} else {
		first_subj_on_screen = (index_point / NOTESLINES) * NOTESLINES;
		if (first_subj_on_screen < 0) {
			first_subj_on_screen = 0;
		}
	}

	last_subj_on_screen = first_subj_on_screen + NOTESLINES;

	if (last_subj_on_screen >= top_base) {
		last_subj_on_screen = top_base;
		first_subj_on_screen = (top_base / NOTESLINES) * NOTESLINES;

		if (first_subj_on_screen == last_subj_on_screen ||
			first_subj_on_screen < 0) {
			if (first_subj_on_screen < 0) {
				first_subj_on_screen = 0;
			} else {
				first_subj_on_screen = last_subj_on_screen - NOTESLINES;
			}
		}
	}

	if (top_base == 0) {
		first_subj_on_screen = 0;
		last_subj_on_screen = 0;
	}

	if (draw_arrow_mark) {
		CleartoEOS ();
	}

	for (i = first_subj_on_screen; i < last_subj_on_screen; ++i) {
		bld_sline(i);
		draw_sline(i, TRUE);
	}

	CleartoEOS ();
	show_mini_help (GROUP_LEVEL);

	if (top_base <= 0) {
		info_message(txt_no_arts);
		return;
	} else if (last_subj_on_screen == top_base) {
		info_message(txt_end_of_arts);
	}

	draw_subject_arrow();

#endif /* INDEX_DAEMON */
}

void 
update_group_page ()
{
#ifndef INDEX_DAEMON
	register int i;

	for (i = first_subj_on_screen; i < last_subj_on_screen; ++i) {
		bld_sline (i);
		draw_sline (i, FALSE);
	}

	if (top_base <= 0)
		return;

	draw_subject_arrow ();
#endif /* INDEX_DAEMON */
}


void 
draw_subject_arrow ()
{
	MoveCursor (INDEX2LNUM(index_point), 0);

	if (draw_arrow_mark) {
		my_fputs ("->", stdout);
		fflush (stdout);
	} else {
		StartInverse();
		draw_sline(index_point, TRUE);
		EndInverse();
	}
	MoveCursor (cLINES, 0);
}

void 
erase_subject_arrow ()
{
	MoveCursor (INDEX2LNUM(index_point), 0);

	if (draw_arrow_mark) {
		my_fputs ("  ", stdout);
	} else {
		if (_hp_glitch) {
			EndInverse ();
		}
		draw_sline(index_point, TRUE);
	}
	fflush (stdout);
}


int
prompt_subject_num (ch, group)
	int ch;
	char *group;
{
	int num;

	if (! top_base) {
		return FALSE;
	}

	clear_message ();

	if ((num = prompt_num (ch, txt_select_thread)) == -1) {
		clear_message ();
		return FALSE;
	}
	num--;		/* index from 0 (internal) vs. 1 (user) */

	if (num < 0) {
		num = 0;
	}
	if (num >= top_base) {
		num = top_base - 1;
	}

	if (num >= first_subj_on_screen
	&&  num < last_subj_on_screen) {
		erase_subject_arrow ();
		index_point = num;
		draw_subject_arrow ();
	} else {
		erase_subject_arrow ();
		index_point = num;
		show_group_page ();
	}

	return TRUE;
}


void 
clear_note_area ()
{
	MoveCursor (INDEX_TOP, 0);
	CleartoEOS ();
}

/*
 * Find new index position after a kill or unkill. Because
 * kill can work on author it is impossible to know which,
 * if any, articles will be left afterwards. So we make a
 * "best attempt" to find a new index point.
 */

int 
find_new_pos (old_top, old_artnum, cur_pos)
	int old_top;
	long old_artnum;
	int cur_pos;
{
	int i, pos;
	
 	if (top == old_top) {
 		return cur_pos;
 	}	
  
	for (i = 0 ; i < top ; i++) {
		if (arts[i].artnum == old_artnum) {
 			pos = which_thread (arts[i].artnum);
 			if (pos >= 0) {
 				return pos;
 			}
 		}
	}
 	
 	if (cur_pos < top_base) {
 		return cur_pos;
 	} else {
 		return (top_base - 1);
 	}	
}


void 
mark_screen (level, screen_row, screen_col, value)
	int level;
	int screen_row;
	int screen_col;
	char *value;
{
	int i, len;

	len = strlen (value);
	
	if (draw_arrow_mark) {
		MoveCursor(INDEX_TOP + screen_row, screen_col);
		my_fputs (value, stdout);
		MoveCursor (cLINES, 0);
		fflush (stdout);
	} else {
		for (i=0 ; i < len ; i++) {
			screen[screen_row].col[screen_col+i] = value[i];
		}
		if (level == SELECT_LEVEL) {
			draw_group_arrow();
		} else {
			draw_subject_arrow();
		}	
	}
}


void 
set_subj_from_size (num_cols)
	int num_cols;
{
	int size;
	
	if (show_author == SHOW_FROM_BOTH) {
              max_subj = (num_cols / 2) - 4;
	} else {
              max_subj = (num_cols / 2) + 3;
	}
	max_from = (num_cols - max_subj) - 17;

	if (show_author != SHOW_FROM_BOTH) {
		if (max_from > 25) {
			size = max_from - 25;
			max_from = 25;
			max_subj = max_subj + size;
		}
	}

	if (show_author != SHOW_FROM_NONE) {
		len_from = max_from - BLANK_GROUP_COLS;
		len_subj = max_subj;
		spaces = "   ";
	} else {
		len_from = 0;
		len_subj = (max_subj+max_from+3) - BLANK_GROUP_COLS;
		spaces = "";
	}
}


void 
toggle_subject_from ()
{
/*
	int i;

	i = my_group[cur_groupnum];

	if (active[i].attribute->show_author != SHOW_FROM_NONE) {
		if (show_author != SHOW_FROM_NONE) {
			show_author = SHOW_FROM_NONE;
		} else {
			show_author = active[i].attribute->show_author;
		}
	} else {
 */
		if (++show_author > SHOW_FROM_BOTH) {
			show_author = SHOW_FROM_NONE;
		}
/*
		 else {
			show_author++;
		}
	}
 */
	set_subj_from_size (cCOLS);
}

/*
 * Build subject line given an index into base[]. 
 *
 * WARNING: the routine is tightly coupled with draw_sline() in the sense
 * that draw_sline() expects bld_sline() to place the article mark
 * (read_art_makr, selected_art_mark, etc) at MARK_OFFSET in the screen[].col.
 * So, if you change the format used in this routine, be sure to check
 * that the value of MARK_OFFSET is still correct. 
 * Yes, this is somewhat kludgy.
 */
 
static int 
bld_sline (i)
	int i;
{
#ifndef INDEX_DAEMON
	int respnum;
	int n, j;
	char from[LEN];
	char new_resps[8];
	char art_cnt[9];
 	struct t_art_stat sbuf;
	register char *buffer;

	from[0] = '\0';
	respnum = (int) base[i];
	
	stat_thread(i, &sbuf);

	/*
	 * n is number of articles in this thread
	 */
	if (CURR_GROUP.attribute->show_only_unread)
		n = sbuf.unread + sbuf.seen;
	else
		n = sbuf.total;
	
/*	n = ART_ADJUST(n); T.Dickey 941027 */
	
	if ((j = line_is_tagged(respnum)) != 0) {
		sprintf (new_resps, "%3d", j);
	} else {
		sprintf (new_resps, "  %c", sbuf.art_mark);
	}

	/*
	 * Find index of first unread in this thread
	 */
	j = (sbuf.unread) ? next_unread(respnum) : respnum;

/*	if (n) { T.Dickey 941027 */

/*
 * TODO: hack in a config var for this
 */
	if (n > 1) {
		if (arts[j].lines != -1)
			sprintf (art_cnt, "%-3d %-4d ", n, arts[j].lines);
		else
			sprintf (art_cnt, "%-3d ?    ", n);
	} else {
		if (arts[j].lines != -1)
			sprintf (art_cnt, "    %-4d ", arts[j].lines);
		else
			strcpy (art_cnt, "    ?    ");
	}

	if (show_author != SHOW_FROM_NONE) {
		get_author (FALSE, &arts[respnum], from);
	}	
	
	j = INDEX2SNUM(i);
	sprintf (buffer = screen[j].col, "  %4d%3s %s%-*.*s%s%-*.*s",
		 i+1, new_resps, art_cnt, len_subj-5, len_subj-5,
		 arts[respnum].subject, spaces, len_from, len_from, from);
	
	/* protect display from non-displayable characters (e.g., form-feed) */
	for (n = 0; buffer[n] != '\0'; n++)
#if 0  /* CHRIS behaves badly with some environments */
		if (!isprint(buffer[n]))
#else
		if (!isprint(buffer[n]) && !((unsigned char)buffer[n]>=0xa0))
#endif
			buffer[n] = '?';

#endif /* INDEX_DAEMON */
	return(0);
}

/*
 * Draw subject line given an index into base[].
 *
 * WARNING: this routine is tightly coupled with bld_sline(); see the warning
 * associated with that routine for details. (C++ would be handy here.)
 *
 * NOTE: the 2nd argument is used to control whether the full line is 
 * redrawn or just the the parts of it that can be changed by a
 * command; i.e., the unread art count and the art mark. This will result
 * in a slightly more efficient update, though at the price of increased 
 * code complexity and readability.
 */

static int 
draw_sline (i, full)
	int i;
	int full;	/* unused at moment */
{
#ifndef INDEX_DAEMON
	size_t tlen;
	int j, x;
	int k = MARK_OFFSET;
	char *s;

	j = INDEX2SNUM(i);

	if (full) {	
		s = screen[j].col;
		tlen = strlen (s);
		x = 0;
		if (strip_blanks) {		
			strip_line (s, tlen);
			tlen = strlen (s);	/* notes new line length */
			CleartoEOLN ();
		}
	} else {
              tlen  = 12;
		s = &screen[j].col[6];
		x = 6;
/* ..0001..+.???.????. */
		if (strip_blanks) {		
			tlen = strlen (s);	/* notes new line length */
			strip_line (s, tlen);
/*			CleartoEOLN ();
*/
		}
	}

	MoveCursor (INDEX2LNUM(i), x);
#ifdef VMS
	sys_fwrite (s, 1, tlen, stdout);
#else
	fwrite (s, 1, tlen, stdout);
#endif

	/*
	 * it is somewhat less efficient to go back and redo that art mark
	 * if selected, but it is quite readable as to what is happening
	 */
	if (screen[j].col[k] == art_marked_selected) {
		MoveCursor (INDEX2LNUM(i), k);
		ToggleInverse ();
		my_fputc (screen[j].col[k], stdout);
		ToggleInverse ();
	}

	MoveCursor(INDEX2LNUM(i)+1, 0);
	
#endif /* INDEX_DAEMON */
	return(0);
}


void 
show_group_title (clear_title)
	int clear_title;
{
#ifndef INDEX_DAEMON

	char buf[PATH_LEN];
	int num;
	register int i, art_cnt = 0;
	
	num = my_group[cur_groupnum];
	
	if (active[num].attribute->show_only_unread) {
		for (i = 0 ; i < top_base ; i++) {
			art_cnt += new_responses (i);
		}
	} else {
		for (i = 0 ; i < top ; i++) {
			if (! IGNORE_ART(i)) {
				++art_cnt;
			}
		}
	}

	if (active[num].attribute->thread_arts) {
		sprintf (buf, "%s (%dT(%c) %dA %dK %dH%s%s)", 
			active[num].name, top_base, 
			(active[num].attribute->thread_arts == THREAD_SUBJ) ? 'S' : 'R',
			art_cnt, num_of_killed_arts, num_of_selected_arts,
			(active[num].attribute->show_only_unread ? " R" : ""),
			(active[num].moderated == 'm' ? " M" : ""));
	} else {
		sprintf (buf, "%s (%dU %dK %dH%s%s)", 
			active[num].name,
			art_cnt, num_of_killed_arts, num_of_selected_arts,
			(active[num].attribute->show_only_unread ? " R" : ""),
			(active[num].moderated == 'm' ? " M" : ""));
	}

	if (clear_title) {
		MoveCursor (0, 0);
		CleartoEOLN ();
	}
	
	show_title (buf);

#endif /* INDEX_DAEMON */
}
