#include <nam.h>

struct filespec {
  char dev[32];
  char dir[256];
  char filename[100];
  char full[NAM$C_MAXRSS];
};

struct filespec *sysparse(char *filename);
