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
 * See if pattern is matched in string. Return TRUE or FALSE
 * if icase=TRUE then ignore case in the compare
 */
t_bool
match_regex(
	const char *string,
	char *pattern,
	t_bool icase)
{
	char errmsg[LEN];
	int flags = REG_NOSUB | REG_EXTENDED;
	int error, ret = FALSE;
	size_t size;
	regex_t preg;
	
	if (icase)
		flags |= REG_ICASE;

	/*
	 * Compile the expression internally.
	 */
	if ((error = regcomp(&preg, pattern, flags)) != 0) {
/* TODO find way to keep this on screen, else it will be overwritten by 'no match' */
		if ((size = regerror(error, &preg, errmsg, sizeof(errmsg))) > sizeof(errmsg))
			error_message("Start of regex error: %s", errmsg);
		else
			error_message("Error in regex: %s", errmsg);

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
			if ((size = regerror(error, &preg, errmsg, sizeof(errmsg))) > sizeof(errmsg))
				error_message("Start of regex error: %s", errmsg);
			else
				error_message("Error in regex: %s", errmsg);
	}

	regfree(&preg);
	return(ret);
}

#endif /* HAVE_POSIX_REGEX */
