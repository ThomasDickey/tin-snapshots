/*
 *  Project   : tin - a Usenet reader
 *  Module    : joinpath.c
 *  Author    : Thomas Dickey
 *  Created   : 10.01.97
 *  Updated   : 10.01.97
 *  Notes     :
 *  Copyright : (c) Copyright 1997 by Thomas Dickey
 *              You may  freely  copy or  redistribute	this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

/*
 * Concatenate dir+file, ensuring that we don't introduce extra '/', since some
 * systems (e.g., Apollo) use "//" for special purposes.
 */
#ifndef M_AMIGA
void
joinpath(result, dir, file)
	char	*result;
	char	*dir;
	char	*file;
{
#ifdef M_UNIX
	(void) strcpy(result, dir);
	if (result[0] == '\0'
	 || result[strlen(result)-1] != '/')
		(void) strcat(result, "/");
	(void) strcat(result, file != 0 ? file : "");
#endif  /* M_UNIX */
}
#endif
