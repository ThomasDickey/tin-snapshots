/*
 *  Project   : tin - a Usenet reader
 *  Module    : feed.c
 *  Author    : I. Lea
 *  Created   : 1991-08-31
 *  Updated   : 1994-12-22
 *  Notes     : provides same interface to mail,pipe,print,save & repost commands
 *  Copyright : (c) Copyright 1991-99 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

#ifdef DEBUG
#	include	"tcurses.h"
#endif /* DEBUG */

#include	"menukeys.h"

char proc_ch_default;				/* set in change_config_file () */

#ifndef INDEX_DAEMON
t_bool do_rfc1521_decoding = FALSE; /* needed for postprocessing saved arts */

/*
 * Local prototypes
 */
static t_bool does_article_exist (int function, struct t_article *art, char *path);
#ifndef DISABLE_PRINTING
	static t_bool print_file (char *command, int respnum, int count);
#endif /* !DISABLE_PRINTING */

void
feed_articles (
	int function,
	int level,
	struct t_group *group,
	int respnum)
{
	char filename[PATH_LEN], *p;
	char group_path[PATH_LEN];
	char my_mailbox[PATH_LEN];
	char save_file[PATH_LEN];
	char proc_ch = proc_ch_default;
	char option = iKeyFeedSupersede;
	char option_default = iKeyFeedSupersede;
	constext *prompt;
	int ch, ch_default;
	int b, i, j;
	int orig_note_page = 0;
	int processed = 0;
	t_bool confirm;
	t_bool is_mailbox = FALSE;
	t_bool orig_note_end = FALSE;
	t_bool proceed;
	t_bool processed_ok = TRUE;
	t_bool redraw_screen = FALSE;
	t_bool ret1 = FALSE;
	t_bool ret2 = FALSE;
	t_bool supersede = FALSE;
#	ifndef DISABLE_PRINTING
	char print_command[LEN];
#	endif /* !DISABLE_PRINTING */
#	ifndef DONT_HAVE_PIPING
	FILE *fp = (FILE *) 0;
#	endif /* !DONT_HAVE_PIPING */
#	ifndef FORGERY
	char from_name[PATH_LEN];
	char user_name[128];
	char full_name[128];
#	endif /* !FORGERY */

#	ifdef DONT_HAVE_PIPING
	if (function == FEED_PIPE) {
		error_message (txt_piping_not_enabled);
		clear_message ();
		return;
	}
#	endif /* DONT_HAVE_PIPING */

	set_xclick_off ();

	/* whilst in the pager, remember current possition in article */
	if (level == PAGE_LEVEL) {
		orig_note_end = note_end;
		orig_note_page = note_page;
	}

	b = which_thread (respnum);

	/*
	 * try and work out what default the user wants
	 * This is dumb. If you have _any_ selected arts, then it picks 'h'
	 */
	ch_default = (num_of_tagged_arts ? iKeyFeedTag : (num_of_selected_arts ? iKeyFeedHot : ((HAS_FOLLOWUPS (b)) ? iKeyFeedThd : iKeyFeedArt)));

	switch (function) {
		case FEED_MAIL:
			prompt = txt_mail;
			break;
#	ifndef DONT_HAVE_PIPING
		case FEED_PIPE:
			prompt = txt_pipe;
			break;
#	endif /* !DONT_HAVE_PIPING */
#	ifndef DISABLE_PRINTING
		case FEED_PRINT:
			prompt = txt_print;
			break;
#	endif /* !DISABLE_PRINTING */
		case FEED_SAVE:
			prompt = txt_save;
			break;
		case FEED_REPOST:
			prompt = txt_repost;
			break;
		default:
			prompt = "";
			break;
	}

	make_group_path (group->name, group_path);
	filename[0] = '\0';

	/*
	 * If not automatic, ask what the user wants to save
	 */
	if (((!tinrc.auto_save || arts[respnum].archive == (char *) 0) || (tinrc.auto_save && function != FEED_SAVE) || ch_default == iKeyFeedTag) && function != FEED_AUTOSAVE_TAGGED)
		ch = prompt_slk_response (ch_default, "ahpqtT\033", "%s%s", prompt, txt_art_thread_regex_tag);
	else {
		filename[0] = '\0';
		ch = ch_default;
	}

	switch (ch) {
		case iKeyQuit:
		case iKeyAbort:
			clear_message ();
			return;

		case iKeyFeedPat:
			sprintf (mesg, txt_feed_pattern, tinrc.default_regex_pattern);
			if (!(prompt_string_default(mesg, tinrc.default_regex_pattern, txt_no_match, HIST_REGEX_PATTERN))) {
				clear_message();
				return;
			}
			break;

		default:
			break;
	}

	switch (function) {
		case FEED_MAIL:
			sprintf (mesg, txt_mail_art_to, cCOLS-(strlen(txt_mail_art_to)+30), tinrc.default_mail_address);
			if (!(prompt_string_default(mesg, tinrc.default_mail_address, txt_no_mail_address, HIST_MAIL_ADDRESS))) {
				clear_message();
				return;
			}
			break;

#	ifndef DONT_HAVE_PIPING
		case FEED_PIPE:
			sprintf (mesg, txt_pipe_to_command, cCOLS-(strlen(txt_pipe_to_command)+30), tinrc.default_pipe_command);
			if (!(prompt_string_default (mesg, tinrc.default_pipe_command, txt_no_command, HIST_PIPE_COMMAND))) {
				clear_message ();
				return;
			}

			got_sig_pipe = FALSE;
			if ((fp = popen (tinrc.default_pipe_command, "w")) == (FILE *) 0) {
				perror_message (txt_command_failed, tinrc.default_pipe_command);
				return;
			}
			wait_message (0, txt_piping);
			EndWin();
			break;
#	endif /* !DONT_HAVE_PIPING */

#	ifndef DISABLE_PRINTING
		case FEED_PRINT:
			sprintf (print_command, "%s %s", group->attribute->printer, REDIRECT_OUTPUT);
			break;
#	endif /* !DISABLE_PRINTING */

		case FEED_SAVE:		/* ask user for filename to save to */
		case FEED_AUTOSAVE_TAGGED:
			free_save_array ();
			if ((!tinrc.auto_save || arts[respnum].archive == (char *) 0)) {
				strcpy (save_file, ((group->attribute->savefile != (char *) 0) ? group->attribute->savefile : tinrc.default_save_file));

				if (function != FEED_AUTOSAVE_TAGGED) {
					sprintf (mesg, txt_save_filename, save_file);

					if (!prompt_string (mesg, filename, HIST_SAVE_FILE)) {
						clear_message ();
						return;
					}
					strip_line (filename);
				}
				if (*filename) {
					if (group->attribute->savefile != (char *) 0) {
						free (group->attribute->savefile);
						group->attribute->savefile = my_strdup (filename);
					}
					my_strncpy (tinrc.default_save_file, filename, sizeof (tinrc.default_save_file));
				} else {
					if (*save_file)
						my_strncpy (filename, save_file, sizeof (filename));
					else {
						info_message (txt_no_filename);
						return;
					}
				}
				for (p = filename; *p && (*p == ' ' || *p == '\t'); p++)
					continue;

				if (!*p) {
					info_message (txt_no_filename);
					return;
				}
				if ((filename[0] == '~' || filename[0] == '+') && strlen (filename) == 1) {
					info_message (txt_no_filename);
					return;
				}
				if (function != FEED_AUTOSAVE_TAGGED) {
					is_mailbox = create_path (filename);
					if (is_mailbox) {
						if ((int) strlen (filename) > 1)
							my_strncpy (my_mailbox, filename+1, sizeof (my_mailbox));
						else
							my_strncpy (my_mailbox, group->name, sizeof (my_mailbox));
						my_strncpy (filename, my_mailbox, sizeof (filename));
					} else {		/* ask for post processing type */
						proc_ch = (char) prompt_slk_response(proc_ch_default, "eElLnqsu\033", txt_choose_post_process_type);
						if (proc_ch == iKeyQuit || proc_ch == iKeyAbort) { /* exit */
							clear_message ();
							return;
						}
						/* FIXME, ugly hack */
						/* check if rfc1521 decoding is needed */
						if (proc_ch == 'n')
							do_rfc1521_decoding = FALSE;
						else
							do_rfc1521_decoding = TRUE;
					}
				}
			}
			wait_message (0, txt_saving);
			break;
		case FEED_REPOST:	/* repost article */
			if (!can_post)
				info_message(txt_cannot_post);
			else {
#	ifndef FORGERY
				get_user_info (user_name, full_name);
				get_from_name (from_name, (struct t_group *) 0);
				if (strstr (from_name, arts[respnum].from)) {
#	endif /* !FORGERY */
					/* repost or supersede ? */
					option = (char) prompt_slk_response (option_default, "\033qrs", sized_message(txt_supersede_article, arts[respnum].subject));

					switch (option) {
						case iKeyFeedSupersede:
							sprintf (mesg, txt_supersede_group, tinrc.default_repost_group);
							supersede = TRUE;
							break;
						case iKeyFeedRepost:
							sprintf (mesg, txt_repost_group, tinrc.default_repost_group);
							supersede = FALSE;
							break;
						default:
							clear_message ();
							return;
					}
#	ifndef FORGERY
				} else {
					sprintf (mesg, txt_repost_group, tinrc.default_repost_group);
					supersede = FALSE;
				}
#	endif /* !FORGERY */
				if (!(prompt_string_default (mesg, tinrc.default_repost_group, txt_no_group, HIST_REPOST_GROUP))) {
					clear_message ();
					return;
				}
			}
			break;

		default:
			break;
	} /* switch (function) */

	switch (ch) {
		case iKeyFeedArt:		/* article */
			if (level == GROUP_LEVEL || level == THREAD_LEVEL) {
				if (!does_article_exist (function, &arts[respnum], group_path))
					break;
			}
			switch (function) {
				case FEED_MAIL:
					redraw_screen = mail_to_someone (respnum, tinrc.default_mail_address, FALSE, TRUE, &processed_ok);
					break;

#	ifndef DONT_HAVE_PIPING
				case FEED_PIPE:
					if (art_open (&arts[respnum], group_path, FALSE) == 0) {
						fseek (note_fp, 0L, SEEK_SET);
						if (got_sig_pipe)
							goto got_sig_pipe_while_piping;
						copy_fp (note_fp, fp);
					}
					break;
#	endif /* !DONT_HAVE_PIPING */

#	ifndef DISABLE_PRINTING
				case FEED_PRINT:
					processed_ok = print_file (print_command, respnum, 1);
					break;
#	endif /* !DISABLE_PRINTING */

				case FEED_SAVE:
					if (art_open (&arts[respnum], group_path, do_rfc1521_decoding) == 0) {
						add_to_save_list (respnum, is_mailbox, TRUE, filename);
						processed_ok = save_art_to_file (0, FALSE, "");
					}
					break;

				case FEED_REPOST:
					if (can_post)
						redraw_screen = repost_article (tinrc.default_repost_group, respnum, supersede);
					else
						info_message (txt_cannot_post);
					break;

				default:
					break;
			}

			if (processed_ok)
				processed++;
			else
				break;

			if (tinrc.mark_saved_read && processed_ok)
				art_mark_read (group, &arts[respnum]);

			if (level == GROUP_LEVEL || level == THREAD_LEVEL)
				art_close ();

			break;

		case iKeyFeedThd:		/* thread */
			confirm = TRUE;
			for (i = (int) base[b]; i >= 0; i = arts[i].thread) {
				if (level == PAGE_LEVEL)
					art_close ();

				if (!does_article_exist (function, &arts[i], group_path))
					continue;

				if (tinrc.process_only_unread && arts[i].status == ART_READ)
					continue;

				switch (function) {
					case FEED_MAIL:
						processed_ok = TRUE;
						mail_to_someone (respnum, tinrc.default_mail_address, FALSE, confirm, &processed_ok);
						confirm = bool_not(processed_ok);
						break;

#	ifndef DONT_HAVE_PIPING
					case FEED_PIPE:
						if (art_open (&arts[i], group_path, FALSE))
							break;
						if (got_sig_pipe)
							goto got_sig_pipe_while_piping;
						fseek (note_fp, 0L, SEEK_SET);
						copy_fp (note_fp, fp);
						break;
#	endif /* !DONT_HAVE_PIPING */

#	ifndef DISABLE_PRINTING
					case FEED_PRINT:
						processed_ok = print_file (print_command, i, processed+1);
						break;
#	endif /* !DISABLE_PRINTING */

					case FEED_SAVE:
						add_to_save_list (i, is_mailbox, TRUE, filename);
						break;

					case FEED_REPOST:
						if (can_post)
							redraw_screen = repost_article (tinrc.default_repost_group, i, supersede);
						else
							info_message (txt_cannot_post);
						break;

					default:
						break;
				}
				if (processed_ok)
					processed++;

				if (tinrc.mark_saved_read && processed_ok)
					art_mark_read (group, &arts[i]);

				art_close ();
			}
			if (function == FEED_SAVE) {
				sort_save_list ();
				processed_ok = save_thread_to_file (is_mailbox, group_path);
			}
			break;

		case iKeyFeedTag:		/* tagged articles */
			confirm = TRUE;
			for (i = 1; i <= num_of_tagged_arts; i++) {
				for (j = 0; j < top; j++) {
					if (arts[j].tagged && arts[j].tagged == i) {
						if (level == PAGE_LEVEL)
							art_close ();

						if (!does_article_exist (function, &arts[j], group_path))
							continue;

						switch (function) {
							case FEED_MAIL:
								processed_ok = TRUE;
								mail_to_someone (respnum, tinrc.default_mail_address, FALSE, confirm, &processed_ok);
								confirm = bool_not(processed_ok);
								break;

#	ifndef DONT_HAVE_PIPING
							case FEED_PIPE:
								wait_message (0, "%s%4d/%-4d ", txt_piping, i, num_of_tagged_arts);
								if (art_open (&arts[j], group_path, FALSE))
									break;
								if (got_sig_pipe)
									goto got_sig_pipe_while_piping;
								fseek (note_fp, 0L, SEEK_SET);
								copy_fp (note_fp, fp);
								break;
#	endif /* !DONT_HAVE_PIPING */

#	ifndef DISABLE_PRINTING
							case FEED_PRINT:
								processed_ok = print_file (print_command, j, processed+1);
								break;
#	endif /* !DISABLE_PRINTING */

							case FEED_SAVE:
							case FEED_AUTOSAVE_TAGGED:
								add_to_save_list (j, is_mailbox, TRUE, filename);
								break;

							case FEED_REPOST:
								if (can_post)
									redraw_screen = repost_article (tinrc.default_repost_group, j, supersede);
								else
									info_message (txt_cannot_post);
								break;

							default:
								break;
						}
						if (processed_ok)
							processed++;

						if (tinrc.mark_saved_read && processed_ok)
							art_mark_read (group, &arts[j]);

						art_close ();
					}
				}
			}
			if (function == FEED_SAVE || function == FEED_AUTOSAVE_TAGGED)
				processed_ok = save_regex_arts_to_file (is_mailbox, group_path);

			untag_all_articles ();
			break;

		case iKeyFeedHot:		/* hot (auto-selected) articles */
		case iKeyFeedPat:		/* regex pattern matched articles */
			confirm = TRUE;
			for (i = 0; i < top_base; i++) {
				for (j = (int) base[i]; j >= 0; j = arts[j].thread) {
					proceed = FALSE;
					if (ch == iKeyFeedPat) {
						if (REGEX_MATCH(arts[j].subject, tinrc.default_regex_pattern, TRUE))
							proceed = TRUE;
					} else
						if (arts[j].selected)
							proceed = TRUE;

					if (proceed) {
						if (level == PAGE_LEVEL)
							art_close ();
						if (!does_article_exist (function, &arts[j], group_path))
							continue;
						if (tinrc.process_only_unread && arts[j].status == ART_READ)
							continue;

						switch (function) {

							case FEED_MAIL:
								processed_ok = TRUE;
								mail_to_someone (respnum, tinrc.default_mail_address, FALSE, confirm, &processed_ok);
								confirm = bool_not(processed_ok);
								break;

#	ifndef DONT_HAVE_PIPING
							case FEED_PIPE:
								if (art_open (&arts[j], group_path, FALSE))
									break;
								if (got_sig_pipe)
									goto got_sig_pipe_while_piping;
								fseek (note_fp, 0L, SEEK_SET);
								copy_fp (note_fp, fp);
								break;
#	endif /* !DONT_HAVE_PIPING */

#  ifndef DISABLE_PRINTING
							case FEED_PRINT:
								processed_ok = print_file (print_command, j, processed+1);
								break;
#	endif /* !DISABLE_PRINTING */

							case FEED_SAVE:
								add_to_save_list (j, is_mailbox, TRUE, filename);
								break;

							case FEED_REPOST:
								if (can_post)
									redraw_screen = repost_article (tinrc.default_repost_group, j, supersede);
								else
									info_message (txt_cannot_post);
								break;

							default:
								break;
						}
						if (processed_ok)
							processed++;

						if (tinrc.mark_saved_read && processed_ok) {
							art_mark_read (group, &arts[j]);
							if (ch == iKeyFeedHot) {
								arts[j].selected = FALSE;
								num_of_selected_arts--;
							}
						}
						art_close ();
					}
				}
			}
			if (function == FEED_SAVE)
				processed_ok = save_regex_arts_to_file (is_mailbox, group_path);
			break;
		default:
			break;
	} /* switch (ch) */

	/*
	 * Now work out what (if anything) needs to be redrawn
	 */
#	ifdef DEBUG
	if (debug == 2) {
		my_printf ("REDRAW=[%d]  ", redraw_screen);
		my_flush ();
	}
#	endif /* DEBUG */
	if (!tinrc.use_mailreader_i)
		redraw_screen = mail_check ();	/* in case of sending to oneself */
#	ifdef DEBUG
	if (debug == 2) {
		my_printf ("REDRAW=[%d]", redraw_screen);
		my_flush ();
		(void) sleep (2);
	}
#	endif /* DEBUG */

	switch (function) {
#	ifndef DONT_HAVE_PIPING
		case FEED_PIPE:
got_sig_pipe_while_piping:
			got_sig_pipe = FALSE;
			(void) pclose (fp);
			Raw (TRUE);
			continue_prompt ();
			redraw_screen = TRUE;
			break;
#	endif /* !DONT_HAVE_PIPING */

		case FEED_SAVE:
		case FEED_AUTOSAVE_TAGGED:
			if (proc_ch != 'n' && !is_mailbox && processed_ok)
				ret2 = post_process_files (proc_ch, (t_bool) (function == FEED_SAVE ? FALSE : TRUE));
			free_save_array ();
			break;
		default:
			break;
	}

	if (level == GROUP_LEVEL || level == THREAD_LEVEL)
		ret1 = tinrc.mark_saved_read;

	if (ret1 || ret2)
		redraw_screen = TRUE;

	if (level == PAGE_LEVEL) {
		if (art_open (&arts[respnum], group_path, TRUE) != 0)
			return;			/* This is bad */
		if (tinrc.force_screen_redraw)
			redraw_screen = TRUE;
		note_end = orig_note_end;
		note_page = orig_note_page;
		fseek (note_fp, note_mark[note_page], SEEK_SET);
		if (redraw_screen) {
			if (!note_page)
				show_note_page (group->name, respnum);
			else
				redraw_page (group->name, respnum);
		} else {
			if (function == FEED_PIPE)
				clear_message ();
		}
	} else {
		if (redraw_screen) {
			switch (level) {
				case GROUP_LEVEL:
					show_group_page ();
					break;
				case THREAD_LEVEL:
					show_thread_page ();
					break;
				default:
					break;
			}
		}
	}

	switch (function) {
		case FEED_MAIL:
			if (tinrc.use_mailreader_i)
				info_message (txt_external_mail_done);
			else
				info_message (txt_mailed, processed, IS_PLURAL(processed));
			break;

#	ifndef DISABLE_PRINTING
		case FEED_PRINT:
			info_message (txt_printed, processed, IS_PLURAL(processed));
			break;
#	endif /* !DISABLE_PRINTING */

		case FEED_SAVE:
		case FEED_AUTOSAVE_TAGGED:
			if (ch == iKeyFeedArt)
				info_message (txt_saved_arts, processed, IS_PLURAL(processed));
			break;
		default:
			break;
	}
}

#	ifndef DISABLE_PRINTING
static t_bool
print_file (
	char *command,
	int respnum,
	int count)
{
	FILE *fp;
#		ifdef DONT_HAVE_PIPING
	char cmd[255], file[255];

	strcpy(cmd, command);
#		endif /* DONT_HAVE_PIPING */

	wait_message (1, "%s%d", txt_printing, count);

#		ifdef DONT_HAVE_PIPING
	sprintf(file, TIN_PRINTFILE, respnum);
	if ((fp = fopen(file,"w")) == (FILE *) 0)
#		else
	if ((fp = popen (command, "w")) == (FILE *) 0)
#		endif /* DONT_HAVE_PIPING */
	{
		perror_message (txt_command_failed, command);
		return FALSE;
	}

	if (tinrc.print_header)
		fseek(note_fp, 0L, SEEK_SET);
	else {
		fprintf (fp, "Newsgroups: %s\n", note_h.newsgroups);
		if (arts[respnum].from == arts[respnum].name || arts[respnum].name == (char *) 0)
			fprintf (fp, "From: %s\n", arts[respnum].from);
		else
			fprintf (fp, "From: %s <%s>\n", arts[respnum].name, arts[respnum].from);
		fprintf (fp, "Subject: %s\n", note_h.subj);
		fprintf (fp, "Date: %s\n\n", note_h.date);
		fseek (note_fp, mark_body, SEEK_SET);
	}
	copy_fp (note_fp, fp);

#		ifdef DONT_HAVE_PIPING
	fclose(fp);
	strcat(cmd, " ");
	strcat(cmd, file);
	system(cmd);
	unlink(file);
#		else
	pclose (fp);
#		endif /* DONT_HAVE_PIPING */

	return TRUE;	/* a hack that will check if file was really checked later */
}
#	endif /* !DISABLE_PRINTING */


/*
 * Opening an article here & also later in the save
 * routine is a real performance (bandwidth) killer
 * as both times the art will be transfered (Ouch!)
 *
 * So if function is to save an article only stat
 * it the first time which saves a lot and almost
 * gets us the elusive free lunch!
 */
static t_bool
does_article_exist (
	int function,
	struct t_article *art,
	char *path)
{
	if (function == FEED_SAVE || function == FEED_PIPE) {
		if (stat_article (art->artnum, path))
			return TRUE;
	} else {
		if (art_open (art, path, TRUE) != ART_UNAVAILABLE)
			return TRUE;		/* Even if the user aborted, the art still exists */
	}

	return FALSE;
}
#endif /* !INDEX_DAEMON */


/*
 * Return the single char hotkey corresponding to the post process type
 * No range check here, it was constrained when the config file was read
 */
char
get_post_proc_type (
	int proc_type)
{
	return(ch_post_process[proc_type]);
}
