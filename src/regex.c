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
#ifdef HAVE_POSIX_REGEX

#include "tin.h"
#include <regex.h>

/*
 * regexec error routine to return an error message in the 'msg' global
 */
static void
regex_error(
	int error,
	regex_t preg)
{
	char errmsg[LEN];
	size_t size;

	if ((size = regerror(error, &preg, errmsg, sizeof(errmsg))) > sizeof(errmsg))
		sprintf(msg, "Start of regex error: %s", errmsg);
	else
		sprintf(msg, "Error in regex: %s", errmsg);

	return;
}

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
	int flags = REG_NOSUB | REG_EXTENDED;
	int error, ret = FALSE;
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

		case REG_NOMATCH:						/* Not found */
			break;

		default:								/* Something wrong... */
			regex_error(error, preg);
	}

	regfree(&preg);
	return(ret);
}

#endif /* HAVE_POSIX_REGEX */
