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

#include	"tin.h"

static char txt_help_empty_line[] = "\r\n";

char *help_select[] = {
	txt_help_ctrl_d,
	txt_help_ctrl_f,
	txt_help_b,
#ifdef HAVE_COLOR
	txt_help_color,
#endif
	txt_help_j,
txt_help_empty_line,
	txt_help_g_4,
	txt_help_g,
	txt_help_g_tab,
	txt_help_n,
	txt_help_g_search,
	txt_help_g_hash,
txt_help_empty_line,
	txt_help_g_cr,
	txt_help_w,
	txt_help_sel_c,
	txt_help_g_z,
	txt_help_g_ctrl_r,
txt_help_empty_line,
	txt_help_g_y,
	txt_help_y,
	txt_help_g_r,
	txt_help_s,
#ifdef CASE_PROBLEM
	txt_help_BIGS,
#else
	txt_help_S,
#endif
	txt_help_m,
#ifdef CASE_PROBLEM
	txt_help_BIGI,
#else
	txt_help_W,
#endif
txt_help_empty_line,
	txt_help_g_q,
	txt_help_esc,
	txt_help_h,
	txt_help_ctrl_l,
#ifndef NO_SHELL_ESCAPE
	txt_help_shell,
#endif
txt_help_empty_line,
#ifdef CASE_PROBLEM
	txt_help_BIGM,
#else
	txt_help_M,
#endif
	txt_help_g_d,
#ifdef CASE_PROBLEM
	txt_help_BIGI,
#else
	txt_help_I,
#endif
txt_help_empty_line,
	txt_help_s_i,
txt_help_empty_line,
	txt_help_v,
	txt_help_bug_report,
	(char *) 0
};

char *help_group[] = {
	txt_help_ctrl_d,
	txt_help_ctrl_f,
	txt_help_b,
	txt_help_j,
txt_help_empty_line,
	txt_help_i_4,
	txt_help_i_search,
	txt_help_hash,
	txt_help_a,
#ifdef CASE_PROBLEM
	txt_help_BIGB,
#else
	txt_help_B,
#endif
txt_help_empty_line,
	txt_help_i_cr,
	txt_help_i_tab,
	txt_help_i_n,
	txt_help_i_p,
	txt_help_dash,
	txt_help_l,
	txt_help_w,
	txt_help_x,
txt_help_empty_line,
	txt_help_p_m,
	txt_help_p_s,
#ifdef CASE_PROBLEM
	txt_help_p_BIGS,
#else
	txt_help_p_S,
#endif
#ifndef NO_PIPING
	txt_help_pipe,
#endif
	txt_help_o,
	txt_help_t,
txt_help_empty_line,
	txt_help_p_z,
#ifdef CASE_PROBLEM
	txt_help_BIGK,
#else
	txt_help_K,
#endif
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
#ifdef CASE_PROBLEM
	txt_help_BIGU,
#else
	txt_help_U,
#endif
txt_help_empty_line,
	txt_help_u,
	txt_help_r,
#ifdef CASE_PROBLEM
	txt_help_BIGX,
	txt_help_BIGW,
#else
	txt_help_X,
	txt_help_W,
#endif
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
#ifdef CASE_PROBLEM
	txt_help_BIGM,
#else
	txt_help_M,
#endif
	txt_help_d,
#ifdef CASE_PROBLEM
	txt_help_BIGI,
#else
	txt_help_I,
#endif
txt_help_empty_line,
	txt_help_g_t_p_i,
txt_help_empty_line,
	txt_help_v,
	txt_help_bug_report,
	(char *) 0
};

char *help_thread[] = {
	txt_help_ctrl_d,
	txt_help_ctrl_f,
	txt_help_b,
	txt_help_j,
txt_help_empty_line,
	txt_help_t_4,
	txt_help_hash,
	txt_help_t_cr,
	txt_help_p_tab,
txt_help_empty_line,
	txt_help_p_z,
	txt_help_ck,
	txt_help_t,
txt_help_empty_line,
	txt_help_i,
	txt_help_q,
	txt_help_esc,
	txt_help_h,
	txt_help_ctrl_l,
txt_help_empty_line,
	txt_help_d,
#ifdef CASE_PROBLEM
	txt_help_BIGI,
#else
	txt_help_I,
#endif
txt_help_empty_line,
	txt_help_g_t_p_i,
txt_help_empty_line,
	txt_help_v,
	txt_help_bug_report,
	(char *) 0
};

char *help_page[] = {
	txt_help_ctrl_d,
	txt_help_ctrl_f,
	txt_help_b,
	txt_help_p_ctrl_r,
	txt_help_p_g,
txt_help_empty_line,
	txt_help_p_0,
	txt_help_p_4,
	txt_help_p_cr,
	txt_help_p_tab,
	txt_help_p_n,
	txt_help_p_p,
	txt_help_thread,
	txt_help_dash,
txt_help_empty_line,
	txt_help_a,
#ifdef CASE_PROBLEM
	txt_help_BIGB,
#else
	txt_help_B,
#endif
	txt_help_p_search,
txt_help_empty_line,
	txt_help_w,
	txt_help_p_f,
	txt_help_p_r,
	txt_help_x,
	txt_help_e,
txt_help_empty_line,
	txt_help_p_m,
	txt_help_p_s,
#ifndef NO_PIPING
	txt_help_pipe,
#endif
	txt_help_o,
	txt_help_t,
txt_help_empty_line,
	txt_help_ctrl_a,
	txt_help_ctrl_k,
	txt_help_quick_select,
	txt_help_quick_kill,
#ifdef CASE_PROBLEM
	txt_help_BIGD,
#else
	txt_help_D,
#endif
txt_help_empty_line,
	txt_help_p_z,
	txt_help_p_k,
	txt_help_c,
	txt_help_cC,
txt_help_empty_line,
	txt_help_ctrl_h,
	txt_help_p_d,
txt_help_empty_line,
#ifdef CASE_PROBLEM
	txt_help_BIGW,
#else
	txt_help_W,
#endif
txt_help_empty_line,
#ifdef CASE_PROBLEM
	txt_help_BIGT,
#else
	txt_help_T,
#endif
	txt_help_i,
	txt_help_q,
	txt_help_esc,
	txt_help_h,
	txt_help_ctrl_l,
#ifndef NO_SHELL_ESCAPE
	txt_help_shell,
#endif
txt_help_empty_line,
#ifdef CASE_PROBLEM
	txt_help_BIGM,
	txt_help_BIGI,
#else
	txt_help_M,
	txt_help_I,
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
	(char *) 0
};

static char *info_title;
static char **info_help;
static int cur_page;
static int group_len = 0;
static int info_type;
static int max_page;
static int pos_help;

void 
show_info_page (type, help, title)
	int type; 
	char *help[];
	char *title;
{
	int ch;
	int i, len;
	int help_lines;
	int old_page = 0;

	if (NOTESLINES <= 0) {
		return;
	}

	if (beginner_level) {
		help_lines = NOTESLINES + MINI_HELP_LINES - 1; 
	} else {
		help_lines = NOTESLINES;
	}
	
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
		for (i=0 ; help[i] ; i++) {
			continue;
		}
	} else {
		for (i=0 ; posted[i].date[0] ; i++) {
			len = strlen (posted[i].group);
			if (len > group_len) {
 				group_len = len;
			}
 		}
	}
	
	max_page = i / help_lines;
	if (i % help_lines) {
		max_page++;
	}

	set_xclick_off ();
	forever {
		if (cur_page != old_page) {
			display_info_page ();
		}

		old_page = cur_page;
		
		ch = ReadCh ();
		switch (ch) {
			case ESC:	/* common arrow keys */
#ifdef HAVE_KEY_PREFIX
			case KEY_PREFIX:
#endif
				switch (get_arrow_key ()) {
					case KEYMAP_LEFT:
						goto help_done;

					case KEYMAP_UP:
					case KEYMAP_PAGE_UP:
						goto help_page_up;

					case KEYMAP_RIGHT:
					case KEYMAP_DOWN:
					case KEYMAP_PAGE_DOWN:
						goto help_page_down;

					case KEYMAP_HOME:
						goto help_home;

					case KEYMAP_END:
						goto help_end;
				}
				break;

			case iKeyHelpPageDown:			/* page down */
			case iKeyHelpPageDown2:			/* vi style */
			case iKeyHelpPageDown3:
			case iKeyHelpPageDown4:
help_page_down:
				if (cur_page < max_page) {
					pos_help = cur_page * help_lines;
					cur_page++;
				} else {
					pos_help = 0;
					cur_page = 1;
				}				
				break;
			
			case iKeyHelpPageUp:			/* page up */
			case iKeyHelpPageUp2:			/* vi style */
			case iKeyHelpPageUp3:
			case iKeyHelpPageUp4:
help_page_up:
				if (cur_page > 1) {
					cur_page--;
					pos_help = (cur_page-1) * help_lines;
				} else {
					pos_help = (max_page-1) * help_lines;
					cur_page = max_page;
				}				
				break;

			case iKeyHelpHome:			/* Home */
			case iKeyHelpHome2:
help_home:
				if (cur_page != 1) {
					cur_page = 1;
					pos_help = 0;
				}
				break;

			case iKeyHelpEnd:				/* End */
			case iKeyHelpEnd2:
help_end:
				if (cur_page != max_page) {
					cur_page = max_page;
					pos_help = (max_page-1) * help_lines;
				}
				break;

			default:
help_done:
				ClearScreen ();
				return;
		}	
	}
}


void 
display_info_page ()
{
	char buf[LEN];
	int i, help_lines;
	
	ClearScreen ();
	sprintf (buf, info_title, cur_page, max_page);
	center_line (0, TRUE, buf);
	MoveCursor (INDEX_TOP, 0);

	if (beginner_level) {
		help_lines = NOTESLINES + MINI_HELP_LINES - 1; 
	} else {
		help_lines = NOTESLINES;
	}

	if (info_type == HELP_INFO) { 
		for (i=pos_help ; i < (pos_help + help_lines) && info_help[i] ; i++) {
			my_fputs (info_help[i], stdout);
		}
	} else {
		for (i=pos_help ; i < (pos_help + help_lines) && posted[i].date[0] ; i++) {
			sprintf (buf, "%8s  %c  %-*s  %s",
				posted[i].date, posted[i].action,  
				group_len, posted[i].group, posted[i].subj);
				buf[cCOLS-2] = '\0';
			printf ("%s\r\n", buf);
		}
	}

	center_line (cLINES, FALSE, txt_hit_space_for_more);
}


void 
show_mini_help (level)
	int level;
{
	int line;
	
	if (! beginner_level) {
		return;
	}

	line = NOTESLINES + (MINI_HELP_LINES - 2);

	switch (level) {
		case SELECT_LEVEL:
			center_line (line,   FALSE, txt_mini_select_1);
			center_line (line+1, FALSE, txt_mini_select_2);
			center_line (line+2, FALSE, txt_mini_select_3);
			break;
		case SPOOLDIR_LEVEL:
			center_line (line,   FALSE, txt_mini_spooldir_1);
			break;
		case GROUP_LEVEL:
			center_line (line,   FALSE, txt_mini_group_1);
			center_line (line+1, FALSE, txt_mini_group_2);
			center_line (line+2, FALSE, txt_mini_group_3);
			break;
		case THREAD_LEVEL:
			center_line (line,   FALSE, txt_mini_thread_1);
			center_line (line+1, FALSE, txt_mini_thread_2);
			break;
		case PAGE_LEVEL:
			center_line (line,   FALSE, txt_mini_page_1);
			center_line (line+1, FALSE, txt_mini_page_2);
			center_line (line+2, FALSE, txt_mini_page_3);
			break;
		default:
			error_message ("Unknown display level", "");
			break;
	}
}


void 
toggle_mini_help (level)
	int level;
{
	beginner_level = !beginner_level;
	set_win_size (&cLINES, &cCOLS);
	show_mini_help (level);
}
