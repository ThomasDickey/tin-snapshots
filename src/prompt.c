/*
 *  Project   : tin - a Usenet reader
 *  Module    : prompt.c
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 24-10-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"menukeys.h"

/*
 *  prompt_num
 *  get a number from the user
 *  Return -1 if missing or bad number typed
 */

int 
prompt_num (ch, prompt)
	int ch;
	char *prompt;
{
	char *p;
	int num;

	set_alarm_clock_off ();
	
	clear_message ();

	sprintf (msg, "%c", ch);

	if ((p = getline (prompt, TRUE, msg)) != (char *) 0) {
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
prompt_string (prompt, buf)
	char *prompt;
	char *buf;
{
	char *p;

	set_alarm_clock_off ();
	
	clear_message ();

	if ((p = getline (prompt, FALSE, (char *) 0)) == (char *) 0) {
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
prompt_menu_string (line, col, var)
	int line;
	int col;
	char *var;
{
	char *p;

	set_alarm_clock_off ();

	MoveCursor (line, col);

	if ((p = getline ("", FALSE, var)) == (char *) 0) {
		set_alarm_clock_on ();
		return FALSE;
	}
	strcpy (var, p);

	set_alarm_clock_on ();
	
	return TRUE;
}


int 
prompt_yn (line, prompt, default_answer)
	int line;
	char *prompt;
	int default_answer;
{
	char ch, prompt_ch;

	set_alarm_clock_off ();

prompt_yn_loop:
	prompt_ch = (default_answer ? iKeyPromptYes : iKeyPromptNo);

	MoveCursor (line, 0);
	CleartoEOLN ();
	printf ("%s%c", prompt, prompt_ch);
	cursoron ();
	fflush (stdout);
	MoveCursor (line, (int) strlen (prompt));

	if (((ch = (char) ReadCh()) == '\n') || (ch == '\r')) {
		ch = prompt_ch;
	}	

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
					default_answer = 1 - default_answer;
					goto prompt_yn_loop;
				
				case KEYMAP_LEFT:
					ch = ESC;
					break;
					
				case KEYMAP_RIGHT:
					ch = prompt_ch;
					break;
#ifndef WIN32
			}
			break;
#endif
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
	fflush (stdout);

	set_alarm_clock_on ();

	return (tolower (ch) == tolower (iKeyPromptYes)) ? 1 : (ch == ESC) ? -1 : 0;
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
prompt_list (row, col, var, help_text, prompt_text, list, size)
	int row;
	int col;
	int var;
	char *help_text;
	char *prompt_text;
	char *list[];
	int size;
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

			printf("%-*s", (int)width, list[var]);
			fflush (stdout);
		}
	} while (ch != '\r' && ch != '\n' && ch != ESC);

	if (ch == ESC) {
		var = var_orig;

		printf("%-*s", (int)width, list[var]);
		fflush (stdout);
	}

	cursoroff ();
	
	set_alarm_clock_on ();

	return(var);
}

/*
 * Special case of prompt_list() Toggle between ON and OFF
 */
void 
prompt_on_off (row, col, var, help_text, prompt_text)
	int row;
	int col;
	int *var;
	char *help_text;
	char *prompt_text;
{
	int ret;

	ret = prompt_list (row, col, *var, help_text, prompt_text, txt_onoff, 2);
	*var = ret;
}

/*
 * Wait until a key is pressed. Pedants will point out that:
 * i)  There is no 'any' key on a keyboard
 * ii) CTRL, SHIFT etc don't work
 */
void 
continue_prompt ()
{
	set_alarm_clock_off ();
	
	info_message (txt_hit_any_key);
	(void) ReadCh ();

	set_alarm_clock_on ();
}
