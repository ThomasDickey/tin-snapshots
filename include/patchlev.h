/*
 *  Project   : tin - a Usenet reader
 *  Module    : patchlev.h
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 27-09-93
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

/* Beta versions are 	"1.n releasedateBETA" */
#define VERSION		"1.3 950824BETA"

/* This is NO official release, but a patched version.
 * Including patches from:
 *
 * 	Chris Blum <chris@phil.uni-sb.de>
 *		MIME support & minor bugfixes/customasations
 *
 * 	Greg Berigan <gberigan@cse.unl.edu>
 *		skip quoted text & show lines
 *		show newsgroup description 
 *
 *	Roland Rosenfeld <roland@spinnaker.rhein.de>
 *	Olaf Kaluza <olaf@criseis.ruhr.de>
 *	Giuseppe De Marco <gdemarco@freenet.hut.fi>
 *		color support
 *		bugfixes
 *
 *	Jason Faultless <jason@radar.demon.co.uk>
 *		post_process_command
 *		bugfixes
 *
 *	Seven Paulus <sven@oops.sub.de>
 *		FIDO support
 *
 * 	Enrik Berkhan <enrik@akk.uni-karlsruhe.de>
 * 	Urs Janﬂen <urs@akk.uni-karlsruhe.de>
 *		bugfixes & customasations
 *
 * 	Stefan Scholl <stesch@sks.inka.de>
 *		minor bugfix
 *
 *	Bernd Eckenfels <ecki@lina.inka.de>
 *		minor bugfix
 */
#define PATCHLEVEL	"MC.960515"

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
