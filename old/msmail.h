/*
 *  Project   : tin - a Usenet reader
 *  Module    : msmail.h
 *  Author    : N.Ellis
 *  Created   : 01-10-94
 *  Updated   : 27-10-94
 *  Notes     : WinNT specific
 *  Copyright : (c) Copyright 1991-94 by Nigel Ellis & Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#if defined(WIN32) && !defined(__MSMAIL_H__)
#define __MSMAIL_H__

int sendmail(LPSTR szFileName, LPSTR szTo, LPSTR szSubject, BOOL fEdit);
void endmailsession(void);

#endif /* __MSMAIL_H__ */
