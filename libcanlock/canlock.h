unsigned char *sha_key(char *secret, int seclen, char *message, int msglen);
unsigned char *sha_lock(char *secret, int seclen, char *message, int msglen);
int sha_verify(unsigned char *key, unsigned char *lock);

unsigned char *md5_key(char *secret, int seclen, char *message, int msglen);
unsigned char *md5_lock(char *secret, int seclen, char *message, int msglen);
int md5_verify(unsigned char *key, unsigned char *lock);

unsigned char *lock_strip(unsigned char *key, char *type);
unsigned char *lock_strip_alpha(unsigned char *key, char *type);
