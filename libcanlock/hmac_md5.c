/* 
 * Functions to implement RFC-2104 (HMAC with MD5 hashes).
 * Written by Fluffy, Ruler of All Usenet,
 * and placed into the public domain.
 * Meow.
 */

#include <stdio.h>
#include "md5.h"

#define ipad 0x36;
#define opad 0x5c;

/* 
 * Encode a string using HMAC - see RFC-2104 for details.
 */
unsigned char *
 hmac_md5(K, Klen, T, Tlen)
unsigned char *K;		/* key */
int Klen;			/* and it size */
unsigned char *T;		/* text to encode */
int Tlen;			/* and its size */
{
    MD5_CTX hash_ctx;
    unsigned char keyin[MD5_LENGTH];
    unsigned char *step2;
    unsigned char step4[MD5_LENGTH];
    unsigned char step5[MD5_BLOCK + MD5_LENGTH];
    unsigned char *hmac_out;
    unsigned char *c;
    int i;
    int j;
    int k;

    MD5Init(&hash_ctx);

    /* If the key is bigger than MD5_BLOCK we need to hash it. */
    if (Klen > MD5_BLOCK) {
	MD5Update(&hash_ctx, K, Klen);
	MD5Final(keyin, &hash_ctx);
	Klen = MD5_LENGTH;
    } else {
	memcpy(keyin, K, Klen);
    }

    step2 = (char *) malloc(Tlen + MD5_BLOCK);

    c = keyin;
    for (i = 0; i < Klen; i++) {
	step2[i] = *c ^ ipad;
	step5[i] = *c ^ opad;
	c++;
    }

    for (j = i; j < MD5_BLOCK; j++) {
	step2[j] = ipad;
	step5[j] = opad;
    }

    memcpy(&step2[MD5_BLOCK], T, Tlen);

    MD5Init(&hash_ctx);
    MD5Update(&hash_ctx, step2, MD5_BLOCK + Tlen);
    MD5Final(step4, &hash_ctx);

    memcpy(&step5[MD5_BLOCK], step4, MD5_LENGTH);

    hmac_out = (char *) malloc(MD5_LENGTH);

    MD5Init(&hash_ctx);
    MD5Update(&hash_ctx, step5, MD5_LENGTH + MD5_BLOCK);
    MD5Final(hmac_out, &hash_ctx);

    return hmac_out;
}
