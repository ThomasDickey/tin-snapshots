#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <rmsdef.h>
#include <fab.h>
#include <nam.h>
#ifdef __GNUC__
#include <vms/rms$routines.h>
#endif
#include "parse.h"

struct filespec *sysparse(char *filename)
{
  struct FAB fab = cc$rms_fab;
  struct NAM nam = cc$rms_nam;
  static struct filespec spec;
  char *ptr1, *ptr2;
  char *ptr;
  char fname[255];

  int len, idx;

  ptr = (char *)&fname;
  strcpy(ptr, filename);

  if (strcmp(ptr, "~") == 0)
    strcpy(ptr, getenv("HOME"));
  if (ptr1 = (char *)strstr(ptr, "[000000]["))
   {
    ptr2 = ptr1+8;
    memmove(ptr1, ptr2, strlen(ptr2)+1);
   }
  while (ptr1 = (char *)strstr(ptr, "]["))
   {  /* Remove any "]["'s in the string */
    ptr2 = ptr1+2;
    memmove(ptr1, ptr2, strlen(ptr2)+1);
   } /* while */

  for (idx=0;idx<strlen(ptr);idx++)
   {
    switch(ptr[idx])
     {
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
      case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
      case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
      case 'V': case 'W': case 'X': case 'Y': case 'Z':
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
      case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
      case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
      case 'v': case 'w': case 'x': case 'y': case 'z':
      case '0': case '1': case '2': case '3': case '4': case '5': case '6':
      case '7': case '8': case '9':
      case '$': case '-': case '_': case '[': case ']': case '<': case '>':
      case ';': case '.': case ':':
            break;
      default : ptr[idx] = '$';     /* for the moment */
     }
   }

  fab.fab$l_fna = ptr;
  fab.fab$b_fns = strlen(ptr);
  fab.fab$l_nam = &nam;
  fab.fab$l_fop = FAB$M_NAM;
  nam.nam$l_esa = spec.full;
  nam.nam$b_ess = sizeof spec.full;
  nam.nam$b_nop |= NAM$M_SYNCHK;

  if (sys$parse(&fab, 0, 0) == RMS$_NORMAL) {
    len = nam.nam$b_esl - 1;
    if (spec.full[len] == ';' && spec.full[len - 1] == '.')
      len -= 2;
    spec.full[len + 1] = '\0';
    if (nam.nam$b_dev > 0)
      sprintf(spec.dev, "%*.*s", nam.nam$b_dev, nam.nam$b_dev, nam.nam$l_dev);
    else
      spec.dev[0] = 0;
    if (nam.nam$b_dir > 0)
      sprintf(spec.dir, "%*.*s", nam.nam$b_dir, nam.nam$b_dir, nam.nam$l_dir);
    else
      spec.dir[0] = 0;
    if (nam.nam$b_name > 0)
      sprintf(spec.filename, "%*.*s%*.*s", nam.nam$b_name, nam.nam$b_name,
	      nam.nam$l_name, nam.nam$b_type, nam.nam$b_type, nam.nam$l_type);
    else
      spec.filename[0] = 0;
    return &spec;
  } else {
    fprintf(stderr, "SYS$PARSE failed on %s\n", ptr);
    exit(0);
  }
}
