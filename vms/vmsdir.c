#include <errno.h>
#include <unixio.h>
#include <stdlib.h>
#include <stdio.h>
#include "strings.h"
#include <sys/stat.h>
#include "ndir.h"

int sys_read (int fildes, char *buf, unsigned int nbyte);

int
/* VARARGS 2 */
sys_open (path, oflag, mode)
     char *path;
     int oflag, mode;
{
  register int rtnval;

  while ((rtnval = open (path, oflag, mode)) == -1
	 && (errno == EINTR));
  return (rtnval);
}

int sys_close (int fd)
{
  register int rtnval;

  while ((rtnval = close (fd)) == -1
	 && (errno == EINTR));
  return rtnval;
}

DIR *
opendir (filename)
     char *filename;	/* name of directory */
{
  register DIR *dirp;		/* -> malloc'ed storage */
  register int fd;		/* file descriptor for read */
  struct stat sbuf;		/* result of fstat() */

  fd = sys_open (filename, 0);
  if (fd < 0)
    return 0;

  if (fstat (fd, &sbuf) < 0
      || (sbuf.st_mode & S_IFMT) != S_IFDIR
      || (dirp = (DIR *) malloc (sizeof (DIR))) == 0)
    {
      sys_close (fd);
      return 0;		/* bad luck today */
    }

  dirp->dd_fd = fd;
  dirp->dd_loc = dirp->dd_size = 0;	/* refill needed */

  return dirp;
}

void
closedir (dirp)
     register DIR *dirp;		/* stream from opendir() */
{
  sys_close (dirp->dd_fd);
  free ((char *) dirp);
}

struct direct dir_static;	/* simulated directory contents */

/* ARGUSED */
struct direct *
readdir (dirp)
     register DIR *dirp;	/* stream from opendir() */
{
#ifndef VMS
  register struct olddir *dp;	/* -> directory data */
#else /* VMS */
  register struct dir$_name *dp; /* -> directory data */
  register struct dir$_version *dv; /* -> version data */
#endif /* VMS */

  for (; ;)
    {
      if (dirp->dd_loc >= dirp->dd_size)
	dirp->dd_loc = dirp->dd_size = 0;

      if (dirp->dd_size == 0 	/* refill buffer */
	  && (dirp->dd_size = sys_read (dirp->dd_fd, dirp->dd_buf, DIRBLKSIZ)) <= 0)
	return 0;

#ifndef VMS
      dp = (struct olddir *) &dirp->dd_buf[dirp->dd_loc];
      dirp->dd_loc += sizeof (struct olddir);

      if (dp->od_ino != 0)	/* not deleted entry */
	{
	  dir_static.d_ino = dp->od_ino;
	  strncpy (dir_static.d_name, dp->od_name, DIRSIZ);
	  dir_static.d_name[DIRSIZ] = '\0';
	  dir_static.d_namlen = strlen (dir_static.d_name);
	  dir_static.d_reclen = sizeof (struct direct)
	    - MAXNAMLEN + 3
	      + dir_static.d_namlen - dir_static.d_namlen % 4;
	  return &dir_static;	/* -> simulated structure */
	}
#else /* VMS */
      dp = (struct dir$_name *) dirp->dd_buf;
      if (dirp->dd_loc == 0)
	dirp->dd_loc = (dp->dir$b_namecount&1) ? dp->dir$b_namecount + 1
	  : dp->dir$b_namecount;
      dv = (struct dir$_version *)&dp->dir$t_name[dirp->dd_loc];
      dir_static.d_ino = dv->dir$w_fid_num;
      dir_static.d_namlen = dp->dir$b_namecount;
      dir_static.d_reclen = sizeof (struct direct)
	- MAXNAMLEN + 3
	  + dir_static.d_namlen - dir_static.d_namlen % 4;
      strncpy (dir_static.d_name, dp->dir$t_name, dp->dir$b_namecount);
      dir_static.d_name[dir_static.d_namlen] = '\0';
      dirp->dd_loc = dirp->dd_size; /* only one record at a time */
      return &dir_static;
#endif /* VMS */
    }
}

int sys_read (int fildes, char *buf, unsigned int nbyte)
{
  register int rtnval;

  while ((rtnval = read (fildes, buf, nbyte)) == -1
	 && (errno == EINTR));
  return (rtnval);
}

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

/* Get the name of current user */
char *
getlogin (void)
{
	char *p;

	if ((p = getenv ("USER")) == NULL) {
		return ( (char *) 0);
	}

	return (p);
}

/*
 *  setenv (char *name, char *value, int notused)
 */
int setenv (char *name, char *value, int notused)
{
  int status = 0;
  char command[1024];

  if (name && value)
  {
    if (sprintf(command,"define/nolog/job %s \"%s\" ",name,value) > sizeof command)
    {
	fprintf (stderr, "FATAL buffer overflow in setenv");
	tin_done (EXIT_FAILURE);
    }
    status = system(command);
  }
  return status;
}

