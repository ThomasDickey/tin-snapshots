s/#define/#	define/
s/\/\* \.\//\/\* /
s/void (\*sig/RETSIGTYPE (\*sig/
s/void (\*func/RETSIGTYPE (\*func/
s/(int sig)/(SIG_ARGS)/g
s/struct[ 	]\+_iobuf/FILE/g
