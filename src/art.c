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
static int parse_headers (char *buf, struct t_article *h);
static int read_group (struct t_group *group, char *group_path, int *pcount);
static int subj_comp (t_comptype *p1, t_comptype *p2);
static int valid_artnum (long art);
static void print_expired_arts (int num_expired);

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
			if (IGNORE_ART(i) || arts[i].inthread != FALSE) {
				continue;
			}
			if (top_base >= max_art) {
				expand_art ();
			}
			if (arts[i].status == ART_UNREAD) {
				base[top_base++] = i;
			} else {
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
			if (IGNORE_ART(i) || arts[i].inthread != FALSE) {
				continue;
			}
			if (top_base >= max_art) {
				expand_art ();
			}
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

	if (!update) {
		sprintf (msg, txt_group, group->name);
		wait_message (msg);
	}

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
#ifdef	PROFILE
	BegStopWatch("setup_base");
#endif	/* PROFILE */

	if (setup_hard_base (group, group_path) < 0)
		return(FALSE);

#ifdef	PROFILE
	EndStopWatch();
	PrintStopWatch();
#endif	/* PROFILE */

#ifdef DEBUG_NEWSRC
	debug_print_comment ("Before iReadNovFile");
	debug_print_bitmap (group, NULL);
#endif

	min = top_base ? base[0] : group->xmin;
	max = top_base ? base[top_base-1] : min - 1;

	/*
	 * Read in the existing index via XOVER or the index file
	 */
	if (iReadNovFile (group, min, max, &expired) == -1) {
		/* user aborted indexing */
		set_alarm_clock_on ();
		return (FALSE);
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
		return (FALSE);
	}

	/*
	 * Do this before calling art_mark_read if you want
	 * the unread count to be correct.
	 */

	parse_unread_arts (group);

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

	if (expired || modified)
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

	if ((modified || filtered) && !update)
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
	char *ptr, buf[PATH_LEN];
	char progress[PATH_LEN];
	char temp[16];
	int count = 0;
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

	sprintf (progress, txt_group, group->name);

	/*
	 *  Count num of arts to index so the user has an idea of index time
	 */
	for (i = 0; i < top_base; i++) {
		if (base[i] <= last_read_article || valid_artnum (base[i]) >= 0) {
			continue;
		}
		total++;
	}

	/*
	 * Reset the next article number index (for when HEAD fails)
	 */
	temp[0] = '\0';
	head_next = -1;
	for (i = 0; i < top_base; i++) {	/* for each article # */
		art = base[i];

		/*
		 *  Do we already have this article in our index?  Change
		 *  arts[].thread from ART_EXPIRED to ART_NORMAL and skip
		 *  reading the header.
		 */
		if ((respnum = valid_artnum (art)) >= 0 || art <= last_read_article) {
			if (respnum >= 0) {
				arts[respnum].thread = ART_NORMAL;
			}
			continue;
		}

		if ((ptr = open_art_header (art)) == (char *) 0) {
			continue;
		}

		/*
		 * we've modified the index so it will need to be re-written
		 */
		modified = TRUE;

		/*
		 *  Add article to arts[]
		 */
		if (top >= max_art) {
			expand_art();
		}

		set_article (&arts[top]);
		arts[top].artnum = art;
		arts[top].thread = ART_NORMAL;

		if (!parse_headers (ptr, &arts[top])) {
			sprintf (buf, "FAILED parse_header(%ld)", art);
#ifdef DEBUG
			debug_nntp ("read_group", buf);
#endif
			continue;
		}

		last_read_article = arts[top].artnum;	/* used if arts are killed */
		top++;

		if (++count % MODULO_COUNT_NUM == 0 && !update) {
			if (input_pending (0)) {
				buf[0] = ReadCh();
				if (buf[0] == ESC || buf[0] == 'q' || buf[0] == 'Q') {
					if (prompt_yn (cLINES, txt_abort_indexing, TRUE) == 1) {
						chdir (dir);
						return -1;
					} else {
						show_progress (temp, progress, 0, total);
					}
				}
			}
			show_progress (temp, progress, count, total);
		}
		if (update && verbose) {
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
	register int i;		/* gcc, at least, will ignore 'register' as */
	register int j;		/* it can do a better job itself */
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
			if (!IGNORE_ART(i) && arts[i].inthread == FALSE &&
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
			wait_message (txt_unthreading_arts);
		else
			wait_message (txt_threading_arts);
	}

#if 0
	if (debug == 2) {
		sprintf (msg, "rethread=[%d]  thread_arts=[%d]  attr_thread_arts=[%d]",
				rethread, default_thread_arts, group->attribute->thread_arts);
		error_message (msg, "");
	}
#endif

	/*
	 * Sort all the articles by date ascending or descending.
	 * When find_base() is called, the bases are created ordered
	 * on arts[] and so the base messages under all threading systems
	 * will be date sorted.
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
		default:
			break;
	}
}

static int
parse_headers (
	char *buf,
	struct t_article *h)
{
	char buf2[HEADER_LEN];
	char art_from_addr[HEADER_LEN];
	char art_full_name[HEADER_LEN];
	char *ptr, *ptrline, *s;
	int flag, n;
	int lineno = 0;
	int max_lineno = 25;
	int got_archive = FALSE;
	int got_date = FALSE;
	int got_from = FALSE;
	int got_lines = FALSE;
	int got_msgid = FALSE;
	int got_received = FALSE;
	int got_refs = FALSE;
	int got_subject = FALSE;
	int got_xref = FALSE;

	if ((n = strlen (buf))== 0) {
		return FALSE;
	}

	buf[--n] = '\0';

	ptr = buf;

	forever {
		for (ptrline = ptr; *ptr && *ptr != '\n'; ptr++) {
			if (((*ptr) & 0xFF) < ' ') {
				if (*ptr == '\n' && *(ptr+1) != '\n')
					*ptr = 1;
				else
					*ptr = ' ';
			}
		}
		flag = *ptr;
		*ptr++ = '\0';
		lineno++;

		switch (toupper((unsigned char)*ptrline)) {
			case 'F':	/* From:  mandatory */
			case 'T':	/* To:    mandatory (mailbox) */
				if (!got_from) {
					if ((match_header (ptrline+1, "rom", buf2, (char*)0, HEADER_LEN) ||
					    match_header (ptrline+1, "o", buf2, (char*)0, HEADER_LEN)) &&
					    *buf2 != '\0') {
						parse_from (buf2, art_from_addr, art_full_name);
						h->from = hash_str (art_from_addr);
						if (art_full_name[0]) {
							h->name = hash_str (art_full_name);
						}
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
							if (s != (char *) 0) {
								*s = '\0';
							}
						} else if (STRNCMPEQ(s+1,"patch",5) ||
								   STRNCMPEQ(s+1,"Patch",5)) {
							h->patch = my_strdup (s+6);
							s = strrchr (h->patch, '\n');
							if (s != (char *) 0) {
								*s = '\0';
							}
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

		if (!flag || lineno > max_lineno || got_archive) {
			/*
			 * The sonofRFC1036 states that the following hdrs are
			 * mandatory. It also states that Subject, Newsgroups
			 * and Path are too.
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
	} /* forever */

	/* NOTREACHED */
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
 	char	*p, *q, *s;
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
	 * open the overview file (whether it be local or via nntp)
	 */
	if ((fp = open_xover_fp (group, "r", min, max)) == (FILE *) 0) {
		return top;
	}

	while ((buf = safe_fgets (fp)) != (char *) 0) {

		if (input_pending (0)) {
			buf[0] = ReadCh();
			if (buf[0] == ESC || buf[0] == 'q' || buf[0] == 'Q') {
				if (prompt_yn (cLINES, txt_abort_indexing, TRUE) == 1)
					return(-1);
			}
		}

#ifdef DEBUG
		debug_nntp ("iReadNovFile", buf);
#endif
		if (STRCMPEQ(buf, ".")) {
			free (buf);
			break;
		}

		if (top >= max_art) {
			expand_art ();
		}

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
		q = strchr (p, '\t');
		if (q == (char *) 0) {
#ifdef DEBUG
			error_message ("Bad overview record (Artnum) '%s'", buf);
			debug_nntp ("iReadNovFile", "Bad overview record (Artnum)");
#endif
			free (buf);
			continue;
		} else {
			p = q + 1;
		}

		/*
		 * READ subject
		 */
		q = strchr (p, '\t');
		if (q == (char *) 0) {
#ifdef DEBUG
			error_message ("Bad overview record (Subject) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (Subject)");
#endif
			free (buf);
			continue;
		} else {
			*q = '\0';
		}
		arts[top].subject = hash_str (eat_re(rfc1522_decode(p),FALSE));
		p = q + 1;

		/*
		 * READ author
		 */
		q = strchr (p, '\t');
		if (q == (char *) 0) {
#ifdef DEBUG
			error_message ("Bad overview record (From) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (From)");
#endif
			free (buf);
			continue;
		} else {
			*q = '\0';
		}
		parse_from (rfc1522_decode (p), art_from_addr, art_full_name);
		arts[top].from = hash_str (art_from_addr);
		if (art_full_name[0]) {
			arts[top].name = hash_str (art_full_name);
		}
		p = q + 1;
		/*
		 * READ article date
		 */
		q = strchr (p, '\t');
		if (q == (char *) 0) {
#ifdef DEBUG
			error_message ("Bad overview record (Date) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (Date)");
#endif
			free (buf);
			continue;
		} else {
			*q = '\0';
		}
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
			free (buf);
			continue;
		} else {
			*q = '\0';
		}
		if (*p)
			arts[top].msgid = my_strdup (p);
		else
			arts[top].msgid = '\0';

		p = q + 1;

		/*
		 * READ article references
		 */
		q = strchr (p, '\t');
		if (q == (char *) 0) {
#ifdef DEBUG
			error_message ("Bad overview record (References) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (References)");
#endif
			free (buf);
			continue;
		} else {
			*q = '\0';
		}
		if (*p)
			arts[top].refs = my_strdup (p);
		else
			arts[top].refs = '\0';

		p = q + 1;

		/*
		 * SKIP article bytes
		 */
		q = strchr (p, '\t');
		if (q == (char *) 0) {
#ifdef DEBUG
			error_message ("Bad overview record (Bytes) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (Bytes)");
#endif
			free (buf);
			continue;
		} else {
			*q = '\0';
		}
		p = (q == (char *) 0 ? (char *) 0 : q + 1);

		/*
		 * READ article lines
		 */
		if (p != (char *) 0) {
			q = strchr (p, '\t');
			if (q != (char *) 0)
				*q = '\0';
			if(isdigit((unsigned char)*p))
				arts[top].lines = atoi (p);
			p = (q == (char *) 0 ? (char *) 0 : q + 1);
		}

		/*
		 * READ article xrefs
		 */
		if (p != (char *) 0 && xref_supported) {
			q = strstr (p, "Xref: ");
			if (q == (char *) 0) {
				q = strstr (p, "xref: ");
			}
			if (q != (char *) 0) {
				p = q + 6;
				q = p;
				while (*q && *q != '\t') {
					q++;
				}
				*q = '\0';
				q = strrchr (p, '\n');
				if (q != (char *) 0) {
					*q = '\0';
				}
				q = p;
				while (*q && *q == ' ') {
					q++;
				}
				arts[top].xref = my_strdup (q);
			}
		}

		/*
		 * READ article archive-name
		 */
		if (p != (char *) 0) {
			q = strstr (p, "Archive-name: ");
			if (q != (char *) 0) {
				p = q + 14;
				q = p;
				while (*q && *q != '\t') {
					q++;
				}
				*q = '\0';
				q = strrchr (p, '\n');
				if (q != (char *) 0) {
					*q = '\0';
				}
				q = p;
				while (*q && *q == ' ') {
					q++;
				}
				if ((s = strchr (q, '/')) != (char *) 0) {
					if (STRNCMPEQ(s+1, "part", 4) ||
					    STRNCMPEQ(s+1, "Part", 4)) {
						arts[top].part = my_strdup (s+5);
					} else if (STRNCMPEQ(s+1, "patch", 5) ||
						   STRNCMPEQ(s+1, "Patch", 5)) {
						arts[top].patch = my_strdup (s+6);
					}
					if (arts[top].part || arts[top].patch) {
						strcpy (buf2, q);
						s = buf2;
						while (*s && *s != '/')
							s++;
						*s = '\0';
						s = buf2;
						arts[top].archive = hash_str (s);
					}
				}
			}
		}

		/*
		 * end of overview line processing
		 */
#ifdef DEBUG
		debug_print_header (&arts[top]);
#endif
		top++;
		free (buf);
	}
 	fclose (fp);

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

	if (xover_supported)		/* Don't write local index if we have XOVER */
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

	if (debug) {
		error_message ("WRITE file=[%s]", pcNovFile);
	}

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

			 	if (psArt->xref) {
				 	fprintf (hFp, "\tXref: %s", psArt->xref);
				}
			 	if (psArt->archive) {
				 	fprintf (hFp, "\tArchive-name: %s/%s%s",
				 		psArt->archive,
				 		(psArt->part ? "part" : "patch"),
				 		(psArt->part ? psArt->part : psArt->patch));
				}
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

	if (psGrp == (struct t_group *) 0) {
		return (char *) 0;
	}

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
			if (read_news_via_nntp && xover_supported) {
				sprintf (acNovFile, "%s%d.idx", TMPDIR, process_id);
			} else {
				vMakeGrpPath (novrootdir, psGrp->name, acBuf);
				sprintf (acNovFile, "%s/%s", acBuf, OVERVIEW_FILE);
				if (iMode == R_OK || iMode == W_OK /*vb*/) {
					if (access (acNovFile, iMode) == 0) {
						overview_index_filename = TRUE;
					}
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

			if ((hFp = fopen (acNovFile, "r")) == (FILE *) 0) {
				return acNovFile;
			}

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
			if (pcPtr != (char *) 0) {
				*pcPtr = '\0';
			}

			if (STRCMPEQ(acBuf, psGrp->name)) {
				return acNovFile;
			}
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

	if (verbose) {
		time (&beg_epoch);
	}

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
			if (verbose) {
				error_message (txt_cannot_stat_group, buf);
			}
			continue;
		}

		group_time = stinfo.st_mtime;

		index_time = (time_t)0;
		pcNovFile = pcFindNovFile (psGrp, R_OK);
		if (debug) {
			error_message ("READ file=[%s]", pcNovFile);
		}
		if (pcNovFile == (char *) 0) {
			continue;
		}

		if (stat (pcNovFile, &stinfo) == -1) {
			if (verbose) {
				my_printf (txt_cannot_stat_index_file, psGrp->name, pcNovFile);
			}
		} else {
			if (delete_index_file) {
				unlink (pcNovFile);
			}
			index_time = stinfo.st_mtime;
		}

		if (debug == 2) {
			my_printf ("[%s] idxtime=[%ld]  old=[%ld]  new=[%ld]\n",
				pcNovFile, index_time,
				psGrp->last_updated_time, group_time);
		}

		if (index_time == (time_t)0 || psGrp->last_updated_time == (time_t)0 ||
		    (psGrp->last_updated_time > index_time) ||
		    (group_time > psGrp->last_updated_time) ||
		    purge_index_files) {
			psGrp->last_updated_time = group_time;
		} else {
			continue;
		}
#endif

		if (verbose) {
			my_printf ("%s %s\n", (catchup ? "Catchup" : "Updating"), psGrp->name);
			my_flush();
		}
		if (!index_group (psGrp)) {
			continue;
		}
		if (catchup) {
			for (j = 0; j < top; j++) {
				art_mark_read (psGrp, &arts[j]);
			}
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
		sprintf (msg, txt_catchup_update_info,
			(catchup ? "Caughtup" : "Updated"),
			group_top, IS_PLURAL(group_top),
			(int)(end_epoch - beg_epoch));
		wait_message (msg);
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
	if (s1->artnum < s2->artnum) {
		return -1;
	}

	/*
	 * s1->artnum greater than s2->artnum
	 */
	if (s1->artnum > s2->artnum) {
		return 1;
	}
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
		if (s1->date < s2->date) {
			return -1;
		}
		/*
		 * s1->date greater than s2->date
		 */
		if (s1->date > s2->date) {
			return 1;
		}
	} else {
		/*
		 * s2->date less than s1->date
		 */
		if (s2->date < s1->date) {
			return -1;
		}
		/*
		 * s2->date greater than s1->date
		 */
		if (s2->date > s1->date) {
			return 1;
		}
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

	while (dctop /= 2) {
		cur = cur << 1;
	}
	range = cur / 2;
	cur--;

	forever {
		if (arts[cur].artnum == art) {
			return cur;
		}
		prev = cur;
		if (arts[cur].artnum < art) {
			cur = cur + range;
		} else {
			cur = cur - range;
		}
		if (prev == cur) {
			return -1;
		}
		if (cur >= top) {
			cur = top - 1;
		}
		range = range / 2;
	}
}

/*
 * safe_fgets
 *
 * Return the next line from a file, regardless of how long it is.
 * Thanks to <emcmanus@gr.osf.org> for hugs, support and bug fixes.
 */

char *
safe_fgets (
	FILE *fp)
{
	char *buf	= 0;
	char *temp	= 0;
	int	next	= 0;
	int	chunk	= 256;

	buf = (char *) malloc (chunk * sizeof(char));

	forever {
		if (fgets (buf + next, chunk, fp) == 0) {
			if (next) {
				return buf;
			}
			free (buf);
			return 0;
		}

		if (buf[strlen(buf)-1] == '\n') {
			return buf;
		}

		next += chunk - 1;

		temp = (char *) realloc (buf, (next+chunk) * sizeof(char));
		if (temp == 0) {
			free (buf);
			return 0;
		}
		buf = temp;
	}
}


static void
print_expired_arts (
	int num_expired)
{
	int i;

	if (cmd_line && verbose) {
		if (debug) {
			my_printf ("Expired Index Arts=[%d]", num_expired);
		}
		for (i = 0; i < num_expired; i++) {
			my_fputc ('P', stdout);
		}
		if (num_expired) {
			my_flush();
		}
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

	if (psArt->name != (char *) 0) {
		sprintf (acFrom, "%s <%s>", psArt->name, psArt->from);
	} else {
		strcpy (acFrom, psArt->from);
	}

	return acFrom;
}
