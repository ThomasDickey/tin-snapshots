dnl Project   : tin - a Usenet reader
dnl Module    : aclocal.m4
dnl Author    : Thomas E. Dickey <dickey@clark.net>
dnl Created   : 24.08.95
dnl Updated   : 09.07.96
dnl Notes     : 
dnl
dnl Copyright 1996 by Thomas Dickey
dnl             You may  freely  copy or  redistribute  this software,
dnl             so  long as there is no profit made from its use, sale
dnl             trade or  reproduction.  You may not change this copy-
dnl             right notice, and it must be included in any copy made
dnl
dnl Macros used in TIN auto-configuration script.
dnl
dnl ---------------------------------------------------------------------------
dnl (macros from ftp.clark.net:/pub/dickey/autoconf)
dnl ---------------------------------------------------------------------------
AC_DEFUN(AC_FUNC_SETPGRP,
[AC_CACHE_CHECK(whether setpgrp takes no argument, ac_cv_func_setpgrp_void,
AC_TRY_RUN([
/*
 * If this system has a BSD-style setpgrp, which takes arguments, exit
 * successfully.
 */
main()
{
    if (setpgrp(1,1) == -1)
	exit(0);
    else
	exit(1);
}
], ac_cv_func_setpgrp_void=no, ac_cv_func_setpgrp_void=yes,
   AC_MSG_ERROR(cannot check setpgrp if cross compiling))
)
if test $ac_cv_func_setpgrp_void = yes; then
  AC_DEFINE(SETPGRP_VOID)
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl ---------------------------------------------------------------------------
dnl Test for ANSI token substitution (used in 'assert').
AC_DEFUN([CF_ANSI_ASSERT],
[
AC_MSG_CHECKING([for ansi token substitution])
AC_CACHE_VAL(cf_cv_ansi_assert,[
	AC_TRY_COMPILE([
#define string(n) #n],
	[char *s = string(token)],
	[cf_cv_ansi_assert=yes],
	[cf_cv_ansi_assert=no])
])
AC_MSG_RESULT($cf_cv_ansi_assert)
test $cf_cv_ansi_assert = yes && AC_DEFINE(HAVE_ANSI_ASSERT)
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
	return (stat("core", &sb) == 0 && ((sb.st_mode & S_IFMT) == S_IFREG));
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
dnl
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
AC_DEFUN([CF_MSG_LOG],
echo "(line __oline__) testing $* ..." 1>&5
)dnl
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
dnl	Check for declaration of sys_errlist in one of stdio.h and errno.h.  
dnl	Declaration of sys_errlist on BSD4.4 interferes with our declaration.
dnl	Reported by Keith Bostic.
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
test $cf_cv_dcl_sys_errlist = no && AC_DEFINE(DECL_SYS_ERRLIST)
])dnl
dnl ---------------------------------------------------------------------------
AC_DEFUN([CF_SYS_NAME],[
SYS_NAME=`(uname -a || hostname) 2>/dev/null | sed 1q`
test -z "$SYS_NAME" && SYS_NAME=unknown
AC_DEFINE_UNQUOTED(SYS_NAME,"$SYS_NAME")
echo "Configuring `make version` for $SYS_NAME"
])dnl
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
dnl $1=uppercase($2)
AC_DEFUN([CF_UPPER],
[
changequote(,)dnl
$1=`echo $2 | tr '[a-z]' '[A-Z]'`
changequote([,])dnl
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
AC_DEFUN([CF_WITH_PATH],
[AC_ARG_WITH($1,[$2 ](default: ifelse($4,,empty,$4)),,
ifelse($4,,[withval="${$3}"],[withval="${$3-$4}"]))dnl
case ".$withval" in #(vi
./*)
  ;; #(vi
*)
  echo 'configure: error: expected a pathname for $3' 1>&2
  exit 1
  ;;
esac
$3="$withval"
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
   ac_cv_path_[cf_path_name]="$withval"
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
AC_SUBST($3)dnl
])dnl
