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
#include	"tcurses.h"

/*
 *  Load the mail active file into active[]
 */

#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
void
read_mail_active_file (void)
{
	char	buf[LEN];
	char	spooldir[PATH_LEN];
	struct	t_group *ptr;
	FILE	*fp;
	long	count = -1L;
	long	min, max;

	if (INTERACTIVE)
		wait_message (0, txt_reading_mail_active_file);

	/*
	 * Open the mail active file
	 */
	if ((fp = open_mail_active_fp ("r")) == (FILE *) 0) {
		if (cmd_line)
			my_fputc ('\n', stderr);

		error_message (txt_cannot_open, mail_active_file);
		/*
		 * FIXME - maybe do an autoscan of maildir, create & do a reopen ?
		 */
		write_mail_active_file ();
		return;
	}

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
/*
my_printf ("Line=[%s", buf);
my_flush();
*/
		if (!parse_active_line (buf, &max, &min, spooldir) || *buf == '\0')
			continue;

		/*
		 * Load mailgroup into group hash table
		 */
		if ((ptr = psGrpAdd (buf)) == NULL)
			continue;

		/*
		 * Load group info. TODO - integrate with active_add()
		 */
		ptr->type = GROUP_TYPE_MAIL;
		ptr->spooldir = my_strdup (spooldir);
		ptr->description = (char *) 0;
		ptr->count = count;
		ptr->xmax = max;
		ptr->xmin = min;
		ptr->moderated = 'y';
		ptr->next = -1;			/* hash chaining */
		ptr->inrange = FALSE;
		ptr->read_during_session = FALSE;
		ptr->art_was_posted = FALSE;
		ptr->subscribed = FALSE;		/* not in my_group[] yet */
		ptr->newgroup = FALSE;
		ptr->newsrc.xbitmap = (t_bitmap *) 0;
		ptr->attribute = (struct t_attribute *) 0;
		ptr->glob_filter = &glob_filter;
		ptr->grps_filter = (struct t_filters *) 0;
		vSetDefaultBitmap (ptr);
	}
	fclose (fp);

	if (INTERACTIVE2)
		wait_message (0, "\n");
}


/*
 *  Write out mailgroups from active[] to ~/.tin/active.mail
 */

void
write_mail_active_file (void)
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


/*
 *  Load the text description from ~/.tin/mailgroups for each mail group into
 *  the active[] array.
 */

void
read_mailgroups_file (void)
{
	FILE *fp;

	if (!show_description || save_news || catchup)
		return;

	if ((fp = open_mailgroups_fp ()) != (FILE *) 0) {
		wait_message (0, txt_reading_mailgroups_file);

		read_groups_descriptions (fp, (FILE *) 0);

		fclose (fp);

/*PLOK2*/		if (cmd_line && !(update || verbose))
			wait_message (0, "\n");
	}
}
#endif	/* !INDEX_DAEMON && HAVE_MAIL_HANDLING */

/*
 *  Load the text description from NEWSLIBDIR/newsgroups for each group into the
 *  active[] array. Save a copy locally if reading via NNTP to save bandwidth.
 */

void
read_newsgroups_file (void)
{
#ifndef INDEX_DAEMON
	FILE *fp;
	FILE *fp_save = (FILE *) 0;

	if (!show_description || save_news || catchup)
		return;

	wait_message (0, txt_reading_newsgroups_file);

	if ((fp = open_newsgroups_fp ()) != (FILE *) 0) {

		if (read_news_via_nntp && !read_local_newsgroups_file)
			fp_save = fopen (local_newsgroups_file, "w" FOPEN_OPTS);

		read_groups_descriptions (fp, fp_save);

		if (fp_save != (FILE *) 0) {
			fclose (fp_save);
			read_local_newsgroups_file = TRUE;
		}

		TIN_FCLOSE (fp);
	}

/*PLOK2*/	if (cmd_line && !(batch_mode || verbose))
		wait_message (0, "\n");

#endif	/* INDEX_DAEMON */
}

/*
 *  Read groups descriptions from opened file & make local backup copy
 *  of all groups that don't have a 'x' in the active file moderated
 *  field & if reading groups of type GROUP_TYPE_NEWS.
 *  Aborting this early won't have any adverse affects, just some missing
 *  descriptions.
 */

void
read_groups_descriptions (
	FILE *fp,
	FILE *fp_save)
{
#ifndef INDEX_DAEMON
	char buf[LEN];
	char group[PATH_LEN];
	char *p, *q;
#ifdef SHOW_PROGRESS
	int count = 0;
#endif
	struct t_group *psGrp;

	while (tin_fgets (buf, sizeof (buf), fp) != (char *) 0) {
		if (buf[0] == '#' || buf[0] == '\0')
			continue;

#if 0 /* tin_fgets() strips \n for us */
		p = strrchr (buf, '\n');
		if (p != (char *) 0)
			*p = '\0';
#endif
/*
 *  This was moved from below and simplified.  I can't test here for the
 *  type of group being read, because that requires having found the
 *  group in the active file, and that truncates the local copy of the
 *  newsgroups file to only subscribed-to groups when tin is called with
 *  the "-q" option.
 */
		if ((fp_save != (FILE *) 0) && read_news_via_nntp && !read_local_newsgroups_file)
			fprintf (fp_save, "%s\n", buf);

		for (p = buf, q = group ; *p && *p != ' ' && *p != '\t' ; p++, q++)
			*q = *p;

		*q = '\0';

		while (*p == '\t' || *p == ' ')
			p++;

		psGrp = psGrpFind (group);

		if (psGrp != (struct t_group *) 0 && psGrp->description == (char *) 0) {
			q = p;
			while ((q = strchr (q, '\t')) != (char *) 0)
				*q = ' ';

			psGrp->description = my_strdup (p);
#if 0 /* not usefull for cache_overview_files */
			if (psGrp->type == GROUP_TYPE_NEWS) {
				if (fp_save != (FILE *) 0 &&
						read_news_via_nntp &&
						!read_local_newsgroups_file) {
					fprintf (fp_save, "%s\n", buf);
				}
			}
#endif /* 0 */
		}
#ifdef SHOW_PROGRESS
		if (++count % 100 == 0)
			spin_cursor ();
#endif
	}
#endif	/* INDEX_DAEMON */
}

void
vPrintActiveHead (
	char	*pcActiveFile)
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
vFindArtMaxMin (
	char	*pcGrpPath,
	long	*plArtMax,
	long	*plArtMin)
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
			lArtNum = atol (tFile->d_name/*, (int) D_NAMLEN(tFile)*/); /* should be '\0' terminated ... */
			if (lArtNum >= 1) {
				if (lArtNum > *plArtMax) {
					*plArtMax = lArtNum;
					if (*plArtMin == 0)
						*plArtMin = lArtNum;
				} else if (lArtNum < *plArtMin) {
					*plArtMin = lArtNum;
				}
			}
		}
		closedir (tDirFile);
	}
	if (*plArtMin == 0)
		*plArtMin = 1;
}

void
vPrintGrpLine (
	FILE	*hFp,
	char	*pcGrpName,
	long	lArtMax,
	long	lArtMin,
	char	*pcBaseDir)
{
	fprintf (hFp, "%s %05ld %05ld %s\n",
		pcGrpName, lArtMax, lArtMin, pcBaseDir);
}


/*
 * Given a base pathname & a newsgroup name build an absolute pathname.
 * base = /usr/spool/news
 * newsgroup = alt.sources
 * absolute path = /usr/spool/news/alt/sources
 */

void
vMakeGrpPath (
	char	*pcBaseDir,
	char	*pcGrpName,
	char	*pcGrpPath)
{
	char	*pcPtr;

	joinpath (pcGrpPath, pcBaseDir, pcGrpName);

	pcPtr = pcGrpPath + strlen (pcBaseDir);
	while ((pcPtr = strchr (pcPtr, '.')) != (char *) 0)
		*pcPtr = '/';
}

/*
 * Given an absolute pathname & a base pathname build a newsgroup name
 * base = /usr/spool/news
 * absolute path = /usr/spool/news/alt/sources
 * newsgroup = alt.sources
 */

void
vMakeGrpName (
	char	*pcBaseDir,
	char	*pcGrpName,
	char	*pcGrpPath)
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
	while ((pcPtrName = strchr (pcPtrName, '/')) != (char *) 0)
		*pcPtrName = '.';
}


#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
void
vGrpDelMailArt (
	struct t_article *psArt)
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
vGrpDelMailArts (
	struct t_group *psGrp)
{
	char	acArtFile[PATH_LEN];
	char	acGrpPath[PATH_LEN];
	int		iNum;
	int		iUpdateIndexFile = FALSE;
	struct	t_article *psArt;

	if (psGrp->type == GROUP_TYPE_MAIL) {
		wait_message (1, "Processing mail messages marked for deletion");

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
 * the active[] entry for the group and rewrite the mail.active file
 */
		if (iUpdateIndexFile)
			vWriteNovFile (psGrp);
	}
}
#endif	/* !INDEX_DAEMON */


int
iArtEdit (
	struct t_group *psGrp,
	struct t_article *psArt)
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
