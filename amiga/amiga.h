/*
 *  Project   : tin - a Usenet reader
 *  Module    : amiga.h
 *  Author    : M.Tomlinson & I.Lea
 *  Created   : 17-09-92
 *  Updated   : 03-02-94
 *  Notes     : Directory support for AmigaDOS
 *  Copyright : (c) Copyright 1991-94 by Mark Tomlinson & Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#if defined(M_AMIGA)

#ifndef AMIGA_H
#define AMIGA_H

#include <proto/dos.h>
#include <proto/exec.h>
#include <fcntl.h>
#ifndef AS225
#include <proto/usergroup.h>
#endif

/* SAS-C and Aztec don't take the mode for mkdir() */
extern int mkdir(char *path);
#define mkdir(p,m) mkdir(p)

/* For opendir(), and readdir() */

struct dirent {
        char *d_name;
        long d_reclen;
};

typedef struct
{
        BPTR    Lock;
        int     more;
        struct  ExAllData *buffer;
        struct  ExAllData *bufp;
        struct  ExAllControl *eac;
        struct  FileInfoBlock fib;
} DIR;

/* We create our own SIGWINCH handling. */
/* There are two currently unused signals we can define */
#define SIGWINCH        _SIG_MAX

#define localtime gmtime

extern DIR *opendir (char *name);
extern struct dirent *readdir (DIR *di);
extern void closedir (DIR *di);

extern FILE *popen (char *command, char *mode);
extern int pclose (FILE *pipe);
extern void *alloca (size_t size);

extern unsigned int sleep(unsigned int secs);
extern int tputs (char *cp, int count, void (*outc)(int));
extern int getpid(void);
extern int setenv(char *, char *, int);
extern char *mktemp(char *);
extern unsigned short umask(unsigned short);

extern int optind;
extern char *optarg;
extern int getopt (int argc, char *argv[], char *options);

extern void make_post_cmd (char *cmd, char *name);
extern void make_post_process_cmd(char *cmd, char *dir, char *file);

extern void AmiGetWinSize(int *lines, int *columns);

/*
 * tmp_close will delete a file on closure if it had been opened
 * with nntp_to_fp(). Otherwise it behaves just like fclose().
 */

#ifdef NNTP_ABLE
extern void log_unlink(FILE *fp, char *fname);
extern int tmp_close(FILE *fp);
#define fclose(fp) tmp_close(fp)
#endif

#endif  /* AMIGA_H */

#endif  /* M_AMIGA */
