/*
 *  Project   : tin - a Usenet reader
 *  Module    : mail.c
 *  Author    : I.Lea
 *  Created   : 02-10-92
 *  Updated   : 11-08-94
 *  Notes     : Mail handling routines for creating pseudo newsgroups
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

/*
 *  Load the mail active file into active[]
 */

#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
void
read_mail_active_file ()
{
	char	buf[LEN];
	char	spooldir[PATH_LEN];
	FILE	*fp;
	int		i;
	long	count = -1L, h;
	long	min, max;

	if (SHOW_UPDATE)
		wait_message (txt_reading_mail_active_file);

	/*
	 * Open the mail active file
	 */
	if ((fp = open_mail_active_fp ("r")) == (FILE *) 0) {
		if (cmd_line) {
			my_fputc ('\n', stderr);
		}
		error_message (txt_cannot_open, mail_active_file);
		/*
		 * FIXME - maybe do an autoscan of maildir, create & do a reopen ?
		 */
		write_mail_active_file ();
		return;
	}

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
/*
printf ("Line=[%s", buf);
fflush(stdout);
*/
		if (!parse_active_line (buf, &max, &min, spooldir) || *buf == '\0') {
			continue;
		}

		/*
		 * Load mailgroup into group hash table
		 */
		if (psGrpAdd (buf) != 0)
			goto read_mail_active_continue;

		/*
		 * Load group info.
		 */
		active[num_active].type = GROUP_TYPE_MAIL;
		active[num_active].name = my_strdup (buf);
		active[num_active].spooldir = my_strdup (spooldir);
		active[num_active].description = (char *) 0;
		active[num_active].count = count;
		active[num_active].xmax = max;
		active[num_active].xmin = min;
		active[num_active].moderated = 'y';
		active[num_active].next = -1;			/* hash chaining */
		active[num_active].inrange = FALSE;
		active[num_active].read_during_session = FALSE;
		active[num_active].art_was_posted = FALSE;
		active[num_active].subscribed = UNSUBSCRIBED;	/* not in my_group[] yet */
		active[num_active].newsrc.xbitmap = (t_bitmap *) 0;
		active[num_active].attribute = (struct t_attribute *) 0;
		active[num_active].glob_filter = &glob_filter;
		active[num_active].grps_filter = (struct t_filters *) 0;
		vSetDefaultBitmap (&active[num_active]);
		num_active++;

read_mail_active_continue:;

	}
	fclose (fp);

	if ((cmd_line && !(update || verbose)) || (update && update_fork)) {
		wait_message ("\n");
	}
}
#endif	/* !INDEX_DAEMON && HAVE_MH_MAIL_HANDLING */

/*
 *  Write out mailgroups from active[] to ~/.tin/active.mail
 */

#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
void
write_mail_active_file ()
{
	char acGrpPath[PATH_LEN];
	FILE *fp;
	register int i;
	struct t_group *psGrp;

	vPrintActiveHead (mail_active_file);

	if ((fp = open_mail_active_fp ("a+")) != (FILE *) 0) {
		for (i = 0 ; i < num_active ; i++) {
			psGrp = &active[i];
			if (psGrp->type == GROUP_TYPE_MAIL) {
				vMakeGrpPath (psGrp->spooldir, psGrp->name, acGrpPath);
				vFindArtMaxMin (acGrpPath, &psGrp->xmax, &psGrp->xmin);
				vPrintGrpLine (fp, psGrp->name, psGrp->xmax, psGrp->xmin, psGrp->spooldir);
			}
		}
		fclose (fp);
	}
}
#endif	/* !INDEX_DAEMON && HAVE_MH_MAIL_HANDLING */

/*
 *  Load the text description from ~/.tin/mailgroups for each mail group into
 *  the active[] array.
 */

#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
void
read_mailgroups_file ()
{
	FILE *fp;

	if (show_description == FALSE || save_news || catchup) {
		return;
	}

	if ((fp = open_mailgroups_fp ()) != (FILE *) 0) {
		wait_message (txt_reading_mailgroups_file);

		read_groups_descriptions (fp, (FILE *) 0);

		fclose (fp);

		if (cmd_line && !(update || verbose)) {
			wait_message ("\n");
		}
	}
}
#endif	/* !INDEX_DAEMON && HAVE_MAIL_HANDLING */

/*
 *  Load the text description from NEWSLIBDIR/newsgroups for each group into the
 *  active[] array. Save a copy locally if reading via NNTP to save bandwidth.
 */

void
read_newsgroups_file ()
{
#ifndef INDEX_DAEMON
	FILE *fp;
	FILE *fp_save = (FILE *) 0;

	if (show_description == FALSE || save_news || catchup) {
		return;
	}

	wait_message (txt_reading_newsgroups_file);

	if ((fp = open_newsgroups_fp ()) != (FILE *) 0) {
		if (read_news_via_nntp && !read_local_newsgroups_file) {
#ifdef VMS
			fp_save = fopen (local_newsgroups_file, "w", "fop=cif");
#else
			fp_save = fopen (local_newsgroups_file, "w");
#endif
		}

		read_groups_descriptions (fp, fp_save);

		fclose (fp);

		if (fp_save != (FILE *) 0) {
			fclose (fp_save);
			read_local_newsgroups_file = TRUE;
		}
	}

	if (cmd_line && !(update || verbose)) {
		wait_message ("\n");
	}
#endif	/* INDEX_DAEMON */
}

/*
 *  Read groups descriptions from opened file & make local backup copy
 *  of all groups that don't have a 'x' in the active file moderated
 *  field & if reading groups of type GROUP_TYPE_NEWS.
 */

void
read_groups_descriptions (fp, fp_save)
	FILE *fp;
	FILE *fp_save;
{
#ifndef INDEX_DAEMON
	char buf[LEN];
	char group[PATH_LEN];
	char *p, *q;
	int count = 0;
	struct t_group *psGrp;

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		if (buf[0] == '#' || buf[0] == '\n') {
			continue;
		}
		p = strrchr (buf, '\n');
		if (p != (char *) 0) {
			*p = '\0';
		}

		for (p = buf, q = group ; *p && *p != ' ' && *p != '\t' ; p++, q++) {
			*q = *p;
		}
		*q = '\0';

		while (*p == '\t' || *p == ' ') {
			p++;
		}

		psGrp = psGrpFind (group);

		if (psGrp != (struct t_group *) 0 && psGrp->description == (char *) 0) {
			q = p;
			while ((q = strchr (q, '\t')) != (char *) 0) {
				*q = ' ';
			}
			psGrp->description = my_strdup (p);
			if (psGrp->type == GROUP_TYPE_NEWS) {
				if (fp_save != (FILE *) 0 &&
				    read_news_via_nntp &&
				    !read_local_newsgroups_file) {
					fprintf (fp_save, "%s\n", buf);
				}
			}
		}
		if (++count % 100 == 0) {
			spin_cursor ();
		}
	}
#endif	/* INDEX_DAEMON */
}

void
vPrintActiveHead (pcActiveFile)
	char	*pcActiveFile;
{
	FILE	*hFp;

	if ((hFp = fopen (pcActiveFile, "w")) != (FILE *) 0) {
		fprintf (hFp, "# [Mail/Save] active file. Format is like news active file:\n");
		fprintf (hFp, "#   groupname  max.artnum  min.artnum  /dir\n");
		fprintf (hFp, "# The 4th field is the basedir (ie. ~/Mail or ~/News)\n#\n");
		fclose (hFp);
	}
}

void
vParseGrpLine (pcLine, pcGrpName, plArtMax, plArtMin, pcModerated)
	char	*pcLine;
	char	*pcGrpName;
	long	*plArtMax;
	long	*plArtMin;
	char	*pcModerated;
{
	char	*pcPtr;

	*pcGrpName = '\0';
	*pcModerated = '\0';

	if (*pcLine == '#' || *pcLine == '\n') {
		return;
	}

	pcPtr = strrchr (pcLine, '\n');
	if (pcPtr != (char *) 0) {
		*pcPtr = '\0';
	}

	/*
	 * Group name
	 */
	pcPtr = pcLine;
	while (*pcPtr && *pcPtr != ' ' && *pcPtr != '\t') {
		pcPtr++;
	}
	*pcPtr++ = '\0';
	strcpy (pcGrpName, pcLine);

	/*
	 * Art max
	 */
	while (*pcPtr && (*pcPtr == ' ' || *pcPtr == '\t')) {
		pcPtr++;
	}
	*plArtMax = atol (pcPtr);

	/*
	 * Art min
	 */
	while (*pcPtr && *pcPtr != ' ' && *pcPtr != '\t') {
		pcPtr++;
	}
	while (*pcPtr && (*pcPtr == ' ' || *pcPtr == '\t')) {
		pcPtr++;
	}
	*plArtMin = atol (pcPtr);

	/*
	 * 4th field (Moderated/base maildir)
	 */
	while (*pcPtr && *pcPtr != ' ' && *pcPtr != '\t') {
		pcPtr++;
	}
	while (*pcPtr && (*pcPtr == ' ' || *pcPtr == '\t')) {
		pcPtr++;
	}
	strcpy (pcModerated, pcPtr);
}

void
vFindArtMaxMin (pcGrpPath, plArtMax, plArtMin)
	char	*pcGrpPath;
	long	*plArtMax;
	long	*plArtMin;
{
	DIR	*tDirFile;
	DIR_BUF	*tFile;
	long	lArtNum;

	*plArtMin = *plArtMax = 0L;

	if (access (pcGrpPath, R_OK) != 0) {
		*plArtMin = 1L;
		return;
	}

	tDirFile = opendir (pcGrpPath);
	if (tDirFile != (DIR *) 0) {
		while ((tFile = readdir (tDirFile)) != (DIR_BUF *) 0) {
			lArtNum = lAtol (tFile->d_name, (int) D_NAMLEN(tFile));
			if (lArtNum >= 1) {
				if (lArtNum > *plArtMax) {
					*plArtMax = lArtNum;
					if (*plArtMin == 0) {
						*plArtMin = lArtNum;
					}
				} else if (lArtNum < *plArtMin) {
					*plArtMin = lArtNum;
				}
			}
		}
		closedir (tDirFile);
	}
	if (*plArtMin == 0) {
		*plArtMin = 1;
	}
}

void
vPrintGrpLine (hFp, pcGrpName, lArtMax, lArtMin, pcBaseDir)
	FILE	*hFp;
	char	*pcGrpName;
	long	lArtMax;
	long	lArtMin;
	char	*pcBaseDir;
{
	fprintf (hFp, "%s %05ld %05ld %s\n",
		pcGrpName, lArtMax, lArtMin, pcBaseDir);
}

long
lAtol (pcStr, iNum)
	char	*pcStr;
	int	iNum;
{
	long	lValue = 0L;

#ifdef QNX4
	lValue = atol (pcStr);
#else
	while (*pcStr && iNum--) {
		if (*pcStr >= '0' && *pcStr <= '9') {
			lValue = lValue * 10 + (*pcStr - '0');
		} else {
			return -1;
		}
		pcStr++;
	}
#endif

	return lValue;
}

/*
 * Given a base pathname & a newsgroup name build an absolute pathname.
 * base = /usr/spool/news
 * newsgroup = alt.sources
 * absolute path = /usr/spool/news/alt/sources
 */

void
vMakeGrpPath (pcBaseDir, pcGrpName, pcGrpPath)
	char	*pcBaseDir;
	char	*pcGrpName;
	char	*pcGrpPath;
{
	char	*pcPtr;

	joinpath (pcGrpPath, pcBaseDir, pcGrpName);

	pcPtr = pcGrpPath + strlen (pcBaseDir);
	while ((pcPtr = strchr (pcPtr, '.')) != (char *) 0) {
		*pcPtr = '/';
	}
}

/*
 * Given an absolute pathname & a base pathname build a newsgroup name
 * base = /usr/spool/news
 * absolute path = /usr/spool/news/alt/sources
 * newsgroup = alt.sources
 */

void
vMakeGrpName (pcBaseDir, pcGrpName, pcGrpPath)
	char	*pcBaseDir;
	char	*pcGrpName;
	char	*pcGrpPath;
{
	char	*pcPtrBase;
	char	*pcPtrName;
	char	*pcPtrPath;

	pcPtrBase = pcBaseDir;
	pcPtrPath = pcGrpPath;

	while (*pcPtrBase && (*pcPtrBase == *pcPtrPath)) {
		pcPtrBase++;
		pcPtrPath++;
	}
	strcpy (pcGrpName, ++pcPtrPath);

	pcPtrName = pcGrpName;
	while ((pcPtrName = strchr (pcPtrName, '/')) != (char *) 0) {
		*pcPtrName = '.';
	}
}


#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
void
vGrpDelMailArt (psArt)
	struct t_article *psArt;
{

	if (psArt->delete) {
		art_mark_undeleted (psArt);
		info_message ("Article undeleted");
	} else {
		art_mark_deleted (psArt);
		info_message ("Article deleted");
	}
}
#endif	/* INDEX_DAEMON */


#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
void
vGrpDelMailArts (psGrp)
	struct t_group *psGrp;
{
	char	acArtFile[PATH_LEN];
	char	acGrpPath[PATH_LEN];
	int		iNum;
	int		iUpdateIndexFile = FALSE;
	struct	t_article *psArt;

	if (psGrp->type == GROUP_TYPE_MAIL) {
		wait_message ("Processing mail messages marked for deletion");

		vMakeGrpPath (psGrp->spooldir, psGrp->name, acGrpPath);

		for (iNum = 0; iNum < top; iNum++) {
			psArt = &arts[iNum];
			if (psArt->delete) {
				sprintf (acArtFile, "%s/%ld", acGrpPath, psArt->artnum);
				unlink (acArtFile);
				psArt->thread = ART_EXPIRED;
				iUpdateIndexFile = TRUE;
			}
		}

/* MAYBE also check if min / max article was deleted.  If so then update
   the active[] entry for the group and rewrite the mail.active file
*/
		if (iUpdateIndexFile)
			vWriteNovFile (psGrp);
	}
}
#endif	/* !INDEX_DAEMON */


int
iArtEdit (psGrp, psArt)
	struct t_group *psGrp;
	struct t_article *psArt;
{
#ifndef INDEX_DAEMON
	char	acArtFile[PATH_LEN];
	char	acTmpFile[PATH_LEN];

	/*
	 * Check if news / mail group
	 */
	if (psGrp->type != GROUP_TYPE_NEWS) {
		vMakeGrpPath (psGrp->spooldir, psGrp->name, acTmpFile);
		sprintf (acArtFile, "%s/%ld", acTmpFile, psArt->artnum);
		sprintf (acTmpFile, "%s%d.art", TMPDIR, process_id);
		if (iCopyFile (acArtFile, acTmpFile)) {
			invoke_editor (acTmpFile, 1);
			rename_file (acTmpFile, acArtFile);
			return TRUE;
		}
	}
#endif	/* INDEX_DAEMON */

	return FALSE;
}
