/*
 *  Project   : tin - a Usenet reader
 *  Module    : list.c
 *  Author    : I.Lea
 *  Created   : 18-12-93
 *  Updated   : 18-12-93
 *  Notes     : Functions used in transition to using linked list
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

static int iGrpCurNum = -1;
static int iGrpMaxNum = -1;

/*
 *  Find group name in active[] array and return pointer to element
 */
 
struct t_group *
psGrpFind (pcGrpName)
	char *pcGrpName;
{
	int		iNum;
	long	lHash;

	lHash = hash_groupname (pcGrpName);

	iNum = group_hash[lHash];
	
	/* 
	 * hash linked list chaining
	 */
	while (iNum >= 0) 
	{
		if (STRCMPEQ(pcGrpName, active[iNum].name)) 
		{
			iGrpCurNum = iNum;
			return &active[iNum];
		}
		iNum = active[iNum].next;
	}

	return (struct t_group *) 0;
}

struct t_group *
psGrpFirst ()
{
	iGrpMaxNum = num_active;

	if (iGrpMaxNum)
	{
		iGrpCurNum = 0;
		return &active[iGrpCurNum];
	} 
	else 
	{
		return (struct t_group *) 0;
	}
}

struct t_group *
psGrpLast ()
{
	if (iGrpMaxNum)
	{
		iGrpCurNum = iGrpMaxNum - 1;
		return &active[iGrpCurNum];
	} 
	else 
	{
		return (struct t_group *) 0;
	}
}

struct t_group *
psGrpNext ()
{
	if (iGrpMaxNum)
	{
		if (iGrpCurNum < (iGrpMaxNum - 1)) 
		{
			iGrpCurNum++;
			return &active[iGrpCurNum];
		}
	} 

	return (struct t_group *) 0;
}

struct t_group *
psGrpPrev ()
{
	if (iGrpMaxNum)
	{
		if (iGrpCurNum > 1) 
		{
			iGrpCurNum--;
			return &active[iGrpCurNum];
		}
	} 

	return (struct t_group *) 0;
}


void
vGrpTest ()
{
	int i = 0;
	struct t_group *psGrp;
	
printf ("First grp & then Next until end\n");
	for (psGrp = psGrpFirst (); psGrp; psGrp = psGrpNext ()) 
	{
		printf ("grp[%d]=[%s]\n", i, psGrp->name);		
		i++;
	}	

printf ("Find grp & then Next 5\n");

	psGrp = psGrpFind ("comp.archives");
	if (psGrp) 
	{
		printf ("Find grp=[%s]\n", psGrp->name);		
		i=0;
		psGrp = psGrpNext ();
		while (psGrp && i < 5)
		{
			printf ("Next grp=[%s]\n", psGrp->name);		
			psGrp = psGrpNext ();
			i++;
		}	
	}

printf ("Find grp & then Prev 5\n");

	psGrp = psGrpFind ("comp.archives");
	if (psGrp) 
	{
		printf ("Find grp=[%s]\n", psGrp->name);		
		i=0;
		psGrp = psGrpPrev ();
		while (psGrp && i < 5)
		{
			printf ("Prev grp=[%s]\n", psGrp->name);		
			psGrp = psGrpPrev ();
			i++;
		}	
	}

printf ("Find grp thats not there\n");

	psGrp = psGrpFind ("NO.group");
	if (psGrp) 
	{
		printf ("Find grp=[%s]\n", psGrp->name);		
	}
	else
	{
		printf ("Find grp NOT FOUND=[%s]\n", "NO.group");		
	}
}
