#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define YYPREFIX "yy"
#line 2 "parsdate.y"
/*
 *  Project   : tin - a Usenet reader
 *  Module    : parsedate.y
 *  Author    : S. Bellovin, R. $alz, J. Berets, P. Eggert
 *  Created   : 1990-08-01
 *  Updated   : 1994-09-23
 *  Notes     : This grammar has 6 shift/reduce conflicts.
 *              Originally written by Steven M. Bellovin <smb@research.att.com>
 *              while at the University of North Carolina at Chapel Hill.
 *              Later tweaked by a couple of people on Usenet.  Completely
 *              overhauled by Rich $alz <rsalz@osf.org> and Jim Berets
 *              <jberets@bbn.com> in August, 1990.
 *              Further revised (removed obsolete constructs and cleaned up
 *              timezone names) in August, 1991, by Rich.
 *              Paul Eggert <eggert@twinsun.com> helped in September 1992.
 *              Roland Rosenfeld added MET DST code in April 1994.
 *  Revision  : 1.13
 *  Copyright : This code is in the public domain and has no copyright.
 */

/* SUPPRESS 530 *//* Empty body for statement */
/* SUPPRESS 593 on yyerrlab *//* Label was not used */
/* SUPPRESS 593 on yynewstate *//* Label was not used */
/* SUPPRESS 595 on yypvt *//* Automatic variable may be used before set */

#include "tin.h"

/*
**  Get the number of elements in a fixed-size array, or a pointer just
**  past the end of it.
*/
#define ENDOF(array)	(&array[SIZEOF(array)])

#define CTYPE(isXXXXX, c) (((unsigned char)(c) < 128) && isXXXXX((c)))

typedef char	*STRING;

#define yyparse		date_parse
#define yylex		date_lex
#define yyerror		date_error


    /* See the LeapYears table in Convert. */
#define EPOCH		1970
#define END_OF_TIME	2038

    /* Constants for general time calculations. */
#define DST_OFFSET	1
#define SECSPERDAY	(24L * 60L * 60L)
    /* Readability for TABLE stuff. */
#define HOUR(x)		(x * 60)

#define LPAREN		'('
#define RPAREN		')'
#define IS7BIT(x)	((unsigned int)(x) < 0200)


/*
**  Daylight-savings mode:  on, off, or not yet known.
*/
typedef enum _DSTMODE {
    DSTon, DSToff, DSTmaybe
} DSTMODE;

/*
**  Meridian:  am, pm, or 24-hour style.
*/
typedef enum _MERIDIAN {
    MERam, MERpm, MER24
} MERIDIAN;


/*
**  Global variables.  We could get rid of most of them by using a yacc
**  union, but this is more efficient.  (This routine predates the
**  yacc %union construct.)
*/
static char	*yyInput;
static DSTMODE	yyDSTmode;
static int	yyHaveDate;
static int	yyHaveRel;
static int	yyHaveTime;
static time_t	yyTimezone;
static time_t	yyDay;
static time_t	yyHour;
static time_t	yyMinutes;
static time_t	yyMonth;
static time_t	yySeconds;
static time_t	yyYear;
static MERIDIAN	yyMeridian;
static time_t	yyRelMonth;
static time_t	yyRelSeconds;

static time_t	ToSeconds(time_t, time_t, time_t, MERIDIAN);
static time_t	Convert(time_t, time_t, time_t, time_t, time_t, time_t, MERIDIAN, DSTMODE);
static time_t	DSTcorrect(time_t, time_t);
static time_t	RelativeMonth(time_t, time_t);
static int	LookupWord(char	*, int);
static int	date_lex(void);
static int	GetTimeInfo(TIMEINFO *Now);

/*
 * The 'date_error()' function is declared here to work around a defect in
 * bison 1.22, which redefines 'const' further down in this file, making it
 * impossible to put a prototype here, and the function later.  We're using
 * 'const' on the parameter to quiet gcc's -Wwrite-strings warning.
 */
/*ARGSUSED*/
static void
date_error(const char *s)
{
    /*NOTREACHED*/
}

#line 118 "parsdate.y"
typedef union {
    time_t		Number;
    enum _MERIDIAN	Meridian;
} YYSTYPE;
#line 132 "y.tab.c"
#define tDAY 257
#define tDAYZONE 258
#define tMERIDIAN 259
#define tMONTH 260
#define tMONTH_UNIT 261
#define tSEC_UNIT 262
#define tSNUMBER 263
#define tUNUMBER 264
#define tZONE 265
#define tDST 266
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    4,    4,    4,    4,    5,    5,    5,    5,
    5,    2,    2,    2,    2,    2,    1,    6,    6,    6,
    6,    6,    6,    6,    7,    7,    7,    7,    3,    3,
};
short yylen[] = {                                         2,
    0,    2,    1,    2,    1,    1,    2,    4,    4,    6,
    6,    1,    1,    2,    2,    1,    1,    3,    5,    2,
    4,    2,    3,    5,    2,    2,    2,    2,    0,    1,
};
short yydefred[] = {                                      1,
    0,    0,    0,    0,    0,    2,    0,    5,    6,    0,
    0,   27,   25,   30,    0,   28,   26,    0,    0,    7,
    0,   17,    0,   16,    4,    0,    0,   23,    0,    0,
   14,   15,    0,   21,    0,    9,    8,    0,   24,    0,
   19,   11,   10,
};
short yydgoto[] = {                                       1,
   24,   25,   20,    6,    7,    8,    9,
};
short yysindex[] = {                                      0,
 -230,  -35, -247, -255,  -47,    0, -250,    0,    0, -246,
  -25,    0,    0,    0, -244,    0,    0, -238, -236,    0,
 -235,    0, -228,    0,    0, -224, -227,    0,  -56,   -9,
    0,    0, -225,    0, -223,    0,    0, -222,    0, -234,
    0,    0,    0,
};
short yyrindex[] = {                                      0,
    0,    0,    0,    0,    1,    0,   23,    0,    0,    0,
   12,    0,    0,    0,   32,    0,    0,    0,    0,    0,
   14,    0,   24,    0,    0,    0,    0,    0,    3,   22,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    3,
    0,    0,    0,
};
short yygindex[] = {                                      0,
  -19,    0,  -24,    0,    0,    0,    0,
};
#define YYTABLESIZE 295
short yytable[] = {                                      19,
   29,   35,   29,   32,   37,   12,   13,   21,   10,   36,
   18,   20,   22,   13,   23,   43,   11,   26,   27,   28,
   42,   18,    3,   12,   14,   29,    2,   30,   22,    3,
   31,   22,    4,    5,   22,   33,   34,   38,   39,    0,
   40,   41,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   14,    0,    0,    0,   22,    0,    0,    0,
    0,   14,   15,   16,   17,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   29,   29,   29,
   29,    0,   29,   29,   29,   29,   29,   29,   20,    0,
   13,   20,    0,   13,   20,   20,   13,   13,   18,    3,
   12,   18,    3,   12,   18,   18,    3,   12,   22,    0,
    0,   22,    0,    0,   22,
};
short yycheck[] = {                                      47,
    0,   58,    0,   23,   29,  261,  262,  258,   44,   29,
   58,    0,  263,    0,  265,   40,  264,  264,   44,  264,
   40,    0,    0,    0,  259,  264,  257,  264,  263,  260,
  266,    0,  263,  264,  263,  260,  264,   47,  264,   -1,
  264,  264,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  259,   -1,   -1,   -1,  263,   -1,   -1,   -1,
   -1,  259,  260,  261,  262,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,  257,
  258,   -1,  260,  263,  264,  265,  264,  265,  257,   -1,
  257,  260,   -1,  260,  263,  264,  263,  264,  257,  257,
  257,  260,  260,  260,  263,  264,  264,  264,  257,   -1,
   -1,  260,   -1,   -1,  263,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 266
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,"','",0,0,"'/'",0,0,0,0,0,0,0,0,0,0,"':'",0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"tDAY","tDAYZONE",
"tMERIDIAN","tMONTH","tMONTH_UNIT","tSEC_UNIT","tSNUMBER","tUNUMBER","tZONE",
"tDST",
};
char *yyrule[] = {
"$accept : spec",
"spec :",
"spec : spec item",
"item : time",
"item : time zone",
"item : date",
"item : rel",
"time : tUNUMBER o_merid",
"time : tUNUMBER ':' tUNUMBER o_merid",
"time : tUNUMBER ':' tUNUMBER numzone",
"time : tUNUMBER ':' tUNUMBER ':' tUNUMBER o_merid",
"time : tUNUMBER ':' tUNUMBER ':' tUNUMBER numzone",
"zone : tZONE",
"zone : tDAYZONE",
"zone : tDAYZONE tDST",
"zone : tZONE numzone",
"zone : numzone",
"numzone : tSNUMBER",
"date : tUNUMBER '/' tUNUMBER",
"date : tUNUMBER '/' tUNUMBER '/' tUNUMBER",
"date : tMONTH tUNUMBER",
"date : tMONTH tUNUMBER ',' tUNUMBER",
"date : tUNUMBER tMONTH",
"date : tUNUMBER tMONTH tUNUMBER",
"date : tDAY ',' tUNUMBER tMONTH tUNUMBER",
"rel : tSNUMBER tSEC_UNIT",
"rel : tUNUMBER tSEC_UNIT",
"rel : tSNUMBER tMONTH_UNIT",
"rel : tUNUMBER tMONTH_UNIT",
"o_merid :",
"o_merid : tMERIDIAN",
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 309 "parsdate.y"

/*
**  An entry in the lexical lookup table.
*/
typedef struct _TABLE {
    const char *name;
    int		type;
    time_t	value;
} TABLE;

/* Month and day table. */
static const TABLE MonthDayTable[] = {
    { "january",	tMONTH,  1 },
    { "february",	tMONTH,  2 },
    { "march",		tMONTH,  3 },
    { "april",		tMONTH,  4 },
    { "may",		tMONTH,  5 },
    { "june",		tMONTH,  6 },
    { "july",		tMONTH,  7 },
    { "august",		tMONTH,  8 },
    { "september",	tMONTH,  9 },
    { "october",	tMONTH, 10 },
    { "november",	tMONTH, 11 },
    { "december",	tMONTH, 12 },
	/* The value of the day isn't used... */
    { "sunday",		tDAY, 0 },
    { "monday",		tDAY, 0 },
    { "tuesday",	tDAY, 0 },
    { "wednesday",	tDAY, 0 },
    { "thursday",	tDAY, 0 },
    { "friday",		tDAY, 0 },
    { "saturday",	tDAY, 0 },
};

/* Time units table. */
static const TABLE	UnitsTable[] = {
    { "year",		tMONTH_UNIT,	12 },
    { "month",		tMONTH_UNIT,	1 },
    { "week",		tSEC_UNIT,	7 * 24 * 60 * 60 },
    { "day",		tSEC_UNIT,	1 * 24 * 60 * 60 },
    { "hour",		tSEC_UNIT,	60 * 60 },
    { "minute",		tSEC_UNIT,	60 },
    { "min",		tSEC_UNIT,	60 },
    { "second",		tSEC_UNIT,	1 },
    { "sec",		tSEC_UNIT,	1 },
};

/* Timezone table. */
static const TABLE	TimezoneTable[] = {
    { "gmt",	tZONE,     HOUR( 0) },	/* Greenwich Mean */
    { "ut",	tZONE,     HOUR( 0) },	/* Universal */
    { "utc",	tZONE,     HOUR( 0) },	/* Universal Coordinated */
    { "cut",	tZONE,     HOUR( 0) },	/* Coordinated Universal */
    { "z",	tZONE,     HOUR( 0) },	/* Greenwich Mean */
    { "wet",	tZONE,     HOUR( 0) },	/* Western European */
    { "bst",	tDAYZONE,  HOUR( 0) },	/* British Summer */
    { "nst",	tZONE,     HOUR(3)+30 }, /* Newfoundland Standard */
    { "ndt",	tDAYZONE,  HOUR(3)+30 }, /* Newfoundland Daylight */
    { "ast",	tZONE,     HOUR( 4) },	/* Atlantic Standard */
    { "adt",	tDAYZONE,  HOUR( 4) },	/* Atlantic Daylight */
    { "est",	tZONE,     HOUR( 5) },	/* Eastern Standard */
    { "edt",	tDAYZONE,  HOUR( 5) },	/* Eastern Daylight */
    { "cst",	tZONE,     HOUR( 6) },	/* Central Standard */
    { "cdt",	tDAYZONE,  HOUR( 6) },	/* Central Daylight */
    { "mst",	tZONE,     HOUR( 7) },	/* Mountain Standard */
    { "mdt",	tDAYZONE,  HOUR( 7) },	/* Mountain Daylight */
    { "pst",	tZONE,     HOUR( 8) },	/* Pacific Standard */
    { "pdt",	tDAYZONE,  HOUR( 8) },	/* Pacific Daylight */
    { "yst",	tZONE,     HOUR( 9) },	/* Yukon Standard */
    { "ydt",	tDAYZONE,  HOUR( 9) },	/* Yukon Daylight */
    { "akst",	tZONE,     HOUR( 9) },	/* Alaska Standard */
    { "akdt",	tDAYZONE,  HOUR( 9) },	/* Alaska Daylight */
    { "hst",	tZONE,     HOUR(10) },	/* Hawaii Standard */
    { "hast",	tZONE,     HOUR(10) },	/* Hawaii-Aleutian Standard */
    { "hadt",	tDAYZONE,  HOUR(10) },	/* Hawaii-Aleutian Daylight */
    { "ces",	tDAYZONE,  -HOUR(1) },	/* Central European Summer */
    { "cest",	tDAYZONE,  -HOUR(1) },	/* Central European Summer */
    { "mez",	tZONE,     -HOUR(1) },	/* Middle European */
    { "mezt",	tDAYZONE,  -HOUR(1) },	/* Middle European Summer */
    { "cet",	tZONE,     -HOUR(1) },	/* Central European */
    { "met",	tZONE,     -HOUR(1) },	/* Middle European */
/* Additional aliases for MET / MET DST *************************************/
    { "mez",    tZONE,     -HOUR(1) },  /* Middle European */
    { "mewt",   tZONE,     -HOUR(1) },  /* Middle European Winter */
    { "mest",   tDAYZONE,  -HOUR(1) },  /* Middle European Summer */
    { "mes",    tDAYZONE,  -HOUR(1) },  /* Middle European Summer */
    { "mesz",   tDAYZONE,  -HOUR(1) },  /* Middle European Summer */
    { "msz",    tDAYZONE,  -HOUR(1) },  /* Middle European Summer */
    { "metdst", tDAYZONE,  -HOUR(1) },  /* Middle European Summer */
/****************************************************************************/
    { "eet",	tZONE,     -HOUR(2) },	/* Eastern Europe */
    { "msk",	tZONE,     -HOUR(3) },	/* Moscow Winter */
    { "msd",	tDAYZONE,  -HOUR(3) },	/* Moscow Summer */
    { "wast",	tZONE,     -HOUR(8) },	/* West Australian Standard */
    { "wadt",	tDAYZONE,  -HOUR(8) },	/* West Australian Daylight */
    { "hkt",	tZONE,     -HOUR(8) },	/* Hong Kong */
    { "cct",	tZONE,     -HOUR(8) },	/* China Coast */
    { "jst",	tZONE,     -HOUR(9) },	/* Japan Standard */
    { "kst",	tZONE,     -HOUR(9) },	/* Korean Standard */
    { "kdt",	tZONE,     -HOUR(9) },	/* Korean Daylight */
    { "cast",	tZONE,     -(HOUR(9)+30) }, /* Central Australian Standard */
    { "cadt",	tDAYZONE,  -(HOUR(9)+30) }, /* Central Australian Daylight */
    { "east",	tZONE,     -HOUR(10) },	/* Eastern Australian Standard */
    { "eadt",	tDAYZONE,  -HOUR(10) },	/* Eastern Australian Daylight */
    { "nzst",	tZONE,     -HOUR(12) },	/* New Zealand Standard */
    { "nzdt",	tDAYZONE,  -HOUR(12) },	/* New Zealand Daylight */

    /* For completeness we include the following entries. */
#if	0

    /* Duplicate names.  Either they conflict with a zone listed above
     * (which is either more likely to be seen or just been in circulation
     * longer), or they conflict with another zone in this section and
     * we could not reasonably choose one over the other. */
    { "fst",	tZONE,     HOUR( 2) },	/* Fernando De Noronha Standard */
    { "fdt",	tDAYZONE,  HOUR( 2) },	/* Fernando De Noronha Daylight */
    { "bst",	tZONE,     HOUR( 3) },	/* Brazil Standard */
    { "est",	tZONE,     HOUR( 3) },	/* Eastern Standard (Brazil) */
    { "edt",	tDAYZONE,  HOUR( 3) },	/* Eastern Daylight (Brazil) */
    { "wst",	tZONE,     HOUR( 4) },	/* Western Standard (Brazil) */
    { "wdt",	tDAYZONE,  HOUR( 4) },	/* Western Daylight (Brazil) */
    { "cst",	tZONE,     HOUR( 5) },	/* Chile Standard */
    { "cdt",	tDAYZONE,  HOUR( 5) },	/* Chile Daylight */
    { "ast",	tZONE,     HOUR( 5) },	/* Acre Standard */
    { "adt",	tDAYZONE,  HOUR( 5) },	/* Acre Daylight */
    { "cst",	tZONE,     HOUR( 5) },	/* Cuba Standard */
    { "cdt",	tDAYZONE,  HOUR( 5) },	/* Cuba Daylight */
    { "est",	tZONE,     HOUR( 6) },	/* Easter Island Standard */
    { "edt",	tDAYZONE,  HOUR( 6) },	/* Easter Island Daylight */
    { "sst",	tZONE,     HOUR(11) },	/* Samoa Standard */
    { "ist",	tZONE,     -HOUR(2) },	/* Israel Standard */
    { "idt",	tDAYZONE,  -HOUR(2) },	/* Israel Daylight */
    { "idt",	tDAYZONE,  -(HOUR(3)+30) }, /* Iran Daylight */
    { "ist",	tZONE,     -(HOUR(3)+30) }, /* Iran Standard */
    { "cst",	 tZONE,     -HOUR(8) },	/* China Standard */
    { "cdt",	 tDAYZONE,  -HOUR(8) },	/* China Daylight */
    { "sst",	 tZONE,     -HOUR(8) },	/* Singapore Standard */

    /* Dubious (e.g., not in Olson's TIMEZONE package) or obsolete. */
    { "gst",	tZONE,     HOUR( 3) },	/* Greenland Standard */
    { "wat",	tZONE,     -HOUR(1) },	/* West Africa */
    { "at",	tZONE,     HOUR( 2) },	/* Azores */
    { "gst",	tZONE,     -HOUR(10) },	/* Guam Standard */
    { "nft",	tZONE,     HOUR(3)+30 }, /* Newfoundland */
    { "idlw",	tZONE,     HOUR(12) },	/* International Date Line West */
    { "mewt",	tZONE,     -HOUR(1) },	/* Middle European Winter */
    { "mest",	tDAYZONE,  -HOUR(1) },	/* Middle European Summer */
    { "swt",	tZONE,     -HOUR(1) },	/* Swedish Winter */
    { "sst",	tDAYZONE,  -HOUR(1) },	/* Swedish Summer */
    { "fwt",	tZONE,     -HOUR(1) },	/* French Winter */
    { "fst",	tDAYZONE,  -HOUR(1) },	/* French Summer */
    { "bt",	tZONE,     -HOUR(3) },	/* Baghdad */
    { "it",	tZONE,     -(HOUR(3)+30) }, /* Iran */
    { "zp4",	tZONE,     -HOUR(4) },	/* USSR Zone 3 */
    { "zp5",	tZONE,     -HOUR(5) },	/* USSR Zone 4 */
    { "ist",	tZONE,     -(HOUR(5)+30) }, /* Indian Standard */
    { "zp6",	tZONE,     -HOUR(6) },	/* USSR Zone 5 */
    { "nst",	tZONE,     -HOUR(7) },	/* North Sumatra */
    { "sst",	tZONE,     -HOUR(7) },	/* South Sumatra */
    { "jt",	tZONE,     -(HOUR(7)+30) }, /* Java (3pm in Cronusland!) */
    { "nzt",	tZONE,     -HOUR(12) },	/* New Zealand */
    { "idle",	tZONE,     -HOUR(12) },	/* International Date Line East */
    { "cat",	tZONE,     HOUR(10) },	/* -- expired 1967 */
    { "nt",	tZONE,     HOUR(11) },	/* -- expired 1967 */
    { "ahst",	tZONE,     HOUR(10) },	/* -- expired 1983 */
    { "hdt",	tDAYZONE,  HOUR(10) },	/* -- expired 1986 */
#endif	/* 0 */
};

static time_t
ToSeconds(
    time_t	Hours,
    time_t	Minutes,
    time_t	Seconds,
    MERIDIAN	Meridian)
{
    if (Minutes < 0 || Minutes > 59 || Seconds < 0 || Seconds > 61)
	return -1;
    if (Meridian == MER24) {
	if (Hours < 0 || Hours > 23)
	    return -1;
    }
    else {
	if (Hours < 1 || Hours > 12)
		return -1;
	if (Hours == 12)
		Hours = 0;
	if (Meridian == MERpm)
		Hours += 12;
    }
    return (Hours * 60L + Minutes) * 60L + Seconds;
}


static time_t
Convert(
    time_t	Month,
    time_t	Day,
    time_t	Year,
    time_t	Hours,
    time_t	Minutes,
    time_t	Seconds,
    MERIDIAN	Meridian,
    DSTMODE	dst)
{
    static const int	DaysNormal[13] = {
	0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    static const int	DaysLeap[13] = {
	0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    static const int	LeapYears[] = {
	1972, 1976, 1980, 1984, 1988, 1992, 1996,
	2000, 2004, 2008, 2012, 2016, 2020, 2024, 2028, 2032, 2036
    };
    register const int	*yp;
    register const int	*mp;
    register time_t	Julian;
    register int	i;
    time_t		tod;

    if (Year < 0)
	Year = -Year;
    if (Year < 100)
	Year += 1900;
    if (Year < EPOCH)
	Year += 100;
    for (mp = DaysNormal, yp = LeapYears; yp < ENDOF(LeapYears); yp++)
	if (Year == *yp) {
	    mp = DaysLeap;
	    break;
	}
    if (Year < EPOCH || Year > END_OF_TIME
     || Month < 1 || Month > 12
     /* NOSTRICT *//* conversion from long may lose accuracy */
     || Day < 1 || Day > mp[(int)Month])
	return -1;

    Julian = Day - 1 + (Year - EPOCH) * 365;
    for (yp = LeapYears; yp < ENDOF(LeapYears); yp++, Julian++)
	if (Year <= *yp)
	    break;
    for (i = 1; i < Month; i++)
	Julian += *++mp;
    Julian *= SECSPERDAY;
    Julian += yyTimezone * 60L;
    if ((tod = ToSeconds(Hours, Minutes, Seconds, Meridian)) < 0)
	return -1;
    Julian += tod;
    tod = Julian;
    if (dst == DSTon || (dst == DSTmaybe && localtime(&tod)->tm_isdst))
	Julian -= DST_OFFSET * 60 * 60;
    return Julian;
}


static time_t
DSTcorrect(
    time_t	Start,
    time_t	Future)
{
    time_t	StartDay;
    time_t	FutureDay;

    StartDay = (localtime(&Start)->tm_hour + 1) % 24;
    FutureDay = (localtime(&Future)->tm_hour + 1) % 24;
    return (Future - Start) + (StartDay - FutureDay) * DST_OFFSET * 60 * 60;
}


static time_t
RelativeMonth(
    time_t	Start,
    time_t	RelMonth)
{
    struct tm	*tm;
    time_t	Month;
    time_t	Year;

    tm = localtime(&Start);
    Month = 12 * tm->tm_year + tm->tm_mon + RelMonth;
    Year = Month / 12;
    Month = Month % 12 + 1;
    return DSTcorrect(Start,
	    Convert(Month, (time_t)tm->tm_mday, Year,
		(time_t)tm->tm_hour, (time_t)tm->tm_min, (time_t)tm->tm_sec,
		MER24, DSTmaybe));
}


static int
LookupWord(
    char		*buff,
    register int	length)
{
    register char	*p;
    register const char *q;
    register const TABLE *tp;
    register int	c;

    p = buff;
    c = p[0];

    /* See if we have an abbreviation for a month. */
    if (length == 3 || (length == 4 && p[3] == '.'))
	for (tp = MonthDayTable; tp < ENDOF(MonthDayTable); tp++) {
	    q = tp->name;
	    if (c == q[0] && p[1] == q[1] && p[2] == q[2]) {
		yylval.Number = tp->value;
		return tp->type;
	    }
	}
    else
	for (tp = MonthDayTable; tp < ENDOF(MonthDayTable); tp++)
	    if (c == tp->name[0] && strcmp(p, tp->name) == 0) {
		yylval.Number = tp->value;
		return tp->type;
	    }

    /* Try for a timezone. */
    for (tp = TimezoneTable; tp < ENDOF(TimezoneTable); tp++)
	if (c == tp->name[0] && p[1] == tp->name[1]
	 && strcmp(p, tp->name) == 0) {
	    yylval.Number = tp->value;
	    return tp->type;
	}

    if (strcmp(buff, "dst") == 0)
      return tDST;

    /* Try the units table. */
    for (tp = UnitsTable; tp < ENDOF(UnitsTable); tp++)
	if (c == tp->name[0] && strcmp(p, tp->name) == 0) {
	    yylval.Number = tp->value;
	    return tp->type;
	}

    /* Strip off any plural and try the units table again. */
    if (--length > 0 && p[length] == 's') {
	p[length] = '\0';
	for (tp = UnitsTable; tp < ENDOF(UnitsTable); tp++)
	    if (c == tp->name[0] && strcmp(p, tp->name) == 0) {
		p[length] = 's';
		yylval.Number = tp->value;
		return tp->type;
	    }
	p[length] = 's';
    }
    length++;

    /* Drop out any periods. */
    for (p = buff, q = (STRING)buff; *q; q++)
	if (*q != '.')
	    *p++ = *q;
    *p = '\0';

    /* Try the meridians. */
    if (buff[1] == 'm' && buff[2] == '\0') {
	if (buff[0] == 'a') {
	    yylval.Meridian = MERam;
	    return tMERIDIAN;
	}
	if (buff[0] == 'p') {
	    yylval.Meridian = MERpm;
	    return tMERIDIAN;
	}
    }

    /* If we saw any periods, try the timezones again. */
    if (p - buff != length) {
	c = buff[0];
	for (p = buff, tp = TimezoneTable; tp < ENDOF(TimezoneTable); tp++)
	    if (c == tp->name[0] && p[1] == tp->name[1]
	    && strcmp(p, tp->name) == 0) {
		yylval.Number = tp->value;
		return tp->type;
	    }
    }

    /* Unknown word -- assume GMT timezone. */
    yylval.Number = 0;
    return tZONE;
}


static int
date_lex(void)
{
    register char	c;
    register char	*p;
    char		buff[20];
    register int	sign;
    register int	i;
    register int	nesting;

    forever {
	/* Get first character after the whitespace. */
	forever {
	    while (CTYPE(isspace, *yyInput))
		yyInput++;
	    c = *yyInput;

	    /* Ignore RFC 822 comments, typically time zone names. */
	    if (c != LPAREN)
		break;
	    for (nesting = 1; (c = *++yyInput) != RPAREN || --nesting; )
		if (c == LPAREN)
		    nesting++;
		else if (!IS7BIT(c) || c == '\0' || c == '\r'
		     || (c == '\\' && ((c = *++yyInput) == '\0' || !IS7BIT(c))))
		    /* Lexical error: bad comment. */
		    return '?';
	    yyInput++;
	}

	/* A number? */
	if (CTYPE(isdigit, c) || c == '-' || c == '+') {
	    if (c == '-' || c == '+') {
		sign = c == '-' ? -1 : 1;
		yyInput++;
		if (!CTYPE(isdigit, *yyInput))
		    /* Skip the plus or minus sign. */
		    continue;
	    }
	    else
		sign = 0;
	    for (i = 0; (c = *yyInput++) != '\0' && CTYPE(isdigit, c); )
		i = 10 * i + c - '0';
	    yyInput--;
	    yylval.Number = sign < 0 ? -i : i;
	    return sign ? tSNUMBER : tUNUMBER;
	}

	/* A word? */
	if (CTYPE(isalpha, c)) {
	    for (p = buff; (c = *yyInput++) == '.' || CTYPE(isalpha, c); )
		if (p < &buff[sizeof buff - 1])
		    *p++ = CTYPE(isupper, c) ? tolower(c) : c;
	    *p = '\0';
	    yyInput--;
	    return LookupWord(buff, p - buff);
	}

	return *yyInput++;
    }
}


static int
GetTimeInfo(
    TIMEINFO		*Now)
{
    static time_t	LastTime;
    static long		LastTzone;
    struct tm		*tm;
#if	defined(HAVE_GETTIMEOFDAY)
    struct timeval	tv;
#endif	/* defined(HAVE_GETTIMEOFDAY) */
#if	defined(DONT_HAVE_TM_GMTOFF)
    struct tm		local;
    struct tm		gmt;
#endif	/* !defined(DONT_HAVE_TM_GMTOFF) */

    /* Get the basic time. */
#if	defined(HAVE_GETTIMEOFDAY)
    if (gettimeofday(&tv, (struct timezone *)NULL) == -1)
	return -1;
    Now->time = tv.tv_sec;
    Now->usec = tv.tv_usec;
#else
    /* Can't check for -1 since that might be a time, I guess. */
    (void)time(&Now->time);
    Now->usec = 0;
#endif	/* defined(HAVE_GETTIMEOFDAY) */

    /* Now get the timezone if it's been an hour since the last time. */
    if (Now->time - LastTime > 60 * 60) {
	LastTime = Now->time;
	if ((tm = localtime(&Now->time)) == NULL)
	    return -1;
#if	defined(DONT_HAVE_TM_GMTOFF)
	/* To get the timezone, compare localtime with GMT. */
	local = *tm;
	if ((tm = gmtime(&Now->time)) == NULL)
	    return -1;
	gmt = *tm;

	/* Assume we are never more than 24 hours away. */
	LastTzone = gmt.tm_yday - local.tm_yday;
	if (LastTzone > 1)
	    LastTzone = -24;
	else if (LastTzone < -1)
	    LastTzone = 24;
	else
	    LastTzone *= 24;

	/* Scale in the hours and minutes; ignore seconds. */
	LastTzone += gmt.tm_hour - local.tm_hour;
	LastTzone *= 60;
	LastTzone += gmt.tm_min - local.tm_min;
#else
	LastTzone =  (0 - tm->tm_gmtoff) / 60;
#endif	/* defined(DONT_HAVE_TM_GMTOFF) */
    }
    Now->tzone = LastTzone;
    return 0;
}


time_t
parsedate(
    char		*p,
    TIMEINFO		*now)
{
    struct tm		*tm;
    TIMEINFO		ti;
    time_t		Start;

    yyInput = p;
    if (now == NULL) {
	now = &ti;
	(void)GetTimeInfo(&ti);
    }

    tm = localtime(&now->time);
    yyYear = tm->tm_year;
    yyMonth = tm->tm_mon + 1;
    yyDay = tm->tm_mday;
    yyTimezone = now->tzone;
    if(tm->tm_isdst)                   /* Correct timezone offset for DST */
      yyTimezone += DST_OFFSET * 60;
    yyDSTmode = DSTmaybe;
    yyHour = 0;
    yyMinutes = 0;
    yySeconds = 0;
    yyMeridian = MER24;
    yyRelSeconds = 0;
    yyRelMonth = 0;
    yyHaveDate = 0;
    yyHaveRel = 0;
    yyHaveTime = 0;

    if (date_parse() || yyHaveTime > 1 || yyHaveDate > 1)
	return -1;

    if (yyHaveDate || yyHaveTime) {
	Start = Convert(yyMonth, yyDay, yyYear, yyHour, yyMinutes, yySeconds,
		    yyMeridian, yyDSTmode);
	if (Start < 0)
	    return -1;
    }
    else {
	Start = now->time;
	if (!yyHaveRel)
	    Start -= (tm->tm_hour * 60L + tm->tm_min) * 60L + tm->tm_sec;
    }

    Start += yyRelSeconds;
    if (yyRelMonth)
	Start += RelativeMonth(Start, yyRelMonth);

    /* Have to do *something* with a legitimate -1 so it's distinguishable
     * from the error return value.  (Alternately could set errno on error.) */
    return Start == -1 ? 0 : Start;
}
#line 885 "y.tab.c"
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse(void)
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 3:
#line 136 "parsdate.y"
{
	    yyHaveTime++;
#if	defined(lint)
	    /* I am compulsive about lint natterings... */
	    if (yyHaveTime == -1) {
		YYERROR;
	    }
#endif	/* defined(lint) */
	}
break;
case 4:
#line 145 "parsdate.y"
{
	    yyHaveTime++;
	    yyTimezone = yyvsp[0].Number;
	}
break;
case 5:
#line 149 "parsdate.y"
{
	    yyHaveDate++;
	}
break;
case 6:
#line 152 "parsdate.y"
{
	    yyHaveRel = 1;
	}
break;
case 7:
#line 157 "parsdate.y"
{
	    if (yyvsp[-1].Number < 100) {
		yyHour = yyvsp[-1].Number;
		yyMinutes = 0;
	    }
	    else {
		yyHour = yyvsp[-1].Number / 100;
		yyMinutes = yyvsp[-1].Number % 100;
	    }
	    yySeconds = 0;
	    yyMeridian = yyvsp[0].Meridian;
	}
break;
case 8:
#line 169 "parsdate.y"
{
	    yyHour = yyvsp[-3].Number;
	    yyMinutes = yyvsp[-1].Number;
	    yySeconds = 0;
	    yyMeridian = yyvsp[0].Meridian;
	}
break;
case 9:
#line 175 "parsdate.y"
{
	    yyHour = yyvsp[-3].Number;
	    yyMinutes = yyvsp[-1].Number;
	    yyTimezone = yyvsp[0].Number;
	    yyMeridian = MER24;
	    yyDSTmode = DSToff;
	}
break;
case 10:
#line 182 "parsdate.y"
{
	    yyHour = yyvsp[-5].Number;
	    yyMinutes = yyvsp[-3].Number;
	    yySeconds = yyvsp[-1].Number;
	    yyMeridian = yyvsp[0].Meridian;
	}
break;
case 11:
#line 188 "parsdate.y"
{
	    yyHour = yyvsp[-5].Number;
	    yyMinutes = yyvsp[-3].Number;
	    yySeconds = yyvsp[-1].Number;
	    yyTimezone = yyvsp[0].Number;
	    yyMeridian = MER24;
	    yyDSTmode = DSToff;
	}
break;
case 12:
#line 198 "parsdate.y"
{
	    yyval.Number = yyvsp[0].Number;
	    yyDSTmode = DSToff;
	}
break;
case 13:
#line 202 "parsdate.y"
{
	    yyval.Number = yyvsp[0].Number;
	    yyDSTmode = DSTon;
	}
break;
case 14:
#line 206 "parsdate.y"
{
	    yyTimezone = yyvsp[-1].Number;
	    yyDSTmode = DSTon;
	}
break;
case 15:
#line 210 "parsdate.y"
{
	    /* Only allow "GMT+300" and "GMT-0800" */
	    if (yyvsp[-1].Number != 0) {
		YYABORT;
	    }
	    yyval.Number = yyvsp[0].Number;
	    yyDSTmode = DSToff;
	}
break;
case 16:
#line 218 "parsdate.y"
{
	    yyval.Number = yyvsp[0].Number;
	    yyDSTmode = DSToff;
	}
break;
case 17:
#line 224 "parsdate.y"
{
	    int	i;

	    /* Unix and GMT and numeric timezones -- a little confusing. */
	    if ((int)yyvsp[0].Number < 0) {
		/* Don't work with negative modulus. */
		yyvsp[0].Number = -(int)yyvsp[0].Number;
		if (yyvsp[0].Number > 9999 || (i = yyvsp[0].Number % 100) >= 60) {
			YYABORT;
		}
		yyval.Number = (yyvsp[0].Number / 100) * 60 + i;
	    }
	    else {
		if (yyvsp[0].Number > 9999 || (i = yyvsp[0].Number % 100) >= 60) {
			YYABORT;
		}
		yyval.Number = -((yyvsp[0].Number / 100) * 60 + i);
	    }
	}
break;
case 18:
#line 245 "parsdate.y"
{
	    yyMonth = yyvsp[-2].Number;
	    yyDay = yyvsp[0].Number;
	}
break;
case 19:
#line 249 "parsdate.y"
{
	    if (yyvsp[-4].Number > 100) {
		yyYear = yyvsp[-4].Number;
		yyMonth = yyvsp[-2].Number;
		yyDay = yyvsp[0].Number;
	    }
	    else {
		yyMonth = yyvsp[-4].Number;
		yyDay = yyvsp[-2].Number;
		yyYear = yyvsp[0].Number;
	    }
	}
break;
case 20:
#line 261 "parsdate.y"
{
	    yyMonth = yyvsp[-1].Number;
	    yyDay = yyvsp[0].Number;
	}
break;
case 21:
#line 265 "parsdate.y"
{
	    yyMonth = yyvsp[-3].Number;
	    yyDay = yyvsp[-2].Number;
	    yyYear = yyvsp[0].Number;
	}
break;
case 22:
#line 270 "parsdate.y"
{
	    yyDay = yyvsp[-1].Number;
	    yyMonth = yyvsp[0].Number;
	}
break;
case 23:
#line 274 "parsdate.y"
{
	    yyDay = yyvsp[-2].Number;
	    yyMonth = yyvsp[-1].Number;
	    yyYear = yyvsp[0].Number;
	}
break;
case 24:
#line 279 "parsdate.y"
{
	    yyDay = yyvsp[-2].Number;
	    yyMonth = yyvsp[-1].Number;
	    yyYear = yyvsp[0].Number;
	}
break;
case 25:
#line 286 "parsdate.y"
{
	    yyRelSeconds += yyvsp[-1].Number * yyvsp[0].Number;
	}
break;
case 26:
#line 289 "parsdate.y"
{
	    yyRelSeconds += yyvsp[-1].Number * yyvsp[0].Number;
	}
break;
case 27:
#line 292 "parsdate.y"
{
	    yyRelMonth += yyvsp[-1].Number * yyvsp[0].Number;
	}
break;
case 28:
#line 295 "parsdate.y"
{
	    yyRelMonth += yyvsp[-1].Number * yyvsp[0].Number;
	}
break;
case 29:
#line 300 "parsdate.y"
{
	    yyval.Meridian = MER24;
	}
break;
case 30:
#line 303 "parsdate.y"
{
	    yyval.Meridian = yyvsp[0].Meridian;
	}
break;
#line 1268 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
