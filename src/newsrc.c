/*
 *  Project   : tin - a Usenet reader
 *  Module    : newsrc.c
 *  Author    : I. Lea & R. Skrenta
 *  Created   : 1991-04-01
 *  Updated   : 1997-12-28
 *  Notes     : ArtCount = (ArtMax - ArtMin) + 1  [could have holes]
 *  Copyright : (c) Copyright 1991-98 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"

#define BITS_TO_BYTES(n)	(size_t)((n+NBITS-1)/NBITS)

static mode_t newsrc_mode = 0;

/*
 * Local prototypes
 */
static char *pcParseNewsrcLine (char *line, int *sub);
static char *pcParseSubSeq (struct t_group *psGrp, char *pcSeq, long *plLow, long *plHigh, int *piSum);
static char *pcParseGetSeq (char *pcSeq, long *plLow, long *plHigh);
static int iWriteNewsrcLine (FILE *fp, char *line);
static void auto_subscribe_groups (char *newsrc_file);
static void create_newsrc (char *newsrc_file);
static void parse_bitmap_seq (struct t_group *group, char *seq);
static void print_bitmap_seq (FILE *fp, struct t_group *group);


/*
 *  Read $HOME/.newsrc into my_group[]. my_group[] ints point to
 *  active[] entries.
 *  If allgroups is set, then my_group[] is completely overwritten,
 *  otherwise, groups are appended
 *  Any bogus groups will be handled accordingly. Bogus groups will _not_
 *  be subscribed to as a design principle.
 */
void
read_newsrc (
	char *newsrc_file,
	t_bool allgroups)
{
	FILE *fp;
	char *grp, *seq;
	int sub, i;
	struct stat statbuf;

	if (allgroups)
		group_top = skip_newgroups();

	/*
	 * make a .newsrc if one doesn't exist & auto subscribe to set groups
	 */
	if (stat (newsrc_file, &statbuf) == -1) {
		create_newsrc (newsrc_file);
		auto_subscribe_groups (newsrc_file);
	} else
		newsrc_mode = (mode_t) statbuf.st_mode;

	if ((fp = fopen (newsrc_file, "r")) != (FILE *) 0) {
		if (INTERACTIVE)
			wait_message (0, txt_reading_newsrc);

		while ((grp = tin_fgets (fp, FALSE)) != (char *) 0) {
			seq = pcParseNewsrcLine (grp, &sub);

			if (sub == SUBSCRIBED) {
				if ((i = my_group_add (grp)) >= 0) {
					if (!active[my_group[i]].bogus) {
						active[my_group[i]].subscribed = SUB_BOOL(sub);
						parse_bitmap_seq (&active[my_group[i]], seq);
					}
				} else
					process_bogus(grp);
			}
		}
		fclose (fp);
		/* If you aborted with 'q', then you get what you get. */

		if (cmd_line) {
			my_fputc ('\n', stdout);
			my_flush ();
		}
	}
}


/*
 * Parse a line from the newsrc file and write it back out with updated
 * sequence information. Return number of lines written (ie, 0 or 1)
 */
static int
iWriteNewsrcLine (
	FILE *fp,
	char *line)
{
	char *seq;
	int sub;
	struct t_group *psGrp;

	seq = pcParseNewsrcLine (line, &sub);

	if (line[0] == '\0' || sub == 0)		/* Insurance against blank line */
		return 0;

	if (seq == NULL) {		/* line has no ':' or '!' in it */
		if (strip_bogus == BOGUS_REMOVE)
			wait_message(2, txt_remove_bogus, line);
		return 0;
	}

	/*
	 * Find the group in active. If we cannot, then junk it if bogus groups
	 * are set to auto removal. Also check for bogus flag just in case
	 * strip_bogus was changed since tin started
	 */
	psGrp = psGrpFind (line);

	if (strip_bogus == BOGUS_REMOVE) {
		if (psGrp == NULL || psGrp->bogus) { /* group dosen't exist */
			wait_message(2, txt_remove_bogus, line);
			return 0;
		}
	}

	if ((psGrp && psGrp->newsrc.present) && (psGrp->subscribed || !strip_newsrc)) {
		fprintf (fp, "%s%c ", psGrp->name, SUB_CHAR(psGrp->subscribed));
		print_bitmap_seq (fp, psGrp);
	} else {
		if (sub == SUBSCRIBED || !strip_newsrc)
			fprintf (fp, "%s%c %s\n", line, sub, seq);
	}

	return 1;
}


/*
 * Read in the users newsrc file and write a new file with all the changes
 * changes from the current session. If this works, replace the original
 * newsrc file.
 * Return TRUE if this succeeds, FALSE otherwise.
 */
t_bool
vWriteNewsrc (void)
{
#ifndef INDEX_DAEMON
	FILE *fp_ip;
	FILE *fp_op;
	char *line;
	int tot = 0;
	struct stat note_stat_newsrc;
	t_bool write_ok = FALSE;

	if ((fp_ip = fopen (newsrc, "r")) == (FILE *) 0)
		return FALSE; /* can't open newsrc */

	/* get size of original newsrc */
	fstat (fileno(fp_ip), &note_stat_newsrc);

	if (!note_stat_newsrc.st_size) {
		fclose (fp_ip);
		return TRUE; /* newsrc is empty */
	}

	if ((fp_op = fopen (newnewsrc, "w" FOPEN_OPTS)) != (FILE *) 0) {
		if (newsrc_mode)
			chmod (newnewsrc, newsrc_mode);

		while ((line = tin_fgets (fp_ip, FALSE)) != (char *) 0)
			tot += iWriteNewsrcLine (fp_op, line);

		/*
		 * Don't rename if either fclose() fails or ferror() is set
		 */
		if (ferror (fp_op) | fclose (fp_op)) {
			error_message (txt_filesystem_full, NEWSRC_FILE);
			unlink (newnewsrc);
		} else
			write_ok = TRUE;
	}

	fclose (fp_ip);

	if (!tot) {
		error_message (txt_newsrc_nogroups);
		return TRUE;		/* So we don't get prompted to try again */
	}

	if (write_ok)
		rename_file (newnewsrc, newsrc);

	return write_ok;
#else
	return TRUE;
#endif	/* !INDEX_DAEMON */
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
		wait_message (0, txt_creating_newsrc);

		for (i = 0; i < num_active; i++)
			fprintf (fp, "%s!\n", active[i].name);

		if (ferror (fp) | fclose (fp))
			error_message (txt_filesystem_full, NEWSRC_FILE);
	}
}


/*
 * Automatically subscribe user to newsgroups specified in
 * NEWSLIBDIR/subscribe (locally) or same file but from NNTP
 * server (LIST SUBSCRIPTIONS) and create .newsrc
 */
static void
auto_subscribe_groups (
	char *newsrc_file)
{
	FILE *fp_newsrc;
	FILE *fp_subs;
	char *ptr;

	/*
	 * If subscription file exists then first unsubscribe to all groups
	 * and then subscribe to just the auto specified groups.
	 */
	if ((fp_subs = open_subscription_fp ()) == (FILE *) 0)
		return;

	wait_message (0, txt_autosubscribing_groups);

	if ((fp_newsrc = fopen (newsrc_file, "w" FOPEN_OPTS)) == (FILE *) 0)
		return;

	if (newsrc_mode)
		chmod (newsrc_file, newsrc_mode);

/* TODO test me ! */
	while ((ptr = tin_fgets (fp_subs, FALSE)) != (char *) 0) {
		if (ptr[0] != '#') {
			if (psGrpFind (ptr) != 0)
				fprintf (fp_newsrc, "%s:\n", ptr);
		}
	}

	/* We ignore user 'q'uits here. They will get them next time in any case */

	if (ferror (fp_newsrc) | fclose (fp_newsrc))
		error_message (txt_filesystem_full, NEWSRC_FILE);

	TIN_FCLOSE (fp_subs);
}


/*
 * make a backup of users .newsrc in case of the bogie man
 */
void
backup_newsrc (void)
{
#ifndef INDEX_DAEMON
	FILE *fp_ip, *fp_op;
	char *line;
	char buf[HEADER_LEN];

	if ((fp_ip = fopen (newsrc, "r")) != (FILE *) 0) {
#	if defined(WIN32)
		joinpath (buf, rcdir, OLDNEWSRC_FILE);
#	else
		joinpath (buf, homedir, OLDNEWSRC_FILE);
#	endif /* WIN32 */
		unlink (buf);	/* because rn makes a link of .newsrc -> .oldnewsrc */

		if ((fp_op = fopen (buf, "w" FOPEN_OPTS)) != (FILE *) 0) {
			if (newsrc_mode)
				chmod (buf, newsrc_mode);

			while ((line = tin_fgets (fp_ip, FALSE)) != (char *) 0)
				fprintf (fp_op, "%s\n", line);

			if (ferror (fp_op) | fclose (fp_op))
				error_message (txt_filesystem_full_backup, NEWSRC_FILE);

		}
		fclose (fp_ip);
	}
#endif /* !INDEX_DAEMON */
}


/*
 * Subscribe/unsubscribe to a group in .newsrc.
 *	This involves rewriting the .newsrc with the new info
 */
void
subscribe (
	struct t_group *group,
	int sub_state)
{
	FILE *fp;
	FILE *newfp;
	char *line;
	char *seq;
	int sub;
	t_bool found = FALSE;

	if ((newfp = fopen (newnewsrc, "w" FOPEN_OPTS)) == (FILE *) 0)
		return;

	if (newsrc_mode)
		chmod (newnewsrc, newsrc_mode);

	if ((fp = fopen (newsrc, "r")) != (FILE *) 0) {

		while ((line = tin_fgets (fp, FALSE)) != (char *) 0) {

			if (STRNCMPEQ("options ", line, 8))
				fprintf (newfp, "%s\n", line);
			else {
				seq = pcParseNewsrcLine (line, &sub);

				if (STRCMPEQ(line, group->name)) {
					fprintf (newfp, "%s%c %s\n", line, sub_state, seq);
					group->subscribed = SUB_BOOL(sub_state);

					/* If previously subscribed to in .newsrc, load up any existing information */
					if (sub_state == SUBSCRIBED)
						parse_bitmap_seq (group, seq);

					found = TRUE;
				} else
					fprintf (newfp, "%s%c %s\n", line, sub, seq);
			}
		}

		fclose (fp);

		if (!found) {
			fprintf (newfp, "%s%c\n", group->name, sub_state);
			group->subscribed = SUB_BOOL(sub_state);

			/* A new group - get min/max/unread counts etc. for it */
			wait_message (0, txt_subscribing);

			if (sub_state == SUBSCRIBED) {
				char null_seq[2];
				null_seq[0] = '\0';

				vGet1GrpArtInfo(group);
				parse_bitmap_seq(group, null_seq);
			}
		}
	}

	if (ferror (newfp) | fclose (newfp)) {
		error_message (txt_filesystem_full, NEWSRC_FILE);
		unlink (newnewsrc);
	} else
		rename_file (newnewsrc, newsrc);
}


void
reset_newsrc (void)
{
	FILE *fp;
	FILE *newfp;
	char *line;
	int sub;
	long i;

	if ((newfp = fopen (newnewsrc, "w" FOPEN_OPTS)) != (FILE *) 0) {

		if (newsrc_mode)
			chmod (newnewsrc, newsrc_mode);

		if ((fp = fopen (newsrc, "r")) != (FILE *) 0) {
			while ((line = tin_fgets (fp, FALSE)) != (char *) 0) {
				(void) pcParseNewsrcLine (line, &sub);
				fprintf (newfp, "%s%c\n", line, sub);
			}
			fclose (fp);
		}
		if (ferror (newfp) | fclose (newfp)) {
			error_message (txt_filesystem_full, NEWSRC_FILE);
			unlink (newnewsrc);
		} else
			rename_file (newnewsrc, newsrc);
	}

	for (i = 0; i < group_top; i++)
		vSetDefaultBitmap (&active[my_group[i]]);
}


/*
 * Rewrite the newsrc file, without the specified group
 */
void
delete_group (
	char *group)
{
	FILE *fp;
	FILE *newfp;
	char *line;
	char *seq;
	int sub;

	if ((newfp = fopen (newnewsrc, "w" FOPEN_OPTS)) != (FILE *) 0) {

		if (newsrc_mode)
			chmod (newnewsrc, newsrc_mode);

		if ((fp = fopen (newsrc, "r")) != (FILE *) 0) {
			while ((line = tin_fgets (fp, FALSE)) != (char *) 0) {
				seq = pcParseNewsrcLine (line, &sub);

				if (!STRCMPEQ(line, group))
					fprintf (newfp, "%s%c %s\n", line, sub, seq);

			}
			fclose (fp);
		}

		if (ferror (newfp) | fclose (newfp)) {
			error_message (txt_filesystem_full, NEWSRC_FILE);
			unlink (newnewsrc);
		} else
			rename_file (newnewsrc, newsrc);
	}
}


/*
 * Mark a group as read
 * If psArt != NULL then we explicitly process each article thus
 * catching crossposts as well, otherwise we simply scrub the
 * bitmap and adjust the highwater mark.
 */
void
grp_mark_read (
	struct t_group *group,
	struct t_article *psArt)
{
	register int i;

#ifdef DEBUG_NEWSRC
	debug_print_comment ("c/C command");
#endif /* DEBUG_NEWSRC */

	if (psArt != (struct t_article *) 0) {
		for (i = 0; i < top; i++)
			art_mark_read (group, &psArt[i]);
	}
	if (group->newsrc.xbitmap != (t_bitmap *) 0) {
		free ((char *) group->newsrc.xbitmap);
		group->newsrc.xbitmap = (t_bitmap *) 0;
	}

	group->newsrc.xbitlen = 0;
	if (group->xmax > group->newsrc.xmax)
		group->newsrc.xmax = group->xmax;
	group->newsrc.xmin = group->newsrc.xmax + 1;
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
#endif /* DEBUG_NEWSRC */

	vGrpGetArtInfo (
		group->spooldir,
		group->name,
		group->type,
		&group->count,
		&group->xmax,
		&group->xmin);

	group->newsrc.num_unread = group->count;
	if (group->xmax > group->newsrc.xmax)
		group->newsrc.xmax = group->xmax;
	if (group->xmin > 0)
		group->newsrc.xmin = group->xmin;

	bitlength = (group->newsrc.xmax - group->newsrc.xmin) + 1;

	if (bitlength < 0)
		bitlength = 0;

	if (bitlength > 0)
		newbitmap = (t_bitmap *) my_malloc (BITS_TO_BYTES(bitlength));

	if (group->newsrc.xbitmap != (t_bitmap *) 0)
		free ((char *) group->newsrc.xbitmap);

	group->newsrc.xbitmap = newbitmap;
	group->newsrc.xbitlen = bitlength;

	if (bitlength)
		NSETRNG1(group->newsrc.xbitmap, 0L, bitlength - 1L);

#ifdef DEBUG_NEWSRC
	debug_print_bitmap (group, NULL);
#endif /* DEBUG_NEWSRC */
}


void
thd_mark_read (
	struct t_group *group,
	long thread)
{
	register int i;

#ifdef DEBUG_NEWSRC
	debug_print_comment ("Mark thread read K command");
#endif /* DEBUG_NEWSRC */

	for (i = (int) thread; i >= 0; i = arts[i].thread)
		art_mark_read (group, &arts[i]);
}


void
thd_mark_unread (
	struct t_group *group,
	long thread)
{
	register int i;

#ifdef DEBUG_NEWSRC
	debug_print_comment ("Mark thread unread Z command");
#endif /* DEBUG_NEWSRC */

	for (i = (int) thread; i >= 0; i = arts[i].thread)
		art_mark_will_return (group, &arts[i]); /* art_mark_unread (group, &arts[i]); */
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
	long low = 0L;
	long high = 0L;
	long min, max;
	t_bool gotseq = FALSE;

	/*
	 * Skip possible non-numeric prefix
	 */
	ptr = seq;
	while (ptr && *ptr && (*ptr < '0' || *ptr > '9'))
		ptr++;

#ifdef DEBUG_NEWSRC
	{
		char buf[NEWSRC_LINE];
		sprintf (buf, "Parsing [%s%c %.*s]", group->name, SUB_CHAR(group->subscribed), (int) (NEWSRC_LINE-strlen(group->name)-20), ptr);
		debug_print_comment(buf);
		debug_print_bitmap(group, NULL);
	}
#endif /* DEBUG_NEWSRC */

	if (ptr) {
		gotseq = TRUE;
		ptr = pcParseGetSeq (ptr, &low, &high);

		if (high < group->xmin - 1)
			high = group->xmin - 1;

		min = ((low <= 1) ? (high + 1) : 1);

		if (group->xmin > min)
			min = group->xmin;

		if (group->xmax > high)
			max = group->xmax;
		else
			max = high;		/* trust newsrc's max */

		if (group->newsrc.xbitmap != (t_bitmap *) 0) {
			free ((char *) group->newsrc.xbitmap);
			group->newsrc.xbitmap = (t_bitmap *) 0;
		}

		group->newsrc.xmax = max;
		group->newsrc.xmin = min;
		group->newsrc.xbitlen = (max - min) + 1;
		if (group->newsrc.xbitlen > 0) {
			group->newsrc.xbitmap = (t_bitmap *) my_malloc (BITS_TO_BYTES(group->newsrc.xbitlen));
			NSETRNG1(group->newsrc.xbitmap, 0L, group->newsrc.xbitlen - 1L);
		}

		if (min <= high) {
			if (low > min)
				sum = low - min;
			else
				low = min;
			NSETRNG0(group->newsrc.xbitmap, low - min, high - min);
		}

		/*
		 * Pick up any additional articles/ranges after the first
		 */
		while (*ptr)
			ptr = pcParseSubSeq (group, ptr, &low, &high, &sum);
	} else {
		if (group->newsrc.xbitmap != (t_bitmap *) 0) {
			free ((char *) group->newsrc.xbitmap);
			group->newsrc.xbitmap = (t_bitmap *) 0;
		}

		group->newsrc.xmax = group->xmax;
		if (group->xmin > 0)
			group->newsrc.xmin = group->xmin;
		else
			group->newsrc.xmin = 1;
		group->newsrc.xbitlen = (group->newsrc.xmax - group->newsrc.xmin) + 1;
		if (group->newsrc.xbitlen > 0) {
			group->newsrc.xbitmap = (t_bitmap *) my_malloc (BITS_TO_BYTES(group->newsrc.xbitlen));
			NSETRNG1(group->newsrc.xbitmap, 0L, group->newsrc.xbitlen - 1L);
		}
/*
wait_message(2, "BITMAP Grp=[%s] MinMax=[%ld-%ld] Len=[%ld]\n",
	group->name, group->xmin, group->xmax, group->newsrc.xbitlen);
*/
	}

	group->newsrc.present = TRUE;

	if (gotseq) {
		if (group->newsrc.xmax > high)
			sum += group->newsrc.xmax - high;
	} else
		sum = (int) ((group->count >= 0) ? group->count : ((group->newsrc.xmax - group->newsrc.xmin) + 1));

	group->newsrc.num_unread = sum;
#ifdef DEBUG_NEWSRC
	debug_print_bitmap(group, NULL);
#endif /* DEBUG_NEWSRC */
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

	lLastHigh = *plHigh;
	pcSeq = pcParseGetSeq(pcSeq, plLow, plHigh);

	/*
	 * Bitmap index
	 */
	lBitMin = *plLow - psGrp->newsrc.xmin;

	/*
	 * check that seq is not out of order
	 */
	if (*plLow > lLastHigh)
		*piSum += (*plLow - lLastHigh) - 1;

	if (*plHigh == *plLow) {
		if (lBitMin >= 0) {
			if (*plHigh > psGrp->newsrc.xmax) {
				/* We trust .newsrc's max. */
				long bitlen;
				t_bitmap *newbitmap;

				psGrp->newsrc.xmax = *plHigh;
				bitlen = psGrp->newsrc.xmax - psGrp->newsrc.xmin + 1;
				newbitmap = (t_bitmap *)my_malloc(BITS_TO_BYTES(bitlen));

				/* Copy over old bitmap */
				memcpy(newbitmap, psGrp->newsrc.xbitmap, BITS_TO_BYTES(psGrp->newsrc.xbitlen));

				/* Mark high numbered articles as unread */
				NSETRNG1(newbitmap, psGrp->newsrc.xbitlen, bitlen - 1);

				free((char *) (psGrp->newsrc.xbitmap));
				psGrp->newsrc.xbitmap = newbitmap;
				psGrp->newsrc.xbitlen = bitlen;
			}
			NSET0(psGrp->newsrc.xbitmap, lBitMin);
		}
	} else if ((*plLow < *plHigh) && (*plHigh >= psGrp->newsrc.xmin)) {
		/*
		 * Restrict the range to min..max
		 */
		if (lBitMin < 0)
			lBitMin = 0;

		lBitMax = *plHigh;

		if (lBitMax > psGrp->newsrc.xmax) {
			/* We trust .newsrc's max. */
			long bitlen;
			t_bitmap *newbitmap;

			psGrp->newsrc.xmax = lBitMax;
			bitlen = psGrp->newsrc.xmax - psGrp->newsrc.xmin + 1;
			newbitmap = (t_bitmap *)my_malloc(BITS_TO_BYTES(bitlen));

			/* Copy over old bitmap */
			memcpy(newbitmap, psGrp->newsrc.xbitmap, BITS_TO_BYTES(psGrp->newsrc.xbitlen));

			/* Mark high numbered articles as unread */
			NSETRNG1(newbitmap, psGrp->newsrc.xbitlen, bitlen - 1);

			free((char *) (psGrp->newsrc.xbitmap));
			psGrp->newsrc.xbitmap = newbitmap;
			psGrp->newsrc.xbitlen = bitlen;
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

	if (*pcSeq == '-') {	/* Range of articles */
		pcSeq++;
		*plHigh = strtol (pcSeq, &pcSeq, 10);
	} else	/* Single article */
		*plHigh = *plLow;

	while (*pcSeq && (*pcSeq < '0' || *pcSeq > '9'))
		pcSeq++;

	return pcSeq;
}


/*
 * Loop thru arts[] array marking state of each article READ/UNREAD
 */
void
parse_unread_arts (
	struct t_group *group)
{
	long	unread = 0;
	long	bitmin, bitmax;
	register int i;
	t_bitmap *newbitmap = (t_bitmap *)0;

	bitmin = group->newsrc.xmin;
	bitmax = group->newsrc.xmax;

	if (group->xmax > group->newsrc.xmax)
		group->newsrc.xmax = group->xmax;

	if (group->newsrc.xmax >= bitmin) {
		newbitmap = (t_bitmap *)my_malloc(BITS_TO_BYTES(group->newsrc.xmax-bitmin+1));
		NSETRNG0(newbitmap, 0L, group->newsrc.xmax - bitmin);
	}

	for (i = 0; i < top; i++) {
		if (arts[i].artnum < bitmin)
			arts[i].status = ART_READ;
		else if (arts[i].artnum > bitmax)
			arts[i].status = ART_UNREAD;
		else if (NTEST(group->newsrc.xbitmap, arts[i].artnum - bitmin) == ART_READ)
			arts[i].status = ART_READ;
		else
			arts[i].status = ART_UNREAD;

		if (arts[i].status == ART_UNREAD && arts[i].artnum >= bitmin) {
			NSET1(newbitmap, arts[i].artnum - bitmin);
			unread++;
		}
	}

	group->newsrc.xbitlen = group->newsrc.xmax - bitmin + 1;

	if (group->newsrc.xbitmap != (t_bitmap *) 0)
		free(group->newsrc.xbitmap);

	group->newsrc.xbitmap = newbitmap;
	group->newsrc.num_unread = unread;
}


static void
print_bitmap_seq (
	FILE *fp,
	struct t_group *group)
{
	long artnum;
	long i, last;
	t_bool flag = FALSE;

#ifdef DEBUG_NEWSRC
	debug_print_comment ("print_bitmap_seq()");
	debug_print_bitmap (group, NULL);
#endif /* DEBUG_NEWSRC */

	if (group->count == 0 || group->xmin > group->xmax) {
		if (group->newsrc.xmax > 1)
			fprintf (fp, "1-%ld", group->newsrc.xmax);

		fprintf (fp, "\n");
		fflush (fp);
#ifdef DEBUG_NEWSRC
		debug_print_comment ("print_bitmap_seq(): group->count == 0");
#endif /* DEBUG_NEWSRC */
		return;
	}

	for (i = group->newsrc.xmin; i <= group->newsrc.xmax; i++) {
		if (group->newsrc.xbitmap && NTEST(group->newsrc.xbitmap, i - group->newsrc.xmin) == ART_READ) {
			if (flag) {
				artnum = i;
				fprintf (fp, ",%ld", i);
			} else {
				artnum = 1;
				flag = TRUE;
				fprintf (fp, "1");
			}
			i++;

			while (i <= group->newsrc.xmax && NTEST(group->newsrc.xbitmap, i - group->newsrc.xmin) == ART_READ)
				i++;

			last = i - 1;

			if (artnum != last)
				fprintf (fp, "-%ld", last);

		} else if (!flag) {
			flag = TRUE;
			if (group->newsrc.xmin > 1) {
				fprintf (fp, "1");

				if (group->newsrc.xmin > 2)
					fprintf (fp, "-%ld", group->newsrc.xmin - 1);

			}
		}
	}

	if (!flag && group->newsrc.xmin > 1) {
		fprintf (fp, "1");

		if (group->newsrc.xmin > 2)
			fprintf (fp, "-%ld", group->newsrc.xmin - 1);

#ifdef DEBUG_NEWSRC
		debug_print_comment ("print_bitmap_seq(): !flag && group->newsrc.xmin > 1");
#endif /* DEBUG_NEWSRC */
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
	int subscribed_pos = 1;
	size_t group_len;
	t_bool found = FALSE;
	t_bool newnewsrc_created = FALSE;
	t_bool option_line = FALSE;
	t_bool repositioned = FALSE;
	t_bool ret_code = FALSE;
	t_bool sub_created = FALSE;
	t_bool unsub_created = FALSE;

	if ((fp_in = fopen (newsrc, "r")) == (FILE *) 0)
		goto rewrite_group_done;

	if ((fp_out = fopen (newnewsrc, "w" FOPEN_OPTS)) == (FILE *) 0)
		goto rewrite_group_done;

	newnewsrc_created = TRUE;

	if (newsrc_mode)
		chmod (newnewsrc, newsrc_mode);

#ifdef VMS
	joinpath (buf, TMPDIR, "subrc");
	sprintf (sub, "%s.%d", buf, (int) process_id);

	joinpath (buf, TMPDIR, "unsubrc");
	sprintf (unsub, "%s.%d", buf, (int) process_id);

#else /* !VMS */
	joinpath (buf, TMPDIR, ".subrc");
	sprintf (sub, "%s.%d", buf, (int) process_id);

	joinpath (buf, TMPDIR, ".unsubrc");
	sprintf (unsub, "%s.%d", buf, (int) process_id);

#endif /* !VMS */

	if ((fp_sub = fopen (sub, "w" FOPEN_OPTS)) == (FILE *) 0)
		goto rewrite_group_done;

	sub_created = TRUE;

	if ((fp_unsub = fopen (unsub, "w" FOPEN_OPTS)) == (FILE *) 0)
		goto rewrite_group_done;

	unsub_created = TRUE;

	/*
	 *  split newsrc into subscribed and unsubscribed to files
	 */
	group_len = strlen (group->name);

	while ((line = tin_fgets (fp_in, FALSE)) != (char *) 0) {
		if (STRNCMPEQ(group->name, line, group_len) && line[group_len] == SUBSCRIBED) {
			newsgroup = my_strdup(line);		/* Take a copy of this line */
			found = TRUE;
			continue;
		} else if (strchr (line, SUBSCRIBED) != (char *) 0) {
			iWriteNewsrcLine(fp_sub, line);
		} else if (strchr (line, UNSUBSCRIBED) != (char *) 0) {
			iWriteNewsrcLine(fp_unsub, line);
		} else {								/* options line at beginning of .newsrc */
			fprintf (fp_sub, "%s\n", line);
			option_line = TRUE;
		}
	}

	if (ferror (fp_sub) | fclose (fp_sub) | ferror (fp_unsub) | fclose (fp_unsub)) {
		error_message (txt_filesystem_full, NEWSRC_FILE);
		fp_sub = fp_unsub = NULL;
		goto rewrite_group_done;
	}

	fp_sub = fp_unsub = NULL;
	fclose (fp_in);
	fp_in = NULL;

	/*
	 * The group to be moved cannot be found, so give up now
	 */
	if (!found)
		goto rewrite_group_done;

	/*
	 *  write subscribed groups & repositioned group to newnewsrc
	 */
	if ((fp_sub = fopen (sub, "r")) == (FILE *) 0)
		goto rewrite_group_done;

	while ((line = tin_fgets(fp_sub, FALSE)) != (char *) 0) {
		if (option_line) {
			if (strchr (line, SUBSCRIBED) == (char *) 0 && strchr (line, UNSUBSCRIBED) == (char *) 0) {
				fprintf (fp_out, "%s\n", line);
				continue;
			} else
				option_line = FALSE;
		}

		if (pos == subscribed_pos) {
			iWriteNewsrcLine(fp_out, newsgroup);
			repositioned = TRUE;
		}

		fprintf (fp_out, "%s\n", line);

		subscribed_pos++;
	}

	if (!repositioned) {
		iWriteNewsrcLine(fp_out, newsgroup);
		repositioned = TRUE;
	}

	/*
	 *  append unsubscribed groups file to newnewsrc
	 */
	if ((fp_unsub = fopen (unsub, "r")) == (FILE *) 0)
		goto rewrite_group_done;

	while ((line = tin_fgets(fp_unsub, FALSE)) != (char *) 0)
		fprintf (fp_out, "%s\n", line);

	/*
	 * Try and cleanly close out the newnewsrc file
	 */
	if (ferror (fp_out) | fclose (fp_out))
		error_message (txt_filesystem_full, NEWSRC_FILE);
	else {
		if (repositioned) {
			rename_file (newnewsrc, newsrc);
			ret_code = TRUE;
		}
	}
	fp_out = NULL;
	newnewsrc_created = FALSE;

rewrite_group_done:
	if (fp_in != (FILE *) 0)
		fclose (fp_in);

	if (fp_out != (FILE *) 0)
		fclose (fp_out);

	if (fp_sub != (FILE *) 0)
		fclose (fp_sub);

	if (fp_unsub != (FILE *) 0)
		fclose (fp_unsub);

	if (newnewsrc_created)
		unlink (newnewsrc);

	if (sub_created)
		unlink (sub);

	if (unsub_created)
		unlink (unsub);

	FreeIfNeeded(newsgroup);

	return ret_code;
}


/*
 *  catchup all groups in .newsrc
 */
void
catchup_newsrc_file (void)
{
	register int i;
	struct t_group *group;

	if (!catchup)
		return;

	for (i = 0; i < group_top; i++) {
		group = &active[my_group[i]];
		group->newsrc.present = TRUE;
		if (group->newsrc.xbitmap != (t_bitmap *) 0) {
			free ((char *) group->newsrc.xbitmap);
			group->newsrc.xbitmap = (t_bitmap *) 0;
		}
		if (group->xmax > group->newsrc.xmax)
			group->newsrc.xmax = group->xmax;
		group->newsrc.xmin = group->newsrc.xmax + 1;
		group->newsrc.num_unread = 0;
		group->newsrc.xbitlen = 0;
	}

	tin_done (EXIT_SUCCESS);
}


/*
 * Break down a line of .newsrc file
 * The sequence information [ eg; 1-3,10,12 ] is returned, line is truncated to
 * just the group name and the subscription flag is copied to sub.
 */
static char *
pcParseNewsrcLine (
	char *line,
	int *sub)
{
	char *ptr, *tmp;

	*sub = UNSUBSCRIBED;				/* Default to no entry */

	if ((ptr = strpbrk(line, " !:")) == NULL)			/* space|SUBSCRIBED|UNSUBSCRIBED */
		return(NULL);

	*sub = *ptr;						/* Save the subscription status */
	tmp = ptr;							/* Keep this blank for later */
	*(ptr++) = '\0';					/* Terminate the group name */

#if 0
	if (ptr == NULL)					/* No seq info, so return a blank */
		return(tmp);
#endif /* 0 */

	if ((ptr = strpbrk(ptr, " \t")) == NULL)
		return(tmp);

	return (ptr + 1);	/* Return pointer to sequence info. At worst this will be \0 */
}


/*
 * expand group->newsrc information if group->xmax is larger than
 * group->newsrc.xmax or min is smaller than group->newsrc.xmin.
 */
void
expand_bitmap (
	struct t_group *group,
	long min)
{
	long bitlen;
	long first;
	long tmp;
	long max;
	t_bool need_full_copy = FALSE;

	/* calculate new max */
	if (group->newsrc.xmax > group->xmax)
		max = group->newsrc.xmax;
	else
		max = group->xmax;

	/* adjust min */
	if (!min)
		min = group->newsrc.xmin;

	/* calculate first */
	if (min >= group->newsrc.xmin)
		first = group->newsrc.xmin;
	else
		first = group->newsrc.xmin - ((group->newsrc.xmin - min + (NBITS-1)) & ~(NBITS-1));

	/* adjust first */
	if (first > group->newsrc.xmax + 1)
		first = first - ((first - (group->newsrc.xmax + 1) + (NBITS-1)) & ~(NBITS-1));

	/* check first */
	if (first < 1) {
		need_full_copy = TRUE;
		first = 1;
	}

	bitlen = max - first + 1;

	if (bitlen <= 0) {
		bitlen = 0;
		free ((char *) group->newsrc.xbitmap);
		group->newsrc.xbitmap = (t_bitmap *) 0;
#ifdef DEBUG_NEWSRC
		debug_print_comment("expand_bitmap: group->newsrc.bitlen == 0");
#endif /* DEBUG_NEWSRC */
	} else if (group->newsrc.xbitmap == (t_bitmap *) 0) {
		group->newsrc.xbitmap = (t_bitmap *)my_malloc (BITS_TO_BYTES(bitlen));
		if (group->newsrc.xmin > first)
			NSETRNG0 (group->newsrc.xbitmap, 0L, group->newsrc.xmin - first - 1L);
		if (bitlen > group->newsrc.xmin - first)
			NSETRNG1 (group->newsrc.xbitmap, group->newsrc.xmin - first, bitlen - 1);
#ifdef DEBUG_NEWSRC
		debug_print_comment("expand_bitmap: group->newsrc.xbitmap == NULL");
#endif /* DEBUG_NEWSRC */
	} else if (need_full_copy) {
		t_bitmap *newbitmap;
		newbitmap = (t_bitmap *)my_malloc(BITS_TO_BYTES(bitlen));

		/* Copy over old bitmap */
		/* TODO: change to use shift */
		for (tmp = group->newsrc.xmin; tmp <= group->newsrc.xmax; tmp++) {
			if (NTEST(group->newsrc.xbitmap, tmp - group->newsrc.xmin))
				NSET1(newbitmap, tmp - first);
			else
				NSET0(newbitmap, tmp - first);
		}

		/* Mark earlier articles as read, updating num_unread */

		if (first < group->newsrc.xmin) {
			NSETRNG0 (newbitmap, 0L, group->newsrc.xmin - first - 1L);
		}
		{
			long i;

			for (i = group->newsrc.xmin; i < min; i++) {
				if (NTEST(newbitmap, i - first) != ART_READ) {
					NSET0(newbitmap, i - first);
					if (group->newsrc.num_unread)
						group->newsrc.num_unread--;
				}
			}
		}

		/* Mark high numbered articles as unread */

		if (group->newsrc.xmin - first + group->newsrc.xbitlen < bitlen) {
			tmp = group->newsrc.xmin - first + group->newsrc.xbitlen;
			NSETRNG1(newbitmap, tmp, bitlen - 1);
		}

		free((char *) (group->newsrc.xbitmap));
		group->newsrc.xbitmap = newbitmap;
#ifdef DEBUG_NEWSRC
		debug_print_comment("expand_bitmap: group->newsrc.bitlen != (group->max-group->min)+1 and need full copy");
#endif /* DEBUG_NEWSRC */
	} else if (max != group->newsrc.xmax || first != group->newsrc.xmin) {
		t_bitmap *newbitmap;
		newbitmap = (t_bitmap *)my_malloc(BITS_TO_BYTES(bitlen));

		/* Copy over old bitmap */

		assert((group->newsrc.xmin - first) / NBITS + BITS_TO_BYTES(group->newsrc.xbitlen) <= BITS_TO_BYTES(bitlen));

		memcpy(newbitmap + (group->newsrc.xmin - first) / NBITS, group->newsrc.xbitmap, BITS_TO_BYTES(group->newsrc.xbitlen));

		/* Mark earlier articles as read, updating num_unread */

		if (first < group->newsrc.xmin) {
			NSETRNG0 (newbitmap, 0L, group->newsrc.xmin - first - 1L);
		}
		{
			long i;

			for (i = group->newsrc.xmin; i < min; i++) {
				if (NTEST(newbitmap, i - first) != ART_READ) {
					NSET0(newbitmap, i - first);
					if (group->newsrc.num_unread)
						group->newsrc.num_unread--;
				}
			}
		}

		/* Mark high numbered articles as unread */

		if (group->newsrc.xmin - first + group->newsrc.xbitlen < bitlen) {
			tmp = group->newsrc.xmin - first + group->newsrc.xbitlen;
			NSETRNG1(newbitmap, tmp, bitlen - 1);
		}

		free((char *) (group->newsrc.xbitmap));
		group->newsrc.xbitmap = newbitmap;
#ifdef DEBUG_NEWSRC
		debug_print_comment("expand_bitmap: group->newsrc.bitlen != (group->max-group->min)+1");
#endif /* DEBUG_NEWSRC */
	}
	group->newsrc.xmin = first;
	if (group->newsrc.xmax < max)
		group->newsrc.num_unread += max - group->newsrc.xmax;
	group->newsrc.xmax = max;
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
#endif /* DEBUG_NEWSRC */
		}
		if ((art->status == ART_UNREAD) || (art->status == ART_WILL_RETURN)) {
			art_mark_xref_read (art);

			if (group != (struct t_group *) 0 && group->newsrc.num_unread)
				group->newsrc.num_unread--;

			if (art->killed)
				num_of_killed_arts++;

			if (art->selected && num_of_selected_arts)
				num_of_selected_arts--;

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

			if (group != (struct t_group *) 0)
				group->newsrc.num_unread++;

			if (art->selected)
				num_of_selected_arts++;

			art->status = ART_UNREAD;
		}
		if (group != (struct t_group *) 0) {
			if (art->artnum < group->newsrc.xmin)
				expand_bitmap (group, art->artnum);
			else {
				NSET1(group->newsrc.xbitmap, art->artnum - group->newsrc.xmin);
#ifdef DEBUG_NEWSRC
				debug_print_bitmap (group, art);
#endif /* DEBUG_NEWSRC */
			}
		}
	}
}


/*
 * make a funktion art_mark(group, art, FLAG)
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

			if (group != (struct t_group *) 0)
				group->newsrc.num_unread++;

			if (art->selected)
				num_of_selected_arts++;

			art->status = ART_WILL_RETURN;
		}
		if (group != (struct t_group *) 0) {
			NSET1(group->newsrc.xbitmap, art->artnum - group->newsrc.xmin);
#ifdef DEBUG_NEWSRC
			debug_print_bitmap (group, art);
#endif /* DEBUG_NEWSRC */
		}
	}
}


#if !defined(INDEX_DAEMON)
void
art_mark_deleted (
	struct t_article *art)
{
	if (art != (struct t_article *) 0)
		art->delete_it = TRUE;
}


void
art_mark_undeleted (
	struct t_article *art)
{
	if (art != (struct t_article *) 0)
		art->delete_it = FALSE;
}
#endif /* !INDEX_DAEMON */


void
vSetDefaultBitmap (
	struct t_group *group)
{
	if (group != (struct t_group *) 0) {
		group->newsrc.num_unread = 0;
		group->newsrc.present = FALSE;

		if (group->newsrc.xbitmap != (t_bitmap *) 0)
			free ((char *) (group->newsrc.xbitmap));

		group->newsrc.xbitmap = (t_bitmap *) 0;
		group->newsrc.xbitlen = 0;
		if (group->xmin > 0)
			group->newsrc.xmin = group->xmin;
		else
			group->newsrc.xmin = 1;
		group->newsrc.xmax = group->newsrc.xmin - 1;
	}
}


/* TEST harness */
#ifdef DEBUG_NEWSRC

#	ifdef DEBUG_NEWSRC_FIXME	/* something's broken here */
		static void set_bitmap_range_read (struct t_newsrc *newsrc, long beg, long end);
		static void set_bitmap_range_unread (struct t_newsrc *newsrc, long beg, long end);
#	endif /* DEBUG_NEWSRC_FIXME */

void
vNewsrcTestHarness (void)
{
	char seq[20000];
	char *temp_file = NULL;
	FILE *fp;
	int i;
	int retry = 10; /* max. retrys */
	int fd;
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

	my_printf ("\nENV Min=[%ld] Max=[%ld] Rng=[%ld-%ld] Count=[%ld] Seq=[%s]\n",
		group.xmin, group.xmax, rng_min, rng_max, group.count, seq);

	for (i = 0; i < 3; i++) {
		if (group.newsrc.xbitmap != (t_bitmap *) 0) {
			free ((char *) group.newsrc.xbitmap);
			group.newsrc.xbitmap = (t_bitmap *) 0;
			group.newsrc.xbitlen = 0;
			group.newsrc.num_unread = 0;
			group.newsrc.xmin = 1;
			group.newsrc.xmax = 0;
		}
		while (retry) {
		/* FIXME - this is secure now, but doesn't write any debug output */
		/* (it didn't before too) */
			if ((temp_file = my_tempnam ("","NEWSRC")) != (char *) 0) {
				if ((fd = open (temp_file, (O_CREAT|O_EXCL), (S_IRUSR|S_IWUSR))) !=-1) {
					if ((fp = fopen (temp_file, "w")) != (FILE *) 0) {
						my_printf ("\n%d. PARSE Seq=[%s]\n", i+1, seq);
						parse_bitmap_seq (&group, seq);
						debug_print_newsrc (&group.newsrc, stdout);
						print_bitmap_seq (fp, &group);
						my_printf("   PRINT Seq=[");
						print_bitmap_seq (stdout, &group);
						fclose(fp);
					} else
						retry--;
					close(fd);
					break;
				} else
					retry--;
			} else
				retry--;
		}

		debug_print_newsrc (&group.newsrc, stdout);

		if (!retry)
			error_message (txt_cannot_create_uniq_name);
		else {
			fp = fopen (temp_file, "r");
			fgets (seq, (int) sizeof(seq), fp);
			seq[strlen(seq)-1] = '\0';
			fclose (fp);
		}
	}
#	ifdef DEBUG_NEWSRC_FIXME	/* something's broken here */
	set_bitmap_range_read (&group.newsrc, rng_min, rng_max);
	debug_print_newsrc (&group.newsrc, stdout);

	set_bitmap_range_unread (&group.newsrc, rng_min, rng_max);
	debug_print_newsrc (&group.newsrc, stdout);


	NSETBLK0(group.newsrc.xbitmap, group.newsrc.xbitlen);
	debug_print_newsrc (&group.newsrc, stdout);

	NSETBLK1(group.newsrc.xbitmap, group.newsrc.xbitlen);
	debug_print_newsrc (&group.newsrc, stdout);
#	endif /* DEBUG_NEWSRC_FIXME */
	my_printf ("\n");
}


#	ifdef DEBUG_NEWSRC_FIXME	/* something's broken here */
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

my_printf ("\nRNG Min-Max=[%ld-%ld] Beg-End=[%ld-%ld] OFF=[%ld] LEN=[%ld]\n",
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

my_printf ("\nRNG Min-Max=[%ld-%ld] Beg-End=[%ld-%ld] OFF=[%ld] LEN=[%ld]\n",
newsrc->xmin, newsrc->xmax, beg, end, offset, length);

		if (beg == end) {
			NSET1(newsrc->xbitmap, offset);
		} else {
			NSETRNG1(newsrc->xbitmap, offset, length);
		}
	}
}
#	endif /* DEBUG_NEWSRC_FIXME */

#endif /* DEBUG_NEWSRC */
