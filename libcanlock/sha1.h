/* 
 * SHA-1 in C
 * By Steve Reid <steve@edmweb.com>
 * 100% Public Domain
 */
#ifndef __SHA1_H__
#define __SHA1_H__

#include "endian.h"

/* #define SHA1HANDSOFF /* Copies data before messing with it. */


#define SHA1_BLOCK	64
#define SHA1_LENGTH	20

typedef struct _SHA1_CTX {
    unsigned long state[5];
    unsigned long count[2];
    unsigned char buffer[SHA1_BLOCK];
} SHA1_CTX;

void SHA1Transform(unsigned long state[5], unsigned char buffer[64]);
void SHA1Init(SHA1_CTX * context);
void SHA1Update(SHA1_CTX * context, unsigned char *data, unsigned int len);
void SHA1Final(unsigned char digest[20], SHA1_CTX * context);

#endif				/* __SHA1_H__ */
