/*
 *  Project   : tin - a Usenet reader
 *  Module    : win32tcp.c
 *  Author    : N.Ellis
 *  Created   : 01-10-94
 *  Updated   : 04-11-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Nigel Ellis
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifdef WIN32

#	define	s_printf
#	define	s_flush	
#	define	s_puts	
#	define	s_end()

typedef struct {
	int fd;
} TCP;

int s_init(void);
TCP *s_fdopen(int s, const char *mode);
void s_close(int s);
int s_dup(int fd);
char *s_gets(LPSTR nntp_line, DWORD length, TCP *tp);

#endif /* WIN32 */
