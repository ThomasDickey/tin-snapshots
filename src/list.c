/*
 *  Project   : tin - a Usenet reader
 *  Module    : list.c
 *  Author    : I. Lea
 *  Created   : 1993-12-18
 *  Updated   : 1997-01-07
 *  Notes     : Low level functions handling the active[] list and its group_hash index
 *  Copyright : (c) Copyright 1991-99 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */

int group_hash[TABLE_SIZE];	/* group name --> active[] */

void
init_group_hash (
	void)
{
	int i;

	if (num_active == -1) {
		num_active = 0;
		for (i = 0; i < TABLE_SIZE; i++)
			group_hash[i] = -1;
	}
}

/*
 * hash group name for fast lookup later
 */
unsigned long
hash_groupname (
	const char *group)
{
#ifdef NEW_HASH_METHOD	/* still testing */
	unsigned long hash = 0L, g, val;
	/* prime == smallest prime number greater than size of string table */
	int prime = 1423;
	char *p;

	for (p = group; *p; p++) {
		hash = (hash << 4) + *p;
		if (g = hash & 0xf0000000) {
			hash ^= g >> 24;
			hash ^= g;
		}
	}
	val = hash % prime;
/*
my_printf ("hash=[%s] [%ld]\n", group, val);
*/
	return val;
#else
	unsigned long hash_value = 0L;
	unsigned int len = 0;
	const unsigned char *ptr = (const unsigned char *) group;

	while (*ptr) {
		hash_value = (hash_value << 1) ^ *ptr++;
		if (++len & 7)
			continue;
		hash_value %= TABLE_SIZE;
	}
	hash_value %= TABLE_SIZE;

	return (hash_value);
#endif /* NEW_HASH_METHOD */
}

/*
 *  Find group name in active[] array and return index otherwise -1
 */
int
find_group_index (
	const char *group)
{
	int i;
	unsigned long h;

	h = hash_groupname (group);
	i = group_hash[h];

	/*
	 * hash linked list chaining
	 */
	while (i >= 0) {
		if (STRCMPEQ(group, active[i].name))
			return i;

		i = active[i].next;
	}

	return -1;
}

/*
 *  Find group name in active[] array and return pointer to element
 */
struct t_group *
psGrpFind (
	char *pcGrpName)
{
	int i;

	if ((i = find_group_index(pcGrpName)) != -1)
		return &active[i];

	return (struct t_group *) 0;
}

/*
 * Make sure memory available for next active slot
 * Adds group to the group_hash of active groups and name it
 * Return pointer to next free active slot or NULL if duplicate
 */
struct t_group *
psGrpAdd (
	char *group)
{
	unsigned long h;
	int i;

	if (num_active >= max_active)		/* Grow memory area if needed */
		expand_active ();

	h = hash_groupname (group);

	if (group_hash[h] == -1)
		group_hash[h] = num_active;
	else {	/* hash linked list chaining */
		for (i = group_hash[h]; active[i].next >= 0; i = active[i].next) {
			if (STRCMPEQ(active[i].name, group))
				return NULL;			/* kill dups */
		}

		if (STRCMPEQ(active[i].name, group))
			return NULL;

		active[i].next = num_active;
	}

	active[num_active].name = my_strdup(group);

	return(&active[num_active++]);
}
