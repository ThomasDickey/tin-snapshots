/* 
 * Functions to implement RFC-2104 (HMAC with SHA-1 hashes).
 * Written by Fluffy, Ruler of All Usenet,
 * and placed into the public domain.
 * Meow.
 */

#include <stdio.h>
#include "sha1.h"

#define ipad 0x36
#define opad 0x5c

/* 
 * Encode a string using HMAC - see RFC-2104 for details.
 */
unsigned char *
 hmac_sha1(K, Klen, T, Tlen)
unsigned char *K;		/* key */
int Klen;			/* and it size */
unsigned char *T;		/* text to encode */
int Tlen;			/* and its size */
{
    SHA1_CTX hash_ctx, hash_ctx2;
    unsigned char keyin[SHA1_LENGTH];
    unsigned char *step2;
    unsigned char step4[SHA1_LENGTH];
    unsigned char step5[SHA1_BLOCK + SHA1_LENGTH];
    unsigned char *hmac_out;
    unsigned char *c;
    int i;
    int j;
    int k;

    SHA1Init(&hash_ctx);

    /* If the key is bigger than SHA1_BLOCK we need to hash it. */
    if (Klen > SHA1_BLOCK) {
	SHA1Update(&hash_ctx, K, Klen);
	SHA1Final(keyin, &hash_ctx);
	Klen = SHA1_LENGTH;
    } else {
	memcpy(keyin, K, Klen);
    }

    step2 = (char *) malloc(Tlen + SHA1_BLOCK);

    c = keyin;
    for (i = 0; i < Klen; i++) {
	step2[i] = *c ^ ipad;
	step5[i] = *c ^ opad;
	c++;
    }
    for (j = i; j < SHA1_BLOCK; j++) {
	step2[j] = 0x36;
	step5[j] = opad;
    }

    memcpy(&step2[SHA1_BLOCK], T, Tlen);

    SHA1Init(&hash_ctx);
    SHA1Update(&hash_ctx, step2, SHA1_BLOCK + Tlen);
    SHA1Final(step4, &hash_ctx);

    memcpy(&step5[SHA1_BLOCK], step4, SHA1_LENGTH);

    hmac_out = (unsigned char *) malloc(SHA1_LENGTH);

    SHA1Init(&hash_ctx);
    SHA1Update(&hash_ctx, step5, SHA1_BLOCK + SHA1_LENGTH);
    SHA1Final(hmac_out, &hash_ctx);

    return hmac_out;
}
