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
#include	"stpwatch.h"
#include        "rfc1522.h"

#define SortBy(func) qsort ((char *) arts, (size_t)top, sizeof (struct t_article), func);
#define CorruptIndex(n)  error = n; goto corrupt_index;

char *glob_art_group;
static long last_read_article;
static int overview_index_filename = FALSE;

static int read_group P_((struct t_group *group, char *group_path, int *pcount));
static int artnum_comp P_((t_comptype *p1, t_comptype *p2));
static int subj_comp P_((t_comptype *p1, t_comptype *p2));
static int from_comp P_((t_comptype *p1, t_comptype *p2));
static int date_comp P_((t_comptype *p1, t_comptype *p2));
static char *pcPrintDate P_((long lSecs));
static char *pcPrintFrom P_((struct t_article *psArt));
static void print_expired_arts P_((int num_expired));
#ifdef INDEX_DAEMON
static void vCreatePath P_((char *pcPath));
#endif

/*
 *  Construct the pointers to the basenotes of each thread
 *  arts[] contains every article in the group.  inthread is
 *  set on each article that is after the first article in the
 *  thread.  Articles which have been expired have their thread
 *  set to -2 (ART_EXPIRED).
 */

void
find_base (group)
	struct t_group *group;
{
	register int i;
	register int j;

	top_base = 0;

	debug_print_arts ();

	if (group->attribute->show_only_unread) {
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
				for (j = i ; j >= 0 ; j = arts[j].thread) {
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
 *  Count the number of non-expired articles in arts[]
 */

#if 0
int
num_of_arts ()
{
	int sum = 0;
	register int i;

	for (i = 0; i < top; i++) {
		if (arts[i].thread != ART_EXPIRED) {
			sum++;
		}
	}

	return sum;
}
#endif

/*
 *  Return TRUE if arts[] contains any expired articles
 *  (articles we have an entry for which don't have a 
 *  corresponding article file in the spool directory)
 */

#if 0
int
purge_needed (group_path)
	char *group_path;
{
	register int i;

	for (i = 0; i < top; i++) {
		if (arts[i].thread == ART_EXPIRED) {
			return TRUE;
		}
	}
	
	return FALSE;
}
#endif

/*
 *  Main group indexing routine.
 *
 *  Will read any existing index, create or incrementally update
 *  the index by looking at the articles in the spool directory,
 *  and attempt to write a new index if necessary.
 */

int
index_group (group)
	struct t_group *group;
{
	char group_path[PATH_LEN];
	int artcount;
	int count;
	int expired;
	int filtered;
	int modified;
	long min;
	long max;
	register int i;

	if (group != (struct t_group *) 0) {
		if (! update) {
			sprintf (msg, txt_group, group->name);
			wait_message (msg);
		}

		make_group_path (group->name, group_path);
		glob_art_group = group->name;

		set_alarm_clock_off ();

		set_signals_art ();

		hash_reclaim ();
		free_art_array ();

		/*
		 *  Load articles within min..max from xover index file if it exists
		 *  and then create base[] article numbers from loaded articles.
		 *  If nov file does not exist then create base[] with setup_base().
		 */
BegStopWatch("setup_base");

		artcount = setup_hard_base (group, group_path);

EndStopWatch();
PrintStopWatch();


debug_print_comment ("Before iReadNovFile");
debug_print_bitmap (group, NULL);

		min = top_base ? base[0] : group->xmin;
		max = top_base ? base[top_base-1] : min - 1;
/*
		min = group->xmin;
		max = group->xmax;

sprintf (msg, "Group %s range=[%ld-%ld]", group->name, min, max);
*/
		artcount = iReadNovFile (group, min, max, &expired);
/*
		if (artcount) {
			artcount = setup_soft_base (group);
		} else {
			artcount = setup_hard_base (group, group_path);
		}
*/
		if (expired) {
			print_expired_arts (expired);
		}

		/*
		 *  add any articles to arts[] that are new or were killed
		 */
		modified = read_group (group, group_path, &count);
		if (modified == -1) {  
			/*
			 *  user aborted indexing
			 */
			set_alarm_clock_on ();
			return FALSE;
		}

		/*
		 * Do this before calling art_mark_read if you want
		 * the unread count to be correct.
		 */

		parse_unread_arts (group);

		filtered = filter_articles (group);

		/*
		 * Stat all articles to see if any have expired
		 */
		for (i = 0; i < top; i++) {
			if (arts[i].thread == ART_EXPIRED) {
				expired = 1;
				debug_print_comment ("art.c: index_group() purging...");
				art_mark_read (group, &arts[i]);
				print_expired_arts (expired);
			}
		}

		if (expired || modified) {
			vWriteNovFile (group);
		}
		
		if ((expired || count) && cmd_line && verbose) {
		    my_fputc ('\n', stdout);
		    fflush (stdout);
		}

		rfc1522_decode_all_headers();

BegStopWatch("make_thread");
		make_threads (group, FALSE);
EndStopWatch();
PrintStopWatch();

		find_base (group);
	
		if ((modified || filtered) && ! update) {
			clear_message ();
		}
		set_alarm_clock_on ();
	}

	return TRUE;
}

/*
 *  Index a group.  Assumes any existing index has already been
 *  loaded. Return values are:
 *    TRUE   loaded index and modified it
 *    FALSE  loaded index but not modified
 *    -1     user aborted indexing operation
 */

static int
read_group (group, group_path, pcount)
	struct t_group *group;
	char *group_path;
	int *pcount;
{
	char *ptr, buf[PATH_LEN];
	char progress[PATH_LEN];
	char temp[16];
	extern long head_next;
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
	get_cwd (dir);
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
		 *  thread from (ART_EXPIRED) to (ART_NORMAL) if so and 
		 *  skip the header eating.
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

		if (! parse_headers (ptr, &arts[top])) {
			sprintf (buf, "FAILED parse_header(%ld)", art);
			debug_nntp ("read_group", buf);
			continue;
		}

		last_read_article = arts[top].artnum;	/* used if arts are killed */
		top++;

		if (++count % MODULO_COUNT_NUM == 0 && ! update) {
			if (input_pending ()) {
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
			fflush (stdout);
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
 *  Go through the articles in arts[] and use .thread to snake threads
 *  through them.  Use the subject line to construct threads.  The
 *  first article in a thread should have .inthread set to FALSE, the
 *  rest TRUE.  Only do unexprired articles we haven't visited yet
 *  (arts[].thread == -1 ART_NORMAL).
 */

void
make_threads (group, rethread)
	struct t_group *group;
	int rethread;
{
	register int i;
	register int j;

	if (! cmd_line) {
		if (group->attribute->thread_arts) {
			wait_message (txt_threading_arts);
		} else {
			wait_message (txt_unthreading_arts);
		}
	}
/*
if (debug == 2) {
	sprintf (msg, "rethread=[%d]  thread_arts=[%d]  attr_thread_arts=[%d]", 	
		rethread, default_thread_arts, group->attribute->thread_arts);
	error_message (msg, "");
}
*/
	/*
	 *  arts[].thread & arts[].inthread need to be reset if re-threading
	 */
	if (rethread || group->attribute->thread_arts) {
/*
if (debug == 2) {
	error_message("Resetting .thread & .inthread", "");
}
*/
		for (i=0 ; i < top ; i++) {
			if (arts[i].thread != ART_EXPIRED) arts[i].thread = ART_NORMAL;
			arts[i].inthread = FALSE;
		}
	}

	sort_arts (group->attribute->sort_art_type);
	
/*
if (debug == 2) {
	error_message("Threading", "");
}
*/
	if (group->attribute->thread_arts == 0 || default_thread_arts == 0) {
		return;
	}
	for (i = 0; i < top; i++) {
#ifndef OLD_THREADING
		int *aptr; 
#endif
		if (arts[i].thread != ART_NORMAL || IGNORE_ART(i)) {
			continue;
		}	
#ifndef OLD_THREADING
		aptr = (int*)arts[i].subject;
		aptr -=2;
	        j = *aptr;	
		if (j != -1 && j < i) {
		  if (! IGNORE_ART(i) && arts[i].inthread == FALSE &&
			   ((arts[i].subject == arts[j].subject) ||
			   ((arts[i].part || arts[i].patch) &&
			   arts[i].archive == arts[j].archive))) {
		       arts[j].thread = i;
		       arts[i].inthread = TRUE;
                   }
		} 
		*aptr = i; 
#else
		for (j = i+1; j < top; j++) {
			if (! IGNORE_ART(j) && arts[j].inthread == FALSE &&
			   ((arts[i].subject == arts[j].subject) ||
			   ((arts[i].part || arts[i].patch) &&
			   arts[i].archive == arts[j].archive))) {
				arts[i].thread = j;
				arts[j].inthread = TRUE;
				break;
			}
		}
#endif
	}
}


void
sort_arts (sort_art_type)
	int sort_art_type;
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

int
parse_headers (buf, h)
	char *buf;
	struct t_article *h;
{
	char buf2[HEADER_LEN];
	char art_from_addr[LEN];
	char art_full_name[LEN];
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

	n = strlen (buf);
	
	if (n == 0) {
		return FALSE;
	}

	buf[n-1] = '\0';
  	
	ptr = buf;

	forever {
		for (ptrline = ptr; *ptr && (*ptr != '\n' || (isspace(*(ptr+1)) && *(ptr+1)!= '\n')); ptr++) {
			if (((*ptr) & 0xFF) < ' ') {
				if (*ptr=='\n'&&isspace(*(ptr+1))&&*(ptr+1)!='\n') *ptr=1;
				else *ptr = ' ';
			}
		}
		flag = *ptr;
		*ptr++ = '\0';
		lineno++;

		switch (toupper(*ptrline)) {
			case 'F':	/* From:  mandatory */
			case 'T':	/* To:    mandatory (mailbox) */
				if (! got_from) {
					if (match_header (ptrline, "From", buf2, HEADER_LEN) ||
					    match_header (ptrline, "To", buf2, HEADER_LEN)) {
						parse_from (buf2, art_from_addr, art_full_name);
						h->from = hash_str (art_from_addr);
						if (art_full_name[0]) {
							h->name = hash_str (art_full_name);
						}
						got_from = TRUE;
					}
				}
				break;
			case 'R':	/* Received:  If found its probably a mail article */
				if (! got_refs) {
					if (match_header (ptrline, "References", buf2, HEADER_LEN)) {
						s = buf2;
						while (*s && *s == ' ') {
							s++;
						}
						h->refs = parse_references (s);
						got_refs = TRUE;
					}
				}
				if (! got_received) {
					if (match_header (ptrline, "Received", buf2, HEADER_LEN)) {
						max_lineno = 50;
						got_received = TRUE;
					}
				}
				break;
			case 'S':	/* Subject:  mandatory */	
				if (! got_subject) {
					if (match_header (ptrline, "Subject", buf2, HEADER_LEN)) {
						s = eat_re (buf2);
						h->subject = hash_str (s);
						got_subject = TRUE;
					}
				}
				break;
			case 'D':	/* Date:  mandatory */
				if (! got_date) {
					if (match_header (ptrline, "Date", buf2, HEADER_LEN)) {
						h->date = parsedate (buf2, (struct _TIMEINFO *) 0);
						got_date = TRUE;
					}
				}
				break;
			case 'X':	/* Xref:  optional */
				if (! got_xref) {
					if (match_header (ptrline, "Xref", buf2, HEADER_LEN)) {
						s = buf2;
						while (*s && *s == ' ') {
							s++;
						}
						h->xref = str_dup (s);
						got_xref = TRUE;
					}
				}
				break;
			case 'M':	/* Message-ID: */
				if (! got_msgid) {
					if (match_header (ptrline, "Message-ID", buf2, HEADER_LEN)) {
						s = buf2;
						while (*s && *s == ' ') {
							s++;
						}
						h->msgid = str_dup (s);
						got_msgid = TRUE;
					}
				}
				break;
			case 'L':	/* Lines:  optional */
				if (! got_lines) {
					if (match_header (ptrline, "Lines", buf2, HEADER_LEN)) {
						h->lines = atoi (buf2);
						got_lines = TRUE;
					}
				}
				break;
			case 'A':	/* Archive-name:  optional */
				if (match_header (ptrline, "Archive-name", buf2, HEADER_LEN)) {
					if ((s = (char *) strchr (buf2, '/')) != (char *) 0) {
						if (STRNCMPEQ(s+1, "part", 4) || 
						    STRNCMPEQ(s+1, "Part", 4)) {
							h->part = str_dup (s+5);
							s = strrchr (h->part, '\n');
							if (s != (char *) 0) {
								*s = '\0';
							}
						} else if (STRNCMPEQ(s+1,"patch",5) || 
								   STRNCMPEQ(s+1,"Patch",5)) {
							h->patch = str_dup (s+6);
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
		}

		if (! flag || lineno > max_lineno || got_archive) {
			if (got_from && got_date) {
				if (! got_subject) {
					h->subject = hash_str ("<No subject>");
				}
				debug_print_header (h);
				return TRUE;
			} else {
				return FALSE;
			}	
		}
	}
	/* NOTREACHED */
}

/*
 *  Read in an Nov/Xover index file. Fields are separated by TAB.
 *
 *  Format:
 *    1.  article number (ie. 183)                [mandatory]
 *    2.  Subject: line  (ie. Which newsreader?)  [mandatory]
 *    3.  From: line     (ie. iain@scn.de)        [mandatory]
 *    4.  Date: line     (rfc822 format)          [mandatory]
 *    5.  MessageID:     (Skipped - not used)     [mandatory]
 *    6.  References:    (Skipped - not used)     [mandatory]
 *    7.  Byte count     (Skipped - not used)     [mandatory]
 *    8.  Lines: line    (ie. 23)                 [mandatory]
 *    9.  Xref: line     (ie. alt.test:389)       [optional]
 *   10.  Archive-name:  (ie. widget/part01)      [optional]
 */

int
iReadNovFile (group, min, max, expired)
	struct t_group *group;
	long min;
	long max;
	int *expired;
{
 	char	*p, *q, *s;
	char	buf[OVERVIEW_LINE];
	char	buf2[LEN];
 	char	art_full_name[LEN];
	char	art_from_addr[LEN];
	FILE	*fp;
	long	artnum;

	top = 0;
	last_read_article = 0L;
	*expired = 0;
 
	/* 
	 * setup the overview file (whether it be local or via nntp)
	 */
	fp = open_xover_fp (group, "r", min, max);
	if (fp == (FILE *) 0) {
		return top;
	}

	while (fgets (buf, sizeof (buf), fp) != (char *) 0) {
		debug_nntp ("iReadNovFile", buf);
		if (STRCMPEQ(buf, ".")) {
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
		if (artnum <= 0) continue;

		/*
		 * Check to make sure article in nov file has not expired in group
		 */
/*
printf ("artnum=[%ld] xmin=[%ld] xmax=[%ld]\n", artnum, group->xmin, group->xmax);
fflush(stdout);
sleep(1);
*/
		if (artnum < group->xmin) {
			(*expired)++;
			continue;
		}
		set_article (&arts[top]);
		arts[top].artnum = last_read_article = artnum;
		q = strchr (p, '\t');
		if (q == (char *) 0) {
			error_message ("Bad overview record (Artnum) '%s'", buf);
			debug_nntp ("iReadNovFile", "Bad overview record (Artnum)");
			continue;
		} else {
			p = q + 1;
		}
 
		/* 
		 * READ subject 
		 */
		q = strchr (p, '\t');
		if (q == (char *) 0) {
			error_message ("Bad overview record (Subject) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (Subject)");
			continue;
		} else {
			*q = '\0';
		}
		arts[top].subject = hash_str (eat_re (p));
		p = q + 1;
 
		/* 
		 * READ author 
		 */
		q = strchr (p, '\t');
		if (q == (char *) 0) {
			error_message ("Bad overview record (From) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (From)");
			continue;
		} else {
			*q = '\0';
		}
		parse_from (p, art_from_addr, art_full_name);
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
			error_message ("Bad overview record (Date) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (Date)");
			continue;
		} else {
			*q = '\0';
		}
		arts[top].date = parsedate (p, NULL);
		p = q + 1;
 
		/* 
		 * READ article messageid 
		 */
		q = strchr (p, '\t');
		if (q == (char *) 0) {
			error_message ("Bad overview record (Msg-id) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (Msg-id)");
			continue;
		} else {
			*q = '\0';
		}
		if (*p) {
			arts[top].msgid = str_dup (p);
		} else {
			arts[top].msgid = (char *) 0;
		}
		p = q + 1;
 
		/* 
		 * READ article references 
		 */
		q = strchr (p, '\t');
		if (q == (char *) 0) {
			error_message ("Bad overview record (References) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (References)");
			continue;
		} else {
			*q = '\0';
		}
		if (*p) {
			arts[top].refs = str_dup (p);
		} else {
			arts[top].refs = 0;
		}
		p = q + 1;
 
		/* 
		 * SKIP article bytes 
		 */
		q = strchr (p, '\t');
		if (q == (char *) 0) {
			error_message ("Bad overview record (Bytes) [%s]", p);
			debug_nntp ("iReadNovFile", "Bad overview record (Bytes)");
			continue;
		} else {
			*q = '\0';
		}
		p = q + 1;
 
		/* 
		 * READ article lines (special case - last standard nov header)
		 */
		q = strchr (p, '\t');
		if (q == (char *) 0) {
			if (!*p || (*p < '0' && *p > '9')) {
				error_message ("Bad overview record (Lines) [%s]", p);
				debug_nntp ("iReadNovFile", "Bad overview record (Lines)");
				continue;
			}
		} else {
			*q = '\0';
		}
		arts[top].lines = atoi (p);
		p = (q == (char *) 0 ? (char *) 0 : q + 1);
 
		/* 
		 * READ article xrefs
		 */
		if (p != (char *) 0 && xref_supported) {
			q = str_str (p, "Xref: ", 6);
			if (q == (char *) 0) {
				q = str_str (p, "xref: ", 6);
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
				arts[top].xref = str_dup (q);
			}
		}

		/* 
		 * READ article archive-name
		 */
		if (p != (char *) 0) {
			q = str_str (p, "Archive-name: ", 14);
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
				if ((s = (char *) strchr (q, '/')) != (char *) 0) {
					if (STRNCMPEQ(s+1, "part", 4) || 
					    STRNCMPEQ(s+1, "Part", 4)) {
						arts[top].part = str_dup (s+5);
					} else if (STRNCMPEQ(s+1, "patch", 5) || 
						   STRNCMPEQ(s+1, "Patch", 5)) {
						arts[top].patch = str_dup (s+6);
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
 *    3.  From: line     (ie. iain@scn.de)        [mandatory]
 *    4.  Date: line     (rfc822 format)          [mandatory]
 *    5.  MessageID:     (ie. <123@ether.net>)    [optional]
 *    6.  References:    (ie. msgid msgid msgid)  [optional]
 *    7.  Byte count     (Skipped - not used)     [mandatory]
 *    8.  Lines: line    (ie. 23)                 [mandatory]
 *    9.  Xref: line     (ie. alt.test:389)       [optional]
 *   10.  Archive-name:  (ie. widget/part01)      [optional]
 */

void
vWriteNovFile (psGrp)
	struct t_group *psGrp;
{
	char	*pcNovFile;
	FILE	*hFp;
	int		iNum;
	struct	t_article *psArt;
	
	set_tin_uid_gid ();

	/* 
	 * setup the overview file (local only)
	 */
	pcNovFile = pcFindNovFile (psGrp, W_OK);
	if (debug) {
		error_message ("WRITE file=[%s]", pcNovFile);
	}

	hFp = open_xover_fp (psGrp, "w", 0L, 0L);

	if (hFp == (FILE *) 0) {
		error_message (txt_cannot_write_index, pcNovFile);
	} else {
		if (psGrp->attribute->sort_art_type != SORT_BY_NOTHING) {
			SortBy(artnum_comp);
		}
		if (! overview_index_filename) {
			fprintf (hFp, "%s\n", psGrp->name);
		}
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
		chmod (pcNovFile, 0644);
		if (psGrp->attribute->sort_art_type != SORT_BY_NOTHING) {
		        sort_arts (psGrp->attribute->sort_art_type);
		}
	}
	set_real_uid_gid ();
}

/*
 *  Create pointer to static group path
 */

#if 0

char *
pcFindGrpPath (psGrp)
	struct t_group *psGrp;
{
	static char acGrpPath[PATH_LEN];

	if (psGrp == (struct t_group *) 0) {
		return (char *) 0;
	}

	vMakeGrpPath (novrootdir, psGrp->name, acGrpPath);

	return acGrpPath;
}
#endif	/* 0 */

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
pcFindNovFile (psGrp, iMode)
	struct t_group *psGrp;
	int iMode;
{
	char *pcPtr;
	char *pcDir;
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
				if (iMode == R_OK) {					
					if (access (acNovFile, iMode) == 0) {
						overview_index_filename = TRUE;
					}
				} else {
					if (access (acBuf, iMode) == 0) {
						overview_index_filename = TRUE;
					}
				}
				if (! overview_index_filename) {
					pcDir = index_newsdir;
					iHashFileName = TRUE; 
				}
			}
			break;
	}
	
	if (iHashFileName) {
		lHash = hash_groupname (psGrp->name);

		for (iNum = 1;;iNum++) {
			sprintf (acNovFile, "%s/%lu.%d", pcDir, lHash, iNum);
		
			if ((hFp = fopen (acNovFile, "r")) == (FILE *) 0) {
				return acNovFile;
			}

			if (fgets (acBuf, sizeof (acBuf), hFp) == (char *) 0) {
				fclose (hFp);
				return (char *) 0;
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
do_update ()
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
	read_group_times_file ();
	
	/*
	 * loop through groups and update any required index files
	 */
	for (i = 0; i < group_top; i++) {
		psGrp = &active[my_group[i]];
		make_group_path (psGrp->name, group_path);

#ifdef INDEX_DAEMON
		
		joinpath (buf, psGrp->spooldir, group_path);

		joinpath (novpath, novrootdir, group_path);
if (verbose) {
	printf ("NOV path=[%s]\n", novpath);
}
/*		if (access (novpath, W_OK) == 0) { */
			vCreatePath (novpath);
/*		} */
		
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
				printf (txt_cannot_stat_index_file, psGrp->name, pcNovFile);
			}
		} else {
			if (delete_index_file) {
				unlink (pcNovFile);
			}
			index_time = stinfo.st_mtime;
		}

		if (debug == 2) {
			printf ("[%s] idxtime=[%ld]  old=[%ld]  new=[%ld]\n", 
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
			printf ("%s %s\n", (catchup ? "Catchup" : "Updating"), psGrp->name);
			fflush (stdout);
		}
		if (! index_group (psGrp)) {
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
	write_group_times_file ();
	
	if (verbose) {
		time (&end_epoch);
		sprintf (msg, txt_catchup_update_info,
			(catchup ? "Caughtup" : "Updated"), 
			group_top, (int)(end_epoch - beg_epoch));
		wait_message (msg);
	}
}

static int
artnum_comp (p1, p2)
	t_comptype *p1;
	t_comptype *p2;
{
	struct t_article *s1 = (struct t_article *) p1;
	struct t_article *s2 = (struct t_article *) p2;

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
subj_comp (p1, p2)
	t_comptype *p1;
	t_comptype *p2;
{
	int retval;
	struct t_article *s1 = (struct t_article *) p1;
	struct t_article *s2 = (struct t_article *) p2;

	/* 
	 * return result of strcmp (reversed for descending) 
	 */
	return (active[my_group[cur_groupnum]].attribute->sort_art_type == SORT_BY_SUBJ_ASCEND 
			? (retval = my_stricmp (s1->subject, s2->subject))
				? retval : ((s1->date - s2->date) > 0) ? 1 : -1
			: (retval = my_stricmp (s2->subject, s1->subject))
				? retval : ((s1->date - s2->date) > 0) ? 1 : -1);
}


static int
from_comp (p1, p2)
	t_comptype *p1;
	t_comptype *p2;
{
	int retval;
	struct t_article *s1 = (struct t_article *) p1;
	struct t_article *s2 = (struct t_article *) p2;

	/* 
	 * return result of strcmp (reversed for descending) 
	 */
	return (active[my_group[cur_groupnum]].attribute->sort_art_type == SORT_BY_FROM_ASCEND 
			? (retval = my_stricmp (s1->from, s2->from))
				? retval : ((s1->date - s2->date) > 0) ? 1 : -1
			: (retval = my_stricmp (s2->from, s1->from))
				? retval : ((s1->date - s2->date) > 0) ? 1 : -1);
}


static int
date_comp (p1, p2)
	t_comptype *p1;
	t_comptype *p2;
{
	struct t_article *s1 = (struct t_article *) p1;
	struct t_article *s2 = (struct t_article *) p2;


	if (active[my_group[cur_groupnum]].attribute->sort_art_type == SORT_BY_DATE_ASCEND) {
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
set_article (art)
	struct t_article *art;
{	
	art->subject = (char *) 0;
	art->from = (char *) 0;
	art->name = (char *) 0;
	art->date = 0L;
	art->xref = (char *) 0;
	art->msgid = (char *) 0;
	art->refs = (char *) 0;
	art->lines = -1;
	art->archive = (char *) 0;
	art->part = (char *) 0;
	art->patch = (char *) 0;
	art->thread = ART_EXPIRED;
	art->status = ART_UNREAD;
	art->inthread = FALSE;
	art->killed = FALSE;
	art->tagged = FALSE;
	art->selected = FALSE;
	art->zombie = FALSE;
	art->delete = FALSE;
	art->inrange = FALSE;
}

#ifdef WIN32
/* Don't want the overhead of windows.h */
int kbhit(void);
#endif

int
input_pending ()
{
#ifdef WIN32
	return kbhit() ? TRUE : FALSE;
#endif

#ifdef HAVE_SELECT
	int fd = STDIN_FILENO;
	fd_set fdread;
	struct timeval tvptr;

	FD_ZERO(&fdread);

	tvptr.tv_sec = 0;
	tvptr.tv_usec = 0;

	FD_SET(fd, &fdread);
	if (select (1, &fdread, NULL, NULL, &tvptr)) {
		if (FD_ISSET(fd, &fdread)) {
			return TRUE;
		}
	}
#endif	/* HAVE_SELECT */

#if defined(HAVE_POLL) && !defined(HAVE_SELECT)
	static int Timeout = 0;
	static long nfds = 1;
	static struct pollfd fds[]= {{ STDIN_FILENO, POLLIN, 0 }};
	
	if (poll (fds, nfds, Timeout) < 0) {
		/* 
		 * Error on poll 
		 */
		return FALSE;
	}
	
	switch (fds[0].revents) {
		case POLLIN:
			return TRUE;
			/* break; */
		/* 
		 * Other conditions on the stream 
		 */
		case POLLHUP:
		case POLLERR:
		default:
			return FALSE;
			/* break; */
	}
#endif	/* HAVE_POLL */

	return FALSE;
}


int
valid_artnum (art)
	long art;
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

static void
print_expired_arts (num_expired)
	int num_expired;
{
	int i;

	if (cmd_line && verbose) {
		if (debug) {
			printf ("Expired Index Arts=[%d]", num_expired);
		}
		for (i = 0; i < num_expired; i++) {
			my_fputc ('P', stdout);
		}
		if (num_expired) {
			fflush (stdout);
		}
	}
}

static char *
pcPrintDate (lSecs)
	long	lSecs;
{
	static	char acDate[80];
	struct	tm *psTm;

	psTm = localtime (&lSecs);
	(void) my_strftime (acDate, sizeof (acDate), "%d %b %Y %X", psTm);
	
	return acDate;	
}

static char *
pcPrintFrom (psArt)
	struct t_article *psArt;
{
	static	char acFrom[PATH_LEN];

	*acFrom = '\0';
	
	if (psArt->name != (char *) 0) {
		sprintf (acFrom, "%s (%s)", psArt->from, psArt->name);
	} else {
		strcpy (acFrom, psArt->from);
	}
	
	return acFrom;	
}

#ifdef INDEX_DAEMON
static void
vCreatePath (pcPath)
	char *pcPath;
{
	char	acCmd[LEN];

	/* HACK HACK HACK to get nov files off my overfull news partition !!! */
	sprintf (acCmd, "/bin/mkdir -p %s", pcPath);
printf ("CREATE Path=[%s]\n", acCmd);
	system (acCmd);
}
#endif

#ifdef XXX
char *
pcFindNovFile (psGrp)
	struct t_group *psGrp;
{
	char *p;
	char dir[PATH_LEN];
	char buf[PATH_LEN];
	FILE *fp;
	int i;
	static char file[PATH_LEN];
	unsigned long h;

	if (psGrp == (struct t_group *) 0) {
		return (char *) 0;
	}

	overview_index_filename = FALSE;

#ifdef INDEX_DAEMON
	vMakeGrpPath (novrootdir, group->name, dir);
/* ADD here path creation code */
	sprintf (file, "%s/%s", dir, OVERVIEW_FILE);
	overview_index_filename = TRUE;
	return file;
#endif
}
#endif
