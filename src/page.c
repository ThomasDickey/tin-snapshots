/*
 *  Project   : tin - a Usenet reader
 *  Module    : page.c
 *  Author    : I. Lea & R. Skrenta
 *  Created   : 1991-04-01
 *  Updated   : 1995-07-26
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

struct t_header note_h;

FILE *note_fp;					/* the body of the current article */

int MORE_POS;				/* set in set_win_size () */
int RIGHT_POS;				/* set in set_win_size () */
int glob_respnum;
int last_resp, this_resp;		/* previous & current article # in arts[] for '-' command */
int note_line;
int note_page;					/* what page we're on */

long note_mark[MAX_PAGES];	/* holds ftell() on beginning of each page */
long mark_body;				/* holds ftell() on beginning of message body */
long note_size;				/* stat() size in bytes of article */

t_bool note_end;				/* we're done showing this article */

static t_bool show_all_headers = FALSE;	/* CTRL-H with headers specified */

#ifndef INDEX_DAEMON
	static char buf2[HEADER_LEN+50];
	static char first_char;
	static char skip_include;

	static int rotate;						/* 0=normal, 13=rot13 decode */
	static int tabwidth = 8;

	static t_bool doing_pgdn;
	static t_bool show_prev_header = FALSE;	/* remember display status of last line */
	static t_bool tex2iso_article;
#endif /* !INDEX_DAEMON */


/*
 * Local prototypes
 */

#ifndef INDEX_DAEMON
	static int prompt_response (int ch, int respnum);
	static int show_last_page (void);
	static t_bool expand_ctrl_chars (char *tobuf, char *frombuf, int length, int do_rotation);
	static void add_persist (char *p_header, char *p_content);
	static void show_cont_header (int respnum);
	static void show_first_header (int respnum, char *group);
#	ifdef HAVE_METAMAIL
		static void show_mime_article (FILE *fp, struct t_article *art);
#	endif /* HAVE_METAMAIL */
#endif /* !INDEX_DAEMON */

#ifndef INDEX_DAEMON
/*
 * The main routine for viewing articles
 * Returns:
 *    >=0	normal exit - return a new base[] note
 *    <0	indicates some unusual condition. See GRP_* in tin.h for possible values
 *			GRP_QUIT		User is doing a 'Q'
 *			GRP_RETURN		Back to selection level due to 'T' command
 *			GRP_ARTFAIL		We didn't make it into the art - don't bother fixing the screen up
 *			GRP_NEXT		Catchup with 'c'
 *			GRP_NEXTUNREAD	   "      "  'C'
 */
int
show_page (
	struct t_group *group,
	int respnum,			/* index into arts[] */
	int *threadnum)			/* to allow movement in thread mode */
{
	char buf[LEN];
	char group_path[LEN];
	int ch, i, n = 0;
	int filter_state = NO_FILTERING;
	int old_sort_art_type = tinrc.sort_article_type;
	long art;
	struct stat note_stat;
	t_bool mouse_click_on = TRUE;

	filtered_articles = FALSE;	/* used in thread level */

	make_group_path (group->name, group_path);

restart:
	if (read_news_via_nntp)
		wait_message (0, txt_reading_article);

	glob_respnum = respnum;

	/* Remember current & previous articles for '-' command */
	if (respnum != this_resp) {
		last_resp = this_resp;
		this_resp = respnum;
	}

	rotate = 0;			/* normal mode, not rot13 */
	art = arts[respnum].artnum;

	switch (art_open (&arts[respnum], group_path, TRUE)) {

		case ART_UNAVAILABLE:
			art_mark_read (group, &arts[respnum]);
			wait_message (1, txt_art_unavailable);
			nobreak;	/* FALLTHROUGH */

		case ART_ABORT:
			return GRP_ARTFAIL;	/* special retcode to stop redrawing screen */

		default:					/* Normal case */
			break;
	}

	art_mark_read (group, &arts[respnum]);

	/* Get article size */
	if (fstat(fileno(note_fp), &note_stat) == -1)
		note_size = 0;
	else
		note_size = note_stat.st_size;

#ifdef HAVE_METAMAIL
	if (*note_h.mimeversion && *note_h.contenttype
	    && (!STRNCMPEQ("text/plain", note_h.contenttype, 10))
	    && tinrc.use_metamail) {
		if (tinrc.ask_for_metamail) {
			show_note_page (group->name, respnum);
			if (prompt_yn (cLINES, txt_use_mime, TRUE) == 1) {
				show_mime_article (note_fp, &arts[respnum]);
				note_page = 0;
				note_end = FALSE;
				rewind (note_fp);
				show_note_page (group->name, respnum);
			}
		} else {
			show_mime_article (note_fp, &arts[respnum]);
			show_note_page (group->name, respnum);
		}
	} else
		show_note_page (group->name, respnum);
#else
	show_note_page (group->name, respnum);
#endif /* HAVE_METAMAIL */

	forever {
		ch = ReadCh ();

		if (ch >= '0' && ch <= '9') {
			if (!HAS_FOLLOWUPS (which_thread (respnum)))
				info_message (txt_no_responses);
			else {
				n = prompt_response (ch, respnum);
				if (n != -1) {
					respnum = n;
					goto restart;
				}
			}
			continue;
		}
		switch (ch) {
			case ESC:
#ifdef HAVE_KEY_PREFIX
			case KEY_PREFIX:
#endif /* HAVE_KEY_PREFIX */
				switch (get_arrow_key (ch)) {
					case KEYMAP_LEFT:
						goto return_to_index;

					case KEYMAP_RIGHT:
						goto page_goto_next_unread;

					case KEYMAP_UP:
					case KEYMAP_PAGE_UP:
						goto page_up;

					case KEYMAP_DOWN:
					case KEYMAP_PAGE_DOWN:
						goto page_down;

					case KEYMAP_HOME:
						goto begin_of_article;

					case KEYMAP_END:
						goto end_of_article;

					case KEYMAP_MOUSE:
						switch (xmouse)
						{
							case MOUSE_BUTTON_1:
								if (xrow < 3 || xrow >= cLINES-1)
									goto page_down;
								goto page_goto_next_unread;
							case MOUSE_BUTTON_2:
								if (xrow < 3 || xrow >= cLINES-1)
									goto page_up;
								goto return_to_index;
							case MOUSE_BUTTON_3:
								if (mouse_click_on) {
									set_xclick_off ();
									mouse_click_on = FALSE;
								} else {
									set_xclick_on ();
									mouse_click_on = TRUE;
								}
								break;
							default:
								break;
						}
						break;
					default:
						break;
				}
				break;

#ifndef NO_SHELL_ESCAPE
			case iKeyShellEscape:
				shell_escape ();
				redraw_page (group->name, respnum);
				break;
#endif /* !NO_SHELL_ESCAPE */

			case iKeyLastPage:	/* goto end of article */
			case iKeyPageLastPage2:
end_of_article:
				if (show_last_page ())
					show_note_page (group->name, respnum);

				break;

			case iKeyPageLastViewed:	/* show last viewed article */
				if (last_resp < 0 || (which_thread(last_resp) == -1)) {
					info_message (txt_no_last_message);
					break;
				}
				art_close ();
				respnum = last_resp;
				goto restart;

			case iKeyLookupMessage:			/* Goto article by Message-ID */

				if ((i = prompt_msgid ()) != ART_UNAVAILABLE) {
					art_close ();
					respnum = i;

					goto restart;
				}
				break;

			case iKeyPageGotoParent:		/* Goto parent of this article */
			{
				struct t_msgid *parent = arts[respnum].refptr->parent;

				if (parent == NULL) {
					info_message(txt_art_parent_none);
					break;
				}

				if (parent->article == ART_UNAVAILABLE) {
					info_message(txt_art_parent_unavail);
					break;
				}

				if (arts[parent->article].killed) {
					info_message(txt_art_parent_killed);
					break;
				}

				art_close ();
				respnum = parent->article;

				goto restart;
			}

			case iKeyPagePipe:	/* pipe article/thread/tagged arts to command */
				feed_articles (FEED_PIPE, PAGE_LEVEL, group, respnum);
				break;

			case iKeyPageMail:	/* mail article/thread/tagged articles to somebody */
				feed_articles (FEED_MAIL, PAGE_LEVEL, group, respnum);
				break;

#ifndef DISABLE_PRINTING
			case iKeyPagePrint:	/* output art/thread/tagged arts to printer */
				feed_articles (FEED_PRINT, PAGE_LEVEL, group, respnum);
				break;
#endif /* !DISABLE_PRINTING */

			case iKeyPageRepost:	/* repost current article */
				feed_articles (FEED_REPOST, PAGE_LEVEL, group, respnum);
				break;

			case iKeyPageSave:	/* save article/thread/tagged articles */
				feed_articles (FEED_SAVE, PAGE_LEVEL, group, respnum);
				break;

			case iKeyPageAutoSaveTagged:	/* Auto-save tagged articles without prompting */
				if (index_point >= 0) {
					if (num_of_tagged_arts)
						feed_articles (FEED_AUTOSAVE_TAGGED, PAGE_LEVEL, &CURR_GROUP, (int) base[index_point]);
					else
						info_message (txt_no_tagged_arts_to_save);
					}
				break;

			case iKeySearchSubjF:	/* search forwards in article */
				if (search_article (TRUE))
					show_note_page (group->name, respnum);
				break;

			case iKeySearchBody:	/* article body search */
				if ((n = search_body (respnum)) != -1) {
					respnum = n;
					art_close ();
					goto restart;
				}
				break;

			case iKeyPageTopThd:	/* goto first article in current thread */
				if (arts[respnum].inthread) {
					n = which_thread (respnum);
					if (n >= 0 && base[n] != respnum) {
						assert (n < top_base);
						respnum = base[n];
						art_close ();
						goto restart;
					}
				}
				break;

			case iKeyPageBotThd:	/* goto last article in current thread */
				for (i = respnum; i >= 0; i = arts[i].thread)
					n = i;

				if (n != respnum) {
					respnum = n;
					art_close ();
					goto restart;
				}
				break;

			case iKeyPageDown:		/* page down or next response */
			case iKeyPageDown2:
			case iKeyPageDown3:
page_down:
				if (!tinrc.space_goto_next_unread) {
					if (note_page != ART_UNAVAILABLE) {
						if (note_end) {
							if (tinrc.pgdn_goto_next)
								art_close();
							else {
								doing_pgdn = FALSE;
								break;
							}
						} else {
							doing_pgdn = TRUE;
							show_note_page (group->name, respnum);
							break;
						}
					}
					if ((n = next_response (respnum)) == -1)
						return (which_thread (respnum));

					respnum = n;
					goto restart;
				} else {
					if ((note_page == ART_UNAVAILABLE) || note_end)
						goto page_goto_next_unread;

					doing_pgdn = TRUE;
					show_note_page (group->name, respnum);
				}
				break;

			case iKeyPageNextThd:
			case iKeyPageNextThd2:	/* go to start of next thread */
				art_close ();
				if ((n = next_thread (respnum)) == -1)
					return (which_thread (respnum));
				respnum = n;
				goto restart;

			case iKeyPageNextUnread:	/* goto next unread article */
page_goto_next_unread:
				skip_include = '\0';
				if (note_page != ART_UNAVAILABLE) {
					if (!(tinrc.tab_goto_next_unread || note_end)) {
						doing_pgdn = TRUE;
						show_note_page (group->name, respnum);
						break;
					}
					art_close();
				}
				if ((n = next_unread (next_response (respnum))) == -1)
					return (which_thread (respnum));
				respnum = n;
				goto restart;

#ifdef HAVE_PGP
			case iKeyPagePGPCheckArticle:
				if (pgp_check_article())
					redraw_page(group->name, respnum);
				break;
#endif /* HAVE_PGP */

			case iKeyPageToggleHeaders:	/* toggle display of article headers */
				if (note_page == ART_UNAVAILABLE) {
					if ((n = next_response (respnum)) == -1)
						return (which_thread (respnum));
					respnum = n;
					goto restart;
				} else {
					note_page = 0;
					note_end = FALSE;
					rewind (note_fp);
					show_all_headers = !show_all_headers;
					show_note_page (group->name, respnum);
				}
				break;

			case iKeyPageToggleTex2iso:	/* toggle german TeX to ISO latin1 style conversion */
				if ((tex2iso_supported = !tex2iso_supported))
					tex2iso_article = iIsArtTexEncoded (art, group_path);

				redraw_page (group->name, respnum);
				info_message (txt_toggled_tex2iso, (tex2iso_supported) ? "on" : "off");
				break;

			/* haeh? */
			case iKeyPageToggleTabs:	/* toggle tab stops 8 vs 4 */
				tabwidth = ((tabwidth == 8) ? 4 : 8);
				redraw_page (group->name, respnum);
				break;

			case iKeyPageQuickAutoSel:		/* quickly auto-select article */
			case iKeyPageQuickKill:		/* quickly kill article */
				if ((filtered_articles = quick_filter (
						(ch == iKeyPageQuickKill) ? FILTER_KILL : FILTER_SELECT,
						group, &arts[respnum])))
					goto return_to_index;

				redraw_page (group->name, respnum);
				break;

			case iKeyPageAutoSel:		/* auto-select article menu */
			case iKeyPageAutoKill:		/* kill article menu */
				if (filter_menu ((ch == iKeyPageAutoKill) ? FILTER_KILL : FILTER_SELECT, group, &arts[respnum])) {
					if ((filtered_articles = filter_articles (group)))
						goto return_to_index;
				}
				redraw_page (group->name, respnum);
				break;

			case iKeyRedrawScr:		/* redraw current page of article */
				my_retouch();
				redraw_page (group->name, respnum);
				break;

			case iKeyFirstPage:		/* goto beginning of article */
			case iKeyPageFirstPage2:
begin_of_article:
				if (note_page == ART_UNAVAILABLE) {
					ClearScreen ();
					my_printf (txt_art_unavailable/*, arts[respnum].artnum*/);
					my_flush ();
				} else {
					note_page = 0;
					note_end = FALSE;
					rewind (note_fp);
					show_note_page (group->name, respnum);
				}
				break;

			case iKeyPageToggleRot:
			case iKeyPageToggleRot2:	/* toggle rot-13 mode */
				rotate = (rotate ? 0 : 13);
				redraw_page (group->name, respnum);
				info_message (txt_toggled_rot13);
				break;

			case iKeySearchAuthF:	/* author search forward */
			case iKeySearchAuthB:	/* author search backward */
				if ((n = search (SEARCH_AUTH, respnum, (ch == iKeySearchAuthF))) < 0)
					break;
				respnum = n;
				goto restart;
				/* NOTREACHED */

			case iKeyPageUp:		/* page up */
			case iKeyPageUp2:
			case iKeyPageUp3:
page_up:
				if (note_page == ART_UNAVAILABLE) {
					art_close ();
					if ((n = prev_response (respnum)) == -1)
						return (respnum);

					respnum = n;
					goto restart;

				} else {
					if (note_page <= 1)
						info_message (txt_begin_of_art);
					else {
						note_page -= 2;
						note_end = FALSE;
						fseek (note_fp, note_mark[note_page], SEEK_SET);
						show_note_page (group->name, respnum);
					}
				}
				break;

			case iKeyPageCatchup:			/* catchup - mark read, goto next */
			case iKeyPageCatchupNextUnread:	/* goto next unread */
				sprintf(buf, txt_mark_thread_read, (ch == iKeyPageCatchupNextUnread) ? txt_enter_next_thread : "");
				if (!tinrc.confirm_action || prompt_yn (cLINES, buf, TRUE) == 1) {
					thd_mark_read (group, base[which_thread(respnum)]);
					art_close ();
					return (ch == iKeyPageCatchupNextUnread) ? GRP_NEXTUNREAD : GRP_NEXT;
				}
				break;

			case iKeyPageMarkThdUnread:
				thd_mark_unread (group, base[which_thread(respnum)]);
				/* FIXME: replace 'Thread' by 'Article' if THREAD_NONE */
				info_message(txt_marked_as_unread, "Thread");
				break;

			case iKeyPageCancel:			/* cancel an article */
				if (can_post) {
					if (cancel_article (group, &arts[respnum], respnum))
						redraw_page (group->name, respnum);
				} else
					info_message (txt_cannot_post);
				break;

			case iKeyPageEdit:				/* edit an article (mailgroup only) */
				if (iArtEdit (group, &arts[respnum])) {
					goto restart;
					/* redraw_page (group->name, respnum); */
				}
				break;

			case iKeyPageFollowupQuote:		/* post a followup to this article */
			case iKeyPageFollowupQuoteHeaders:
			case iKeyPageFollowup:
				if (!can_post) {
					info_message (txt_cannot_post);
					break;
				}
				(void) post_response (group->name, respnum,
				  (ch == iKeyPageFollowupQuote || ch == iKeyPageFollowupQuoteHeaders) ? TRUE : FALSE,
				  ch == iKeyPageFollowupQuoteHeaders ? TRUE : FALSE);
				redraw_page (group->name, respnum);
				break;

			case iKeyPageHelp:	/* help */
				show_info_page (HELP_INFO, help_page, txt_art_pager_com);
				redraw_page (group->name, respnum);
				break;

			case iKeyPageToggleHelpDisplay:	/* toggle mini help menu */
				toggle_mini_help (PAGE_LEVEL);
				redraw_page (group->name, respnum);
				break;

			case iKeyQuit:	/* return to index page */
return_to_index:
				art_close ();

				if (filter_state == NO_FILTERING && tinrc.sort_article_type != old_sort_art_type) {
					make_threads (group, TRUE);
				}

				i = which_thread (respnum);
				if (threadnum)
					*threadnum = which_response (respnum);

				if (filter_state == FILTERING || filtered_articles) {
					int old_top = top;
					long old_artnum = arts[respnum].artnum;
					filter_articles (group);
					make_threads (group, FALSE);
					i = find_new_pos (old_top, old_artnum, i);
				}

				return i;

			case iKeyPageToggleInverseVideo:	/* toggle inverse video */
				toggle_inverse_video ();
				redraw_page (group->name, respnum);
				show_inverse_video_status ();
				break;

#ifdef HAVE_COLOR
			case iKeyToggleColor:		/* toggle color */
				if (toggle_color ()) {
					redraw_page (group->name, respnum);
					show_color_status ();
				}
				break;
#endif /* HAVE_COLOR */

			case iKeyPageKillThd:	/* mark rest of thread as read */
				thd_mark_read (group, respnum);
				if ((n = next_unread (next_response (respnum))) == -1)
					goto return_to_index;
				art_close ();
				respnum = n;
				goto restart;
				/* NOTREACHED */

			case iKeyPageListThd:	/* -> thread page that this article is in */
				art_close ();
				fixup_thread (respnum, FALSE);
				return GRP_GOTOTHREAD;

			case iKeyOptionMenu:	/* option menu */
				if (change_config_file (group) == FILTERING)
					filter_state = FILTERING;
				set_subj_from_size (cCOLS);
				redraw_page (group->name, respnum);
				break;

			case iKeyPageNextArt:	/* skip to next article */
				art_close ();
				if ((n = next_response (respnum)) == -1)
					return (which_thread(respnum));
				respnum = n;
				goto restart;
				/* NOTREACHED */

			/* TODO: combine this with iKeyPageNextUnreadArt */
			case iKeyPageKillArt:
				if (note_page != ART_UNAVAILABLE)
					art_close ();

				if ((n = next_unread (next_response(respnum))) == -1)
					return (which_thread (respnum));

				respnum = n;
				goto restart;
				/* NOTREACHED */

			case iKeyPageNextUnreadArt:	/* next unread article */
				if ((n = next_unread (next_response (respnum))) == -1)
					info_message (txt_no_next_unread_art);
				else {
					art_close ();
					respnum = n;
					goto restart;
				}
				break;

			case iKeyPagePrevArt:	/* previous article */
				art_close ();
				if ((n = prev_response (respnum)) == -1)
					return (respnum);

				respnum = n;
				goto restart;

			case iKeyPagePrevUnreadArt:	/* previous unread article */
				if ((n = prev_unread (prev_response (respnum))) == -1)
					info_message (txt_no_prev_unread_art);
				else {
					art_close ();
					respnum = n;
					goto restart;
				}
				break;

			case iKeyQuitTin:	/* quit */
				return GRP_QUIT;

			case iKeyPageReplyQuote:	/* reply to author through mail */
			case iKeyPageReplyQuoteHeaders:
			case iKeyPageReply:
				mail_to_author (group->name, respnum, (ch == iKeyPageReplyQuote || ch == iKeyPageReplyQuoteHeaders) ? TRUE : FALSE, ch == iKeyPageReplyQuoteHeaders ? TRUE : FALSE);
				redraw_page (group->name, respnum);
				break;

			case iKeyPageTag:	/* tag/untag article for saving */
				if (arts[respnum].tagged) {
					decr_tagged(arts[respnum].tagged);
					arts[respnum].tagged = 0;
					--num_of_tagged_arts;
					info_message (txt_untagged_art);
				} else {
					arts[respnum].tagged = ++num_of_tagged_arts;
					info_message (txt_tagged_art);
				}
				break;

			case iKeyPageGroupSel:	/* return to group selection page */
				art_close ();
				if (filter_state == FILTERING) {
					filter_articles (group);
					make_threads (group, FALSE);
				}
				return GRP_RETURN;

			case iKeyVersion:
				info_message (cvers);
				break;

			case iKeyPagePost:	/* post a basenote */
				if (post_article (group->name))
					redraw_page (group->name, respnum);
				break;

			case iKeyPostponed:
			case iKeyPostponed2:	/* post postponed article */
				if (can_post) {
					if (pickup_postponed_articles (FALSE, FALSE))
						redraw_page (group->name, respnum);
				} else
					info_message(txt_cannot_post);
				break;

			case iKeyDisplayPostHist:	/* display messages posted by user */
				if (user_posted_messages ())
					redraw_page (group->name, respnum);
				break;

			case iKeyPageMarkArtUnread:	/* mark article as unread (to return) */
				art_mark_will_return (group, &arts[respnum]);
				info_message (txt_marked_as_unread, "Article");
				break;

			case iKeyPageSkipIncludedText:	/* skip included text */
				skip_include = first_char;
				goto page_down;

			case iKeyToggleInfoLastLine: /* this is _not_ correct, we do not toggle status here */
				info_message ("%s", arts[respnum].subject);
				break;

#ifdef HAVE_COLOR
			case iKeyPageToggleHighlight:
				if (use_color) { /* make sure we have color turned on */
					word_highlight = !word_highlight;
					redraw_page(group->name, respnum);
					info_message(txt_toggled_high, (word_highlight) ? "on" : "off");
				}
				break;
#endif /* HAVE_COLOR */

			default:
				info_message(txt_bad_command);
		}
	}
	/* NOTREACHED */
	return GRP_ARTFAIL; /* default-value - I don't think we should get here */
}
#endif /* !INDEX_DAEMON */


void
redraw_page (
	char *group,
	int respnum)
{
#ifndef INDEX_DAEMON
	if (note_page == ART_UNAVAILABLE) {
		ClearScreen ();
		my_printf (txt_art_unavailable/*, arts[respnum].artnum*/);
		my_flush ();
	} else if (note_page > 0) {
		note_page--;
		fseek (note_fp, note_mark[note_page], SEEK_SET);
		show_note_page (group, respnum);
	}
#endif /* !INDEX_DAEMON*/
}


#ifndef INDEX_DAEMON
static t_bool
expand_ctrl_chars (
	char *tobuf,
	char *frombuf,
	int length,
	int do_rotation)
{
	char *p, *q;
	int i, j;
	t_bool ctrl_L = FALSE;

	for (p = frombuf, q = tobuf; *p && *p != '\n' && q < &tobuf[length]; p++) {
		if (*p == '\b' && q > tobuf) {
			q--;
		} else if (*p == '\t') {
			i = q - tobuf;
			j = ((i+tabwidth)/tabwidth) * tabwidth;

			while (i++ < j)
				*q++ = ' ';

		} else if (((*p) & 0xFF) < ' ') {
			*q++ = '^';
			*q++ = ((*p) & 0xFF) + '@';
			if (*p == 12)
				ctrl_L = TRUE;
		} else if (do_rotation) {
			if (*p >= 'A' && *p <= 'Z')
				*q++ = (*p - 'A' + do_rotation) % 26 + 'A';
			else if (*p >= 'a' && *p <= 'z')
				*q++ = (*p - 'a' + do_rotation) % 26 + 'a';
			else
				*q++ = *p;
		} else
			*q++ = *p;
	}
	*q = '\0';

	return ctrl_L;
}
#endif /* !INDEX_DAEMON */


/*
 * This is the core routine that actually gets a chunk of article onto the
 * display
 */
#ifndef INDEX_DAEMON
void
show_note_page (
	char *group,
	int respnum)
{
	char buf3[2*HEADER_LEN+200];
	int lines;
	int i;
	static char buf[HEADER_LEN];
	t_bool below_sig;				/* are we in the signature? */
	t_bool ctrl_L = FALSE;		/* form feed character detected */
	t_bool do_display_header;
	t_bool first = TRUE;

	signal_context = cPage;

	lines = (tinrc.beginner_level ? (cLINES - (MINI_HELP_LINES - 1)) : cLINES);

	ClearScreen ();

	note_line = 1;

	if (!note_page) {
		buf2[0] = '\0';
		doing_pgdn = FALSE;
		show_first_header (respnum, group);
	} else
		show_cont_header (respnum);

	if (skip_include)
		note_page--;

	below_sig = FALSE;				/* begin of article -> not in signature */

	while (note_line < lines) { /* loop show_note_page */

		if (tinrc.show_last_line_prev_page) {
			note_mark[note_page+1] = ftell (note_fp);
			if (doing_pgdn && first && buf2[0])
				goto print_a_line;
		}
		first = FALSE;
		if (fgets (buf, (int) sizeof(buf), note_fp) == 0) {
			note_end = TRUE;
			skip_include = '\0';
			break;
		}

		in_headers = (ftell (note_fp) < mark_body); /* still in header? */

		if (in_headers && !show_all_headers) {
			if (*buf == ' ' || *buf == '\t')	{	/* continuation line */
				if (!show_prev_header)	/* last line was not displayed */
					continue;				/* so don't display this line, too */
			} else {
				/* no continuation line */
				do_display_header = FALSE; /* default: don't display header */
				if (num_headers_to_display
					 && (news_headers_to_display_array[0][0] == '*')) {
					do_display_header = TRUE; /* wild do */
				} else {
					for (i = 0; i < num_headers_to_display; i++) {
						if (!strncasecmp (buf, news_headers_to_display_array[i],
											strlen (news_headers_to_display_array[i]))) {
							do_display_header = TRUE;
							break;
						}
					}
				}
				if (num_headers_to_not_display
					 && (news_headers_to_not_display_array[0][0] == '*')) {
					do_display_header = FALSE; /* wild don't: doesn't make sense! */
				} else {
					for (i = 0; i < num_headers_to_not_display; i++) {
						if (!strncasecmp (buf, news_headers_to_not_display_array[i],
									  strlen (news_headers_to_not_display_array[i]))) {
							do_display_header = FALSE;
							break;
						}
					}
				}

				/* do_display_header is set if line should be displayed */
				show_prev_header = do_display_header;	/* remember for cont. */
				if (!do_display_header)
					continue;
			} /* endif continuation line */
		} /* endif in_headers && !show_all_headers */

		buf[sizeof (buf) - 1] = '\0';

		ctrl_L = expand_ctrl_chars(buf2, buf, sizeof (buf), rotate);

print_a_line:
		if (first)
			StartInverse ();

		if (!strcmp (buf2, "-- "))
			below_sig = TRUE;			/* begin of signature */

		strip_line (buf2);

		/*
		 * RFC 2047 headers spanning two or more lines should be
		 * concatenated, but it's not done yet for fear that it may
		 * disrupt other parts.
		 */

		if (in_headers
			 && ((!tinrc.display_mime_header_asis && !show_all_headers)
					|| (!tinrc.display_mime_allheader_asis && show_all_headers)))  {
			/* check if it's a continuation header line */
			if (buf2[0] != ' ' && buf2[0] != '\t') {
				/*
				 * necessary, if there were only blanks in the header line, which
				 * are stripped by strip_line (buf2) above
				 */
				if (strstr (buf2, ": ")) {
					char header_name[80];	/* is the a length limit for a header-name ?*/
					size_t header_name_len;

					header_name_len = strstr (buf2, ": ") - buf2;

					if (header_name_len >= 79)
						header_name_len = 79;

					strncpy(header_name, buf2, header_name_len);
					header_name[header_name_len] = '\0';
					match_header(buf2, header_name, buf3, (char *) 0, HEADER_LEN);
					strncpy(buf2 + header_name_len + 2, buf3, HEADER_LEN - 2 - header_name_len);
				}
			} else
				strcpy(buf2, rfc1522_decode(buf2));
		}

		if (tex2iso_supported && tex2iso_article) {
			strcpy (buf3, buf2);
			ConvertTeX2Iso (buf3, buf2);
		}

		if (iso2asc_supported >= 0) {
			strcpy (buf3, buf2);
			ConvertIso2Asc (buf3, buf2, iso2asc_supported);
		}

		ConvertBody2Printable (buf2);

		first_char = buf2[0] ? buf2[0] : first_char;

		if (!below_sig || tinrc.show_signatures) {
			if (!skip_include  || (first_char != skip_include)) {
				if (skip_include) {
					skip_include = '\0';
					note_page++;
				}
#	ifdef HAVE_COLOR
#		ifdef USE_CURSES
				move(note_line - 1, 0);
#		endif /* USE_CURSES */
				print_color (buf2, below_sig);
#	else
#		ifdef USE_CURSES
#			if 0 /* this cut's off log header lines after cCOLS bytes */
				WriteLine(note_line - 1, buf2);
#			else
				my_fputs(buf2, stdout);
				my_fputs(cCRLF, stdout);
#			endif /* 0 */
#		else
				my_printf ("%s" cCRLF, buf2);
#		endif /* USE_CURSES */
#	endif /* HAVE_COLOR */
				note_line += ((int) (strlen (buf2) - 1) / cCOLS) + 1;
			}
		}

		if (first)
			EndInverse ();

		first = FALSE;
		doing_pgdn = FALSE;

		if (ctrl_L)
			break;

	} /* loop show_note_page */

	if (!tinrc.show_last_line_prev_page)
		note_mark[++note_page] = ftell (note_fp);
	else
		note_page++;

	if (ftell (note_fp) == note_size)
		note_end = TRUE;

#	ifdef HAVE_COLOR
	fcol(tinrc.col_text);
#	endif /* HAVE_COLOR */
	if (note_end) {
		MoveCursor (cLINES, MORE_POS-(5+BLANK_PAGE_COLS));
		StartInverse ();

		my_fputs (((arts[respnum].thread != -1) ? txt_next_resp : txt_last_resp), stdout);

		my_flush ();
		EndInverse ();
	} else {
		if (note_size > 0)
			draw_percent_mark (note_mark[note_page], note_size);
		else {
			MoveCursor (cLINES, MORE_POS-BLANK_PAGE_COLS);
			StartInverse ();
			my_fputs (txt_more, stdout);
			my_flush ();
			EndInverse ();
		}
	}

	show_mini_help (PAGE_LEVEL);

	stow_cursor();
}
#endif /* !INDEX_DAEMON */


#if defined(HAVE_METAMAIL) && !defined(INDEX_DAEMON)
static void
show_mime_article (
	FILE *fp,
	struct t_article *art)
{
	FILE *mime_fp;
	char *mm;
	char buf[PATH_LEN];
	long offset;

	offset = ftell (fp);
	rewind (fp);
	if ((mm = getenv("METAMAIL")))
		sprintf (buf, mm);
	else
		sprintf (buf, METAMAIL_CMD, PATH_METAMAIL);

	EndWin();
	Raw(FALSE);
	if ((mime_fp = popen (buf, "w"))) {
		while (fgets (buf, (int) sizeof(buf), fp) != 0)
			fputs (buf, mime_fp);

		fflush (mime_fp);
		pclose (mime_fp);
	} else
		info_message (txt_error_metamail_failed, strerror(errno));

	/*
	 * if we don't set note_end the undecoded article is displayed
	 *  after metamail quits
	 */
#if 0
	note_end = TRUE;
#endif /* 0*/
	Raw(TRUE);
	InitWin ();
	continue_prompt ();

	fseek (fp, offset, SEEK_SET);	/* goto old position */
	MoveCursor (cLINES, MORE_POS-(5+BLANK_PAGE_COLS));
	StartInverse ();

	my_fputs (((art->thread != -1) ? txt_next_resp : txt_last_resp), stdout);

	my_flush ();
	EndInverse ();
}
#endif /* defined(HAVE_METAMAIL) && !defined(INDEX_DAEMON) */


#ifndef INDEX_DAEMON
static void
show_first_header (
	int respnum,
	char *group)
{
	char buf[HEADER_LEN];
	char tmp[LEN];
	int whichresp;
	int x_resp;
	int pos, i, n;
	int grplen, maxlen;

	whichresp = which_response (respnum);
	x_resp = num_of_responses (which_thread (respnum));

	ClearScreen ();

	if (!my_strftime (buf, sizeof (buf), "%a, %d %b %Y %H:%M:%S",
							localtime (&arts[respnum].date)))
		strcpy (buf, note_h.date);

	/*
	 * Work out how much room we have for group name, allow 1 space before and
	 * after it
	 */
	grplen = strlen(group);
	maxlen = RIGHT_POS - strlen(buf) - 2;

	if (grplen < maxlen)
		maxlen = grplen;

	/*
	 * Aesthetics - Add 3 to compensate for the fact that
	 * the left hand margin (date) is longer than the right hand margin
	 */
	pos = 3 + (cCOLS - maxlen) / 2;

	for (i = strlen(buf); i < pos; i++)		/* Pad out to left */
		buf[i] = ' ';

	buf[i] = '\0';

	if (maxlen != grplen) {					/* ie groupname was too long */
		strncat (buf, group, maxlen-3);
		strcat (buf, "...");
	} else
		strncat (buf, group, maxlen);

	for (i = strlen(buf); i < RIGHT_POS; i++)	/* Pad out to right */
		buf[i] = ' ';

	buf[i] = '\0';

#	ifdef HAVE_COLOR
	fcol(tinrc.col_head);
#	endif /* HAVE_COLOR */

	/* Displaying the value of X-Comment-To header in the upper right corner */
	if (note_h.ftnto[0] && tinrc.show_xcommentto) {
		char ftbuf[HEADER_LEN]; /* FTN-To aka X-Comment-To */
		my_fputs (buf, stdout);
		strip_address(note_h.ftnto, ftbuf);
		ftbuf[19] = '\0';
		Convert2Printable (ftbuf);
		StartInverse ();
		my_fputs (ftbuf, stdout);
		EndInverse ();
		my_fputs (cCRLF, stdout);
	} else {
		char x[5];

		/* Can't eval tin_ltoa() more than once in a statement due to statics */
		strcpy(x, tin_ltoa(which_thread(respnum) + 1, 4));

		sprintf (tmp, txt_thread_x_of_n, buf, x, tin_ltoa(top_base, 4));
		my_fputs (tmp, stdout);
	}

	if (arts[respnum].lines < 0)
		strcpy (tmp, "?");
	else
		sprintf (tmp, "%-4d", arts[respnum].lines);

#	ifdef HAVE_COLOR
	fcol(tinrc.col_head);
#	endif /* HAVE_COLOR */

	sprintf (buf, txt_lines, tmp);
	n = strlen (buf);
	my_fputs (buf, stdout);

#	ifdef HAVE_COLOR
	fcol(tinrc.col_subject);
#	endif /* HAVE_COLOR */

	if (tex2iso_article) {
		*buf = '\0';
		strcpy (buf, "TeX ");
		n += strlen (buf);
		my_fputs (buf, stdout);
	}

	strncpy (buf, (*note_h.subj ? note_h.subj : arts[respnum].subject), HEADER_LEN - 1);

	buf[RIGHT_POS - 5 - n] = '\0';

	pos = ((cCOLS - (int) strlen (buf)) / 2) - 2;

	MoveCursor (1, ((pos > n) ? pos : n));

	Convert2Printable (buf);

	StartInverse ();

	my_fputs (buf, stdout);
	EndInverse ();

#	ifdef HAVE_COLOR
	fcol(tinrc.col_response);
#	endif /* HAVE_COLOR */

	MoveCursor (1, RIGHT_POS);
	if (whichresp)
		my_printf (txt_resp_x_of_n, whichresp, x_resp);
	else {
		if (!x_resp)
			my_fputs (txt_no_resp, stdout);
		else if (x_resp == 1)
			my_fputs (txt_1_resp, stdout);
		else
			my_printf (txt_x_resp, x_resp);
	}

#	ifdef HAVE_COLOR
	fcol(tinrc.col_normal);
#	endif /* HAVE_COLOR */

	if (arts[respnum].name)
		sprintf (buf, "%s <%s>", arts[respnum].name, arts[respnum].from);
	else
		strncpy (buf, arts[respnum].from, cCOLS-1);
	buf[cCOLS-1] = '\0';

	if (*note_h.org) {
		sprintf (tmp, txt_at_s, note_h.org);
		tmp[sizeof(tmp)-1] = '\0';

		if ((int) strlen (buf) + (int) strlen (tmp) >= cCOLS -1) {
			strncat (buf, tmp, cCOLS - 1 - strlen(buf));
			buf[cCOLS-1] = '\0';
		} else {
			pos = cCOLS - 1 - (int) strlen(tmp);
			for (i = strlen(buf); i < pos; i++)
				buf[i] = ' ';
			buf[i] = '\0';
			strcat (buf, tmp);
		}
	}

	strip_line (buf);

	Convert2Printable (buf);

#	ifdef HAVE_COLOR
	fcol(tinrc.col_from);
#	endif /* HAVE_COLOR */

	my_printf ("%s" cCRLF cCRLF, buf);

#	ifdef HAVE_COLOR
	fcol(tinrc.col_normal);
#	endif /* HAVE_COLOR */

	note_line += 4;
}
#endif /* !INDEX_DAEMON */


#ifndef INDEX_DAEMON
static void
show_cont_header (
	int respnum)
{
	char *buf;
	int maxresp;
	int whichresp;
	int whichbase;

	whichresp = which_response (respnum);
	whichbase = which_thread (respnum);
	maxresp = num_of_responses (whichbase);

	assert (whichbase < top_base);

	/*
	 * the last term in the length of the buffer is mainly to shut
	 * checker up although we still depend on txt_thread_resp_page
	 * not being too long
	 */
	buf = (char *) my_malloc (strlen((arts[respnum].name ? arts[respnum].name : arts[respnum].from)) + strlen(note_h.subj) + cCOLS + 5*3*sizeof(int));

	if (whichresp) {
		sprintf(buf, txt_thread_resp_page,
			whichbase + 1,
			top_base,
			whichresp,
			maxresp,
			note_page + 1,
			arts[respnum].name ? arts[respnum].name : arts[respnum].from, note_h.subj);
	} else {
		sprintf(buf, txt_thread_page,
			whichbase + 1,
			top_base,
			note_page + 1,
			arts[respnum].name ? arts[respnum].name : arts[respnum].from, note_h.subj);
	}
	strip_line (buf);

	if (cCOLS)
		buf[cCOLS-1] = '\0';

	Convert2Printable (buf);

#	ifdef HAVE_COLOR
	fcol(tinrc.col_head);
#	endif /* HAVE_COLOR */

	my_printf("%s" cCRLF cCRLF, buf);

#	ifdef HAVE_COLOR
	fcol(tinrc.col_normal);
#	endif /* HAVE_COLOR */

	free(buf);

	note_line += 2;
}
#endif /* !INDEX_DAEMON */


#ifndef INDEX_DAEMON
/*
 * Returns:
 *		0				Art opened successfully
 *		ART_UNAVAILABLE	Couldn't find article
 *		ART_ABORT		User aborted during read of article
 */
int
art_open (
	struct t_article *art,
	char *group_path,
	t_bool rfc1521decode)
{
	char *ptr;
	char buf[8192];
	char p_header[8192];
	char p_content[8192];
	struct t_header_list *pptr, *pqtr;
	int c, is_summary;

	art_close ();	/* just in case */

	if ((tex2iso_article = (tex2iso_supported ? iIsArtTexEncoded (art->artnum, group_path) : FALSE)))
		wait_message (0, txt_is_tex_ecoded);

	if ((note_fp = open_art_fp (group_path, art->artnum, art->lines, rfc1521decode)) == (FILE *) 0) {
		note_page = ART_UNAVAILABLE;		/* Flag error for later */
		return ((tin_errno == 0) ? ART_UNAVAILABLE : ART_ABORT);
	}

	note_h.from[0] = '\0';
	note_h.path[0] = '\0';
	note_h.subj[0] = '\0';
	note_h.org[0] = '\0';
	note_h.date[0] = '\0';
	note_h.newsgroups[0] = '\0';
	note_h.messageid[0] = '\0';
	note_h.references[0] = '\0';
	note_h.distrib[0] = '\0';
	note_h.followup[0] = '\0';
	note_h.keywords[0] = '\0';
	note_h.summary[0] = '\0';
	note_h.mimeversion[0] = '\0';
	note_h.contenttype[0] = '\0';
	note_h.contentenc[0] = '\0';
	note_h.ftnto[0] = '\0';
	if (note_h.persist != NULL) {
		for (pptr = note_h.persist; pptr != NULL; pptr = pqtr) {
			pqtr = pptr->next;
			free (pptr);
		}
	}
	note_h.persist = NULL;

	while (fgets(buf, (int) sizeof(buf), note_fp) != 0) {
		buf[sizeof(buf)-1] = '\0';

		if (*buf == '\n')
			break;

		/* do not remove \n in Summary lines */
		/* FIXME */
		is_summary = (strncasecmp (buf, "Summary: ", 9) == 0);

		/* check for continued header line */
		while((c = peek_char(note_fp)) != EOF && isspace(c) && c != '\n' && (strlen(buf) < (sizeof(buf)-1))) {
			if (strlen(buf) > 0 && buf[strlen(buf)-1] == '\n') {
				if (!is_summary)
					buf[strlen(buf)-1] = '\0';
			}
			fgets(buf + strlen(buf), sizeof buf - strlen(buf), note_fp);
		}

		for (ptr = buf; *ptr && ((*ptr != '\n') || (ptr[1] != '\0')); ptr++) {
			if ((((*ptr) & 0xFF) < ' ') && (*ptr != '\n') && ((*ptr != '\t') || (!is_summary)))
				*ptr = ' ';
		}
		*ptr = '\0';

		if (match_header (buf, "Path", note_h.path, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "From", note_h.from, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "Subject", note_h.subj, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "Organization", note_h.org, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "Date", note_h.date, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "Newsgroups", note_h.newsgroups, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "Message-ID", note_h.messageid, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "References", note_h.references, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "Distribution", note_h.distrib, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "Followup-To", note_h.followup, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "Keywords", note_h.keywords, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "Summary", note_h.summary, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "Mime-Version", note_h.mimeversion, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "Content-Type", note_h.contenttype, (char*)0, HEADER_LEN)) {
			str_lwr (note_h.contenttype);
			continue;
		}
		if (match_header (buf, "Content-Transfer-Encoding", note_h.contentenc, (char*)0, HEADER_LEN)) {
			str_lwr (note_h.contentenc);
			continue;
		}
		if (match_header (buf, "X-Comment-To", note_h.ftnto, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "Author-IDs", note_h.authorids, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "P-Author-IDs", note_h.authorids, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "X-P-Author-IDs", note_h.authorids, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "P-", p_content, p_header, HEADER_LEN)) {
			add_persist (p_header, p_content);
			continue;
		}
		if (match_header (buf, "X-P-", p_content, p_header, HEADER_LEN)) {
			add_persist (p_header, p_content);
			continue;
		}
	}

	mark_body = ftell (note_fp);
	note_mark[0] = 0L;
	rewind (note_fp);
	note_end = FALSE;

	/*
	 * If Newsgroups is empty its a good bet the article is a mail article
	 */
	if (!*note_h.newsgroups) {
		strcpy (note_h.newsgroups, group_path);
		while ((ptr = strchr (note_h.newsgroups, '/')))
			*ptr = '.';
	}

	/* This is used as some warped success indicator in art_close() */
	note_page = 0;

	return 0;
}
#endif /* !INDEX_DAEMON */


void
art_close (
	void)
{
	show_all_headers = FALSE; /* start without displaying ALL headers */
	if (note_fp && note_page >= 0) {
		fclose (note_fp);
		note_fp = (FILE *) 0;
	}
}


#ifndef INDEX_DAEMON
static int
prompt_response (
	int ch,
	int respnum)
{
	int num;

	clear_message ();

	if ((num = prompt_num (ch, txt_read_resp)) == -1) {
		clear_message ();
		return -1;
	}

	return find_response (which_thread (respnum), num);
}


static int
show_last_page (
	void)
{
	char buf[LEN];
	char buf3[LEN+50];
	t_bool ctrl_L;		/* form feed character detected */

	if (note_end)
		return FALSE;

	while (!note_end) {
		note_line = (!note_page ? 5 : 3);

		while (note_line < cLINES) {
			if (fgets (buf, (int) sizeof(buf), note_fp) == 0) {
				note_end = TRUE;
				break;
			}
			buf[sizeof (buf)-1] = '\0';

			ctrl_L = expand_ctrl_chars (buf3, buf, sizeof (buf), 0);

			note_line += ((int) strlen (buf3) / cCOLS) + 1;

			if (ctrl_L)
				break;
		}
		if (note_mark[note_page] == note_size) {
			note_end = TRUE;
			note_page--;
			break;
		} else if (!note_end)
			note_mark[++note_page] = ftell(note_fp);
	}
	fseek (note_fp, note_mark[note_page], SEEK_SET);
	return TRUE;
}
#endif /* !INDEX_DAEMON */


/*
 * buf:  Article header
 * pat:  Text to match in header
 * body: Return buffer
 * len:  sizeof(body)
 *
 * Returns:
 *	FALSE	Header was not matched.
 *	TRUE	Header was matched. body contains NULL terminated content
 *			portion of buf (ie with pat: and leading space removed)
 */
t_bool
match_header (
	char *buf,
	const char *pat,
	char *body,
	char *nodec_body,
	size_t len)
{
	size_t hlen;
	size_t plen = strlen (pat);

	/* A quick check on the length before calling strnicmp() etc. */

	/*
	 * Does ': ' follow the header text?
	 * or are we searching for a prefix?
	 */
	if (strlen(buf) < plen + 2 || buf[plen] != ':' || buf[plen+1] != ' ')
		/* if (pat[plen-1] != '-') */
		if (!(body && nodec_body))
			return FALSE;

	/*
	 * If the header matches, skip past the ': ' and any leading whitespace
	 */
	if (strncasecmp(buf, pat, plen) == 0) {
		while (buf[plen] != ' ')
			plen++;
		hlen = plen - 1;
		plen++;
		while (buf[plen] == ' ')
			plen++;

		/*
		 * Copy the 'body' of the header into return string
		 */

#ifdef LOCAL_CHARSET
		/*
		 * we have a bit of a problem here, if the header
		 * contains 8 bit character, they were already
		 * converted to local charset in rfc1521_decode, they
		 * will be decoded again by rfc1522_decode in modified
		 * strncpy. We just convert the chars back to network
		 * charset for now, but this should be done
		 * differently, I would guess.
		 */
		buffer_to_network(buf+plen);
#endif /* LOCAL_CHARSET */

		if (body) {
			modifiedstrncpy (body, &buf[plen], len, TRUE);
			body[len - 1] = '\0';
		}

		if (body && nodec_body) {
			strncpy (nodec_body, buf, len);
			nodec_body[hlen] = '\0';
		} else if (nodec_body) {
			modifiedstrncpy (nodec_body, &buf[plen], len, FALSE);
			nodec_body[len - 1] = '\0';
		}

		return TRUE;
	}

	return FALSE;
}


#ifndef INDEX_DAEMON
static void
add_persist (
	char *p_header,
	char *p_content)
{
	struct t_header_list *pptr;

	if (note_h.persist != NULL) {
		for (pptr = note_h.persist; pptr->next != NULL; pptr = pptr->next)
			;
		pptr->next = (struct t_header_list *) my_malloc(sizeof(struct t_header_list));
		pptr = pptr->next;
		strcpy(pptr->header, p_header);
		strcpy(pptr->content, p_content);
		pptr->next = NULL;
	} else {
		note_h.persist = (struct t_header_list *) my_malloc(sizeof(struct t_header_list));
		strcpy(note_h.persist->header, p_header);
		strcpy(note_h.persist->content, p_content);
		note_h.persist->next = NULL;
	}
}
#endif /* !INDEX_DAEMON */
