/*
 *  Project   : tin - a Usenet reader
 *  Module    : win32tcp.c
 *  Author    : N.Ellis
 *  Created   : 01-10-94
 *  Updated   : 07-11-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Nigel Ellis
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"

#if defined(WIN32) && defined(NNTP_ABLE)

#include "win32tcp.h"
#include <winsock.h>

#define	cmemBlock	(8*1024)
static char memBlock[cmemBlock + 4];
static UINT imemBlock = 0;
static UINT iendBlock = 0;

static TCP mytcp;

static int sock_dups;

int
s_init()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		return 0;
	}
	return 1;
}

TCP *
s_fdopen(int s, const char *mode)
{
	mytcp.fd = s;
	return &mytcp;
}

void
s_fclose(TCP * tp)
{
	s_close(tp->fd);
}

void
s_close(int s)
{
	if (--sock_dups[s] <= 0)
		closesocket(s);
}

int
s_dup(int fd)
{
	sock_dups[fd]++;
	return fd;
}

char *
s_gets(LPSTR nntp_line, DWORD length, TCP * tp)
{
	DWORD cbRead = 0, cb;
	char *pEol;

	if (imemBlock == iendBlock) {
		pEol = 0;
	} else {
		pEol = strstr(memBlock + imemBlock, "\r\n");		/* search for a newline */
		if (pEol > memBlock + iendBlock - 2)	/* if one was found outside the buffer */
			pEol = 0;				  /* then we didn't really find one */
	}

	if (!pEol) {
		iendBlock -= imemBlock;
		if (iendBlock)
			memcpy(memBlock, memBlock + imemBlock, iendBlock);
		imemBlock = 0;
		do {
			cbRead = recv(nntp_rd_fp, memBlock + iendBlock, cmemBlock - iendBlock, 0);
			if (cbRead == 0 || cbRead == SOCKET_ERROR) {
				errno = SOCKET_ERROR;
				return (char *) NULL;
			}
			iendBlock += cbRead;

			pEol = strstr(memBlock, "\r\n");		// search for a newline

		} while (pEol > memBlock + iendBlock - 2);
	}
	cb = pEol - (memBlock + imemBlock);

	memcpy(nntp_line, memBlock + imemBlock, cb);
	nntp_line[cb] = '\0';
	imemBlock += cb + 2;

	assert(imemBlock <= iendBlock);

	return nntp_line;
}

#endif /* WIN32 */
