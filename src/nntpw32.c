/*
 *  Project   : tin - a Usenet reader
 *  Module    : nntpw32.c
 *  Author    : N.Ellis (portions taken from nntplib.c)
 *  Created   : 12-01-91
 *  Updated   : 15-12-94
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

#if defined(WIN32)
#if defined(NNTP_ABLE)

char	last_put[NNTP_STRLEN];
static	char nntp_line[NNTP_STRLEN];

#ifdef WIN32XNS
#define PACKET_SIZE 4096
#define cmemBlock PACKET_SIZE
// #define XNS_ASYNC			// XNS ASYNC send and receive
#else
#define cmemBlock (8*1024)
#endif

static char memBlock[cmemBlock+4];
static UINT imemBlock = 0;
static UINT iendBlock = 0;

#if !defined(WIN32IP) && !defined(WIN32XNS) && !defined(WIN32PIPES)
#pragma message("No network transport defined, defaulting to WIN32IP")
#define WIN32IP
#endif

#ifdef DEBUG
void DebugAssertDialog(LPSTR, DWORD, LPSTR);
#define DEBUGCheckString(s, m)	if (*s=='\0') DebugAssertDialog(m, __LINE__, __FILE__);
#else
#define DEBUGCheckString(s, m)
#endif

#endif /* NNTP_ABLE */

// Transport independant routines...

/*
 * handle_server_response
 *
 *	Print some informative messages based on the server's initial
 *	response code.  This is here so inews, rn, etc. can share
 *	the code.
 *
 *	Parameters:	"response" is the response code which the
 *			server sent us, presumably from "server_init",
 *			above.
 *			"nntpserver" is the news server we got the
 *			response code from.
 *
 *	Returns:	-1 if the error is fatal (and we should exit).
 *			0 otherwise.
 *
 *	Side effects:	None.
 */

int
handle_server_response (response, nntpserver)
	int	response;
	char	*nntpserver;
{
#ifdef NNTP_ABLE
	switch (response) {
		case OK_NOPOST:		/* fall through */
   		 	printf ("NOTE: This machine does not have permission to post articles.\n");
			printf ("      Please don't waste your time trying.\n\n");

		case OK_CANPOST:
			return (0);
			break;

		case ERR_ACCESS:
			printf ("This machine does not have permission to use the %s news server.\n", nntpserver);
			return (-1);
			break;

		default:
			printf ("Unexpected response code from %s news server: %d\n",
				nntpserver, response);
			return (-1);
			break;
    }
	/*NOTREACHED*/
#else
	return (-1);
#endif /* NNTP_ABLE */
}

#ifdef DEBUG
/*
 * NNTP strings for get_respcode()
 */
char *
nntp_respcode (respcode)
	int respcode;
{
#ifdef NNTP_ABLE
	static char *text;

	/*
	 * If the last response line matches and has a description, return it
	 */
	if (atoi (nntp_line) == respcode && strlen (nntp_line) > 4) {
		return nntp_line;
	}

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

/*
 * getserverbyfile	Get the name of a server from a named file.
 *			Handle white space and comments.
 *			Use NNTPSERVER environment variable if set.
 *
 *	Parameters:	"file" is the name of the file to read.
 *
 *	Returns:	Pointer to static data area containing the
 *			first non-ws/comment line in the file.
 *			NULL on error (or lack of entry in file).
 *
 *	Side effects:	None.
 */

char *
getserverbyfile (file)
	char	*file;
{
#ifdef NNTP_ABLE
	register FILE	*fp;
	register char	*cp;
	static char	buf[256];

	cp = getenv ("NNTPSERVER");
	if (cp != (char *) 0) {
		(void) strcpy (buf, cp);
		return (buf);
	}

	cp = GetConfigValue (_CONF_SERVER);
	if (cp != (char *) 0) {
		(void) strcpy (buf, cp);
		return (buf);
	}

	if (file == (char *) 0) {
		return (char *) 0;
	}

	if ((fp = fopen (file, "r")) == (FILE *) 0) {
		return (char *) 0;
	}

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		if (*buf == '\n' || *buf == '#') {
			continue;
		}
		cp = strrchr (buf, '\n');
		if (cp) {
			*cp = '\0';
		}
		(void) fclose (fp);
		return (buf);
	}

	(void) fclose (fp);
#endif /* NNTP_ABLE */
	return (char *) 0;	/* No entry */
}

/*=-=-=-=-=-=-=-=-=-=
 * TCP/IP Transport *
 *=-=-=-=-=-=-=-=-=-=*/

#ifdef WIN32IP	/* TCP/IP Transport */

#include <winsock.h>

int nntp_rd_fp = 0;
int nntp_wr_fp = 0;

/*
 * server_init  Get a connection to the remote server.
 *
 *	Parameters:	"machine" is the machine to connect to.
 *			"service" is the service to connect to on the machine.
 *			"port" is the servive port to connect to.
 *
 *	Returns:	-1 on error
 *			server's initial response code on success.
 *
 *	Side effects:	Connects to server.
 *			"nntp_rd_fp" and "nntp_wr_fp" are fp's
 *			for reading and writing to server.
 */

int
server_init (machine, service, port)
	char	*machine;
	char	*service;
	int	port;
{
#ifdef NNTP_ABLE
	int	sockt_rd;
	WSADATA wsaData;

	if ( WSAStartup( MAKEWORD(1,1), &wsaData) != 0) {
		return -1;
	}

	sockt_rd = get_tcp_socket (machine, service, port);

	if (sockt_rd < 0)
		return (-1);

	nntp_rd_fp = nntp_wr_fp = sockt_rd;
	/*
	 * Now get the server's signon message
	 */
	(void) get_server (nntp_line, sizeof (nntp_line));
	return (atoi (nntp_line));
#else
	return (-1);
#endif /* NNTP_ABLE */
}

/*
 * get_tcp_socket -- get us a socket connected to the specified server.
 *
 *	Parameters:	"machine" is the machine the server is running on.
 *			"service" is the service to connect to on the server.
 *			"port" is the port to connect to on the server.
 *
 *	Returns:	Socket connected to the server if
 *			all is ok, else -1 on error.
 *
 *	Side effects:	Connects to server.
 *
 *	Errors:		Printed via perror.
 */

int
get_tcp_socket (machine, service, port)
	char	*machine;	/* remote host */
	char	*service;	/* nttp/smtp etc. */
	unsigned port;		/* tcp port number */
{
#ifdef NNTP_ABLE
	int	s = -1;
	struct	sockaddr_in sin;
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

	if ((sp = (struct servent *) getservbyname (service, "tcp")) ==  NULL) {
		fprintf (stderr, "%s/tcp: Unknown service.\n", service);
		return (-1);
	}
	/* If not a raw ip address, try nameserver */
	if (! isdigit((unsigned char)*machine) ||
	    (long)(defaddr.s_addr = (long) inet_addr (machine)) == -1) {
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
		fprintf (stderr, "\n%s: Unknown host.\n", machine);
		return (-1);
	}

	memset((char *) &sin, '\0', sizeof (sin));
	sin.sin_family = hp->h_addrtype;
	sin.sin_port = sp->s_port;

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
	 * get a socket and initiate connection -- use multiple addresses
	 */

	for (cp = hp->h_addr_list; cp && *cp; cp++) {
		s = socket (hp->h_addrtype, SOCK_STREAM, 0);
		if (s < 0) {
			perror ("socket");
			return (-1);
		}
		memcpy((char *) &sin.sin_addr, *cp, hp->h_length);

		if (x < 0) {
			fprintf (stderr, "Trying %s", (char *) inet_ntoa (sin.sin_addr));
		}
		x = connect (s, (struct sockaddr *) &sin, sizeof (sin));
		if (x == 0) {
			break;
		}
		fprintf (stderr, "\nConnection to %s: ", (char *) inet_ntoa (sin.sin_addr));
		perror ("");
		(void) closesocket (s);
	}
	if (x < 0) {
		fprintf (stderr, "Giving up...\n");
		return (-1);
	}
#else	/* no name server */
	if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror ("socket");
		return (-1);
	}

	/* And then connect */

	memcpy((char *) &sin.sin_addr, hp->h_addr, hp->h_length);
	if (connect (s, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
		perror ("connect");
		(void) closesocket (s);
		return (-1);
	}

#endif /* !h_addr */
	/* Disable nagling to prevent packet delays */
	{
	BOOL fOn = TRUE;
	(void) setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char *) &fOn, sizeof(fOn));
	}

	return (s);
#else
	return (-1);
#endif /* NNTP_ABLE */
}


/*
 * u_put_server -- send data to the server. Do not flush output.
 */

void
u_put_server (string)
	char *string;
{
#ifdef	NNTP_ABLE
	send(nntp_wr_fp, string, strlen(string), 0);
#endif
}

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

void
put_server (string)
	char *string;
{
#ifdef NNTP_ABLE
	int respno;
	static time_t time_last;
	time_t time_now;

	DEBUGCheckString(string, "NULL NNTP Command");
	/*
	 *  Check how idle we have been, if too idle send a STAT to check
	 */
	time (&time_now);
	if (time_last != 0 && time_last+NNTP_IDLE_RETRY_SECS < time_now) {
		send(nntp_wr_fp, "stat\r\n", 6, 0);
		respno = get_respcode ();
		if (respno != OK_NOTEXT && respno != ERR_NCING && respno != ERR_NOCRNT) {
			/*
			 *  STAT was not happy, close the connection
			 *  it will reopen on next get_server
			 */
			closesocket (nntp_wr_fp);
			closesocket (nntp_rd_fp);
			strcpy (last_put, string);
			time_last = 0;
			return;
		}
	}
	time_last = time_now;

	sprintf(nntp_line, "%s\r\n", string);
	send(nntp_wr_fp, nntp_line, strlen(nntp_line), 0);

	strcpy (last_put, string);
#endif /* NNTP_ABLE */
}

#ifdef NNTP_ABLE
static int read_from_server (LPSTR nntp_line, DWORD length)
{
	DWORD cbRead = 0, cb;
	char *pEol;

	if ( imemBlock == iendBlock )
	{
		pEol = 0;
	}
	else
	{
		pEol = strstr(memBlock+imemBlock, "\r\n");	/* search for a newline                */
		if (pEol > memBlock+iendBlock-2)			/* if one was found outside the buffer */
			pEol = 0;								/* then we didn't really find one      */
	}

	if ( !pEol)
	{
		iendBlock -= imemBlock;
		if ( iendBlock)
			memcpy( memBlock, memBlock+imemBlock, iendBlock );
		imemBlock = 0;
		do {
			cbRead = recv( nntp_rd_fp, memBlock+iendBlock, cmemBlock - iendBlock, 0);
			if (cbRead == 0 || cbRead == SOCKET_ERROR)
			{
				return -1;
			}

			iendBlock += cbRead;

			pEol = strstr(memBlock, "\r\n");				/* search for a newline */
		} while( pEol > memBlock+iendBlock-2);
	}

	cb = pEol - (memBlock+imemBlock);

	memcpy (nntp_line, memBlock+imemBlock, cb);
	nntp_line[cb] = '\0';
	imemBlock += cb + 2;

	assert(imemBlock<=iendBlock);

	return (strlen (nntp_line));
}
#endif /* NNTP_ABLE */

/*
 * get_server -- get a line of text from the server.  Strips
 * CR's and LF's.
 *
 *	Parameters:	"string" has the buffer space for the
 *			line received.
 *			"size" is the size of the buffer.
 *
 *	Returns:	-1 on error, 0 otherwise, -2 if user said no to reconnection.
 *
 *	Side effects:	Talks to server, changes contents of "string".
 *			Reopens connection when necessary and requested.
 */

int
get_server (string, size)
	char	*string;
	int	size;
{
#ifdef NNTP_ABLE
	int cbRead;

	cbRead = read_from_server(string, size);
	if ( cbRead == -1)
	{
		closesocket(nntp_wr_fp);
		if (nntp_open () == 0)
		{
			char buf[NNTP_STRLEN];

			strcpy(buf, last_put);
			if (glob_group != (char *) 0)
			{
				char tmp[NNTP_STRLEN];
				sprintf (tmp, "group %s", glob_group);
				put_server (tmp);
				cbRead = read_from_server (tmp, NNTP_STRLEN);
			}
			put_server (buf);
			cbRead = read_from_server (string, size);
		}
	}

	return cbRead;
#else
	return -1;
#endif /* NNTP_ABLE */
}

/*
 * close_server -- close the connection to the server, after sending
 *		the "quit" command.
 *
 *	Parameters:	None.
 *
 *	Returns:	Nothing.
 *
 *	Side effects:	Closes the connection with the server.
 *			You can't use "put_server" or "get_server"
 *			after this routine is called.
 */

void
close_server ()
{
#ifdef NNTP_ABLE
	if (nntp_wr_fp == 0 || nntp_rd_fp == 0)
		return;

	put_server ("QUIT");
	(void) get_server (nntp_line, sizeof (nntp_line));

	(void) closesocket (nntp_wr_fp);
#endif /* NNTP_ABLE */
}

#endif /* WIN32IP */

/*=-=-=-=-=-=-=-=-=-=-=-=*
 * Named pipes Transport *
 *=-=-=-=-=-=-=-=-=-=-=-=*/

#ifdef WIN32PIPES
HANDLE hPipe;

/*
 * server_init get a connection to a remote server
 *
 *
 *  returns -1 on error
 */

int server_init (LPCSTR machine, LPCSTR service, USHORT port)
{
	BOOL fSuccess;
	DWORD dwMode;
	CHAR lpszPipename[128];

	strcpy (lpszPipename, "\\\\");
	strcat (lpszPipename, machine);
	strcat (lpszPipename, "\\pipe\\");
	strcat (lpszPipename, service);

	/* try and open the pipe */
	while (1)
	{
		hPipe = CreateFile (
							   lpszPipename,	/* pipe name           */
							   GENERIC_READ |	/* read/write access   */
							   GENERIC_WRITE,
							   0,	/* no sharing          */
							   NULL,	/* no security attr. */
							   OPEN_EXISTING,	/* opens existing pipe */
							   0,	/* default attributes  */
							   NULL);	/* no template file    */

		/* Break if the pipe handle is valid. */

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		/* Exit if an error other than ERROR_PIPE_BUSY occurs */

		if (GetLastError () != ERROR_PIPE_BUSY)
			return (-1);

		/* All pipe instances are busy, so wait for 20 seconds. */

		if (! WaitNamedPipe (lpszPipename, 20000))
			return (-1);

	}
	dwMode = PIPE_READMODE_BYTE;
	fSuccess = SetNamedPipeHandleState (
										   hPipe,	/* pipe handle          */
										   &dwMode,		/* new pipe mode        */
										   NULL,	/* don't set max. bytes */
										   NULL);	/* don't set max. time  */

	if (! fSuccess)
		return (-1);
	/*
	 * now get the servers signon message
	 */
	(void) get_server (nntp_line, sizeof (nntp_line));

	return (atoi (nntp_line));
}

/*
 * real_put_server -- send a line of text to the server, terminating it
 * with  CR and LF if asked, as per the ARPA standard
 *
 * Parameters: the line to put to the server
 * Side effects: talks to the server and closes the link if problems
 *
 */
static void real_put_server (LPCSTR plpvMessage, int fCRLF)
{
	BOOL fSuccess;
	DWORD cbWritten;

	fSuccess = WriteFile (hPipe, plpvMessage, strlen (plpvMessage), &cbWritten, NULL);
	if ( fSuccess && fCRLF)
		fSuccess = WriteFile (hPipe, "\r\n", 2, &cbWritten, NULL);

	if (! fSuccess)
	{
		char buf[NNTP_STRLEN];

		CloseHandle (hPipe);
		strcpy (buf, plpvMessage);
		if (nntp_open () == 0)
		{
			if (glob_group != (char *) 0)
			{
				char tmp[NNTP_STRLEN];
				sprintf (tmp, "group %s", glob_group);
				put_server (tmp);
				get_server (tmp, NNTP_STRLEN);
			}
			put_server (buf);
		}
	}
}

/*
 * u_put_server -- send data to the server. Do not flush output.
 */
_inline void
u_put_server (string)
	char *string;
{
#ifdef	NNTP_ABLE
	real_put_server(string, 0);
#endif
}

/*
 * put_server -- send a line of text to the server, terminating it
 * with  CR and LF, as per the ARPA standard
 */
void put_server (LPCSTR plpvMessage)
	{
	DEBUGCheckString(plpvMessage, "NULL NNTP Command");
	real_put_server(plpvMessage, 1);
	}

/*
 * close_server close the connection to the server after sending the 'quit'
 * command
 *
 * Side effects: closes the connections to the server, you can't use put/get
 * after this call
 */

void close_server ()
{
	put_server ("QUIT\r\n");
	get_server (nntp_line, sizeof (nntp_line));
	CloseHandle (hPipe);
}

int get_server (LPSTR nntp_line, DWORD length)
{
	DWORD cbRead = 0, cb;
	char *pEol;
	BOOL fSuccess;

	if (imemBlock == iendBlock)
	{
		pEol = 0;
	}
	else
	{
		/* search for a newline */
		pEol = strstr (memBlock + imemBlock, "\r\n");

		/* if one was found outside the buffer */
		if (pEol > memBlock + iendBlock - 2)
			pEol = 0;		/* then we didn't really find one */

	}

	if (! pEol)
	{
		iendBlock -= imemBlock;
		if (iendBlock)
			memcpy (memBlock, memBlock + imemBlock, iendBlock);
		imemBlock = 0;
		do
		{
			fSuccess = ReadFile (
				hPipe,	/* pipe handle */
				(LPVOID) (memBlock + iendBlock),	/* buffer to receive reply */
				cmemBlock - iendBlock,				/* size of buffer          */
				&cbRead,							/* number of bytes read    */
				NULL);								/* not overlapped          */

			assert(fSuccess);

			iendBlock += cbRead;

			pEol = strstr (memBlock, "\r\n");	/* search for a newline */

		}
		while (pEol > memBlock + iendBlock - 2);
	}

	cb = pEol - (memBlock + imemBlock);

	memcpy (nntp_line, memBlock + imemBlock, cb);
	nntp_line[cb] = '\0';
	imemBlock += cb + 2;

	assert (imemBlock <= iendBlock);

	return (strlen (nntp_line));
}

#endif /* WIN32PIPES */


/*=-=-=-=-=-=-=-=-=-=-=-=*
 * XNS Netbios Transport *
 *=-=-=-=-=-=-=-=-=-=-=-=*/

#ifdef WIN32XNS
static int LSN;
static unsigned LANA;
static int neterrno = 0x00;

static int rc;          /* generic return code placeholder */

/* Misc NB functions for low level network I/O */
static int netpname(unsigned, char *);
static int checknet(void);
static int resetnet(unsigned);
static int netconnect(unsigned *, char *, char *);
static int netaddname(unsigned, char *);
static int netsend(unsigned, int, char *, unsigned);
static int netreceive(unsigned, int, char *, unsigned);
static void nethangup(unsigned, int);
static int netlana(LANA_ENUM *plenum);
static int get_connection(unsigned *, char *);

static int passncb(struct _NCB *);
static void netperror(char *);

#define SubmitNCB(ncb)	Netbios(ncb)
#define HOSTNAMSZ       8       /* max size of a machine name */
#define ACK_CHAR        0x06    /* expected reply from server daemon */

/*
 * server_init get a connection to a remote server
 *
 *
 *  returns -1 on error
 */

int server_init (LPCSTR machine, LPCSTR service, USHORT port)
	{
	/*
	 * Make the connection to the server
	 *
	 */
	strcpy(nntp_line, machine);

	if ((LSN = get_connection(&LANA, nntp_line)) < 0)
		return -1;

	printf("connected...");

	/* Now get the server's signon message */
	get_server(nntp_line, sizeof(nntp_line));

	return (atoi (nntp_line));
	}

void real_put_server (LPCSTR plpvMessage, int fCRLF)
	{
	DWORD cbWritten;
	unsigned packet, length;

	length = (unsigned) strlen(plpvMessage);
	packet = 0;

	if (length - packet)
		{
		cbWritten = netsend(LANA, LSN, (char *) plpvMessage + packet, length - packet);
		assert(cbWritten!=-1);
		}

	if (fCRLF)
		{
		cbWritten = netsend(LANA, LSN, (char *)"\r\n", 2);
		assert(cbWritten!=1);
		}

	return /* (packet + cbWritten) */;
	}

/*
 * u_put_server -- send data to the server. Do not flush output.
 */

_inline void u_put_server (string)
	char *string;
	{
#ifdef	NNTP_ABLE
	real_put_server(string, 0);
#endif
	}

/*
 * put_server -- send a line of text to the server, terminating it
 * with  CR and LF, as per the ARPA standard
 *
 * Parameters: the line to put to the server
 * Side effects: talks to the server and closes the link if problems
 *
 */
void put_server(LPCSTR plpvMessage)
	{
	DEBUGCheckString(plpvMessage, "NULL NNTP Command");
	real_put_server(plpvMessage, 1);
	}

/*
 * close_server close the connection to the server after sending the 'quit'
 * command
 *
 * Side effects: closes the connections to the server, you can't use put/get
 * after this call
 */

void close_server ()
	{
	if (LSN == 0)
		return;

	put_server ("QUIT");
	get_server (nntp_line, sizeof (nntp_line));

	nethangup(LANA, LSN);
	LSN = 0;
	}

int get_server (LPSTR nntp_line, DWORD length)
	{
	DWORD cbRead = 0, cb;
	char *pEol;
	BOOL fSuccess;

	if (imemBlock == iendBlock)
		{
		pEol = 0;
		}
	else
		{
		/* search for a newline */
		pEol = strstr (memBlock + imemBlock, "\r\n");

		/* if one was found outside the buffer */
		if (pEol > memBlock + iendBlock - 2)
			pEol = 0;		/* then we didn't really find one */
		}

	if (! pEol)
		{
		iendBlock -= imemBlock;
		if (iendBlock)
			memcpy (memBlock, memBlock + imemBlock, iendBlock);
		imemBlock = 0;
		do
			{
			cbRead = netreceive(LANA, LSN, memBlock + iendBlock, cmemBlock - iendBlock);
			fSuccess = (cbRead != -1);

			assert(fSuccess);

			iendBlock += cbRead;

			pEol = strstr (memBlock, "\r\n");	/* search for a newline */
			}
		while (pEol > memBlock + iendBlock - 2);
		}

	assert(pEol != (char *) NULL);

	cb = pEol - (memBlock + imemBlock);

	assert(cb <= length);
	memcpy (nntp_line, memBlock + imemBlock, cb);
	nntp_line[cb] = '\0';
	imemBlock += cb + 2;

	assert (imemBlock <= iendBlock);

	return (strlen (nntp_line));
	}

int	netaddname(unsigned lana, char *lname)
	{
	struct _NCB ncb={0};

	if (lname == NULL || *lname == '\0')
		return -1;

	ncb.ncb_command = NCBADDNAME;
	ncb.ncb_lana_num = LANA;
	memcpy(ncb.ncb_name, (char *)lname, NCBNAMSZ);

	rc = passncb(&ncb);
	if (rc < 0)
		rc = -1;
	else
		rc = ncb.ncb_num;

	return rc;
	}

/*
 * get_connection -- get us a connection to the news server.
 *
 *	Parameters:	"machine" is the machine the server is running on.
 *
 *      Returns:        LSN connected to the news server if
 *                      all is ok, else -1 on error.
 */

int	get_connection(unsigned *lana, char *machine)
	{
	int s;

	if ((s = checknet()) < 0)
		{
		netperror("Net not found");
		}
	else
		{
		if ((s = netconnect(lana, machine, "nntp")) < 0)
			{
			netperror("Trouble connecting");
			}
		}
	return s;
	}

void nethangup(unsigned lana, int lsn)
	{
	struct _NCB ncb={0};

	ncb.ncb_command = NCBHANGUP;
	ncb.ncb_lsn = (unsigned char) lsn;
	ncb.ncb_lana_num = lana;

	passncb(&ncb);
	}

unsigned char netcall(unsigned lana, char *lname, char *rname)
	{
	struct _NCB ncb={0};

	if (lname == NULL)
		{
	    /* use permanent node name */
		if (netpname(lana, ncb.ncb_name) < 0)
			{
			return 0;
			}
		}
	else
		{
		if ((netaddname(lana, lname) < 0) &&
			(neterrno != NRC_DUPNAME))
			{
			return 0;
			}

		memcpy(ncb.ncb_name, (char *)lname, NCBNAMSZ);
		}

	ncb.ncb_command = NCBCALL;
	ncb.ncb_lana_num = lana;
	memcpy(ncb.ncb_callname, (char *)rname, NCBNAMSZ);

	rc = passncb(&ncb);

	if (rc < 0)
		rc = 0;
	else
		rc = ncb.ncb_lsn;

	return rc;
	}

int	netconnect(unsigned *lana, char *rname, char *service)
	{
	char    callname[NCBNAMSZ];
	unsigned lsn=0;
	int     saverr;
	int i;
	LANA_ENUM lenum={0};

	/* find the valid lana numbers for the network */
	if (netlana(&lenum)==0)
		{
		perror("No adapters found");
		return -1;
		}

	memset(callname, '\0', NCBNAMSZ);
	strncpy(callname, rname, HOSTNAMSZ);
	strcat(callname, ".srv");

	// Try to connect to the service using the valid lana numbers
	for (i=0; i < lenum.length; i++)
		{
		if ((resetnet(lenum.lana[i])!=-1) && ((lsn = netcall(lenum.lana[i], NULL, callname)) != (unsigned char) 0))
			break;
		}

	/* Failed ... */
	if (i == lenum.length)
		{
		*lana = 0;
		return -1;
		}
	else
		{
		/* Found.. */
		*lana = lenum.lana[i];
		}

	/* tell the server daemon what service we want */
	if (netsend(*lana, lsn, service, strlen(service) + 1) != -1)
		{
		if (netreceive(*lana, lsn, callname, 1) != -1)
			{
			if (*callname == ACK_CHAR)
				{
				return lsn;
				}
			}
		}

	nethangup(*lana, lsn);
	*lana = 0;
	saverr = neterrno;
	neterrno = saverr;
	return -1;
	}

int netpname(unsigned lana, char *pname)
	{
	ADAPTER_STATUS stats={0};
	struct _NCB ncb={0};

	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = lana;
	ncb.ncb_callname[0] = '*';
	ncb.ncb_length = sizeof(ADAPTER_STATUS);
	ncb.ncb_buffer = (char *) &stats;

	memset(stats.adapter_address, 0, 6);

	if (passncb(&ncb) < 0)
		rc = -1;
	else
		{
		memset(pname, '\0', 10);
		memcpy(pname + 10, stats.adapter_address, 6);
		rc = 0;
		}

	return rc;
	}

/*
** This function needs to calculate the 'real' number of bytes returned
** into the buffer.
*/
int	netreceive(unsigned lana, int lsn, char *data, unsigned nbytes)
	{
	struct _NCB ncb = {0};
	char buffer[PACKET_SIZE + 1]={0};

	ncb.ncb_command = NCBRECV;
#ifdef XNS_ASYNC
	ncb.ncb_command |= ASYNCH;
#endif
	ncb.ncb_lana_num = lana;
	ncb.ncb_lsn = (unsigned char) lsn;
	ncb.ncb_length = nbytes;
	ncb.ncb_buffer = (char *) buffer;

	rc = passncb(&ncb);

	if (rc < 0)
		rc = -1;
	else
		{
		if (nbytes==1)
			{
			rc = 1;
			*data = *buffer;
			}
		else
			{
			rc = strlen(buffer);
			memcpy(data, buffer, rc);
			}
		}

	return rc;
	}

int	netsend(unsigned lana, int lsn, char *data, unsigned nbytes)
	{
	struct _NCB ncb={0};
	char buffer[PACKET_SIZE + 1];

	if (nbytes == 0)
		{
		neterrno = NRC_BUFLEN;
		return -1;
		}

	memcpy(buffer, (char *) data, nbytes);

	ncb.ncb_command = NCBSEND;
#ifdef XNS_ASYNC
	ncb.ncb_command |= ASYNCH;
#endif
	ncb.ncb_lana_num = lana;
	ncb.ncb_lsn = (unsigned char) lsn;
	ncb.ncb_length = nbytes;
	ncb.ncb_buffer = (char *) buffer;

	rc = passncb(&ncb);

	if (rc < 0)
		rc = -1;
	else
		rc = ncb.ncb_length;

	return rc;
	}

int	checknet(void)
	{
	struct _NCB ncb={0};

	/* Send illegal command */
	ncb.ncb_command = 0x7f;

	passncb(&ncb);

	rc = neterrno;
	if (rc == NRC_ILLCMD)
		rc = neterrno = 0;

	return rc;
	}

int	resetnet(unsigned lana)
	{
	struct _NCB ncb={0};

	// Send a RESET for a specific LANA number.
	ncb.ncb_command = NCBRESET;
	ncb.ncb_lana_num = lana;
	ncb.ncb_lsn = 0;

	ncb.ncb_callname[0] = (unsigned char) 16;	// 16 sessions
	ncb.ncb_callname[1] = (unsigned char) 255;	// 255 commands
	ncb.ncb_callname[2] = (unsigned char) 8;	// 8 names
	ncb.ncb_callname[3] = (unsigned char) 1;	// Request non-perm node name

	rc = passncb(&ncb);

	rc = neterrno;
	if (rc != NRC_GOODRET)
		rc = neterrno = -1;

	return rc;
	}

int	netlana(LANA_ENUM *plenum)
	{
	struct _NCB ncb={0};

	/* Send a ENUM to get the network numbers */
	ncb.ncb_command = NCBENUM;
	ncb.ncb_length = sizeof(LANA_ENUM);
	ncb.ncb_buffer = (unsigned char *)plenum;
	if (ncb.ncb_buffer == (unsigned char *) NULL)
		{
		rc = neterrno = -1;
		return rc;
		}

	passncb(&ncb);

	rc = plenum->length;
	if (ncb.ncb_retcode != NRC_GOODRET)
		rc = neterrno = -1;

	return rc;
	}

#pragma optimize("a", on)
#pragma optimize("gw", off)
int passncb(PNCB ncb)
	{
	if (ncb->ncb_command & ASYNCH)
		{
		ncb->ncb_event = CreateEvent(NULL, FALSE, FALSE, "passncb");
		assert(ncb->ncb_event != NULL);

		neterrno = Netbios(ncb);

		if (WaitForSingleObject(ncb->ncb_event, INFINITE)==WAIT_FAILED)
			{
			CloseHandle(ncb->ncb_event);
			return -1;
			}

		CloseHandle(ncb->ncb_event);
		}
	else
		{
		neterrno = Netbios(ncb);
		}

	if (neterrno && neterrno != NRC_INCOMP)
		return -1;
	else
		return 0;
	}
#pragma optimize("gw", on)
#pragma optimize("a", off)

void
netperror(char *wintitle)
	{
	fprintf(stderr, "%s (NetBIOS code 0x%x)\n", wintitle, neterrno);
	neterrno = 0;
	}

#endif /* WIN32XNS */

#endif /* WIN32 */
