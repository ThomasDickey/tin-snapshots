/*
 *  Project   : tin - a Usenet reader
 *  Module    : prompt.c
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 14.06.1997
 *  Notes     :
 *  Copyright : (c) Copyright 1991-97 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"
#include	"extern.h"
#include	"menukeys.h"

/*
 * FIXME put in tin.h (or find a better solution). See also: config.c
 */
#define option_lines_per_page (cLINES - INDEX_TOP - 3)

/*
 *  prompt_num
 *  get a number from the user
 *  Return -1 if missing or bad number typed
 */

int
prompt_num (
	int ch,
	const char *prompt)
{
	char *p;
	int num;

	set_alarm_clock_off ();

	clear_message ();

	sprintf (msg, "%c", ch);

	if ((p = getline (prompt, TRUE, msg, 0, HIST_OTHER)) != (char *) 0) {
		strcpy (msg, p);
		num = atoi (msg);
	} else {
		num = -1;
	}

	clear_message ();

	set_alarm_clock_on ();

	return (num);
}

/*
 *  prompt_string
 *  get a string from the user
 *  Return TRUE if a valid string was typed, FALSE otherwise
 */

int
prompt_string (
	const char *prompt,
	char *buf,
	int which_hist)
{
	char *p;

	set_alarm_clock_off ();

	clear_message ();

	if ((p = getline (prompt, FALSE, (char *) 0, 0, which_hist)) == (char *) 0) {
		buf[0] = '\0';
		clear_message ();
		set_alarm_clock_on ();
		return FALSE;
	}
	strcpy (buf, p);

	clear_message ();
	set_alarm_clock_on ();

	return TRUE;
}

/*
 *  prompt_menu_string
 *  get a string from the user
 *  Return TRUE if a valid string was typed, FALSE otherwise
 */

int
prompt_menu_string (
	int line,
	int col,
	char *var)
{
	char *p;

	set_alarm_clock_off ();

	MoveCursor (line, col);

	if ((p = getline ("", FALSE, var, 0, HIST_OTHER)) == (char *) 0) {
		set_alarm_clock_on ();
		return FALSE;
	}
	strcpy (var, p);

	set_alarm_clock_on ();

	return TRUE;
}

/*
 * prompt_yn
 * prompt user for 'y'es or 'n'o decision. "prompt" will be displayed in line
 * "line" giving the default answer "default_answer".
 * The function returns 1 if the user decided "yes", -1 if the user wanted
 * to escape, or 0 for any other key or decision.
 */

int
prompt_yn (
	int line,
	const char *prompt,
	t_bool default_answer)
{
	char ch, prompt_ch;
	int yn_loop = TRUE;

	set_alarm_clock_off ();

	/*
	 * clear buffer - this is needed, otherwise a lost
	 * connection right before a resync_active() call
	 * would lead to a 'n' answer to the reconect prompt
	 */
	fflush(stdin);

	while (yn_loop) {
		prompt_ch = (default_answer ? iKeyPromptYes : iKeyPromptNo);

		MoveCursor (line, 0);
		CleartoEOLN ();
		my_printf ("%s%c", prompt, prompt_ch);
		cursoron ();
		my_flush ();
		MoveCursor (line, (int) strlen (prompt));

		if (((ch = (char) ReadCh()) == '\n') || (ch == '\r')) {
			ch = prompt_ch;
		}
		yn_loop = FALSE; /* normal case: leave loop */

		switch (ch) {
#ifndef WIN32
			case ESC:	/* (ESC) common arrow keys */
#	ifdef HAVE_KEY_PREFIX
			case KEY_PREFIX:
#	endif
				switch (get_arrow_key ()) {
#endif /* WIN32 */
					case KEYMAP_UP:
					case KEYMAP_DOWN:
						default_answer = !default_answer;
						yn_loop = TRUE; /* don't leave loop */
						break;

					case KEYMAP_LEFT:
						ch = ESC;
						break;

					case KEYMAP_RIGHT:
						ch = prompt_ch;
						break;
#ifndef WIN32
					default:
						break;
				}
				break;
#endif
			default:
				break;
		}
	}

	if (line == cLINES) {
		clear_message ();
	} else {
		MoveCursor (line, (int) strlen (prompt));
		if (ch == ESC) {
			my_fputc (prompt_ch, stdout);
		} else {
			my_fputc (ch, stdout);
		}
	}
	cursoroff ();
	my_flush ();

	set_alarm_clock_on ();

	return (tolower ((unsigned char)ch) == tolower ((unsigned char)iKeyPromptYes)) ? 1 : (ch == ESC) ? -1 : 0;
}


/*
 * help_text is displayed near the bottom of the screen.
 * var is an index into a list containing size elements.
 * The text from list is shown at row, col + len(prompt_text)
 * Choice is incremented using the space bar, wrapping to 0
 * ESC is used to abort any changes, RET saves changes.
 * The new value is returned.
 */
int
prompt_list (
	int row,
	int col,
	int var,
	constext *help_text,
	constext *prompt_text,
	constext *list[],
	int size)
{
	int ch, var_orig;
	int i;
	size_t width = 0;

	set_alarm_clock_off ();

	/*
	 * Find the length of longest printable text
	 */
	for (i = 0; i < size; i++)
		width = MAX(width, strlen(list[i]));

	var_orig = var;

	show_menu_help (help_text);
	cursoron ();

	do {
		MoveCursor (row, col + (int) strlen (prompt_text));
		if ((ch = (char) ReadCh ()) == ' ') {

			/*
			 * Increment list, looping around at the max
			 */
			var = ++var % size;

			my_printf("%-*s", (int)width, list[var]);
			my_flush ();
		}
	} while (ch != '\r' && ch != '\n' && ch != ESC);

	if (ch == ESC) {
		var = var_orig;

		my_printf("%-*s", (int)width, list[var]);
		my_flush ();
	}

	cursoroff ();

	set_alarm_clock_on ();

	return(var);
}

/*
 * Special case of prompt_list() Toggle between ON and OFF
 */
void
prompt_on_off (
	int row,
	int col,
	t_bool *var,
	constext *help_text,
	constext *prompt_text)
{
	t_bool ret;

	ret = prompt_list (row, col, (int)*var, help_text, prompt_text, txt_onoff, 2);
	*var = (ret != 0);
}

/*
 * Displays option text and actual option value for string based options in
 * one line, help text for that option near the bottom of the screen. Allows
 * change of the old value by normal editing; history function of getline()
 * will be used properly so that editing won't leave the actual line. Note
 * that "option" is the number the user will see, which is not the same as
 * the array position for this option in option_table (since the latter
 * starts counting with zero instead of one).
 * The function returns TRUE, if the value was changed, FALSE otherwise.
 */

int
prompt_option_string (
	int option)
{
	char prompt[LEN];
	char *p;
	char *variable = OPT_STRING_list[option_table[option - 1].var_index];

	set_alarm_clock_off ();

	show_menu_help (option_table[option - 1].help_text);
	MoveCursor (INDEX_TOP + (option - 1) % option_lines_per_page, 0);
	sprintf (&prompt[0], "-> %3d. %s ", option, option_table[option - 1].option_text);

	if ((p = getline (prompt, FALSE, variable, 0, HIST_OTHER)) == (char *) 0) {
		set_alarm_clock_on ();
		return FALSE;
	}
	strcpy (variable, p);

	set_alarm_clock_on ();

	return TRUE;
}

/*
 * Displays option text and actual option value for number based options in
 * one line, help text for that option near the bottom of the screen. Allows
 * change of the old value by normal editing; history function of getline()
 * will be used properly so that editing won't leave the actual line. Note
 * that "option" is the number the user will see, which is not the same as
 * the array position for this option in option_table (since the latter
 * starts counting with zero instead of one).
 * The function returns TRUE if the value was changed, FALSE otherwise.
 */

int
prompt_option_num (
	int option)
{
	char prompt[LEN];
	char number[LEN];
	char *p;
	int num;

	set_alarm_clock_off ();

	show_menu_help (option_table[option - 1].help_text);
	MoveCursor (INDEX_TOP + (option - 1) % option_lines_per_page, 0);
	sprintf (&prompt[0], "-> %3d. %s ", option, option_table[option - 1].option_text);
	sprintf (&number[0], "%d", *(option_table[option - 1].variable));

	if ((p = getline (prompt, TRUE, number, 0, HIST_OTHER)) == (char *) 0) {
		return FALSE;
	}
	strcpy (number, p);
	num = atoi (number);
	*(option_table[option - 1].variable) = num;

	clear_message ();
	set_alarm_clock_on ();

	return TRUE;
}

/*
 * Displays option text and actual option value for character based options
 * in one line, help text for that option near the bottom of the screen.
 * Allows change of the old value by normal editing. Note that "option" is
 * the number the user will see, which is not the same as the array position
 * for this option in option_table (since the latter starts counting with
 * zero instead of one).
 * The function returns TRUE if the value was changed, FALSE otherwise.
 */

int
prompt_option_char (
	int option)
{
	char prompt[LEN];
	char input[2];
	char *p = &input[0];
	char *variable = OPT_CHAR_list[option_table[option - 1].var_index];

	input[0] = *variable;
	input[1] = '\0';

	set_alarm_clock_off ();

	show_menu_help (option_table[option - 1].help_text);
	MoveCursor (INDEX_TOP + (option - 1) % option_lines_per_page, 0);
	sprintf (&prompt[0], "-> %3d. %s ", option, option_table[option - 1].option_text);

	if ((p = getline (prompt, FALSE, p, 1, HIST_OTHER)) == (char *) 0) {
		set_alarm_clock_on ();
		return FALSE;
	}

	*variable = p[0];

	clear_message ();
	set_alarm_clock_on ();

	return TRUE;
}


void
prompt_1 (
	const char *format,
	int ch_default)
{
	sprintf (msg, "%s%c", format, ch_default);
	wait_message (msg);
	MoveCursor (cLINES, (int) strlen (format));
}


void
prompt_2(
	const char *format,
	const char *subject,
	int ch_default)
{
	int have = cCOLS - strlen (format) + 4;
	int want = strlen(subject);

	if (want > 0 && subject[want-1] == '\n')
		want--;
	if (have > want)
		have = want;
	sprintf (msg, format, have, subject, ch_default);

	wait_message (msg);
	MoveCursor (cLINES, (int) strlen (msg) - 1);
}

/*
 * Wait until a key is pressed. We specify the <RETURN> key otherwise
 * pedants will point out that:
 * i)  There is no 'any' key on a keyboard
 * ii) CTRL, SHIFT etc don't work
 */
void
continue_prompt (void)
{
	set_alarm_clock_off ();

	info_message (txt_return_key);
	(void) ReadCh ();
#if USE_CURSES
	cmd_line = FALSE;
	my_retouch();
#endif

	set_alarm_clock_on ();
}

