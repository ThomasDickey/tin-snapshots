/*
 *  Project   : tin - a Usenet reader
 *  Module    : nrctbl.c
 *  Author    : Sven Paulus
 *  Created   : 06-10-96
 *  Updated   : 06-11-96
 *  Notes     : This module does the NNTP server name lookup in
 *              ~/.tin/newsrctable and returns the real hostname
 *              and the name of the newsrc file for a given
 *              alias of the server.
 *  Copyright : (c) Copyright 1996 by Sven Paulus
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include 	"patchlev.h"
#include	"menukeys.h"
#include	"tin.h"


/* write_newsrctable_file()
 * create newsrctable file in local rc directory
 */

void
write_newsrctable_file()
{

	FILE *fp;

	if ((fp = fopen(local_newsrctable_file, "w")) == (FILE *) 0)
		return;

	fprintf(fp, "# NNTP-server -> newrc translation table and NNTP-server\n");
	fprintf(fp, "# shortname list for %s %s\n#\n", progname, VERSION);
	fprintf(fp, "# the format of this file is\n");
	fprintf(fp, "#   <FQDN of NNTP-server> <newsrc file> <shortname> ...\n#\n");
	fprintf(fp, "# if <newsrc file> is given without path, $HOME is assumed as its location\n#\n");
	fprintf(fp, "# valid examples are (don't use these servers, they are only examples)\n");
	fprintf(fp, "#   news.sub.net             .newsrc-subnet  news subnet newssub\n");
	fprintf(fp, "#   news.rz.uni-karlsruhe.de /tmp/nrc-rz     rz news.rz\n");
	fprintf(fp, "#\n");

	fclose(fp);
}


/* get_nnpserver ()
 * returns the FQDN of NNTP server by looking up a given
 * nickname or alias in the newsrctable
 * ---> extend to allow nameserver-lookups, if search in table
 *      failed
 */

void
get_nntpserver (nntpserver_name, nick_name)
	char *nntpserver_name;
	char *nick_name;
{
	FILE 	*fp;
	char	line[LEN];
	char	*line_entry;
	int	line_entry_counter;
	char	name_found[PATH_LEN];
	int 	found = 0;

	if ((fp = fopen(local_newsrctable_file, "r")) != (FILE *) 0) {
		while ((fgets(line, sizeof(line), fp) != NULL) && (!found)) {
			line_entry_counter = 0;

			if (!strchr("# ;", line[0])) {
				while ((line_entry = strtok(line_entry_counter ? NULL : line, " \t\n")) != NULL) {
					line_entry_counter++;

					if (line_entry_counter == 1)
						strcpy(name_found, line_entry);

					if ((line_entry_counter > 2) && (!strcasecmp(line_entry, nick_name)))
						found = 1;
				}
			}
		}
		fclose(fp);
		if (found) strcpy(nntpserver_name, name_found);
		else strcpy(nntpserver_name, nick_name);
	}
	else {
		write_newsrctable_file();
		strcpy(nntpserver_name, nick_name);
	}
}

/* get_newsrcname()
 * get name of newsrc file with given name of nntp server
 * returns TRUE if name was found, FALSE if the search failed
 */

int
get_newsrcname (newsrc_name, nntpserver_name)
	char *newsrc_name;
	char *nntpserver_name;
{
	FILE 	*fp;
	char	line[LEN];
	char	*line_entry;
	int	line_entry_counter;
	char	name_found[PATH_LEN];
	int 	found = 0;
	int	do_cpy = 0;

	if ((fp = fopen(local_newsrctable_file, "r")) != (FILE *) 0) {
		while ((fgets(line, sizeof(line), fp) != NULL) && (found != 1)) {
			line_entry_counter = 0;

			if (!strchr("# ;", line[0])) {
				while ((line_entry = strtok(line_entry_counter ? NULL :line, " \t\n")) != NULL) {
					line_entry_counter++;

					if ((line_entry_counter == 1) && (! strcasecmp(line_entry, nntpserver_name))) {
						found = 1;
						do_cpy = 1;
					}

					if ((line_entry_counter ==1 ) && ((! strcasecmp(line_entry, "default")) || (! strcmp(line_entry,"*")))) {
						found = 2;
						do_cpy =1;
					}
					if (do_cpy && (line_entry_counter == 2)) {
						strcpy(name_found, line_entry);
						do_cpy = 0;
					}
				}
			}
		}
		fclose(fp);
		if (found) {
			int	error=0;
			char	dir[PATH_LEN];
			char    tmp_newsrc[PATH_LEN];

			if (! strfpath (name_found, tmp_newsrc, sizeof (tmp_newsrc),
				homedir, (char *) 0, (char *) 0, (char *) 0)) {
					fprintf (stderr, "couldn't expand %s\n", name_found);
					error=1;
			} else {
				if (tmp_newsrc[0] == '/')
					(void) strcpy (newsrc_name, tmp_newsrc);
				else
					joinpath (newsrc_name, homedir, tmp_newsrc);
			}
			strcpy (dir, newsrc_name);
			*strrchr (dir, '/') = (char) 0;

			if (! error) {
			/* FIXME - write a global permssion check routine */
				if (access (dir, X_OK)) {
					fprintf (stderr, txt_error_no_enter_permission, dir);
					error=1;
				} else if (access (newsrc_name, F_OK)) {
					fprintf (stderr, txt_error_no_such_file, newsrc_name);
					error=2;
				} else if (access (dir, R_OK)) {
					fprintf (stderr, txt_error_no_read_permission, dir);
					error=1;
				} else if (access (newsrc_name, R_OK)) {
					fprintf (stderr, txt_error_no_read_permission, newsrc_name);
					error=1;
				} else if (access (dir, W_OK)) {
					fprintf (stderr, txt_error_no_write_permission, dir);
					error=1;
				} else if (access (newsrc_name, W_OK)) {
					fprintf (stderr, txt_error_no_write_permission, newsrc_name);
					error=1;
				}
			}
			if (error) {
				char ch;
				char default_ch = iKeyNrctblIgnore;

				do {
					if (error >= 2) {
						default_ch = iKeyNrctblCreate;
						sprintf (msg, "%s%c", txt_nrctbl_create, default_ch);
					} else {
						sprintf (msg, "%s%c", txt_nrctbl_default, default_ch);
					}
					wait_message (msg);

					/*
					 * FIXME - cursor possition is wrong &
					 * <return> is needed at the end
					 */

					if ((ch = (char) ReadCh ()) == '\r' || ch == '\n')
						ch = default_ch;
				} while (! strchr ("\033cdiq", ch));

				switch(ch) {
					case iKeyNrctblCreate:
						/* FIXME this doesn't check if we could create the file */
						return TRUE;
					case iKeyNrctblDefault:
						joinpath(newsrc_name, homedir, ".newsrc");
						return TRUE;
					case iKeyNrctblIgnore:
						sprintf(msg, "%s", txt_warn_difficulties);
						wait_message (msg);
						return TRUE;
					case iKeyQuit:
						exit(0);
					case ESC:
					default:
						break;
				}
			}
			return TRUE;
		}
	}
	else {
		(void) write_newsrctable_file();
	}
	return FALSE;
}
