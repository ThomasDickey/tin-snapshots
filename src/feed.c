/*
 *  Project   : tin - a Usenet reader
 *  Module    : feed.c
 *  Author    : I.Lea
 *  Created   : 31-08-91
 *  Updated   : 22-12-94
 *  Notes     : provides same interface to mail,pipe,print and save commands
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"menukeys.h"

char default_mail_address[LEN];
char default_pipe_command[LEN];
char default_save_file[PATH_LEN];
char default_regex_pattern[LEN];
char default_repost_group[LEN];
char proc_ch_default;			/* set in change_config_file () */


void
feed_articles (function, level, group, respnum)
	int function;
	int level;
	struct t_group *group;
	int respnum;
{
#ifndef INDEX_DAEMON

	char address[LEN];
	char command[LEN];
	char filename[PATH_LEN], *p;
	char group_path[PATH_LEN];
	char group_name[LEN];
	char my_mailbox[PATH_LEN];
	char pattern[LEN];
	char save_file[PATH_LEN];
	char *prompt;
	char proc_ch = proc_ch_default;
	char option = iKeyFeedSupersede;
	char option_default = iKeyFeedSupersede;
	FILE *fp = (FILE *) 0;
	int ch, ch_default;
	int b, i, j;
	int confirm;
	int proceed;
	int processed_ok = TRUE;
	int is_mailbox = FALSE;
	int orig_note_end = 0;
	int orig_note_page = 0;
	int processed = 0;
	int redraw_screen = FALSE;
	int ret1 = FALSE;
	int ret2 = FALSE;
/*	int retcode; */
	int supersede = FALSE;
#ifndef FORGERY
	char from_name[PATH_LEN];
	char host_name[PATH_LEN];
	char user_name[128];
	char full_name[128];
#endif

#ifdef DONT_HAVE_PIPING
	if (function == FEED_PIPE) {
		error_message (txt_piping_not_enabled, "");
		clear_message ();
		return;
	}
#endif

	set_xclick_off ();
	if (level == PAGE_LEVEL) {
		orig_note_end = note_end;
		orig_note_page = note_page;
	}

	b = which_thread (respnum);

	/*
	 * try and work out what default the user wants
	 */
	if (num_of_tagged_arts) {
		ch_default = iKeyFeedTag;
	} else if (num_of_selected_arts) {
		ch_default = iKeyFeedHot;
	} else if (num_of_responses (b)) {
		ch_default = iKeyFeedThd;
	} else {
		ch_default = iKeyFeedArt;
	}

	switch (function) {
		case FEED_MAIL:
			prompt = "Mail";
			break;
		case FEED_PIPE:
			prompt = "Pipe";
			break;
		case FEED_PRINT:
			prompt = "Print";
			break;
		case FEED_SAVE:
			prompt = "Save";
			break;
		case FEED_REPOST:
			prompt = "Repost";
			break;
		default:
			prompt = "";
	}

	make_group_path (group->name, group_path);
	filename[0] = '\0';

	if (((default_auto_save == FALSE || arts[respnum].archive == (char *) 0) ||
		(default_auto_save == TRUE && function != FEED_SAVE) ||
		ch_default == iKeyFeedTag) && function != FEED_SAVE_TAGGED) {
		do {
			sprintf (msg, "%s%s%c", prompt, txt_art_thread_regex_tag, ch_default);
			wait_message (msg);
			MoveCursor (cLINES, (int) strlen (msg)-1);
			if ((ch = ReadCh ()) == '\r' || ch == '\n') {
				ch = ch_default;
			}
		} while (! strchr ("ahpqtT\033", ch));
	} else {
		filename[0] = '\0';
		ch = ch_default;
	}

	switch (ch) {
		case iKeyQuit:
		case iKeyAbort:
			clear_message ();
			return;

		case iKeyFeedPat:
			sprintf (msg, txt_feed_pattern, default_regex_pattern);
			if (! prompt_string (msg, pattern)) {
				clear_message ();
				return;
			}
			if (strlen (pattern)) {
				my_strncpy (default_regex_pattern, pattern,
					sizeof (default_regex_pattern));
			} else {
				if (default_regex_pattern[0]) {
					my_strncpy (pattern, default_regex_pattern,
						sizeof (default_regex_pattern));
				} else {
					info_message (txt_no_match);
					return;
				}
			}
	}

	switch (function) {
		case FEED_MAIL:
			sprintf (msg, txt_mail_art_to,
				cCOLS-(strlen(txt_mail_art_to)+30), default_mail_address);
			if (! prompt_string (msg, address)) {
				clear_message ();
				return;
			}
			if (strlen (address)) {
				strcpy (default_mail_address, address);
			} else {
				if (default_mail_address[0]) {
					strcpy (address, default_mail_address);
				} else {
					info_message (txt_no_mail_address);
					return;
				}
			}
			break;
		case FEED_PIPE:
			sprintf (msg, txt_pipe_to_command,
				cCOLS-(strlen(txt_pipe_to_command)+30), default_pipe_command);
			if (! prompt_string (msg, command)) {
				clear_message ();
				return;
			}
			if (strlen (command)) {
				strcpy (default_pipe_command, command);
			} else {
				if (default_pipe_command[0]) {
					strcpy (command, default_pipe_command);
				} else {
					info_message (txt_no_command);
					return;
				}
			}

			got_sig_pipe = FALSE;
			if ((fp = popen (command, "w")) == NULL) {
				perror_message (txt_command_failed_s, command);
				return;
			}
			wait_message (txt_piping);
			Raw (FALSE);
			break;
		case FEED_PRINT:
			if (cmd_line_printer[0]) {
				sprintf (command, "%s %s",
					cmd_line_printer, REDIRECT_OUTPUT);
			} else {
				sprintf (command, "%s %s", group->attribute->printer, REDIRECT_OUTPUT);
			}
			break;
		case FEED_SAVE:		/* ask user for filename */
		case FEED_SAVE_TAGGED:
			free_save_array ();
			if ((default_auto_save == FALSE ||
				arts[respnum].archive == (char *) 0)) {

				if (group->attribute->savefile != (char *) 0) {
					strcpy (save_file, group->attribute->savefile);
				} else {
					strcpy (save_file, default_save_file);
				}

				if (function != FEED_SAVE_TAGGED) {
					sprintf (msg, txt_save_filename, save_file);

					if (! prompt_string (msg, filename)) {
						clear_message ();
						return;
					}
				}
				if (strlen (filename)) {
					if (group->attribute->savefile != (char *) 0) {
						free (group->attribute->savefile);
						group->attribute->savefile = str_dup (filename);
					} else {
						strcpy (default_save_file, filename);
					}

					my_strncpy (default_save_file, filename,
						sizeof (default_save_file));
				} else {
					if (save_file[0]) {
						my_strncpy (filename, save_file, sizeof (filename));
					} else {
						info_message (txt_no_filename);
						return;
					}
				}
				for (p = filename; *p && (*p == ' ' || *p == '\t'); p++) {
					continue;
				}
				if (! *p) {
					info_message (txt_no_filename);
					return;
				}
				if ((filename[0] == '~' || filename[0] == '+') && strlen (filename) == 1) {
					info_message (txt_no_filename);
					return;
				}
				if (function != FEED_SAVE_TAGGED) {
					is_mailbox = create_path (filename);
					if (is_mailbox) {
						if ((int) strlen (filename) > 1) {
							my_strncpy (my_mailbox, filename+1, sizeof (my_mailbox));
						} else {
							my_strncpy (my_mailbox, group->name, sizeof (my_mailbox));
							/*
							 *  convert 1st letter to uppercase
							 */
							if (my_mailbox[0] >= iKeyFeedArt && my_mailbox[0] <= 'z') {
								my_mailbox[0] = my_mailbox[0] - 32;
							}
						}
						my_strncpy (filename, my_mailbox, sizeof (filename));
					} else {		/* ask for post processing type */
						do {
							sprintf (msg, "%s%c", txt_post_process_type, proc_ch_default);
							wait_message (msg);
							MoveCursor (cLINES, (int) strlen (msg)-1);
							if ((proc_ch = ReadCh ()) == '\n' || proc_ch == '\r')
								proc_ch = proc_ch_default;
						} while (! strchr ("eElLnqsu\033", proc_ch));
						if (proc_ch == iKeyQuit || proc_ch == iKeyAbort) { /* exit */
							clear_message ();
							return;
						}
					}
				}
			}
			wait_message (txt_saving);
			break;
		case FEED_REPOST:	/* repost article */
#ifndef FORGERY
			get_host_name (host_name);
			get_user_info (user_name, full_name);
			get_from_name (user_name, host_name, full_name, from_name);

			if (str_str (from_name, arts[respnum].from, strlen (arts[respnum].from))) {
#endif
				/* repost or supersede ? */
				do {
					sprintf (msg,txt_supersede_article, arts[respnum].subject, option_default);
					wait_message (msg);
					MoveCursor (cLINES, (int) (strlen (msg)-1));
					if ((option = (char) ReadCh ()) == '\r' || option == '\n')
						option = option_default;
				} while (! strchr ("qrs\033", option));

				switch (option) {
					case iKeyFeedSupersede:
						sprintf (msg, txt_supersede_group, default_repost_group);
						supersede = TRUE;
						/*
						 * FIXME!
						 * don't ask q)uit, e)dit, p)ost [subject]:
						 * but open editor emediately
						 */
						break;
					case iKeyFeedRepost:
						sprintf (msg, txt_repost_group, default_repost_group);
						supersede = FALSE;
						break;
					default:
						clear_message ();
						return;
				}
#ifndef FORGERY
			} else {
				sprintf (msg, txt_repost_group, default_repost_group);
				supersede = FALSE;
			}
#endif

			if (! prompt_string (msg, group_name)) {
				clear_message ();
				return;
			}

			if (strlen (group_name)) {
				my_strncpy (default_repost_group, group_name,
					sizeof (default_repost_group));
			} else {
				if (default_repost_group[0]) {
					my_strncpy (group_name, default_repost_group,
						sizeof (group_name));
				} else {
					info_message (txt_no_group);
					return;
				}
			}
			break;
	}

	switch (ch) {
		case iKeyFeedArt:		/* article */
			if (level == GROUP_LEVEL) {
				if (! does_article_exist (function, arts[respnum].artnum, group_path)) {
					break;
				}
			}
			switch (function) {
				case FEED_MAIL:
					redraw_screen = mail_to_someone (respnum, address, FALSE, TRUE, &processed_ok);
					break;
				case FEED_PIPE:
					fseek (note_fp, 0L, 0);
					if (got_sig_pipe) goto got_sig_pipe_while_piping;
					copy_fp (note_fp, fp, "");
					break;
				case FEED_PRINT:
					processed_ok = print_file (command, respnum, 1);
					break;
				case FEED_SAVE:
					note_page = art_open (arts[respnum].artnum, group_path);
					if (note_page != ART_UNAVAILABLE) {
						add_to_save_list (0, &arts[respnum], is_mailbox, TRUE, filename);
						processed_ok = save_art_to_file (respnum, 0, FALSE, "");
					}
					break;
				case FEED_REPOST:
					redraw_screen = repost_article (group_name, &arts[respnum], respnum, supersede);
					break;
			}
			if (processed_ok) {
				processed++;
			} else {
				break;
			}
			if (mark_saved_read) {
				if (processed_ok) {
					art_mark_read (group, &arts[respnum]);
				}
			}
			if (level == GROUP_LEVEL) {
				art_close ();
			}
			break;

		case iKeyFeedThd: 		/* thread */
			confirm = TRUE;
			for (i = (int) base[b]; i >= 0; i = arts[i].thread) {
				if (level == PAGE_LEVEL) {
					art_close ();
				}
				if (! does_article_exist (function, arts[i].artnum, group_path)) {
					continue;
				}
				if (process_only_unread && arts[i].status == ART_READ) {
					continue;
				}
				switch (function) {
					case FEED_MAIL:
						processed_ok = TRUE;
						mail_to_someone (respnum, address, FALSE, confirm, &processed_ok);
						if (processed_ok) {
							confirm = FALSE;
						} else {
							confirm = TRUE;
						}
						break;
					case FEED_PIPE:
   					if (got_sig_pipe) goto got_sig_pipe_while_piping;
						fseek (note_fp, 0L, 0);
						copy_fp (note_fp, fp, "");
						break;
					case FEED_PRINT:
						processed_ok = print_file (command, i, processed+1);
						break;
					case FEED_SAVE:
						add_to_save_list (i, &arts[i], is_mailbox, TRUE, filename);
						break;
					case FEED_REPOST:
						redraw_screen = repost_article (group_name, &arts[i], i, supersede);
						break;
				}
				if (processed_ok) {
					processed++;
				}
				if (mark_saved_read) {
					if (processed_ok) {
						art_mark_read (group, &arts[i]);
					}
				}
				art_close ();
			}
			if (function == FEED_SAVE) {
				sort_save_list ();
				(void) save_thread_to_file (is_mailbox, group_path);
			}
			break;

		case iKeyFeedTag: 		/* tagged articles */
			confirm = TRUE;
			for (i=1 ; i <= num_of_tagged_arts ; i++) {
				for (j=0 ; j < top ; j++) {
					if (arts[j].tagged && arts[j].tagged == i) {
						if (level == PAGE_LEVEL) {
							art_close ();
						}
						if (! does_article_exist (function, arts[j].artnum, group_path)) {
							continue;
						}
						switch (function) {
							case FEED_MAIL:
								processed_ok = TRUE;
								mail_to_someone (respnum, address, FALSE, confirm, &processed_ok);
								if (processed_ok) {
									confirm = FALSE;
								} else {
									confirm = TRUE;
								}
								break;
							case FEED_PIPE:
         					if (got_sig_pipe) goto got_sig_pipe_while_piping;
								fseek (note_fp, 0L, 0);
								copy_fp (note_fp, fp, "");
								break;
							case FEED_PRINT:
								processed_ok = print_file (command, j, processed+1);
								break;
							case FEED_SAVE:
							case FEED_SAVE_TAGGED:
								add_to_save_list (j, &arts[j], is_mailbox, TRUE, filename);
								break;
							case FEED_REPOST:
								redraw_screen = repost_article (group_name, &arts[j], j, supersede);
								break;
						}
						if (processed_ok) {
							processed++;
						}
						if (mark_saved_read) {
							if (processed_ok) {
								art_mark_read (group, &arts[j]);
							}
						}
						art_close ();
					}
				}
			}
			if (function == FEED_SAVE || function == FEED_SAVE_TAGGED) {
				(void) save_regex_arts (is_mailbox, group_path);
			}
			untag_all_articles ();
			break;

		case iKeyFeedHot: 		/* hot (auto-selected) articles */
		case iKeyFeedPat: 		/* regex pattern matched articles */
			confirm = TRUE;
			for (i = 0 ; i < top_base ; i++) {
				for (j = (int) base[i]; j >= 0; j = arts[j].thread) {
					proceed = FALSE;
					if (ch == iKeyFeedPat) {
						if (STR_MATCH(arts[j].subject, pattern)) {
							proceed = TRUE;
						}
					} else if (arts[j].selected) {
						proceed = TRUE;
					}
					if (proceed) {
						if (level == PAGE_LEVEL) {
							art_close ();
						}
						if (! does_article_exist (function, arts[j].artnum, group_path)) {
							continue;
						}
						if (process_only_unread && arts[j].status == ART_READ) {
							continue;
						}
						switch (function) {
							case FEED_MAIL:
								processed_ok = TRUE;
								mail_to_someone (respnum, address, FALSE, confirm, &processed_ok);
								if (processed_ok) {
									confirm = FALSE;
								} else {
									confirm = TRUE;
								}
								break;
							case FEED_PIPE:
         					if (got_sig_pipe) goto got_sig_pipe_while_piping;
								fseek (note_fp, 0L, 0);
								copy_fp (note_fp, fp, "");
								break;
							case FEED_PRINT:
								processed_ok = print_file (command, j, processed+1);
								break;
							case FEED_SAVE:
								add_to_save_list (j, &arts[j], is_mailbox, TRUE, filename);
								break;
							case FEED_REPOST:
								redraw_screen = repost_article (group_name, &arts[j], j, supersede);
								break;
						}
						if (processed_ok) {
							processed++;
						}
						if (mark_saved_read) {
							if (processed_ok) {
								art_mark_read (group, &arts[j]);
								if (ch == iKeyFeedHot) {
									arts[j].selected = FALSE;
									num_of_selected_arts--;
								}
							}
						}
	 					art_close ();
					}
				}
			}
			if (function == FEED_SAVE) {
				(void) save_regex_arts (is_mailbox, group_path);
			}
			break;
	}

	if (debug == 2) {
		printf ("REDRAW=[%d]  ", redraw_screen);
		fflush (stdout);
	}

	if( ! use_mailreader_i )
		redraw_screen = mail_check ();	/* in case of sending to oneself */

	if (debug == 2) {
		printf ("REDRAW=[%d]", redraw_screen);
		fflush (stdout);
		sleep (2);
	}

	switch (function) {
		case FEED_PIPE:
got_sig_pipe_while_piping:
			got_sig_pipe = FALSE;
#if defined(SIGCHLD) && !defined(RS6000)
			pclose (fp);
			/* retcode = */ (void) system_status;
#else
			/* retcode = */ (void) pclose (fp);
#endif
			Raw (TRUE);
			continue_prompt ();
			redraw_screen = TRUE;
			break;
		case FEED_SAVE:
		case FEED_SAVE_TAGGED:
			if (proc_ch != 'n' && is_mailbox == FALSE) {
				ret2 = post_process_files (
					proc_ch, (function == FEED_SAVE ? FALSE : TRUE));
			}
			free_save_array ();
			break;
	}

	if (level == GROUP_LEVEL) {
		ret1 = (mark_saved_read ? TRUE : FALSE);
	}
	if ((ret1 || ret2) && is_mailbox == FALSE) {
		redraw_screen = TRUE;
	}

	if (level == PAGE_LEVEL) {
		if (ch != iKeyFeedArt) {
			note_page = art_open (arts[respnum].artnum, group_path);
		} else if (force_screen_redraw) {
			redraw_screen = TRUE;
		}
		note_end = orig_note_end;
		note_page = orig_note_page;
		fseek (note_fp, note_mark[note_page], 0);
		if (redraw_screen) {
			if (note_page == 0) {
				show_note_page (group->name, respnum);
			} else {
				redraw_page (group->name, respnum);
			}
		} else {
			if (function == FEED_PIPE) {
				clear_message ();
			}
		}
	} else {
		if (redraw_screen) {
			show_group_page ();
		}
	}

	if (function == FEED_MAIL) {
		if( use_mailreader_i ) {
			strcpy (msg, txt_external_mail_done);
			info_message (msg);
		} else {
			sprintf (msg, txt_mailed, processed);
			info_message (msg);
		}
	} else if (function == FEED_PRINT) {
		sprintf (msg, txt_printed, processed);
		info_message (msg);
	} else if (function == FEED_SAVE || function == FEED_SAVE_TAGGED) {
		if (ch == iKeyFeedArt) {
			sprintf (msg, txt_saved, processed);
			info_message (msg);
		}
	}

#endif /* INDEX_DAEMON */
}


int
print_file (command, respnum, count)
	char *command;
	int respnum;
	int count;
{
	FILE *fp;
#ifdef DONT_HAVE_PIPING
    char cmd[255],  file[255];
    strcpy(cmd, command);
#endif

	sprintf (msg, "%s%d", txt_printing, count);
	wait_message (msg);

#ifdef DONT_HAVE_PIPING
    sprintf(file, TIN_PRINTFILE, respnum);
	if ((fp = (FILE *) fopen(file,"w")) == NULL) {
#else
	if ((fp = popen (command, "w")) == NULL) {
#endif
		perror_message (txt_command_failed_s, command);
		return FALSE;
	}

	if (print_header) {
		fseek(note_fp, 0L, 0);
	} else {
		fprintf (fp, "Newsgroups: %s\n", note_h_newsgroups);
		if (arts[respnum].from == arts[respnum].name ||
			arts[respnum].name == (char *) 0) {
			fprintf (fp, "From: %s\n", arts[respnum].from);
		} else {
			fprintf (fp, "From: %s <%s>\n",
				arts[respnum].name, arts[respnum].from);
		}
		fprintf (fp, "Subject: %s\n", note_h_subj);
		fprintf (fp, "Date: %s\n\n", note_h_date);
		fseek (note_fp, note_mark[0], 0);
	}
	copy_fp (note_fp, fp, "");

#ifdef DONT_HAVE_PIPING
    fclose(fp);
    strcat(cmd, " ");
    strcat(cmd, file);
    system(cmd);
	unlink(file);
#else
	pclose (fp);
#endif

	return (TRUE);	/* a hack that will check if file was really checked later */
}

/*
 * Return the single char hotkey corresponding to the post process type
 * No range check here, it was constrained when the config file was read
 */
int
get_post_proc_type (proc_type)
	int proc_type;
{
	return(ch_post_process[proc_type]);
}

/*
 * Opening an article here & also later in the save
 * routine is a real performance (bandwidth) killer
 * as both times the art will be transfered (Ouch!)
 *
 * So if function is to save an article only stat
 * it the first time which saves a lot and almost
 * gets us the elusive free lunch!
 */

int
does_article_exist (function, artnum, path)
	int function;
	long artnum;
	char *path;
{
	int retcode = FALSE;

	if (function == FEED_SAVE) {
		if (stat_article (artnum, path)) {
			retcode = TRUE;
		}
	} else {
		note_page = art_open (artnum, path);
		if (note_page != ART_UNAVAILABLE) {
			retcode = TRUE;
		}
	}

	return retcode;
}
