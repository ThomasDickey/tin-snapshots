/*
 *  Project   : tin - a Usenet reader
 *  Module    : art.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 19-04-95
 *  Notes     :
 *  Copyright : (c) Copyright 1991-95 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"
#include	"tcurses.h"

#ifdef	PROFILE
#	include	"stpwatch.h"
#endif

#define SortBy(func) qsort (arts, (size_t)top, sizeof (struct t_article), func);

char *glob_art_group;
static long last_read_article;
static int overview_index_filename = FALSE;

/*
 * Local prototypes
 */
static char *pcPrintDate (time_t lSecs);
static char *pcPrintFrom (struct t_article *psArt);
static int artnum_comp (t_comptype *p1, t_comptype *p2);
static int date_comp (t_comptype *p1, t_comptype *p2);
static int from_comp (t_comptype *p1, t_comptype *p2);
static int iReadNovFile (struct t_group *group, long min, long max, int *expired);
static int parse_headers (FILE *fp, struct t_article *h);
static int read_group (struct t_group *group, char *group_path, int *pcount);
static int score_comp (t_comptype *p1, t_comptype *p2);
static int subj_comp (t_comptype *p1, t_comptype *p2);
static int valid_artnum (long art);
static void print_expired_arts (int num_expired);
static void thread_by_subject (void);

#ifdef INDEX_DAEMON
	static void vCreatePath (char *pcPath);
#endif /* INDEX_DAEMON */
   

/*
 *  Construct the pointers to the base article in each thread.
 *  If we are showing only unread, then point to the first unread. I have
 *  no idea why this should be so, it causes problems elsewhere [which_response]
 *  .inthread is set on each article that is after the first article in the
 *  thread.  Articles which have been expired have their .thread set to
 *  ART_EXPIRED
 */

void
find_base (
	struct t_group *group)
{
	register int i;
	register int j;

	top_base = 0;

#ifdef DEBUG
	debug_print_arts ();
#endif

	if (group->attribute && group->attribute->show_only_unread) {
		for (i = 0; i < top; i++) {
			if (IGNORE_ART(i) || arts[i].inthread)
				continue;

			if (top_base >= max_art)
				expand_art ();

			if (arts[i].status == ART_UNREAD)
				base[top_base++] = i;
			else {
				for (j = i; j >= 0; j = arts[j].thread) {
					if (arts[j].status == ART_UNREAD) {
						base[top_base++] = i;
						break;
					}
				}
			}
		}
	} else {
		for (i = 0; i < top; i++) {
			if (IGNORE_ART(i) || arts[i].inthread)
				continue;

			if (top_base >= max_art)
				expand_art ();

			base[top_base++] = i;
		}
	}
}

/*
 *  Main group indexing routine.
 *
 *  Will read any existing index, create or incrementally update
 *  the index by looking at the articles in the spool directory,
 *  and attempt to write a new index if necessary.
 *
 *  Returns FALSE if the user aborted the indexing, otherwise TRUE
 */

int
index_group (
	struct t_group *group)
{
	char group_path[PATH_LEN];
	int count;
	int expired;
	int filtered;
	int modified;
	long min;
	long max;
	register int i;

	if (group == (struct t_group *) 0)
		return (TRUE);

	if (INTERACTIVE)
		wait_message (0, txt_group, group->name);

	make_group_path (group->name, group_path);
	glob_art_group = group->name;

	set_alarm_clock_off ();
	set_signals_art ();

	hash_reclaim ();
	free_art_array ();
	free_msgids ();

	/*
	 *  Load articles within min..max from xover index file if it exists
	 *  and then create base[] article numbers from loaded articles.
	 *  If nov file does not exist then create base[] with setup_base().
	 */
#ifdef PROFILE
	BegStopWatch("setup_base");
#endif /* PROFILE */

	if (setup_hard_base (group, group_path) < 0)
		return(FALSE);

#ifdef PROFILE
	EndStopWatch();
	PrintStopWatch();
#endif /* PROFILE */

#ifdef DEBUG_NEWSRC
	debug_print_comment ("Before iReadNovFile");
	debug_print_bitmap (group, NULL);
#endif

	min = top_base ? base[0] : group->xmin;
	max = top_base ? base[top_base-1] : min - 1;

	/*
	 * Quit now if no articles
	 */
	if (max < 0)
		return FALSE;

	/*
	 * Read in the existing index via XOVER or the index file
	 */
	if (iReadNovFile (group, min, max, &expired) == -1) {
		/* user aborted indexing */
		set_alarm_clock_on ();
		return FALSE;
	}

	/*
	 * Prints 'P' for each expired article if verbose
	 */
	if (expired)
		print_expired_arts (expired);

	/*
	 * Add any articles to arts[] that are new or were killed
	 */
	if ((modified = read_group (group, group_path, &count)) == -1) {
		/* user aborted indexing */
		set_alarm_clock_on ();
		return FALSE;
	}

	/*
	 * Do this before calling art_mark_read if you want
	 * the unread count to be correct.
	 */
#ifdef DEBUG_NEWSRC
	debug_print_comment ("Before parse_unread_arts()");
	debug_print_bitmap (group, NULL);
#endif
	parse_unread_arts (group);
#ifdef DEBUG_NEWSRC
	debug_print_comment ("After parse_unread_arts()");
	debug_print_bitmap (group, NULL);
#endif

	/*
	 * Stat all articles to see if any have expired
	 */
	for (i = 0; i < top; i++) {
		if (arts[i].thread == ART_EXPIRED) {
			expired = 1;
#ifdef DEBUG_NEWSRC
			debug_print_comment ("art.c: index_group() purging...");
#endif
			art_mark_read (group, &arts[i]);
			print_expired_arts (expired);
		}
	}

	if (expired || modified || cache_overview_files)
		vWriteNovFile (group);

	/*
	 * Create the reference tree. The msgid and ref ptrs will
	 * be free()d now that the NovFile has been written.
	 */
	build_references(group);

	/*
	 * Needs access to the reference tree
	 */
	filtered = filter_articles (group);

	if ((expired || count) && cmd_line && verbose) {
		my_fputc ('\n', stdout);
		my_flush ();
	}

#ifdef	PROFILE
	BegStopWatch("make_thread");
#endif	/* PROFILE */

	make_threads (group, FALSE);

#ifdef	PROFILE
	EndStopWatch();
	PrintStopWatch();
#endif	/* PROFILE */

	find_base (group);

	if ((modified || filtered) && !batch_mode)
		clear_message ();

	set_alarm_clock_on ();
	return (TRUE);
}

/*
 * Index a group.  Assumes any existing NOV index has already been loaded.
 * Return values are:
 *    TRUE   loaded index and modified it
 *    FALSE  loaded index but not modified
 *    -1     user aborted indexing operation
 */

static int
read_group (
	struct t_group *group,
	char *group_path,
	int *pcount)
{
	FILE *fp;
	char buf[PATH_LEN];
	int count = 0, res;
	int modified = FALSE;
	int respnum, total = 0;
	long art;
	register int i;
	static char dir[PATH_LEN] = "";

	/*
	 * change to groups spooldir to optimize fopen()'s on local articles
	 */
#ifdef INDEX_DAEMON
	if (dir[0] == 0)
#endif
		get_cwd (dir);						/* TODO: no point via NNTP ? */

	joinpath (buf, group->spooldir, group_path);
	my_chdir (buf);

	buf[0] = '\0';

	/*
	 *  Count num of arts to index so the user has an idea of index time
	 */
	for (i = 0; i < top_base; i++) {
		if (base[i] <= last_read_article || valid_artnum (base[i]) >= 0)
			continue;

		total++;
	}

	/*
	 * Reset the next article number index (for when HEAD fails)
	 */
	head_next = -1;

	for (i = 0; i < top_base; i++) {	/* for each article # */

		art = base[i];

		/*
		 *  Do we already have this article in our index?  Change
		 *  arts[].thread from ART_EXPIRED to ART_NORMAL and skip
		 *  reading the header.
		 */
		if ((respnum = valid_artnum (art)) >= 0 || art <= last_read_article) {
			if (respnum >= 0)
				arts[respnum].thread = ART_NORMAL;

			continue;
		}

		/*
		 * Try and open the article
	 	 */
		if ((fp = open_art_header (art)) == (FILE *) 0)
			continue;

		/*
		 * we've modified the index so it will need to be re-written
		 */
		modified = TRUE;

		/*
		 *  Add article to arts[]
		 */
		if (top >= max_art)
			expand_art();

		set_article (&arts[top]);
		arts[top].artnum = art;
		arts[top].thread = ART_NORMAL;

		res = parse_headers (fp, &arts[top]);

		TIN_FCLOSE(fp);
		if (tin_errno != 0) {
			chdir (dir);
			return(-1);
		}

		if (!res) {
			sprintf (buf, "FAILED parse_header(%ld)", art);
#ifdef DEBUG
			debug_nntp ("read_group", buf);
#endif
			continue;
		}

		last_read_article = arts[top].artnum;	/* used if arts are killed */
		top++;

#ifdef SHOW_PROGRESS
		if (++count % MODULO_COUNT_NUM == 0)
			show_progress (msg, count, total);
#endif

		if (batch_mode && verbose) {
			my_fputc ('.', stdout);
			my_flush();
		}

	}

	/*
	 * Update number of article we 'read'
	 */
	*pcount = count;

	/*
	 * change to previous dir before indexing started
	 */
	my_chdir (dir);

	return modified;
}


/*
 * The algorithm is elegant, using the fact that identical Subject lines
 * are hashed to the same node in table[] (see hashstr.c)
 *
 * Mark i as being in j's thread list if
 * . The article is _not_ being ignored
 * . The article is not already threaded
 * . One of the following is true:
 *    1) The subject lines are the same
 *    2) Both are part of the same archive (name's match and arch bit set)
 *
 */
static void
thread_by_subject(void)
{
	int i, j;
	struct t_hashnode *h;

	for (i = 0; i < top; i++) {

		if (arts[i].thread != ART_NORMAL || IGNORE_ART(i))
			continue;

		/*
		 * Get the contents of the magic marker in the hashnode
		 */
		h = (struct t_hashnode *)(arts[i].subject - sizeof(int) - sizeof(void *)); /* FIXME: cast increases required alignment of target type */

		j = h->aptr;

		if (j != -1 && j < i) {

			/*
			 * Surely the test for IGNORE_ART() was done 12 lines ago ??
			 */
			if (/*!IGNORE_ART(i) &&*/ !arts[i].inthread &&
						   ((arts[i].subject == arts[j].subject) ||
						   ((arts[i].part || arts[i].patch) &&
							 arts[i].archive == arts[j].archive))) {
				arts[j].thread = i;
				arts[i].inthread = TRUE;
			}
		}

		/*
		 * Update the magic marker with the highest numbered msg in
		 * arts[] that has been used in this thread so far
		 */
		h->aptr = i;
	}

#if 0
	fprintf(stderr, "Subj dump\n");
	fprintf(stderr, "%3s %3s %3s %3s : %3s %3s\n", "#", "Par", "Sib", "Chd", "In", "Thd");
	for (i=0 ; i < top ; i++) {
		fprintf(stderr, "%3d %3d %3d %3d : %3d %3d : %.50s %s\n", i,
			(arts[i].refptr->parent)  ? arts[i].refptr->parent->article : -2,
			(arts[i].refptr->sibling) ? arts[i].refptr->sibling->article : -2,
			(arts[i].refptr->child)   ? arts[i].refptr->child->article : -2,
			arts[i].inthread, arts[i].thread, arts[i].refptr->txt, arts[i].subject);
	}
#endif

}

/*
 *  Go through the articles in arts[] and create threads. There are
 *  4 strategies currently defined :
 *
 *	THREAD_NONE		No threading
 *	THREAD_SUBJ		Threads are created using like Subject lines
 *	THREAD_REFS		Threads are created using the References headers
 *	THREAD_BOTH		Threads created using References and then Subject
 *
 *  Apart from THREAD_NONE, .thread and .inthread are used, the
 *  first article in a thread should have .inthread set to FALSE, the
 *  rest TRUE.  Only do unexprired articles we haven't visited yet
 *  (arts[].thread == -1 ART_NORMAL).
 *
 *  The rethread parameter is a misnomer. Its only effect (if set) is
 *  to delete all threading information, not to rethread
 *
 */

void
make_threads (
	struct t_group *group,
	int rethread)
{
	int i;

	if (!cmd_line) {
		if (group->attribute && group->attribute->thread_arts == THREAD_NONE)
			info_message (txt_unthreading_arts);
		else
			info_message (txt_threading_arts);
	}

#ifdef DEBUG
	if (debug == 2)
		error_message ("rethread=[%d]  thread_arts=[%d]  attr_thread_arts=[%d]",
				rethread, default_thread_arts, group->attribute->thread_arts);
#endif

	/*
	 * Sort all the articles using the preferred method
	 * When find_base() is called, the bases are created ordered
	 * on arts[] and so the base messages under all threading systems
	 * will be sorted in this way.
	 */
	if (group->attribute)
		sort_arts (group->attribute->sort_art_type);

	/*
	 * Reset all the ptrs to articles following the above sort
	 */
	clear_art_ptrs();

	/*
	 *  The threading pointers need to be reset if re-threading
	 *	If using ref threading, revector the links back to the articles
	 */
	if (rethread || (group->attribute && group->attribute->thread_arts)) {

		for (i=0 ; i < top ; i++) {

			if (arts[i].thread != ART_EXPIRED)
				arts[i].thread = ART_NORMAL;

			arts[i].inthread = FALSE;

			/* Should never happen if tree is built properly */
			if (arts[i].refptr == 0) {
				my_fprintf(stderr, "\nError  : art->refptr is NULL\n");
				my_fprintf(stderr, "Artnum : %ld\n", arts[i].artnum);
				my_fprintf(stderr, "Subject: %s\n", arts[i].subject);
				my_fprintf(stderr, "From   : %s\n", arts[i].from);
			}
			assert(arts[i].refptr != 0);

			arts[i].refptr->article = i;
		}
	}

	/*
	 * Do the right thing according to the threading strategy
	 */
	if (group->attribute)
	switch (group->attribute->thread_arts) {
		case THREAD_NONE:
			return;

		case THREAD_SUBJ:
			thread_by_subject();
			return;

		case THREAD_REFS:
			thread_by_reference();
			return;

		case THREAD_BOTH:
			thread_by_reference();
			collate_subjects();
			return;

		default: /* not reached */
			return;
	}
}


void
sort_arts (
	int sort_art_type)
{
	switch (sort_art_type) {
		case SORT_BY_NOTHING:		/* don't sort at all */
			SortBy(artnum_comp);
			break;
		case SORT_BY_SUBJ_DESCEND:
		case SORT_BY_SUBJ_ASCEND:
			SortBy(subj_comp);
			break;
		case SORT_BY_FROM_DESCEND:
		case SORT_BY_FROM_ASCEND:
			SortBy(from_comp);
			break;
		case SORT_BY_DATE_DESCEND:
		case SORT_BY_DATE_ASCEND:
			SortBy(date_comp);
			break;
		case SORT_BY_SCORE_DESCEND:
		case SORT_BY_SCORE_ASCEND:
			SortBy(score_comp);
			break;
		default:
			break;
	}
}

static int
parse_headers (
	FILE *fp,
	struct t_article *h)
{
	char buf[HEADER_LEN];
	char buf2[HEADER_LEN];
	char art_from_addr[HEADER_LEN];
	char art_full_name[HEADER_LEN];
	char *ptr, *ptrline, *s;
	int flag = FALSE;				/* TODO what's this for now ? */
	int lineno = 0;
	int max_lineno = 25;
	t_bool got_archive, got_date, got_from, got_lines;
	t_bool got_msgid, got_received, got_refs, got_subject, got_xref;

	got_archive = got_date = got_from = got_lines = FALSE;
	got_msgid = got_received = got_refs = got_subject = got_xref = FALSE;

#if 1 /* join continuation headers */
	while ((ptr = fgets_hdr(buf, sizeof(buf), fp)) != NULL) {
		if (*buf != '\0' && buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = '\0';
#else
	while ((ptr = tin_fgets(buf, sizeof(buf), fp)) != NULL) {
#endif
		/*
		 * Look for end of headers - only applies when reading local spool
		 */
		if (!read_news_via_nntp && *ptr == '\0')
			break;

#if 0 /* TODO join broken continuation headers */
		for (ptrline = ptr; *ptr; ptr++) {
			if (*ptr == '\n') {
				/* Join continuation lines */
				if (*(ptr + 1) == ' ' || *(ptr + 1) == '\t') {
					*ptr = ' ';
					continue;
				}
				/* End of header? */
				if (*(ptr + 1) == '\n')
					*ptr = '\0';

				break;
			}
			if ((*(unsigned char *)ptr) < ' ')
				*ptr = ' ';

		}

		flag = *ptr;
		*ptr++ = '\0';
#endif /* 0 */

		ptrline = ptr;
		lineno++;		/* TODO is this needed ? */

		switch (toupper((unsigned char)*ptrline)) {
			case 'F':	/* From:  mandatory */
			case 'T':	/* To:    mandatory (mailbox) */
				if (!got_from) {
					if ((match_header (ptrline+1, "rom", buf2, (char*)0, HEADER_LEN) ||
					    match_header (ptrline+1, "o", buf2, (char*)0, HEADER_LEN)) &&
					    *buf2 != '\0') {
						parse_from (buf2, art_from_addr, art_full_name);
						h->from = hash_str (art_from_addr);
						if (art_full_name[0])
							h->name = hash_str (art_full_name);
						got_from = TRUE;
					}
				}
				break;
			case 'R':	/* References: optional */
				if (!got_refs) {
					if (match_header (ptrline+1, "eferences", buf2, (char*)0, HEADER_LEN) && *buf2 != '\0') {
						h->refs = my_strdup (buf2);
						got_refs = TRUE;
					}
				}

				/* Received:  If found its probably a mail article */
				if (!got_received) {
					if (match_header (ptrline+1, "eceived", buf2, (char*)0, HEADER_LEN) && *buf2 != '\0') {
						max_lineno = 50;
						got_received = TRUE;
					}
				}
				break;
			case 'S':	/* Subject:  mandatory */
				if (!got_subject) {
					if (match_header (ptrline+1, "ubject", buf2, (char*)0, HEADER_LEN) && *buf2 != '\0') {
						s = eat_re (buf2, FALSE);
						h->subject = hash_str (s);
						got_subject = TRUE;
					}
				}
				break;
			case 'D':	/* Date:  mandatory */
				if (!got_date) {
					if (match_header (ptrline+1, "ate", buf2, (char*)0, HEADER_LEN) && *buf2 != '\0') {
						h->date = parsedate (buf2, (struct _TIMEINFO *) 0);
						got_date = TRUE;
					}
				}
				break;
			case 'X':	/* Xref:  optional */
				if (!got_xref) {
					if (match_header (ptrline+1, "ref", buf2, (char*)0, HEADER_LEN) && *buf2 != '\0') {
						h->xref = my_strdup (buf2);
						got_xref = TRUE;
					}
				}
				break;
			case 'M':	/* Message-ID:  mandatory */
				if (!got_msgid) {
					if (match_header (ptrline+1, "essage-ID", buf2, (char*)0, HEADER_LEN) && *buf2 != '\0') {
						h->msgid = my_strdup (buf2);
						got_msgid = TRUE;
					}
				}
				break;
			case 'L':	/* Lines:  optional */
				if (!got_lines) {
					if (match_header (ptrline+1, "ines", buf2, (char*)0, HEADER_LEN) && *buf2 != '\0') {
						h->lines = atoi (buf2);
						got_lines = TRUE;
					}
				}
				break;
			case 'A':	/* Archive-name:  optional */
				if (match_header (ptrline+1, "rchive-name", buf2, (char*)0, HEADER_LEN) && *buf2 != '\0') {
					if ((s = strchr (buf2, '/')) != (char *) 0) {
						if (STRNCMPEQ(s+1, "part", 4) ||
						    STRNCMPEQ(s+1, "Part", 4)) {
							h->part = my_strdup (s+5);
							s = strrchr (h->part, '\n');
							if (s != (char *) 0)
								*s = '\0';
						} else if (STRNCMPEQ(s+1,"patch",5) || STRNCMPEQ(s+1,"Patch",5)) {
							h->patch = my_strdup (s+6);
							s = strrchr (h->patch, '\n');
							if (s != (char *) 0)
								*s = '\0';
						}
						if (h->part || h->patch) {
							s = buf2;
							while (*s && *s != '/')
								s++;
							*s = '\0';
							s = buf2;
							h->archive = hash_str (s);
							got_archive = TRUE;
						}
					}
				}
				break;
			default:
				break;
		} /* switch */

	} /* while */

	if (tin_errno != 0)
		return FALSE;

	/* TODO its possible some of these tests must go back to break; the main loop */
	if (!flag || (lineno > max_lineno) || got_archive) {
		/*
		 * The sonofRFC1036 states that the following hdrs are
		 * mandatory. It also states that Subject, Newsgroups
		 * and Path are too. Ho hum.
		 */
		if (got_from && got_date && got_msgid) {
			if (!got_subject)
				h->subject = hash_str ("<No subject>");

#ifdef DEBUG
			debug_print_header (h);
#endif
			return TRUE;
		} else
			return FALSE;
	}

	return FALSE;
}

/*
 *  Read in an Nov/Xover index file. Fields are separated by TAB.
 *  return the new value of 'top' or -1 if user quit partway.
 *
 *  Format:
 *    1.  article number (ie. 183)                [mandatory]
 *    2.  Subject: line  (ie. Which newsreader?)  [mandatory]
 *    3.  From: line     (ie. iain@ecrc.de)       [mandatory]
 *    4.  Date: line     (rfc822 format)          [mandatory]
 *    5.  MessageID:     (ie. <123@ether.net>)    [mandatory]
 *    6.  References:    (ie. <message-id> ....)  [mandatory]
 *    7.  Byte count     (Skipped - not used)     [mandatory]
 *    8.  Lines: line    (ie. 23)                 [optional]
 *    9.  Xref: line     (ie. alt.test:389)       [optional]
 *   10.  Archive-name:  (ie. widget/part01)      [optional]
 */

static int
iReadNovFile (
	struct t_group *group,
	long min,
	long max,
	int *expired)
{
	char	*p, *q;
	char	*buf;
	char	buf2[HEADER_LEN];
	char	art_full_name[HEADER_LEN];
	char	art_from_addr[HEADER_LEN];
	FILE	*fp;
	long	artnum;

	top = 0;
	last_read_article = 0L;
	*expired = 0;
/*
 *  Call ourself recursively to read the cached overview file, if we are
 *  supposed to be doing NNTP caching and we aren't already the recursive
 *  instance.  (Turn off read_news_via_nntp while we're recursing so we
 *  will know we're recursing while we're doing it.)  If there aren't
 *  any new articles, just return, without going on to read the NNTP
 *  overview file.  If we're going to read from NNTP, adjust min to the
 *  next article past last_read_article; there's no reason to read them
 *  from NNTP if they're cached locally.
 */
	if (cache_overview_files && read_news_via_nntp && xover_supported) {
		read_news_via_nntp = FALSE;
		iReadNovFile (group, min, max, expired);
		read_news_via_nntp = TRUE;
		if (last_read_article >= max)
			return top;
		if (last_read_article >= min)
			min = last_read_article + 1;
	}
	/*
	 * open the overview file (whether it be local or via nntp)
	 */
	if ((fp = open_xover_fp (group, "r", min, max)) == (FILE *) 0)
		return top;

	while ((buf = tin_fgets (buf2, sizeof(buf2), fp)) != (char *) 0) {

#ifdef DEBUG
		debug_nntp ("iReadNovFile", buf);
#endif

		if (top >= max_art)
			expand_art ();

		p = buf;

		/*
		 * read the article number
		 */
		artnum = atol (p);

		/* catches case of 1st line being groupname */
		if (artnum <= 0)
			continue;

		/*
		 * Check to make sure article in nov file has not expired in group
		 */
#if 0
my_printf ("artnum=[%ld] xmin=[%ld] xmax=[%ld]\n", artnum, group->xmin, group->xmax);
my_flush();
sleep(1);
#endif
		if (artnum < group->xmin) {
			(*expired)++;
			continue;
		}
		set_article (&arts[top]);
		arts[top].artnum = last_read_article = artnum;

		if ((q = strchr (p, '\t')) == (char *) 0) {
#ifdef DEBUG
			error_message ("Bad overview record (Artnum) '%s'", buf);
			debug_nntp ("iReadNovFile", "Bad overview record (Artnum)");
#endif
			continue;
		} else
			p = q + 1;

		/*
		 * READ subject
		 */
		if ((q = strchr (p, '\t')) == (char *) 0) {
#ifdef DEBUG
			error_message ("Bad overview record (Subject) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (Subject)");
#endif
			continue;
		} else
			*q = '\0';

		arts[top].subject = hash_str (eat_re(rfc1522_decode(p), FALSE));
		p = q + 1;

		/*
		 * READ author
		 */
		if ((q = strchr (p, '\t')) == (char *) 0) {
#ifdef DEBUG
			error_message ("Bad overview record (From) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (From)");
#endif
			continue;
		} else
			*q = '\0';

		parse_from (rfc1522_decode (p), art_from_addr, art_full_name);
		arts[top].from = hash_str (art_from_addr);

		if (art_full_name[0])
			arts[top].name = hash_str (art_full_name);

		p = q + 1;
		/*
		 * READ article date
		 */
		if ((q = strchr (p, '\t')) == (char *) 0) {
#ifdef DEBUG
			error_message ("Bad overview record (Date) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (Date)");
#endif
			continue;
		} else
			*q = '\0';

		arts[top].date = parsedate (p, (TIMEINFO*)0);
		p = q + 1;

		/*
		 * READ article message id
		 */
		q = strchr (p, '\t');
		if (q == (char *) 0 || p == q) {	/* Empty msgid's */
#ifdef DEBUG
			error_message ("Bad overview record (Msg-id) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (Msg-id)");
#endif
			continue;
		} else
			*q = '\0';

		if (*p)
			arts[top].msgid = my_strdup (p);
		else		/* TODO is no msg-id allowed in rfc ? */
			arts[top].msgid = '\0';

		p = q + 1;

		/*
		 * READ article references
		 */
		if ((q = strchr (p, '\t')) == (char *) 0) {
#ifdef DEBUG
			error_message ("Bad overview record (References) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (References)");
#endif
			continue;
		} else
			*q = '\0';

		if (*p)
			arts[top].refs = my_strdup (p);
		else
			arts[top].refs = '\0';

		p = q + 1;

		/*
		 * SKIP article bytes
		 */
		if ((q = strchr (p, '\t')) == (char *) 0) {
#ifdef DEBUG
			error_message ("Bad overview record (Bytes) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (Bytes)");
#endif
			continue;
		} else
			*q = '\0';

		p = (q == (char *) 0 ? (char *) 0 : q + 1);

		/*
		 * READ article lines
		 */
		if (p != (char *) 0) {
			if ((q = strchr (p, '\t')) != (char *) 0)
				*q = '\0';

			if(isdigit((unsigned char)*p))
				arts[top].lines = atoi (p);

			p = (q == (char *) 0 ? (char *) 0 : q + 1);
		}

		/*
		 * READ article xrefs
		 */
		if (p != (char *) 0 && xref_supported) {
			if ((q = strstr (p, "Xref: ")) == (char *) 0)
				q = strstr (p, "xref: ");

			if (q != (char *) 0) {
				p = q + 6;
				q = p;
				while (*q && *q != '\t')
					q++;

				*q = '\0';
				q = strrchr (p, '\n');
				if (q != (char *) 0)
					*q = '\0';

				q = p;
				while (*q && *q == ' ')
					q++;

				arts[top].xref = my_strdup (q);
			}
		}

		/*
		 * end of overview line processing
		 */
#ifdef DEBUG
		debug_print_header (&arts[top]);
#endif

#ifdef SHOW_PROGRESS
		if (artnum % MODULO_COUNT_NUM == 0)
			show_progress(msg, artnum, max);
#endif

		top++;
	}

	TIN_FCLOSE (fp);

	if (tin_errno != 0)
		return(-1);

	return top;
}

/*
 *  Write an Nov/Xover index file. Fields are separated by TAB.
 *
 *  Format:
 *    1.  article number (ie. 183)                [mandatory]
 *    2.  Subject: line  (ie. Which newsreader?)  [mandatory]
 *    3.  From: line     (ie. iain@ecrc.de)       [mandatory]
 *    4.  Date: line     (rfc822 format)          [mandatory]
 *    5.  MessageID:     (ie. <123@ether.net>)    [optional]
 *    6.  References:    (ie. <message-id> ....)  [mandatory]
 *    7.  Byte count     (Skipped - not used)     [mandatory]
 *    8.  Lines: line    (ie. 23)                 [mandatory]
 *    9.  Xref: line     (ie. alt.test:389)       [optional]
 *   10.  Archive-name:  (ie. widget/part01)      [optional]
 */

void
vWriteNovFile (
	struct t_group *psGrp)
{
	char	*pcNovFile;
	FILE	*hFp;
	int	iNum;
	struct	t_article *psArt;
	char	tmp[PATH_LEN];

	/*
	 * Don't write local index if we have XOVER, unless the user has
	 * asked for caching.
	 */
	if (xover_supported && ! cache_overview_files)
		return;

	set_tin_uid_gid ();

	/*
	 * setup the overview file (local only)
	 */

	/*
	 * don't write an overview file if R_OK returns a different name
	 * than W_OK, since we won't read it anyway.
	 */

	if((pcNovFile = pcFindNovFile (psGrp, R_OK))!=0)
		strcpy(tmp, pcNovFile);
	else
		strcpy(tmp, "");

	pcNovFile = pcFindNovFile (psGrp, W_OK);

	if(strcmp(tmp, pcNovFile)!=0) {
		set_real_uid_gid ();
		return;
	}
#ifdef DEBUG
	if (debug)
		error_message ("WRITE file=[%s]", pcNovFile);
#endif

	hFp = open_xover_fp (psGrp, "w", 0L, 0L);

	if (hFp == (FILE *) 0)
		error_message (txt_cannot_write_index, pcNovFile);
	else {
		if (psGrp->attribute && psGrp->attribute->sort_art_type != SORT_BY_NOTHING)
			SortBy(artnum_comp);

		if (!overview_index_filename)
			fprintf (hFp, "%s\n", psGrp->name);

		for (iNum = 0; iNum < top; iNum++) {
			psArt = &arts[iNum];

			if (psArt->thread != ART_EXPIRED && psArt->artnum >= psGrp->xmin) {
			 	fprintf (hFp, "%ld\t%s\t%s\t%s\t%s\t%s\t%d\t%d",
			 		psArt->artnum,
			 		psArt->subject,
			 		pcPrintFrom (psArt),
			 		pcPrintDate (psArt->date),
			 		(psArt->msgid ? psArt->msgid : ""),
			 		(psArt->refs ? psArt->refs : ""),
			 		0,	/* bytes */
			 		psArt->lines);

			 	if (psArt->xref)
				 	fprintf (hFp, "\tXref: %s", psArt->xref);

			 	if (psArt->archive)
				 	fprintf (hFp, "\tArchive-name: %s/%s%s",
				 		psArt->archive,
				 		(psArt->part ? "part" : "patch"),
				 		(psArt->part ? psArt->part : psArt->patch));

				fprintf (hFp, "\n");
			}
		}

		fclose (hFp);
		chmod (pcNovFile, (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH));

	}
	set_real_uid_gid ();
}



/*
 *  A complex little function to determine where to read the index file
 *  from and where to write it.
 *
 *  GROUP_TYPE_MAIL index files are read/written in ~/.tin/.mail
 *
 *  GROUP_TYPE_SAVE index files are read/written in ~/.tin/.save
 *
 *  GROUP_TYPE_NEWS index files are a little bit more complex :(
 *  READ:
 *    if  reading via NNTP
 *       path = TMPDIR/num.idx
 *       hash = FALSE
 *    else if  SPOOLDIR/group/name/.overview exists
 *       path = SPOOLDIR/group/name/.overview
 *    else if  SPOOLDIR/.news exists
 *       path = SPOOLDIR/.news
 *       hash = TRUE
 *    else
 *       path = ~/.tin/.news
 *       hash = TRUE
 *
 *  WRITE:
 *    if  SPOOLDIR/group/name writable
 *       path = SPOOLDIR/group/name/.overview
 *       hash = FALSE
 *    else if  SPOOLDIR/.news exists AND writable
 *       path = SPOOLDIR/.news
 *       hash = TRUE
 *    else
 *       path = ~/.tin/.news
 *       hash = TRUE
 *
 *  If hash = TRUE the index filename will be in format number.number.
 *  Hashing the groupname gets a number.  See if that #.1 file exists;
 *  if so, read first line. Is this the group we want?  If no, try #.2.
 *  Repeat until no such file or we find an existing file that matches
 *  our group. Return pointer to path or NULL if not found.
 */

char *
pcFindNovFile (
	struct t_group *psGrp,
	int iMode)
{
	char *pcPtr;
	const char *pcDir;
	char acBuf[PATH_LEN];
	FILE *hFp;
	int iHashFileName;
	int iNum;
	static char acNovFile[PATH_LEN];
	unsigned long lHash;

	if (psGrp == (struct t_group *) 0)
		return (char *) 0;

	overview_index_filename = FALSE;	/* Write groupname in nov file ? */

	iHashFileName = FALSE;
	pcDir = "";

	switch (psGrp->type) {
		case GROUP_TYPE_MAIL:
			pcDir = index_maildir;
			iHashFileName = TRUE;
			break;
		case GROUP_TYPE_SAVE:
			pcDir = index_savedir;
			iHashFileName = TRUE;
			break;
		case GROUP_TYPE_NEWS:
			if (read_news_via_nntp && xover_supported && ! cache_overview_files)
				sprintf (acNovFile, "%s%d.idx", TMPDIR, process_id);
			else {
				vMakeGrpPath (novrootdir, psGrp->name, acBuf);
				sprintf (acNovFile, "%s/%s", acBuf, OVERVIEW_FILE);
				if (iMode == R_OK || iMode == W_OK /*vb*/) {
					if (access (acNovFile, iMode) == 0)
						overview_index_filename = TRUE;
				}
				if (!overview_index_filename) {
					pcDir = index_newsdir;
					iHashFileName = TRUE;
				}
			}
			break;
		default: /* not reached */
			break;
	}

	if (iHashFileName) {
		lHash = hash_groupname (psGrp->name);

		for (iNum = 1;;iNum++) {

			sprintf (acNovFile, "%s/%lu.%d", pcDir, lHash, iNum);

			if ((hFp = fopen (acNovFile, "r")) == (FILE *) 0)
				return acNovFile;

			/*
			 * Don't follow, why should a zero length index file
			 * cause the write to fail ?
			 */
			if (fgets (acBuf, sizeof (acBuf), hFp) == (char *) 0) {
				fclose (hFp);
				return acNovFile;
			}
			fclose (hFp);

			pcPtr = strrchr (acBuf, '\n');
			if (pcPtr != (char *) 0)
				*pcPtr = '\0';

			if (STRCMPEQ(acBuf, psGrp->name))
				return acNovFile;

		}
	}

	return acNovFile;
}

/*
 *  Run the index file updater only for the groups we've loaded.
 */

void
do_update (void)
{
	char group_path[PATH_LEN];
	register int i, j;
	time_t beg_epoch, end_epoch;
	struct t_group *psGrp;
#ifdef INDEX_DAEMON
	char *pcNovFile;
	char buf[PATH_LEN];
	char novpath[PATH_LEN];
	time_t group_time, index_time;
	struct stat stinfo;
#endif

	if (verbose)
		time (&beg_epoch);

	/*
	 * load last updated times for each group (tind daemon only)
	 */
#ifdef INDEX_DAEMON
	read_group_times_file ();
#endif /* INDEX_DAEMON */

	/*
	 * loop through groups and update any required index files
	 */
	for (i = 0; i < group_top; i++) {
		psGrp = &active[my_group[i]];
		make_group_path (psGrp->name, group_path);
#ifdef INDEX_DAEMON
		joinpath (buf, psGrp->spooldir, group_path);
		joinpath (novpath, novrootdir, group_path);

		if (verbose) my_printf ("NOV path=[%s]\n", novpath);
		vCreatePath (novpath);
		if (stat (buf, &stinfo) == -1) {
			if (verbose)
				error_message (txt_cannot_stat_group, buf);
			continue;
		}

		group_time = stinfo.st_mtime;

		index_time = (time_t)0;
		pcNovFile = pcFindNovFile (psGrp, R_OK);
#ifdef DEBUG
		if (debug)
			error_message ("READ file=[%s]", pcNovFile);
#endif
		if (pcNovFile == (char *) 0)
			continue;

		if (stat (pcNovFile, &stinfo) == -1) {
			if (verbose)
				my_printf (txt_cannot_stat_index_file, psGrp->name, pcNovFile);

		} else {
			if (delete_index_file)
				unlink (pcNovFile);

			index_time = stinfo.st_mtime;
		}
#ifdef DEBUG
		if (debug == 2)
			my_printf ("[%s] idxtime=[%ld]  old=[%ld]  new=[%ld]\n",
				pcNovFile, index_time,
				psGrp->last_updated_time, group_time);
#endif
		if (index_time == (time_t)0 || psGrp->last_updated_time == (time_t)0 ||
		    (psGrp->last_updated_time > index_time) ||
		    (group_time > psGrp->last_updated_time) ||
		    purge_index_files)
			psGrp->last_updated_time = group_time;
		else
			continue;
#endif

		if (verbose) {
			my_printf ("%s %s\n", (catchup ? "Catchup" : "Updating"), psGrp->name);
			my_flush();
		}
		if (!index_group (psGrp))
			continue;

		if (catchup) {
			for (j = 0; j < top; j++)
				art_mark_read (psGrp, &arts[j]);
		}
	}

	/*
	 * save last updated times for each group (tind daemon only)
	 */
#ifdef INDEX_DAEMON
	write_group_times_file ();
#endif /* INDEX_DAEMON */

	if (verbose) {
		time (&end_epoch);
		wait_message (0, txt_catchup_update_info,
			(catchup ? "Caughtup" : "Updated"), group_top, IS_PLURAL(group_top), (int)(end_epoch - beg_epoch));
	}
}

static int
artnum_comp (
	t_comptype *p1,
	t_comptype *p2)
{
	const struct t_article *s1 = (const struct t_article *) p1;
	const struct t_article *s2 = (const struct t_article *) p2;

	/*
	 * s1->artnum less than s2->artnum
	 */
	if (s1->artnum < s2->artnum)
		return -1;

	/*
	 * s1->artnum greater than s2->artnum
	 */
	if (s1->artnum > s2->artnum)
		return 1;

	return 0;
}


static int
subj_comp (
	t_comptype *p1,
	t_comptype *p2)
{
	int retval;
	const struct t_article *s1 = (const struct t_article *) p1;
	const struct t_article *s2 = (const struct t_article *) p2;

	/*
	 * return result of strcmp (reversed for descending)
	 */
	return (CURR_GROUP.attribute->sort_art_type == SORT_BY_SUBJ_ASCEND
			? (retval = strcasecmp (s1->subject, s2->subject))
				? retval : ((s1->date - s2->date) > 0) ? 1 : -1
			: (retval = strcasecmp (s2->subject, s1->subject))
				? retval : ((s1->date - s2->date) > 0) ? 1 : -1);
}


static int
from_comp (
	t_comptype *p1,
	t_comptype *p2)
{
	int retval;
	const struct t_article *s1 = (const struct t_article *) p1;
	const struct t_article *s2 = (const struct t_article *) p2;

	/*
	 * return result of strcmp (reversed for descending)
	 */
	return (CURR_GROUP.attribute->sort_art_type == SORT_BY_FROM_ASCEND
			? (retval = strcasecmp (s1->from, s2->from))
				? retval : ((s1->date - s2->date) > 0) ? 1 : -1
			: (retval = strcasecmp (s2->from, s1->from))
				? retval : ((s1->date - s2->date) > 0) ? 1 : -1);
}


/*
 * Works like strcmp() for comparing time_t type values
 * Return codes:
 *  -1:		If p1 is before p2
 *   0:		If they are the same time
 *   1:		If p1 is after p2
 * If the sort order is _not_ DATE_ASCEND then the sense of the above
 * is reversed.
 */
static int
date_comp (
	t_comptype *p1,
	t_comptype *p2)
{
	const struct t_article *s1 = (const struct t_article *) p1;
	const struct t_article *s2 = (const struct t_article *) p2;

	if (CURR_GROUP.attribute->sort_art_type == SORT_BY_DATE_ASCEND) {
		/*
		 * s1->date less than s2->date
		 */
		if (s1->date < s2->date)
			return -1;

		/*
		 * s1->date greater than s2->date
		 */
		if (s1->date > s2->date)
			return 1;

	} else {
		/*
		 * s2->date less than s1->date
		 */
		if (s2->date < s1->date)
			return -1;

		/*
		 * s2->date greater than s1->date
		 */
		if (s2->date > s1->date)
			return 1;

	}
	return 0;
}

/*
 * Same again, but for art[].score
 */
static int
score_comp (
	t_comptype *p1,
	t_comptype *p2)
{
	const struct t_article *s1 = (const struct t_article *) p1;
	const struct t_article *s2 = (const struct t_article *) p2;

	if (CURR_GROUP.attribute->sort_art_type == SORT_BY_SCORE_ASCEND) {
		if (s1->score < s2->score)
			return -1;

		if (s1->score > s2->score)
			return 1;
	} else {
		if (s2->score < s1->score)
			return -1;

		if (s2->score > s1->score)
			return 1;
	}

	return 0;
}

void
set_article (
	struct t_article *art)
{
	art->subject	= (char *) 0;
	art->from	= (char *) 0;
	art->name	= (char *) 0;
	art->date	= 0L;
	art->xref	= (char *) 0;
	art->msgid	= (char *) 0;
	art->refs	= (char *) 0;
	art->refptr	= (struct t_msgid *) 0;
	art->lines	= -1;
	art->archive	= (char *) 0;
	art->part	= (char *) 0;
	art->patch	= (char *) 0;
	art->thread	= ART_EXPIRED;
	art->status	= ART_UNREAD;
	art->inthread	= FALSE;
	art->killed	= FALSE;
	art->tagged	= FALSE;
	art->selected	= FALSE;
	art->zombie	= FALSE;
	art->delete	= FALSE;
	art->inrange	= FALSE;
}


/*
 * Do a binary chop to see if 'art' (an article number) exists in arts[]
 * Return index into arts[] or -1
 */
static int
valid_artnum (
	long art)
{
	register int prev, range;
	register int dctop = top;
	register int cur = 1;

	while (dctop /= 2)
		cur = cur << 1;

	range = cur / 2;
	cur--;

	forever {
		if (arts[cur].artnum == art)
			return cur;

		prev = cur;
		if (arts[cur].artnum < art)
			cur = cur + range;
		else
			cur = cur - range;

		if (prev == cur)
			return -1;

		if (cur >= top)
			cur = top - 1;

		range = range / 2;
	}
}

static void
print_expired_arts (
	int num_expired)
{
	int i;

	if (cmd_line && verbose) {
#ifdef DEBUG
		if (debug)
			my_printf ("Expired Index Arts=[%d]", num_expired);
#endif
		for (i = 0; i < num_expired; i++)
			my_fputc ('P', stdout);

		if (num_expired)
			my_flush();
	}
}

static char *
pcPrintDate (
	time_t	lSecs)
{
	static	char acDate[25];
	struct	tm *psTm;

	static const char *const months_a[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
	};

	psTm = localtime (&lSecs);
	sprintf(acDate, "%02d %s %04d %02d:%02d:%02d",
			psTm->tm_mday,
			months_a[psTm->tm_mon],
			1900+psTm->tm_year,
			psTm->tm_hour, psTm->tm_min, psTm->tm_sec);

	return acDate;
}

static char *
pcPrintFrom (
	struct t_article *psArt)
{
	static	char acFrom[PATH_LEN];

	*acFrom = '\0';

	if (psArt->name != (char *) 0)
		sprintf (acFrom, "%s <%s>", psArt->name, psArt->from);
	else
		strcpy (acFrom, psArt->from);

	return acFrom;
}

#ifdef INDEX_DAEMON
static void
vCreatePath (
	char *pcPath)
{
	char acCmd[LEN];

	/* HACK HACK HACK to get nov files off my overfull news partition !!!*/
	sprintf (acCmd, "/bin/mkdir -p %s", pcPath);
	my_printf ("CREATE Path=[%s]\n", acCmd);
	system (acCmd);
	}
#endif /* INDEX_DAEMON */
                  
