#ifndef BOOL_H
#	define BOOL_H 1

#	ifndef FALSE
#		define FALSE 0
#	endif /* !FALSE */

#	ifndef TRUE
#		define TRUE (!FALSE)
#	endif /* !TRUE */

	typedef unsigned t_bool;	/* don't make this a char or short! */

	extern /*@unused@*/ t_bool bool_equal (t_bool, t_bool) /*@*/ ;
#	define bool_equal(a,b) ((a) ? (b) : !(b))

	extern /*@unused@*/ t_bool bool_not (t_bool) /*@*/ ;
#	define bool_not(b) ((b) ? FALSE : TRUE)

	extern /*@unused@*/ /*@observer@*/ char *bool_unparse (t_bool) /*@*/ ;
#	define bool_unparse(b) ((b) ? "TRUE" : "FALSE")

#endif /* !BOOL_H */
