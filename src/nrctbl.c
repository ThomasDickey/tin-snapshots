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
	fprintf(fp, "# shortname list for %s %s.\n#\n", progname, VERSION);
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
	
	if ((fp = fopen(local_newsrctable_file, "r")) != (FILE *) 0) {
		while ((fgets(line, sizeof(line), fp) != NULL) && (!found)) {
			line_entry_counter = 0;

			if (!strchr("# ;", line[0])) {
				while ((line_entry = strtok(line_entry_counter ? NULL :line, " \t\n")) != NULL) {
					line_entry_counter++;
					
					if ((line_entry_counter == 1) && (!strcasecmp(line_entry, nntpserver_name)))
						found = 1;

					if (line_entry_counter == 2)
						strcpy(name_found, line_entry);
				}
			}			
		}
		fclose(fp);
		if (found) {
			if (!strchr(name_found, '/'))
				joinpath(newsrc_name, homedir, name_found);
			else
				strcpy(newsrc_name, name_found);
			return TRUE;
		}	
	}
	else {
		(void) write_newsrctable_file();
	}
	return FALSE;
}
