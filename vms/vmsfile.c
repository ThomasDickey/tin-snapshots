#include <string.h>
#include <stdio.h>
#include "parse.h"

void joinpath(char *result, const char *dir, const char *file)
{
  struct filespec *spec;
  char tmpdir[255], tmpfile[255];
  char *ptr1, *ptr2;

  strcpy(tmpdir, dir);
  strcpy(tmpfile, file);

  if (strlen (tmpdir) == 0) {
    strcpy (result, tmpfile);
    return;
  }
  if (tmpfile[0] == '[')
   {
    strcat(tmpdir,tmpfile);
    tmpfile[0] = '\0';
   }
  spec = sysparse(tmpdir);
  sprintf(result, "%s%s%s", spec->dev, spec->dir, tmpfile);
  if (spec->filename[0]) {
    fprintf(stderr, "joinpath: throwing away filename %s\n", spec->filename);
  }
}

void joindir(char *result, const char *dir, const char *file)
{
  struct filespec *spec;
  int i;
  char tmpdir[255], tmpfile[255];

  strcpy(tmpdir, dir);
  strcpy(tmpfile, file);

  if (strlen (tmpdir) == 0) {
    sprintf(result, "[%s]", tmpfile);
    return;
  }
  spec = sysparse(tmpdir);
  i = strlen(spec->dir);
  if (spec->dir[i-1] == ']') {
    spec->dir[i-1] = 0;
    sprintf(result, "%s%s.%s]", spec->dev, spec->dir, tmpfile);
    if (spec->filename[0]) {
      fprintf(stderr, "joinpath: throwing away filename %s\n", spec->filename);
    }
  } else {
    fprintf(stderr, "joinpath: dir %s didn't end with ']' (passed %s)\n",
	    spec->dir, tmpfile);
    exit(0);
  }
}

