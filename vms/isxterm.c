
#include <descrip.h>
#include <iodef.h>
#include <ssdef.h>
#include <stsdef.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

is_xterm()
{
    Display *dpy;                       /* X connection */
    char *displayname = NULL;           /* server to contact */

#ifdef __alpha
    dpy = XOpenDisplay (displayname);
    if (!dpy)
      return FALSE;

    XCloseDisplay (dpy);
    return TRUE;
#else
    return FALSE;
#endif
}

