/*
 *  Project   : tin - a Usenet reader
 *  Module    : os_2.c
 *  Author    : A.Wrede & I.Lea
 *  Created   : 05-07-93
 *  Updated   : 02-11-93
 *  Notes     : Extra functions for OS/2 port
 *  Copyright : (c) Copyright 1991-94 by Andreas Wrede & Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"version.h"

#if defined(M_OS2)

#include "os_2.h"

struct passwd no_pw = {
	"OS2User",
	"x",
	1,
	1,
	"",
	"OS/2 User",
	"OS/2 User",
	"C:/",
	"CMD"
};

char *optarg;
FILE *_pwf;
int fakepw = 1;
int optind = 1;

/*
 * stub for tputs
 */

#ifndef INDEX_DAEMON

int
tputs (str, zzz, func)
	register char *str;
	int zzz;
	OutcPtr func;
{
	if (! str) {
		return 0;
	}
	if (func == outchar) {
		my_fputs (str, stdout);
	} else {
		while (*str) {
			if (*str == '\n') {
				func('\r');
			}
			func(*str++);
		}
	}
	return 0;	/* OK */
}

#endif

void
backslash (
	char *ptr)
{
	while (*ptr) {
		if (*ptr == '/') {
			*ptr = '\\';
		}
		ptr++;
	}
}

/*
 * joinpath tacks a file (or sub dir) on to the end of a directory name.
 * Have to check existing / or \ to avoid doubling them.
 */

void
joinpath (
	char *str,
	char *dir,
	char *file)
{
	char c;

	if (strlen (dir) == 0) {
		strcpy (str, file);
		return;
	}
	c = dir[strlen (dir) - 1];
	if (c=='/' || c=='\\') {
		sprintf (str, "%s%s", dir, file);
	} else {
		sprintf (str, "%s/%s", dir, file);
	}
}


#ifndef __TURBOC__

FILE *
popen (
	char *command,
	char *mode)
{
	return (FILE *) 0;
}


void
pclose (
	FILE *pipe)
{
	return;
}

DIR *
opendir (
	const char *name)
{
	return NULL;
}


struct dirent *
readdir (
	DIR *di)
{
	return NULL;
}


int
closedir (
	DIR *di)
{
	return 0;
}

#endif	/* __TURBOC__ */

char
getopt (
	int argc,
	char *argv[],
	char *options)
{
	char c, *z;
	static int subind = 0;

	for (;optind < argc ; optind++, subind = 0) {
		if (subind == 0) {
			c = argv[optind][0];
			if (c != '-') {
				return EOF;
			}
			subind = 1;
		}
		c = argv[optind][subind];
		if (c != 0) {
			break;
		}
	}

	if (optind == argc) {
		return EOF;
	}

	/*
	 * get rid of funnies
	 */
	if (c == ':' || c == '?') {
		return '?';
	}

	if ((z = strchr (options, c)) == 0) {
		return '?';
	}

	if (z[1] == ':') {
		if (argv[optind][subind+1]) {
			optarg = &argv[optind][subind+1];
		} else {
			optarg = argv[++optind];
		}
		optind++;
		subind = 0;
		return c;
	}
	subind++;

	return c;
}


int
make_post_cmd (
	char *cmd,
	char *name)
{
	char *p;

	if ((p = getenv (ENV_VAR_POSTER)) != (char *) 0) {
		sprintf (cmd, p, name);
	} else {
		sprintf (cmd, DEFAULT_POSTER, name);
	}

	return 0;
}


int
gethostname (
	char *name,
	int namelen)
{
	char *p;

	if ((p = getenv ("HOSTNAME")) == (char *) 0) {
		errno = 1;
		return (-1);
	}
	strncpy (name, p, namelen);

	return (0);
}

#ifdef __TURBOC__

int
sleep (
	int sec)
{
	DosSleep (sec * 1000);
	return;
}

#endif	/* __TURBOC__ */


struct passwd *
	fakepwent (void)
{
	if (! fakepw) {
		return (struct passwd *) 0;
	}
	fakepw = 0;
	if ((no_pw.pw_name = getenv ("USER")) == NULL) {
		no_pw.pw_name = "OS2USER";
	}
	no_pw.pw_uid = 1;
	no_pw.pw_gid = 1;
	no_pw.pw_age = "";
	if ((no_pw.pw_comment = getenv ("REALNAME")) == NULL) {
		no_pw.pw_comment = "OS2 User";
	}
	no_pw.pw_gecos = no_pw.pw_comment;
	if ((no_pw.pw_dir = getenv ("HOME")) == NULL) {
		no_pw.pw_dir = "C:";
	}
	if ((no_pw.pw_shell = getenv ("SHELL")) == NULL) {
		if ((no_pw.pw_shell = getenv ("COMSPEC")) == NULL) {
			no_pw.pw_shell = "CMD.EXE";
		}
	}

	return (&no_pw);
}

struct passwd *
getpwent (void)
{
	char pwn[512];

	if (! _pwf) {
		if (getenv ("ETC") == NULL) {
			return (fakepwent ());
		}
		sprintf (pwn, "%s/passwd", getenv ("ETC"));
		if ((_pwf = fopen (pwn, "r")) == NULL) {
			return(fakepwent ());
		}
	}

	return (fgetpwent (_pwf));
}


struct passwd *
getpwuid (
	uid_t uid)
{
	struct passwd *pw;

	endpwent ();
	while (1) {
		pw = getpwent ();
		if ((!pw) || (pw->pw_uid == uid)) {
			return (pw);
		}
	}

	return NULL;
}


struct passwd *
getpwnam (
	const char *name)
{
	struct passwd *pw;

	endpwent();

	while (1) {
		pw = getpwent ();
		if ((!pw) || STRCMPEQ(pw->pw_name, name)) {
			return (
		}
	}

	return NULL;
}


void
setpwent (void)
{
	if (_pwf) {
		rewind (_pwf);
	}
}


void
endpwent (void)
{
	if (_pwf) {
		fclose (_pwf);
		_pwf = NULL;
	}
	fakepw = 1;
}


struct passwd *
fgetpwent (
	FILE *f)
{
	char *line;
	static char pwe[512];
	static struct passwd _pw;

	if (fgets (pwe, sizeof (pwe), f) == NULL) {
		return (fakepwent ());
	}

	line = pwe;
	if (line[strlen (line)-1] == '\n') {
		line[strlen (line)-1] = 0;
	}
	if (line[strlen (line)-1] == '\r') {
		line[strlen (line)-1] = 0;
	}
	_pw.pw_name = line;
	while ((*line) && (*line != ':')) {
		line++;
	}
	if (! *line) {
		return (struct passwd *) 0;
	}
	*line = 0;
	_pw.pw_passwd = ++line;
	while ((*line) && (*line != ':')) {
		line++;
	}
	if (! *line) {
		return (struct passwd *) 0;
	}
	*line = 0;
	_pw.pw_uid = atoi (++line);
	while ((*line) && (*line != ':')) {
		line++;
	}
	if (! *line) {
		return (struct passwd *) 0;
	}
	*line = 0;
	_pw.pw_gid = atoi (++line);
	while ((*line) && (*line != ':')) {
		line++;
	}
	if (! *line) {
		return (struct passwd *) 0;
	}
	*line = 0;
	_pw.pw_age = "";
	_pw.pw_comment = ++line;
	_pw.pw_gecos = line;
	while ((*line) && (*line != ':')) {
		line++;
	}
	if (! *line) {
		return (struct passwd *) 0;
	}
	*line = 0;
	_pw.pw_dir = ++line;
	if ((_pw.pw_dir[1] == ':') &&
	    (tolower (_pw.pw_dir[0]) >= 'c') &&
	    (tolower (_pw.pw_dir[0]) <= 'z')) {
		line += 2;
	}
	while ((*line) && (*line != ':')) {
		line++;
	}
	if (! *line) {
		return (struct passwd *) 0;
	}
	*line = 0;
	_pw.pw_shell = ++line;

	return (&_pw);
}


int
putpwent (
	const struct passwd *pwd,
	FILE *f)
{
	return(-1);
}


char *
getlogin (void)
{
	char *p;

	if ((p = getenv ("USER")) == NULL) {
		return (char *) 0;
	}

	return (p);
}


int
getuid (void)
{
	char *p;

	if ((p = getenv ("UID")) == NULL) {
		return (0);	/* pretend we are root */
	}

	return atoi (p);
}

#endif	/* M_OS2 */
