/*
 *  Project   : tin - a Usenet reader
 *  Module    : help.c
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 22-12-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

/* TODO -- use txt_help_empty_line to organize listed commands, possibly
 * like this:
 *
 * navigation within the level
 * navigation to other levels
 * operations on threads and/or articles (tagging, printing, saving, etc.)
 * universal features (quit tin, display information, etc.)
 */

#include	"tin.h"
#include	"tcurses.h"
#include	"menukeys.h"

static constext txt_help_empty_line[] = cCRLF;
static const char *info_title;
static const char **info_help;
static int cur_page;
static int group_len = 0;
static int info_type;
static int max_page;
static int pos_help;

static int ReadHelpCh (void);

constext *help_select[] = {
	txt_help_ctrl_d,
	txt_help_ctrl_f,
	txt_help_b,
	txt_help_j,
		txt_help_empty_line,
	txt_help_g,
	txt_help_g_cr,
	txt_help_g_tab,
	txt_help_g_num,
	txt_help_g_caret_dollar,
	txt_help_g_search,
	txt_help_n,
		txt_help_empty_line,
	txt_help_w,
		txt_help_empty_line,
	txt_help_sel_c,
	txt_help_g_z,
	txt_help_g_ctrl_r,
		txt_help_empty_line,
	txt_help_g_y,
	txt_help_y,
	txt_help_g_r,
		txt_help_empty_line,
	txt_help_g_hash,
	txt_help_s,
	txt_help_S,
	txt_help_m,
		txt_help_empty_line,
	txt_help_W,
		txt_help_empty_line,
	txt_help_g_q,
	txt_help_g_x,
#ifndef NO_SHELL_ESCAPE
	txt_help_shell,
#endif
	txt_help_esc,
	txt_help_ctrl_l,
		txt_help_empty_line,
	txt_help_h,
	txt_help_M,
		txt_help_empty_line,
	txt_help_s_i,
	txt_help_g_d,
	txt_help_I,
#ifdef HAVE_COLOR
	txt_help_color,
#endif
		txt_help_empty_line,
	txt_help_v,
	txt_help_bug_report,
	0
};

constext *help_group[] = {
	txt_help_ctrl_d,
	txt_help_ctrl_f,
	txt_help_b,
	txt_help_j,
		txt_help_empty_line,
	txt_help_i_cr,
	txt_help_i_tab,
	txt_help_i_n,
	txt_help_i_p,
	txt_help_dash,
	txt_help_l,
		txt_help_empty_line,
	txt_help_i_caret_dollar,
	txt_help_i_num,
	txt_help_i_search,
	txt_help_a,
	txt_help_B,
		txt_help_empty_line,
	txt_help_w,
	txt_help_x,
		txt_help_empty_line,
	txt_help_hash,
		txt_help_empty_line,
	txt_help_p_m,
	txt_help_p_s,
	txt_help_p_S,
#ifndef DONT_HAVE_PIPING
	txt_help_pipe,
#endif
	txt_help_o,
	txt_help_t,
		txt_help_empty_line,
	txt_help_p_z,
	txt_help_K,
	txt_help_c,
	txt_help_cC,
		txt_help_empty_line,
	txt_help_plus,
	txt_help_i_star,
	txt_help_equal,
	txt_help_semicolon,
	txt_help_i_dot,
	txt_help_i_coma,
	txt_help_i_tilda,
		txt_help_empty_line,
	txt_help_U,
		txt_help_empty_line,
	txt_help_u,
	txt_help_r,
	txt_help_X,
	txt_help_W,
		txt_help_empty_line,
	txt_help_i,
	txt_help_q,
	txt_help_esc,
	txt_help_g,
	txt_help_h,
	txt_help_ctrl_l,
#ifndef NO_SHELL_ESCAPE
	txt_help_shell,
#endif
		txt_help_empty_line,
	txt_help_ctrl_a,
	txt_help_ctrl_k,
	txt_help_quick_select,
	txt_help_quick_kill,
		txt_help_empty_line,
	txt_help_M,
	txt_help_d,
	txt_help_I,
#ifdef HAVE_COLOR
	txt_help__,
#endif
		txt_help_empty_line,
	txt_help_g_t_p_i,
		txt_help_empty_line,
	txt_help_v,
	txt_help_bug_report,
	0
};

constext *help_thread[] = {
	txt_help_ctrl_d,
	txt_help_ctrl_f,
	txt_help_b,
	txt_help_j,
		txt_help_empty_line,
	txt_help_t_caret_dollar,
	txt_help_t_num,
	txt_help_i_search,
	txt_help_t_cr,
	txt_help_p_tab,
		txt_help_empty_line,
	txt_help_hash,
		txt_help_empty_line,
	txt_help_p_z,
	txt_help_ck,
	txt_help_t,
	txt_help_U,
		txt_help_empty_line,
	txt_help_p_s,
	txt_help_p_S,
		txt_help_empty_line,
	txt_help_i,
	txt_help_q,
	txt_help_esc,
	txt_help_h,
	txt_help_ctrl_l,
		txt_help_empty_line,
	txt_help_d,
	txt_help_I,
		txt_help_empty_line,
	txt_help_v,
	txt_help_bug_report,
	0
};

constext *help_page[] = {
	txt_help_ctrl_d,
	txt_help_ctrl_f,
	txt_help_b,
	txt_help_p_caret_dollar,
	txt_help_p_g,
		txt_help_empty_line,
	txt_help_p_num,
	txt_help_p_cr,
	txt_help_p_tab,
	txt_help_p_n,
	txt_help_p_p,
	txt_help_thread,
	txt_help_dash,
	txt_help_p_u,
	txt_help_L,
		txt_help_empty_line,
	txt_help_a,
	txt_help_B,
	txt_help_p_search,
		txt_help_empty_line,
	txt_help_w,
	txt_help_p_f,
	txt_help_p_r,
	txt_help_x,
	txt_help_e,
	txt_help_p_ctrl_w,
	txt_help_p_ctrl_e,
		txt_help_empty_line,
	txt_help_p_m,
	txt_help_p_s,
#ifndef DONT_HAVE_PIPING
	txt_help_pipe,
#endif
	txt_help_o,
	txt_help_t,
		txt_help_empty_line,
	txt_help_ctrl_a,
	txt_help_ctrl_k,
	txt_help_quick_select,
	txt_help_quick_kill,
	txt_help_D,
		txt_help_empty_line,
	txt_help_p_z,
	txt_help_p_k,
	txt_help_c,
	txt_help_cC,
		txt_help_empty_line,
	txt_help_ctrl_h,
	txt_help_p_d,
#ifdef HAVE_PGP
	txt_help_ctrl_g,
#endif
		txt_help_empty_line,
	txt_help_W,
		txt_help_empty_line,
	txt_help_T,
	txt_help_i,
	txt_help_q,
	txt_help_esc,
	txt_help_h,
	txt_help_ctrl_l,
#ifndef NO_SHELL_ESCAPE
	txt_help_shell,
#endif
		txt_help_empty_line,
	txt_help_M,
	txt_help_I,
#ifdef HAVE_COLOR
	txt_help__,
#endif
		txt_help_empty_line,
	txt_help_p_star,
	txt_help_p_dot,
	txt_help_p_coma,
	txt_help_p_tilda,
	txt_help_tex,
		txt_help_empty_line,
	txt_help_v,
		txt_help_empty_line,
	txt_help_colon,
		txt_help_empty_line,
	txt_help_g_t_p_i,
	0
};


static int
	ReadHelpCh (void)
{
	int ch = ReadCh ();

	switch (ch) {
		case ESC:	/* common arrow keys */
#ifdef HAVE_KEY_PREFIX
		case KEY_PREFIX:
#endif
			switch (get_arrow_key (ch)) {
				case KEYMAP_LEFT:
					break;

				case KEYMAP_UP:
#if USE_CURSES
					ch = iKeyUp;
					break;
#endif

				case KEYMAP_PAGE_UP:
					ch = iKeyPageUp;
					break;

				case KEYMAP_DOWN:
#if USE_CURSES
					ch = iKeyDown;
					break;
#endif

				case KEYMAP_RIGHT:
				case KEYMAP_PAGE_DOWN:
					ch = iKeyPageDown;
					break;

				case KEYMAP_HOME:
					ch = iKeyFirstPage;
					break;

				case KEYMAP_END:
					ch = iKeyLastPage;
					break;

				default:
					break;
			}
			break;
		default:
			break;
	}
	return ch;
}

void
show_info_page (
	int type,
	const char *help[],
	const char *title)
{
	int max_line, len;
	int help_lines;
	int old_page = 0;

	if (NOTESLINES <= 0)
		return;

	if (beginner_level)
		help_lines = NOTESLINES + MINI_HELP_LINES - 1;
	else
		help_lines = NOTESLINES;

	set_signals_help ();

	cur_page = 1;
	max_page = 1;
	pos_help = 0;

	info_help = help;
	info_type = type;
	info_title = title;

	/*
	 *  find how many elements in array
	 */
	if (type == HELP_INFO) {
		for (max_line=0 ; help[max_line] ; max_line++)
			continue;
	} else {
		for (max_line=0 ; posted[max_line].date[0] ; max_line++) {
			len = strlen (posted[max_line].group);
			if (len > group_len)
				group_len = len;
		}
	}

	max_page = max_line / help_lines;
	if (max_line % help_lines)
		max_page++;

	set_xclick_off ();
#if USE_CURSES
	ClearScreen();
#endif
	forever {
		if (cur_page != old_page)
			display_info_page (FALSE);

		old_page = cur_page;

		switch (ReadHelpCh()) {
			case ESC:	/* common arrow keys */
				break;

#if USE_CURSES
			case iKeyUp:				/* line up */
				if (--pos_help < 0)
					pos_help = (max_page-1) * help_lines;
				old_page = -1;
				cur_page = pos_help / help_lines + 1;
				break;
			case iKeyDown:				/* line down */
				if (++pos_help >= max_line)
					pos_help = 0;
				old_page = -1;
				cur_page = pos_help / help_lines + 1;
				break;
#endif
			case iKeyPageDown:			/* page down */
			case iKeyPageDown2:
			case iKeyPageDown3:
				if (cur_page < max_page) {
					pos_help = cur_page * help_lines;
					cur_page++;
				} else {
					pos_help = 0;
					cur_page = 1;
				}
				break;

			case iKeyPageUp:			/* page up */
			case iKeyPageUp2:
			case iKeyPageUp3:
				if (--cur_page <= 0)
					cur_page = max_page;
				pos_help = (cur_page-1) * help_lines;
				break;

			case iKeyFirstPage:			/* Home */
			case iKeyHelpFirstPage:
				if (cur_page != 1) {
					cur_page = 1;
					pos_help = 0;
				}
				break;

			case iKeyLastPage:			/* End */
			case iKeyHelpLastPage:
				if (cur_page != max_page)
					cur_page = max_page;
				pos_help = (max_page-1) * help_lines;
				break;

			default:
				ClearScreen ();
				return;
		}
	}
}


void
display_info_page (t_bool first)
{
	char buf[LEN];
	int i, help_lines;

#ifdef HAVE_COLOR
	fcol(col_help);
#endif
#if USE_CURSES
	if (first)
#endif
		ClearScreen ();
	sprintf (buf, info_title, cur_page, max_page);
	center_line (0, TRUE, buf);
	MoveCursor (INDEX_TOP, 0);

	if (beginner_level)
		help_lines = NOTESLINES + MINI_HELP_LINES - 1;
	else
		help_lines = NOTESLINES;

	if (info_type == HELP_INFO) {
		for (i=pos_help ; i < (pos_help + help_lines) && info_help[i] ; i++)
			my_fputs (info_help[i], stdout);
	} else {
		for (i=pos_help ; i < (pos_help + help_lines) && posted[i].date[0] ; i++) {
			sprintf (buf, "%8s  %c  %-*s  %s",
				posted[i].date, posted[i].action,
				group_len, posted[i].group, posted[i].subj);
				buf[cCOLS-2] = '\0';
			my_printf ("%s" cCRLF, buf);
		}
	}
	CleartoEOS ();

	center_line (cLINES, FALSE, txt_hit_space_for_more);
#ifdef HAVE_COLOR
	fcol(col_normal);
#endif
}


void
show_mini_help (
	int level)
{
	int line;

	if (!beginner_level)
		return;

	line = NOTESLINES + (MINI_HELP_LINES - 2);

#ifdef HAVE_COLOR
	fcol(col_minihelp);
#endif

	switch (level) {
		case SELECT_LEVEL:
			center_line (line, FALSE, txt_mini_select_1);
			center_line (line+1, FALSE, txt_mini_select_2);
			center_line (line+2, FALSE, txt_mini_select_3);
			break;
		case GROUP_LEVEL:
			center_line (line, FALSE, txt_mini_group_1);
			center_line (line+1, FALSE, txt_mini_group_2);
			center_line (line+2, FALSE, txt_mini_group_3);
			break;
		case THREAD_LEVEL:
			center_line (line, FALSE, txt_mini_thread_1);
			center_line (line+1, FALSE, txt_mini_thread_2);
			break;
		case PAGE_LEVEL:
			center_line (line, FALSE, txt_mini_page_1);
			center_line (line+1, FALSE, txt_mini_page_2);
			center_line (line+2, FALSE, txt_mini_page_3);
			break;
		default:
			error_message ("Unknown display level");
			break;
	}
#ifdef HAVE_COLOR
	fcol(col_normal);
#endif
}


void
toggle_mini_help (
	int level)
{
	beginner_level = !beginner_level;
	set_win_size (&cLINES, &cCOLS);
	show_mini_help (level);
}
