/*
 *  Project   : tin - a Usenet reader
 *  Module    : memory.c
 *  Author    : I.Lea & R.Skrenta
 *  Created   : 01-04-91
 *  Updated   : 05-06-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

/*
 * Dynamic arrays maximum & current sizes
 * num_* values are one past top of used part of array
 */
int max_active = 0;
int num_active = -1;
int max_newnews = 0;
int num_newnews = 0;
int max_art = 0;
int max_kill = 0;
int num_kill = 0;
int max_save = 0;
int num_save = 0;
int max_spooldir = 0;
int num_spooldir = 0;

/*
 * Dynamic arrays
 */
int *my_group;				/* .newsrc --> active[] */
long *base;				/* base articles for each thread */
struct t_group *active;			/* active newsgroups */
struct t_newnews *newnews;		/* active file sizes on differnet servers */
struct t_article *arts;			/* articles headers in current group */
struct t_save *save;			/* sorts articles before saving them */
struct t_spooldir *spooldirs;		/* spooldirs on NNTP server (cdrom) */


/*
 *  Dynamic table management
 *  These settings are memory conservative:  small initial allocations
 *  and a 50% expansion on table overflow.  A fast vm system with
 *  much memory might want to start with higher initial allocations
 *  and a 100% expansion on overflow, especially for the arts[] array.
 */

void
init_alloc ()
{
	/*
	 * active file arrays
	 */
	max_active = get_active_num ();
	max_newnews = DEFAULT_NEWNEWS_NUM;

	active = (struct t_group *) my_malloc (sizeof(*active) * max_active);
	newnews = (struct t_newnews *) my_malloc (sizeof(*newnews) * max_newnews);
	my_group = (int *) my_malloc (sizeof(int) * max_active);

	/*
	 * article headers array
	 */
	max_art = DEFAULT_ARTICLE_NUM;

	arts = (struct t_article *) my_malloc (sizeof(*arts) * max_art);
	base = (long *) my_malloc (sizeof(long) * max_art);

	/*
	 * save file array
	 */
	max_save = DEFAULT_SAVE_NUM;

	save = (struct t_save *) my_malloc (sizeof(*save) * max_save);

	/*
	 * spooldirs array
	 */
	max_spooldir = DEFAULT_SPOOLDIR_NUM;

	spooldirs = (struct t_spooldir *) my_malloc (sizeof(*spooldirs) * max_spooldir);

	screen = (struct t_screen *) 0;
}


void
expand_art ()
{
	max_art += max_art / 2;		/* increase by 50% */

	arts = (struct t_article *) my_realloc ((char *) arts, sizeof(*arts) * max_art);
	base = (long *) my_realloc ((char *) base, sizeof(long) * max_art);
}


void
expand_active ()
{
	max_active += max_active / 2;		/* increase by 50% */

	if (active == (struct t_group *) 0) {
		active = (struct t_group *) my_malloc (sizeof (*active) * max_active);
		my_group = (int *) my_malloc (sizeof (int) * max_active);
	} else {
		active = (struct t_group *) my_realloc((char *) active,
				 sizeof (*active) * max_active);
		my_group = (int *) my_realloc((char *) my_group, sizeof (int) * max_active);
	}
}


void
expand_save ()
{
	max_save += max_save / 2;		/* increase by 50% */

	save = (struct t_save *) my_realloc((char *) save,
		sizeof (struct t_save) * max_save);
}


void
expand_spooldirs ()
{
	max_spooldir += max_spooldir / 2;	/* increase by 50% */

	spooldirs = (struct t_spooldir *) my_realloc((char *) spooldirs,
		sizeof (struct t_spooldir) * max_spooldir);
}


void
expand_newnews ()
{
	max_newnews += max_newnews / 2;			/* increase by 50% */

	newnews = (struct t_newnews *) my_realloc((char *) newnews,
		sizeof(struct t_newnews) * max_newnews);
}


void
init_screen_array (allocate)
	int allocate;
{
	int i;

	if (allocate) {
		screen = (struct t_screen *) my_malloc (
			sizeof (struct t_screen) * cLINES+1);

		for (i=0 ; i < cLINES ; i++) {
			screen[i].col = (char *) my_malloc (cCOLS+2);
		}
	} else {
		if (screen != (struct t_screen *) 0) {
			for (i=0 ; i < cLINES ; i++) {
				FreeAndNull(screen[i].col);
			}

			free ((char *) screen);
			screen = (struct t_screen *) 0;
		}
	}
}


void
free_all_arrays ()
{
	hash_reclaim ();

	init_screen_array (FALSE);

	free_art_array ();

	free_msgids ();

	if (arts != (struct t_article *) 0) {
		free ((char *) arts);
		arts = (struct t_article *) 0;
	}

	free_all_filter_arrays();
	free_active_arrays ();

	if (base != (long *) 0) {
		free ((char *) base);
		base = (long *) 0;
	}

	if (save != (struct t_save *) 0) {
		free_save_array ();
		if (save != (struct t_save *) 0) {
			free ((char *) save);
			save = (struct t_save *) 0;
		}
	}

	if (spooldirs != (struct t_spooldir *) 0) {
		free_spooldirs_array ();
		if (spooldirs != (struct t_spooldir *) 0) {
			free ((char *) spooldirs);
			spooldirs = (struct t_spooldir *) 0;
		}
	}

	if (newnews != (struct t_newnews *) 0) {
		free_newnews_array ();
		if (newnews != (struct t_newnews *) 0) {
			free ((char *) newnews);
			newnews = (struct t_newnews *) 0;
		}
	}
}


void
free_art_array ()
{
	register int i;

	for (i=0 ; i < top ; i++) {
		arts[i].artnum = 0L;
		arts[i].thread = ART_EXPIRED;
		arts[i].inthread = FALSE;
		arts[i].status = ART_UNREAD;
		arts[i].killed = FALSE;
		arts[i].tagged = FALSE;
		arts[i].selected = FALSE;
		arts[i].date = 0L;
		
		FreeAndNull(arts[i].part);
		FreeAndNull(arts[i].patch);
		FreeAndNull(arts[i].xref);

		/* .refs & .msgid are cleared in build_references() */
		arts[i].refs = '\0';
		arts[i].msgid = '\0';
	}
}


void
free_attributes_array ()
{
	register int i;
	struct t_group *psGrp;

	for (i = 0 ; i < num_active ; i++) {
		psGrp = &active[i];
		if (psGrp->attribute && psGrp->attribute->global == FALSE) {
			if (psGrp->attribute->maildir != (char *) 0 &&
				psGrp->attribute->maildir != default_maildir) {
				free ((char *) psGrp->attribute->maildir);
				psGrp->attribute->maildir = (char *) 0;
			}
			if (psGrp->attribute->savedir != (char *) 0 &&
			    psGrp->attribute->savedir != default_savedir) {
				free ((char *) psGrp->attribute->savedir);
				psGrp->attribute->savedir = (char *) 0;
			}
			if (psGrp->attribute->organization != (char *) 0 &&
			    psGrp->attribute->organization != default_organization) {
				free ((char *) psGrp->attribute->organization);
				psGrp->attribute->organization = (char *) 0;
			}
			if (psGrp->attribute->sigfile != (char *) 0 &&
			    psGrp->attribute->sigfile != default_sigfile) {
				free ((char *) psGrp->attribute->sigfile);
				psGrp->attribute->sigfile = (char *) 0;
			}
			if (psGrp->attribute->printer != (char *) 0 &&
			    psGrp->attribute->printer != default_printer) {
				free ((char *) psGrp->attribute->printer);
				psGrp->attribute->printer = (char *) 0;
			}

			FreeAndNull(psGrp->attribute->followup_to);			
			FreeAndNull(psGrp->attribute->quick_kill_scope);
			FreeAndNull(psGrp->attribute->quick_select_scope);

			free ((char *) psGrp->attribute);
		}
		psGrp->attribute = (struct t_attribute *) 0;
	}
}


void
free_active_arrays ()
{
	register int i;

	if (my_group != (int *) 0) {			/* my_group[] */
		free ((char *) my_group);
		my_group = (int *) 0;
	}

	if (active != (struct t_group *) 0) {		/* active[] */
		for (i=0 ; i < num_active ; i++) {

			FreeAndNull(active[i].name);			
			FreeAndNull(active[i].description);
			
			if (active[i].type == GROUP_TYPE_MAIL &&
			    active[i].spooldir != (char *) 0) {
				free ((char *) active[i].spooldir);
				active[i].spooldir = (char *) 0;
			}
			if (active[i].newsrc.xbitmap != 0) {
				free ((char *) active[i].newsrc.xbitmap);
				active[i].newsrc.xbitmap = 0;
			}
			if (active[i].grps_filter != 0) {
				free ((char *) active[i].grps_filter);
				active[i].grps_filter = 0;
			}
		}

		free_attributes_array ();

		if (active != (struct t_group *) 0) {
			free ((char *) active);
			active = (struct t_group *) 0;
		}
	}
	num_active = -1;
}


void
free_save_array ()
{
	int i;

	for (i=0 ; i < num_save ; i++) {
	
		FreeAndNull(save[i].subject);
		FreeAndNull(save[i].archive);
		FreeAndNull(save[i].dir);
		FreeAndNull(save[i].file);
		FreeAndNull(save[i].part);
		FreeAndNull(save[i].patch);
		
		save[i].index   = -1;
		save[i].saved   = FALSE;
		save[i].is_mailbox = FALSE;
	}

	num_save = 0;
}


void
free_spooldirs_array ()
{
	int i;

	for (i=0 ; i < num_spooldir ; i++) {

		FreeAndNull(spooldirs[i].name);
		FreeAndNull(spooldirs[i].comment);

		spooldirs[i].state = 0;
	}

	num_spooldir = 0;
}


void
free_newnews_array ()
{
	int i;

	for (i=0 ; i < num_newnews ; i++) {	
		FreeAndNull(newnews[i].host);
	}

	num_newnews = 0;
}


void *
my_malloc1 (file, line, size)
	char *file;
	int line;
	size_t size;
{
	char	buf[128];
	char	*p;

#ifdef DEBUG
	vDbgPrintMalloc (TRUE, file, line, size);
#endif

/*	if ((p = (char *) calloc (1, size)) == NULL) { */
	if ((p = (char *) malloc (size)) == NULL) {

		sprintf (buf, txt_out_of_memory, progname, size, file, line);
		error_message (buf, "");

/*		vPrintBugAddress (); */
/*		tin_done (1); */
		exit (1);
	}
	return (void *) p;
}


void *
my_realloc1 (file, line, p, size)
	char *file;
	int line;
	char *p;
	size_t size;
{
	char	buf[128];

#ifdef DEBUG
	vDbgPrintMalloc (FALSE, file, line, size);
#endif
	if (!p) {
		p = (char *) calloc (1, size);
	} else {
		p = (char *) realloc (p, size);
	}

	if (p == (char *) 0) {
		sprintf (buf, txt_out_of_memory, progname, size, file, line);
		error_message (buf, "");
/*		vPrintBugAddress (); */
/*		tin_done (1); */
		exit (1);
	}
	return (void *) p;
}
