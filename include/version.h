/*
 *  Project   : tin - a Usenet reader
 *  Module    : version.h
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 12-06-96
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

/*
 * This is NO official release, but a patched version.
 */

#define VERSION		"1.3 X-Mas BETA"
#define RELEASEDATE	"961224"

#ifdef M_AMIGA
#	define	OS	"AMIGA"
#	define	AMIVER	"1.3B"
#endif
#ifdef M_OS2
#	define	OS	"OS/2"
#endif
#ifdef M_UNIX
#	define	OS	"UNIX"
#endif
#ifdef WIN32
#	define	OS	"Windows/NT"
#endif
