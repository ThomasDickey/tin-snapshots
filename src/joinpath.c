/*
 *  Project   : tin - a Usenet reader
 *  Module    : joinpath.c
 *  Author    : Thomas Dickey <dickey@clark.net>
 *  Created   : 1997-01-10
 *  Updated   : 1997-01-10
 *  Notes     :
 *  Copyright : (c) Copyright 1997-99 by Thomas Dickey
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */

/*
 * Concatenate dir+file, ensuring that we don't introduce extra '/', since some
 * systems (e.g., Apollo) use "//" for special purposes.
 */
#ifndef M_AMIGA
void
joinpath (
	char *result,
	const char *dir,
	const char *file)
{
#	ifdef __amigaos
	int i=0, tmp=0, tmp2=1;
#	endif /* __amigaos */
#	ifdef M_UNIX
	(void) strcpy(result, dir);
	if (result[0] == '\0' || result[strlen(result)-1] != '/')
		(void) strcat(result, "/");
	(void) strcat(result, file != 0 ? file : "");
#	endif /* M_UNIX */
/*
 * JK - horrible hack to convert "/foo/baz/bar" to "foo:baz/bar" (editors bug with *NIX-paths)
 * "foo:baz/bar" -styled paths should always work on Amiga
 */
#	ifdef __amigaos
	if (result[0] == '/')
		while (result[tmp++] = result[tmp2++])
			;
	while (result[i] != '/' && result[i] != ':')
		i++;
	result[i]=':';
#	endif /* __amigaos */
}
#endif /* !M_AMIGA */
