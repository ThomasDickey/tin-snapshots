/*
 *  Project   : tin - a Usenet reader
 *  Module    : misc.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 22-12-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute	this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change th/////is copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"
#include	"version.h"
#include	"trace.h"

/*
** Local prototypes
*/
static int strfeditor (char *editor, int linenum, char *filename, char *s, size_t maxsize, char *format);

#ifdef M_UNIX
/*
 * append_file instead of rename_file
 * minimum error trapping - only unix support
 */
void
append_file (
	char *old_filename,
	char *new_filename)
{
	char buf[1024];
	size_t n;
	FILE *fp_old, *fp_new;

	if ((fp_new = fopen (new_filename, "r")) == (FILE *) 0) {
		sprintf (buf, txt_cannot_open, new_filename);
		perror_message (buf, "ONE");
		return;
	}
	if ((fp_old = fopen (old_filename, "a+")) == (FILE *) 0) {
		sprintf (buf, txt_cannot_open, old_filename);
		perror_message (buf, "ONE");
		fclose (fp_new);
		return;
	}
	while ((n = fread (buf, 1, sizeof (buf), fp_new)) != 0) {
		if (n != fwrite (buf, 1, n, fp_old)) {
			sprintf (msg, "Failed copy_fp(). errno=%d", errno);
			perror_message (msg, "");
			return;
		}
	}
	fclose (fp_old);
 	fclose (fp_new);
}
#endif  /* M_UNIX */

void
asfail (
	const char *file,
	int	line,
	const char *cond)
{
	my_fprintf (stderr, "%s: assertion failure: %s (%d): %s\n",
		progname, file, line, cond);
	my_fflush (stderr);

	/*
	 * create a core dump
	 */
#ifdef HAVE_COREFILE
#ifdef SIGABRT
	sigdisp(SIGABRT, SIG_DFL);
	kill (process_id, SIGABRT);
#else
#	ifdef SIGILL
		sigdisp(SIGILL, SIG_DFL);
		kill (process_id, SIGILL);
#	else
#		ifdef SIGIOT
			sigdisp(SIGIOT, SIG_DFL);
			kill (process_id, SIGIOT);
#		endif
#	endif
#endif
#endif	/* HAVE_COREFILE */

	exit(1);
}


void
copy_fp (
	FILE *fp_ip,
	FILE *fp_op,
	const char *prefix)
{
	char buf[8192];
	int retcode;

	if (!prefix || !prefix[0]) {
		size_t n;
		while ((n = fread (buf, 1, sizeof (buf), fp_ip)) != 0) {
			if (n != fwrite (buf, 1, n, fp_op)) {
				if (!got_sig_pipe) {
					sprintf (msg, "Failed copy_fp(). errno=%d", errno);
					perror_message (msg, "");
				}
				return;
			}
		}
		return;
	}

	while (fgets (buf, sizeof (buf), fp_ip) != (char *) 0) {
		retcode = fprintf (fp_op, "%s%s", prefix, buf);
/*
		if (buf[0] != '\n') {
			retcode = fprintf (fp_op, "%s%s", prefix, buf);
		} else {
			retcode = fprintf (fp_op, "%s", buf);
		}
*/
		if (retcode == EOF) {
			sprintf (msg, "Failed copy_fp(). errno=%d", errno);
			perror_message (msg, "");
			return;
		}
	}
}

void
copy_body (
	FILE *fp_ip,
	FILE *fp_op,
	char *prefix,
	char *initl)
{
	char buf[8192];
	char buf2[8192];
	int retcode;
	char zprefix[256];
	char prefixbuf[256];
	int status_space;
	int status_char;
	int double_quote;
	int i;

	if (!prefix || !prefix[0]) {
		size_t n;
		while ((n = fread (buf, 1, sizeof (buf), fp_ip)) != 0) {
			if (n != fwrite (buf, 1, n, fp_op)) {
				sprintf (msg, "Failed copy_fp(). errno=%d", errno);
				perror_message (msg, "");
				return;
			}
		}
		return;
	}

	strcpy(zprefix, prefix);

	if (strstr(prefix, "%S")) {
		double_quote = 1;
		status_char = 0;
		for (i=0; prefix[i]; i++) {
			if ((status_char) && (prefix[i] == 'S'))
				zprefix[i] = 's';
			if (prefix[i] == '%') status_char = 1;
			else status_char = 0;
		}
	}
	else double_quote = 0;

	if (strstr(zprefix, "%s")) sprintf(prefixbuf, zprefix, initl);

	while (fgets (buf, sizeof (buf), fp_ip) != (char *) 0) {
		if (buf[0] != '\n') {
			if (strstr(zprefix, "%s")) { /* initials wanted */
				if (strchr(buf, '>')) {
					status_space = 0;
					status_char = 1;
					for (i=0; buf[i] && (buf[i] != '>'); i++) {
						buf2[i] = buf[i];
						if (buf[i] != ' ') status_space = 1;
						if ((status_space) &&
						  !((buf[i] >= 'A' && buf[i] <= 'Z') ||
						    (buf[i] >= 'a' && buf[i] <= 'z') ||
						    (buf[i] == '>'))) status_char = 0;
					}
					buf2[i] = '\0';
					if (status_char) {  /* already quoted */
						if (double_quote) {
							retcode = fprintf (fp_op, "%s>%s", buf2, strchr(buf, '>'));
						}
						else retcode = fprintf (fp_op, "%s", buf);
					} else {   /* ... to be quoted ... */
						retcode = fprintf (fp_op, "%s%s", prefixbuf, buf);
					}
				} else {   /* line was not already quoted (no >) */
					retcode = fprintf (fp_op, "%s%s", prefixbuf, buf);
				}
			} else {    /* no initials in quote_string, just copy */
				retcode = fprintf (fp_op, "%s%s", prefix, buf);
			}
		} else {
			retcode = fprintf (fp_op, "%s", buf);
		}
		if (retcode == EOF) {
			sprintf (msg, "Failed copy_fp(). errno=%d", errno);
			perror_message (msg, "");
			return;
		}
	}
}

const char *
get_val (
	const char *env,	/* Environment variable we're looking for	*/
	const char *def)	/* Default value if no environ value found	*/
{
	const char *ptr;

	ptr = getenv(env);

	return (ptr != (char *) 0 ? ptr : def);
}


#define EDITOR_BACKUP_FILE_EXT ".b"

int
invoke_editor (
	char *filename,
	int lineno)
{
	char buf[PATH_LEN],fnameb[PATH_LEN];
	char editor_format[PATH_LEN];
	char *my_editor;
	int retcode;
	static char editor[PATH_LEN];
	static int first = TRUE;

	if (first) {
		my_editor = getenv ("EDITOR");

		strcpy (editor, my_editor != NULL ? my_editor : get_val ("VISUAL", DEFAULT_EDITOR));
		first = FALSE;
	}

	if (start_editor_offset) {
		if (default_editor_format[0]) {
			strcpy (editor_format, default_editor_format);
		} else {
			strcpy (editor_format, EDITOR_FORMAT_ON);
		}
	} else {
		strcpy (editor_format, EDITOR_FORMAT_OFF);
	}

	retcode = strfeditor (editor, lineno, filename, buf, sizeof (buf), editor_format);

	if (!retcode) {
		sprintf (buf, "%s %s", editor, filename);
	}

	wait_message (buf);

	retcode = invoke_cmd (buf);
#ifdef EDITOR_BACKUP_FILE_EXT
	strcpy (fnameb, filename);
	strcat (fnameb, EDITOR_BACKUP_FILE_EXT);
	unlink (fnameb);
#endif
	return retcode;
}

#ifdef HAVE_ISPELL
int
invoke_ispell (
	char *nam)
{
	char buf[PATH_LEN];
	char *my_ispell;
	static char ispell[PATH_LEN];
	static int first = TRUE;

	if (first) {
#ifdef VMS
	*my_ispell = '\0';
	strcpy(ispell, "ispell");
	first = FALSE;
#else
		my_ispell = getenv ("ISPELL");

		strcpy (ispell, my_ispell != NULL ? my_ispell : PATH_ISPELL);
		first = FALSE;
#endif
	}

	sprintf (buf, "%s %s", ispell, nam);

	wait_message (buf);

	return invoke_cmd (buf);
}
#endif


#ifndef NO_SHELL_ESCAPE
void
shell_escape (void)
{
	char shell[LEN];
	char *p;

	sprintf (msg, txt_shell_escape, default_shell_command);

	if (!prompt_string (msg, shell))
		my_strncpy (shell, get_val (ENV_VAR_SHELL, DEFAULT_SHELL), sizeof (shell));

	for (p = shell; *p && (*p == ' ' || *p == '\t'); p++)
		continue;

	if (*p) {
		my_strncpy (default_shell_command, p, sizeof (default_shell_command));
	} else {
		if (default_shell_command[0]) {
			my_strncpy (shell, default_shell_command, sizeof (shell));
		} else {
			my_strncpy (shell, get_val (ENV_VAR_SHELL, DEFAULT_SHELL), sizeof (shell));
		}
		p = shell;
	}

	ClearScreen ();
	sprintf (msg, "Shell Command (%s)", p);
	center_line (0, TRUE, msg);
	MoveCursor (INDEX_TOP, 0);

	(void)invoke_cmd(p);

	continue_prompt ();

	if (draw_arrow_mark) {
		ClearScreen ();
	}
}
#endif /* !NO_SHELL_ESCAPE */


void
tin_done (
	int ret)
{
	int ask = TRUE;
	register int i, j;
	struct t_group *group;

	/*
	 * check if any groups were read & ask if they should marked read
	 */
	if (catchup_read_groups && !cmd_line) {
		for (i = 0 ; i < group_top ; i++) {
			group = &active[my_group[i]];
			if (group->read_during_session) {
				if (ask) {
					if (prompt_yn (cLINES, txt_catchup_all_read_groups, FALSE) == 1) {
						ask = FALSE;
						default_thread_arts = THREAD_NONE;	/* speeds up index loading */
					} else {
						break;
					}
				}
				sprintf (msg, "Catchup %s...", group->name);
				wait_message (msg);
				if (index_group (group)) {
					for (j = 0; j < top; j++) {
						art_mark_read (group, &arts[j]);
					}
				}
			}
		}
	}

	vWriteNewsrc ();
#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
	write_mail_active_file ();
#endif
	if (ret != EXIT_NNTP_ERROR) {
		nntp_close ();			/* disconnect from NNTP server */
	}
	free_all_arrays ();
	if (!cmd_line)
		ClearScreen ();
#ifdef HAVE_COLOR
	use_color=FALSE;
	EndInverse();
	ClearScreen();
#endif

	EndWin ();
	Raw (FALSE);

	cleanup_tmp_files ();

#ifdef DOALLOC
	no_leaks();	/* free permanent stuff */
	show_alloc();	/* memory-leak testing */
#endif

#ifdef USE_DBMALLOC
	/* force a dump, circumvents a bug in Linux libc */
	{
	extern int malloc_errfd;	/* FIXME */
	malloc_dump(malloc_errfd);
	}
#endif

#ifdef VMS
	if (ret == 0) ret = 1;
#endif

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

	char	ngroup1[HEADER_LEN];	/* outer newsgroup to compare       */
	char	ngroup2[HEADER_LEN];	/* inner newsgroup to compare       */
	char	cmplist[HEADER_LEN];	/* last loops output                */
	char	newlist[HEADER_LEN];	/* the newly generated list without */
										/* any duplicates of the first nwsg */
	int	ncnt1;			/* counter for the first newsgroup  */
	int	ncnt2;			/* counter for the second newsgroup */
	int 	over1;			/* TRUE when the outer loop is over */
	int	over2;			/* TRUE when the inner loop is over */
	char	*ptr;				/* start of next (outer) newsgroup  */
	char	*ptr2;			/* temporary pointer                */

	/* shortcut, if the is only 1 group */
	if (strchr(ngs_list, ',') != (char *) 0) {

		over1 = FALSE;
		ncnt1 = 0;
		strcpy(newlist, ngs_list);	/* make a "working copy"            */
		ptr = newlist;			/* the next outer newsg. is the 1st */

		while (!over1) {
			ncnt1++;			/* inc. outer counter */
			strcpy(cmplist, newlist);	/* duplicate groups for inner loop */
			ptr2 = strchr(ptr, ',');	/* search "," ...                  */
			if (ptr2 != (char *) 0) {       /* if found ...                    */
				*ptr2 = '\0';
				strcpy(ngroup1, ptr);   /* chop off first outer newsgroup  */
				ptr = ptr2 + 1;		/* pointer points to next newsgr.  */
			} else {			/* ... if not: last group          */
				over1 = TRUE;		/* wow, everything is done after . */
				strcpy(ngroup1, ptr);   /* ... this last outer newsgroup   */
			}

/*			my_printf("1[%d]: %s\n", ncnt1, ngroup1); */ /* debug */

			over2 = FALSE;
			ncnt2 = 0;
			
			/* now compare with each inner newsgroup on the list,
			 * which is behind the momentary outer newsgroup
			 * if it is different from the outer newsgroup, append
			 * to list, strip double-commas
			 */
			
			while (!over2) {
				ncnt2++;
				strcpy(ngroup2, cmplist);
/*				my_printf("2[%d]: %s\n", ncnt2, ngroup2); */
				ptr2 = strchr(ngroup2, ',');
				if (ptr2 != (char *) 0) {
					strcpy(cmplist, ptr2+1);
					*ptr2 = '\0';
				} else {
					over2 = TRUE;
				}
				
				if ((ncnt2 > ncnt1) && (strcasecmp(ngroup1, ngroup2))
					&& (strlen(ngroup2) != 0)) {
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
	int mode)
{
#ifndef HAVE_MKDIR
	char buf[LEN];
	struct stat sb;

	sprintf(buf, "mkdir %s", path);
	if (stat (path, &sb) == -1) {
		system (buf);
		chmod (path, mode);
	}
#else
#	if  defined(M_OS2) || defined(WIN32)
		return mkdir (path);
#	else
		return mkdir (path, mode);
#	endif
#endif
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
#endif

	return retcode;
}

#ifdef M_UNIX
void
rename_file (
	char *old_filename,
	char *new_filename)
{
	char buf[1024];
	FILE *fp_old, *fp_new;

	unlink (new_filename);

	if (link (old_filename, new_filename) == -1) {
		if (errno == EXDEV) {	/* create & copy file across filesystem */
			if ((fp_old = fopen (old_filename, "r")) == (FILE *) 0) {
				sprintf (buf, txt_cannot_open, old_filename);
				perror_message (buf, "ONE");
				return;
			}
			if ((fp_new = fopen (new_filename, "w")) == (FILE *) 0) {
				sprintf (buf, txt_cannot_open, new_filename);
				perror_message (buf, "ONE");
				fclose (fp_old);
				return;
			}
			copy_fp (fp_old, fp_new, "");
			fclose (fp_new);
			fclose (fp_old);
			errno = 0;
		} else {
			sprintf (buf, txt_rename_error, old_filename, new_filename);
			perror_message (buf, "THREE");
			return;
		}
	}
	if (unlink (old_filename) == -1) {
		sprintf (buf, txt_rename_error, old_filename, new_filename);
		perror_message (buf, "TWO");
		return;
	}
}
#else
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
	if (rename (old_filename, new_filename)==EOF)
	{	sprintf (buf, txt_rename_error, old_filename, new_filename);
		perror_message (buf, "THREE");
	}
	return;
}
#endif	/* M_UNIX */



int
invoke_cmd (
	char *nam)
{
	int ret;

	set_alarm_clock_off ();

	EndWin ();
	Raw (FALSE);
	set_signal_catcher (FALSE);

	TRACE(("called system(%s)", _nc_visbuf(nam)))
#	if USE_SYSTEM_STATUS
		system(nam);
		ret = system_status;
#	else
		ret = system (nam);
#	endif
	TRACE(("return %d", ret))

	set_signal_catcher (TRUE);
	Raw (TRUE);
	InitWin ();
#if defined(SIGWINCH)
	handle_resize(FALSE);
#endif

	set_alarm_clock_on ();

#ifdef VMS
	return ret != 0;
#else
	return ret == 0;
#endif
}


void
draw_percent_mark (
	long cur_num,
	long max_num)
{
	char buf[32];
	int percent;

	if (NOTESLINES <= 0) {
		return;
	}

	if (cur_num <= 0 && max_num <= 0) {
		return;
	}

	percent = cur_num * 100 / max_num;
	sprintf (buf, "%s(%d%%) [%ld/%ld]", txt_more, percent, cur_num, max_num);
	MoveCursor (cLINES, (cCOLS - (int) strlen (buf))-(1+BLANK_PAGE_COLS));
	StartInverse ();
	my_fputs (buf, stdout);
	my_flush ();
	EndInverse ();
}

/*
 * setuid/setgid - SYSV, Posix (with caveat about portability!)
 * seteuid/setegid - BSD 4.3
 * setreuid/setregid - BSD 4.2
 */
void
set_real_uid_gid (void)
{
#ifdef HAVE_SET_GID_UID
	if (local_index)
		return;

	umask (real_umask);

#if HAVE_SETREUID && HAVE_SETREGID
	if (setreuid (-1, real_uid) == -1) {
		perror_message ("Error setreuid(real) failed", "");
	}
	if (setregid (-1, real_gid) == -1) {
		perror_message ("Error setregid(real) failed", "");
	}
#else
#  if HAVE_SETEUID && HAVE_SETEGID
	if (seteuid (real_uid) == -1) {
		perror_message ("Error seteuid(real) failed", "");
	}
	if (setegid (real_gid) == -1) {
		perror_message ("Error setegid(real) failed", "");
	}
#  else
	if (setuid (real_uid) == -1) {
		perror_message ("Error setuid(real) failed", "");
	}
	if (setgid (real_gid) == -1) {
		perror_message ("Error setgid(real) failed", "");
	}
#  endif
#endif

#endif	/* HAVE_SET_GID_UID */
}

void
set_tin_uid_gid (void)
{
#ifdef HAVE_SET_GID_UID
	if (local_index)
		return;

	umask (0);

#if HAVE_SETREUID && HAVE_SETREGID
	if (setreuid (-1, tin_uid) == -1) {
		perror_message ("Error setreuid(tin) failed", "");
	}
	if (setregid (-1, tin_gid) == -1) {
		perror_message ("Error setregid(tin) failed", "");
	}
#else
#  if HAVE_SETEUID && HAVE_SETEGID
	if (seteuid (tin_uid) == -1) {
		perror_message ("Error seteuid(real) failed", "");
	}
	if (setegid (tin_gid) == -1) {
		perror_message ("Error setegid(real) failed", "");
	}
#  else
	if (setuid (tin_uid) == -1) {
		perror_message ("Error setuid(tin) failed", "");
	}
	if (setgid (tin_gid) == -1) {
		perror_message ("Error setgid(tin) failed", "");
	}
#  endif
#endif

#endif	/* HAVE_SET_GID_UID */
}


void
base_name (
	char *dirname,		/* argv[0] */
	char *program)		/* progname is returned */
{
	int i;
#ifdef VMS
    char *cp;
#endif

	strcpy (program, dirname);

	for (i=(int) strlen (dirname)-1 ; i ; i--) {
#ifndef VMS
		if (dirname[i] == SEPDIR) {
#else
		if (dirname[i] == ']') {
#endif
			strcpy (program, dirname+(i+1));
			break;
		}
	}
#ifdef M_OS2
	str_lwr (program, program);
#endif
#ifdef VMS
	if (cp = strrchr(program, '.')) *cp = '\0';
#endif
}


/*
 *  Return TRUE if new mail has arrived
 */

int
mail_check (void)
{
#ifndef WIN32 /* No unified mail transport on WIN32 */
	const char *mailbox_name;
	struct stat buf;
#ifdef M_AMIGA
	static long mbox_size = 0;
#endif

	mailbox_name = get_val ("MAIL", mailbox);

#ifdef M_AMIGA
	/*
	** Since AmigaDOS does not distinguish between atime and mtime
	** we have to find some other way to figure out if the mailbox
	** was modified (to bad that Iain removed the mail_setup() and
	** mail_check() scheme used prior to 1.30 260694 which worked also
	** on AmigaDOS). (R. Luebke 10.7.94)
	*/

	/* this is only a first try, but it seems to work :) */

	if (mailbox_name != 0) {
		if (stat (mailbox_name, &buf) >= 0) {
			if (buf.st_size > 0) {
				if (buf.st_size > mbox_size) {
					mbox_size = buf.st_size;
					return TRUE;
				} else {
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
				}
			} else {
				mbox_size = 0;
			}
		}
	}
#else
	if (mailbox_name != 0 && stat (mailbox_name, &buf) >= 0 &&
		buf.st_atime < buf.st_mtime && buf.st_size > 0) {
		return TRUE;
	}
#endif
#endif /* !WIN32 */
	return FALSE;
}

/*
 * Returns the user name and E-mail address of the user
 *
 * Written by ahd 15 July 1989
 * Borrowed from UUPC/extended with some mods by nms
 * Rewritten from scratch by Th. Quinot, 1997-01-03
 */

# define APPEND_TO(dest, src) do { \
	(void) sprintf ((dest), "%s", (src)); \
	(dest)=strchr((dest), '\0'); \
	} while (0)
# define RTRIM(whatbuf, whatp) do { (whatp)--; \
	while ((whatp) >= (whatbuf) && \
	(*(whatp)==' ')) \
	*((whatp)--) = '\0'; } while (0)
# define LTRIM(whatbuf, whatp) for ((whatp) = (whatbuf) ; \
	(whatp) && (*(whatp) == ' ') ; \
	(whatp)++)
# define TRIM(whatbuf, whatp) do { RTRIM ((whatbuf), (whatp)); \
	LTRIM ((whatbuf), (whatp)); \
	} while (0)

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
	for (; *ap ; ap++) {
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
	if (state == 0)
		if ((atom_type == 1) || !asbuf[0])
			APPEND_TO (asp, atom_buf);
		else
			APPEND_TO (cmtp, atom_buf);
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
# undef APPEND_TO
# undef RTRIM
# undef LTRIM
# undef TRIM



/*
 *  Return a pointer into s eliminating any leading Re:'s.  Example:
 *
 *	  Re: Reorganization of misc.jobs
 *	  ^   ^
 *    Re^2: Reorganization of misc.jobs
 *
 *  now also strips trailing (was: ...) (obw) 
 */

char *
eat_re (
	char *s,
	t_bool eat_was)
{
	char *e;

	while (*s == 'r' || *s == 'R') {
		if ((*(s+1) == 'e' || *(s+1) == 'E')) {
			if (*(s+2) == ':')
				s += 3;
			else if (*(s+2) == ' ' && *(s+3) == ':')
				s += 4;
			else if (*(s+2) == '^' && isdigit((unsigned char)*(s+3)) && *(s+4) == ':')
				s += 5; 		/* hurray nn */
			else
				break;
		} else
			break;

		while (*s == ' ')		/* And skip leading whitespace */
			s++;

	}
 	if (eat_was) { /* kill "(was: ...)" ? */
		if ((e = strstr(s, "(was:"))) /* should we take more spaces into consideration? */
			*e = '\0';
	}

	for (e = s; *e; e++)	/* NULL out trailing whitespace */
		;		/* moved here from the loop */

	while (e-- > s && isspace((unsigned char)*e)) {
		*e = '\0';
	}

	return s;
}

/*
 *  Hash the subjects (after eating the Re's off) for a quicker
 *  thread search later.  We store the hashes for subjects in the
 *  index file for speed.
 */
#if 0
long
hash_s (
	char *s)
{
	long h = 0;
	unsigned char *t = (unsigned char *) s;

	while (*t)
		h = h * 64 + *t++;

	return h;
}
#endif

/*
 * Clear tag status of all articles. If articles were untagged, return TRUE
 */
int
untag_all_articles (void)
{
	int untagged = FALSE;
	register int i;

	for (i=0 ; i < top ; i++) {
		if (arts[i].tagged) {
			arts[i].tagged = FALSE;
			untagged = TRUE;
		}
	}
	num_of_tagged_arts = 0;

	return (untagged);
}


int
my_isprint (
	int c)
{
	/* See son-of-1036 4.4 for more information about printable characters */
#ifndef NO_LOCALE
	return (isprint(c) || c==8 || c==9 || c==12);
#else
	/* dirty hack for iso-8859-1, if locale isn't installed correct */
	return (isprint(c) || c==8 || c==9 || c==12 || c>=160);
#endif
}


/*
 * Returns author information
 * thread	if true, assume we're on thread menu and show all author info if
 *			subject not shown
 * art		ptr to article
 * str		ptr in which to return the author. Must be a valid data area
 * len		max length of data to return
 *
 * The data will be null terminated
 */
void
get_author (
	int thread,
	struct t_article *art,
	char *str, int len)
{
	int author;

	if (thread && !show_subject)
		author = SHOW_FROM_BOTH;
	else
		author = CURR_GROUP.attribute->show_author;

	switch (author) {
		case SHOW_FROM_NONE:
			str[0] = '\0';
			break;
		case SHOW_FROM_ADDR:
			strncpy (str, art->from, len);
			break;
		case SHOW_FROM_NAME:
			if (art->name)
				strncpy (str, art->name, len);
			else
				strncpy (str, art->from, len);
			break;
		case SHOW_FROM_BOTH:
			if (art->name) {
				char buff[LEN];			/* TODO eliminate this ? */

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
toggle_inverse_video (void)
{
	inverse_okay = !inverse_okay;
	if (inverse_okay) {
#ifndef USE_INVERSE_HACK
		draw_arrow_mark = FALSE;
#endif
	} else {
		draw_arrow_mark = TRUE;
	}
}

void
show_inverse_video_status (void)
{
	if (inverse_okay) {
		info_message (txt_inverse_on);
	} else {
		info_message (txt_inverse_off);
	}
}

#ifdef HAVE_COLOR
void
toggle_color (void)
{
	use_color = !use_color;
}

void
show_color_status (void)
{
	if (use_color) {
		info_message (txt_color_on);
	} else {
		info_message (txt_color_off);
	}
}
#endif /* HAVE_COLOR */

int
get_arrow_key (void)
{
#if USE_CURSES
#if NCURSES_MOUSE_VERSION
	MEVENT my_event;
#endif
	int ch = getch();
	int code = KEYMAP_UNKNOWN;

	switch (ch) {
		case KEY_BACKSPACE:
			code = '\b';
			break;
		case KEY_DC:
			code = '\177';
			break;
		case KEY_UP:
			code = KEYMAP_UP;
			break;
		case KEY_DOWN:
			code = KEYMAP_DOWN;
			break;
		case KEY_LEFT:
			code = KEYMAP_LEFT;
			break;
		case KEY_RIGHT:
			code = KEYMAP_RIGHT;
			break;
		case KEY_NPAGE:
			code = KEYMAP_PAGE_DOWN;
			break;
		case KEY_PPAGE:
			code = KEYMAP_PAGE_UP;
			break;
		case KEY_HOME:
			code = KEYMAP_HOME;
			break;
		case KEY_END:
			code = KEYMAP_END;
			break;
#ifdef NCURSES_MOUSE_VERSION
		case KEY_MOUSE:
			if (getmouse(&my_event) != ERR) {
				switch (my_event.bstate) {
					case BUTTON1_CLICKED:
						xmouse = MOUSE_BUTTON_1;
						break;
					case BUTTON2_CLICKED:
						xmouse = MOUSE_BUTTON_2;
						break;
					case BUTTON3_CLICKED:
						xmouse = MOUSE_BUTTON_3;
						break;
				}
				xcol = my_event.x; 	/* column */
				xrow = my_event.y; 	/* row */
				code = KEYMAP_MOUSE;
			}
			break;
#endif
	}
	return code;
#else
	int ch;
	int ch1;

	ch = ReadCh ();
	if (ch == '[' || ch == 'O')  {
		ch = ReadCh ();
	}
	switch (ch) {
		case 'A':
		case 'i':
#ifdef QNX42
		case 0xA1:
#endif
			return KEYMAP_UP;

		case 'B':
#ifdef QNX42
		case 0xA9:
#endif
			return KEYMAP_DOWN;

		case 'D':
#ifdef QNX42
		case 0xA4:
#endif
			return KEYMAP_LEFT;

		case 'C':
#ifdef QNX42
		case 0xA6:
#endif
			return KEYMAP_RIGHT;

		case 'I':		/* ansi  PgUp */
		case 'V':		/* at386 PgUp */
		case 'S':		/* 97801 PgUp */
		case 'v':		/* emacs style */
#ifdef QNX42
		case 0xA2:
#endif
#ifdef M_AMIGA
			return KEYMAP_PAGE_DOWN;
#else
			return KEYMAP_PAGE_UP;
#endif

		case 'G':		/* ansi  PgDn */
		case 'U':		/* at386 PgDn */
		case 'T':		/* 97801 PgDn */
#ifdef QNX42
		case 0xAA:
#endif
#ifdef M_AMIGA
			return KEYMAP_PAGE_UP;
#else
			return KEYMAP_PAGE_DOWN;
#endif

		case 'H':		/* at386  Home */
#ifdef QNX42
		case 0xA0:
#endif
			return KEYMAP_HOME;

		case 'F':		/* ansi   End */
		case 'Y':		/* at386  End */
#ifdef QNX42
		case 0xA8:
#endif
			return KEYMAP_END;

		case '5':		/* vt200 PgUp */
			(void) ReadCh ();	/* eat the ~ (interesting use of words :) */
			return KEYMAP_PAGE_UP;

		case '6':		/* vt200 PgUp */
			(void) ReadCh ();	/* eat the ~  */
			return KEYMAP_PAGE_DOWN;

		case '1':		/* vt200 PgUp */
			ch = ReadCh (); /* eat the ~  */
			if (ch == '5') {	/* RS/6000 PgUp is 150g, PgDn is 154g */
				ch1 = ReadCh ();
				(void) ReadCh ();
				if (ch1 == '0')
					return KEYMAP_PAGE_UP;
				if (ch1 == '4')
					return KEYMAP_PAGE_DOWN;
			}
			return KEYMAP_HOME;

		case '4':		/* vt200 PgUp */
			(void) ReadCh ();	/* eat the ~  */
			return KEYMAP_END;

		case 'M':		/* xterminal button press */
			xmouse = ReadCh () - ' ';	/* button */
			xcol = ReadCh () - '!'; 	/* column */
			xrow = ReadCh () - '!'; 	/* row */
			return KEYMAP_MOUSE;

		default:
			return KEYMAP_UNKNOWN;
	}
#endif
}

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
			fgets (buf, sizeof (buf), fp);
			fclose (fp);
#ifdef INDEX_DAEMON
			sprintf (msg, "%s: Already started pid=[%d] on %s",
				progname, atoi(buf), buf+8);
#else
			sprintf (msg, "\n%s: Already started pid=[%d] on %s",
				progname, atoi(buf), buf+8);
#endif
			error_message (msg, "");
			exit (1);
		}
	} else	if ((fp = fopen (the_lock_file, "w")) != (FILE *) 0) {
		time (&epoch);
		fprintf (fp, "%6d  %s\n", process_id, ctime (&epoch));
		fclose (fp);
		chmod (the_lock_file, (S_IRUSR|S_IWUSR));
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
	char tbuf[PATH_LEN];
	int i, j, iflag;

	if (s == (char *) 0 || format == (char *) 0 || maxsize == 0) {
		return 0;
	}

	if (strchr (format, '%') == (char *) 0 && strlen (format) + 1 >= maxsize) {
		return 0;
	}

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
				} else {
					return 0;
				}
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
					strcpy(tbuf, note_h_date);
					break;
				case 'F':	/* Articles Address+Name */
					if (arts[respnum].name) {
						sprintf (tbuf, "%s <%s>",
							arts[respnum].name,
							arts[respnum].from);
					} else {
						strcpy (tbuf, arts[respnum].from);
					}
					break;
				case 'G':	/* Groupname of Article */
					strcpy (tbuf, group);
					break;
				case 'I':	/* Initials of author */
					if (arts[respnum].name != (char *) 0) {
						strcpy (tbuf, arts[respnum].name);
					} else {
						strcpy (tbuf, arts[respnum].from);
					}
					j = 0;
					iflag = 1;
					for (i=0; tbuf[i]; i++) {
						if (iflag) {
							tbuf[j++] = tbuf[i];
							iflag = 0;
						}
						if (strchr(" ._@", tbuf[i])) iflag = 1;
					}
					tbuf[j] = '\0';
					break;
				case 'M':	/* Articles MessageId */
					strcpy (tbuf, note_h_messageid);
					break;
				case 'N':	/* Articles Name of author */
					if (arts[respnum].name != (char *) 0) {
						strcpy (tbuf, arts[respnum].name);
					} else {
						strcpy (tbuf, arts[respnum].from);
					}
					break;
				case 'C':   /* First Name of author */
					if (arts[respnum].name != (char *) 0) {
						strcpy (tbuf, arts[respnum].name);
						if (strrchr (arts[respnum].name, ' ')) {
							*(strrchr (tbuf, ' ')) = '\0';
						}
					} else {
						strcpy (tbuf, arts[respnum].from);
					}
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
				} else {
					return 0;
				}
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

	if (s == (char *) 0 || format == (char *) 0 || maxsize == 0) {
		return 0;
	}

	if (strchr (format, '%') == (char *) 0 && strlen (format) + 1 >= maxsize) {
		return 0;
	}

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
#endif
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
				} else {
					return 0;
				}
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
				} else {
					return 0;
				}
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
 *   ~/News    -> /usr/iain/News
 *   ~abc/News -> /usr/abc/News
 *   $var/News -> /env/var/News
 *   =file     -> /usr/iain/Mail/file
 *   +file     -> /usr/iain/News/group.name/file
 *   ~/News/%G -> /usr/iain/News/group.name
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
#endif

	if (str == (char *) 0 || format == (char *) 0 || maxsize == 0) {
		return 0;
	}

	if (strlen (format) + 1 >= maxsize) {
		return 0;
	}

	for (; *format && str < endp - 1; format++) {
		tbuf[0] = '\0';

		/*
		 * If just a normal part of the pathname copy it
		 */
#ifdef VMS
		if (!strchr ("~=+", *format))
#else
		if (!strchr ("~$=+", *format))
#endif
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
						while (*format && *format != '/') {
							tbuf[i++] = *format++;
						}
						tbuf[i] = '\0';
						/*
						 * OK lookup the username in/etc/passwd
						 */
						pwd = getpwnam (tbuf);
						if (pwd == (struct passwd *) 0) {
							str[0] = '\0';
							return 0;
						} else {
							sprintf (tbuf, "%s/", pwd->pw_dir);
						}
#else
						/* Amiga has no ther users */
						return 0;
#endif
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
					while (*format && !(strchr("}-", *format))) {
						tbuf[i++] = *format++;
					}
					tbuf[i] = '\0';
					i = 0;
					if (*format && *format == '-') {
						format++;
						while (*format && *format != '}') {
							defbuf[i++] = *format++;
						}
					}
					defbuf[i] = '\0';
				} else {
					while (*format && *format != '/') {
						tbuf[i++] = *format++;
					}
					tbuf[i] = '\0';
					format--;
					defbuf[0] = '\0';
				}
				/*
				 * OK lookup the variable in the shells environment
				 */
				envptr = getenv (tbuf);
				if (envptr == (char *) 0 || (*envptr == '\0')) {
					strncpy(tbuf, defbuf, sizeof (tbuf)-1);
				} else {
					strncpy (tbuf, envptr, sizeof (tbuf)-1);
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
				} else {
					*str++ = *format;
				}
				break;
			case '+':
				/*
				 * Shorthand for saving to savedir/groupname/file
				 * Only convert if 1st char in format
				 */
				if (startp == format && savedir != (char *) 0) {
					if (strfpath (savedir, buf, sizeof (buf), the_homedir,
					    (char *) 0, (char *) 0, (char *) 0)) {

#ifdef HAVE_LONG_FILE_NAMES
						my_strncpy (tmp, group, sizeof (tmp));
#else
						my_strncpy (tmp, group, 14);
#endif
						/*
						 *  convert 1st letter to uppercase
						 */
						if (tmp[0] >= 'a' && tmp[0] <= 'z') {
							tmp[0] = tmp[0] - 32;
						}
#ifndef VMS
						joinpath (tbuf, buf, tmp);
#ifdef WIN32
						strcat (tbuf, "\\");
#else
						strcat (tbuf, "/");
#endif
#else /* VMS */
			joindir (tbuf, buf, tmp);
#endif
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
				} else {
					*str++ = *format;
				}
				break;
			case '%':	/* Different forms of parsing cmds */
				*str++ = *format;
				break;
			default:
				break;
		}
	}

	if (str < endp && *format == '\0') {
		*str = '\0';
/*
clear_message ();
my_printf ("!!!format=[%s]  path=[%s]", startp, start);
my_flush ();
sleep (2);
*/
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
	char *format)
{
	char *endp = s + maxsize;
	char *start = s;
	char tbuf[PATH_LEN];
	int i, quote_area = no_quote;

	if (s == (char *) 0 || format == (char *) 0 || maxsize == 0) {
		return 0;
	}

	if (strchr (format, '%') == (char *) 0 && strlen (format) + 1 >= maxsize) {
		return 0;
	}

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
					tbuf[0] = '%';
					tbuf[1] = *format;
					tbuf[2] = '\0';
					break;
			}
			i = strlen (tbuf);
			if (i) {
				if (s + i < endp - 1) {
					strcpy (s, tbuf);
					s += i;
				} else {
					return 0;
				}
			}
		}
		if (*format == '%') {
			t_bool ismail=TRUE;
			switch (*++format) {
				case '\0':
					*s++ = '%';
					goto out;
				case '%':
					*s++ = '%';
					continue;
				case 'F':	/* Filename */
					strcpy (tbuf, filename);
					break;
				case 'M':	/* Mailer */
					strcpy (tbuf, the_mailer);
					break;
				case 'S':	/* Subject */
					strcpy (tbuf, escape_shell_meta (rfc1522_encode (subject,ismail), quote_area));
					break;
				case 'T':	/* To */
					strcpy (tbuf, escape_shell_meta (rfc1522_encode (to,ismail), quote_area));
					break;
				case 'U':	/* User */
					strcpy (tbuf, rfc1522_encode (userid,ismail));
					break;
				default:
					tbuf[0] = '%';
					tbuf[1] = *format;
					tbuf[2] = '\0';
					break;
			}
			i = strlen (tbuf);
			if (i) {
				if (s + i < endp - 1) {
					strcpy (s, tbuf);
					s += i;
				} else {
					return 0;
				}
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
 *
 */

int
get_initials (
	int respnum,
	char *s,
	int maxsize)
{
	char tbuf[PATH_LEN];
	int i, j;
	int iflag;

	if (s == (char *) 0 || maxsize == 0) {
		return 0;
	}

	if (arts[respnum].name != (char *) 0) {
		strcpy (tbuf, arts[respnum].name);
	} else {
		strcpy (tbuf, arts[respnum].from);
	}
	iflag = 1;
	j = 0;
	for (i=0; tbuf[i]; i++) {
		if (iflag) {
			s[j++] = tbuf[i];
			iflag = 0;
		}
		if (strchr(" ._@", tbuf[i])) iflag = 1;
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
#endif
}


void
make_group_path (
	char *name,
	char *path)
{
#if 0
	char *ptr;
#endif

#ifdef VMS
	sprintf(path, "[%s]", name);
#else
	while (*name) {
		if (*name == '.')
			*path = '/';
		else
			*path = *name;
		name++;
		path++;
	} 
	*path = '\0';
#if 0	/* TODO */
	strcpy (path, name);

	ptr = path;

	while (*ptr) {
		if (*ptr == '.') {
			*ptr = '/';
		}
		ptr++;
	}
#endif
#endif
}

/*
 * Delete tmp index & local newsgroups file
 */

void
cleanup_tmp_files (void)
{
	char acFile[PATH_LEN];

	if (read_news_via_nntp && xover_supported) {
		sprintf (acFile, "%s%d.idx", TMPDIR, process_id);
		unlink (acFile);
	}
	unlink (local_newsgroups_file);
	unlink (lock_file);
}


void
make_post_process_cmd (
	char *cmd,
	char *dir,
	char *file)
{
	char buf[LEN];
	char currentdir[PATH_LEN];

	get_cwd (currentdir);
	chdir (dir);
#ifdef M_OS2
	backslash (file);
#endif
	sprintf (buf, cmd, file);
	invoke_cmd (buf);
	chdir (currentdir);
}


int
stat_file (
	char *file)
{
	struct stat st;

	return (stat (file, &st) == -1 ? FALSE : TRUE);
}


void
vPrintBugAddress (void)
{
	my_fprintf (stderr, "%s %s %s [%s]: send a DETAILED bug report to %s%s\n",
		progname, VERSION, RELEASEDATE, OS, BUG_REPORT_ADDRESS, add_addr);
	my_fflush (stderr);
}

/*
 *  Copy file from pcSrcFile to pcDstFile
 */

int
iCopyFile (
	char	*pcSrcFile,
	char	*pcDstFile)
{
	char	acBuffer[8192];
	FILE	*hFpDst;
	FILE	*hFpSrc;
	int	iRetCode = FALSE;
	int	iReadOk = -1;
	int	iWriteOk = -1;
	size_t	iWriteSize = 0;
	long	lCurFilePos = 0L;
	long	lSrcFilePos = 0L;

	if ((hFpSrc = fopen (pcSrcFile, "r")) != (FILE *) 0)
	{
		if ((hFpDst = fopen (pcDstFile, "w")) != (FILE *) 0)
		{
			while (!feof (hFpSrc) && (iReadOk = fread (acBuffer, sizeof (acBuffer), 1, hFpSrc)) != -1)
			{
				lCurFilePos = ftell (hFpSrc);
				iWriteSize = (size_t) (lCurFilePos - lSrcFilePos);
				lSrcFilePos = lCurFilePos;
				if ((iWriteOk = fwrite (acBuffer, iWriteSize, 1, hFpDst)) == -1)
				{
					break;
				}
			}
			if (iReadOk != -1 && iWriteOk != -1)
			{
				iRetCode = TRUE;
			}
			fclose (hFpDst);
		}
		fclose (hFpSrc);
	}

	return iRetCode;
}


/*
 * take a peek at the next char in file
 */

int
peek_char (
	FILE *fp)
{
	int c=fgetc(fp);
	if (c!=EOF)
		ungetc(c, fp);
	return c;
}

#ifdef LOCAL_CHARSET

/*
 * convert between local and network charset (e.g. NeXT and latin1)
 */


#define CHARNUM 256
#define BAD (-1)

#include "l1_next.tab"
#include "next_l1.tab"

static int
to_local (
	int c)
{
	if (use_local_charset) {
		c = c_l1_next[(unsigned char)c];
		if (c == BAD) return '?';
		else return c;
	} else {
		return c;
	}
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
		c = c_next_l1[(unsigned char) c];
		if (c==BAD) return '?';
		else return c;
	} else {
		return c;
	}
}

void
buffer_to_network (
	char *b)
{
	for(; *b; b++)
		*b = to_network(*b);
}

#endif /* LOCAL_CHARSET */


char 
*random_organization(
	char *in_org)
{
	static char selorg[512];
	int nool = 0, sol;
	FILE *orgfp;

	*selorg = '\0';

	if (*in_org != '/')
		return in_org;
		
	if ((orgfp = fopen(in_org, "r")) == NULL)
		return selorg;

	/* count lines */
	while (fgets(selorg, sizeof(selorg), orgfp))
		nool++;
	
	fseek(orgfp, 0, SEEK_SET);
	sol = rand () % nool + 1;
	nool = 0;
	while ((nool != sol) && (fgets(selorg, sizeof(selorg), orgfp)))
		nool++;
		
	fclose(orgfp);
	
	return selorg;
}	
