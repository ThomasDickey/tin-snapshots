/* 
 * canlocktest.c - just checking.
 * 
 * This program doesn't really do anything but lightly exercise all the
 * library functions, so you can make sure it all compiled correctly. 
 * Everything's kept simple so that you can also see how they would be
 * called in a real application.
 */
#include <stdio.h>
#include <sys/types.h>
#if 0
// #include "sha1.h"
// #include "md5.h"
#endif /* 0 */
#include "canlock.h"

#define BUFFSIZE 512

void checker(char *key, char *lock)
{
    int r;
    char *rawkey, *rawlock;
    char keytype[BUFFSIZE], locktype[BUFFSIZE];

    printf("L %s K %s ", lock, key);

    rawkey = lock_strip_alpha(key, keytype);
    rawlock = lock_strip_alpha(lock, locktype);

    if (!strcmp(keytype, locktype)) {
	if (!strcmp(keytype, "md5")) {
	    if (!md5_verify(rawkey, rawlock))
		printf("md5 OK\n");
	    else
		printf("md5 no\n");
	} else if (!strcmp(keytype, "sha1")) {
	    if (!sha_verify(rawkey, rawlock))
		printf("sha1 OK\n");
	    else
		printf("sha1 no\n");
	} else
	    printf("unknown\n");
    } else {
	printf("Mismatch %s %s\n", keytype, locktype);
    }
}


int main()
{
    unsigned char cankey[256];
    unsigned char canlock[256];
    unsigned char *lkey, *llock;
    unsigned char secret[] = "fluffy";
    unsigned char message[] = "<lkr905851929.22670@meow.invalid>";
    unsigned char junk[32], junk2[32];

    printf("Secret %s\n", secret);
    printf("Message %s\n", message);

    llock = sha_lock(secret, strlen(secret), message, strlen(message));
    lkey = sha_key(secret, strlen(secret), message, strlen(message));

    printf("%s%s %s\n", "SHA Expect Lock/Key:\n",
	   "L sha1:ScU1gyAi9bd/aFEOyzg4m99lwXs=",
	   "K sha1:C1Me/4n0l/V778Ih3J2UnhAoHrA=");

    checker(lkey, llock);
    printf("---\n");

    llock = md5_lock(secret, strlen(secret), message, strlen(message));
    lkey = md5_key(secret, strlen(secret), message, strlen(message));

    printf("%s%s %s\n", "MD5 Expect Lock/Key:\n",
	   "L md5:equ7DQVveXcZpqwCiMflmQ==",
	   "K md5:ByhbX+72NZC1xZdiPBNTOQ==");

    checker(lkey, llock);
    printf("---\n");

    printf("Testing against usefor cancel lock draft 01 samples...\n");

	sprintf(canlock, "%s", "sha1:bNXHc6ohSmeHaRHHW56BIWZJt+4=");
    sprintf(cankey, "%s", "sha1:aaaBBBcccDDDeeeFFF");
    checker(cankey, canlock);
    printf("---above should have been ok---\n");

    sprintf(canlock, "%s", "SHA1:H7/zsCUemvbvSDyARDaMs6AQu5s=");
    sprintf(cankey, "%s", "sha1:chW8hNeDx3iNUsGBU6/ezDk88P4=");
    checker(cankey, canlock);

    sprintf(canlock, "%s", "SHA1:H7/zsCUemvbvSDyARDaMs6AQu5s=");
    sprintf(cankey, "%s", "sha1:4srkWaRIzvK51ArAP:Hc");
    checker(cankey, canlock);
    printf("---above should have been okay, no---\n");

    sprintf(canlock, "%s", "sha1:JyEBL4w9/abCBuzCxMIE/E73GM4=");
    sprintf(cankey, "%s", "sha1:K4rkWRjRcXmIzvK51ArAP:Jy");
    checker(cankey, canlock);

    sprintf(canlock, "%s", "sha1:2Bmg+zWaY1noRiCdy8k3IapwSDU=");
    sprintf(cankey, "%s", "sha1:K4rkWRjRcXmIzvK51ArAP:Jy");
    checker(cankey, canlock);
    printf("---above should have been okay, no---\n");
}
