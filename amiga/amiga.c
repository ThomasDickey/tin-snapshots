/*
 *  Project   : tin - a Usenet reader
 *  Module    : amiga.c
 *  Author    : M.Tomlinson, R.Luebke & I.Lea
 *  Created   : 01-04-91
 *  Updated   : 17-07-95
 *  Notes     : Extra functions for Amiga port
 *  Copyright : (c) Copyright 1991-94 by Mark Tomlinson & Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include        "tin.h"
#include        "version.h"

#if defined(M_AMIGA)

#include        <exec/libraries.h>
#include        <exec/memory.h>
#include        <libraries/dos.h>
#include        <dos/exall.h>
#ifdef __SASC
#       include <proto/dos.h>
#       include <proto/exec.h>
#else
#       include <clib/dos_protos.h>
#       include <clib/exec_protos.h>
#       include <pragmas/dos_lib.h>
#       include <pragmas/exec_lib.h>
#endif

#include        <ctype.h>
#include        <fcntl.h>
#include        <ios1.h>
#include        <error.h>

#define BUFSIZE 1000

static LONG dopkt(struct MsgPort *pid, LONG action, LONG args[], LONG nargs);

extern struct DosLibrary *DOSBase;
extern int errno;
#ifdef __SASC
extern unsigned long __fmask;
#endif /* __SASC */

/*
** something for the AmigaDOS Version command
** AMIVER is defined in ../include/version.h
** __AMIGADATE__ is defined by the SC 6.55 preprocessor
*/
static const char verstag[] = "$VER: tin " AMIVER " " __AMIGADATE__ "\0";

int optind = 1;
char *optarg;

#if 0
#ifdef __SASC
long __stack = 40000;   /* TIN requires lots of stack */
#endif
#endif /* 0 */

static APTR old_windowptr;

/* This routine gets called before main() */

void __interrupt _STI_no_req(void)
{       struct Process *pr;

        pr = (struct Process *)FindTask(0L);
        old_windowptr = pr->pr_WindowPtr;
        pr->pr_WindowPtr = (APTR)-1;
}

/* And this one after exit() */

void __interrupt _STD_restore_req(void)
{       struct Process *pr;

        pr = (struct Process *)FindTask(0L);
        pr->pr_WindowPtr = old_windowptr;               /* Enable requesters. */
}

/*
 * The next is to avoid the generation of SIGINT whenever ctrl-d is pressed.
 * Remember tin uses ctrl-d for page down...
 *
 * Our version of chkabort() is listening only for ctrl-c.
 */

#ifdef __SASC
void __interrupt __chkabort (void)
{
        if (SetSignal(0, SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D) & SIGBREAKF_CTRL_C)
        {
                raise(SIGINT);
        }
}
#endif

int
chmod (const char *file, int mode)
{
    int amimode;
                /* un*x's rwx bits are mapped to AmigaOS's rwe bits *
                 * possible misfeature: the AmigaOS d-bit is not set */
    amimode = ~mode >> 5 & 0xe;   /* owner, uh, sick! */
    amimode |= (mode & 070)<<6;     /* group */
    amimode |= (mode & 07)<<13;     /* other */
    if (SetProtection(file,amimode) == TRUE)
        return 0;
    else
        {
            errno = EINVAL; /* best possible, if I don't include a complete */
            return -1;      /* OSERR -> errno translation */
        }
}

unsigned short umask (unsigned short mask)
{
#ifdef __SASC
        __fmask = (~mask & 0700) >> 5 | 01; /* directly mapped, no use of fcntl.h */
#endif /* __SASC */
        return mask;
}

/* YAUAF (Yet Another Unnecessary ANSI Function) */

/* time_t time(time_t *pt) deleted */

/*
 * use the task address for pid which is unique.
 */

int getpid (void)
{
        return ((long) FindTask(0L) >> 2);
}

void *alloca(size_t dummy)
{
        return NULL; /* fails, fails, fails! */
}
    
/*
 *  dopkt() by A. Finkel, P. Lindsay, C. Scheppner
 *  Send a packet in a 1.3-compatible manner
 *  and wait for completion; returns Res1 of the
 *  reply packet
 */

static LONG dopkt(struct MsgPort *pid, LONG action, LONG args[], LONG nargs)

/*
 * struct MsgPort *pid;    process identifier (handler message port)
 * LONG action;            packet type (desired action)
 * LONG args[];            a pointer to an argument list
 * LONG nargs;             number of arguments in list
*/
{       struct MsgPort  *replyport;
        struct StandardPacket *packet;

        LONG    count, *pargs, res1;

        replyport = (struct MsgPort *)CreatePort(NULL,0);
        if (! replyport) return NULL;

        packet = (struct StandardPacket *)
                        AllocMem((long)sizeof(struct StandardPacket),
                                        MEMF_PUBLIC|MEMF_CLEAR);
        if (! packet)
        {       DeletePort(replyport);
                return NULL;
        }

        packet->sp_Msg.mn_Node.ln_Name  = (char *)&(packet->sp_Pkt);
        packet->sp_Pkt.dp_Link                  = &(packet->sp_Msg);
        packet->sp_Pkt.dp_Port                  = replyport;
        packet->sp_Pkt.dp_Type                  = action;

        pargs = &(packet->sp_Pkt.dp_Arg1);      /* address of first arg */
        for (count=0;count < nargs; count++)
                pargs[count] = args[count];

        PutMsg(pid,(struct Message *)packet);   /* send packet */

        WaitPort(replyport);
        GetMsg(replyport);

        res1 = packet->sp_Pkt.dp_Res1;

        FreeMem(packet,(long)sizeof(struct StandardPacket));
        DeletePort(replyport);

        return (res1);
}

/*
 * We don't like SAS/C's rawcon() anymore. It relies on
 * pr_ConsoleTask being correct.
 */

/* should be fixed since SAS/C 6.55, maybe a SetMode(Input()) or similar
 * should be put here to satisfy the picky. (Damned, rawcon() is not in
 * another compiler-lib anyway AFAIK :-( )
 */

/*
int rawcon(int setraw)
{
        long args[1];
        args[0] = setraw;
        return (dopkt(((struct FileHandle *)BADDR(chkufb(fileno(stdout))
                                ->ufbfh))->fh_Type,
                        ACTION_SCREEN_MODE,args,1) == DOSTRUE) ?
                0 : -1;
}
*/

/*
 * stub for tputs
 */

#ifndef INDEX_DAEMON
int
tputs (char *str, int count, void (*func)(int))
{
        if (! str) {
                return 0;
        }

        while (*str) {
                if (*str == '\n')
                        func('\r');
                func(*str++);
        }
        return 0;
}
#endif

/*
 * joinpath tacks a file (or sub dir) on to the end of a directory name.
 * Not just as simple as putting a '/' between the two, as the directory
 * name may be an assign!
 */

int tin_bbs_mode = FALSE;

void
joinpath (char *str, const char *dir, const char *file)
{
        char c, *p;

        if (tin_bbs_mode) {
                if (p = strrchr(file,':')) file = p;
                while (*file == '/') file++;
        } else {
                if (strchr(file,':')) {
                        strcpy(str, file);
                        return;
                }
        }

        if (strlen (dir) == 0) {
                strcpy (str, file);
                return;
        }
        c = dir[strlen(dir)-1];
        if (c=='/' || c==':') {
                sprintf (str, "%s%s", dir, file);
        } else {
                sprintf (str, "%s/%s", dir, file);
        }
}


unsigned int sleep (unsigned int seconds)
{
        if (seconds) Delay (50*seconds);
        return seconds;
}

/*
 * I'm not really sure how well popen and pclose work, but they seem OK
 * at least with metamail 2.3a they work rather bad. :-(
 */

FILE *popen (char *command, char *mode)
{
        char cmd[256];
        char pname[16];

        sprintf(pname, "PIPE:%08X", FindTask(NULL));

        if (mode[0] == 'w') {
                sprintf (cmd, "run <>NIL: %s <%s", command, pname);
                system (cmd);
                return fopen (pname, mode);
        } else {
                FILE *rp;
                rp = fopen (pname, mode);
                sprintf (cmd,"run %s >%s",command, pname);
                system (cmd);
                return rp;
        }
}


int pclose (FILE *pipe)
{
        return fclose (pipe);
}

/*
 * Directory stuff
 */

DIR *opendir (char *name)
{
        DIR *di;

        di = calloc (1, sizeof (DIR));
        if (di == 0) {
                return 0;
        }
        di->Lock = Lock (name,ACCESS_READ);
        if (di->Lock == 0) {
                free (di);
                return 0;
        }
        if (DOSBase->dl_lib.lib_Version >= 37) {
                di->buffer = (struct ExAllData *)malloc(BUFSIZE);
                if (di->buffer == 0) {
                        UnLock(di->Lock);
                        free(di);
                        return 0;
                }
                di->eac = AllocDosObject(DOS_EXALLCONTROL,NULL);
                if ((di->eac) == 0) {
                        free(di->buffer);
                        UnLock(di->Lock);
                        free(di);
                        return 0;
                }
                di->eac->eac_LastKey = 0;
                di->more = 1;
        } else {
                if (Examine(di->Lock,&di->fib)==0) {
                        UnLock(di->Lock);
                        free (di);
                        return 0;
                }
        }
        return di;
}


struct dirent *readdir (DIR *di)
{
        static struct dirent de;

        if (DOSBase->dl_lib.lib_Version >= 37) {
                while (! di->bufp) {
                        if (! di->more) {
                                return 0;
                        }
                        di->more = ExAll(di->Lock, di->buffer, BUFSIZE, ED_NAME, di->eac);
                        if (di->eac->eac_Entries) {
                                di->bufp = di->buffer;
                        }
                }
                de.d_name = di->bufp->ed_Name;
                de.d_reclen = strlen (de.d_name);
                di->bufp = di->bufp->ed_Next;
        } else {
                if (ExNext (di->Lock, &di->fib) == 0) {
                        return 0;
                }
                de.d_name = di->fib.fib_FileName;
                de.d_reclen = strlen (de.d_name);
        }

        return &de;
}


void closedir (DIR *di)
{
        if (DOSBase->dl_lib.lib_Version >= 37) {
                if (di->more)
                        while (ExAll(di->Lock, di->buffer, BUFSIZE, ED_NAME, di->eac))
                                /* do nothing */ ;
                free(di->buffer);
                FreeDosObject(DOS_EXALLCONTROL,di->eac);
        }

        UnLock (di->Lock);
        free (di);
}

int getopt (int argc, char **argv, char *options)
{
        char c, *z;
        static int subind = 0;

        for (;optind < argc ; optind++, subind = 0) {
                if (subind == 0) {
                        c = argv[optind][0];
                        if (c != '-') {
                                return EOF;
                        }
                        subind = 1;
                }

                c = argv[optind][subind];
                if (c != 0) {
                        break;
                }
        }

        if (optind == argc) {
                return EOF;
        }

        /* get rid of funnies */
        if (c == ':' || c == '?') {
                return '?';
        }

        if ((z = strchr (options,c)) == 0) {
                return '?';
        }

        if (z[1] == ':') {
                if (argv[optind][subind+1]) {
                        optarg = &argv[optind][subind+1];
                } else {
                        optarg = argv[++optind];
                }
                optind++;
                subind = 0;
                return c;
        }
        subind++;
        return c;
}

/* nobody should use 1.3 anymore (does anybody use Win 1.0?), and system() is ANSI */
/* int system (const  char *str)
 * (deleted)
 */
/*
 * The stat call in Aztec C doesn't tell us if the entry is a directory
 * or not. This one does. You will have to change <stat.h> to define
 * ST_DIRECT though
 */

int stat (char *name, struct stat *buf)
{
        BPTR dirlock;
        register struct FileInfoBlock *inf;

        if (! (dirlock = Lock (name, ACCESS_READ))) {
                return -1;
        }
        if (! (inf = malloc(sizeof(*inf)))) {
                UnLock (dirlock);
                return -1;
        }
        Examine (dirlock,inf);
        UnLock (dirlock);
        buf->st_attr = ((inf->fib_EntryType>0) ? ST_DIRECT : 0)
                        | (inf->fib_Protection & 0xf);
        buf->st_size = inf->fib_Size;
        buf->st_mtime = ((inf->fib_Date.ds_Days + 2922) * (24 * 60) +
                        inf->fib_Date.ds_Minute) * 60
                        + inf->fib_Date.ds_Tick / TICKS_PER_SECOND;
        free (inf);
        return 0;
}

/*
 * This getenv and setenv will use the WB2.0 calls if you have the new
 * rom. If not, it resorts to looking in the ENV: directory.
 */

/* what new rom? 2.04? Anyway, SAS's getenv() doesn't use local vars */

char *getenv (register const char *name)
{
    int blen = 256;
    static char buffer[256];

    if (GetVar(name, buffer, blen, 0L) == -1)
        return 0;
    else
        return buffer;
}

int
setenv (char *name, char *value, int notused)
{
        if (DOSBase->dl_lib.lib_Version >= 36) {
                SetVar ((char *)name,(char *)value,strlen(value)+1,GVF_LOCAL_ONLY);
        }
        return 0;
}


char *mktemp (char *template)
{
        static const char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        static size_t count = 0;
        size_t len;
        char c;

        len = strlen(template);
        if (len < 6 || strcmp(&template[len-6],"XXXXXX")) {
                errno = EINVAL;
                return template;
        }

        c = letters[count++];
        count %= sizeof(letters);

        sprintf(&template[len-6], "%c%05x", c,
                (unsigned int) (getpid() & 0xfffff));

        return template;
}


void make_post_cmd (char *cmd, char *name)
{
        char *p;

        if ((p = getenv (ENV_VAR_POSTER)) != (char *) 0) {
                sprintf (cmd, p, name);
        } else {
                sprintf (cmd, DEFAULT_POSTER, name);
        }
}

#ifdef NNTP_ABLE

#define NUM_TEMP_FP 4
static struct {
        FILE *fp;
        char name[PATH_LEN];
} temp_fp[NUM_TEMP_FP];

void log_unlink(FILE *fp, char *fname)
{
        int i;

        for (i=0; i<NUM_TEMP_FP; i++) {
                if (temp_fp[i].fp == (FILE *) 0) {
                        temp_fp[i].fp = fp;
                        strcpy (temp_fp[i].name, fname);
                        break;
                }
        }
}

#undef fclose

int tmp_close(FILE *fp)
{
        int i, ret;

        ret = fclose(fp);
        for (i=0; i<NUM_TEMP_FP; i++) {
                if (temp_fp[i].fp == fp) {
                        unlink (temp_fp[i].name);
                        temp_fp[i].fp = (FILE *) 0;
                        break;
                }
        }
        return ret;
}

void __interrupt _STD_550_close_all(void)
{       int i;

        for(i = 0; i < NUM_TEMP_FP; i++) {
                if (temp_fp[i].fp) {
                        fclose(temp_fp[i].fp);
                        unlink (temp_fp[i].name);
                        temp_fp[i].fp = (FILE *) 0;
                }
        }
}

#endif

#ifdef MEM_DEBUG

/* These malloc/free routines are useful when debugging with mungwall.
 * Since the library routines use memory pools, memory leaks are more
 * difficult to determine. By calling AllocMem for each memory allocation,
 * mungwall can detect all of the nasty things it says that may be going on.
 */

struct memhead {
        struct memhead *next;
        long size;
        char mem[0];
};

static struct memhead *alloc_list = (struct memhead *) 0;

void * __interrupt malloc(size_t size)
{
        struct memhead *p;

        if (size == 0) return 0;
        p = (struct memhead *)AllocMem(size + sizeof(struct memhead), 0L);
        if (! p) return (void *) 0;
        p->next = alloc_list;
        p->size = size;
        alloc_list = p;
        return (void *)&p->mem;
}

void __interrupt free(void *p)
{
        struct memhead *p1,**q;

        p1 = &((struct memhead *)p)[-1];
        for (q = &alloc_list; q; q = &(*q)->next) {
                if (*q == p1) break;
        }
        if (q == (struct memhead **) 0) return;

        *q = p1->next;
        FreeMem(p1,p1->size + sizeof(struct memhead));
}

void * __interrupt realloc(void *p, size_t size)
{
        if (size == 0) {
                if (p) free(p);
                return 0;
        }

        if (p == 0) {
                return malloc(size);
        }

        {       int oldsize;
                void *p1;

                if ((oldsize = ((struct memhead *)p)[-1].size) == size)
                        return p;

                p1 = malloc(size);
                memcpy(p1, p, (oldsize < size) ? oldsize : size);
                free(p);
                return p1;
        }
}

void __interrupt _STD_250_free_all(void)
{
        struct memhead *p, *q;

        for (p = alloc_list; p; p = q)
        {       q = p->next;
                FreeMem(p, p->size + sizeof(struct memhead));
        }
        alloc_list = 0;
}

#endif

#else

/*
 * The ';' is to satisfy a really picky Ansi compiler
 */

;

#endif  /* M_AMIGA */
