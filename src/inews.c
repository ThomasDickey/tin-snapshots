/*
 *  Project   : tin - a Usenet reader
 *  Module    : inews.c
 *  Author    : I.Lea
 *  Created   : 17-03-92
 *  Updated   : 22-08-95
 *  Notes     : NNTP builtin version of inews
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

#ifdef VMS
#   ifdef MULTINET
#		define netwrite    socket_write
#		include "multinet_root:[multinet.include]netdb.h"
#   else
#		define netwrite    write
#		include <netdb.h>
#   endif
#else
#	ifdef HAVE_NETDB_H
#		include	<netdb.h>
#	endif
#endif

#define	PATHMASTER	"not-for-mail"

#if defined(M_AMIGA) && (defined (INEWS_MAIL_GATEWAY) || defined (INEWS_MAIL_DOMAIN))
/* Damn compiler bugs...
 * Without this hack, SASC 6.55 produces a TST.B d16(pc),
 * which is illegal on a 68000
 */
static const char *inews_mail_gateway = INEWS_MAIL_GATEWAY;
static const char *inews_mail_domain = INEWS_MAIL_DOMAIN;

#undef INEWS_MAIL_GATEWAY
#undef INEWS_MAIL_DOMAIN
#define INEWS_MAIL_GATEWAY inews_mail_gateway
#define INEWS_MAIL_DOMAIN inews_mail_domain
#endif

int
submit_inews (name)
	char *name;
{
	int	ret_code = FALSE;

#if !defined(INDEX_DAEMON) /* && !defined(XSPOOLDIR) */

#ifdef NNTP_INEWS
#if !defined(FORGERY) && defined INEWS_MAIL_DOMAIN
	char	buf[NNTP_STRLEN];
	char	domain_name[NNTP_STRLEN];
#endif /* !FORGERY && INEWS_MAIL_DOMAIN */
	char	from_name[PATH_LEN];
	char	host_name[PATH_LEN];
	char	full_name[128];
	char	user_name[128];
	char	line[NNTP_STRLEN];
	char	*ptr;
	FILE	*fp;
	int	respcode;

	if ((fp = fopen (name, "r")) == (FILE *) 0) {
		return ret_code;
	}

	get_host_name (host_name);
	get_user_info (user_name, full_name);
	get_from_name (user_name, host_name, full_name, from_name);

	/*
	 * Check that at least one '.' comes after the '@' in the From: line
	 */
	ptr = strchr (from_name, '@');
	if (ptr != (char *) 0) {
		ptr = strchr (ptr, '.');
		if (ptr == (char *) 0) {
			error_message ("Invalid  From: %s  line. Read the INSTALL file again.", from_name);
			fclose (fp);
			return ret_code;
		}
	}

	/*
	 * Check that domain is not of type  host.subdomain.domain
	 */
	ptr = str_str (from_name, "subdomain.domain", 16);
	if (ptr != (char *) 0) {
		error_message ("Invalid  From: %s line. Reconfigure your domainname.", from_name);
		fclose (fp);
		return ret_code;
	}

	/*
	 * Send POST command to NNTP server
	 */
	put_server ("post");

	/*
	 * Receive CONT_POST or ERROR response code from NNTP server
	 */
	if ((respcode = get_respcode ()) != CONT_POST) {
		error_message ("%s", nntp_respcode (respcode));
		debug_nntp ("submit_inews", nntp_respcode (respcode));
		fclose (fp);
		return ret_code;
	}

	/*
	 * Send Path: and From: article headers
	 */
#ifndef FORGERY
#if defined(INEWS_MAIL_GATEWAY) || defined(INEWS_MAIL_DOMAIN)
	if (*(INEWS_MAIL_GATEWAY)) {
		sprintf (line, "Path: %s", PATHMASTER);
	} else if (*(INEWS_MAIL_DOMAIN)) {
		strcpy (buf, INEWS_MAIL_DOMAIN);
		get_domain_name (buf, domain_name);
		if (*domain_name == '.') {
			sprintf (line, "Path: %s%s!%s", host_name, domain_name, PATHMASTER);
		} else {
			/* mail mark@garden.equinox.gen.nz if you think
			 * host_name should be domain_name here...
			 */
			sprintf (line, "Path: %s!%s", host_name, PATHMASTER);
		}
	} else {
		sprintf (line, "Path: %s!%s", host_name, PATHMASTER);
	}
#else
	sprintf (line, "Path: %s!%s", host_name, PATHMASTER);
#endif
	put_server (line);

	sprintf (line, "From: %s", rfc1522_encode(from_name));
	put_server (line);
#endif /* !FORGERY */

	/*
	 * Send article 1 line at a time ending with "."
	 */
	while (fgets (line, sizeof (line), fp) != (char *) 0) {
		/*
		 * Remove linefeed from line
		 */
		ptr = strrchr (line, '\n');
		if (ptr != (char *) 0) {
			*ptr = '\0';
		}
		/*
		 * If line starts with a '.' add another '.' to stop truncation
		 */
		if (line[0] == '.') {
			u_put_server (".");
		}
		u_put_server (line);
		u_put_server ("\r\n");
	}

	put_server (".");
	fclose (fp);

	/*
	 * Receive OK_POSTED or ERROR response code from NNTP server
	 */
	if ((respcode = get_respcode ()) != OK_POSTED) {
		error_message ("%s", nntp_respcode (respcode));
		debug_nntp ("submit_inews", nntp_respcode (respcode));
		return ret_code;
  	}

	ret_code = TRUE;

#endif /* NNTP_INEWS */

#endif /* INDEX_DAEMON */

	return ret_code;
}

/*
 * Find real hostname / substitute hostname if news gateway name
 */

void
get_host_name (host_name)
	char *host_name;
{
#ifndef INDEX_DAEMON

	char *ptr, host[PATH_LEN];
	char nntp_inews_gateway[PATH_LEN];
#ifdef NEWSLIBDIR
	char sitename[PATH_LEN];
#endif
	FILE *fp, *sfp;

	host_name[0] = '\0';
	nntp_inews_gateway[0] = '\0';

#ifdef INEWS_MAIL_GATEWAY
	if (*(INEWS_MAIL_GATEWAY)) {
		strcpy (nntp_inews_gateway, INEWS_MAIL_GATEWAY);
	}
#endif

	if (nntp_inews_gateway[0]) {
		/*
		 * If 1st letter is '$' read gateway name from shell variable
		 */
		if (nntp_inews_gateway[0] == '$' && nntp_inews_gateway[1]) {
			ptr = getenv (&nntp_inews_gateway[1]);
			if (ptr != (char *) 0) {
				strncpy (nntp_inews_gateway, ptr, sizeof (nntp_inews_gateway));
			}
		}
		/*
		 * If 1st letter is '/' read gateway name from specified file
		 */
		if (nntp_inews_gateway[0] == '/') {
			if ((fp = fopen (nntp_inews_gateway, "r")) != (FILE *) 0) {
				if (fgets (host, sizeof (host), fp) != (char *) 0) {
					strcpy (host_name, host);
					ptr = strrchr (host_name, '\n');
					if (ptr != (char *) 0) {
						*ptr = '\0';
					}
				}
				fclose (fp);
			}
			if (! host_name[0]) {
				strcpy (host_name, "PROBLEM_WITH_YOUR_MAIL_GATEWAY_FILE");
			}
		} else {
			strcpy (host_name, nntp_inews_gateway);
		}
	} else {
		/*
		 * Get the FQDN that the article will have from
		 * 1 of 5 locations:
		 *   /etc/HOSTNAME (linux)
		 *   NEWSLIBDIR/sitename
		 *   NEWSLIBDIR/mailname
		 *   gethostbyname()
		 *   uname()
		 */
#ifndef M_AMIGA
		sfp = fopen ("/etc/HOSTNAME", "r");
		if (sfp == (FILE *) 0)
#endif
		{
#ifdef NEWSLIBDIR
			joinpath (sitename, NEWSLIBDIR, "sitename");
			sfp = fopen (sitename, "r");
#ifndef M_AMIGA
			if (sfp == (FILE *) 0) {
				sprintf (sitename, "%s/mailname", NEWSLIBDIR);
				sfp = fopen (sitename, "r");
			}
#endif /* !M_AMIGA */
#endif /* NEWSLIBDIR */
		}
		if (sfp != (FILE *) 0) {
			fgets (host, sizeof (host), sfp);
			if (strlen (host) != 0) {
				ptr = strrchr (host, '\n');
				if (ptr != (char *) 0) {
					*ptr = '\0';
				}
			}
			fclose (sfp);
		} else {
			ptr = GetFQDN ();
			if (ptr != (char *) 0) {
				my_strncpy (host, ptr, sizeof (host));
			} else {
#				ifdef HAVE_GETHOSTBYNAME
				{
					struct hostent *host_entry;

					gethostname (host, sizeof (host));
					host_entry = gethostbyname (host);
					if (host_entry != NULL)
						my_strncpy (host, host_entry->h_name, sizeof (host));
				}
#				else
#					if defined(M_AMIGA) || defined(M_OS2)
						my_strncpy (host, get_val ("NodeName", "PROBLEM_WITH_NODE_NAME"), sizeof (host));
#					else
#					if defined(WIN32)
						my_strncpy (host, get_val ("COMPUTERNAME", "PROBLEM_WITH_COMPUTERNAME"), sizeof (host));
#					else
					{
						struct utsname uts_name;

						uname (&uts_name);
						my_strncpy (host, uts_name.nodename, sizeof (host));
					}
#					endif
#					endif
#				endif
			}
		}
		strcpy (host_name, host);
	}

#endif /* INDEX_DAEMON */
}

/*
 * Find username & fullname
 */

void
get_user_info (user_name, full_name)
	char *user_name;
	char *full_name;
{
#ifndef INDEX_DAEMON
	char buf[128];
	char tmp[128];
	char *ptr;

#if defined(M_AMIGA)
	ptr = (char *) get_val ("REALNAME", "Unknown");
	my_strncpy (full_name, ptr, 128);
	strcpy (user_name, userid);
#else
#ifndef VMS
		my_strncpy (buf, myentry->pw_gecos, 128);
		ptr = strchr (buf, ',');
		if (ptr != (char *) 0) {
			*ptr = '\0';
		}
		/*
		 * check if SYSV (& lastname) hack is in gecos field
		 */
		ptr = strchr (buf, '&');
		if (ptr != (char *) 0) {
			*ptr++ = '\0';
			strcpy (tmp, userid);
			if (*tmp && *tmp >= 'a' && *tmp <= 'z') {
				*tmp = *tmp - 32;
			}
			sprintf (full_name, "%s%s%s", buf, tmp, ptr);
		} else {
			strcpy (full_name, buf);
		}
#else
		strcpy(full_name, fix_fullname(get_uaf_fullname()));
#endif

	if ((ptr = getenv ("NAME")) != (char *) 0) {
		my_strncpy (full_name, ptr, 128);
	}

	/*
	 * haha, supplying a from: line is not allowed, but this!
	 */

	if (userid == (char *) 0) {
		ptr = get_val ("USER", "please-set-USER-variable");
	} else {
		ptr = (char *) userid;
	}
	my_strncpy (user_name, ptr, 128);
#endif

#endif /* INDEX_DAEMON */
}

/*
 * Find full From: name in 'user@host (name)' format
 */

void
get_from_name (user_name, host_name, full_name, from_name)
	char *user_name;
	char *host_name;
	char *full_name;
	char *from_name;
{
#ifndef INDEX_DAEMON

	char domain[PATH_LEN];
	char nntp_inews_domain[PATH_LEN];
	char *ptr;

#ifdef FORGERY
	if (*mail_address) {
		strcpy(from_name, mail_address);
		return;
	}
#endif

	domain[0] = '\0';
	nntp_inews_domain[0] = '\0';

#ifdef INEWS_MAIL_DOMAIN
	if (*(INEWS_MAIL_DOMAIN)) {
		strcpy (nntp_inews_domain, INEWS_MAIL_DOMAIN);
	}
#endif

	if (! nntp_inews_domain[0]) {
		ptr = GetConfigValue (_CONF_FROMHOST);
		if (ptr != (char *) 0) {
			strncpy (nntp_inews_domain, ptr, sizeof (nntp_inews_domain));
		}
	}

	if (nntp_inews_domain[0]) {
		get_domain_name (nntp_inews_domain, domain);

		if (domain[0] == '.') {
			/*
			 * If host_name is a FQDN just get the hostname from it
			 * as a INEWS_MAIL_DOMAIN was specified to override it.
			 */
			ptr = strchr (host_name, '.');
			if (ptr != (char *) 0) {
				*ptr = '\0';
			}
			sprintf (from_name, "%s@%s%s (%s)",
				user_name, host_name, domain, full_name);
		} else {
			sprintf (from_name, "%s@%s (%s)", user_name, domain, full_name);
		}
	} else {
		if (host_name[0] == '%') {
			sprintf (from_name, "%s%s (%s)", user_name, host_name, full_name);
		} else {
			sprintf (from_name, "%s@%s (%s)", user_name, host_name, full_name);
		}
	}

	if (debug == 2) {
		sprintf (msg, "FROM=[%s] USER=[%s] HOST=[%s] NAME=[%s]",
			from_name, user_name, host_name, full_name);
		error_message (msg, "");
	}

#endif /* INDEX_DAEMON */
}


void
get_domain_name (inews_domain, domain)
	char	*inews_domain;
	char	*domain;
{
	char	*ptr;
	char	buf[PATH_LEN];
	FILE	*fp;

	*domain = '\0';

	/*
	 * If 1st letter is '$' read domain name from shell variable
	 */
	if (inews_domain[0] == '$' && inews_domain[1]) {
		ptr = getenv (&inews_domain[1]);
		if (ptr != (char *) 0) {
			strcpy (inews_domain, ptr);
		}
	}
	/*
	 * If 1st letter is '/' read domain name from specified file
	 */
	if (inews_domain[0] == '/') {
		if ((fp = fopen (inews_domain, "r")) != (FILE *) 0) {
			if (fgets (buf, sizeof (buf), fp) != (char *) 0) {
				ptr = strrchr (buf, '\n');
				if (ptr != (char *) 0) {
					*ptr = '\0';
					strcpy (domain, buf);
				}
			}
			fclose (fp);
		}
		if (! domain[0]) {
			strcpy (domain, "PROBLEM_WITH_YOUR_MAIL_DOMAIN_FILE");
		}
	} else {
		strcpy (domain, inews_domain);
	}
}


int
submit_news_file (name, lines)
	char *name;
	int   lines;
{
	char buf[LEN];
	char *cp = buf;
	int ret_code;

	checknadd_headers (name, lines);

    /* 7bit ISO-2022-KR is NEVER to be used in Korean news posting. */
        if ( !strcasecmp(mm_charset,"euc-kr") && ! strcasecmp(post_mime_encoding,"7bit") )
             post_mime_encoding[0] = '8';

	rfc15211522_encode(name, post_mime_encoding,post_8bit_header);

	if (read_news_via_nntp && use_builtin_inews) {
#ifdef DEBUG
		if (debug == 2) {
			error_message ("Using BUILTIN inews", "");
		}
#endif /* DEBUG */
		ret_code = submit_inews (name);
	} else {
#ifdef DEBUG
		if (debug == 2) {
			error_message ("Using EXTERNAL inews", "");
		}
#endif /* DEBUG */

#ifdef M_UNIX
#	ifdef INEWSDIR
		strcpy (buf, INEWSDIR);
		strcat (buf, "/");
		cp += strlen (cp);
#	endif /* INEWSDIR */
		sprintf (cp, "inews -h < %s", name);
#else
		make_post_cmd (cp, name);
#endif	/* M_UNIX */

		ret_code = invoke_cmd (buf);
	}

	return ret_code;
}
