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
#include	"tnntp.h"

#if 0
#ifdef VMS
#   ifdef MULTINET
#		define netwrite	socket_write
#		include "multinet_root:[multinet.include]netdb.h"
#   else
#		define netwrite	write
#		include <netdb.h>
#   endif
#else
#	ifdef HAVE_NETDB_H
#		include	<netdb.h>
#	endif
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#endif	/* 0 */

#define	PATHMASTER	"not-for-mail"

static int
submit_inews (
	char *name)
{
	int	ret_code = FALSE;

#if !defined(INDEX_DAEMON)

#ifdef NNTP_INEWS
	char	from_name[PATH_LEN];
	char	full_name[128];
	char	user_name[128];
	char	line[NNTP_STRLEN];
	char	*ptr;
	FILE	*fp;
	int	respcode;
#ifndef FORGERY
	int	ismail=FALSE;
#endif

	if ((fp = fopen (name, "r")) == (FILE *) 0) {
		return ret_code;
	}

	get_user_info (user_name, full_name);
	get_from_name (from_name);

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
	 * Check that domain is not of type host.subdomain.domain
	 */
	ptr = strstr (from_name, "subdomain.domain");
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
	sprintf (line, "Path: %s", PATHMASTER);
	put_server (line);

	if ((ptr = build_sender())) {
		if(strcasecmp(from_name, ptr)) {
			sprintf (line, "Sender: %s", rfc1522_encode(ptr,ismail));
			put_server (line);
		}
	}
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

int
submit_news_file (
	char *name,
	int   lines)
{
	char buf[LEN];
	char *cp = buf;
	int ret_code;
	t_bool ismail=FALSE;

	checknadd_headers (name, lines);

    /* 7bit ISO-2022-KR is NEVER to be used in Korean news posting. */
    if (!(strcasecmp(mm_charset, "euc-kr") || strcasecmp(txt_mime_types[post_mime_encoding], txt_7bit)))
    	post_mime_encoding = 0;	/* FIXME: txt_8bit */

	rfc15211522_encode(name, txt_mime_types[post_mime_encoding], post_8bit_header, ismail);

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
