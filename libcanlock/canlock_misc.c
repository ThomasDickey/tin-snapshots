/* 
 * Functions to implement cancel locks.  Written by Fluffy, Ruler of All
 * Usenet, and placed into the public domain.  Meow.
 */

#include <stdio.h>
#include <sys/types.h>
#include "base64.h"

/* 
 * Return a stripped cancel lock, that is, with the xx: prefix
 * removed, else NULL on failure.
 * type is set to the lock type, else zero on failure.
 */
unsigned char *
 lock_strip_alpha(unsigned char *key, unsigned char *type)
{
    unsigned char *c;
    unsigned char typetext[256], keyout[256];
    int i;

    strcpy(typetext, "");
    strcpy(keyout, "");

    while ((*key != ':') && (*key != '\0')) {
	strncat(typetext, key++, 1);
    }
    if (*key == '\0') {
	type = NULL;
    } else {
	key++;
	for (c = key; *c != '\0'; c++) {
	    if (*c == ':')
		*c = '\0';
	}

	strcpy(type, typetext);
	for (c = type; *c != '\0'; ++c) {
	    *c = tolower(*c);
	}
    }
    return (key);
}


/* 
 * Return a stripped cancel lock, that is, with the xx: prefix
 * removed, else NULL on failure.
 * type is set to the lock type, else zero on failure.
 */
unsigned char *
 lock_strip(unsigned char *key, char *type)
{
    return (lock_strip_alpha(key, type));
}
