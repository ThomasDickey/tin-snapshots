/*
 *  Project   : tin - a Usenet reader
 *  Module    : bugrep.h
 *  Author    : Urs Janssen <urs@akk.uni-karlsruhe.de>
 *  Created   : 30.07.1997
 *  Updated   : 
 *  Notes     :
 *  Copyright : (c) Copyright 1997 by Urs Janssen
 *              You may  freely  copy or  redistribute	this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef BUGREP_H
#	define BUGREP_H	1

#	ifdef M_AMIGA
#		define	BUG_REPORT_ADDRESS	"obw@amarok.ping.de"
#	endif

#	ifdef M_UNIX
#		define	BUG_REPORT_ADDRESS	"urs@akk.uni-karlsruhe.de"
#	endif

#	ifdef M_OS2
#		define	BUG_REPORT_ADDRESS	"andreas@scilink.org"
#	endif

#	ifdef VMS
#		define	BUG_REPORT_ADDRESS	"mcquill@next.duq.edu"
#	endif /* VMS */

#	ifdef WIN32
#		define	BUG_REPORT_ADDRESS	"nigele@microsoft.com"
#	endif

#	ifndef BUG_REPORT_ADDRESS
#		define	BUG_REPORT_ADDRESS	"urs@akk.uni-karlsruhe.de"
#	endif

#endif /* !BUGREP_H */
