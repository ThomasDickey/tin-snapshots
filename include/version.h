/*
 *  Project   : tin - a Usenet reader
 *  Module    : version.h
 *  Author    : I. Lea
 *  Created   : 1991-04-01
 *  Updated   : 2002-08-16
 *  Notes     :
 *  Copyright : (c) Copyright 1991-2002 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef VERSION_H
#	define VERSION_H	1

#	define PRODUCT		"tin"
#	define VERSION 	"1.4.6"
#	define RELEASEDATE	"20020816"
#	define RELEASENAME	"Aerials"
#	define TINRC_VERSION	"1.2"

#	ifdef M_AMIGA
#		define OSNAME	"AMIGA"
#		define AMIVER	VERSION
#	endif /* M_AMIGA */

#	ifdef M_OS2
#		define OSNAME	"OS/2"
#	endif /* M_OS2 */

#	ifdef M_UNIX
#		if !defined(__amiga)
#			define OSNAME	"UNIX"
#		else
#			define OSNAME	"AMIGA"
#		endif /* !__amiga */
#	endif /* M_UNIX */

#	ifdef WIN32
#		define OSNAME	"Windows/NT"
#	endif /* WIN32 */

#	ifndef OSNAME
#		define OSNAME	"Unknown"
#	endif /* !OSNAME */

#endif /* !VERSION_H */
