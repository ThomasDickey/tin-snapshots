/*
 *  Project   : tin - a Usenet reader
 *  Module    : actived.c
 *  Author    : I.Lea
 *  Created   : 11-11-93
 *  Updated   : 25-07-94
 *  Notes     : Creates & updates mail & save active files
 *  Functions :
 *    main (iNumArgs, pacArgs)
 *    vInitVariables ()
 *    vReadCmdLineOptions (iNumArgs, pacArgs)
 *    vPrintUsage (pcProgName)
 *    vParseGrpLine (pcLine, pcGrpName, plArtMax, plArtMin, pcGrpDir)
 *    vMakeGrpPath (pcBaseDir, pcGrpName, pcGrpPath)
 *    vFindArtMaxMin (pcGrpPath, plArtMax, plArtMin)
 *    vPrintGrpLine (tFp, pcGrpName, lArtMax, lArtMin, pcBaseDir)
 *    vMakeGrpList (pcActiveFile, pcBaseDir, pcGrpPath)
 *    vAppendGrpLine (pcActiveFile, pcGrpPath, lArtMax, lArtMin, pcBaseDir)
 *    vUpdateActiveFile (pcActiveFile, pcDir)
 *
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *		You may  freely  copy or  redistribute	this software,
 *		so  long as there is no profit made from its use, sale
 *		trade or  reproduction.  You may not change this copy-
 *		right notice, and it must be included in any copy made
 */

#include	"tin.h"

#ifndef S_ISDIR
#define S_ISDIR(m)  ((m & S_IFMT) == S_IFDIR)
#endif

char	acHomeDir[PATH_LEN];
char	acMailActiveFile[PATH_LEN];
char	acSaveActiveFile[PATH_LEN];
char	acTempActiveFile[PATH_LEN];
char	acMailDir[PATH_LEN];
char	acSaveDir[PATH_LEN];
int		iAllGrps;
int		iRecursive;
int		iVerbose;

#ifndef M_AMIGA
struct	passwd *psPwd;
struct	passwd sPwd;
#endif

#ifdef ACTIVE_DAEMON

void
main (iNumArgs, pacArgs)
	int	iNumArgs;
	char	*pacArgs[];
{
	char	acGrpPath[PATH_LEN];
	
	vInitVariables ();
	
	vReadCmdLineOptions (iNumArgs, pacArgs);

	if (iRecursive) {
		vPrintActiveHead (acMailActiveFile);
		strcpy (acGrpPath, acMailDir);
		vMakeGrpList (acMailActiveFile, acMailDir, acGrpPath);

		vPrintActiveHead (acSaveActiveFile);
		strcpy (acGrpPath, acSaveDir);
		vMakeGrpList (acSaveActiveFile, acSaveDir, acGrpPath);

		exit (1);
	}
		
	/*
	 * Open mail & save active files and update group entries
	 */
	vUpdateActiveFile (acMailActiveFile, acMailDir);
	vUpdateActiveFile (acSaveActiveFile, acSaveDir);	 
}

#endif	/* ACTIVE_DAEMON */


void
create_save_active_file ()
{
	char	acGrpPath[PATH_LEN];

	printf ("Creating active file for saved groups...\n");
		
	vInitVariables ();

	iRecursive = TRUE;
	
	vPrintActiveHead (acSaveActiveFile);
	strcpy (acGrpPath, acSaveDir);
	vMakeGrpList (acSaveActiveFile, acSaveDir, acGrpPath);
}


void
vInitVariables ()
{
	char	*pcPtr;
	
#ifndef M_AMIGA
	psPwd = (struct passwd *) 0;
	if (((pcPtr = (char *) getlogin ()) != (char *) 0) && strlen (pcPtr)) {
		psPwd = getpwnam (pcPtr);
	}
	if (psPwd == (struct passwd *) 0) {	
		psPwd = getpwuid (getuid ());
	}
	if (psPwd != (struct passwd *) 0) {	
		memcpy ((char *) &sPwd, (char *) psPwd, sizeof (struct passwd));
		psPwd = &sPwd;
	}
#endif

	if ((pcPtr = (char *) getenv ("TIN_HOMEDIR")) != (char *) 0) {
		strcpy (acHomeDir, pcPtr);
	} else if ((pcPtr = (char *) getenv ("HOME")) != (char *) 0) {
		strcpy (acHomeDir, pcPtr);
#ifndef M_AMIGA
	} else if (psPwd != (struct passwd *) 0) {
		strcpy (acHomeDir, psPwd->pw_dir);
	} else {
		strcpy (acHomeDir, "/tmp");
	}
#else
	} else {
		strcpy (acHomeDir, "T:");
	}
#endif
	
#ifdef WIN32
#define DOTTINDIR "tin"
#else
#define DOTTINDIR ".tin"
#endif
	sprintf (acTempActiveFile, "%s/%s/%ld.tmp", acHomeDir, DOTTINDIR, (long) getpid ());
	sprintf (acMailActiveFile, "%s/%s/%s", acHomeDir, DOTTINDIR, ACTIVE_MAIL_FILE);
	sprintf (acSaveActiveFile, "%s/%s/%s", acHomeDir, DOTTINDIR, ACTIVE_SAVE_FILE);
	sprintf (acMailDir, "%s/Mail", acHomeDir);
	sprintf (acSaveDir, "%s/News", acHomeDir);
	iAllGrps = FALSE;
	iRecursive = FALSE;
	iVerbose = FALSE;
}

void
vReadCmdLineOptions (iNumArgs, pacArgs)
	int	iNumArgs;
	char	*pacArgs[];
{
	int iOption;

	while ((iOption = getopt (iNumArgs, pacArgs, "ahM:m:rS:s:v")) != EOF) {
		switch (iOption) {
			case 'a':	/* all groups including empty ones */
				iAllGrps = TRUE;
				break;

			case 'm':	/* mail directory */
				strncpy (acMailDir, optarg, sizeof (acMailDir));
				break;

			case 'M':	/* mail active file */
				strncpy (acMailActiveFile, optarg, sizeof (acMailActiveFile));
				break;

			case 'r':	/* recursively descend maildir */
				iRecursive = TRUE;
				break;

			case 's':	/* save directory */
				strncpy (acSaveDir, optarg, sizeof (acSaveDir));
				break;

			case 'S':	/* save active file */
				strncpy (acSaveActiveFile, optarg, sizeof (acSaveActiveFile));
				break;

			case 'v':	/* verbose mode */
				iVerbose = TRUE;
				break;

			case 'h':
			case '?':
			default:
				vPrintUsage (pacArgs[0]);
				exit (1);
		}
	}
}	

void
vUpdateActiveFile (pcActiveFile, pcDir)
	char	*pcActiveFile;
	char	*pcDir;
{
	char	acLine[NEWSRC_LINE];
	char	acGrpName[PATH_LEN];
	char	acGrpPath[PATH_LEN];
	char	acGrpDir[PATH_LEN];
	FILE	*hFpIp;
	FILE	*hFpOp;
	long	lArtMin;
	long	lArtMax;

	hFpIp = fopen (pcActiveFile, "r");
	if (hFpIp != (FILE *) 0) {
		hFpOp = fopen (acTempActiveFile, "w");
		if (hFpOp != (FILE *) 0) {
			while (fgets (acLine, sizeof (acLine), hFpIp) != (char *) 0) {
				vParseGrpLine (acLine, acGrpName, &lArtMax, &lArtMin, acGrpDir);
				if (*acLine == '#') {
					my_fputs (acLine, hFpOp);
				} else {
					vMakeGrpPath (pcDir, acGrpName, acGrpPath);
					
					vFindArtMaxMin (acGrpPath, &lArtMax, &lArtMin);
				
					vPrintGrpLine (hFpOp, acGrpName, lArtMax, lArtMin, acGrpDir);
				}
			}
			fclose (hFpOp);
		}
		fclose (hFpIp);
	}
}

void
vPrintUsage (pcProgName)
	char	*pcProgName;
{
	printf ("Mail & Save active file update daemon. (c) Copyright 1991-94 Iain Lea.\n\n");
	printf ("Usage: %s [options]\n", pcProgName);
	printf ("  -a       all groups including empty ones to be included\n");
	printf ("  -h       help\n");
	printf ("  -M file  mailgroups file [default=%s]\n", acMailActiveFile);
	printf ("  -m dir   mail directory [default=%s]\n", acMailDir);
	printf ("  -r       recursively build mailgroups file\n");
	printf ("  -S file  savegroups file [default=%s]\n", acSaveActiveFile);
	printf ("  -s dir   save directory [default=%s]\n", acSaveDir);
	printf ("  -v       verbose output\n");
	printf ("\nMail bug reports/comments to %s\n", BUG_REPORT_ADDRESS);
}

void
vMakeGrpList (pcActiveFile, pcBaseDir, pcGrpPath)
	char	*pcActiveFile;
	char	*pcBaseDir;
	char	*pcGrpPath;
{
	char	*pcPtr;
	char	acFile[PATH_LEN];
	char	acPath[PATH_LEN];
	DIR		*tDirFile;
	DIR_BUF	*tFile;
	int		iIsDir;
	long	lArtMax;
	long	lArtMin;
	struct	stat sStatInfo;

if (iVerbose) {
printf ("BEG Base=[%s] path=[%s]\n", pcBaseDir, pcGrpPath);
}
	if (access (pcGrpPath, R_OK) != 0) {
		return;
	}

	tDirFile = opendir (pcGrpPath);
/*
printf ("opendir(%s)\n", pcGrpPath);
*/
	if (tDirFile != (DIR *) 0) {
		iIsDir = FALSE;
		while ((tFile = readdir (tDirFile)) != (DIR_BUF *) 0) {
			strncpy (acFile, tFile->d_name, (size_t) tFile->D_LENGTH);
			acFile[tFile->D_LENGTH] = '\0';
			sprintf (acPath, "%s/%s", pcGrpPath, acFile);
/*
printf ("STAT=[%s]\n", acPath);
*/
			if (!(acFile[0] == '.' && acFile[1] == '\0') &&
				!(acFile[0] == '.' && acFile[1] == '.' && acFile[2] == '\0')) {
				if (stat (acPath, &sStatInfo) != -1) {
					if (S_ISDIR(sStatInfo.st_mode)) {
						iIsDir = TRUE;
					}
				}
			}
			if (iIsDir) {
				iIsDir = FALSE;
				strcpy (pcGrpPath, acPath);
				if (iVerbose) {
					printf ("Base=[%s] path=[%s]\n", pcBaseDir, pcGrpPath);
				}

				vMakeGrpList (pcActiveFile, pcBaseDir, pcGrpPath);
				vFindArtMaxMin (pcGrpPath, &lArtMax, &lArtMin);
				vAppendGrpLine (pcActiveFile, pcGrpPath, lArtMax, lArtMin, pcBaseDir);

				pcPtr = strrchr (pcGrpPath, '/');
				if (pcPtr != (char *) 0) {
					*pcPtr = '\0';
				}
			}
		}
		closedir (tDirFile);
	}
}

void
vAppendGrpLine (pcActiveFile, pcGrpPath, lArtMax, lArtMin, pcBaseDir)
	char	*pcActiveFile;
	char	*pcGrpPath;
	long	lArtMax;
	long	lArtMin;
	char	*pcBaseDir;
{
	char	acGrpName[PATH_LEN];
	FILE	*hFp;

	if (! iAllGrps && (lArtMax == 0 && lArtMin == 1)) {
		return;
	}
	
	if ((hFp = fopen (pcActiveFile, "a+")) != (FILE *) 0) {
		vMakeGrpName (pcBaseDir, acGrpName, pcGrpPath);
		printf ("Appending=[%s %ld %ld %s]\n", acGrpName, lArtMax, lArtMin, pcBaseDir);
		vPrintGrpLine (hFp, acGrpName, lArtMax, lArtMin, pcBaseDir);
		fclose (hFp);
	}
}

