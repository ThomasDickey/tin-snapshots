/*
 *  Project   : tin - a Usenet reader
 *  Module    : amigatcp.c
 *  Author    : M.Tomlinson
 *  Created   : 15-05-94
 *  Updated   : 15-05-94
 *  Notes     : TCP support for AmigaDOS
 *  Copyright : (c) Copyright 1994 by Mark Tomlinson
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"

#if defined (M_AMIGA) && defined (NNTP_ABLE)

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "amigatcp.h"

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <proto/exec.h>

/* #include <clib/netlib_protos.h> */

#ifndef EOF
#define EOF (-1)
#endif

#undef  s_close
#undef  socket

#ifdef AS225
#       define SocketBase       SockBase
#       define SOCKLIB          "inet:libs/socket.library"
#       define MAXSOCK          5
#       define CloseSocket      s_close
#       define IoctlSocket      s_ioctl
#else
#       define SOCKLIB          "bsdsocket.library"
#       define cleanup_sockets()
#endif

struct Library *SocketBase= 0;
#ifndef AS225
struct Library *UserGroupBase = NULL;
#endif

static int sock_dups[32];

#ifndef AS225
#ifndef select
select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exeptfds,
         struct timeval *timeout)
{
  /* call WaitSelect with NULL signal mask pointer */
  return WaitSelect(nfds, readfds, writefds, exeptfds, timeout, NULL);
}
#endif
#ifndef inet_ntoa
char *inet_ntoa(struct in_addr addr)
{
  return Inet_NtoA(addr.s_addr);
}
#endif
#endif

int s_init(void)
{
        if (! SocketBase) {
                SocketBase = OpenLibrary(SOCKLIB,0L);
                if (! SocketBase) return 0;

#ifdef AS225
                setup_sockets(MAXSOCK,&errno);
#else
                SetErrnoPtr(&errno, sizeof(errno));
                if (!(UserGroupBase = OpenLibrary(USERGROUPNAME, 0L)))
                    return 0;
#endif
                atexit(s_end);
        }
        return 1;
}

void __interrupt s_end(void)
{
        if (SocketBase) {
                cleanup_sockets();
                CloseLibrary(SocketBase);
#ifndef AS225
                CloseLibrary(UserGroupBase);
#endif
                SocketBase = 0;
        }
}

TCP *s_fdopen(int s, const char *mode)
{
        TCP *tp;

        tp = malloc(sizeof(TCP));
        if (! tp) return (TCP *)0;

        tp->size = 2048;
        tp->base = malloc(tp->size);
        if (! tp->base) {
                free(tp);
                return (TCP *)0;
        }

        tp->_file = s;
        tp->cnt = 0;
        tp->buf = tp->base;
        tp->flags = (mode[0] == 'w') ? TIO_WRITE : TIO_READ;

        return tp;
}

void s_fclose(TCP *tp)
{
        if (tp->cnt)
                s_flush(tp);
        free(tp->base);
        xs_close(tp->_file);
        free(tp);
}

void xs_close(int fd)
{       if (--sock_dups[fd] <= 0)
                CloseSocket(fd);
}

int s_socket(long domain, long type, long protocol)
{
        int s;
        s = socket(domain, type, protocol);

        if (s >= 0) {
                sock_dups[s] = 1;
        }
        return s;
}

int s_dup(int fd)
{
        sock_dups[fd]++;
        return fd;
}

int s_flush(TCP *tp)
{
        int ret;

        if (tp->cnt && tp->flags & TIO_WRITE) {
                ret = send(tp->_file, tp->base, tp->cnt, 0);
                tp->cnt = 0;
                tp->buf = tp->base;
                return ret;
        }
        return 0;
}

int s_printf(TCP *tp, const char *format, ...)
{
        va_list ap;
        int ret;

        va_start(ap, format);

        if (tp->size - tp->cnt < 1024) {
                ret = s_flush(tp);
                if (ret < 0) return ret;
        }

        ret = vsprintf(tp->buf, format, ap);

        if (ret > 0) {
                tp->buf += ret;
                tp->cnt += ret;
        }

        va_end(ap);
        return ret;
}

int s_puts(const char *str, TCP *tp)
{
        int ret, length;

        length = strlen(str);

        if (tp->cnt && tp->size - tp->cnt < length) {
                ret = s_flush(tp);
                if (ret < 0) return ret;
        }

        while (tp->size - tp->cnt < length) {
                ret = send(tp->_file, (char *)str, length, 0);
                if (ret < 0) return ret;
                length -= ret;
                if (length == 0) return 0;
                str += ret;
        }

        memcpy(tp->buf, str, length);
        tp->buf += length;
        tp->cnt += length;

        return 0;
}

char *s_gets(char *str, int size, TCP *tp)
{
        int c;
        char *cp;
        long length;

        cp = str;
        if (size == 0) return (char *)0;

        forever {
                while(tp->cnt && size > 1) {
                        --tp->cnt;
                        --size;
                        c = *cp++ = *tp->buf++;
                        if (c == '\n') {
                                *cp = 0;
                                return str;
                        }
                }

                if (size <= 1) {
                        *cp = 0;
                        return str;
                }

                tp->buf = tp->base;
                do {
                        IoctlSocket(tp->_file, FIONREAD, (char *)&length);
                        if (length == 0) {
                                fd_set rfd, efd;
                                struct timeval timeout;

                                timeout.tv_secs = 60;
                                timeout.tv_micro = 0;

                                FD_ZERO(&rfd);
                                FD_SET(tp->_file, &rfd);
                                efd = rfd;
                                select(tp->_file+1, &rfd, NULL, &efd, &timeout);
                                if (! FD_ISSET(tp->_file, &rfd)) {
                                        *cp = 0;
                                        return 0;
                                }
                        }
                } while(length == 0);

                if (length > tp->size) length = tp->size;
                length = recv(tp->_file, tp->buf, length, 0L);
                tp->cnt = length;
        }
}

#endif  /* M_AMIGA */
