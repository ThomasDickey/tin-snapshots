/*
 *  Project   : tin - a Usenet reader
 *  Module    : vms.c
 *  Author    : Andrew Greer
 *  Created   : 1995-06-19
 *  Updated   : 1998-01-24 by Michael Stenns
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
#include <vms/sys$routines.h>
#else
#include <starlet.h>
#endif
#include "tin.h"

/* local prototypes */
char * lower (char * str);
static int vms_open_channel (char * device);

/* global variables for current module */
static int stdin_chan = 0;  /* vms channel to sys$input */
static int stdin_ef   = 0;  /* event flag for read from stdin_chan */


/* return a pointer to string descriptor for zero terminated strings */
struct dsc$descriptor *desz(char *c$_str)
{
  static struct dsc$descriptor c$_tmpdesc;

  c$_tmpdesc.dsc$w_length = strlen(c$_str);
  c$_tmpdesc.dsc$b_dtype  = DSC$K_DTYPE_T;
  c$_tmpdesc.dsc$b_class  = DSC$K_CLASS_S;
  c$_tmpdesc.dsc$a_pointer= c$_str;
  return(&c$_tmpdesc);
}


/*
 * Author: unknown
 * Last edit: M. Stenns  (Oct 1997)
 * Comment:
 * returns the process owner (Owner field in uaf)
 */
char *get_uaf_fullname()
{
  static char uaf_owner[40];
  int i,status;

  struct item_list {
    short bl, ic;
    char *ba;
    short *rl;
  } getuai_itmlist[] = {
    {
      sizeof uaf_owner - 1,
      UAI$_OWNER,
      uaf_owner,
      0
    },
    { 0, 0, 0, 0}
  };

  memset (uaf_owner, '\0', sizeof uaf_owner);
  status = sys$getuai(0,0,desz(cuserid(NULL)),getuai_itmlist,0,0,0);
  if (!(status & 1)) strcpy (uaf_owner, cuserid(NULL));

  for (i=0;uaf_owner[i] && iscntrl(uaf_owner[i]);i++);
  return &uaf_owner[i];
}

/* Converts "TOD_MCQUILLIN" to "Tod McQuillin" */
char *fix_fullname(char *p)
{
  char *tmp;

  if (p && *p) {
    while ((tmp = strchr(p,'_'))) *tmp = ' ';
    tmp = lower(p);
    if (*tmp) *tmp = toupper (*tmp);
    while ((tmp = strchr(tmp,' '))) {
      tmp++;
      if (*tmp) *tmp = toupper (*tmp);
    }
    if ((tmp = strstr(p, "Mc"))) {
      tmp += 2;
      if (*tmp) *tmp = toupper (*tmp);
    }
    tmp = p + strlen(p) - 1;
    while ((tmp > p) && isspace(*tmp)) *tmp-- = '\0';
  }

  return p;
}

#ifndef INDEX_DAEMON
#ifndef tputs

int tputs (char * str, int zzz, OutcPtr func)
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
#endif


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
 * vms_open_channel (char * device)
 *
 * Opens a channel to the device specified as argument.
 * Return value:
 *  VMS channel number
 */

static int vms_open_channel (char * device)
{
  int channel = 0;
  int status;

  status = sys$assign (desz(device), &channel,0,0);
  if ((status & 1) != 1)
  {
     perror ("cannot assgin channel to sys$input");
     exit (EXIT_FAILURE);
  }
  status = lib$get_ef (&stdin_ef);
  if ((status & 1) != 1)
  {
     perror ("cannot get event flag");
     exit (EXIT_FAILURE);
  }
  return channel;
}

/*
 * int vms_close_stdin (void)
 *
 * frees the resources used by ReadCh() and ReadChNowait()
 */
int vms_close_stdin (void)
{
  int status;

  status = sys$cancel (stdin_chan);
  if ((status & 1) != 1)
  {
     perror ("cannot cancel I/O request");
     return status;
  }

  status = sys$dassgn (stdin_chan);
  if ((status & 1) != 1)
  {
     perror ("cannot close channel");
     return status;
  }
  stdin_chan = 0;

  status = lib$free_ef (&stdin_ef);
  if ((status & 1) != 1)
  {
     perror ("cannot free event flag");
     exit (EXIT_FAILURE);
  }
  stdin_ef = 0;

  return status;
}

/*
 * Read character from termianl without echo
 * Return zero if no input hanging
 */
int ReadChNowait (void)
{
  static int tt_char = 0;
  static short int iosb[4] = {1,0,0,0};
  int character     = 0;
  int status = SS$_NORMAL;

  if (!stdin_chan) stdin_chan = vms_open_channel ("sys$input");

  if (iosb[0])  /* no qio pending */
  {
    if (iosb[0] == SS$_TIMEOUT)
      character = 0;
    else if (iosb[0] != SS$_NORMAL)
      character = EOF;
    else
      character = tt_char;

    status = sys$qio (stdin_ef, stdin_chan,
                     IO$_READVBLK|IO$M_NOFILTR|IO$M_NOECHO|IO$M_TIMED,
                     iosb,0,0,
                     &tt_char,1,1,0,0,0);
    if (status != SS$_NORMAL)
    {
      fprintf (stderr,"sys$qiow failed in ReadCh ()\n");
      exit (status);
    }
  }

  return character;
}


/*
 * Read character from termianl without echo
 */
int ReadCh (void)
{
  int character     = 0;
  int status = SS$_NORMAL;
  short int iosb[4];

  while ((status == SS$_NORMAL) && !character)
  {
    character = ReadChNowait();
    if (!character) status = sys$waitfr (stdin_ef);
  }
  return character;
}


/* ======================================================================== */
/* lower - convert string to lowercase */
/* ======================================================================== */
char * lower (char * str)
{
char * tmp;

 /* convert to lower */
 if (str)
 {
    for (tmp = str; *tmp; tmp++) *tmp = tolower (*tmp);
 }
 return str;
}


/******************************************************************************
 * vms_errmsg converts errno to an readable string
 * return values:
 *   the converted string
 *****************************************************************************/
char * vms_errmsg (int error_number)
{
  char * message;

  message = strerror(errno);
  if (!message) message = "unknown error code";
  return message;
}

#endif /* VMS */
