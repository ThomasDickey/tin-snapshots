/* 
 * Functions to implement type SHA1 cancel locks.  Written by Fluffy, Ruler
 * of All Usenet, and placed into the public domain.  Meow.
 */

#include <stdio.h>
#include <sys/types.h>
#include "base64.h"
#include "sha1.h"
#include "hmac_sha1.h"
#include "canlock.h"

#define BUFFSIZE 40

/* 
 * Generate an SHA1 cancel key.
 */
unsigned char *
 sha_key(char *secret, int seclen, char *message, int msglen)
{
    unsigned char *cankey;
    unsigned char *canlock;
    unsigned char *hmacbuff;
    SHA1_CTX hash_ctx;

    cankey = (char *) malloc(BUFFSIZE);

    hmacbuff = hmac_sha1(secret, seclen, message, msglen);
    strcpy(cankey, "sha1:");
    (void) b64_ntop(hmacbuff, SHA1_LENGTH, cankey + 5, BUFFSIZE);

    return (cankey);
}

/* 
 * Generate an SHA1 cancel lock.
 */
unsigned char *
 sha_lock(char *secret, int seclen, char *message, int msglen)
{
    unsigned char *cankey, *canlock;
    unsigned char hmacbuff[SHA1_LENGTH];
    unsigned char junk[SHA1_LENGTH];
    SHA1_CTX hash_ctx;

    cankey = lock_strip_alpha(sha_key(secret, seclen, message, msglen), junk);

    canlock = (char *) malloc(BUFFSIZE);
    SHA1Init(&hash_ctx);
    SHA1Update(&hash_ctx, cankey, strlen(cankey));
    SHA1Final(hmacbuff, &hash_ctx);
    strcpy(canlock, "sha1:");
    (void) b64_ntop(hmacbuff, SHA1_LENGTH, canlock + 5, BUFFSIZE);

    return (canlock);
}

/* 
 * Verify an SHA cancel key against a cancel lock.
 * Returns 0 on success, nonzero on failure.
 */
int sha_verify(unsigned char *key, unsigned char *lock)
{
    unsigned char binkey[SHA1_LENGTH + 4];
    unsigned char templock[BUFFSIZE];
    unsigned char hmacbuff[SHA1_LENGTH];
    SHA1_CTX hash_ctx;
    int verified;

    /* Convert the key back into binary */
    (void) b64_pton(key, binkey, (SHA1_LENGTH + 4));

    SHA1Init(&hash_ctx);
    SHA1Update(&hash_ctx, key, strlen(key));
    SHA1Final(hmacbuff, &hash_ctx);
    (void) b64_ntop(hmacbuff, SHA1_LENGTH, templock, BUFFSIZE);

    verified = strcmp(templock, lock);

    return (verified);
}
