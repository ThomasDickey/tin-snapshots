/*
 *  Project   : tin - a Usenet reader
 *  Module    : version.h
 *  Author    : I. Lea
 *  Created   : 1991-04-01
 *  Updated   : 1998-11-14
 *  Notes     :
 *  Copyright : (c) Copyright 1991-98 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef VERSION_H
#	define VERSION_H	1

#	define PRODUCT		"tin"
#	define VERSION		"pre-1.4"
#	define RELEASEDATE	"981114"
#	define RELEASENAME	"The Watchman"
#	define TINRC_VERSION	"1.1"

#	ifdef M_AMIGA
#		define OS	"AMIGA"
#		define AMIVER	"1.3B"
#	endif /* M_AMIGA */

#	ifdef M_OS2
#		define OS	"OS/2"
#	endif /* M_OS2 */

#	ifdef M_UNIX
#		if !defined( __amiga )
#			define OS	"UNIX"
#		else
#			define OS	"AMIGA"
#		endif /* !__amiga */
#	endif /* M_UNIX */

#	ifdef WIN32
#		define OS	"Windows/NT"
#	endif /* WIN32 */

#	ifndef OS
#		define OS	"Unknown"
#	endif /* !OS */

#endif /* !VERSION_H */
