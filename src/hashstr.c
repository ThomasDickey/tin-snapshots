/*
 *  Project   : tin - a Usenet reader
 *  Module    : hashstr.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 01-12-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

/*
 *  Maintain a table of all strings we have seen.
 *  If a new string comes in, add it to the table and return a pointer
 *  to it.  If we've seen it before, just return the pointer to it.
 *
 *  Usage:  hash_str("some string") returns char *
 *
 *  Spillovers are chained on the end
 */

/*
 *  Arbitrary table size, but make sure it's prime!
 */

#define		HASHNODE_TABLE_SIZE	2411

#ifdef M_AMIGA
struct t_hashnode **table=0;
#else
struct t_hashnode *table[HASHNODE_TABLE_SIZE];
#endif

static struct t_hashnode *add_string (char *s);

char *
hash_str (
	char *s)
{
	long h;				/* result of hash:  index into hash table */
	struct t_hashnode **p;	/* used to descend the spillover structs */

	if (s == (char *) 0) {
		return ((char *) 0);
	}

	{
		unsigned char *t = (unsigned char *) s;
		int len = 0;

		h = 0;
		while (*t) {
			h = (h << 1) ^ *t++;
			if (++len & 7) continue;
			h %= (long) HASHNODE_TABLE_SIZE;
		}
		h %= (long) HASHNODE_TABLE_SIZE;
	}

	p = &table[h];

	while (*p) {
		if (STRCMPEQ(s, (*p)->s)) {
			return (*p)->s;
		}
		p = &(*p)->next;
	}

	*p = add_string (s);
	return (*p)->s;			/* Return ptr to text, _not_ the struct */
}


/*
 * Add a string to the hash table
 * Each entry will have the following structure:
 *
 * char *s		-+			Pointer to the text
 * char *next	 |			Pointer to the next hashnode in chain
 * int			 |			'magic' ptr used to speed subj threading
 * int			 |			Unknown ptr ?
 * T		 <---+			The text itself. The ptr that hash_str()
 * E					    returns points here - the earlier fields
 * X						are 'hidden'.
 * T
 * \0						String terminator
 */
static struct t_hashnode *
add_string (
	char *s)
{
	int *iptr;
	struct t_hashnode *p;

	p = (struct t_hashnode *) my_malloc (sizeof (struct t_hashnode)
		+ sizeof (int) * 2 + strlen(s) + 1);

	p->next = (struct t_hashnode *) 0;

	iptr = (int *) &p[1];

	*iptr++ = -1;			/* Initialise 'magic' ptr to -1 */
	*iptr++ = -1;			/* TODO: Is this ever used ? */

	p->s = (char *) iptr;	/* Bolt the text onto the end */
	strcpy (p->s, s);

	return p;
}


void
hash_init (void)
{
	int i;

#ifdef M_AMIGA
	if (!table) {
		table = (struct t_hashnode **) my_malloc (HASHNODE_TABLE_SIZE * sizeof (void *));
	}
#endif

	for (i = 0; i < HASHNODE_TABLE_SIZE; i++) {
		table[i] = (struct t_hashnode *) 0;
	}
}


void
hash_reclaim (void)
{
	int i;
	struct t_hashnode *p, *next;

#ifdef M_AMIGA
	if (!table) {
		return;
	}
#endif

	for (i = 0; i < HASHNODE_TABLE_SIZE; i++)
		if (table[i] != (struct t_hashnode *) 0) {
			p = table[i];
			while (p != (struct t_hashnode *) 0) {
				next = p->next;
				free ((char *) p);
				p = next;
			}
			table[i] = (struct t_hashnode *) 0;
		}
}
