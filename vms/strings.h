#ifndef __STRING_LOADED
#define __STRING_LOADED	1

/*	STRING - V3.0 - String handling function definitions	*/

#pragma nostandard
#include stddef
#pragma standard

char   *strcpy	(char *s1, const char *s2);
char   *strncpy	(char *s1, const char *s2, size_t n);
char   *strcat	(char *s1, const char *s2);
char   *strncat	(char *s1, const char *s2, size_t n);
int	strcmp	(const char *s1, const char *s2);
int	strncmp	(const char *s1, const char *s2, size_t n);
char   *strchr	(const char *s, int character);
char   *strrchr	(const char *s, int character);
size_t	strspn	(const char *s1, const char *s2);
size_t	strcspn	(const char *s1, const char *s2);
size_t	strlen	(const char *s);
char   *strpbrk	(const char *s1, const char *s2);
char   *strstr	(const char *s1, const char *s2);
char   *strtok	(char *s1, const char *s2);
char   *strerror(int errnum, ...);

int	memcmp	(const void *s1, const void *s2, size_t size);
void   *memchr	(const void *s1, int c, size_t size);
void   *memcpy	(void *s1, const void *s2, size_t size);
void   *memmove	(void *s1, const void *s2, size_t size);
void   *memset	(void *s, int character, size_t size);

#endif					/* __STRING_LOADED */
