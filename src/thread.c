/*
 *  Project   : tin - a Usenet reader
 *  Module    : thread.c
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 27-09-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"menukeys.h"

#define INDEX2TNUM(i)	((i) % NOTESLINES)
#define TNUM2LNUM(i)	(INDEX_TOP + (i))
#define INDEX2LNUM(i)	(TNUM2LNUM(INDEX2TNUM(i)))

#define MARK_OFFSET 8

int thread_basenote = 0;
int show_subject;

static int top_thread = 0;
static int thread_index_point = 0;
static int thread_respnum = 0;
static int first_thread_on_screen = 0;
static int last_thread_on_screen = 0;

static int draw_tline P_((int i, int full));
static int bld_tline P_((int l, struct t_article *art));


static int 
bld_tline (l, art)
	int l;
	struct t_article *art;
{
#ifndef INDEX_DAEMON
	char mark;
	char from[LEN];
	char new_resps[8];
	char lines[8];
	char *spaces;
	int j;
	int len_from;
	int len_subj = 0;
	int off_subj = 0;
	int off_both = 0;

	if (! draw_arrow_mark) {
		off_subj = 2;
		off_both = 5;
	}	

	if (show_subject) {
		if (show_author != SHOW_FROM_NONE) {
			len_from = max_from - 3;
			len_from += 8 * (1 - show_lines);
			len_subj = (max_subj+off_subj) - 5;
			spaces = "   ";
		} else {
			len_from = 0;
			len_subj = (max_from+max_subj+off_subj) - 5;
			len_subj += 8 * (1 - show_lines);
			spaces = "";
		}
	} else {
		len_from = (max_subj+max_from+off_both) - 8;
		len_from += 8 * (1 - show_lines);
		spaces = "";
	}	
	
	j = INDEX2TNUM(l);

	if (art->tagged) {
		sprintf (new_resps, "%3d", art->tagged);
	} else {
		if (art->inrange) {
			mark = art_marked_inrange;
		} else if (art->status == ART_UNREAD) {
			mark = (art->selected ? art_marked_selected : art_marked_unread);
		} else if (art->status == ART_WILL_RETURN) {
			mark =  art_marked_return;
		} else {
			mark = ART_MARK_READ;
		}
		sprintf (new_resps, "  %c", mark);
	}
	
	from[0] = '\0';
	if (!show_subject || show_author != SHOW_FROM_NONE) {
		get_author (TRUE, art, from);
	}	

	if (art->lines != -1) {
		sprintf (lines, "%-4d", art->lines);
	} else {
		strcpy (lines, "   ?");
	}

	if (show_lines) {
		sprintf (screen[j].col, "  %4d%3s  [%-4s]  %-*.*s%s%-*.*s",
			 l, new_resps, lines, len_subj, len_subj, art->subject, 
			 spaces, len_from, len_from, from);
	} else {
		sprintf (screen[j].col, "  %4d%3s  %-*.*s%s%-*.*s",
			 l, new_resps, len_subj, len_subj, art->subject, 
			 spaces, len_from, len_from, from);
	}

#endif
	return(0);
}


static int 
draw_tline (i, full)
	int i;
	int full;
{
#ifndef INDEX_DAEMON
	size_t tlen;
	int j, x;
	int k = MARK_OFFSET;
	char *s;

	j = INDEX2TNUM(i);

	if (full) {
		s = screen[j].col;
		tlen = strlen (s);
		x = 0;
		if (strip_blanks) {		
			strip_line (s, tlen);
			tlen = strlen (s);	/* note new line length */
			CleartoEOLN ();
		}
	} else {
		tlen  = 3;
		s = &screen[j].col[6];
		x = 6;
	}

	MoveCursor(INDEX2LNUM(i), x);
	fwrite (s, 1, tlen, stdout);

	/* it is somewhat less efficient to go back and redo that art mark
	 * if selected, but it is quite readable as to what is happening 
	 */
	if (screen[j].col[k] == art_marked_selected) {
		MoveCursor (INDEX2LNUM(i), k);
		ToggleInverse ();
		my_fputc (screen[j].col[k], stdout);
		ToggleInverse ();
	}

	MoveCursor(INDEX2LNUM(i)+1, 0);
#endif
	return(0);
}

/*
 * show current thread. If threaded on Subject: show
 *   <respnum> <name> 
 * If threaded on References: or Archive-name: show
 *   <respnum> <subject> <name>
 */
 
int 
show_thread (group, group_path, respnum)
	struct t_group *group;
	char *group_path;
	int respnum;
{
	int ret_code = TRUE;
#ifndef INDEX_DAEMON
	int ch;
	int i, n;
	int scroll_lines;
	int flag;

	thread_respnum = respnum;
	thread_basenote = which_thread (thread_respnum);
	top_thread = num_of_responses (thread_basenote) + 1;

	if (top_thread <= 0) {
		info_message (txt_no_resps_in_thread);
		return FALSE;
	}

	/*
	 * If threading by Refs, it helps to see the subject line
	 */
	if ((arts[thread_respnum].archive != (char *)0) || (group->attribute->thread_arts >= THREAD_REFS))
		show_subject = TRUE;
	else
		show_subject = FALSE;

	thread_index_point = top_thread;
	if (space_mode) {
		i = new_responses (thread_basenote);
		if (i) {
			for (n=0, i = base[thread_basenote]; i >= 0 ; i = arts[i].thread, n++) {
				if (arts[i].status == ART_UNREAD) {
					if (arts[i].thread == ART_EXPIRED) {
						art_mark_read (group, &arts[i]);
					} else {
						thread_index_point = n;
					}
					break;
				}
			}
		}
	}

	if (thread_index_point < 0) {
		thread_index_point = 0;
	}

	show_thread_page ();

	forever {
		set_xclick_on ();
		ch = ReadCh ();

		if (ch >= '0' && ch <= '9') {	/* 0 goes to basenote */
			if (top_thread == 1) {
				info_message (txt_no_responses);
			} else {
				prompt_thread_num (ch);
			}
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
						goto thread_up;

					case KEYMAP_DOWN:
						goto thread_down;

					case KEYMAP_LEFT:
						if (thread_catchup_on_exit) {
							goto thread_catchup;
						} else {
							goto thread_done;
						}
					
					case KEYMAP_RIGHT:
						goto thread_read_article;

					case KEYMAP_PAGE_UP:
						goto thread_page_up;

					case KEYMAP_PAGE_DOWN:
						goto thread_page_down;

					case KEYMAP_HOME:
						goto top_of_thread;
					
					case KEYMAP_END:
						goto end_of_thread;

#ifndef WIN32
					case KEYMAP_MOUSE:
						switch (xmouse)
						{
							case MOUSE_BUTTON_1:
							case MOUSE_BUTTON_3:
								if (xrow < INDEX2LNUM(first_thread_on_screen) ||
									xrow > INDEX2LNUM(last_thread_on_screen-1)) {
									goto thread_page_down;
								}
								erase_thread_arrow ();
								thread_index_point = xrow-INDEX2LNUM(first_thread_on_screen)+first_thread_on_screen;
								draw_thread_arrow ();
								if (xmouse == MOUSE_BUTTON_1)
									goto thread_read_article;
								break;
							case MOUSE_BUTTON_2:
								if (xrow < INDEX2LNUM(first_thread_on_screen) ||
									xrow > INDEX2LNUM(last_thread_on_screen-1)) {
									goto thread_page_up;
								}
								if (thread_catchup_on_exit) {
									goto thread_catchup;
								} else {
									goto thread_done;
								}
								/* break; */
						}
						break;
				}
				break;
#endif /* ! WIN32 */

#ifndef NO_SHELL_ESCAPE
			case iKeyThreadShell:
				shell_escape ();
				show_thread_page ();
				break;
#endif

			case iKeyThreadFirstPage:	/* show first page of articles */
top_of_thread:
				if (thread_index_point != 0) {
					if (0 < first_thread_on_screen) {
						erase_thread_arrow ();
						thread_index_point = 0;
						show_thread_page ();
					} else {
						erase_thread_arrow ();
						thread_index_point = 0;
						draw_thread_arrow ();
					}
				}
				break;

			case iKeyThreadLastPage:	/* show last page of articles */
end_of_thread:			
				if (thread_index_point < top_thread - 1) {
					if (top_thread > last_thread_on_screen) {
						erase_thread_arrow ();
						thread_index_point = top_thread - 1;
						show_thread_page ();
					} else {
						erase_thread_arrow ();
						thread_index_point = top_thread - 1;
						draw_thread_arrow ();
					}
				}
				break;
				
			case iKeyThreadSetRange:	/* set range */
				if (iSetRange (THREAD_LEVEL, 0, top_thread, thread_index_point)) {
					show_thread_page ();
				}
				break;

			case iKeyThreadReadArt:
			case iKeyThreadReadArt2:	/* read current article within thread */
thread_read_article:
				n = choose_response (thread_basenote, thread_index_point);
				n = show_page (group, group_path, n, &thread_index_point);
				if (n != GRP_NOREDRAW) {
					if (local_filtered_articles) {
						ret_code = GRP_KILLED;
						goto thread_done;
					} else if (n == thread_basenote) {
						show_thread_page ();
					} else {
						index_point = n;
						goto thread_done;
					}
				}
				break;

			case iKeyThreadReadNextArtOrThread:
thread_tab_pressed:
 				space_mode = TRUE;
				if (thread_index_point == 0) {
					n = thread_respnum;
				} else {
					n = choose_response (thread_basenote, thread_index_point);
				}
				for (i = n ; i != -1 ; i = arts[i].thread) {
					if (arts[i].status == ART_UNREAD) {
						n = show_page (group, group_path, i, &thread_index_point);
						break;
					}
				}
				if (i == -1) {
					ret_code = GRP_GOTONEXT;
					goto thread_done;
				} else if (n == GRP_NOREDRAW) {
					goto thread_tab_pressed;
				} else if (local_filtered_articles) {
					ret_code = GRP_KILLED;
					goto thread_done;
				} else if (n == thread_basenote) {
					show_thread_page ();
				} else {
					index_point = n;
					goto thread_done;
				}
				break;
	
			case iKeyThreadPageDown:		/* page down */
			case iKeyThreadPageDown2:
			case iKeyThreadPageDown3:
thread_page_down:
				if (thread_index_point + 1 == top_thread) {
					if (0 < first_thread_on_screen) {
						erase_thread_arrow ();
						thread_index_point = 0;
						show_thread_page ();
					} else {
						erase_thread_arrow ();
						thread_index_point = 0;
						draw_thread_arrow ();
					}
					break;
				}
				erase_thread_arrow ();
				scroll_lines = (full_page_scroll ? NOTESLINES : NOTESLINES / 2);
				thread_index_point = ((thread_index_point + scroll_lines) /
							scroll_lines) * scroll_lines;
				if (thread_index_point >= top_thread) {
					thread_index_point = (top_thread / scroll_lines) * scroll_lines;
					if (thread_index_point < top_thread - 1) {
						thread_index_point = top_thread - 1;
					}
				}
				if (thread_index_point < first_thread_on_screen ||
					thread_index_point >= last_thread_on_screen) {
					show_thread_page ();
				} else {
					draw_thread_arrow ();
				}
				break;

			case iKeyThreadRedrawScr:		/* redraw screen */
				ClearScreen ();
				set_xclick_off ();
				show_thread_page ();
				break;

			case iKeyThreadDown:
			case iKeyThreadDown2:		/* line down */
thread_down:
				if (thread_index_point + 1 >= top_thread) {
					if (_hp_glitch) {
						erase_thread_arrow ();
					}
					if (0 < first_thread_on_screen) {
						thread_index_point = 0;
						show_thread_page ();
					} else {
						erase_thread_arrow ();
						thread_index_point = 0;
						draw_thread_arrow ();
					}
					break;
				}
				if (thread_index_point + 1 >= last_thread_on_screen) {
					erase_thread_arrow ();
					thread_index_point++;
					show_thread_page ();
				} else {
					erase_thread_arrow ();
					thread_index_point++;
					draw_thread_arrow ();
				}
				break;

			case iKeyThreadUp:
			case iKeyThreadUp2:		/* line up */
thread_up:
				if (thread_index_point == 0) {
					if (_hp_glitch) {
						erase_thread_arrow ();
					}
					if (top_thread > last_thread_on_screen) {
						thread_index_point = top_thread - 1;
						show_thread_page ();
					} else {
						erase_thread_arrow ();
						thread_index_point = top_thread - 1;
						draw_thread_arrow ();
					}
					break;
				}
				if (_hp_glitch) {
					erase_thread_arrow ();
				}
				if (thread_index_point <= first_thread_on_screen) {
					thread_index_point--;
					show_thread_page ();
				} else {
					erase_thread_arrow ();
					thread_index_point--;
					draw_thread_arrow ();
				}
				break;

			case iKeyThreadPageUp:		/* page up */
			case iKeyThreadPageUp2:
			case iKeyThreadPageUp3:
thread_page_up:
				if (thread_index_point == 0) {
					if (_hp_glitch) {
						erase_thread_arrow ();
					}
					if (top_thread > last_thread_on_screen) {
						thread_index_point = top_thread - 1;
						show_thread_page ();
					} else {
						erase_thread_arrow ();
						thread_index_point = top_thread - 1;
						draw_thread_arrow ();
					}
					break;
				}
				clear_message ();
				erase_thread_arrow ();
				scroll_lines = (full_page_scroll ? NOTESLINES : NOTESLINES / 2);
				if ((n = thread_index_point % scroll_lines) > 0) {
					thread_index_point = thread_index_point - n;
				} else {
					thread_index_point = ((thread_index_point - scroll_lines) / scroll_lines) * scroll_lines;
				}
				if (thread_index_point < 0) {
					thread_index_point = 0;
				}
				if (thread_index_point < first_thread_on_screen
				|| thread_index_point >= last_thread_on_screen)
					show_thread_page ();
				else
					draw_thread_arrow ();
				break;

			case iKeyThreadCatchupConditional:	/* catchup thread but ask for confirmation */
thread_catchup:
				if (ch == iKeyThreadCatchupConditional) {
					if (confirm_action && prompt_yn (cLINES, txt_mark_thread_read, TRUE) != 1) {
						break;
					}
				}
				thd_mark_read (group, base[thread_basenote]);
				goto thread_done;

			case iKeyThreadCatchup:	/* mark thread as read immediately */
				thd_mark_read (group, base[thread_basenote]);
				goto thread_done;

			case iKeyThreadMarkArtRead: /* mark article as read */
				n = choose_response (thread_basenote, thread_index_point);
				arts[n].selected = 0;
				arts[n].status = ART_READ;
				art_mark_xref_read (&arts[n]);
				bld_tline (thread_index_point, &arts[n]);
				draw_tline (thread_index_point, FALSE);
				goto thread_down;
				
			case iKeyThreadToggleSubjDisplay:	/* toggle display of subject & subj/author */
				if (show_subject) {
					toggle_subject_from ();
					show_thread_page ();
				}
				break;

#ifdef HAVE_REF_THREADING				
case 'a':	/* Very dirty temp. hack - Show threaded tree */
{
	char tmp[3];
	int i=0;

	if (group->attribute->thread_arts >= THREAD_REFS) {

		struct t_msgid *ptr;

		/*
		 * The root article may not be the original root of the
		 * thread (may have expired, for example) - so find it.
		 * Make sure we don't run haywire if the ptrs are broken
		 */
		for (ptr = arts[thread_respnum].msgid; ptr->parent != NULL; ptr = ptr->parent) {
			if (++i > 100) {
				fprintf(stderr, "\nCan't find thread root - Infinite loop!\n");
				break;
			}
		}

		if (i <= 100) {
			fprintf(stderr, "\n");
			dump_thread(stderr, ptr, 1);
		}

		puts("Press <RETURN>");
		fgets(tmp, 2, stdin);

		show_thread_page ();
	}

	break;
}
#endif
			case iKeyThreadHelp:	/* help */
				show_info_page (HELP_INFO, help_thread, txt_thread_com);
				show_thread_page ();
				break;

			case iKeyThreadToggleHelpDisplay:	/* toggle mini help menu */
				toggle_mini_help (THREAD_LEVEL);
				show_thread_page();
				break;

			case iKeyThreadToggleInverseVideo:	/* toggle inverse video */
				toggle_inverse_video ();
				show_thread_page ();
				show_inverse_video_status ();
				break;

#ifdef HAVE_COLOR
			case iKeyThreadToggleColor:		/* toggle color */
				toggle_color ();
				show_thread_page ();
				show_color_status ();
				break;
#endif

			case iKeyThreadQuit:	/* return to previous level */
				goto thread_done;

			case iKeyThreadQuitTin:	/* quit */
				ret_code = GRP_QUIT;
				goto thread_done;

 			case iKeyThreadTag:	/* tag/untag art for mailing/piping/printing/saving */
				n = choose_response (thread_basenote, thread_index_point);

 				if (n < 0)
 					break;
 
 				if (arts[n].tagged) {
 					arts[n].tagged = 0;
 					--num_of_tagged_arts;
 					info_message (txt_untagged_art);
 				} else {
 					arts[n].tagged = ++num_of_tagged_arts;
					info_message (txt_tagged_art);
 				}
				bld_tline (thread_index_point, &arts[n]);
				draw_tline (thread_index_point, FALSE);
				if (thread_index_point + 1 < top_thread)
					goto thread_down;
				draw_thread_arrow ();
				break;

			case iKeyThreadBugReport:	/* bug/gripe/comment mailed to author */
				mail_bug_report ();
				ClearScreen ();
				show_thread_page ();
				break;

			case iKeyThreadVersion:	/* version */
				info_message (cvers);
				break;

			case iKeyThreadMarkArtUnread:	/* mark article as unread */
				n = choose_response (thread_basenote, thread_index_point);
				art_mark_will_return (group, &arts[n]); /*art_mark_unread (group, &arts[n]);*/
				bld_tline (thread_index_point, &arts[n]);
				draw_tline (thread_index_point, FALSE);
				info_message (txt_art_marked_as_unread);
				draw_thread_arrow ();
				break;

			case iKeyThreadMarkThdUnread:	/* mark thread as unread */
				thd_mark_unread (group, base[thread_basenote]);
				update_thread_page ();
				info_message (txt_thread_marked_as_unread);
				break;
				
			case iKeyThreadMarkArtSel:	/* mark article as selected */
			case iKeyThreadToggleArtSel:	/* toggle article as selected */
				n = choose_response (thread_basenote, thread_index_point);

				if (n < 0)
					break;
				if (ch == iKeyThreadToggleArtSel && arts[n].selected == 1)
					flag = 0;
				else
					flag = 1;
				arts[n].selected = flag;
/*				update_thread_page (); */
				bld_tline (thread_index_point, &arts[n]);
				draw_tline (thread_index_point, FALSE);
				if (thread_index_point + 1 < top_thread)
					goto thread_down;
				draw_thread_arrow ();
#if 0
				info_message (flag 
					      ? txt_art_marked_as_selected
					      : txt_art_marked_as_deselected)
#endif
				break;

			case iKeyThreadReverseSel:	/* reverse selections */
				for (i = (int) base[thread_basenote] ; i != -1 ; i = arts[i].thread) {
					arts[i].selected = (arts[i].selected ? 0 : 1);
				}
				update_thread_page ();
				break;

			case iKeyThreadUndoSel:	/* undo selections */
				for (i = (int) base[thread_basenote] ; i != -1 ; i = arts[i].thread) {
					arts[i].selected = 0;
				}
				update_thread_page ();
				break;

			case iKeyThreadDisplaySubject:
				info_message(arts[(choose_response (thread_basenote, thread_index_point))].subject);
				break;
				
			default:
			    info_message (txt_bad_command);
		}
	}

thread_done:
	set_xclick_off ();
	clear_note_area ();

#endif /* INDEX_DAEMON */

	return ret_code;
}


void 
show_thread_page ()
{
#ifndef INDEX_DAEMON

	int i, j;
	static int index = 0;

	set_signals_thread ();
	
	ClearScreen ();

	if (thread_index_point > top_thread - 1) {
		thread_index_point = top_thread - 1;
	}

	if (NOTESLINES <= 0) {
		first_thread_on_screen = 0;
	} else {
		first_thread_on_screen = (thread_index_point / NOTESLINES) * NOTESLINES;
		if (first_thread_on_screen < 0) {
			first_thread_on_screen = 0;
		}
	}

	last_thread_on_screen = first_thread_on_screen + NOTESLINES;

	if (last_thread_on_screen >= top_thread) {
		last_thread_on_screen = top_thread;
		first_thread_on_screen = (top_thread / NOTESLINES) * NOTESLINES;

		if (first_thread_on_screen == last_thread_on_screen ||
			first_thread_on_screen < 0) {
			if (first_thread_on_screen < 0) {
				first_thread_on_screen = 0;
			} else {
				first_thread_on_screen = last_thread_on_screen - NOTESLINES;
			}
		}
	}

	if (top_thread == 0) {
		first_thread_on_screen = 0;
		last_thread_on_screen = 0;
	}

	index = choose_response (thread_basenote, first_thread_on_screen);

	assert(first_thread_on_screen != 0 || index == thread_respnum);

	if (show_subject)
		sprintf (msg, "List Thread (%d of %d)", index_point+1, top_base);
	else
		sprintf (msg, "Thread (%.*s)", cCOLS-23, arts[thread_respnum].subject);

	show_title (msg);

	MoveCursor (INDEX_TOP, 0);

	for (j=0, i = first_thread_on_screen; j < NOTESLINES && i < last_thread_on_screen; i++, j++) {
		bld_tline (i, &arts[index]);
		draw_tline (i, TRUE);
		if ((index = next_response (index)) == -1) {
			break;
		}	
	}

	CleartoEOS ();
	show_mini_help (THREAD_LEVEL);

	if (last_thread_on_screen == top_thread) {
		info_message (txt_end_of_thread);
	}

	draw_thread_arrow ();

#endif /* INDEX_DAEMON */
}


void 
update_thread_page ()
{
#ifndef INDEX_DAEMON
	register int i, j, index;

	index = choose_response (thread_basenote, first_thread_on_screen);
	assert(first_thread_on_screen != 0 || index == thread_respnum);

	for (j=0, i = first_thread_on_screen; j < NOTESLINES && i < last_thread_on_screen; ++i, ++j) {
		bld_tline (i, &arts[index]);
		draw_tline (i, FALSE);
		if ((index = next_response (index)) == -1) {
			break;
		}	
	}

	draw_thread_arrow();
#endif /* INDEX_DAEMON */
}


void 
draw_thread_arrow ()
{
	MoveCursor (INDEX2LNUM(thread_index_point), 0);

	if (draw_arrow_mark) {
		my_fputs ("->", stdout);
		fflush (stdout);
	} else {
		StartInverse ();
		draw_tline (thread_index_point, TRUE);
		EndInverse ();
	}
	MoveCursor (cLINES, 0);
}


void 
erase_thread_arrow ()
{
	MoveCursor (INDEX2LNUM(thread_index_point), 0);

	if (draw_arrow_mark) {
		my_fputs ("  ", stdout);
	} else {
		if (_hp_glitch) {
			EndInverse ();
		}
		draw_tline (thread_index_point, TRUE);
	}
	fflush (stdout);
}


int 
prompt_thread_num (ch)
	int ch;
{
	int num;

	clear_message ();

	if ((num = prompt_num (ch, txt_select_art)) == -1) {
		clear_message ();
		return FALSE;
	}

	if (num >= top_thread)
		num = top_thread - 1;

	if (num >= first_thread_on_screen
	&&  num < last_thread_on_screen) {
		erase_thread_arrow ();
		thread_index_point = num;
		draw_thread_arrow ();
	} else {
		erase_thread_arrow ();
		thread_index_point = num;
		show_thread_page ();
	}
	return TRUE;
}

/*
 *  Return the number of unread articles there are within a thread
 */

int 
new_responses (thread)
	int thread;
{
	int i;
	int sum = 0;

	for (i = (int) base[thread]; i >= 0; i = arts[i].thread) {
		if (arts[i].status != ART_READ) {
			sum++;
		}
	}
	
	return sum;
}

/*
 *  Which base note (an index into base[]) does a respnum
 *  (an index into arts[]) correspond to?
 *
 *  In other words, base[] points to an entry in arts[] which is
 *  the head of a thread, linked with arts[].thread.  For any q: arts[q],
 *  find i such that base[i]->arts[n]->arts[o]->...->arts[q]
 *
 *  Note that which_thread() can return -1 if in show_read_only mode and
 *  the article of interest has been read as well as all other articles in
 *  the thread,  thus resulting in no base[] entry for it.
 */

int 
which_thread (n)
	int n;
{
	register int i, j;

	for (i = 0; i < top_base; i++) {
		for (j = (int) base[i] ; j >= 0 ; j = arts[j].thread) {
			if (j == n) {
				return i;
			}
		}
	}

	sprintf (msg, "%d", n);
	error_message (txt_cannot_find_base_art, msg);

	return -1;
}

/*
 *  Find how deep in a thread a response is.  Start counting at zero
 */

int 
which_response (n)
	int n;
{
	int i, j;
	int num = 0;

	i = which_thread (n);
	assert(i >= 0);

	for (j = (int) base[i]; j != -1; j = arts[j].thread)
		if (j == n)
			break;
		else
			num++;

	return num;
}

/*
 *  Given an index into base[], find the number of responses for
 *  that basenote
 */

int 
num_of_responses (n)
	int n;
{
	int i;
	int oldi = -3;
	int sum = 0;

	assert (n < top_base);

	for (i = (int) base[n]; i != -1; i = arts[i].thread) {
		assert (i != -2);
		assert (i != oldi);
		oldi = i;
		sum++;
	}

	return sum - 1;
}

/*
 * Given an index into base[], return relevant statistics
 */

int 
stat_thread (n, sbuf)
	int n;
	struct t_art_stat *sbuf;
{
	int i;

	sbuf->total  = 0;
	sbuf->unread = 0;
	sbuf->seen   = 0;
	sbuf->inrange = 0;
	sbuf->deleted = 0;
	sbuf->selected_total = 0;
	sbuf->selected_unread= 0;
	sbuf->selected_seen  = 0;

	for (i = (int) base[n]; i >= 0; i = arts[i].thread) {
		++sbuf->total;
		if (arts[i].inrange) {
			++sbuf->inrange;
		}
		if (arts[i].delete) {
			++sbuf->deleted;
		}
		if (arts[i].status == ART_UNREAD) {
			++sbuf->unread;
		} else if (arts[i].status == ART_WILL_RETURN) {
			++sbuf->seen;
		}

		if (arts[i].selected) {
			++sbuf->selected_total;
			if (arts[i].status == ART_UNREAD) {
				++sbuf->selected_unread;
			} else if (arts[i].status == ART_WILL_RETURN) {
				++sbuf->selected_seen;
			}
		}

#if 0
		if (arts[i].killed) {
			++sbuf->killed;
		}
#endif
	}


	if (sbuf->inrange) 
		sbuf->art_mark = art_marked_inrange;
	else if (sbuf->deleted)
		sbuf->art_mark = art_marked_deleted;
	else if (sbuf->selected_unread)
		sbuf->art_mark = art_marked_selected;
	else if (sbuf->unread)
		sbuf->art_mark = art_marked_unread;
	else if (sbuf->seen)
		sbuf->art_mark = art_marked_return;
	else
		sbuf->art_mark = ART_MARK_READ;

	return(sbuf->total);
}


/*
 *  Find the next response.  Go to the next basenote if there
 *  are no more responses in this thread
 */

int 
next_response (n)
	int n;
{
	int i;

	if (arts[n].thread >= 0) {
		return arts[n].thread;
	}
	
	i = which_thread (n) + 1;

	if (i >= top_base) {
		return -1;
	}
	
	return (int) base[i];
}

/*
 *  Given a respnum (index into arts[]), find the respnum of the
 *  next basenote
 */

int 
next_thread (n)
	int n;
{
	int i;

	i = which_thread (n) + 1;
	if (i >= top_base)
		return -1;

	return (int) base[i];
}

/*
 *  Find the previous response.  Go to the last response in the previous
 *  thread if we go past the beginning of this thread.
 */

int 
prev_response (n)
	int n;
{
	int resp;
	int i;

	resp = which_response (n);

	if (resp > 0)
		return choose_response (which_thread (n), resp-1);

	i = which_thread (n) - 1;

	if (i < 0)
		return -1;

	return choose_response (i, num_of_responses (i));
}

/*
 *  return response number n from thread i
 */

int 
choose_response (i, n)
	int i;
	int n;
{
	int j;

	j = (int) base[i];

	while (n-- > 0 && arts[j].thread >= 0) {
		j = arts[j].thread;
	}

	return j;
}

/*
 *  Find the next unread response in this group. If no response is found
 *  from current point to the end restart from beginning of articles.
 */

int 
next_unread (n)
	int n;
{
	int cur_base_art = n;
	
	while (n >= 0) {
		if ((arts[n].status == ART_UNREAD 
		     /* || arts[n].status == ART_WILL_RETURN */ ) 
		     && arts[n].thread != ART_EXPIRED) {
			return n;
		}
		n = next_response (n);
	}

	n = base[0];
	while (n != cur_base_art) {
		if ((arts[n].status == ART_UNREAD 
		     /* || arts[n].status == ART_WILL_RETURN */ ) 
		     && arts[n].thread != ART_EXPIRED) {
			return n;
		}
		n = next_response (n);
	}
	
	return -1;
}

/*
 *  Find the previous unread response in this thread
 */

int 
prev_unread (n)
	int n;
{
	while (n >= 0) {
		if (arts[n].status == ART_UNREAD && arts[n].thread != ART_EXPIRED) {
			return n;
		}
		n = prev_response (n);
	}

	return -1;
}
