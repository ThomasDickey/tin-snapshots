extern char *rfc1522_decode (char *);
extern char *rfc1522_encode (char *);
extern void rfc1522_decode_all_headers (void);
extern void rfc15211522_encode (char *);
extern void get_mm_charset (void);
extern int mmdecode ();

extern char mm_charset[128];
extern const char base64_alphabet[64];
