/*
 *  Project   : tin - a Usenet reader
 *  Module    : string.c
 *  Author    : Urs Janssen <urs@tin.org>
 *  Created   : 1997-01-20
 *  Updated   : 1997-12-31
 *  Notes     :
 *  Copyright : (c) Copyright 1997-99 by Urs Janssen
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */

/*
 * this file needs some work
 */

/*
 * special ltoa()
 * converts value into a string with a len of digits
 * last char may be one of the following
 * Kilo, Mega, Giga, Terra
 */
char *
tin_ltoa (
	long value,
	int digits)
{
	static char buffer[256];
	static const char power[]=" KMGT";
	int len;
	int i=0;

	sprintf (buffer, "%ld", value);
	len = (int) strlen (buffer);

	while (len > digits) {
		len -= 3;
		i++;
	}

	if (i) {
		while (len < (digits-1))
			buffer[len++] = ' ';

		buffer[digits-1] = power[i];
		buffer[digits] = '\0';
	} else
		sprintf (buffer, "%*ld", digits, value);

	return (buffer);
}


/*
 * Handrolled version of strdup(), presumably to take advantage of
 * the enhanced error detection in my_malloc
 *
 * also, strdup is not mandatory in ANSI-C
 */
char *my_strdup (
	const char *str)
{
	size_t len = strlen (str) + 1;
	void *ptr = my_malloc (len);

	if (ptr == NULL)
		return NULL;

	memcpy (ptr, str, len);
	return (char *) ptr;
}


/*
 *  strncpy that stops at a newline and null terminates
 */
void
my_strncpy (
	char *p,
	const char *q,
	size_t n)
{
	while (n--) {
		if (!*q || *q == '\n')
			break;
		*p++ = *q++;
	}
	*p = '\0';
}


/* this strcpy variant removes \n and "" */
void
strcpynl (
	char *to,
	const char *from)
{
	while (*from && *from != '\r' && *from != '\n') {
		if (*from == '"')
			from++;	/* this is just plain silly */
		else
			*to++ = *from++;
	}
	*to = 0;
}


/*
 * case-insensitive version of strstr()
 */
char *
strcasestr (
	char *haystack,
	const char *needle)
{
	char *h;
	const char *n;

	h = haystack;
	n = needle;
	while (*haystack) {
		if (tolower ((unsigned char)*h) == tolower ((unsigned char)*n)) {
			h++;
			n++;
			if (!*n)
				return haystack;
		} else {
			h = ++haystack;
			n = needle;
		}
	}
	return NULL;
}


size_t
mystrcat (
	char **t,
	const char *s)
{
	size_t len = 0;

	while (*s) {
		*((*t)++) = *s++;
		len++;
	}
	**t = 0;
	return len;
}


void
modifiedstrncpy (
	char *target,
	const char *source,
	size_t size,
	int decode)
{
	char buf[2048];
	int count;
	char *c;

	count = sizeof(buf)-1;
	c = buf;
	while (*source) {
		if (*source!= 1) {
			*c++ = *source++;
			if (!--count)
				break;
		}
		else source++;
	}
	*c = 0;
	c = (decode ? (rfc1522_decode(buf)) : buf);

	while (--size)
		*target++ = *c++;

	*target = 0;
}


void
str_lwr (
	char *dst,
	const char *src)
{
	while (*src) {
		*dst++ = (char)tolower((unsigned char)*src);
		src++;
	}
	*dst = '\0';
}


/*
 * normal systems come with these...
 */

#ifndef HAVE_STRPBRK
/*
 * find first occurrence of any char from str2 in str1
 */
char *
strpbrk (
	char *str1,
	char *str2)
{
	register char *ptr1;
	register char *ptr2;

	for (ptr1 = str1; *ptr1 != '\0'; ptr1++) {
		for (ptr2 = str2; *ptr2 != '\0';) {
			if (*ptr1 == *ptr2++)
				return (ptr1);
		}
	}
	return (char *) 0;
}
#endif /* !HAVE_STRPBRK */

#ifndef HAVE_STRSTR
/*
 * ANSI C strstr () - Uses Boyer-Moore algorithm.
 */
char *
strstr (
	char *text,
	char *pattern)
{
	register unsigned char *p, *t;
	register int i, j, *delta;
	register size_t p1;
	int deltaspace[256];
	size_t textlen;
	size_t patlen;

	textlen = strlen (text);
	patlen = strlen (pattern);

	/* algorithm fails if pattern is empty */
	if ((p1 = patlen) == 0)
		return (text);

	/* code below fails (whenever i is unsigned) if pattern too long */
	if (p1 > textlen)
		return (NULL);

	/* set up deltas */
	delta = deltaspace;
	for (i = 0; i <= 255; i++)
		delta[i] = p1;
	for (p = (unsigned char *) pattern, i = p1; --i > 0;)
		delta[*p++] = i;

	/*
	 * From now on, we want patlen - 1.
	 * In the loop below, p points to the end of the pattern,
	 * t points to the end of the text to be tested against the
	 * pattern, and i counts the amount of text remaining, not
	 * including the part to be tested.
	 */
	p1--;
	p = (unsigned char *) pattern + p1;
	t = (unsigned char *) text + p1;
	i = textlen - patlen;
	forever {
		if (*p == *t && memcmp ((p - p1), (t - p1), p1) == 0)
			return ((char *)t - p1);
		j = delta[*t];
		if (i < j)
			break;
		i -= j;
		t += j;
	}
	return (NULL);
}
#endif /* !HAVE_STRSTR */

#ifndef HAVE_ATOL
/*
 * handrolled atol
 */
long
atol (
	const char *s)
{
	long ret = 0;

	/* skip leading whitespace(s) */
	while (*s && isspace((unsigned char) *s))
		s++;

	while (*s) {
		if (isdigit(*s))
			ret = ret * 10 + (*s - '0');
		else
			return -1;
		s++;
	}
	return ret;
}
#endif /* !HAVE_ATOL */

#ifndef HAVE_STRTOL
/* fix me - put me in tin.h */
#define DIGIT(x) (isdigit((unsigned char)x)? ((x)-'0'): (10+tolower((unsigned char)x)-'a'))
#define MBASE 36
long
strtol (
	const char *str,
	char **ptr,
	int use_base)
{
	long val = 0L;
	int xx = 0, sign = 1;

	if (use_base < 0 || use_base > MBASE)
		goto OUT;
	while (isspace ((unsigned char)*str))
		++str;
	if (*str == '-') {
		++str;
		sign = -1;
	} else if (*str == '+')
		++str;
	if (use_base == 0) {
		if (*str == '0') {
			++str;
			if (*str == 'x' || *str == 'X') {
				++str;
				use_base = 16;
			} else
				use_base = 8;
		} else
			use_base = 10;
	} else if (use_base == 16)
		if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
			str += 2;
		/*
		 * for any base > 10, the digits incrementally following
		 * 9 are assumed to be "abc...z" or "ABC...Z"
		 */
		while (isalnum ((unsigned char)*str) && (xx = DIGIT(*str)) < use_base) {
			/* accumulate neg avoids surprises near maxint */
			val = use_base * val - xx;
			++str;
		}
OUT:
	if (ptr != (char **) 0)
		*ptr = str;

	return (sign * (-val));
}
#undef DIGIT(x)
#undef MBASE
#endif /* !HAVE_STRTOL */


/*
 *  strcmp that ignores case
 */

/*fix me - put me in tin.h */
#define	FOLD_TO_UPPER(a)	(toupper ((unsigned char)(a)))

#ifndef HAVE_STRCASECMP
int
strcasecmp (
	const char *p,
	const char *q)
{
	int r;
	for (; (r = FOLD_TO_UPPER (*p) - FOLD_TO_UPPER (*q)) == 0; ++p, ++q) {
		if (*p == '\0')
			return 0;
	}

	return r;
}
#endif /* !HAVE_STRCASECMP */

#ifndef HAVE_STRNCASECMP
int
strncasecmp (
	const char *p,
	const char *q,
	size_t n)
{
	int r = 0;
	for (; n && (r = (FOLD_TO_UPPER (*p) - FOLD_TO_UPPER (*q))) == 0; ++p, ++q, --n) {
		if (*p == '\0')
			return 0;
	}
	return n ? r : 0;
}
#endif /* !HAVE_STRNCASECMP */


/*
 * str_trim - substitute multiple white spaces with one space
 *            and delete heading and trailing whitespace
 *
 * INPUT:  string  - string to trim
 *
 * OUTPUT: string  - trimmed string
 *
 * RETURN: trimmed string
 */
char *
str_trim (
	char *string)
{
	char *rp;		/* reading string pointer */
	char *wp;		/* writing string pointer */
	t_bool ws = TRUE;		/* white space flag */

	for (wp = rp = string; *rp; rp++) {		/* loop over string */
		if (isspace((int)*rp)) {		/* is it a white space? */
			if (!ws) {		/* was the last character not a white space? */
				*wp++ = ' ';		/* store a blank */
				ws = TRUE;
			}
		} else /* no white space */ {
			*wp++ = *rp;		/* store the character */
			ws = FALSE;
		}
	}
	if (ws)		/* delete trailing blank */
		wp--;

	*wp = '\0';

	return (string);
}


/*
 *  Return a pointer into s eliminating any TAB, CR and LF.
 */
char *
eat_tab (
	char *s)
{
	char *p1 = s;
	char *p2 = s;

	while (*p1) {
		if (*p1 == '\t' || *p1 == '\r' || *p1 == '\n') {
			p1++;
		} else if (p1 != p2) {
			*p2++ = *p1++;
		} else {
			p1++;
			p2++;
		}
	}
	if (p1 != p2)
		*p2 = '\0';

	return s;
}


/*
 * Format a shell command, escaping blanks and other awkward characters that
 * appear in the string arguments.  Replaces sprintf, except that we pass in
 * the buffer limit, so we can refrain from trashing memory on very long
 * pathnames.
 *
 * Returns the number of characters written (not counting null), or -1 if there
 * is not enough room in the 'dst' buffer.
 */

#define SH_FORMAT(c)	if (++result >= (int) len) \
				break; \
			*dst++ = c

#define SH_SINGLE "\\\'"
#define SH_DOUBLE "\\\'\"`$"
#define SH_META   "\\\'\"`$*%?()[]{}|<>^&;#~"

int
sh_format (
	char *dst,
	size_t len,
	const char *fmt,
	...)
{
	int result = 0;
	char *src;
	char temp[20];
	va_list ap;
	int quote = 0;

	va_start(ap, fmt);

	while (*fmt != 0) {
		int ch = *fmt++;

		if (ch == '\\') {
			SH_FORMAT(ch);
			if (*fmt != '\0') {
				SH_FORMAT(*fmt++);
			}
			continue;
		} else if (ch == '%') {
			if (*fmt == '\0') {
				SH_FORMAT('%');
				break;
			}

			switch (*fmt++) {
			case '%':
				src = strcpy(temp, "%");
				break;
			case 's':
				src = va_arg(ap, char *);
				break;
			case 'd':
				sprintf(temp, "%d", va_arg(ap, int));
				src = temp;
				break;
			default:
				src = strcpy(temp, "");
				break;
			}

			while (*src != '\0') {
				t_bool fix;

				/*
				 * This logic works for Unix.  Non-Unix systems
				 * may require a different set of problem
				 * chars, and may need quotes around the whole
				 * string rather than escaping individual
				 * chars.
				 */
				if (quote == '"') {
					fix = (strchr(SH_DOUBLE, *src) != 0);
				} else if (quote == '\'') {
					fix = (strchr(SH_SINGLE, *src) != 0);
				} else
					fix = (strchr(SH_META, *src) != 0);
				if (fix) {
					SH_FORMAT('\\');
				}
				SH_FORMAT(*src++);
			}
		} else {
			if (quote) {
				if (ch == quote)
					quote = 0;
			} else {
				if (ch == '"' || ch == '\'')
					quote = ch;
			}
			SH_FORMAT(ch);
		}
	}
	va_end(ap);

	if (result+1 >= (int) len)
		result = -1;
	else
		*dst = '\0';

	return (result);
}

#ifndef HAVE_STRERROR
#	ifdef HAVE_SYS_ERRLIST
#		ifdef M_AMIGA
#			ifndef sys_errlist
				extern char *__sys_errlist[];
#				define sys_errlist	__sys_errlist
#			endif /* !sys_errlist */
#		else
#			ifdef DECL_SYS_ERRLIST
				extern char *sys_errlist[];
#			endif /* DECL_SYS_ERRLIST */
#		endif /* M_AMIGA */
		extern int sys_nerr;
#	endif /* HAVE_SYS_ERRLIST */

char *
my_strerror (
	int n)
{
	static char temp[32];
#	ifdef HAVE_SYS_ERRLIST
	if (n >= 0 && n < sys_nerr)
		return sys_errlist[n];
#	endif /* HAVE_SYS_ERRLIST */
	sprintf(temp, "Errno: %i", n);
	return temp;
}
#endif /* !HAVE_STRERROR */
