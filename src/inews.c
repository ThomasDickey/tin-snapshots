/*
 *  Project   : tin - a Usenet reader
 *  Module    : inews.c
 *  Author    : I. Lea
 *  Created   : 1992-03-17
 *  Updated   : 1997-12-31
 *  Notes     : NNTP builtin version of inews
 *  Copyright : (c) Copyright 1991-98 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */


#include	"tin.h"
#include	"tnntp.h"


/*
 * local prototypes
 */
#ifdef NNTP_INEWS
	static int submit_inews (char *name);
#endif /* NNTP_INEWS */

#if defined (NNTP_INEWS) && !defined(FORGERY)
	static int sender_needed (char * from, char * sender);
#endif /* NNTP_INEWS && !FORGERY */

#if 0
#	ifdef VMS
#		ifdef MULTINET
#			define netwrite	socket_write
#			include "multinet_root:[multinet.include]netdb.h"
#		else
#			define netwrite	write
#			include <netdb.h>
#		endif /* MULTINET */
#	else
#		ifdef HAVE_NETDB_H
#			include	<netdb.h>
#		endif /* HAVE_NETDB_H */
#	endif /* VMS */

#	ifdef HAVE_SYS_SOCKET_H
#		include <sys/socket.h>
#	endif /* HAVE_SYS_SOCKET_H */
#	ifdef HAVE_NETINET_IN_H
#		include <netinet/in.h>
#	endif /* HAVE_NETINET_IN_H */
#endif /* 0 */

#define	PATHMASTER	"not-for-mail"

/*
 * Submit an article using the NNTP POST command
 */
#ifdef NNTP_INEWS
static int
submit_inews (
	char *name)
{
	FILE *fp;
	char *ptr;
	char from_name[PATH_LEN];
	char message_id[PATH_LEN];
	char line[NNTP_STRLEN];
	int auth_error = 0;
	int respcode;
	t_bool leave_loop = FALSE;
	t_bool id_in_article = FALSE;
	t_bool ret_code = FALSE;
#	ifndef FORGERY
	t_bool ismail = FALSE;
#	endif /* FORGERY */

	if ((fp = fopen (name, "r")) == (FILE *) 0) {
		perror_message (txt_cannot_open, name);
		return ret_code;
	}

	from_name[0] = '\0';
	message_id[0] = '\0';

	while (fgets (line, (int) sizeof (line), fp) != NULL) {
		if (line[0] != '\n') {
			ptr = strchr (line, ':');
			if (ptr - line == 4 && !strncasecmp (line, "From", 4)) {
				strcpy(from_name, ptr+2);
				if ((ptr = strchr(from_name, '\n')))
					*ptr = '\0';
			}
			if (ptr - line == 10 && !strncasecmp (line, "Message-ID", 10)) {
				strcpy(message_id, ptr+2);
				id_in_article = TRUE;
				if ((ptr = strchr(message_id, '\n')))
					*ptr = '\0';
			}
		} else
			break; /* end of headers */
	}

	if (from_name[0] == '\0') {
		/* we could silently add a From: line here if we want to... */
		error_message (txt_error_no_from);
		fclose (fp);
		return ret_code;
	}
	/*
	 * check for @ and that at least one '.' comes after the '@' in the From: line
	 */
	/* see also post.c/insert_from_header() */
	if ((ptr = strchr (from_name, '@')) != (char *) 0) {
		if ((ptr = strchr (ptr, '.')) == (char *) 0) { /* no '.' */
			error_message (txt_invalid_from, from_name);
			fclose (fp);
			return ret_code;
		}
	} else { /* no '@' */
		error_message (txt_invalid_from, from_name);
		fclose (fp);
		return ret_code;
	}


	/*
	 * Send POST command to NNTP server
	 * Receive CONT_POST or ERROR response code from NNTP server
	 */
	do {
		rewind(fp);
		if (nntp_command("POST", CONT_POST, line) == NULL) {
			error_message ("%s", line);
			fclose (fp);
			return ret_code;
		}

		/*
		 * check article if it contains a Message-ID header
		 * if not scan line if it contains a Message-ID
		 * if it's present: use it.
		 */
		if (message_id[0] == '\0') {
			char * ptr2;

			/* simple syntax check - locate last '<' */
			if ((ptr = strrchr (line, '<')) != (char *) 0) {
				/* search next '>' */
				if ((ptr2 = strchr (ptr, '>')) != (char *) 0) {
					/* terminate string */
					*++ptr2 = '\0';
					/* check for @ and no whitespaces */
					if ((strchr(ptr, '@') != (char *) 0) && (strpbrk(ptr, " \t") == (char *) 0))
						/* copy Message-ID */
						strcpy(message_id, ptr);
				}
			}
		}

		/*
		 * Send Path: and From: article headers
		 */
#	ifndef FORGERY
		sprintf (line, "Path: %s", PATHMASTER);
		put_server (line);

		if ((ptr = build_sender())) {
			if (sender_needed(rfc1522_decode(from_name), ptr) == 1) {
				sprintf (line, "Sender: %s", rfc1522_encode(ptr, ismail));
				put_server (line);
			}
		}
#	endif /* !FORGERY */

		/*
		 * check if Message-ID comes from the server
		 */
		if (*message_id && !id_in_article) {
				sprintf (line, "Message-ID: %s", message_id);
				put_server (line);
		}

		/*
		 * Send article 1 line at a time ending with "."
		 */
		while (fgets (line, (int) sizeof (line), fp) != (char *) 0) {
			/*
			 * Remove linefeed from line
			 */
			if ((ptr = strrchr (line, '\n')) != (char *) 0)
				*ptr = '\0';

			/*
			 * If line starts with a '.' add another '.' to stop truncation
			 */
			if (line[0] == '.')
				u_put_server (".");

			u_put_server (line);
			u_put_server ("\r\n");
		}

		put_server (".");

		/*
		 * Receive OK_POSTED or ERROR response code from NNTP server
		 * Don't use get_respcode at this point, because then we would not
		 * recognize if posting has failed due to missing authentication in
		 * which case the complete posting has to be resent. Besides, because
		 * of the put_server(".") above a "." would be resent as the last
		 * "command".
		 */
		 /*
		  * here we could add a check if the server returns the
		  * Message-ID in the response string...
		  * if it does so, compare it with message_id (if set)
		  * and pass it to update_posted_info_file() and
		  * quick_filter_select_posted_art() if add_posted_to_filter
		  * is set
		  * make sure, that quick_filter_select_posted_art() and
		  * update_posted_info_file() arn't called twice!
		  */
		respcode = get_only_respcode (line);
		leave_loop = TRUE;

		/*
		 * Don't leave this loop if we only tried once to post and an
		 * authentication request was received. Leave loop on any other
		 * response or any further authentication requests.
		 */
		if (((respcode == ERR_NOAUTH) || (respcode == NEED_AUTHINFO)) && (auth_error++ < 1) && (authenticate (nntp_server, userid, FALSE)))
			leave_loop = FALSE;
	} while (!leave_loop);

	fclose (fp);

	/*
	 * FIXME: The displayed message may be wrong if authentication has
	 * failed.  (The message will be sth. like "Authentication required"
	 * which is not really wrong but misleading. The problem is that
	 * authenticate() does only return a bool value and not the server
	 * response.)
	 */
	if (respcode != OK_POSTED) {
		error_message ("Posting failed %s", line);
		return ret_code;
	}

	ret_code = TRUE;

	return ret_code;
}
#endif /* NNTP_INEWS */

/*
 * Call submit_inews() if using builtin inews, else invoke external inews prog
 */
int
submit_news_file (
	char *name)
{
	char buf[LEN];
	char *cp = buf;
	int ret_code;
	t_bool ismail=FALSE;

	checknadd_headers (name);

	/* 7bit ISO-2022-KR is NEVER to be used in Korean news posting. */
	if (!(strcasecmp(mm_charset, "euc-kr") || strcasecmp(txt_mime_encodings[post_mime_encoding], txt_7bit)))
		post_mime_encoding = 0;	/* FIXME: txt_8bit */

	rfc15211522_encode(name, txt_mime_encodings[post_mime_encoding], post_8bit_header, ismail);

#ifdef NNTP_INEWS
	if (read_news_via_nntp && !read_saved_news && use_builtin_inews) {
#	ifdef DEBUG
		if (debug == 2)
			error_message ("Using BUILTIN inews");
#	endif /* DEBUG */
		ret_code = submit_inews (name);
	} else
#endif /* NNTP_INEWS */
		{
#ifdef DEBUG
		if (debug == 2)
			error_message ("Using EXTERNAL inews");
#endif /* DEBUG */

#ifdef M_UNIX
		if (*inewsdir) {
			strcpy (buf, inewsdir);
			strcat (buf, "/");
			cp += strlen (cp);
		}
		sh_format (cp, sizeof(buf) - (cp - buf), "inews -h < %s", name);
#else
		make_post_cmd (cp, name);
#endif /* M_UNIX */

		ret_code = invoke_cmd (buf);
	}

	return ret_code;
}


/*
 * FIXME: do _real_ RFC822-parsing - currently this is a quick hack
 *        to cover the most usual cases...
 *
 * returnvalues:  1 = Sender needed
 *                0 = no Sender needed
 *               -1 = error (no '.' and/or '@' in From)
 *               -2 = error (no '.' and/or '@' in Sender)
 */
#if defined (NNTP_INEWS) && !defined(FORGERY)
static int sender_needed (
	char * from,
	char * sender)
{
	char * from_at_pos;
	char * from_login_pos;
	char * from_end_pos;
	char * sender_at_pos;
	char * sender_dot_pos;
	char * sender_login_pos;

	/* skip realname in from */
	if ((from_login_pos = strchr (from, '<')) == (char *) 0) {
		/* address in user@domain (realname) syntax or realname is missing */
		from_login_pos = from;
		if ((from_end_pos = strchr (from_login_pos, ' ')) == (char *) 0)
			from_end_pos = from_login_pos+strlen(from_login_pos);
	} else {
		from_login_pos++; /* skip '<' */
		from_end_pos = from_login_pos+strlen(from_login_pos)-1; /* skip '>' */
	}

	if ((from_at_pos = strchr (from_login_pos, '@')) == (char *) 0)
		return -1; /* no '@' in from */

	/* skip realname in sender */
	if ((sender_login_pos = strchr (sender, '<')) == (char *) 0)
		/* address in user@domain (realname) syntax or realname missing */
		sender_login_pos = sender;
	else
		sender_login_pos++; /* skip '<' */

	if ((sender_at_pos = strchr (sender_login_pos, '@')) == (char *) 0)
		return -2; /* no '@' in sender */

	if (strncasecmp (from_login_pos, sender_login_pos, (from_at_pos - from_login_pos)))
		return 1; /* login differs */

	if (strchr (from_at_pos, '.') == (char *) 0)
		return -1; /* no '.' in from */

	if ((sender_dot_pos = strchr (sender_at_pos, '.')) == (char *) 0)
		return -2; /* no '.' in sender */

	if (strncasecmp (from_at_pos, sender_at_pos, (from_end_pos - from_at_pos))) {
		/* skip the 'hostname' in sender */
		if (strncasecmp (from_at_pos+1, sender_dot_pos+1, (from_end_pos - from_at_pos - 1)))
			return 1; /* domainname differs */
	}

	return 0;
}
#endif /* NNTP_INEWS && !FORGERY */
