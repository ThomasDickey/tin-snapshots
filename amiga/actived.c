/*
 *  Project   : tin - a Usenet reader
 *  Module    : actived.c
 *  Author    : M.Tomlinson & I.Lea
 *  Created   : 23-08-92
 *  Updated   : 02-12-94
 *  Notes     : Creates an active file  by looking through all the
 *              .next files in  the news directories, and  writing
 *              this to UULIB:newactive. The UULIB:newsgroups file
 *              must exist.
 *  Copyright : (c) Copyright 1991-94 by Mark Tomlinson & Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define PATH_LEN 256
#define NG_LEN 256

#if defined(M_AMIGA)
#      define  NEWSGROUPS_FILE "UULIB:newsgroups"
#      define  NEWSACTIVE_FILE "UULIB:newsactive"
#      define  ACTIVE_FILE     "UULIB:active"
#      define  ACTIVE_TIMES    "UULIB:active.times"
#      define  SPOOLDIR        "UUNEWS:"
#      define  ENV_VAR_GROUPS  "TIN_GROUPS"
#endif

long maxgroups;
char **grouplist;

static long readgroups(void);

int main (void)
{
       FILE *active, *oldactive;
       long numgroups, i;
       long min = 1, max;
       char oldgroup[NG_LEN] = "";

       /* Just who does live in CST timezone anyway? */

       _TZ = "GMT0";
       __tzset();

       numgroups = readgroups();

       if ((active = fopen (NEWSACTIVE_FILE, "w")) == (FILE *) 0) {
               perror (NEWSACTIVE_FILE);
               exit (1);
       }

       oldactive = fopen(ACTIVE_FILE, "r");

       for (i = 0; i < numgroups; i++) {
               int new;
               if (oldactive) while ((new = strcmp(oldgroup, grouplist[i])) < 0) {
                       if (fgets (oldgroup, sizeof(oldgroup), oldactive)) {
                               char *p;
                               for (p = oldgroup; *p && *p != ' ' && *p != '\t' && *p != '\n'; p++) {
                                       ;
                               }
                               *p = '\0';
                       } else {
                               fclose(oldactive);
                               oldactive = (FILE *) 0;
                               break;
                       }
               } else new = 0;

               if (new) {
                       FILE *times;
                       times = fopen(ACTIVE_TIMES, "a");
                       if (times) {
                               time_t t;
                               time(&t);
                               fprintf(times, "%s %lu actived\n", grouplist[i], t);
                               fclose(times);
                       }
               }

               {       FILE *fp;
                       {       char next_path[PATH_LEN];
                               char *p;

                               sprintf (next_path, "%s%s", SPOOLDIR, grouplist[i]);

                               for (p = next_path; *p ; p++) {
                                       if (*p == '.') {
                                               *p = '/';       /* convert to tree structure */
                                       }
                               }
                               strcpy (p, "/.next");
                               fp = fopen (next_path, "r");
                       }
                       if (fp) {
                               char last[20];
                               fgets (last, sizeof(last), fp);
                               max = atol (last) - 1;
                               fclose (fp);
                       } else {
                               max = 0;
                       }
               }
               fprintf (active, "%s %05ld %05ld y\n", grouplist[i], max, min);
       }
       fclose (oldactive);
       fclose (active);

       {       char buf[40];
               sprintf (buf, "%s=%d", ENV_VAR_GROUPS, numgroups+1);
               putenv (buf);
       }
       return 0;
}

static long
readgroups(void)
{
       FILE *fp;
       long numgroups = 0;
       char group[NG_LEN];
       char *p;

       maxgroups = 1000;
       grouplist = malloc(maxgroups*sizeof(char *));

       if ((fp = fopen(NEWSGROUPS_FILE, "r")) == (FILE *) 0) return -1;

       while (fgets(group, sizeof(group), fp)) {
               for (p = group; *p && *p != ' ' && *p != '\t' && *p != '\n'; p++) {
                       ;
               }
               *p = '\0';

               p = strdup(group);

               if (numgroups >= maxgroups) {
                       maxgroups += maxgroups/2;
                       grouplist = realloc(grouplist,maxgroups*sizeof(char *));
                       if (grouplist == (char **) 0) return -1;
               }
               grouplist[numgroups++] = p;
       }
       tqsort(grouplist, numgroups);
       return numgroups;
}
