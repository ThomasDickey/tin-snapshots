/*
 *  Project   : tin - a Usenet reader
 *  Module    : curses.c
 *  Author    : D.Taylor & I.Lea
 *  Created   : 01-01-86
 *  Updated   : 17-05-94
 *  Notes     : This is a screen management library borrowed with permission
 *              from the Elm mail system. This library was hacked to provide 
 *              what tin needs.
 *  Copyright : Copyright (c) 1986-94 Dave Taylor & Iain Lea
 *              The Elm Mail System  -  @Revision: 2.1 $   $State: Exp @
 */

#include "tin.h"

#ifndef ns32000
#	undef	sinix
#endif

#ifdef VMS
#include <descrip.h>
#include <iodef.h>
#include <ssdef.h>
#include <dvidef.h>
#ifdef __GNUC__
#include <sys$routines.h>
#include <lib$routines.h>
#endif
#endif

#ifdef HAVE_ERRNO_H
#	include <errno.h>
#else
#	include <sys/errno.h>
#endif

#define DEFAULT_LINES_ON_TERMINAL	24
#define DEFAULT_COLUMNS_ON_TERMINAL	80

int cLINES = DEFAULT_LINES_ON_TERMINAL - 1;
int cCOLS  = DEFAULT_COLUMNS_ON_TERMINAL;
int inverse_okay = TRUE;
static int _inraw = FALSE;	/* are we IN rawmode?    */
int _hp_glitch = FALSE;		/* standout not erased by overwriting on HP terms */
static int xclicks=FALSE;	/* do we have an xterm? */

#ifndef INDEX_DAEMON

#define BACKSPACE        '\b'
#define VERY_LONG_STRING 2500
#define TTYIN            0

#ifdef HAVE_CONFIG_H

#if HAVE_TERMIOS_H
#	ifdef HAVE_IOCTL_H
#		include <ioctl.h>
#	else
#		ifdef HAVE_SYS_IOCTL_H
#			include <sys/ioctl.h>
#		endif
#	endif
#	if !defined(sun) || !defined(NL0)
#		include <termios.h>
#	endif
#	define USE_POSIX_TERMIOS 1
#	define TTY struct termios
#else
#	if HAVE_TERMIO_H
#		include <termio.h>
#		define USE_TERMIO 1
#		define TTY struct termio
#	else
#		if HAVE_SGTTY_H
#			include <sgtty.h>
#			define USE_SGTTY 1
#			define TTY struct sgttyb
#		else
			please-fix-me(thanks)
#		endif
#	endif
#endif
static	TTY _raw_tty, _original_tty;

#else	/* FIXME: prune the non-autoconf'd stuff */

#if (defined(M_AMIGA) && !defined(__SASC)) || defined(COHERENT) || defined(BSD)
#	ifdef BSD4_1
#		include <termio.h>
#		define USE_TERMIO 1
#	else
#		include <sgtty.h>
#		define USE_SGTTY 1
#	endif
#else
#	ifndef SYSV
#		ifdef MINIX
#			include <sgtty.h>
#			define USE_SGTTY 1
#		else
#			ifndef QNX42
#				ifdef sinix
#					include <termios.h>
#					define USE_POSIX_TERMIOS 1
#				else
#					ifdef VMS
#						include <curses.h>
#					else
#						include <termio.h>
#						define USE_TERMIO 1
#					endif
#				endif
#			endif
#		endif
#	else
#		if defined(__hpux) || (defined(sun) && defined(SVR4))
#			include <termio.h>
#			define USE_TERMIO 1
#		endif
#	endif
#endif

#ifndef VMS
#if (defined(M_AMIGA) && !defined(__SASC)) || defined(BSD) || defined(MINIX)
#	ifdef TCGETA
#		undef TCGETA
#	endif
#	define TCGETA	TIOCGETP
#	ifdef TCSETAW
#		undef TCSETAW
#	endif
#	define TCSETAW	TIOCSETP
#	define USE_SGTTY 1
struct sgttyb _raw_tty, _original_tty;
#else
#	if !defined(M_AMIGA) && !defined(M_OS2)
#		if defined(HAVE_TERMIOS_H) || defined(sinix)
#			ifndef TCGETA
#				define TCGETA	STCGETA
#			endif
#			ifndef TCSETAW
#				define TCSETAW	STCSETAW
#			endif
#			define USE_POSIX_TERMIOS 1
struct termios _raw_tty, _original_tty;
#		else
#			define USE_TERMIO 1
struct termio _raw_tty, _original_tty;
#		endif
#	endif
#endif
#endif /* VMS */

#endif /* HAVE_CONFIG_H */

#ifndef USE_SGTTY
#define USE_SGTTY 0
#endif

#ifndef USE_TERMIO
#define USE_TERMIO 0
#endif

#ifndef USE_POSIX_TERMIOS
#define USE_POSIX_TERMIOS 0
#endif

static char *_clearscreen, *_moveto, *_cleartoeoln, *_cleartoeos,
			*_setinverse, *_clearinverse, *_setunderline, *_clearunderline,
			*_xclickinit, *_xclickend, *_cursoron, *_cursoroff,
			*_terminalinit, *_terminalend, *_keypadlocal, *_keypadxmit;

#ifdef M_AMIGA
static char *_getwinsize;
#endif

static int _columns, _line, _lines;

#ifdef M_UNIX

#ifndef HAVE_TCSETATTR
#define tcsetattr(fd,cmd,arg) ioctl(fd, cmd, arg)
#endif

#ifndef HAVE_TCGETATTR
#define tcgetattr(fd, arg)    ioctl(fd, TCGETA, arg)
#endif

static char _terminal[1024];		/* Storage for terminal entry */
static char _capabilities[1024];	/* String for cursor motion */

static char *ptr = _capabilities;	/* for buffering         */

/*
 * Prototypes for the termcap functions, if we cannot find them in a standard
 * header file (e.g., curses.h).
 */
#if defined(DECL_TGETSTR)
extern int	tgetent P_((char *, char *));	/* get termcap entry */
extern char	*tgetstr P_((char *, char **));	/* Get termcap capability (string) */
extern int	tgetflag P_((char *));			/* Get termcap capability (boolean) */
extern int	tgetnum P_((char *));			/* Get termcap capability (number) */
extern char	*tgoto P_((char *, int, int));	/* and the goto stuff    */
#endif

#endif	/* M_UNIX */

static int in_inverse;			/* 1 when in inverse, 0 otherwise */

int	outchar P_((int c));	/* char output for tputs */

#endif /* INDEX_DAEMON */


void
setup_screen ()
{
#ifndef INDEX_DAEMON
	/*
	 * get screen size from termcap entry & setup sizes
	 */
	_line = 1;
	ScreenSize (&cLINES, &cCOLS);
	cmd_line = FALSE;
	Raw (TRUE);
	set_win_size (&cLINES, &cCOLS);

#endif /* INDEX_DAEMON */
}


#ifdef M_UNIX

int
InitScreen ()
{
#ifndef INDEX_DAEMON
	char termname[40], *p;
	
	if ((p = (char *) getenv ("TERM")) == (char *) 0) {
		fprintf (stderr, "%s: TERM variable must be set to use screen capabilities\n", progname);
		return (FALSE);
	}
	if (strcpy (termname, p) == NULL) {
		fprintf (stderr,"%s: Can't get TERM variable\n", progname);
		return (FALSE);
	}
	if (tgetent (_terminal, termname) != 1) {
		fprintf (stderr,"%s: Can't get entry for TERM\n", progname);
		return (FALSE);
	}

	/* load in all those pesky values */
	_clearscreen    = tgetstr ("cl", &ptr);
	_moveto         = tgetstr ("cm", &ptr);
	_cleartoeoln    = tgetstr ("ce", &ptr);
	_cleartoeos     = tgetstr ("cd", &ptr);
	_lines          = tgetnum ("li");
	_columns        = tgetnum ("co");
	_setinverse     = tgetstr ("so", &ptr);
	_clearinverse   = tgetstr ("se", &ptr);
	_setunderline   = tgetstr ("us", &ptr);
	_clearunderline = tgetstr ("ue", &ptr);
	_hp_glitch = tgetflag ("xs");
#ifdef HAVE_BROKEN_TGETSTR
	_terminalinit   = "";
	_terminalend    = "";
	_keypadlocal    = "";
	_keypadxmit     = "";
#else
	_terminalinit   = tgetstr ("ti", &ptr);
	_terminalend    = tgetstr ("te", &ptr);
	_keypadlocal    = tgetstr ("ke", &ptr);
	_keypadxmit     = tgetstr ("ks", &ptr);
#endif
	_cursoron = NULL;
	_cursoroff = NULL;

	if (STRCMPEQ(termname, "xterm")) {
		xclicks = TRUE;
		_xclickinit	= "\033[?9h";
		_xclickend	= "\033[?9l";
	}

	InitWin ();

	if (!_clearscreen) {
		fprintf (stderr,
			"%s: Terminal must have clearscreen (cl) capability\n",progname);
		return (FALSE);
	}
	if (!_moveto) {
		fprintf (stderr,
			"%s: Terminal must have cursor motion (cm)\n", progname);
		return (FALSE);
	}
	if (!_cleartoeoln) {
		fprintf (stderr,
			"%s: Terminal must have clear to end-of-line (ce)\n", progname);
		return (FALSE);
	}
	if (!_cleartoeos) {
		fprintf (stderr,
			"%s: Terminal must have clear to end-of-screen (cd)\n", progname);
		return (FALSE);
	}
	if (_lines == -1)
		_lines = DEFAULT_LINES_ON_TERMINAL;
	if (_columns == -1)
		_columns = DEFAULT_COLUMNS_ON_TERMINAL;
	/* 
	 * kludge to workaround no inverse 
	 */
	if (_setinverse == 0) {
		_setinverse = _setunderline;
		_clearinverse = _clearunderline;
		if (_setinverse == 0)
			draw_arrow_mark = 1;
	}
	return (TRUE);

#else

	return (FALSE);

#endif /* INDEX_DAEMON */
}

#else	/* ! M_UNIX  */

int
InitScreen ()
{
#ifndef	INDEX_DAEMON

	char *ptr;

	/* 
	 * we're going to assume a terminal here... 
	 */

	_clearscreen	= "\033[1;1H\033[J";
	_moveto		= "\033[%d;%dH";	/* not a termcap string! */
	_cleartoeoln	= "\033[K";
	_setinverse	= "\033[7m";
	_clearinverse	= "\033[0m";
	_setunderline	= "\033[4m";
	_clearunderline	= "\033[0m";
	_keypadlocal	= "";
	_keypadxmit	= "";
#ifdef M_AMIGA
	_terminalinit	= "\033[12{\033[0 p";
	_terminalend	= "\033[12}\033[ p";
	_cursoron		= "\033[ p";
	_cursoroff		= "\033[0 p";
	_cleartoeos		= "\033[J";
	_getwinsize		= "\2330 q";
#endif
#if defined(M_OS2)
	_cleartoeos	= NULL;
	_terminalinit	= NULL;
	_terminalend	= "";
	initscr ();
#endif /* M_OS2 */
#if defined(VMS)
	_cleartoeos		= "\033[J";
	_terminalinit	= NULL;
	_terminalend	= "";
#endif

	_lines = _columns = -1;

	/*
	 * Get lines and columns from environment settings - useful when
	 * you're using something other than an Amiga window
	 */

	if (ptr = getenv ("LINES")) {
		_lines = atol (ptr);
	}
	if (ptr = getenv ("COLUMNS")) {
		_columns = atol (ptr);
	}

	/* 
	 * If that failed, try get a response from the console itself 
	 */
#ifdef M_AMIGA
	if (_lines == -1 || _columns == -1) {
		_lines = DEFAULT_LINES_ON_TERMINAL;
		_columns = DEFAULT_COLUMNS_ON_TERMINAL;
	} else {
		_terminalinit = NULL;		/* don't do fancy things on a non-amiga console */
		_terminalend = NULL;
		_cursoroff = NULL;
		_cursoron = NULL;
		_getwinsize = NULL;
	}
#endif	/* M_AMIGA */
#ifdef M_OS2
	if (_lines == -1 || _columns == -1) {
		_lines = LINES;
		_columns = COLS;
	}
#endif	/* M_OS2 */

	InitWin ();

#ifdef VMS
	{
	  int input_chan, status;
	  int item_code, eightbit;
	  struct sensemode {
	    short status;
	    unsigned char xmit_baud;
	    unsigned char rcv_baud;
	    unsigned char crfill;
	    unsigned char lffill;
	    unsigned char parity;
	    unsigned char unused;
	    char class;
	    char type;
	    short scr_wid;
	    unsigned long tt_char : 24, scr_len : 8;
	    unsigned long tt2_char;
	  } tty;
	  $DESCRIPTOR (input_dsc, "TT");

	  status = SYS$ASSIGN (&input_dsc, &input_chan, 0, 0);
	  if (! (status & 1))
	      LIB$STOP (status);
	  SYS$QIOW (0, input_chan, IO$_SENSEMODE, &tty, 0, 0,
		    &tty.class, 12, 0, 0, 0, 0);
	  item_code = DVI$_TT_EIGHTBIT;
	  status = LIB$GETDVI( &item_code, &input_chan, 0, &eightbit, 0,0 );
	  _columns = tty.scr_wid;
	  _lines = tty.scr_len;

	if (eightbit)
	 { /* if using eightbit then use CSI (octal 233) rather than ESC "[" */
	  _clearscreen	= "\2331;1H\233J";
	  _moveto	= "\233%d;%dH"; /* not a termcap string! */
	  _cleartoeoln	= "\233K";
	  _cleartoeos	= "\233J";
	  _setinverse	= "\2337m";
	  _clearinverse = "\2330m";
	  _setunderline = "\2334m";
	  _clearunderline	= "\2330m";
	  _keypadlocal	= "";
	  _keypadxmit	= "";
	 }
	else
	 {
	  _clearscreen	= "\033[1;1H\033[J";
	  _moveto	= "\033[%d;%dH";	/* not a termcap string! */
	  _cleartoeoln	= "\033[K";
	  _cleartoeos	= "\033[J";
	  _setinverse	= "\033[7m";
	  _clearinverse = "\033[0m";
	  _setunderline = "\033[4m";
	  _clearunderline	= "\033[0m";
	  _keypadlocal	= "";
	  _keypadxmit	= "";
	 }
#ifdef HAVE_IS_XTERM
	if (is_xterm())
	 {
	  xclicks = TRUE;
	  if (eightbit)
	   { /* These are the settings for a DECterm but the reply can't easily be parsed */
	     /* Reply is of the form - CSI Pe ; Pb ; Pr ; Pc & w
		    Where Pe is the event, Pb the button, Pr and Pc the row and column */
/**
	     _xclickinit = "\2331;2'z";
	     _xclickend = "\2330;0'z";
**/
	    }
	   else
	    {
	     _xclickinit = "\033[?9h";
	     _xclickend  = "\033[?9l";
	    }
	 }
#endif

	}
#endif
	Raw (FALSE);

	return (TRUE);
#else
	return (FALSE);

#endif /* INDEX_DAEMON */
}

#endif	/* M_UNIX */

/*
 *  returns the number of lines and columns on the display.
 */
 
void
ScreenSize (num_lines, num_columns)
	int *num_lines, *num_columns;
{
#ifndef INDEX_DAEMON

	if (_lines == 0) _lines = DEFAULT_LINES_ON_TERMINAL;
	if (_columns == 0) _columns = DEFAULT_COLUMNS_ON_TERMINAL;

	*num_lines = _lines - 1;		/* assume index from zero*/
	*num_columns = _columns;		/* assume index from one */

#endif /* INDEX_DAEMON */
}


void
InitWin ()
{
#ifndef INDEX_DAEMON

	if (_terminalinit) {
		tputs (_terminalinit, 1, outchar);
		fflush (stdout);
	}	
	set_keypad_on ();
	set_xclick_on ();

#endif /* INDEX_DAEMON */
}


void
EndWin ()
{
#ifndef INDEX_DAEMON

	if (_terminalend) {
		tputs (_terminalend, 1, outchar);
		fflush (stdout);
	}
	set_keypad_off ();
	set_xclick_off ();
	
	
#endif /* INDEX_DAEMON */
}


void
set_keypad_on ()
{
#ifndef INDEX_DAEMON
#    ifdef HAVE_KEYPAD
 	if (use_keypad && _keypadxmit) {
		tputs (_keypadxmit, 1, outchar);
		fflush (stdout);
	}
#    endif
#endif /* INDEX_DAEMON */
}


void
set_keypad_off ()
{
#ifndef INDEX_DAEMON
#    ifdef HAVE_KEYPAD
	if (use_keypad && _keypadlocal) {
		tputs (_keypadlocal, 1, outchar);
		fflush (stdout);
	}
#    endif
#endif /* INDEX_DAEMON */
}

/*
 *  clear the screen: returns -1 if not capable
 */

void
ClearScreen ()
{
#ifndef INDEX_DAEMON

	tputs (_clearscreen, 1, outchar);
	fflush (stdout);      /* clear the output buffer */
	_line = 1;
	
#endif /* INDEX_DAEMON */
}

/*
 *  move cursor to the specified row column on the screen.
 *  0,0 is the top left!
 */

#ifdef M_UNIX

void
MoveCursor (row, col)
	int row, col;
{
#ifndef INDEX_DAEMON

	char *stuff;

	stuff = tgoto (_moveto, col, row);
	tputs (stuff, 1, outchar);
	fflush (stdout);
	_line = row + 1;
	
#endif /* INDEX_DAEMON */
}

#else	/* ! M_UNIX */

void
MoveCursor (row, col)
	int row, col;
{
#ifndef INDEX_DAEMON

	char stuff[12], *tgoto();

	if (_moveto) {
		sprintf (stuff, _moveto, row+1, col+1);
		tputs (stuff, 1, outchar);
		fflush (stdout);
		_line = row + 1;
	}

#endif /* INDEX_DAEMON */
}

#endif	/* M_UNIX */

/*
 *  clear to end of line
 */

void
CleartoEOLN ()
{
#ifndef INDEX_DAEMON

	tputs (_cleartoeoln, 1, outchar);
	fflush (stdout);  /* clear the output buffer */

#endif /* INDEX_DAEMON */
}

/*
 *  clear to end of screen
 */

void
CleartoEOS ()
{
#ifndef INDEX_DAEMON

	int i;
	
	if (_cleartoeos) {
		tputs (_cleartoeos, 1, outchar);
	} else {
		for (i=_line - 1 ; i < _lines ; i++) {
			MoveCursor (i, 0);
			CleartoEOLN ();
		}
	}
	fflush (stdout);  /* clear the output buffer */

#endif /* INDEX_DAEMON */
}

/*
 *  set inverse video mode
 */

void
StartInverse ()
{
#ifndef INDEX_DAEMON

	in_inverse = 1;
	if (_setinverse && inverse_okay) {
#ifdef HAVE_COLOR
		if (use_color) {
			bcol(col_invers);
		} else {
			tputs (_setinverse, 1, outchar);
		}
#else
		tputs (_setinverse, 1, outchar);
#endif
	}
	fflush (stdout);

#endif /* INDEX_DAEMON */
}

/*
 *  compliment of startinverse
 */

void
EndInverse ()
{
#ifndef INDEX_DAEMON

	in_inverse = 0;
	if (_clearinverse && inverse_okay) {
#ifdef HAVE_COLOR
		if (use_color) {
			bcol(col_back);
		} else {
			tputs (_clearinverse, 1, outchar);
		}
#else
		tputs (_clearinverse, 1, outchar);
#endif
	}
	fflush (stdout);

#endif /* INDEX_DAEMON */
}

/*
 *  toggle inverse video mode
 */

void
ToggleInverse ()
{
#ifndef INDEX_DAEMON

	if (in_inverse == 0)
		StartInverse();
	else
		EndInverse();

#endif /* INDEX_DAEMON */
}

/*
 *  returns either 1 or 0, for ON or OFF
 */

int
RawState()
{
	return (_inraw);
}

/*
 *  state is either TRUE or FALSE, as indicated by call
 */

void
Raw (state)
	int state;
{
#ifdef VMS
	if (state == FALSE && _inraw) {
/*	  vmsnoraw();*/
	  _inraw = 0;
	} else if (state == TRUE && ! _inraw) {
/*	  vmsraw();*/
	  _inraw = 1;
	}
#else
#if !defined(INDEX_DAEMON) && !defined(M_OS2)

#if defined(M_AMIGA) && defined(__SASC)
	_inraw = state;
	rawcon (state);
#else
	if (state == FALSE && _inraw) {
#ifdef HAVE_TCSETATTR
		(void) tcsetattr (TTYIN, TCSANOW, &_original_tty);
#else
		(void) ioctl (TTYIN, TCSETAW, &_original_tty);
#endif
		_inraw = 0;
	} else if (state == TRUE && ! _inraw) {
		(void) tcgetattr (TTYIN, &_original_tty);
		(void) tcgetattr (TTYIN, &_raw_tty);

#if USE_SGTTY || defined(M_AMIGA) || defined(MINIX)
		_raw_tty.sg_flags &= ~(ECHO | CRMOD);	/* echo off */
		_raw_tty.sg_flags |= CBREAK;		/* raw on */
#ifdef M_AMIGA	
		_raw_tty.sg_flags |= RAW; /* Manx-C 5.2 does not support CBREAK */
#endif
#else
		_raw_tty.c_lflag &= ~(ICANON | ECHO);	/* noecho raw mode        */
		_raw_tty.c_cc[VMIN] = '\01';	/* minimum # of chars to queue    */
		_raw_tty.c_cc[VTIME] = '\0';	/* minimum time to wait for input */
#endif

#ifdef HAVE_TCSETATTR
		(void) tcsetattr (TTYIN, TCSANOW, &_raw_tty);
#else
		(void) ioctl (TTYIN, TCSETAW, &_raw_tty);
#endif
		_inraw = 1;
	}
#endif	/* M_AMIGA */

#endif /* INDEX_DAEMON */
#endif /* !VMS */
}

/*
 *  read a character with Raw mode set!
 */

#ifndef VMS
#ifdef M_OS2

int
ReadCh ()
{
#ifndef INDEX_DAEMON

	extern int errno;
	char ch;
	KBDKEYINFO os2key;
	int rc;
	register int result = 0;
	static secondkey = 0;

	if (secondkey) {
		result = secondkey;
		secondkey = 0;
	} else {
		rc = KbdCharIn((PKBDKEYINFO)&os2key, IO_WAIT, 0);
		result = os2key.chChar;
		if (result == 0xe0) {
			result = 0x1b;
			switch (os2key.chScan) {
				case 'H': 
					secondkey = 'A';
					break;
				case 'P': 
					secondkey = 'B';
					break;
				case 'K': 
					secondkey = 'D';
					break;
				case 'M': 
					secondkey = 'C';
					break;
				case 'I': 
					secondkey = 'I';
					break;
				case 'Q': 
					secondkey = 'G';
					break;
				case 'G': 
					secondkey = 'H';
					break;
				case 'O': 
					secondkey = 'F';
					break;
				default:
					secondkey = '?';
			}
		} else if (result == 0x0d) {
			result = 0x0a;
		}
	}
	return ((result == EOF) ? EOF : result & 0xFF);

#endif /* INDEX_DAEMON */
}

#endif /* M_OS2 */

#ifdef M_AMIGA
#include <sprof.h>

static int new_lines, new_columns;

static int
AmiReadCh (int getscrsize)
{
#ifndef INDEX_DAEMON

static unsigned char buf[128];
static int buflen = 0, bufp = 0;
	int result;
	unsigned char ch;

	while (getscrsize || buflen == 0) {
PROFILE_OFF();
		result = read (0, (char *)&buf[buflen], 1);
PROFILE_ON();
		if (result <= 0) return EOF;
		buflen++;
		if (buf[bufp] == KEY_PREFIX) {
			do {
				result = read (0, (char *)&buf[buflen], 1);
				if (result <= 0) return EOF;
			} while (buf[buflen++] < 0x40);

			switch (buf[buflen-1])
			{	char *ptr;
				long class;
			  case 'r':		/* Window bounds report */
				ptr = (char *)&buf[bufp+1];
				my_strtol(ptr,&ptr,10); ptr++;
				my_strtol(ptr,&ptr,10); ptr++;
				new_lines = my_strtol(ptr,&ptr,10); ptr++;
				new_columns = my_strtol(ptr,&ptr,10);
				buflen = bufp;
				if (getscrsize) return 0;
				break;
			  case '|':		/* Raw Input Events */
				ptr = (char *)&buf[bufp+1];
				class = my_strtol(ptr,&ptr,10); ptr++;
				switch (class)
				{	int x,y;
				  case 12:	/* Window resized */
					buflen = bufp; /* Must do this before raise() */
					raise(SIGWINCH);
					break;
#ifdef notdef
				  case 2:	/* Mouse event */
					/*
					 * At this point we know what button was pressed
					 * but we don't really know where the mouse is.
					 * The <x> and <y> parameters don't help.
					 * Perhaps looking directly in the window's structure
					 * is the easiest thing to do (after finding out where
					 * the window's structure is! Sending an ACTION_INFO
					 * packet to the handler gives us this. I don't know
					 * if there is an easier way.
					 */
					buflen = bufp;
					break;
#endif
				  default:
					buflen = bufp;
					break;
				}
				break;
			}
		}
	}

	ch = buf[bufp++];
	if (bufp >= buflen) buflen = bufp = 0;
	return ch;
#else
	return -1;
#endif /* INDEX_DAEMON */
}

int
ReadCh()
{
	return AmiReadCh(0);
}

void
AmiGetWinSize(int *lines, int *columns)
{
#ifndef INDEX_DAEMON
	if (_getwinsize) {
		tputs (_getwinsize,1,outchar);	/* identify yourself */
		fflush (stdout);
		AmiReadCh(1);		/* Look for the identification */
		*lines = new_lines;
		*columns = new_columns;
	}
#endif /* INDEX_DAEMON */
}

#endif /* M_AMIGA */

#ifdef M_UNIX

int
ReadCh ()
{
#ifndef INDEX_DAEMON
	extern int errno;
	char ch;
	register int result;
	
#ifdef READ_CHAR_HACK
#undef getc
	while ((result = getc(stdin)) == EOF) {
		if (feof(stdin))
			break;

#ifdef EINTR
		if (ferror(stdin) && errno != EINTR)
#else
		if (ferror(stdin))
#endif
			break;

		clearerr(stdin);
	}

	return ((result == EOF) ? EOF : result & 0xFF);
#else
#  ifdef EINTR
	while ((result = read (0, &ch, 1)) < 0 && errno == EINTR)
		;	/* spin on signal interrupts */
#  else

	result = read (0, &ch, 1);
#  endif
	return((result <= 0 ) ? EOF : ch & 0xFF);
#endif

#endif /* INDEX_DAEMON */
}

#endif	/* M_UNIX */
#endif	/* !VMS */

/*
 *  output a character. From tputs... (Note: this CANNOT be a macro!)
 */

int
outchar (c)
	int c;
{
	return fputc (c, stdout);
}

/*
 *  setup to monitor mouse buttons if running in a xterm
 */

void
xclick (state)
	int state;
{
#ifndef INDEX_DAEMON
	static int prev_state = 999;

	if (xclicks && prev_state != state) {
		if (state == TRUE) {
			tputs (_xclickinit, 1, outchar);
		} else {
			tputs (_xclickend, 1, outchar);
		}
		fflush (stdout);
		prev_state = state;
	}
#endif	/* INDEX_DAEMON */
}

/*
 *  switch on monitoring of mouse buttons
 */

void
set_xclick_on ()
{
	if (use_mouse) xclick (TRUE);
}

/*
 *  switch off monitoring of mouse buttons
 */

void
set_xclick_off ()
{
	if (use_mouse) xclick (FALSE);
}

void
cursoron ()
{
#ifndef INDEX_DAEMON
	if (_cursoron)
		tputs (_cursoron, 1, outchar);
#endif
}

void
cursoroff ()
{
#ifndef INDEX_DAEMON
	if (_cursoroff)
		tputs (_cursoroff, 1, outchar);
#endif
}
