/*
 *  Project   : tin - a Usenet reader
 *  Module    : refs.c
 *  Author    : Jason Faultless <jason@radar.demon.co.uk>
 *  Created   : 09-05-96
 *  Updated   : 04-12-96
 *  Notes     : Cacheing of message ids / References based threading
 *  Credits   : Richard Hodson <richard@radar.demon.co.uk>
 *		hash_msgid, free_msgid
 *  Copyright : (c) 1996 by Jason Faultless
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"

#define MAX_REFS	100			/* Limit recursion depth */
#define REF_SEP	" "			/* Separator chars in ref headers */

/* Produce disgusting amounts of output to help me tame this thing */
#undef DEBUG_REFS

#ifdef DEBUG_REFS
#	define DEBUG_PRINT(x)	fprintf x
#else
#	define DEBUG_PRINT(x)
#endif

/*
 * The msgids are all hashed into a big array, with overspill
 */
struct t_msgid *msgids[MSGID_HASH_SIZE] = {0};

/*
 * Local prototypes
 */
static unsigned int hash_msgid P_((char *key));
static void add_to_parent P_((struct t_msgid *ptr));
static struct t_msgid *add_msgid P_((int key, char *msgid, struct t_msgid *newparent));
static struct t_msgid *parse_references P_((char *r));
static char *_get_references P_((struct t_msgid *refptr, int depth));
#ifdef HAVE_REF_THREADING
#ifdef DEBUG_REFS
static void dump_msgid_thread  P_((struct t_msgid *ptr, int level));
static void dump_msgid_threads P_((void));
#endif
static struct t_msgid *find_next P_((struct t_msgid *ptr));
static void build_thread P_((struct t_msgid *ptr));
#endif

/*
 * This part of the code deals with the cacheing and retrieval
 * of Message-id and References headers
 *
 * Rationale:
 *    Even though the Message-id is unique to an article, the References
 *    field contains msgids from elsewhere in the group. As the expiry
 *    period increases, so does the redundancy of data.
 *    At the time of writing, comp.os.ms-windows.advocacy held ~850
 *    articles. The references fields contained 192k of text, of which
 *    169k was saved using the new cacheing.
 *
 *    When threading on Refs, a much better view of the original thread
 *    can be built up using this data, and threading is much faster
 *	   because all the article relationships are automatically available
 *	   to us.
 *
 *	  NB: We don't cache msgids from the filter file.
 */

/*
 * Hash a message id. A msgid is of the form <unique@sitename>
 * (But badly broken message id's do occur)
 * We hash on the unique portion which should have good randomness in
 * the lower 5 bits. Propagate the random bits up with a shift, and
 * mix in the new bits with an exclusive or.
 *
 * This should generate about 5+strlen(string_start) bits of randomness.
 * MSGID_HASH_SIZE is a prime of order 2^11
 */
static unsigned int
hash_msgid(key)
	char *key;
{
	unsigned int hash = 0;

	while (*key && *key != '@') {
		hash = (hash << 1) ^ *key;
		++key;
	}

	hash %= MSGID_HASH_SIZE;

	return(hash);
}

/*
 * Thread us into our parents' list of children.
 */
static void
add_to_parent(ptr)
	struct t_msgid *ptr;
{
#ifdef HAVE_REF_THREADING
	struct t_msgid *p;

	if (!ptr->parent)
		return;

	/*
	 * Trivial case - if we are the first child (followup)
	 */
	if (ptr->parent->child == NULL)	{
		ptr->parent->child = ptr;
		return;
	}

	/*
	 * Add this followup to the sibling chain of our parent.
	 * arts[] is already date sorted (ASCEND or DESCEND) and thus adding
	 * at the end of the chain does what we want.
	 * Reference data goes at the start of the chain if ASCEND (because
	 * we presume unavailable arts have expired), otherwise at the end.
	 * ie: if ASCEND && REF
	 *	       add_to_start
	 *     else
	 *        add_to_end
	 */
	if ((CURR_GROUP.attribute->sort_art_type == SORT_BY_DATE_ASCEND) &&
												 (ptr->article == ART_NORMAL)) {
		/* Add to start */
		ptr->sibling = ptr->parent->child;
		ptr->parent->child = ptr;
	} else {
		/* Add to end */
		for (p = ptr->parent->child; p->sibling != NULL; p = p->sibling);

/*		ptr->sibling is already NULL */
		p->sibling = ptr;
	}

#endif /* HAVE_REF_THREADING */
}

/*
 * Adds or updates a message id in the cache.
 * We return a ptr to the msgid, whether located or newly created.
 *
 * . If the message id is new, add it to the cache, creating parent, child
 *   & sibling ptrs if a parent is given.
 *
 * . If the message id is a duplicate, then:
 *     a) If no parent or the same parent, is given, no action is needed.
 *
 *     b) If a parent is specified and the current parent is NULL, then
 *        add in the new parent and create child/sibling ptrs.
 *        Because we add Message-ID headers first, we don't have to worry
 *        about bogus Reference headers messing things up.
 *
 *     c) If a conflicting parent is given:
 *
 *        If (key == REF_REF) ignore the error - probably the refs
 *        headers are broken or have been truncated.
 *
 *        Otherwise we have a genuine problem, two articles in one group
 *        with identical Message-IDs. This is indicative of a broken
 *        overview database.
 */
static struct t_msgid *
add_msgid(key, msgid, newparent)
	int key;
	char *msgid;
	struct t_msgid *newparent;
{
	struct t_msgid *ptr;
	struct t_msgid *i = NULL;
	unsigned int h;

 	if (!msgid) {
 		error_message("add_msgid: NULL msgid\n", "");
 		exit(1);
 	}
  
	h = hash_msgid(msgid+1);				/* Don't hash the initial '<' */

	DEBUG_PRINT((stderr, "---------------- Add %s %s with parent %s\n", (key==MSGID_REF)?"MSG":"REF", msgid, (newparent == NULL)?"unchanged":newparent->txt));

	/*
	 * Look for this message id in the cache.
	 * Broken software will sometimes damage the case of a message-id.
	 */
	for (i = msgids[h]; i != NULL; i = i->next) {

		if (strcasecmp(i->txt, msgid) != 0)				/* No match yet */
			continue;

		/*
		 * CASE 1a - No parent specified, do nothing
		 */
		if (newparent == NULL) {
			DEBUG_PRINT((stderr, "nop: %s No parent specified\n", i->txt));
			return(i);
		}

		/*
		 * CASE 1b - Parent not changed, do nothing
		 */
		if (newparent == i->parent) {
			DEBUG_PRINT((stderr, "dup: %s -> %s (no change)\n", i->txt, (i->parent)?i->parent->txt:"NULL"));
			return(i);
		}

		/*
		 * CASE2 - A parent has been given where there was none before.
		 *         Change parent from null -> not-null & update ptrs
		 */
		if (i->parent == NULL) {

			/*
			 * Detect & ignore circular reference paths by looking for the
			 * new parent in this thread
			 */
			for (ptr = newparent; ptr != NULL; ptr = ptr->parent) {
				if (ptr == i) {
					DEBUG_PRINT((stderr, "Avoiding circular reference! (%s)\n", (key==MSGID_REF)?"MSG":"REF"));
					return(i);
				}
			}

			i->parent = newparent;
			add_to_parent(i);

			DEBUG_PRINT((stderr, "set: %s -> %s\n", i->txt, (newparent)?newparent->txt:"None"));
			return(i);
		}

		/*
		 * CASE 3 - A new parent has been given that conflicts with the
		 *			current one. This is caused by
		 * 1) A duplicate Message-ID in the spool (very bad !)
		 * 2) corrupt References header
		 * All we can do is ignore the error
		 */
		if (i->parent != newparent) {
			DEBUG_PRINT((stderr, "Warning: (%s) Ignoring %s -> %s (already %s)\n",
				(key==MSGID_REF)?"MSG":"REF", i->txt,
				(newparent)?newparent->txt:"None", i->parent->txt));

			return(i);
		}

		error_message("Error: Impossible combination of conditions !\n", "");
		return(i);
	}

	DEBUG_PRINT((stderr, "new: %s -> %s\n", msgid, (newparent)?newparent->txt:"None"));

	/*
	 * This is a new node, so build a structure for it
	 */
	ptr = (struct t_msgid *)my_malloc(sizeof(struct t_msgid));

	ptr->txt = my_strdup(msgid);
	ptr->parent = newparent;

#ifdef HAVE_REF_THREADING
	ptr->child = ptr->sibling = NULL;
	ptr->article = (key == MSGID_REF ? top : ART_NORMAL);
	add_to_parent(ptr);
#endif

	/*
	 * Insert at head of list for speed.
	 */
	ptr->next = msgids[h];
	msgids[h] = ptr;

	return(ptr);
}

/*
 * Take a raw line of references data and return a ptr to a linked list of
 * msgids, starting with the most recent entry. (Thus the list is reversed)
 * Following the parent ptrs leads us back to the start of the thread.
 *
 * We iterate through the refs, adding each to the msgid cache, with
 * the previous ref as the parent.
 * The space saving vs. storing the refs as a single string is significant.
 */
static struct t_msgid *
parse_references(r)
	char *r;
{
	char *ptr;
	struct t_msgid *parent, *current;

	if (!r)
		return(NULL);

	DEBUG_PRINT((stderr, "parse_references: %s\n", r));

	/*
	 * Break the refs down, using a space as delimiters
	 * A msgid can't contain a space, right ?
	 */
	if ((ptr = strtok(r, REF_SEP)) == NULL)
		return(NULL);

	/*
	 * By definition, the head of the thread has no parent
	 */
	parent = NULL;
	current = add_msgid(REF_REF, ptr, parent);
	while ((ptr = strtok(NULL, REF_SEP)) != NULL) {
		parent = current;
		current = add_msgid(REF_REF, ptr, parent);
	}

	return(current);
}

/*
 * Reconstruct the References: field from the parent pointers
 * NB: The original Refs: can be no longer than HEADER_LEN (see open.c)
 *     Broken headers sometimes have malformed or circular reference
 *	   lists, which we strive to work around.
 */
static char *
_get_references(refptr, depth)
	struct t_msgid *refptr;
	int depth;
{
	char *refs;
	static short len;							/* Accumulated size */

	if (refptr->parent == NULL || depth > MAX_REFS) {

#ifdef DEBUG_REFS
		if (depth > MAX_REFS) {
			fprintf(stderr, "Warning: Too many refs near to %s. Truncated\n", refptr->txt);
			sleep(2);
		}
#endif

		refs = (char *) my_malloc(HEADER_LEN);
		len  = 0;
	} else
		refs = _get_references(refptr->parent, depth+1);

	/*
	 * Attempt at damage limitation in case of broken Refs fields
	 */
	if (len < HEADER_LEN-500) {
		sprintf(refs + len, "%s ", refptr->txt);
		len = strlen(refs);
	}

	return (refs);
}

/*
 * A wrapper to the above, null terminate the string and shrink it
 * to correct size
 */
char *
get_references(refptr)
	struct t_msgid *refptr;
{
	char *refs;
	short len;

	if (refptr == NULL)
		return(NULL);

	refs = _get_references(refptr, 1);

	len = strlen(refs);

	refs[len-1] = '\0';

	refs = (char *) my_realloc(refs, len);

	return(refs);
}

/*
 * Clear the entire msgid cache, freeing up all chains. This is
 * normally only needed when entering a new group
 */
void
free_msgids()
{
	int i;
	struct t_msgid *ptr, *next, **msgptr;

	msgptr = msgids;				/* first list */

	for (i = MSGID_HASH_SIZE-1; i >= 0 ; i--) {	/* count down is faster */
		ptr = *msgptr;
		*msgptr++ = NULL;			/* declare list empty */

		while (ptr != NULL) {	/* for each node in the list */

			next = ptr->next;		/* grab ptr before we free node */
			free(ptr->txt);			/* release text */
			free(ptr);			/* release node */

			ptr = next;			/* hop down the chain */
		}
	}
}

#if 0
static void
dump_msgids()
{
	int i;
	struct t_msgid *ptr;

	fprintf(stderr, "Dumping...\n");

	for (i=0; i<MSGID_HASH_SIZE; i++) {

		if (msgids[i] != NULL) {

			fprintf(stderr, "node %d", i);

			for (ptr = msgids[i]; ptr != NULL; ptr = ptr->next) {
				fprintf(stderr, " -> %s", ptr->txt);
			}

			fprintf(stderr, "\n");

		}
	}
}
#endif

/*
 * The rest of this code deals with reference threading
 */
#ifdef HAVE_REF_THREADING
/*
 * Legend:
 *
 * . When a new thread is started, the root message will have no
 *   References: field
 *
 * . When a followup is posted, the message-id that was referred to
 *   will be appended to the References: field. If no References:
 *   field exists, a new one will be created, containing the single
 *   message-id
 *
 * . The References: field should not be truncated, though in practice
 *   this will happen, often in badly broken ways.
 *
 * This is simplistic, so check out RFC1036 & son of RFC1036 for full
 * details from the posting point of view.
 *
 * We attempt to maintain 3 pointers in each message-id to handle threading
 * on References:
 *
 * 1) parent  - the article that the current one was in reply to
 *              An article with no References: has no parent, therefore
 *              it is the root of a thread.
 *
 * 2) sibling - the next reply in sequence to parent.
 *
 * 3) child   - the first reply to the current article.
 *
 * These pointers are automatically set up when we read in the
 * headers for a group.
 *
 * It remains for us to fill in the .thread and .inthread ptrs in
 * each article that exists in the spool, using the intelligence of
 * the reference tree to locate the 'next' article in the thread.
 *
 * TODO:
 * . We could differentiate between bona fide root messages
 *   and root messages whose parent has expired on the group menu
 *	 (currently + is unread, - is marked unread and ' ' is read)
 */

/*
 * Clear out all the article fields from the msgid hash prior to a
 * rethread.
 */
void
clear_art_ptrs()
{
	int i;
	struct t_msgid *ptr;

	for (i = MSGID_HASH_SIZE-1; i >= 0 ; i--) {
		for (ptr = msgids[i]; ptr != NULL; ptr = ptr->next)
			ptr->article = ART_NORMAL;
	}
}

/*
 * Function to dump an ASCII tree map of a thread rooted at msgid.
 * Output goes to fp, level is the current depth of the tree.
 */
void
dump_thread(fp, msgid, level)
	FILE *fp;
	struct t_msgid *msgid;
	int level;
{
	char buff[120];		/* This is _probably_ enough */
	char *ptr = buff;
	int i, len;

	/*
	 * Dump the current article
	 */
	sprintf(ptr, "%3d %*s", msgid->article, 2*level, "  ");

	len = strlen(ptr);
	i = cCOLS - len - 20;

	if (msgid->article >= 0) {
		sprintf(ptr+len, "%-*.*s   %-17.17s", i, i,
				arts[msgid->article].subject,
				(arts[msgid->article].name) ?
					arts[msgid->article].name :
					arts[msgid->article].from);
	} else
		sprintf(ptr+len, "%-*.*s", i, i, "[- Unavailable -]");

	fprintf(fp, "%s\n", ptr);

	if (msgid->child != NULL)
		dump_thread(fp, msgid->child, level + 1);

	if (msgid->sibling != NULL)
		dump_thread(fp, msgid->sibling, level);

	return;
}

#ifdef DEBUG_REFS
/*
 * Dump out all the threads from the msgid point of view, show the
 * related article index in arts[] where possible
 * A thread is defined as a starting article with no parent
 */
static void
dump_msgid_thread(ptr, level)
	struct t_msgid *ptr;
	int level;
{
	fprintf(stderr, "%*s %s (%d)\n", level*3, "   ", ptr->txt, ptr->article);

	if (ptr->child != NULL)
		dump_msgid_thread(ptr->child, level+1);

	if (ptr->sibling != NULL)
		dump_msgid_thread(ptr->sibling, level);

	return;
}

static void
dump_msgid_threads()
{
	int i;
	struct t_msgid *ptr;

	fprintf(stderr, "Dump started.\n\n");

	for (i=0; i<MSGID_HASH_SIZE; i++) {
		if (msgids[i] != NULL) {

			for (ptr = msgids[i]; ptr != NULL; ptr = ptr->next) {

				if (ptr->parent == NULL) {
					dump_msgid_thread(ptr, 1);
					fprintf(stderr, "\n");
				}
			}
		}
	}

	fprintf(stderr, "Dump complete.\n\n");
}
#endif

/*
 * Find the next message in the thread.
 * We descend children before siblings, and only return articles that
 * exist in arts[] or NULL if we are truly at the end of a thread.
 * If there are no more down pointers, backtrack to find a sibling
 * to continue the thread, we note this with the 'bottom' flag.
 *
 * A Message-ID will not be included in a thread if
 *  It doesn't point to an article OR
 *     (it's already threaded/expired OR it has been autokilled)
 */
#define SKIP_ART(ptr)	\
	(ptr && (ptr->article == ART_NORMAL || \
		(arts[ptr->article].thread != ART_NORMAL || arts[ptr->article].killed)))

static struct t_msgid *
find_next(ptr)
	struct t_msgid *ptr;
{
	static int bottom = FALSE;

	/*
	 * Keep going while we haven't bottomed out and we haven't
	 * got something in arts[]
	 */
	while (ptr != NULL) {

		/*
		 * Children first, unless bottom is set
		 */
		if (!bottom && ptr->child != NULL) {
			ptr = ptr->child;

			/*
			 * If article not present, keep going
			 */
			if (SKIP_ART(ptr))
				continue;
			else
				break;
		}

		if (ptr->sibling != NULL) {
			bottom = FALSE;

			ptr = ptr->sibling;

			/*
			 * If article not present, keep going
			 */
			if (SKIP_ART(ptr))
				continue;
			else
				break;
		}

		/*
		 * No more child or sibling to follow, backtrack up to
		 * a sibling if we can find one
		 */
		if (ptr->child == NULL && ptr->sibling == NULL) {

			while(ptr != NULL && ptr->sibling == NULL)
				ptr = ptr->parent;

#if 0	/* TODO The fix for self-referencing reference loops obseletes this */
				/*
				 * Skip self referencing arts
				 */
				if (ptr ==  ptr->parent) {
					DEBUG_PRINT((stderr, "Self Referencing article avoided\n"));
					return (NULL);
				} else
					ptr = ptr->parent;
			}
#endif

			/*
			 * We've backtracked up to the parent with a suitable sibling
			 * go round once again to move to this sibling
			 */
			if (ptr)
				bottom = TRUE;
			else
				break;		/* Nothing found, exit with NULL */
		}
	}

	return(ptr);
}

/*
 * Run the .thread and .inthread pointers through the members of this
 * thread.
 */
static void
build_thread(ptr)
	struct t_msgid *ptr;
{
	struct t_msgid *newptr;

	/*
	 * If the root article has gone, advance to the first valid one
	 */
	if (ptr->article == ART_NORMAL)
		ptr = find_next(ptr);

	/*
	 * Keep working through the thread, updating the ptrs as we go
	 */
	while ((newptr = find_next(ptr)) != NULL) {

		arts[newptr->article].inthread = TRUE;
		arts[ptr->article].thread = newptr->article;

		ptr = newptr;
	}

}

/*
 * Run a new set of threads through the base articles, using the
 * parent / child / sibling  / article pointers in the msgid hash.
 */
void
thread_by_reference()
{
	int i;
	struct t_msgid *ptr;

#ifdef DEBUG_REFS
	freopen("Refs.info", "w", stderr);
	setvbuf(stderr, NULL, _IONBF, 0);

	dump_msgid_threads();
#endif

	/*
	 * Build threads starting from root msgids (ie without parent)
	 */
	for (i=0; i<MSGID_HASH_SIZE; i++) {
		if (msgids[i] != NULL) {
			for (ptr = msgids[i]; ptr!=NULL; ptr = ptr->next) {
				if (ptr->parent == NULL) {
					build_thread(ptr);
				}
			}
		}
	}

#ifdef DEBUG_REFS
	fprintf(stderr, "Full dump of threading info...\n");

	for (i=0 ; i < top ; i++) {
		fprintf(stderr, "%3d %3d %3d %3d : %3d %3d : %.50s %s\n", i,
			(arts[i].refptr->parent)  ? arts[i].refptr->parent->article : -2,
			(arts[i].refptr->sibling) ? arts[i].refptr->sibling->article : -2,
			(arts[i].refptr->child)   ? arts[i].refptr->child->article : -2,
			arts[i].inthread, arts[i].thread, arts[i].refptr->txt, arts[i].subject);
	}

	freopen("/dev/tty", "w", stderr);
#endif

	return;

}

/*
 * Do the equivalent of subject threading, but only on the thread base
 * messages.
 * This should help thread together mistakenly multiply posted articles,
 * articles which were posted to a group rather than as followups, those
 * with missing ref headers etc.
 * We add joined threads onto the end of the .thread chain of the previous
 * thread. arts[] is already sorted, so the sorting of these will be
 * correct.
 * TODO: merged threads don't display with 'a' - only 1st thread shows up
 */
void
collate_subjects()
{
	int i, j, art;
	int *aptr;

	/*
 	 * Run through the root messages of each thread. We have to traverse
	 * using arts[] and not msgids[] to preserve the sorting.
	 */
	for (i = 0; i < top; i++) {

		/*
		 * Ignore already threaded and expired arts
		 */
		if (arts[i].inthread || IGNORE_ART(i))
			continue;

		/*
		 * Get the contents of the magic marker in the hashnode
		 */
		aptr = (int *)arts[i].subject - 2; /* arghh */

		j = *aptr;

		if (j != -1 && j < i) {

			/*
			 * Modified form of the subject threading - the only difference
			 * is that we have to add later threads onto the end of the
			 * previous thread
			 */
			if (((arts[i].subject == arts[j].subject) ||
						   ((arts[i].part || arts[i].patch) &&
							 arts[i].archive == arts[j].archive))) {
/*DEBUG_PRINT((stderr, "RES: %d is now inthread, at end of %d\n", i, j));*/

				for (art = j; arts[art].thread >= 0; art = arts[art].thread);

				arts[art].thread = i;
				arts[i].inthread = TRUE;
			}
		}

		/*
		 * Update the magic marker with the highest numbered msg in
		 * arts[] that has been used in this thread so far
		 */
		*aptr = i;
	}

	return;
}

#endif /* HAVE_REF_THREADING */

/*
 * Builds the reference tree:
 *
 * 1) Sort the article base. This will ensure that articles and their
 *    siblings are inserted in the correct order.
 * 2) Add each Message-ID header and its direct reference ('reliable info')
 *    to the cache. Son of RFC1036 mandates that if References headers must
 *    be trimmed, then at least the (1st three and) last reference should be
 *    maintained.
 * 3) Add rest of References header to the cache. This information is less
 *    reliable than the info added in 2) and is only used to fill in any
 *    gaps in the reference tree - no information is superceded.
 * 4) Free() up the msgid and refs headers once cached
 */
void
build_references(group)
	struct t_group *group;
{
	int i;
	struct t_article *art;
	char *s;

/* TODO - do we need this test, we may _want_ to sort on artnum for some reason */
	if (group->attribute->sort_art_type != SORT_BY_NOTHING)
		sort_arts (group->attribute->sort_art_type);

	/*
	 * Add the Message-ID headers to the cache, using the last Reference
	 * as the parent
	 */
	DEBUG_PRINT((stderr, "MSGID phase\n"));
	for (i = 0; i < top; i++) {
		art = &arts[i];

#ifndef INFERIOR_REF_HANDLING
		if (art->refs) {

			/*
			 * Must strip trailing white space from Refs: lines
			 */
			while (*(art->refs + strlen(art->refs) - 1) == ' ')
				*(art->refs + strlen(art->refs) - 1) = '\0';

			if ((s = strrchr(art->refs, ' ')) != NULL)
				++s;						/* More than 1 ref - use last */
			else
				s = art->refs;				/* Only 1 ref - use it */

			art->refptr = add_msgid(MSGID_REF, art->msgid, add_msgid(REF_REF, s, NULL));
		} else
#endif
			art->refptr = add_msgid(MSGID_REF, art->msgid, NULL);

		free(art->msgid);					/* Now cached - discard this */
	}

	DEBUG_PRINT((stderr, "REFS phase\n"));
	/*
	 * Add the References data to the cache
	 */
	for (i = 0; i < top; i++) {

		if (!arts[i].refs)						/* No refs - skip */
			continue;

		art = &arts[i];

		/*
		 * Use add_msgid() to add the references, this will neatly sort out
		 * all the special cases, circular refs, child/sibling ptrs etc..
		 */
/* TODO there is a lot of redundancy here still */
		add_msgid(MSGID_REF, art->refptr->txt, parse_references(art->refs));

		free(art->refs);
	}
}

/* end of refs.c */
