/*
** Project   : tin - a Usenet reader
** Module    : header.c
** Author    : Urs Janssen <urs@akk.uni-karlsruhe.de>
** Created   : 10.03.97
** Updated   : 19.03.97
** Copyright : (c) Copyright 1997 by Urs Janssen
**             You may  freely  copy or  redistribute  this software,
**             so  long as there is no profit made from its use, sale
**             trade or  reproduction.  You may not change this copy-
**             right notice, and it must be included in any copy made.
*/

#include "tin.h"
#include "tnntp.h"

/* find hostname */
char *
get_host_name (
void)
{
	char *ptr;
	static char hostname[MAXHOSTNAMELEN];

	hostname[0]='\0';
	
#ifdef HAVE_GETHOSTBYNAME
	gethostname(hostname, sizeof(hostname));
#else
#	if defined(M_AMIGA) || defined(M_OS2)	
	if ((ptr = getenv("NodeName")) != (char *) 0) {
		strncpy(hostname, ptr, MAXHOSTNAMELEN);
	}
#	else
#		if defined(WIN32)
	if ((ptr = getenv("COMPUTERNAME")) != (char *) 0) {
		strncpy(hostname, ptr, MAXHOSTNAMELEN);
	}
#		endif /* WIN32 */
#	endif /* M_AMIGA || M_OS2 */
#endif /* HAVE_GETHOSTBYNAME */
#ifdef HAVE_SYS_UTSNAME_H
	if (! *hostname){
		strcpy(hostname, system_info.nodename);
	}
#endif /* HAVE_SYS_UTSNAME_H */
	if (! *hostname) {
		if ((ptr = getenv("HOST")) != (char *) 0) {
			strncpy (hostname, ptr, MAXHOSTNAMELEN);
		} else {
			if ((ptr = getenv("HOSTNAME")) != (char *) 0) {
				strncpy (hostname, ptr, MAXHOSTNAMELEN);
			} else {
				hostname[0]='\0';
			}
		}
	}
	hostname[MAXHOSTNAMELEN]='\0';
	return (hostname);
}


#ifdef DOMAIN_NAME
/* find domainname - check DOMAIN_NAME */
char *
get_domain_name (
	void)
{
	char *ptr;
	static char domain[8192];
	char buff[MAXHOSTNAMELEN];
	FILE *fp;

	domain[0]='\0';

#	if defined(M_AMIGA)
/* Damn compiler bugs...
 * Without this hack, SASC 6.55 produces a TST.B d16(pc),
 * which is illegal on a 68000
 */
static const char *domain_name_hack = DOMAIN_NAME;
#	undef	DOMAIN_NAME
#	define DOMAIN_NAME domain_name_hack
#	endif /* M_AMIGA */

	if (strlen(DOMAIN_NAME)) {
		strcpy(domain, DOMAIN_NAME);
	}

	if (domain[0] == '/' && domain[1]) {
		/* If 1st letter is '/' read domianname from specified file */
		if ((fp = fopen (domain, "r")) != (FILE *) 0) {
			while (fgets (buff, sizeof (buff), fp) != (char *) 0) {
				if (buff[0] == '#' || buff[0] == '\n') {
					continue;
				}
				if((ptr = strrchr (buff, '\n'))) {
					*ptr = '\0';
				strcpy (domain, buff);
				}
			}
			if (domain[0] == '/') {
				/* file was empty */
				domain[0]='\0';
			} 
			fclose (fp);
		} else {
			domain[0]='\0';
		}
	}
	domain[MAXHOSTNAMELEN]='\0';
	return (domain);
}
#endif /* DOMAIN_NAME */

#ifdef HAVE_GETHOSTBYNAME
/* find FQDN - gethostbyaddr() */
const char *get_fqdn(host)
const char	*host;
{
	char	name[MAXHOSTNAMELEN+2];
	struct hostent	*hp;
	struct in_addr	in;

	name[MAXHOSTNAMELEN]='\0';
	if (host)
		(void)strncpy(name,host,MAXHOSTNAMELEN);
	else
		if (gethostname(name,MAXHOSTNAMELEN))
			return(NULL);

	if ('0'<=*name&&*name<='9') {
		in.s_addr=inet_addr(name);
		if ((hp=gethostbyaddr((char *)&in.s_addr,4,AF_INET)))
			in.s_addr=(*hp->h_addr);
		return(hp&&strchr(hp->h_name,'.')?hp->h_name:inet_ntoa(in));
	}
	if ((hp=gethostbyname(name))&&!strchr(hp->h_name,'.'))
		if ((hp=gethostbyaddr(hp->h_addr,hp->h_length,hp->h_addrtype)))
			in.s_addr=(*hp->h_addr);
	return(hp?strchr(hp->h_name,'.')?hp->h_name:inet_ntoa(in):NULL);
}
#endif

/*
 * FIXME: split get_user_info to get_full_name() and get_user_name()
 */


/*
 * Find username & fullname
 */
void
get_user_info (
	char *user_name,
	char *full_name)
{
#ifndef INDEX_DAEMON
	char buf[128];
	char tmp[128];
	const char *ptr;
	char *p;

#if defined(M_AMIGA)
	ptr = get_val ("REALNAME", "Unknown");
	my_strncpy (full_name, ptr, 128);
	strcpy (user_name, userid);
#else
#ifndef VMS
		my_strncpy (buf, myentry->pw_gecos, 128);
		p = strchr (buf, ',');
		if (p != 0) {
			*p = '\0';
		}
		/*
		 * check if SYSV (& lastname) hack is in gecos field
		 */
		p = strchr (buf, '&');
		if (p != 0) {
			*p++ = '\0';
			strcpy (tmp, userid);
			if (*tmp && *tmp >= 'a' && *tmp <= 'z') {
				*tmp = *tmp - 32;
			}
			sprintf (full_name, "%s%s%s", buf, tmp, p);
		} else {
			strcpy (full_name, buf);
		}
#else
		strcpy(full_name, fix_fullname(get_uaf_fullname()));
#endif

	if ((ptr = getenv ("NAME")) != 0) {
		my_strncpy (full_name, ptr, 128);
	}

	/*
	 * haha, supplying a from: line is not allowed, but this!
	 */

	if (userid == 0) {
		ptr = get_val ("USER", "please-set-USER-variable");
	} else {
		ptr = userid;
	}
	my_strncpy (user_name, ptr, 128);
#endif

#endif /* INDEX_DAEMON */
}


/*
 * FIXME to:
 * char * build_from(full_name, user_name, domain_name)
 */
/*
 * build From: in 'name <user@host.doma.in>' format
 */
void
get_from_name (
	char *user_name,
	char *full_name,
	char *from_name)
{
#ifndef INDEX_DAEMON

#ifdef FORGERY
	if (*mail_address) {
		strcpy(from_name, mail_address);
		return;
	}
#endif

	sprintf (from_name, "%s <%s@%s>", full_name, user_name, domain_name);

	if (debug == 2) {
		sprintf (msg, "FROM=[%s] USER=[%s] HOST=[%s] NAME=[%s]",
			from_name, user_name, domain_name, full_name);
		error_message (msg, "");
	}

#endif /* INDEX_DAEMON */
}
