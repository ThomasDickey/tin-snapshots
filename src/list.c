/*
 *  Project   : tin - a Usenet reader
 *  Module    : list.c
 *  Author    : I.Lea
 *  Created   : 18-12-93
 *  Updated   : 07-01-97
 *  Notes     : Low level functions handling the active[] list and its group_hash index
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

#if 0
static int iGrpCurNum = -1;
static int iGrpMaxNum = -1;
#endif

int group_hash[TABLE_SIZE];			/* group name --> active[] */

void
init_group_hash (void)
{
	int i;

	if (num_active == -1) {
		num_active = 0;
		for (i = 0; i < TABLE_SIZE; i++) {
			group_hash[i] = -1;
		}
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
printf ("hash=[%s] [%ld]\n", group, val);
*/
	return val;
#else
	unsigned long hash_value = 0L;
	unsigned int len = 0;
	const unsigned char *ptr = (const unsigned char *) group;

	while (*ptr) {
		hash_value = (hash_value << 1) ^ *ptr++;
		if (++len & 7) continue;
		hash_value %= TABLE_SIZE;
	}
	hash_value %= TABLE_SIZE;

	return (hash_value);
#endif
}

/*
 *  Find group name in active[] array and return index otherwise -1
 */
int
find_group_index (
	const char *group)
{
	int i;
	long h;

	h = hash_groupname (group);
	i = group_hash[h];

	/*
	 * hash linked list chaining
	 */
	while (i >= 0) {
		if (STRCMPEQ(group, active[i].name)) {
			return i;
		}
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

	if ((i = find_group_index(pcGrpName)) != -1) {
#if 0
		iGrpCurNum = i;
#endif
		return &active[i];
	}

	return (struct t_group *) 0;
}

/*
 * Add group to the group_hash of active groups
 * utilises the num_active and max_active global variables
 */
int
psGrpAdd (
	char *group)
{
	long h;
	int i;

	if (num_active >= max_active)		/* Grow memory area if needed */
		expand_active ();

	h = hash_groupname (group);

	if (group_hash[h] == -1) {
		group_hash[h] = num_active;

	} else {	/* hash linked list chaining */

		for (i=group_hash[h]; active[i].next >= 0; i=active[i].next) {
			if (STRCMPEQ(active[i].name, group))
				return(-1);				/* kill dups */
		}

		if (STRCMPEQ(active[i].name, group))
			return(-1);

		active[i].next = num_active;
	}

	return(0);
}

/*
 * What is all the rest of this for ???
 */
#if 0
struct t_group *
psGrpFirst (void)
{
	iGrpMaxNum = num_active;

	if (iGrpMaxNum) {
		iGrpCurNum = 0;
		return &active[iGrpCurNum];
	} else {
		return (struct t_group *) 0;
	}
}

struct t_group *
psGrpLast (void)
{
	if (iGrpMaxNum) {
		iGrpCurNum = iGrpMaxNum - 1;
		return &active[iGrpCurNum];
	} else {
		return (struct t_group *) 0;
	}
}

struct t_group *
psGrpNext (void)
{
	if (iGrpMaxNum) {
		if (iGrpCurNum < (iGrpMaxNum - 1)) {
			iGrpCurNum++;
			return &active[iGrpCurNum];
		}
	}
	return (struct t_group *) 0;
}

struct t_group *
psGrpPrev (void)
{
	if (iGrpMaxNum) {
		if (iGrpCurNum > 1) {
			iGrpCurNum--;
			return &active[iGrpCurNum];
		}
	}
	return (struct t_group *) 0;
}
#endif
