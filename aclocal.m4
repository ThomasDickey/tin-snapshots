dnl Project   : tin - a Usenet reader
dnl Module    : aclocal.m4
dnl Author    : Thomas E. Dickey <dickey@clark.net>
dnl Created   : 1995-08-24
dnl Updated   : 1999-09-08
dnl Notes     :
dnl
dnl Copyright 1996,1997,1998,1999 by Thomas Dickey
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
dnl redundant.  We don't normally need to add -I/usr/local/include for gcc,
dnl but old versions (and some misinstalled ones) need that.
AC_DEFUN([CF_ADD_INCDIR],
[
for cf_add_incdir in $1
do
	while true
	do
		case $cf_add_incdir in
		/usr/include) # (vi
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
for cf_arg in "-DCC_HAS_PROTOS" \
	"" \
	-qlanglvl=ansi \
	-std1 \
	-Ae \
	"-Aa -D_HPUX_SOURCE" \
	-Xc
do
	CFLAGS="$cf_save_CFLAGS $cf_arg"
	AC_TRY_COMPILE(
[
#ifndef CC_HAS_PROTOS
#if !defined(__STDC__) || (__STDC__ != 1)
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
[CF_ARG_OPTION($1,[$2],[$3],[$4],yes)])dnl
dnl ---------------------------------------------------------------------------
dnl Allow user to enable a normally-off option.
AC_DEFUN([CF_ARG_ENABLE],
[CF_ARG_OPTION($1,[$2],[$3],[$4],no)])dnl
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
dnl Restricted form of AC_ARG_WITH that requires user to specify a value
dnl $1 = option name
dnl $2 = help message
dnl $3 = variable to set with the --with value
dnl $4 = default value, if any
AC_DEFUN([CF_ARG_WITH],
[AC_ARG_WITH($1,[$2 ](default: ifelse($4,,empty,$4)),,
ifelse($4,,[withval="${$3}"],[withval="${$3-$4}"]))dnl
ifelse($4,,[test -n "$withval" && \
],[test -z "$withval" && withval=no
])dnl
case "$withval" in #(vi
yes)
  AC_ERROR(expected a value for --with-$1)
  ;; #(vi
no) withval=""
  ;;
esac
$3="$withval"
])dnl
dnl ---------------------------------------------------------------------------
dnl Check for missing declarations in the system headers (adapted from vile).
dnl
dnl CHECK_DECL_FLAG and CHECK_DECL_HDRS must be set in configure.in
AC_DEFUN([CF_CHECK_1_DECL],
[
AC_MSG_CHECKING([for missing "$1" extern])
AC_CACHE_VAL([cf_cv_func_$1],[
CF_MSG_LOG([for missing "$1" external])
cf_save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS $CHECK_DECL_FLAG"
AC_TRY_LINK([
$CHECK_DECL_HDRS

#undef $1
struct zowie { int a; double b; struct zowie *c; char d; };
extern struct zowie *$1();
],
[
],
[if test -n "$CHECK_DECL_HDRS" ; then
# try to workaround system headers which are infested with non-standard syntax
CF_UPPER(cf_1_up,$1)
AC_TRY_COMPILE([
#define DECL_${cf_1_up}
$CHECK_DECL_HDRS
],[long x = 0],
[eval 'cf_cv_func_'$1'=yes'],
[eval 'cf_cv_func_'$1'=no'])
else
eval 'cf_cv_func_'$1'=yes'
fi
],
[eval 'cf_cv_func_'$1'=no'])
CFLAGS="$cf_save_CFLAGS"
])
eval 'cf_result=$cf_cv_func_'$1
AC_MSG_RESULT($cf_result)
test $cf_result = yes && AC_DEFINE_UNQUOTED(DECL_$2)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check if we're accidentally using a cache from a different machine.
dnl Derive the system name, as a check for reusing the autoconf cache.
dnl
dnl If we've packaged config.guess and config.sub, run that (since it does a
dnl better job than uname).
AC_DEFUN([CF_CHECK_CACHE],
[
if test -f $srcdir/config.guess ; then
	AC_CANONICAL_HOST
	system_name="$host_os"
else
	system_name="`(uname -s -r) 2>/dev/null`"
	if test -z "$system_name" ; then
		system_name="`(hostname) 2>/dev/null`"
	fi
fi
test -n "$system_name" && AC_DEFINE_UNQUOTED(SYSTEM_NAME,"$system_name")
AC_CACHE_VAL(cf_cv_system_name,[cf_cv_system_name="$system_name"])

test -z "$system_name" && system_name="$cf_cv_system_name"
test -n "$cf_cv_system_name" && AC_MSG_RESULT("Configuring for $cf_cv_system_name")

if test ".$system_name" != ".$cf_cv_system_name" ; then
	AC_MSG_RESULT(Cached system name ($system_name) does not agree with actual ($cf_cv_system_name))
	AC_ERROR("Please remove config.cache and try again.")
fi
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
dnl Check for data that is usually declared in <stdio.h> or <errno.h>, e.g.,
dnl the 'errno' variable.  Define a DECL_xxx symbol if we must declare it
dnl ourselves.
dnl
dnl (I would use AC_CACHE_CHECK here, but it will not work when called in a
dnl loop from CF_SYS_ERRLIST).
dnl
dnl $1 = the name to check
AC_DEFUN([CF_CHECK_ERRNO],
[
AC_MSG_CHECKING(if external $1 is declared)
AC_CACHE_VAL(cf_cv_dcl_$1,[
    AC_TRY_COMPILE([
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <stdio.h>
#include <sys/types.h>
#include <errno.h> ],
    [long x = (long) $1],
    [eval 'cf_cv_dcl_'$1'=yes'],
    [eval 'cf_cv_dcl_'$1'=no]')
])

eval 'cf_result=$cf_cv_dcl_'$1
AC_MSG_RESULT($cf_result)

if test "$cf_result" = no ; then
    eval 'cf_result=DECL_'$1
    CF_UPPER(cf_result,$cf_result)
    AC_DEFINE_UNQUOTED($cf_result)
fi

# It's possible (for near-UNIX clones) that the data doesn't exist
CF_CHECK_EXTERN_DATA($1,int)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check for existence of external data in the current set of libraries.  If
dnl we can modify it, it's real enough.
dnl $1 = the name to check
dnl $2 = its type
AC_DEFUN([CF_CHECK_EXTERN_DATA],
[
AC_MSG_CHECKING(if external $1 exists)
AC_CACHE_VAL(cf_cv_have_$1,[
    AC_TRY_LINK([
#undef $1
extern $2 $1;
],
    [$1 = 2],
    [eval 'cf_cv_have_'$1'=yes'],
    [eval 'cf_cv_have_'$1'=no'])])

eval 'cf_result=$cf_cv_have_'$1
AC_MSG_RESULT($cf_result)

if test "$cf_result" = yes ; then
    eval 'cf_result=HAVE_'$1
    CF_UPPER(cf_result,$cf_result)
    AC_DEFINE_UNQUOTED($cf_result)
fi

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
dnl Check if curses supports color.  (Note that while SVr3 curses supports
dnl color, it does this differently from SVr4 curses; more work would be needed
dnl to accommodate SVr3).
dnl
AC_DEFUN([CF_COLOR_CURSES],
[
AC_MSG_CHECKING(if curses supports color attributes)
AC_CACHE_VAL(cf_cv_color_curses,[
	AC_TRY_LINK([
#include <${cf_cv_ncurses_header-curses.h}>
],
	[chtype x = COLOR_BLUE;
	 has_colors();
	 start_color();
#ifndef NCURSES_BROKEN
	 wbkgd(curscr, getbkgd(stdscr)); /* X/Open XPG4 aka SVr4 Curses */
#endif
	],
	[cf_cv_color_curses=yes],
	[cf_cv_color_curses=no])
	])
AC_MSG_RESULT($cf_cv_color_curses)
if test $cf_cv_color_curses = yes ; then
	AC_DEFINE(COLOR_CURSES)
	test ".$cf_cv_ncurses_broken" != .yes && AC_DEFINE(HAVE_GETBKGD)
fi
])
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
dnl Look for the curses libraries.  Older curses implementations may require
dnl termcap/termlib to be linked as well.
AC_DEFUN([CF_CURSES_LIBS],[
AC_CHECK_FUNC(initscr,,[
case $host_os in #(vi
freebsd*) #(vi
	AC_CHECK_LIB(mytinfo,tgoto,[LIBS="-lmytinfo $LIBS"])
	;;
hpux10.*)
	AC_CHECK_LIB(cur_colr,initscr,[
		LIBS="-lcur_colr $LIBS"
		CFLAGS="-I/usr/include/curses_colr $CFLAGS"
		ac_cv_func_initscr=yes
		],[
	AC_CHECK_LIB(Hcurses,initscr,[
		# HP's header uses __HP_CURSES, but user claims _HP_CURSES.
		LIBS="-lHcurses $LIBS"
		CFLAGS="-D__HP_CURSES -D_HP_CURSES $CFLAGS"
		ac_cv_func_initscr=yes
		])])
	;;
linux*) # Suse Linux does not follow /usr/lib convention
	LIBS="$LIBS -L/lib"
	;;
esac

if test ".$With5lib" != ".no" ; then
if test -d /usr/5lib ; then
	# SunOS 3.x or 4.x
	CPPFLAGS="$CPPFLAGS -I/usr/5include"
	LIBS="$LIBS -L/usr/5lib"
fi
fi

if test ".$ac_cv_func_initscr" != .yes ; then
	cf_save_LIBS="$LIBS"
	cf_term_lib=""
	cf_curs_lib=""

	# Check for library containing tgoto.  Do this before curses library
	# because it may be needed to link the test-case for initscr.
	AC_CHECK_FUNC(tgoto,[cf_term_lib=predefined],[
		for cf_term_lib in termcap termlib unknown
		do
			AC_CHECK_LIB($cf_term_lib,tgoto,[break])
		done
	])

	# Check for library containing initscr
	test "$cf_term_lib" != predefined && test "$cf_term_lib" != unknown && LIBS="-l$cf_term_lib $cf_save_LIBS"
	for cf_curs_lib in cursesX curses ncurses xcurses jcurses unknown
	do
		AC_CHECK_LIB($cf_curs_lib,initscr,[break])
	done
	test $cf_curs_lib = unknown && AC_ERROR(no curses library found)

	LIBS="-l$cf_curs_lib $cf_save_LIBS"
	if test "$cf_term_lib" = unknown ; then
		AC_MSG_CHECKING(if we can link with $cf_curs_lib library)
		AC_TRY_LINK([#include <${cf_cv_ncurses_header-curses.h}>],
			[initscr()],
			[cf_result=yes],
			[cf_result=no])
		AC_MSG_RESULT($cf_result)
		test $cf_result = no && AC_ERROR(Cannot link curses library)
	elif test "$cf_term_lib" != predefined ; then
		AC_MSG_CHECKING(if we need both $cf_curs_lib and $cf_term_lib libraries)
		AC_TRY_LINK([#include <${cf_cv_ncurses_header-curses.h}>],
			[initscr(); tgoto((char *)0, 0, 0);],
			[cf_result=no],
			[
			LIBS="-l$cf_curs_lib -l$cf_term_lib $cf_save_LIBS"
			AC_TRY_LINK([#include <${cf_cv_ncurses_header-curses.h}>],
				[initscr()],
				[cf_result=yes],
				[cf_result=error])
			])
		AC_MSG_RESULT($cf_result)
	fi
fi

])])
dnl ---------------------------------------------------------------------------
dnl Check if we should include <curses.h> to pick up prototypes for termcap
dnl functions.  On terminfo systems, these are normally declared in <curses.h>,
dnl but may be in <term.h>.  We check for termcap.h as an alternate, but it
dnl isn't standard (usually associated with GNU termcap).
dnl
dnl The 'tgoto()' function is declared in both terminfo and termcap.
dnl
dnl See CF_TYPE_OUTCHAR for more details.
AC_DEFUN([CF_CURSES_TERMCAP],
[
AC_REQUIRE([CF_CURSES_TERM_H])
AC_MSG_CHECKING(if we should include curses.h or termcap.h)
AC_CACHE_VAL(cf_cv_need_curses_h,[
cf_save_CFLAGS="$CFLAGS"
cf_cv_need_curses_h=no

for cf_t_opts in "" "NEED_TERMCAP_H"
do
for cf_c_opts in "" "NEED_CURSES_H"
do

    CFLAGS="$cf_save_CFLAGS $CHECK_DECL_FLAG"
    test -n "$cf_c_opts" && CFLAGS="$CFLAGS -D$cf_c_opts"
    test -n "$cf_t_opts" && CFLAGS="$CFLAGS -D$cf_t_opts"

    AC_TRY_LINK([/* $cf_c_opts $cf_t_opts */
$CHECK_DECL_HDRS],
	[char *x = (char *)tgoto("")],
	[test "$cf_cv_need_curses_h" = no && {
	     cf_cv_need_curses_h=maybe
	     cf_ok_c_opts=$cf_c_opts
	     cf_ok_t_opts=$cf_t_opts
	}],
	[echo "Recompiling with corrected call (C:$cf_c_opts, T:$cf_t_opts)" >&AC_FD_CC
	AC_TRY_LINK([
$CHECK_DECL_HDRS],
	[char *x = (char *)tgoto("",0,0)],
	[cf_cv_need_curses_h=yes
	 cf_ok_c_opts=$cf_c_opts
	 cf_ok_t_opts=$cf_t_opts])])

	CFLAGS="$cf_save_CFLAGS"
	test "$cf_cv_need_curses_h" = yes && break
done
	test "$cf_cv_need_curses_h" = yes && break
done

if test "$cf_cv_need_curses_h" != no ; then
	echo "Curses/termcap test = $cf_cv_need_curses_h (C:$cf_ok_c_opts, T:$cf_ok_t_opts)" >&AC_FD_CC
	if test -n "$cf_ok_c_opts" ; then
		if test -n "$cf_ok_t_opts" ; then
			cf_cv_need_curses_h=both
		else
			cf_cv_need_curses_h=curses.h
		fi
	elif test -n "$cf_ok_t_opts" ; then
		cf_cv_need_curses_h=termcap.h
	elif test "$cf_cv_have_term_h" = yes ; then
		cf_cv_need_curses_h=term.h
	else
		cf_cv_need_curses_h=no
	fi
fi
])
AC_MSG_RESULT($cf_cv_need_curses_h)

case $cf_cv_need_curses_h in
both) #(vi
	AC_DEFINE_UNQUOTED(NEED_CURSES_H)
	AC_DEFINE_UNQUOTED(NEED_TERMCAP_H)
	;;
curses.h) #(vi
	AC_DEFINE_UNQUOTED(NEED_CURSES_H)
	;;
termcap.h) #(vi
	AC_DEFINE_UNQUOTED(NEED_TERMCAP_H)
	;;
esac

])dnl
dnl ---------------------------------------------------------------------------
dnl SVr4 curses should have term.h as well (where it puts the definitions of
dnl the low-level interface).  This may not be true in old/broken implementations,
dnl as well as in misconfigured systems (e.g., gcc configured for Solaris 2.4
dnl running with Solaris 2.5.1).
AC_DEFUN([CF_CURSES_TERM_H],
[
AC_MSG_CHECKING([for term.h])
AC_CACHE_VAL(cf_cv_have_term_h,[
	AC_TRY_COMPILE([
#include <curses.h>
#include <term.h>],
	[WINDOW *x],
	[cf_cv_have_term_h=yes],
	[cf_cv_have_term_h=no])
	])
AC_MSG_RESULT($cf_cv_have_term_h)
test $cf_cv_have_term_h = yes && AC_DEFINE(HAVE_TERM_H)
])dnl
dnl ---------------------------------------------------------------------------
dnl You can always use "make -n" to see the actual options, but it's hard to
dnl pick out/analyze warning messages when the compile-line is long.
dnl
dnl Sets:
dnl	ECHO_LD - symbol to prefix "cc -o" lines
dnl	RULE_CC - symbol to put before implicit "cc -c" lines (e.g., .c.o)
dnl	SHOW_CC - symbol to put before explicit "cc -c" lines
dnl	ECHO_CC - symbol to put before any "cc" line
dnl
AC_DEFUN([CF_DISABLE_ECHO],[
AC_MSG_CHECKING(if you want to see long compiling messages)
CF_ARG_DISABLE(echo,
	[  --disable-echo          display "compiling" commands],
	[
    ECHO_LD='@echo linking [$]@;'
    RULE_CC='	@echo compiling [$]<'
    SHOW_CC='	@echo compiling [$]@'
    ECHO_CC='@'
],[
    ECHO_LD=''
    RULE_CC='# compiling'
    SHOW_CC='# compiling'
    ECHO_CC=''
])
AC_MSG_RESULT($enableval)
AC_SUBST(ECHO_LD)
AC_SUBST(RULE_CC)
AC_SUBST(SHOW_CC)
AC_SUBST(ECHO_CC)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check if 'errno' is declared in <errno.h>
AC_DEFUN([CF_ERRNO],
[
CF_CHECK_ERRNO(errno)
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
case $host_os in #(vi
linux*) # Suse Linux does not follow /usr/lib convention
	LIBS="$LIBS -L/lib"
	;;
esac
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
dnl Check for memmove, or a bcopy that can handle overlapping copy.  If neither
dnl is found, add our own version of memmove to the list of objects.
AC_DEFUN([CF_FUNC_MEMMOVE],
[
AC_CHECK_FUNC(memmove,,[
AC_CHECK_FUNC(bcopy,[
	AC_CACHE_CHECK(if bcopy does overlapping moves,cf_cv_good_bcopy,[
		AC_TRY_RUN([
int main() {
	static char data[] = "abcdefghijklmnopqrstuwwxyz";
	char temp[40];
	bcopy(data, temp, sizeof(data));
	bcopy(temp+10, temp, 15);
	bcopy(temp+5, temp+15, 10);
	exit (strcmp(temp, "klmnopqrstuwwxypqrstuwwxyz"));
}
		],
		[cf_cv_good_bcopy=yes],
		[cf_cv_good_bcopy=no],
		[cf_cv_good_bcopy=unknown])
		])
	],[cf_cv_good_bcopy=no])
	if test $cf_cv_good_bcopy = yes ; then
		AC_DEFINE(USE_OK_BCOPY)
	else
		AC_DEFINE(USE_MY_MEMMOVE)
	fi
])])dnl
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
int main(int argc, char *argv[]) { return (argv[argc-1] == 0) ; }
EOF
	changequote([,])dnl
	AC_CHECKING([for $CC warning options])
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
test "$prefix" != /opt           && $1="[$]$1 /opt/include /opt/include/$2"
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
test "$prefix" != /opt           && $1="[$]$1 /opt/lib /opt/lib/$2"
])dnl
dnl ---------------------------------------------------------------------------
dnl Compute the library-prefix for the given host system
dnl $1 = variable to set
AC_DEFUN([CF_LIB_PREFIX],
[
	case $cf_cv_system_name in
	os2)	$1=''     ;;
	*)	$1='lib'  ;;
	esac
	LIB_PREFIX=[$]$1
	AC_SUBST(LIB_PREFIX)
])dnl
dnl ---------------------------------------------------------------------------
dnl Some 'make' programs support $(MAKEFLAGS), some $(MFLAGS), to pass 'make'
dnl options to lower-levels.  It's very useful for "make -n" -- if we have it.
dnl (GNU 'make' does both, something POSIX 'make', which happens to make the
dnl $(MAKEFLAGS) variable incompatible because it adds the assignments :-)
AC_DEFUN([CF_MAKEFLAGS],
[
AC_MSG_CHECKING([for makeflags variable])
AC_CACHE_VAL(cf_cv_makeflags,[
	cf_cv_makeflags=''
	for cf_option in '-$(MAKEFLAGS)' '$(MFLAGS)'
	do
		cat >cf_makeflags.tmp <<CF_EOF
all :
	@ echo '.$cf_option'
CF_EOF
		cf_result=`${MAKE-make} -k -f cf_makeflags.tmp 2>/dev/null`
		case "$cf_result" in
		.*k)
			cf_result=`${MAKE-make} -k -f cf_makeflags.tmp CC=cc 2>/dev/null`
			case "$cf_result" in
			.*CC=*)	cf_cv_makeflags=
				;;
			*)	cf_cv_makeflags=$cf_option
				;;
			esac
			break
			;;
		*)	echo no match "$cf_result"
			;;
		esac
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
dnl Check for pre-1.9.9g ncurses (among other problems, the most obvious is
dnl that color combinations don't work).
AC_DEFUN([CF_NCURSES_BROKEN],
[
AC_REQUIRE([CF_NCURSES_VERSION])
if test "$cf_cv_ncurses_version" != no ; then
AC_MSG_CHECKING(for obsolete/broken version of ncurses)
AC_CACHE_VAL(cf_cv_ncurses_broken,[
AC_TRY_COMPILE([
#include <${cf_cv_ncurses_header-curses.h}>],[
#if defined(NCURSES_VERSION) && defined(wgetbkgd)
	make an error
#else
	int x = 1
#endif
],
	[cf_cv_ncurses_broken=no],
	[cf_cv_ncurses_broken=yes])
])
AC_MSG_RESULT($cf_cv_ncurses_broken)
if test "$cf_cv_ncurses_broken" = yes ; then
	AC_MSG_WARN(hmm... you should get an up-to-date version of ncurses)
	AC_DEFINE(NCURSES_BROKEN)
fi
fi
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

	# This works, except for the special case where we find gpm, but
	# ncurses is in a nonstandard location via $LIBS, and we really want
	# to link gpm.
cf_ncurses_LIBS=""
cf_ncurses_SAVE="$LIBS"
AC_CHECK_LIB(gpm,Gpm_Open,
	[AC_CHECK_LIB(gpm,initscr,
		[LIBS="$cf_ncurses_SAVE"],
		[cf_ncurses_LIBS="-lgpm"])])

case $host_os in #(vi
freebsd*)
	# This is only necessary if you are linking against an obsolete
	# version of ncurses (but it should do no harm, since it's static).
	AC_CHECK_LIB(mytinfo,tgoto,[cf_ncurses_LIBS="-lmytinfo $cf_ncurses_LIBS"])
	;;
esac

LIBS="$cf_ncurses_LIBS $LIBS"
CF_FIND_LIBRARY(ncurses,
	[#include <${cf_cv_ncurses_header-curses.h}>],
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
	AC_TRY_LINK([#include <${cf_cv_ncurses_header-curses.h}>],
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
#include <${cf_cv_ncurses_header-curses.h}>
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
#include <${cf_cv_ncurses_header-curses.h}>
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
dnl Look for the default editor (vi)
AC_DEFUN([CF_PATH_EDITOR],
[
AC_MSG_CHECKING(for default editor)
CF_ARG_WITH(editor,
    [  --with-editor=PROG      specify editor (default: vi)],
    [DEFAULT_EDITOR])
if test -z "$DEFAULT_EDITOR" ; then
    if test -n "$EDITOR" ; then
    	DEFAULT_EDITOR="$EDITOR"
    elif test -n "$VISUAL" ; then
    	DEFAULT_EDITOR="$VISUAL"
    else
	AC_PATH_PROG(DEFAULT_EDITOR,vi,vi,$PATH:/usr/bin:/usr/ucb)
    fi
fi
AC_MSG_RESULT($DEFAULT_EDITOR)
AC_DEFINE_UNQUOTED(DEFAULT_EDITOR,"$DEFAULT_EDITOR")
])dnl
dnl ---------------------------------------------------------------------------
dnl Look for the directory that contains incoming mail.  I would check for an
dnl actual mail-file, to verify this, but that is not always easy to arrange.
AC_DEFUN([CF_PATH_MAILBOX],
[
AC_MSG_CHECKING(for incoming-mail directory)
CF_ARG_WITH(mailbox,
    [  --with-mailbox=DIR      directory for incoming mailboxes],
    [DEFAULT_MAILBOX])
if test -z "$DEFAULT_MAILBOX" ; then
for cf_dir in \
	/var/spool/mail \
	/usr/spool/mail \
	/var/mail \
	/usr/mail \
	/mail
    do
    	if test -d $cf_dir ; then
	    DEFAULT_MAILBOX=$cf_dir
	    break
	fi
    done
fi
if test -n "$DEFAULT_MAILBOX" ; then
	AC_DEFINE_UNQUOTED(DEFAULT_MAILBOX,"$DEFAULT_MAILBOX")
else
	DEFAULT_MAILBOX=none
fi
AC_MSG_RESULT($DEFAULT_MAILBOX)
])dnl
dnl ---------------------------------------------------------------------------
dnl Look for the program that sends outgoing mail.
AC_DEFUN([CF_PATH_MAILER],
[
AC_PATH_PROG(DEFAULT_MAILER,sendmail,,$PATH:/usr/sbin:/usr/lib)
CF_ARG_WITH(mailer,
     [  --with-mailer=PROG      specify default mailer-program],
     [DEFAULT_MAILER])
if test -z "$DEFAULT_MAILER" ; then
AC_PATH_PROG(DEFAULT_MAILER,mailx,,$PATH:/usr/bin)
fi
if test -z "$DEFAULT_MAILER" ; then
AC_PATH_PROG(DEFAULT_MAILER,mail,,$PATH:/usr/bin)
fi
AC_MSG_CHECKING(for default mailer)
if test -n "$DEFAULT_MAILER" ; then
	AC_DEFINE_UNQUOTED(DEFAULT_MAILER,"$DEFAULT_MAILER")
else
	DEFAULT_MAILER=none
fi
AC_MSG_RESULT($DEFAULT_MAILER)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check the argument to see that it looks like a pathname.  Rewrite it if it
dnl begins with one of the prefix/exec_prefix variables, and then again if the
dnl result begins with 'NONE'.  This is necessary to workaround autoconf's
dnl delayed evaluation of those symbols.
AC_DEFUN([CF_PATH_SYNTAX],[
case ".[$]$1" in #(vi
./*) #(vi
  ;;
.\[$]{*prefix}*) #(vi
  eval $1="[$]$1"
  case ".[$]$1" in #(vi
  .NONE/*)
    $1=`echo [$]$1 | sed -e s@NONE@$ac_default_prefix@`
    ;;
  esac
  ;; #(vi
.NONE/*)
  $1=`echo [$]$1 | sed -e s@NONE@$ac_default_prefix@`
  ;;
*)
  AC_ERROR(expected a pathname)
  ;;
esac
])dnl
dnl ---------------------------------------------------------------------------
dnl Check if we have POSIX-style job control (i.e., sigaction), or if we must
dnl use the signal functions.  Use AC_CHECK_FUNCS(sigaction) first.
AC_DEFUN([CF_POSIX_JC],[

AC_REQUIRE([AC_TYPE_SIGNAL])
AC_REQUIRE([CF_SIG_ARGS])

if test "$ac_cv_func_sigaction" = yes; then

AC_CACHE_CHECK(whether sigaction needs _POSIX_SOURCE,cf_cv_sigact_bad,[
AC_TRY_COMPILE([
#include <sys/types.h>
#include <signal.h>],[struct sigaction act],
  [cf_cv_sigact_bad=no],[cf_cv_sigact_bad=yes AC_DEFINE(SVR4_ACTION)])
])

test "$cf_cv_sigact_bad" = yes && AC_DEFINE(SVR4_ACTION)

AC_CACHE_CHECK(if we have sigaction/related functions,cf_cv_sigaction_funcs,[
AC_TRY_LINK([
#ifdef SVR4_ACTION
#define _POSIX_SOURCE
#endif
#include <sys/types.h>
#include <signal.h>],[
    RETSIGTYPE (*func)(SIG_ARGS) = SIG_IGN;
    struct sigaction sa, osa;
    sa.sa_handler = func;
    sa.sa_flags = 0;
    sigemptyset (&sa.sa_mask);
    sigaction (SIGBUS,&sa,&osa);],
    [cf_cv_sigaction_funcs=yes],
    [cf_cv_sigaction_funcs=no])])

test "$cf_cv_sigaction_funcs" = yes && AC_DEFINE(HAVE_POSIX_JC)

fi
])dnl
dnl ---------------------------------------------------------------------------
dnl Compute $PROG_EXT, used for non-Unix ports, such as OS/2 EMX.
AC_DEFUN([CF_PROG_EXT],
[
AC_REQUIRE([CF_CHECK_CACHE])
PROG_EXT=
case $cf_cv_system_name in
os2*)
    # We make sure -Zexe is not used -- it would interfere with @PROG_EXT@
    CFLAGS="$CFLAGS -Zmt -D__ST_MT_ERRNO__"
    CXXFLAGS="$CXXFLAGS -Zmt -D__ST_MT_ERRNO__"
    LDFLAGS=`echo "$LDFLAGS -Zmt -Zcrtdll" | sed "s/-Zexe//g"`
    PROG_EXT=".exe"
    ;;
cygwin*)
    PROG_EXT=".exe"
    ;;
esac
AC_SUBST(PROG_EXT)
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
dnl A better version of AC_PROC_YACC, verifies that we'll only choose bison if
dnl we'll be able to compile with it.  Bison uses alloca, which isn't all that
dnl portable.
AC_DEFUN([CF_PROG_YACC],
[
AC_REQUIRE([AC_PROG_CC])
AC_CACHE_VAL(cf_cv_prog_YACC,[
if test -n "$YACC" ; then
  cf_cv_prog_YACC="$YACC" # Let the user override the test.
else
cat >conftest.y <<EOF
%{
void yyerror(s) char *s; { }
%}
%token	NUMBER
%%
time	: NUMBER ':' NUMBER
	;
%%
int yylex() { return NUMBER; }
int main() { return yyparse(); }
EOF
  for cf_prog in 'bison -y' byacc yacc
  do
    rm -f y.tab.[ch]
    AC_MSG_CHECKING(for $cf_prog)
    cf_command="$cf_prog conftest.y"
    cf_result=no
    if AC_TRY_EVAL(cf_command) && test -s y.tab.c ; then
      mv y.tab.c conftest.c
      rm -f y.tab.h
      if test "$cf_prog" = 'bison -y' ; then
        if AC_TRY_EVAL(ac_link) && test -s conftest ; then
          cf_result=yes
        fi
      else
        cf_result=yes
      fi
    fi
    AC_MSG_RESULT($cf_result)
    if test $cf_result = yes ; then
      cf_cv_prog_YACC="$cf_prog"
      break
    fi
  done
fi
])
YACC="$cf_cv_prog_YACC"
AC_SUBST(YACC)dnl
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
dnl Check for definitions & structures needed for window size-changing
dnl FIXME: check that this works with "snake" (HP-UX 10.x)
AC_DEFUN([CF_SIZECHANGE],
[
AC_MSG_CHECKING([declaration of size-change])
AC_CACHE_VAL(cf_cv_sizechange,[
    cf_cv_sizechange=unknown
    cf_save_CFLAGS="$CFLAGS"

for cf_opts in "" "NEED_PTEM_H"
do

    CFLAGS="$cf_save_CFLAGS"
    test -n "$cf_opts" && CFLAGS="$CFLAGS -D$cf_opts"
    AC_TRY_COMPILE([#include <sys/types.h>
#if HAVE_TERMIOS_H
#include <termios.h>
#else
#if HAVE_TERMIO_H
#include <termio.h>
#endif
#endif
#if NEED_PTEM_H
/* This is a workaround for SCO:  they neglected to define struct winsize in
 * termios.h -- it's only in termio.h and ptem.h
 */
#include        <sys/stream.h>
#include        <sys/ptem.h>
#endif
#if !defined(sun) || !defined(HAVE_TERMIOS_H)
#include <sys/ioctl.h>
#endif
],[
#ifdef TIOCGSIZE
	struct ttysize win;	/* FIXME: what system is this? */
	int y = win.ts_lines;
	int x = win.ts_cols;
#else
#ifdef TIOCGWINSZ
	struct winsize win;
	int y = win.ws_row;
	int x = win.ws_col;
#else
	no TIOCGSIZE or TIOCGWINSZ
#endif /* TIOCGWINSZ */
#endif /* TIOCGSIZE */
	],
	[cf_cv_sizechange=yes],
	[cf_cv_sizechange=no])

	CFLAGS="$cf_save_CFLAGS"
	if test "$cf_cv_sizechange" = yes ; then
		echo "size-change succeeded ($cf_opts)" >&AC_FD_CC
		test -n "$cf_opts" && AC_DEFINE_UNQUOTED($cf_opts)
		break
	fi
done
	])
AC_MSG_RESULT($cf_cv_sizechange)
test $cf_cv_sizechange != no && AC_DEFINE(HAVE_SIZECHANGE)
])dnl
dnl ---------------------------------------------------------------------------
dnl Check for socks library
dnl $1 = the [optional] directory in which the library may be found
dnl $2 = the [optional] name of the library
AC_DEFUN([CF_SOCKS],[
case "$1" in #(vi
no|yes) #(vi
  ;;
*)
  LIBS="$LIBS -L$1"
  ;;
esac
LIBS="$LIBS -lsocks"
AC_DEFINE(SOCKS)
AC_DEFINE(accept,Raccept)
AC_DEFINE(bind,Rbind)
AC_DEFINE(connect,Rconnect)
AC_DEFINE(getpeername,Rgetpeername)
AC_DEFINE(getsockname,Rgetsockname)
AC_DEFINE(listen,Rlisten)
AC_DEFINE(recvfrom,Rrecvfrom)
AC_DEFINE(select,Rselect)
AC_TRY_LINK([
#include <stdio.h>],[
	accept((char *)0)],,
	[AC_ERROR(Cannot link with socks library)])
])dnl
dnl ---------------------------------------------------------------------------
dnl Check for socks5 configuration
dnl $1 = the [optional] directory in which the library may be found
AC_DEFUN([CF_SOCKS5],[
case "$1" in #(vi
no|yes) #(vi
  ;;
*)
  LIBS="$LIBS -L$1"
  CFLAGS="$CFLAGS -I$1/../include"
  ;;
esac
LIBS="$LIBS -lsocks5"
AC_DEFINE(USE_SOCKS5)
AC_DEFINE(SOCKS)
AC_MSG_CHECKING(if the socks library uses socks4 prefix)
AC_TRY_LINK([
#include <socks.h>],[
	Rinit((char *)0)],
	[AC_DEFINE(USE_SOCKS4_PREFIX)
	 cf_use_socks4=yes],
	[AC_TRY_LINK([#include <socks.h>],
		[SOCKSinit((char *)0)],
		[cf_use_socks4=no],
		[AC_ERROR(Cannot link with socks5 library)])])
AC_MSG_RESULT($cf_use_socks4)
if test "$cf_use_socks4" = "yes" ; then
	AC_DEFINE(accept,Raccept)
	AC_DEFINE(bind,Rbind)
	AC_DEFINE(connect,Rconnect)
	AC_DEFINE(getpeername,Rgetpeername)
	AC_DEFINE(getsockname,Rgetsockname)
	AC_DEFINE(listen,Rlisten)
	AC_DEFINE(recvfrom,Rrecvfrom)
	AC_DEFINE(select,Rselect)
else
	AC_DEFINE(accept,SOCKSaccept)
	AC_DEFINE(getpeername,SOCKSgetpeername)
	AC_DEFINE(getsockname,SOCKSgetsockname)
	AC_DEFINE(recvfrom,SOCKSrecvfrom)
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl Check for declaration of sys_nerr and sys_errlist in one of stdio.h and
dnl errno.h.  Declaration of sys_errlist on BSD4.4 interferes with our
dnl declaration.  Reported by Keith Bostic.
AC_DEFUN([CF_SYS_ERRLIST],
[
for cf_name in sys_nerr sys_errlist
do
    CF_CHECK_ERRNO($cf_name)
done
])dnl
dnl ---------------------------------------------------------------------------
dnl Check if there is a conflict between <sys/select.h> and <sys/time.h>.
dnl This is known to be a problem with SCO.
AC_DEFUN([CF_SYS_SELECT_TIMEVAL],
[
AC_MSG_CHECKING(if sys/time.h conflicts with sys/select.h)
AC_CACHE_VAL(cf_cv_sys_select_timeval,[
for cf_opts in no yes
do
AC_TRY_COMPILE([
#define yes 1
#define no 0
#if $cf_opts
#define timeval fake_timeval
#endif
#include <sys/types.h>
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
#undef timeval
#ifdef HAVE_SYS_SELECT_H
#	include <sys/select.h>
#endif
],[struct timeval foo],
	[cf_cv_sys_select_timeval=$cf_opts
	 break],
	[cf_cv_sys_select_timeval=no])
done
])
AC_MSG_RESULT($cf_cv_sys_select_timeval)
test $cf_cv_sys_select_timeval = yes && AC_DEFINE(NEED_TIMEVAL_FIX)
])
dnl ---------------------------------------------------------------------------
dnl Look for termcap libraries, or the equivalent in terminfo.
AC_DEFUN([CF_TERMCAP_LIBS],
[
AC_CACHE_VAL(cf_cv_termlib,[
cf_cv_termlib=none
AC_TRY_LINK([],[char *x=(char*)tgoto("",0,0)],
[AC_TRY_LINK([],[int x=tigetstr("")],
	[cf_cv_termlib=terminfo],
	[cf_cv_termlib=termcap])
	CF_VERBOSE(using functions in predefined $cf_cv_termlib LIBS)
],[
ifelse([$1],,,[
if test "$1" = ncurses; then
	CF_NCURSES_CPPFLAGS
	CF_NCURSES_LIBS
	cf_cv_termlib=terminfo
fi
])
if test "$cf_cv_termlib" = none; then
	# FreeBSD's linker gives bogus results for AC_CHECK_LIB, saying that
	# tgetstr lives in -lcurses when it is only an unsatisfied extern.
	cf_save_LIBS="$LIBS"
	for cf_lib in curses ncurses termlib termcap
	do
	LIBS="-l$cf_lib $cf_save_LIBS"
	for cf_func in tigetstr tgetstr
	do
		AC_MSG_CHECKING(for $cf_func in -l$cf_lib)
		AC_TRY_LINK([],[int x=$cf_func("")],[cf_result=yes],[cf_result=no])
		AC_MSG_RESULT($cf_result)
		if test "$cf_result" = yes ; then
			if test "$cf_func" = tigetstr ; then
				cf_cv_termlib=terminfo
			else
				cf_cv_termlib=termcap
			fi
			break
		fi
	done
		test "$cf_result" = yes && break
	done
	test "$cf_result" = no && LIBS="$cf_save_LIBS"
fi
if test "$cf_cv_termlib" = none; then
	# allow curses library for broken AIX system.
	AC_CHECK_LIB(curses, initscr, [LIBS="$LIBS -lcurses" cf_cv_termlib=termcap])
	AC_CHECK_LIB(termcap, tgoto, [LIBS="$LIBS -ltermcap" cf_cv_termlib=termcap])
fi
])
if test "$cf_cv_termlib" = none; then
	AC_ERROR([Can't find -ltermlib, -lcurses, or -ltermcap])
fi
])])dnl
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
dnl
dnl CHECK_DECL_FLAG and CHECK_DECL_HDRS must be set in configure.in
AC_DEFUN([CF_TIOCGWINSZ],
[
AC_MSG_CHECKING([for working TIOCGWINSZ])
AC_CACHE_VAL(cf_cv_use_tiocgwinsz,[
	cf_save_CFLAGS="$CFLAGS"
	CFLAGS="$CFLAGS -D__CPROTO__ $CHECK_DECL_FLAG"
	AC_TRY_RUN([
$CHECK_DECL_HDRS
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
		CFLAGS="$cf_save_CFLAGS"])
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
dnl
dnl For this check, and for CF_CURSES_TERMCAP, the $CHECK_DECL_HDRS variable
dnl must point to a header file containing this (or equivalent):
dnl
dnl	#ifdef NEED_CURSES_H
dnl	# if HAVE_NCURSES_H
dnl	#  include <ncurses.h>
dnl	# else
dnl	#  include <curses.h>
dnl	# endif
dnl	#endif
dnl	#if HAVE_TERM_H
dnl	# include <term.h>
dnl	#endif
dnl	#if NEED_TERMCAP_H
dnl	# include <termcap.h>
dnl	#endif
dnl
AC_DEFUN([CF_TYPE_OUTCHAR],
[
AC_REQUIRE([CF_CURSES_TERMCAP])

AC_MSG_CHECKING([declaration of tputs 3rd param])
AC_CACHE_VAL(cf_cv_type_outchar,[

cf_cv_type_outchar="int OutChar(int)"
cf_cv_found=no
cf_save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS $CHECK_DECL_FLAG"

for P in int void; do
for Q in int void; do
for R in int char; do
for S in "" const; do

	AC_TRY_COMPILE([$CHECK_DECL_HDRS],
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

CFLAGS="$cf_save_CFLAGS"
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
	AC_TRY_LINK($cf_wait_headers,
	[int x;
	 int y = WEXITSTATUS(x);
	 int z = WTERMSIG(x);
	 wait(&x);
	],
	[cf_cv_type_unionwait=no
	 echo compiles ok w/o union wait 1>&AC_FD_CC
	],[
	AC_TRY_LINK($cf_wait_headers,
	[union wait x;
#ifdef WEXITSTATUS
	 int y = WEXITSTATUS(x);
#endif
#ifdef WTERMSIG
	 int z = WTERMSIG(x);
#endif
	 wait(&x);
	],
	[cf_cv_type_unionwait=yes
	 echo compiles ok with union wait and possibly macros too 1>&AC_FD_CC
	],
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
dnl Use AC_VERBOSE w/o the warnings
AC_DEFUN([CF_VERBOSE],
[test -n "$verbose" && echo "	$1" 1>&AC_FD_MSG
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
dnl $1 = option name
dnl $2 = help-message
dnl $3 = name of variable to inherit/override
dnl $4 = default value of variable, if any
AC_DEFUN([CF_WITH_DFTENV],
[AC_ARG_WITH($1,[$2 ](default: ifelse($4,,empty,$4)),,
ifelse($4,,[withval="${$3}"],[withval="${$3-$4}"]))dnl
case "$withval" in #(vi
yes|no)
  AC_ERROR(expected a value for --with-$1)
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
CF_PATH_SYNTAX(withval)
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
   AC_ERROR(expected a value for --with-$1)
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
dnl $1 = option name
dnl $2 = help-message
dnl $3 = variable to inherit/override
dnl $4 = default value, if any.
AC_DEFUN([CF_WITH_VALUE],
[CF_ARG_WITH($1,[$2],[$3],[$4])
 AC_DEFINE_UNQUOTED($3,"$withval")dnl
])dnl
