dnl Project   : tin - a Usenet reader
dnl Module    : aclocal.m4
dnl Author    : Thomas E. Dickey <dickey@clark.net>
dnl Created   : 24.08.95
dnl Updated   : 22.10.97
dnl Notes     :
dnl
dnl Copyright 1996,1997 by Thomas Dickey
dnl             You may  freely  copy or  redistribute  this software,
dnl             so  long as there is no profit made from its use, sale
dnl             trade or  reproduction.  You may not change this copy-
dnl             right notice, and it must be included in any copy made
dnl
dnl Macros used in TIN auto-configuration script.
dnl
dnl ---------------------------------------------------------------------------
dnl ---------------------------------------------------------------------------
dnl Add an include-directory to $CPPFLAGS.  Don't add /usr/include, since it's
dnl redundant.  Also, don't add /usr/local/include if we're using gcc.
AC_DEFUN([CF_ADD_INCDIR],
[
for cf_add_incdir in $1
do
	while true
	do
		case $cf_add_incdir in
		/usr/include) # (vi
			;;
		/usr/local/include) # (vi
			test -z "$GCC" && CPPFLAGS="$CPPFLAGS -I$cf_add_incdir"
			;;
		*) # (vi
			CPPFLAGS="$CPPFLAGS -I$cf_add_incdir"
			;;
		esac
		cf_top_incdir=`echo $cf_add_incdir | sed -e 's:/include/.*$:/include:'`
		test "$cf_top_incdir" = "$cf_add_incdir" && break
		cf_add_incdir="$cf_top_incdir"
	done
done
])dnl
dnl ---------------------------------------------------------------------------
dnl This is adapted from the macros 'fp_PROG_CC_STDC' and 'fp_C_PROTOTYPES'
dnl in the sharutils 4.2 distribution.
AC_DEFUN([CF_ANSI_CC_CHECK],
[
AC_MSG_CHECKING(for ${CC-cc} option to accept ANSI C)
AC_CACHE_VAL(cf_cv_ansi_cc,[
cf_cv_ansi_cc=no
cf_save_CFLAGS="$CFLAGS"
# Don't try gcc -ansi; that turns off useful extensions and
# breaks some systems' header files.
# AIX			-qlanglvl=ansi
# Ultrix and OSF/1	-std1
# HP-UX			-Aa -D_HPUX_SOURCE
# SVR4			-Xc
# UnixWare 1.2		(cannot use -Xc, since ANSI/POSIX clashes)
for cf_arg in "-DCC_HAS_PROTOS" "" -qlanglvl=ansi -std1 "-Aa -D_HPUX_SOURCE" -Xc
do
	CFLAGS="$cf_save_CFLAGS $cf_arg"
	AC_TRY_COMPILE(
[
#ifndef CC_HAS_PROTOS
#if !defined(__STDC__) || __STDC__ != 1
choke me
#endif
#endif
],[
	int test (int i, double x);
	struct s1 {int (*f) (int a);};
	struct s2 {int (*f) (double a);};],
	[cf_cv_ansi_cc="$cf_arg"; break])
done
CFLAGS="$cf_save_CFLAGS"
])
AC_MSG_RESULT($cf_cv_ansi_cc)

if test "$cf_cv_ansi_cc" != "no"; then
if test ".$cf_cv_ansi_cc" != ".-DCC_HAS_PROTOS"; then
	CFLAGS="$CFLAGS $cf_cv_ansi_cc"
else
	AC_DEFINE(CC_HAS_PROTOS)
fi
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl For programs that must use an ANSI compiler, obtain compiler options that
dnl will make it recognize prototypes.  We'll do preprocessor checks in other
dnl macros, since tools such as unproto can fake prototypes, but only part of
dnl the preprocessor.
AC_DEFUN([CF_ANSI_CC_REQD],
[AC_REQUIRE([CF_ANSI_CC_CHECK])
if test "$cf_cv_ansi_cc" = "no"; then
	AC_ERROR(
[Your compiler does not appear to recognize prototypes.
You have the following choices:
	a. adjust your compiler options
	b. get an up-to-date compiler
	c. use a wrapper such as unproto])
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl Allow user to disable a normally-on option.
AC_DEFUN([CF_ARG_DISABLE],
[CF_ARG_OPTION($1,[$2 (default: on)],[$3],[$4],yes)])dnl
dnl ---------------------------------------------------------------------------
dnl Allow user to enable a normally-off option.
AC_DEFUN([CF_ARG_ENABLE],
[CF_ARG_OPTION($1,[$2 (default: off)],[$3],[$4],no)])dnl
dnl ---------------------------------------------------------------------------
dnl Restricted form of AC_ARG_ENABLE that ensures user doesn't give bogus
dnl values.
dnl
dnl Parameters:
dnl $1 = option name
dnl $2 = help-string
dnl $3 = action to perform if option is not default
dnl $4 = action if perform if option is default
dnl $5 = default option value (either 'yes' or 'no')
AC_DEFUN([CF_ARG_OPTION],
[AC_ARG_ENABLE($1,[$2],[test "$enableval" != ifelse($5,no,yes,no) && enableval=ifelse($5,no,no,yes)
  if test "$enableval" != "$5" ; then
ifelse($3,,[    :]dnl
,[    $3]) ifelse($4,,,[
  else
    $4])
  fi],[enableval=$5 ifelse($4,,,[
  $4
])dnl
  ])])dnl
dnl ---------------------------------------------------------------------------
dnl Check for missing declarations in the system headers (adapted from vile).
AC_DEFUN([CF_CHECK_1_DECL],
[
AC_MSG_CHECKING([for missing "$1" extern])
AC_CACHE_VAL([cf_cv_func_$1],[
CF_MSG_LOG([for missing "$1" external])
AC_TRY_LINK([
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h>
#ifdef HAVE_ERRNO_H
#	include <errno.h>
#else
#	include <sys/errno.h>
#endif
#ifdef HAVE_LIBC_H
#	include <libc.h>
#endif
#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif
#ifdef HAVE_STDARG_H
#	include <stdarg.h>
#else
#	ifdef HAVE_VARARGS_H
#		include <varargs.h>
#	endif
#endif
#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#	include <stdlib.h>
#endif
#ifdef HAVE_STDDEF_H
#	include <stddef.h>
#endif
#ifdef HAVE_PWD_H
#	include <pwd.h>
#endif
#ifdef HAVE_NETDB_H
#	include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#	include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#	include <sys/socket.h>
#endif

#if STDC_HEADERS || HAVE_STRING_H
#	include <string.h>
  /* An ANSI string.h and pre-ANSI memory.h might conflict.  */
#	if !STDC_HEADERS && HAVE_MEMORY_H
#		include <memory.h>
#	endif /* not STDC_HEADERS and HAVE_MEMORY_H */
#else /* not STDC_HEADERS and not HAVE_STRING_H */
#	if HAVE_STRINGS_H
#		include <strings.h>
  /* memory.h and strings.h conflict on some systems */
#	endif
#endif /* not STDC_HEADERS and not HAVE_STRING_H */

/* unistd.h defines _POSIX_VERSION on POSIX.1 systems.  */
#if defined(HAVE_DIRENT_H) || defined(_POSIX_VERSION)
#	include <dirent.h>
#else /* not (HAVE_DIRENT_H or _POSIX_VERSION) */
#	ifdef HAVE_SYS_NDIR_H
#		include <sys/ndir.h>
#	endif /* HAVE_SYS_NDIR_H */
#	ifdef HAVE_SYS_DIR_H
#		include <sys/dir.h>
#	endif /* HAVE_SYS_DIR_H */
#	ifdef HAVE_NDIR_H
#		include <ndir.h>
#	endif /* HAVE_NDIR_H */
#endif /* not (HAVE_DIRENT_H or _POSIX_VERSION) */

#ifdef HAVE_SYS_FILE_H
#	include <sys/file.h>
#endif
#ifdef HAVE_SYS_STAT_H
#	include <sys/stat.h>
#endif

#ifdef TIME_WITH_SYS_TIME
#	include <sys/time.h>
#	include <time.h>
#else
#	ifdef HAVE_SYS_TIME_H
#		include <sys/time.h>
#	else
#		include <time.h>
#	endif
#endif

#ifdef HAVE_SYS_TIMES_H
#	include <sys/times.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#	include <sys/param.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#	include <sys/wait.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#	include <sys/select.h>
#endif

#ifdef HAVE_CURSES_H
#	include <curses.h>
#endif

#if 0	/* FIXME: this has prototypes, but creates new problems */
#ifdef HAVE_TERM_H
#	include <term.h>
#endif
#endif

#ifdef HAVE_TERMCAP_H
#	include <termcap.h>
#endif

#ifdef HAVE_IOCTL_H
#	include <ioctl.h>
#else
#	ifdef HAVE_SYS_IOCTL_H
#		include <sys/ioctl.h>
#	endif
#endif

#undef $1
struct zowie { int a; double b; struct zowie *c; char d; };
extern struct zowie *$1();
],
[
],
[eval 'cf_cv_func_'$1'=yes'],
[eval 'cf_cv_func_'$1'=no'])])
eval 'cf_result=$cf_cv_func_'$1
AC_MSG_RESULT($cf_result)
test $cf_result = yes && AC_DEFINE_UNQUOTED(DECL_$2)
])dnl
dnl ---------------------------------------------------------------------------
AC_DEFUN([CF_CHECK_DECL],
[for ac_func in $1
do
CF_UPPER(ac_tr_func,$ac_func)
CF_CHECK_1_DECL(${ac_func}, ${ac_tr_func})dnl
done
])dnl
dnl ---------------------------------------------------------------------------
dnl Check if the compiler allows nested parameter lists (some don't)
AC_DEFUN([CF_CHECK_NESTED_PARAMS],
[
AC_MSG_CHECKING([if nested parameters work])
AC_CACHE_VAL(cf_cv_nested_params,[
	AC_TRY_COMPILE([],
	[extern void (*sigdisp(int sig, void (*func)(int sig)))(int sig)],
	[cf_cv_nested_params=yes],
	[cf_cv_nested_params=no])
])
AC_MSG_RESULT($cf_cv_nested_params)
test $cf_cv_nested_params = yes && AC_DEFINE(HAVE_NESTED_PARAMS)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check if the compiler uses 'void *' for qsort's compare function parameters
dnl (i.e., it's an ANSI prototype).
AC_DEFUN([CF_COMPTYPE],
[
AC_MSG_CHECKING([for ANSI qsort])
AC_CACHE_VAL(cf_cv_comptype,[
	AC_TRY_COMPILE([
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif],
	[extern int compare(const void *, const void *);
	 char *foo = "string";
	 qsort(foo, sizeof(foo)/sizeof(*foo), sizeof(*foo), compare)],
	[cf_cv_comptype=yes],
	[cf_cv_comptype=no])
])
AC_MSG_RESULT($cf_cv_comptype)
if test $cf_cv_comptype = yes; then
	AC_DEFINE(HAVE_COMPTYPE_VOID)
else
	AC_DEFINE(HAVE_COMPTYPE_CHAR)
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl Check if the application can dump core (for debugging).
AC_DEFUN([CF_COREFILE],
[
AC_MSG_CHECKING([if application can dump core])
AC_CACHE_VAL(cf_cv_corefile,[
	AC_TRY_RUN([
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
int found()
{
	struct stat sb;
	return ((stat("core", &sb) == 0			/* UNIX */
	   ||    stat("conftest.core", &sb) == 0	/* FreeBSD */
		)
		&& ((sb.st_mode & S_IFMT) == S_IFREG));
}
int main()
{
#ifdef __amiga__
/* Nicholas d'Alterio (nagd@ic.ac.uk) reports that the check for ability to
 * core dump causes the machine to crash - reason unknown (gcc 2.7.2)
 */
	exit(1);
#else
	int	pid, status;
	if (found())
		unlink("core");
	if (found())
		exit(1);
	if ((pid = fork()) != 0) {
		while (wait(&status) <= 0)
			;
	} else {
		abort();	/* this will dump core, if anything will */
	}
	if (found()) {
		unlink("core");
		exit(0);
	}
	exit(1);
#endif
}],
	[cf_cv_corefile=yes],
	[cf_cv_corefile=no],
	[cf_cv_corefile=unknown])])
AC_MSG_RESULT($cf_cv_corefile)
test $cf_cv_corefile = yes && AC_DEFINE(HAVE_COREFILE)
])dnl
dnl ---------------------------------------------------------------------------
dnl Test for ANSI token expansion (used in 'assert').
AC_DEFUN([CF_CPP_CONCATS],
[
AC_MSG_CHECKING([for ansi token concatenation])
AC_CACHE_VAL(cf_cv_cpp_concats,[
	AC_TRY_COMPILE([
#define concat(a,b) a ## b],
	[char *firstlast = "y", *s = concat(first,last)],
	[cf_cv_cpp_concats=yes],
	[cf_cv_cpp_concats=no])
])
AC_MSG_RESULT($cf_cv_cpp_concats)
test $cf_cv_cpp_concats = yes && AC_DEFINE(CPP_DOES_CONCAT)
])dnl
dnl ---------------------------------------------------------------------------
dnl Test for ANSI token expansion (used in 'assert').
AC_DEFUN([CF_CPP_EXPANDS],
[
AC_MSG_CHECKING([for ansi token expansion/substitution])
AC_CACHE_VAL(cf_cv_cpp_expands,[
	AC_TRY_COMPILE([
#define string(n) #n],
	[char *s = string(token)],
	[cf_cv_cpp_expands=yes],
	[cf_cv_cpp_expands=no])
])
AC_MSG_RESULT($cf_cv_cpp_expands)
test $cf_cv_cpp_expands = yes && AC_DEFINE(CPP_DOES_EXPAND)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check if 'errno' is declared in <errno.h>
AC_DEFUN([CF_ERRNO],
[
AC_MSG_CHECKING([for errno external decl])
AC_CACHE_VAL(cf_cv_extern_errno,[
    AC_TRY_COMPILE([
#include <errno.h>],
        [int x = errno],
        [cf_cv_extern_errno=yes],
        [cf_cv_extern_errno=no])])
AC_MSG_RESULT($cf_cv_extern_errno)
test $cf_cv_extern_errno = no && AC_DEFINE(DECL_ERRNO)
])dnl
dnl ---------------------------------------------------------------------------
dnl Look for a non-standard library, given parameters for AC_TRY_LINK.  We
dnl prefer a standard location, and use -L options only if we do not find the
dnl library in the standard library location(s).
dnl	$1 = library name
dnl	$2 = includes
dnl	$3 = code fragment to compile/link
dnl	$4 = corresponding function-name
dnl
dnl Sets the variable "$cf_libdir" as a side-effect, so we can see if we had
dnl to use a -L option.
AC_DEFUN([CF_FIND_LIBRARY],
[
	cf_cv_have_lib_$1=no
	cf_libdir=""
	AC_CHECK_FUNC($4,cf_cv_have_lib_$1=yes,[
		cf_save_LIBS="$LIBS"
		AC_MSG_CHECKING(for $4 in -l$1)
		LIBS="-l$1 $LIBS"
		AC_TRY_LINK([$2],[$3],
			[AC_MSG_RESULT(yes)
			 cf_cv_have_lib_$1=yes
			],
			[AC_MSG_RESULT(no)
			CF_LIBRARY_PATH(cf_search,$1)
			for cf_libdir in $cf_search
			do
				AC_MSG_CHECKING(for -l$1 in $cf_libdir)
				LIBS="-L$cf_libdir -l$1 $cf_save_LIBS"
				AC_TRY_LINK([$2],[$3],
					[AC_MSG_RESULT(yes)
			 		 cf_cv_have_lib_$1=yes
					 break],
					[AC_MSG_RESULT(no)
					 LIBS="$cf_save_LIBS"])
			done
			])
		])
if test $cf_cv_have_lib_$1 = no ; then
	AC_ERROR(Cannot link $1 library)
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl Check if 'fork()' is available, and working.  Amiga (and possibly other
dnl machines) have a non-working 'fork()' entrypoint.
AC_DEFUN([CF_FUNC_FORK],
[AC_MSG_CHECKING([for fork])
AC_CACHE_VAL(cf_cv_func_fork,[
AC_TRY_RUN([
int main()
{
	if (fork() < 0)
		exit(1);
	exit(0);
}],	[cf_cv_func_fork=yes],
	[cf_cv_func_fork=no],
	[cf_cv_func_fork=unknown])
])dnl
AC_MSG_RESULT($cf_cv_func_fork)
test $cf_cv_func_fork = yes && AC_DEFINE(HAVE_FORK)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check if the 'system()' function returns a usable status, or if not, try
dnl to use the status returned by a SIGCHLD.
AC_DEFUN([CF_FUNC_SYSTEM],
[
AC_REQUIRE([CF_UNION_WAIT])
AC_MSG_CHECKING(if the system function returns usable child-status)
AC_CACHE_VAL(cf_cv_system_status,[
	AC_TRY_RUN([
#include <stdio.h>
#include <signal.h>
#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

RETSIGTYPE signal_handler (int sig)
{
#if HAVE_TYPE_UNIONWAIT
	union wait wait_status;
#else
	int wait_status = 1;
#endif
	int system_status;
	wait (&wait_status);
	system_status = WEXITSTATUS(wait_status); /* should be nonzero */
	exit(system_status != 23);
}

int main()
{
	/* this looks weird, but apparently the SIGCHLD gets there first on
	 * machines where 'system()' doesn't return a usable code, so ...
	 */
	signal (SIGCHLD, signal_handler);
	system("exit 23");
	exit(1);
}
],
	[cf_cv_system_status=no],
	[AC_TRY_RUN(
	[int main() { exit(system("exit 23") != (23 << 8)); }],
	[cf_cv_system_status=yes],
	[cf_cv_system_status=unknown],
	[cf_cv_system_status=unknown])],
	[cf_cv_system_status=unknown])
])
AC_MSG_RESULT($cf_cv_system_status)
test $cf_cv_system_status = no && AC_DEFINE(USE_SYSTEM_STATUS)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check if the compiler supports useful warning options.  There's a few that
dnl we don't use, simply because they're too noisy:
dnl
dnl	-Wconversion (useful in older versions of gcc, but not in gcc 2.7.x)
dnl	-Wredundant-decls (system headers make this too noisy)
dnl	-Wtraditional (combines too many unrelated messages, only a few useful)
dnl	-Wwrite-strings (too noisy, but should review occasionally)
dnl	-pedantic
dnl
AC_DEFUN([CF_GCC_WARNINGS],
[
if test -n "$GCC"
then
	changequote(,)dnl
	cat > conftest.$ac_ext <<EOF
#line __oline__ "configure"
int main(int argc, char *argv[]) { return argv[argc-1] == 0; }
EOF
	changequote([,])dnl
	AC_CHECKING([for gcc warning options])
	cf_save_CFLAGS="$CFLAGS"
	EXTRA_CFLAGS="-W -Wall"
	cf_warn_CONST=""
	test "$with_ext_const" = yes && cf_warn_CONST="Wwrite-strings"
	for cf_opt in \
		Wbad-function-cast \
		Wcast-align \
		Wcast-qual \
		Winline \
		Wmissing-declarations \
		Wmissing-prototypes \
		Wnested-externs \
		Wpointer-arith \
		Wshadow \
		Wstrict-prototypes $cf_warn_CONST
	do
		CFLAGS="$cf_save_CFLAGS $EXTRA_CFLAGS -$cf_opt"
		if AC_TRY_EVAL(ac_compile); then
			test -n "$verbose" && AC_MSG_RESULT(... -$cf_opt)
			EXTRA_CFLAGS="$EXTRA_CFLAGS -$cf_opt"
			test "$cf_opt" = Wcast-qual && EXTRA_CFLAGS="$EXTRA_CFLAGS -DXTSTRINGDEFINES"
		fi
	done
	rm -f conftest*
	CFLAGS="$cf_save_CFLAGS"
fi
AC_SUBST(EXTRA_CFLAGS)
])dnl
dnl ---------------------------------------------------------------------------
dnl Construct a search-list for a nonstandard header-file
AC_DEFUN([CF_HEADER_PATH],
[$1=""
if test -d "$includedir"  ; then
test "$includedir" != NONE       && $1="[$]$1 $includedir $includedir/$2"
fi
if test -d "$oldincludedir"  ; then
test "$oldincludedir" != NONE    && $1="[$]$1 $oldincludedir $oldincludedir/$2"
fi
if test -d "$prefix"; then
test "$prefix" != NONE           && $1="[$]$1 $prefix/include $prefix/include/$2"
fi
test "$prefix" != /usr/local     && $1="[$]$1 /usr/local/include /usr/local/include/$2"
test "$prefix" != /usr           && $1="[$]$1 /usr/include /usr/include/$2"
])dnl
dnl ---------------------------------------------------------------------------
dnl Construct a search-list for a nonstandard library-file
AC_DEFUN([CF_LIBRARY_PATH],
[$1=""
if test -d "$libdir"  ; then
test "$libdir" != NONE           && $1="[$]$1 $libdir $libdir/$2"
fi
if test -d "$exec_prefix"; then
test "$exec_prefix" != NONE      && $1="[$]$1 $exec_prefix/lib $exec_prefix/lib/$2"
fi
if test -d "$prefix"; then
test "$prefix" != NONE           && \
test "$prefix" != "$exec_prefix" && $1="[$]$1 $prefix/lib $prefix/lib/$2"
fi
test "$prefix" != /usr/local     && $1="[$]$1 /usr/local/lib /usr/local/lib/$2"
test "$prefix" != /usr           && $1="[$]$1 /usr/lib /usr/lib/$2"
])dnl
dnl ---------------------------------------------------------------------------
dnl Some 'make' programs support $(MAKEFLAGS), some $(MFLAGS), to pass 'make'
dnl options to lower-levels.  It's very useful for "make -n" -- if we have it.
dnl (GNU 'make' does both :-)
AC_DEFUN([CF_MAKEFLAGS],
[
AC_MSG_CHECKING([for makeflags variable])
AC_CACHE_VAL(cf_cv_makeflags,[
	cf_cv_makeflags=''
	for cf_option in '$(MFLAGS)' '-$(MAKEFLAGS)'
	do
		cat >cf_makeflags.tmp <<CF_EOF
all :
	echo '.$cf_option'
CF_EOF
		set cf_result=`${MAKE-make} -f cf_makeflags.tmp 2>/dev/null`
		if test "$cf_result" != "."
		then
			cf_cv_makeflags=$cf_option
			break
		fi
	done
	rm -f cf_makeflags.tmp])
AC_MSG_RESULT($cf_cv_makeflags)
AC_SUBST(cf_cv_makeflags)
])dnl
dnl ---------------------------------------------------------------------------
dnl Write a debug message to config.log, along with the line number in the
dnl configure script.
AC_DEFUN([CF_MSG_LOG],[
echo "(line __oline__) testing $* ..." 1>&AC_FD_CC
])dnl
dnl ---------------------------------------------------------------------------
dnl Look for the SVr4 curses clone 'ncurses' in the standard places, adjusting
dnl the CPPFLAGS variable.
dnl
dnl The header files may be installed as either curses.h, or ncurses.h
dnl (obsolete).  If not installed for overwrite, the curses.h file would be
dnl in an ncurses subdirectory (e.g., /usr/include/ncurses), but someone may
dnl have installed overwriting the vendor's curses.  Only very old versions
dnl (pre-1.9.2d, the first autoconf'd version) of ncurses don't define
dnl either __NCURSES_H or NCURSES_VERSION in the header.
dnl
dnl If the installer has set $CFLAGS or $CPPFLAGS so that the ncurses header
dnl is already in the include-path, don't even bother with this, since we cannot
dnl easily determine which file it is.  In this case, it has to be <curses.h>.
dnl
AC_DEFUN([CF_NCURSES_CPPFLAGS],
[
AC_MSG_CHECKING(for ncurses header file)
AC_CACHE_VAL(cf_cv_ncurses_header,[
	AC_TRY_COMPILE([#include <curses.h>],[
#ifdef NCURSES_VERSION
printf("%s\n", NCURSES_VERSION);
#else
#ifdef __NCURSES_H
printf("old\n");
#else
make an error
#endif
#endif
	],
	[cf_cv_ncurses_header=predefined],[
	CF_HEADER_PATH(cf_search,ncurses)
	test -n "$verbose" && echo
	for cf_incdir in $cf_search
	do
		for cf_header in \
			curses.h \
			ncurses.h
		do
changequote(,)dnl
			if egrep "NCURSES_[VH]" $cf_incdir/$cf_header 1>&AC_FD_CC 2>&1; then
changequote([,])dnl
				cf_cv_ncurses_header=$cf_incdir/$cf_header
				test -n "$verbose" && echo $ac_n "	... found $ac_c" 1>&AC_FD_MSG
				break
			fi
			test -n "$verbose" && echo "	... tested $cf_incdir/$cf_header" 1>&AC_FD_MSG
		done
		test -n "$cf_cv_ncurses_header" && break
	done
	test -z "$cf_cv_ncurses_header" && AC_ERROR(not found)
	])])
AC_MSG_RESULT($cf_cv_ncurses_header)
AC_DEFINE(NCURSES)

changequote(,)dnl
cf_incdir=`echo $cf_cv_ncurses_header | sed -e 's:/[^/]*$::'`
changequote([,])dnl

case $cf_cv_ncurses_header in # (vi
*/ncurses.h)
	AC_DEFINE(HAVE_NCURSES_H)
	;;
esac

case $cf_cv_ncurses_header in # (vi
predefined) # (vi
	cf_cv_ncurses_header=curses.h
	;;
*)
	CF_ADD_INCDIR($cf_incdir)
	;;
esac
CF_NCURSES_VERSION
])dnl
dnl ---------------------------------------------------------------------------
dnl Look for the ncurses library.  This is a little complicated on Linux,
dnl because it may be linked with the gpm (general purpose mouse) library.
dnl Some distributions have gpm linked with (bsd) curses, which makes it
dnl unusable with ncurses.  However, we don't want to link with gpm unless
dnl ncurses has a dependency, since gpm is normally set up as a shared library,
dnl and the linker will record a dependency.
AC_DEFUN([CF_NCURSES_LIBS],
[AC_REQUIRE([CF_NCURSES_CPPFLAGS])

cf_ncurses_LIBS=""
AC_CHECK_LIB(gpm,Gpm_Open,[AC_CHECK_LIB(gpm,initscr,,[cf_ncurses_LIBS="-lgpm"])])

case $host_os in #(vi
freebsd*)
	# This is only necessary if you are linking against an obsolete
	# version of ncurses (but it should do no harm, since it's static).
	AC_CHECK_LIB(mytinfo,tgoto,[cf_ncurses_LIBS="-lmytinfo $cf_ncurses_LIBS"])
	;;
esac

LIBS="$cf_ncurses_LIBS $LIBS"
CF_FIND_LIBRARY(ncurses,
	[#include <$cf_cv_ncurses_header>],
	[initscr()],
	initscr)

if test -n "$cf_ncurses_LIBS" ; then
	AC_MSG_CHECKING(if we can link ncurses without $cf_ncurses_LIBS)
	cf_ncurses_SAVE="$LIBS"
	for p in $cf_ncurses_LIBS ; do
		q=`echo $LIBS | sed -e 's/'$p' //' -e 's/'$p'$//'`
		if test "$q" != "$LIBS" ; then
			LIBS="$q"
		fi
	done
	AC_TRY_LINK([#include <$cf_cv_ncurses_header>],
		[initscr(); mousemask(0,0); tgoto((char *)0, 0, 0);],
		[AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)
		 LIBS="$cf_ncurses_SAVE"])
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl Check for the version of ncurses, to aid in reporting bugs, etc.
AC_DEFUN([CF_NCURSES_VERSION],
[AC_MSG_CHECKING(for ncurses version)
AC_CACHE_VAL(cf_cv_ncurses_version,[
	cf_cv_ncurses_version=no
	cf_tempfile=out$$
	AC_TRY_RUN([
#include <$cf_cv_ncurses_header>
int main()
{
	FILE *fp = fopen("$cf_tempfile", "w");
#ifdef NCURSES_VERSION
# ifdef NCURSES_VERSION_PATCH
	fprintf(fp, "%s.%d\n", NCURSES_VERSION, NCURSES_VERSION_PATCH);
# else
	fprintf(fp, "%s\n", NCURSES_VERSION);
# endif
#else
# ifdef __NCURSES_H
	fprintf(fp, "old\n");
# else
	make an error
# endif
#endif
	exit(0);
}],[
	cf_cv_ncurses_version=`cat $cf_tempfile`
	rm -f $cf_tempfile],,[

	# This will not work if the preprocessor splits the line after the
	# Autoconf token.  The 'unproto' program does that.
	cat > conftest.$ac_ext <<EOF
#include <$cf_cv_ncurses_header>
#undef Autoconf
#ifdef NCURSES_VERSION
Autoconf NCURSES_VERSION
#else
#ifdef __NCURSES_H
Autoconf "old"
#endif
;
#endif
EOF
	cf_try="$ac_cpp conftest.$ac_ext 2>&AC_FD_CC | grep '^Autoconf ' >conftest.out"
	AC_TRY_EVAL(cf_try)
	if test -f conftest.out ; then
changequote(,)dnl
		cf_out=`cat conftest.out | sed -e 's@^Autoconf @@' -e 's@^[^"]*"@@' -e 's@".*@@'`
changequote([,])dnl
		test -n "$cf_out" && cf_cv_ncurses_version="$cf_out"
		rm -f conftest.out
	fi
])])
AC_MSG_RESULT($cf_cv_ncurses_version)
])
dnl ---------------------------------------------------------------------------
dnl After checking for functions in the default $LIBS, make a further check
dnl for the functions that are netlib-related (these aren't always in the
dnl libc, etc., and have to be handled specially because there are conflicting
dnl and broken implementations.
dnl Common library requirements (in order):
dnl	-lresolv -lsocket -lnsl
dnl	-lnsl -lsocket
dnl	-lsocket
dnl	-lbsd
AC_DEFUN([CF_NETLIBS],[
cf_test_netlibs=no
AC_MSG_CHECKING(for network libraries)
AC_CACHE_VAL(cf_cv_netlibs,[
AC_MSG_RESULT(working...)
cf_cv_netlibs=""
cf_test_netlibs=yes
AC_CHECK_FUNCS(gethostname,,[
	CF_RECHECK_FUNC(gethostname,nsl,cf_cv_netlibs,[
		CF_RECHECK_FUNC(gethostname,socket,cf_cv_netlibs)])])
#
# FIXME:  sequent needs this library (i.e., -lsocket -linet -lnsl), but
# I don't know the entrypoints - 97/7/22 TD
AC_HAVE_LIBRARY(inet,cf_cv_netlibs="-linet $cf_cv_netlibs")
#
if test "$ac_cv_func_lsocket" != no ; then
AC_CHECK_FUNCS(socket,,[
	CF_RECHECK_FUNC(socket,socket,cf_cv_netlibs,[
		CF_RECHECK_FUNC(socket,bsd,cf_cv_netlibs)])])
fi
#
AC_CHECK_FUNCS(gethostbyname,,[
	CF_RECHECK_FUNC(gethostbyname,nsl,cf_cv_netlibs)])
#
AC_CHECK_FUNCS(strcasecmp,,[
	CF_RECHECK_FUNC(strcasecmp,resolv,cf_cv_netlibs)])
])
LIBS="$LIBS $cf_cv_netlibs"
test $cf_test_netlibs = no && echo "$cf_cv_netlibs" >&AC_FD_MSG
])dnl
dnl ---------------------------------------------------------------------------
dnl See if sum can take -r
AC_DEFUN([CF_PROG_SUM_R],
[
if test $ac_cv_path_PATH_SUM
then
AC_MSG_CHECKING([if $ac_cv_path_PATH_SUM takes -r])
AC_CACHE_VAL(ac_cv_prog_sum_r,[
if AC_TRY_COMMAND($ac_cv_path_PATH_SUM -r config.log 1>&AC_FD_CC)
then
	ac_cv_prog_sum_r=yes
else
	ac_cv_prog_sum_r=no
fi
])
if test $ac_cv_prog_sum_r = yes; then
	AC_DEFINE(SUM_TAKES_DASH_R)
	AC_DEFINE_UNQUOTED(PATH_SUM_R, "$ac_cv_path_PATH_SUM -r")
else
	AC_DEFINE_UNQUOTED(PATH_SUM_R, "$ac_cv_path_PATH_SUM")
fi
AC_MSG_RESULT($ac_cv_prog_sum_r)
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl Re-check on a function to see if we can pick it up by adding a library.
dnl	$1 = function to check
dnl	$2 = library to check in
dnl	$3 = environment to update (e.g., $LIBS)
dnl	$4 = what to do if this fails
dnl
dnl This uses 'unset' if the shell happens to support it, but leaves the
dnl configuration variable set to 'unknown' if not.  This is a little better
dnl than the normal autoconf test, which gives misleading results if a test
dnl for the function is made (e.g., with AC_CHECK_FUNC) after this macro is
dnl used (autoconf does not distinguish between a null token and one that is
dnl set to 'no').
AC_DEFUN([CF_RECHECK_FUNC],[
AC_CHECK_LIB($2,$1,[
	CF_UPPER(cf_tr_func,$1)
	AC_DEFINE_UNQUOTED(HAVE_$cf_tr_func)
	ac_cv_func_$1=yes
	$3="-l$2 [$]$3"],[
	ac_cv_func_$1=unknown
	unset ac_cv_func_$1 2>/dev/null
	$4],
	[[$]$3])
])dnl
dnl ---------------------------------------------------------------------------
dnl Attempt to determine if we've got one of the flavors of regular-expression
dnl code that we can support.
AC_DEFUN([CF_REGEX],
[
AC_MSG_CHECKING([for regular-expression headers])
AC_CACHE_VAL(cf_cv_regex,[
AC_TRY_LINK([#include <sys/types.h>
#include <regex.h>],[
	regex_t *p;
	int x = regcomp(p, "", 0);
	int y = regexec(p, "", 0, 0, 0);
	regfree(p);
	],[cf_cv_regex="regex.h"],[
	AC_TRY_LINK([#include <regexp.h>],[
		char *p = compile("", "", "", 0);
		int x = step("", "");
	],[cf_cv_regex="regexp.h"],[
		cf_save_LIBS="$LIBS"
		LIBS="-lgen $LIBS"
		AC_TRY_LINK([#include <regexpr.h>],[
			char *p = compile("", "", "");
			int x = step("", "");
		],[cf_cv_regex="regexpr.h"],[LIBS="$cf_save_LIBS"])])])
])
AC_MSG_RESULT($cf_cv_regex)
case $cf_cv_regex in
	regex.h)   AC_DEFINE(HAVE_REGEX_H_FUNCS) ;;
	regexp.h)  AC_DEFINE(HAVE_REGEXP_H_FUNCS) ;;
	regexpr.h) AC_DEFINE(HAVE_REGEXPR_H_FUNCS) ;;
esac
])dnl
dnl ---------------------------------------------------------------------------
dnl Check for the functions that set effective/real uid/gid.  This has to
dnl follow the AC_CHECK_FUNCS call.
AC_DEFUN([CF_SET_GID_UID],
[
AC_MSG_CHECKING([for setuid-like functions])
AC_CACHE_VAL(cf_cv_setuid_funcs,[
if test -n "${ac_cv_func_setuid}${ac_cv_func_seteuid}${ac_cv_func_setreuid}"; then
	cf_cv_setuid_funcs=yes
else
	cf_cv_setuid_funcs=no
fi
])
AC_MSG_RESULT($cf_cv_setuid_funcs)
test $cf_cv_setuid_funcs = yes && AC_DEFINE(HAVE_SET_GID_UID)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check for systems that have signal-handlers prototyped with one argument
dnl versus those with more than one argument, define the symbol SIG_ARGS to
dnl match.  (If it's empty, that's ok too).
AC_DEFUN([CF_SIG_ARGS],
[
AC_MSG_CHECKING([declaration of signal arguments])
AC_CACHE_VAL(cf_cv_sig_args,[
cf_cv_sig_args=
for cf_test in "int sig" "int sig, ..."
do
	AC_TRY_COMPILE([
#include <signal.h>],
	[extern RETSIGTYPE catch($cf_test); signal(SIGINT, catch)],
	[cf_cv_sig_args="$cf_test";break])
done
])
AC_MSG_RESULT($cf_cv_sig_args)
AC_DEFINE_UNQUOTED(SIG_ARGS,$cf_cv_sig_args)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check for systems where the special signal constants aren't prototyped
dnl (there's a lot of them, and the compiler can generate a lot of warning
dnl messages that make it hard to pick out genuine errors).
AC_DEFUN([CF_SIG_CONST],
[
AC_REQUIRE([CF_SIG_ARGS])
AC_MSG_CHECKING([for redefinable signal constants])
AC_CACHE_VAL(cf_cv_sig_const,[
cf_cv_sig_const=no
if test -n "$cf_cv_sig_args"; then
	cf_test=`echo $cf_cv_sig_args|sed -e s/sig//`
	AC_TRY_RUN([
#define NEW_DFL	((RETSIGTYPE (*)($cf_test))0)
#define NEW_IGN	((RETSIGTYPE (*)($cf_test))1)
#define NEW_ERR	((RETSIGTYPE (*)($cf_test))-1)

#include <signal.h>

int main()
{
	if (NEW_DFL != SIG_DFL
	 || NEW_IGN != SIG_IGN
	 || NEW_ERR != SIG_ERR
	 /* at least one system won't let me redefine these! */
#undef SIG_DFL
#undef SIG_IGN
#undef SIG_ERR
#define SIG_DFL NEW_DFL
#define SIG_IGN NEW_IGN
#define SIG_ERR NEW_ERR
	 || NEW_DFL != SIG_DFL)
	 	exit(1);
	signal(SIGINT, SIG_DFL);
	exit(0);
}],
	[cf_cv_sig_const=yes],
	[cf_cv_sig_const=no],
	[cf_cv_sig_const=unknown])
fi
])
AC_MSG_RESULT($cf_cv_sig_const)
test "$cf_cv_sig_const" = yes && AC_DEFINE(DECL_SIG_CONST)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check for socks5 configuration
AC_DEFUN([CF_SOCKS5],[
AC_MSG_CHECKING(if we can link against socks5 library)
AC_CACHE_VAL(cf_cv_lib_socks5,[
LIBS="$LIBS -lsocks5"
AC_TRY_LINK([
#define SOCKS
#include <socks.h>],[
#ifdef USE_SOCKS4_PREFIX
	Rinit((char *)0);
#else
	SOCKSinit((char *)0);
#endif
	getpeername(0, (struct sockaddr *)0, (int *)0);],
	[cf_cv_lib_socks5=yes],
	[cf_cv_lib_socks5=no])
])
AC_MSG_RESULT($cf_cv_lib_socks5)
if test $cf_cv_lib_socks5 = yes ; then
	AC_DEFINE(USE_SOCKS5)
else
	AC_ERROR(Sorry.  Cannot link against socks5 library)
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl Check for declaration of sys_errlist in one of stdio.h and errno.h.
dnl Declaration of sys_errlist on BSD4.4 interferes with our declaration.
dnl Reported by Keith Bostic.
AC_DEFUN([CF_SYS_ERRLIST],
[
AC_MSG_CHECKING([declaration of sys_errlist])
AC_CACHE_VAL(cf_cv_dcl_sys_errlist,[
    AC_TRY_COMPILE([
#include <stdio.h>
#include <sys/types.h>
#include <errno.h> ],
    [char *c = (char *) *sys_errlist],
    [cf_cv_dcl_sys_errlist=yes],
    [cf_cv_dcl_sys_errlist=no])])
AC_MSG_RESULT($cf_cv_dcl_sys_errlist)

# It's possible (for near-UNIX clones) that sys_errlist doesn't exist
if test $cf_cv_dcl_sys_errlist = no ; then
    AC_DEFINE(DECL_SYS_ERRLIST)
    AC_MSG_CHECKING([existence of sys_errlist])
    AC_CACHE_VAL(cf_cv_have_sys_errlist,[
        AC_TRY_LINK([#include <errno.h>],
            [char *c = (char *) *sys_errlist],
            [cf_cv_have_sys_errlist=yes],
            [cf_cv_have_sys_errlist=no])])
    AC_MSG_RESULT($cf_cv_have_sys_errlist)
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl Derive the system name, as a check for reusing the autoconf cache
AC_DEFUN([CF_SYS_NAME],
[
SYS_NAME=`(uname -s -r || uname -a || hostname) 2>/dev/null | sed 1q`
test -z "$SYS_NAME" && SYS_NAME=unknown
AC_DEFINE_UNQUOTED(SYS_NAME,"$SYS_NAME")

AC_CACHE_VAL(cf_cv_system_name,[cf_cv_system_name="$SYS_NAME"])

if test ".$SYS_NAME" != ".$cf_cv_system_name" ; then
	AC_MSG_RESULT("Cached system name does not agree with actual")
	AC_ERROR("Please remove config.cache and try again.")
fi])
dnl ---------------------------------------------------------------------------
dnl See if we can link with the termios functions tcsetattr/tcgetattr
AC_DEFUN([CF_TERMIOS],
[
AC_MSG_CHECKING([for nonconflicting termios.h])
AC_CACHE_VAL(cf_cv_use_termios_h,[
	AC_TRY_LINK([
#ifdef HAVE_IOCTL_H
#	include <ioctl.h>
#else
#	ifdef HAVE_SYS_IOCTL_H
#		include <sys/ioctl.h>
#	endif
#endif

#if !defined(sun) || !defined(NL0)
#include <termios.h>
#endif
],[
	struct termios save_tty;
	(void) tcsetattr (0, TCSANOW, &save_tty);
	(void) tcgetattr (0, &save_tty)],
	[cf_cv_use_termios_h=yes],
	[cf_cv_use_termios_h=no])
])
AC_MSG_RESULT($cf_cv_use_termios_h)
if test $cf_cv_use_termios_h = yes; then
	AC_DEFINE(HAVE_TERMIOS_H)
	AC_DEFINE(HAVE_TCGETATTR)
	AC_DEFINE(HAVE_TCSETATTR)
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl On some systems ioctl(fd, TIOCGWINSZ, &size) will always return {0,0} until
dnl ioctl(fd, TIOCSWINSZ, &size) is called to explicitly set the size of the
dnl screen.
dnl
dnl Attempt to determine if we're on such a system by running a test-program.
dnl This won't work, of course, if the configure script is run in batch mode,
dnl since we've got to have access to the terminal.
AC_DEFUN([CF_TIOCGWINSZ],
[
AC_MSG_CHECKING([for working TIOCGWINSZ])
AC_CACHE_VAL(cf_cv_use_tiocgwinsz,[
	cf_save="$CFLAGS"
	CFLAGS="-I. -I$srcdir/include -DHAVE_CONFIG_H -D__CPROTO__ $CFLAGS"
	rm -f autoconf.h
	echo > autoconf.h
	AC_TRY_RUN([
#ifndef M_UNIX
#define M_UNIX
#endif
#include <tin.h>
int main()
{
	int fd;
	for (fd = 0; fd <= 2; fd++) {	/* try in/out/err in case redirected */
#ifdef TIOCGSIZE
		struct ttysize size;
		if (ioctl (0, TIOCGSIZE, &size) == 0
		 && size.ts_lines > 0
		 && size.ts_cols > 0)
			exit(0);
#else
		struct winsize size;
		if (ioctl(0, TIOCGWINSZ, &size) == 0
		 && size.ws_row > 0
		 && size.ws_col > 0)
			exit(0);
#endif
	}
	exit(0);	/* we cannot guarantee this is run interactively */
}],
		[cf_cv_use_tiocgwinsz=yes],
		[cf_cv_use_tiocgwinsz=no],
		[cf_cv_use_tiocgwinsz=unknown])
		rm -f autoconf.h
		CFLAGS="$cf_save"])
AC_MSG_RESULT($cf_cv_use_tiocgwinsz)
test $cf_cv_use_tiocgwinsz != yes && AC_DEFINE(DONT_HAVE_SIGWINCH)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check if the tm-struct defines the '.tm_gmtoff' member (useful in decoding
dnl dates).
AC_DEFUN([CF_TM_GMTOFF],
[
AC_MSG_CHECKING([for tm.tm_gmtoff])
AC_CACHE_VAL(cf_cv_tm_gmtoff,[
	AC_TRY_COMPILE([
#ifdef TIME_WITH_SYS_TIME
#	include <sys/time.h>
#	include <time.h>
#else
#	ifdef HAVE_SYS_TIME_H
#		include <sys/time.h>
#	else
#		include <time.h>
#	endif
#endif
],[
	struct tm foo;
	long bar = foo.tm_gmtoff],
	[cf_cv_tm_gmtoff=yes],
	[cf_cv_tm_gmtoff=no])])
AC_MSG_RESULT($cf_cv_tm_gmtoff)
test $cf_cv_tm_gmtoff = no && AC_DEFINE(DONT_HAVE_TM_GMTOFF)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check for return and param type of 3rd -- OutChar() -- param of tputs().
AC_DEFUN([CF_TYPE_OUTCHAR],
[AC_MSG_CHECKING([declaration of tputs 3rd param])
AC_CACHE_VAL(cf_cv_type_outchar,[
cf_cv_type_outchar="int OutChar(int)"
cf_cv_found=no
for P in int void; do
for Q in int void; do
for R in int char; do
for S in "" const; do
	AC_TRY_COMPILE([
#ifdef USE_TERMINFO
#include <curses.h>
#if HAVE_TERM_H
#include <term.h>
#endif
#else
#if HAVE_CURSES_H
#include <curses.h>	/* FIXME: this should be included only for terminfo */
#endif
#if HAVE_TERMCAP_H
#include <termcap.h>
#endif
#endif ],
	[extern $Q OutChar($R);
	extern $P tputs ($S char *string, int nlines, $Q (*_f)($R));
	tputs("", 1, OutChar)],
	[cf_cv_type_outchar="$Q OutChar($R)"
	 cf_cv_found=yes
	 break])
done
	test $cf_cv_found = yes && break
done
	test $cf_cv_found = yes && break
done
	test $cf_cv_found = yes && break
done
	])
AC_MSG_RESULT($cf_cv_type_outchar)
case $cf_cv_type_outchar in
int*)
	AC_DEFINE(OUTC_RETURN)
	;;
esac
case $cf_cv_type_outchar in
*char*)
	AC_DEFINE(OUTC_ARGS,char c)
	;;
esac
])dnl
dnl ---------------------------------------------------------------------------
dnl
AC_DEFUN([CF_TYPE_SIGACTION],
[
AC_MSG_CHECKING([for type sigaction_t])
AC_CACHE_VAL(cf_cv_type_sigaction,[
	AC_TRY_COMPILE([
#include <signal.h>],
		[sigaction_t x],
		[cf_cv_type_sigaction=yes],
		[cf_cv_type_sigaction=no])])
AC_MSG_RESULT($cf_cv_type_sigaction)
test $cf_cv_type_sigaction = yes && AC_DEFINE(HAVE_TYPE_SIGACTION)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check to see if the BSD-style union wait is declared.  Some platforms may
dnl use this, though it is deprecated in favor of the 'int' type in Posix.
dnl Some vendors provide a bogus implementation that declares union wait, but
dnl uses the 'int' type instead; we try to spot these by checking for the
dnl associated macros.
dnl
dnl Ahem.  Some implementers cast the status value to an int*, as an attempt to
dnl use the macros for either union wait or int.  So we do a check compile to
dnl see if the macros are defined and apply to an int.
dnl
dnl Sets: $cf_cv_type_unionwait
dnl Defines: HAVE_TYPE_UNIONWAIT
AC_DEFUN([CF_UNION_WAIT],
[
AC_REQUIRE([CF_WAIT_HEADERS])
AC_MSG_CHECKING([for union wait])
AC_CACHE_VAL(cf_cv_type_unionwait,[
	AC_TRY_COMPILE($cf_wait_headers,
	[union wait x;
	 int y = WEXITSTATUS(x);
	 int z = WTERMSIG(x);
	],
	[cf_cv_type_unionwait=no],[
	AC_TRY_COMPILE($cf_wait_headers,
	[union wait x;
#ifdef WEXITSTATUS
	 int y = WEXITSTATUS(x);
#endif
#ifdef WTERMSIG
	 int z = WTERMSIG(x);
#endif
	],
	[cf_cv_type_unionwait=yes],
	[cf_cv_type_unionwait=no])])])
AC_MSG_RESULT($cf_cv_type_unionwait)
test $cf_cv_type_unionwait = yes && AC_DEFINE(HAVE_TYPE_UNIONWAIT)
])dnl
dnl ---------------------------------------------------------------------------
dnl Make an uppercase version of a variable
dnl $1=uppercase($2)
AC_DEFUN([CF_UPPER],
[
changequote(,)dnl
$1=`echo $2 | tr '[a-z]' '[A-Z]'`
changequote([,])dnl
])dnl
dnl ---------------------------------------------------------------------------
dnl Build up an expression $cf_wait_headers with the header files needed to
dnl compile against the prototypes for 'wait()', 'waitpid()', etc.  Assume it's
dnl Posix, which uses <sys/types.h> and <sys/wait.h>, but allow SVr4 variation
dnl with <wait.h>.
AC_DEFUN([CF_WAIT_HEADERS],
[
AC_HAVE_HEADERS(sys/wait.h)
cf_wait_headers="#include <sys/types.h>
"
if test $ac_cv_header_sys_wait_h = yes; then
cf_wait_headers="$cf_wait_headers
#include <sys/wait.h>
"
else
AC_HAVE_HEADERS(wait.h)
AC_HAVE_HEADERS(waitstatus.h)
if test $ac_cv_header_wait_h = yes; then
cf_wait_headers="$cf_wait_headers
#include <wait.h>
"
fi
if test $ac_cv_header_waitstatus_h = yes; then
cf_wait_headers="$cf_wait_headers
#include <waitstatus.h>
"
fi
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl Wrapper for AC_ARG_WITH to inherit/override an environment variable's
dnl "#define" in the compile.
AC_DEFUN([CF_WITH_DFTENV],
[AC_ARG_WITH($1,[$2 ](default: ifelse($4,,empty,$4)),,
ifelse($4,,[withval="${$3}"],[withval="${$3-$4}"]))dnl
case "$withval" in #(vi
yes|no)
  echo 'configure: error: expected a value for $3' 1>&2
  exit 1
  ;;
esac
$3="$withval"
AC_DEFINE_UNQUOTED($3,"[$]$3")dnl
])dnl
dnl ---------------------------------------------------------------------------
dnl Wrapper for AC_ARG_WITH to ensure that user supplies a pathname, not just
dnl defaulting to yes/no.
dnl
dnl $1 = option name
dnl $2 = help-text
dnl $3 = environment variable to set
dnl $4 = default value, shown in the help-message, must be a constant
dnl $5 = default value, if it's an expression & cannot be in the help-message
dnl
AC_DEFUN([CF_WITH_PATH],
[AC_ARG_WITH($1,[$2 ](default: ifelse($4,,empty,$4)),,
ifelse($4,,[withval="${$3}"],[withval="${$3-ifelse($5,,$4,$5)}"]))dnl
case ".$withval" in #(vi
./*) #(vi
  ;;
.\[$]{*prefix}*) #(vi
  eval withval="$withval"
  case ".$withval" in #(vi
  .NONE/*)
    withval=`echo $withval | sed -e s@NONE@$ac_default_prefix@`
    ;;
  esac
  ;; #(vi
.NONE/*)
  withval=`echo $withval | sed -e s@NONE@$ac_default_prefix@`
  ;;
*)
  AC_ERROR(expected a pathname for $1)
  ;;
esac
eval $3="$withval"
AC_SUBST($3)dnl
])dnl
dnl ---------------------------------------------------------------------------
dnl Wrapper for AC_PATH_PROG, with command-line option.
dnl Params:
dnl $1 = program name
dnl $2 = help-string (I'd use format, but someone's disable it in autoconf)
dnl $3 = caller-supplied default if no --with option is given.  If this is
dnl      blank, the macro uses AC_PATH_PROG.
AC_DEFUN([CF_WITH_PROGRAM],
[dnl
changequote(<<,>>)dnl
define(<<cf_path_name>>, PATH_<<>>translit($1, [a-z], [A-Z]))dnl
define(<<cf_have_name>>, HAVE_<<>>translit($1, [a-z], [A-Z]))dnl
changequote([,])dnl
AC_ARG_WITH($1,[$2],ifelse($3,,
[case "$withval" in #(vi
  yes[)]
   echo 'configure: error: "--with-$1" requires value' 1>&2
   exit 1
   ;; #(vi
  no[)]
   ;; #(vi
  *[)]
   # user supplied option-value for "--with-$1=path"
   AC_MSG_CHECKING(for $1)
   ac_cv_path_]cf_path_name[="$withval"
   AC_DEFINE(cf_have_name)dnl
   AC_MSG_RESULT($withval)
   ;;
 esac],[$3]),[
  # user did not specify "--with-$1"; do automatic check
  AC_PATH_PROG(cf_path_name,$1)
  if test -n "$cf_path_name"; then
    AC_DEFINE_UNQUOTED(cf_path_name,"$cf_path_name")dnl
    AC_DEFINE(cf_have_name)dnl
  fi
])dnl
undefine([cf_path_name])undefine([cf_have_name])])dnl
dnl ---------------------------------------------------------------------------
dnl Wrapper for AC_ARG_WITH to ensure that if the user supplies a value, it is
dnl not simply defaulting to yes/no.  Empty strings are ok if the macro is
dnl invoked without a default value
AC_DEFUN([CF_WITH_VALUE],
[AC_ARG_WITH($1,[$2 ](default: ifelse($4,,empty,$4)),,
ifelse($4,,[withval="${$3}"],[withval="${$3-$4}"]))dnl
ifelse($4,,[test -n "$withval" && \
],[test -z "$withval" && withval=no
])dnl
case "$withval" in #(vi
yes) echo 'configure: error: expected a value for $3' 1>&2
  exit 1
  ;; #(vi
no) withval=""
  ;;
esac
$3="$withval"
AC_DEFINE_UNQUOTED($3,"$withval")dnl
])dnl
