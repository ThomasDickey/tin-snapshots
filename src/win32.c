/*
 *  Project   : tin - a Usenet reader
 *  Module    : win32.c
 *  Author    : S. Lam, D. Derbyshire, T. Loebach, N. Ellis, P. Haken
 *  Created   : 01-06-87
 *  Updated   : 10-17-94
 *  Notes     : Berkeley-style directory reading routine on Windows NT
 *				Console <-> Curses mapping layer.
 *  Copyright : (c) Copyright 1987-94 by Sam Lam, Drew Derbyshire, Tom Loebach
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"

#if defined(WIN32)

/*
 * Windows/NT include files
 */

#define INCL_BASE

#include <time.h>

#ifdef ERR
#undef ERR
#endif

#define ERR(s, c)	if (opterr) {\
	    fprintf(stderr, "%s%s%c\n", argv[0], s, c);}

int currentfile ();

typedef struct
	{
	DIR dir;
	char *pathname;
	HANDLE dirHandle;
	WIN32_FIND_DATA dirData;
	} XDIR;

BOOL inverse_okay = TRUE;
BOOL _hp_glitch = FALSE;

char TMPDIR[MAX_PATH];

int opterr = 1;
int optind = 1;
int optopt;
char *optarg;

struct passwd no_pw={0};
char szHomeDir[MAX_PATH];

static int curX, curY;
static WORD curAttr, baseAttr;

int cLINES, cCOLS;

int raw=TRUE;

static HANDLE hConIn, hConOut;
static CONSOLE_SCREEN_BUFFER_INFO csInfo;

/*
 * Open a directory
 */

DIR *opendirx (const char *dirname, char *pattern)
{
	XDIR *dirp;
	char *pathname;
	HANDLE dirHandle;
	WIN32_FIND_DATA dirData;

	pathname = malloc (strlen (dirname) + strlen (pattern) + 2);
	strcpy (pathname, dirname);
	if ((*pattern != '\\') || (dirname[strlen (dirname) - 1] != '\\'))
	{
		strcat (pathname, "\\");
	}
	strcat (pathname, pattern);	// Read the first file in the directory

	dirHandle = FindFirstFile (pathname, &dirData);
	if ((int) dirHandle == -1)
	{
		return NULL;
	}
	else
	{
		dirp = malloc (sizeof (XDIR));
		dirp->dir.dirfirst = 1;
		strcpy (dirp->dir.dirid, "DIR");
		dirp->dirHandle = dirHandle;
		dirp->dirData = dirData;
		dirp->pathname = pathname;
		return (DIR *) dirp;
	}
}
/*
 * Get next entry in a directory
 */
struct direct *readdir (DIR * dirp)
{
	BOOL rc;
	XDIR *xdirp = (XDIR *) dirp;

	assert (strcmp (xdirp->dir.dirid, "DIR") == 0);
	if (xdirp->dir.dirfirst)
	{
		xdirp->dir.dirfirst = 0;
	}
	else
	{
		rc = FindNextFile (xdirp->dirHandle, &xdirp->dirData);
	}
	if (! strcmp (xdirp->dirData.cFileName, "."))
	{
		rc = FindNextFile (xdirp->dirHandle, &xdirp->dirData);
	}
	if (! strcmp (xdirp->dirData.cFileName, ".."))
	{
		rc = FindNextFile (xdirp->dirHandle, &xdirp->dirData);
	}
	if (rc)
	{
		xdirp->dir.dirent.d_ino = -1;	/*
									 * no inode information
									 */
		strlwr (strcpy (xdirp->dir.dirent.d_name, xdirp->dirData.cFileName));
		xdirp->dir.dirent.d_namlen = strlen (xdirp->dirData.cFileName);
		xdirp->dir.dirent.d_reclen = sizeof (struct direct) - (MAXNAMLEN + 1) +
		    ((((xdirp->dir.dirent.d_namlen + 1) + 3) / 4) * 4);

		return &(xdirp->dir.dirent);
	}
	else
	{
		return NULL;
	}
}
/*
 * Close a directory
 */
void closedir (DIR * dirp)
{
	BOOL rc;
	XDIR *xdirp = (XDIR *) dirp;

	assert (strcmp (xdirp->dir.dirid, "DIR") == 0);
	rc = FindClose (xdirp->dirHandle);
	free (xdirp->pathname);
	free (xdirp);
	dirp = NULL;
}


char getopt (int argc, char *argv[], char *opts)
{
	static int sp = 1;
	register int c;
	register char *cp;

	if (sp == 1)
		if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
			return (EOF);
		else if (strcmp (argv[optind], "--") == 0)
		{
			optind++;
			return (EOF);
		}
	optopt = c = argv[optind][sp];
	if (c == ':' || (cp = strchr (opts, c)) == NULL)
	{
		ERR (": illegal option -- ", c);
		if (argv[optind][++sp] == '\0')
		{
			optind++;
			sp = 1;
		}
		return ('?');
	}
	if (*++cp == ':')
	{
		if (argv[optind][sp + 1] != '\0')
			optarg = &argv[optind++][sp + 1];
		else if (++optind >= argc)
		{
			ERR (": option requires an argument -- ", c);
			sp = 1;
			return ('?');
		}
		else
			optarg = argv[optind++];
		sp = 1;
	}
	else
	{
		if (argv[optind][++sp] == '\0')
		{
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return (c);
}
/*
 * joinpath tacks a file (or sub dir) on to the end of a directory name.
 * Have to check existing / or \ to avoid doubling them.
 */

void joinpath ( char *str, char *dir, char *file)
{
	char c;
	struct passwd *pw;

	if (strlen (dir) == 0)
	{
		strcpy (str, file);
		return;
	}

	if (file[0] == '.')
		file++;

	if (dir[0] == '~')
	{
		if (no_pw.pw_dir == NULL)
			pw = getpwnam(getlogin());
		else
			pw = &no_pw;

		strcpy(str, pw->pw_dir);
		dir++;
		str += strlen(str);
	}

	c = dir[strlen (dir) - 1];
	if (c == '/' || c == '\\')
	{
		sprintf (str, "%s%s", dir, file);
	}
	else
	{
		sprintf (str, "%s\\%s", dir, file);
	}
}

int make_post_cmd ( char *cmd, char *name)
{
	char *p;

	if ((p = getenv (ENV_VAR_POSTER)) != (char *) 0)
	{
		sprintf (cmd, p, name);
	}
	else
	{
		sprintf (cmd, DEFAULT_POSTER, name);
	}

	return 0;
}

long getpid () { return GetCurrentProcessId (); }
int getgid ()	{ return 1; }
int getuid ()	{ return 1; }
int setgid (int id)	{ return 1; }
int setuid (int id)	{ return 1; }
int getegid ()	{ return 1; }
int geteuid ()	{ return 1; }

#pragma warning( disable: 4035 )
struct passwd *getpwuid(int id)
{
	fprintf(stderr, "\nYou must set the environment variable USERNAME");
	exit(1);
}
#pragma warning( default: 4035 )

char * getlogin (void)
{
	return getenv("USERNAME");
}


struct passwd * getpwnam ( const char *name)
{
	char *env;

	if ( !(env=getenv("USERNAME")) )
	{
		fprintf(stderr, "\nYou must set the environment variable USERNAME");
		exit(1);
	}

	no_pw.pw_name = no_pw.pw_gecos = env;

	if ( !(env=getenv("HOMEDRIVE")) )
	{
		fprintf(stderr, "\nYou must set the environment variable HOMEDRIVE");
		exit(1);
	}
	strcpy(szHomeDir, env);

	if ( !(env=getenv("HOMEPATH")) )
	{
		fprintf(stderr, "\nYou must set the environment variable HOMEPATH");
		exit(1);
	}
	strcat(szHomeDir, env);
	no_pw.pw_dir = szHomeDir;

	return &no_pw;
}


void setup_screen()
{
	int cols, lines;

	cmd_line = FALSE;
//	setupterm();
	cols = cCOLS;
	lines = cLINES;
	ClearScreen();
	set_win_size(&lines,&cols);
	refresh();
}

void set_xclick_on()
{
}

void set_xclick_off()
{
}


// Curses functionality

void cursoron(void)
{
	CONSOLE_CURSOR_INFO ccinfo;

	if (GetConsoleCursorInfo(hConOut, &ccinfo))
		{
		ccinfo.bVisible = TRUE;
		SetConsoleCursorInfo(hConOut, &ccinfo);
		}
}

void cursoroff(void)
{
	CONSOLE_CURSOR_INFO ccinfo;

	if (GetConsoleCursorInfo(hConOut, &ccinfo))
		{
		ccinfo.bVisible = FALSE;
		SetConsoleCursorInfo(hConOut, &ccinfo);
		}
}

void GetConsoleCursorPosition(HANDLE hCon, COORD *pCoord)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	assert(pCoord);

	GetConsoleScreenBufferInfo( hCon, &csbi);
	*pCoord = csbi.dwCursorPosition;
}

void StartInverse(void)
{
	curAttr = baseAttr;
	ToggleInverse();
}

void EndInverse(void)
{
	curAttr = baseAttr;
	SetConsoleTextAttribute(hConOut, curAttr);
}

void ToggleInverse(void)
{
	short color = curAttr & 0x00FF;
	curAttr = (curAttr & 0xFF00) | (color >> 4) | ((color & 0x000F) << 4);

	SetConsoleTextAttribute(hConOut, curAttr);
}


void MoveCursor(int y, int x)
{
	COORD dwOrigin;
	dwOrigin.X = curX = x;
	dwOrigin.Y = curY = y;
	SetConsoleCursorPosition (hConOut, dwOrigin);
}

void addch(UCHAR c)
{
	assert(hConOut != NULL);

	switch(c)
	{
		case '\n':
			MoveCursor(curY+1, curX);
			break;

		case '\r':
			MoveCursor(curY, 0);
			break;

		case '\b':
			if (curX>0)
				MoveCursor(curY, curX-1);
			break;

		default:
			{
				DWORD cchWritten;

				WriteConsole(hConOut, &c, 1, &cchWritten, NULL);
				if (curX++ >= cCOLS)
				{
					curX = 0;
					curY++;
				}
			}
			break;
	}
}


// Print a line of text and draw to the end of the line
#undef addline
void addline (unsigned char *lpszStr)
{
	DWORD cchWritten;
	COORD pt;
	INT ich = strlen(lpszStr);

	if (lpszStr[ich - 2] == '\r')
		ich -= 2;

	assert(hConOut != NULL);

	WriteConsole(hConOut, lpszStr, ich, &cchWritten, NULL);

	GetConsoleCursorPosition(hConOut, &pt);
	MoveCursor( pt.Y, pt.X);

	CleartoEOLN();
}

void addstr(unsigned char *lpszStr)
{
	DWORD cchWritten;
	COORD pt;

	assert(hConOut != NULL);

	WriteConsole(hConOut, lpszStr, strlen(lpszStr), &cchWritten, NULL);

	GetConsoleCursorPosition(hConOut, &pt);
	MoveCursor( pt.Y, pt.X);
}

void CleartoEOS(void)
{
	CONSOLE_SCREEN_BUFFER_INFO csInfo;
	DWORD cCharCells;	/* number of character cells to write to	*/
	COORD coordChar;	/* x- and y-coordinates of first cell	*/
	DWORD cWritten;

	coordChar.X = 0;
	coordChar.Y = curY;

	cCharCells = cCOLS * (cLINES+1-curY);

	GetConsoleScreenBufferInfo (hConOut, &csInfo);

	baseAttr = curAttr = csInfo.wAttributes;

	FillConsoleOutputCharacter(hConOut, ' ',     cCharCells, coordChar, &cWritten);
	FillConsoleOutputAttribute(hConOut, curAttr, cCharCells, coordChar, &cWritten);
}

void CleartoEOLN(void)
{
	DWORD cCharCells;	/* number of character cells to write to	*/
	COORD coordChar;	/* x- and y-coordinates of first cell	*/
	DWORD cWritten;

	coordChar.X = curX;
	coordChar.Y = curY;

	cCharCells = cCOLS - curX;

	FillConsoleOutputCharacter(hConOut, ' ',     cCharCells, coordChar, &cWritten);
	FillConsoleOutputAttribute(hConOut, curAttr, cCharCells, coordChar, &cWritten);
}


int InitScreen(void)
{
	hConIn = GetStdHandle (STD_INPUT_HANDLE);
	hConOut = GetStdHandle (STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo (hConOut, &csInfo);

	baseAttr = curAttr = csInfo.wAttributes;
#ifdef USE_WINDOW_BUFFER_SIZE
	cLINES = csInfo.dwSize.Y - 1;
	cCOLS = csInfo.dwSize.X;
#else
	{
	COORD wCoord;

	cLINES = csInfo.srWindow.Bottom - csInfo.srWindow.Top;
	cCOLS = csInfo.srWindow.Right - csInfo.srWindow.Left + 1;
	wCoord.Y = cLINES;
	wCoord.X = cCOLS;
	SetConsoleScreenBufferSize (hConOut, wCoord);
	}
#endif

	return -1;
}

void ClearScreen(void)
{
	MoveCursor( 0,0 );
	CleartoEOS();
}

void refresh(void)
{
}

void InitWin(void)
{
}

void EndWin(void)
{
#ifndef USE_WINDOW_BUFFER_SIZE
	COORD wCoord;

	wCoord.Y = csInfo.dwSize.Y;
	wCoord.X = csInfo.dwSize.X;
	SetConsoleScreenBufferSize (hConOut, wCoord);
#endif
}

int RawState(void)
{
	return raw;
}

void Raw(int f)
{
	raw = f;
}

int ReadCh (void)
{
	INPUT_RECORD irBuffer;
	DWORD cRead;
	BOOL fOkay;

	DWORD dwMode;
	DWORD dwModeSave = 0L;

	assert(hConIn != NULL);

	if (GetConsoleMode (hConIn, &dwMode))
	{
		dwModeSave = dwMode;
		dwMode &= ~ENABLE_LINE_INPUT;
		if (raw)
			dwMode &= ~ENABLE_ECHO_INPUT;
		SetConsoleMode (hConIn, dwMode);
	}

	do
	{
		WaitForSingleObject( hConIn, INFINITE );
		fOkay = ReadConsoleInput( hConIn, &irBuffer, 1, &cRead);
	} while(
		(irBuffer.EventType != KEY_EVENT) ||
		(!irBuffer.Event.KeyEvent.bKeyDown) ||
		((irBuffer.Event.KeyEvent.wVirtualKeyCode >= VK_SHIFT) &&
		(irBuffer.Event.KeyEvent.wVirtualKeyCode <= VK_CAPITAL)) );

	if (dwModeSave)
		SetConsoleMode (hConIn, dwModeSave);

	if (irBuffer.Event.KeyEvent.uChar.AsciiChar)
	{
		return (char) irBuffer.Event.KeyEvent.uChar.AsciiChar;
	}
	else
	{
		if ((irBuffer.Event.KeyEvent.wVirtualKeyCode > VK_SPACE) &&
			(irBuffer.Event.KeyEvent.wVirtualKeyCode < VK_HELP))
		{
			return (char) irBuffer.Event.KeyEvent.wVirtualKeyCode | 0x80;
		}
		else
		{
			return irBuffer.Event.KeyEvent.wVirtualKeyCode;
		}
	}
}

/*
 * Rename, clobber the new file if it exists.
 */
int win32rename(char *src, char *dest)
	{
	if (MoveFile(src,dest))
		return 0;
	else
		{
		int rc=GetLastError();
		return -1;
		}
	}

/*
 * Stuff for handling NT's inability to unlink an open file...  Grabbed from
 * AMI sources.
 */

#define NUM_TEMP_FP 6
static struct {
    FILE *fp;
    char name[PATH_LEN];
} temp_fp[NUM_TEMP_FP];

void log_unlink(FILE *fp, char *fname)
{   int i;
    for (i=0; i<NUM_TEMP_FP; i++) {
        if (temp_fp[i].fp == (FILE *) 0) {
            temp_fp[i].fp = fp;
            strcpy (temp_fp[i].name, fname);
            break;
        }
    }
}

#undef fclose

int
tmp_close(FILE *fp)
{   int i, ret;

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


#ifdef DEBUG
void DebugAssertDialog(LPSTR szMessage, DWORD dwLine, LPSTR szFile)
	{
	char szMessageBuf[256];
	UINT uType;

	sprintf(szMessageBuf, "%s at line %ld of %s", szMessage, dwLine, szFile);

	uType = MB_SYSTEMMODAL | MB_ICONSTOP | MB_OKCANCEL;
	if (MessageBox(NULL, (LPTSTR) szMessageBuf, (LPTSTR) "Assert Failed", uType) == IDCANCEL)
    	DebugBreak();
	}
#endif

#endif /* WIN32 */
