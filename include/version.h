/*
 *  Project   : tin - a Usenet reader
 *  Module    : version.h
 *  Author    : I.Lea
 *  Created   : 01.04.91
 *  Updated   : 30.07.97
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef VERSION_H
#	define VERSION_H	1

#	define VERSION		"1.3 unoff BETA"
#	define RELEASEDATE	"970922"
#	define TINRC_VERSION	"1.0"

#	ifdef M_AMIGA
#		define	OS	"AMIGA"
#		define	AMIVER	"1.3B"
#	endif

#	ifdef M_OS2
#		define	OS	"OS/2"
#	endif

#	ifdef M_UNIX
#		define	OS	"UNIX"
#	endif

#	ifdef WIN32
#		define	OS	"Windows/NT"
#	endif

#	ifndef OS
#		define	OS "Unknown"
#	endif

#endif /* !VERSION_H */
