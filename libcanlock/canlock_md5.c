/* 
 * Functions to implement type MD5 cancel locks.  Written by Fluffy, Ruler
 * of All Usenet, and placed into the public domain.  Meow.
 */

#include <stdio.h>
#include <sys/types.h>
#include "base64.h"
#include "md5.h"
#include "hmac_md5.h"
#include "canlock.h"

#define BUFFSIZE 40

/* 
 * Generate an MD5 cancel key.
 */
unsigned char *
 md5_key(char *secret, int seclen, char *message, int msglen)
{
    unsigned char *cankey;
    unsigned char *canlock;
    unsigned char *hmacbuff;
    MD5_CTX hash_ctx;

    cankey = (char *) malloc(BUFFSIZE);

    hmacbuff = hmac_md5(secret, seclen, message, msglen);
    strcpy(cankey, "md5:");
    (void) b64_ntop(hmacbuff, MD5_LENGTH, cankey + 4, BUFFSIZE);

    return (cankey);
}

/* 
 * Generate an MD5 cancel lock.
 */
unsigned char *
 md5_lock(char *secret, int seclen, char *message, int msglen)
{
    unsigned char *cankey, *canlock;
    unsigned char hmacbuff[MD5_LENGTH];
    unsigned char junk[MD5_LENGTH];
    MD5_CTX hash_ctx;

    cankey = lock_strip_alpha(md5_key(secret, seclen, message, msglen), junk);

    canlock = (char *) malloc(BUFFSIZE);
    MD5Init(&hash_ctx);
    MD5Update(&hash_ctx, cankey, strlen(cankey));
    MD5Final(hmacbuff, &hash_ctx);
    strcpy(canlock, "md5:");
    (void) b64_ntop(hmacbuff, MD5_LENGTH, canlock + 4, BUFFSIZE);

    return (canlock);
}

/* 
 * Verify an SHA cancel key against a cancel lock.
 * Returns 0 on success, nonzero on failure.
 */
int md5_verify(unsigned char *key, unsigned char *lock)
{
    unsigned char binkey[MD5_LENGTH + 4];
    unsigned char templock[BUFFSIZE];
    unsigned char hmacbuff[MD5_LENGTH];
    MD5_CTX hash_ctx;
    int verified;

    /* Convert the key back into binary */
    (void) b64_pton(key, binkey, (MD5_LENGTH + 4));

    MD5Init(&hash_ctx);
    MD5Update(&hash_ctx, key, strlen(key));
    MD5Final(hmacbuff, &hash_ctx);
    (void) b64_ntop(hmacbuff, MD5_LENGTH, templock, BUFFSIZE);

    verified = strcmp(templock, lock);

    return (verified);
}
