/*
 *  Project   : tin - a Usenet reader
 *  Module    : color.c
 *  Original  : Olaf Kaluza <olaf@criseis.ruhr.de>
 *  Author    : Roland Rosenfeld <roland@spinnaker.rhein.de>
 *  Created   : 02-06-95
 *  Updated   : 06-03-95, 30-03-96
 *  Notes     : This are the basic function for ansi-color
 *              and word highlightning
 *  Copyright : (c) 1995 by Olaf Kalzuga and Roland Rosenfeld
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifdef HAVE_COLOR
#include "tin.h"

/* setting foregroundcolor */
void fcol (int color)
{
	if (!use_color) return;
	printf ("\033[%dm", (color+30));
}

/* setting backgroundcolor */
void bcol (int color)
{
	if (!use_color) return;
	printf ("\033[%dm", (color+40));
}


void print_color (char* str) {
	if ( str[0]==':' || str[0]=='>' || str[0]=='|' || str[0]==']'
	    || str[1]=='>' || str[2]=='>' || str[3]=='>'
	    || (str[0]==' ' && str[1]==':'))
	{
		fcol(col_quote);
	} else {
		fcol(col_text);
	}
	printf("%s\r\n", str);
}
#endif
