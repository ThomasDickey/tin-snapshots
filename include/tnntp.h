/*
 *  Project   : tin - a Usenet reader
 *  Module    : tnntp.h
 *  Author    : Thomas Dickey <dickey@clark.net>
 *  Created   : 1997-03-05
 *  Updated   : 1997-12-01
 *  Notes     : #include files, #defines & struct's
 *  Copyright : (c) Copyright 1997 by Thomas Dickey
 *	             You may  freely  copy or  redistribute  this software,
 *	             so  long as there is no profit made from its use, sale
 *	             trade or  reproduction.  You may not change this copy-
 *	             right notice, and it must be included in any copy made
 */

#ifndef TNNTP_H
#define TNNTP_H 1

#if defined(VMS) && defined(SOCKETSHR_TCP)
#	ifdef __GNUC__
#		define __SOCKET_TYPEDEFS
#	endif /* __GNUC__ */
#	include <netdb.h>
#	include <in.h>
#	include <inet.h>
#	include <socket.h>
#	define s_printf	si_fprintf
#	define s_fdopen	si_fdopen
#	define s_flush	si_fflush
#	define s_fclose	si_fclose
#	define s_gets	si_fgets
#	define s_close	si_close
#	define s_puts	si_fputs
#	define s_dup
#	define s_init()	(1)
#	define s_end()
#else
#	ifdef M_AMIGA
#		include "amigatcp.h"
#	else
#		define s_printf	fprintf
#		define s_fdopen	fdopen
#		define s_flush	fflush
#		define s_fclose	fclose
#		define s_gets	fgets
#		define s_close	close
#		define s_puts	fputs
#		define s_dup		dup
#		define s_init()	(1)
#		define s_end()
#	endif /* M_AMIGA */
#endif /* VMS && SOCKETSHR_TCP */

#if defined(NNTP_ABLE) || defined(HAVE_GETHOSTBYNAME)
#	ifdef HAVE_NETDB_H
#		include <netdb.h>
#	endif /* HAVE_NETDB_H */
#if defined( __amigaos__ ) /* JK 101097 */
#	define IPPORT_NNTP ((unsigned short) 119)
#endif /* !__amigaos__ */
#	ifdef TLI
#		include	<fcntl.h>
#		include	<tiuser.h>
#		include	<stropts.h>
#		include	<sys/socket.h>
#		include	<netinet/in.h>
#		define IPPORT_NNTP	((unsigned short) 119)
#	else
#		ifdef VMS
#			ifdef MULTINET
#				include "MULTINET_ROOT:[multinet.include]errno.h"
#				include "MULTINET_ROOT:[multinet.include]netdb.h"
#				include "MULTINET_ROOT:[multinet.include.vms]inetiodef.h"
#				include "MULTINET_ROOT:[multinet.include.sys]socket.h"
#				include "MULTINET_ROOT:[multinet.include.netinet]in.h"
#				define netopen	socket_open
#				define netread	socket_read
#				define netwrite socket_write
#				define netclose socket_close
#			else
#				ifdef UCX
#					include <errno.h>
#					include <iodef.h>
#					include <in.h>
#					include <socket.h>
#					define netopen	open
#					define netread	read
#					define netwrite	write
#					define netclose	close
#					define IPPORT_NNTP	((unsigned short) 119)
#				endif /* UCX */
#			endif /* MULTINET */
#		else
#			include <sys/socket.h>
#			include <netinet/in.h>
#			ifdef HAVE_NETLIB_H
#				include <netlib.h>
#			endif /* HAVE_NETLIB_H */
#			ifdef HAVE_ARPA_INET_H
#				include <arpa/inet.h>
#			endif /* HAVE_ARPA_INET_H */
#		endif /* VMS */
#	endif /* TLI */

#	ifdef EXCELAN
		extern int connect (int, struct sockaddr *);
		extern unsigned short htons (unsigned short);
		extern unsigned long rhost (char **);
		extern int rresvport (int);
		extern int socket (int, struct sockproto *, struct sockaddr_in *, int);
#	endif /* EXCELAN */

#	ifdef DECNET
#		include <netdnet/dn.h>
#		include <netdnet/dnetdb.h>
#	endif /* DECNET */

#endif /* NNTP_ABLE || HAVE_GETHOSTBYNAME */

#ifndef MAXHOSTNAMELEN
#	define MAXHOSTNAMELEN 255
#endif /* !MAXHOSTNAMELEN */

#ifndef SOCKS
#	ifdef DECL_CONNECT
	extern int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
#	endif /* DECL_CONNECT */
#endif /* !SOCKS */

#ifdef DECL_INET_NTOA
	extern char *inet_ntoa (struct in_addr);
#endif /* DECL_INET_NTOA */

#endif /* TNNTP_H */
