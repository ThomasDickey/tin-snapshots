/*
 *  Project   : tin - a Usenet reader
 *  Module    : misc.c
 *  Author    : I. Lea & R. Skrenta
 *  Created   : 1991-04-01
 *  Updated   : 2003-02-15
 *  Notes     :
 *  Copyright : (c) Copyright 1991-99 by Iain Lea & Rich Skrenta
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
#ifndef VERSION_H
#	include  "version.h"
#endif /* !VERSION_H */
#ifndef BUGREP_H
#	include  "bugrep.h"
#endif /* !BUGREP_H */
#ifndef included_trace_h
#	include "trace.h"
#endif /* !included_trace_h */
#ifndef TIN_POLICY_H
#	include	"policy.h"
#endif /* !TIN_POLICY_H */

/*
 * defines to control GNKSA-checks behaviour:
 * - ENFORCE_RFC1034
 *   require domain name components not to start with a digit
 *
 * - REQUIRE_BRACKETS_IN_DOMAIN_LITERAL
 *   require domain literals to be enclosed in square brackets
 */

/*
 * Local prototypes
 */
static char *escape_shell_meta (char *source, int quote_area);
static int gnksa_check_domain (char *domain);
static int gnksa_check_domain_literal (char *domain);
static int gnksa_check_localpart (char *localpart);
static int gnksa_dequote_plainphrase (char *realname, char *decoded, int addrtype);
static int gnksa_split_from (char *from, char *address, char *realname, int *addrtype);
static int strfeditor (char *editor, int linenum, char *filename, char *s, size_t maxsize, char *format);
static void write_input_history_file (void);
#ifdef LOCAL_CHARSET
	static int to_local (int c);
	static int to_network (int c);
#endif /* LOCAL_CHARSET */

/*
 * append_file instead of rename_file
 * minimum error trapping
 */
void
append_file (
	char *old_filename,
	char *new_filename)
{
	FILE *fp_old, *fp_new;

	if ((fp_new = fopen (new_filename, "r")) == (FILE *) 0) {
		perror_message (txt_cannot_open, new_filename);
		return;
	}
	if ((fp_old = fopen (old_filename, "a+")) == (FILE *) 0) {
		perror_message (txt_cannot_open, old_filename);
		fclose (fp_new);
		return;
	}
	copy_fp (fp_new, fp_old);
	fclose (fp_old);
	fclose (fp_new);
}


void
asfail (
	const char *file,
	int line,
	const char *cond)
{
	my_fprintf (stderr, txt_error_asfail, tin_progname, file, line, cond);
	my_fflush (stderr);

/*
 * create a core dump
 */
#ifdef HAVE_COREFILE
#	ifdef SIGABRT
		sigdisp(SIGABRT, SIG_DFL);
		kill (process_id, SIGABRT);
#	else
#		ifdef SIGILL
			sigdisp(SIGILL, SIG_DFL);
			kill (process_id, SIGILL);
#		else
#			ifdef SIGIOT
				sigdisp(SIGIOT, SIG_DFL);
				kill (process_id, SIGIOT);
#			endif /* SIGIOT */
#		endif /* SIGILL */
#	endif /* SIGABRT */
#endif /* HAVE_COREFILE */

	giveup();
}


/*
 * Quick copying of files
 * Returns FALSE if copy failed. Caller may wish to check for SIGPIPE
 */
t_bool
copy_fp (
	FILE *fp_ip,
	FILE *fp_op)
{
	char buf[8192];
	size_t have, sent;

	while ((have = fread (buf, 1, sizeof(buf), fp_ip)) != 0) {
		sent = fwrite (buf, 1, have, fp_op);
		if (sent != have) {
			TRACE(("copy_fp wrote %d of %d:{%.*s}",
				sent, have, (int) sent, buf));
			if (!got_sig_pipe) /* !SIGPIPE => more serious error */
				perror_message (txt_error_copy_fp);
			return FALSE;
		}
		TRACE(("copy_fp wrote %d:{%.*s}", sent, (int) sent, buf));
	}
	return TRUE;
}


/*
 * copy the body of articles with given file pointers,
 * prefix (= quote_chars), initials of the articles author
 * and a flag, if the signature should be quoted
 */

void
copy_body (
	FILE *fp_ip,
	FILE *fp_op,
	char *prefix,
	char *initl,
	t_bool with_sig)
{
	char buf[8192];
	char buf2[8192];
	char prefixbuf[256];
	int i;
	int retcode;
	t_bool status_char;
	t_bool status_space;

	/* This is a shortcut for speed reasons: if no prefix (= quote_chars) is given just copy */
	if (!prefix || !*prefix) {
		copy_fp (fp_ip, fp_op);
		return;
	}

	if (strlen(prefix) > 240) /* truncate and terminate */
		prefix[240] = '\0';

	/* convert %S to %s, for compability reasons only */
	if (strstr(prefix, "%S")) {
		status_char = FALSE;
		for (i = 0; prefix[i]; i++) {
			if ((status_char) && (prefix[i] == 'S'))
				prefix[i] = 's';
			status_char = (prefix[i] == '%');
		}
	}

	if (strstr(prefix, "%s"))
		sprintf(prefixbuf, prefix, initl);
	else {
		/* strip tailing space from quote-char for quoting quoted lines */
		strcpy(prefixbuf, prefix);
		if (prefixbuf[strlen(prefixbuf)-1] == ' ')
			prefixbuf[strlen(prefixbuf)-1] = '\0';
	}

	while (fgets (buf, (int) sizeof(buf), fp_ip) != (char *) 0) {
		if (!with_sig && !strcmp(buf, "-- \n"))
			break;
		if (strstr(prefix, "%s")) { /* initials wanted */
			if (buf[0] != '\n') { /* line is not empty */
				if (strchr(buf, '>')) {
					status_space = FALSE;
					status_char = TRUE;
					for (i=0; buf[i] && (buf[i] != '>'); i++) {
						buf2[i] = buf[i];
						if (buf[i] != ' ')
							status_space = TRUE;
						if ((status_space) && !(isalpha((int)buf[i]) || buf[i] == '>'))
							status_char = FALSE;
					}
					buf2[i] = '\0';
					if (status_char)	/* already quoted */
						retcode = fprintf (fp_op, "%s>%s", buf2, strchr(buf, '>'));
					else	/* ... to be quoted ... */
						retcode = fprintf (fp_op, "%s%s", prefixbuf, buf);
				} else	/* line was not already quoted (no >) */
					retcode = fprintf (fp_op, "%s%s", prefixbuf, buf);
			} else	/* line is empty */
					retcode = fprintf (fp_op, "%s\n", (tinrc.quote_empty_lines ? prefixbuf : ""));
		} else {		/* no initials in quote_string, just copy */
			if ((buf[0] != '\n') || tinrc.quote_empty_lines) {
				/* use blank-stripped quote string if line is already quoted */
					retcode = fprintf (fp_op, "%s%s", ((buf[0]=='>') ? prefixbuf : prefix), buf);
			} else
				retcode = fprintf (fp_op, "\n");
		}
		if (retcode == EOF) {
			perror_message ("copy_body() failed");
			return;
		}
	}
}


/*
 * Lookup 'env' in the environment. If it exists, return its value,
 * else return 'def'
 */
const char *
get_val (
	const char *env,	/* Environment variable we're looking for */
	const char *def)	/* Default value if no environ value found */
{
	const char *ptr;

	return ((ptr = getenv(env)) != (char *) 0 ? ptr : def);
}


/*
 * IMHO it's not tins job to take care about dumb editor backupfiles
 * otherwise BACKUP_FILE_EXT should be configurable via configure
 * or 'M'enu
 */
#define BACKUP_FILE_EXT ".b"

t_bool
invoke_editor (
	char *filename,
	int lineno) /* return value is always ignored */
{
	char *my_editor;
	char buf[PATH_LEN], fnameb[PATH_LEN];
	char editor_format[PATH_LEN];
	t_bool retcode;
	static char editor[PATH_LEN];
	static t_bool first = TRUE;

	if (first) {
		my_editor = getenv ("EDITOR");

		strcpy (editor, my_editor != NULL ? my_editor : get_val ("VISUAL", DEFAULT_EDITOR));
		first = FALSE;
	}

	strcpy (editor_format, (*tinrc.editor_format ? tinrc.editor_format : (tinrc.start_editor_offset ? TIN_EDITOR_FMT_ON : TIN_EDITOR_FMT_OFF)));

	if (!strfeditor (editor, lineno, filename, buf, sizeof(buf), editor_format))
		sh_format (buf, sizeof(buf), "%s %s", editor, filename);

	retcode = invoke_cmd (buf);

#ifdef BACKUP_FILE_EXT
	strcpy (fnameb, filename);
	strcat (fnameb, BACKUP_FILE_EXT);
	unlink (fnameb);
#endif /* BACKUP_FILE_EXT */
	return retcode;
}


#ifdef HAVE_ISPELL
t_bool
invoke_ispell (
	char *nam,
	struct t_group *psGrp) /* return value is always ignored */
{
	FILE *fp_all, *fp_body, *fp_head;
	char buf[PATH_LEN], nam_body[100], nam_head[100];
	t_bool retcode;
	char ispell[PATH_LEN];

/*
 * IMHO we don't need an exception for VMS as PATH_ISPELL
 * defaults to ispell (uj 19990617)
 */
/*
#	ifdef VMS
	strcpy (ispell, "ispell");
#	else
*/
	if (psGrp && psGrp->attribute->ispell != (char *) 0)
		strcpy (ispell, psGrp->attribute->ispell);
	else
		strcpy (ispell, get_val("ISPELL", PATH_ISPELL));
/*
#	endif *//* VMS */

	/*
	 * Now seperating the header and body in two different files so that
	 * the header is not checked by ispell
	 */

	strncpy (nam_body, nam, 90);
	strcat (nam_body, ".body");

	strncpy (nam_head, nam, 90);
	strcat (nam_head, ".head");

	if ((fp_all = fopen(nam, "r")) == (FILE *) 0) {
		perror_message(txt_cannot_open, nam);
		return FALSE;
	}


	if ((fp_head = fopen (nam_head, "w")) == NULL) {
		perror_message(txt_cannot_open, nam_head);
		fclose (fp_all);
		return FALSE;
	}

	if ((fp_body = fopen (nam_body, "w")) == NULL) {
		perror_message(txt_cannot_open, nam_body);
		fclose (fp_head);
		fclose (fp_all);
		return FALSE;
	}

	while (fgets (buf, (int) sizeof(buf), fp_all) != NULL) {
		fputs (buf, fp_head);
		if (buf[0] == '\n' || buf[0] == '\r') {
			fclose (fp_head);
			break;
		}
	}

	while (fgets(buf, (int) sizeof(buf), fp_all) != NULL)
		fputs(buf, fp_body);

	fclose (fp_body);
	fclose (fp_all);

	sh_format (buf, sizeof(buf), "%s %s", ispell, nam_body);
	retcode = invoke_cmd(buf);

	append_file (nam_head, nam_body);
	unlink (nam_body);
	rename_file (nam_head, nam);

	return retcode;
}
#endif /* HAVE_ISPELL */


#ifndef NO_SHELL_ESCAPE
void
shell_escape (
	void)
{
	char *p;
	char shell[LEN];

	sprintf (mesg, txt_shell_escape, tinrc.default_shell_command);

	if (!prompt_string (mesg, shell, HIST_SHELL_COMMAND))
		return;

	for (p = shell; *p && (*p == ' ' || *p == '\t'); p++)
		continue;

	if (*p)
		my_strncpy (tinrc.default_shell_command, p, sizeof(tinrc.default_shell_command));
	else {
		my_strncpy (shell, (*tinrc.default_shell_command ? tinrc.default_shell_command : (get_val (ENV_VAR_SHELL, DEFAULT_SHELL))), sizeof(shell));
		p = shell;
	}

	ClearScreen ();
	sprintf (mesg, "Shell Command (%s)", p); /* FIXME: -> lang.c */
	center_line (0, TRUE, mesg);
	MoveCursor (INDEX_TOP, 0);

	(void)invoke_cmd(p);

	continue_prompt ();

	if (tinrc.draw_arrow)
		ClearScreen ();
}
#endif /* !NO_SHELL_ESCAPE */


void
tin_done (
	int ret)
{
	static int nested;
	register int i;
	t_bool ask = TRUE;
	struct t_group *group;
	signed long int wrote_newsrc_lines = -1;

	if (nested++)
		giveup();

	signal_context = cMain;

	/*
	 * check if any groups were read & ask if they should marked read
	 */
	if (tinrc.catchup_read_groups && !cmd_line && !no_write) {
		for (i = 0; i < group_top; i++) {
			group = &active[my_group[i]];
			if (group->read_during_session) {
				if (ask) {
					if (prompt_yn (cLINES, txt_catchup_all_read_groups, FALSE) == 1) {
						ask = FALSE;
						tinrc.thread_articles = THREAD_NONE;	/* speeds up index loading */
					} else
						break;
				}
				wait_message (0, "Catchup %s...", group->name);
				grp_mark_read (group, NULL);
			}
		}
	}

	/*
	 * Save the newsrc file. If it fails for some reason, give the user a
	 * chance to try again
	 */
	if (!no_write) {
		forever {
			if (((wrote_newsrc_lines = vWriteNewsrc ()) >= 0L) && (wrote_newsrc_lines >= read_newsrc_lines)) {
				my_fputs(txt_newsrc_saved, stdout);
				break;
			}

			if (wrote_newsrc_lines < read_newsrc_lines) {
				/* FIXME: prompt for retry? (i.e. remove break) */
				wait_message(5, txt_warn_newsrc, newsrc, (read_newsrc_lines - wrote_newsrc_lines), (read_newsrc_lines - wrote_newsrc_lines) == 1 ? "" : txt_plural, OLDNEWSRC_FILE);
				break;
			}

			if (!prompt_yn (cLINES, txt_newsrc_again, TRUE))
				break;
		}

		write_input_history_file ();
#if 0 /* FIXME */
#	ifndef INDEX_DAEMON
		write_attributes_file (local_attributes_file);
#	endif /* !INDEX_DAEMON */
#endif /* 0 */

#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
		write_mail_active_file ();
#endif /* !INDEX_DAEMON && HAVE_MH_MAIL_HANDLING */
	}

	/* Do this sometime after we save the newsrc in case this hangs up for any reason */
	if (ret != NNTP_ERROR_EXIT)
		nntp_close ();			/* disconnect from NNTP server */

	free_all_arrays ();
#ifdef SIGUSR1
	if (ret != -SIGUSR1) {
#endif /* SIGUSR1 */
#ifdef HAVE_COLOR
		use_color = FALSE;
		EndInverse();
#else
		if (!cmd_line)
#endif /* HAVE_COLOR */
		{
			if (!ret)
				ClearScreen ();
		}
		EndWin ();
		Raw (FALSE);
#ifdef SIGUSR1
	} else {
		ret = SIGUSR1;
	}
#endif /* SIGUSR1 */
	cleanup_tmp_files ();

#ifdef DOALLOC
	no_leaks();	/* free permanent stuff */
	show_alloc();	/* memory-leak testing */
#endif /* DOALLOC */

#ifdef USE_DBMALLOC
	/* force a dump, circumvents a bug in Linux libc */
	{
		extern int malloc_errfd;	/* FIXME */
		malloc_dump(malloc_errfd);
	}
#endif /* USE_DBMALLOC */

#ifdef VMS
	ret = !ret;
	vms_close_stdin (); /* free resources used by ReadCh */
#endif /* VMS */

	exit (ret);
}


/*
 * strip_double_ngs ()
 * Strip duplicate newsgroups from within a given list of comma
 * separated groups
 *
 * 14-Jun-'96 Sven Paulus <sven@oops.sub.de>
 *
 */
void
strip_double_ngs (
	char *ngs_list)
{
	char *ptr;			/* start of next (outer) newsgroup */
	char *ptr2;			/* temporary pointer */
	char ngroup1[HEADER_LEN];	/* outer newsgroup to compare */
	char ngroup2[HEADER_LEN];	/* inner newsgroup to compare */
	char cmplist[HEADER_LEN];	/* last loops output */
	char newlist[HEADER_LEN];	/* the newly generated list without */
										/* any duplicates of the first nwsg */
	int ncnt1;			/* counter for the first newsgroup */
	int ncnt2;			/* counter for the second newsgroup */
	t_bool over1;		/* TRUE when the outer loop is over */
	t_bool over2;		/* TRUE when the inner loop is over */

	/* shortcut, check if there is only 1 group */
	if (strchr(ngs_list, ',') != (char *) 0) {
		over1 = FALSE;
		ncnt1 = 0;
		strcpy(newlist, ngs_list);		/* make a "working copy" */
		ptr = newlist;						/* the next outer newsg. is the 1st */

		while (!over1) {
			ncnt1++;							/* inc. outer counter */
			strcpy(cmplist, newlist);	/* duplicate groups for inner loop */
			ptr2 = strchr(ptr, ',');	/* search "," ... */
			if (ptr2 != (char *) 0) {	/* if found ... */
				*ptr2 = '\0';
				strcpy(ngroup1, ptr);	/* chop off first outer newsgroup */
				ptr = ptr2 + 1;			/* pointer points to next newsgr. */
			} else {							/* ... if not: last group */
				over1 = TRUE;				/* wow, everything is done after . */
				strcpy(ngroup1, ptr);	/* ... this last outer newsgroup */
			}

			over2 = FALSE;
			ncnt2 = 0;

			/*
			 * now compare with each inner newsgroup on the list,
			 * which is behind the momentary outer newsgroup
			 * if it is different from the outer newsgroup, append
			 * to list, strip double-commas
			 */

			while (!over2) {
				ncnt2++;
				strcpy(ngroup2, cmplist);
				ptr2 = strchr(ngroup2, ',');
				if (ptr2 != (char *) 0) {
					strcpy(cmplist, ptr2+1);
					*ptr2 = '\0';
				} else
					over2 = TRUE;

				if ((ncnt2 > ncnt1) && (strcasecmp(ngroup1, ngroup2)) && (strlen(ngroup2) != 0)) {
					strcat(newlist, ",");
					strcat(newlist, ngroup2);
				}
			}
		}
		strcpy(ngs_list, newlist);	/* move string to its real location */
	}
}


int
my_mkdir (
	char *path,
	mode_t mode)
{
#ifndef HAVE_MKDIR
	char buf[LEN];
	struct stat sb;

	sprintf(buf, "mkdir %s", path); /* redirect stderr to /dev/null ? */
	if (stat (path, &sb) == -1) {
		system (buf);
		return chmod (path, mode);
	} else
		return -1;
#else
#	if defined(M_OS2) || defined(WIN32)
		return mkdir (path);
#	else
		return mkdir (path, mode);
#	endif /* M_OS2 || WIN32 */
#endif /* !HAVE_MKDIR */
}

int
my_chdir (
	char *path)
{
	int retcode;

	retcode = chdir (path);

#ifdef M_OS2
	if (*path && path[1] == ':') {
		_chdrive (toupper((unsigned char)path[0]) - 'A' + 1);
	}
#endif /* M_OS2 */

	return retcode;
}

#ifdef M_UNIX
void
rename_file (
	char *old_filename,
	char *new_filename)
{
	FILE *fp_old, *fp_new;

	unlink (new_filename);

#	ifdef HAVE_LINK
	if (link (old_filename, new_filename) == -1)
#	else
	if (rename (old_filename, new_filename) < 0)
#	endif /* HAVE_LINK */
	{
		if (errno == EXDEV) {	/* create & copy file across filesystem */
			if ((fp_old = fopen (old_filename, "r")) == (FILE *) 0) {
				perror_message (txt_cannot_open, old_filename);
				return;
			}
			if ((fp_new = fopen (new_filename, "w")) == (FILE *) 0) {
				perror_message (txt_cannot_open, new_filename);
				fclose (fp_old);
				return;
			}
			copy_fp (fp_old, fp_new);
			fclose (fp_new);
			fclose (fp_old);
			errno = 0;
		} else {
			perror_message (txt_rename_error, old_filename, new_filename);
			return;
		}
	}
#	ifdef HAVE_LINK
	if (unlink (old_filename) == -1) {
		perror_message (txt_rename_error, old_filename, new_filename);
		return;
	}
#	endif /* HAVE_LINK */
}
#endif /* M_UNIX */

#ifdef VMS
void
rename_file (
	char *old_filename,
	char *new_filename)
{
	char new_filename_vms[1024];

	if (!strchr(strchr(new_filename, ']') ? strchr(new_filename, ']') : new_filename, '.')) {
		/* without final dot the new filename is not as tin expects */
		if (strlen(new_filename) >= sizeof(new_filename_vms)) {
			perror_message ("length of %s is too large", new_filename);
			return;
		}
		strcpy(new_filename_vms, new_filename);
		strcat(new_filename_vms, ".");
		new_filename = &new_filename_vms[0];
	}

	if (rename(old_filename, new_filename))
		perror_message (txt_rename_error, old_filename, new_filename);
}
#endif /* VMS */


#ifdef M_AMIGA
/*
 * AmigaOS now has links. Better not to use them as not everybody has new ROMS
 */
void
rename_file (
	char *old_filename,
	char *new_filename)
{
	char buf[1024];

	unlink (new_filename);
	if (rename (old_filename, new_filename) == EOF)
		perror_message (txt_rename_error, old_filename, new_filename);

	return;
}
#endif /* M_AMIGA */


t_bool
invoke_cmd (
	char *nam)
{
	int ret;

	t_bool save_cmd_line = cmd_line;
	if (!save_cmd_line) {
		EndWin ();
		Raw (FALSE);
	}
	set_signal_catcher (FALSE);

	TRACE(("called system(%s)", _nc_visbuf(nam)))
#ifdef USE_SYSTEM_STATUS
	system(nam);
	ret = system_status;
#else
	ret = system (nam);
#endif /* USE_SYSTEM_STATUS */
	TRACE(("return %d", ret))
#ifdef VMS
	ret = !ret;	  /* good enough to test success/fail */
#endif

	set_signal_catcher (TRUE);
	if (!save_cmd_line) {
		Raw (TRUE);
		InitWin ();
		need_resize = cYes;		/* Flag a redraw */
	}

	if (ret != 0)
		error_message (txt_command_failed, nam);

	return ret == 0;
}


void
draw_percent_mark (
	long cur_num,
	long max_num)
{
	char buf[32]; /* FIXME: ensure it's always big enough */
	int percent;

	if (NOTESLINES <= 0)
		return;

	if (cur_num <= 0 && max_num <= 0)
		return;

	percent = (int) (cur_num * 100 / max_num);
	sprintf (buf, "%s(%d%%) [%ld/%ld]", txt_more, percent, cur_num, max_num);
	MoveCursor (cLINES, (cCOLS - (int) strlen (buf))-(1+BLANK_PAGE_COLS));
	StartInverse ();
	my_fputs (buf, stdout);
	my_flush ();
	EndInverse ();
}

/*
 * seteuid/setegid - BSD 4.3 (based on POSIX setuid/setgid)
 * setreuid/setregid - BSD 4.2
 * setuid/setgid - SYSV, POSIX (Std003.1-1988)
 */
void
set_real_uid_gid (
	void)
{
#ifdef HAVE_SET_GID_UID
	if (local_index)
		return;

	umask (real_umask);

#	if defined(HAVE_SETEUID) && defined(HAVE_SETEGID)
	if (seteuid (real_uid) == -1)
		perror_message ("Error seteuid(real) failed");

	if (setegid (real_gid) == -1)
		perror_message ("Error setegid(real) failed");

#	else
#		if defined(HAVE_SETREUID) && defined(HAVE_SETREGID)
	if (setreuid (-1, real_uid) == -1)
		perror_message ("Error setreuid(real) failed");

	if (setregid (-1, real_gid) == -1)
		perror_message ("Error setregid(real) failed");

#		else
	if (setuid (real_uid) == -1)
		perror_message ("Error setuid(real) failed");

	if (setgid (real_gid) == -1)
		perror_message ("Error setgid(real) failed");

#		endif /* HAVE_SETREUID && HAVE_SETREGID */
#	endif /* HAVE_SETEUID && HAVE_SETEGID */
#endif /* HAVE_SET_GID_UID */
}

void
set_tin_uid_gid (
	void)
{
#ifdef HAVE_SET_GID_UID
	if (local_index)
		return;

	umask (0);

#	if defined(HAVE_SETEUID) && defined(HAVE_SETEGID)
	if (seteuid (tin_uid) == -1)
		perror_message ("Error seteuid(real) failed");

	if (setegid (tin_gid) == -1)
		perror_message ("Error setegid(real) failed");

#	else
#		if defined(HAVE_SETREUID) && defined(HAVE_SETREGID)
	if (setreuid (-1, tin_uid) == -1)
		perror_message ("Error setreuid(tin) failed");

	if (setregid (-1, tin_gid) == -1)
		perror_message ("Error setregid(tin) failed");

#		else
	if (setuid (tin_uid) == -1)
		perror_message ("Error setuid(tin) failed");

	if (setgid (tin_gid) == -1)
		perror_message ("Error setgid(tin) failed");

#		endif /* HAVE_SETREUID && HAVE_SETREGID */
#	endif /* HAVE_SETEUID && HAVE_SETEGID */
#endif /* HAVE_SET_GID_UID */
}


void
base_name (
	char *dirname,		/* argv[0] */
	char *program)		/* tin_progname is returned */
{
	int i;
#ifdef VMS
	char *cp;
#endif /* VMS */

	strcpy (program, dirname);

	for (i = (int) strlen (dirname)-1; i; i--) {
#ifndef VMS
		if (dirname[i] == SEPDIR) {
#else
		if (dirname[i] == ']') {
#endif /* !VMS */
			strcpy (program, dirname+(i+1));
			break;
		}
	}
#ifdef M_OS2
	str_lwr (program);
#endif /* M_OS2 */
#ifdef VMS
	if (cp = strrchr(program, '.'))
		*cp = '\0';
#endif /* VMS */
}


/*
 *  Return TRUE if new mail has arrived
 */
t_bool
mail_check (
	void)
{
#ifndef WIN32 /* No unified mail transport on WIN32 */
	const char *mailbox_name;
	struct stat buf;
#	ifdef M_AMIGA
	static long mbox_size = 0;
#	endif /* M_AMIGA */

	mailbox_name = get_val ("MAIL", mailbox);

#	ifdef M_AMIGA
	/*
	 * Since AmigaDOS does not distinguish between atime and mtime
	 * we have to find some other way to figure out if the mailbox
	 * was modified (to bad that Iain removed the mail_setup() and
	 * mail_check() scheme used prior to 1.30 260694 which worked also
	 * on AmigaDOS). (R. Luebke 10.7.1994)
	 */

	/* this is only a first try, but it seems to work :) */

	if (mailbox_name != 0) {
		if (stat (mailbox_name, &buf) >= 0) {
			if (buf.st_size > 0) {
				if (buf.st_size > mbox_size) {
					mbox_size = buf.st_size;
					return TRUE;
				} else
					/*
					 * at this point we have to calculate how much the
					 * mailbox has to grow until we say "new mail"
					 * Unfortunately, some MUAs write status information
					 * back to to the users mailbox. This is a size increase
					 * and would result in "new mail" if we only look for some
					 * size increase. The mbox_size calculation below works
					 * for me for some time now (I use AmigaELM).
					 * Probably there is a better method, if you know one
					 * you are welcome... :-)
					 * I think a constant offset is more accurate today,
					 * 1k is the average size of mail-headers alone in each
					 * message I receive. (obw)
					 */
					mbox_size = buf.st_size + 1024;
			} else
				mbox_size = 0;
		}
	}
#	else
	if (mailbox_name != 0 && stat (mailbox_name, &buf) >= 0 && buf.st_atime < buf.st_mtime && buf.st_size > 0)
		return TRUE;
#	endif /* M_AMIGA */
#endif /* !WIN32 */
	return FALSE;
}

#if 0
/*
 * Returns the user name and E-mail address of the user
 *
 * Written by ahd 15 July 1989
 * Borrowed from UUPC/extended with some mods by nms
 * Rewritten from scratch by Th. Quinot, 1997-01-03
 */

#	ifdef lint
static int once;
#		define ONCE while(once)
#	else
#		define ONCE while(0)
#	endif /* lint */

#	define APPEND_TO(dest, src) do { \
	(void) sprintf ((dest), "%s", (src)); \
	(dest) = strchr((dest), '\0'); \
	} ONCE
#	define RTRIM(whatbuf, whatp) do { (whatp)--; \
	while ((whatp) >= (whatbuf) && \
	(*(whatp) == ' ')) \
	*((whatp)--) = '\0'; } ONCE
#	define LTRIM(whatbuf, whatp) for ((whatp) = (whatbuf); \
	(whatp) && (*(whatp) == ' '); \
	(whatp)++)
#	define TRIM(whatbuf, whatp) do { RTRIM ((whatbuf), (whatp)); \
	LTRIM ((whatbuf), (whatp)); \
	} ONCE

void
parse_from (
	char *addr,
	char *addrspec,
	char *comment)
{
	char atom_buf[HEADER_LEN];
	char quoted_buf[HEADER_LEN];

	char *atom_p = atom_buf;
	char *quoted_p = quoted_buf;

	char asbuf[HEADER_LEN];
	char cmtbuf[HEADER_LEN];

	char *ap = addr,
		 *asp = asbuf,
		*cmtp = cmtbuf;
	unsigned int state = 0;
/*
 * 0 = fundamental, 1 = in quotes, 2 = escaped in quotes,
 * 3 = in angle brackets, 4 = in parentheses
 */

	unsigned int plevel = 0;
	/* Parentheses nesting level */

	unsigned int atom_type = 0;
	/* 0 = unknown, 1 = address */

	*asp = *cmtp = '\0';
	for (; *ap; ap++) {
		switch (state) {
			case 0 :
				switch (*ap) {
					case '\"' :
						*atom_p = '\0';
						quoted_p = quoted_buf;
						*(quoted_p++) = '\"';
						state = 1;
						break;
					case '<' :
						*atom_p = '\0';
						APPEND_TO (cmtp, atom_buf);
						atom_p = atom_buf;
						atom_type = 0;
						asp = asbuf;
						state = 3;
						break;
					case '(' :
						*atom_p = '\0';
						APPEND_TO (asp, atom_buf);
						atom_p = atom_buf;
						atom_type = 0;
						plevel++;
						state = 4;
						break;
					case ' ' : case '\t' :
						if (atom_type == 1) {
							*atom_p = '\0';
							APPEND_TO (asp, atom_buf);
							atom_p = atom_buf;
							atom_type = 0;
						} else
							*(atom_p++) = *ap;
						break;
					default :
						*(atom_p++) = *ap;
						break;
				}
				break;
			case 1 :
				if (*ap == '\"') {
					switch (*(ap + 1)) {
						case '@' : case '%' :
							*(quoted_p++) = '\"'; *quoted_p = '\0';
							APPEND_TO (asp, quoted_buf);
							APPEND_TO (cmtp, atom_buf);
							atom_type = 1;
							break;
						default :
							*quoted_p = '\0';
							APPEND_TO (asp, atom_buf);
							APPEND_TO (cmtp, quoted_buf + 1);
							break;
					}
					state = 0;
					break;
				} else if (*ap == '\\')
					state = 2;
				*(quoted_p++) = *ap;
				break;
			case 2 :
				*(quoted_p++) = *ap;
				state = 1;
				break;
			case 3 :
				if (*ap == '>') {
					*asp = '\0';
					state = 0;
				} else
					*(asp++) = *ap;
				break;
			case 4 :
				switch (*ap) {
					case ')' :
						if (!--plevel) {
							*cmtp = '\0';
							state = 0;
						} else
							*(cmtp++) = *ap;
						break;
					case '(' :
						plevel++;
						nobreak; /* FALLTHROUGH */
					default :
						*(cmtp++) = *ap;
					break;
				}
				break;
			default :
				/* Does not happen. */
				goto FATAL;
		}
	}
	*cmtp = *asp = *atom_p = '\0';
	if (!state) {
		if ((atom_type == 1) || !*asbuf) {
			APPEND_TO (asp, atom_buf);
		} else {
			APPEND_TO (cmtp, atom_buf);
		}
	}
	/* Address specifier */
	TRIM (asbuf, asp);
	/* Comment */
	TRIM (cmtbuf, cmtp);
	strcpy (addrspec, asp);
	strcpy (comment, cmtp);
	return;
FATAL:
	strcpy(addrspec, "error@hell");
	*comment = '\0';
}
#	undef APPEND_TO
#	undef RTRIM
#	undef LTRIM
#	undef TRIM

#endif /* 0 */

/*
 *  Return a pointer into s eliminating any leading Re:'s.  Example:
 *
 *	  Re: Reorganization of misc.jobs
 *	  ^   ^
 *    Re^2: Reorganization of misc.jobs
 *
 *  now also strips trailing (was: ...) (obw)
 */
const char *
eat_re (
	char *s,
	t_bool eat_was)
{
	int data, slen;
	int offsets[6];
	int size_offsets = sizeof(offsets)/sizeof(int);

	if (!s || !*s)
		return "<No subject>";

	do {
		slen = strlen(s);
		data = pcre_exec(strip_re_regex.re, strip_re_regex.extra, s, slen, 0, 0, offsets, size_offsets);
		if (offsets[0] == 0)
			s += offsets[1];
	} while (data > 0);

	if (eat_was) do {
		slen = strlen(s);
		data = pcre_exec(strip_was_regex.re, strip_was_regex.extra, s, slen, 0, 0, offsets, size_offsets);
		if (offsets[0] > 0)
			s[offsets[0]] = '\0';
	} while (data > 0);

	return s;
}


/*
 * Clear tag status of all articles. If articles were untagged, return TRUE
 * FIXME: Move to same place as other tagging code
 */
t_bool
untag_all_articles (
	void)
{
	t_bool untagged = FALSE;
	register int i;

	for (i = 0; i < top; i++) {
		if (arts[i].tagged) {
			arts[i].tagged = 0;
			untagged = TRUE;
		}
	}
	num_of_tagged_arts = 0;

	return untagged;
}


int
my_isprint (
	int c)
{
#ifndef NO_LOCALE
	/* use locale */
	return isprint(c);
#else
#	ifdef LOCAL_CHARSET
		/* use some conversation table */
		return (isprint(c) || (c>=0x80 && c<=0xff));
#	else
		/* assume iso-8859-1 */
		return (isprint(c) || (c>=0xa0 && c<=0xff));
#	endif /* LOCAL_CHARSET */
#endif /* !NO_LOCALE */
}


/*
 * Returns author information
 * thread   if true, assume we're on thread menu and show all author info if
 *          subject not shown
 * art      ptr to article
 * str      ptr in which to return the author. Must be a valid data area
 * len      max length of data to return
 *
 * The data will be null terminated
 */
void
get_author (
	t_bool thread,
	struct t_article *art,
	char *str, size_t len)
{
	int author;

	author = ((thread && !show_subject) ? SHOW_FROM_BOTH : CURR_GROUP.attribute->show_author);

	switch (author) {
		case SHOW_FROM_NONE:
			str[0] = '\0';
			break;
		case SHOW_FROM_ADDR:
			strncpy (str, art->from, len);
			break;
		case SHOW_FROM_NAME:
			strncpy (str, (art->name ? art->name : art->from), len);
			break;
		case SHOW_FROM_BOTH:
			if (art->name) {
				char buff[LEN];			/* TODO eliminate this with snprintf() */

				sprintf (buff, "%s <%s>", art->name, art->from);
				strncpy (str, buff, len);
			} else
				strncpy (str, art->from, len);
			break;
		default:
			break;
	}

	*(str + len) = '\0';				/* NULL terminate */
}


void
toggle_inverse_video (
	void)
{
	tinrc.inverse_okay = !tinrc.inverse_okay;
	if (tinrc.inverse_okay) {
#ifndef USE_INVERSE_HACK
		tinrc.draw_arrow = FALSE;
#endif /* !USE_INVERSE_HACK */
	} else {
		tinrc.draw_arrow = TRUE;
	}
}


void
show_inverse_video_status (
	void)
{
		info_message ((tinrc.inverse_okay ? txt_inverse_on : txt_inverse_off));
}


#ifdef HAVE_COLOR
t_bool
toggle_color (
	void)
{
#	ifdef USE_CURSES
	if (!has_colors()) {
		use_color = FALSE;
		info_message (txt_no_colorterm);
		return FALSE;
	} else
#	endif /* USE_CURSES */
		use_color = !use_color;

	return TRUE;
}


void
show_color_status (
	void)
{
	info_message ((use_color ? txt_color_on : txt_color_off));
}
#endif /* HAVE_COLOR */


/* moved from art.c */
#ifdef WIN32
	/* Don't want the overhead of windows.h */
	int kbhit(void);
#endif /* WIN32 */


/*
 * Check for lock file to stop multiple copies of tind or tin -U running
 * and if it does not exist create it so this is the only copy running
 */
void
create_index_lock_file (
	char *the_lock_file)
{
	char buf[64];
	FILE *fp;
	time_t epoch;
	struct stat sb;

	if (stat (the_lock_file, &sb) == 0) {
		if ((fp = fopen (the_lock_file, "r")) != (FILE *) 0) {
			fgets (buf, (int) sizeof(buf), fp);
			fclose (fp);
#ifdef INDEX_DAEMON
			error_message ("%s: Already started pid=[%d] on %s",
				tin_progname, atoi(buf), buf+8);
#else
			error_message ("\n%s: Already started pid=[%d] on %s",
				tin_progname, atoi(buf), buf+8);
#endif /* INDEX_DAEMON */
			giveup();
		}
	} else {
		if ((fp = fopen (the_lock_file, "w")) != (FILE *) 0) {
			(void) time (&epoch);
			fprintf (fp, "%6d  %s\n", (int) process_id, ctime (&epoch));
			fclose (fp);
			chmod (the_lock_file, (mode_t)(S_IRUSR|S_IWUSR));
		}
	}
}


/*
 * strfquote() - produce formatted quote string
 *   %A  Articles Email address
 *   %D  Articles Date
 *   %F  Articles Address+Name
 *   %G  Groupname of Article
 *   %M  Articles MessageId
 *   %N  Articles Name of author
 *   %C  First Name of author
 *   %I  Initials of author
 */
int
strfquote (
	char *group,
	int respnum,
	char *s,
	size_t maxsize,
	char *format)
{
	char *endp = s + maxsize;
	char *start = s;
	char tbuf[LEN];
	int i, j;
	t_bool iflag;

	if (s == (char *) 0 || format == (char *) 0 || maxsize == 0)
		return 0;

	if (strchr (format, '%') == (char *) 0 && strlen (format) + 1 >= maxsize)
		return 0;

	for (; *format && s < endp - 1; format++) {
		tbuf[0] = '\0';

		if (*format != '\\' && *format != '%') {
			*s++ = *format;
			continue;
		}

		if (*format == '\\') {
			switch (*++format) {
				case '\0':
					*s++ = '\\';
					goto out;
				case 'n':	/* linefeed */
					strcpy (tbuf, "\n");
					break;
				case 't':	/* tab */
					strcpy (tbuf, "\t");
					break;
				default:
					tbuf[0] = '%';
					tbuf[1] = *format;
					tbuf[2] = '\0';
					break;
			}
			i = strlen(tbuf);
			if (i) {
				if (s + i < endp - 1) {
					strcpy (s, tbuf);
					s += i;
				} else
					return 0;
			}
		}
		if (*format == '%') {
			switch (*++format) {
				case '\0':
					*s++ = '%';
					goto out;
				case '%':
					*s++ = '%';
					continue;
				case 'A':	/* Articles Email address */
					strcpy (tbuf, arts[respnum].from);
					break;
				case 'D':	/* Articles Date */
					strcpy(tbuf, note_h.date);
					break;
				case 'F':	/* Articles Address+Name */
					if (arts[respnum].name) {
						sprintf (tbuf, "%s <%s>",
							arts[respnum].name,
							arts[respnum].from);
					} else
						strcpy (tbuf, arts[respnum].from);
					break;
				case 'G':	/* Groupname of Article */
					strcpy (tbuf, group);
					break;
				case 'I':	/* Initials of author */
					strcpy (tbuf, ((arts[respnum].name != (char *) 0) ? arts[respnum].name : arts[respnum].from));
					j = 0;
					iflag = TRUE;
					for (i=0; tbuf[i]; i++) {
						if (iflag) {
							tbuf[j++] = tbuf[i];
							iflag = FALSE;
						}
						if (strchr(" ._@", tbuf[i]))
							iflag = TRUE;
					}
					tbuf[j] = '\0';
					break;
				case 'M':	/* Articles MessageId */
					strcpy (tbuf, note_h.messageid);
					break;
				case 'N':	/* Articles Name of author */
					strcpy (tbuf, ((arts[respnum].name != (char *) 0) ? arts[respnum].name : arts[respnum].from));
					break;
				case 'C':	/* First Name of author */
					if (arts[respnum].name != (char *) 0) {
						strcpy (tbuf, arts[respnum].name);
						if (strrchr (arts[respnum].name, ' '))
							*(strrchr (tbuf, ' ')) = '\0';
					} else
						strcpy (tbuf, arts[respnum].from);
					break;
				default:
					tbuf[0] = '%';
					tbuf[1] = *format;
					tbuf[2] = '\0';
					break;
			}
			i = strlen(tbuf);
			if (i) {
				if (s + i < endp - 1) {
					strcpy (s, tbuf);
					s += i;
				} else
					return 0;
			}
		}
	}
out:
	if (s < endp && *format == '\0') {
		*s = '\0';
		return (s - start);
	} else
		return 0;
}


/*
 * strfeditor() - produce formatted editor string
 *   %E  Editor
 *   %F  Filename
 *   %N  Linenumber
 */
static int
strfeditor (
	char *editor,
	int linenum,
	char *filename,
	char *s,
	size_t maxsize,
	char *format)
{
	char *endp = s + maxsize;
	char *start = s;
	char tbuf[PATH_LEN];
	int i;

	if (s == (char *) 0 || format == (char *) 0 || maxsize == 0)
		return 0;

	if (strchr (format, '%') == (char *) 0 && strlen (format) + 1 >= maxsize)
		return 0;

	for (; *format && s < endp - 1; format++) {
		tbuf[0] = '\0';

		if (*format != '\\' && *format != '%') {
			*s++ = *format;
			continue;
		}

		if (*format == '\\') {
			switch (*++format) {
				case '\0':
					*s++ = '\\';
					goto out;
				case 'n':	/* linefeed */
					strcpy (tbuf, "\n");
					break;
#ifdef WIN32
				case '\"':
					strcpy (tbuf, "\\\"");
					break;
#endif /* WIN32 */
				default:
					tbuf[0] = '%';
					tbuf[1] = *format;
					tbuf[2] = '\0';
					break;
			}
			i = strlen(tbuf);
			if (i) {
				if (s + i < endp - 1) {
					strcpy (s, tbuf);
					s += i;
				} else
					return 0;
			}
		}
		if (*format == '%') {
			switch (*++format) {
				case '\0':
					*s++ = '%';
					goto out;
				case '%':
					*s++ = '%';
					continue;
				case 'E':	/* Editor */
					strcpy (tbuf, editor);
					break;
				case 'F':	/* Filename */
					strcpy(tbuf, filename);
					break;
				case 'N':	/* Line number */
					sprintf (tbuf, "%d", linenum);
					break;
				default:
					tbuf[0] = '%';
					tbuf[1] = *format;
					tbuf[2] = '\0';
					break;
			}
			i = strlen(tbuf);
			if (i) {
				if (s + i < endp - 1) {
					strcpy (s, tbuf);
					s += i;
				} else
					return 0;
			}
		}
	}
out:
	if (s < endp && *format == '\0') {
		*s = '\0';
		return (s - start);
	} else
		return 0;
}


/*
 * strfpath - produce formatted pathname expansion. Handles following forms:
 *   ~/News    -> $HOME/News
 *   ~abc/News -> /usr/abc/News
 *   $var/News -> /env/var/News
 *   =file     -> $HOME/Mail/file
 *   +file     -> tinrc.default_savedir/group.name/file
 *   ~/News/%G -> $HOME/News/group.name
 *
 * Inputs:
 *   format		The string to be converted
 *   str			Return buffer
 *   maxsize	Size of str
 *   dir/group	The strings to be substituted in this case
 * Returns:
 *   0			on error
 *   !0			in all other cases
 */
int
strfpath (
	char *format,
	char *str,
	size_t maxsize,
	char *the_homedir,
	char *maildir,
	char *savedir,
	char *group)
{
	char *endp = str + maxsize;
	char *start = str;
	char *envptr;
	char *startp = format;
	char buf[PATH_LEN];
	char tbuf[PATH_LEN];
	char defbuf[PATH_LEN];
	char tmp[PATH_LEN];
	int i;
#ifndef M_AMIGA
	struct passwd *pwd;
#endif /* !M_AMIGA */

	if (str == (char *) 0 || format == (char *) 0 || maxsize == 0)
		return 0;

	if (strlen (format) + 1 >= maxsize)
		return 0;

	for (; *format && str < endp - 1; format++) {
		tbuf[0] = '\0';

		/*
		 * If just a normal part of the pathname copy it
		 */
#ifdef VMS
		if (!strchr ("~=+%", *format))
#else
		if (!strchr ("~$=+%", *format))
#endif /* VMS */
		{
			*str++ = *format;
			continue;
		}

		switch (*format) {
			case '~':	/* Users or another users homedir */
				switch (*++format) {
					case '/':	/* users homedir */
						joinpath (tbuf, the_homedir, "");
						break;
					default:	/* some other users homedir */
#ifndef M_AMIGA
						i = 0;
						while (*format && *format != '/')
							tbuf[i++] = *format++;
						tbuf[i] = '\0';
						/*
						 * OK lookup the username in /etc/passwd
						 */
						pwd = getpwnam (tbuf);
						if (pwd == (struct passwd *) 0) {
							str[0] = '\0';
							return 0;
						} else
							sprintf (tbuf, "%s/", pwd->pw_dir);
#else
						/* Amiga has no other users */
						return 0;
#endif /* !M_AMIGA */
						break;
				}
				i = strlen (tbuf);
				if (i) {
					if (str + i < endp - 1) {
						strcpy (str, tbuf);
						str += i;
					} else {
						str[0] = '\0';
						return 0;
					}
				}
				break;
#ifndef VMS
			case '$':	/* Read the envvar and use its value */
				i = 0;
				format++;
				if (*format && *format == '{') {
					format++;
					while (*format && !(strchr("}-", *format)))
						tbuf[i++] = *format++;
					tbuf[i] = '\0';
					i = 0;
					if (*format && *format == '-') {
						format++;
						while (*format && *format != '}')
							defbuf[i++] = *format++;
					}
					defbuf[i] = '\0';
				} else {
					while (*format && *format != '/')
						tbuf[i++] = *format++;
					tbuf[i] = '\0';
					format--;
					defbuf[0] = '\0';
				}
				/*
				 * OK lookup the variable in the shells environment
				 */
				envptr = getenv (tbuf);
				if (envptr == (char *) 0 || (*envptr == '\0'))
					strncpy (tbuf, defbuf, sizeof(tbuf)-1);
				else
					strncpy (tbuf, envptr, sizeof(tbuf)-1);
				i = strlen (tbuf);
				if (i) {
					if (str + i < endp - 1) {
						strcpy (str, tbuf);
						str += i;
					} else {
						str[0] = '\0';
						return 0;
					}
				} else {
					str[0] = '\0';
					return 0;
				}
				break;
#endif /* !VMS */
			case '=':
				/*
				 * Shorthand for group maildir
				 * Only convert if 1st char in format
				 */
				if (startp == format && maildir != (char *) 0) {
					joinpath (tbuf, maildir, "");
					i = strlen (tbuf);
					if (i) {
						if (str + i < endp - 1) {
							strcpy (str, tbuf);
							str += i;
						} else {
							str[0] = '\0';
							return 0;
						}
					}
				} else
					*str++ = *format;
				break;
			case '+':
				/*
				 * Shorthand for saving to savedir/groupname/file
				 * Only convert if 1st char in format
				 */
				if (startp == format && savedir != (char *) 0) {
					if (strfpath (savedir, buf, sizeof(buf), the_homedir,
					    (char *) 0, (char *) 0, (char *) 0)) {

#ifdef HAVE_LONG_FILE_NAMES
						my_strncpy (tmp, group, sizeof(tmp));
#else
						my_strncpy (tmp, group, 14);
#endif /* HAVE_LONG_FILE_NAMES */
#if 0 /* this looks ugly */
						/* convert 1st letter to uppercase */
						tmp[0] = (char) toupper(tmp[0]);
#endif /* 0 */
#ifndef VMS
						joinpath (tbuf, buf, tmp);
#	ifdef WIN32
						strcat (tbuf, "\\");
#	else
						strcat (tbuf, "/");
#	endif /* WIN32 */
#else
			joindir (tbuf, buf, tmp);
#endif /* !VMS */
						i = strlen (tbuf);
						if (i) {
							if (str + i < endp - 1) {
								strcpy (str, tbuf);
								str += i;
							} else {
								str[0] = '\0';
								return 0;
							}
						}
					} else {
						str[0] = '\0';
						return 0;
					}
				} else
					*str++ = *format;
				break;
			case '%':	/* Different forms of parsing cmds */
				format++;
				if (*format && *format == 'G') {
					memset(tbuf, 0, sizeof(tbuf));
					STRCPY(tbuf, group);
					i = strlen(tbuf);
					if (((str + i) < (endp - 1)) && (i > 0)) {
						strcpy(str, tbuf);
						str += i;
						break;
					} else {
						str[0] = '\0';
						return 0;
					}
				} else
					*str++ = *format;
				nobreak; /* FALLTHROUGH */
			default:
				break;
		}
	}

	if (str < endp && *format == '\0') {
		*str = '\0';
#if 0
	wait_message (2, "!!!format=[%s]  path=[%s]", startp, start);
#endif /* 0 */
		return (str - start);
	} else {
		str[0] = '\0';
		return 0;
	}
}


enum quote_enum {
	no_quote = 0,
	dbl_quote,
	sgl_quote };


static char *
escape_shell_meta (
	char *source,
	int quote_area)
{
	static char buf[PATH_LEN];
	char *dest = buf;

	switch (quote_area) {
		case no_quote:
			while (*source) {
				if (*source == '\'' || *source == '\\' || *source == '"' ||
					*source == '$' || *source == '`' || *source == '*' ||
					*source == '&' || *source == '|' || *source == '<' ||
					*source == '>' || *source == ';' || *source == '(' ||
					*source == ')')
					*dest++ = '\\';
				*dest++ = *source++;
			}
			break;

		case dbl_quote:
			while (*source) {
				if (*source == '\\' || *source == '"' || *source == '$' ||
					*source == '`')
					*dest++ = '\\';
				*dest++ = *source++;
			}
			break;

		case sgl_quote:
			while (*source) {
				if (*source == '\'') {
					*dest++ = '\'';
					*dest++ = '\\';
					*dest++ = '\'';
				}
				*dest++ = *source++;
			}
			break;

		default:
			break;
	}

	*dest = '\0';
	return buf;
}


/*
 * strfmailer() - produce formatted mailer string
 *   %M  Mailer
 *   %F  Filename
 *   %T  To
 *   %S  Subject
 *   %U  User
 */
int
strfmailer (
	char *the_mailer,
	char *subject,
	char *to,
	char *filename,
	char *s,
	size_t maxsize,
	char *format) /* return value is always ignored */
{
	char *endp = s + maxsize;
	char *start = s;
	char tbuf[PATH_LEN];
	int quote_area = no_quote;

	if (s == (char *) 0 || format == (char *) 0 || maxsize == 0)
		return 0;

	if (strchr (format, '%') == (char *) 0 && strlen (format) + 1 >= maxsize)
		return 0;

	for (; *format && s < endp - 1; format++) {
		tbuf[0] = '\0';

		if (*format != '\\' && *format != '%') {
			if (*format == '"' && quote_area != sgl_quote)
				quote_area = (quote_area == dbl_quote ? no_quote : dbl_quote);
			if (*format == '\'' && quote_area != dbl_quote)
				quote_area = (quote_area == sgl_quote ? no_quote : sgl_quote);
			*s++ = *format;
			continue;
		}

		if (*format == '\\') {
			switch (*++format) {
				case '\0':
					*s++ = '\\';
					goto out;
				case 'n':	/* linefeed */
					strcpy (tbuf, "\n");
					break;
				default:
					tbuf[0] = '\\';
					tbuf[1] = *format;
					tbuf[2] = '\0';
					break;
			}
			if (*tbuf) {
				if (sh_format (s, endp - s, "%s", tbuf) >= 0)
					s += strlen(s);
				else
					return 0;
			}
		}
		if (*format == '%') {
			t_bool ismail = TRUE;
			t_bool escaped = FALSE;
			switch (*++format) {
				case '\0':
					*s++ = '%';
					goto out;
				case '%':
					*s++ = '%';
					continue;
				case 'F':	/* Filename */
					STRCPY(tbuf, filename);
					break;
				case 'M':	/* Mailer */
					STRCPY(tbuf, the_mailer);
					break;
				case 'S':	/* Subject */
					if (tinrc.use_mailreader_i) {
						STRCPY(tbuf, escape_shell_meta (subject, quote_area));
					} else {
						STRCPY(tbuf, escape_shell_meta (rfc1522_encode (subject, ismail) , quote_area));
					}
					escaped = TRUE;
					break;
				case 'T':	/* To */
					if (tinrc.use_mailreader_i) {
						STRCPY(tbuf, escape_shell_meta (to, quote_area));
					} else {
						STRCPY(tbuf, escape_shell_meta (rfc1522_encode (to, ismail), quote_area));
					}
					escaped = TRUE;
					break;
				case 'U':	/* User */
					if (tinrc.use_mailreader_i) {
						STRCPY(tbuf, userid);
					} else {
						STRCPY(tbuf, rfc1522_encode (userid, ismail));
					}
					break;
				default:
					tbuf[0] = '%';
					tbuf[1] = *format;
					tbuf[2] = '\0';
					break;
			}
			if (*tbuf) {
				if (escaped) {
					if (endp - s > 0) {
						strncpy(s, tbuf, endp - s);
						s += strlen(s);
					}
				} else if (sh_format (s, endp - s, "%s", tbuf) >= 0) {
					s += strlen(s);
				} else
					return 0;
			}
		}
	}
out:
	if (s < endp && *format == '\0') {
		*s = '\0';
		return (s - start);
	} else
		return 0;
}

/*
 * get_initials() - get initial letters of a posters name
 */
int
get_initials (
	int respnum,
	char *s,
	int maxsize) /* return value is always ignored */
{
	char tbuf[PATH_LEN];
	int i, j;
	t_bool iflag;

	if (s == (char *) 0 || maxsize == 0)
		return 0;

	strcpy (tbuf, ((arts[respnum].name != (char *) 0) ? arts[respnum].name : arts[respnum].from));

	iflag = FALSE;
	j = 0;
	for (i=0; tbuf[i] && j < maxsize-1; i++) {
		if (isalpha((int)tbuf[i])) {
			if (!iflag) {
				s[j++] = tbuf[i];
				iflag = TRUE;
			}
		} else
			iflag = FALSE;
	}
	s[j] = '\0';
	return 0;
}


void get_cwd (
	char *buf)
{
#ifdef HAVE_GETCWD
	getcwd (buf, PATH_LEN);
#else
	getwd (buf);
#endif /* HAVE_GETCWD */
}


/*
 * Convert a newsgroup name to a newsspool path
 * No effect when reading via NNTP
 */
void
make_group_path (
	char *name,
	char *path)
{
#ifdef VMS
	sprintf(path, "[%s]", name);
#else
	while (*name) {
		*path = ((*name == '.') ? '/' : *name);
		name++;
		path++;
	}
	*path = '\0';
#endif /* VMS */
}


/*
 * Delete tmp index & local newsgroups file
 */
void
cleanup_tmp_files (
	void)
{
	char acNovFile[PATH_LEN];

	if (read_news_via_nntp && xover_supported && !tinrc.cache_overview_files) {
		sprintf (acNovFile, "%s%d.idx", TMPDIR, (int) process_id);
		unlink (acNovFile);
	}

	if (!tinrc.cache_overview_files)
		unlink (local_newsgroups_file);

	if (batch_mode || update_fork)
		unlink (lock_file);
}

#if !defined(M_UNIX)
void
make_post_process_cmd (
	char *cmd,
	char *dir,
	char *file)
{
	char buf[LEN];
	char currentdir[PATH_LEN];

	get_cwd (currentdir);
	my_chdir (dir);
#	ifdef M_OS2
	backslash (file);
#	endif /* M_OS2 */
	sh_format (buf, sizeof(buf), cmd, file);
	invoke_cmd (buf);
	my_chdir (currentdir);
}
#endif /*! M_UNIX */


/*
 * returns filesize in bytes
 * -1 in case of an error (file not found, or !S_IFREG)
 */
long /* we use long here as off_t might be unsigned on some systems */
file_size (
	char *file)
{
	struct stat statbuf;

	return (stat (file, &statbuf) == -1 ? -1L : (S_ISREG(statbuf.st_mode)) ? (long) statbuf.st_size : -1L);
}

/*
 * returns mtime
 * -1 in case of an error (file not found, or !S_IFREG)
 */
long /* we use long here for file_changed() macro */
file_mtime (
	char *file)
{
	struct stat statbuf;

	return (stat (file, &statbuf) == -1 ? -1L : (S_ISREG(statbuf.st_mode)) ? (long) statbuf.st_mtime : -1L);
}


void
vPrintBugAddress (
	void)
{
	my_fprintf (stderr, "%s %s %s (\"%s\") [%s]: send a DETAILED bug report to %s\n",
		tin_progname, VERSION, RELEASEDATE, RELEASENAME, OSNAME, BUG_REPORT_ADDRESS);
	my_fflush (stderr);
}


/*
 *  Copy file from pcSrcFile to pcDstFile
 *  FIXME: Used only in mail.c, rework using copy_fp and junk this
 */
t_bool
copy_file (
	char *pcSrcFile,
	char *pcDstFile)
{
	FILE *hFpDst;
	FILE *hFpSrc;
	char acBuffer[8192];
	int iReadOk = -1;
	int iWriteOk = -1;
	long lCurFilePos = 0L;
	long lSrcFilePos = 0L;
	size_t iWriteSize = 0;
	t_bool retcode = FALSE;

	if ((hFpSrc = fopen (pcSrcFile, "r")) != (FILE *) 0) {
		if ((hFpDst = fopen (pcDstFile, "w")) != (FILE *) 0) {
			while (!feof (hFpSrc) && (iReadOk = (int) fread (acBuffer, sizeof(acBuffer), 1, hFpSrc)) != -1) {
				lCurFilePos = ftell (hFpSrc);
				iWriteSize = (size_t) (lCurFilePos - lSrcFilePos);
				lSrcFilePos = lCurFilePos;
				if ((iWriteOk = (int) fwrite (acBuffer, iWriteSize, 1, hFpDst)) == -1)
					break;
			}
			if (iReadOk != -1 && iWriteOk != -1)
				retcode = TRUE;

			fclose (hFpDst);
		}
		fclose (hFpSrc);
	}
	return retcode;
}


/*
 * take a peek at the next char in file
 */
int
peek_char (
	FILE *fp)
{
	int c = fgetc(fp);
	if (c != EOF)
		ungetc(c, fp);
	return c;
}


char *
random_organization (
	char *in_org)
{
	FILE *orgfp;
	int nool = 0, sol;
	static char selorg[512];

	*selorg = '\0';

	if (*in_org != '/')
		return in_org;

	srand ((unsigned int) time(NULL));

	if ((orgfp = fopen(in_org, "r")) == NULL)
		return selorg;

	/* count lines */
	while (fgets(selorg, (int) sizeof(selorg), orgfp))
		nool++;

	if (!nool) {
		fclose(orgfp);
		return selorg;
	}

	rewind (orgfp);
	sol = rand () % nool + 1;
	nool = 0;
	while ((nool != sol) && (fgets(selorg, (int) sizeof(selorg), orgfp)))
		nool++;

	fclose(orgfp);

	return selorg;
}


void
read_input_history_file (
	void)
{
	FILE *fp;
	char *chr;
	char buf[HEADER_LEN];
	int his_w = 0, his_e = 0, his_free = 0;

	/* this is usually .tin/.inputhistory */
	if ((fp = fopen(local_input_history_file, "r")) == NULL)
		return;

	if (INTERACTIVE)
		wait_message (0, txt_reading_input_history_file);

	/* to be safe ;-) */
	memset((void *) input_history, 0, sizeof(input_history));
	memset((void *) hist_last, 0, sizeof(hist_last));
	memset((void *) hist_pos, 0, sizeof(hist_pos));

	while (fgets(buf, (int) sizeof(buf), fp)) {

		if ((chr = strpbrk(buf, "\n\r")) != NULL)
			*chr = '\0';

		if (*buf)
			input_history[his_w][his_e] = my_strdup(buf);
		else {
			/* empty lines in tin_getline's history buf are stored as NULL pointers */
			input_history[his_w][his_e] = NULL;

			/* get the empty slot in the circular buf */
			if (!his_free)
				his_free = his_e;
		}

		his_e++;
		/* check if next type is reached */
		if (his_e >= HIST_SIZE) {
			hist_last[his_w] = his_free;
			hist_pos[his_w] = hist_last[his_w];
			his_free = his_e = 0;
			his_w++;
		}
		/* check if end is reached */
		if (his_w > HIST_MAXNUM)
			break;
	}
	fclose(fp);

	if (cmd_line)
		printf ("\r\n");
}


static void
write_input_history_file (
	void)
{
	FILE *fp;
	char *chr;
	int his_w, his_e;
	mode_t mask;

	if (no_write)
		return;

	mask = umask((mode_t) (S_IRWXO|S_IRWXG));

	if ((fp = fopen(local_input_history_file, "w")) == NULL) {
		umask(mask);
		return;
	}

	for (his_w = 0; his_w <= HIST_MAXNUM; his_w++) {
		for (his_e = 0; his_e < HIST_SIZE; his_e++) {
			/* write an empty line for empty slots */
			if (input_history[his_w][his_e] == NULL)
				fprintf(fp, "\n");
			else {
				if ((chr = strpbrk(input_history[his_w][his_e], "\n\r")) != NULL)
					*chr = '\0';
				fprintf(fp, "%s\n", input_history[his_w][his_e]);
			}
		}
	}

	fclose(fp);
	umask(mask);
	/* fix modes for all pre 1.4.1 local_input_history_file files */
	chmod (local_input_history_file, (mode_t)(S_IRUSR|S_IWUSR));
}


/*
 * quotes wildcards * ? \ [ ] with \
 */
char *
quote_wild (
	char *str)
{
	char *target;
	static char buff[2*LEN];	/* on the safe side */

	for (target = buff; *str != '\0'; str++) {
		if (tinrc.wildcard) { /* regex */
			/*
			 * quote meta characters ()[]{}\^$*+?.#
			 * replace whitespace with '\s' (pcre)
			 */
			if (*str == '(' || *str == ')' || *str == '[' || *str == ']' || *str == '{' || *str == '}'
			    || *str == '\\' || *str == '^' || *str == '$'
			    || *str == '*' || *str == '+' || *str == '?' || *str == '.'
			    || *str == '#'
			    || *str == ' ' || *str == '\t') {
				*target++ = '\\';
				*target++ = ((*str == ' ' || *str == '\t')? 's' : *str);
			} else
				*target++ = *str;
		} else {	/* wildmat */
			if (*str == '*' || *str == '\\' || *str == '[' || *str == ']' || *str == '?')
				*target++ = '\\';
			*target++ = *str;
		}
	}
	*target = '\0';
	return (buff);
}


/*
 * quotes whitespace in regexps for pcre
 */
char *
quote_wild_whitespace (
	char *str)
{
	char *target;
	static char buff[2*LEN];	/* on the safe side */

	for (target = buff; *str != '\0'; str++) {
		if (tinrc.wildcard) { /* regex */
			/*
			 * replace whitespace with '\s' (pcre)
			 */
			if (*str == ' ' || *str == '\t') {
				*target++ = '\\';
				*target++ = 's';
			} else
				*target++ = *str;
		} else	/* wildmat */
			*target++ = *str;
	}
	*target = '\0';
	return (buff);
}



/*
 * strip_address () removes the address part from a given e-mail address
 */
void
strip_address (
	char *the_address,
	char *stripped_address)
{
	char *end_pos;
	char *start_pos;

	if (strchr(the_address, '@') != (char *) 0) {
		if ((end_pos = strchr(the_address,'<')) == (char *) 0) {
			if ((start_pos = strchr(the_address, ' ')) == (char *) 0)
				strcpy (stripped_address, the_address);
			else {
				strcpy (stripped_address, start_pos + 2);
				if (stripped_address[strlen(stripped_address) - 1] == ')')
					stripped_address[strlen(stripped_address) - 1] = '\0';
			}
		} else
			if (end_pos > the_address)
				strncpy (stripped_address, the_address, end_pos - the_address - 1);
			else
				strcpy (stripped_address, the_address);
	} else {
		if (the_address[0] == '(')
			strcpy (stripped_address, the_address + 1);
		else
			strcpy (stripped_address, the_address);
		if (stripped_address[strlen(stripped_address) - 1] == ')')
			stripped_address[strlen(stripped_address) - 1] = '\0';
	}
}


/*
 * strip_name () removes the realname part from a given e-mail address
 */
void
strip_name (
	char *the_address,
	char *stripped_address)
{
	char *end_pos;
	char *start_pos;

	/* skip realname in address */
	if ((start_pos = strchr (the_address, '<')) == (char *) 0) {
		/* address in user@domain (realname) syntax or realname is missing */
		strcpy (stripped_address, the_address);
		start_pos = stripped_address;
		if ((end_pos = strchr (start_pos, ' ')) == (char *) 0)
			end_pos = start_pos+strlen(start_pos);
	} else {
		start_pos++; /* skip '<' */
		strcpy (stripped_address, start_pos);
		start_pos = stripped_address;
		if ((end_pos = strchr (start_pos, '>')) == (char *) 0)
			end_pos = start_pos+strlen(start_pos); /* skip '>' */
	}
	*(end_pos) = '\0';
}


/*
 * Return the new line index following a PageUp request.
 * Take half page scrolling into account
 */
int
page_up (
	int curslot,
	int maxslot)
{
	int n, scroll_lines;

	if (curslot == 0)
		return (maxslot - 1);

	scroll_lines = (tinrc.full_page_scroll ? NOTESLINES : NOTESLINES / 2);

	if ((n = curslot % scroll_lines) > 0)
		curslot -= n;
	else
		curslot = ((curslot - scroll_lines) / scroll_lines) * scroll_lines;

	return ((curslot < 0) ? 0 : curslot);
}

/*
 * Return the new line index following a PageDown request.
 * Take half page scrolling into account
 */
int
page_down (
	int curslot,
	int maxslot)
{
	int scroll_lines;

	if (curslot == maxslot - 1)
		return 0;

	scroll_lines = (tinrc.full_page_scroll ? NOTESLINES : NOTESLINES / 2);

	curslot = ((curslot + scroll_lines) / scroll_lines) * scroll_lines;

	if (curslot >= maxslot) {
		curslot = (maxslot / scroll_lines) * scroll_lines;
		if (curslot < maxslot - 1)
			curslot = maxslot - 1;
	}

	return (curslot);
}


/*
 * Calculate the first and last objects that will appear on the current screen
 * based on the current position and the max available
 */
void
set_first_screen_item (
	int cur,
	int max,
	int *first,
	int *last)
{
	if (NOTESLINES <= 0)
		*first = 0;
	else {
		*first = (cur / NOTESLINES) * NOTESLINES;
		if (*first < 0)
			*first = 0;
	}

	*last = *first + NOTESLINES;

	if (*last >= max) {
		*last = max;
		*first = (max / NOTESLINES) * NOTESLINES;

		if (*first == *last || *first < 0)
			*first = ((*first < 0) ? 0 : *last - NOTESLINES);
	}

	if (!max)
		*first = *last = 0;
}


#ifdef LOCAL_CHARSET
/*
 * convert between local and network charset (e.g. NeXT and latin1)
 */

#	define CHARNUM 256
#	define BAD (-1)

/* use the appropriate conversion tables */
#	if LOCAL_CHARSET == 437
#		include "l1_ibm437.tab"
#		include "ibm437_l1.tab"
#	else
#		if LOCAL_CHARSET == 850
#			include "l1_ibm850.tab"
#			include "ibm850_l1.tab"
#		else
#			include "l1_next.tab"
#			include "next_l1.tab"
#		endif /* 850 */
#	endif /* 437 */

static int
to_local (
	int c)
{
	if (use_local_charset) {
		c = c_network_local[(unsigned char)c];
		if (c == BAD)
			return '?';
		else
			return c;
	} else
		return c;
}

void
buffer_to_local (
	char *b)
{
	for(; *b; b++)
		*b = to_local(*b);
}

static int
to_network (
	int c)
{
	if (use_local_charset) {
		c = c_local_network[(unsigned char) c];
		if (c == BAD)
			return '?';
		else
			return c;
	} else
		return c;
}

void
buffer_to_network (
	char *b)
{
	for(; *b; b++)
		*b = to_network(*b);
}
#endif /* LOCAL_CHARSET */


/*
 * checking of mail adresses for GNKSA compliance
 *
 * son-of-rfc1036:
 *   article         = 1*header separator body
 *   header          = start-line *continuation
 *   start-line      = header-name ":" space [ nonblank-text ] eol
 *   continuation    = space nonblank-text eol
 *   header-name     = 1*name-character *( "-" 1*name-character )
 *   name-character  = letter / digit
 *   letter          = <ASCII letter A-Z or a-z>
 *   digit           = <ASCII digit 0-9>
 *   separator       = eol
 *   body            = *( [ nonblank-text / space ] eol )
 *   eol             = <EOL>
 *   nonblank-text   = [ space ] text-character *( space-or-text )
 *   text-character  = <any ASCII character except NUL (ASCII 0),
 *                       HT (ASCII 9), LF (ASCII 10), CR (ASCII 13),
 *                       or blank (ASCII 32)>
 *   space           = 1*( <HT (ASCII 9)> / <blank (ASCII 32)> )
 *   space-or-text   = space / text-character
 *   encoded-word  = "=?" charset "?" encoding "?" codes "?="
 *   charset       = 1*tag-char
 *   encoding      = 1*tag-char
 *   tag-char      = <ASCII printable character except !()<>@,;:\"[]/?=>
 *   codes         = 1*code-char
 *   code-char     = <ASCII printable character except ?>
 *   From-content  = address [ space "(" paren-phrase ")" ]
 *                 /  [ plain-phrase space ] "<" address ">"
 *   paren-phrase  = 1*( paren-char / space / encoded-word )
 *   paren-char    = <ASCII printable character except ()<>\>
 *   plain-phrase  = plain-word *( space plain-word )
 *   plain-word    = unquoted-word / quoted-word / encoded-word
 *   unquoted-word = 1*unquoted-char
 *   unquoted-char = <ASCII printable character except !()<>@,;:\".[]>
 *   quoted-word   = quote 1*( quoted-char / space ) quote
 *   quote         = <" (ASCII 34)>
 *   quoted-char   = <ASCII printable character except "()<>\>
 *   address       = local-part "@" domain
 *   local-part    = unquoted-word *( "." unquoted-word )
 *   domain        = unquoted-word *( "." unquoted-word )
*/


/*
 * legal domain name components according to RFC 1034
 * includes also '.' as valid separator
 */
static char gnksa_legal_fqdn_chars[256] = {
/*         0 1 2 3  4 5 6 7  8 9 a b  c d e f */
/* 0x00 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0x10 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0x20 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,1,1,0,
/* 0x30 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,
/* 0x40 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
/* 0x50 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
/* 0x60 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
/* 0x70 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
/* 0x80 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0x90 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xa0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xb0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xc0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xd0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xe0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xf0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};


/*
 * legal localpart components according to son-of-rfc1036
 * includes also '.' as valid separator
 */
static char gnksa_legal_localpart_chars[256] = {
/*         0 1 2 3  4 5 6 7  8 9 a b  c d e f */
/* 0x00 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0x10 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0x20 */ 0,0,0,1, 1,1,1,1, 0,0,1,1, 0,1,1,1,
/* 0x30 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,1,0,1,
/* 0x40 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
/* 0x50 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,1,1,
/* 0x60 */ 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
/* 0x70 */ 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,0,
/* 0x80 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0x90 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xa0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xb0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xc0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xd0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xe0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xf0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};


/*
 * legal realname characters according to son-of-rfc1036
 */
static char gnksa_legal_realname_chars[256] = {
/*         0 1 2 3  4 5 6 7  8 9 a b  c d e f */
/* 0x00 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0x10 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0x20 */ 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
/* 0x30 */ 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
/* 0x40 */ 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
/* 0x50 */ 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
/* 0x60 */ 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
/* 0x70 */ 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,0,
/* 0x80 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0x90 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xa0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xb0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xc0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xd0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xe0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
/* 0xf0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};


/*
 * return error message string for given code
 */
const char *
gnksa_strerror (
	int errcode)
{
	const char *message;

	switch (errcode) {
		case GNKSA_INTERNAL_ERROR:
			message = txt_error_gnksa_internal;
			break;

		case GNKSA_LANGLE_MISSING:
			message = txt_error_gnksa_langle;
			break;

		case GNKSA_LPAREN_MISSING:
			message = txt_error_gnksa_lparen;
			break;

		case GNKSA_RPAREN_MISSING:
			message = txt_error_gnksa_rparen;
			break;

		case GNKSA_ATSIGN_MISSING:
			message = txt_error_gnksa_atsign;
			break;

		case GNKSA_SINGLE_DOMAIN:
			message = txt_error_gnksa_sgl_domain;
			break;

		case GNKSA_INVALID_DOMAIN:
			message = txt_error_gnksa_inv_domain;
			break;

		case GNKSA_ILLEGAL_DOMAIN:
			message = txt_error_gnksa_ill_domain;
			break;

		case GNKSA_UNKNOWN_DOMAIN:
			message = txt_error_gnksa_unk_domain;
			break;

		case GNKSA_INVALID_FQDN_CHAR:
			message = txt_error_gnksa_fqdn;
			break;

		case GNKSA_ZERO_LENGTH_LABEL:
			message = txt_error_gnksa_zero;
			break;

		case GNKSA_ILLEGAL_LABEL_LENGTH:
			message = txt_error_gnksa_length;
			break;

		case GNKSA_ILLEGAL_LABEL_HYPHEN:
			message = txt_error_gnksa_hyphen;
			break;

		case GNKSA_ILLEGAL_LABEL_BEGNUM:
			message = txt_error_gnksa_begnum;
			break;

		case GNKSA_BAD_DOMAIN_LITERAL:
			message = txt_error_gnksa_bad_lit;
			break;

		case GNKSA_LOCAL_DOMAIN_LITERAL:
			message = txt_error_gnksa_local_lit;
			break;

		case GNKSA_RBRACKET_MISSING:
			message = txt_error_gnksa_rbracket;
			break;

		case GNKSA_LOCALPART_MISSING:
			message = txt_error_gnksa_lp_missing;
			break;

		case GNKSA_INVALID_LOCALPART:
			message = txt_error_gnksa_lp_invalid;
			break;

		case GNKSA_ZERO_LENGTH_LOCAL_WORD:
			message = txt_error_gnksa_lp_zero;
			break;

		case GNKSA_ILLEGAL_UNQUOTED_CHAR:
			message = txt_error_gnksa_rn_unq;
			break;

		case GNKSA_ILLEGAL_QUOTED_CHAR:
			message = txt_error_gnksa_rn_qtd;
			break;

		case GNKSA_ILLEGAL_ENCODED_CHAR:
			message = txt_error_gnksa_rn_enc;
			break;

		case GNKSA_BAD_ENCODE_SYNTAX:
			message = txt_error_gnksa_rn_encsyn;
			break;

		case GNKSA_ILLEGAL_PAREN_CHAR:
			message = txt_error_gnksa_rn_paren;
			break;

		case GNKSA_INVALID_REALNAME:
			message = txt_error_gnksa_rn_invalid;
			break;

		case GNKSA_OK:
		default:
			/* shouldn't happen */
			message = "";
			break;
	}

	return message;
}


/*
 * decode realname into displayable string
 * this only does RFC822 decoding, decoding RFC2047 encoded parts must
 * be done by another call to the appropriate function
 */
static int
gnksa_dequote_plainphrase (
	char *realname,
	char *decoded,
	int addrtype)
{
	char *rpos;	/* read position */
	char *wpos;	/* write position */
	int initialstate;	/* initial state */
	int state;	/* current state */


	/* initialize state machine */
	switch (addrtype) {
		case GNKSA_ADDRTYPE_ROUTE:
			initialstate = 0;
			break;
		case GNKSA_ADDRTYPE_OLDSTYLE:
			initialstate = 5;
			break;
		default:
			/* shouldn't happen */
			return GNKSA_INTERNAL_ERROR;
			/* NOTREACHED */
			break;
	}
	state = initialstate;
	rpos = realname;
	wpos = decoded;

	/* decode realname */
	while (*rpos) {
		if (!gnksa_legal_realname_chars[(int) *rpos])
			return GNKSA_INVALID_REALNAME;

		switch (state) {
			case 0:
				/* in unquoted word, route address style */
				switch (*rpos) {
					case '"':
						state = 1;
						rpos++;
						break;

					case '!':
					case '(':
					case ')':
					case '<':
					case '>':
					case '@':
					case ',':
					case ';':
					case ':':
					case '\\':
					case '.':
					case '[':
					case ']':
						return GNKSA_ILLEGAL_UNQUOTED_CHAR;
						/* NOTREACHED */
						break;

					case '=':
						*(wpos++) = *(rpos++);
						if ('?' == *rpos) {
							state = 2;
							*(wpos++) = *(rpos++);
						} else
							state = 0;
						break;

					default:
						state = 0;
						*(wpos++) = *(rpos++);
						break;
				}
				break;

			case 1:
				/* in quoted word */
				switch (*rpos) {
					case '"':
						state = 0;
						rpos++;
						break;

					case '(':
					case ')':
					case '<':
					case '>':
					case '\\':
						return GNKSA_ILLEGAL_QUOTED_CHAR;
						/* NOTREACHED */
						break;

					default:
						state = 1;
						*(wpos++) = *(rpos++);
						break;
				}
				break;

			case 2:
				/* in encoded word, charset part */
				switch (*rpos) {
					case '?':
						state = 3;
						*(wpos++) = *(rpos++);
						break;

					case '!':
					case '(':
					case ')':
					case '<':
					case '>':
					case '@':
					case ',':
					case ';':
					case ':':
					case '\\':
					case '"':
					case '[':
					case ']':
					case '/':
					case '=':
						return GNKSA_ILLEGAL_ENCODED_CHAR;
						/* NOTREACHED */
						break;

					default:
						state = 2;
						*(wpos++) = *(rpos++);
						break;
				}
				break;

			case 3:
				/* in encoded word, encoding part */
				switch (*rpos) {
					case '?':
						state = 4;
						*(wpos++) = *(rpos++);
						break;

					case '!':
					case '(':
					case ')':
					case '<':
					case '>':
					case '@':
					case ',':
					case ';':
					case ':':
					case '\\':
					case '"':
					case '[':
					case ']':
					case '/':
					case '=':
						return GNKSA_ILLEGAL_ENCODED_CHAR;
						/* NOTREACHED */
						break;

					default:
						state = 3;
						*(wpos++) = *(rpos++);
						break;
				}
				break;

			case 4:
				/* in encoded word, codes part */
				switch (*rpos) {
					case '?':
						*(wpos++) = *(rpos++);
						if ('=' == *rpos) {
							state = initialstate;
							*(wpos++) = *(rpos++);
						} else
							return GNKSA_BAD_ENCODE_SYNTAX;
						break;

					default:
						state = 4;
						*(wpos++) = *(rpos++);
						break;
					}
				break;

			case 5:
				/* in word, old style address */
				switch (*rpos) {
					case '(':
					case ')':
					case '<':
					case '>':
					case '\\':
						return GNKSA_ILLEGAL_PAREN_CHAR;
						/* NOTREACHED */
						break;

					case '=':
						*(wpos++) = *(rpos++);
						if ('?' == *rpos) {
							state = 2;
							*(wpos++) = *(rpos++);
						} else
							state = 5;
						break;

					default:
						state = 5;
						*(wpos++) = *(rpos++);
						break;
				}
				break;

			default:
				/* shouldn't happen */
				return GNKSA_INTERNAL_ERROR;
		}
	}

	/* successful */
	*wpos = '\0';
	return GNKSA_OK;
}


/*
 * check domain literal
 */
static int
gnksa_check_domain_literal (
	char *domain)
{
	char term;
	int n;
	unsigned int x1, x2, x3, x4;

	/* parse domain literal into ip number */
	x1 = x2 = x3 = x4 = 666;
	term = '\0';

	if ('[' == *domain) { /* literal bracketed */
		n = sscanf(domain, "[%u.%u.%u.%u%c", &x1, &x2, &x3, &x4, &term);
		if (5 != n)
			return GNKSA_BAD_DOMAIN_LITERAL;

		if (']' != term)
			return GNKSA_BAD_DOMAIN_LITERAL;

	} else { /* literal not bracketed */
#ifdef REQUIRE_BRACKETS_IN_DOMAIN_LITERAL
		return GNKSA_RBRACKET_MISSING;
#else
		n = sscanf(domain, "%u.%u.%u.%u%c", &x1, &x2, &x3, &x4, &term);
		/* there should be no terminating character present */
		if (4 != n)
			return GNKSA_BAD_DOMAIN_LITERAL;
#endif /* REQUIRE_BRACKETS_IN_DOMAIN_LITERAL */
	}

	/* check ip parts for legal range */
	if ((255 < x1) || (255 < x2) || (255 < x3) || (255 < x4))
		return GNKSA_BAD_DOMAIN_LITERAL;

	/* check for private ip or localhost */
	if ((!disable_gnksa_domain_check)
	    && ((0 == x1)				/* local network */
		|| (10 == x1)				/* private class A */
		|| ((172 == x1) && (16 == (x2 & 0xf0)))	/* private class B */
		|| ((192 == x1) && (168 == x2))		/* private class C */
		|| (127 == x1)))			/* localhost */
		return GNKSA_LOCAL_DOMAIN_LITERAL;


	return GNKSA_OK;
}


static int
gnksa_check_domain (
	char *domain)
{
	char *aux;
	char *last;
	int i;
	int result;

	/* check for domain literal */
	if ('[' == *domain) /* check value of domain literal */
		return gnksa_check_domain_literal(domain);

	/* check for leading or trailing dot */
	if (('.' == *domain) || ('.' == *(domain+strlen(domain)-1)))
		return GNKSA_ZERO_LENGTH_LABEL;

	/* look for TLD start */
	aux = strrchr(domain, '.');
	if (NULL == aux)
		return GNKSA_SINGLE_DOMAIN;

	aux++;

	/* check existence of toplevel domain */
	switch ((int) strlen(aux)) {
		case 1:
			/* no numeric components allowed */
			if (('0' <= *aux) && ('9' >= *aux))
				return gnksa_check_domain_literal(domain);

			/* single letter TLDs do not exist */
			return GNKSA_ILLEGAL_DOMAIN;
			/* NOTREACHED */
			break;

		case 2:
			/* no numeric components allowed */
			if (('0' <= *aux) && ('9' >= *aux)
			    && ('0' <= *(aux + 1)) && ('9' >= *(aux + 1)))
				return gnksa_check_domain_literal(domain);

			if (('a' <= *aux) && ('z' >= *aux)
			    && ('a' <= *(aux + 1)) && ('z' >= *(aux + 1))) {
				i = ((*aux - 'a') * 26) + (*(aux + 1)) - 'a';
				if (!gnksa_country_codes[i])
					return GNKSA_UNKNOWN_DOMAIN;
			} else
				return GNKSA_ILLEGAL_DOMAIN;
			break;

		case 3:
			/* no numeric components allowed */
			if (('0' <= *aux) && ('9' >= *aux)
			    && ('0' <= *(aux + 1)) && ('9' >= *(aux + 1))
			    && ('0' <= *(aux + 2)) && ('9' >= *(aux + 2)))
				return gnksa_check_domain_literal(domain);
			nobreak; /* FALLTHROUGH */
		default:
			/* check for valid domains */
			result = GNKSA_INVALID_DOMAIN;
			for (i = 0; *gnksa_domain_list[i]; i++) {
				if (!strcmp(aux, gnksa_domain_list[i]))
					result = GNKSA_OK;
			}
			if (disable_gnksa_domain_check)
				result = GNKSA_OK;
			if (GNKSA_OK != result)
				return result;
			break;
	}

	/* check for illegal labels */
	last = domain;
	for (aux = domain; *aux; aux++) {
		if ('.' == *aux) {
			if (aux - last - 1 > 63)
				return GNKSA_ILLEGAL_LABEL_LENGTH;

			if ('.' == *(aux + 1))
				return GNKSA_ZERO_LENGTH_LABEL;

			if (('-' == *(aux + 1)) || ('-' == *(aux - 1)))
				return GNKSA_ILLEGAL_LABEL_HYPHEN;

#ifdef ENFORCE_RFC1034
			if (('0' <= *(aux + 1)) && ('9' >= *(aux + 1)))
				return GNKSA_ILLEGAL_LABEL_BEGNUM;
#endif /* ENFORCE_RFC1034 */
			last = aux;
		}
	}

	/* check for illegal characters in FQDN */
	for (aux = domain; *aux; aux++) {
		if (!gnksa_legal_fqdn_chars[(int) *aux])
			return GNKSA_INVALID_FQDN_CHAR;
	}

	return GNKSA_OK;
}


/*
 * check localpart of address
 */
static int
gnksa_check_localpart (
	char *localpart)
{
	char *aux;

	/* no localpart at all? */
	if (!*localpart)
		return GNKSA_LOCALPART_MISSING;

	/* check for zero-length domain parts */
	if (('.' == *localpart) || ('.' == *(localpart + strlen(localpart) -1)))
		return GNKSA_ZERO_LENGTH_LOCAL_WORD;

	for (aux = localpart; *aux; aux++) {
		if (('.' == *aux) && ('.' == *(aux + 1)))
			return GNKSA_ZERO_LENGTH_LOCAL_WORD;
	}

	/* check for illegal characters in FQDN */
	for (aux = localpart; *aux; aux++) {
		if (!gnksa_legal_localpart_chars[(int) *aux])
			return GNKSA_INVALID_LOCALPART;
	}

	return GNKSA_OK;
}


/*
 * split mail address into realname and address parts
 */
static int
gnksa_split_from (
	char *from,
	char *address,
	char *realname,
	int *addrtype)
{
	char *addr_begin;
	char *addr_end;
	char work[HEADER_LEN];

	/* init return variables */
	*address = *realname = '\0';

	/* copy raw address into work area */
	strncpy(work, from, 998);
	work[998] = '\0';
	work[999] = '\0';

	/* skip trailing whitespace */
	addr_end = work + strlen(work) - 1;
	while (addr_end >= work && (' ' == *addr_end || '\t' == *addr_end))
		addr_end--;

	if (addr_end < work) {
		*addrtype = GNKSA_ADDRTYPE_OLDSTYLE;
		return GNKSA_LPAREN_MISSING;
	}

	*(addr_end + 1) = '\0';
	*(addr_end + 2) = '\0';

	if ('>' == *addr_end) {
		/* route-address used */
		*addrtype = GNKSA_ADDRTYPE_ROUTE;

		/* get address part */
		addr_begin = addr_end;
		while (('<' != *addr_begin) && (addr_begin > work))
			addr_begin--;

		if ('<' != *addr_begin) /* syntax error in mail address */
			return GNKSA_LANGLE_MISSING;

		/* copy route address */
		*addr_end = *addr_begin = '\0';
		strcpy(address, addr_begin + 1);

		/* get realname part */
		addr_end = addr_begin - 1;
		addr_begin = work;

		/* strip surrounding whitespace */
		while (addr_end >= work && (' ' == *addr_end || '\t' == *addr_end))
			addr_end--;

		while ((' ' == *addr_begin) || ('\t' == *addr_begin))
			addr_begin++;

		*++addr_end = '\0';
		/* copy realname */
		strcpy(realname, addr_begin);
	} else {
		/* old-style address used */
		*addrtype = GNKSA_ADDRTYPE_OLDSTYLE;

		/* get address part */
		/* skip leading whitespace */
		addr_begin = work;
		while ((' ' == *addr_begin) || ('\t' == *addr_begin))
			addr_begin++;

		/* scan forward to next whitespace or null */
		addr_end = addr_begin;
		while ((' ' != *addr_end) && ('\t' != *addr_end) && (*addr_end))
			addr_end++;

		*addr_end = '\0';
		/* copy route address */
		strcpy(address, addr_begin);

		/* get realname part */
		addr_begin = addr_end + 1;
		addr_end = addr_begin + strlen(addr_begin) - 1;
		/* strip surrounding whitespace */
		while ((' ' == *addr_end) || ('\t' == *addr_end))
			addr_end--;

		while ((' ' == *addr_begin) || ('\t' == *addr_begin))
			addr_begin++;

		/* any realname at all? */
		if (*addr_begin) {
			/* check for parentheses */
			if ('(' != *addr_begin)
				return GNKSA_LPAREN_MISSING;

			if (')' != *addr_end)
				return GNKSA_RPAREN_MISSING;

			/* copy realname */
			*addr_end = '\0';
			strcpy(realname, addr_begin + 1);
		}
	}

	if (!strchr(address, '@')) /* check for From: without an @ */
		return GNKSA_ATSIGN_MISSING;

	/* split successful */
	return GNKSA_OK;
}


/*
 * restrictive check for valid address conforming to RFC 1036, son-of-rfc1036
 * and draft-usefor-article-xx.txt
 */
int
gnksa_do_check_from (
	char *from,
	char *address,
	char *realname)
{
	char *addr_begin;
	char *aux;
	char decoded[HEADER_LEN];
	int result = 0;
	int code;
	int addrtype;

	decoded[0] = '\0';

#ifdef DEBUG
	if (debug == 2)
		wait_message (0, "From:=[%s]", from);
#endif /* DEBUG */

	/* split from */
	code = gnksa_split_from(from, address, realname, &addrtype);
	if ('\0' == *address) /* address missing or not extractable */
		return code;

#ifdef DEBUG
	if (debug == 2)
		wait_message (0, "address=[%s]", address);
#endif /* DEBUG */

	/* parse address */
	addr_begin = strrchr(address, '@');
	if (NULL == addr_begin) {
		if (GNKSA_OK == code)
			code = result;
	} else {
		/* temporarily terminate string at separator position */
		*addr_begin++ = '\0';

#ifdef DEBUG
		if (debug == 2)
			wait_message (0, "FQDN=[%s]", addr_begin);
#endif /* DEBUG */

		/* convert FQDN part to lowercase */
		for (aux = addr_begin; *aux; aux++)
			*aux = tolower(*aux);

		if (GNKSA_OK != (result = gnksa_check_domain(addr_begin))
		    && (GNKSA_OK == code)) /* error detected */
			code = result;

		if (GNKSA_OK != (result = gnksa_check_localpart(address))
		    && (GNKSA_OK == code)) /* error detected */
			code = result;

		/* restore separator character */
		*--addr_begin= '@';
	}

#ifdef DEBUG
	if (debug == 2)
		wait_message (0, "realname=[%s]", realname);
#endif /* DEBUG */

	/* check realname */
	if (GNKSA_OK != (result = gnksa_dequote_plainphrase(realname, decoded, addrtype))) {
		if (GNKSA_OK == code) /* error detected */
			code = result;
	} else	/* copy dequoted realname to result variable */
		strcpy(realname, decoded);

#ifdef DEBUG
	if (debug == 2) {
		if (GNKSA_OK != code)
			wait_message (3, "From:=[%s], GNKSA=[%d]", from, code);
		else
			wait_message (0, "GNKSA=[%d]", code);
	}
#endif /* DEBUG */

	return code;
}


/*
 * check given address
 */
int
gnksa_check_from (
	char *from)
{
	char address[HEADER_LEN];	/* will be initialised in gnksa_split_from () */
	char realname[HEADER_LEN];	/* which is called by gnksa_do_check_from() */

	return gnksa_do_check_from(from, address, realname);
}


#if 1
/*
 * parse given address
 * return error code on GNKSA check failure
 */
int
parse_from (
	char *from,
	char *address,
	char *realname)
{
	return gnksa_do_check_from(from, address, realname);
}
#endif /* 1 */

/*
 * This is a work-around for a conflict between the sleep() function with
 * OpenVMS 6.x using socketshr.  It does not appear to be needed for
 * executables built on OpenVMS 7.x
 */
#ifdef SOCKETSHR_TCP
#	if defined(__DECC) && __CRTL_VER < 70000000
int my_sleep(unsigned n)
{
	return 0;
}
#	endif
#endif
