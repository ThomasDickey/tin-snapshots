/*
 *  Project   : tin - a Usenet reader
 *  Module    : rfc1522.c
 *  Author    : Chris Blum <chris@phil.uni-sb.de>
 *  Created   : September '95
 *  Updated   : 1998-04-05
 *  Notes     : MIME header encoding/decoding stuff
 *  Copyright : (c) Copyright 1995-99 by Chris Blum
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made.
 */

#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */

#define isreturn(c) ((c) == '\r' || ((c) == '\n'))

/*
 * Modified to return TRUE for '(' and ')' only if
 * it's in structured header field. '(' and ')' are
 * NOT to be treated differently than other characters
 * in unstructured headers like Subject, Keyword and Summary
 * c.f. RFC 2047
 */
#define isbetween(c, s) (isspace((unsigned char)c) || ((s) && ((c) == '(' || (c) == ')' || (c) == '"')))

/*
 * NOTE: these routines expect that MM_CHARSET is set to the charset
 * your system is using.  If it is not defined, US-ASCII is used.
 * Can be overridden by setting MM_CHARSET as environment variable.
 */
#ifndef MM_CHARSET
#	define MM_CHARSET "US-ASCII"
#endif /* !MM_CHARSET */

#define NOT_RANKED 255

#if 0
	/* inside a quoted word these 7bit chars need to be encoded too */
#	define RFC2047_ESPECIALS "[]<>.;@,=?_\"\\"
#endif /* 0 */

const char base64_alphabet[64] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

static unsigned char base64_rank[256];
static int base64_rank_table_built;
static int quoteflag;

/*
 * local prototypes
 */
static int do_b_encode (char *w, char *b, int max_ewsize, t_bool isstruct_head);
static int rfc1522_do_encode (char *what, char **where, t_bool break_long_line);
static int sizeofnextword (char *w);
static int which_encoding (char *w);
static t_bool contains_nonprintables (char *w, t_bool isstruct_head);
static unsigned hex2bin (int x);
static void build_base64_rank_table (void);
static void str2b64 (char *from, char *to);


static void
build_base64_rank_table (
	void)
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
	t_bool decode_gt128 = FALSE;

#ifdef MIME_STRICT_CHARSET
	if (charset && !strcasecmp(charset, tinrc.mm_charset))
#endif /* MIME_STRICT_CHARSET */
		decode_gt128 = TRUE;

	t = where;
	encoding = tolower((unsigned char)encoding);
	if (encoding == 'q') {		  /* quoted-printable */
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

			hi = hex2bin(x);
			lo = hex2bin(*what);
			what++;
			if (hi == 255 || lo == 255)
				return -1;
			x = (hi << 4) + lo;
			if (x >= 128 && !decode_gt128)
				x = '?';
			*EIGHT_BIT(t)++ = x;
		}
		return t - where;
	} else if (encoding == 'b') {		/* base64 */
		static unsigned short pattern = 0;
		static int bits = 0;
		unsigned char x;

		build_base64_rank_table();
		if (!what || !where) {	  /* flush */
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
get_mm_charset (
	void)
{
	if (!*tinrc.mm_charset) {
		STRCPY(tinrc.mm_charset, get_val("MM_CHARSET", MM_CHARSET));
	}
}


char *
rfc1522_decode (
	const char *s)
{
	const char *c, *d;
	char *t;
	static char buffer[2048];
	char charset[1024];
	char encoding;
	char adjacentflag = 0;

	get_mm_charset();
	c = s;
	t = buffer;
	while (*c && t - buffer < 2048) {
		if (*c != '=') {
			if (adjacentflag && isspace((unsigned char) *c)) {
				const char *dd;

				dd = c + 1;
				while (isspace((unsigned char) *dd))
					dd++;
				if (*dd == '=') {	  /* brute hack, makes mistakes under certain circumstances comp. 6.2 */
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
				encoding = tolower((unsigned char)*c);
				if (encoding == 'b')
					(void) mmdecode((char *) 0, 'b', 0, (char *) 0, (char *) 0);		/* flush */
				c++;
				if (*c == '?') {
					c++;
					if ((e = strchr(c, '?'))) {
						int i;

						i = mmdecode(c, encoding, '?', t, charset);
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
#endif /* LOCAL_CHARSET */

	return buffer;
}


/*
 * adopted by J. Shin(jshin@pantheon.yale.edu) from
 * Woohyung Choi's(whchoi@cosmos.kaist.ac.kr) sdn2ks and ks2sdn
 */
static void
str2b64 (
	char *from,
	char *to)
{
	short int i, count;
	unsigned long tmp;

	while (*from) {
		for (i = count = 0, tmp = 0; i < 3; i++)
			if (*from) {
				tmp = (tmp << 8) | (unsigned long) (*from++ & 0x0ff);
				count++;
			} else
				tmp = (tmp << 8) | (unsigned long) 0;

		*to++ = base64_alphabet[(0x0fc0000 & tmp) >> 18];
		*to++ = base64_alphabet[(0x003f000 & tmp) >> 12];
		*to++ = count >= 2 ? base64_alphabet[(0x0000fc0 & tmp) >> 6] : '=';
		*to++ = count >= 3 ? base64_alphabet[0x000003f & tmp] : '=';
	}

	*to = '\0';
	return;
}


static int
do_b_encode (
	char *w,
	char *b,
	int max_ewsize,
	t_bool isstruct_head)
{

	char tmp[60];				/* strings to be B encoded */
	char *t = tmp;
	int len8 = 0;				/* the number of trailing 8bit chars, which
									   should be even(i.e. the first and second byte
									   of wide_char should NOT be split into two
									   encoded words) in order to be compatible with
									   some CJK mail client */

	int count = max_ewsize / 4 * 3;
	t_bool isleading_between = TRUE;		/* are we still processing leading space */

	while (count-- > 0 && (!isbetween(*w, isstruct_head) || isleading_between) && *w) {
		len8 += is_EIGHT_BIT(w) ? 1 : (-len8);
		if (!isbetween(*w, isstruct_head))
			isleading_between = FALSE;
		*(t++) = *(w++);
	}

/* if (len8 & (unsigned long) 1 && !isbetween(*w,isstruct_head)) */
	if (len8 != len8 / 2 * 2 && !isbetween(*w, isstruct_head) && (*w))
		t--;

	*t = '\0';

	str2b64(tmp, b);

	return t - tmp;
}


/*
 * find out whether encoding is necessary and which encoding
 * to use if necessary by scanning the whole header field
 * instead of each fragment of it.
 * This will ensure that  either Q or B encoding will be used in a single
 * header (i.e. two encoding won't be mixed in a single header line).
 * Mixing two encodings is not a violation of RFC 2047 but may break
 * some news/mail clients.
 */
static int
which_encoding (
	char *w)
{
	int chars = 0;
	int schars = 0;
	int nonprint = 0;

	while (*w && isspace((unsigned char) *w))
		w++;
	while (*w) {
		if (is_EIGHT_BIT(w))
			nonprint++;
		if (!nonprint && *w == '=' && *(w + 1) == '?')
			nonprint = 1;
		if (*w == '=' || *w == '?' || *w == '_')
			schars++;
		chars++;
		w++;
	}
	if (nonprint) {
/*
 * Always use B encoding regardless of the efficiency if charset is
 * EUC-KR for backward compatibility with old Korean mail program
 */
		if (chars + 2 * (nonprint + schars) /* QP size */ >
			 (chars * 4 + 3) / 3	  /* B64 size */
			 || !strcasecmp(tinrc.mm_charset, "EUC-KR"))
			return 'B';
		return 'Q';
	}
	return 0;
}


/* now only checks if there's any 8bit chars in a given "fragment" */
static t_bool
contains_nonprintables (
	char *w,
	t_bool isstruct_head)
{
	t_bool nonprint = FALSE;

	/* first skip all leading whitespaces */
	while (*w && isspace((unsigned char) *w))
		w++;

	/* then check the next word */
	while (*w && !isbetween(*w, isstruct_head)) {
		if (is_EIGHT_BIT(w))
			nonprint = TRUE;
		if (!nonprint && *w == '=' && *(w + 1) == '?')
			nonprint = TRUE;
		w++;
	}
	return nonprint;
}


/*
 * Uncommented this conditional compilation statement
 * in order to implement mandatory break-up of long lines
 * in mail messages in accordance with rfc 2047 (rfc 1522)
 */
/* #ifdef MIME_BREAK_LONG_LINES */
static int
sizeofnextword (
	char *w)
{
	char *x;

	x = w;
	while (*x && isspace((unsigned char) *x))
		x++;
	while (*x && !isspace((unsigned char) *x))
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
	/*
	 * We need to meet several partly contradictional requirements here.
	 * First of all, a line containing MIME encodings must not be longer
	 * than 76 chars (including delimiters, charset, encoding). Second,
	 * we should not encode more than necessary. Third, we should not
	 * produce more overhead than absolutely necessary; this means we
	 * should extend chunks over several words if there are more
	 * characters-to-quote to come. This means we have to rely on some
	 * heuristics. We process whole words, checking if it contains
	 * characters to be quoted. If not, the word is output 'as is',
	 * previous quoting being terminated before. If two adjoining words
	 * contain non-printable characters, they are encoded together (up
	 * to 60 characters). If a resulting encoded word would break the
	 * 76 characters boundary, we 'break' the line, output a SPACE, then
	 * output the encoded word. Note that many wide-spread news applications,
	 * notably INN's xover support, does not understand multiple-lines,
	 * so it's a compile-time feature with default off.
	 *
	 * To make things a bit easier, we do all processing in two stages;
	 * first we build all encoded words without any bells and whistles
	 * (just checking that they don get longer than 76 characters),
	 * then, in a second pass, we replace all SPACEs inside encoded
	 * words by '_', break long lines, etc.
	 */

	int encoding;					  /* which encoding to use ('B' or 'Q') */
	t_bool quoting = FALSE;		  /* currently inside quote block? */
	t_bool any_quoting_done = FALSE;
	t_bool isbroken_within = FALSE;	/* is word broken due to length restriction on encoded of word? */
	t_bool rightafter_ew = FALSE;

/*
 * Uncommented this and other conditional compilation statement
 * depending on MIME_BREAK_LONG_LINES in this function
 * in order to implement mandatory break-up of long lines
 * in mail messages in accordance with RFC 2047 (RFC 1522).
 * Whether or not long lines are broken up depends on
 * boolean variable break_long_line, instead.
 * break_long_line is  FALSE for news posting unless MIME_BREAK_LONG_LINES
 * is defined, but it's TRUE for mail messages regardless of whether or not
 * MIME_BREAK_LONG_LINES is defined
 */
/* #ifdef MIME_BREAK_LONG_LINES */
	int column = 0;				/* current column */
	int word_cnt = 0;

/* #endif */
	size_t ewsize = 0;			/* size of current encoded-word */
	char buf[2048];				/* buffer for encoded stuff */
	char buf2[80];				/* buffer for this and that */
	char *c;
	char *t;
	t_bool isstruct_head = FALSE;		/* are we dealing with structured header? */
	int ew_taken_len;

/*
 * the list of structured header fields where '(' and ')' are
 * treated specially in rfc 1522 encoding
 */
	static const char *struct_header[] =
	{
		"Approved: ", "From: ", "Originator: ",
		"Reply-To: ", "Sender: ", "X-Cancelled-By: ", "X-Comment-To: ",
		"X-Submissions-To: ", "To: ", "Cc: ", "Bcc: ", "X-Originator: ", 0};
	const char **strptr = struct_header;

	do {
		if (!strncasecmp(what, *strptr, strlen(*strptr))) {
			isstruct_head = TRUE;
			break;
		}
	} while (*(++strptr) != 0);

	t = buf;
	encoding = which_encoding(what);
	ew_taken_len = strlen(tinrc.mm_charset) + 7;		/* the minimum encoded word length without any encoded text */

	while (*what) {
		if (break_long_line)
			word_cnt++;
/*
 * if a word with 8bit chars is broken in the middle, whatever follows
 * after the point where it's split should be encoded (i.e. even if
 * they are made of only 7bit chars)
 */
		if (contains_nonprintables(what, isstruct_head) || isbroken_within) {
			if (encoding == 'Q') {
				if (!quoting) {
					sprintf(buf2, "=?%s?%c?", tinrc.mm_charset, encoding);
					ewsize = mystrcat(&t, buf2);
					if (break_long_line) {
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
				isbroken_within = FALSE;
				while (*what && !isbetween(*what, isstruct_head)) {
#if 0
					if (is_EIGHT_BIT(what) || (strchr (RFC2047_ESPECIALS, *what))) {
#else
					if (is_EIGHT_BIT(what) || !isalnum((int)*what)) {
#endif /* 0 */
						sprintf(buf2, "=%2.2X", *EIGHT_BIT(what));
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
						isbroken_within = TRUE;
						break;
					}
				}
				if (!contains_nonprintables(what, isstruct_head) || ewsize >= 70 - strlen(tinrc.mm_charset)) {
					/* next word is 'clean', close encoding */
					*t++ = '?';
					*t++ = '=';
					ewsize += 2;
/*
 * if a word with 8bit chars is broken in the middle, whatever follows
 * after the point where it's split should be encoded (i.e. even if
 * they are made of only 7bit chars)
 */
					if (ewsize >= 70 - strlen(tinrc.mm_charset) && (contains_nonprintables(what, isstruct_head) || isbroken_within)) {
						*t++ = ' ';
						ewsize++;
					}
					quoting = FALSE;
				} else {
					/* process whitespace in-between by quoting it properly */
					while (*what && isspace((unsigned char) *what)) {
						if (*what == 32 /* not ' ', compare chapter 4! */ ) {
							*t++ = '_';
							ewsize++;
						} else {
							sprintf(buf2, "=%2.2X", *EIGHT_BIT(what));
							*t++ = buf2[0];
							*t++ = buf2[1];
							*t++ = buf2[2];
							ewsize += 3;
						}
						what++;
					}					  /* end of while */
				}						  /* end of else */
			} else {					  /* end of Q encoding and beg. of B encoding */
				/*
				 * if what immediately precedes the current fragment with 8bit char is
				 * encoded word, the leading spaces should be encoded together with
				 * 8bit chars following them. No need to worry about '(',')' and '"'
				 * as they're already excluded with contain_nonprintables used in outer if-clause
				 */
				while (*what && (!isbetween(*what, isstruct_head) || rightafter_ew)) {

					sprintf(buf2, "=?%s?%c?", tinrc.mm_charset, encoding);
					ewsize = mystrcat(&t, buf2);

					if (word_cnt == 2)
						ewsize = t - buf;
					what += do_b_encode(what, buf2, 75 - ew_taken_len, isstruct_head);
					ewsize += mystrcat(&t, buf2);
					*t++ = '?';
					*t++ = '=';
					*t++ = ' ';
					ewsize += 3;
					if (break_long_line)
						word_cnt++;
					rightafter_ew = FALSE;
					any_quoting_done = TRUE;
				}
				rightafter_ew = TRUE;
				word_cnt--;			  /* compensate double counting */
				/*
				 * if encoded word is followed by 7bit-only fragment, we need to
				 * eliminate ' ' inserted in while-block above
				 */
				if (!contains_nonprintables(what, isstruct_head)) {
					t--;
					ewsize--;
				}
			}							  /* end of B encoding */
		} else {
			while (*what && !isbetween(*what, isstruct_head))
				*t++ = *what++;	  /* output word unencoded */
			while (*what && isbetween(*what, isstruct_head))
				*t++ = *what++;	  /* output trailing whitespace unencoded */
			rightafter_ew = FALSE;
		}
	}									  /* end of pass 1 while loop */
	*t = 0;
	/* Pass 2: break long lines if there are MIME-sequences in the result */
	c = buf;
	if (break_long_line) {
		column = 0;
		if (any_quoting_done) {
			word_cnt = 1;			  /*
											* note, if the user has typed a
										   * continuation line, we will consider the
										   * initial whitespace to be delimiting
										   * word one (well, just assume an empty
										   * word).
										   */
			while (*c) {
				if (isspace((unsigned char) *c)) {
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
					if (sizeofnextword(c) + column > 76 && word_cnt != 1) {
						*((*where)++) = '\n';
						column = 0;
					}
					if (c > buf && !isspace((unsigned char) *(c - 1)))
						word_cnt++;
					*((*where)++) = *c++;
					column++;
				} else
					while (*c && !isspace((unsigned char) *c)) {
						*((*where)++) = *c++;
						column++;
					}
			}
		} else {
			while (*c)
				*((*where)++) = *c++;
		}
	} else {							  /* !break_long_line */
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
	char *b;
	int x;
	static char buf[2048];

/*
 * break_long_line is  FALSE for news posting unless MIME_BREAK_LONG_LINES
 * is defined, but it's TRUE for mail messages regardless of whether or not
 * MIME_BREAK_LONG_LINES is defined
 */
#ifdef MIME_BREAK_LONG_LINES
	t_bool break_long_line = TRUE;
#else
	t_bool break_long_line = FALSE;
#endif /* MIME_BREAK_LONG_LINES */

/*
 * Even if MIME_BREAK_LONG_LINES is NOT defined,
 * long headers in mail messages should be broken up in
 * accordance with RFC 2047(1522)
 */
#ifndef MIME_BREAK_LONG_LINES
	if (ismail)
		break_long_line = TRUE;
#endif /* !MIME_BREAK_LONG_LINES */

	get_mm_charset();
	b = buf;
	x = rfc1522_do_encode(s, &b, break_long_line);
	quoteflag = quoteflag || x;
	return buf;
}


void
rfc15211522_encode (
	char *filename,
	constext * mime_encoding,
	t_bool allow_8bit_header,
	t_bool ismail)
{
	FILE *f;
	FILE *g;
	char *c, *d;
	char encoding;
	char buffer[2048];
	char header[4096];
	t_bool body_encoding_needed = FALSE;
	t_bool umlauts = FALSE;
	BodyPtr body_encode;

	g = tmpfile();
	if (!g)
		return;
	f = fopen(filename, "r");
	if (!f) {
		fclose(g);
		return;
	}
	header[0] = 0;
	d = header;
	quoteflag = 0;
	while (fgets(buffer, 2048, f)) {
#ifdef LOCAL_CHARSET
		buffer_to_network(buffer);
#endif /* LOCAL_CHARSET */
		if (header[0]
			 && (!isspace((unsigned char) buffer[0]) || isreturn(buffer[0]))) {
			if (allow_8bit_header)
				fputs(header, g);
			else
				fputs(rfc1522_encode(header, ismail), g);
			fputc('\n', g);
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
	fputc('\n', g);
	while (fgets(buffer, 2048, f)) {
#ifdef LOCAL_CHARSET
		buffer_to_network(buffer);
#endif /* LOCAL_CHARSET */
		fputs(buffer, g);
		/* see if there are any umlauts in the body... */
		for (c = buffer; *c && !isreturn(*c); c++)
			if (is_EIGHT_BIT(c)) {
				umlauts = TRUE;
				body_encoding_needed = TRUE;
				break;
			}
	}
	fclose(f);
	rewind(g);
	f = fopen(filename, "w");
	if (!f) {
		fclose(g);
		return;
	}
	while (fgets(buffer, 2048, g) && !isreturn(buffer[0]))
		fputs(buffer, f);

	/* now add MIME headers as necessary */
#if 0									  /* RFC1522 does not require MIME headers just because there are
										     encoded header lines */
	if (quoteflag || umlauts) {
#else
	if (umlauts) {
#endif /* 0 */
		fputs("MIME-Version: 1.0\n", f);
		if (body_encoding_needed) {

		/* added for CJK charsets like EUC-KR/JP/CN and others */

			if (!strncasecmp(tinrc.mm_charset, "euc-", 4) &&
				 !strcasecmp(mime_encoding, txt_7bit))
				fprintf(f, "Content-Type: text/plain; charset=ISO-2022-%s\n", &tinrc.mm_charset[4]);
			else
				fprintf(f, "Content-Type: text/plain; charset=%s\n", tinrc.mm_charset);
			fprintf(f, "Content-Transfer-Encoding: %s\n", mime_encoding);
		} else {
			fputs("Content-Type: text/plain; charset=US-ASCII\n", f);
			fputs("Content-Transfer-Encoding: 7bit\n", f);
		}
	}
	fputc('\n', f);

	if (!strcasecmp(mime_encoding, txt_base64))
		encoding = 'b';
	else if (!strcasecmp(mime_encoding, txt_quoted_printable))
		encoding = 'q';
	else
		encoding = '8';

	if (!body_encoding_needed)
		encoding = '8';

/* added for CJK charsets like EUC-KR/JP/CN and others */

	if (!strcasecmp(mime_encoding, txt_7bit)) {
		encoding = '7';

/* For EUC-KR, 7bit means conversion to ISO-2022-KR specified in RFC 1557 */

		if (!strcasecmp(tinrc.mm_charset, "euc-kr"))
			body_encode = rfc1557_encode;

#if 0
/*
 * Not only EUC-JP but also other Japanese charsets such as
 * SJIS  might need RFC 1468 encoding. To be confirmed.
 */
		else if (!strcasecmp(tinrc.mm_charset, "euc-jp"))
			body_encode = rfc1468_encode;

/*
 * Not only  EUC-CN but also other Chinese charsets such as
 * BIG5 and EUC-TW might need RFC 1922 encoding. To be confirmed.
 */
		else if (!strcasecmp(tinrc.mm_charset, "euc-cn"))
			body_encode = rfc1922_encode;
#endif /* 0 */
		else {
			body_encode = rfc1521_encode;
			encoding = '8';
		}
	} else
		body_encode = rfc1521_encode;

	while (fgets(buffer, 2048, g))
		body_encode(buffer, f, encoding);

	if (encoding == 'b' || encoding == 'q' || encoding == '7')
		body_encode(NULL, f, encoding);	/* flush */

	fclose(g);
	fclose(f);
}
