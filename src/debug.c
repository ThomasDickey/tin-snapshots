/*
 *  Project   : tin - a Usenet reader
 *  Module    : debug.c
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 09-12-94
 *  Notes     : debug routines
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"

int debug;

/*
** Local prototypes
*/
#ifdef DEBUG
static void debug_print_attributes (struct t_attribute *attr, FILE *fp);
static void debug_print_filter (FILE *fp, int num, struct t_filter *the_filter);
#endif

/*
 *  nntp specific debug routines
 */

void
debug_delete_files (void)
{
#ifdef DEBUG
	char file[PATH_LEN];

	if (debug) {
		sprintf (file, "%sNNTP", TMPDIR);
		unlink (file);
		sprintf (file, "%sARTS", TMPDIR);
		unlink (file);
		sprintf (file, "%sSAVE_COMP", TMPDIR);
		unlink (file);
		sprintf (file, "%sBASE", TMPDIR);
		unlink (file);
		sprintf (file, "%sACTIVE", TMPDIR);
		unlink (file);
		sprintf (file, "%sBITMAP", TMPDIR);
		unlink (file);
		sprintf (file, "%sMALLOC", TMPDIR);
		unlink (file);
		sprintf (file, "%sFILTER", TMPDIR);
		unlink (file);
	}
#endif
}


/*
 *  nntp specific debug routines
 */

void
debug_nntp (
	const char *func,
	const char *line)
{
#ifdef DEBUG
	char file[PATH_LEN];
	FILE *fp;

	if (debug == 0)
		return;

	sprintf (file, "%sNNTP", TMPDIR);

	if ((fp = fopen (file, "a+")) != (FILE *) 0) {
		fprintf (fp,"%s: %s\n", func, line);
		fclose (fp);
		chmod (file, (S_IRUGO|S_IWUGO));
	}
#endif
}


void
debug_nntp_respcode (
	int respcode)
{
#ifdef DEBUG
	debug_nntp ("get_respcode", nntp_respcode (respcode));
#endif
}

/*
 *  tin specific debug routines
 */

void
debug_print_arts (void)
{
#ifdef DEBUG
	int i;

	if (debug != 2)
		return;

	for (i = 0; i < top; i++) {	/* for each group */
		debug_print_header (&arts[i]);
	}
#endif
}


void
debug_print_header (
	struct t_article *s)
{
#ifdef DEBUG
	char file[PATH_LEN];
	FILE *fp;

	if (debug != 2)
		return;

	sprintf (file, "%sARTS", TMPDIR);

	if ((fp = fopen (file, "a+")) != (FILE *) 0) {
		fprintf (fp,"art=[%5ld] tag=[%s] kill=[%s] selected=[%s]\n", s->artnum,
			(s->tagged ? "TRUE" : "FALSE"),
			(s->killed ? "TRUE" : "FALSE"),
			(s->selected ? "TRUE" : "FALSE"));
		fprintf (fp,"subj=[%-38s]\n", s->subject);
		fprintf (fp,"date=[%ld]  from=[%s]  name=[%s]\n", s->date, s->from, s->name);
		fprintf (fp,"msgid=[%s]  refs=[%s]\n",
			(s->msgid ? s->msgid : ""),
			(s->refs ? s->refs : ""));

 		if (s->archive) {
 		    fprintf (fp, "arch=[%-38s]  ", s->archive);
 		} else {
 		    fprintf (fp, "arch=[]  ");
 		}
 		if (s->part) {
 		    fprintf (fp, "part=[%s]  ", s->part);
 		} else {
 		    fprintf (fp, "part=[]  ");
 		}
 		if (s->patch) {
 		    fprintf (fp, "patch=[%s]\n", s->patch);
		} else {
 		    fprintf (fp, "patch=[]\n");
 		}
		fprintf (fp,"thread=[%d]  inthread=[%d]  status=[%d]\n\n",
			s->thread, s->inthread, s->status);
/*		fprintf (fp,"thread=[%s]  inthread=[%s]  status=[%s]\n",
			(s->thread == ART_NORMAL ? "ART_NORMAL" : "ART_EXPIRED"),
			(s->inthread ? "TRUE" : "FALSE"),
			(s->status ? "TRUE" : "FALSE"));
*/
		fflush (fp);
		fclose (fp);
		chmod (file, (S_IRUGO|S_IWUGO));
	}
#endif
}


void
debug_save_comp (void)
{
#ifdef DEBUG
	char file[PATH_LEN];
	FILE *fp;
	int i;

	if (debug != 2)
		return;

	sprintf (file, "%sSAVE_COMP", TMPDIR);

	if ((fp = fopen (file, "a+")) != (FILE *) 0) {
		for (i = 0 ; i < num_save ; i++) {

			fprintf (fp,"subj=[%-38s]\n", save[i].subject);
			fprintf (fp,"dir=[%s]  file=[%s]\n", save[i].dir, save[i].file);
 			if (save[i].archive) {
 			    fprintf (fp, "arch=[%-38s]  ", save[i].archive);
 			} else {
 			    fprintf (fp, "arch=[]  ");
 			}
 			if (save[i].part) {
 			    fprintf (fp, "part=[%s]  ", save[i].part);
 			} else {
 			    fprintf (fp, "part=[]  ");
 			}
 			if (save[i].patch) {
 			    fprintf (fp, "patch=[%s]\n", save[i].patch);
			} else {
 			    fprintf (fp, "patch=[]\n");
 			}
			fprintf (fp,"index=[%d]  saved=[%d]  mailbox=[%d]\n\n",
				save[i].index, save[i].saved, save[i].is_mailbox);
		}
		fflush (fp);
		fclose (fp);
		chmod (file, (S_IRUGO|S_IWUGO));
	}
#endif
}


void
debug_print_comment (
	const char *comment)
{
#ifdef DEBUG_NEWSRC
	char file[PATH_LEN];
	FILE *fp;

	if (debug < 2)
		return;

	sprintf (file, "%sBITMAP", TMPDIR);

	if ((fp = fopen (file, "a+")) != (FILE *) 0) {
		fprintf (fp,"\n%s\n", comment);
		fclose (fp);
		chmod (file, (S_IRUGO|S_IWUGO));
	}
#endif
}

#ifdef DEBUG
static void
debug_print_base (void)
{
	char file[PATH_LEN];
	FILE *fp;
	int i;

	if (debug != 2)
		return;

	sprintf (file, "%sBASE", TMPDIR);

	if ((fp = fopen (file, "a+")) != (FILE *) 0) {
		for (i = 0; i < top_base; i++) {
			fprintf (fp, "base[%3d]=[%5ld]\n",i,base[i]);
		}
		fflush (fp);
		fclose (fp);
		chmod (file, (S_IRUGO|S_IWUGO));
	}
}
#endif


void
debug_print_active (void)
{
#ifdef DEBUG
	char file[PATH_LEN];
	FILE *fp;
	int i;
	struct t_group *group;

	if (debug != 2)
		return;

	sprintf (file, "%sACTIVE", TMPDIR);

	if ((fp = fopen (file, "w")) != (FILE *) 0) {
		for (i = 0; i < num_active; i++) {	/* for each group */
			group = &active[i];
/* 			fprintf (fp, "[%4d]=[%-28s] type=[%s] spooldir=[%s]\n",
*/
			fprintf (fp, "[%4d]=[%s] type=[%s] spooldir=[%s]\n",
				i, group->name,
				(group->type == GROUP_TYPE_NEWS ? "NEWS" : "MAIL"),
				group->spooldir);
			fprintf (fp, "count=[%4ld] max=[%4ld] min=[%4ld] mod=[%c]\n",
				group->count, group->xmax, group->xmin, group->moderated);
			fprintf (fp, " nxt=[%4d] hash=[%ld]  description=[%s]\n", group->next,
				hash_groupname (group->name), (group->description ? group->description : ""));
			debug_print_newsrc (&group->newsrc, fp);
			debug_print_attributes (group->attribute, fp);
		}
		fclose (fp);
		chmod (file, (S_IRUGO|S_IWUGO));
	}
#endif
}

#ifdef DEBUG
static void
debug_print_attributes (
	struct t_attribute *attr,
	FILE *fp)
{
	if (attr == 0)
		return;
	fprintf (fp, "global=[%d] show=[%d] thread=[%d] sort=[%d] author=[%d] auto_select=[%d] auto_save=[%d] batch_save=[%d] process=[%d]\n",
		attr->global,
		attr->show_only_unread,
		attr->thread_arts,
		attr->sort_art_type,
		attr->show_author,
		attr->auto_select,
		attr->auto_save,
		attr->batch_save,
		attr->post_proc_type);
	fprintf (fp, "select_header=[%d] select_global=[%s] select_expire=[%s]\n",
		attr->quick_select_header,
		(attr->quick_select_scope ? attr->quick_select_scope : ""),
		(attr->quick_select_expire ? "TRUE" : "FALSE"));
	fprintf (fp, "kill_header  =[%d] kill_global  =[%s] kill_expire  =[%s]\n",
		attr->quick_kill_header,
		(attr->quick_kill_scope ? attr->quick_kill_scope : ""),
		(attr->quick_kill_expire ? "TRUE" : "FALSE"));
	fprintf (fp, "maildir=[%s] savedir=[%s] savefile=[%s]\n",
		(attr->maildir == (char *) 0 ? "" : attr->maildir),
		(attr->savedir == (char *) 0 ? "" : attr->savedir),
		(attr->savefile == (char *) 0 ? "" : attr->savefile));
	fprintf (fp, "sigfile=[%s] followup_to=[%s]\n\n",
		(attr->sigfile == (char *) 0 ? "" : attr->sigfile),
		(attr->followup_to  == (char *) 0 ? "" : attr->followup_to));
	fflush (fp);
}
#endif


void
debug_print_bitmap (
	struct t_group *group,
	struct t_article *art)
{
#ifdef DEBUG_NEWSRC
	char file[PATH_LEN];
	FILE *fp;

	if (debug != 3) {
		return;
	}

	sprintf (file, "%sBITMAP", TMPDIR);

	if ((fp = fopen (file, "a+")) != (FILE *) 0) {
		fprintf (fp, "\nGroup=[%s] sub=[%c] min=[%ld] max=[%ld] count=[%ld] num_unread=[%ld]\n",
			group->name, SUB_CHAR(group->subscribed),
			group->xmin, group->xmax, group->count,
			group->newsrc.num_unread);
		if (art != (struct t_article *) 0) {
			fprintf (fp, "art=[%5ld] tag=[%s] kill=[%s] selected=[%s] subj=[%s]\n",
				art->artnum,
				(art->tagged ? "TRUE" : "FALSE"),
				(art->killed ? "TRUE" : "FALSE"),
				(art->selected ? "TRUE" : "FALSE"),
				art->subject);
			fprintf (fp, "thread=[%s]  inthread=[%s]  status=[%s]\n",
				(art->thread == ART_NORMAL ? "ART_NORMAL" : "ART_EXPIRED"),
				(art->inthread ? "TRUE" : "FALSE"),
				(art->status == ART_READ ? "READ" : "UNREAD"));
		}
		debug_print_newsrc (&group->newsrc, fp);
		fclose (fp);
		chmod (file, (S_IRUGO|S_IWUGO));
	}
#endif
}


#ifdef DEBUG_NEWSRC
void
debug_print_newsrc (
	struct t_newsrc *NewSrc,
	FILE *fp)
{
	register int i, j;

	fprintf (fp, "min=[%ld] max=[%ld] bitlen=[%ld] num_unread=[%ld] present=[%d]\n",
		NewSrc->xmin, NewSrc->xmax, NewSrc->xbitlen,
		NewSrc->num_unread, NewSrc->present);

	fprintf (fp, "bitmap=[");
	if (NewSrc->xbitlen && NewSrc->xbitmap) {
		for (j=0, i=NewSrc->xmin; i <= NewSrc->xmax; i++) {
			fprintf (fp, "%d",
				(NTEST(NewSrc->xbitmap, i - NewSrc->xmin) == ART_READ ?
				ART_READ : ART_UNREAD));
			if ((j++ % 8) == 7 && i < NewSrc->xmax) {
				fprintf (fp, " ");
			}
		}
	}
	fprintf (fp, "]\n");
	fflush (fp);
}
#endif


#ifdef DEBUG
void
vDbgPrintMalloc (
	int	iIsMalloc,
	const char *pcFile,
	int	iLine,
	size_t	iSize)
{
	char file[PATH_LEN];
	FILE *fp;
	static int iTotal = 0;

	if (debug == 4) {
		sprintf (file, "%sMALLOC", TMPDIR);
		if ((fp = fopen (file, "a+")) != (FILE *) 0) {
			iTotal += iSize;
			/* sometimes size_t is long */
			if (iIsMalloc) {
				fprintf (fp, "%10s:%-4d Malloc  %6ld. Total %d\n",
					pcFile, iLine, (long)iSize, iTotal);
			} else {
				fprintf (fp, "%10s:%-4d Realloc %6ld. Total %d\n",
					pcFile, iLine, (long)iSize, iTotal);
			}
			fclose (fp);
			chmod (file, (S_IRUGO|S_IWUGO));
		}
	}
}
#endif


#ifdef DEBUG
void
debug_print_filter (
	FILE *fp,
	int num,
	struct t_filter *the_filter)
{
	fprintf (fp, "[%3d]  scope=[%s] inscope=[%s] type=[%d][%s] case=[%d][%s] lines=[%d %d]\n",
		num, (the_filter->scope ? the_filter->scope : ""),
		(the_filter->inscope ? "TRUE" : "FILTER"),
		the_filter->type, (the_filter->type == 0 ? "KILL" : "SELECT"),
		the_filter->icase, (the_filter->icase == 0 ? "FALSE" : "TRUE"),
		the_filter->lines_cmp, the_filter->lines_num);
	fprintf (fp, "       subj=[%s] from=[%s] msgid=[%s]\n",
		(the_filter->subj  ? the_filter->subj  : ""),
		(the_filter->from  ? the_filter->from  : ""),
		(the_filter->msgid ? the_filter->msgid : ""));

	if (the_filter->time) {
		fprintf (fp, "       time=[%ld][%s",
			the_filter->time,
			(the_filter->time ? ctime (&the_filter->time) : "]\n"));
	}
}
#endif


void
debug_print_filters (void)
{
#ifdef DEBUG
	char file[PATH_LEN];
	FILE *fp;
	int i, j, num;
	struct t_filter *the_filter;

	if (debug < 2)
		return;

	sprintf (file, "%sFILTER", TMPDIR);

	if ((fp = fopen (file, "w")) != (FILE *) 0) {
		/*
		 * print global filter
		 */
		num = glob_filter.num;
		the_filter = glob_filter.filter;
		fprintf (fp, "*** BEG GLOBAL FILTER=[%3d] ***\n", num);
		for (i=0 ; i < num ; i++) {
			debug_print_filter (fp, i, &the_filter[i]);
			fprintf (fp, "\n");
 		}
		fprintf (fp, "*** END GLOBAL FILTER ***\n");

		for (j=0 ; j < num_active ; j++) {
			if (active[j].grps_filter) {
				num = active[j].grps_filter->num;
				the_filter = active[j].grps_filter->filter;
				fprintf (fp, "\n*** group=[%3d] [%s]\n", num, active[j].name);
				for (i=0 ; i < num ; i++) {
					debug_print_filter (fp, i, &the_filter[i]);
 				}
 			}
 		}
		fclose (fp);
		chmod (file, (S_IRUGO|S_IWUGO));
 	}
#endif
}


#ifdef DEBUG

/*
 * Prints out hash distribution of active[]
 */

static void
debug_print_active_hash (void)
{
	int empty = 0, number;
	int collisions[32];
	register i, j;

	for (i = 0; i < 32; i++) {
		collisions[i] = 0;
	}

	for (i = 0; i < TABLE_SIZE; i++) {
/*
		printf ("HASH[%4d]  ", i);
*/

		if (group_hash[i] == -1) {
/*
			printf ("EMPTY\n");
*/
			empty++;
		} else {
			number = 1;
			for (j=group_hash[i]; active[j].next >= 0; j=active[j].next) {
				number++;
			}
			if (number > 31) {
				printf ("MEGA HASH COLLISION > 31 HASH[%d]=[%d]!!!\n", i, number);
			} else {
				collisions[number]++;
			}
		}
	}

	printf ("HashTable Active=[%d] Size=[%d] Filled=[%d] Empty=[%d]\n",
		num_active, TABLE_SIZE, TABLE_SIZE-empty, empty);
	printf ("01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32\n");
	printf ("-----------------------------------------------------------------------------------------------\n");
	for (i = 0; i < 32; i++) {
		if (i) {
			printf ("%2d ", collisions[i]);
		}
	}
	printf ("\n");
}
#endif

#ifdef DEBUG
static void
debug_print_group_hash (void)
{
	int i;

	for (i = 0; i < TABLE_SIZE; i++) {
		printf ("group_hash[%4d]=[%4d]\n", i, group_hash[i]);
	}
}
#endif
