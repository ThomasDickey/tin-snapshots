/*
 *  Project   : tin - a Usenet reader
 *  Module    : memory.c
 *  Author    : I. Lea & R. Skrenta
 *  Created   : 1991-04-01
 *  Updated   : 1994-06-05
 *  Notes     :
 *  Copyright : (c) Copyright 1991-99 by Iain Lea & Rich Skrenta
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef TIN_H
#	include "tin.h"
#endif /* !TIN_H */

/*
 * Dynamic arrays maximum & current sizes
 * num_* values are one past top of used part of array
 */
int max_active = 0;
int num_active = -1;
int max_newnews = 0;
int num_newnews = 0;
int max_art = 0;
int max_save = 0;
int num_save = 0;
/*
 * Dynamic arrays
 */
int *my_group;				/* .newsrc --> active[] */
long *base;				/* base articles for each thread */
struct t_group *active;			/* active newsgroups */
struct t_newnews *newnews;		/* active file sizes on differnet servers */
struct t_article *arts;			/* articles headers in current group */
struct t_save *save;			/* sorts articles before saving them */

/*
 * Local prototypes
 */
static void free_newnews_array (void);
static void free_active_arrays (void);

/*
 *  Dynamic table management
 *  These settings are memory conservative:  small initial allocations
 *  and a 50% expansion on table overflow.  A fast vm system with
 *  much memory might want to start with higher initial allocations
 *  and a 100% expansion on overflow, especially for the arts[] array.
 */

void
init_alloc (
	void)
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

#ifndef USE_CURSES
	screen = (struct t_screen *) 0;
#endif /* !USE_CURSES */
}


void
expand_art (
	void)
{
	max_art += max_art / 2;		/* increase by 50% */

	arts = (struct t_article *) my_realloc ((char *) arts, sizeof(*arts) * max_art);
	base = (long *) my_realloc ((char *) base, sizeof(long) * max_art);
}


void
expand_active (
	void)
{
	max_active += max_active / 2;		/* increase by 50% */

	if (active == (struct t_group *) 0) {
		active = (struct t_group *) my_malloc (sizeof (*active) * max_active);
		my_group = (int *) my_malloc (sizeof (int) * max_active);
	} else {
		active = (struct t_group *) my_realloc((char *) active, sizeof (*active) * max_active);
		my_group = (int *) my_realloc((char *) my_group, sizeof (int) * max_active);
	}
}


void
expand_save (
	void)
{
	max_save += max_save / 2;		/* increase by 50% */

	save = (struct t_save *) my_realloc((char *) save,
		sizeof (struct t_save) * max_save);
}


void
expand_newnews (
	void)
{
	max_newnews += max_newnews / 2;			/* increase by 50% */

	newnews = (struct t_newnews *) my_realloc((char *) newnews,
		sizeof(struct t_newnews) * max_newnews);
}


#ifndef USE_CURSES
void
init_screen_array (
	t_bool allocate)
{
	int i;

	if (allocate) {
		screen = (struct t_screen *) my_malloc (
			sizeof (struct t_screen) * cLINES+1);

		for (i = 0; i < cLINES; i++)
			screen[i].col = (char *) my_malloc ((size_t)(cCOLS+2));

	} else {
		if (screen != (struct t_screen *) 0) {
			for (i = 0; i < cLINES; i++)
				FreeAndNull(screen[i].col);

			free ((char *) screen);
			screen = (struct t_screen *) 0;
		}
	}
}
#endif /* !USE_CURSES */


void
free_all_arrays (
	void)
{
	hash_reclaim ();

#ifndef USE_CURSES
	init_screen_array (FALSE);
#endif /* !USE_CURSES */

	free_art_array ();
	free_msgids ();

	if (arts != (struct t_article *) 0) {
		free ((char *) arts);
		arts = (struct t_article *) 0;
	}

	free_all_filter_arrays();
	free_active_arrays ();

#ifdef HAVE_COLOR
	FreeIfNeeded(quote_regex.re);
	FreeIfNeeded(quote_regex.extra);
	FreeIfNeeded(quote_regex2.re);
	FreeIfNeeded(quote_regex2.extra);
	FreeIfNeeded(quote_regex3.re);
	FreeIfNeeded(quote_regex3.extra);
#endif /* HAVE_COLOR */
	FreeIfNeeded(strip_re_regex.re);
	FreeIfNeeded(strip_re_regex.extra);
	FreeIfNeeded(strip_was_regex.re);
	FreeIfNeeded(strip_was_regex.extra);

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

	if (newnews != (struct t_newnews *) 0) {
		free_newnews_array ();
		if (newnews != (struct t_newnews *) 0) {
			free ((char *) newnews);
			newnews = (struct t_newnews *) 0;
		}
	}
}


void
free_art_array (
	void)
{
	register int i;

	for (i = 0; i < top; i++) {
		arts[i].artnum = 0L;
		arts[i].thread = ART_EXPIRED;
		arts[i].inthread = FALSE;
		arts[i].status = ART_UNREAD;
		arts[i].killed = FALSE;
		arts[i].tagged = FALSE;
		arts[i].selected = FALSE;
		arts[i].date = (time_t) 0;

		FreeAndNull(arts[i].part);
		FreeAndNull(arts[i].patch);
		FreeAndNull(arts[i].xref);

		/* .refs & .msgid are cleared in build_references() */
		arts[i].refs = (char *) '\0';
		arts[i].msgid = (char *) '\0';
	}
}


/* FIXME: checks which entrys are missing and should be freed */
void
free_attributes_array (
	void)
{
	register int i;
	struct t_group *psGrp;

	for (i = 0; i < num_active; i++) {
		psGrp = &active[i];
		if (psGrp->attribute && !psGrp->attribute->global) {
			if (psGrp->attribute->maildir != (char *) 0 &&
				psGrp->attribute->maildir != tinrc.maildir) {
				free ((char *) psGrp->attribute->maildir);
				psGrp->attribute->maildir = (char *) 0;
			}
			if (psGrp->attribute->savedir != (char *) 0 &&
			    psGrp->attribute->savedir != tinrc.savedir) {
				free ((char *) psGrp->attribute->savedir);
				psGrp->attribute->savedir = (char *) 0;
			}

			FreeAndNull(psGrp->attribute->savefile);

			if (psGrp->attribute->sigfile != (char *) 0 &&
			    psGrp->attribute->sigfile != tinrc.sigfile) {
				free ((char *) psGrp->attribute->sigfile);
				psGrp->attribute->sigfile = (char *) 0;
			}

			if (psGrp->attribute->organization != (char *) 0 &&
			    psGrp->attribute->organization != default_organization) {
				free ((char *) psGrp->attribute->organization);
				psGrp->attribute->organization = (char *) 0;
			}

			FreeAndNull(psGrp->attribute->from);
			FreeAndNull(psGrp->attribute->followup_to);

#ifndef DISABLE_PRINTING
			if (psGrp->attribute->printer != (char *) 0 &&
			    psGrp->attribute->printer != tinrc.printer) {
				free ((char *) psGrp->attribute->printer);
				psGrp->attribute->printer = (char *) 0;
			}
#endif /* !DISABLE_PRINTING */

			FreeAndNull(psGrp->attribute->quick_kill_scope);
			FreeAndNull(psGrp->attribute->quick_select_scope);

			FreeAndNull(psGrp->attribute->mailing_list);
			FreeAndNull(psGrp->attribute->x_headers);
			FreeAndNull(psGrp->attribute->x_body);

#ifdef HAVE_ISPELL
			FreeAndNull(psGrp->attribute->ispell);
#endif /* HAVE_ISPELL */

			free ((char *) psGrp->attribute);
		}
		psGrp->attribute = (struct t_attribute *) 0;
	}
}


static void
free_active_arrays (
	void)
{
	register int i;

	if (my_group != (int *) 0) {			/* my_group[] */
		free ((char *) my_group);
		my_group = (int *) 0;
	}

	if (active != (struct t_group *) 0) {		/* active[] */
		for (i = 0; i < num_active; i++) {

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
free_save_array (
	void)
{
	int i;

	for (i = 0; i < num_save; i++) {

		FreeAndNull(save[i].subject);
		FreeAndNull(save[i].archive);
		FreeAndNull(save[i].dir);
		FreeAndNull(save[i].file);
		FreeAndNull(save[i].part);
		FreeAndNull(save[i].patch);

		save[i].index = -1;
		save[i].saved = FALSE;
		save[i].is_mailbox = FALSE;
	}

	num_save = 0;
}

static void
free_newnews_array (
	void)
{
	int i;

	for (i = 0; i < num_newnews; i++)
		FreeAndNull(newnews[i].host);

	num_newnews = 0;
}


void *
my_malloc1 (
	const char *file,
	int line,
	size_t size)
{
	char *p;

#ifdef DEBUG
	vDbgPrintMalloc (TRUE, file, line, size);
#endif /* DEBUG */

	if ((p = (char *) malloc (size)) == (char *) 0) {
		error_message (txt_out_of_memory, tin_progname, size, file, line);
		giveup();
	}
	return (void *) p;
}


void *
my_realloc1 (
	const char *file,
	int line,
	char *p,
	size_t size)
{

#ifdef DEBUG
	vDbgPrintMalloc (FALSE, file, line, size);
#endif /* DEBUG */

	p = (char *) ((!p) ? (calloc (1, size)) : realloc (p, size));

	if (p == (char *) 0) {
		error_message (txt_out_of_memory, tin_progname, size, file, line);
		giveup();
	}
	return (void *) p;
}
