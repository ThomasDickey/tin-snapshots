#ifndef VMSTIMEVAL_H
#define VMSTIMEVAL_H

/* In VMS, the timeval structure doesn't really exist, so let's define it.

   Warning: timeval IS defined in the UCX socket.h, so your compiler might
	get screwed! The UCX definition is exactly the same as the one below
*/

struct timeval {
  long tv_sec;
  long tv_usec;
};

struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};

#endif
