/*
 *  Project   : tin - a Usenet reader
 *  Module    : refs.c
 *  Author    : Jason Faultless <jason@radar.demon.co.uk>
 *  Created   : 09-05-96
 *  Updated   : 16-07-96
 *  Notes     : Cacheing of message ids / References based threading
 *  Credits   : Richard Hodson <richard@radar.demon.co.uk>
 *	             hash_msgid, free_msgid
 *  Copyright : (c) 1996 by Jason Faultless
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"

#define MAX_REFS	100			/* Limit recursion depth */

/* Produce disgusting amounts of output to help me tame this thing */
#undef DEBUG_REFS

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
		hash = (hash << 1 ) ^ *key;
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

	if (! ptr->parent)
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
 *        Otherwise we have a genuine paradox, two articles in one group
 *        with identical Message-IDs and different parents.
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

	if (! msgid)
		return(NULL);

	h = hash_msgid(msgid+1);				/* Don't hash the initial '<' */

#ifdef DEBUG_REFS
	fprintf(stderr, "---------------- Add %s %s\n", (key==MSGID_REF)?"MSG":"REF", msgid);
#endif

	/*
	 * Look for this message id in the cache. Broken software will sometimes
	 * not preserve the original case of a message-id.
	 */
	for (i = msgids[h]; i != NULL; i = i->next) {

		if (strcasecmp(i->txt, msgid) == 0) {

			/*
			 * CASE 1
			 * No parent given or parent not changed - no update required
			 */
			if ((newparent == NULL) || (newparent == i->parent)) {
#ifdef DEBUG_REFS
				if (newparent == i->parent)
					fprintf(stderr, "dup: %s -> no change\n", msgid);
				else
					fprintf(stderr, "nop: No parent specified\n");
#endif
				return(i);
			}

			/*
			 * CASE2
			 * A parent has been given where there was none before.
			 * Need to change parent from null -> not-null & update ptrs
			 */
			if (i->parent == NULL) {
				/*
				 * Check for circular reference paths by looking for the
				 * new parents' presence in this thread
				 * I'm assuming here that if we do find the parent, then this
				 * can only happen in a broken ref thread and so the rest of
				 * this block is pointless as it only applies when we're a
				 * MSGID_REF
				 */
				for (ptr = newparent; ptr != NULL; ptr = ptr->parent) {
					if (strcasecmp(ptr->txt, msgid) == 0) {
#ifdef DEBUG_REFS
						fprintf(stderr, "Avoiding circular reference! (key=%s)\n", (key==MSGID_REF)?"MSG":"REF");
#endif
						return(i);
					}
				}

				i->parent = newparent;

				add_to_parent(i);
#ifdef DEBUG_REFS
				fprintf(stderr, "set: %s -> %s\n", msgid,
										(newparent)?newparent->txt:"None");
#endif
				return(i);
			}

			/*
			 * CASE 3
			 * A new parent has been given that conflicts with the current
			 * one. This is bad - caused by incorrectly trimmed or
			 * otherwise corrupt references.
			 * If the information is from a message-ID header, then
			 * we have a genuine conflict here. All we can do is flag the
			 * fact.
			 */
			if (i->parent != newparent) {
#ifdef DEBUG_REFS
				fprintf(stderr, "Warning: (%s) %s -> %s (already %s)\n",
					(key==MSGID_REF)?"MSG":"REF", msgid,
					(newparent)?newparent->txt:"None", i->parent->txt);
#endif
				if (key == MSGID_REF)
					fprintf(stderr, "Warning: Duplicate parent for Message-ID %s -> %s (already %s)\n",
								msgid, (newparent)?newparent->txt:"None", i->parent->txt);

				return(i);
			}

			error_message("Impossible: combination of conditions !\n", "");
			return(i);
		}
	}

#ifdef DEBUG_REFS
	fprintf(stderr, "new: %s -> %s\n", msgid, (newparent)?newparent->txt:"None");
#endif

	/*
	 * This is a new node, so build a structure for it
	 * Insert at start of list for speed.
	 */
	ptr = (struct t_msgid *)my_malloc(sizeof(struct t_msgid));

	ptr->txt = str_dup(msgid);
	ptr->parent = newparent;
#ifdef HAVE_REF_THREADING
	ptr->child = ptr->sibling = NULL;
	ptr->article = (key == MSGID_REF ? top : ART_NORMAL);
	add_to_parent(ptr);
#endif

	/*
	 * Makes no difference to insert at the head of the hash buckets
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
struct t_msgid *
parse_references(r)
	char *r;
{
	char *ptr;
	struct t_msgid *parent, *current;

	if (! r)
		return(NULL);

#ifdef DEBUG_REFS
	fprintf(stderr, "Refs: %s\n", r);
#endif

	/*
	 * Break the refs down, using ' ' as delimiters
	 * A msgid can't contain a ' ', right ?
	 */
	if ((ptr = strtok(r, " ")) == NULL)
		return(NULL);

	/*
	 * By definition, the head of the thread has no parent
	 */
	parent = NULL;
	current = add_msgid(REF_REF, ptr, parent);
	while ((ptr = strtok(NULL, " ")) != NULL) {
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
{
	char *refs;
	static short len;							/* Accumulated size */

	if (refptr->parent == NULL || depth > MAX_REFS) {

		if (depth > MAX_REFS) {
			fprintf(stderr, "Warning: Too many refs near to %s. Truncated\n", refptr->txt);
			sleep(2);
		}

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
/* TODO  */
#ifndef OLD_FREE_CODE
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

#else
/*
 * Clear the entire msgid cache, freeing up all chains. This is
 * normally only needed when entering a new group
 */

static void _free_list P_((struct t_msgid *ptr));

static void
_free_list(ptr)
	struct t_msgid *ptr;
{
	if (ptr->next != NULL)
		_free_list(ptr->next);

	free(ptr->txt);
	free(ptr);
	return;
}

void
free_msgids()
{
	int i;

	for (i=0; i<MSGID_HASH_SIZE; i++) {

		if (msgids[i] != NULL) {
			_free_list(msgids[i]);
			msgids[i] = NULL;
		}
	}
}
#endif

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
 * Function to messily dump an ASCII tree map of a thread.
 * msgid is a ptr to the root article, level is the current depth
 * of the tree.
 */
void
dump_thread(fp, msgid, level)
	FILE *fp;
	struct t_msgid *msgid;
	int level;
{
	char buff[120];		/* This will probably break - test only */

	/*
	 * Dump the current article
	 */
	sprintf(buff, "%3d %*s %-.18s", msgid->article, 2*level, "  ",
	        (msgid->article >= 0) ? ((arts[msgid->article].name) ?
	        arts[msgid->article].name : arts[msgid->article].from) :
	        "[- Unavailable -]");

	fprintf(fp, "%-*s  %-.45s\n", 30, buff, (msgid->article >= 0) ?
	        arts[msgid->article].subject : "");

	if (msgid->child != NULL) {
		dump_thread(fp, msgid->child, level + 1);
	}

	if (msgid->sibling != NULL) {
		dump_thread(fp, msgid->sibling, level);
	}

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

	for (i=0; i<MSGID_HASH_SIZE; i++) {
		if (msgids[i] != NULL) {
			for (ptr = msgids[i]; ptr!=NULL; ptr = ptr->next) {
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
 * A messagid will not be included in a thread if
 *  It doesn't point to an article OR
 *     (it's already threaded OR it has been autokilled)
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
		if (! bottom && ptr->child != NULL) {
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

			while(ptr != NULL && ptr->sibling == NULL) {
				if (ptr ==  ptr->parent) {
				/* Jehova */
					return (NULL);
				} else {
					ptr = ptr->parent;
				}
			}

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

		if (arts[i].inthread)			/* Ignore already threaded arts */
			continue;

		/*
		 * Get the contents of the magic marker in the hashnode
		 */
		aptr = (int *)arts[i].subject - 2;

		j = *aptr;

		if (j != -1 && j < i) {

			/*
			 * Modified form of the subject threading - the only difference
			 * is that we have to add later threads onto the end of the
			 * previous thread
			 */
			if ( ((arts[i].subject == arts[j].subject) ||
						   ((arts[i].part || arts[i].patch) &&
							 arts[i].archive == arts[j].archive))) {
/*fprintf(stderr, "RES: %d is now inthread, at end of %d\n", i, j);*/

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
 * Builds the reference tree
 * 1) Sort the article base. This will ensure that articles and their
 *    siblings are inserted in the correct order.
 * 2) Add the Message-ID headers to the cache.
 * 3) Add the Reference header to the cache. This information is less
 *    reliable than the Message-ID info and will only be used to add
 *    to the cache and not to supercede
 * 4) Frees up the msgid and refs headers once cached
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
	 * Add the Message-ID headers to the cache, the last ref being the parent.
	 */
	for (i = 0; i < top; i++) {
		art = &arts[i];

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
			art->refptr = add_msgid(MSGID_REF, art->msgid, NULL);

		free(art->msgid);					/* Now cached - discard this */
	}

	/*
	 * Add all the References data to the cache
	 */
	for (i = 0; i < top; i++) {
		if (arts[i].refs) {
			arts[i].refptr->parent = parse_references(arts[i].refs);
			free(arts[i].refs);
		}
	}
}

/* end of refs.c */
