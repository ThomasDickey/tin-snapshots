#include "tin.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifndef MAXHOSTNAMELEN
#	define MAXHOSTNAMELEN 255
#endif

/*
** find hostname
*/
void
get_host_name (
	char *hostname)
{
	char *ptr;

#ifdef HAVE_GETHOSTBYNAME
	char shortname[MAXHOSTNAMELEN+2]="";

	if	(gethostname(shortname, sizeof(shortname))){
		strcpy(host_name, shortname);
	}
#else
#	if defined(M_AMIGA) || defined(M_OS2)

	if ((ptr = getenv("NodeName")) != (char *) 0) {
		strcpy(host_name, ptr);
	}
#	else
#		if defined(WIN32)
	if ((ptr = getenv("COMPUTERNAME")) != (char *) 0) {
		strcpy(host_name, ptr);
	}
#		endif /* WIN32 */
#	endif /* M_AMIGA || M_OS2 */
#endif /* HAVE_GETHOSTBYNAME */

	else {
		struct utsname uts_name;
			
		if (!uname (&uts_name)) {
			strcpy (host_name, uts_name.nodename);
		} else {
			if ((ptr = getenv("HOST")) != (char *) 0) {
				strcpy(host_name, ptr);
			} else {
				if ((ptr = getenv("HOSTNAME")) != (char *) 0) {
					strcpy(host_name, ptr);
				}
			}
		}
	}
}

/*
** find domainname - check DOMAIN_NAME, /etc/resolv.conf
*/
void
get_domain_name (
	char *domainname)
{
	char *ptr;
	char domain[MAXHOSTNAMELEN+2]="";
	char buff[MAXHOSTNAMELEN+2]="";
	FILE *fp;

#ifdef DOMAIN_NAME
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
				strcpy (domain_name, buff);
				}
			}
			fclose (fp);
		}
		domain[0] = '\0';
	}
	if (*domain_name=='\0') { /* domain_name is still in domain */
		strcpy(domain_name, domain);
	}
#endif /* DOMAIN_NAME */

	/*
	** DOMAIN_NAME did not carrie any usefull information or an error
	** occured while reading -> check domain-line in /etc/resolv.conf
	*/

	if (*domain_name=='\0') {
		/* read /etc/resolv.conf and parse domain-line */
		if ((fp = fopen ("/etc/resolv.conf", "r")) != (FILE *) 0) {
			while (fgets (buff, sizeof (buff), fp) != (char *) 0) {
			switch(buff[0]) {
				case 'd':
					if(! strncmp(buff,"domain",6)) {
						if ((ptr = strrchr(buff,' '))) {
							strcpy (domain, ptr+1);
						} else {
							strcpy (domain, buff+6);
						}
						if ((ptr = strrchr(domain, '\t'))) {
							strcpy (domain_name, ptr+1);
						} else {
							strcpy (domain_name, domain);
						}
						if ((ptr = strrchr (domain_name, '\n'))) {
							*ptr = '\0';
						}
					}
					break;
				default:
					break;
				}
			}
			fclose (fp);
		}
		/*
		** if we got domain-name from /etc/resolv.conf
		** switch domain-name && FQDN to lowercase
		** and check is domain-name is a substring of FQDN
		*/
		strcpy (domain, get_fqdn(host_name));
		ptr = domain;
		while (*ptr != '\0') {
			*ptr = tolower(*ptr++);
		}
		ptr = domain_name;
		while (*ptr != '\0') {
			*ptr = tolower(*ptr++);
		}
		if (! strstr (get_fqdn (host_name), domain_name)) {
			*domain_name = '\0';
		}
	}
}

#ifdef HAVE_GETHOSTBYNAME

/*
** find FQDN - gethostbyaddr()
*/
char *
get_fqdn (
	char *host)
{
	char	name[MAXHOSTNAMELEN+2];
	struct hostent	*hp;
	struct in_addr	in;

	name[MAXHOSTNAMELEN]='\0';
	if (host) {
		(void) strncpy (name, host, MAXHOSTNAMELEN);
	} else {
		if (gethostname (name, MAXHOSTNAMELEN)) {
			/* oh oh */
			return(NULL);
		}
	}

	if (isdigit(*name)) {
		in.s_addr = inet_addr(name);
		if ((hp = gethostbyaddr((char *) &in.s_addr, 4, AF_INET)))
			in.s_addr = (*hp->h_addr);
		return(hp && strchr(hp->h_name, '.') ? hp->h_name : inet_ntoa(in));
	}
	if ((hp = gethostbyname(name)) && !strchr (hp->h_name, '.'))
		if ((hp = gethostbyaddr(hp->h_addr, hp->h_length, hp->h_addrtype)))
			in.s_addr = (*hp->h_addr);
	return(hp ? strchr(hp->h_name, '.') ? hp->h_name : inet_ntoa(in) : NULL);
}
#endif
