/*
 *  Project   : tin - a Usenet reader
 *  Module    : rfc1521.c
 *  Author    : Chris Blum <chris@phil.uni-sb.de>
 *  Created   : September '95
 *  Updated   : 27-05-96
 *  Notes     : MIME text/plain support
 *  Copyright : (c) Copyright 1995-96 by Chris Blum
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made.
 */

#include "tin.h"

/* this strcpy variant removes \n and "" */
void
strcpynl (to, from)
	char *to;
	char *from;
{
	while (*from && *from != '\r' && *from != '\n') {
		if (*from == '"')
			from++;	/* this is just plain silly */
		else
			*to++ = *from++;
	}
	*to = 0;
}

unsigned char
bin2hex (x)
	unsigned int x;
{
	if (x < 10)
		return x + '0';
	return x - 10 + 'A';
}

char *
strcasestr (haystack, needle)
	char *haystack;
	char *needle;
{
	char *h, *n;

	h = haystack;
	n = needle;
	while (*haystack) {
		if (tolower (*h) == tolower (*n)) {
			h++;
			n++;
			if (! *n)
				return haystack;
		} else {
			h = ++haystack;
			n = needle;
		}
	}
	return NULL;
}

/* KNOWN BUG: this function is also called before piping and saving
   articles, so these get saved with incorrect MIME headers */

FILE *
rfc1521_decode (file)
	FILE *file;
{
	FILE *f;
	char buf[2048];
	char buf2[2048];
	char content_type[128];
	char content_transfer_encoding[128];
	char *charset;
	char encoding = '\0';

	if (! file)
		return file;
	f = tmpfile ();
	if (! f)
		return file;

	content_type[0] = '\0';
	content_transfer_encoding[0] = '\0';

	/* pass article header unchanged */
	while (fgets (buf, sizeof (buf), file)) {
#ifdef LOCAL_CHARSET
		buffer_to_local(buf);
#endif
		fputs (buf, f);
		/* NOTE:  I know it is a bug not to merge header lines starting
		 * with whitespace to the preceding one, but I guess we can
		 * live with that here.
		 */
		if (! strncasecmp (buf, "Content-Type: ", 14)) {
			strcpynl (content_type, buf + 14);
		} else if (! strncasecmp (buf, "Content-Transfer-Encoding: ", 27)) {
			strcpynl (content_transfer_encoding, buf + 27);
		}
		if (*buf == '\r' || *buf == '\n')
			break;
	}
#ifndef LOCAL_CHARSET
	/*
	 * if we have a different local charset, we also convert articles
	 * that do not have MIME headers, since e.g. quoted text may contain
	 * accented chars on non-MIME newsreaders.
	 */

	/* no MIME headers, no decoding */
	if (!*content_transfer_encoding) {
		fclose (f);
		rewind (file);
		return file;
	}
	
	/*
	 * see if type text/plain. if content-type is empty,
	 *	"text/plain; charset=us-ascii" is implicit.
	 */
	if (*content_type && strncasecmp(content_type, "text/plain", 10)!=0) {
		fclose (f);
		rewind (file);
		return file;
	}
#endif

	/*
	 * see if charset matches (we do not attempt to convert charsets at
	 * this point of time - maybe in the future)
	 */
	charset = strcasestr (content_type, "charset=");
	if (! charset)
		charset = "US-ASCII";
	else
		charset += 8;
	get_mm_charset ();

	/* see if content transfer encoding requires decoding anyway */
	if (! strcasecmp (content_transfer_encoding, txt_base64))
		encoding = 'b';
	else if (! strcasecmp (content_transfer_encoding, txt_quoted_printable))
		encoding = 'q';

	if (encoding) {
		int i;

		if (encoding == 'b')
			mmdecode (NULL, 'b', 0, NULL, NULL);	/* flush */
		while (fgets (buf, sizeof (buf), file)) {
			i = mmdecode (buf, encoding, '\0', buf2, charset);
			if (i >= 0)
				buf2[i] = '\0';
			else
				strcpy (buf2, buf);
#ifdef LOCAL_CHARSET
			buffer_to_local(buf2);
#endif
			fputs (buf2, f);
		}
		fclose (file);
		rewind (f);
		return f;
	}
#ifdef LOCAL_CHARSET
	/* if we have a different local charset, we also have to convert
	   8bit articles (and we also convert 7bit articles thay may contain
	   accented characters due to incorrectly configured newsreaders */
	while (fgets(buf, 2048, file)) {
		buffer_to_local(buf);
		fputs(buf, f);
	}
	
	fclose(file);
	rewind(f);
	return f;
#else
	fclose (f);
	rewind (file);
	return file;
#endif
}

#define HI4BITS(c) (unsigned)(*EIGHT_BIT(c) >> 4)
#define LO4BITS(c) (unsigned)(*c & 0xf)

/* A MIME replacement for fputs.  e can be 'b' for base64, 'q' for
 * quoted-printable, or 8 (default) for 8bit.  Long lines get broken in
 * encoding modes.  If line is the null pointer, flush internal buffers.
 */
void
rfc1521_encode (line, f, e)
	char *line;
	FILE *f;
	int e;
{
	static char buffer[80];	/* they must be static for base64 */
	static char *b = NULL;
	static int xpos = 0;
	static unsigned long pattern = 0;
	static int bits = 0;
	int i;

	if (e == 'b') {
		if (! b) {
			b = buffer;
			*buffer = '\0';
		}
		if (! line) {	/* flush */
			if (bits) {
				if (xpos >= 73) {
					*b++ = '\n';
					*b = 0;
					fputs (buffer, f);
					b = buffer;
					xpos = 0;
				}
				pattern <<= 24 - bits;
				for (i = 0; i < 4; i++) {
					if (bits >= 0) {
						*b++ = base64_alphabet[(pattern & 0xfc0000) >> 18];
						pattern <<= 6;
						bits -= 6;
					} else
						*b++ = '=';
					xpos++;
				}
				pattern = 0;
			}
			if (xpos) {
				*b = 0;
				fputs (buffer, f);
				xpos = 0;
			}
			b = NULL;
		} else {
			while (*line) {
				pattern <<= 8;
				pattern |= *EIGHT_BIT(line)++;
				bits += 8;
				if (bits >= 24) {
					if (xpos >= 73) {
						*b++ = '\n';
						*b = 0;
						b = buffer;
						xpos = 0;
						fputs (buffer, f);
					}
					for (i = 0; i < 4; i++) {
						*b++ = base64_alphabet[(pattern >> (bits - 6)) & 0x3f];
						xpos++;
						bits -= 6;
					}
					pattern = 0;
				}
			}
		}
	} else if (e == 'q') {
		if (! line) {
			/*
			 * we don't really flush anything in qp mode, just set
			 * xpos to 0 in case the last line wasn't terminated by
			 * \n.
			 */
			xpos = 0;
			return;
		}
		b = buffer;
		while (*line) {
			if (isspace (*line) && *line != '\n') {
				char *l = line + 1;

				while (*l) {
					if (! isspace (*l)) {	/* it's not trailing whitespace, no encoding needed */
						*b++ = *line++;
						xpos++;
						break;
					}
					l++;
				}
				if (! *l) {	/* trailing whitespace must be encoded */
					*b++ = '=';
					*b++ = bin2hex (HI4BITS(line));
					*b++ = bin2hex (LO4BITS(line));
					xpos += 3;
					line++;
				}
			} else if ((!is_EIGHT_BIT(line) && *line != '=')
				   || (*line == '\n')) {
				*b++ = *line++;
				xpos++;
				if (*(line - 1) == '\n')
					break;
			} else {
				*b++ = '=';
				*b++ = bin2hex (HI4BITS(line));
				*b++ = bin2hex (LO4BITS(line));
				xpos += 3;
				line++;
			}
			if (xpos > 73 && *line != '\n') {	/* 73 +3 [worst case] = 76 :-) */
				*b++ = '=';	/* break long lines with a 'soft line break' */
				*b++ = '\n';
				*b++ = '\0';
				fputs (buffer, f);
				b = buffer;
				xpos = 0;
			}
		}
		*b = 0;
		if (b != buffer)
			fputs (buffer, f);
		if (b != buffer && b[-1] == '\n')
			xpos = 0;
	} else if (line)
		fputs (line, f);
}

/*
 * KS C 5601 -> ISO 2022-KR encoding for Korean mail exchange
 * NOT to be used for News posting, which is made certain
 * by setting post_mime_encoding to 8bit
 */

#define KSC 1
#define ASCII 0
#define isksc(c)	((unsigned char) (c) > (unsigned char) '\240' && \
			(unsigned char) (c) < (unsigned char) '\377')
#define SI '\017'
#define SO '\016'

void
rfc1557_encode (line, f, e)
	char *line;
	FILE *f;
	int e;       /* dummy argument : not used */
{
	int i = 0;
        int mode = ASCII;
        static int iskorean = 0;


	if (! line) {
		iskorean = 0;
		return;
	}

	if (! iskorean) { /* search for KS C 5601 character(s) in line */
		while (line[i]) {
			if (isksc(line[i])) {
				iskorean = 1;               /* found KS C 5601 */
				fprintf(f, "\033$)C\n");   /* put out the designator */
				break;
			}
			i++;
		}
	}

	if (! iskorean) { /* KS C 5601 doesn't appear, yet -  no conversion */
		fputs(line, f);
		return;
	}

	i = 0;		/* back to the beginning of the line */
	while (line[i] && line[i] != '\n') {
		if (mode == ASCII && isksc(line[i])) {
			fputc(SO, f);
			fputc(0x7f & line[i], f);
			mode = KSC;
		}
		else if (mode == ASCII &&! isksc(line[i])) {
			fputc(line[i], f);
		}
		else if (mode == KSC && isksc(line[i])) {
			fputc(0x7f & line[i], f);
		} else {
			fputc(SI, f);
			fputc(line[i], f);
			mode = ASCII;
		}
		i++;
	}

	if (mode == KSC) {
		fputc(SI,f);
	}

	if (line[i] == '\n') {
		fputc('\n',f);
	}
	return;
}

/* Not yet implemented */
void
rfc1468_encode (line, f, e)
	char *line;
	FILE *f;
	int e;       /* dummy argument : not used */
{
	if (line)
		fputs(line, f);
}

/* Not yet implemented */
void
rfc1922_encode (line, f, e)
	char *line;
	FILE *f;
	int e;       /* dummy argument : not used */
{
	if (line)
		fputs(line, f);
}
