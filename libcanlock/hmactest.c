/*
 * hmac test program
 */

#include <stdio.h>
#include "sha1.h"
#include "hmac_sha1.h"
#include "md5.h"
#include "hmac_md5.h"

main()
{
    unsigned char *hmachash, *md5hash;
    unsigned char key1[] = "Jefe";
    unsigned char message1[] = "what do ya want for nothing?";
    unsigned char key2[20];
    unsigned char message2[] = "Hi There";
    unsigned char key3[80];
    unsigned char message3[] =
    "Test Using Larger Than Block-Size Key - Hash Key First";
    unsigned char key4[80];
    unsigned char message4[] =
    "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data";
    int i;
    unsigned char key5[] =
    {
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
	0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x00
    };
    unsigned char message5[50];

    unsigned char j;


    for (i = 0; i < 20; i++)
	key2[i] = 0x0b;

    for (i = 0; i < 80; i++)
	key3[i] = 0xaa;

    for (i = 0; i < 80; i++)
	key4[i] = 0xaa;

    for (i = 0; i < 50; i++)
	message5[i] = (unsigned char) 0xcd;

    printf("Key: %s\n", key1);
    printf("Msg: %s\n", message1);
    hmachash = hmac_sha1(key1, strlen(key1), message1, strlen(message1));
    printf("Expected SHA Digest: %s\n",
	   "0xeffcdf6ae5eb2fa2d27416d5f184df9c259a7c79");
    printf("  Actual SHA Digest: 0x");
    for (i = 0; i < SHA1_LENGTH; i++)
	printf("%02x", hmachash[i]);
    putchar('\n');


    md5hash = hmac_md5(key1, strlen(key1), message1, strlen(message1));
    printf("Expected MD5 Digest: %s\n",
	   "0x750c783e6ab0b503eaa86e310a5db738");
    printf("  Actual MD5 Digest: 0x");
    for (i = 0; i < MD5_LENGTH; i++)
	printf("%02x", md5hash[i]);
    putchar('\n');


/********/
    printf("\nKey: 0x0b, len 20 for SHA, 16 for MD5\n");
    printf("Msg: %s\n", message2);


    hmachash = hmac_sha1(key2, 20, message2, strlen(message2));
    printf("Expected SHA Digest: %s\n",
	   "0xb617318655057264e28bc0b6fb378c8ef146be00");
    printf("  Actual SHA Digest: 0x");
    for (i = 0; i < SHA1_LENGTH; i++)
	printf("%02x", hmachash[i]);
    putchar('\n');


    md5hash = hmac_md5(key2, 16, message2, strlen(message2));
    printf("Expected MD5 Digest: %s\n",
	   "0x9294727a3638bb1c13f48ef8158bfc9d");
    printf("  Actual MD5 Digest: 0x");
    for (i = 0; i < MD5_LENGTH; i++)
	printf("%02x", md5hash[i]);
    putchar('\n');

/********/
    printf("\nKey: 0xaa repeated 80 times\n");
    printf("Msg: %s\n", message3);

    hmachash = hmac_sha1(key3, 80, message3, strlen(message3));
    printf("Expected SHA Digest: %s\n",
	   "0xaa4ae5e15272d00e95705637ce8a3b55ed402112");
    printf("  Actual SHA Digest: 0x");
    for (i = 0; i < SHA1_LENGTH; i++)
	printf("%02x", hmachash[i]);
    putchar('\n');

    md5hash = hmac_md5(key3, 80, message3, strlen(message3));
    printf("Expected MD5 Digest: %s\n",
	   "0x6b1ab7fe4bd7bf8f0b62e6ce61b9d0cd");
    printf("  Actual MD5 Digest: 0x");
    for (i = 0; i < MD5_LENGTH; i++)
	printf("%02x", md5hash[i]);
    putchar('\n');

/********/
    printf("\nKey: 0xaa repeated 80 times\n");
    printf("Msg: %s\n", message4);

    hmachash = hmac_sha1(key4, 80, message4, strlen(message4));
    printf("Expected SHA Digest: %s\n",
	   "0xe8e99d0f45237d786d6bbaa7965c7808bbff1a91");
    printf("  Actual SHA Digest: 0x");
    for (i = 0; i < SHA1_LENGTH; i++)
	printf("%02x", hmachash[i]);
    putchar('\n');

    md5hash = hmac_md5(key4, 80, message4, strlen(message4));
    printf("Expected MD5 Digest: %s\n",
	   "0x6f630fad67cda0ee1fb1f562db3aa53e");
    printf("  Actual MD5 Digest: 0x");
    for (i = 0; i < MD5_LENGTH; i++)
	printf("%02x", md5hash[i]);
    putchar('\n');

/********/
    printf("\nKey: 0x");
    for (i = 0; i < 25; i++)
	printf("%02x", key5[i]);

    printf("\nMsg: 0xcd repeated 50 times\n");

    hmachash = hmac_sha1(key5, 25, message5, 50);
    printf("Expected SHA Digest: %s\n",
	   "0x4c9007f4026250c6bc8414f9bf50c86c2d7235da");
    printf("  Actual SHA Digest: 0x");
    for (i = 0; i < SHA1_LENGTH; i++)
	printf("%02x", hmachash[i]);
    putchar('\n');

    md5hash = hmac_md5(key5, 25, message5, 50);
    printf("Expected MD5 Digest: %s\n",
	   "0x697eaf0aca3a3aea3a75164746ffaa79");
    printf("  Actual MD5 Digest: 0x");
    for (i = 0; i < MD5_LENGTH; i++)
	printf("%02x", md5hash[i]);
    putchar('\n');


}
