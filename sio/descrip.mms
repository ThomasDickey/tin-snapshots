# Available entries:
#               lib                     --> creates the library
#               clean                   --> removes all .obj and .a files
#               spotless                --> clean + uninstall
#               tags                    --> creates a tags file (from the SOURCES and HEADERS)

NAME                    = sio
VERSION                 = 1.6.1

HEADERS                 = sio.h, impl.h, events.h, sioconf.h
MODULES                 = sprint, sio, siosup
SOURCES                 = $(MODULES:=.c)
OBJECTS                 = $(SOURCES:.c=.obj)

MANFILES                = sio.3, Sprint.3
INCLUDEFILES            = sio.h

# Available flags:
#  -DDEBUG           :  enables assertions in the code. A failed assertion
#                       terminates the program
#  -DEVENTS          :  enables code that records events (currently limited
#                       to which functions have been called on a given fd)
#                       and code that accesses the event buffers.
#  -DLITTLE_ENDIAN   :  says that the machine is a little endian. This is
#                       needed if you enable EVENTS and your machine is a
#                       little endian (big endian is the default).
#
# DEFS should be set from the command line; the current group of defs
# is for SunOS 4.x
#
.IFDEF VAXC
CC = cc/vaxc
.ELSE
.IFDEF GNUC
CC = gcc
.ELSE
CC = cc/decc
.ENDIF
.ENDIF

.IFDEF VAXC
DEFS            = HAS_ISATTY,$(TCP),HAS_MEMOPS,CANT_ENUM
.ELSE
.IFDEF GNUC
DEFS            = HAS_ISATTY,$(TCP),HAS_MEMOPS,CANT_ENUM
.ELSE
DEFS            = HAS_ISATTY,$(TCP),HAS_MEMOPS,CANT_ENUM
.ENDIF
.ENDIF

.IFDEF DEBUG
DBG             = /debug /nooptim
.ELSE
.IFDEF VAXC
DBG             = /optim
.ELSE
.IFDEF GNUC
DBG             = /optim=2
.ELSE
DBG             = /optim
.ENDIF
.ENDIF
.ENDIF

.IFDEF VAXC
OPTS        = 
.ELSE
.IFDEF GNUC
OPTS        = 
.ELSE
OPTS        = /warning=(disable=implicitfunc)
.ENDIF
.ENDIF

VERSION_DEF     = VERSION="""SIO Version $(VERSION)"""

CPP_DEFS        = /define=($(VERSION_DEF),$(DEFS))

#
# The following variables shouldn't need to be changed
#
LINT_FLAGS              = -hbux
CPP_FLAGS               = $(CPP_DEFS)
CC_FLAGS                = $(DBG) $(OPTS)
CFLAGS                  = $(CPP_FLAGS) $(CC_FLAGS)

LIBNAME                 = lib$(NAME)$(OLB)

lib : $(LIBNAME)($(MODULES))
    write sys$output "Built $(LIBNAME)"

clean :
        del/log *$(OBJ);*, $(LIBNAME);*

#
# PUT HERE THE RULES TO MAKE THE OBJECT FILES
#
sprint.obj :   sio.h, impl.h, sioconf.h
sio.obj :      sio.h, impl.h, sioconf.h, events.h
siosup.obj :   sio.h, impl.h, sioconf.h, events.h

.IFDEF VAXC

.FIRST :
    define sys Multinet_Common_Root:[Multinet.Include.Sys],SYS$LIBRARY
    define VAXC$INCLUDE	Multinet_Common_Root:[Multinet.Include.Sys],SYS$LIBRARY

.LAST :
    deassign sys
    deassign vaxc$include

.ELSE
.IFDEF GNUC

.first :
    define sys sys$share
.last :
    deassign sys
.ENDIF
.ENDIF

