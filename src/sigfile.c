/*
 *  Project   : tin - a Usenet reader
 *  Module    : sigfile.c
 *  Author    : M.Gleason & I.Lea
 *  Created   : 17-10-92
 *  Updated   : 08-11-94
 *  Notes     : Generate random signature for posting/mailing etc.
 *  Copyright : (c) Copyright 1989-94 by Mike Gleason & Iain Lea
 *		You may  freely  copy or  redistribute	this software,
 *		so  long as there is no profit made from its use, sale
 *		trade or  reproduction.  You may not change this copy-
 *		right notice, and it must be included in any copy made
 */

#include	"tin.h"

#define MAXLOOPS 1000

#ifndef M_AMIGA
#	define CURRENTDIR "."
#else
#	define CURRENTDIR ""
#endif

extern char note_h_ftnto[LEN];       /* X-Comment-To: (Used by FIDO) */

static char sigfile[PATH_LEN];

static FILE *open_random_sig P_((char *sigdir));
static int thrashdir P_((char *sigdir));
static char *process_tagline P_((char *tagline));
static void add_tagline P_((FILE *fp, struct t_group *group));

char *process_tagline (tagline)
	char *tagline;
{
	static char pt_buf[1024];
	char buf[256];
	char *p=pt_buf,*b;

	for (;*tagline;tagline++)
	{
		if (*tagline=='\n' || *tagline=='\r') continue; /* Skip CR and LF */
	
		b=buf;
		if (*tagline=='@') /* Macro! */
		{
			if (tagline[1]=='@')
			{
				*p='@';
				++tagline;
				continue;
			}
			if (strncasecmp (tagline+1,"TFNAME",6)==0)
			{
				if (!*note_h_ftnto) return NULL; /* No TO:, so no expansion of TFName */
				tagline+=6;

				strcpy (b,note_h_ftnto);
		
				if (strchr(b,')'))
				{
					*strchr(b,')')=0;
					if (strchr(b,'(')) b=strchr(b,'(')+1;
				}

				if (strchr(b,'@')) *strchr(b,'@')=0;
				if (strchr(b,' ')) *strchr(b,' ')=0;
				if (strchr(b,'<')) *strchr(b,'<')=0;
				if (strchr(b,'.')) *strchr(b,'.')=0;


				if (!*b) return NULL; /* Can't find a first name */

				strcpy (p, b);
				p+=strlen(p);
				continue;
			}
			if (strncasecmp (tagline+1,"TLNAME",6)==0)
			{
				if (!*note_h_ftnto) return NULL; /* No TO:, so no expansion of TLName */
				tagline+=6;
	
				strcpy (b,note_h_ftnto);

				if (strchr(b,')'))
				{
					*strchr(b,')')=0;
					if (strchr(b,'(')) b=strchr(b,'(')+1;
				}

				if (strchr(b,'@')) *strchr(b,'@')=0;
				if (strchr(b,'<')) *strchr(b,'<')=0;
				while (b[strlen(b)-1]=='.') b[strlen(b)-1]=0;
				if (strrchr(b,'.')) b=strrchr(b,'.')+1;
				while (b[strlen(b)-1]==' ') b[strlen(b)-1]=0;
				if (strrchr(b,' ')) b=strrchr(b,' ')+1;

				if (!*b) return NULL; /* Can't find a last name */

				strcpy (p, b);
				p+=strlen(p);
				continue;
			}
			if (strncasecmp (tagline+1,"TNAME",5)==0)
			{
				if (!*note_h_ftnto) return NULL; /* No TO:, so no expansion of TName */
				tagline+=5;

				strcpy (b,note_h_ftnto);

				if (strchr(b,')'))
				{
					*strchr(b,')')=0;
					if (strchr(b,'('))
					{
						b=strchr(b,'(')+1;
						strcpy (p,b);
						p+=strlen(p);
						continue;
					}
				}
		
				if (strchr(b,'@')) *strchr(b,'@')=0;
				if (strchr(b,'<')) *strchr(b,'<')=0;

				while (strchr(b,'.')) *strchr(b,'.')=' ';

				while (b[strlen(b)-1]==' ') b[strlen(b)-1]=0;

				if (!*b) return NULL; /* Can't find a nice name */

				strcpy (p, b);
				p+=strlen(p);
				continue;
			}
		}
		*p=*tagline;
		p++;
	}
	*p=0;

	return pt_buf;
}

void add_tagline (fp,group)
	FILE *fp;
	struct t_group *group;
{
	char pathfile[PATH_LEN],pathindex[PATH_LEN];
	char taglinebuf[256],*tagline;
	FILE *tagfile;
	FILE *tagindex;
	struct stat statfile,statindex;
	char rebuild=0;
	long lines;
	long position;

	if (!strfpath (group->attribute->tagline_file, pathfile, sizeof (pathfile)- (4*sizeof(char)),
	    homedir, (char *) 0, (char *) 0, group->name)) 
	{
		if (!strfpath (tagline_file, pathfile, sizeof (pathfile)-(4*sizeof(char)),
		    homedir, (char *) 0, (char *) 0, group->name)) 
		{
			return; /* I guess this means no tag lines... */
		    
		}
	}
	
	{
		long epoch;
		time (&epoch);
		srand ((unsigned int) epoch);
	}

	strcat (strcpy (pathindex,pathfile),".idx");

	if (stat(pathfile,&statfile)) return;
	if (stat(pathindex,&statindex)) rebuild=1;

	if (statfile.st_mtime > statindex.st_mtime) rebuild=1;

	tagfile=fopen (pathfile,"rt");
	if (!tagfile) return;

	if (rebuild)
	{
		tagindex=fopen (pathindex,"wb");
		if (!tagindex) { fclose (tagfile); return; }

		while (!feof(tagfile) && !ferror(tagfile) )
		{
			position=ftell(tagfile);
			
			*taglinebuf=0;
			while ( (*taglinebuf==0 || *taglinebuf=='\n') && !feof(tagfile) && !ferror(tagfile) )
				fgets (taglinebuf, 250, tagfile);

			if (!feof(tagfile) && !ferror(tagfile) )
				fwrite (&position,sizeof(position),1,tagindex);
		}
		fclose (tagindex);
	}

	if (stat(pathindex,&statindex)) return;
	lines=statindex.st_size/sizeof(position);

	tagindex=fopen (pathindex,"rb");
	if (!tagindex) { fclose (tagfile); return; }

	while (-1)
	{
		fseek (tagindex, sizeof(position)*((int) ((double)(lines)*rand()/(RAND_MAX+1.0))), SEEK_SET);

		fread (&position,sizeof(position),1,tagindex);

		fseek (tagfile, position, SEEK_SET);
	
		fgets (taglinebuf, 256, tagfile);

		tagline=process_tagline(taglinebuf);
		if (tagline) break;
	}

	fprintf (fp,"\n... %s",tagline);

	fclose (tagfile);
	fclose (tagindex);
}


void
msg_write_signature (fp, flag)
	FILE *fp;
	int flag;
{
	char path[PATH_LEN];
	char cwd[PATH_LEN];
	FILE *fixfp;
	FILE *sigfp;
	int i;

#ifdef NNTP_INEWS
	if (read_news_via_nntp && use_builtin_inews) {
		flag = TRUE;
	}
#endif

	i = my_group[cur_groupnum];

	if (!strcmp(active[i].attribute->sigfile, "---none")) {
		add_tagline(fp,&active[i]);
		return;
	}
	if (active[i].attribute->sigfile[0] == '!') {
		char cmd[PATH_LEN];
		FILE *pipe_fp;
		fprintf (fp, "\n%s", sigdashes ? "-- \n" : "\n");
		if ((pipe_fp = popen (active[i].attribute->sigfile+1, "r")) != (FILE *) 0) {
			while (fgets (cmd, PATH_LEN, pipe_fp))
				fputs (cmd, fp);
			fclose (pipe_fp);
		}
		add_tagline(fp,&active[i]);
		return;
	}
	get_cwd (cwd);

	if (!strfpath (active[i].attribute->sigfile, path, sizeof (path),
	    homedir, (char *) 0, (char *) 0, active[i].name)) {
		if (!strfpath (default_sigfile, path, sizeof (path),
		    homedir, (char *) 0, (char *) 0, active[i].name)) {
			joinpath (path, homedir, ".Sig");
		}
	}

	/*
	 *  Check to see if sigfile is a directory & if it is  generate a
	 *  random signature from sigs in sigdir. If the file ~/.sigfixed
	 *  exists (fixed part of random sig) then  read it  in first and
	 *  append the random sig part onto the end.
	 */
	if ((sigfp = open_random_sig (path)) != (FILE *) 0) {
		if (debug == 2) {
			error_message ("USING random sig=[%s]", sigfile);
		}
		fprintf (fp, "\n%s", sigdashes ? "-- \n" : "\n");
		joinpath (path, homedir, ".sigfixed");
		if ((fixfp = fopen (path, "r")) != (FILE *) 0) {
			copy_fp (fixfp, fp, "");
			fclose (fixfp);
		}
		copy_fp (sigfp, fp, "");
		fclose (sigfp);
		my_chdir (cwd);
		add_tagline(fp,&active[i]);
		return;
	}

	/*
	 *  Use ~/.signature or ~/.Sig or custom .Sig files
	 */
	if ((sigfp = fopen (default_signature, "r")) != (FILE *) 0) {
		if (flag) {
			fprintf (fp, "\n%s", sigdashes ? "-- \n" : "\n");
			copy_fp (sigfp, fp, "");
		}
		fclose (sigfp);
		add_tagline(fp,&active[i]);
		return;
	}

	if ((sigfp = fopen (path, "r")) != (FILE *) 0) {
		fprintf (fp, "\n%s", sigdashes ? "-- \n" : "\n");
		copy_fp (sigfp, fp, "");
		fclose (sigfp);
		add_tagline(fp,&active[i]);
	}
}


static FILE *
open_random_sig (sigdir)
	char *sigdir;
{
	time_t epoch;
	struct stat st;

	if (stat (sigdir, &st) != -1) {
		if (S_ISDIR(st.st_mode)) {
			time (&epoch);
			srand ((unsigned int) epoch);
			my_chdir (sigdir);

			if (thrashdir (sigdir) || !sigfile[0]) {
				if (debug == 2) {
					error_message ("NO sigfile=[%s]", sigfile);
				}
				return (FILE *) 0;
			} else {
				if (debug == 2) {
					error_message ("sigfile=[%s]", sigfile);
				}
				return fopen (sigfile, "r");
			}
		}
	}

	return (FILE *) 0;
}


static int
thrashdir (sigdir)
	char *sigdir;
{
	char *cwd;
	int safeguard, recurse;
	register DIR *dirp;
	register DIR_BUF *dp;
	register int c, numentries, pick;
	struct stat st;

	sigfile[0] = '\0';

	if ((dirp = opendir (CURRENTDIR)) == NULL) {
		return (1);
	}

	numentries = 0;
	while ((dp = readdir (dirp)) != NULL) {
		numentries++;
	}

	/*
	 * consider "." and ".." non-entries
	 */
	cwd = (char *) my_malloc (PATH_LEN + 1);
#ifndef M_AMIGA
	if (numentries < 3 || cwd == (char *) 0) {
#else
	if (numentries == 0 || cwd == (char *) 0) {
#endif
		closedir (dirp);
		return (-1);
	}

	get_cwd (cwd);
	recurse = strcmp (cwd, sigdir);

	/* If we are using the root sig directory, we don't want
	 * to recurse, or else we might use a custom sig intended
	 * for a specific newsgroup (and not this one).
	 */
	for (safeguard=0, dp=NULL; safeguard<MAXLOOPS && dp==NULL; safeguard++) {
if (debug == 2) {
	sprintf (msg, "sig loop=[%d] recurse=[%d]", safeguard, recurse);
	error_message (msg, "");
}
#ifdef HAVE_REWINDDIR
		rewinddir (dirp);
#else
		closedir (dirp);
		if ((dirp = opendir (CURRENTDIR)) == NULL) {
			return (1);
		}
#endif
		pick = rand () % numentries + 1;
		while (--pick >= 0) {
			if ((dp = readdir (dirp)) == NULL) {
				break;
			}
		}
		if (dp != NULL) {	/* if we could open the dir entry */
			if (!strcmp (dp->d_name, CURRENTDIR) ||
			    !strcmp (dp->d_name, "..")) {
				dp = NULL;
			} else {	/* if we have a non-dot entry */
				if (stat (dp->d_name, &st) == -1) {
gak:
					closedir (dirp);
					return (1);
				}
				if (S_ISDIR(st.st_mode)) {
					if (recurse) {
						/*
						 * do subdirectories
						 */
						if (my_chdir (dp->d_name) < 0) {
							goto gak;
						}
						if ((c = thrashdir (sigdir)) == 1) {
							goto gak;
						} else if (c == -1) {
							/*
							 * the one we picked was an
							 * empty dir so try again.
							 */
							dp = NULL;
							my_chdir (cwd);
						}
					} else {
						dp = NULL;
					}
				} else {	/* end dir; we have a file */
					get_cwd (sigfile);
#ifdef WIN32
					strcat (sigfile, "\\");
#else
					strcat (sigfile, "/");
#endif
					strcat (sigfile, dp->d_name);
if (debug == 2) {
	error_message ("Found a file=[%s]", sigfile);
}
				}
			}
		}
	}
	free (cwd);

if (debug == 2) {
	error_message ("return 0: sigfile=[%s]", sigfile);
}
	closedir (dirp);

	return (0);
}
