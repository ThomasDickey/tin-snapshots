$! --- make.com ---
$!
$! Compile using VAXC	22-Jun-94	L.Brennan@cchs.su.edu.au
$!
$ if (P1 .eqs. "DEBUG")
$ then dbg := "/Debug=ALL"
$ else dbg :=
$ endif
$!
$ Define SYS Sys$Library
$!
$ If (f$search("strings.h") .eqs. "") then Copy Sys$Library:String.h strings.h
$!
$ CC 'dbg' /Include=([]) vmsdir
$ CC 'dbg' /Include=([]) vmspwd
$ CC 'dbg' /Include=([]) vmsfile
$ CC 'dbg' /Include=([]) parse
$ CC 'dbg' /Include=([]) getopt
$ CC 'dbg' /Include=([]) getopt1
$ CC 'dbg' /Include=([]) qsort
$ CC 'dbg' /Include=([]) isxterm
$!
$ Library/Object/Create LIBVMS vmsdir,vmspwd,vmsfile,parse,getopt,getopt1,qsort,isxterm
$!
$ Exit
