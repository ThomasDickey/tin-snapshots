/*
 *  Project   : tin - a Usenet reader
 *  Module    : win32.h
 *  Author    : S.Lam, N.Ellis & P.Haken
 *  Created   : 01-06-87
 *  Updated   : 02-24-95
 *  Notes     : ndir.h for MS-DOS by Samuel Lam <skl@van-bc.UUCP>, June/87
 *              additional WIN32 support by N.Ellis and P.Haken.
 *
 *  Copyright : (c) Copyright 1987-93 by Samuel Lam
 *          (c) Copyright 1994-95 by N.Ellis & P.Haken
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#if defined(WIN32)

#ifndef WIN32_H
#define WIN32_H

#include <windows.h>
#include <direct.h>
#include <io.h>
#include <assert.h>
#include <malloc.h>

#define MSDOS_MAXNAMLEN	12
#define MAXNAMLEN		MSDOS_MAXNAMLEN

extern char TMPDIR[MAX_PATH];

#define popen _popen
#define pclose _pclose
#define sleep(s) Sleep(s*1000)  // WIN32 sleep is in Millseconds.
#define pipe(a) _pipe(a,1024,0)
#define rename(s,d) win32rename(s,d)

typedef int uid_t;
typedef int gid_t;

struct direct {
	long d_ino;
	short d_reclen;
	short d_namlen;
	char d_name[MAXNAMLEN + 1];
};

typedef struct {
	char filereserved[21];
	char fileattr;
	int filetime, filedate;
	long filesize;
	char filename[MSDOS_MAXNAMLEN + 1];
} DTA;

typedef struct {
	char dirid[4];
	struct direct dirent;
	DTA dirdta;
	int dirfirst;
} DIR;

struct passwd {
	char *pw_name;
	char *pw_passwd;
	uid_t pw_uid;
	gid_t pw_gid;
	char *pw_age;
	char *pw_comment;
	char *pw_gecos;
	char *pw_dir;
	char *pw_shell;
};

extern char *getlogin(void);
extern char getopt(int argc, char *argv[], char *options);

extern DIR *opendirx(const char *dirname, char *pattern);

// extern DIR *opendir (const char *__dirname);
#define opendir(x) opendirx(x, "*.*")
extern void closedir(DIR * __dir);
extern struct direct *readdir(DIR * __dir);
extern void rewinddir(DIR * __dir);

extern int getuid(void);
extern int geteuid(void);
extern int getegid(void);
extern int getgid(void);
extern long getpid(void);
extern int putpwent(const struct passwd *, FILE *);
extern void endpwent(void);
extern void setpwent(void);
extern struct passwd *fgetpwent(FILE *);
extern struct passwd *getpwent(void);
extern struct passwd *getpwnam(const char *);
extern struct passwd *getpwuid(uid_t);

extern void backslash(char *ptr);
extern int make_post_cmd(char *cmd, char *name);

#define alarm(x) (1)

extern void setup_screen(void);

extern void set_xclick_on(void);
extern void set_xclick_off(void);
extern int win32rename(char *, char *);

/*
 * tmp_close will delete a file on closure if it had been opened
 * with nntp_to_fp(). Otherwise it behaves just like fclose().
 */

extern void log_unlink(FILE * fp, char *fname);
extern int tmp_close(FILE * fp);

#define fclose(fp) tmp_close(fp)

#if defined(DEBUG) && defined(CHECKHEAP)
void DebugAssertDialog(LPSTR szMessage, DWORD dwLine, LPSTR szFile);
_inline void
CheckHeap(DWORD dwLine, LPSTR szFile)
{
	if (_heapchk() != _HEAPOK)
		DebugAssertDialog("Invalid Heap", dwLine, szFile);
};

_inline void *__cdecl
mymalloc(size_t cb, DWORD dwLine, LPSTR szFile)
{
	CheckHeap(dwLine, szFile);
	return malloc(cb);
};

_inline void *__cdecl
myrealloc(void *pv, size_t cb, DWORD dwLine, LPSTR szFile)
{
	CheckHeap(dwLine, szFile);
	return realloc(pv, cb);
};

_inline void *__cdecl
mycalloc(size_t cbnum, size_t cbsize, DWORD dwLine, LPSTR szFile)
{
	CheckHeap(dwLine, szFile);
	return calloc(cbnum, cbsize);
};

_inline void __cdecl
myfree(void *pv, DWORD dwLine, LPSTR szFile)
{
	CheckHeap(dwLine, szFile);
	free(pv);
	CheckHeap(dwLine, szFile);
};

#endif

#endif // WIN32_H
#endif // WIN32
