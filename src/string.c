/*
 *  Project   : tin - a Usenet reader
 *  Module    : string.c
 *  Author    : 
 *  Created   : 20.01.1997
 *  Updated   : 
 *  Notes     :
 *  Copyright : (c) Copyright 1997 by Urs Janssen
 *              You may  freely  copy or  redistribute	this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"


/*
** this file needs some work
*/


/*
 * special itoa()
 * converts value into a string with a len of digits
 * last char may be one of the following
 * Kilo, Mega, Giga, Terra
 */

char *
tin_itoa (value, digits)
	int value;	/* change to long int if needed */
	int digits;
{
	static char buffer[256];
	static char power[]=" KMGT";
	int len;
	int i=0;

	sprintf (buffer, "%d", value);
	len = (int) strlen (buffer);

	while (len > digits) {
		len-=3;
		i++;
	}

	if (i) {
		while (len < (digits-1)) {
			buffer[len++]=' ';
		}
		buffer[digits-1] = power[i];
		buffer[digits] = '\0';
	} else {
		sprintf(buffer, "%*d", digits, value);
	}
	return (buffer);
}



/*
 * Handrolled version of strdup(), presumably to take advantage of
 * the enhanced error detection in my_malloc
 *
 * also, strdup is not mandatory in ANSI-C
 */
char *my_strdup (str)
	char *str;
{
	char *duplicate = (char *) 0;

	if (str != (char *) 0) {
		duplicate = (char *) my_malloc (strlen (str)+2);
		duplicate[strlen(str)+1]= '\0';
		strcpy (duplicate, str);
	}
	return duplicate;
}



/*
 *  strncpy that stops at a newline and null terminates
 */

void
my_strncpy (p, q, n)
	char *p;
	/* const */ char *q;
	int n;		/* we should use size_t instead of int */
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
			if (!*n)
				return haystack;
		} else {
			h = ++haystack;
			n = needle;
		}
	}
	return NULL;
}

int
mystrcat (t, s)
	char **t;
	char *s;
{
	int len = 0;

	while (*s) {
		*((*t)++) = *s++;
		len++;
	}
	**t = 0;
	return len;
}

void modifiedstrncpy(target, source, size, decode)
char *target;
char *source;
size_t size;
int decode;
{
	char buf[2048];
	int count;
	char *c;

	count = sizeof(buf)-1;
	c = buf;
	while (*source) {
		if (*source!= 1) {
			*c++ = *source++;
			if (!--count) break;
		}
		else source++;
	}
	*c = 0;
	if(decode)
		c = rfc1522_decode(buf);
	else
		c = buf;

	while (--size) {
	        *target++ = *c++;
	}
	*target = 0;
}


void
str_lwr (src, dst)
	char *src;
	char *dst;
{
	while (*src) {
		*dst++ = (char)tolower((int)*src);
		src++;
	}
	*dst = '\0';
}


/*
** normal systems come with these...
*/

#ifndef HAVE_STRPBRK
/*
**  find first occurrence of any char from str2 in str1
*/

char *
strpbrk (str1, str2)
	char *str1;
	char *str2;
{
	register char *ptr1;
	register char *ptr2;

	for (ptr1 = str1; *ptr1 != '\0'; ptr1++) {
		for (ptr2 = str2; *ptr2 != '\0';) {
			if (*ptr1 == *ptr2++) {
				return (ptr1);
			}
		}
	}
	return (char *) 0;
}
#endif

#ifndef HAVE_STRSTR
/*
 * ANSI C strstr () - Uses Boyer-Moore algorithm.
 */
char *
strstr (text, pattern)
	char *text;
	char *pattern;
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
#endif

#ifndef HAVE_ATOL
/*
** handrolled atol
*/
long
atol (s)
	char *s;
{
	long ret = 0;
	while (*s) {
		if (*s >= '0' && *s <= '9')
			ret = ret * 10 + (*s - '0');
		else
			return -1;
		s++;
	}
	return ret;
}
#endif /* HAVE_ATOL */

#ifndef HAVE_STRTOL
/* fix me - put me in tin.h */
#define DIGIT(x) (isdigit((unsigned char)x)? ((x)-'0'): (10+tolower(x)-'a'))
#define MBASE 36
long
strtol (str, ptr, use_base)
	/* const */ char *str;
	char **ptr;
	int use_base;
{

	long	val;
	int	xx, sign;

	val = 0L;
	sign = 1;

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
		while (isalnum ((unsigned char)*str) && (xx = DIGIT (*str)) < use_base) {
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
#endif /* HAVE_STRTOL */


/*
 *  strcmp that ignores case
 */

/*fix me - put me in tin.h */
#define FOLD_TO_UPPER(a)	(toupper ((int) (a)))

#ifndef HAVE_STRCASECMP
int
strcasecmp (p, q)
	/* const */ char *p;
	/* const */ char *q;
{
	int r;
	for (; (r = FOLD_TO_UPPER (*p) - FOLD_TO_UPPER (*q)) == 0; ++p, ++q) {
		if (*p == '\0') {
			return (0);
		}
	}

	return r;
}
#endif

#ifndef HAVE_STRNCASECMP
int
strncasecmp(p, q, n)
	/* const */ char *p;
	/* const */ char *q;
	size_t n;
{
	int r=0;
	for (; n && (r = (FOLD_TO_UPPER (*p) - FOLD_TO_UPPER (*q))) == 0;
			++p, ++q, --n) {
		if (*p == '\0') {
			return (0);
		}
	}
	return n ? r : 0;
}
#endif