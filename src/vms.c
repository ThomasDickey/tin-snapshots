/*
 *  Project   : tin - a Usenet reader
 *  Module    : vms.c
 *  Author    : Andrew Greer
 *  Created   : 19-06-95
 *  Updated   : 19-06-95
 *  Notes     :
 *  Copyright : (c) Copyright 1991-95 by Iain Lea & Andrew Greer
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifdef VMS

#include <stdio.h>
#include <descrip.h>
#include <iodef.h>
#include <ssdef.h>
#include <uaidef.h>
#include <string.h>
#include <stdlib.h>
#include <sys/file.h>
#ifdef __GNUC__
#include <sys$routines.h>
#else
#include <starlet.h>
#endif
#include "tin.h"
#include "sio.h"

char *
getlogin (void)
{
	char *p;

	if ((p = getenv ("USER")) == NULL) {
		return ( (char *) 0);
	}

	return (p);
}

struct dsc$descriptor *c$dsc(char *c$_str)
{
  static struct dsc$descriptor c$_tmpdesc;

  c$_tmpdesc.dsc$w_length = strlen(c$_str);
  c$_tmpdesc.dsc$b_dtype  = DSC$K_DTYPE_T;
  c$_tmpdesc.dsc$b_class  = DSC$K_CLASS_S;
  c$_tmpdesc.dsc$a_pointer= c$_str;
  return(&c$_tmpdesc);
}

char *get_uaf_fullname()
{
  static char uaf_owner[40];
  char loc_username[13];
  int i, pos;

  struct item_list {
    short bl, ic;
    char *ba;
    short *rl;
  } getuai_itmlist[] = {
    {
      sizeof(uaf_owner),
      UAI$_OWNER,
      &uaf_owner[0],
      0
    },
    { 0, 0, 0, 0}
  };

  strcpy(loc_username, getenv("USER"));
  for (i = strlen(loc_username); i < 12; ++i)
    loc_username[i] = ' ';
  loc_username[i] = '\0';

  sys$getuai(0,0,c$dsc(loc_username),getuai_itmlist,0,0,0);

  pos=1;
  if (uaf_owner[pos]=='|')
    pos += 3;
  while (uaf_owner[pos] == ' ')
    pos++;
  uaf_owner[uaf_owner[0] + 1] = '\0';
  return(uaf_owner + pos);
}

/* Converts "TOD_MCQUILLIN" to "Tod McQuillin" */
char *fix_fullname(char *p)
{
  int cc = 0;
  char *q = p;

  while (*q) {
    if (cc > 0) {
      if (cc > 1 && *(q-1) == 'c' && *(q-2) == 'M') {
	if (islower(*q))
	  *q = toupper(*q);
      } else
	if (isupper(*q))
	  *q = tolower(*q);
    } else
      if (cc == 0)
	if (islower(*q))
	  *q = toupper(*q);
    if (*q == '_' || *q == ' ') {
      *q = ' ';
      cc = 0;
    } else
      cc++;
    q++;
  }
  return (p);
}

#ifndef INDEX_DAEMON

int tputs (str, zzz, func)
	register char *str;
	int zzz;
	int (*func)(int);
{
	if (! str) {
		return(0);
	}
	if (func == outchar) {
		fputs (str, stdout);
	} else {
		while (*str) {
			if (*str == '\n') {
				func('\r');
			}
			func(*str++);
		}
	}
	return(0);
}

#endif

FILE *
popen (
	char *command,
	char *mode)
{
	return ((FILE *) 0);
}


void
pclose (FILE *pipe)
{
	return;
}

void tzset(void)
{
	return;
}

void
make_post_cmd (cmd, name)
	char *cmd;
	char *name;
{
	char *p;

	if ((p = getenv (ENV_VAR_POSTER)) != (char *) 0) {
		sprintf (cmd, p, name);
	} else {
		sprintf (cmd, DEFAULT_POSTER, name);
	}
	return;
}


/*
 * Replacement for getchar() for Prof. Davis' C programming class
 *
 * V1.0-00 - 10-Jan-93 - tmk - Original version
 */

int ReadCh(void)
{
  $DESCRIPTOR(tt_name,"SYS$INPUT");
  static int __sgetchar_first_time=1;
  static short __sgetchar_channel, iosb[4];
  static char __sgetchar_char[8];
  static int __sgetchar_status;
#ifdef DEBUG_READCH
  static int dribble_fd;
#endif

  if (__sgetchar_first_time) {
    __sgetchar_status = sys$assign(&tt_name, &__sgetchar_channel, 0, 0);
    if (__sgetchar_status != SS$_NORMAL) {
      printf("sgetchar() error %08x from SYS$ASSIGN\n", __sgetchar_status);
      exit(__sgetchar_status);
    }
#ifdef DEBUG_READCH
    if ((dribble_fd = open("dribble.dat", O_WRONLY|O_CREAT, 0777)) == -1) {
      perror("dribble.dat");
      exit(0);
    }
#endif
    __sgetchar_first_time = 0;
  }

 requeue:
  __sgetchar_status = sys$qiow(0, __sgetchar_channel,
			       IO$_READVBLK|IO$M_NOFILTR|IO$M_NOECHO,
			       iosb, 0, 0, __sgetchar_char, 1, 0, 0, 0, 0);

  if (__sgetchar_status != SS$_NORMAL) {
    printf("sgetchar() error %08x from SYS$QIOW\n", __sgetchar_status);
    exit(__sgetchar_status);
  }

  if ((iosb[0] & 7) != 1) {
    printf("iosb[0] == %04X\n", iosb[0]);
    goto requeue;
  }

#ifdef DEBUG_READCH
  write(dribble_fd, __sgetchar_char, 1);
#endif
  if (__sgetchar_char[0] == 26)
    return(EOF);
  else
    return(__sgetchar_char[0] & 0xFF);
}

/* fwrite to stdout is S L O W.  Speed it up by using fputc...*/
#ifdef VAXC
size_t sys_fwrite (void *ptr, size_t size_of_item,
		   size_t number_items, FILE *file_ptr)
{
  register int tot = number_items * size_of_item;

  while (tot--)
    fputc (*((char *)ptr)++, file_ptr);
}
#else /* not VAXC */
#ifdef USE_EMACS_SYSFWRITE
void sys_fwrite (ptr, size, num, fp)
     register char * ptr;
     int size;
     int num;
     FILE * fp;
{
  register int tot = num * size;

  while (tot--)
    fputc (*ptr++, fp);
}
#else
void sys_fwrite (ptr, size, num, fp)
     register char * ptr;
     int size;
     int num;
     FILE * fp;
{
  register int tot = num * size;

  fflush(fp);
  write(fileno(fp), ptr, tot);
}
#endif
#endif /* not VAXC */

#ifdef USE_SFGETS
char *Sfgets(char *s, int n, int fd)
{
  register c;
  register char *cs;

  cs = s;
  while (--n>0 && (c = Sgetchar(fd)) != SIO_EOF) {
                *cs++ = c;
                if (c=='\n')
                        break;
        }
        if (c == SIO_EOF && cs == s)
                return(NULL);
        *cs++ = '\0';
        return(s);
}
#endif

#endif /* VMS */
