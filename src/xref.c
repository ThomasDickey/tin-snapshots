/*
 *  Project   : tin - a Usenet reader
 *  Module    : xref.c
 *  Author    : I.Lea & H.Brugge
 *  Created   : 01-07-93
 *  Updated   : 02-12-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Herman ten Brugge
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

/* dbmalloc checks memset() parameters, so we'll use it to check the assignments */
#ifdef USE_DBMALLOC
#define BIT_OR(n,b,mask)	memset(n+NOFFSET(b), n[NOFFSET(b)] | (mask), 1)
#define BIT_AND(n,b,mask)	memset(n+NOFFSET(b), n[NOFFSET(b)] & (mask), 1)
#else
#define BIT_OR(n,b,mask)	n[NOFFSET(b)] |= mask
#define BIT_AND(n,b,mask)	n[NOFFSET(b)] &= mask
#endif

/*
 *  Read NEWSLIBDIR/overview.fmt file to check if Xref:full is enabled/disabled
 *  Note: if the file cannot be found set the default Xref supported to TRUE
 */

int
overview_xref_support (void)
{
	char buf[HEADER_LEN];
	char *ptr;
	FILE *fp;
	int supported = TRUE;

	fp = open_overview_fmt_fp ();

	if (fp != (FILE *) 0) {
		supported = FALSE;
		while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
			ptr = strrchr (buf, '\n');
			if (ptr != (char *) 0) {
				*ptr = '\0';
			}
			if (STRCMPEQ(buf, "Xref:full")) {
				supported = TRUE;
				break;
			}
		}
		fclose (fp);
	}

	return supported;
}

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
	int artread;
	long artnum;
	struct t_group *psGrp;

	if (art->xref == '\0') {
		return;
	}

	xref_ptr = art->xref;

	/*
	 *  check sitename matches nodename of current machine (ignore for now!)
	 */
	while (*xref_ptr != ' ' && *xref_ptr) {
		xref_ptr++;
	}

	/*
	 *  tokenize each pair and update that newsgroup if it is in my_group[].
	 */
	forever {
		while (*xref_ptr == ' ') {
			xref_ptr++;
		}
		group = xref_ptr;
		while (*xref_ptr != ':' && *xref_ptr) {
			xref_ptr++;
		}
		if (*xref_ptr != ':') {
			break;
		}
		ptr = xref_ptr++;
		artnum = atol (xref_ptr);
		while (*xref_ptr >= '0' && *xref_ptr <= '9') {
			xref_ptr++;
		}
		if (&ptr[1] == xref_ptr) {
			break;
		}
		c = *ptr;
		*ptr = 0;

		psGrp = psGrpFind (group);

		if (debug == 3) {
			sprintf (msg, "LOOKUP Xref: [%s:%ld] active=[%s] num_unread=[%ld]",
				group, artnum,
				(psGrp ? psGrp->name : ""),
				(psGrp ? psGrp->newsrc.num_unread : 0));
#ifdef DEBUG_NEWSRC
			debug_print_comment (msg);
			debug_print_bitmap (psGrp, NULL);
#endif
			error_message (msg, "");
		}

		if (psGrp && psGrp->newsrc.xbitmap) {
			if (artnum >= psGrp->newsrc.xmin && artnum <= psGrp->xmax) {
			    artread = (NTEST(psGrp->newsrc.xbitmap, artnum - psGrp->newsrc.xmin) == ART_READ ? TRUE : FALSE);
				if (!artread) {
					NSET0(psGrp->newsrc.xbitmap, artnum - psGrp->newsrc.xmin);
					if (psGrp->newsrc.num_unread > 0) {
						psGrp->newsrc.num_unread--;
					}
					if (debug == 3) {
						sprintf (msg, "FOUND!Xref: [%s:%ld] marked READ num_unread=[%ld]",
							group, artnum, psGrp->newsrc.num_unread);
#ifdef DEBUG_NEWSRC
						debug_print_comment (msg);
						debug_print_bitmap (psGrp, NULL);
#endif
						wait_message (msg);
					}
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
		error_message ("NSETRNG1() failed. Bitmap == NULL", "");
		return;
	}

	if (high >= low) {
		if (NOFFSET(high) == NOFFSET(low)) {
			for (i=low; i <= high; i++) {
				NSET1(bitmap, i);
			}
		} else {
			BIT_OR(bitmap, low, (NBITSON << NBITIDX(low)));
			if (NOFFSET(high) > NOFFSET(low) + 1) {
				memset (&bitmap[NOFFSET(low) + 1], NBITSON,
					(size_t) (NOFFSET(high)-NOFFSET(low)-1));
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
		error_message ("NSETRNG0() failed. Bitmap == NULL", "");
		return;
	}

	if (high >= low) {
		if (NOFFSET(high) == NOFFSET(low)) {
			for (i=low; i <= high; i++) {
				NSET0(bitmap, i);
			}
		} else {
			BIT_AND(bitmap, low, ~(NBITSON << NBITIDX(low)));

			if (NOFFSET(high) > NOFFSET(low) + 1) {
				memset (&bitmap[NOFFSET(low) + 1], 0,
					(size_t) (NOFFSET(high)-NOFFSET(low)-1));
			}
			BIT_AND(bitmap, high, NBITNEG1 << NBITIDX(high));
		}
	}
}
