/*
 *  Project   : tin - a Usenet reader
 *  Module    : page.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 26-07-95
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"
#include	"menukeys.h"

char note_h_from[HEADER_LEN];		/* From:         */
char note_h_path[HEADER_LEN];		/* Path:         */
char note_h_date[HEADER_LEN];		/* Date:         */
char note_h_subj[HEADER_LEN];		/* Subject:      */
char note_h_org[HEADER_LEN];		/* Organization: */
char note_h_newsgroups[HEADER_LEN];	/* Newsgroups:   */
char note_h_messageid[HEADER_LEN];	/* Message-ID:   */
char note_h_references[HEADER_LEN];	/* References:   */
char note_h_distrib[HEADER_LEN];	/* Distribution: */
char note_h_keywords[HEADER_LEN];	/* Keywords:     */
char note_h_summary[HEADER_LEN];	/* Summary:      */
char note_h_followup[HEADER_LEN];	/* Followup-To:  */
char note_h_mimeversion[HEADER_LEN];	/* Mime-Version: */
char note_h_contenttype[HEADER_LEN];	/* Content-Type: */
char note_h_contentenc[HEADER_LEN];	/* Content-Transfer-Encoding: */
char note_h_ftnto[HEADER_LEN];		/* Old X-Comment-To: (Used by FIDO) */

char *glob_page_group;

FILE *note_fp;				/* the body of the current article */
FILE *tmp_fp;				/* TODO: hack var for broken overview */

int glob_respnum;
int last_resp;				/* current & previous article for - command */
int note_end;				/* we're done showing this article */
int note_line;
int note_page;				/* what page we're on */
int rotate;				/* 0=normal, 13=rot13 decode */
int this_resp;
int doing_pgdn;
int tabwidth = 8;
int show_all_headers = 0;			/* CTRL-H with headers specified -- swp */
char skip_include;
char buf2[HEADER_LEN+50];
char first_char;

long note_mark[MAX_PAGES];		/* holds ftell() on beginning of each page */
long note_size;				/* stat() size in bytes of article */

static int tex2iso_article;

/*
 * Local prototypes
 */
#ifndef INDEX_DAEMON
#ifdef HAVE_METAMAIL
static void show_mime_article (FILE *fp, struct t_article *art);
#endif
static void show_first_header (int respnum, char *group);
static void show_cont_header (int respnum);
static int prompt_response (int ch, int respnum);
static int show_last_page (void);
#endif /* INDEX_DAEMON */

#ifndef INDEX_DAEMON
int
show_page (
	struct t_group *group,
	char *group_path,
	int respnum,		/* index into arts[] */
	int *threadnum)		/* to allow movement in thread mode */
{
	char buf[LEN];
	int ch, i, n = 0;
	int filter_state = NO_FILTERING;
	int mouse_click_on = TRUE;
	int old_sort_art_type = default_sort_art_type;
	int old_top;
	int posted_flag;
	int ret_code;
	long old_artnum;
	long art;

	local_filtered_articles = FALSE;	/* used in thread level */

restart:
	if (read_news_via_nntp)
		wait_message (txt_reading_article);

	glob_respnum = respnum;
	glob_page_group = group->name;

	set_signals_page ();

	/* Remember current & previous articles for '-' command */
	if (respnum != this_resp) {
		last_resp = this_resp;
		this_resp = respnum;
	}

	rotate = 0;			/* normal mode, not rot13 */
	art = arts[respnum].artnum;

	art_mark_read (group, &arts[respnum]);

	if ((note_page = art_open (art, group_path)) == ART_UNAVAILABLE) {
		wait_message (txt_art_unavailable);
		return GRP_NOREDRAW;	/* special retcode to stop redrawing screen */
	} else {
		if (num_headers_to_display || num_headers_to_not_display) {
			note_page = 0;
			note_end = FALSE;
			fseek(note_fp, 0L, 0);
		}
		show_note_page (group->name, respnum);
	}

	forever {
		ch = ReadCh ();

		if (ch >= '0' && ch <= '9') {
			n = which_thread (respnum);
			if (!num_of_responses (n)) {
				info_message (txt_no_responses);
			} else {
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
#endif
				switch (get_arrow_key ()) {
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
								if (xrow < 3 || xrow >= cLINES-1) {
									goto page_down;
								}
								goto page_goto_next_unread;
							case MOUSE_BUTTON_2:
								if (xrow < 3 || xrow >= cLINES-1) {
									goto page_up;
								}
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
#endif

			case iKeyLastPage:	/* goto end of article */
			case iKeyPageLastPage2:
end_of_article:
				if (show_last_page ()) {
					show_note_page (group->name, respnum);
				}
				break;

			case iKeyPageLastViewed:	/* show last viewed article */
				if (last_resp < 0) {
					info_message (txt_no_last_message);
					break;
				}
				art_close ();
				respnum = last_resp;
				goto restart;

			case iKeyLookupMessage:			/* Goto article by Message-ID */

				if (prompt_string(txt_enter_message_id, buf+1) && buf[1]) {
					char *ptr = buf+1;
					struct t_msgid *msgid;

					/*
					 * If the user failed to supply Message-ID in <>, add them
					 */
					if (buf[1] != '<') {
						buf[0] = '<';
						strcat(buf, ">");
						ptr = buf;
					}

					if ((msgid = find_msgid(ptr)) == NULL) {
						info_message(txt_art_unavailable);
						break;
					}

					/*
					 * Is it expired or otherwise not on the spool ?
					 */
					if (msgid->article == ART_NORMAL) {
						info_message(txt_art_unavailable);
						break;
					}

					/*
					 * Goto this article 
					 */
					art_close ();
					respnum = msgid->article;
					goto restart;
				}
				break;

			case iKeyPageGotoParent:		/* Goto parent of this article */

				if (arts[respnum].refptr->parent == NULL) {
					info_message(txt_art_parent_none);
					break;
				}

				if (arts[respnum].refptr->parent->article == ART_NORMAL) {
					info_message(txt_art_parent_unavail);
					break;
				}

				/* TODO: fix this hack properly */
				/* See if parent really exists in case overview is broken */
				if ((tmp_fp = open_art_fp (group_path,
						arts[arts[respnum].refptr->parent->article].artnum)) == (FILE *) 0) {
					info_message(txt_art_parent_unavail);
					break;
				}
				fclose (tmp_fp);
				/* TODO: end of hack */

				art_close ();
				respnum = arts[respnum].refptr->parent->article;
				goto restart;


			case iKeyPagePipe:	/* pipe article/thread/tagged arts to command */
				feed_articles (FEED_PIPE, PAGE_LEVEL, group, respnum);
				break;

			case iKeySearchSubjF:	/* search forwards in article */
				if (search_article (TRUE)) {
					show_note_page (group->name, respnum);
				}
				break;

			case iKeyPageBSearchBody:	/* article body search */
				n = search_body (group, respnum);
				if (n != -1) {
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
				for (i = respnum; i >= 0; i = arts[i].thread) {
					n = i;
				}
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
				if (!space_goto_next_unread) {
					if (note_page != ART_UNAVAILABLE) {
						if (note_end) {
							art_close();
						} else {
							doing_pgdn = TRUE;
							show_note_page (group->name, respnum);
							break;
						}
					}
					n = next_response (respnum);
					if (n == -1) {
						return (which_thread (respnum));
					}
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
				n = next_thread (respnum);
				if (n == -1)
					return (which_thread (respnum));
				respnum = n;
				goto restart;

			case iKeyPageNextUnread: 	/* goto next unread article */
page_goto_next_unread:
				skip_include = '\0';
				if (note_page != ART_UNAVAILABLE) {
					if (!(tab_goto_next_unread || note_end)) {
						doing_pgdn = TRUE;
						show_note_page (group->name, respnum);
						break;
					}
					art_close();
				}
				n = next_unread (next_response (respnum));
				if (n == -1)
					return (which_thread (respnum));
				respnum = n;
				goto restart;

#ifdef HAVE_PGP
			case iKeyPagePGPCheckArticle:
				if (pgp_check_article())
					redraw_page(group->name, respnum);
				break;
#endif

			case iKeyPageDisplayHeaders:	/* show article headers */
				if (note_page == ART_UNAVAILABLE) {
					n = next_response (respnum);
					if (n == -1)
						return (which_thread (respnum));
					respnum = n;
					goto restart;
				} else {
					note_page = 0;
					note_end = FALSE;
					fseek(note_fp, 0L, 0);
					show_all_headers = 1;
					show_note_page (group->name, respnum);
					show_all_headers = 0;
				}
				break;

			case iKeyPageToggleTex2iso:	/* toggle german TeX to ISO latin1 style conversion */
				if ((tex2iso_supported = !tex2iso_supported)) {
					tex2iso_article = iIsArtTexEncoded (art, group_path);
				}
				redraw_page (group->name, respnum);
				if (tex2iso_supported) {
					info_message (txt_toggled_tex2iso_on);
				} else {
					info_message (txt_toggled_tex2iso_off);
				}
				break;

			case iKeyPageToggleTabs:	/* toggle tab stops 8 vs 4 */
				if (tabwidth == 8)
					tabwidth = 4;
				else
					tabwidth = 8;
				redraw_page (group->name, respnum);
				break;

			case iKeyThreadQuickAutosel:		/* quickly auto-select article */
				local_filtered_articles = quick_filter_select (group, &arts[respnum]);
				if (local_filtered_articles) {
					goto return_to_index;
				}
				redraw_page (group->name, respnum);
				break;

			case iKeyThreadQuickKill:		/* quickly kill article */
				local_filtered_articles = quick_filter_kill (group, &arts[respnum]);
				if (local_filtered_articles) {
					goto return_to_index;
				}
				redraw_page (group->name, respnum);
				break;

			case iKeyPageAutoSel:		/* auto-select article menu */
				if (filter_menu (FILTER_SELECT, group, &arts[respnum])) {
					local_filtered_articles = filter_articles (group);
					if (local_filtered_articles) {
						goto return_to_index;
					}
				}
				redraw_page (group->name, respnum);
				break;

			case iKeyPageAutoKill:		/* kill article menu */
				if (filter_menu (FILTER_KILL, group, &arts[respnum])) {
					local_filtered_articles = filter_articles (group);
					if (local_filtered_articles) {
						goto return_to_index;
					}
				}
				redraw_page (group->name, respnum);
				break;

			case iKeyPageRedrawScr:		/* redraw current page of article */
				my_retouch();
				redraw_page (group->name, respnum);
				break;

			case iKeyFirstPage:		/* goto beginning of article */
			case iKeyPageFirstPage2:
			case iKeyPageFirstPage3:
begin_of_article:
				if (note_page == ART_UNAVAILABLE) {
					ClearScreen ();
					my_printf (txt_art_unavailable, arts[respnum].artnum);
					my_flush ();
				} else {
					note_page = 0;
					note_end = FALSE;
					/* so we get any headers -- swp */
					if (num_headers_to_display || num_headers_to_not_display) {
						fseek (note_fp, 0L, 0);
					} else {
						fseek (note_fp, note_mark[0], 0);
					}
					show_note_page (group->name, respnum);
				}
				break;

			case iKeyPageToggleRot:
			case iKeyPageToggleRot2:	/* toggle rot-13 mode */
				rotate = (rotate ? 0 : 13);
				redraw_page (group->name, respnum);
				info_message (txt_toggled_rot13);
				break;

			case iKeyPageFsearchAuth:	/* author search forward */
			case iKeyPageBsearchAuth:	/* author search backward */
				i = (ch == iKeyPageFsearchAuth);
				n = search_author (my_group[cur_groupnum], respnum, i);
				if (n < 0)
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
					n = prev_response (respnum);
					if (n == -1)
						return (which_response (respnum));
					respnum = n;
					goto restart;

				} else {
					if (note_page <= 1) {
						info_message (txt_begin_of_art);
					} else {
						note_page -= 2;
						note_end = FALSE;
						/* to get any headers -- swp */
						if (note_page <= 1 && (num_headers_to_display || num_headers_to_not_display)) {
							fseek (note_fp, 0L, 0);
						} else {
							fseek (note_fp, note_mark[note_page], 0);
						}
						show_note_page (group->name, respnum);
					}
				}
				break;

			case iKeyPageCatchup:	/* catchup - mark all articles as read */
			case iKeyPageCatchupGotoNext:	/* and goto next group */
				if (!confirm_action || prompt_yn (cLINES, txt_mark_all_read, TRUE) == 1) {
					grp_mark_read (group, arts);
					ret_code = (ch == iKeyPageCatchupGotoNext ? GRP_CONTINUE : GRP_UNINDEXED);
					if (!(cur_groupnum + 1 < group_top)) {
						ret_code = GRP_UNINDEXED;
					}
					art_close ();
					space_mode = TRUE;
					return ret_code;
				}
				break;

			case iKeyPageCancel:	/* delete an article */
				if (cancel_article (group, &arts[respnum], respnum)) {
					redraw_page (group->name, respnum);
				}
				break;

			case iKeyPageEdit:	/* edit an article (mailgroup only) */
				if (iArtEdit (group, &arts[respnum])) {
					goto restart;
					/* redraw_page (group->name, respnum); */
				}
				break;

			case iKeyPageFollowupQuote:	/* post a followup to this article */
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

				if (filter_state == NO_FILTERING &&
					default_sort_art_type != old_sort_art_type) {
					make_threads (group, TRUE);
					find_base (group);
				}

				i = which_thread (respnum);
				*threadnum = which_response (respnum);

				if (filter_state == FILTERING || local_filtered_articles) {
					old_top = top;
					old_artnum = arts[respnum].artnum;
					filter_articles (group);
					make_threads (group, FALSE);
					find_base (group);
					i = find_new_pos (old_top, old_artnum, i);
				}

				return i;

			case iKeyPageToggleInverseVideo:	/* toggle inverse video */
				toggle_inverse_video ();
				redraw_page (group->name, respnum);
				show_inverse_video_status ();
				break;

#ifdef HAVE_COLOR
			case iKeyPageToggleColor:		/* toggle color */
				toggle_color ();
				redraw_page (group->name, respnum);
				show_color_status ();
				break;
#endif

			case iKeyPageKillArt:
				if (note_page == ART_UNAVAILABLE) {
					n = next_unread (next_response(respnum));
					if (n == -1)
						return (which_thread (respnum));
				} else {
					art_close ();
					n = next_unread (next_response (respnum));
					if (n == -1)
						return (which_thread (respnum));
				}
				respnum = n;
				goto restart;
				/* NOTREACHED */

			case iKeyPageKillThd:	/* mark rest of thread as read */
				thd_mark_read (group, respnum);
				n = next_unread (next_response (respnum));
				if (n == -1)
					goto return_to_index;
				art_close ();
				respnum = n;
				goto restart;
				/* NOTREACHED */

			case iKeyPageMail:	/* mail article/thread/tagged articles to somebody */
				feed_articles (FEED_MAIL, PAGE_LEVEL, group, respnum);
				break;

			case iKeyOptionMenu:	/* option menu */
				if (change_config_file (group, FALSE) == FILTERING) {
					filter_state = FILTERING;
				}
				set_signals_page ();		/* Just to be sure */
				set_subj_from_size (cCOLS);
				redraw_page (group->name, respnum);
				break;

			case iKeyPageNextArt:	/* skip to next article */
				art_close ();
				n = next_response (respnum);
				if (n == -1)
					return (which_thread(respnum));
				respnum = n;
				goto restart;
				/* NOTREACHED */

			case iKeyPageNextUnreadArt:	/* next unread article */
				n = next_unread (next_response (respnum));
				if (n == -1)
					info_message (txt_no_next_unread_art);
				else {
					art_close ();
					respnum = n;
					goto restart;
				}
				break;

			case iKeyPagePrint:	/* output art/thread/tagged arts to printer */
				feed_articles (FEED_PRINT, PAGE_LEVEL, group, respnum);
				break;

			case iKeyPagePrevArt:	/* previous article */
				art_close ();
				n = prev_response (respnum);
				if (n == -1)
					return (which_response (respnum));
				respnum = n;
				goto restart;

			case iKeyPagePrevUnreadArt:	/* previous unread article */
				n = prev_unread (prev_response (respnum));
				if (n == -1)
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
				mail_to_author (group->name, respnum,
				  (ch == iKeyPageReplyQuote || ch == iKeyPageReplyQuoteHeaders) ? TRUE : FALSE,
				  ch == iKeyPageReplyQuoteHeaders ? TRUE : FALSE);
				redraw_page (group->name, respnum);
				break;

			case iKeyPageSave:	/* save article/thread/tagged articles */
				feed_articles (FEED_SAVE, PAGE_LEVEL, group, respnum);
				break;

			case iKeyPageTag:	/* tag/untag article for saving */
				if (arts[respnum].tagged) {
					arts[respnum].tagged = 0;
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
					find_base (group);
				}
				return -1;

			case iKeyVersion:
				info_message (cvers);
				break;

			case iKeyPagePost:	/* post a basenote */
				if (post_article (group->name, &posted_flag)) {
					redraw_page (group->name, respnum);
				}
				break;

			case iKeyPagePostponed:	/* post postponed article */
				if (pickup_postponed_articles (FALSE, FALSE)) {
					redraw_page (group->name, respnum);
				}
				break;

			case iKeyPagePostHist:	/* display messages posted by user */
				if (user_posted_messages ()) {
					redraw_page (group->name, respnum);
				}
				break;

			case iKeyPageRepost:	/* repost current article */
				feed_articles (FEED_REPOST, PAGE_LEVEL, group, respnum);
				break;

			case iKeyPageMarkArtUnread:	/* mark article as unread (to return) */
				art_mark_will_return (group, &arts[respnum]);
				info_message (txt_art_marked_as_unread);
				break;

			case iKeyPageSkipIncludedText:	/* skip included text */
				skip_include = first_char;
				goto page_down;
				/* break; */

			case iKeyPageDisplaySubject:
				info_message(arts[respnum].subject);
				break;

#ifdef HAVE_COLOR
			case iKeyPageToggleHighlight:
				if(use_color) { /* make sure we have color turned on */
					word_highlight = !word_highlight;
					redraw_page(group->name, respnum);
					if (word_highlight)
						info_message(txt_toggled_high_on);
					else
						info_message(txt_toggled_high_off);
				}
				break;
#endif				

			default:
				info_message(txt_bad_command);
		}
	}
}
#endif /* INDEX_DAEMON */


void
redraw_page (
	char *group,
	int respnum)
{
	if (note_page == ART_UNAVAILABLE) {
		ClearScreen ();
		my_printf (txt_art_unavailable, arts[respnum].artnum);
		my_flush ();
	} else if (note_page > 0) {
		note_page--;
		/* to get any headers -- swp */
		if (note_page <= 1 && (num_headers_to_display || num_headers_to_not_display)) {
			fseek (note_fp, 0L, 0);
		} else {
			fseek (note_fp, note_mark[note_page], 0);
		}
		show_note_page (group, respnum);
	}
}

#ifndef INDEX_DAEMON
static int
expand_ctrl_chars(
	char *tobuf,
	char *frombuf,
	int length,
	int do_rotation)
{
	char *p, *q;
	int ctrl_L = FALSE;
	int i, j;

	for (p = frombuf, q = tobuf; *p && *p != '\n' && q < &tobuf[length]; p++) {
		if (*p == '\b' && q > tobuf) {
			q--;
		} else if (*p == '\t') {
			i = q - tobuf;
			j = ((i+tabwidth)/tabwidth) * tabwidth;

			while (i++ < j) {
				*q++ = ' ';
			}
		} else if (((*p) & 0xFF) < ' ') {
			*q++ = '^';
			*q++ = ((*p) & 0xFF) + '@';
			if (*p == 12) ctrl_L = TRUE;
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
#endif /* INDEX_DAEMON */

void
show_note_page (
	char *group,
	int respnum)
{
#ifndef INDEX_DAEMON

	int below_sig;			/* are we in the signature? */

	char buf3[2*HEADER_LEN+200];
	int ctrl_L = FALSE;		/* form feed character detected */
	int first  = TRUE;
	int lines;
	long tmp_pos;
	static char buf[HEADER_LEN];
	int do_display_header, dont_display_header;
	t_bool wild_do_display_headers = FALSE;
	t_bool wild_dont_display_headers = FALSE;
	int display_header, i;

	if (beginner_level) {
		lines = cLINES - (MINI_HELP_LINES - 1);
	} else {
		lines = cLINES;
	}

	ClearScreen ();

	note_line = 1;

	if (note_size == 0L) {
		tmp_pos = ftell (note_fp);
		fseek (note_fp, 0L, 2);			/* goto end of article */
		note_size = ftell (note_fp);
		fseek (note_fp, tmp_pos, 0);	/* goto old position */
	}

	if (note_page == 0) {
		buf2[0] = '\0';
		doing_pgdn = FALSE;
		show_first_header (respnum, group);
	} else {
		show_cont_header (respnum);
	}

#ifdef HAVE_METAMAIL
	if (note_page == 0 && *note_h_mimeversion && *note_h_contenttype &&
		(!STRNCMPEQ("text/plain", note_h_contenttype, 10))) {
		if (use_metamail && (!ask_for_metamail ||
			prompt_yn (cLINES, txt_use_mime, TRUE) == 1)) {
			show_mime_article (note_fp, &arts[respnum]);
			return;
		}
		show_first_header (respnum, group);
	}
#endif

	if (skip_include)
		note_page--;

	below_sig = FALSE;				/* begin of article -> not in signature */

	/* give 'em headers if they want headers -- swp */
	if (!doing_pgdn && (num_headers_to_display || num_headers_to_not_display)) {
		in_headers = 1;
		do_display_header = 1;
		dont_display_header = 0;
		if (num_headers_to_display && news_headers_to_display_array[0][0]=='*') {
			wild_do_display_headers = TRUE;
		}
		if (num_headers_to_not_display && news_headers_to_not_display_array[0][0]=='*') {
			wild_dont_display_headers = TRUE;
		}
		if (wild_do_display_headers && wild_dont_display_headers) {
			/* you're dumb */
			in_headers = 0;
		}
	} else if (!doing_pgdn && show_all_headers) {
		in_headers = 1;
	} else {
		in_headers = 0;
	}

	while (note_line < lines) {
		note_mark[note_page+1] = ftell (note_fp);
		if (show_last_line_prev_page) {
			if (doing_pgdn && first && buf2[0]) {
				goto print_a_line;
			}
		}
		first = FALSE;
		if (fgets (buf, sizeof (buf), note_fp) == 0) {
			note_end = TRUE;
			skip_include = '\0';
			break;
		}

		if (in_headers) {
			if (*buf == '\n') {
				in_headers = 0;
			} else if (!show_all_headers) {
				do_display_header = 0;
					if (!wild_do_display_headers) {
						for (i = 0; i < num_headers_to_display; i++) {
							if (!strncasecmp(buf, news_headers_to_display_array[i], strlen(news_headers_to_display_array[i]))) {
								do_display_header = 1;
								break;
							}
						}
					}
				dont_display_header = 0;
				if (!wild_dont_display_headers) {
					for (i = 0; i < num_headers_to_not_display; i++) {
						if (!strncasecmp(buf, news_headers_to_not_display_array[i], strlen(news_headers_to_not_display_array[i]))) {
							dont_display_header = 1;
							break;
						}
					}
				}
				if (wild_dont_display_headers) {
					display_header = (do_display_header);
				} else if (wild_do_display_headers) {
					display_header = (!dont_display_header);
				} else if (dont_display_header || !do_display_header) {
					display_header = 0;
				} else {
					display_header = 1;
				}
				if (!display_header) {
					continue;
				}
			}
		}

		buf[sizeof (buf)-1] = '\0';

		ctrl_L = expand_ctrl_chars(buf2, buf, sizeof (buf), rotate);

print_a_line:
		if (first) {
			StartInverse ();
		}

		if (!strcmp (buf2, "-- "))
			below_sig = TRUE;			/* begin of signature */
		strip_line (buf2);

/* decode RFC 1522(RFC 2047) style headers back to 8bit before
   further processiing. It doesn't work if header part is longer
   than a pageful. A quick patch would be remove check for
   in_headers, but that would introduce an uncessary load 
   as well as make it impossible to have header-like lines
   in article body. Somehow, in_headers is set to FALSE
   even if we're still in header part of article when
   header part is longer than a pageful. need to FIX
   There's another problem to be addressed, namely
   RFC 2047 headers spanning two or more lines should
   be concatenated, but it's not done, yet for feat that
   it may distrupt other parts.
*/

		if (in_headers && ! display_mime_header_asis) {
			/* check if it's  a continuation header line */
			if ( buf2[0] != ' ' && buf2[0] != '\t' ) {
				char header_name[80];
				size_t header_name_len;
				/* necessary, if there were only blanks in the header line, which
					are stripped by strip_line (buf2) above */
				if ( strstr(buf2, ": ") ) {
					header_name_len = strstr(buf2,": ")-buf2;
					strncpy(header_name,buf2,header_name_len);
					header_name[header_name_len]='\0';
					match_header(buf2,header_name,buf3,(char *) 0,HEADER_LEN);
					strcpy(buf2+header_name_len+2,buf3);
				}
			} else {
				strcpy(buf2,rfc1522_decode(buf2));
			}
		}
		if (tex2iso_supported && tex2iso_article) {
			strcpy (buf3, buf2);
			ConvertTeX2Iso (buf3, buf2);
		}

		if (iso2asc_supported >= 0) {
			strcpy (buf3, buf2);
			ConvertIso2Asc (buf3, buf2, iso2asc_supported);
		}

		Convert2Printable ((unsigned char*) buf2);

		first_char = buf2[0] ? buf2[0] : first_char;

		if (skip_include) {
			if (first_char != skip_include) {
				skip_include = '\0';
#ifdef HAVE_COLOR
				print_color (buf2, below_sig);
#else
				my_printf ("%s" cCRLF, buf2);
#endif
				note_line += ((int) (strlen (buf2) - 1) / cCOLS) + 1;
				note_page++;
			}
		} else {
#ifdef HAVE_COLOR
			print_color (buf2, below_sig);
#else
			my_printf ("%s" cCRLF, buf2);
#endif
			note_line += ((int) (strlen (buf2) - 1) / cCOLS) + 1;
		}

		if (first) {
			EndInverse ();
		}
		first = FALSE;
		doing_pgdn = FALSE;

		if (ctrl_L) {
			break;
		}
	}

	if (!show_last_line_prev_page) {
		note_mark[++note_page] = ftell (note_fp);
	} else {
		note_page++;
	}

	if (ftell (note_fp) == note_size) {
		note_end = TRUE;
	}

#ifdef HAVE_COLOR
	fcol(col_text);
#endif
	if (note_end) {
		MoveCursor (cLINES, MORE_POS-(5+BLANK_PAGE_COLS));
		StartInverse ();
		if (arts[respnum].thread != -1) {
			my_fputs (txt_next_resp, stdout);
		} else {
			my_fputs (txt_last_resp, stdout);
		}
		my_flush ();
		EndInverse ();
	} else {
		if (note_size > 0) {
			draw_percent_mark (note_mark[note_page], note_size);
		} else {
			MoveCursor (cLINES, MORE_POS-BLANK_PAGE_COLS);
			StartInverse ();
			my_fputs (txt_more, stdout);
			my_flush ();
			EndInverse ();
		}
	}

	show_mini_help (PAGE_LEVEL);

	stow_cursor();

#endif /* INDEX_DAEMON */
}


#if defined(HAVE_METAMAIL) && !defined(INDEX_DAEMON)
static void
show_mime_article (
	FILE	*fp,
	struct	t_article *art)
{
	char	buf[PATH_LEN];
	FILE	*mime_fp;
	long	offset;

	Raw(FALSE);
	offset = ftell (fp);
	rewind (fp);
	sprintf (buf, METAMAIL_CMD, PATH_METAMAIL);
	mime_fp = popen (buf, "w");
	while (fgets (buf, sizeof (buf), fp) != 0) {
		my_fputs (buf, mime_fp);
	}
	fflush (mime_fp);
	pclose (mime_fp);
	note_end = TRUE;
	Raw(TRUE);
	fseek (fp, offset, 0);	/* goto old position */
	MoveCursor (cLINES, MORE_POS-(5+BLANK_PAGE_COLS));
	StartInverse ();
	if (art->thread != -1) {
		my_fputs (txt_next_resp, stdout);
	} else {
		my_fputs (txt_last_resp, stdout);
	}
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

	if (!my_strftime (buf, sizeof (buf), "%a, %d %b %Y %H:%M:%S", localtime (&arts[respnum].date)))
		strcpy (buf, note_h_date);

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
	} else {
		strncat (buf, group, maxlen);
	}

	for (i = strlen(buf); i < RIGHT_POS ; i++)	/* Pad out to right */
		buf[i] = ' ';
	buf[i] = '\0';

#ifdef HAVE_COLOR
	fcol(col_head);
#endif

	/* What's this for ? */
	/* Oh, this question is easy: It's for displaying the value
	   of a X-Comment-To header in the upper right corner! */
	if (note_h_ftnto[0] && show_xcommentto && highlight_xcommentto) {
		char ftbuf[HEADER_LEN];	/* FTN-To aka X-Comment-To */

		my_fputs (buf, stdout);
		parse_from(note_h_ftnto, buf, ftbuf);
		if (strlen(ftbuf) > 19)
			ftbuf[19] = '\0';
		StartInverse ();
		my_fputs (ftbuf, stdout);
		EndInverse ();
		my_fputs (cCRLF, stdout);
	}
	else {
		char x[5];

		/* Can't eval tin_itoa() more than once in a statement due to statics */
		strcpy(x, tin_itoa(which_thread(respnum) + 1, 4));

		sprintf (tmp, txt_thread_x_of_n, buf, x, tin_itoa(top_base, 4));
		my_fputs (tmp, stdout);
	}

	if (arts[respnum].lines < 0) {
		strcpy (tmp, "?");
	} else {
		sprintf (tmp, "%-4d", arts[respnum].lines);
	}

	sprintf (buf, txt_lines, tmp);
	n = strlen (buf);
	my_fputs (buf, stdout);

#ifdef HAVE_COLOR
	fcol(col_subject);
#endif

	if (tex2iso_article) {
		*buf = '\0';
		strcpy (buf, "TeX ");
		n += strlen (buf);
		my_fputs (buf, stdout);
	}

	if (note_h_subj[0]) {
		strcpy (buf, note_h_subj);
	} else {
		strcpy (buf, arts[respnum].subject);
	}
	buf[RIGHT_POS - 5 - n] = '\0';

	pos = ((cCOLS - (int) strlen (buf)) / 2) - 2;

	if (pos > n) {
		MoveCursor (1, pos);
	} else {
		MoveCursor (1, n);
	}

	StartInverse ();
	my_fputs (buf, stdout);
	EndInverse ();

#ifdef HAVE_COLOR
	fcol(col_response);
#endif

	MoveCursor (1, RIGHT_POS);
	if (whichresp)
		my_printf (txt_resp_x_of_n, whichresp, x_resp);
	else {
		if (x_resp == 0)
			my_fputs (txt_no_resp, stdout);
		else if (x_resp == 1)
			my_fputs (txt_1_resp, stdout);
		else
			my_printf (txt_x_resp, x_resp);
	}

#ifdef HAVE_COLOR
	fcol(col_normal);
#endif

	if (*note_h_org) {
		if (arts[respnum].name) {
			sprintf (tmp, txt_s_at_s, arts[respnum].name, note_h_org);
		} else {
			strcpy (tmp, note_h_org);
		}
	} else if (arts[respnum].name) {
		strcpy (tmp, arts[respnum].name);
	} else {
		strcpy (tmp, " ");
	}

	tmp[sizeof(tmp)-1] = '\0';

	sprintf (buf, "%s  ", arts[respnum].from);

	pos = cCOLS - 1 - (int) strlen(tmp);
	if ((int) strlen (buf) + (int) strlen (tmp) >= cCOLS - 1) {
		strncat (buf, tmp, cCOLS - 1 - strlen(buf));
		buf[cCOLS-1] = '\0';
	} else {
		for (i = strlen(buf); i < pos; i++)
			buf[i] = ' ';
		buf[i] = '\0';
		strcat (buf, tmp);
	}
	strip_line (buf);

#ifdef HAVE_COLOR
	fcol(col_from);
#endif

	my_printf ("%s" cCRLF cCRLF, buf);

#ifdef HAVE_COLOR
	fcol(col_normal);
#endif

	note_line += 4;

/* with the new news_headers_to_display this is 'obsolete' */
/*
	if (note_h_keywords[0]) {
		my_printf ("Keywords: %s" cCRLF, note_h_keywords);
		note_line++;
	}

	if (note_h_summary[0]) {
		char *cp, *cp1;

		my_printf ("Summary: ");
		for (cp = note_h_summary; cp;)
		{	if ((cp1 = strchr (cp, '\n')) != 0) {
				strncpy (tmp, cp, (size_t)(cp1-cp));
				tmp[cp1-cp] = '\0';
				my_fputs (tmp, stdout);
				cp = cp1 + 1;
			} else {
				my_fputs (cp, stdout);
				cp = (char *) 0;
			}
			my_fputs (cCRLF, stdout);
			note_line++;
		}
	}
*/

/* FIXME */
	if (note_h_ftnto[0] && show_xcommentto && !highlight_xcommentto) {
		my_printf ("X-Comment-To: %s" cCRLF, note_h_ftnto);
		note_line++;
	}

	if (/*note_h_keywords[0] || note_h_summary[0] ||*/ (note_h_ftnto[0] && show_xcommentto && !highlight_xcommentto)) {
		my_printf (cCRLF);
		note_line++;
	}
}
#endif /* INDEX_DAEMON */


#ifndef INDEX_DAEMON
static void
show_cont_header (
	int respnum)
{
	int maxresp;
	int whichresp;
	int whichbase;
	char buf[LEN];

	whichresp = which_response (respnum);
	whichbase = which_thread (respnum);
	maxresp = num_of_responses (whichbase);

	assert (whichbase < top_base);

	if (whichresp) {
		sprintf(buf, txt_thread_resp_page,
			whichbase + 1,
			top_base,
			whichresp,
			maxresp,
			note_page + 1,
			arts[respnum].name?arts[respnum].name:arts[respnum].from,note_h_subj);
	} else {
		sprintf(buf, txt_thread_page,
			whichbase + 1,
			top_base,
			note_page + 1,
			arts[respnum].name?arts[respnum].name:arts[respnum].from,note_h_subj);
	}
	strip_line (buf);
	if (cCOLS) {
		buf[cCOLS-1] = '\0';
	}

#ifdef HAVE_COLOR
	fcol(col_head);
#endif

	my_printf("%s" cCRLF cCRLF, buf);

#ifdef HAVE_COLOR
	fcol(col_normal);
#endif

	note_line += 2;
}
#endif /* INDEX_DAEMON */


int
art_open (
	long art,
	char *group_path)
{
	char buf[8192];
	char *ptr;
	int c;
	int is_summary;

	note_page = 0;

	art_close ();	/* just in case */

	if (tex2iso_supported) {
		tex2iso_article = iIsArtTexEncoded (art, group_path);
		if (tex2iso_article) {
			wait_message ("TeX2Iso encoded article");
		}
	} else {
		tex2iso_article = FALSE;
	}

	if ((note_fp = open_art_fp (group_path, art)) == (FILE *) 0) {
		return (ART_UNAVAILABLE);
	}

	note_h_from[0] = '\0';
	note_h_path[0] = '\0';
	note_h_subj[0] = '\0';
	note_h_org[0] = '\0';
	note_h_date[0] = '\0';
	note_h_newsgroups[0] = '\0';
	note_h_messageid[0] = '\0';
	note_h_references[0] = '\0';
	note_h_distrib[0] = '\0';
	note_h_followup[0] = '\0';
	note_h_keywords[0] = '\0';
	note_h_summary[0] = '\0';
	note_h_mimeversion[0] = '\0';
	note_h_contenttype[0] = '\0';
	note_h_contentenc[0] = '\0';
	note_h_ftnto[0] = '\0';

	while (fgets(buf, sizeof buf, note_fp) != 0) {
		buf[sizeof(buf)-1] = '\0';

		if (*buf == '\n')
			break;

		/* do not remove \n in Summary lines */
/* FIXME */
		is_summary = (strncasecmp (buf, "Summary: ", 9) == 0);

		/* check for continued header line */
		while((c=peek_char(note_fp))!=EOF && isspace(c) && c!='\n'
		      && strlen(buf)<sizeof(buf)-1) {
			if (strlen(buf)>0 && buf[strlen(buf)-1]=='\n') {
				if (!is_summary) {
				  buf[strlen(buf)-1]='\0';
				}
			}
		fgets(buf+strlen(buf), sizeof buf-strlen(buf), note_fp);
		}

		for (ptr = buf ; *ptr && ((*ptr != '\n') || (ptr[1] != '\0')); ptr++) {
			if ((((*ptr) & 0xFF) < ' ') 
				&& (*ptr != '\n') 
				&& ((*ptr != '\t') || (!is_summary)))
				*ptr = ' ';
		}
		*ptr = '\0';

  		if (match_header (buf, "Path", note_h_path, (char*)0, HEADER_LEN))
  			continue;
		if (match_header (buf, "From", note_h_from, (char*)0, HEADER_LEN))
			continue;
  		if (match_header (buf, "Subject", note_h_subj, (char*)0, HEADER_LEN))
  			continue;
  		if (match_header (buf, "Organization", note_h_org, (char*)0, HEADER_LEN))
  			continue;
  		if (match_header (buf, "Date", note_h_date, (char*)0, HEADER_LEN))
  			continue;
  		if (match_header (buf, "Newsgroups", note_h_newsgroups, (char*)0, HEADER_LEN))
  			continue;
  		if (match_header (buf, "Message-ID", note_h_messageid, (char*)0, HEADER_LEN))
  			continue;
  		if (match_header (buf, "References", note_h_references, (char*)0, HEADER_LEN))
  			continue;
  		if (match_header (buf, "Distribution", note_h_distrib, (char*)0, HEADER_LEN))
  			continue;
  		if (match_header (buf, "Followup-To", note_h_followup, (char*)0, HEADER_LEN))
  			continue;
  		if (match_header (buf, "Keywords", note_h_keywords, (char*)0, HEADER_LEN))
  			continue;
  		if (match_header (buf, "Summary", note_h_summary, (char*)0, HEADER_LEN))
  			continue;
		if (match_header (buf, "Mime-Version", note_h_mimeversion, (char*)0, HEADER_LEN))
			continue;
		if (match_header (buf, "Content-Type", note_h_contenttype, (char*)0, HEADER_LEN)) {
			str_lwr (note_h_contenttype, note_h_contenttype);
			continue;
		}
		if (match_header (buf, "Content-Transfer-Encoding", note_h_contentenc, (char*)0, HEADER_LEN)) {
			str_lwr (note_h_contentenc, note_h_contentenc);
			continue;
		}
		if (match_header (buf, "X-Comment-To", note_h_ftnto, (char*)0, HEADER_LEN))
			continue;
	}

	note_mark[0] = ftell (note_fp);
	note_end = FALSE;

	/*
	 * If Newsgroups is empty its a good bet the article is a mail article
	 */
	if (!note_h_newsgroups[0]) {
		strcpy (note_h_newsgroups, group_path);
		while ((ptr = strchr (note_h_newsgroups, '/'))) {
			*ptr = '.';
		}
	}

	return (0);
}


void
art_close (void)
{
	if (note_fp && note_page != ART_UNAVAILABLE) {
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

	return choose_response (which_thread (respnum), num);
}
#endif /* INDEX_DAEMON */


void
yank_to_addr (
	char *orig,
	char *addr)
{
	char *p;
	int open_parens;

	for (p = orig; *p; p++)
		if (((*p) & 0xFF) < ' ')
			*p = ' ';

	while (*addr)
		addr++;

	while (*orig) {
		while (*orig && (*orig == ' ' /* || *orig == '"' */ || *orig == ','))
			orig++;
		*addr++ = ' ';
		while (*orig && (*orig != ' ' && *orig != ',' /* && *orig != '"' */ ))
			*addr++ = *orig++;
		while (*orig && (*orig == ' ' /* || *orig == '"' */ || *orig == ','))
			orig++;
		if (*orig == '(') {
			orig++;
			open_parens = 1;
			while (*orig && open_parens) {
				if (*orig == '(')
					open_parens++;
				if (*orig == ')')
					open_parens--;
				orig++;
			}
			if (*orig == ')')
				orig++;
		}
	}
	*addr = '\0';
}


#ifndef INDEX_DAEMON
static int
show_last_page (void)
{
	char buf[LEN];
	char buf3[LEN+50];
	int ctrl_L;		/* form feed character detected */
	long tmp_pos;

	if (note_end) {
		return FALSE;
	}

	if (note_size == 0L) {
		tmp_pos = ftell (note_fp);
		fseek (note_fp, 0L, 2);			/* goto end of article */
		note_size = ftell (note_fp);
		fseek (note_fp, tmp_pos, 0);	/* goto old position */
	}

	while (!note_end) {
		note_line = 1;

		if (note_page == 0) {
			note_line += 4;
		} else {
			note_line += 2;
		}
		while (note_line < cLINES) {
			if (fgets (buf, sizeof buf, note_fp) == 0) {
				note_end = TRUE;
				break;
			}
			buf[sizeof (buf)-1] = '\0';

			ctrl_L = expand_ctrl_chars (buf3, buf, sizeof (buf), 0);

			note_line += ((int) strlen (buf3) / cCOLS) + 1;

			if (ctrl_L) {
				break;
			}
		}
		if (note_mark[note_page] == note_size) {
			note_end = TRUE;
			note_page--;
			break;
		} else if (!note_end) {
			note_mark[++note_page] = ftell(note_fp);
		}
	}
	fseek (note_fp, note_mark[note_page], 0);
	return TRUE;
}
#endif /* INDEX_DAEMON */


/*
 * buf:  Article header
 * pat:  Text to match in header
 * body: Return buffer
 * len:  sizeof(body)
 *
 * Returns:
 *	FALSE	Header was not matched.
 *	TRUE	Header was matched. body contains NULL terminated content
 * 			portion of buf (ie with pat: and leading space removed)
 */
int
match_header (
	char *buf,
	const char *pat,
	char *body,
	char *nodec_body,
	size_t len)
{
	size_t	plen = strlen (pat);

	/* A quick check on the length before calling strnicmp() etc. */

	/*
	 * Does ': ' follow the header text ?
	 */
	if (buf[plen] != ':' || buf[plen+1] != ' ') {
		return FALSE;
	}

	/*
	 * If the header matches, skip the ': ' and any leading whitespace
	 */
	if (strncasecmp(buf, pat, plen) == 0) {
		plen += 2;

		while (buf[plen] == ' ') {
			plen++;
		}

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
#endif

		modifiedstrncpy (body, &buf[plen], len, TRUE);
		body[len - 1] = '\0';

		if(nodec_body) {
			modifiedstrncpy (nodec_body, &buf[plen], len, FALSE);
			nodec_body[len - 1] = '\0';
		}

		return TRUE;
	}

	return FALSE;
}
