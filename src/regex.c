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
	int flags = PCRE_EXTENDED;
	int error;
	pcre *re;
	char *errmsg;

	msg[0] = '\0';

	if (icase)
		flags |= PCRE_CASELESS;

	/*
	 * Compile the expression internally.
	 */
	if ((re = pcre_compile(pattern, flags, &errmsg, &error)) == NULL) {
		sprintf(msg, "Error in regex: %s at pos. %d", errmsg, error);
		return(FALSE);
	}

	/*
	 * Since we are running the the compare only once,
	 * we don't need to use pcre_study() to improve
	 * performance
	 */

	/*
	 * Only a single compare is needed to see if a match exists
	 *
	 * pcre_exec(precompile pattern, hints pointer, string to match,
	 *           length of string (string may contain '\0', but not in
	 *           out case), options, vector of offsets to be filled,
	 *           number of elements in offsets);
	 *           
	 */
	error = pcre_exec(re, NULL, string, strlen(string), 0, NULL, 0);
	if (error >= 0)
		ret = TRUE;
	else if (error == -1)
		ret = FALSE;
	else
		sprintf(msg, "Error in regex: pcre internal error %d", error);

	free(re);
	return(ret);
}
