/*
 *  Project   : tin - a Usenet reader
 *  Module    : rfc1522.c
 *  Author    : Chris Blum <chris@phil.uni-sb.de>
 *  Created   : September '95
 *  Updated   : 08-15-96
 *  Notes     : MIME header encoding/decoding stuff
 *  Copyright : (c) Copyright 1995-96 by Chris Blum
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made.
 */



/*
 * allow base64 encoding in headers if the
 * result is shorter than quoted printable
 * encoding. THIS IS NOT YET IMPLEMENTED,
 * so leave this off
 */
#undef MIME_BASE64_ALLOWED

#include "tin.h"

#define isreturn(c) ((c) == '\r' || ((c) == '\n'))
#define isbetween(c) (isspace((unsigned char)c) || (c) == '(' || (c) == ')' || (c) == '"')
/*
 * NOTE: these routines expect that MM_CHARSET is set to the charset
 * your system is using.  If it is not defined, US-ASCII is used.
 * Can be overridden by setting MM_CHARSET as environment variable.
 */

#ifndef MM_CHARSET
#define MM_CHARSET "US-ASCII"
#endif

#define NOT_RANKED 255

const char base64_alphabet[64] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

static unsigned char base64_rank[256];
static int base64_rank_table_built;
static int quoteflag;

static void
build_base64_rank_table (void)
{
	int i;

	if (!base64_rank_table_built) {
		for (i = 0; i < 256; i++)
			base64_rank[i] = NOT_RANKED;
		for (i = 0; i < 64; i++)
			base64_rank[(int) base64_alphabet[i]] = i;
		base64_rank_table_built = TRUE;
	}
}

static unsigned
hex2bin (
	int x)
{
	if (x >= '0' && x <= '9')
		return (x - '0');
	if (x >= 'A' && x <= 'F')
		return (x - 'A') + 10;
	if (x >= 'a' && x <= 'f')
		return (x - 'a') + 10;
	return 255;
}

int
mmdecode (
	const char *what,
	int encoding,
	int delimiter,
	char *where,
	const char *charset)
{
	char *t;
	int decode_gt128 = 0;

#ifdef MIME_STRICT_CHARSET
	if (charset && !strcasecmp (charset, mm_charset))
		decode_gt128 = TRUE;
#else
	decode_gt128 = TRUE;
#endif
	t = where;
	encoding = tolower (encoding);
	if (encoding == 'q') {	/* quoted-printable */
		int x;
		unsigned hi, lo;

		while (*what != delimiter) {
			if (*what != '=') {
				if (!delimiter || *what != '_')
					*t++ = *what++;
				else
					*t++ = ' ', what++;
				continue;
			}
			what++;
			if (*what == delimiter)		/* failed */
				return -1;

			x = *what++;
			if (x == '\n')
				continue;
			if (*what == delimiter)
				return -1;

			hi = hex2bin (x);
			lo = hex2bin (*what);
			what++;
			if (hi == 255 || lo == 255)
				return -1;
			x = (hi << 4) + lo;
			if (x >= 128 && !decode_gt128)
				x = '?';
			*EIGHT_BIT(t)++ = x;
		}
		return t - where;
	} else if (encoding == 'b') {	/* base64 */
		static unsigned short pattern = 0;
		static int bits = 0;
		unsigned char x;

		build_base64_rank_table ();
		if (!what || !where) {	/* flush */
			pattern = bits = 0;
			return 0;
		}
		while (*what != delimiter) {
			x = base64_rank[(int) (*what++)];
			/* ignore everything not in the alphabet, including '=' */
			if (x == NOT_RANKED)
				continue;
			pattern <<= 6;
			pattern |= x;
			bits += 6;
			if (bits >= 8) {
				x = (pattern >> (bits - 8)) & 0xff;
				if (x >= 128 && !decode_gt128)
					x = '?';
				*t++ = x;
				bits -= 8;
			}
		}
		return t - where;
	}
	return -1;
}

void
get_mm_charset (void)
{
	char *c;

	if (!mm_charset[0]) {
		c = getenv ("MM_CHARSET");
		if (!c)
			strcpy (mm_charset, MM_CHARSET);
		else {
			strncpy (mm_charset, c, 128);
			mm_charset[127] = '\0';
		}
	}
}

char *
rfc1522_decode (
	const char *s)
{
	const char *c, *d;
	char *t;
	static char buffer[2048];
	char charset[256];
	char encoding;
	char adjacentflag = 0;

	get_mm_charset ();
	c = s;
	t = buffer;
	while (*c && t - buffer < 2048) {
		if (*c != '=') {
			if (adjacentflag && isspace ((unsigned char)*c)) {
				const char *dd;

				dd = c + 1;
				while (isspace ((unsigned char)*dd))
					dd++;
				if (*dd == '=') {	/* brute hack, makes mistakes under certain circumstances comp. 6.2 */
					c++;
					continue;
				}
			}
			adjacentflag = 0;
			*t++ = *c++;
			continue;
		}
		d = c++;
		if (*c == '?') {
			char *e;

			e = charset;
			c++;
			while (*c && *c != '?')
				*e++ = *c++;
			*e = 0;
			if (*c == '?') {
				c++;
				encoding = tolower (*c);
				if (encoding == 'b')
					mmdecode ((char *) 0, 'b', 0, (char *) 0, (char *) 0);	/* flush */
				c++;
				if (*c == '?') {
					c++;
					if ((e = strchr (c, '?'))) {
						int i;

						i = mmdecode (c, encoding, '?', t, charset);
						if (i > 0) {
							t += i;
							e++;
							if (*e == '=')
								e++;
							d = c = e;
							adjacentflag = TRUE;
						}
					}
				}
			}
		}
		while (d != c)
			*t++ = *d++;
	}
	*t = '\0';

#ifdef LOCAL_CHARSET
	buffer_to_local(buffer);
#endif

	return buffer;
}

static int
contains_nonprintables (
	char *w)
{
#ifdef MIME_BASE64_ALLOWED
	int chars = 0;
	int schars = 0;
#endif
	int nonprint = 0;

	/* first skip all leading whitespaces */
	while (*w && isspace ((unsigned char)*w))
		w++;

	/* then check the next word */
	while (*w && !isbetween(*w)) {
		if (is_EIGHT_BIT(w))
			nonprint++;
		if (!nonprint && *w == '=' && *(w + 1) == '?')
			nonprint = TRUE;
#ifdef MIME_BASE64_ALLOWED
		if (*w == '=' || *w == '?' || *w == '_')
			schars++;
		chars++;
#endif
		w++;
	}
	if (nonprint) {
#ifdef MIME_BASE64_ALLOWED
/* Use B encoding if charset is EUC-KR for backward compatibility 
                                with old Korean mail program */
		if (chars + 2 * (nonprint + schars) /* QP size */ >
		    (chars * 4 + 3) / 3 /* B64 size */
                    || ! strcasecmp(mm_charset,"EUC-KR") )
			return 'B';
#endif
		return 'Q';
	}
	return 0;
}

/* Uncommented this conditional compilation statement
   in order to implement mandatory break-up of long lines
   in mail messages in accordance with rfc 2047(rfc 1522) */
/* #ifdef MIME_BREAK_LONG_LINES */
static int
sizeofnextword (
	char *w)
{
	char *x;

	x = w;
	while (*x && isspace ((unsigned char)*x))
		x++;
	while (*x && !isspace ((unsigned char)*x))
		x++;
	return x - w;
}
/* #endif */


static int
rfc1522_do_encode (
	char *what,
	char **where,
        t_bool break_long_line)
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

	int quoting = 0;	/* currently inside quote block? */
	int encoding;		/* which encoding to use ('B' or 'Q') */
	int any_quoting_done = 0;

/* Uncommented this and other conditional compilation statement
   depending on MIME_BREAK_LONG_LINES in this function 
   in order to implement mandatory break-up of long lines
   in mail messages in accordance with RFC 2047(RFC 1522). 
   Whether or not long lines are broken up depends on
   boolean variable break_long_line, instead.
   break_long_line is  FALSE for news posting unless MIME_BREAK_LONG_LINES
   is defined, but it's TRUE for mail messages regardless of whether or not
   MIME_BREAK_LONG_LINES is defined
*/
/* #ifdef MIME_BREAK_LONG_LINES */
	int column = 0;		/* current column */
	int word_cnt = 0;
/* #endif */
	int ewsize = 0;		/* size of current encoded-word */
	char buf[2048];		/* buffer for encoded stuff */
	char buf2[64];		/* buffer for this and that */
	char *c;
	char *t;

	t = buf;
	while (*what) {
                if (break_long_line == TRUE) {
		   word_cnt++;
                }
		if ((encoding = contains_nonprintables (what))) {
			if (!quoting) {
				sprintf (buf2, "=?%s?%c?", mm_charset, encoding);
				ewsize = mystrcat (&t, buf2);
                                if (break_long_line==TRUE) {
					if (word_cnt == 2) {
				       	/* Make sure we fit the first encoded
					 * word in with the header keyword,
					 * since we cannot break the line
					 * directly after the keyword.
					 */
						ewsize = t - buf;
					}
                                }
				quoting = TRUE;
				any_quoting_done = TRUE;
			}
			while (*what && !isbetween(*what)) {
				if (is_EIGHT_BIT(what)
				    || *what == '='
				    || *what == '?'
				    || *what == '_') {
					sprintf (buf2, "=%2.2X", *EIGHT_BIT(what));
					*t++ = buf2[0];
					*t++ = buf2[1];
					*t++ = buf2[2];
					ewsize += 3;
				} else {
					*t++ = *what;
					ewsize++;
				}
				what++;
				/* Be sure to encode at least one char, even if
				 * that overflows the line limit, otherwise, we
				 * will be stuck in a loop (if this were in the
				 * while condition above).  (Can only happen in
				 * the first line, if we have a very long
				 * header keyword, I think).
				 */
				if (ewsize >= 71) {
					break;
				}
			}
			if (!contains_nonprintables (what) || ewsize >= 60) {
				/* next word is 'clean', close encoding */
				*t++ = '?';
				*t++ = '=';
				ewsize += 2;
#ifdef MIME_BREAK_LONG_LINES
/* if our line is too long, but the next word will not be quoted, we
   just use the space that separates the words as header continuation
   space. Note that apparently the xover files in INN convert the nl
   also to space, which inserts a 2nd space into our string. This is
   not a problem when we continue with a quoted word, since whitespace
   between quoted words is ignored. (We could insert a quoted space
   like =?us-ascii?Q?_?= after the line break, but that's kind of
   ugly. As long as we are threading by Refs, it will work, but
   threading by Subject will break with this. Since we parse the
   header lines ourself before generating followups, at least the
   error will not be in the next article. */
#else
/* if we do not break long lines, we could just continue with the
   encoded text, but rfc1522 says that encoded words are only
   non-whitespace strings of up to 75 chars, delimited by whitespace
   or the line start/end, so we break and insert a space here also. */
#endif
				if (ewsize >= 60 && contains_nonprintables (what)) {
					*t++ = ' ';
					ewsize++;
				}
				quoting = 0;
			} else {
				/* process whitespace in-between by quoting it properly */
				while (*what && isspace ((unsigned char)*what)) {
					if (*what == 32 /* not ' ', compare chapter 4!*/) {
						*t++ = '_';
						ewsize++;
					} else {
						sprintf (buf2, "=%2.2X", *EIGHT_BIT(what));
						*t++ = buf2[0];
						*t++ = buf2[1];
						*t++ = buf2[2];
						ewsize += 3;
					}
					what++;
				}
			}
		} else {
			while (*what && !isbetween(*what))
				*t++ = *what++;		/* output word unencoded */
			while (*what && isbetween(*what))
				*t++ = *what++;		/* output trailing whitespace unencoded */
		}
	}
	*t = 0;
	/* Pass 2: break long lines if there are MIME-sequences in the result */
	c = buf;
        if (break_long_line==TRUE) {
		column = 0;
		if (any_quoting_done) {
			word_cnt = 1;	/* note, if the user has typed a
					   continuation line, we will consider the
					   initial whitespace to be delimiting
					   word one (well, just assume an empty
					   word). */
			while (*c) {
				if (isspace ((unsigned char)*c)) {
					/* According to rfc1522, header lines
					 * containing encoded words are limited to 76
					 * chars, but if the first line is too long
					 * (due to a long header keyword), we cannot
					 * stick to that, since we would break the line
					 * directly after the keyword's colon, which is
					 * not allowed.  The same is necessary for a
					 * continuation line with an unencoded word
					 * that is too long.
					 */
					if (sizeofnextword (c) + column > 76 && word_cnt != 1) {
						*((*where)++) = '\n';
						column = 0;
					}
					if (c > buf && !isspace ((unsigned char)*(c - 1))) {
						word_cnt++;
					}
					*((*where)++) = *c++;
					column++;
				} else
					while (*c && !isspace ((unsigned char)*c)) {
						*((*where)++) = *c++;
						column++;
					}
			}
		} 
                else {
			while (*c)
				*((*where)++) = *c++;
		}
        }
	else {     /* break_long_line == FALSE */
		while (*c)
			*((*where)++) = *c++;
	}
      
	**where = 0;
	return any_quoting_done;
}

char *
rfc1522_encode (
	char *s,
        t_bool ismail)
{
	static char buf[2048];
	char *b;
	int x;
/* break_long_line is  FALSE for news posting unless MIME_BREAK_LONG_LINES
   is defined, but it's TRUE for mail messages regardless of whether or not
   MIME_BREAK_LONG_LINES is defined */
#ifdef MIME_BREAK_LONG_LINES
        t_bool break_long_line=TRUE;
#else
        t_bool break_long_line=FALSE;
#endif

/* Even if MIME_BREAK_LONG_LINES is NOT defined,
   long headers in mail messages should be broken up in 
   accordance with RFC 2047(1522) */
#ifndef MIME_BREAK_LONG_LINES
        if ( ismail == TRUE) {
            break_long_line=TRUE;
        }
#endif

	get_mm_charset ();
	b = buf;
	x = rfc1522_do_encode (s, &b,break_long_line);
	quoteflag = quoteflag || x;
	return buf;
}

void
rfc15211522_encode (
	char *filename,
	constext *mime_encoding,
        t_bool  allow_8bit_header,
        t_bool ismail)
{
	FILE *f;
	FILE *g;
	char header[4096];
	char buffer[2048];
	char *c, *d;
	int umlauts = 0;
	int body_encoding_needed = 0;
	char encoding;
	BodyPtr body_encode;

	g = tmpfile ();
	if (!g)
		return;
	f = fopen (filename, "r");
	if (!f) {
		fclose (g);
		return;
	}
	header[0] = 0;
	d = header;
	quoteflag = 0;
	while (fgets (buffer, 2048, f)) {
#ifdef LOCAL_CHARSET
		buffer_to_network(buffer);
#endif
		if (header[0]
		    && (!isspace ((unsigned char)buffer[0]) || isreturn(buffer[0]))) {
                        if (allow_8bit_header)
                           fputs(header,g);
                        else
			   fputs (rfc1522_encode (header,ismail), g);
			fputc ('\n', g);
			header[0] = '\0';
			d = header;
		}
		if (isreturn(buffer[0]))
			break;
		c = buffer;
		while (*c && !isreturn(*c))
			*d++ = *c++;
		*d = 0;
	}
	fputc ('\n', g);
	while (fgets (buffer, 2048, f)) {
#ifdef LOCAL_CHARSET
		buffer_to_network(buffer);
#endif
		fputs (buffer, g);
		/* see if there are any umlauts in the body... */
		for (c = buffer; *c && !isreturn(*c); c++)
			if (is_EIGHT_BIT(c)) {
				umlauts = TRUE;
				body_encoding_needed = TRUE;
				break;
			}
	}
	fclose (f);
	rewind (g);
	f = fopen (filename, "w");
	if (!f) {
		fclose (g);
		return;
	}
	while (fgets (buffer, 2048, g)
	       && !isreturn(buffer[0]))
		fputs (buffer, f);

	/* now add MIME headers as necessary */
#if 0				/* RFC1522 does not require MIME headers just because there are
				   encoded header lines */
	if (quoteflag || umlauts) {
#else
	if (umlauts) {
#endif
		fputs ("MIME-Version: 1.0\n", f);
		if (body_encoding_needed) {

/* added for CJK charsets like EUC-KR/JP/CN and others */

                        if (!strncasecmp(mm_charset, "euc-", 4) &&
                            !strcasecmp(mime_encoding, txt_7bit))

			   fprintf (f, "Content-Type: text/plain; charset=ISO-2022-%s\n", &mm_charset[4]);
                        else

			   fprintf (f, "Content-Type: text/plain; charset=%s\n", mm_charset);
			fprintf (f, "Content-Transfer-Encoding: %s\n", mime_encoding);
		} else {
			fputs ("Content-Type: text/plain; charset=US-ASCII\n", f);
			fputs ("Content-Transfer-Encoding: 7bit\n", f);
		}
	}
	fputc ('\n', f);

	if (!strcasecmp (mime_encoding, txt_base64))
		encoding = 'b';
	else if (!strcasecmp (mime_encoding, txt_quoted_printable))
		encoding = 'q';
	else
		encoding = '8';

	if (!body_encoding_needed)
		encoding = '8';

/* added for CJK charsets like EUC-KR/JP/CN and others */

	if (!strcasecmp (mime_encoding, txt_7bit)) {
		encoding = '7';

/* For EUC-KR, 7bit means ISO-2022-KR encoding specified in RFC 1557 */

		if (!strcasecmp(mm_charset, "euc-kr"))
			body_encode = rfc1557_encode;

/*
 * Not only  EUC-JP but also other Japanese charsets such as
 * SJIS and JIS might need RFC 1468 encoding. To be confirmed.
 */
		else if (!strcasecmp(mm_charset, "euc-jp"))
			body_encode = rfc1468_encode;

/*
 * Not only  EUC-CN but also other Chinese charsets such as
 * BIG5 and Traditional  might need RFC 1922 encoding. To be confirmed.
 */
		else if (!strcasecmp(mm_charset, "euc-cn"))
			body_encode = rfc1922_encode;
		else {
			body_encode = rfc1521_encode;
			encoding='8';
		}
	}
	else
		body_encode = rfc1521_encode;

	while (fgets (buffer, 2048, g)) {
		body_encode (buffer, f, encoding);
	}
	if (encoding == 'b' || encoding == 'q' || encoding == '7')
		body_encode (NULL, f, encoding);	/* flush */

	fclose (g);
	fclose (f);
}
