/*
 *  Project   : tin - a Usenet reader
 *  Module    : amigatcp.h
 *  Author    : M.Tomlinson
 *  Created   : 15-05-94
 *  Updated   : 05-06-94
 *  Notes     : TCP support for AmigaDOS
 *  Copyright : (c) Copyright 1994 by Mark Tomlinson
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#if defined(M_AMIGA)

#ifndef AMIGATCP_H
#define AMIGATCP_H

#ifdef NNTP_ABLE
#ifdef AS225
#include <ss/socket.h>
#else
/* Stop bsdsocket including an incompatible unistd.h! */
#define UNISTD_H
/* And also get rid of prototypes for the netlib we're not using */
#define CLIB_NETLIB_PROTOS_H
#include <proto/usergroup.h>
#include <bsdsocket.h>
/* But we still need prototypes for select() and inet_ntoa() */
/* if we're not using optimisation... */
#ifndef inet_ntoa
char * inet_ntoa(struct in_addr addr);
#endif
#ifndef select
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exeptfds,
           struct timeval *timeout);
#endif
#endif

struct __tcpbuf
{
        int             _file;  /*vb*/          /* socket number */
	char 	*buf;	/* current buffer pointer */
	char	*base;	/* beginning of buffer */
	int		size;	/* size of buffer */
	int		cnt;	/* number of bytes in buf */
	int		flags;	/* read/write */
};

#define TIO_WRITE	1
#define	TIO_READ	2

extern int	s_printf(TCP *, const char *, ...);
extern TCP	*s_fdopen(int, const char *);
extern int	s_flush(TCP *);
extern void	s_fclose(TCP *);
extern char	*s_gets(char *, int, TCP *);
extern int	s_puts(const char *, TCP *);
extern int	s_init(void);
extern void	s_end(void);
extern int	s_dup(int);
extern int	s_socket(long, long, long);
extern void	xs_close(int);

#define s_close	xs_close
#define socket	s_socket

/* AS225 screws up a bit on types for inet_ntoa... */
#ifdef AS225
static char * __inline xinet_ntoa(long x)
{	return inet_ntoa(x);
}
#define inet_ntoa(x)	xinet_ntoa((x).s_addr)
#endif

#endif	/* NNTP_ABLE */

#endif	/* AMIGATCP_H */

#endif	/* M_AMIGA */
