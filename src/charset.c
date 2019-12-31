/*
 *  Project   : tin - a Usenet reader
 *  Module    : charset.c
 *  Author    : M.Kuhn, T.Burmester
 *  Created   : 10-12-93
 *  Updated   : 28-02-94
 *  Notes     : ISO to ascii charset conversion routines
 *  Copyright : (c) Copyright 1991-94 by Markus Kuhn & Tilman Burmester
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

/*
 *  Table for the iso2asc convertion...
 *  iso2asc  by  (unrza3@cd4680fs.rrze.uni-erlangen.de)
 *  included by  (root@aspic.han.de) on 11/04/93
 */

#define	SUB			"?"
#define	ISO_TABLES	7

/* 
 * TABSTOP(x) is the column of the character after the TAB
 * at column x. First column is 0, of course.
 */

#define	TABSTOP(x)	(((x) - ((x)&7)) + 8)

static constext *iso2asc[ISO_TABLES][96] =
{
	{
	" ","!","c",SUB,SUB,"Y","|",SUB,"\"","(c)","a","<<","-","-","(R)","-",
	" ","+/-","2","3","'","u","P",".",",","1","o",">>"," 1/4"," 1/2"," 3/4","?",
	"A","A","A","A","A","A","AE","C","E","E","E","E","I","I","I","I",
	"D","N","O","O","O","O","O","x","O","U","U","U","U","Y","Th","ss",
	"a","a","a","a","a","a","ae","c","e","e","e","e","i","i","i","i",
	"d","n","o","o","o","o","o",":","o","u","u","u","u","y","th","y"
	},
	{
	" ","!","c",SUB,SUB,"Y","|",SUB,"\"","c","a","<","-","-","R","-",
	" ",SUB,"2","3","'","u","P",".",",","1","o",">",SUB,SUB,SUB,"?",
	"A","A","A","A","A","A","A","C","E","E","E","E","I","I","I","I",
	"D","N","O","O","O","O","O","x","O","U","U","U","U","Y","T","s",
	"a","a","a","a","a","a","a","c","e","e","e","e","i","i","i","i",
	"d","n","o","o","o","o","o",":","o","u","u","u","u","y","t","y"
	},
	{
	" ","!","c",SUB,SUB,"Y","|",SUB,"\"","(c)","a","<<","-","-","(R)","-",
	" ","+/-","2","3","'","u","P",".",",","1","o",">>"," 1/4"," 1/2"," 3/4","?",
	"A","A","A","A","Ae","Aa","AE","C","E","E","E","E","I","I","I","I",
	"D","N","O","O","O","O","Oe","x","Oe","U","U","U","Ue","Y","Th","ss",
	"a","a","a","a","ae","aa","ae","c","e","e","e","e","i","i","i","i",
	"d","n","o","o","o","o","oe",":","oe","u","u","u","ue","y","th","ij"
	},
	{
	" ","!","c",SUB,"$","Y","|",SUB,"\"","(c)","a","<<","-","-","(R)","-",
	" ","+/-","2","3","'","u","P",".",",","1","o",">>"," 1/4"," 1/2"," 3/4","?",
	"A","A","A","A","[","]","[","C","E","@","E","E","I","I","I","I",
	"D","N","O","O","O","O","\\","x","\\","U","U","U","^","Y","Th","ss",
	"a","a","a","a","{","}","{","c","e","`","e","e","i","i","i","i",
	"d","n","o","o","o","o","|",":","|","u","u","u","~","y","th","y"
	},
	{
	"[NS]","[!I]","[Ct]","[Pd]","[Cu]","[Ye]","[BB]","[SE]",
	"[':]","[Co]","[-a]","[<<]","[NO]","[--]","[Rg]","['-]",
	"[DG]","[+-]","[2S]","[3S]","['']","[My]","[PI]","[.M]",
	"[',]","[1S]","[-o]","[>>]","[14]","[12]","[34]","[?I]",
	"[A!]","[A']","[A>]","[A?]","[A:]","[AA]","[AE]","[C,]",
	"[E!]","[E']","[E>]","[E:]","[I!]","[I']","[I>]","[I:]",
	"[D-]","[N?]","[O!]","[O']","[O>]","[O?]","[O:]","[*X]",
	"[O/]","[U!]","[U']","[U>]","[U:]","[Y']","[TH]","[ss]",
	"[a!]","[a']","[a>]","[a?]","[a:]","[aa]","[ae]","[c,]",
	"[e!]","[e']","[e>]","[e:]","[i!]","[i']","[i>]","[i:]",
	"[d-]","[n?]","[o!]","[o']","[o>]","[o?]","[o:]","[-:]",
	"[o/]","[u!]","[u']","[u>]","[u:]","[y']","[th]","[y:]"
	},
	{
	" ","!","c\b|","L\b-","o\bX","Y\b=","|",SUB,
	"\"","(c)","a\b_","<<","-\b,","-","(R)","-",
	" ","+\b_","2","3","'","u","P",".",
	",","1","o\b_",">>"," 1/4"," 1/2"," 3/4","?",
	"A\b`","A\b'","A\b^","A\b~","A\b\"","Aa","AE","C\b,",
	"E\b`","E\b'","E\b^","E\b\"","I\b`","I\b'","I\b^","I\b\"",
	"D\b-","N\b~","O\b`","O\b'","O\b^","O\b~","O\b\"","x",
	"O\b/","U\b`","U\b'","U\b^","U\b\"","Y\b'","Th","ss",
	"a\b`","a\b'","a\b^","a\b~","a\b\"","aa","ae","c\b,",
	"e\b`","e\b'","e\b^","e\b\"","i\b`","i\b'","i\b^","i\b\"",
	"d\b-","n\b~","o\b`","o\b'","o\b^","o\b~","o\b\"","-\b:",
	"o\b/","u\b`","u\b'","u\b^","u\b\"","y\b'","th","y\b\""
	},
	{
	"\377","\255","\233","\234",SUB,"\235","|","\25",
	"\"","(c)","\246","\256","\252","-","(R)","-",
	"\370","\361","\375","3","'","\346","\24","\371",
	",","1","\247","\257","\254","\253"," 3/4","\250",
	"A","A","A","A","\216","\217","\222","\200",
	"E","\220","E","E","I","I","I","I",
	"D","\245","O","O","O","O","\231","x",
	"\355","U","U","U","\232","Y","T","\341",
	"\205","\240","\203","a","\204","\206","\221","\207",
	"\212","\202","\210","\211","\215","\241","\214","\213",
	"d","\244","\225","\242","\223","o","\224","\366",
	"\355","\227","\243","\226","\201","y","t","\230"
	}
};

/* 
 * german tex style to latin1 conversion (by root@aspic, 12/04/93) 
 */
 
#define	TEX_SUBST	15
#define	SPACES		"                                                                                                         "

static char *tex_from[TEX_SUBST] = 
{
	"\"a","\\\"a","\"o","\\\"o","\"u","\\\"u","\"A","\\\"A","\"O","\\\"O","\"U","\\\"U","\"s","\\\"s","\\3"
};
static char *tex_to[TEX_SUBST] = 
{
	"�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�", "�"
};
        
/*
 *  Now the convertion function...
 */

void
ConvertIso2Asc (iso, asc, t)
	unsigned char *iso;
	unsigned char *asc;
	int t;
{
	char *p, **tab;
	int first;   /* flag for first SPACE/TAB after other characters */
	int i, a;    /* column counters in iso and asc */
  
	if (iso == (unsigned char *) 0 || 
	    asc == (unsigned char *) 0) {
		return;
	}
  
	tab = (char **) (iso2asc[t] - 0xa0);
	first = 1;
	i = a = 0;
	while (*iso) {
		if (*iso > 0x9f) {
			p = tab[*(iso++)]; i++;
			first = 1;
			while (*p) { 
				*(asc++) = *(p++); 
				a++; 
			}
		} else {
			if (a > i && ((*iso == ' ') || (*iso == '\t'))) {
				/* 
				 * spaces or TABS should be removed 
				 */
				if (*iso == ' ') {
	  				/* 
	  				 * only the first space after a letter must not be removed 
	  				 */
					if (first) { 
						*(asc++) = ' '; 
						a++; 
						first = 0; 
					}
					i++;
				} else {   /* here: *iso == '\t' */
					if (a >= TABSTOP(i)) {
						/* 
						 * remove TAB or replace it with SPACE if necessary 
						 */
						if (first) { 
							*(asc++) = ' '; 
							a++; 
							first = 0; 
						}
					} else {
						/* 
						 * TAB will correct the column difference 
						 */
						*(asc++) = '\t';   /* = *iso */
						a = TABSTOP(a);    /* = TABSTOP(i), because i < a < TABSTOP(i) */
					}
					i = TABSTOP(i);
				}
				iso++;
			} else {
				/* 
				 * just copy the characters and advance the column counters 
				 */
				if (*iso == '\t') {
					a = i = TABSTOP(i);  /* = TABSTOP(a), because here a = i */
				} else if (*iso == '\b') {
					a--; 
					i--;
				} else {
					a++; 
					i++;
				}
				*(asc++) = *(iso++);
				first = 1;
			}
		}
	}
	*asc = 0;
   
	return;
}
 

void
ConvertTeX2Iso (from, to)
	unsigned char *from;
	unsigned char *to;
{
	size_t len, col, spaces;  /* length of from, col counter, spaces to add */
	size_t subst_len;
	int i, ex;
 
	*to = '\0';
	len = strlen ((char *)from);
	col = 0;
 
	spaces = 0;
	while (col < len) {
		i = subst_len = ex = 0;
		while ((i < TEX_SUBST) && !ex) {
			subst_len = strlen (tex_from[i]);
			if (! strncmp ((char *)from + col, tex_from[i], subst_len)) {
				strcat ((char *)to, tex_to[i]);
				spaces += subst_len - 1;
				col += subst_len - 1;
				ex = 1;
			}
			i++;
		}
		if (! ex)
			strncat ((char *)to, (char *)from + col, 1);
		if (from[col] == ' ') {
			strncat ((char *)to, SPACES, spaces);
			spaces = 0;
		}
 
		col++;
	}
}

/* 
 * Check for german TeX encoding 
 */
 
int
iIsArtTexEncoded (art, group_path)
	long art;
	unsigned char *group_path;
{
	char line[LEN];
	FILE *fp;
	int body = FALSE;
	int i, len;

	fp = open_art_fp (group_path, art);

	while (fgets (line, sizeof line, fp) != (char *) 0) {
		if (line[0] == '\n') {
			body = TRUE;
		}
		if (! body) {
			continue;
		}

		i = 0;
		while (line[i++] == ' ')
			;  /* search for first non blank */
		i--;
		if (! isalnum(line[i])) {
			continue;  /*quoting char */
		}

		len = strlen (line) - 1;
		for (i = 1; i < len; i++) {
			if (((line[i] == '\\') || (line[i] == '\"')) &&
			    (isalnum(line[i-1])) && (isalnum(line[i+1]))) {
				art_close ();
				return TRUE;
			}
		}
	}
	art_close ();

	return FALSE;
}
