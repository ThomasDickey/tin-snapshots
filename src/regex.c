/*
 *  Project   : tin - a Usenet reader
 *  Module    : regex.c
 *  Author    : Jason Faultless <jason@radar.demon.co.uk>
 *  Created   : 21-02-97
 *  Updated   : 25-02-97
 *  Notes     : Regular expression subroutines
 *  Credits   :
 *  Copyright : (c) 1997 by Jason Faultless
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"

#ifdef HAVE_REGEX_FUNCS

#ifdef HAVE_REGEX_H_FUNCS
#	include <regex.h>
	static void regex_error (int error, regex_t preg);
#else
#	undef RETURN
	static int reg_errno;

	static char * RegEx_Init (char *instring);
	static char * RegEx_Error (int code);
      
	static char *
	RegEx_Init (
		char *instring)
	{
		reg_errno = 0;
		return instring;
	}

	static char *
	RegEx_Error (
		int code)
	{
		reg_errno = code;
		return 0;
	}
#	define INIT 		   register char *sp = RegEx_Init(instring);
#	define GETC()		   (*sp++)
#	define PEEKC()		   (*sp)
#	define UNGETC(c)	   (--sp)
#	define RETURN(c)	   return(c)
#	define ERROR(c) 	   return RegEx_Error(c)
#	ifdef HAVE_REGEXP_H_FUNCS
#		include <regexp.h>
#	else
#		ifdef HAVE_REGEXPR_H_FUNCS
#			include <regexpr.h>
#		endif
#	endif
#endif

/* HP-UX doesn't define REG_NOERROR */
#ifndef REG_NOERROR
#	define REG_NOERROR 0
#endif

/*
 * regexec error routine to return an error message in the 'msg' global
 */
#ifdef HAVE_REGEX_H_FUNCS
static void
regex_error(
	int error,
	regex_t preg)
{
	char errmsg[LEN];
	size_t size;	/* 'unused' */

	if ((size = regerror(error, &preg, errmsg, sizeof(errmsg))) > sizeof(errmsg))
		sprintf(msg, "Start of regex error: %s", errmsg);
	else
		sprintf(msg, "Error in regex: %s", errmsg);

	return;
}
#endif

/*
 * See if pattern is matched in string. Return TRUE or FALSE
 * if icase=TRUE then ignore case in the compare
 */
t_bool
match_regex(
	const char *string,
	char *pattern,
	t_bool icase)
{
	int ret = FALSE;
#ifdef HAVE_REGEX_H_FUNCS
	int flags = REG_NOSUB | REG_EXTENDED;
	int error;
	regex_t preg;

	msg[0] = '\0';

	if (icase)
		flags |= REG_ICASE;

	/*
	 * Compile the expression internally.
	 */
	if ((error = regcomp(&preg, pattern, flags)) != 0) {
		regex_error(error, preg);
		return(FALSE);
	}

	/*
	 * Only a single compare is needed to see if a match exists
	 */
	switch (error = regexec(&preg, string, 0, NULL, 0)) {
		case REG_NOERROR:						/* Found */
			ret = TRUE;
		/* FALLTHROUGH */
		case REG_NOMATCH:						/* Not found */
			break;

		default:								/* Something wrong... */
			regex_error(error, preg);
	}

	regfree(&preg);
#else
	size_t buflen = BUFSIZ;
	char *buffer = malloc(buflen);
	char *preg;
#	ifdef HAVE_REGEXP_H_FUNCS
		preg = compile(pattern, buffer, buffer + buflen, 0);
#	else
		preg = compile(pattern, buffer, buffer + buflen);
#	endif
	ret = step(string, buffer);
	free(buffer);
#endif
	return(ret);
}

#endif /* HAVE_REGEX_FUNCS */
