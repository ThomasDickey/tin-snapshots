/*
 *  Project   : tin - a Usenet reader
 *  Module    : rfc1522.c
 *  Author    : Chris Blum <chris@phil.uni-sb.de>
 *  Created   : September '95
 *  Updated   : 08-06-96
 *  Notes     : MIME header encoding/decoding stuff
 *  Copyright : (c) Copyright 1995-96 by Chris Blum
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made.
 */

#undef MIME_BASE64_ALLOWED	/*
				 * allow base64 encoding in headers if the
				 * result is shorter than quoted printable
				 * encoding. THIS IS NOT YET IMPLEMENTED,
				 * so leave this off
				 */

#ifndef RFCDEBUG
#include "tin.h"
#else
#include "extern.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#if HAVE_MALLOC_H
#include <malloc.h>
#endif
#undef MIME_BREAK_LONG_LINES	/*
				 * define this to make TIN strictly observe
				 * RFC1522 and break lots of other software
				 */
#define MIME_STRICT_CHARSET	/*
				 * define this to force MM_CHARSET obeyance
				 * when decoding.  If you don't, everything
				 * is thought to match your machine's charset
				 */
#endif /* RFCDEBUG */

/* NOTE: these routines expect that MM_CHARSET is set to the charset
   your system is using.  If it is not defined, US-ASCII is used.
   Can be overridden by setting MM_CHARSET as environment variable. */

#ifndef MM_CHARSET
#define MM_CHARSET "US-ASCII"
#endif

char mm_charset[128] = "";
const char base64_alphabet[64] = {
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
  'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
  'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
  'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/' };
static unsigned char base64_rank[256];
static int base64_rank_table_built;
static int quoteflag;

void
build_base64_rank_table()
{
	int i;

	if (base64_rank_table_built) return;
	for (i=0; i<256; i++) base64_rank[i]=255;
	for (i=0; i<64; i++) base64_rank[(int)base64_alphabet[i]]=i;
	base64_rank_table_built=1;
}


unsigned char 
hex2bin(x)
	char x;
{
	if (x>='0'&&x<='9') return x-'0';
	if (x>='A'&&x<='F') return x-'A'+10;
	if (x>='a'&&x<='f') return x-'a'+10;
	return 255;
}


int
mmdecode(what, encoding, delimiter, where, charset)
	char *what;
	char encoding;
	char delimiter;
	char *where;
	char *charset;
{
	char *t;
	int decode_gt128 = 0;

#ifdef MIME_STRICT_CHARSET
	if (charset && !strcasecmp(charset,mm_charset)) decode_gt128=1;
#else
	decode_gt128=1;
#endif
	t=where;
	encoding=tolower(encoding);
	if (encoding=='q') {  /* quoted-printable */
		unsigned char x;
		unsigned char hi,lo;
		
		while (*what!=delimiter) {
			if (*what!='=') {
				if (!delimiter || *what!='_')	*t++=*what++;
				else *t++=' ',what++;
				continue;
			}
			what++;
			if (*what==delimiter) return -1; /* failed */
			x=(unsigned char)(*what++);
			if (x=='\n') continue;
			if (*what==delimiter) return -1;
			hi=hex2bin(x);
			lo=hex2bin(*what);
			what++;
			if (hi==255||lo==255) return -1;
			x=(hi<<4)+lo;
			if (x>=128 && !decode_gt128) x='?';
			*t++=x;
		}
		return t-where;
	}
	else if (encoding=='b') { /* base64 */
		static unsigned short pattern=0;
		static int bits=0;
		unsigned char x;

		build_base64_rank_table();
		if (!what || !where) { /* flush */
			pattern=bits=0;
			return 0;
		}
		while (*what!=delimiter) {
			x=base64_rank[(int)(*what++)];
			if (x==255) continue; /* ignore everything not in the alphabet, including '=' */
			pattern<<=6;
			pattern|=x;
			bits+=6;
			if (bits>=8) {
				x=(pattern>>(bits-8))&0xff;
				if (x>=128 && !decode_gt128) x='?';
				*t++=x;
				bits-=8;
			}
		}
		return t-where;
	}
	return -1;
}


void
get_mm_charset()
{
	char *c;

	if (!mm_charset[0]) {
		c=getenv("MM_CHARSET");
		if (!c) strcpy(mm_charset,MM_CHARSET);
		else {
			strncpy(mm_charset,c,128);
			mm_charset[127]='\0';
		}
	}
}


char *
rfc1522_decode(s)
	char *s;
{
	char *c, *d, *t;
	static char buffer[2048];
	char charset[256];
	char encoding;
	char adjacentflag=0;

	get_mm_charset();
	c=s;
	t=buffer;
	while (*c && t-buffer<2048) {
		if (*c!='=') {
			if (adjacentflag && isspace(*c)) {
				char *dd;
				dd=c+1;
				while (isspace(*dd)) dd++;
				if (*dd=='=') { /* brute hack, makes mistakes under certain circumstances comp. 6.2 */
					c++;
					continue;
				}
			}
			adjacentflag=0;
			*t++=*c++;
			continue;
		}
		d=c++;
		if (*c=='?') {
			char *e;
			e=charset;
			c++;
			while (*c&&*c!='?') *e++=*c++;
			*e=0;
			if (*c=='?') {
				c++;
				encoding=tolower(*c);
				if (encoding=='b') mmdecode(NULL,'b',0,NULL,NULL); /* flush */
				c++;
				if (*c=='?') {
					c++;
					if ((e=strchr(c,'?'))) {
						int i;
						i=mmdecode(c,encoding,'?',t,charset);
						if (i>0) {
							t+=i;
							e++;
							if (*e=='=') e++;
							d=c=e;
							adjacentflag=1;
						}
					}
				}
			}
		}
		while (d!=c) *t++=*d++;
	}
	*t=0;
	return buffer;
}


int
contains_nonprintables(w)
	unsigned char *w;
{
#ifdef MIME_BASE64_ALLOWED	
	int chars=0;
	int schars=0;
#endif
	int nonprint=0;

	/* first skip all leading whitespaces */
  	while (*w&&isspace(*w)) w++;
  	
	/* then check the next word */
	while (*w&&!isspace(*w)&&*w!='('&&*w!=')') {
		if (*w<32||*w>127) nonprint++;
		if (!nonprint && *w=='=' && *(w+1)=='?') nonprint=1;
#ifdef MIME_BASE64_ALLOWED
		if (*w=='=' || *w=='?') schars++;
		chars++;
#endif		
		w++;
  	}
	if (nonprint) {
#ifdef MIME_BASE64_ALLOWED
		if (chars+2*(nonprint+schars) /* QP size */ >
		   (chars*4+3)/3 /* B64 size */) return 'B';
#endif
		return 'Q';
	}
  	return 0;
}

int
sizeofnextword(w)
	unsigned char *w;
{
	unsigned char *x;

	x=w;
	while (*x&&isspace(*x)) x++;
	while (*x&&!isspace(*x)) x++;
	return x-w;
}


int
mystrcat(t,s)
	char **t;
	char *s;
{
	int len=0;

	while (*s) {
		*((*t)++)=*s++;
		len++;
	}
	**t=0;
	return len;
}


int
rfc1522_do_encode(what, where)
	unsigned char *what;
	unsigned char **where;
{
  /* We need to meet several partly contradictional requirements here.
     First of all, a line containing MIME encodings must not be longer
     than 76 chars (including delimiters, charset, encoding).  Second,
     we should not encode more than necessary.  Third, we should not
     produce more overhead than absolutely necessary; this means we
     should extend chunks over several words if there are more
     characters-to-quote to come.  This means we have to rely on some
     heuristics.  We process whole words, checking if it contains
     characters to be quoted.  If not, the word is output 'as is',
     previous quoting being terminated before.  If two adjoining words
     contain non-printable characters, they are encoded together (up
     to 60 characters).  If a resulting encoded word would break the
     76 characters boundary, we 'break' the line, output a SPACE, then
     output the encoded word.  Note that many wide-spread news applications,
     notably INN's xover support, does not understand multiple-lines,
     so it's a compile-time feature with default off.

     To make things a bit easier, we do all processing in two stages;
     first we build all encoded words without any bells and whistles
     (just checking that they don get longer than 76 characters),
     then, in a second pass, we replace all SPACEs inside encoded
     words by '_', break long lines, etc. */

	int quoting=0;		/* currently inside quote block? */
	int encoding;		/* which encoding to use ('B' or 'Q') */
	int any_quoting_done=0;
#ifdef MIME_BREAK_LONG_LINES
	int column=0;		/* current column */
#endif
	int ewsize=0;		/* size of current encoded-word */
	unsigned char buf[2048];/* buffer for encoded stuff */
	unsigned char buf2[64];	/* buffer for this and that */
	unsigned char *c;
	unsigned char *t;

	t=buf;
	while (*what) {
		if ((encoding=contains_nonprintables(what))) {
			if (!quoting) {
				sprintf((char *)buf2,"=?%s?%c?",mm_charset,encoding);
				ewsize=mystrcat((char **)&t,(char *)buf2);
				quoting=1;
				any_quoting_done=1;
			}
			while (*what && !isspace(*what)&&*what!='('&&*what!=')') {
				if (*what<32||*what>127||*what=='='||*what=='?') {
					sprintf((char *)buf2,"=%2.2X",*what);
					*t++=buf2[0];
					*t++=buf2[1];
					*t++=buf2[2];
					ewsize+=3;
				} else {
					*t++=*what;
					ewsize++;
				}
				what++;
			}
			if (!contains_nonprintables(what) || ewsize>=60) {
				/* next word is 'clean', close encoding */
				*t++='?';
				*t++='=';
				ewsize+=2;
				quoting=0;
			} else {
				/* process whitespace in-between by quoting it properly */
				while (*what&&isspace(*what)) {
					if (*what==32 /* not ' ', compare chapter 4! */) {
						*t++='_';
						ewsize++;
					} else {
						sprintf((char *)buf2,"=%2.2X",*what);
						*t++=buf2[0];
						*t++=buf2[1];
						*t++=buf2[2];
						ewsize+=3;
					}
					what++;
				}
			}
		} else {
			while (*what&&!isspace(*what)&&*what!='('&&*what!=')') *t++=*what++; /* output word unencoded */
			while ((*what&&isspace(*what))||*what=='('||*what==')') *t++=*what++; /* output trailing whitespace unencoded */
		}
	}
	*t=0;
	/* Pass 2: break long lines if there are MIME-sequences in the result */
	c=buf;
#ifdef MIME_BREAK_LONG_LINES
	column=0;
	if (any_quoting_done) {
		while (*c) {
			if (isspace(*c)) {
				if (sizeofnextword(c)+column>76) {
					*((*where)++)='\n';
					column=0;
				}
				*((*where)++)=*c++;
				column++;
			}
			else while (*c&&!isspace(*c)) {
				*((*where)++)=*c++;
				column++;
			}
		}
	}
#else
	if (0);
#endif
	else {
		while (*c) *((*where)++)=*c++;
	}
	**where=0;
	return any_quoting_done;
}


char *
rfc1522_encode(s)
	char *s;
{
	static char buf[2048];
	unsigned char *b;
	int x;

	get_mm_charset();
	b=(unsigned char *)buf;
	x=rfc1522_do_encode((unsigned char *)s,&b);
	quoteflag=quoteflag || x;
	return buf;
}

void
rfc15211522_encode(filename)
	char *filename;
{
	FILE *f;
	FILE *g;
	unsigned char header[4096];
	unsigned char buffer[2048];
	unsigned char *c, *d;
	int umlauts = 0;
	int body_encoding_needed = 0;
	char encoding;

	g=tmpfile();
	if (!g) return;
	f=fopen(filename,"r");
	if (!f) {
		fclose(g);
		return;
	}
	header[0]=0;
	d=header;
	quoteflag=0;
	while (fgets((char *)buffer,2048,f)) {
		if (header[0]&&(!isspace(buffer[0])||buffer[0]=='\r'||buffer[0]=='\n')) {
			fputs(rfc1522_encode((char *)header),g);
 			fputc('\n',g);
			header[0]=0;
			d=header;
		}
		if (buffer[0]=='\n'||buffer[0]=='\r') break;
		c=buffer;
		while (*c&&*c!='\r'&&*c!='\n') *d++=*c++;
		*d=0;
	}
	fputc('\n',g);
	while (fgets((char *)buffer,2048,f)) {
		fputs((char *)buffer,g);
		/* see if there are any umlauts in the body... */
		for (c=buffer; *c&&!*c!='\r'&&*c!='\n'; c++) 
		if (*c<32||*c>127) {
			umlauts=1;
			body_encoding_needed=1;
			break;
      		}
	}
	fclose(f);
	rewind(g);
	f=fopen(filename,"w");
	if (!f) {
		fclose(g);
		return;
	}
	while (fgets((char *)buffer,2048,g)) {
		if (buffer[0]=='\r'||buffer[0]=='\n') break;
		fputs((char *)buffer,f);
	}

	/* now add MIME headers as necessary */
#if 0  /* RFC1522 does not require MIME headers just because there are
	   encoded header lines */
  	if (quoteflag||umlauts) {
#else
	if (umlauts) {
#endif
		fputs("MIME-Version: 1.0\n",f);
		if (body_encoding_needed) {
			fprintf(f,"Content-Type: text/plain; charset=%s\n",mm_charset);
			fprintf(f,"Content-Transfer-Encoding: %s\n",mime_encoding);
		} else {
			fputs("Content-Type: text/plain; charset=US-ASCII\n",f);
			fputs("Content-Transfer-Encoding: 7bit\n",f);
		}
	}
	fputc('\n',f);

	if (!strcasecmp(mime_encoding,"base64")) encoding='b';
	else if (!strcasecmp(mime_encoding,"quoted-printable")) encoding='q';
	else encoding='8';

	if (!body_encoding_needed) encoding='8';

	while (fgets((char *)buffer,2048,g)) {
		rfc1521_encode(buffer,f,encoding);
	}
	if (encoding=='b') rfc1521_encode(NULL,f,encoding); /* flush */

	fclose(g);
	fclose(f);
}


#ifdef RFCDEBUG
#define xTESTHEADER "vorher =?ISO-8859-1?Q?Kristian_K=F6hntopp?= nachher"
#define yTESTHEADER "vorher =?ISO-8859-1?B?S3Jpc3RpYW4gS/ZobnRvcHA=?= nachher"
#define TESTHEADER "Kristian Köhntopp 	Müller 123 (hähäsdkäfas)."

main()
{
  char *c;

  printf("%s\n",c=rfc1522_encode(TESTHEADER));
  printf("%s\n",rfc1522_decode(c));
}
#endif /* RFCDEBUG */
