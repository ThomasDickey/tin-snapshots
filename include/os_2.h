/*
 *  Project   : tin - a Usenet reader
 *  Module    : os_2.h
 *  Author    : A.Wrede & I.Lea
 *  Created   : 05-07-92
 *  Updated   : 22-08-92
 *  Notes     : Directory support for OS/2
 *  Copyright : (c) Copyright 1991-94 by Andreas Wrede & Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#if defined(M_OS2)

#ifndef OS_2_H
#define OS_2_H

#include <io.h>
#include <process.h>

#define	INCL_KBD

#ifdef __TURBOC__
#define	INCL_NOPMAPI
#define	INCL_DOSPROCESS

APIRET APIENTRY DosSleep(ULONG msec);
#endif

#define popen _popen
#define pclose _pclose

#define	NOBSD
#undef	BSD	/* stupid define in IBM libs ? */

#define	u_char	unsigned char
#define	u_short	unsigned short
#define	u_long	unsigned long

typedef int uid_t;
typedef int gid_t;

struct dirent
{
	char	d_name[256];
};

typedef struct
{
	unsigned long	_d_hdir;             
	char			*_d_dirname;          
	unsigned		_d_magic;            
	unsigned		_d_nfiles;           
	char			*_d_bufp;             
	char			_d_buf[512];         
} DIR;

struct passwd 
{
	char	*pw_name;
	char	*pw_passwd;
	uid_t	pw_uid;
	gid_t	pw_gid;
	char	*pw_age;
	char	*pw_comment;
	char	*pw_gecos;
	char	*pw_dir;
	char	*pw_shell;
};

#if !defined(_POSIX_SOURCE) 

struct comment 
{
	char	*c_dept;
	char	*c_name;
	char	*c_acct;
	char	*c_bin;
};

#endif	/* _POSIX_SOURCE */ 

extern int closedir (DIR *__dir);
extern char getopt (int argc,char *argv[],char *options);
extern void endpwent (void);
extern struct passwd *fgetpwent (FILE *);
extern char *getlogin (void);
extern struct passwd *getpwent (void);
extern struct passwd *getpwuid (uid_t);
extern struct passwd *getpwnam (const char *);
extern int getuid (void);
extern void backslash(char *ptr);
extern int make_post_cmd (char *cmd,char *name);
extern int make_post_process_cmd (char *cmd,char *dir,char *file);
extern DIR *opendir (const char *__dirname);
extern int putpwent (const struct passwd *, FILE *);
extern struct dirent *readdir (DIR *__dir);
extern void rewinddir (DIR *__dir);
extern void setpwent (void);

#endif	/* OS_2_H */

#endif	/* M_OS2 */
