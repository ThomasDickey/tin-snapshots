/*
 *  Project   : tin - a Usenet reader
 *  Module    : xref.c
 *  Author    : I. Lea & H. Brugge
 *  Created   : 1993-07-01
 *  Updated   : 1998-20-24
 *  Notes     :
 *  Copyright : (c) Copyright 1991-99 by Iain Lea & Herman ten Brugge
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */


#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */

/* dbmalloc checks memset() parameters, so we'll use it to check the assignments */
#ifdef USE_DBMALLOC
#	define BIT_OR(n, b, mask)	memset(n+NOFFSET(b), n[NOFFSET(b)] | (mask), 1)
#	define BIT_AND(n, b, mask)	memset(n+NOFFSET(b), n[NOFFSET(b)] & (mask), 1)
#else
#	define BIT_OR(n, b, mask)	n[NOFFSET(b)] |= mask
#	define BIT_AND(n, b, mask)	n[NOFFSET(b)] &= mask
#endif /* USE_DBMALLOC */

#ifdef NNTP_ABLE
#	ifdef XHDR_XREF
		static void read_xref_header (struct t_article *art);
#	endif /* XHDR_XREF */
#endif /* NNTP_ABLE */


/*
 *  Read NEWSLIBDIR/overview.fmt file to check if Xref:full is enabled/disabled
 */
t_bool
overview_xref_support (
	void)
{
	FILE *fp;
	char *ptr;
	t_bool supported = FALSE;

	if ((fp = open_overview_fmt_fp ()) != (FILE *) 0) {
		while ((ptr = tin_fgets (fp, FALSE)) != (char *) 0) {

			if (STRNCASECMPEQ(ptr, "Xref:full", 9)) {
				supported = TRUE;
#ifdef NNTP_ABLE
				drain_buffer(fp);
#endif /* NNTP_ABLE */
				break;
			}
		}
		TIN_FCLOSE (fp);
		/*
		 * If user aborted with 'q', then we continue regardless. If Xref was
		 * found, then fair enough. If not, tough. No real harm done
		 */
	}

	if (!supported)
		wait_message (2, txt_warn_xref_not_supported);

	return supported;
}

/*
 * read xref reference for current article
 * This enables crosspost marking even if the xref records are not
 * part of the xover record.
 */
#ifdef NNTP_ABLE
#	ifdef XHDR_XREF
static void
read_xref_header (
	struct t_article *art)
{
	/* xref_supported means already supported in xover record */
	if (!xref_supported && read_news_via_nntp && art && !art->xref) {
		FILE *fp;
		char *ptr, *q;
		char buf[HEADER_LEN];
		long artnum = 0L;

		sprintf(buf, "XHDR XREF %ld", art->artnum);
		if ((fp = nntp_command (buf, OK_HEAD, NULL)) == NULL)
			return;

		while ((ptr = tin_fgets (fp, FALSE)) != (char *) 0) {
			while (*ptr && isspace((int)*ptr))
				ptr++;
			if (*ptr == '.')
				break;
			/*
			 * read the article number
			 */
			artnum = atol (ptr);
			if ((artnum == art->artnum) && !art->xref && !strstr (ptr, "(none)")) {
				q = strchr (ptr, ' ');	/* skip article number */
				if (q == NULL)
					continue;
				ptr = q;
				while (*ptr && isspace((int)*ptr))
					ptr++;
				q = strchr (ptr, '\n');
				if (q)
					*q = '\0';
				art->xref = my_strdup (ptr);
			}
		}

	}
	return;
}
#	endif /* XHDR_XREF */
#endif /* NNTP_ABLE */


/*
 *  mark all other Xref: crossposted articles as read when one article read
 *  Xref: sitename newsgroup:artnum newsgroup:artnum [newsgroup:artnum ...]
 */
void
art_mark_xref_read (
	struct t_article *art)
{
	char *xref_ptr;
	char *group;
	char *ptr, c;
	long artnum;
	t_bool artread;
	struct t_group *psGrp;

#ifdef NNTP_ABLE
#	ifdef XHDR_XREF
	read_xref_header (art);
#	endif /* XHDR_XREF */
#endif /* NNTP_ABLE */

	if (art->xref == (char *) 0)
		return;

	xref_ptr = art->xref;

	/*
	 *  check sitename matches nodename of current machine (ignore for now!)
	 */
	while (*xref_ptr != ' ' && *xref_ptr)
		xref_ptr++;

	/*
	 *  tokenize each pair and update that newsgroup if it is in my_group[].
	 */
	forever {
		while (*xref_ptr == ' ')
			xref_ptr++;

		group = xref_ptr;
		while (*xref_ptr != ':' && *xref_ptr)
			xref_ptr++;

		if (*xref_ptr != ':')
			break;

		ptr = xref_ptr++;
		artnum = atol (xref_ptr);
		while (isdigit((int)*xref_ptr))
			xref_ptr++;

		if (&ptr[1] == xref_ptr)
			break;

		c = *ptr;
		*ptr = '\0';

		psGrp = psGrpFind (group);

#ifdef DEBUG
		if (debug == 3) {
			sprintf (mesg, "LOOKUP Xref: [%s:%ld] active=[%s] num_unread=[%ld]",
				group, artnum,
				(psGrp ? psGrp->name : ""),
				(psGrp ? psGrp->newsrc.num_unread : 0));
#	ifdef DEBUG_NEWSRC
			debug_print_comment (mesg);
			debug_print_bitmap (psGrp, NULL);
#	endif /* DEBUG_NEWSRC */
			error_message (mesg);
		}
#endif /* DEBUG */

		if (psGrp && psGrp->newsrc.xbitmap) {
			if (artnum >= psGrp->newsrc.xmin && artnum <= psGrp->xmax) {
				artread = ((NTEST(psGrp->newsrc.xbitmap, artnum - psGrp->newsrc.xmin) == ART_READ) ? TRUE : FALSE);
				if (!artread) {
					NSET0(psGrp->newsrc.xbitmap, artnum - psGrp->newsrc.xmin);
					if (psGrp->newsrc.num_unread > 0)
						psGrp->newsrc.num_unread--;
#ifdef DEBUG
					if (debug == 3) {
						sprintf (mesg, "FOUND!Xref: [%s:%ld] marked READ num_unread=[%ld]",
							group, artnum, psGrp->newsrc.num_unread);
#	ifdef DEBUG_NEWSRC
						debug_print_comment (mesg);
						debug_print_bitmap (psGrp, NULL);
#	endif /* DEBUG_NEWSRC */
						wait_message (2, mesg);
					}
#endif /* DEBUG */
				}
			}
		}
		*ptr = c;
	}
}


/*
 * Set bits [low..high] of 'bitmap' to 1's
 */
void
NSETRNG1 (
	t_bitmap *bitmap,
	long low,
	long high)
{
	register long i;

	if (bitmap == (t_bitmap *) 0) {
		error_message ("NSETRNG1() failed. Bitmap == NULL");
		return;
	}

	if (high >= low) {
		if (NOFFSET(high) == NOFFSET(low)) {
			for (i = low; i <= high; i++) {
				NSET1(bitmap, i);
			}
		} else {
			BIT_OR(bitmap, low, (NBITSON << NBITIDX(low)));
			if (NOFFSET(high) > NOFFSET(low) + 1) {
				memset (&bitmap[NOFFSET(low) + 1], NBITSON,
					(size_t) (NOFFSET(high) - NOFFSET(low) - 1));
			}
			BIT_OR(bitmap, high, ~ (NBITNEG1 << NBITIDX(high)));
		}
	}
}

/*
 * Set bits [low..high] of 'bitmap' to 0's
 */
void
NSETRNG0 (
	t_bitmap *bitmap,
	long low,
	long high)
{
	register long i;

	if (bitmap == (t_bitmap *) 0) {
		error_message ("NSETRNG0() failed. Bitmap == NULL");
		return;
	}

	if (high >= low) {
		if (NOFFSET(high) == NOFFSET(low)) {
			for (i = low; i <= high; i++) {
				NSET0(bitmap, i);
			}
		} else {
			BIT_AND(bitmap, low, ~(NBITSON << NBITIDX(low)));

			if (NOFFSET(high) > NOFFSET(low) + 1) {
				memset (&bitmap[NOFFSET(low) + 1], 0,
					(size_t) (NOFFSET(high) - NOFFSET(low) - 1));
			}
			BIT_AND(bitmap, high, NBITNEG1 << NBITIDX(high));
		}
	}
}
