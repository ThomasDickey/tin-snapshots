/*
 *  Project   : tin - a Usenet reader
 *  Module    : regex.c
 *  Author    : Jason Faultless <jason@radar.tele2.co.uk>
 *  Created   : 1997-02-21
 *  Updated   : 1997-02-25
 *  Notes     : Regular expression subroutines
 *  Credits   :
 *  Copyright : (c) 1997-99 by Jason Faultless
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */

/*
 * See if pattern is matched in string. Return TRUE or FALSE
 * if icase=TRUE then ignore case in the compare
 */
t_bool
match_regex (
	const char *string,
	char *pattern,
	t_bool icase)
{
	const char *errmsg;
	int flags = PCRE_EXTENDED;
	int error;
	pcre *re;
	t_bool ret = FALSE;

	mesg[0] = '\0';

	if (icase)
		flags |= PCRE_CASELESS;

	/*
	 * Compile the expression internally.
	 */
	if ((re = pcre_compile(pattern, flags, &errmsg, &error, NULL)) == NULL) {
		sprintf(mesg, txt_pcre_error_at, errmsg, error);
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
	 *           out case), startoffset, options,
	 *           vector of offsets to be filled,
	 *           number of elements in offsets);
	 *
	 */
	error = pcre_exec(re, NULL, string, strlen(string), 0, 0, NULL, 0);
	if (error >= 0)
		ret = TRUE;
	else if (error == -1)
		ret = FALSE;
	else
		sprintf(mesg, txt_pcre_error_num, error);

	free(re);
	return(ret);
}


/*
 * Compile and optimise 'regex'. Return TRUE if all went well
 */
t_bool
compile_regex(
	char *regex,
	struct regex_cache *cache,
	int options)
{
	const char *regex_errmsg = 0;
	int regex_errpos;

	if ((cache->re = pcre_compile (regex, PCRE_EXTENDED | options, &regex_errmsg, &regex_errpos, NULL)) == NULL)
		error_message (txt_pcre_error_at, regex_errmsg, regex_errpos);
	else {
		cache->extra = pcre_study (cache->re, 0, &regex_errmsg);
		if (regex_errmsg != NULL)
			error_message (txt_pcre_error_text, regex_errmsg);
		else
			return TRUE;
	}
	return FALSE;
}

