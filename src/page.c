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

struct t_header note_h;

char buf2[HEADER_LEN+50];

char *glob_page_group;

char first_char;
char skip_include;

FILE *note_fp;					/* the body of the current article */

int glob_respnum;
int last_resp;					/* current & previous article for - command */
int note_line;
int note_page;					/* what page we're on */
int rotate;						/* 0=normal, 13=rot13 decode */
int this_resp;
int tabwidth = 8;

long note_mark[MAX_PAGES];	/* holds ftell() on beginning of each page */
long mark_body;				/* holds ftell() on beginning of message body */
long note_size;				/* stat() size in bytes of article */

t_bool doing_pgdn;
t_bool note_end;				/* we're done showing this article */
t_bool show_all_headers = FALSE;	/* CTRL-H with headers specified */
t_bool show_prev_header = FALSE;	/* remember display status of last line */

static int tex2iso_article;

/*
 * Local prototypes
 */
#ifndef INDEX_DAEMON
	static void show_first_header (int respnum, char *group);
	static void show_cont_header (int respnum);
	static int prompt_response (int ch, int respnum);
	static int show_last_page (void);
	static t_bool expand_ctrl_chars (char *tobuf, char *frombuf, int length, int do_rotation);
#	ifdef HAVE_METAMAIL
		static void show_mime_article (FILE *fp, struct t_article *art);
#	endif /* HAVE_METAMAIL */
#endif /* !INDEX_DAEMON */

#ifndef INDEX_DAEMON
int
show_page (
	struct t_group *group,
	char *group_path,
	int respnum,			/* index into arts[] */
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
	struct stat note_stat_blubb;

	local_filtered_articles = FALSE;	/* used in thread level */

restart:
	if (read_news_via_nntp)
		wait_message (0, txt_reading_article);

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

	switch (art_open (&arts[respnum], group_path)) {

		case ART_UNAVAILABLE:
			art_mark_read (group, &arts[respnum]);
			info_message (txt_art_unavailable);
			/* FALLTHROUGH */

		case ART_ABORT:
			return GRP_NOREDRAW;	/* special retcode to stop redrawing screen */

		default:					/* Normal case */
			break;
	}

	art_mark_read (group, &arts[respnum]);

	/* Get article size */
	fstat(fileno(note_fp), &note_stat_blubb);
	note_size = note_stat_blubb.st_size;
	show_note_page (group->name, respnum);

	forever {
		ch = ReadCh ();

		if (ch >= '0' && ch <= '9') {
			n = which_thread (respnum);
			if (!num_of_responses (n))
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
#endif
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
#endif

			case iKeyLastPage:	/* goto end of article */
			case iKeyPageLastPage2:
end_of_article:
				if (show_last_page ())
					show_note_page (group->name, respnum);

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

				if (prompt_string(txt_enter_message_id, buf+1, HIST_MESSAGE_ID) && buf[1]) {
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
					if (msgid->article == ART_UNAVAILABLE) {
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

			case iKeySearchSubjF:	/* search forwards in article */
				if (search_article (TRUE))
					show_note_page (group->name, respnum);
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
				if (!space_goto_next_unread) {
					if (note_page != ART_UNAVAILABLE) {
						if (note_end) {
							if (pgdn_goto_next)
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
				if ((n = next_unread (next_response (respnum))) == -1)
					return (which_thread (respnum));
				respnum = n;
				goto restart;

#ifdef HAVE_PGP
			case iKeyPagePGPCheckArticle:
				if (pgp_check_article())
					redraw_page(group->name, respnum);
				break;
#endif

			case iKeyPageToggleHeaders:	/* toggle display of article headers */
				if (note_page == ART_UNAVAILABLE) {
					if ((n  = next_response (respnum)) == -1)
						return (which_thread (respnum));
					respnum = n;
					goto restart;
				} else {
					note_page = 0;
					note_end = FALSE;
					fseek(note_fp, 0L, SEEK_SET);
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
				if (tabwidth == 8)
					tabwidth = 4;
				else
					tabwidth = 8;
				redraw_page (group->name, respnum);
				break;

			case iKeyThreadQuickAutosel:		/* quickly auto-select article */
				local_filtered_articles = quick_filter_select (group, &arts[respnum]);
				if (local_filtered_articles)
					goto return_to_index;

				redraw_page (group->name, respnum);
				break;

			case iKeyThreadQuickKill:		/* quickly kill article */
				local_filtered_articles = quick_filter_kill (group, &arts[respnum]);
				if (local_filtered_articles)
					goto return_to_index;

				redraw_page (group->name, respnum);
				break;

			case iKeyPageAutoSel:		/* auto-select article menu */
				if (filter_menu (FILTER_SELECT, group, &arts[respnum])) {
					local_filtered_articles = filter_articles (group);
					if (local_filtered_articles)
						goto return_to_index;
				}
				redraw_page (group->name, respnum);
				break;

			case iKeyPageAutoKill:		/* kill article menu */
				if (filter_menu (FILTER_KILL, group, &arts[respnum])) {
					local_filtered_articles = filter_articles (group);
					if (local_filtered_articles)
						goto return_to_index;
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
					my_printf (txt_art_unavailable/*, arts[respnum].artnum*/);
					my_flush ();
				} else {
					note_page = 0;
					note_end = FALSE;
					fseek (note_fp, 0L, SEEK_SET);
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
				if ((n = search_author (my_group[cur_groupnum], respnum, i)) < 0)
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
						return (which_response (respnum));
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

			case iKeyPageCatchup:	/* catchup - mark all articles as read */
			case iKeyPageCatchupGotoNext:	/* and goto next group */
				if (!confirm_action || prompt_yn (cLINES, txt_mark_all_read, TRUE) == 1) {
					grp_mark_read (group, arts);
					ret_code = (ch == iKeyPageCatchupGotoNext ? GRP_CONTINUE : GRP_UNINDEXED);
					if (!(cur_groupnum + 1 < group_top))
						ret_code = GRP_UNINDEXED;

					art_close ();
					space_mode = TRUE;
					return ret_code;
				}
				break;

			case iKeyPageCancel:	/* delete an article */
				if (can_post) {
					if (cancel_article (group, &arts[respnum], respnum))
						redraw_page (group->name, respnum);
				} else
					info_message (txt_cannot_post);
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
				if (toggle_color ()) {
					redraw_page (group->name, respnum);
					show_color_status ();
				}
				break;
#endif

			case iKeyPageKillArt:
				if (note_page == ART_UNAVAILABLE) {
					if ((n = next_unread (next_response(respnum))) == -1)
						return (which_thread (respnum));
				} else {
					art_close ();
					if ((n = next_unread (next_response(respnum))) == -1)
						return (which_thread (respnum));
				}
				respnum = n;
				goto restart;
				/* NOTREACHED */

			case iKeyPageKillThd:	/* mark rest of thread as read */
				thd_mark_read (group, respnum);
				if ((n = next_unread (next_response (respnum))) == -1)
					goto return_to_index;
				art_close ();
				respnum = n;
				goto restart;
				/* NOTREACHED */

			case iKeyPageMail:	/* mail article/thread/tagged articles to somebody */
				feed_articles (FEED_MAIL, PAGE_LEVEL, group, respnum);
				break;

			case iKeyOptionMenu:	/* option menu */
				if (change_config_file (group) == FILTERING)
					filter_state = FILTERING;

				set_signals_page ();		/* Just to be sure */
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

			case iKeyPageNextUnreadArt:	/* next unread article */
				if ((n = next_unread (next_response (respnum))) == -1)
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
				if ((n = prev_response (respnum)) == -1)
					return (which_response (respnum));
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
				if (post_article (group->name, &posted_flag))
					redraw_page (group->name, respnum);
				break;

			case iKeyPostponed:	/* post postponed article */
				if (can_post) {
					if (pickup_postponed_articles (FALSE, FALSE))
						redraw_page (group->name, respnum);
				} else
					info_message(txt_cannot_post);
				break;

			case iKeyPagePostHist:	/* display messages posted by user */
				if (user_posted_messages ())
					redraw_page (group->name, respnum);
				break;

			case iKeyPageRepost:	/* repost current article */
				feed_articles (FEED_REPOST, PAGE_LEVEL, group, respnum);
				break;

			case iKeyPageMarkArtUnread:	/* mark article as unread (to return) */
				art_mark_will_return (group, &arts[respnum]);
				info_message (txt_marked_as_unread, "Article");
				break;

			case iKeyPageSkipIncludedText:	/* skip included text */
				skip_include = first_char;
				goto page_down;

			case iKeyPageDisplaySubject:
				clear_message();
				center_line (cLINES, FALSE, arts[respnum].subject);
				break;

#ifdef HAVE_COLOR
			case iKeyPageToggleHighlight:
				if(use_color) { /* make sure we have color turned on */
					word_highlight = !word_highlight;
					redraw_page(group->name, respnum);
					info_message(txt_toggled_high, (word_highlight) ? "on" : "off");
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
		my_printf (txt_art_unavailable/*, arts[respnum].artnum*/);
		my_flush ();
	} else if (note_page > 0) {
		note_page--;
		fseek (note_fp, note_mark[note_page], SEEK_SET);
		show_note_page (group, respnum);
	}
}

#ifndef INDEX_DAEMON
static t_bool
expand_ctrl_chars(
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

	char buf3[2*HEADER_LEN+200];
	int do_display_header;
	int lines;
	int i;
	static char buf[HEADER_LEN];
	t_bool below_sig;				/* are we in the signature? */
	t_bool ctrl_L = FALSE;		/* form feed character detected */
	t_bool first  = TRUE;

	if (beginner_level)
		lines = cLINES - (MINI_HELP_LINES - 1);
	else
		lines = cLINES;

	ClearScreen ();

	note_line = 1;

	if (note_page == 0) {
		buf2[0] = '\0';
		doing_pgdn = FALSE;
		show_first_header (respnum, group);
	} else
		show_cont_header (respnum);

#ifdef HAVE_METAMAIL
	if (note_page == 0 && *note_h.mimeversion && *note_h.contenttype 
		 && (!STRNCMPEQ("text/plain", note_h.contenttype, 10))
		 && use_metamail) {
		if (!ask_for_metamail || prompt_yn (cLINES, txt_use_mime, TRUE) == 1) {
			show_mime_article (note_fp, &arts[respnum]);
			return;
		}
		show_first_header (respnum, group);
	}
#endif

	if (skip_include)
		note_page--;

	below_sig = FALSE;				/* begin of article -> not in signature */

	while (note_line < lines) { /* loop show_note_page */

		if (show_last_line_prev_page) {
		   note_mark[note_page+1] = ftell (note_fp);
			if (doing_pgdn && first && buf2[0])
				goto print_a_line;
		}
		first = FALSE;
		if (fgets (buf, sizeof (buf), note_fp) == 0) {
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
				do_display_header = 0; /* default: don't display header */
				if (num_headers_to_display 
					 && (news_headers_to_display_array[0][0] == '*')) {
					do_display_header = 1; /* wild do */
				} else {
					for (i = 0; i < num_headers_to_display; i++) {
						if (!strncasecmp (buf, news_headers_to_display_array[i],
											strlen (news_headers_to_display_array[i]))) {
							do_display_header = 1;
							break;
						}
					}
				}
				if (num_headers_to_not_display
					 && (news_headers_to_not_display_array[0][0] == '*')) {
					do_display_header = 0; /* wild don't: doesn't make sense! */
				} else {
					for (i = 0; i < num_headers_to_not_display; i++) {
						if (!strncasecmp (buf, news_headers_to_not_display_array[i],
									  strlen (news_headers_to_not_display_array[i]))) {
							do_display_header = 0;
							break;
						}
					}
				}
				
				/* do_display_header is set iff line should be displayed */
				show_prev_header = do_display_header;	/* remember for cont. */
				if (!do_display_header)
					continue;
			}  /* endif continuation line */
		} /* endif in_headers && !show_all_headers */

		buf[sizeof (buf)-1] = '\0';

		ctrl_L = expand_ctrl_chars(buf2, buf, sizeof (buf), rotate);

print_a_line:
		if (first)
			StartInverse ();

		if (!strcmp (buf2, "-- "))
			below_sig = TRUE;			/* begin of signature */

		strip_line (buf2);

		/* RFC 2047 headers spanning two or more lines should be
		 * concatenated, but it's not done, yet for feat that it may
		 * distrupt other parts.
		 */

		if (in_headers 
			 && ( (!display_mime_header_asis && !show_all_headers) 
					|| (!display_mime_allheader_asis && show_all_headers) ) )  {
			/* check if it's a continuation header line */
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
			} else
				strcpy(buf2,rfc1522_decode(buf2));
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

		if (!below_sig || show_signatures) {
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
		}

		if (first)
			EndInverse ();

		first = FALSE;
		doing_pgdn = FALSE;

		if (ctrl_L)
			break;

	} /* loop show_note_page */

	if (!show_last_line_prev_page)
		note_mark[++note_page] = ftell (note_fp);
	else
		note_page++;

	if (ftell (note_fp) == note_size)
		note_end = TRUE;

#ifdef HAVE_COLOR
	fcol(col_text);
#endif
	if (note_end) {
		MoveCursor (cLINES, MORE_POS-(5+BLANK_PAGE_COLS));
		StartInverse ();

		if (arts[respnum].thread != -1)
			my_fputs (txt_next_resp, stdout);
		else
			my_fputs (txt_last_resp, stdout);

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

	while (fgets (buf, sizeof (buf), fp) != 0)
		my_fputs (buf, mime_fp);

	fflush (mime_fp);
	pclose (mime_fp);
	note_end = TRUE;
	Raw(TRUE);
	fseek (fp, offset, SEEK_SET);	/* goto old position */
	MoveCursor (cLINES, MORE_POS-(5+BLANK_PAGE_COLS));
	StartInverse ();

	if (art->thread != -1)
		my_fputs (txt_next_resp, stdout);
	else
		my_fputs (txt_last_resp, stdout);

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

	for (i = strlen(buf); i < RIGHT_POS ; i++)	/* Pad out to right */
		buf[i] = ' ';

	buf[i] = '\0';

#ifdef HAVE_COLOR
	fcol(col_head);
#endif

	/* Displaying the value of X-Comment-To header in the upper right corner */ 
	if (note_h.ftnto[0] && show_xcommentto) {
		char ftbuf[HEADER_LEN]; /* FTN-To aka X-Comment-To */

		my_fputs (buf, stdout);
		parse_from(note_h.ftnto, buf, ftbuf);
		if (*ftbuf == '\0')
			strncpy (ftbuf, buf, 19);
		ftbuf[19] = '\0';
		Convert2Printable (ftbuf);
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

	if (arts[respnum].lines < 0)
		strcpy (tmp, "?");
	else
		sprintf (tmp, "%-4d", arts[respnum].lines);

#ifdef HAVE_COLOR
	fcol(col_head);
#endif

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

	if (note_h.subj[0])
		strcpy (buf, note_h.subj);
	else
		strcpy (buf, arts[respnum].subject);

	buf[RIGHT_POS - 5 - n] = '\0';

	pos = ((cCOLS - (int) strlen (buf)) / 2) - 2;

	if (pos > n)
		MoveCursor (1, pos);
	else
		MoveCursor (1, n);

	Convert2Printable (buf);

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

#ifdef HAVE_COLOR
	fcol(col_from);
#endif

	my_printf ("%s" cCRLF cCRLF, buf);

#ifdef HAVE_COLOR
	fcol(col_normal);
#endif

	note_line += 4;
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
			arts[respnum].name?arts[respnum].name:arts[respnum].from,note_h.subj);
	} else {
		sprintf(buf, txt_thread_page,
			whichbase + 1,
			top_base,
			note_page + 1,
			arts[respnum].name?arts[respnum].name:arts[respnum].from,note_h.subj);
	}
	strip_line (buf);

	if (cCOLS)
		buf[cCOLS-1] = '\0';

	Convert2Printable (buf);

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

/*
 * Returns:
 *		0						Art opened successfully
 *		ART_UNAVAILABLE	Couldn't find article
 *		ART_ABORT			User aborted during read of article
 */
int
art_open (
	struct t_article *art,
	char *group_path)
{
	char buf[8192];
	char *ptr;
	int c;
	int is_summary;

	art_close ();	/* just in case */

	if (tex2iso_supported) {
		tex2iso_article = iIsArtTexEncoded (art->artnum, group_path);

		if (tex2iso_article)
			wait_message (0, txt_is_tex_ecoded);

	} else
		tex2iso_article = FALSE;

	if ((note_fp = open_art_fp (group_path, art->artnum, art->lines))
		 == (FILE *) 0) {
		note_page = ART_UNAVAILABLE;		/* Flag error for later */

		if (tin_errno == 0)
			return (ART_UNAVAILABLE);
		else
			return (ART_ABORT);
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
				if (!is_summary)
					buf[strlen(buf)-1]='\0';
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
			str_lwr (note_h.contenttype, note_h.contenttype);
			continue;
		}
		if (match_header (buf, "Content-Transfer-Encoding", note_h.contentenc, (char*)0, HEADER_LEN)) {
			str_lwr (note_h.contentenc, note_h.contentenc);
			continue;
		}
		if (match_header (buf, "X-Comment-To", note_h.ftnto, (char*)0, HEADER_LEN))
			continue;
	}

	mark_body = ftell (note_fp);
	note_mark[0] = 0L;
	fseek (note_fp, 0L, SEEK_SET);
	note_end = FALSE;

	/*
	 * If Newsgroups is empty its a good bet the article is a mail article
	 */
	if (!note_h.newsgroups[0]) {
		strcpy (note_h.newsgroups, group_path);
		while ((ptr = strchr (note_h.newsgroups, '/')))
			*ptr = '.';
	}

	/* This is used as some warped success indicator in art_close() */
	note_page = 0;	

	return (0);
}


void
art_close (void)
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
	t_bool ctrl_L;		/* form feed character detected */

	if (note_end) {
		return FALSE;
	}

	while (!note_end) {
		note_line = 1;

		if (note_page == 0)
			note_line += 4;
		else
			note_line += 2;

		while (note_line < cLINES) {
			if (fgets (buf, sizeof buf, note_fp) == 0) {
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
		} else if (!note_end) {
			note_mark[++note_page] = ftell(note_fp);
		}
	}
	fseek (note_fp, note_mark[note_page], SEEK_SET);
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
	size_t plen = strlen (pat);

	/* A quick check on the length before calling strnicmp() etc. */

	/*
	 * Does ': ' follow the header text ?
	 */
	if (buf[plen] != ':' || buf[plen+1] != ' ')
		return FALSE;

	/*
	 * If the header matches, skip the ': ' and any leading whitespace
	 */
	if (strncasecmp(buf, pat, plen) == 0) {
		plen += 2;

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
