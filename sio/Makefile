# Available entries:
# 		lib 			--> creates the library
#		clean			--> removes all .obj and .a files
#		spotless		--> clean + uninstall
#		tags			--> creates a tags file (from the SOURCES and HEADERS)

NAME			= sio
VERSION			= 1.6.1

HEADERS			= sio.h impl.h events.h sioconf.h
SOURCES			= sprint.c sio.c siosup.c
OBJECTS			= sprint.obj sio.obj siosup.obj

MANFILES		= sio.3 Sprint.3
INCLUDEFILES		= sio.h

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
DEFS		= HAS_ISATTY

DEBUG		= /noopt
VERSION_DEF	= VERSION="""SIO Version $(VERSION)"""

CPP_DEFS	= /define=($(VERSION_DEF),$(DEFS))

#
# The following variables shouldn't need to be changed
#
LINT_FLAGS		= -hbux
CPP_FLAGS		= $(CPP_DEFS)
CC_FLAGS		= $(DEBUG)
CFLAGS			= $(CPP_FLAGS) $(CC_FLAGS)

LIBNAME			= lib$(NAME).olb

lib: $(LIBNAME)

$(LIBNAME): $(OBJECTS)
	library/create $(LIBNAME) sprint.obj,sio.obj,siosup.obj

clean:
	del/log $(OBJECTS) $(LIBNAME) core

#
# PUT HERE THE RULES TO MAKE THE OBJECT FILES
#
sprint.obj:   sio.h impl.h sioconf.h
sio.obj:      sio.h impl.h sioconf.h events.h
siosup.obj:   sio.h impl.h sioconf.h events.h

