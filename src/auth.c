/*
 *  Project   : tin - a Usenet reader
 *  Module    : auth.c
 *  Author    : Dirk Nimmich <nimmich@uni-muenster.de>
 *  Created   : 05-04-97
 *  Updated   : no
 *  Notes     : Routines to authenticate to a news server via NNTP
 *  Copyright : (c) Copyright 1991-97 by Iain Lea & Dirk Nimmich
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"

/*
 * we don't need authentication stuff at all if we are building an index
 * daemon or don't have NNTP support
 */

#if !defined(INDEX_DAEMON) && defined(NNTP_ABLE)
/*
 * local prototypes
 */
static int do_authinfo_original (char *server, char *authuser, char *authpass);
static t_bool authinfo_generic (void);
static t_bool read_newsauth_file (char *server, char *authuser, char *authpass);
static t_bool authinfo_original (char *server, char *authuser, t_bool startup);

/*
 * Process AUTHINFO GENERIC method.
 * TRUE means succeeded.
 * FALSE means failed
 */

static t_bool
authinfo_generic (void)
{
	char tmpbuf[NNTP_STRLEN];
	char authval[NNTP_STRLEN];
	char *authcmd;
	FILE *fp;
	t_bool builtinauth = FALSE;
	static int cookiefd = -1;
#ifdef HAVE_PUTENV
	char *new_env;
	static char *old_env = 0;
#endif

#ifdef DEBUG
	debug_nntp ("authorization", "authinfo generic");
#endif

	/*
	 * If we have authenticated before, NNTP_AUTH_FDS already
	 * exists, pull out the cookiefd. Just in case we've nested.
	 */
	if (cookiefd == -1 && (authcmd = getenv ("NNTP_AUTH_FDS")))
	    sscanf (authcmd, "%*d.%*d.%d", &cookiefd);

	if (cookiefd == -1) {
		char tempfile[BUFSIZ];

		sprintf (tempfile, "%stin_AXXXXXX", TMPDIR);
		if (!mktemp (tempfile)) {
			error_message (txt_cannot_create_uniq_name);
#ifdef DEBUG
			debug_nntp ("authorization", txt_cannot_create_uniq_name);
#endif
			return FALSE;
		} else {
			fp = fopen (tempfile, "w+");
			if (!fp) {
				error_message (txt_cannot_open, tempfile);
				return FALSE;
			}
		}
		(void) unlink (tempfile);
		cookiefd = fileno (fp);
	}

	strcpy (tmpbuf, "AUTHINFO GENERIC ");
	strcpy (authval, get_val ("NNTPAUTH", ""));
	if (strlen (authval))
		strcat (tmpbuf, authval);
	else {
		strcat (tmpbuf, "ANY ");
		strcat (tmpbuf, userid);
		builtinauth = TRUE;
	}
	put_server (tmpbuf);

#ifdef HAVE_PUTENV
	sprintf (tmpbuf, "NNTP_AUTH_FDS=%d.%d.%d", fileno (nntp_rd_fp), fileno (nntp_wr_fp), cookiefd);
	new_env = my_malloc (strlen (tmpbuf) + 1);
	strcpy (new_env, tmpbuf);
	putenv (new_env);
	FreeIfNeeded (old_env);
	old_env = new_env;
#else
	sprintf (tmpbuf, "%d.%d.%d", fileno (nntp_rd_fp), fileno (nntp_wr_fp), cookiefd);
	setenv ("NNTP_AUTH_FDS", tmpbuf, 1);
#endif

	if (!builtinauth)
		return (invoke_cmd (authval));	/* TODO - is it possible that we should have drained server here ? */
	else
		return (get_respcode(NULL) == OK_AUTH);
}

/*
 * Read the $TIN_HOME/.newsauth file and put authentication username
 * and password for the specified server in the given strings.
 * Returns TRUE if at least a password was found, FALSE if there was
 * no .newsauth file or no matching server.
 */

static t_bool
read_newsauth_file (
	char *server,
	char *authuser,
	char *authpass)
{
	char line[PATH_LEN];
	char *_authpass;
	char *ptr;
	FILE *fp;
	int found = 0;

	joinpath (line, homedir, ".newsauth");

	/*
	 * TODO: check file permissions of .newsauth file and warn user if the
	 * file could be read by others.
	 */
	if ((fp = fopen (line, "r"))) {

		/*
		 * Search through authorization file for correct NNTP server
		 * File has format:  'nntp-server' 'password' ['username']
		 */
		while (fgets (line, PATH_LEN, fp) != (char *) 0) {

			/*
			 * strip trailing newline character
			 */

			ptr = strchr (line, '\n');
			if (ptr != (char *) 0)
				*ptr = '\0';

			/*
			 * Get server from 1st part of the line
			 */

			ptr = strchr (line, ' ');

			if (ptr == (char *) 0)		/* no passwd, no auth, skip */
				continue;

			*ptr++ = '\0'; 			/* cut off server part */

			if ((strcasecmp (line, server)))
				continue;		/* wrong server, keep on */

			/*
			 * Get password from 2nd part of the line
			 */

			while (*ptr == ' ')
				ptr++;		/* skip any blanks */

			_authpass = ptr;

			if (*_authpass == '"') {		/* skip "embedded" password string */
				ptr = strrchr (_authpass, '"');
				if ((ptr != (char *) 0) && (ptr > _authpass)) {
					_authpass++;
					*ptr++ = '\0';	/* cut off trailing " */
				} else			/* no matching ", proceede as normal */
					ptr = _authpass;
			}

			/*
			 * Get user from 3rd part of the line
			 */

			ptr = strchr (ptr, ' ');		/* find next separating blank */

			if (ptr != (char *) 0) {		/* a 3rd argument follows */
				while (*ptr == ' ')	/* skip any blanks */
					*ptr++ = '\0';
				if (*ptr != '\0')	/* if its not just empty */
					strcpy (authuser, ptr);	/* so will replace default user */
			}
			strcpy (authpass, _authpass);
			found++;
			break;	/* if we end up here, everything seems OK */
		}
		fclose (fp);
		return (found > 0);
	} else
		return FALSE;
}

/*
 * Perform authentication with ORIGINAL AUTHINFO method. Return response
 * code from server.
 */

static int
do_authinfo_original (
	char *server,
	char *authuser,
	char *authpass)
{
	char line[PATH_LEN];
	int ret;

	sprintf (line, "AUTHINFO USER %s", authuser);
#ifdef DEBUG
	debug_nntp ("authorization", line);
#endif
	put_server (line);
	if ((ret = get_respcode(NULL)) != NEED_AUTHDATA)
		return ret;

	if ((authpass == (char *) 0) || (*authpass == '\0')) {
#ifdef DEBUG
		debug_nntp ("authorization", "failed: no password");
#endif
		error_message (txt_nntp_authorization_failed, server);
		return ERR_AUTHBAD;
	}

	sprintf (line, "AUTHINFO PASS %s", authpass);
#ifdef DEBUG
	debug_nntp ("authorization", line);
#endif
	put_server (line);
	if ((ret = get_respcode(line)) == OK_AUTH)
		wait_message(2, txt_authorization_ok, authuser);
	else
		wait_message(2, txt_authorization_fail, authuser);
	return ret;
}

/*
 * NNTP user authorization. Returns TRUE if authorization succeeded,
 * FALSE if not.
 * Password read from ~/.newsauth or, if not present or no matching server
 * found, from console.
 * The ~/.newsauth authorization file has the format:
 *   nntpserver1 password [user]
 *   nntpserver2 password [user]
 *   etc.
 */

static t_bool
authinfo_original (
	char *server,
	char *authuser,
	t_bool startup)
{
	char *authpass, *ptr;
	char authusername[PATH_LEN];
	char authpassword[PATH_LEN];
	int ret = ERR_AUTHBAD, changed;
	static char last_server[PATH_LEN] = "";
	static t_bool already_failed = FALSE;

#ifdef DEBUG
	debug_nntp ("authorization", "original authinfo");
#endif

	authpassword[0]='\0';
	authuser = strncpy (authusername, authuser, PATH_LEN);
	authpass = authpassword;

	/*
	 * read .newsauth only if we had not failed authentication yet for the
	 * current server (we don't want to try wrong username/password pairs
	 * more than once because this may lead to an infinite loop at connection
	 * startup: nntp_open tries to authenticate, it fails, server closes
	 * connection; next time tin tries to access the server it will do
	 * nntp_open again ...). This means, however, that if configuration
	 * changed on the server between two authentication attempts tin will
	 * prompt you the second time instead of reading .newsauth (except when
	 * at startup time; in this case, it will just leave); you have to leave
	 * and restart tin or change to another server and back in order to get
	 * it read again.
	 */
	if ((changed = strcmp (server, last_server)) || ((!changed) && (!already_failed))) {
		already_failed = FALSE;
		if (read_newsauth_file (server, authuser, authpass)) {
			ret = do_authinfo_original (server, authuser, authpass);
			if (!(already_failed = (ret != OK_AUTH))) {
#ifdef DEBUG
				debug_nntp ("authorization", "succeeded");
#endif
				return TRUE;
			}
		}
	}

	/*
	 * At this point, either authentication with username/password pair from
	 * .newsauth has failed or there's no .newsauth file respectively no
	 * matching username/password for the current server.  If we are not at
	 * startup we ask the user to enter such a pair by hand.  Don't ask him
	 * at startup because if he doesn't need to authenticate (we don't know),
	 * the "Server expects authentication" messages are annoying (and even
	 * wrong).
	 * UNSURE: Maybe we want to make this decision configurable in the
	 * options menu so that the user doesn't need to create a .newsauth file.
	 * TODO: Put questions into do_authinfo_original because it is possible
	 * that the server doesn't want a password; so only ask for it if needed.
	 */
	if (!startup) {
/* FIXME: add default value to prompt */
		if (! prompt_string(txt_auth_user_needed, authusername, HIST_OTHER)) {
#ifdef DEBUG
			debug_nntp ("authorization", "failed: no username");
#endif /* DEBUG */
			return FALSE;
		}

		clear_message ();
		ptr = getpass (txt_auth_pass_needed);
		authpass = strncpy (authpassword, ptr, PATH_LEN);
		ret = do_authinfo_original (server, authuser, authpass);
	}
#ifdef DEBUG
	debug_nntp ("authorization", (ret == OK_AUTH ? "succeeded" : "failed"));
#endif
	return (ret == OK_AUTH);
}

/*
 * Do authentication stuff. Return TRUE if authentication was successful,
 * FALSE otherwise.
 *
 * First try AUTHINFO GENERIC method, then, if that failed, ORIGINAL
 * AUTHINFO method. Other authentication methods can easily be added.
 */
t_bool
authenticate (
	char *server,
	char *user,
	t_bool startup)
{
	return (authinfo_generic () || authinfo_original (server, user, startup));
}
#else
static void no_authenticate (void);
static void no_authenticate (void) { }
#endif
