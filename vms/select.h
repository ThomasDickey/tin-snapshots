#ifdef __DECC
#include <time.h>
#else
#define NBBY	8		/* number of bits in a byte */
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= CHANNELCNT (SYSGEN parameter)
 */
#ifndef FD_SETSIZE
#define FD_SETSIZE	512
#endif
#define CHANNELSIZE	16	/* Size of a channel */

typedef long	fd_mask;
#define NFDBITS (sizeof(fd_mask) * NBBY)	/* bits per mask */
#ifndef howmany
#define howmany(x, y)	(((x)+((y)-1))/(y))
#endif

typedef struct fd_set {
	fd_mask fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define FD_SET(n, p)	((p)->fds_bits[(n)/CHANNELSIZE/NFDBITS] |= (1 << (((n)/CHANNELSIZE) % NFDBITS)))
#define FD_CLR(n, p)	((p)->fds_bits[(n)/CHANNELSIZE/NFDBITS] &= ~(1 << (((n)/CHANNELSIZE) % NFDBITS)))
#define FD_ISSET(n, p)	((!((n) % CHANNELSIZE)) && ((p)->fds_bits[(n)/CHANNELSIZE/NFDBITS] & (1 << (((n)/CHANNELSIZE) % NFDBITS))))
#define FD_ZERO(p)	memset((char *)(p),'\0',sizeof(*(p)))
#endif
