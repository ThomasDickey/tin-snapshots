/*
 *  Project   : tin - a Usenet reader
 *  Module    : prompt.c
 *  Author    : I. Lea
 *  Created   : 1991-04-01
 *  Updated   : 1997-12-31
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

	clear_message ();

	sprintf (mesg, "%c", ch);

	if ((p = tin_getline (prompt, TRUE, mesg, 0, FALSE, HIST_OTHER)) != (char *) 0) {
		strcpy (mesg, p);
		num = atoi (mesg);
	} else
		num = -1;

	clear_message ();

	return (num);
}


/*
 *  prompt_string
 *  get a string from the user
 *  Return TRUE if a valid string was typed, FALSE otherwise
 */
t_bool
prompt_string (
	const char *prompt,
	char *buf,
	int which_hist)
{
	return prompt_default_string (prompt, buf, 0, (char *) NULL, which_hist);
}


/*
 * prompt_default_string
 * get a string from the user, display default value
 * Return TRUE if a valid string was typed, FALSE otherwise
 */
t_bool
prompt_default_string (
	const char *prompt,
	char *buf,
	int buf_len,
	char *default_prompt,
	int which_hist)
{
	char *p;

	clear_message ();

	if ((p = tin_getline (prompt, FALSE, default_prompt, buf_len, FALSE, which_hist)) == (char *) 0) {
		buf[0] = '\0';
		clear_message ();
		return FALSE;
	}
	strcpy (buf, p);

	clear_message ();

	return TRUE;
}


/*
 *  prompt_menu_string
 *  get a string from the user
 *  Return TRUE if a valid string was typed, FALSE otherwise
 */
t_bool
prompt_menu_string (
	int line,
	int col,
	char *var)
{
	char *p;

	/*
	 * clear buffer - this is needed, otherwise a lost
	 * connection right before a resync_active() call
	 * would lead to a 'n' answer to the reconect prompt
	 */
	fflush(stdin);

	MoveCursor (line, col);

	if ((p = tin_getline ("", FALSE, var, 0, FALSE, HIST_OTHER)) == (char *) 0)
		return FALSE;

	strcpy (var, p);

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
	int ch, prompt_ch;
	t_bool yn_loop = TRUE;

/*	fflush(stdin);*/		/* Prevent finger trouble from making important decisions */

	while (yn_loop) {
		prompt_ch = (default_answer ? iKeyPromptYes : iKeyPromptNo);

		if (!cmd_line) {
			MoveCursor (line, 0);
			CleartoEOLN ();
		}
		my_printf ("%s%c", prompt, prompt_ch);
		if (!cmd_line)
			cursoron ();
		my_flush ();
		if (!cmd_line)
			MoveCursor (line, (int) strlen (prompt));

		if (((ch = (char) ReadCh()) == '\n') || (ch == '\r'))
			ch = prompt_ch;

		yn_loop = FALSE; /* normal case: leave loop */

		switch (ch) {
#ifndef WIN32
			case ESC:	/* (ESC) common arrow keys */
#	ifdef HAVE_KEY_PREFIX
			case KEY_PREFIX:
#	endif /* HAVE_KEY_PREFIX */
				switch (get_arrow_key (ch)) {
#endif /* !WIN32 */
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
#endif /* !WIN32 */
			default:
				break;
		}
	}

	if (!cmd_line) {
		if (line == cLINES)
			clear_message ();
		else {
			MoveCursor (line, (int) strlen (prompt));
			my_fputc (((ch == ESC) ? prompt_ch : ch), stdout);
		}
		cursoroff ();
		my_flush ();
	}

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
	int adjust = (strcasecmp(list[0], txt_default) == 0);
	size_t width = 0;

	var += adjust;
	size += adjust;
	var_orig = var;

	/*
	 * Find the length of longest printable text
	 */
	for (i = 0; i < size; i++)
		width = MAX(width, strlen(list[i]));

	show_menu_help (help_text);
	cursoron ();

	do {
		MoveCursor (row, col + (int) strlen (prompt_text));
		if ((ch = (char) ReadCh ()) == ' ') {

			/*
			 * Increment list, looping around at the max
			 */
			++var;
			var %= size;

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

	return(var - adjust);
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
 * change of the old value by normal editing; history function of tin_getline()
 * will be used properly so that editing won't leave the actual line. Note
 * that "option" is the number the user will see, which is not the same as
 * the array position for this option in option_table (since the latter
 * starts counting with zero instead of one).
 * The function returns TRUE, if the value was changed, FALSE otherwise.
 */
t_bool
prompt_option_string (
	int option) /* return value is always ignored */
{
	char prompt[LEN];
	char *p;
	char *variable = OPT_STRING_list[option_table[option].var_index];

	show_menu_help (option_table[option].txt->help);
	MoveCursor (option_row(option), 0);
	sprintf (&prompt[0], "-> %3d. %s ", option+1, option_table[option].txt->opt);

	if ((p = tin_getline (prompt, FALSE, variable, 0, FALSE, HIST_OTHER)) == (char *) 0)
		return FALSE;

	strcpy (variable, p);

	return TRUE;
}


/*
 * Displays option text and actual option value for number based options in
 * one line, help text for that option near the bottom of the screen. Allows
 * change of the old value by normal editing; history function of tin_getline()
 * will be used properly so that editing won't leave the actual line. Note
 * that "option" is the number the user will see, which is not the same as
 * the array position for this option in option_table (since the latter
 * starts counting with zero instead of one).
 * The function returns TRUE if the value was changed, FALSE otherwise.
 */
t_bool
prompt_option_num (
	int option) /* return value is always ignored */
{
	char prompt[LEN];
	char number[LEN];
	char *p;
	int num;

	show_menu_help (option_table[option].txt->help);
	MoveCursor (option_row(option), 0);
	sprintf (&prompt[0], "-> %3d. %s ", option+1, option_table[option].txt->opt);
	sprintf (&number[0], "%d", *(option_table[option].variable));

	if ((p = tin_getline (prompt, 2, number, 0, FALSE, HIST_OTHER)) == (char *) 0)
		return FALSE;

	strcpy (number, p);
	num = atoi (number);
	*(option_table[option].variable) = num;

	clear_message ();

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
t_bool
prompt_option_char (
	int option) /* return value is always ignored */
{
	char prompt[LEN];
	char input[2];
	char *p = &input[0];
	char *variable = OPT_CHAR_list[option_table[option].var_index];

	input[0] = *variable;
	input[1] = '\0';

	show_menu_help (option_table[option].txt->help);
	MoveCursor (option_row(option), 0);
	sprintf (&prompt[0], "-> %3d. %s ", option+1, option_table[option].txt->opt);

	if ((p = tin_getline (prompt, FALSE, p, 1, FALSE, HIST_OTHER)) == (char *) 0)
		return FALSE;

	*variable = p[0];

	clear_message ();

	return TRUE;
}

/*
 * Get a string. Make it the new default.
 * If none given, use the default.
 * Return the string or NULL if we can't get anything useful
 */
char *
prompt_string_default (
	const char *prompt,
	char *def,
	const char *failtext,
	int history)
{
	char pattern[LEN];

	clear_message();

	if (!prompt_string (prompt, pattern, history)) {
		clear_message();
		return NULL;
	}

	if (pattern[0] != '\0')			/* got a string - make it the default */
		my_strncpy (def, pattern, LEN);
	else {
		if (def[0] == '\0')	{		/* no default - give up */
			error_message (failtext);
			return NULL;
		}
	}

	return(def);					/* use the default */
}


/*
 * Get a message ID for the 'L' command. Add <> if needed
 * If the msgid exists and is reachable, return its index
 * in arts[], else ART_UNAVAILABLE
 */
int
prompt_msgid (
	void)
{
	char buf[LEN];

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
			return ART_UNAVAILABLE;
		}

		/*
		 * Is it expired or otherwise not on the spool ?
		 */
		if (msgid->article == ART_UNAVAILABLE) {
			info_message(txt_art_unavailable);
			return ART_UNAVAILABLE;
		}

		/*
		 * If the article is no longer part of a thread, then there is
		 * no way to display it
		 */
		if (which_thread(msgid->article) == -1) {
			info_message (txt_no_last_message);
			return ART_UNAVAILABLE;
		}

		return msgid->article;
	}

	return ART_UNAVAILABLE;
}


/*
 * Format a message such that it'll fit within the screen width
 * Useful for fitting long Subjects and newsgroup names into prompts
 * TODO - maybe add a '...' to the string to show it was truncated. See center_line()
 */
char *
sized_message (
	const char *format,
	const char *subject)
{
	/* The formatting info (%.*s) wastes 4 chars, but our prompt needs 1 char */
	int have = cCOLS - strlen (format) + 4 - 1;
	int want = strlen(subject);

	if (want > 0 && subject[want-1] == '\n')
		want--;
	if (have > want)
		have = want;
	sprintf (mesg, format, have, subject);
	return(mesg);
}


/*
 * Implement the Single-Letter-Key mini menus at the bottom of the screen
 * eg, Press a)ppend, o)verwrite, q)uit :
 */
int
prompt_slk_response (
	int ch_default,
	const char *responses,
	const char *fmt, ...)
{
	va_list ap;
	char ch;
	char buf[LEN];

	va_start (ap, fmt);
	vsprintf(buf, fmt, ap);	/* We need to do this, else wait_message() will clobber us */
	va_end (ap);

	do {
		wait_message (0, "%s%c", buf, ch_default);

		/* Get the cursor _just_ right */
		MoveCursor (cLINES, (int) strlen (buf));

		if ((ch = ReadCh ()) == '\r' || ch == '\n')
			ch = ch_default;

	} while (!strchr (responses, ch));

	return(ch);
}


/*
 * Wait until a key is pressed. We specify the <RETURN> key otherwise
 * pedants will point out that:
 * i)  There is no 'any' key on a keyboard
 * ii) CTRL, SHIFT etc don't work
 */
void
continue_prompt (
	void)
{
	int ch;

#ifdef USE_CURSES
	cmd_line = TRUE;
#endif /* USE_CURSES */
	info_message (txt_return_key);
	ch = ReadCh ();

#ifndef WIN32
	switch (ch) {
		case ESC:
#	ifdef HAVE_KEY_PREFIX
		case KEY_PREFIX:
#	endif /* HAVE_KEY_PREFIX */
			(void) get_arrow_key(ch);
			nobreak;	/* FALLTHROUGH */
		default:
			break;
	}
#endif /* !WIN32 */

#ifdef USE_CURSES
	cmd_line = FALSE;
	my_retouch();
#endif /* USE_CURSES */
}
