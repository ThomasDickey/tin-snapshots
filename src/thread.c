/*
 *  Project   : tin - a Usenet reader
 *  Module    : thread.c
 *  Author    : I. Lea
 *  Created   : 1991-04-01
 *  Updated   : 1997-12-26
 *  Notes     :
 *  Copyright : (c) Copyright 1991-99 by Iain Lea
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

#define INDEX2TNUM(i)	((i) % NOTESLINES)
#define TNUM2LNUM(i)	(INDEX_TOP + (i))
#define INDEX2LNUM(i)	(TNUM2LNUM(INDEX2TNUM(i)))
#define EXPIRED(a) ((a)->article == ART_UNAVAILABLE || arts[(a)->article].thread == ART_EXPIRED)

int thread_basenote = 0;						/* Index in base[] of basenote */
t_bool show_subject;

#ifndef INDEX_DAEMON
	static int thread_index_point = 0;			/* Current screen cursor position in thread */
	static int top_thread = 0;					/* Essentially = # arts in current thread */
	static int thread_respnum = 0;				/* Art # of basenote ie base[thread_basenote] */
	static int first_thread_on_screen = 0;		/* response # at top of screen */
	static int last_thread_on_screen = 0;		/* response # at end of screen */
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
	static void make_prefix (struct t_msgid *art, char *prefix, int maxlen);
	static void move_to_response (int n);
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
	int gap;
	int rest_of_line = cCOLS;
	int len_from, len_subj;
	struct t_msgid *ptr;

	/*
	 * Start with 2 spaces for ->
	 * then index number of the message and whitespace (2+4+1 chars)
	 */
	sprintf (buff, "  %s ", tin_ltoa(l, 4));
	rest_of_line -= 7;

	/*
	 * Add the article flags, tag number, or whatever (3 chars)
	 */
	rest_of_line -= 3;
	if (art->tagged)
		strcat (buff, tin_ltoa(art->tagged, 3));
	else {
		strcat(buff, "   ");
		if (art->inrange) {
			mark = tinrc.art_marked_inrange;
		} else if (art->status == ART_UNREAD) {
			mark = (art->selected ? tinrc.art_marked_selected : tinrc.art_marked_unread);
		} else if (art->status == ART_WILL_RETURN) {
			mark = tinrc.art_marked_return;
		} else if (art->killed) {
			mark = 'K';
		} else {
			if (tinrc.kill_level == KILL_THREAD && art->score >= SCORE_SELECT)
				mark = ART_MARK_READ_SELECTED ; /* read hot chil^H^H^H^H article */
			else
				mark = ART_MARK_READ;
		}

		buff[MARK_OFFSET] = mark;			/* insert mark */
	}

	strcat(buff, "  ");					/* 2 more spaces */
	rest_of_line -= 2;

	/*
	 * Add the number of lines and/or the score if enabled
	 * (inside "[,]", 1+4[+1+6]+1+2 chars total)
	 */
	if (tinrc.show_lines || tinrc.show_score) { /* add [ */
		strcat (buff, "[");
		rest_of_line--;
	}

	if (tinrc.show_lines) { /* add lines */
		strcat (buff, ((art->lines != -1) ? tin_ltoa(art->lines, 4): "   ?"));
		rest_of_line -= 4;
	}

	if (tinrc.show_score) {
		char tmp_score[15];

		if (tinrc.show_lines) { /* insert a sperator if show lines and score */
			strcat (buff, ",");
			rest_of_line--;
		}

		sprintf (tmp_score, "%6d", art->score);
		strcat (buff, tmp_score); /* add score */
		rest_of_line -= 6;
	}

	if (tinrc.show_lines || tinrc.show_score) { /* add closing ] and two spaces */
		strcat (buff, "]  ");
		rest_of_line -= 3;
	}

	/*
	 * There are two formats for the rest of the line:
	 * 1) subject + optional author info
	 * 2) mandatory author info (eg, if subject threading)
	 *
	 * Add the subject and author information if required
	 */
	if (show_subject) {

		if (CURR_GROUP.attribute->show_author == SHOW_FROM_NONE)
				len_from = 0;
		else {
			len_from = rest_of_line;

			if (CURR_GROUP.attribute->show_author == SHOW_FROM_BOTH)
				len_from /= 2; /* if SHOW_FROM_BOTH use 50% for author info */
			else
				len_from /= 3; /* otherwise use 33% for author info */

			if (len_from < 0) /* security check - small screen ? */
				len_from = 0;
		}
		rest_of_line -= len_from;
		len_subj = rest_of_line - (len_from? 2 : 0);

		/*
		 * Mutt-like thread tree. by sjpark@sparcs.kaist.ac.kr
		 * Insert tree-structure strings "`->", "+->", ...
		 */

		make_prefix(art->refptr, buff+strlen(buff), len_subj);

		/*
		 * Copy in the subject up to where the author (if any) starts
		 */
		gap = cCOLS - strlen(buff) - len_from; /* gap = gap (no. of chars) between tree and author/border of window */

		if (len_from)	/* Leave gap before author */
			gap -= 2;

		/*
		 * Mutt-like thread tree. by sjpark@sparcs.kaist.ac.kr
		 * Hide subject if same as parent's.
		 */
		if (gap > 0) {
			int len = strlen(buff);
			for (ptr = art->refptr->parent; ptr && EXPIRED (ptr); ptr = ptr->parent)
				;
			if (!(ptr && arts[ptr->article].subject == art->subject))
				strncat(buff, art->subject, gap);

			buff[len+gap] = '\0';	/* Just in case */
		}

		/*
		 * If we need to show the author, pad out to the start of the author field,
		 */
		if (len_from) {
			for (gap = strlen(buff); gap < (cCOLS - len_from); gap++)
				buff[gap] = ' ';

			/*
			 * Now add the author info at the end. This will be 0 terminated
			 */
			get_author (TRUE, art, buff + cCOLS - len_from, len_from);
		}

	} else /* Add the author info. This is always shown if subject is not */
		get_author (TRUE, art, buff+strlen(buff), cCOLS-strlen(buff));

	/* protect display from non-displayable characters (e.g., form-feed) */
	Convert2Printable (buff);

	if (!tinrc.strip_blanks) {
		/*
		 * Pad to end of line so that inverse bar looks 'good'
		 */
		for (gap = strlen(buff); gap < cCOLS; gap++)
			buff[gap] = ' ';

		buff[gap] = '\0';
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
		if (tinrc.strip_blanks) {
			strip_line (s);
			CleartoEOLN ();
		}
		tlen = strlen (s);	/* note new line length */
	} else
		tlen = 3; /* tagged/mark is 3 chars wide */

	MoveCursor(INDEX2LNUM(i), x);
	if (tlen)
		my_printf("%.*s", tlen, s);
	/*
	 * it is somewhat less efficient to go back and redo that art mark
	 * if selected, but it is quite readable as to what is happening
	 */
	if (s[k-x] == tinrc.art_marked_selected || (tinrc.kill_level == KILL_THREAD && s[k-x] == ART_MARK_READ_SELECTED)) {
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
 * Show current thread.
 * If threaded on Subject: show
 *   <respnum> <name>
 * If threaded on References: or Archive-name: show
 *   <respnum> <subject> <name>
 * Return values:
 *		>= 0				Normal return to group level
 *		GRP_RETURN		Return to selection screen
 *		GRP_QUIT			'Q'uit all the way out
 *		GRP_NEXT			Catchup goto next group
 *		GRP_NEXTUNREAD	Catchup enter next unread thread
 *		GRP_KILLED		Thread was killed at art level ?????
 */
int
thread_page (
	struct t_group *group,
	int respnum,				/* base[] article of thread to view */
	int thread_depth)			/* initial depth in thread */
{
	char buf[LEN];
	int ret_code = 0;
	int ch = 0;
	int i, n;
	t_bool ignore_unavail = FALSE;	/* Set if we keep going after an 'article unavailable' */

	thread_respnum = respnum;		/* Bodge to make this variable global */
	if ((n = which_thread (thread_respnum)) >= 0)
		thread_basenote = n;
	top_thread = num_of_responses (thread_basenote) + 1;

	if (top_thread <= 0) {
		info_message (txt_no_resps_in_thread);
		return 0;
	}

	/*
	 * If threading by Refs, it helps to see the subject line
	 */
	show_subject = ((arts[thread_respnum].archive != (char *)0) || (group->attribute->thread_arts >= THREAD_REFS));

	/*
	 * Set the cursor to the last response unless space_mode is active
	 * or an explicit thread_depth has been specified
	 */
	thread_index_point = top_thread;

	if (thread_depth)
		thread_index_point = thread_depth;
	else {
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
	}

	if (thread_index_point < 0)
		thread_index_point = 0;

	/* Now we know where the cursor is, actually put something on the screen */
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
						if (tinrc.thread_catchup_on_exit)
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
								if (tinrc.thread_catchup_on_exit)
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

			case iKeyThreadLastViewed:	/* show last viewed article */
				if (this_resp < 0 || (which_thread(this_resp) == -1)) {
					info_message (txt_no_last_message);
					break;
				}
				i = this_resp;
				goto enter_pager;

			case iKeySetRange:	/* set range */
				if (bSetRange (THREAD_LEVEL, 0, top_thread, thread_index_point))
					show_thread_page ();
				break;

			case iKeyThreadMail:	/* mail article to somebody */
				if (thread_basenote >= 0)
					feed_articles (FEED_MAIL, THREAD_LEVEL, &CURR_GROUP, find_response (thread_basenote, thread_index_point));
				break;

			case iKeyThreadSave:	/* save articles with prompting */
				if (thread_basenote >= 0)
					feed_articles (FEED_SAVE, THREAD_LEVEL, &CURR_GROUP, find_response (thread_basenote, thread_index_point));
				break;

			case iKeyThreadAutoSaveTagged:	/* Auto-save tagged articles without prompting */
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
				i = find_response (thread_basenote, thread_index_point);
				goto enter_pager;

			case iKeyThreadReadNextArtOrThread:
thread_tab_pressed:
				space_mode = TRUE;
				n = ((thread_index_point == 0) ? thread_respnum : find_response (thread_basenote, thread_index_point));

				for (i = n; i != -1; i = arts[i].thread) {
					if ((arts[i].status == ART_UNREAD) || (arts[i].status == ART_WILL_RETURN))
						break;
				}

				if (i == -1) {					/* We ran out of thread */
					ret_code = GRP_NEXTUNREAD;
					goto thread_done;
				}

				ignore_unavail = TRUE;

				/*
				 * General entry point into the pager. 'i' is the arts[i] we wish to view
				 */
enter_pager:
				n = show_page (group, i, &thread_index_point);

				/*
				 * In some cases, we want to keep going after an ARTFAIL
				 */
				if (ignore_unavail) {
					ignore_unavail = FALSE;
					if (n == GRP_ARTFAIL)
						n = GRP_NEXTUNREAD;
				}

				switch (n) {

					case GRP_ARTFAIL:
					case GRP_GOTOTHREAD:
						show_thread_page();
						break;

					case GRP_RETURN:
					case GRP_NEXT:				/* Skip to next thread */
						ret_code = n;
						goto thread_done;

					case GRP_QUIT:
						ret_code = GRP_QUIT;
						goto thread_done;

					case GRP_NEXTUNREAD:
						goto thread_tab_pressed;

					default:
						if (filtered_articles) {
							ret_code = GRP_KILLED; /* ?? */
							goto thread_done;
						}
						fixup_thread (this_resp, FALSE);
						show_thread_page();
				}
				break;

			case iKeyPageDown:		/* page down */
			case iKeyPageDown2:
			case iKeyPageDown3:
thread_page_down:
				move_to_response (page_down (thread_index_point, top_thread));
				break;

			case iKeyThreadPost:	/* post a basenote */
				if (post_article (group->name))
					show_thread_page();
				break;

			case iKeyRedrawScr:		/* redraw screen */
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
				move_to_response (page_up (thread_index_point, top_thread));
				break;

			case iKeyThreadCatchup:					/* catchup thread, move to next one */
			case iKeyThreadCatchupNextUnread:	/* -> next with unread arts */
thread_catchup:										/* come here when exiting thread via <- */
				n = ((thread_index_point == 0) ? thread_respnum : find_response (thread_basenote, 0));
				for (i = n; i != -1; i = arts[i].thread) {
					if ((arts[i].status == ART_UNREAD) || (arts[i].status == ART_WILL_RETURN))
						break;
				}
				if (i != -1) {	/* still unread arts in the thread */
					sprintf(buf, txt_mark_thread_read, (ch == iKeyThreadCatchupNextUnread) ? txt_enter_next_thread : "");
					if (!tinrc.confirm_action || (tinrc.confirm_action && prompt_yn (cLINES, buf, TRUE) == 1))
						thd_mark_read (group, base[thread_basenote]);
				}
				ret_code = (ch == iKeyThreadCatchupNextUnread ? GRP_NEXTUNREAD : GRP_NEXT);
				goto thread_done;

			case iKeyThreadMarkArtRead: /* mark article as read */
				n = find_response (thread_basenote, thread_index_point);
				art_mark_read (group, &arts[n]);
				bld_tline (thread_index_point, &arts[n]);
				draw_tline (thread_index_point, FALSE);
				if ((n = next_unread (n)) == -1) /* no more articles in this thread _and_ group */
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

			case iKeyLookupMessage:
				if ((n = prompt_msgid ()) != ART_UNAVAILABLE) {
					i = n;
					goto enter_pager;
				}
				break;

			case iKeySearchBody:			/* search article body */
				if ((n = search_body (find_response (thread_basenote, thread_index_point))) != -1) {
					fixup_thread (n, TRUE);
/*					goto enter_pager;*/
				}
				break;

			case iKeySearchSubjF:			/* subject search */
			case iKeySearchSubjB:
				if ((n = search (SEARCH_SUBJ, find_response (thread_basenote, thread_index_point),
										(ch == iKeySearchSubjF))) != -1) {
					fixup_thread (n, TRUE);
				}
				break;

			case iKeySearchAuthF:			/* author search */
			case iKeySearchAuthB:
				if ((n = search (SEARCH_AUTH, find_response (thread_basenote, thread_index_point),
										(ch == iKeySearchAuthF))) != -1) {
					fixup_thread (n, TRUE);
				}
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
			case iKeyToggleColor:		/* toggle color */
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
				arts[n].selected = (!(ch == iKeyThreadToggleArtSel && arts[n].selected == 1));
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

			case iKeyThreadUndoSel:		/* undo selections */
				for (i = (int) base[thread_basenote]; i != -1; i = arts[i].thread)
					arts[i].selected = 0;
				update_thread_page ();
				break;

			case iKeyPostponed:
			case iKeyPostponed2:		/* post postponed article */
				if (can_post) {
					if (pickup_postponed_articles(FALSE, FALSE))
						show_thread_page();
				} else
					info_message(txt_cannot_post);
				break;

			case iKeyDisplayPostHist:	/* display messages posted by user */
				if (user_posted_messages ())
					show_thread_page ();
				break;

			case iKeyToggleInfoLastLine:		/* display subject in last line */
				tinrc.info_in_last_line = !tinrc.info_in_last_line;
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
show_thread_page (
	void)
{
#ifndef INDEX_DAEMON

	int i, j;
	static int the_index = 0;

	signal_context = cThread;

	ClearScreen ();

	/* TODO - if we ever count threads from 1 and not 0, then fold this into set_first_screen_item() */
	if (thread_index_point > top_thread - 1)
		thread_index_point = top_thread - 1;

	set_first_screen_item (thread_index_point, top_thread, &first_thread_on_screen, &last_thread_on_screen);

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
		if (the_index < 0 || the_index >= max_art)
			break;
		bld_tline (i, &arts[the_index]);
		draw_tline (i, TRUE);
		the_index = next_response (the_index);
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
update_thread_page (
	void)
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


static void
draw_thread_arrow (
	void)
{
	MoveCursor (INDEX2LNUM(thread_index_point), 0);

	if (tinrc.draw_arrow) {
		my_fputs ("->", stdout);
		my_flush ();
	} else {
		StartInverse ();
		draw_tline (thread_index_point, TRUE);
		EndInverse ();
	}
	stow_cursor();

	if (tinrc.info_in_last_line)
		info_message ("%s", arts[find_response (thread_basenote, thread_index_point)].subject);
}


static void
erase_thread_arrow (
	void)
{
	MoveCursor (INDEX2LNUM(thread_index_point), 0);

	if (tinrc.draw_arrow)
		my_fputs ("  ", stdout);
	else {
		HpGlitch(EndInverse ());
		draw_tline (thread_index_point, TRUE);
	}
	my_flush ();
}


/*
 * Fix all the internal pointers if the current thread/response has
 * changed.
 */
void
fixup_thread (
	int respnum,
	t_bool redraw)
{
	int basenote = which_thread(respnum);

	if (basenote != thread_basenote && basenote >= 0) {
		thread_basenote = basenote;
		top_thread = num_of_responses (thread_basenote) + 1;
		thread_respnum = base[thread_basenote];
		index_point = basenote;
		if (redraw)
			show_thread_page();
	}

	if (redraw)
		move_to_response (which_response(respnum));		/* Redraw screen etc.. */
}

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
	int i, j;

	for (i = 0; i < top_base; i++) {
		for (j = (int) base[i]; j >= 0; j = arts[j].thread) {
			if (j == n)
#ifdef JUST_TESTING
			{
				/* We can much more rapidly locate the topmost available parent than
				 * the brute force method above. This is definitely the Right Way to
				 * go in 1.5
				 */
				int val = n;
				struct t_msgid *ptr;

				for (ptr = arts[n].refptr; ptr->parent; ptr = ptr->parent) {
					if (ptr->parent->article != ART_UNAVAILABLE)
						val = ptr->parent->article;
				}
				fprintf(stderr, "TEST: which_thread found %d, actual is %d\n", val, base[i]);

				return i;
			}
#else
				return i;
#endif /* JUST_TESTING */
		}
	}

	error_message (txt_cannot_find_base_art, n);

	return -1;
}


/*
 *  Find how deep in its' thread arts[n] is.  Start counting at zero
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

	if (n < 0)
		n = 0;

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
	sbuf->score = 0 /*-(SCORE_MAX) */;

	for (i = (int) base[n]; i >= 0; i = arts[i].thread) {
		++sbuf->total;
		if (arts[i].inrange)
			++sbuf->inrange;

		if (arts[i].delete_it)
			++sbuf->deleted;

		if (arts[i].status == ART_UNREAD) {
			++sbuf->unread;
			/* we use the maximum article score for the complete thread */
			if ((arts[i].score > sbuf->score) && (arts[i].score > 0))
				sbuf->score = arts[i].score;
			else {
				if ((arts[i].score < sbuf->score) && (sbuf->score <= 0))
					sbuf->score = arts[i].score;
			}
		} else if (arts[i].status == ART_WILL_RETURN)
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

	sbuf->art_mark = (sbuf->inrange ? tinrc.art_marked_inrange : (sbuf->deleted ? tinrc.art_marked_deleted : (sbuf->selected_unread ? tinrc.art_marked_selected : (sbuf->unread ? tinrc.art_marked_unread : (sbuf->seen ? tinrc.art_marked_return : ART_MARK_READ)))));
	return(sbuf->total);
}


/*
 *  Find the next response to arts[n].  Go to the next basenote if there
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
 *  Return -1 if we are are the start of the group
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
 *  return index in arts[] of the 'n'th response in thread base 'i'
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
 *  Find the next unread response to art[n] in this group. If no response is found
 *  from current point to the end restart from beginning of articles. If no more
 *  responses can be found, return -1
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
static void
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
 * string in prefix will be overwritten up to length len
 * prefix will always be terminated with \0
 * make sure prefix is at least len+1 bytes long (to hold the terminating
 * null byte)
 */
static void
make_prefix (
	struct t_msgid *art,
	char *prefix,
	int maxlen)
{
	char *buf;
	int prefix_ptr;
	int depth = 0;
	struct t_msgid *ptr;

	for (ptr = art->parent; ptr; ptr = ptr->parent)
		depth += (!EXPIRED (ptr) ? 1 : 0);

	if ((depth == 0) || (maxlen < 1)) {
		prefix[0] = '\0';
		return;
	}

	prefix_ptr = depth * 2 - 1;

	if (!(buf = my_malloc (prefix_ptr + 3)))
		return;	/* out of memory */

	strcpy (&buf[prefix_ptr], "->");
	buf[--prefix_ptr] = (has_sibling (art) ? '+' : '`');

	for (ptr = art->parent; prefix_ptr != 0; ptr = ptr->parent) {
		if (EXPIRED (ptr))
			continue;
		buf[--prefix_ptr] = ' ';
		buf[--prefix_ptr] = (has_sibling (ptr) ? '|' : ' ');
	}
	strncpy (prefix, buf, maxlen);
	prefix[maxlen] = '\0'; /* just in case strlen(buf) > maxlen */
	free (buf);
	return;
}
#endif /* !INDEX_DAEMON */
