/*
SYNOPSIS
     #include <stdlib.h>

     char *getpass(const char *prompt);

MT-LEVEL
     Unsafe

DESCRIPTION
     getpass() reads up  to  a  newline  or  EOF  from  the  file
     SYS$INPUT,  after prompting on the standard error output with
     the null-terminated string prompt and disabling echoing.   A
     pointer is returned to a null-terminated string of at most 8
     characters.  If SYS$INPUT cannot be opened, a null pointer is
     returned.

FILES
    SYS$INPUT

NOTES
     The return value points to  static  data  whose  content  is
     overwritten by each call.

AUTHOR
    Michael Stenns

COPYRIGHT
    (c) 1997    The content of this file is in the public domain.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <descrip.h>
#include <iodef.h>
#include <ssdef.h>

typedef struct
{
   short cond_value;
   short count;
   int info;
} io_statblk;

char *getpass(const char *prompt)
{
  static char password[64];
  char cMyChar = ' ', Backstr[] = {8,32,8,0};
  int pw_len = 0, status;
  io_statblk status_block;
  short int in_chan = 0;
  short int out_chan = 0;

  memset (password, '\0', sizeof password);
  sys$assign (desz("SYS$INPUT"), &in_chan,0,0);
  sys$assign (desz("SYS$ERROR"), &out_chan,0,0);
  /*  Queue the I/O.  */
  fprintf (stderr,"\n%s",prompt); fflush(stderr);
  while ( !iscntrl(cMyChar) && (pw_len < (sizeof password / sizeof (char) - 1)))
  {
    if ( status = (sys$qiow (0, in_chan, IO$_READVBLK|IO$M_NOFILTR|IO$M_NOECHO,
                       &status_block,0,0,
                       &cMyChar,1,0,0,0,0)) != SS$_NORMAL)
    {
      fprintf (stderr,"\n%s\n",vms_errmsg (status_block.cond_value));
      cMyChar = '\r';
    }
    else
    {
      if ((cMyChar == 127) || (cMyChar == 8))
      {
        cMyChar = ' ';
        password[pw_len] = '\0';
        if (pw_len)
        {
          pw_len--;
          sys$qiow (0, out_chan, IO$_WRITEVBLK,
                   &status_block,0,0,
                   Backstr,3,0,0,0,0);
        }
      }
      else
      {
        password[pw_len++] = cMyChar;
        sys$qiow (0, out_chan, IO$_WRITEVBLK,
                 &status_block,0,0,
                 "*",1,0,0,0,0);
      }
    }
  }
  while (pw_len < 8) password[pw_len++] = ' ';
  password[--pw_len] = '\0';
  sys$dassgn (in_chan);
  sys$dassgn (out_chan);
  fprintf (stderr,"\n");
  return password;
}
