/*
 *  Project   : tin - a Usenet reader
 *  Module    : save.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 22-08-95
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *		You may  freely  copy or  redistribute	this software,
 *		so  long as there is no profit made from its use, sale
 *		trade or  reproduction.  You may not change this copy-
 *		right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"menukeys.h"

#undef  OFF

#define INITIAL		1
#define MIDDLE		2
#define OFF		3
#define END		4

int create_subdir = TRUE;


/*
 * types of archive programs
 */

struct archiver_t {
	char *name;
	char *ext;
	char *test;
	char *list;
	char *extract;
} archiver[] = {
	{ "",		"",		"",		"",		""	},
	{ "",		"",		"",		"",		""	},
	{ "",		"",		"",		"",		""	},
#ifdef M_AMIGA
	{ "lha",	"lha",		"t",		"l",		"x" },
#else
	{ "zoo",	"zoo",		"-test",	"-list",	"-extract" },
#endif
	{ "unzip",	"zip",		"-t",		"-l",		"-o"	},
	{ (char *) 0,	(char *) 0,	(char *) 0,	(char *) 0,	(char *) 0 }
};

static int any_saved_files P_((void));

/*
 *  Check for articles and say how many new/unread in each group.
 *  or
 *  Start if new/unread articles and return first group with new/unread.
 *  or
 *  Save any new articles to savedir and mark arts read and mail user
 *  and inform how many arts in which groups were saved.
 *  or
 *  Mail any new articles to specified user and mark arts read and mail
 *  user and inform how many arts in which groups were mailed.
 */

int
check_start_save_any_news (check_start_save)
	int check_start_save;
{
#ifndef INDEX_DAEMON

	char buf[LEN], logfile[LEN];
	char group_path[PATH_LEN];
	char savefile[PATH_LEN];
	char subject[HEADER_LEN];
	char path[PATH_LEN];
	char *ich;
	FILE *fp;
	FILE *fp_log = (FILE *) 0;
	int i, j, print_group;
	int check_arts;
	int log_opened = TRUE;
	int print_first = TRUE;
	int saved_arts = 0;
/*	int saved_groups = 0; */
	int unread_news = FALSE;
	time_t epoch;
	struct t_group *group;

	switch (check_start_save) {
		case CHECK_ANY_NEWS:
		case START_ANY_NEWS:
			if (verbose) {
				wait_message (txt_checking_for_news);
			}
			break;
		case MAIL_ANY_NEWS:
		case SAVE_ANY_NEWS:
#ifdef VMS
			joinpath (logfile, rcdir, "log.");
			if ((fp_log = fopen (logfile, "w", "fop=cif")) == NULL) {
#else
			sprintf (logfile, "%s/log", rcdir);
			if ((fp_log = fopen (logfile, "w")) == NULL) {
#endif
				perror_message (txt_cannot_open, logfile);
				fp_log = stdout;
				verbose = FALSE;
				log_opened = FALSE;
			}
			time (&epoch);
			fprintf (fp_log, "To: %s\n", userid);
			sprintf (subject, "Subject: NEWS LOG %s", ctime (&epoch));
			/** Remove trailing \n introduced by ctime() **/
			if ((ich=strrchr(subject, '\n'))!=(char *)NULL)
				*ich='\0';
			fprintf (fp_log, "%s\n\n", subject);
			break;
	}

	for (i = 0; i < group_top; i++) {
		group = &active[my_group[i]];
		make_group_path (group->name, group_path);
		if (!index_group (group)) {
			continue;
		}
		print_group = TRUE;
		check_arts = 0;

		for (j = 0; j < top; j++) {
			if (arts[j].status == ART_UNREAD)  {
				switch (check_start_save) {
					case CHECK_ANY_NEWS:
						if (print_first && verbose) {
							my_fputc ('\n', stdout);
							print_first = FALSE;
						}
						check_arts++;
						break;
					case START_ANY_NEWS:
						return i;	/* return first group with unread news */
						/* NOTREACHED */
					case MAIL_ANY_NEWS:
					case SAVE_ANY_NEWS:
						if (print_group) {
							sprintf (buf, "Saved %s...\n", group->name);
							fprintf (fp_log, "%s", buf);
							if (verbose) {
								wait_message (buf);
							}
							print_group = FALSE;
							/* saved_groups++; */
							if (check_start_save == SAVE_ANY_NEWS) {
#ifdef VMS
				sprintf (buf, "[.%s]dummy", group_path);
#else
								sprintf (buf, "%s/dummy", group_path);
#endif
								create_path (buf);
							}
						}

						if (check_start_save == MAIL_ANY_NEWS) {
							sprintf (savefile, "%stin.%d", TMPDIR, process_id);
						} else {
							if (!strfpath (group->attribute->savedir,
							    path, sizeof (path), homedir, (char *) 0,
							    (char *) 0, group->name)) {
								joinpath (path, homedir, DEFAULT_SAVEDIR);
							}
#ifdef VMS
							sprintf (savefile, "%s.%s]%ld", path, group_path, arts[j].artnum);
#else
							sprintf (savefile, "%s/%s/%ld", path, group_path, arts[j].artnum);
#endif
						}

						note_page = art_open (arts[j].artnum, group_path);
						if (note_page == ART_UNAVAILABLE) {
							continue;
						}

#ifdef VMS
						if ((fp = fopen (savefile, "w", "fop=cif")) == (FILE *) 0) {
#else
						if ((fp = fopen (savefile, "w")) == (FILE *) 0) {
#endif
							fprintf (fp_log, txt_cannot_open, savefile);
							if (verbose) {
								perror_message (txt_cannot_open, savefile);
							}
							continue;
						}

						if (check_start_save == MAIL_ANY_NEWS) {
							fprintf (fp, "To: %s\n", mail_news_user);
						}

						sprintf (buf, "[%5ld]  %s\n", arts[j].artnum, arts[j].subject);
						fprintf (fp_log, "%s", buf);
						if (verbose) {
							wait_message (buf);
						}
						fseek (note_fp, 0L, 0);
						copy_fp (note_fp, fp, "");
						art_close ();
						fclose (fp);
						saved_arts++;

						if (check_start_save == MAIL_ANY_NEWS) {
							strfmailer (mailer, arts[j].subject, mail_news_user,
								savefile, buf, sizeof (buf), default_mailer_format);
							if (!invoke_cmd (buf)) {
								error_message (txt_command_failed_s, buf);
							}
							unlink (savefile);
						}
						if (catchup) {
							art_mark_read (group, &arts[j]);
						}
						break;
				}
			}
		}

		if (check_arts) {
			if (verbose) {
				sprintf (buf, "%4d unread articles in %s\n",
					check_arts, group->name);
				wait_message (buf);
			}
			unread_news = TRUE;
		}
	}

	switch (check_start_save) {
		case CHECK_ANY_NEWS:
			if (unread_news) {
				return 2;
			} else {
				if (verbose) {
					wait_message (txt_there_is_no_news);
				}
				return 0;
			}
			/* NOTREACHED */
		case START_ANY_NEWS:
			wait_message (txt_there_is_no_news);
			return -1;
			/* NOTREACHED */
		case MAIL_ANY_NEWS:
		case SAVE_ANY_NEWS:
			sprintf (buf, "\n%s %d article(s) from %d group(s)\n",
				(check_start_save == MAIL_ANY_NEWS ? "Mailed" : "Saved"),
				saved_arts, group_top); /*saved_groups); */
			fprintf (fp_log, "%s", buf);
			if (verbose) {
				wait_message (buf);
			}
			if (log_opened) {
				fclose (fp_log);
				if (verbose) {
					sprintf (buf, "Mailing log to %s\n",
						(check_start_save == MAIL_ANY_NEWS ? mail_news_user : userid));
					wait_message (buf);
				}
				strfmailer (mailer, subject,
					(check_start_save == MAIL_ANY_NEWS ? mail_news_user : userid),
					logfile, buf, sizeof (buf), default_mailer_format);
				if (!invoke_cmd (buf)) {
					error_message (txt_command_failed_s, buf);
				}
			}
			break;
	}

#endif /* INDEX_DAEMON */

	return 0;
}


int
save_art_to_file (respnum, indexnum, the_mailbox, filename)
	int respnum;
	int indexnum;
	int the_mailbox;
	char *filename;
{
#ifndef INDEX_DAEMON

	char *file;
	char buf[LEN];
	char mode[8];
	char save_art_info[LEN];
	FILE *fp;
	int ch, ch_default;
	int is_mailbox = FALSE;
	int i = 0, ret_code = FALSE;
	time_t epoch;
	struct stat st;

	if (strlen (filename)) {
		is_mailbox = the_mailbox;
		i = indexnum;
	}

	file = save_filename (i);
	strcpy (mode, "a+");

	if (!save[i].is_mailbox) {
		if (stat (file, &st) != -1) {
			ch_default = default_save_mode;
			do {
				sprintf (buf, txt_append_overwrite_quit, file);
				sprintf (msg, "%s%c", buf, ch_default);
				wait_message (msg);
				MoveCursor (cLINES, (int) strlen (buf));
				if ((ch = (char) ReadCh ()) == '\n' || ch == '\r')
					ch = ch_default;
			} while (!strchr ("aoq\033", ch));
			switch (ch) {
				case iKeySaveAppendFile:
					strcpy (mode, "a+");
					break;
				case iKeySaveOverwriteFile:
					strcpy (mode, "w");
					break;
				case iKeyAbort:
				case iKeySaveDontSaveFile2:
					save[i].saved = FALSE;
					info_message (txt_art_not_saved);
					sleep (1);
					return ret_code;
			}
			default_save_mode = ch;
		}
	}

	if (debug == 2) {
		sprintf (msg, "Save respnum=[%d] index=[%d] mbox=[%d] filename=[%s] file=[%s] mode=[%s]",
			respnum, indexnum, the_mailbox, filename, file, mode);
		error_message (msg, "");
	}

	if ((fp = fopen (file, mode)) == (FILE *) 0) {
		save[i].saved = FALSE;
		info_message (txt_art_not_saved);
		return ret_code;
	}

 	time (&epoch);
 	fprintf (fp, "From %s %s", note_h_path, ctime (&epoch));

	if (fseek (note_fp, 0L, 0) == -1) {
		perror_message ("fseek() error on [%s]", arts[respnum].subject);
	}
	copy_fp (note_fp, fp, "");

	print_art_seperator_line (fp, the_mailbox);

	fclose (fp);
	fseek (note_fp, note_mark[note_page], 0);

	save[i].saved = TRUE;

	if (filename == (char *) 0) {
		if (is_mailbox) {
			sprintf (save_art_info, txt_saved_to_mailbox, get_first_savefile ());
		} else {
			sprintf (save_art_info, txt_art_saved_to, get_first_savefile ());
		}
		info_message (save_art_info);
	}

#endif /* INDEX_DAEMON */

	return TRUE;
}


int
save_thread_to_file (is_mailbox, group_path)
	int is_mailbox;
	char *group_path;
{
#ifndef INDEX_DAEMON

	char file[PATH_LEN];
	char save_thread_info[LEN];
	char *first_savefile;
	int count = 0;
	int i;

	if (num_save == 0) {
		info_message(txt_saved_nothing);
		sleep(2);
		return FALSE;
	}

	for (i=0 ; i < num_save ; i++) {
		sprintf (msg, "%s%d", txt_saving, ++count);
		wait_message (msg);

		if (is_mailbox) {
			file[0] = 0;
		}else {
			sprintf (file, "%s.%03d", save[i].file, i+1);
		}

		note_page = art_open (arts[save[i].index].artnum, group_path);
		if (note_page != ART_UNAVAILABLE) {
			(void) save_art_to_file (save[i].index, i, is_mailbox, file);
			art_close ();
		}
	}

	first_savefile = get_first_savefile ();

	if (first_savefile == (char *) 0) {
		info_message (txt_thread_not_saved);
	} else {
		if (is_mailbox) {
			sprintf (save_thread_info, txt_saved_to_mailbox, first_savefile);
		} else {
			if (num_save == 1) {
				sprintf (save_thread_info, txt_art_saved_to, first_savefile);
			} else {
				sprintf (save_thread_info, txt_thread_saved_to_many,
					first_savefile, get_last_savefile ());
			}
			if (first_savefile != (char *) 0) {
				free (first_savefile);
			}
		}
		info_message (save_thread_info);
		sleep (2);
	}

#endif /* INDEX_DAEMON */

	return TRUE;
}


int
save_regex_arts (is_mailbox, group_path)
	int is_mailbox;
	char *group_path;
{
#ifndef INDEX_DAEMON

	char buf[PATH_LEN];
	int i, ret_code = FALSE;

	for (i=0 ; i < num_save ; i++) {
		sprintf(msg, "%s%d", txt_saving, i+1);
		wait_message (msg);

		if (is_mailbox) {
			buf[0] = 0;
		}else {
			sprintf (buf, "%s.%03d", save[i].file, i+1);
		}

		note_page = art_open (arts[save[i].index].artnum, group_path);
		if (note_page != ART_UNAVAILABLE) {
			ret_code = save_art_to_file (save[i].index, i, is_mailbox, buf);
			art_close ();
		}
	}

	if (!num_save) {
		info_message (txt_no_match);
	} else {
		if (is_mailbox) {
			sprintf (buf, txt_saved_to_mailbox, get_first_savefile ());
		} else {
			sprintf (buf,txt_saved_pattern_to,
				get_first_savefile (), get_last_savefile ());
		}
		info_message (buf);
	}

	return ret_code;

#else

	return FALSE;

#endif /* INDEX_DAEMON */
}

int
create_path (path)
	char *path;
{
	int mbox_format = FALSE;

#ifndef INDEX_DAEMON

	char tmp[PATH_LEN];
	char buf[PATH_LEN];
	int i, j, len;
	struct stat st;

	i = my_group[cur_groupnum];

	/*
	 * expand "$var..." first, so variables starting with
	 * '+', '$' or '=' will be processed correctly later
	 */
	if (path[0] == '$') {
		if (strfpath (path, buf, sizeof (buf), homedir,
		    (char *) 0, (char *) 0, active[i].name)) {
			my_strncpy (path, buf, PATH_LEN);
		}
	}

	/*
	 * save in mailbox format to ~/Mail/<group.name> or
	 * attribute->maildir for current group
	 */
	if (path[0] == '=') {
		mbox_format = TRUE;
		strcpy (tmp, path);
		if (!strfpath (active[i].attribute->maildir, buf, sizeof (buf),
		    homedir, (char *) 0, (char *) 0, active[i].name)) {
#ifdef VMS
			joindir  (buf, homedir, DEFAULT_MAILDIR);
#else
			joinpath (buf, homedir, DEFAULT_MAILDIR);
#endif
		}
#ifdef VMS
		joinpath (path, buf, "dummy");
#else
		sprintf (path, "%s/dummy", buf);
#endif
	} else {
		if (!strchr ("~$=+/.", path[0])) {
			if (!strfpath (active[i].attribute->savedir, buf, sizeof (buf),
			    homedir, (char *) 0, (char *) 0, active[i].name)) {
#ifdef VMS
				joindir (buf, homedir, DEFAULT_SAVEDIR);
#else
				joinpath (buf, homedir, DEFAULT_SAVEDIR);
#endif
			}
			joinpath (tmp, buf, path);
			my_strncpy (path, tmp, PATH_LEN);
		}
		if (strfpath (path, buf, sizeof (buf), homedir,
		    (char *) 0, active[i].attribute->savedir, active[i].name)) {
			my_strncpy (path, buf, PATH_LEN);
		}
	}

#ifndef VMS			/* no good answer to this yet XXX */
	/*
	 *  create any directories, otherwise check
	 *  errno and give appropiate error message
	 */
	len = (int) strlen (path);

	for (i=0, j=0 ; i < len ; i++, j++) {
		buf[j] = path[i];
		if (i+1 < len && path[i+1] == '/') {
			buf[j+1] = '\0';
			if (stat (buf, &st) == -1) {
				if (my_mkdir (buf, 0755) == -1) {
					if (errno != EEXIST) {
						perror_message ("Cannot create %s", buf);
						return FALSE;
					}
				}
			}
		}
	}
#else
	if (my_mkdir (buf, 0755) == -1) {
		if (errno != EEXIST) {
			perror_message ("Cannot create %s", buf);
			return FALSE;
		}
	}
#endif

	if (mbox_format) {
		strcpy (path, tmp);
	}

#endif /* INDEX_DAEMON */

	return mbox_format;
}


int
create_sub_dir (i)
	int i;
{
#ifndef INDEX_DAEMON

	char dir[LEN];
	struct stat st;

	if (!save[i].is_mailbox && save[i].archive) {
		joinpath (dir, save[i].dir, save[i].archive);
		if (stat (dir, &st) == -1) {
			my_mkdir (dir, 0755);
			return TRUE;
		}
#ifdef M_AMIGA
		if (st.st_attr & ST_DIRECT) {
#else
#	ifdef M_OS2
		if (st.st_mode & S_IFDIR) {
#	else
		if ((st.st_mode & S_IFMT) == S_IFDIR) {
#	endif
#endif
			return TRUE;
		} else {
			return FALSE;
		}
	}

#endif /* INDEX_DAEMON */

	return FALSE;
}

/*
 *  add files to be saved to save array
 */

void
add_to_save_list (the_index, the_article, is_mailbox, archive_save, path)
	int the_index;
	struct t_article *the_article;
	int is_mailbox;
	int archive_save;
	char *path;
{
#ifndef INDEX_DAEMON
	char tmp[PATH_LEN];
	char dir[PATH_LEN];
	char file[PATH_LEN];
	int i;

	dir[0] = '\0';
	file[0] = '\0';

	if (num_save == max_save-1) {
		expand_save ();
	}

	save[num_save].index   = the_index;
	save[num_save].saved   = FALSE;
	save[num_save].is_mailbox = is_mailbox;
	save[num_save].dir     = (char *) 0;
	save[num_save].file    = (char *) 0;
	save[num_save].archive = (char *) 0;
	save[num_save].part    = (char *) 0;
	save[num_save].patch   = (char *) 0;

	save[num_save].subject = str_dup (the_article->subject);
	if (archive_save && the_article->archive) {
		save[num_save].archive = str_dup (the_article->archive);
		if (the_article->part) {
			save[num_save].part = str_dup (the_article->part);
		}
		if (the_article->patch) {
			save[num_save].patch = str_dup (the_article->patch);
		}
	}

	if (is_mailbox) {
		if ((int) strlen (path) > 1) {
			if (path[0] == '=') {
				my_strncpy (file, path+1, sizeof (file));
			} else {
				my_strncpy (file, path, sizeof (file));
			}
		} else {
			my_strncpy (file, glob_group, sizeof (file));
		}

		i = my_group[cur_groupnum];
		if (!strfpath (active[i].attribute->maildir, tmp, sizeof (tmp),
		    homedir, (char *) 0, (char *) 0, active[i].name)) {
#ifdef VMS
			joindir (tmp, homedir, DEFAULT_MAILDIR);
#else
			joinpath (tmp, homedir, DEFAULT_MAILDIR);
#endif
		}
		save[num_save].dir = str_dup (tmp);
		save[num_save].file = str_dup (file);
	} else {
		if (path[0]) {
#ifdef VMS
#include "parse.h"
			struct filespec *spec;

			spec = sysparse(path);
			sprintf(dir, "%s%s", spec->dev, spec->dir);
			strcpy(file, spec->filename);
#else /* !VMS */
			for (i=strlen (path) ; i ; i--) {
#ifdef WIN32
				/*
				** Under WIN32, paths can be in form D:\a\b\c\file.  Optionally,
				** User can override the default, so we need to deal with input in
				** Form: D:\a\b\c\F:\x\y\article or D:\a\b\c\\x\y\article.
				** In these cases, we want to use F:\x\y\article or \x\y\article
				** as the basepath.
				*/
				if (path[i] == '\\') {
					int j;
					for (j=i-1; j; j--) {
						if (path[j] == '\\' && path[j+1] == '\\') {
							if (i-j-1 == 0)
								strcpy(dir, "\\");
							else {
								strncpy (dir, &path[j+1], (size_t)(i-j-1));
								dir[i-j-1] = '\0';
							}
							break;
						}
						else if (path[j] == ':') {
							strncpy (dir, &path[j-1], (size_t)(i-j+1));
							dir[i-j+1] = '\0';
							break;
						}
					}
					strcpy (file, path+i+1);
					break;
#else /* !WIN32 */
				if (path[i] == '/') {
					strncpy (dir, path, (size_t)i);
					dir[i] = '\0';
					strcpy (file, path+i+1);
					break;
#endif /* WIN32 */
				}
			}
#endif /* !VMS */
		}

#ifdef M_AMIGA
		if (tin_bbs_mode) {
			dir[0] = 0;
		}
#endif

		if (dir[0]) {
			save[num_save].dir = str_dup (dir);
		} else {
			i = my_group[cur_groupnum];
			if (!strfpath (active[i].attribute->savedir, tmp, sizeof (tmp),
			    homedir, (char *) 0, (char *) 0, active[i].name)) {
#ifdef VMS
				joindir (tmp, homedir, DEFAULT_SAVEDIR);
#else
				joinpath (tmp, homedir, DEFAULT_SAVEDIR);
#endif
			}
			save[num_save].dir = str_dup (tmp);
		}

		if (file[0]) {
			save[num_save].file = str_dup (file);
		} else {
			if (path[0]) {
				save[num_save].file = str_dup (path);
			} else {
				save[num_save].file = str_dup (save[num_save].archive);
			}
		}
	}
	num_save++;

#endif /* INDEX_DAEMON */
}

/*
 *  print save array of files to be saved
 */

void
sort_save_list ()
{
	qsort ((char *) save, (size_t)num_save, sizeof (struct t_save), save_comp);
	debug_save_comp ();
}

/*
 *  string comparison routine for the qsort()
 *  ie. qsort(array, 5, 32, save_comp);
 */

int
save_comp (p1, p2)
	t_comptype *p1;
	t_comptype *p2;
{
	struct t_save *s1 = (struct t_save *) p1;
	struct t_save *s2 = (struct t_save *) p2;

	/*
	 * Sort on Archive-name: part & patch otherwise Subject:
	 */
	if (s1->archive != (char *) 0) {
		if (s1->part != (char *) 0) {
			if (s2->part != (char *) 0) {
				if (strcmp (s1->part, s2->part) < 0) {
					return -1;
				}
				if (strcmp (s1->part, s2->part) > 0) {
					return 1;
				}
			} else {
				return 0;
			}
		} else if (s1->patch != (char *) 0) {
			if (s2->patch != (char *) 0) {
				if (strcmp (s1->patch, s2->patch) < 0) {
					return -1;
				}
				if (strcmp (s1->patch, s2->patch) > 0) {
					return 1;
				}
			} else {
				return 0;
			}
		}
	} else {
		if (strcmp (s1->subject, s2->subject) < 0) {
			return -1;
		}
		if (strcmp (s1->subject, s2->subject) > 0) {
			return 1;
		}
	}

	return 0;
}


char *
save_filename (i)
	int i;
{
	char *p;
	static char *filename;

	FreeIfNeeded(filename);	/* leak only the last instance */
#ifdef DOALLOC
	if (i < 0)
		return 0;
#endif
	filename = (char *) my_malloc (PATH_LEN);

	if (save[i].is_mailbox) {
		joinpath (filename, save[i].dir, save[i].file);
		return (filename);
	}

	if (!default_auto_save || (!(save[i].part || save[i].patch))) {
		joinpath (filename, save[i].dir, save[i].file);
		if (num_save != 1) {
#ifdef VMS
			sprintf (&filename[strlen(filename)], "-%03d", i+1);
#else
			sprintf (&filename[strlen(filename)], ".%03d", i+1);
#endif

		}
	} else if ((p = save[i].part) || (p = save[i].patch)) {
		joinpath (filename, save[i].dir, save[i].archive);
		if (create_sub_dir (i)) {
#ifdef VMS
	    sprintf(&filename[strlen(filename)], "%s.%s%s", save[i].archive, LONG_PATH_PART, p);
#else
			sprintf (&filename[strlen(filename)], "/%s.%s%s", save[i].archive, LONG_PATH_PART, p);
#endif
		} else {
#ifdef VMS
			sprintf (&filename[strlen(filename)], "%s%s", LONG_PATH_PART, p);
#else
			sprintf (&filename[strlen(filename)], ".%s%s", LONG_PATH_PART, p);
#endif
		}
	}

	return filename;
}


char *
get_first_savefile ()
{
	char *file;
	int i;

	for (i=0 ; i < num_save ; i++) {
		if (save[i].saved) {
			file = (char *) my_malloc (PATH_LEN);
			if (save[i].is_mailbox) {
#ifdef VMS
				joinpath (file, save[i].dir, save[i].file);
#else
				sprintf (file, "%s/%s", save[i].dir, save[i].file);
#endif
				return file;
			} else {
				if (save[i].archive && default_auto_save) {
					if (save[i].part) {
						if (create_subdir) {
#ifdef VMS
							char fbuf[256], dbuf[256];
							sprintf(fbuf, "%s.%s%s", save[i].archive, LONG_PATH_PART, save[i].part);
							joinpath(file, save[i].archive, fbuf);
#else
							sprintf (file, "%s/%s.%s%s", save[i].archive, save[i].archive, LONG_PATH_PART, save[i].part);
#endif
						} else {
							sprintf (file, "%s.%s%s", save[i].archive, LONG_PATH_PART, save[i].part);
						}
					} else {
						if (create_subdir) {
#ifdef VMS
							char fbuf[256];
							sprintf(fbuf, "%s.%s%s", save[i].archive, LONG_PATH_PATCH, save[i].patch);
							joinpath(file, save[i].archive, fbuf);
#else
							sprintf (file, "%s/%s.%s%s", save[i].archive, save[i].archive, LONG_PATH_PATCH, save[i].patch);
#endif
						} else {
							sprintf (file, "%s.%s%s", save[i].archive, LONG_PATH_PATCH, save[i].patch);
						}
					}
				} else {
					if (num_save == 1) {
						sprintf (file, "%s", save[i].file);
					} else {
#ifdef VMS
						sprintf (file, "%s-%03d", save[i].file, i+1);
#else
						sprintf (file, "%s.%03d", save[i].file, i+1);
#endif
					}
				}
				return file;
			}
		}
	}
	return "";
}


char *
get_last_savefile ()
{
	char *file;
	int i;

	for (i=num_save-1 ; i >= 0 ; i--) {
		if (save[i].saved) {
			file = (char *) my_malloc (PATH_LEN);
			if (save[i].is_mailbox) {
#ifdef VMS
				joinpath (file, save[i].dir, save[i].file);
#else
				sprintf (file, "%s/%s", save[i].dir, save[i].file);
#endif
				return file;
			} else {
				if (save[i].archive && default_auto_save) {
					if (save[i].part) {
						if (create_subdir) {
#ifdef VMS
							char fbuf[256];
							sprintf(fbuf, "%s.%s%s", save[i].archive, LONG_PATH_PART, save[i].part);
							joinpath(file, save[i].archive, fbuf);
#else
							sprintf (file, "%s/%s.%s%s", save[i].archive, save[i].archive, LONG_PATH_PART, save[i].part);
#endif
						} else {
							sprintf (file, "%s.%s%s", save[i].archive, LONG_PATH_PART, save[i].part);
						}
					} else {
						if (create_subdir) {
#ifdef VMS
							char fbuf[256];
							sprintf(fbuf, "%s.%s%s", save[i].archive, LONG_PATH_PATCH, save[i].patch);
							joinpath(file, save[i].archive, fbuf);
#else
							sprintf (file, "%s/%s.%s%s", save[i].archive, save[i].archive, LONG_PATH_PATCH, save[i].patch);
#endif
						} else {
							sprintf (file, "%s.%s%s", save[i].archive, LONG_PATH_PATCH, save[i].patch);
						}
					}
				} else {
					if (num_save == 1) {
						sprintf (file, "%s", save[i].file);
					} else {
#ifdef VMS
						sprintf (file, "%s-%03d", save[i].file, i+1);
#else
						sprintf (file, "%s.%03d", save[i].file, i+1);
#endif
					}
				}
				return file;
			}
		}
	}
	return "";
}


int
post_process_files (proc_type_ch, auto_delete)
	int proc_type_ch;
	int auto_delete;
{
	if (any_saved_files ()) {
		wait_message (txt_post_processing);

		switch (proc_type_ch) {
			case iKeyPProcShar:
				post_process_sh (auto_delete);
				break;
			case iKeyPProcUUDecode:
				post_process_uud (POST_PROC_UUDECODE, auto_delete);
				break;
			case iKeyPProcListZoo:
				post_process_uud (POST_PROC_UUD_LST_ZOO, auto_delete);
				break;
			case iKeyPProcExtractZoo:
				post_process_uud (POST_PROC_UUD_EXT_ZOO, auto_delete);
				break;
			case iKeyPProcListZip:
				post_process_uud (POST_PROC_UUD_LST_ZIP, auto_delete);
				break;
			case iKeyPProcExtractZip:
				post_process_uud (POST_PROC_UUD_EXT_ZIP, auto_delete);
				break;
		}

		info_message (txt_post_processing_finished);
		sleep (1);
		return TRUE;
	}
	return FALSE;
}


void
post_process_uud (pp, auto_delete)
	int pp;
	int auto_delete;
{
#ifndef INDEX_DAEMON

	char s[LEN], t[LEN], u[LEN];
	char buf[LEN];
	char file_out[PATH_LEN];
	char file_out_dir[PATH_LEN];
	FILE *fp_in, *fp_out = NULL;
	int i, state;
	int open_out_file;

	t[0] = '\0';
	u[0] = '\0';

	my_strncpy (file_out_dir, save_filename (0), sizeof (file_out_dir));
	for (i=strlen(file_out_dir) ; i > 0 ; i--) {
#ifdef VMS
		if (file_out_dir[i] == ']') {
			file_out_dir[i+1] = '\0';
#else
		if (file_out_dir[i] == '/') {
			file_out_dir[i] = '\0';
#endif
			break;
		}
	}

#ifdef VMS
	sprintf (file_out, "%suue.%05d", file_out_dir, process_id);
#else
	sprintf (file_out, "%s/uue%05d", file_out_dir, process_id);
#endif
	state = INITIAL;
	open_out_file = TRUE;

	for (i=0 ; i < num_save ; i++) {
		if (!save[i].saved) {
			continue;
		}
		if (open_out_file) {
			if ((fp_out = fopen (file_out, "w")) == (FILE *) 0) {
				perror_message (txt_cannot_open, file_out);
				unlink (file_out);
				return;
			}
			open_out_file = FALSE;
		}

		my_strncpy (buf, save_filename (i), sizeof (buf));
		if ((fp_in = fopen (buf, "r")) != (FILE *) 0) {
			if (fgets (s, sizeof s, fp_in) == (char *) 0) {
				fclose (fp_in);
				continue;
			}
			while (state != END) {
				switch (state) {
					case INITIAL:
						if (!strncmp ("begin ", s, 6)) {
							state = MIDDLE;
							fprintf (fp_out, "%s", s);
						}
						break;

					case MIDDLE:
						if (s[0] == 'M') {
							fprintf (fp_out, "%s", s);
						} else if (strncmp("end", s, 3)) {
							state = OFF;
						} else { /* end */
							state = END;
							if (u[0] != 'M') {
								fprintf (fp_out, "%s", u);
							}
							if (t[0] != 'M') {
							    fprintf (fp_out, "%s", t);
							}
							fprintf (fp_out, "%s\n", s);
						}
						break;

					case OFF:
						if ((s[0] == 'M') && (t[0] == 'M') && (u[0] == 'M')) {
							fprintf (fp_out, "%s", u);
							fprintf (fp_out, "%s", t);
							fprintf (fp_out, "%s", s);
							state = MIDDLE;
						} else if (STRNCMPEQ("end", s, 3)) {
							state = END;
							if (u[0] != 'M') {
							    fprintf (fp_out, "%s", u);
							}
							if (t[0] != 'M') {
							    fprintf (fp_out, "%s", t);
							}
							fprintf (fp_out, "%s\n", s);
						}
						break;

					case END:
						break;

					default:
						fprintf (stderr, "\r\nError: ASSERT - default state\n");
						fclose (fp_in);
						fclose (fp_out);
						unlink (file_out);
						return;
				}
				strcpy (u,t);
				strcpy (t,s);
				/*
				 *  read next line & if error goto next file in save array
				 */
				if (fgets (s, sizeof s, fp_in) == (char *) 0) {
					break;
				}
			}
			fclose (fp_in);
		}
		if (state == END) {
			fclose (fp_out);
			uudecode_file (pp, file_out_dir, file_out);
			state = INITIAL;
			open_out_file = TRUE;
		}
	}

	delete_processed_files (auto_delete); /* TRUE = auto-delete files */
	unlink (file_out);

#endif /* INDEX_DAEMON */
}

/*
 *  uudecode a single file
 */

void
uudecode_file (pp, file_out_dir, file_out)
	int	pp;
	char	*file_out_dir;
	char	*file_out;
{
	char	buf[LEN];
	char	*file, *ptr;
	FILE	*fp_in;
	int	i, file_size = 0;
	struct	stat st;

	sprintf (buf, txt_uudecoding, file_out);
	wait_message (buf);

#if !defined(M_UNIX)
	make_post_process_cmd (DEFAULT_UUDECODE, file_out_dir, file_out);
#else
	sleep (1);
	sprintf (buf, "cd %s; uudecode %s", file_out_dir, file_out);
	if (invoke_cmd (buf)) {
		/*
		 *  Sum file
		 */
		if ((file = get_archive_file (file_out_dir)) != (char *) 0) {
			sprintf (buf, "%s '%s'", DEFAULT_SUM, file);
			printf (txt_checksum_of_file, file);
			fflush (stdout);
			if ((fp_in = popen (buf, "r")) == (FILE *) 0) {
				printf ("Cannot execute %s\r\n", buf);
				fflush (stdout);
			} else {
				if (stat (file, &st) != -1) {
					file_size = (int) st.st_size;
				}
				if (fgets (buf, sizeof buf, fp_in) != (char *) 0) {
					ptr = strchr (buf, '\n');
					if (ptr != (char *) 0) {
						*ptr = '\0';
					}
				}
				pclose (fp_in);
				printf ("%s  %8d bytes\r\n\r\n", buf, file_size);
				fflush (stdout);
			}

			/* If defined, invoke post processor command */
			if (*post_proc_command) {
				sprintf (buf, "cd %s; %s %s", file_out_dir, post_proc_command, file);

				if (!invoke_cmd (buf))
					error_message (txt_command_failed_s, buf);
			}

			if (pp > POST_PROC_UUDECODE) {
				/*
				 *  Test archive integrity
				 */
				if (pp > POST_PROC_UUDECODE && archiver[pp].test != (char *) 0) {
					i = (pp == POST_PROC_UUD_LST_ZOO || pp == POST_PROC_UUD_EXT_ZOO ? 3 : 4);
					sprintf (buf, "cd %s; %s %s %s", file_out_dir,
						archiver[i].name, archiver[i].test, file);
					printf (txt_testing_archive, file);
					fflush (stdout);
					if (!invoke_cmd (buf)) {
						error_message (txt_post_processing_failed, "");
					}
				}
				/*
				 *  List archive
				 */
				if (pp == POST_PROC_UUD_LST_ZOO || pp == POST_PROC_UUD_LST_ZIP) {
					i = (pp == POST_PROC_UUD_LST_ZOO ? 3 : 4);
					sprintf (buf, "cd %s; %s %s %s", file_out_dir,
						archiver[i].name, archiver[i].list, file);
					printf (txt_listing_archive, file);
					fflush (stdout);
					if (!invoke_cmd (buf)) {
						error_message (txt_post_processing_failed, "");
					}
					sleep (3);
				}
				/*
				 *  Extract archive
				 */
				if (pp == POST_PROC_UUD_EXT_ZOO || pp == POST_PROC_UUD_EXT_ZIP) {
					i = (pp == POST_PROC_UUD_EXT_ZOO ? 3 : 4);
					sprintf (buf, "cd %s; %s %s %s", file_out_dir,
						archiver[i].name, archiver[i].extract, file);
					printf (txt_extracting_archive, file);
					fflush (stdout);
					if (!invoke_cmd (buf)) {
						error_message (txt_post_processing_failed, "");
					}
					sleep (3);
				}

				if (file != (char *) 0) {
					free (file);
				}
			}
		}
	}
#endif	/* M_UNIX */
}

/*
 *  Unpack /bin/sh archives
 */

void
post_process_sh (auto_delete)
	int auto_delete;
{
#ifndef INDEX_DAEMON

	char buf[LEN];
	char file_in[PATH_LEN];
	char file_out[PATH_LEN];
	char file_out_dir[PATH_LEN];
	char *ptr1, *ptr2, *ptr3;
	char sh_pattern_1[16];
	char sh_pattern_2[16];
	char sh_pattern_3[64];
	FILE *fp_in, *fp_out;
	int found_header;
	int i, j;

	strcpy (sh_pattern_1, "#!/bin/sh");
	strcpy (sh_pattern_2, "#!/bin/sh");
	strcpy (sh_pattern_3, "# This is a shell archive.");

	my_strncpy (file_out_dir, save_filename (0), sizeof (file_out_dir));
	for (i=strlen(file_out_dir) ; i > 0 ; i--) {
#ifdef VMS
		if (file_out_dir[i] == ']') {
			file_out_dir[i+1] = '\0';
#else
		if (file_out_dir[i] == '/') {
			file_out_dir[i] = '\0';
#endif
			break;
		}
	}

#ifdef VMS
	sprintf (file_out, "%ssh.%05d", file_out_dir, process_id);
#else
	sprintf (file_out, "%s/sh%05d", file_out_dir, process_id);
#endif

	for (j=0 ; j < num_save ; j++) {
		if (!save[j].saved) {
			continue;
		}
		my_strncpy (file_in, save_filename (j), sizeof (file_in));

		printf (txt_extracting_shar, file_in);
		fflush (stdout);

		found_header = FALSE;

#ifdef VMS
		if ((fp_out = fopen (file_out, "w", "fop=cif")) != NULL) {
#else
		if ((fp_out = fopen (file_out, "w")) != NULL) {
#endif
			if ((fp_in = fopen (file_in, "r")) != NULL) {
				ptr1 = sh_pattern_1;
				ptr2 = sh_pattern_2;
				ptr3 = sh_pattern_3;

				while (!feof (fp_in)) {
					if (fgets (buf, sizeof buf, fp_in)) {
						/*
						 *  find #!/bin/sh or #!/bin/sh pattern
						 */
						if (!found_header) {
							if (strstr (buf, ptr1) != 0 ||
								strstr (buf, ptr2) != 0 ||
								strstr (buf, ptr3) != 0) {
								found_header = TRUE;
							}
						}

						/*
						 *  Write to temp file
						 */
						if (found_header) {
							my_fputs (buf, fp_out);
						}
					}
				}
				fclose (fp_in);
			}
			fclose (fp_out);

#if !defined(M_UNIX)
			make_post_process_cmd (DEFAULT_UNSHAR, file_out_dir, file_out);
#else
			sprintf (buf, "cd %s; sh %s", file_out_dir, file_out);
			my_fputs ("\r\n", stdout);
			fflush (stdout);
			Raw (FALSE);
			invoke_cmd (buf);
			Raw (TRUE);
#endif
			unlink (file_out);
		}
	}
	delete_processed_files (auto_delete);

#endif /* INDEX_DAEMON */
}

/*
 * Returns the most recently modified file in the specified drectory
 */

char *
get_archive_file (dir)
	char *dir;
{
	char buf[LEN];
	char *file;
	DIR *dirp;
	DIR_BUF *dp;
	struct stat sbuf;
	time_t last = 0;

	file = (char *) my_malloc (LEN);
	if (file == (char *) 0) {
		return (char *) 0;
	}

	if ((dirp = opendir (dir)) == (DIR *) 0) {
		free (file);
		return (char *) 0;
	}

	dp = (DIR_BUF *) readdir (dirp);
	while (dp != (DIR_BUF *) 0) {
#ifdef VMS
		joinpath (buf, dir, dp->d_name);
#else
		sprintf (buf, "%s/%s", dir, dp->d_name);
#endif
		stat (buf, &sbuf);
		if ((sbuf.st_mtime > last) && S_ISREG(sbuf.st_mode)) {
			last = sbuf.st_mtime;
			strcpy (file, buf);
		}
		dp = (DIR_BUF *) readdir (dirp);
	}
	closedir (dirp);

	if (last == 0) {
		free (file);
		file = (char *) 0;
	}

	return file;
}


void
delete_processed_files (auto_delete)
	int auto_delete;
{
#ifndef INDEX_DAEMON

	int delete = FALSE;
	int i;

	if (any_saved_files ()) {
		if (CURR_GROUP.attribute->delete_tmp_files ||
			auto_delete) {
			delete = TRUE;
		} else if (prompt_yn (cLINES, txt_delete_processed_files, TRUE) == 1) {
			delete = TRUE;
		}

		if (delete) {
			wait_message ("\r\n");
			wait_message (txt_deleting);

			for (i=0 ; i < num_save ; i++) {
				unlink (save_filename (i));
			}
		}
	}

#endif	/* INDEX_DAEMON */
}

static int
any_saved_files ()
{
	int i, saved = FALSE;

	for (i=0 ; i < num_save ; i++) {
		if (save[i].saved) {
			saved = TRUE;
			break;
		}
	}

	return saved;
}

void
print_art_seperator_line (fp, the_mailbox)
	FILE *fp;
	int the_mailbox;
{
	int sep = 0x01;	/* Ctrl-A */

	if (debug == 2) {
		sprintf (msg, "Mailbox=[%d]  MMDF=[%d]", the_mailbox, save_to_mmdf_mailbox);
		error_message (msg, "");
	}

	if (the_mailbox && save_to_mmdf_mailbox) {
		fprintf (fp, "%c%c%c%c\n", sep, sep, sep, sep);
	} else {
		my_fputc ('\n', fp);
	}
}
