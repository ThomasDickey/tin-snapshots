/*
 *  Project   : tin - a Usenet reader
 *  Module    : bugrep.h
 *  Author    : Urs Janssen <urs@tin.org>
 *  Created   : 30.07.1997
 *  Updated   : 01.02.1998
 *  Notes     :
 *  Copyright : (c) Copyright 1997-98 by Urs Janssen
 *              You may  freely  copy or  redistribute this  software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef BUGREP_H
#	define BUGREP_H	1

#	ifdef M_UNIX
#		define	BUG_REPORT_ADDRESS	"tin-bugs@tin.org"
#	endif /* M_UNIX */

#	ifdef M_AMIGA
#		define	BUG_REPORT_ADDRESS	"obw@amarok.ping.de"
#	endif /* M_AMIGA */

#	ifdef VMS
#		define	BUG_REPORT_ADDRESS	"stenns@vw.tci.uni-hannover.de"
#	endif /* VMS */

/* OS2 and WIN32 are currently not supported */
#	ifdef M_OS2
#		define	BUG_REPORT_ADDRESS	"andreas@scilink.org"
#	endif /* M_OS2 */

#	ifdef WIN32
#		define	BUG_REPORT_ADDRESS	"nigele@microsoft.com"
#	endif /* WIN32 */

/* fallback */
#	ifndef BUG_REPORT_ADDRESS
#		define	BUG_REPORT_ADDRESS	"tin-bugs@tin.org"
#	endif /* BUG_REPORT_ADDRESS */

#endif /* !BUGREP_H */
