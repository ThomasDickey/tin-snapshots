/*
 *  Project   : tin - a Usenet reader
 *  Module    : group.c
 *  Author    : I. Lea & R. Skrenta
 *  Created   : 1991-04-01
 *  Updated   : 1997-12-31
 *  Notes     :
 *  Copyright : (c) Copyright 1991-99 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */
#ifndef TCURSES_H
#	include "tcurses.h"
#endif /* !TCURSES_H */
#ifndef MENUKEYS_H
#	include  "menukeys.h"
#endif /* !MENUKEYS_H */

#define INDEX2SNUM(i)	((i) % NOTESLINES)
#define SNUM2LNUM(i)	(INDEX_TOP + (i))
#define INDEX2LNUM(i)	(SNUM2LNUM(INDEX2SNUM(i)))

char *glob_group;

/*
 * Essentially, an index into base[]
 * Equates to the cursor location (thread number) on group page
 */
int index_point;
int max_from = 0;
int max_subj = 0;

static const char *spaces = "XXXX";
static int len_from;
static int len_subj;
#ifndef INDEX_DAEMON
	static int first_subj_on_screen;
	static int last_subj_on_screen;
	static int thread_depth;			/* Stating depth in threads we enter */
	typedef struct
	{
		char *subject;
		int subject_compare_len;
		int part_number;
		int total;
		int base_index;
	} MultiPartInfo;
#endif /* !INDEX_DAEMON */

/*
 * Local prototypes
 */
static void draw_subject_arrow (void);

#ifndef INDEX_DAEMON
	static int get_multipart_info (int base_index, MultiPartInfo *setme);
	static int get_multiparts (int base_index, MultiPartInfo **malloc_and_setme_info);
	static int look_for_multipart_info (int base_index, MultiPartInfo* setme, char start, char stop);
	static int line_is_tagged (int n);
	static int tag_multipart (int base_index);
	static void bld_sline (int i);
	static void draw_sline (int i, t_bool full);
	static void erase_subject_arrow (void);
	static void move_to_thread (int n);
	static void prompt_subject_num (int ch);
	static void update_group_page (void);
	static void show_group_title (t_bool clear_title);
	static void show_tagged_lines (void);
	static void toggle_read_unread (t_bool force);
#endif /* !INDEX_DAEMON */

#ifndef INDEX_DAEMON
/*
 * Parses a subject header of the type "multipart message subject (01/42)"
 * into a MultiPartInfo struct, or fails if the message subject isn't in the
 * right form.
 *
 * @return nonzero on success
 */
static int
get_multipart_info (
	int base_index,
	MultiPartInfo *setme)
{
	int i = look_for_multipart_info(base_index, setme, '[', ']');
	if (i)
		return i;
	return look_for_multipart_info(base_index, setme, '(', ')');
}


static int
look_for_multipart_info (
	int base_index,
	MultiPartInfo* setme,
	char start,
	char stop)
{
	MultiPartInfo tmp;
	char *subj = (char *) 0;
	char *pch = (char *) 0;

	/* entry assertions */
	assert (0 <= base_index && base_index < top_base && "invalid base_index");
	assert (setme != NULL && "setme must not be NULL");

	/* parse the message */
	subj = arts[base[base_index]].subject;
	pch = strrchr (subj, start);
	if (!pch)
		return 0;
	if (!isdigit((int)pch[1]))
		return 0;
	tmp.base_index = base_index;
	tmp.subject_compare_len = pch - subj;
	tmp.part_number = (int) strtol(pch+1, &pch, 10);
	if (*pch != '/' && *pch!='|')
		return 0;
	if (!isdigit((int)pch[1]))
		return 0;
	tmp.total = (int) strtol (pch+1, &pch, 10);
	if (*pch != stop)
		return 0;
	tmp.subject = subj;
	*setme = tmp;
	return 1;
}


/*
 * Tries to find all the parts to the multipart message pointed to by
 * base_index.
 *
 * Weakness(?): only walks through the base messages.
 *
 * @return on success, the number of parts found.  On failure, zero if not a
 * multipart or the negative value of the first missing part.
 * @param base_index index pointing to one of the messages in a multipart
 * message.
 * @param malloc_and_setme_info on success, set to a malloced array the
 * parts found.  Untouched on failure.
 */
static int
get_multiparts (
	int base_index,
	MultiPartInfo **malloc_and_setme_info)
{
	MultiPartInfo tmp, tmp2;
	MultiPartInfo *info = 0;
	int i = 0;

	/* entry assertions */
	assert (0<=base_index && base_index<top_base && "Invalid base index");
	assert (malloc_and_setme_info!=NULL && "malloc_and_setme_info must not be NULL");

	/* make sure this is a multipart message... */
	if (!get_multipart_info(base_index, &tmp) || tmp.total < 1)
		return 0;

	/* make a temporary buffer to hold the multipart info... */
	info = my_malloc (sizeof(MultiPartInfo) * tmp.total);

	/* zero out part-number for the repost check below */
	for (i = 0; i < tmp.total; ++i)
		info[i].part_number = -1;

	/* try to find all the multiparts... */
	for (i = 0; i < top_base; ++i) {
		int part_index = 0;

		if (strncmp (arts[base[i]].subject, tmp.subject, tmp.subject_compare_len))
			continue;
		if (!get_multipart_info (i, &tmp2))
			continue;

		part_index = tmp2.part_number - 1;

		/* skip the "blah (00/102)" info messages... */
		if (part_index < 0)
			continue;

		/* skip insane "blah (103/102) subjects... */
		if (part_index >= tmp.total)
			continue;

		/* repost check: do we already have this part? */
		if (info[part_index].part_number != -1) {
			assert (info[part_index].part_number == tmp2.part_number && "bookkeeping error");
			continue;
		}

		/* we have a match, hooray! */
		info[part_index] = tmp2;
	}

	/* see if we got them all. */
	for (i = 0; i < tmp.total; ++i) {
		if (info[i].part_number != i+1) {
			free (info);
			return -(i+1); /* missing part #(i+1) */
		}
	}

	/* looks like a success .. */
	*malloc_and_setme_info = info;
	return tmp.total;
}


/*
 * Tags all parts of a multipart index if base_index points
 * to a multipart message and all its parts can be found.
 *
 * @param base_index points to one message in a multipart message.
 * @return number of messages tagged, or zero on failure
 */
static int
tag_multipart (
	int base_index)
{
	MultiPartInfo *info = 0;
	int i = 0;
	const int qty = get_multiparts(base_index, &info);

	/* check for failure... */
	if (qty == 0) {
		info_message ("Not a multi-part message"); /* FIXME: -> lang.c */
		return 0;
	}
	if (qty < 0) {
		char tmp[48];
		sprintf (tmp, "Missing part #%d", -qty);
		info_message(tmp); /* FIXME: -> lang.c? */
		return 0;
	}

	/*
	 * if any are already tagged, untag 'em first
	 * so num_of_tagged_arts doesn't get corrupted
	 */
	for ( i=0; i<qty; ++i ) {
		int *tagged = &arts[base[info[i].base_index]].tagged;
		if (*tagged) {
			decr_tagged (*tagged);
			*tagged = 0;
			--num_of_tagged_arts;
		}
	}

	/*
	 * get_multiparts sorts info by part number,
	 * so a sinmple for loop tags in the right order
	 */
	for (i = 0; i < qty; ++i) {
		const int my_base_index = info[i].base_index;
		arts[base[my_base_index]].tagged = ++num_of_tagged_arts;
	}

	free (info);

	info_message ("All parts tagged"); /* FIXME -> lang.c */
	update_group_page();
	return qty;
}
#endif /* !INDEX_DAEMON */


#ifndef INDEX_DAEMON
static int
line_is_tagged (
	int n)
{
	int code = 0;

	if (CURR_GROUP.attribute->thread_arts) {
		register int i;
		for (i = n; i >= 0; i = arts[i].thread) {
			if (arts[i].tagged > code)
				code = arts[i].tagged;
		}
	} else
		code = arts[n].tagged;

	return code;
}
#endif /* !INDEX_DAEMON */


#ifndef INDEX_DAEMON
static void
show_tagged_lines (
	void)
{
	register int i;

	for (i = first_subj_on_screen; i < last_subj_on_screen; ++i) {
		if ((i != index_point) && line_is_tagged(base[i])) {
			bld_sline (i);
			draw_sline (i, FALSE);
		}
	}
}
#endif /* !INDEX_DAEMON */

/*
 * Remove the current tag from the tag 'chain'
 * Work through all the threads and decrement the tag counter on all arts
 * greater than 'tag'
 */
void
decr_tagged (
	int tag)
{
	int i, j;

	for (i = 0; i < top_base; ++i) {
		for (j = (int) base[i]; j != -1; j = arts[j].thread)
			if (arts[j].tagged > tag)
				--arts[j].tagged;
	}
}


#ifndef INDEX_DAEMON
/*
 * index_point is overloaded internally as a return code
 * >= 0				'Normal'
 * GRP_RETURN		We are en route from pager to the selection screen
 * GRP_QUIT			User has done a 'Q'
 * GRP_NEXT			User wants to move onto next group
 * GRP_NEXTUNREAD	User did a 'C'atchup
 * GRP_ENTER		'g' command has been used to set group to enter
 */
int
group_page (
	struct t_group *group)
{
	char buf[128];
	char pat[128];
	int ch = 0;
	int i, n;
	int filter_state;
	int old_selected_arts;
	int old_top = 0;
	int old_group_top;
	unsigned int flag;
	long old_artnum = 0L;
	struct t_art_stat sbuf;
	t_bool ignore_unavail = FALSE;		/* Set if we keep going after an 'article unavailable' */
	t_bool range_active = FALSE;		/* Set if a range is defined */
	t_bool xflag = FALSE;

	/*
	 * Set the group attributes
	 */
	group->read_during_session = TRUE;

	proc_ch_default = get_post_proc_type (group->attribute->post_proc_type);

	glob_group = group->name;			/* For global access to the current group */
	num_of_tagged_arts = 0;

	last_resp = -1;
	this_resp = -1;

	/*
	 * update index file. quit group level if user aborts indexing
	 */
	if (!index_group (group))
		return GRP_RETURN;

	/*
	 * Position 'index_point' accordingly
	 */
	pos_first_unread_thread();

	set_subj_from_size (cCOLS);
	clear_note_area ();

	if (group->attribute->auto_select) {
		error_message (txt_autoselecting_articles);
		goto do_auto_select_arts;	/* 'X' command */
	}

	show_group_page ();

#	ifdef DEBUG_NEWSRC
	debug_print_comment ("group.c: before forever loop...");
	debug_print_bitmap (group, NULL);
#	endif /* DEBUG_NEWSRC */

	forever {
		set_xclick_on ();
		ch = ReadCh ();

		if (ch > '0' && ch <= '9') {			/* 0 goes to basenote */
			if (top_base)
				prompt_subject_num (ch);
			continue;
		}
		switch (ch) {
#	ifndef WIN32
			case ESC:	/* common arrow keys */
#		ifdef HAVE_KEY_PREFIX
			case KEY_PREFIX:
#		endif /* HAVE_KEY_PREFIX */
				switch (get_arrow_key (ch)) {
#	endif /* !WIN32 */
				case KEYMAP_UP:
					goto group_up;

				case KEYMAP_DOWN:
					goto group_down;

				case KEYMAP_LEFT:
					if (tinrc.group_catchup_on_exit)
						goto group_catchup;
					else
						goto group_done;

				case KEYMAP_RIGHT:
					if (tinrc.auto_list_thread && index_point >= 0 && HAS_FOLLOWUPS (index_point)) {
						thread_depth = 0;
						goto group_list_thread;
					} else
						goto group_read_basenote;

				case KEYMAP_PAGE_UP:
					goto group_page_up;

				case KEYMAP_PAGE_DOWN:
					goto group_page_down;

				case KEYMAP_HOME:
					goto top_of_list;

				case KEYMAP_END:
					goto end_of_list;
#	ifndef WIN32
				case KEYMAP_MOUSE:
					switch (xmouse) {
						case MOUSE_BUTTON_1:
						case MOUSE_BUTTON_3:
							if (xrow < INDEX2LNUM(first_subj_on_screen) || xrow > INDEX2LNUM(last_subj_on_screen-1))
								goto group_page_down;

							erase_subject_arrow ();
							index_point = xrow-INDEX2LNUM(first_subj_on_screen)+first_subj_on_screen;
							draw_subject_arrow ();
							if (xmouse == MOUSE_BUTTON_1) {
								if (tinrc.auto_list_thread && HAS_FOLLOWUPS (index_point)) {
									thread_depth = 0;
									goto group_list_thread;
								} else
									goto group_tab_pressed;
							}
							break;
						case MOUSE_BUTTON_2:
							if (xrow < INDEX2LNUM(first_subj_on_screen) || xrow > INDEX2LNUM(last_subj_on_screen-1))
								goto group_page_up;
							if (tinrc.group_catchup_on_exit)
								goto group_catchup;
							else
								goto group_done;

						default:
							break;
					}
					break;

				default:
					break;
				}
				break;
#	endif /* !WIN32 */

#	ifndef NO_SHELL_ESCAPE
			case iKeyShellEscape:
				shell_escape ();
				show_group_page ();
				break;
#	endif /* !NO_SHELL_ESCAPE */

			case iKeyFirstPage: /* show first page of threads */
top_of_list:
				if (top_base)
					move_to_thread(0);
				break;

			case iKeyLastPage:	/* show last page of threads */
end_of_list:
				if (top_base)
					move_to_thread(top_base - 1);
				break;

			case iKeyGroupLastViewed:	/* go to last viewed article */
				/*
				 * If the last art is no longer in a thread then we can't display it
				 */
				if (this_resp < 0 || (which_thread(this_resp) == -1)) {
					info_message (txt_no_last_message);
					break;
				}
				n = this_resp;
				goto enter_pager;

			case iKeyGroupPipe:	/* pipe article/thread/tagged arts to command */
				if (index_point >= 0)
					feed_articles (FEED_PIPE, GROUP_LEVEL, &CURR_GROUP, (int) base[index_point]);
				break;

			case iKeyGroupMail:	/* mail article to somebody */
				if (index_point >= 0)
					feed_articles (FEED_MAIL, GROUP_LEVEL, &CURR_GROUP, (int) base[index_point]);
				break;

#ifndef DISABLE_PRINTING
			case iKeyGroupPrint:	/* output art/thread/tagged arts to printer */
				if (index_point >= 0)
					feed_articles (FEED_PRINT, GROUP_LEVEL, &CURR_GROUP, (int) base[index_point]);
				break;
#endif /* !DISABLE_PRINTING */

			case iKeyGroupRepost:	/* repost current article */
				if (index_point >= 0)
					feed_articles (FEED_REPOST, GROUP_LEVEL, &CURR_GROUP, (int) base[index_point]);
				break;

			case iKeyGroupSave:	/* save articles with prompting */
				if (index_point >= 0)
					feed_articles (FEED_SAVE, GROUP_LEVEL, &CURR_GROUP, (int) base[index_point]);
				break;

			case iKeyGroupAutoSaveTagged:	/* Auto-save tagged articles without prompting */
				if (index_point >= 0) {
					if (num_of_tagged_arts)
						feed_articles (FEED_AUTOSAVE_TAGGED, GROUP_LEVEL, &CURR_GROUP, (int) base[index_point]);
					else
						info_message (txt_no_tagged_arts_to_save);
				}
				break;

			case iKeySetRange:	/* set range */
				if (bSetRange (GROUP_LEVEL, 1, top_base, index_point+1)) {
					range_active = TRUE;
					show_group_page ();
				}
				break;

			case iKeySearchAuthF:	/* author search forward / backward */
			case iKeySearchAuthB:
				i = (ch == iKeySearchAuthF) ? 1 : 0;
				n = SEARCH_AUTH;
				goto do_search;

			case iKeySearchSubjF:	/* subject forward/backward search */
			case iKeySearchSubjB:
				i = (ch == iKeySearchSubjF) ? 1 : 0;
				n = SEARCH_SUBJ;

do_search:		/* Search for type 'n' in direction 'i' */

				if (index_point < 0)
					break;

				{
					/* Not intuitive to search current thread in fwd search */
					int start = (i && index_point < top_base-1) ? prev_response((int)base[index_point+1]) : (int)base[index_point];

					if ((n = search (n, start, i)) != -1) {
						index_point = which_thread(n);

						/*
						 * If the search found something deeper in a thread (not the base art)
						 * then enter the thread
						 */
						if ((thread_depth = which_response(n)) != 0)
							goto group_list_thread;

						show_group_page ();
					}
				}
				break;

			case iKeySearchBody:	/* search article body */
				if (index_point < 0) {
					info_message (txt_no_arts);
					break;
				}

				if ((n = search_body ((int) base[index_point])) == -1)
					break;

				goto enter_pager;

			case iKeyGroupReadBasenote:
			case iKeyGroupReadBasenote2:	/* read current basenote */
group_read_basenote:
				if (index_point < 0) {
					info_message(txt_no_arts);
					break;
				}

				n = (int) base[index_point];
				ignore_unavail = TRUE;
				goto enter_pager;

			case iKeyGroupNextUnreadArtOrGrp:	/* goto next unread article/group */
group_tab_pressed:
				space_mode = TRUE;

				n = ((index_point < 0) ? -1 : next_unread ((int) base[index_point]));
				if (n < 0) {
					index_point = GRP_NEXTUNREAD;	/* => Enter next unread group */
					goto group_done;
				}

				/* We still have unread arts in the current group ... */
				ignore_unavail = TRUE;
				goto enter_pager;

			case iKeyPageDown:		/* page down */
			case iKeyPageDown2:
			case iKeyPageDown3:
group_page_down:
				if (top_base)
					move_to_thread (page_down (index_point, top_base));
				break;

			case iKeyGroupAutoSel:		/* auto-select article menu */
			case iKeyGroupKill:		/* kill article menu */
				if (index_point < 0) {
					info_message (txt_no_arts);
					break;
				}
				old_top = top;
				n = (int) base[index_point];
				old_artnum = arts[n].artnum;
				if (filter_menu ((ch == iKeyGroupKill) ? FILTER_KILL : FILTER_SELECT, group, &arts[n])) {
					if (filter_articles (group)) {
						make_threads (group, FALSE);
						index_point = find_new_pos (old_top, old_artnum, index_point);
					}
				}
				show_group_page ();
				break;

			case iKeyGroupQuickAutoSel:		/* quickly auto-select article */
			case iKeyGroupQuickKill:		/* quickly kill article */
				if (index_point < 0) {
					info_message (txt_no_arts);
					break;
				}
				if (!tinrc.confirm_action || prompt_yn (cLINES, (ch == iKeyGroupQuickKill) ? txt_quick_filter_kill : txt_quick_filter_select, TRUE) == 1) {
					old_top = top;
					n = (int) base[index_point];
					old_artnum = arts[n].artnum;
					if (quick_filter ((ch == iKeyGroupQuickKill) ? FILTER_KILL : FILTER_SELECT, group, &arts[n])) {
						info_message ((ch == iKeyGroupQuickKill) ? txt_info_add_kill : txt_info_add_select);
						if (filter_articles (group)) {
							make_threads (group, FALSE);
							index_point = find_new_pos (old_top, old_artnum, index_point);
						}
					}
					show_group_page ();
				}
				break;

			case iKeyRedrawScr:	/* redraw screen */
				my_retouch ();
				set_xclick_off ();
				show_group_page ();
				break;

			case iKeyDown:		/* line down */
			case iKeyDown2:
group_down:
				if (top_base)
					move_to_thread ((index_point + 1 >= top_base) ? 0 : (index_point + 1));
				break;

			case iKeyUp:		/* line up */
			case iKeyUp2:
group_up:
				if (top_base)
					move_to_thread ((index_point == 0) ? (top_base - 1) : (index_point - 1));
				break;

			case iKeyPageUp:		/* page up */
			case iKeyPageUp2:
			case iKeyPageUp3:
group_page_up:
				if (top_base)
					move_to_thread (page_up (index_point, top_base));
				break;

			case iKeyGroupCatchup:				/* & return to group menu */
			case iKeyGroupCatchupNextUnread:	/* catchup & go to next group with unread */
group_catchup:									/* came here on group exit via left arrow */
				{
					int yn = 1;

					if (num_of_tagged_arts && prompt_yn (cLINES, txt_catchup_despite_tags, TRUE) != 1)
						break;

					sprintf(buf, txt_mark_arts_read, (ch == iKeyGroupCatchupNextUnread) ? " and enter next unread group" : "");
					if (!CURR_GROUP.newsrc.num_unread || !tinrc.confirm_action || (yn = prompt_yn (cLINES, buf, TRUE)) == 1)
						grp_mark_read (&CURR_GROUP, arts);

					switch (ch) {
						case iKeyGroupCatchup:				/* Return to menu */
							if (yn == 1) {
								index_point = GRP_NEXT;
								goto group_done;
							}
							break;

						case iKeyGroupCatchupNextUnread:
							if (yn == 1)
								goto group_tab_pressed;
							break;

						default:							/* Must be <- group catchup on exit */
							switch (yn) {
								case -1:					/* ESCAPE - do nothing */
									break;
								case 1:						/* We caught up - advance group */
									index_point = GRP_NEXT;
									nobreak;	/* FALLTHROUGH */
								default:					/* Just leave the group */
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
					index_point = GRP_ENTER;
					goto group_done;
				}
				break;

			case iKeyGroupHelp:	/* help */
				show_info_page (HELP_INFO, help_group, txt_index_page_com);
				show_group_page ();
				break;

			case iKeyGroupToggleHelpDisplay:	/* toggle mini help menu */
				toggle_mini_help (GROUP_LEVEL);
				show_group_page ();
				break;

			case iKeyGroupToggleInverseVideo:	/* toggle inverse video */
				toggle_inverse_video ();
				show_group_page ();
				show_inverse_video_status ();
				break;

#	ifdef HAVE_COLOR
			case iKeyToggleColor:
				if (toggle_color ()) {
					show_group_page ();
					show_color_status ();
				}
				break;
#	endif /* HAVE_COLOR */

			case iKeyGroupMarkThdRead:	/* mark thread as read */

				if (index_point < 0) {
					info_message (txt_no_next_unread_art);
					break;
				}

				old_selected_arts = num_of_selected_arts;

				/*
				 * If a range is active, use it.
				 */
				if (range_active) {
					/*
					 * We check all arts, in case the user did something clever like
					 * change the threading mode on us since the range was created
					 */
					for (n = 0; n < top; ++n)
						if (arts[n].inrange) {
							arts[n].inrange = FALSE;	/* Clear the range */
							art_mark_read(&CURR_GROUP, &arts[n]);
						}
				} else
					thd_mark_read (&CURR_GROUP, base[index_point]);

				/*
				 * If # of 'hot' articles changed, update the header
				 */
				if (num_of_selected_arts != old_selected_arts)
					show_group_title (TRUE);

				bld_sline (index_point);
				draw_sline (index_point, FALSE);

				/*
				 * Move cursor to next unread
				 */
				if ((n = next_unread (next_response ((int) base[index_point]))) < 0) {
					draw_subject_arrow ();
					info_message (txt_no_next_unread_art);
					break;
				}

				/*
				 * If range defined, we have to redraw whole page anyway.
				 */
				if (range_active) {
					range_active = FALSE;			/* Range has gone now */
					show_group_page ();
				}

				if ((n = which_thread (n)) < 0) {
					error_message ("Internal error: which_thread(%d) < 0", n);
					break;
				}
				move_to_thread (n);
				break;

			case iKeyGroupListThd:			/* list articles within current thread */
				thread_depth = 0;			/* Enter thread at the top */
group_list_thread:							/* Possibly enter thread lower down here */
				if (index_point < 0) {
					info_message (txt_no_arts);
					break;
				}
				space_mode = TRUE;		/* Pos on first unread */
next_thread:
				switch (thread_page (group, (int) base[index_point], thread_depth)) {

					case GRP_QUIT:
						index_point = GRP_QUIT;
						goto group_done;

					case GRP_RETURN:
						index_point = GRP_RETURN;
						goto group_done;

					case GRP_NEXT:
						show_group_page ();
						goto group_down;

					case GRP_NEXTUNREAD:
						if ((n = next_unread ((int) base[index_point])) >= 0) {
							if ((n = which_thread (n)) >= 0) {
								index_point = n;
								thread_depth = 0;
								goto next_thread;
							}
						}
						/* No more unread thread in this group */
						/* FALLTHROUGH ?? */

					case GRP_KILLED:
						index_point = 0;
						/* FALLTHROUGH ?? */

					default:		/* ie, >= 0 */
						break;
				}

				clear_note_area ();
				show_group_page ();
				break;

			case iKeyLookupMessage:
				if ((i = prompt_msgid ()) != ART_UNAVAILABLE) {
					n = i;
					goto enter_pager;
				}
				break;

			case iKeyOptionMenu:	/* option menu */
				if (top_base > 0) {
					old_top = top;
					n = (int) base[index_point];
					old_artnum = arts[n].artnum;
				}
				n = tinrc.sort_article_type;
				filter_state = change_config_file (group);
				if (filter_state == NO_FILTERING && n != tinrc.sort_article_type)
					make_threads (&CURR_GROUP, TRUE);
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
					index_point = GRP_NEXTUNREAD;
					space_mode = tinrc.pos_first_unread;
					goto group_done;
				}
				break;

			case iKeyGroupNextUnreadArt:	/* goto next unread article */
				if (index_point < 0) {
					info_message(txt_no_next_unread_art);
					break;
				}

				if ((n = next_unread ((int) base[index_point])) == -1) {
					info_message (txt_no_next_unread_art);
					break;
				}

				goto enter_pager;

			case iKeyGroupPrevUnreadArt:	/* go to previous unread article */
				if (index_point < 0) {
					info_message(txt_no_prev_unread_art);
					break;
				}

				n = prev_response ((int) base[index_point]);

				if ((n = prev_unread (n)) == -1) {
					info_message(txt_no_prev_unread_art);
					break;
				}

				/*
				 * This is the main way to page articles, at this point
				 * 'n' should be the arts[n] we wish to read
				 */
enter_pager:
				if ((i = show_page (group, n, 0)) >= 0) {
					index_point = i;
					clear_note_area ();
					show_group_page ();
					break;
				}

				space_mode = tinrc.pos_first_unread; /* TODO space_mode sucks, kill it ? */

				/*
				 * In some cases, we have to keep going after an ARTFAIL
				 */
				if (ignore_unavail) {
					ignore_unavail = FALSE;
					if (i == GRP_ARTFAIL)
						i = GRP_NEXTUNREAD;
				}

				switch (i) {
					case GRP_ARTFAIL:			 /* Failed to open art - get a valid index_point and continue */
						clear_message ();
						break;

					case GRP_GOTOTHREAD:		/* Enter thread menu, at the top */
						thread_depth = which_response(this_resp);
						goto next_thread;

					case GRP_NEXT:				/* Thread was 'c'aught up */
						show_group_page ();
						goto group_down;

					case GRP_NEXTUNREAD:		/* Thread was 'C'aught up - enter next unread thread */
						goto group_tab_pressed;

					case GRP_QUIT:				/* 'Q' */
						index_point = GRP_QUIT;
						goto group_done;

					case GRP_RETURN:
					default:
						goto group_done;		/* All other cases -> group_done -> select:read_groups() */
				}

				break;

			case iKeyGroupPrevGroup:	/* previous group */
				clear_message();

				for (i=cur_groupnum-1; i>=0; i--) {
					if (UNREAD_GROUP (i))
						break;
				}

				if (i < 0)
					info_message(txt_no_prev_group);
				else {
					cur_groupnum = i;
					index_point = GRP_NEXTUNREAD;
					space_mode = tinrc.pos_first_unread;
					goto group_done;
				}
				break;

			case iKeyQuit:	/* return to group selection page */
				if (num_of_tagged_arts && prompt_yn (cLINES, txt_quit_despite_tags, TRUE) != 1)
					break;
				goto group_done;

			case iKeyQuitTin:		/* quit */
				if (num_of_tagged_arts && prompt_yn (cLINES, txt_quit_despite_tags, TRUE) != 1)
					break;
				index_point = GRP_QUIT;
				space_mode = FALSE;
				goto group_done;

			case iKeyGroupToggleReadUnread:
				toggle_read_unread(FALSE);
				show_group_page ();
				break;

			case iKeyGroupToggleGetartLimit:
				clear_message ();
				tinrc.use_getart_limit = !tinrc.use_getart_limit;
				index_point = GRP_NEXTUNREAD;
				space_mode = tinrc.pos_first_unread;
				goto group_done;
				/* NOTREACHED */
				break;

			case iKeyGroupBugReport:	/* bug/gripe/comment mailed to author */
				mail_bug_report ();
				ClearScreen ();
				show_group_page ();
				break;

			case iKeyGroupTagParts: /* tag all in order */
				if (0 <= index_point) {
					int new_tag_qty = tag_multipart (index_point);
					/*
					 * on success, move the pointer to the next
					 * untagged article just for ease of use's sake
					 */
					if (new_tag_qty != 0) {
						int k = index_point;
						do {
							k++;
							k %= top_base;
							if (!arts[base[k]].tagged ) {
								move_to_thread (k);
								break;
							}
						} while (k != index_point);
					}
				}
				break;

			case iKeyGroupTag:	/* tag/untag threads for mailing/piping/printing/saving */
				if (index_point >= 0) {
					int ii;
					t_bool tagged = TRUE;

					n = (int) base[index_point];

					/*
					 * This loop looks for any article in the thread that
					 * isn't already tagged.
					 */
					for (ii = n; ii != -1 && tagged; ii = arts[ii].thread) {
						if (!arts[ii].tagged) {
							tagged = FALSE;
							break;
						}
					}

					/*
					 * If the whole thread is tagged, untag it. Otherwise, tag
					 * any untagged articles
					 */
					if (tagged) {
						/*
						 * Here we repeat the tagged test in both blocks
						 * to leave the choice of tagged/untagged
						 * determination politic in the previous lines.
						 */
						info_message (txt_untagged_thread);
						for (ii = n; ii != -1; ii = arts[ii].thread) {
							if (arts[ii].tagged) {
								tagged = TRUE;
								decr_tagged (arts[ii].tagged);
								arts[ii].tagged = 0;
								--num_of_tagged_arts;
							}
						}
					} else {
						info_message (txt_tagged_thread);
						for (ii = n; ii != -1; ii = arts[ii].thread) {
							if (!arts[ii].tagged)
								arts[ii].tagged = ++num_of_tagged_arts;
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

			case iKeyGroupToggleThreading:	/* Cycle through the threading types */
				CURR_GROUP.attribute->thread_arts = (CURR_GROUP.attribute->thread_arts + 1) % (THREAD_MAX + 1);
				if (index_point >= 0) {
					i = base[index_point];								/* Save a copy of current thread */
					make_threads (&CURR_GROUP, TRUE);
					find_base (&CURR_GROUP);
					if ((index_point = which_thread(i)) < 0)			/* Restore current position in group */
						index_point = 0;
				}
				show_group_page ();
				break;

			case iKeyGroupUntag:	/* untag all articles */
				if (index_point >= 0) {
					if (untag_all_articles())
						update_group_page();
				}
				break;

			case iKeyVersion:
				info_message (cvers);
				break;

			case iKeyGroupPost:	/* post an article */
				if (post_article (group->name))
					show_group_page ();
				break;

			case iKeyPostponed:
			case iKeyPostponed2:	/* post postponed article */
				if (can_post) {
					if (pickup_postponed_articles(FALSE, FALSE))
						show_group_page ();
				} else
					info_message(txt_cannot_post);
				break;

			case iKeyDisplayPostHist:	/* display messages posted by user */
				if (user_posted_messages ())
					show_group_page ();
				break;

			case iKeyGroupMarkArtUnread:	/* mark base article of thread unread */
				if (index_point < 0) {
					info_message (txt_no_arts);
					break;
				}
				if (range_active) {
					int ii;

					/*
					 * We are tied to following base[] here, not arts[], as we operate on
					 * the base articles by definition.
					 */
					for (ii = 0; ii < top_base; ++ii) {
						if (arts[base[ii]].inrange) {
							arts[base[ii]].inrange = FALSE;
							art_mark_will_return (&CURR_GROUP, &arts[base[ii]]);
							for (i = arts[base[ii]].thread; i != -1; i = arts[i].thread)
								arts[i].inrange = FALSE;
						}
					}
					range_active = FALSE;
					show_group_page();
					strcpy(buf, "Base article range"); /* FIXME: -> lang.c */
				} else {
					art_mark_will_return (&CURR_GROUP, &arts[base[index_point]]);
					strcpy(buf, "Base article"); /* FIXME: -> lang.c */
				}

				show_group_title (TRUE);
				bld_sline(index_point);
				draw_sline (index_point, FALSE);
				draw_subject_arrow();
				info_message (txt_marked_as_unread, buf);
				break;

			case iKeyGroupMarkThdUnread:	/* mark whole thread as unread */
				if (index_point < 0) {
					info_message (txt_no_arts);
					break;
				}

				/*
				 * We process all articles in case the threading changed since
				 * the range was created
				 */
				if (range_active) {
					int ii;

					for (ii = 0; ii < top; ++ii) {
						if (arts[ii].inrange) {
							arts[ii].inrange = FALSE;
							art_mark_will_return (&CURR_GROUP, &arts[ii]);
						}
					}
					range_active = FALSE;
					show_group_page();
					strcpy(buf, "Thread range"); /* FIXME: -> lang.c */
				} else {
					thd_mark_unread (&CURR_GROUP, base[index_point]);
					strcpy(buf, "Thread");
				}

				show_group_title (TRUE);
				bld_sline(index_point);
				draw_sline (index_point, FALSE);
				draw_subject_arrow();
				info_message (txt_marked_as_unread, buf);
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
				for (i = (int) base[index_point]; i != -1; i = arts[i].thread) {
					arts[i].selected = flag;
					++n;
				}
				assert (n > 0);
				bld_sline(index_point);
				draw_sline (index_point, FALSE);

				info_message (flag
					      ? txt_thread_marked_as_selected
					      : txt_thread_marked_as_deselected);

				if (index_point + 1 < top_base)
					goto group_down;
				draw_subject_arrow ();
				break;

			case iKeyGroupReverseSel:	/* reverse selections */
				for (i=0; i < top; i++)
					arts[i].selected = !arts[i].selected;
				update_group_page ();
				break;

			case iKeyGroupUndoSel:	/* undo selections */
				for (i=0; i < top; i++) {
					arts[i].selected = FALSE;
					arts[i].zombie = FALSE;
				}
				xflag = FALSE;
				update_group_page ();
				break;

			case iKeyGroupSelPattern:	/* select matching patterns */
				sprintf (mesg, txt_select_pattern, tinrc.default_select_pattern);
				if (!prompt_string (mesg, buf, HIST_SELECT_PATTERN))
					break;

				if (buf[0] == '\0') {
					if (tinrc.default_select_pattern[0] == '\0') {
						info_message ("No previous expression"); /* FIXME: -> lang.c */
						break;
					}
					sprintf (pat, REGEX_FMT, tinrc.default_select_pattern);
				} else if (STRCMPEQ(buf, "*")) {	/* all */
					strcpy (pat, buf);
					strcpy (tinrc.default_select_pattern, pat);
				} else {
					strcpy (tinrc.default_select_pattern, buf);
					sprintf (pat, REGEX_FMT, tinrc.default_select_pattern);
				}

				flag = 0;
				for (n=0; n < top_base; n++) {
					if (!REGEX_MATCH (arts[base[n]].subject, pat, TRUE))
						continue;

					for (i = (int) base[n]; i != -1; i = arts[i].thread)
						arts[i].selected = TRUE;

					bld_sline(n);
					flag++;
				}
				if (flag)
					update_group_page ();

				break;

			case iKeyGroupSelThdIfUnreadSelected:	/* select all unread arts in thread hot if 1 is hot */
				for (n=0; n < top_base; n++) {
					stat_thread(n, &sbuf);
					if (!sbuf.selected_unread || sbuf.selected_unread == sbuf.unread)
						continue;

					for (i = (int) base[n]; i != -1; i = arts[i].thread)
						arts[i].selected = 1;

				}
				/* no screen update needed */
				break;

			case iKeyGroupMarkUnselArtRead:	/* mark read all unselected arts */
do_auto_select_arts:
				if (xflag)
					goto undo_auto_select_arts;

				for (i=0; i < top; ++i) {
					if (arts[i].status == ART_UNREAD && arts[i].selected != 1) {
#	ifdef DEBUG_NEWSRC
						debug_print_comment ("group.c: X command");
#	endif /* DEBUG_NEWSRC */
						art_mark_read (&CURR_GROUP, &arts[i]);
						arts[i].zombie = TRUE;
					}
				}
				if (CURR_GROUP.attribute->show_only_unread)
					find_base (&CURR_GROUP);

				xflag = TRUE;
				index_point = 0;
				show_group_page ();
				break;

			case iKeyGroupDoAutoSel:	/* perform auto-selection on group */
				/* selection already happened in filter_articles() */

undo_auto_select_arts:
				for (i=0; i<top; ++i) {
					if (arts[i].status == ART_READ && arts[i].zombie) {
#	ifdef DEBUG_NEWSRC
						debug_print_comment ("group.c: + command");
#	endif /* DEBUG_NEWSRC */
						art_mark_unread (&CURR_GROUP, &arts[i]);
						arts[i].zombie = FALSE;
					}
				}
				if (CURR_GROUP.attribute->show_only_unread)
					find_base (&CURR_GROUP);

				xflag = FALSE;
				index_point = 0;	/* do we want this ? */
				show_group_page ();
				break;

			case iKeyToggleInfoLastLine:
				tinrc.info_in_last_line = !tinrc.info_in_last_line;
				show_group_page ();
				break;

			default:
				info_message (txt_bad_command);
		} /* switch(ch) */
	} /* forever */

group_done:

	set_xclick_off ();

	clear_note_area ();
	vGrpDelMailArts (&CURR_GROUP);

	return (index_point);
}
#endif /* !INDEX_DAEMON */


void
show_group_page (
	void)
{
#ifndef INDEX_DAEMON
	int i;

	signal_context = cGroup;

	MoveCursor (0, 0);
	CleartoEOLN ();

	show_group_title (FALSE);

	MoveCursor (1, 0);
	CleartoEOLN ();
	MoveCursor (INDEX_TOP, 0);

	if (index_point >= top_base)
		index_point = top_base - 1;

	set_first_screen_item (index_point, top_base, &first_subj_on_screen, &last_subj_on_screen);

	if (tinrc.draw_arrow)
		CleartoEOS ();

	for (i = first_subj_on_screen; i < last_subj_on_screen; ++i) {
		bld_sline(i);
		draw_sline (i, TRUE);
	}

	CleartoEOS ();
	show_mini_help (GROUP_LEVEL);

	if (top_base <= 0) {
		info_message (txt_no_arts);
		return;
	} else if (last_subj_on_screen == top_base)
		info_message(txt_end_of_arts);

	draw_subject_arrow();

#endif /* !INDEX_DAEMON */
}


#ifndef INDEX_DAEMON
static void
update_group_page (
	void)
{
	register int i;

	for (i = first_subj_on_screen; i < last_subj_on_screen; ++i) {
		bld_sline (i);
		draw_sline (i, FALSE);
	}

	if (top_base <= 0)
		return;

	draw_subject_arrow ();
}
#endif /* !INDEX_DAEMON */


static void
draw_subject_arrow (
	void)
{
#ifndef INDEX_DAEMON
	MoveCursor (INDEX2LNUM(index_point), 0);

	if (tinrc.draw_arrow) {
		my_fputs ("->", stdout);
		my_flush ();
	} else {
		StartInverse();
		draw_sline (index_point, TRUE);
		EndInverse();
	}
	if (tinrc.info_in_last_line) {
		struct t_art_stat statbuf;

		stat_thread (index_point, &statbuf);
		info_message ("%s", arts[(statbuf.unread ? next_unread(base[index_point]) : base[index_point])].subject);
	} else {
		if (last_subj_on_screen == top_base)
			info_message(txt_end_of_arts);
	}
	stow_cursor();
#endif /* !INDEX_DAEMON */
}


#ifndef INDEX_DAEMON
static void
erase_subject_arrow (
	void)
{
	MoveCursor (INDEX2LNUM(index_point), 0);

	if (tinrc.draw_arrow)
		my_fputs ("  ", stdout);
	else {
		HpGlitch(EndInverse ());
		draw_sline (index_point, TRUE);
	}
	my_flush ();
}
#endif /* !INDEX_DAEMON */


#ifndef INDEX_DAEMON
static void
prompt_subject_num (
	int ch)
{
	int num;

	clear_message ();

	if ((num = prompt_num (ch, txt_select_thread)) == -1) {
		clear_message ();
		return;
	}

	num--;		/* index from 0 (internal) vs. 1 (user) */

	if (num < 0)
		num = 0;

	if (num >= top_base)
		num = top_base - 1;

	move_to_thread (num);
}
#endif /* !INDEX_DAEMON */


void
clear_note_area (
	void)
{
	MoveCursor (INDEX_TOP, 0);
	CleartoEOS ();
}

#ifndef INDEX_DAEMON
/*
 * If in show_only_unread mode or there are unread articles we know this thread
 * will exist after toggle. Otherwise we find the next closest to return to.
 * 'force' can be set to force tin to show all messages
 */
static void
toggle_read_unread (
	t_bool force)
{
	int i, n;

	if (force)
		CURR_GROUP.attribute->show_only_unread = TRUE;	/* Yes - really, we change it in a bit */

	wait_message (0, txt_reading_arts, (CURR_GROUP.attribute->show_only_unread) ? "all " : "unread ");

	i = -1;

	if (index_point >= 0) {
		if (CURR_GROUP.attribute->show_only_unread || new_responses (index_point))
			i = base[index_point];
		else if ((n = prev_unread ((int)base[index_point])) >= 0)
			i = n;
		else if ((n = next_unread ((int)base[index_point])) >= 0)
			i = n;
	}

	CURR_GROUP.attribute->show_only_unread = !CURR_GROUP.attribute->show_only_unread;

	find_base (&CURR_GROUP);
	if (i >= 0 && (n = which_thread (i)) >= 0)
		index_point = n;
	else if (top_base > 0)
		index_point = top_base - 1;
}
#endif /* !INDEX_DAEMON */


/*
 * Find new index position after a kill or unkill. Because
 * kill can work on author it is impossible to know which,
 * if any, articles will be left afterwards. So we make a
 * "best attempt" to find a new index point.
 */
int
find_new_pos (
	int old_top,
	long old_artnum,
	int cur_pos)
{
	int i, pos;

	if (top == old_top)
		return cur_pos;

	for (i = 0; i < top; i++) {
		if (arts[i].artnum == old_artnum) {
			pos = which_thread (arts[i].artnum);
			if (pos >= 0)
				return pos;
		}
	}

	return ((cur_pos < top_base) ? cur_pos : (top_base - 1));
}


/*
 * Set index_point to the first unread or the last thread depending on
 * the internal variable 'space_mode'
 */
void
pos_first_unread_thread (
	void)
{
	int i;

	if (space_mode) {
		for (i = 0; i < top_base; i++) {
			if (new_responses (i))
				break;
		}
		index_point = ((i < top_base) ? i : (top_base - 1));
	} else
		index_point = top_base - 1;
}


void
mark_screen (
	int level,
	int screen_row,
	int screen_col,
	const char *value)
{
	if (tinrc.draw_arrow) {
		MoveCursor(INDEX_TOP + screen_row, screen_col);
		my_fputs (value, stdout);
		stow_cursor();
		my_flush ();
	} else {
#ifdef USE_CURSES
		int y, x;
		getyx(stdscr, y, x);
		mvaddstr(INDEX_TOP + screen_row, screen_col, value);
		move(y, x);
#else
		int i;
		for (i = 0; value[i] != '\0'; i++)
			screen[screen_row].col[screen_col+i] = value[i];
#endif /* USE_CURSES */
		if (level == SELECT_LEVEL)
			draw_group_arrow();
		else
			draw_subject_arrow();
	}
}


void
set_subj_from_size (
	int num_cols)
{
	int size, show_author;

	/*
	 * This function is called early during startup when we only have
	 * very limited information loaded.
	 */
	show_author = ((group_top && CURR_GROUP.attribute) ? CURR_GROUP.attribute->show_author : tinrc.show_author);
	max_subj = ((show_author == SHOW_FROM_BOTH) ? ((num_cols / 2) - 4): ((num_cols / 2) + 3));
	max_from = (num_cols - max_subj) - 17;

	if (show_author != SHOW_FROM_BOTH) {
		if (max_from > 25) {
			size = max_from - 25;
			max_from = 25;
			max_subj += size;
		}
	}

	if (show_author != SHOW_FROM_NONE) {
		len_from = max_from - BLANK_GROUP_COLS;
		len_subj = max_subj;
		spaces = "  ";
	} else {
		len_from = 0;
		len_subj = (max_subj + max_from + 2) - BLANK_GROUP_COLS;
		spaces = "";
	}

	if (!tinrc.show_lines)
		len_subj += 5;

	if (!tinrc.show_score)
		len_subj += 7;
}


void
toggle_subject_from (
	void)
{
	if (++CURR_GROUP.attribute->show_author > SHOW_FROM_BOTH)
		CURR_GROUP.attribute->show_author = SHOW_FROM_NONE;

	set_subj_from_size (cCOLS);
}


#ifndef INDEX_DAEMON

/*
 * Build subject line given an index into base[].
 *
 * WARNING: the routine is tightly coupled with draw_sline() in the sense
 * that draw_sline() expects bld_sline() to place the article mark
 * (ART_MARK_READ, ART_MARK_SELECTED, etc) at MARK_OFFSET in the
 * screen[].col.
 * So, if you change the format used in this routine, be sure to check
 * that the value of MARK_OFFSET (tin.h) is still correct.
 * Yes, this is somewhat kludgy.
 */
static void
bld_sline (
	int i)
{
#ifdef USE_CURSES
	char buffer[BUFSIZ];	/* FIXME: allocate? */
#else
	char *buffer;
#endif /* USE_CURSES */
	char from[HEADER_LEN];
	char new_resps[8];
	char art_cnt[10];
	char arts_sub[255];
	int respnum;
	int n, j;
	struct t_art_stat sbuf;

	from[0] = '\0';
	respnum = (int) base[i];

	stat_thread(i, &sbuf);

	/*
	 * n is number of articles in this thread
	 */
	n = ((CURR_GROUP.attribute->show_only_unread) ? (sbuf.unread + sbuf.seen) : sbuf.total);
	/*
	 * if you like to see the number of responses excluding the first
	 *	art in thread - add the following:
	 *	n--;
	 */

	if ((j = line_is_tagged(respnum)))
		strcpy (new_resps, tin_ltoa(j, 3));
	else
		sprintf (new_resps, "  %c", sbuf.art_mark);

	/*
	 * Find index of first unread in this thread
	 */
	j = (sbuf.unread) ? next_unread(respnum) : respnum;


	if (tinrc.show_lines) {
		if (n > 1) { /* change this to (n > 0) if you do a n-- above */
			if (arts[j].lines != -1) {
				char tmp_buffer[4];
				strcpy (tmp_buffer, tin_ltoa(n, 3));
				sprintf (art_cnt, "%s %s ", tmp_buffer, tin_ltoa(arts[j].lines, 4));
			} else
				sprintf (art_cnt, "%s    ? ", tin_ltoa(n, 3));
		} else {
			if (arts[j].lines != -1)
				sprintf (art_cnt, "    %s ", tin_ltoa(arts[j].lines, 4));
			else
				strcpy (art_cnt, "       ? ");
		}
	} else {
		if (n > 1) /* change this to (n > 0) if you do a n-- above */
			sprintf (art_cnt, "%s ", tin_ltoa(n, 3));
		else
			strcpy (art_cnt, "    ");
	}

	if (CURR_GROUP.attribute->show_author != SHOW_FROM_NONE)
		get_author (FALSE, &arts[j], from, len_from);

	strncpy(arts_sub, arts[j].subject, len_subj+12);
	j = INDEX2SNUM(i);
	arts_sub[len_subj-12+1] = '\0';

#ifndef USE_CURSES
	buffer = screen[j].col;
#endif /* !USE_CURSES */
	if (tinrc.show_score)
		sprintf (buffer, "  %s %s %s%6d %-*.*s%s%-*.*s",
			 tin_ltoa(i+1, 4), new_resps, art_cnt, sbuf.score,
			 len_subj-12, len_subj-12, arts_sub,
			 spaces, len_from, len_from, from);
	else
		sprintf (buffer, "  %s %s %s%-*.*s%s%-*.*s",
			 tin_ltoa(i+1, 4), new_resps, art_cnt,
			 len_subj-12, len_subj-12, arts_sub,
			 spaces, len_from, len_from, from);

	/* protect display from non-displayable characters (e.g., form-feed) */
	Convert2Printable (buffer);

	WriteLine(INDEX2LNUM(i), buffer);
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
static void
draw_sline (
	int i,
	t_bool full)
{
	int tlen;
	int x = full ? 0 : 6;
	int k = MARK_OFFSET;
#	ifdef USE_CURSES
	char buffer[BUFSIZ];
	char *s = screen_contents(INDEX2LNUM(i), x, buffer);
#	else
	char *s = &(screen[INDEX2SNUM(i)].col[x]);
#	endif /* USE_CURSES */

	if (full) {
		if (tinrc.strip_blanks) {
			strip_line (s);
			CleartoEOLN ();
		}
		tlen = strlen (s);	/* notes new line length */
	} else
		tlen = 12; /* ??? */

	MoveCursor (INDEX2LNUM(i), x);
	if (tlen)
		my_printf("%.*s", tlen, s);

	/*
	 * it is somewhat less efficient to go back and redo that art mark
	 * if selected, but it is quite readable as to what is happening
	 */
	if (s[k-x] == tinrc.art_marked_selected) {
		MoveCursor (INDEX2LNUM(i), k);
		ToggleInverse ();
		my_fputc (s[k-x], stdout);
		ToggleInverse ();
	}

	MoveCursor(INDEX2LNUM(i)+1, 0);
}


static void
show_group_title (
	t_bool clear_title)
{
	char buf[PATH_LEN];
	int num;
	register int i, art_cnt = 0;

	num = my_group[cur_groupnum];

	if (active[num].attribute->show_only_unread) {
		for (i = 0; i < top_base; i++)
			art_cnt += new_responses (i);
	} else {
		for (i = 0; i < top; i++) {
			if (!IGNORE_ART(i))
				++art_cnt;
		}
	}

	if (tinrc.use_getart_limit)
		sprintf (buf, "%s (%dT(%c) %dA %dK %dH [%dL]%s%c)",
			active[num].name, top_base,
			*txt_thread[active[num].attribute->thread_arts],
			art_cnt, num_of_killed_arts, num_of_selected_arts,
			tinrc.getart_limit,
			(active[num].attribute->show_only_unread ? " R" : ""),
			group_flag(active[num].moderated));
	else
		sprintf (buf, "%s (%dT(%c) %dA %dK %dH%s%c)",
			active[num].name, top_base,
			*txt_thread[active[num].attribute->thread_arts],
			art_cnt, num_of_killed_arts, num_of_selected_arts,
			(active[num].attribute->show_only_unread ? " R" : ""),
			group_flag(active[num].moderated));

	if (clear_title) {
		MoveCursor (0, 0);
		CleartoEOLN ();
	}

	show_title (buf);
}


/*
 * Move the on-screen pointer & internal variable to the given thread number
 */
static void
move_to_thread (
	int n)
{
	if (index_point == n)
		return;

	HpGlitch(erase_subject_arrow ());
	erase_subject_arrow ();
	index_point = n;

	if (n >= first_subj_on_screen && n < last_subj_on_screen)
		draw_subject_arrow ();
	else
		show_group_page ();
}

#endif /* !INDEX_DAEMON */
