/*
 *  Project   : tin - a Usenet reader
 *  Module    : tin_getline.c
 *  Author    : Chris Thewalt & Iain Lea
 *  Created   : 1991-11-09
 *  Updated   : 1994-07-27
 *  Notes     : emacs style line editing input package.
 *  Copyright : (c) Copyright 1991-99 by Chris Thewalt & Iain Lea
 *              Permission to use, copy, modify, and distribute this
 *              software for any purpose and without fee is hereby
 *              granted, provided that the above copyright notices
 *              appear in all copies and that both the copyright
 *              notice and this permission notice appear in supporting
 *              documentation. This software is provided "as is" without
 *              express or implied warranty.
 */

#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */
#ifndef TCURSES_H
#	include "tcurses.h"
#endif /* !TCURSES_H */

#define BUF_SIZE	1024
#define SCROLL		30
#define TABSIZE		4

#define CTRL_A	'\001'
#define CTRL_B	'\002'
#define CTRL_D	'\004'
#define CTRL_E	'\005'
#define CTRL_F	'\006'
#define CTRL_H	'\010'
#define CTRL_K	'\013'
#define CTRL_L	'\014'
#define CTRL_R	'\022'
#define CTRL_N	'\016'
#define CTRL_P	'\020'
#define CTRL_U	'\025'
#define CTRL_W	'\027'
#define TAB	'\t'
#define DEL	'\177'

static char gl_buf[BUF_SIZE];	/* input buffer */
static int gl_init_done = 0;	/* -1 is terminal, 1 is batch */
static const char *gl_prompt;	/* to save the prompt string */
static int gl_width = 0;	/* net size available for input */
static int gl_pos, gl_cnt = 0;	/* position and size of input */
t_bool is_passwd;

/*
 * local prototypes
 */
static void gl_addchar (int c);
static void gl_del (int loc);
static void gl_fixup (int change, int cursor);
static int gl_tab (char *buf, int offset, int *loc);
static void gl_redraw (void);
static void gl_newline (int w);
static void gl_kill (void);
static void gl_kill_back_word (void);
static void hist_add (int w);
static void hist_next (int w);
static void hist_prev (int w);

static int (*gl_in_hook) (char *) = 0;
static int (*gl_out_hook) (char *) = 0;
static int (*gl_tab_hook) (char *, int, int *) = gl_tab;

char *
tin_getline (
	const char *prompt,
	int number_only,	/* 1=positive numbers only, 2=negative too */
	char *str,
	int max_chars,
	t_bool passwd,
	int which_hist)
{
	int c, i, loc, tmp, gl_max;

	is_passwd = passwd;

	set_xclick_off ();
	if (prompt == (char *) 0) {
		prompt = "";
	}
	gl_buf[0] = 0;		/* used as end of input indicator */
	gl_fixup (-1, 0);	/* this resets gl_fixup */
	gl_width = cCOLS - strlen (prompt);
	gl_prompt = prompt;
	gl_pos = gl_cnt = 0;

	if (max_chars == 0) {
		if (number_only) {
			gl_max = 6;
		} else {
			gl_max = BUF_SIZE;
		}
	} else {
		gl_max = max_chars;
	}

	my_fputs (prompt, stdout);
	cursoron ();
	my_flush ();

	if (gl_in_hook) {
		loc = gl_in_hook (gl_buf);
		if (loc >= 0)
			gl_fixup (0, BUF_SIZE);
	}
	if (str != (char *) 0) {
		for (i = 0; str[i]; i++)
			gl_addchar (str[i]);
	}
	while ((c = ReadCh ()) != EOF) {
		c &= 0xff;
		if ((gl_cnt < gl_max) && my_isprint(c)) {
			if (number_only) {
				if (isdigit (c)) {
					gl_addchar (c);
				/* Minus */
				} else if (number_only == 2 && gl_pos == 0 && c == '-') {
					gl_addchar (c);
				} else {
					ring_bell ();
				}
			} else {
				gl_addchar (c);
			}
		} else {
			switch (c) {
				case ESC:	/* abort */
#ifdef HAVE_KEY_PREFIX
				case KEY_PREFIX:
#endif /* HAVE_KEY_PREFIX */
					switch (get_arrow_key (c)) {
						case KEYMAP_UP:
						case KEYMAP_PAGE_UP:
							hist_prev (which_hist);
							break;
						case KEYMAP_PAGE_DOWN:
						case KEYMAP_DOWN:
							hist_next (which_hist);
							break;
						case KEYMAP_RIGHT:
							gl_fixup (-1, gl_pos + 1);
							break;
						case KEYMAP_LEFT:
							gl_fixup (-1, gl_pos - 1);
							break;
						case KEYMAP_HOME:
							gl_fixup (-1, 0);
							break;
						case KEYMAP_END:
							gl_fixup (-1, gl_cnt);
							break;
						case KEYMAP_DEL:
							gl_del (0);
							break;
						case KEYMAP_INS:
							gl_addchar (' ');
							break;
						default:
							return (char *) 0;
					}
					break;
				case '\n':	/* newline */
				case '\r':
					gl_newline (which_hist);
					return gl_buf;
				case CTRL_A:
					gl_fixup (-1, 0);
					break;
				case CTRL_B:
					gl_fixup (-1, gl_pos - 1);
					break;
				case CTRL_D:
					if (gl_cnt == 0) {
						gl_buf[0] = 0;
						my_fputc ('\n', stdout);
						return gl_buf;
					} else {
						gl_del (0);
					}
					break;
				case CTRL_E:
					gl_fixup (-1, gl_cnt);
					break;
				case CTRL_F:
					gl_fixup (-1, gl_pos + 1);
					break;
				case CTRL_H:
				case DEL:
					gl_del (-1);
					break;
				case TAB:
					if (gl_tab_hook) {
						tmp = gl_pos;
						loc = gl_tab_hook (gl_buf, strlen (gl_prompt), &tmp);
						if (loc >= 0 || tmp != gl_pos)
							gl_fixup (loc, tmp);
					}
					break;
				case CTRL_W:
					gl_kill_back_word ();
					break;
				case CTRL_U:
					gl_fixup (-1, 0);
					/* FALLTHROUGH */
				case CTRL_K:
					gl_kill ();
					break;
				case CTRL_L:
				case CTRL_R:
					gl_redraw ();
					break;
				case CTRL_N:
					hist_next (which_hist);
					break;
				case CTRL_P:
					hist_prev (which_hist);
					break;
				default:
					ring_bell ();
					break;
			}
		}
	}
	return gl_buf;
}

/*
 * adds the character c to the input buffer at current location if
 * the character is in the allowed template of characters
 */

static void
gl_addchar (
	int c)
{
	int i;

	if (gl_cnt >= BUF_SIZE - 1) {
		error_message ("tin_getline: input buffer overflow");
		giveup();
	}

	for (i = gl_cnt; i >= gl_pos; i--) {
		gl_buf[i + 1] = gl_buf[i];
	}
	gl_buf[gl_pos] = c;
	gl_fixup (gl_pos, gl_pos + 1);
}

/*
 * Cleans up entire line before returning to caller. A \n is appended.
 * If line longer than screen, we redraw starting at beginning
 */

static void
gl_newline (
	int w)
{
	int change = gl_cnt;
	int len = gl_cnt;
	int loc = gl_width - 5;	/* shifts line back to start position */

	if (gl_cnt >= BUF_SIZE - 1) {
		error_message ("tin_getline: input buffer overflow");
		giveup();
	}
	hist_add (w);		/* only adds if nonblank */
	if (gl_out_hook) {
		change = gl_out_hook (gl_buf);
		len = strlen (gl_buf);
	}
	if (loc > len)
		loc = len;
	gl_fixup (change, loc);	/* must do this before appending \n */
	gl_buf[len] = '\0';
}

/*
 * Delete a character.  The loc variable can be:
 *    -1 : delete character to left of cursor
 *     0 : delete character under cursor
 */

static void
gl_del (
	int loc)
{
	int i;

	if ((loc == -1 && gl_pos > 0) || (loc == 0 && gl_pos < gl_cnt)) {
		for (i = gl_pos + loc; i < gl_cnt; i++)
			gl_buf[i] = gl_buf[i + 1];
		gl_fixup (gl_pos + loc, gl_pos + loc);
	} else {
		ring_bell ();
	}
}

/*
 * delete from current position to the end of line
 */

static void
gl_kill (
	void)
{
	if (gl_pos < gl_cnt) {
		gl_buf[gl_pos] = '\0';
		gl_fixup (gl_pos, gl_pos);
	} else {
		ring_bell ();
	}
}

/*
 * delete from the start of current or last word to current position
 */

static void
gl_kill_back_word (
	void)
{
	int i, cur;

	/* delete spaces */
	for (i = gl_pos - 1; i >= 0 && isspace((int)gl_buf[i]); --i)
		;
	/* delete not alnum characters but graph characters */
	for (; i >= 0 && isgraph((int)gl_buf[i]) && !isalnum((int)gl_buf[i]); --i)
		;
	/* delete all graph characters except '/' */
	for (; i >= 0 && gl_buf[i] != '/' && isgraph((int)gl_buf[i]); --i)
		;
	i++;
	if (i != gl_pos) {
		cur = gl_pos;
		gl_fixup (-1, i);
		strcpy (&gl_buf[i], &gl_buf[cur]);
		gl_fixup (i, i);
	} else {
		ring_bell ();
	}
}

/*
 * emit a newline, reset and redraw prompt and current input line
 */

static void
gl_redraw (
	void)
{
	if (gl_init_done == -1) {
		my_fputc ('\n', stdout);
		my_fputs (gl_prompt, stdout);
		gl_pos = 0;
		gl_fixup (0, BUF_SIZE);
	}
}

/*
 * This function is used both for redrawing when input changes or for
 * moving within the input line.  The parameters are:
 *   change : the index of the start of changes in the input buffer,
 *            with -1 indicating no changes.
 *   cursor : the desired location of the cursor after the call.
 *            A value of BUF_SIZE can be used to indicate the cursor
 *            should move just past the end of the input line.
 */

static void
gl_fixup (
	int change,
	int cursor)
{
	static int gl_shift;	/* index of first on screen character */
	static int off_right;	/* true if more text right of screen */
	static int off_left;	/* true if more text left of screen */
	int left = 0, right = -1;	/* bounds for redraw */
	int pad;		/* how much to erase at end of line */
	int backup;		/* how far to backup before fixing */
	int new_shift;		/* value of shift based on cursor */
	int extra;		/* adjusts when shift (scroll) happens */
	int i;

	if (change == -1 && cursor == 0 && gl_buf[0] == 0) {	/* reset */
		gl_shift = off_right = off_left = 0;
		return;
	}
	pad = (off_right) ? gl_width - 1 : gl_cnt - gl_shift;	/* old length */
	backup = gl_pos - gl_shift;
	if (change >= 0) {
		gl_cnt = strlen (gl_buf);
		if (change > gl_cnt)
			change = gl_cnt;
	}
	if (cursor > gl_cnt) {
		if (cursor != BUF_SIZE)		/* BUF_SIZE means end of line */
			ring_bell ();
		cursor = gl_cnt;
	}
	if (cursor < 0) {
		ring_bell ();
		cursor = 0;
	}
	if (!is_passwd) {
		if (off_right || (off_left && (cursor < gl_shift + gl_width - SCROLL / 2)))
			extra = 2;	/* shift the scrolling boundary */
		else
			extra = 0;
		new_shift = cursor + extra + SCROLL - gl_width;
		if (new_shift > 0) {
			new_shift /= SCROLL;
			new_shift *= SCROLL;
		} else
			new_shift = 0;
		if (new_shift != gl_shift) {	/* scroll occurs */
			gl_shift = new_shift;
			off_left = (gl_shift) ? 1 : 0;
			off_right = (gl_cnt > gl_shift + gl_width - 1) ? 1 : 0;
			left = gl_shift;
			right = (off_right) ? gl_shift + gl_width - 2 : gl_cnt;
		} else if (change >= 0) {	/* no scroll, but text changed */
			if (change < gl_shift + off_left)
				left = gl_shift;
			else {
				left = change;
				backup = gl_pos - change;
			}
			off_right = (gl_cnt > gl_shift + gl_width - 1) ? 1 : 0;
			right = (off_right) ? gl_shift + gl_width - 2 : gl_cnt;
		}
		pad -= (off_right) ? gl_width - 1 : gl_cnt - gl_shift;
		pad = (pad < 0) ? 0 : pad;
		if (left <= right) {	/* clean up screen */
			for (i = 0; i < backup; i++)
				my_fputc ('\b', stdout);
			if (left == gl_shift && off_left) {
				my_fputc ('$', stdout);
				left++;
			}
			for (i = left; i < right; i++)
				my_fputc (gl_buf[i], stdout);
			if (off_right) {
				my_fputc ('$', stdout);
				gl_pos = right + 1;
			} else {
				for (i = 0; i < pad; i++)	/* erase remains of prev line */
					my_fputc (' ', stdout);
				gl_pos = right + pad;
			}
		}
		i = gl_pos - cursor;	/* move to final cursor location */
		if (i > 0) {
			while (i--)
				my_fputc ('\b', stdout);
		} else {
			for (i = gl_pos; i < cursor; i++)
				my_fputc (gl_buf[i], stdout);
		}
		my_flush ();
	}
	gl_pos = cursor;
}

/*
 * default tab handler, acts like tabstops every TABSIZE cols
 */

static int
gl_tab (
	char *buf,
	int offset,
	int *loc)
{
	int i, count, len;

	len = strlen (buf);
	count = TABSIZE - (offset + *loc) % TABSIZE;
	for (i = len; i >= *loc; i--)
		buf[i + count] = buf[i];
	for (i = 0; i < count; i++)
		buf[*loc + i] = ' ';
	i = *loc;
	*loc = i + count;
	return i;
}

static void
hist_add (
	int w)
{
	char *p = gl_buf;

	if (w == HIST_NONE)
		return;

	while (*p == ' ' || *p == '\t')		/* only save nonblank line */
		p++;
	if (*p) {
		input_history[w][hist_last[w]] = my_strdup (gl_buf);
		hist_last[w] = (hist_last[w] + 1) % HIST_SIZE;
		if (input_history[w][hist_last[w]]) {	/* erase next location */
			free (input_history[w][hist_last[w]]);
			input_history[w][hist_last[w]] = (char *) 0;
		}
	}
	hist_pos[w] = hist_last[w];
}

/*
 * loads previous hist entry into input buffer, sticks on first
 */

static void
hist_prev (
	int w)
{
	int next;

	if (w == HIST_NONE)
		return;

	next = (hist_pos[w] - 1 + HIST_SIZE) % HIST_SIZE;
	if (next != hist_last[w]) {
		if (input_history[w][next]) {
			hist_pos[w] = next;
			strcpy (gl_buf, input_history[w][hist_pos[w]]);
		} else {
			ring_bell ();
		}
	} else {
		ring_bell ();
	}
	if (gl_in_hook)
		gl_in_hook (gl_buf);
	gl_fixup (0, BUF_SIZE);
}


/*
 * loads next hist entry into input buffer, clears on last
 */
static void
hist_next (
	int w)
{

	if (w == HIST_NONE)
		return;

	if (hist_pos[w] != hist_last[w]) {
		hist_pos[w] = (hist_pos[w] + 1) % HIST_SIZE;
		if (input_history[w][hist_pos[w]]) {
			strcpy (gl_buf, input_history[w][hist_pos[w]]);
		} else {
			gl_buf[0] = 0;
		}
	} else {
		ring_bell ();
	}
	if (gl_in_hook)
		gl_in_hook (gl_buf);
	gl_fixup (0, BUF_SIZE);
}
