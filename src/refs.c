/*
 *  Project   : tin - a Usenet reader
 *  Module    : refs.c
 *  Author    : Jason Faultless <jason@radar.demon.co.uk>
 *  Created   : 09-05-96
 *  Notes     : Cacheing of message ids
 *				References based threading
 *	Credits   : Richard Hodson <richard@radar.demon.co.uk>
 * 				Hashing function
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
static void add_to_parent P_((struct t_msgid *ptr));
static char *_get_references P_((struct t_msgid *refptr, int depth));
static void _free_list P_((struct t_msgid *ptr));
static unsigned int hash_msgid P_((char *key));
#ifdef DEBUG_REFS
static void dump_msgid_threads P_((void));
#endif
#ifdef HAVE_REF_THREADING
static struct t_msgid *find_next P_((struct t_msgid *ptr));
static void build_thread P_((struct t_msgid *ptr));
#endif

/*========================================================================
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
 *    When threading on Refs, a much better view of the original thead
 *    can be built up using this data, and threading is much faster
 *	  because all the article relationships are automatically available
 *	  to us.
 *
 *	  NB: We don't cache msgids from the filter file.
 */

/*-------------------------------------------------------------------------
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

/*-------------------------------------------------------------------------
 * Thread us into our parents' list of children.
 */
void
add_to_parent(ptr)
	struct t_msgid *ptr;
{
#ifdef HAVE_REF_THREADING
	if (!ptr->parent)
		return;

	if (ptr->parent->child == NULL)	{		/* We are 1st child */
		ptr->parent->child = ptr;
	} else {
		/* TODO: use date_comp() to insert siblings by date */
		ptr->sibling = ptr->parent->child;
		ptr->parent->child = ptr;
	}
#endif
}

/*-------------------------------------------------------------------------
 * Adds or updates a message id in the cache.
 * If the message id is new, add it.
 * If parent is defined, update this information & add child / sibling ptrs
 * Returns a ptr to the structure containing the msgid
 * NB: By definition, a msgid can only have one parent and the 1st one
 *     assigned stands.
 * TODO - better handling on duplicate parent assignment ?
 */
struct t_msgid *
add_msgid(msgid, newparent)
	char *msgid;
	struct t_msgid *newparent;
{
	struct t_msgid *ptr;
	struct t_msgid *i = NULL;
	unsigned int h;

	if (!msgid)								/* Shouldn't happen */
		return(NULL);

	h = hash_msgid(msgid+1);				/* Don't hash the initial '<' */

	/*
	 * Look for this message id in the cache. Broken software will sometimes
	 * not preserve the original case of a message-id.
	 */
	for (i = msgids[h]; i != NULL; i = i->next) {

		if (strcasecmp(i->txt, msgid) == 0) {

			/*
			 * Only update the parent if not already set. In theory, a
			 * message-id can only follow up to one other message id. Due
			 * to broken headers, this is not always the case.
			 */

			/* Parent update not required */
			if ((newparent == NULL) || (newparent == i->parent)) {
#ifdef DEBUG_REFS
				if (newparent == i->parent)
					fprintf(stderr, "dup: %s -> no change\n", msgid);
#endif
				return(i);
			}
				
			/* Change from null -> not-null */
			if (i->parent == NULL) {
				i->parent = newparent;
				add_to_parent(i);
#ifdef DEBUG_REFS
				fprintf(stderr, "set: %s -> %s\n", msgid,
										(newparent)?newparent->txt:"None");
#endif
				return(i);
			}

			/* This is bad - caused by bad headers ? */
/* TODO - add preference to msgids over refs to cut this down */
			if (i->parent != newparent) {
#ifdef DEBUG_REFS
				fprintf(stderr, "Warning: %s -> %s (already %s)!\n",
								msgid, (newparent)?newparent->txt:"None",
								i->parent->txt);
#endif
			}

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
	add_to_parent(ptr);					/* Setup children / siblings */
#endif

	/*
	 * Makes no difference where we insert into the hash buckets
	 */
	ptr->next = msgids[h];
	msgids[h] = ptr;

	return(ptr);
}

/*-------------------------------------------------------------------------
 * Take a raw line of references data and return a ptr to a linked list of
 * msgids, starting with the most recent entry. (Thus the list is reversed)
 * Following the parent ptrs leads us back to the start of the thread.
 *
 * We iterate through the refs, adding each to the msgid cache, with
 * the previous ref as the parent. 
 * The space saving vs. storing the refs as a single string is large
 */
struct t_msgid *
parse_references (r)
	char *r;
{
	char *ptr;
	struct t_msgid *parent, *current;

	if (!r)
		return(NULL);

	/*
	 * Break the refs down, using ' ' and <TAB> as delimiters
	 * A msgid can't contain a ' ', right ?
	 */
	if ((ptr = strtok(r, " \t")) == NULL)
		return(NULL);

	parent = NULL;					/* By defn, top of thread has no parent */
	current = add_msgid(ptr, parent);

	while ((ptr = strtok(NULL, " \t")) != NULL) {
		parent = current;
		current = add_msgid(ptr, parent);
	}

	return(current);
}

/*-------------------------------------------------------------------------
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
			fprintf(stderr, "Warning: Too many refs (%d). Truncated\n", MAX_REFS);
			sleep(2);
		}

		refs = my_malloc(HEADER_LEN);
		len  = 0;
	} else
		refs = _get_references(refptr->parent, depth+1);

	/*
	 * Attempt at damage limitation in case of broken Refs fields
	 */
	if (len < HEADER_LEN-500)
		len += sprintf(refs + len, "%s ", refptr->txt);

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

	refs = my_realloc(refs, len);

	return(refs);
}

/*-------------------------------------------------------------------------
 * Clear the entire msgid cache, freeing up all chains. This is
 * normally only needed when entering a new group
 */
/* TODO - Fixup the new code */
#if 0
void
free_msgids()
{
	int i;
    struct t_msgid *ptr, *next, *msgptr;

	msgptr = &msgids[0];		/* first list */

	for (i = MSGID_HASH_SIZE-1; i >= 0 ; i--) {	/* count down is faster */
		ptr = *msgptr;
		*msgptr++ = NULL;		/* declare list empty */

		while (ptr != NULL) {	/* for each node in the list */

			next = ptr->next;	/* grab ptr before we free node */
			free(ptr->txt);		/* release text */
			free(ptr);			/* release node */

			ptr = next;			/* hop down the chain */
		}
	}
}
#endif

#if 1
/*-------------------------------------------------------------------------
 * Clear the entire msgid cache, freeing up all chains. This is
 * normally only needed when entering a new group
 */

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

/*========================================================================
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
 * It remains for us to:
 * i)  Create pointers back from message-ids to the articles themselves
 * ii) Run the thread pointer through the articles that form threads.
 * 
 * TODO:
 * 
 * . Add threading on both references & subject.
 *   (ought to be able to reuse the current thread on subject code ??)
 * 
 * . When inserting sibling messages, we currently insert at the head of
 *   the list. This should be sorted by date. This isn't easy, as the
 *	 date isn't always available when the tree is built.
 *   (Could this be done in find_next(), by choosing the 'next' based
 *	  on date_comp() of all possible children, skipping those that
 * 	  are already inthread==TRUE as an elimination policy ?)
 *
 * . We could differentiate between bona fide root messages
 *   and root messages whose parent has expired on the group menu
 *	 (currently + is unread, - is marked unread and ' ' is read)
 */

/*-------------------------------------------------------------------------
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

/*-------------------------------------------------------------------------
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
					(msgid->article >= 0) ? 
						((arts[msgid->article].name) ?
							arts[msgid->article].name :
							arts[msgid->article].from) :
						"[- Unavailable -]"
	);

	fprintf(fp, "%-*s  %-.45s\n", 30, buff,
										(msgid->article >= 0) ?
											arts[msgid->article].subject :
											""
	);

	if (msgid->child != NULL)
		dump_thread(fp, msgid->child, level + 1);

	if (msgid->sibling != NULL)
		dump_thread(fp, msgid->sibling, level);

	return;
}

#ifdef DEBUG_REFS
/*--------------------------------------------------------------------------
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
 * An messagid will not be included in a thread if
 *  It doesn't point to an article OR
 *     (Its already threaded OR Its has been autokilled)
 */
/* WAS:		(ptr && ptr->article == ART_NORMAL) */

#define SKIP_ART(ptr)	\
	(ptr && (ptr->article == ART_NORMAL || \
		(arts[ptr->article].thread != ART_NORMAL || arts[ptr->article].killed)))

/*-------------------------------------------------------------------------
 * Find the next message in the thread.
 * We descend children before siblings, and only return articles that
 * exist in arts[] or NULL if we are truly at the end of a thread.
 * If there are no more down pointers, backtrack to find a sibling
 * to continue the thread, we note this with the bottom flag.
 */
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

/*-------------------------------------------------------------------------
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

/*-------------------------------------------------------------------------
 * Run a new set of threads through the base articles, using the
 * parent / child / sibling  / article pointers in the msgid hash.
 */
void
thread_by_reference (group)
	struct t_group *group;
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
			(arts[i].msgid->parent) ? arts[i].msgid->parent->article : -2,
			(arts[i].msgid->sibling) ? arts[i].msgid->sibling->article : -2,
			(arts[i].msgid->child) ? arts[i].msgid->child->article : -2,
			arts[i].inthread, arts[i].thread, arts[i].msgid->txt, arts[i].subject);
	}

	freopen("/dev/tty", "w", stderr);
#endif

	return;

}
#endif /* HAVE_REF_THREADING */

/* end of refs.c */
