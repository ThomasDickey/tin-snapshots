/*
 *  Project   : tin - a Usenet reader
 *  Module    : newsrc.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 13-12-94
 *  Notes     : ArtCount = (ArtMax - ArtMin) + 1  [could have holes]
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

#define BITS_TO_BYTES(n)	(size_t)((n+NBITS-1)/NBITS)

static int newsrc_mode = 0;

/*
** Local prototypes
*/
static char *getaline (FILE *fp);
static char *pcParseNewsrcLine (char *line, char *grp, int *sub);
static void print_bitmap_seq (FILE *fp, struct t_group *group);
static char *pcParseSubSeq (struct t_group *psGrp, char *pcSeq, long *plLow, long *plHigh, int *piSum);
static char *pcParseGetSeq (char *pcSeq, long *plLow, long *plHigh);
static void parse_bitmap_seq (struct t_group *group, char *seq);
static void create_newsrc (char *newsrc_file);
static void auto_subscribe_groups (char *newsrc_file);

/*
 *  Read $HOME/.newsrc into my_group[]. my_group[] ints point to
 *  active[] entries.
 *  If allgroups is set, then my_group[] is completely overwritten,
 *  otherwise, groups are appended
 */
void
read_newsrc (
	char *newsrc_file,
	int allgroups)
{
	char *line;
	char *seq;
	char grp[HEADER_LEN];
	FILE *fp;
	int sub;
	int i;
	struct stat buf;

	if (allgroups)
		group_top = 0;

	/*
	 * make a .newsrc if one doesn't exist & auto subscribe to set groups
	 */
	if (!stat_file (newsrc_file)) {
		create_newsrc (newsrc_file);
		auto_subscribe_groups (newsrc_file);
	}

	if (stat (newsrc_file, &buf) == 0) {
		newsrc_mode = buf.st_mode;
	}

	if ((fp = fopen (newsrc_file, "r")) != (FILE *) 0) {
		if (SHOW_UPDATE)
			wait_message (txt_reading_newsrc);

		while ((line = getaline (fp)) != (char *) 0) {
			seq = pcParseNewsrcLine (line, grp, &sub);

			if (sub == SUBSCRIBED) {
/* TODO used to be my_group_find() if command line groups supplied.
 *      Since cmd line groups are completely broken, so what !
 */
				if ((i = my_group_add (grp)) >= 0) {
					active[my_group[i]].subscribed = SUB_BOOL(sub);
					parse_bitmap_seq (&active[my_group[i]], seq);
				} else {
/* TODO - create dummy group and mark as Deleteable ? */
					fprintf(stderr, "\nBogus %s in .newsrc, not in active", grp);
				}
			}
			free (line);
		}
		fclose (fp);
		if (cmd_line) {
			my_fputc ('\n', stdout);
			fflush (stdout);
		}
	}
}


static void
vWriteNewsrcLine (
	FILE *fp,
	char *line)
{
	char grp[HEADER_LEN];
	char *seq;
	int sub;
	struct t_group *psGrp;

	seq = pcParseNewsrcLine (line, grp, &sub);
	if (grp[0] == 0 || sub == 0) {		/* Insurance against blank line */
		return;
	}
	psGrp = psGrpFind (grp);

 	if ((psGrp && psGrp->newsrc.present) && (psGrp->subscribed || !strip_newsrc)) {
		fprintf (fp, "%s%c ", psGrp->name, SUB_CHAR(psGrp->subscribed));
		print_bitmap_seq (fp, psGrp);
	} else {
 		if (WRITE_NEWSRC(sub))
 			fprintf (fp, "%s%c %s\n", grp, sub, seq);
	}
}

/*
 * Read in the users newsrc file and update the line with sessions changes
 */

void
vWriteNewsrc (void)
{
#ifndef INDEX_DAEMON
	char *line;
	FILE *fp_ip;
	FILE *fp_op;
	int rename_ok = FALSE;

	if ((fp_ip = fopen (newsrc, "r")) != (FILE *) 0) {
#ifdef VMS
		if ((fp_op = fopen (newnewsrc, "w", "fop=cif")) != (FILE *) 0) {
#else
		if ((fp_op = fopen (newnewsrc, "w")) != (FILE *) 0) {
#endif
			if (newsrc_mode) {
				chmod (newnewsrc, newsrc_mode);
			}
			while ((line = getaline (fp_ip)) != (char *) 0) {
				vWriteNewsrcLine(fp_op,line);
				free (line);
			}
			/*
			 * Don't rename if either fclose() fails or ferror() is set
			 */
			if (ferror (fp_op) | fclose (fp_op)) {
				error_message (txt_filesystem_full, NEWSRC_FILE);
				unlink (newnewsrc);
			} else {
				rename_ok = TRUE;
			}
		}
		fclose (fp_ip);
		if (rename_ok) {
			rename_file (newnewsrc, newsrc);
		}
	}
#endif	/* INDEX_DAEMON */
}

/*
 *  Create a newsrc from active[] groups. Subscribe to all groups.
 */

static void
create_newsrc (
	char *newsrc_file)
{
	FILE *fp;
	register int i;

	if ((fp = fopen (newsrc_file, "w")) != (FILE *) 0) {
		wait_message (txt_creating_newsrc);

		for (i=0 ; i < num_active ; i++) {
			fprintf (fp, "%s!\n", active[i].name);
		}

		if (ferror (fp) | fclose (fp)) {
			error_message (txt_filesystem_full, NEWSRC_FILE);
		}
	}
}

/*
 * Automatically subscribe user to newsgroups specified in
 * NEWSLIBDIR/subscribe (locally) or same file but from NNTP
 * server (LIST AUTOSUBSCRIBE) and create .newsrc
 */

static void
auto_subscribe_groups (
	char *newsrc_file)
{
	char *ptr, buf[HEADER_LEN];
	FILE *fp_newsrc;
	FILE *fp_subs;
	struct t_group *psGrp;

	/*
	 * If subscription file exists then first unsubscribe to all groups
	 * and then subscribe to just the auto specified groups.
	 */
	if ((fp_subs = open_subscription_fp ()) != (FILE *) 0) {
		wait_message (txt_autosubscribing_groups);

#ifdef VMS
		if ((fp_newsrc = fopen (newsrc_file, "w", "fop=cif")) != (FILE *) 0) {
#else
		if ((fp_newsrc = fopen (newsrc_file, "w")) != (FILE *) 0) {
#endif
			if (newsrc_mode) {
				chmod (newsrc_file, newsrc_mode);
			}
			while (fgets (buf, sizeof (buf), fp_subs) != (char *) 0) {
				if (buf[0] != '#' && buf[0] != '\n') {
					ptr = strrchr (buf, '\n');
					if (ptr != (char *) 0) {
						*ptr = '\0';
					}
					psGrp = psGrpFind (buf);
					if (psGrp) {
						fprintf (fp_newsrc, "%s:\n", buf);
					}
				}
			}
			if (ferror (fp_newsrc) | fclose (fp_newsrc)) {
				error_message (txt_filesystem_full, NEWSRC_FILE);
			}
		}
		fclose (fp_subs);
	}
}

/*
 * make a backup of users .newsrc in case of the bogie man
 */

void
backup_newsrc (void)
{
#ifndef INDEX_DAEMON
	char *line;
	char buf[HEADER_LEN];
	FILE *fp_ip, *fp_op;

	if ((fp_ip = fopen (newsrc, "r")) != (FILE *) 0) {
#if defined(WIN32)
		joinpath (buf, rcdir, OLDNEWSRC_FILE);
#else
		joinpath (buf, homedir, OLDNEWSRC_FILE);
#endif
		unlink (buf);	/* because rn makes a link of .newsrc -> .oldnewsrc */
#ifdef VMS
		if ((fp_op = fopen (buf, "w", "fop=cif")) != (FILE *) 0) {
#else
		if ((fp_op = fopen (buf, "w")) != (FILE *) 0) {
#endif
			if (newsrc_mode) {
				chmod (buf, newsrc_mode);
			}
			while ((line = getaline (fp_ip)) != (char *) 0) {
				fprintf (fp_op, "%s\n", line);
				free (line);
			}
			if (ferror (fp_op) | fclose (fp_op)) {
				error_message (txt_filesystem_full_backup, NEWSRC_FILE);
			}
		}
		fclose (fp_ip);
	}
#endif /* INDEX_DAEMON */
}

/*
 *  Subscribe/unsubscribe to a group in .newsrc.
 */

void
subscribe (
	struct t_group *group,
	int sub_state)
{
	char *line;
	char *seq;
	char grp[HEADER_LEN];
	FILE *fp;
	FILE *newfp;
	int found = FALSE;
	int sub;

#ifdef VMS
	if ((newfp = fopen (newnewsrc, "w", "fop=cif")) != (FILE *) 0) {
#else
	if ((newfp = fopen (newnewsrc, "w")) != (FILE *) 0) {
#endif
		if (newsrc_mode) {
			chmod (newnewsrc, newsrc_mode);
		}
		if ((fp = fopen (newsrc, "r")) != (FILE *) 0) {
			while ((line = getaline (fp)) != (char *) 0) {
				if (STRNCMPEQ("options ", line, 8)) {
					fprintf (newfp, "%s\n", line);
				} else {
					seq = pcParseNewsrcLine (line, grp, &sub);

					if (STRCMPEQ(grp, group->name)) {

 						if (WRITE_NEWSRC(sub_state))
 							fprintf (newfp, "%s%c %s\n", grp, sub_state, seq);

						group->subscribed = SUB_BOOL(sub_state);
						found = TRUE;
					} else {
 						if (WRITE_NEWSRC(sub))
 							fprintf (newfp, "%s%c %s\n", grp, sub, seq);
					}
				}
				free (line);
			}
			fclose (fp);
			if (!found) {
 				if (WRITE_NEWSRC(sub_state))
 					fprintf (newfp, "%s%c\n", group->name, sub_state);
				group->subscribed = SUB_BOOL(sub_state);
			}
		}
		if (ferror (newfp) | fclose (newfp)) {
			error_message (txt_filesystem_full, NEWSRC_FILE);
			unlink (newnewsrc);
		} else {
			rename_file (newnewsrc, newsrc);
		}
	}
}


void
reset_newsrc (void)
{
	char *line;
	char grp[HEADER_LEN];
	FILE *fp;
	FILE *newfp;
	int sub;
	long i;

#ifdef VMS
	if ((newfp = fopen (newnewsrc, "w", "fop=cif")) != (FILE *) 0) {
#else
	if ((newfp = fopen (newnewsrc, "w")) != (FILE *) 0) {
#endif
		if (newsrc_mode) {
			chmod (newnewsrc, newsrc_mode);
		}
		if ((fp = fopen (newsrc, "r")) != (FILE *) 0) {
			while ((line = getaline (fp)) != (char *) 0) {
				(void) pcParseNewsrcLine (line, grp, &sub);
				fprintf (newfp, "%s%c\n", grp, sub);
				free (line);
			}
			fclose (fp);
		}
		if (ferror (newfp) | fclose (newfp)) {
			error_message (txt_filesystem_full, NEWSRC_FILE);
			unlink (newnewsrc);
		} else {
			rename_file (newnewsrc, newsrc);
		}
	}

	for (i = 0; i < group_top; i++) {
		vSetDefaultBitmap (&active[my_group[i]]);
	}
}

#if 0 /* never used */
static void
delete_group (
	char *group)
{
	char *line;
	char *seq;
	char grp[HEADER_LEN];
	FILE *fp;
	FILE *newfp;
	int sub;

#ifdef VMS
	if ((newfp = fopen (newnewsrc, "w", "fop=cif")) != (FILE *) 0) {
#else
	if ((newfp = fopen (newnewsrc, "w")) != (FILE *) 0) {
#endif
		if (newsrc_mode) {
			chmod (newnewsrc, newsrc_mode);
		}
		if ((fp = fopen (newsrc, "r")) != (FILE *) 0) {
			while ((line = getaline (fp)) != (char *) 0) {
				seq = pcParseNewsrcLine (line, grp, &sub);

				if (!STRCMPEQ(grp, group)) {
					fprintf (newfp, "%s%c %s\n", grp, sub, seq);
				}
				free (line);
			}
			fclose (fp);
		}
		if (ferror (newfp) | fclose (newfp)) {
			error_message (txt_filesystem_full, NEWSRC_FILE);
			unlink (newnewsrc);
		} else {
			rename_file (newnewsrc, newsrc);
		}
	}
}
#endif /* 0 */

void
grp_mark_read (
	struct t_group *group,
	struct t_article *psArt)
{
	register int i;

#ifdef DEBUG_NEWSRC
	debug_print_comment ("c/C command");
#endif

	if (psArt != (struct t_article *) 0) {
		for (i = 0; i < top; i++) {
			art_mark_read (group, &psArt[i]);
		}
	}
	if (group->newsrc.xbitmap != (t_bitmap *) 0) {
		free ((char *) group->newsrc.xbitmap);
		group->newsrc.xbitmap = (t_bitmap *) 0;
	}

	group->newsrc.xbitlen = 0;
	group->newsrc.xmax = group->xmax;
	group->newsrc.xmin = group->xmax+1;
	group->newsrc.num_unread = 0;
}


void
grp_mark_unread (
	struct t_group *group)
{
	int bitlength;
	t_bitmap *newbitmap = (t_bitmap *)0;

#ifdef DEBUG_NEWSRC
	debug_print_comment ("Z command");
#endif

/* TODO - this is bogus - test return code */
	vGrpGetArtInfo (
		group->spooldir,
		group->name,
		group->type,
		&group->count,
		&group->xmax,
		&group->xmin);

	bitlength = (group->xmax - group->xmin) + 1;
	if (bitlength < 0) bitlength = 0;
	if (bitlength > 0) {
		newbitmap = (t_bitmap *) my_malloc (BITS_TO_BYTES(bitlength));
	}
	if (group->newsrc.xbitmap != (t_bitmap *) 0) {
		free ((char *) group->newsrc.xbitmap);
	}
	group->newsrc.xbitmap = newbitmap;
	group->newsrc.xbitlen = bitlength;
	group->newsrc.num_unread = group->count;
	group->newsrc.xmax = group->xmax;
	group->newsrc.xmin = group->xmin;

	if (bitlength) NSETRNG1(group->newsrc.xbitmap, 0, bitlength - 1);

#ifdef DEBUG_NEWSRC
	debug_print_bitmap (group, NULL);
#endif
}


void
thd_mark_read (
	struct t_group *group,
	long thread)
{
	register int i;

#ifdef DEBUG_NEWSRC
	debug_print_comment ("Mark thread read K command");
#endif

	for (i = (int) thread; i >= 0; i = arts[i].thread) {
		art_mark_read (group, &arts[i]);
	}
}


void
thd_mark_unread (
	struct t_group *group,
	long thread)
{
	register int i;

#ifdef DEBUG_NEWSRC
	debug_print_comment ("Mark thread unread Z command");
#endif

	for (i = (int) thread; i >= 0; i = arts[i].thread) {
		art_mark_will_return (group, &arts[i]); /* art_mark_unread (group, &arts[i]); */
	}
}

/*
 * Parse the newsrc sequence for the specified group
 */

static void
parse_bitmap_seq (
	struct t_group *group,
	char *seq)
{
	char *ptr;
	int sum = 0;
	int gotseq = FALSE;
	long low = 0;
	long high = 0;
	long min, max;

	/*
	 * Skip possible non-numeric prefix
	 */
	ptr = seq;
	while (*ptr && (*ptr < '0' || *ptr > '9')) {
		ptr++;
	}
#ifdef DEBUG_NEWSRC
	{
	char buf[NEWSRC_LINE];
	sprintf (buf, "Parsing [%s%c %.*s]", group->name, group->subscribed, (int) (NEWSRC_LINE-strlen(group->name)-20), ptr);
	debug_print_comment(buf);
	debug_print_bitmap(group,NULL);
	}
#endif
	if (*ptr) {
		gotseq = TRUE;
		ptr = pcParseGetSeq (ptr, &low, &high);

		if (high < group->xmin - 1) {
			high = group->xmin - 1;
		}

		if (high > group->xmax) {
			high = group->xmax;
		}

		if (low <= 1) {
			min = high + 1;
		} else {
			min = 1;
		}

		if (group->xmin > min) {
			min = group->xmin;
		}

		max = group->xmax;

		if (group->newsrc.xbitmap != (t_bitmap *) 0) {
			free ((char *) group->newsrc.xbitmap);
			group->newsrc.xbitmap = (t_bitmap *) 0;
		}

		group->newsrc.xmax = max;
		group->newsrc.xmin = min;
		group->newsrc.xbitlen = (max - min) + 1;
		if (group->newsrc.xbitlen > 0) {
			group->newsrc.xbitmap =
				(t_bitmap *) my_malloc (BITS_TO_BYTES(group->newsrc.xbitlen));
			NSETRNG1(group->newsrc.xbitmap, 0, group->newsrc.xbitlen - 1);
		}

		if (min <= high) {
			if (low > min) {
				sum = low - min;
			} else {
				low = min;
			}
			NSETRNG0(group->newsrc.xbitmap, low - min, high - min);
		}

		/*
		 * Pick up any additional articles/ranges after the first
		 */
		while (*ptr) {
			ptr = pcParseSubSeq (group, ptr, &low, &high, &sum);
		}
	} else {
		if (group->newsrc.xbitmap != (t_bitmap *) 0) {
			free ((char *) group->newsrc.xbitmap);
			group->newsrc.xbitmap = (t_bitmap *) 0;
		}

		group->newsrc.xmax = group->xmax;
		group->newsrc.xmin = group->xmin;
		group->newsrc.xbitlen = (group->xmax - group->xmin) + 1;
		if (group->newsrc.xbitlen > 0) {
			group->newsrc.xbitmap =
				(t_bitmap *) my_malloc (BITS_TO_BYTES(group->newsrc.xbitlen));
			NSETRNG1(group->newsrc.xbitmap, 0, group->newsrc.xbitlen - 1);
		}
/*
sprintf (msg, "BITMAP Grp=[%s] MinMax=[%ld-%ld] Len=[%ld]\n",
	group->name, group->xmin, group->xmax, group->newsrc.xbitlen);
wait_message (msg);
*/
	}

	group->newsrc.present = TRUE;

	if (gotseq) {
		if (group->xmax > high) {
			sum += group->xmax - high;
		}
	} else {
		if (group->count >= 0) {
			sum = (int) group->count;
		} else {
			sum = (int) (group->xmax - group->xmin) + 1;
		}
	}

	group->newsrc.num_unread = sum;
#ifdef DEBUG_NEWSRC
	debug_print_bitmap(group, NULL);
#endif
}

/*
 * Parse a subsection of the newsrc sequencer ie., 1-34,23-90,93,97-99
 * would parse the sequence if called in a loop in the following way:
 *   1st call would parse  1-34 and return 23-90,93,97-99
 *   2nd call would parse 23-90 and return 93,97-99
 *   3rd call would parse    93 and return 97-99
 *   4th call would parse 97-99 and return NULL
 */

static char *
pcParseSubSeq (
	struct	t_group *psGrp,
	char	*pcSeq,
	long	*plLow,
	long	*plHigh,
	int	*piSum)
{
	long	lBitMin;
	long	lBitMax;
	long	lLastHigh;

/*
#ifdef DEBUG_NEWSRC
	printf ("Seq=[%s]\n", pcSeq);
#endif
*/

	lLastHigh = *plHigh;
	pcSeq = pcParseGetSeq(pcSeq, plLow, plHigh);

	/*
	 * Bitmap index
	 */
	lBitMin = *plLow - psGrp->newsrc.xmin;

	/*
	 * check that seq is not out of order
	 */
	if (*plLow > lLastHigh) {
		*piSum += (*plLow - lLastHigh) - 1;
	}

	if (*plHigh == *plLow) {
		if (lBitMin >= 0 && *plHigh <= psGrp->newsrc.xmax) {
			NSET0(psGrp->newsrc.xbitmap, lBitMin);
		}
	} else if ((*plLow < *plHigh) &&
	           (*plLow <= psGrp->newsrc.xmax) &&
	           (*plHigh >= psGrp->newsrc.xmin)) {
		/*
		 * Restrict the range to min..max
		 */
		if (lBitMin < 0) {
			lBitMin = 0;
		}
		lBitMax = *plHigh;
		if (lBitMax > psGrp->newsrc.xmax) {
			lBitMax = psGrp->newsrc.xmax;
		}
		lBitMax = lBitMax - psGrp->newsrc.xmin;

		/*
		 * Fill in the whole range as read
		 */
		NSETRNG0(psGrp->newsrc.xbitmap, lBitMin, lBitMax);
	}

	return pcSeq;
}


static char *
pcParseGetSeq(
	char *pcSeq,
	long *plLow,
	long *plHigh)
{
	*plLow = strtol (pcSeq, &pcSeq, 10);

	if (*pcSeq == '-') {
		/*
		 * Range of articles
		 */
		pcSeq++;
		*plHigh = strtol (pcSeq, &pcSeq, 10);
	} else {
		/*
		 * Single article
		 */
		*plHigh = *plLow;
	}

	while (*pcSeq && (*pcSeq < '0' || *pcSeq > '9')) {
		pcSeq++;
	}
/*
#ifdef DEBUG_NEWSRC
	printf ("Min=[%ld] Max=[%ld] Seq=[%s]\n", *plLow, *plHigh, pcSeq);
#endif
*/
	return pcSeq;
}

/*
 * Loop thru arts[] array marking state of each article READ/UNREAD
 */

void
parse_unread_arts (
	struct t_group *group)
{
	long	bitmin, bitmax;
	register int i;
	t_bitmap *newbitmap = (t_bitmap *)0;
	long	unread = 0;

	bitmin = group->newsrc.xmin;
	bitmax = group->newsrc.xmax;
	if (group->xmax >= bitmin) {
		newbitmap = (t_bitmap *)my_malloc(BITS_TO_BYTES(group->xmax-bitmin+1));
		NSETRNG0(newbitmap, 0, group->xmax - bitmin);
	}
	for (i = 0; i < top; i++) {
		if (arts[i].artnum < bitmin) arts[i].status = ART_READ;
		else if (arts[i].artnum > bitmax) arts[i].status = ART_UNREAD;
		else if (NTEST(group->newsrc.xbitmap, arts[i].artnum - bitmin) == ART_READ) {
			arts[i].status = ART_READ;
		} else {
			arts[i].status = ART_UNREAD;
		}
		if (arts[i].status == ART_UNREAD && arts[i].artnum >= bitmin) {
			NSET1(newbitmap, arts[i].artnum - bitmin);
			unread++;
		}
	}
	group->newsrc.xmin = bitmin;
	group->newsrc.xmax = group->xmax;
	group->newsrc.xbitlen = group->xmax - bitmin + 1;
	if (group->newsrc.xbitmap != (t_bitmap *) 0) {
		free(group->newsrc.xbitmap);
	}
	group->newsrc.xbitmap = newbitmap;
	group->newsrc.num_unread = unread;
}

static void
print_bitmap_seq (
	FILE *fp,
	struct t_group *group)
{
	int flag = FALSE;
	long artnum;
	long i, last;

#ifdef DEBUG_NEWSRC
	debug_print_comment ("print_bitmap_seq()");
	debug_print_bitmap (group, NULL);
#endif

	if (group->count == 0 || group->xmin > group->xmax) {
/*		fprintf (fp, "1"); */
		if (group->xmax > 1) {
			fprintf (fp, "1-%ld", group->xmax);
		}
		fprintf (fp, "\n");
		fflush (fp);
#ifdef DEBUG_NEWSRC
		debug_print_comment ("print_bitmap_seq(): group->count == 0");
#endif
		return;
	}

	for (i = group->newsrc.xmin ; i <= group->newsrc.xmax ; i++) {
 		if (NTEST(group->newsrc.xbitmap, i - group->newsrc.xmin) == ART_READ) {
  			if (flag) {
				artnum = i;
  				fprintf (fp, ",%ld", i);
			} else {
				artnum = 1;
  				flag = TRUE;
				fprintf (fp, "1");
			}
			i++;
			while (i <= group->xmax && NTEST(group->newsrc.xbitmap, i - group->newsrc.xmin) == ART_READ) {
				i++;
			}
			last = i - 1;
			if (artnum != last) {
				fprintf (fp, "-%ld", last);
			}
		} else if (flag == FALSE && group->xmax) {
			flag = TRUE;
			if (group->newsrc.xmin > 1) {
				fprintf (fp, "1");
				if (group->newsrc.xmin > 2) {
					fprintf (fp, "-%ld", group->newsrc.xmin - 1);
				}
			}
		}
	}
	if (flag == FALSE && group->newsrc.xmin) {
		fprintf (fp, "1");
		if (group->newsrc.xmin > 2) {
			fprintf (fp, "-%ld", group->newsrc.xmin - 1);
		}
#ifdef DEBUG_NEWSRC
		debug_print_comment ("print_bitmap_seq(): flag == FALSE && group->max > 0");
#endif
	}

	fprintf (fp, "\n");
  	fflush (fp);
}


/*
 *  rewrite .newsrc and position group at specifed position
 */

int
pos_group_in_newsrc (
	struct t_group *group,
	int pos)
{
	char buf[HEADER_LEN];
	char sub[HEADER_LEN];
	char unsub[HEADER_LEN];
	char *newsgroup = NULL;
	char *line;
	FILE *fp_in = NULL, *fp_out = NULL;
	FILE *fp_sub = NULL, *fp_unsub = NULL;
	int repositioned = FALSE;
	int subscribed_pos = 1;
	size_t group_len;
	int option_line = FALSE;
	int ret_code = FALSE;
	int found = FALSE;
	int newnewsrc_created = FALSE;
	int sub_created = FALSE, unsub_created = FALSE;

	if ((fp_in = fopen (newsrc, "r")) == (FILE *) 0) {
		goto rewrite_group_done;
	}
#ifdef VMS
	if ((fp_out = fopen (newnewsrc, "w", "fop=cif")) == (FILE *) 0) {
#else
	if ((fp_out = fopen (newnewsrc, "w")) == (FILE *) 0) {
#endif
		goto rewrite_group_done;
	}
	newnewsrc_created = TRUE;
	if (newsrc_mode) {
		chmod (newnewsrc, newsrc_mode);
	}

#ifdef VMS
	joinpath (buf, TMPDIR, "subrc");
	sprintf (sub, "%s.%d", buf, process_id);

	joinpath (buf, TMPDIR, "unsubrc");
	sprintf (unsub, "%s.%d", buf, process_id);

	if ((fp_sub = fopen (sub, "w", "fop=cif")) == (FILE *) 0) {
		goto rewrite_group_done;
	}
	if ((fp_unsub = fopen (unsub, "w", "fop=cif")) == (FILE *) 0) {
		fclose(fp_sub);
		unlink(sub);
		goto rewrite_group_done;
	}
#else /* !VMS */
	joinpath (buf, TMPDIR, ".subrc");
	sprintf (sub, "%s.%d", buf, process_id);

	joinpath (buf, TMPDIR, ".unsubrc");
	sprintf (unsub, "%s.%d", buf, process_id);

	if ((fp_sub = fopen (sub, "w")) == (FILE *) 0) {
		goto rewrite_group_done;
	}
	sub_created = TRUE;
	if ((fp_unsub = fopen (unsub, "w")) == (FILE *) 0) {
		goto rewrite_group_done;
	}
	unsub_created = TRUE;

	/*
	 *  split newsrc into subscribed and unsubscribed to files
	 */
	group_len = strlen (group->name);

	while ((line = getaline(fp_in)) != (char *) 0) {
		if (STRNCMPEQ(group->name, line, group_len) && line[group_len] == SUBSCRIBED) {
			newsgroup = line;
			found = TRUE;
			continue;	/* Don't free line */
		} else if (strchr (line, SUBSCRIBED) != (char *) 0) {
			vWriteNewsrcLine(fp_sub,line);
		} else if (strchr (line, UNSUBSCRIBED) != (char *) 0) {
			vWriteNewsrcLine(fp_unsub,line);
		} else {	/* options line at beginning of .newsrc */
			fprintf (fp_sub, "%s\n", line);
			option_line = TRUE;
		}
		free(line);
	}

	if (ferror (fp_sub) | fclose (fp_sub)
		| ferror (fp_unsub) | fclose (fp_unsub)) {

		error_message (txt_filesystem_full, NEWSRC_FILE);
		fp_sub = fp_unsub = NULL;
		goto rewrite_group_done;
	}
	fp_sub = fp_unsub = NULL;
	fclose (fp_in);
	fp_in = NULL;

	if (!found) {
		goto rewrite_group_done;
	}
#endif /* !VMS */

	/*
	 *  write subscribed groups & position group to newnewsrc
	 */
	if ((fp_sub = fopen (sub, "r")) == (FILE *) 0) {
		goto rewrite_group_done;
	}
	while ((line = getaline(fp_sub)) != (char *) 0) {
		if (option_line) {
			if (strchr (line, ':') == (char *) 0 &&
			    strchr (line, '!') == (char *) 0) {
				fprintf (fp_out, "%s\n", line);
				free (line);
				continue;
			} else {
				option_line = FALSE;
			}
		}

		if (pos == subscribed_pos) {
			vWriteNewsrcLine(fp_out,newsgroup);
			repositioned = TRUE;
		}

		fprintf (fp_out, "%s\n", line);
		free(line);

		subscribed_pos++;
	}
	if (!repositioned) {
		vWriteNewsrcLine(fp_out,newsgroup);
		repositioned = TRUE;
	}

	/*
	 *  write unsubscribed groups to newnewsrc
	 */
	if ((fp_unsub = fopen (unsub, "r")) == (FILE *) 0) {
		goto rewrite_group_done;
	}
	while ((line = getaline(fp_unsub)) != (char *) 0) {
		fprintf (fp_out, "%s\n", line);
		free (line);
	}

	if (ferror (fp_out) | fclose (fp_out)) {
		error_message (txt_filesystem_full, NEWSRC_FILE);
	} else {
		if (repositioned) {
			cur_groupnum = pos;
			rename_file (newnewsrc, newsrc);
			ret_code = TRUE;
		}
	}
	fp_out = NULL;
	newnewsrc_created = FALSE;

rewrite_group_done:
	if (fp_out != (FILE *) 0) {
	  fclose (fp_out);
	}
	if (fp_sub != (FILE *) 0) {
	  fclose (fp_sub);
	}
	if (fp_unsub != (FILE *) 0) {
	  fclose (fp_unsub);
	}
	if (newnewsrc_created) {
		unlink (newnewsrc);
	}
	if (sub_created) {
		unlink (sub);
	}
	if (unsub_created) {
		unlink (unsub);
	}

	FreeIfNeeded(newsgroup);

	return ret_code;
}

/*
 *  catchup all groups in .newsrc
 */

void
catchup_newsrc_file (
	char *newsrc_file)
{
	FILE *fp;
	register int i;

	if (catchup && !update) {
		if ((fp = fopen (newsrc_file, "w")) != (FILE *) 0) {
			if (newsrc_mode) {
				chmod (newsrc_file, newsrc_mode);
			}
			for (i = 0 ; i < group_top ; i++) {

 				if (active[my_group[i]].subscribed || !strip_newsrc) {
 					fprintf (fp, "%s%c 1-%ld\n",
 						active[my_group[i]].name,
						SUB_CHAR(active[my_group[i]].subscribed),
 						active[my_group[i]].xmax);
 				}
			}
			fclose (fp);
		}
		tin_done (EXIT_OK);
	}
}

/*
 * Break down a line of .newsrc file
 * The sequence information [ eg; 1-3,10,12 ] is returned
 */
static char *
pcParseNewsrcLine (
	char *line,
	char *grp,
	int *sub)
{
	char *grpptr = grp;
	static char *ptr;

	ptr = line;

	while (*ptr && *ptr != ' ' && *ptr != SUBSCRIBED && *ptr != UNSUBSCRIBED) {
			*grpptr = *ptr;
			grpptr++;
			ptr++;
	}
	*grpptr = '\0';

	*sub = *ptr;
	if (*ptr) ptr++;

	while (*ptr && (*ptr == ' ' || *ptr == '\t')) {
			ptr++;
	}

	return ptr;
}

void
expand_bitmap (
	struct t_group *group,
	long min)
{
	long bitlen;
	long first;
	long new;

/*
** that shouldn' happen - looks like the newsservers database is broken
*/
if (group->newsrc.xmax > group->xmax) {
#ifdef DEBUG
	fprintf(stderr, "\ngroup: %s - newsrc.max %ld > read.max %ld\n", group->name, group->newsrc.xmax, group->xmax);	
	sleep(4);
#endif
/*
** (silently) fix it - we trust our newsrc
*/
	group->xmax = group->newsrc.xmax;
} 
	if (group->newsrc.xmin > group->newsrc.xmax + 1)
		group->newsrc.xmin = group->newsrc.xmax + 1;

	if (min == 0)
		first = min = group->newsrc.xmin;
	else if (min >= group->newsrc.xmin)
		first = group->newsrc.xmin;
	else
		first = group->newsrc.xmin - ((group->newsrc.xmin - min + (NBITS-1)) & ~(NBITS-1));

	bitlen = group->xmax - first + 1;

	if (bitlen <= 0) {
		bitlen = 1;
	}

	if (group->newsrc.xbitmap == (t_bitmap *) 0) {
		group->newsrc.xbitmap = (t_bitmap *)my_malloc (BITS_TO_BYTES(bitlen));
		if (group->newsrc.xmin > first)
			NSETRNG0 (group->newsrc.xbitmap, 0, group->newsrc.xmin - first - 1);
		if (bitlen > group->newsrc.xmin - first)
			NSETRNG1 (group->newsrc.xbitmap, group->newsrc.xmin - first, bitlen - 1);
#ifdef DEBUG_NEWSRC
		debug_print_comment("expand_bitmap: group->newsrc.xbitmap == NULL");
#endif
	} else if (group->newsrc.xmax != group->xmax || min != group->newsrc.xmin) {
		t_bitmap *newbitmap;
		newbitmap = (t_bitmap *)my_malloc(BITS_TO_BYTES(bitlen));

		/* Copy over old bitmap */

		assert((group->newsrc.xmin - first) / NBITS + BITS_TO_BYTES(group->newsrc.xbitlen) <= BITS_TO_BYTES(bitlen));

		memcpy(newbitmap + (group->newsrc.xmin - first) / NBITS,
			group->newsrc.xbitmap,
			BITS_TO_BYTES(group->newsrc.xbitlen));

		/* Mark earlier articles as read, updating num_unread */

		if (first < group->newsrc.xmin) {
			NSETRNG0 (newbitmap, 0, group->newsrc.xmin - first - 1);
		}
		{	long i;
			for (i = group->newsrc.xmin; i < min; i++) {
				if (NTEST(newbitmap, i - first) != ART_READ) {
					NSET0(newbitmap, i - first);
					if (group->newsrc.num_unread) {
						group->newsrc.num_unread--;
					}
				}
			}
		}

		/* Mark high numbered articles as unread */

		if (group->newsrc.xmin - first + group->newsrc.xbitlen < bitlen) {
			new = group->newsrc.xmin - first + group->newsrc.xbitlen;
			NSETRNG1(newbitmap, new, bitlen - 1);
/*
sprintf (msg, "EXPAND BY=[%ld] grp->newsrc.xmin(%ld) - first(%ld) + grp->newsrc.xbitlen(%ld) < bitlen(%ld)",
	new, group->newsrc.xmin, first, group->newsrc.xbitlen, bitlen);
error_message (msg, "");
*/
		}

		free((char *) (group->newsrc.xbitmap));
		group->newsrc.xbitmap = newbitmap;
#ifdef DEBUG_NEWSRC
		debug_print_comment("expand_bitmap: group->newsrc.bitlen != (group->max-group->min)+1");
#endif
	}
	group->newsrc.xmin = first;
	if (group->newsrc.xmax < group->xmax) {
		group->newsrc.num_unread += group->xmax - group->newsrc.xmax;
/*
sprintf (msg, "EXPAND MAX BY=[%ld] to unread=[%ld]",
	group->xmax - group->newsrc.xmax, group->newsrc.num_unread);
error_message (msg, "");
*/
	}
	group->newsrc.xmax = group->xmax;
	group->newsrc.xbitlen = bitlen;
	group->newsrc.present = TRUE;
}


void
art_mark_read (
	struct t_group *group,
	struct t_article *art)
{
	if (art != (struct t_article *) 0) {
		if (group != (struct t_group *) 0) {
			if (art->artnum >= group->newsrc.xmin && art->artnum <= group->newsrc.xmax)
				NSET0(group->newsrc.xbitmap, art->artnum - group->newsrc.xmin);
#ifdef DEBUG_NEWSRC
			debug_print_bitmap (group, art);
#endif
		}
		if ((art->status  == ART_UNREAD) || (art->status == ART_WILL_RETURN)) {
			art_mark_xref_read (art);
			if (group != (struct t_group *) 0 && group->newsrc.num_unread) {
				group->newsrc.num_unread--;
			}
			if (art->killed) {
				num_of_killed_arts++;
			}
			if (art->selected && num_of_selected_arts) {
				num_of_selected_arts--;
			}
			art->status = ART_READ;
		}
	}
}

void
art_mark_unread (
	struct t_group *group,
	struct t_article *art)
{
	if (art != (struct t_article *) 0) {
		if (art->status == ART_READ) {
			if (group != (struct t_group *) 0) {
				group->newsrc.num_unread++;
			}
			if (art->selected) {
				num_of_selected_arts++;
			}
			art->status = ART_UNREAD;
		}
		if (group != (struct t_group *) 0) {
			if (art->artnum < group->newsrc.xmin) {
				expand_bitmap (group, art->artnum);

			} else {
				NSET1(group->newsrc.xbitmap, art->artnum - group->newsrc.xmin);
#ifdef DEBUG_NEWSRC
				debug_print_bitmap (group, art);
#endif
			}
		}
	}
}

/* make a funktion art_mark(group, art, FLAG)
 * could be uses for art_mark_will_return(),
 * art_mark_unread(),...
 */

void
art_mark_will_return (
	struct t_group *group,
	struct t_article *art)
{
	if (art != (struct t_article *) 0) {
		if (art->status == ART_READ) {
			if (group != (struct t_group *) 0) {
				group->newsrc.num_unread++;
			}
			if (art->selected) {
				num_of_selected_arts++;
			}
			art->status = ART_WILL_RETURN;
		}
		if (group != (struct t_group *) 0) {
			NSET1(group->newsrc.xbitmap, art->artnum - group->newsrc.xmin);
#ifdef DEBUG_NEWSRC
			debug_print_bitmap (group, art);
#endif
		}
	}
}

#if !defined(INDEX_DAEMON) && defined(HAVE_MH_MAIL_HANDLING)
void
art_mark_deleted (
	struct t_article *art)
{
	if (art != (struct t_article *) 0) {
		art->delete = TRUE;
wait_message("FIXME  article marked for deletion");
	}
}


void
art_mark_undeleted (
	struct t_article *art)
{
	if (art != (struct t_article *) 0) {
		art->delete = FALSE;
wait_message("FIXME  article marked for undeletion");
	}
}
#endif /* !INDEX_DAEMON && HAVE_MH_MAIL_HANDLING */

void
vSetDefaultBitmap (
	struct t_group *group)
{
	if (group != (struct t_group *) 0) {
		group->newsrc.num_unread = 0;
		group->newsrc.present = FALSE;
		if (group->newsrc.xbitmap != (t_bitmap *) 0) {
			free ((char *) (group->newsrc.xbitmap));
		}
		group->newsrc.xbitmap = (t_bitmap *) 0;
		group->newsrc.xbitlen = 0;
		group->newsrc.xmin = group->xmin;
		group->newsrc.xmax = group->xmin-1;
	}
}

static char *
getaline(
	FILE *fp)
{
	char *res=safe_fgets(fp);

	if(res && strlen(res) && res[strlen(res)-1]=='\n')
		res[strlen(res)-1]='\0';
	return res;
}

/* TEST harness */
#ifdef DEBUG_NEWSRC

#ifdef DEBUG_NEWSRC_FIXME	/* something's broken here */
static void set_bitmap_range_read (struct t_newsrc *newsrc, long beg, long end);
static void set_bitmap_range_unread (struct t_newsrc *newsrc, long beg, long end);
#endif

void
vNewsrcTestHarness (void)
{
	char seq[20000];
	FILE *fp;
	int i;
	long rng_min, rng_max;
	struct t_group group;

	rng_min = atoi (get_val ("TIN_RNG_MIN", "1"));
	rng_max = atoi (get_val ("TIN_RNG_MAX", "1"));

	group.name = my_strdup("test.newsrc");
	group.xmin = atoi (get_val ("TIN_MIN", "1"));
	group.xmax = atoi (get_val ("TIN_MAX", "0"));
	group.count = atoi (get_val ("TIN_COUNT", "-1"));
	group.type = GROUP_TYPE_NEWS;
	group.subscribed = TRUE;
	group.newsrc.xbitmap = (t_bitmap *) 0;
	vSetDefaultBitmap (&group);

	strcpy (seq, get_val ("TIN_SEQ", ""));

	printf ("\nENV Min=[%ld] Max=[%ld] Rng=[%ld-%ld] Count=[%ld] Seq=[%s]\n",
		group.xmin, group.xmax, rng_min, rng_max, group.count, seq);

	for (i=0; i < 3 ; i++) {
		if (group.newsrc.xbitmap != (t_bitmap *) 0) {
			free ((char *) group.newsrc.xbitmap);
			group.newsrc.xbitmap = (t_bitmap *) 0;
			group.newsrc.xbitlen = 0;
			group.newsrc.num_unread = 0;
			group.newsrc.xmin = 1;
			group.newsrc.xmax = 0;
		}
		fp = fopen ("/tmp/NEWSRC", "w");
		printf ("\n%d. PARSE Seq=[%s]\n", i+1, seq);

		parse_bitmap_seq (&group, seq);
debug_print_newsrc (&group.newsrc, stdout);
		print_bitmap_seq (fp, &group);

		printf("   PRINT Seq=[");
		print_bitmap_seq (stdout, &group);

		fclose (fp);

		debug_print_newsrc (&group.newsrc, stdout);

		fp = fopen ("/tmp/NEWSRC", "r");
		fgets (seq, sizeof(seq), fp);
		seq[strlen(seq)-1] = '\0';
		fclose (fp);
	}
#ifdef DEBUG_NEWSRC_FIXME	/* something's broken here */
	set_bitmap_range_read (&group.newsrc, rng_min, rng_max);
	debug_print_newsrc (&group.newsrc, stdout);

	set_bitmap_range_unread (&group.newsrc, rng_min, rng_max);
	debug_print_newsrc (&group.newsrc, stdout);


	NSETBLK0(group.newsrc.xbitmap, group.newsrc.xbitlen);
	debug_print_newsrc (&group.newsrc, stdout);

	NSETBLK1(group.newsrc.xbitmap, group.newsrc.xbitlen);
	debug_print_newsrc (&group.newsrc, stdout);
#endif
	printf ("\n");
}


#ifdef DEBUG_NEWSRC_FIXME	/* something's broken here */
static void
set_bitmap_range_read (
	struct	t_newsrc *newsrc,
	long	beg,
	long	end)
{
	long	length, offset;

	if (beg >= newsrc->xmin && end <= newsrc->xmax) {
		offset = beg - newsrc->xmin;
		length = end - newsrc->xmin;

printf ("\nRNG Min-Max=[%ld-%ld] Beg-End=[%ld-%ld] OFF=[%ld] LEN=[%ld]\n",
newsrc->xmin, newsrc->xmax, beg, end, offset, length);

		if (beg == end) {
			NSET0(newsrc->xbitmap, offset);
		} else {
			NSETRNG0(newsrc->xbitmap, offset, length);
		}
	}
}


static void
set_bitmap_range_unread (
	struct	t_newsrc *newsrc,
	long	beg,
	long	end)
{
	long	length, offset;

	if (beg >= newsrc->xmin && end <= newsrc->xmax) {
		offset = beg - newsrc->xmin;
		length = end - newsrc->xmin;

printf ("\nRNG Min-Max=[%ld-%ld] Beg-End=[%ld-%ld] OFF=[%ld] LEN=[%ld]\n",
newsrc->xmin, newsrc->xmax, beg, end, offset, length);

		if (beg == end) {
			NSET1(newsrc->xbitmap, offset);
		} else {
			NSETRNG1(newsrc->xbitmap, offset, length);
		}
	}
}
#endif /* DEBUG_NEWSRC_FIXME */

#endif	/* DEBUG_NEWSRC */
