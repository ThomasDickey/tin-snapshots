/*
 *  Project   : tin - a Usenet reader
 *  Module    : refs.c
 *  Author    : Jason Faultless <jason@radar.demon.co.uk>
 *  Created   : 09-05-96
 *  Notes     : Implementation of References: based threading
 *  Copyright : (c) 1996 by Jason Faultless
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifdef REF_THREADING
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
 *   this will happen
 *
 * This is simplistic, so check out RFC1036 & son of RFC1036 for full
 * details from the posting point of view.
 * 
 * We attempt to maintain 3 pointers in each article to handle threading
 * on References:
 *
 * 1) parent  - the article that the current one was in reply to
 *              An article with no References: has no parent, therefore
 *              it is the root of a thread.
 * 
 * 2) sibling - the next reply in sequence to parent.
 *
 * 3) child   - the first reply to this current article.
 *
 * 
 * FINDING PARENT:
 *
 * If there is no (or NULL for some reason) references, we are a
 * root message.
 * Due to the out-of-order delivery nature of UseNet, if the parent is
 * not found, we backtrack through the References history to try
 * to find an 'upstream' match.
 *
 * If no parent article can be located, we are a root message.
 *
 * FINDING CHILDREN:
 *
 * We need to insert ourself into the parents list of children.
 * If we are the first child (parent->child == NULL), things are simple.
 *
 * If parent->child is not NULL, we insert outself at the head of
 * the parents list of children.
 *
 * TODO:
 *
 * . Hash all message id's. This includes both the Message-id and
 *   all id's in the References header. Quite a bit of memory can be
 *   saved by doing this - there is great duplication of msgids within
 *   a group.
 *
 * . Hashed message ids will greatly speed up location of parent.
 *   Current code is O(n^2), just like the subject threading was in
 *   tin-1.22
 *
 * . Hashing all references will allow us to combine threads by
 *   finding common expired ancestors.
 *
 * . Add threading on both references then subject.
 * 
 * All of the above should hopefully get tin up to date in the
 * threading arena.
 * 
 * . When inserting child messages, should we insert at the beginning,
 *   the end, or sort child threads on date ? Does this matter ?
 *   The whole group is presorted by date anyway.
 *	 What do other readers do?
 *
 * . We could differentiate between bona fide root messages
 *   and messages whose parent has expired.
 */

#include "tin.h"

FILE *dumpfd;

/*-------------------------------------------------------------------------
 * THIS IS ABYSMALLY INEFFICIENT
 * *** Currently does a linear search, needs hashing ***
 * Find an article by its message id, returning its index in arts[]
 */
static int
find_by_msgid(msgid)
	char *msgid;
{
	int i;

	for (i=0 ; i < top ; i++) {
/*		fprintf(stderr, "!%3d %3s (%s) (%s)\n", i, (strcmp(arts[i].msgid, msgid) == 0) ? "YES" : "NO ", arts[i].msgid, msgid);*/
		if (strcmp(arts[i].msgid, msgid) == 0)
			return(i);
	}

	return(NO_THREAD);
}

/*-------------------------------------------------------------------------
 * Reverse-search refs list looking for a matching article
 * I know rewriting strings is evil, but this routine gets called
 * a _lot_
 * It will all change once msgids are hashed anyway.
 */
int
search_older_refs(refs, end)
	char *refs;
	char *end;
{
	char *current;
	int i;

	/*
	 * Quit if nothing more to search
	 */
	if (end == refs - 1)
		return(NO_THREAD);

	*end = '\0';								/* Terminate previous */

	/*
	 * If this is the only remaining Ref, fixup current
	 */
	if ((current = strrchr(refs, ' ')) == NULL)
		current = refs - 1;
		
	/*
	 * Search the next item,
	 */
	i = find_by_msgid(current + 1);
/*fprintf(stderr, "Backref: !%s! %d\n", current+1, i);*/

	*end = ' ';									/* Restore previous */

	/*
	 * Backtrack to the previous references
	 */
	if (i == NO_THREAD)
		i = search_older_refs(refs, current);

	return(i);
}

/*-------------------------------------------------------------------------
 * Function to dump an ASCII tree style map of a thread.
 * i is the index of the root article, level is the current depth
 * of the tree.
 */
/* static */
void
dump_thread(fp, i, level)
	FILE *fp;
	int i;
	int level;
{
	char buff[120];		/* This will probably break - test only */

	/*
	 * The current article
	 */
	sprintf(buff, "%3d %*s %-.16s", i, 2*level, "  ", arts[i].name);
	fprintf(fp, "%-*s  %-.34s\n", 30, buff, arts[i].subject);

	if (arts[i].child != NO_THREAD)
		dump_thread(fp, arts[i].child, level + 1);

	if (arts[i].sibling != NO_THREAD)
		dump_thread(fp, arts[i].sibling, level);

	return;
}

/*-------------------------------------------------------------------------
 * Flatten a tree into a linear thread to mesh in with the current
 * threading code. This will do until I add a new screen to display
 * a thread tree
 *
 * This is straight recursion, except that the last most child at the
 * bottom of the tree needs a pointer to the next sibling somewhere furthur
 * up the tree
 */
static int
collapse_thread(i, level)
	int i;
	int level;
{
	int bottom = -1;			/* If set, id of last most child in sub-tree */

	/*
	 * If child tree exists, descend it
	 */
	if (arts[i].child != NO_THREAD) {

		/*
		 * There is no way this can already be set, because
		 * we only assign stray child ptrs to siblings
		 */
		arts[arts[i].child].inthread = TRUE;		/* Child is in a thread */
		arts[i].thread = arts[i].child;				/* We -> to child */

		bottom = collapse_thread(arts[i].child, level + 1);
	}

	/*
	 * If a sibling exists
	 */
	if (arts[i].sibling != NO_THREAD) {

		arts[arts[i].sibling].inthread = TRUE;		/* Sibling is in a thread */

/* The next two lines are mutually Xclusive and should be optimised ? */

		/*
	     * If we have a propogated ptr from the bottom of this thread,
		 * point it at our sibling, to preserve the flow of the thread
   	  	 */
	    if (bottom != -1)
	        arts[bottom].thread = arts[i].sibling;

		/*
		 * If we haven't already got a ptr down to a child we
		 * move on to our sibling
	 	 */
		if (arts[i].thread == NO_THREAD)
			arts[i].thread = arts[i].sibling;		/* We -> to sibling */

		bottom = collapse_thread(arts[i].sibling, level);
	}

	/*
	 * This is a leaf node at the end of subtree. We must pass its
	 * id back up to assign it to the next available sibling to
	 * preserve the thread. If the resursion unwinds without finding
	 * a sibling, then this will remain the last article in the thread.
	 */
	if (arts[i].child == NO_THREAD && arts[i].sibling == NO_THREAD)
		bottom = i;

	return(bottom);
}

/*-------------------------------------------------------------------------
 * Traverse the articles in the current group, adding parent, sibling
 * & child ptrs as needed
 */
void
thread_by_reference (group)
	struct t_group *group;
{
	int i;
	char *refer;

#if 0
	fprintf (stderr, "\nthread_arts=[%d]  attr_thread_arts=[%d]\n", 	
		default_thread_arts, group->attribute->thread_arts);
#endif

	/*
	 * Clear all the old threading pointers
	 * The parent pointers get set explicitly later on
	 */
	for (i=0 ; i < top ; i++) {

		arts[i].sibling = arts[i].child = NO_THREAD;

		if (arts[i].thread != ART_EXPIRED)
			arts[i].thread = ART_NORMAL;

		arts[i].inthread = FALSE;
	}

	/*
	 * Here goes... :)
	 */

	for (i=0 ; i < top ; i++) {

#if 0
fprintf(stderr, "------- i=[%2d] %.60s\n", i, arts[i].subject);
fprintf(stderr, "        msgid: %s\n", arts[i].msgid);
#endif

		/*
		 * If there are refs, try to find the parent
		 * Find the referred to article & look it up by msgid
		 */
		if (arts[i].refs) {

			/*
			 * Only a single reference
			 */
			if ((refer = strrchr(arts[i].refs, ' ')) == NULL)
				arts[i].parent = find_by_msgid(arts[i].refs);

			/*
			 * Multiple references, try the first one (refer -> ' ')
			 * If we do not find a parent, work back through the list
			 * of references and keep trying.
			 */
			else {
				if ((arts[i].parent = find_by_msgid(refer+1)) == NO_THREAD)
					arts[i].parent = search_older_refs(arts[i].refs, refer);
			}

			/*
			 * If we found a parent, thread us into its list of children
			 */
			if (arts[i].parent != NO_THREAD) {

				if (arts[arts[i].parent].child == NO_THREAD)
					arts[arts[i].parent].child = i;		/* 1st child */

/* TODO: For now, stick us at head of the list */
/* This code can be optimized, if we stick with it */
				else {
					arts[i].sibling = arts[arts[i].parent].child;
					arts[arts[i].parent].child = i;
				}
			}

		} else {

			/*
			 * No Refs:, mark it as root message
			 */
			arts[i].parent = NO_THREAD;
		}

#if 0
fprintf(stderr, "        paren: %d %.60s\n", arts[i].parent,
										 arts[arts[i].parent].subject);
#endif

	}		/* end of main threading loop */


	/*
	 * For now, fold the reference tree into a linear thread to conform
	 * with the current threading code
	 */
	for (i=0 ; i < top ; i++) {
		if (arts[i].parent == NO_THREAD)
			collapse_thread(i, 1);
	}

#if 0
dumpfd = fopen ("1thread", "w");

	for (i=0 ; i < top ; i++) {
		if (arts[i].parent == NO_THREAD)
			dump_thread(dumpfd, i, 1);
	}

	for (i=0 ; i < top ; i++) {
		fprintf(dumpfd, "%3d %3d %3d %3d : %3d %3d : %.50s\n", i, 
			arts[i].parent, arts[i].sibling, arts[i].child,
			arts[i].inthread, arts[i].thread, arts[i].subject);
	}

fclose(dumpfd);
#endif

	return;

}
#endif
