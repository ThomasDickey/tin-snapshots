/*
 *  Project   : tin - a Usenet reader
 *  Module    : makecfg.c
 *  Author    : Thomas E. Dickey <dickey@clark.net>
 *  Created   : 1997-08-23
 *  Updated   : 1999-11-22
 *  Notes     : #defines and structs for config.c
 *  Copyright : (c) Copyright 1997-99 by Thomas E. Dickey
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#define __BUILD__
#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */

#define L_CURL '{'
#define R_CURL '}'

#define MAXNAME 27 /* maximum name-length (just for readability formatting) */
#define MAXTYPE 5  /* limits opt_type to keep names unique within 31 chars */

#define MYDATA struct mydata
MYDATA {
	MYDATA *link;
	char *name;
	char *type;
	char *size;
};

static MYDATA *all_data;
static int line_no;

static void
failed (
	const char *message)
{
	perror (message);
	exit (EXIT_FAILURE);
}

static FILE *
open_it (
	const char *filename,
	const char *mode)
{
	FILE *fp = fopen (filename, mode);

	if (fp == 0)
		failed (filename);
	return fp;
}

static char *
string_dup (
	const char *string)
{
	return strcpy ((char *)malloc (strlen (string) + 1), string);
}

static void
store_data (
	const char *name,
	const char *type,
	const char *size)
{
	MYDATA *p = (MYDATA *) malloc (sizeof (MYDATA));
	MYDATA *q;

	p->link = 0;
	p->name = string_dup (name);
	p->type = string_dup (type);
	p->size = string_dup (size);

	if ((q = all_data) == 0)
		all_data = p;
	else {
		while (q->link != 0)
			q = q->link;
		q->link = p;
	}
}

static void
parse_tbl (
	char *buffer)
{
	char *s = buffer;
	char *t = s + strlen (s);

	/* strip leading/trailing blanks */
	while ((t-- != s) && isspace ((int)*t))
		*t = '\0';
	while (isspace ((int)*s))
		s++;
	buffer = s;

	line_no++;
	if (*buffer != ';' && *buffer != '\0') {	/* ignore comments */
		if (*buffer == '#') {
			store_data (buffer, "", "");
		} else {
			/*
			 * otherwise the data consists of up to 3 blank
			 * separated columns (name, type, size).
			 */
			while (*s && !isspace ((int)*s))
				s++;
			while (isspace ((int)*s))
				*s++ = '\0';
			t = s;
			while (*t && !isspace ((int)*t))
				t++;
			while (isspace ((int)*t))
				*t++ = '\0';
			store_data (buffer, s, t);
		}
	}
}

static void
write_it (
	FILE *ofp,
	const char * const *table)
{
	int n;

	for (n = 0; table[n] != 0; n++)
		fprintf (ofp, "%s\n", table[n]);
}

static int
index_of(
	MYDATA *p)
{
	int result = 0;
	MYDATA *q;

	for (q = all_data; q != 0 && q != p; q = q->link) {
		if (!strcmp(p->type, q->type)) {
			result++;
		}
	}
	return result;
}

static int
type_is_int(
	MYDATA *p)
{
	return strcmp(p->type, "OPT_STRING")
	  &&   strcmp(p->type, "OPT_CHAR")
	  &&   strcmp(p->type, "OPT_ON_OFF");
}

static const char *
typename_of(
	MYDATA *p)
{
	if (!strcmp(p->type, "OPT_STRING") || !strcmp(p->type, "OPT_CHAR"))
		return "char *";
	if (!strcmp(p->type, "OPT_ON_OFF"))
		return "t_bool *";
	return "int *";
}

static void
generate_tbl (
	FILE *ofp)
{
	static const char *const table_1[] =
	{
		 ""
		,"enum option_enum {"
		,0
	};
	static const char *const table_2[] =
	{
		"\tVERY_LAST_OPT };"
		,""
		,"#define LAST_OPT VERY_LAST_OPT - 1"
		,"#define OPT_ARG_COLUMN	9"
		,""
		,"#define OPT_ON_OFF    1"
		,"#define OPT_LIST      2"
		,"#define OPT_STRING    3"
		,"#define OPT_NUM       4"
		,"#define OPT_CHAR      5"
		,""
		,"struct t_option option_table[]={"
		,0
	};
	static const char *const table_3[] =
	{
		"};"
		,0
	};
	static const char prefix[] = "  { ";
	static const char suffix[] = "},";
	MYDATA *p;
	char temp[BUFSIZ];

	/* generate enumerated type */
	write_it (ofp, table_1);
	for (p = all_data; p != 0; p = p->link) {
		if (p->name[0] == '#')
			fprintf (ofp, "%s\n", p->name);
		else {
			char *s = p->name;

			fprintf (ofp, "\tOPT_");
			while (*s != '\0') {
				fprintf (ofp, "%c",
					 isalpha ((unsigned char)*s) && islower ((unsigned char)*s)
					 ? toupper ((unsigned char)*s)
					 : *s);
				s++;
			}
			fprintf (ofp, ",\n");
		}
	}

	/* generate the access table */
	write_it (ofp, table_2);
	for (p = all_data; p != 0; p = p->link) {
		if (p->name[0] == '#')
			fprintf (ofp, "%s\n", p->name);
		else {
			int is_opt = !strncmp (p->type, "OPT_", 4);
			int is_int = type_is_int(p);
			char *dft_name = p->name;
			/* TODO is this still necessary ? */
			/* shorten message-variable names */
			if (!strncmp (dft_name, "default_", 8))
				dft_name += 8;

			fprintf (ofp, "%s", prefix);
			sprintf (temp, "%s,", is_opt ? p->type : "OPT_LIST");
			fprintf (ofp, "%-13s", temp);

			if (!is_int) {
				fprintf(ofp, "OINX_%s, 0, ", p->name);
			} else {
				fprintf (ofp, "0, &tinrc.%s, ", p->name);
			}

			if (is_opt)
				fprintf (ofp, "NULL, 0, ");
			else
				fprintf (ofp, "%s, %s, ", p->type, p->size);
			fprintf (ofp, "&txt_%s ", dft_name);
			fprintf (ofp, "%s\n", suffix);
		}
	}

	write_it (ofp, table_3);
}

static void
generate_ptr(
	FILE *ofp,
	const char *opt_type,
	const char *ptr_type,
	int mode)
{
	MYDATA *p, *q;
	int after;
	const char *addr = !strcmp(opt_type, "OPT_STRING") ? "" : "&";

	switch (mode) {
	case 0:
		fprintf(ofp, "\n%s %s_list[] = %c\n", ptr_type, opt_type, L_CURL);
		break;
	case 1:
		fprintf(ofp, "\ntypedef OTYP %c\n", L_CURL);
		break;
	case 2:
		fprintf(ofp, "\n");
		break;
	}
	after  = FALSE;

	for (p = all_data, q = 0; p != 0; p = p->link) {
		if (p->name[0] == '#') {
			if (!strcmp(p->name, "#endif")) {
				if (after) {
					fprintf(ofp, "%s\n", p->name);
					after = FALSE;
				}
				q = 0;
			} else {
				q = p;
			}
		} else if (!strcmp(p->type, opt_type)) {
			if (q != 0) {
				fprintf(ofp, "%s\n", q->name);
				q = 0;
				after = TRUE;
			}
			switch (mode) {
			case 0:
				fprintf(ofp, "\t%stinrc.%s,%*s/* %2d: %s__ */\n",
					addr,
					p->name,
					MAXNAME - (int)(strlen(addr) + strlen(p->name)),
					" ",
					index_of(p),
					p->name);
				break;
			case 1:
				fprintf(ofp, "\tOVAL(oinx_%.*s, %s__)\n",
					MAXTYPE, opt_type,
					p->name);
				break;
			case 2:
				fprintf(ofp, "#define OINX_%-*.*s OINX(oinx_%.*s, %s__)\n",
					MAXNAME, MAXNAME,
					p->name,
					MAXTYPE, opt_type,
					p->name);
				break;
			}
		}
	}

	switch (mode) {
	case 0:
		fprintf(ofp, "%c;\n", R_CURL);
		break;
	case 1:
		fprintf(ofp, "\tOVAL(oinx_%.*s, s_MAX)\n", MAXTYPE, opt_type);
		fprintf(ofp, "\tOEND(oinx_%.*s, Q1)\n", MAXTYPE, opt_type);
		fprintf(ofp, "%c oinx_%.*s;\n", R_CURL, MAXTYPE, opt_type);
		break;
	case 2:
		break;
	}
}

static void
makecfg (
	FILE *ifp,
	FILE *ofp)
{
	char buffer[BUFSIZ];
	MYDATA *p, *q;

	static const char *const table_1[] =
	{
		"/* This file is generated by MAKECFG */"
		,""
		,"#ifndef TINTBL_H"
		,"#define TINTBL_H 1"
		,""
		,"/* Macros for defining symbolic offsets that can be ifdef'd */"
		,"#undef OINX"
		,"#undef OVAL"
		,"#undef OEND"
		,"#undef OTYP"
		,""
		,"#ifdef lint"
		,"#\tdefine OINX(T, M) 0 /* 'lint -c' cannot be appeased */"
		,"#\tdefine OVAL(T, M) char M;"
		,"#\tdefine OEND(T, M) char M;"
		,"#\tdefine OTYP struct"
		,"#else"
		,"#\tifdef CPP_DOES_CONCAT"
		,"#\t\tdefine OINX(T, M) T ## M"
		,"#\t\tdefine OVAL(T, M) T ## M,"
		,"#\t\tdefine OEND(T, M) T ## M"
		,"#\t\tdefine OTYP enum"
		,"#\telse"
		,"#\t\tdefine OINX(T, M) \\"
		,"\t\t\t(((int)&(((T*)0)->M))/ \\"
		,"\t\t\t ((int)&(((T*)0)->Q1) - (int)&(((T*)0)->s_MAX)))"
		,"#\t\tdefine OVAL(T, M) char M;"
		,"#\t\tdefine OEND(T, M) char M;"
		,"#\t\tdefine OTYP struct"
		,"#\tendif"
		,"#endif"
		,0
	};
	static const char *const table_2[] =
	{
		 ""
		,"/* We needed these only to make the table compile */"
		,"#undef OINX"
		,"#undef OVAL"
		,"#undef OEND"
		,"#undef OTYP"
		,""
		,"#endif /* TINTBL_H */"
		,0
	};

	/*
	 * Process the input file.
	 */
	line_no = 0;
	while (fgets (buffer, sizeof (buffer) - 1, ifp))
		parse_tbl (buffer);
	fclose (ifp);

	/*
	 * Generate the output file
	 */
	write_it (ofp, table_1);

	/*
	 * For each type used in the table, generate a list of pointers to
	 * that type.
	 */
	for (p = all_data; p != 0; p = p->link) {
		int found = FALSE;

		if (p->name[0] == '#')
			continue;
		if (type_is_int(p))
			continue;

		for (q = all_data; p != q; q = q->link) {
			if (!strcmp(p->type, q->type)) {
				found = TRUE;
				break;
			}
		}
		if (!found
		 && !strncmp(p->type, "OPT_", 4)) {
			generate_ptr (ofp, p->type, typename_of(p), 0);
			generate_ptr (ofp, p->type, typename_of(p), 1);
			generate_ptr (ofp, p->type, typename_of(p), 2);
		}
	}
	generate_tbl (ofp);

	write_it (ofp, table_2);

	fclose (ofp);
}

int
main (
	int argc,
	char *argv[])
{
	FILE *input = stdin;
	FILE *output = stdout;

	if (argc > 1)
		input = open_it (argv[1], "r");
	if (argc > 2)
		output = open_it (argv[2], "w");
	makecfg (input, output);

	return (0);
}
