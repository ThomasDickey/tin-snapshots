/*
 *  Project   : tin - a Usenet reader
 *  Module    : nntplib.c
 *  Author    : S.Barber & I.Lea
 *  Created   : 12-01-91
 *  Updated   : 22-12-94
 *  Notes     : NNTP client routines taken from clientlib.c 1.5.11 (10-02-91)
 *  Copyright : (c) Copyright 1991-94 by Stan Barber & Iain Lea
 *              Permission is hereby granted to copy, reproduce, redistribute
 *              or otherwise use this software  as long as: there is no
 *              monetary  profit  gained  specifically  from the use or
 *              reproduction or this software, it is not  sold, rented,
 *              traded or otherwise marketed, and this copyright notice
 *              is included prominently in any copy made.
 */

#include "tin.h"
#include "tcurses.h"
#include "tnntp.h"

/* Copy of last NNTP command sent, so we can retry it if needed */
char	last_put[NNTP_STRLEN];

#ifdef NNTP_ABLE
	TCP *nntp_rd_fp = NULL;
	TCP *nntp_wr_fp = NULL;
#endif

/* Close the NNTP connection with prejudice */
#define NNTP_HARD_CLOSE					\
	if (nntp_wr_fp)						\
		s_fclose (nntp_wr_fp);			\
	if (nntp_rd_fp)						\
		s_fclose (nntp_rd_fp);			\
	nntp_rd_fp = nntp_wr_fp = NULL;

/*
 * getserverbyfile(file)
 *
 * Get the name of a server from a named file.
 *	Handle white space and comments.
 *	Use NNTPSERVER environment variable if set.
 *
 *	Parameters: "file" is the name of the file to read.
 *
 *	Returns: Pointer to static data area containing the
 *	         first non-ws/comment line in the file.
 *	         NULL on error (or lack of entry in file).
 *
 *	Side effects: None.
 */

char *
getserverbyfile (
	const char *file)
{
#ifdef NNTP_ABLE
	FILE *fp;
	char *cp;
	static char buf[256];
#	ifdef HAVE_PUTENV
	char tmpbuf[256];
	char *new_env;
	static char *old_env = 0;
#	endif /* HAVE_PUTENV */

	if (cmdline_nntpserver[0] != '\0') {
		get_nntpserver (buf, cmdline_nntpserver);
#	ifdef HAVE_PUTENV
		sprintf (tmpbuf, "NNTPSERVER=%s", buf);
		new_env = my_malloc (strlen (tmpbuf) + 1);
		strcpy (new_env, tmpbuf);
		putenv (new_env);
		FreeIfNeeded (old_env);
		old_env = new_env;
#	else
		setenv ("NNTPSERVER", buf, 1);
#	endif /* HAVE_PUTENV */
		return (buf);
	}

	if ((cp = getenv ("NNTPSERVER")) != (char *) 0) {
		get_nntpserver (buf, cp);
		return (buf);
	}

	if (file == (char *) 0)
		return (char *) 0;

	if ((fp = fopen (file, "r")) != (FILE *) 0) {

		while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
			if (*buf == '\n' || *buf == '#')
				continue;

			if ((cp = strrchr (buf, '\n')) != NULL)
				*cp = '\0';

			(void) fclose (fp);
			return (buf);
		}

		(void) fclose (fp);

		if (cp != (char *) 0) {
			get_nntpserver (buf, cp);
			return (buf);
		}
	}

	if ((cp = GetConfigValue (_CONF_SERVER)) != (char *) 0) {
		(void) strcpy (buf, cp);
		return (buf);
	}

#endif /* NNTP_ABLE */
	return (char *) 0;	/* No entry */
}

/*
 * server_init  Get a connection to the remote server.
 *
 *	Parameters:	"machine" is the machine to connect to.
 *			"service" is the service to connect to on the machine.
 *			"port" is the servive port to connect to.
 *
 *	Returns:	server's initial response code
 *			or -errno
 *
 *	Side effects:	Connects to server.
 *			"nntp_rd_fp" and "nntp_wr_fp" are fp's
 *			for reading and writing to server.
 *			"text" is updated to contain the rest of response string from server
 */

#ifdef NNTP_ABLE
int
server_init (
	char *machine,
	const char *service,
	int port,
	char *text)
{
#ifndef VMS
	int	sockt_rd, sockt_wr;
#endif

#if defined (M_AMIGA) || defined(WIN32)
	if (s_init() == 0)                /* some initialisation ... */
		return -1;
#endif

#ifdef DECNET
	char	*cp;

	cp = strchr (machine, ':');

	if (cp && cp[1] == ':') {
		*cp = '\0';
		sockt_rd = get_dnet_socket (machine, service);
	} else {
		sockt_rd = get_tcp_socket (machine, service, port);
	}
#else
	sockt_rd = get_tcp_socket (machine, service, port);
#endif

	if (sockt_rd < 0)
		return (sockt_rd);

#ifndef VMS
	/*
	 * Now we'll make file pointers (i.e., buffered I/O) out of
	 * the socket file descriptor.  Note that we can't just
	 * open a fp for reading and writing -- we have to open
	 * up two separate fp's, one for reading, one for writing.
	 */
	if ((nntp_rd_fp = (TCP *) s_fdopen (sockt_rd, "r")) == NULL) {
		perror ("server_init: fdopen #1");
		return (-errno);
	}

	if ((sockt_wr = s_dup (sockt_rd)) < 0) {
		perror ("server_init: dup");
		return (-errno);
	}

#ifdef TLI
	if (t_sync (sockt_rd) < 0) {	/* Sync up new fd with TLI */
		t_error ("server_init: t_sync");
		nntp_rd_fp = NULL;		/* from above */
		return (-EPROTO);
	}
#endif

	if ((nntp_wr_fp = (TCP *) s_fdopen (sockt_wr, "w")) == NULL) {
		perror ("server_init: fdopen #2");
		nntp_rd_fp = NULL;		/* from above */
		return (-errno);
	}
#else /* VMS */
	sockt_wr = sockt_rd;
#endif

	/*
	 * Now get the server's signon message
	 */
	return (get_respcode(text));
}
#endif /* NNTP_ABLE */

/*
 * get_tcp_socket -- get us a socket connected to the specified server.
 *
 *	Parameters:	"machine" is the machine the server is running on.
 *			"service" is the service to connect to on the server.
 *			"port" is the port to connect to on the server.
 *
 *	Returns:	Socket connected to the server if all if ok
 *			EPROTO		for internal socket errors
 *			EHOSTUNREACH	if specified NNTP port/server can't be located
 *			errno		any valid errno value on connection
 *
 *	Side effects:	Connects to server.
 *
 *	Errors:		Returned & printed via perror.
 */

#ifdef NNTP_ABLE
int
get_tcp_socket (
	const char *machine,	/* remote host */
	const char *service,	/* nttp/smtp etc. */
	unsigned port)		/* tcp port number */
{
	int	s = -1;
	int save_errno = 0;
	struct	sockaddr_in	sock_in;

#ifdef TLI
	extern struct	hostent *gethostbyname ();
	struct	hostent	*hp;
	struct	t_call	*callptr;

	/*
	 * Create a TCP transport endpoint.
	 */
	if ((s = t_open ("/dev/tcp", O_RDWR, (struct t_info*) 0)) < 0){
		t_error ("t_open: can't t_open /dev/tcp");
		return (-EPROTO);
	}
	if (t_bind (s, (struct t_bind *) 0, (struct t_bind *) 0) < 0) {
		t_error ("t_bind");
		t_close (s);
		return (-EPROTO);
	}
	memset((char *) &sock_in, '\0', sizeof (sock_in));
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = htons (port);

	if (!isdigit((unsigned char)*machine) || (long)(sock_in.sin_addr.s_addr = inet_addr (machine)) == -1) {
		if ((hp = gethostbyname (machine)) == NULL) {
			my_fprintf (stderr, "gethostbyname: %s: host unknown\n", machine);
			t_close (s);
			return (-EHOSTUNREACH);
		}
		memcopy((char *) &sock_in.sin_addr, hp->h_addr, hp->h_length);
	}

	/*
	 * Allocate a t_call structure and initialize it.
	 * Let t_alloc() initialize the addr structure of the t_call structure.
	 */
	if ((callptr = (struct t_call *) t_alloc (s,T_CALL,T_ADDR)) == NULL){
		t_error ("t_alloc");
		t_close (s);
		return (-EPROTO);
	}

	callptr->addr.maxlen = sizeof (sock_in);
	callptr->addr.len = sizeof (sock_in);
	callptr->addr.buf = (char *) &sock_in;
	callptr->opt.len = 0;			/* no options */
	callptr->udata.len = 0;			/* no user data with connect */

	/*
	 * Connect to the server.
	 */
	if (t_connect (s, callptr, (struct t_call *) 0) < 0) {
		save_errno = t_errno;
		t_error ("t_connect");
		t_close (s);
		return (-save_errno);
	}

	/*
	 * Now replace the timod module with the tirdwr module so that
	 * standard read() and write() system calls can be used on the
	 * descriptor.
	 */

	if (ioctl (s,  I_POP,  (char *) 0) < 0) {
		perror ("I_POP(timod)");
		t_close (s);
		return (-EPROTO);
	}

	if (ioctl (s,  I_PUSH, "tirdwr") < 0) {
		perror ("I_PUSH(tirdwr)");
		t_close (s);
		return (-EPROTO);
	}

#else /* !TLI */
#ifndef EXCELAN
	struct servent *sp;
	struct hostent *hp;
#ifdef h_addr
	int	x = 0;
	register char **cp;
	static char *alist[1];
#endif /* h_addr */
	static struct hostent def;
	static struct in_addr defaddr;
	static char namebuf[256];

#ifdef HAVE_GETSERVBYNAME
	if ((sp = (struct servent *) getservbyname (service, "tcp")) ==  NULL) {
		my_fprintf (stderr, "%s/tcp: Unknown service.\n", service);
		return (-EHOSTUNREACH);
	}
#else
	sp = (struct servent *) malloc (sizeof (struct servent));
	sp->s_port = htons (IPPORT_NNTP);
#endif
	/* If not a raw ip address, try nameserver */
	if (!isdigit((unsigned char)*machine) || (long)(defaddr.s_addr = (long) inet_addr (machine)) == -1) {
		hp = gethostbyname (machine);
	} else {
		/* Raw ip address, fake  */
		(void) strcpy (namebuf, machine);
		def.h_name = (char *) namebuf;
#ifdef h_addr
		def.h_addr_list = alist;
#endif
		def.h_addr = (char *) &defaddr;
		def.h_length = sizeof (struct in_addr);
		def.h_addrtype = AF_INET;
		def.h_aliases = 0;
		hp = &def;
	}

	if (hp == NULL) {
		my_fprintf (stderr, "\n%s: Unknown host.\n", machine);
		return (-EHOSTUNREACH);
	}

	memset((char *) &sock_in, '\0', sizeof (sock_in));
	sock_in.sin_family = hp->h_addrtype;
	sock_in.sin_port = htons (port);
/*	sock_in.sin_port = sp->s_port; */
#else /* EXCELAN */
	memset((char *) &sock_in, '\0', sizeof (sock_in));
	sock_in.sin_family = AF_INET;
#endif /* EXCELAN */

	/*
	 * The following is kinda gross.  The name server under 4.3
	 * returns a list of addresses, each of which should be tried
	 * in turn if the previous one fails.  However, 4.2 hostent
	 * structure doesn't have this list of addresses.
	 * Under 4.3, h_addr is a #define to h_addr_list[0].
	 * We use this to figure out whether to include the NS specific
	 * code...
	 */

#ifdef h_addr

	/*
	 * Get a socket and initiate connection -- use multiple addresses
	 */
	for (cp = hp->h_addr_list; cp && *cp; cp++) {

		if ((s = socket (hp->h_addrtype, SOCK_STREAM, 0)) < 0) {
			perror ("socket");
			return (-errno);
		}

		memcpy((char *) &sock_in.sin_addr, *cp, hp->h_length);

		if (x < 0)
			my_fprintf (stderr, "Trying %s", (char *) inet_ntoa (sock_in.sin_addr));

#if defined(__hpux) && defined(SVR4)	/* recommended by raj@cup.hp.com */
#define	HPSOCKSIZE 0x8000
		getsockopt(s, SOL_SOCKET, SO_SNDBUF, (caddr_t)&socksize, (caddr_t)&socksizelen);
		if (socksize < HPSOCKSIZE) {
			socksize = HPSOCKSIZE;
			setsockopt(s, SOL_SOCKET, SO_SNDBUF, (caddr_t)&socksize, sizeof(socksize));
		}
		socksize = 0;
		socksizelen = sizeof(socksize);
		getsockopt(s, SOL_SOCKET, SO_RCVBUF, (caddr_t)&socksize, (caddr_t)&socksizelen);
		if (socksize < HPSOCKSIZE) {
			socksize = HPSOCKSIZE;
			setsockopt(s, SOL_SOCKET, SO_RCVBUF, (caddr_t)&socksize, sizeof(socksize));
		}
#endif

		if ((x = connect (s, (struct sockaddr *) &sock_in, sizeof (sock_in))) == 0)
			break;

		save_errno = errno;									/* Keep for later */
		my_fprintf (stderr, "\nConnection to %s: ", (char *) inet_ntoa (sock_in.sin_addr));
		perror ("");
		(void) s_close (s);
	}

	if (x < 0) {
		my_fprintf (stderr, "Giving up...\n");
		return (-save_errno);					/* Return the last errno we got */
	}
#else	/* no name server */

#ifdef EXCELAN
	if ((s = socket (SOCK_STREAM,(struct sockproto *)NULL,&sock_in,SO_KEEPALIVE)) < 0) {
		perror ("socket");
		return (-errno);
	}

	/* set up addr for the connect */
	memset((char *) &sock_in, '\0', sizeof (sock_in));
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = htons (IPPORT_NNTP);

	if ((sock_in.sin_addr.s_addr = rhost (&machine)) == -1) {
		my_fprintf (stderr, "\n%s: Unknown host.\n", machine);
		return (-1);
	}

	/* And connect */
	if (connect (s, (struct sockaddr *)&sock_in) < 0) {
		save_errno = errno;
		perror ("connect");
		(void) s_close (s);
		return (-save_errno);
	}

#else /* not EXCELAN */
	if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror ("socket");
		return (-errno);
	}

	/* And then connect */

	memcpy((char *) &sock_in.sin_addr, hp->h_addr, hp->h_length);

	if (connect (s, (struct sockaddr *) &sock_in, sizeof (sock_in)) < 0) {
		save_errno = errno;
		perror ("connect");
		(void) s_close (s);
		return (-save_errno);
	}

#endif /* !EXCELAN */
#endif /* !h_addr */
#endif /* !TLI */
	return (s);
}
#endif /* NNTP_ABLE */

#ifdef DECNET
/*
 * get_dnet_socket -- get us a socket connected to the server.
 *
 *	Parameters:	"machine" is the machine the server is running on.
 *			"service" is the name of the service to connect to.
 *
 *	Returns:	Socket connected to the news server if
 *			all is ok, else -1 on error.
 *
 *	Side effects:	Connects to server.
 *
 *	Errors:		Printed via nerror.
 */

int
get_dnet_socket (
	char	*machine,
	char	*service)
{
#ifdef NNTP_ABLE
	int	s, area, node;
	struct	sockaddr_dn sdn;
	struct	nodeent *getnodebyname(), *np;

	memset((char *) &sdn, '\0', sizeof (sdn));

	switch (s = sscanf (machine, "%d%*[.]%d", &area, &node)) {
		case 1:
			node = area;
			area = 0;
		case 2:
			node += area*1024;
			sdn.sdn_add.a_len = 2;
			sdn.sdn_family = AF_DECnet;
			sdn.sdn_add.a_addr[0] = node % 256;
			sdn.sdn_add.a_addr[1] = node / 256;
			break;
		default:
			if ((np = getnodebyname (machine)) == NULL) {
				my_fprintf (stderr, "%s: Unknown host.\n", machine);
				return (-1);
			} else {
				memcpy((char *) sdn.sdn_add.a_addr, np->n_addr, np->n_length);
				sdn.sdn_add.a_len = np->n_length;
				sdn.sdn_family = np->n_addrtype;
			}
			break;
	}
	sdn.sdn_objnum = 0;
	sdn.sdn_flags = 0;
	sdn.sdn_objnamel = strlen ("NNTP");
	memcpy(&sdn.sdn_objname[0], "NNTP", sdn.sdn_objnamel);

	if ((s = socket (AF_DECnet, SOCK_STREAM, 0)) < 0) {
		nerror ("socket");
		return (-1);
	}

	/* And then connect */

	if (connect (s, (struct sockaddr *) &sdn, sizeof (sdn)) < 0) {
		nerror ("connect");
		close (s);
		return (-1);
	}

	return (s);
#else
	return (-1);
#endif /* NNTP_ABLE */
}
#endif


/*
 * u_put_server -- send data to the server. Do not flush output.
 */

#ifndef VMS
#ifdef NNTP_ABLE
void
u_put_server (
	const char *string)
{
	s_puts(string, nntp_wr_fp);
}
#endif

/*
 * put_server -- send a line of text to the server, terminating it
 * with CR and LF, as per ARPA standard.
 *
 *	Parameters:	"string" is the string to be sent to the
 *			server.
 *
 *	Returns:	Nothing.
 *
 *	Side effects:	Talks to the server.
 *			Closes connection if things are not right.
 *
 *	Note:	This routine flushes the buffer each time
 *			it is called.  For large transmissions
 *			(i.e., posting news) don't use it.  Instead,
 *			do the fprintf's yourself, and then a final
 *			fflush.
 */
#ifndef VMS
#ifdef NNTP_ABLE
void
put_server (
	const char *string)
{
#if 0
	static time_t time_last;
	time_t time_now;
	int respcode;
#endif /* 0 */

	/*
	 * We remember the last thing we wrote, in case we have to do a command retry in the future
	 */
DEBUG_IO((stderr, "put_server(%s)\n", string));
	strcpy (last_put, string);

#if 0
	/*
	 *  Check how idle we have been, if too idle send a STAT to check
	 */
	time (&time_now);


	if (nntp_wr_fp == NULL || (time_last != 0 && time_last+NNTP_IDLE_RETRY_SECS-299 < time_now)) {

		if (nntp_wr_fp) {
			/*
			 * We don't care about the answer, just the fact that the connection still works
			 * Don't use nntp_command() here - this is a lower level
			 */
fprintf(stderr, "Timeout - sending STAT\n");
			s_printf (nntp_wr_fp, "STAT\r\n");
			s_flush (nntp_wr_fp);
			respcode = get_respcode (NULL);

			if (respcode == -1) {
				/*
				 *  STAT was not happy, close the connection
				 *  it will get reopened on next get_server
				 */
				NNTP_HARD_CLOSE;

				time_last = 0;
				return;
			}
		}
	}

	time_last = time_now;

#endif /* 0 */

	s_printf (nntp_wr_fp, "%s\r\n", string);
	(void) s_flush (nntp_wr_fp);

	return;
}
#endif /* NNTP_ABLE */
#endif /* VMS */

/*
 * Reconnect to server after a timeout, reissue last command to
 * get us back into the pre-timeout state
 */
#ifdef NNTP_ABLE
static int
reconnect(
	int retry)
{
	char buf[NNTP_STRLEN];

	/*
	 * Tear down current connection
	 */
	NNTP_HARD_CLOSE;
	ring_bell ();

DEBUG_IO((stderr, "\nServer timed out, trying reconnect # %d\n", retry));

	if (prompt_yn (cLINES, txt_reconnect_to_news_server, TRUE) != 1)
		tin_done(EXIT_OK);		/* user said no to reconnect */

	clear_message ();

	strcpy (buf, last_put);			/* Keep copy here, it will be clobbered a lot otherwise */

	if (nntp_open () == 0) {

		/*
		 * Re-establish our current group and resend last command
		 */
		if (glob_group != (char *) 0) {
DEBUG_IO((stderr, "Rejoin current group\n"));
			sprintf (last_put, "GROUP %s", glob_group);
			put_server (last_put);
			s_gets (last_put, NNTP_STRLEN, nntp_rd_fp);
DEBUG_IO((stderr, "Read (%s)\n", last_put));
		}
DEBUG_IO((stderr, "Resend last command (%s)\n", buf));
		put_server (buf);
		return(0);
	}

	if (--retry == 0)					/* No more tries ? */
		tin_done(EXIT_NNTP_ERROR);

	return(retry);
}

/*
 * Read a line of data from the NNTP socket. If something gives, do reconnect
 *
 *	Parameters:	"string" has the buffer space for the line received.
 *			"size" is maximum size of the buffer to read.
 *
 *	Returns:	NULL on end of stream, or a line of data.
 *				Basically, we try to act like fgets() on an NNTP stream.
 *
 *	Side effects:	Talks to server, changes contents of "string".
 *			Reopens connection when necessary and requested.
 *			Exits via tin_done() if fatal error occurs.
 */
char *
get_server (
	char *string,
	int	size)
{
	int retry = NNTP_TRY_RECONNECT;

	errno = 0;

	/*
	 * NULL socket reads indicates socket has closed. Try a few times more
	 */
	while (nntp_rd_fp == NULL || s_gets (string, size, nntp_rd_fp) == (char *) 0) {

#ifdef DEBUG
		if (errno != 0 && errno != EINTR)	/*	I'm sure this will only confuse end users*/
			perror_message("get_server()");
#endif

		retry = reconnect(retry);			/* Will abort when out of tries */
	}

	return string;
}
#endif /* NNTP_ABLE */

/*
 * close_server -- close the connection to the server, after sending
 *		the "QUIT" command.
 *
 *	Parameters:	None.
 *
 *	Returns:	Nothing.
 *
 *	Side effects:	Closes the connection with the server.
 *			You can't use "put_server" or "get_server"
 *			after this routine is called.
 */

#ifdef NNTP_ABLE
void
close_server (void)
{
	if (nntp_wr_fp == NULL || nntp_rd_fp == NULL)
		return;

	info_message("Disconnecting from server...");
	nntp_command("QUIT", OK_GOODBYE, NULL);

	(void) s_fclose (nntp_wr_fp);
	(void) s_fclose (nntp_rd_fp);
	s_end();
	nntp_wr_fp = nntp_rd_fp = NULL;
}
#endif /* NNTP_ABLE */
#endif /* VMS */

#ifdef DEBUG
/*
 * NNTP strings for get_respcode()
 */
const char *
nntp_respcode (
	int respcode)
{
#ifdef NNTP_ABLE
	static const char *text;

	switch (respcode) {
		case 0:
			text = "";
			break;
		case INF_HELP:
			text = "100  Help text on way";
			break;
		case INF_AUTH:
			text = "180  Authorization capabilities";
			break;
		case INF_DEBUG:
			text = "199  Debug output";
			break;
		case OK_CANPOST:
			text = "200  Hello; you can post";
			break;
		case OK_NOPOST:
			text = "201  Hello; you can't post";
			break;
		case OK_SLAVE:
			text = "202  Slave status noted";
			break;
		case OK_GOODBYE:
			text = "205  Closing connection";
			break;
		case OK_GROUP:
			text = "211  Group selected";
			break;
		case OK_GROUPS:
			text = "215  Newsgroups follow";
			break;
		case OK_XMOTD:
			text = "217  News motd file follows";
			break;
		case OK_XINDEX:
			text = "218  Group index file follows";
			break;
		case OK_ARTICLE:
			text = "220  Article (head & body) follows";
			break;
		case OK_HEAD:
			text = "221  Head follows";
			break;
		case OK_BODY:
			text = "222  Body follows";
			break;
		case OK_NOTEXT:
			text = "223  No text sent -- stat, next, last";
			break;
		case OK_NEWNEWS:
			text = "230  New articles by message-id follow";
			break;
		case OK_NEWGROUPS:
			text = "231  New newsgroups follow";
			break;
		case OK_XFERED:
			text = "235  Article transferred successfully";
			break;
		case OK_POSTED:
			text = "240  Article posted successfully";
			break;
		case OK_AUTHSYS:
			text = "280  Authorization system ok";
			break;
		case OK_AUTH:
			text = "281  Authorization (user/pass) ok";
			break;
		case OK_BIN:
			text = "282  binary data follows";
			break;
		case OK_SPLIST:
			text = "283  spooldir list follows";
			break;
		case OK_SPSWITCH:
			text = "284  Switching to a different spooldir";
			break;
		case OK_SPNOCHANGE:
			text = "285  Still using same spooldir";
			break;
		case OK_SPLDIRCUR:
			text = "286  Current spooldir";
			break;
		case OK_SPLDIRAVL:
			text = "287  Available spooldir";
			break;
		case OK_SPLDIRERR:
			text = "288  Unavailable spooldir or invalid entry";
			break;
		case CONT_XFER:
			text = "335  Continue to send article";
			break;
		case CONT_POST:
			text = "340  Continue to post article";
			break;
		case NEED_AUTHINFO:
			text = "380  authorization is required";
			break;
		case NEED_AUTHDATA:
			text = "381  <type> authorization data required";
			break;
		case ERR_GOODBYE:
			text = "400  Have to hang up for some reason";
			break;
		case ERR_NOGROUP:
			text = "411  No such newsgroup";
			break;
		case ERR_NCING:
			text = "412  Not currently in newsgroup";
			break;
		case ERR_XMOTD:
			text = "417  No news motd file";
			break;
		case ERR_XINDEX:
			text = "418  No index file for this group";
			break;
		case ERR_NOCRNT:
			text = "420  No current article selected";
			break;
		case ERR_NONEXT:
			text = "421  No next article in this group";
			break;
		case ERR_NOPREV:
			text = "422  No previous article in this group";
			break;
		case ERR_NOARTIG:
			text = "423  No such article in this group";
			break;
		case ERR_NOART:
			text = "430  No such article at all";
			break;
		case ERR_GOTIT:
			text = "435  Already got that article, don't send";
			break;
		case ERR_XFERFAIL:
			text = "436  Transfer failed";
			break;
		case ERR_XFERRJCT:
			text = "437  Article rejected, don't resend";
			break;
		case ERR_NOPOST:
			text = "440  Posting not allowed";
			break;
		case ERR_POSTFAIL:
			text = "441  Posting failed";
			break;
		case ERR_NOAUTH:
			text = "480  authorization required for command";
			break;
		case ERR_AUTHSYS:
			text = "481  Authorization system invalid";
			break;
		case ERR_AUTHREJ:
			text = "482  Authorization data rejected";
			break;
		case ERR_INVALIAS:
			text = "483  Invalid alias on spooldir cmd";
			break;
		case ERR_INVNOSPDIR:
			text = "484  No spooldir file found";
			break;
		case ERR_COMMAND:
			text = "500  Command not recognized";
			break;
		case ERR_CMDSYN:
			text = "501  Command syntax error";
			break;
		case ERR_ACCESS:
			text = "502  Access to server denied";
			break;
		case ERR_FAULT:
			text = "503  Program fault, command not performed";
			break;
		case ERR_AUTHBAD:
			text = "580  Authorization Failed";
			break;
		default:
			text = "Unknown NNTP response code";
			break;
	}
	return (text);

#else
	return ("");
#endif
}
#endif /* DEBUG */
