/*
 *  Project   : tin - a Usenet reader
 *  Module    : version.h
 *  Author    : I. Lea
 *  Created   : 1991-04-01
 *  Updated   : 1998-08-10
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
#	define RELEASEDATE	"980818"
#	define RELEASENAME	"Laura"
#	define TINRC_VERSION	"1.1"

#	ifdef M_AMIGA
#		define	OS	"AMIGA"
#		define	AMIVER	"1.3B"
#	endif

#	ifdef M_OS2
#		define	OS	"OS/2"
#	endif

#	ifdef M_UNIX
#		if !defined( __amiga )
#			define	OS	"UNIX"
#		else
#			define	OS	"AMIGA"
#		endif
#	endif

#	ifdef WIN32
#		define	OS	"Windows/NT"
#	endif

#	ifndef OS
#		define	OS "Unknown"
#	endif

#endif /* !VERSION_H */
