/*
 *  Project   : tin - a Usenet reader
 *  Module    : thread.c
 *  Author    : I. Lea
 *  Created   : 1991-04-01
 *  Updated   : 1997-12-26
 *  Notes     :
 *  Copyright : (c) Copyright 1991-98 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"
#include	"menukeys.h"

#define INDEX2TNUM(i)	((i) % NOTESLINES)
#define TNUM2LNUM(i)	(INDEX_TOP + (i))
#define INDEX2LNUM(i)	(TNUM2LNUM(INDEX2TNUM(i)))
#define EXPIRED(a) ((a)->article == ART_UNAVAILABLE || \
		    arts[(a)->article].thread == ART_EXPIRED)

int thread_basenote = 0;
t_bool show_subject;

#ifndef INDEX_DAEMON
	static int thread_index_point = 0;
	static int top_thread = 0;
	static int thread_respnum = 0;
	static int first_thread_on_screen = 0;
	static int last_thread_on_screen = 0;
#endif /* !INDEX_DAEMON */

/*
 * Local prototypes
 */
#ifndef INDEX_DAEMON
	static t_bool find_unexpired (struct t_msgid *ptr);
	static t_bool has_sibling (struct t_msgid *ptr);
	static void prompt_thread_num (int ch);
	static void bld_tline (int l, struct t_article *art);
	static void draw_tline (int i, t_bool full);
	static void draw_thread_arrow (void);
	static void erase_thread_arrow (void);
	static void make_prefix (struct t_msgid *art, char *prefix);
	static void update_thread_page (void);
#endif /* !INDEX_DAEMON */

/*
 * Build one line of the thread page display. Looks long winded, but
 * there are a lot of variables in the format for the output
 */
#ifndef INDEX_DAEMON
static void
bld_tline (
	int l,
	struct t_article *art)
{
	char mark;
#	ifdef USE_CURSES
	char buff[BUFSIZ];
#	else
	char *buff = screen[INDEX2TNUM(l)].col;
#	endif /* USE_CURSES */
	int i;
	int len_from;
	struct t_msgid *ptr;

	/*
	 * Start with 2 spaces for ->
	 * then index number of the message and whitespace (2+4+1 chars)
	 */
	sprintf (buff, "  %s ", tin_ltoa(l, 4));

	/*
	 * Add the article flags, tag number, or whatever (3 chars)
	 */
	if (art->tagged)
		strcat (buff, tin_ltoa(art->tagged, 3));
	else {
		strcat(buff, "   ");
		if (art->inrange) {
			mark = art_marked_inrange;
		} else if (art->status == ART_UNREAD) {
			mark = (art->selected ? art_marked_selected : art_marked_unread);
		} else if (art->status == ART_WILL_RETURN) {
			mark = art_marked_return;

/*
 * TODO - add kill_level
 * } else if (art->killed) {
 *		mark = 'K';
 */

		} else
			mark = ART_MARK_READ;

		*(buff+MARK_OFFSET) = mark;			/* insert mark */
	}

	strcat(buff, "  ");					/* 2 more spaces */

	/*
	 * Add the number of lines if enabled (inside [], 8 chars total)
	 */
	if (show_lines) {
		strcat (buff, "[");
		strcat (buff, ((art->lines != -1) ? tin_ltoa(art->lines, 4): "   ?"));
		strcat (buff, "]  ");
	}

	/*
	 * There are two formats for the rest of the line:
	 * 1) subject + optional author info
	 * 2) mandatory author info (eg, if subject threading)
	 *
	 * Add the subject and author information if required
	 */
	if (show_subject) {
		/*
		 * Work out in advance the length of the author field if needed
		 * TODO why the -3 ???
		 * show_lines takes up 8 chars if enabled
		 */
		len_from = ((CURR_GROUP.attribute->show_author != SHOW_FROM_NONE) ? (max_from - 3) + 8 * (1 - show_lines) : 0);

		/*
		 * Mutt-like thread tree. by sjpark@sparcs.kaist.ac.kr
		 * Insert tree-structure strings "`->", "+->", ...
		 */

		make_prefix(art->refptr, buff+strlen(buff));

		if ((int)strlen(buff) >= cCOLS) /* If extremely nested */
			buff[cCOLS] = '\0';

		/*
		 * Copy in the subject up to where the author (if any) starts
		 */
		i = cCOLS - strlen(buff) - len_from;

		if (len_from)	/* Leave gap before author */
			i -= 2;
		/*
		 * Mutt-like thread tree. by sjpark@sparcs.kaist.ac.kr
		 * Hide subject if same as parent's.
		 */

		if (i > 0) {
			for (ptr = art->refptr->parent; ptr && EXPIRED (ptr); ptr = ptr->parent);
			if (!(ptr && arts[ptr->article].subject == art->subject))
				strncat(buff, art->subject, i);

			*(buff + strlen(buff)) = '\0';	/* Just in case */
		}

		/*
		 * If we need to show the author, pad out to the start of the author field,
		 */
		if (len_from) {
			for (i = strlen(buff); i < (cCOLS - len_from); i++)
				*(buff + i) = ' ';

			/*
			 * Now add the author info at the end. This will be 0 terminated
			 */
			get_author (TRUE, art, buff + cCOLS - len_from, len_from);
		}

	} else /* Add the author info. This is always shown if subject is not */
		get_author (TRUE, art, buff+strlen(buff), cCOLS-strlen(buff));

	/* protect display from non-displayable characters (e.g., form-feed) */
	Convert2Printable (buff);

	if (!strip_blanks) {
		/*
		 * Pad to end of line so that inverse bar looks 'good'
		 */
		for (i = strlen(buff); i < cCOLS; i++)
			*(buff + i) = ' ';

		*(buff + i) = '\0';
	}

	WriteLine(INDEX2LNUM(l), buff);
}
#endif /* !INDEX_DAEMON */


#ifndef INDEX_DAEMON
static void
draw_tline (
	int i,
	t_bool full)
{
	int tlen;
	int x = full ? 0 : (MARK_OFFSET-2);
	int k = MARK_OFFSET;
#	ifdef USE_CURSES
	char buffer[BUFSIZ];
	char *s = screen_contents(INDEX2LNUM(i), x, buffer);
#	else
	char *s = &(screen[INDEX2TNUM(i)].col[x]);
#	endif /* USE_CURSES */

	if (full) {
		if (strip_blanks) {
			strip_line (s);
			CleartoEOLN ();
		}
		tlen = strlen (s);	/* note new line length */
	} else
		tlen = 3; /* tagged/mark is 3 chars wide */

	MoveCursor(INDEX2LNUM(i), x);
	if (tlen)
		my_printf("%.*s", tlen, s);

	/* it is somewhat less efficient to go back and redo that art mark
	 * if selected, but it is quite readable as to what is happening
	 */
	if (s[k-x] == art_marked_selected) {
		MoveCursor (INDEX2LNUM(i), k);
		ToggleInverse ();
		my_fputc (s[k-x], stdout);
		ToggleInverse ();
	}

	MoveCursor(INDEX2LNUM(i)+1, 0);
	return;
}
#endif /* !INDEX_DAEMON */

#ifndef INDEX_DAEMON
/*
 * show current thread. If threaded on Subject: show
 *   <respnum> <name>
 * If threaded on References: or Archive-name: show
 *   <respnum> <subject> <name>
 */
int
show_thread (
	struct t_group *group,
	char *group_path,
	int respnum,
	int thread_depth)
{
	int ret_code = TRUE;
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
	show_subject = ((arts[thread_respnum].archive != (char *)0) || (group->attribute->thread_arts >= THREAD_REFS));

	thread_index_point = top_thread;
	if (space_mode) {
		if ((i = new_responses (thread_basenote))) {
			for (n = 0, i = base[thread_basenote]; i >= 0; i = arts[i].thread, n++) {
				if (arts[i].status == ART_UNREAD) {
					if (arts[i].thread == ART_EXPIRED)
						art_mark_read (group, &arts[i]);
					else
						thread_index_point = n;
					break;
				}
			}
		}
	}

	if (thread_index_point < 0)
		thread_index_point = 0;

	/*
	 * If we explicitly want to enter part way down a thread, do so
	 */
	if (thread_depth)
		thread_index_point = thread_depth;

	show_thread_page ();

	forever {
		set_xclick_on ();
		ch = ReadCh ();

		if (ch >= '0' && ch <= '9') {	/* 0 goes to basenote */
			if (top_thread == 1)
				info_message (txt_no_responses);
			else
				prompt_thread_num (ch);
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
						goto thread_up;

					case KEYMAP_DOWN:
						goto thread_down;

					case KEYMAP_LEFT:
						if (thread_catchup_on_exit)
							goto thread_catchup;
						else
							goto thread_done;

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

#	ifndef WIN32
					case KEYMAP_MOUSE:
						switch (xmouse)
						{
							case MOUSE_BUTTON_1:
							case MOUSE_BUTTON_3:
								if (xrow < INDEX2LNUM(first_thread_on_screen) || xrow > INDEX2LNUM(last_thread_on_screen-1))
									goto thread_page_down;
								erase_thread_arrow ();
								thread_index_point = xrow-INDEX2LNUM(first_thread_on_screen)+first_thread_on_screen;
								draw_thread_arrow ();
								if (xmouse == MOUSE_BUTTON_1)
									goto thread_read_article;
								break;

							case MOUSE_BUTTON_2:
								if (xrow < INDEX2LNUM(first_thread_on_screen) || xrow > INDEX2LNUM(last_thread_on_screen-1))
									goto thread_page_up;
								if (thread_catchup_on_exit)
									goto thread_catchup;
								else
									goto thread_done;

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
				show_thread_page ();
				break;
#	endif /* !NO_SHELL_ESCAPE */

			case iKeyFirstPage:	/* show first page of articles */
top_of_thread:
				if (thread_index_point != 0)
					move_to_response(0);
				break;

			case iKeyLastPage:	/* show last page of articles */
end_of_thread:
				if (thread_index_point < top_thread - 1)
					move_to_response(top_thread - 1);
				break;

			case iKeySetRange:	/* set range */
				if (bSetRange (THREAD_LEVEL, 0, top_thread, thread_index_point))
					show_thread_page ();
				break;

			case iKeyThreadSave: /* save articles with prompting */
				if (thread_basenote >= 0)
					feed_articles (FEED_SAVE, THREAD_LEVEL, &CURR_GROUP, find_response (thread_basenote, thread_index_point));
				break;

			case iKeyThreadAutoSaveTagged:   /* Auto-save tagged articles without prompting */
				if (thread_basenote >= 0) {
					if (num_of_tagged_arts)
						feed_articles (FEED_AUTOSAVE_TAGGED, THREAD_LEVEL, &CURR_GROUP, (int) base[index_point]);
					else
						info_message (txt_no_tagged_arts_to_save);
				}
				break;

			case iKeyThreadReadArt:
			case iKeyThreadReadArt2:	/* read current article within thread */
thread_read_article:
				n = find_response (thread_basenote, thread_index_point);
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
				n = ((thread_index_point == 0) ? thread_respnum : find_response (thread_basenote, thread_index_point));

				for (i = n; i != -1; i = arts[i].thread) {
					if ((arts[i].status == ART_UNREAD) || (arts[i].status == ART_WILL_RETURN)) {
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

			case iKeyPageDown:		/* page down */
			case iKeyPageDown2:
			case iKeyPageDown3:
thread_page_down:
				if (thread_index_point + 1 == top_thread) {
					move_to_response(0);
					break;
				}
				erase_thread_arrow ();
				scroll_lines = (full_page_scroll ? NOTESLINES : NOTESLINES / 2);
				thread_index_point = ((thread_index_point + scroll_lines) / scroll_lines) * scroll_lines;
				if (thread_index_point >= top_thread) {
					thread_index_point = (top_thread / scroll_lines) * scroll_lines;
					if (thread_index_point < top_thread - 1)
						thread_index_point = top_thread - 1;
				}
				if (thread_index_point < first_thread_on_screen || thread_index_point >= last_thread_on_screen)
					show_thread_page ();
				else
					draw_thread_arrow ();
				break;

			case iKeyThreadRedrawScr:		/* redraw screen */
				my_retouch ();
				set_xclick_off ();
				show_thread_page ();
				break;

			case iKeyDown:		/* line down */
			case iKeyDown2:
thread_down:
				if (thread_index_point + 1 >= top_thread) {
					move_to_response(0);
					break;
				}
				move_to_response(thread_index_point + 1);
				break;

			case iKeyUp:
			case iKeyUp2:		/* line up */
thread_up:
				if (thread_index_point == 0) {
					move_to_response(top_thread - 1);
					break;
				}
				move_to_response(thread_index_point - 1);
				break;

			case iKeyPageUp:		/* page up */
			case iKeyPageUp2:
			case iKeyPageUp3:
thread_page_up:
				if (thread_index_point == 0) {
					move_to_response (top_thread - 1);
					break;
				}
				clear_message ();
				erase_thread_arrow ();
				scroll_lines = (full_page_scroll ? NOTESLINES : NOTESLINES / 2);

				thread_index_point = (((n = thread_index_point % scroll_lines) > 0) ? (thread_index_point - n) : (((thread_index_point - scroll_lines) / scroll_lines) * scroll_lines));

				if (thread_index_point < 0)
					thread_index_point = 0;
				if (thread_index_point < first_thread_on_screen || thread_index_point >= last_thread_on_screen)
					show_thread_page ();
				else
					draw_thread_arrow ();
				break;

			case iKeyThreadCatchupConditional:	/* catchup thread but ask for confirmation */
thread_catchup:
				if (ch == iKeyThreadCatchupConditional && confirm_action && prompt_yn (cLINES, txt_mark_thread_read, TRUE) != 1)
					break;
				thd_mark_read (group, base[thread_basenote]);
				goto thread_done;

			case iKeyThreadCatchup:	/* mark thread as read immediately */
				thd_mark_read (group, base[thread_basenote]);
				goto thread_done;

			case iKeyThreadMarkArtRead: /* mark article as read */
				n = find_response (thread_basenote, thread_index_point);
				art_mark_read (group, &arts[n]);
				bld_tline (thread_index_point, &arts[n]);
				draw_tline (thread_index_point, FALSE);
				n = next_unread (n);
				if (n == -1) /* no more articles in this thread _and_ group */
					goto thread_done;
				n = which_response (n);
				move_to_response(n);
				break;

			case iKeyThreadToggleSubjDisplay:	/* toggle display of subject & subj/author */
				if (show_subject) {
					toggle_subject_from ();
					show_thread_page ();
				}
				break;

			case iKeyThreadHelp:			/* help */
				show_info_page (HELP_INFO, help_thread, txt_thread_com);
				show_thread_page ();
				break;

			case iKeySearchSubjF:	/* forward/backward search */
/*			case iKeySearchSubjB: we don't have backwards pointers to do this easily */
				/* Subject won't change within thread if not ref threading */
				if (CURR_GROUP.attribute->thread_arts < THREAD_REFS)
					break;
				search_subject_thread (ch == iKeySearchSubjF, thread_respnum, thread_index_point);
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

#	ifdef HAVE_COLOR
			case iKeyThreadToggleColor:		/* toggle color */
				if (toggle_color ()) {
					show_thread_page ();
					show_color_status ();
				}
				break;
#	endif /* HAVE_COLOR */

			case iKeyQuit:				/* return to previous level */
				goto thread_done;

			case iKeyQuitTin:			/* quit */
				ret_code = GRP_QUIT;
				goto thread_done;

			case iKeyThreadTag:			/* tag/untag art for mailing/piping/printing/saving */
				/* Find index of current article */
				if ((n = find_response (thread_basenote, thread_index_point)) < 0)
					break;
				if (arts[n].tagged) {
					decr_tagged(arts[n].tagged);
					arts[n].tagged = 0;
					--num_of_tagged_arts;
					info_message (txt_untagged_art);
					update_thread_page();						/* Must update whole page */
				} else {
					arts[n].tagged = ++num_of_tagged_arts;
					info_message (txt_tagged_art);
					bld_tline (thread_index_point, &arts[n]);	/* Update just this line */
					draw_tline (thread_index_point, FALSE);
				}
				/* Automatically advance to next art if not at end of thread */
				if (thread_index_point + 1 < top_thread)
					goto thread_down;
				draw_thread_arrow ();
				break;

			case iKeyThreadBugReport:		/* bug/gripe/comment mailed to author */
				mail_bug_report ();
				ClearScreen ();
				show_thread_page ();
				break;

			case iKeyThreadUntag:			/* untag all articles */
				if (index_point >= 0 && untag_all_articles())
					update_thread_page();
				break;

			case iKeyVersion:			/* version */
				info_message (cvers);
				break;

			case iKeyThreadMarkArtUnread:		/* mark article as unread */
				n = find_response (thread_basenote, thread_index_point);
				art_mark_will_return (group, &arts[n]); /*art_mark_unread (group, &arts[n]);*/
				bld_tline (thread_index_point, &arts[n]);
				draw_tline (thread_index_point, FALSE);
				info_message (txt_marked_as_unread, "Article");
				draw_thread_arrow ();
				break;

			case iKeyThreadMarkThdUnread:		/* mark thread as unread */
				thd_mark_unread (group, base[thread_basenote]);
				update_thread_page ();
				info_message (txt_marked_as_unread, "Thread");
				break;

			case iKeyThreadMarkArtSel:		/* mark article as selected */
			case iKeyThreadToggleArtSel:		/* toggle article as selected */
				n = find_response (thread_basenote, thread_index_point);
				if (n < 0)
					break;
				flag = (!(ch == iKeyThreadToggleArtSel && arts[n].selected == 1));
				arts[n].selected = flag;
/*				update_thread_page (); */
				bld_tline (thread_index_point, &arts[n]);
				draw_tline (thread_index_point, FALSE);
				if (thread_index_point + 1 < top_thread)
					goto thread_down;
				draw_thread_arrow ();
				break;

			case iKeyThreadReverseSel:		/* reverse selections */
				for (i = (int) base[thread_basenote]; i != -1; i = arts[i].thread)
					arts[i].selected = (arts[i].selected ? 0 : 1);
				update_thread_page ();
				break;

			case iKeyThreadUndoSel:			/* undo selections */
				for (i = (int) base[thread_basenote]; i != -1; i = arts[i].thread)
					arts[i].selected = 0;
				update_thread_page ();
				break;

			case iKeyPostponed:
			case iKeyPostponed2:	/* post postponed article */
				if (can_post) {
					if (pickup_postponed_articles(FALSE, FALSE))
						show_thread_page();
				} else
					info_message(txt_cannot_post);
				break;

			case iKeyToggleInfoLastLine:  /* display subject in last line */
				info_in_last_line = !info_in_last_line;
				show_thread_page();
				break;

			default:
				info_message (txt_bad_command);
		}
	}

thread_done:
	set_xclick_off ();
	clear_note_area ();

	return ret_code;
}
#endif /* !INDEX_DAEMON */


void
show_thread_page (void)
{
#ifndef INDEX_DAEMON

	int i, j;
	static int the_index = 0;

	set_signals_thread ();

	ClearScreen ();

	if (thread_index_point > top_thread - 1)
		thread_index_point = top_thread - 1;

	if (NOTESLINES <= 0)
		first_thread_on_screen = 0;
	else {
		first_thread_on_screen = (thread_index_point / NOTESLINES) * NOTESLINES;
		if (first_thread_on_screen < 0)
			first_thread_on_screen = 0;
	}

	last_thread_on_screen = first_thread_on_screen + NOTESLINES;

	if (last_thread_on_screen >= top_thread) {
		last_thread_on_screen = top_thread;
		first_thread_on_screen = (top_thread / NOTESLINES) * NOTESLINES;

		if (first_thread_on_screen == last_thread_on_screen || first_thread_on_screen < 0)
			first_thread_on_screen = ((first_thread_on_screen < 0) ? 0 : last_thread_on_screen - NOTESLINES);
	}

	if (top_thread == 0) {
		first_thread_on_screen = 0;
		last_thread_on_screen = 0;
	}

	the_index = find_response (thread_basenote, first_thread_on_screen);

	assert(first_thread_on_screen != 0 || the_index == thread_respnum);

	if (show_subject)
		sprintf (mesg, "List Thread (%d of %d)", index_point+1, top_base);
	else
		sprintf (mesg, "Thread (%.*s)", cCOLS-23, arts[thread_respnum].subject);

	/*
	 * Slight misuse of the 'mesg' buffer here. We need to clear it so that progress messages
	 * are displayed correctly
	 */
	show_title (mesg);
	mesg[0] = '\0';

	MoveCursor (INDEX_TOP, 0);

	for (j=0, i = first_thread_on_screen; j < NOTESLINES && i < last_thread_on_screen; i++, j++) {
		bld_tline (i, &arts[the_index]);
		draw_tline (i, TRUE);
		if ((the_index = next_response (the_index)) == -1)
			break;
	}

	CleartoEOS ();
	show_mini_help (THREAD_LEVEL);

	if (last_thread_on_screen == top_thread)
		info_message (txt_end_of_thread);

	draw_thread_arrow ();

#endif /* !INDEX_DAEMON */
}


#ifndef INDEX_DAEMON
static void
update_thread_page (void)
{
	register int i, j, the_index;

	the_index = find_response (thread_basenote, first_thread_on_screen);
	assert(first_thread_on_screen != 0 || the_index == thread_respnum);

	for (j=0, i = first_thread_on_screen; j < NOTESLINES && i < last_thread_on_screen; ++i, ++j) {
		bld_tline (i, &arts[the_index]);
		draw_tline (i, FALSE);
		if ((the_index = next_response (the_index)) == -1)
			break;
	}

	draw_thread_arrow();
}
#endif /* !INDEX_DAEMON */


#ifndef INDEX_DAEMON
static void
draw_thread_arrow (void)
{
	MoveCursor (INDEX2LNUM(thread_index_point), 0);

	if (draw_arrow_mark) {
		my_fputs ("->", stdout);
		my_flush ();
	} else {
		StartInverse ();
		draw_tline (thread_index_point, TRUE);
		EndInverse ();
	}
	stow_cursor();

	if (info_in_last_line)
		info_message ("%s", arts[find_response (thread_basenote, thread_index_point)].subject);
}
#endif /* !INDEX_DAEMON */


#ifndef INDEX_DAEMON
static void
erase_thread_arrow (void)
{
	MoveCursor (INDEX2LNUM(thread_index_point), 0);

	if (draw_arrow_mark)
		my_fputs ("  ", stdout);
	else {
		HpGlitch(EndInverse ());
		draw_tline (thread_index_point, TRUE);
	}
	my_flush ();
}
#endif /* !INDEX_DAEMON */


#ifndef INDEX_DAEMON
static void
prompt_thread_num (
	int ch)
{
	int num;

	clear_message ();

	if ((num = prompt_num (ch, txt_select_art)) == -1) {
		clear_message ();
		return;
	}

	if (num >= top_thread)
		num = top_thread - 1;

	move_to_response (num);
}
#endif /* !INDEX_DAEMON */


/*
 *  Return the number of unread articles there are within a thread
 */
int
new_responses (
	int thread)
{
	int i;
	int sum = 0;

	for (i = (int) base[thread]; i >= 0; i = arts[i].thread) {
		if (arts[i].status != ART_READ)
			sum++;
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
which_thread (
	int n)
{
	register int i, j;

	for (i = 0; i < top_base; i++) {
		for (j = (int) base[i]; j >= 0; j = arts[j].thread) {
			if (j == n)
				return i;
		}
	}

	error_message (txt_cannot_find_base_art, n);

	return -1;
}


/*
 *  Find how deep in a thread a response is.  Start counting at zero
 */
int
which_response (
	int n)
{
	int i, j;
	int num = 0;

	i = which_thread (n);
	assert(i >= 0);

	for (j = (int) base[i]; j != -1; j = arts[j].thread) {
		if (j == n)
			break;
		else
			num++;
	}

	return num;
}


/*
 *  Given an index into base[], find the number of responses for
 *  that basenote
 */
int
num_of_responses (
	int n)
{
	int i;
	int oldi = -3;
	int sum = 0;

	assert (n < top_base);

	for (i = (int) base[n]; i != -1; i = arts[i].thread) {
		assert (i != ART_EXPIRED);
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
stat_thread (
	int n,
	struct t_art_stat *sbuf) /* return value is always ignored */
{
	int i;

	sbuf->total  = 0;
	sbuf->unread = 0;
	sbuf->seen   = 0;
	sbuf->deleted = 0;
	sbuf->inrange = 0;
	sbuf->selected_total = 0;
	sbuf->selected_unread= 0;
	sbuf->selected_seen  = 0;
	sbuf->art_mark = ART_MARK_READ;

	for (i = (int) base[n]; i >= 0; i = arts[i].thread) {
		++sbuf->total;
		if (arts[i].inrange)
			++sbuf->inrange;

		if (arts[i].delete_it)
			++sbuf->deleted;

		if (arts[i].status == ART_UNREAD)
			++sbuf->unread;
		else if (arts[i].status == ART_WILL_RETURN)
			++sbuf->seen;

		if (arts[i].selected) {
			++sbuf->selected_total;
			if (arts[i].status == ART_UNREAD)
				++sbuf->selected_unread;
			else if (arts[i].status == ART_WILL_RETURN)
				++sbuf->selected_seen;
		}

#if 0
		if (arts[i].killed)
			++sbuf->killed;
#endif /* 0 */
	}

	sbuf->art_mark = (sbuf->inrange ? art_marked_inrange : (sbuf->deleted ? art_marked_deleted : (sbuf->selected_unread ? art_marked_selected : (sbuf->unread ? art_marked_unread : (sbuf->seen ? art_marked_return : ART_MARK_READ)))));
	return(sbuf->total);
}


/*
 *  Find the next response.  Go to the next basenote if there
 *  are no more responses in this thread
 */
int
next_response (
	int n)
{
	int i;

	if (arts[n].thread >= 0)
		return arts[n].thread;

	i = which_thread (n) + 1;

	if (i >= top_base)
		return -1;

	return (int) base[i];
}


/*
 *  Given a respnum (index into arts[]), find the respnum of the
 *  next basenote
 */
int
next_thread (
	int n)
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
prev_response (
	int n)
{
	int resp;
	int i;

	resp = which_response (n);

	if (resp > 0)
		return find_response (which_thread (n), resp-1);

	i = which_thread (n) - 1;

	if (i < 0)
		return -1;

	return find_response (i, num_of_responses (i));
}


/*
 *  return response number n from thread i
 *	ie return the index in arts[] of the nth followup to thread base 'i'
 */
int
find_response (
	int i,
	int n)
{
	int j;

	j = (int) base[i];

	while (n-- > 0 && arts[j].thread >= 0)
		j = arts[j].thread;

	return j;
}


/*
 *  Find the next unread response in this group. If no response is found
 *  from current point to the end restart from beginning of articles.
 */
int
next_unread (
	int n)
{
	int cur_base_art = n;

	while (n >= 0) {
		if (((arts[n].status == ART_UNREAD) || (arts[n].status == ART_WILL_RETURN)) && arts[n].thread != ART_EXPIRED)
			return n;

		n = next_response (n);
	}

	n = base[0];
	while (n != cur_base_art) {
		if (((arts[n].status == ART_UNREAD) || (arts[n].status == ART_WILL_RETURN)) && arts[n].thread != ART_EXPIRED)
			return n;

		n = next_response (n);
	}

	return -1;
}


/*
 *  Find the previous unread response in this thread
 */
int
prev_unread (
	int n)
{
	while (n >= 0) {
		if (arts[n].status == ART_UNREAD && arts[n].thread != ART_EXPIRED)
			return n;

		n = prev_response (n);
	}

	return -1;
}


#ifndef INDEX_DAEMON
/*
 * Move the on-screen pointer & internal variable to the given reponse
 * within the thread
 */
void
move_to_response (
	int n)
{
	HpGlitch(erase_thread_arrow ());
	erase_thread_arrow ();
	thread_index_point = n;

	if (n >= first_thread_on_screen && n < last_thread_on_screen)
		draw_thread_arrow ();
	else
		show_thread_page ();
}


static t_bool
find_unexpired (
	struct t_msgid *ptr)
{
	return ptr && (!EXPIRED (ptr) || find_unexpired (ptr->child) || find_unexpired (ptr->sibling));
}


static t_bool
has_sibling (
	struct t_msgid *ptr)
{
	do {
		if (find_unexpired (ptr->sibling))
			return TRUE;
		ptr = ptr->parent;
	} while (ptr && EXPIRED (ptr));
	return FALSE;
}


/*
 * mutt-like subject according. by sjpark@sparcs.kaist.ac.kr
 */
static void
make_prefix (
	struct t_msgid *art,
	char *prefix)
{
	int prefix_ptr;
	int depth = 0;
	struct t_msgid *ptr;

	for (ptr = art->parent; ptr; ptr = ptr->parent)
		depth += (!EXPIRED (ptr) ? 1 : 0);

	if (depth == 0) {
		prefix[0] = '\0';
		return;
	}

	prefix_ptr = depth * 2 - 1;
	strcpy (&prefix[prefix_ptr], "->");
	prefix[--prefix_ptr] = (has_sibling (art) ? '+' : '`');

	for (ptr = art->parent; prefix_ptr != 0; ptr = ptr->parent) {
		if (EXPIRED (ptr))
			continue;
		prefix[--prefix_ptr] = ' ';
		prefix[--prefix_ptr] = (has_sibling (ptr) ? '|' : ' ');
	}
	return;
}
#endif /* !INDEX_DAEMON */
