/*
 *  Project   : tin - a Usenet reader
 *  Module    : msmail.c
 *  Author    : N.Ellis
 *  Created   : 01-10-94
 *  Updated   : 17-10-94
 *  Notes     : WinNT specific
 *  Copyright : (c) Copyright 1991-94 by Nigel Ellis & Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"

#if defined(WIN32)

#include <windows.h>
#include <stdio.h>
#include "mapi.h"
#include "msmail.h"

static HINSTANCE hLibrary=NULL;	/* handle for MAPI library. */
static HINSTANCE hMAPISession=NULL;

static MapiMessage mmMapiMessage;

static ULONG nMessageSize = 0x000004000;
static lpMapiMessage FAR *lppMessage;
static lpMapiMessage lpMessage;
static char szSeedMessageID[512];
static char szMessageID[512];
static char szSubject[512];
static char szNoteText[4096];

static LPSTR lpszSeedMessageID = &szSeedMessageID[0];
static LPSTR lpszMessageID = &szMessageID[0];

static LPSTR pszSubject = &szSubject[0];
static LPSTR pszNoteText = &szNoteText[0];
static MapiRecipDesc rdOriginator = { 0L,MAPI_ORIG,(LPSTR)"ONE",NULL};

#define SZ_MAPILOGON "MAPILogon"
#define SZ_MAPILOGOFF "MAPILogoff"
#define SZ_MAPISENDMAIL "MAPISendMail"
#define SZ_MAPISENDDOC "MAPISendDocuments"
#define SZ_MAPIFINDNEXT "MAPIFindNext"
#define SZ_MAPIREADMAIL "MAPIReadMail"
#define SZ_MAPISAVEMAIL "MAPISaveMail"
#define SZ_MAPIDELMAIL "MAPIDeleteMail"
#define SZ_MAPIFREEBUFFER "MAPIFreeBuffer"
#define SZ_MAPIADDRESS "MAPIAddress"
#define SZ_MAPIDETAILS "MAPIDetails"
#define SZ_MAPIRESOLVENAME "MAPIResolveName"
#define MAPIDLL "MAPI32.DLL"

#define ERR_LOAD_LIB  0x02
#define ERR_LOAD_FUNC 0x04

ULONG (FAR PASCAL *lpfnMAPILogon)(ULONG, LPSTR, LPSTR, FLAGS, ULONG, HINSTANCE *);

ULONG (FAR PASCAL *lpfnMAPILogoff)(HINSTANCE, ULONG, FLAGS,ULONG);

ULONG (FAR PASCAL *lpfnMAPISendMail)(HINSTANCE, ULONG, lpMapiMessage, FLAGS,
                                     ULONG);

ULONG (FAR PASCAL *lpfnMAPISendDocuments)(ULONG, LPSTR, LPSTR, LPSTR, ULONG);

ULONG (FAR PASCAL *lpfnMAPIFindNext)(HINSTANCE, ULONG, LPSTR, LPSTR, FLAGS,
                                     ULONG, LPSTR);

ULONG (FAR PASCAL *lpfnMAPIReadMail)(HINSTANCE, ULONG, LPSTR, FLAGS, ULONG,
                                     lpMapiMessage FAR *);

ULONG (FAR PASCAL *lpfnMAPISaveMail)(HINSTANCE, ULONG, lpMapiMessage, FLAGS,
                                     ULONG, LPSTR);

ULONG (FAR PASCAL *lpfnMAPIDeleteMail)(HINSTANCE, ULONG, LPSTR, FLAGS, ULONG);

ULONG (FAR PASCAL *lpfnMAPIFreeBuffer)(LPVOID);

ULONG (FAR PASCAL *lpfnMAPIAddress)(HINSTANCE, ULONG, LPSTR, ULONG, LPSTR,
					                ULONG, lpMapiRecipDesc, FLAGS, ULONG,
                                    LPULONG, lpMapiRecipDesc FAR *);

ULONG (FAR PASCAL *lpfnMAPIDetails)(HINSTANCE, ULONG,lpMapiRecipDesc, FLAGS,
                                    ULONG);

ULONG (FAR PASCAL *lpfnMAPIResolveName)(HINSTANCE, ULONG, LPSTR, FLAGS,
						                ULONG, lpMapiRecipDesc FAR *);

static int FAR PASCAL DeInitMAPI();

static int FAR PASCAL InitMAPI()
{
  FLAGS flFlag;
  int err=0;

  if (hLibrary != NULL) // Already init..
  	return err;

  if ((hLibrary = LoadLibrary(MAPIDLL)) == NULL)
    return(ERR_LOAD_LIB);

#pragma warning (disable : 4113)

  if ((lpfnMAPILogon = GetProcAddress(hLibrary,SZ_MAPILOGON)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPILogoff= GetProcAddress(hLibrary,SZ_MAPILOGOFF)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPISendMail= GetProcAddress(hLibrary,SZ_MAPISENDMAIL)) == NULL)
    return(ERR_LOAD_FUNC);

#ifdef FULLMAPI // not needed here
  if ((lpfnMAPISendDocuments= GetProcAddress(hLibrary,SZ_MAPISENDDOC)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIFindNext= GetProcAddress(hLibrary,SZ_MAPIFINDNEXT)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIReadMail= GetProcAddress(hLibrary,SZ_MAPIREADMAIL)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPISaveMail= GetProcAddress(hLibrary,SZ_MAPISAVEMAIL)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIDeleteMail= GetProcAddress(hLibrary,SZ_MAPIDELMAIL)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIFreeBuffer= GetProcAddress(hLibrary,SZ_MAPIFREEBUFFER)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIAddress= GetProcAddress(hLibrary,SZ_MAPIADDRESS)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIDetails= GetProcAddress(hLibrary,SZ_MAPIDETAILS)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIResolveName= GetProcAddress(hLibrary,SZ_MAPIRESOLVENAME)) == NULL)
    return(ERR_LOAD_FUNC);
#endif // not needed here

#pragma warning (default : 4113)
  
  flFlag =MAPI_NEW_SESSION | MAPI_LOGON_UI;
  if ((err=(*lpfnMAPILogon)(0L, NULL, NULL, flFlag, 0L, &hMAPISession))!=0)
	DeInitMAPI();

  return(err);
}

void endmailsession(void)
	{
	DeInitMAPI();
	}

static int FAR PASCAL DeInitMAPI()
{

  lpfnMAPILogon = NULL;
  lpfnMAPILogoff= NULL;
  lpfnMAPISendMail= NULL;
  lpfnMAPISendDocuments= NULL;
  lpfnMAPIFindNext= NULL;
  lpfnMAPIReadMail= NULL;
  lpfnMAPISaveMail= NULL;
  lpfnMAPIDeleteMail= NULL;
  lpfnMAPIFreeBuffer = NULL;
  lpfnMAPIAddress= NULL;
  lpfnMAPIDetails = NULL;
  lpfnMAPIResolveName = NULL;

  if (hLibrary)
  	FreeLibrary(hLibrary);

  return(0);
}

static int sendMail(char *To, char *Subject, char *Text, BOOL editBeforeSend) 
{
    MapiRecipDesc MAPIRecip;
    SYSTEMTIME st;
	char localTime[40];
	FLAGS flFlag;

	GetLocalTime(&st);
	sprintf(localTime, "%.2d/%.2d/%d %.2d:%.2d", 
					st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute); 

	MAPIRecip.ulReserved = 0L;
    MAPIRecip.ulRecipClass = MAPI_TO;
	MAPIRecip.lpszName = To;
	MAPIRecip.lpszAddress = "";
	MAPIRecip.ulEIDSize = 0L;
	MAPIRecip.lpEntryID = (LPVOID) NULL;

	mmMapiMessage.ulReserved = 0L;
	mmMapiMessage.lpszSubject = Subject;
	mmMapiMessage.lpszNoteText = Text;
	mmMapiMessage.lpszMessageType = NULL;
	mmMapiMessage.lpszDateReceived = localTime;
	mmMapiMessage.flFlags = MAPI_UNREAD;
	mmMapiMessage.lpOriginator = &rdOriginator;
	mmMapiMessage.nRecipCount = 1L;
	mmMapiMessage.lpRecips = &MAPIRecip;
	mmMapiMessage.nFileCount = 0L;
	mmMapiMessage.lpFiles = NULL;

    flFlag = MAPI_DIALOG;
    return (*lpfnMAPISendMail)(hMAPISession, 0L, &mmMapiMessage, 
    		editBeforeSend ? MAPI_DIALOG : 0L, 0L);
}

int sendmail(LPSTR szFileName, LPSTR szTo, LPSTR szSubject, BOOL fEdit)
{
	int err = 1;
	HANDLE hFile = NULL;
	LPSTR pText = NULL;
	DWORD dwSize = 0, dwRead = 0;

	hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE )
		goto exit;

	if ((dwSize = GetFileSize(hFile, NULL)) == 0)
		goto exit;

	if ((pText=(LPSTR) GlobalAlloc(GPTR, dwSize)) == NULL)
		goto exit;

	if (! ReadFile(hFile, pText, dwSize, &dwRead, NULL))
		goto exit;

	if (dwSize != dwRead)
		goto exit;

	if (InitMAPI() != 0)
		goto exit;

	err = sendMail(szTo, szSubject, pText, fEdit);

	// BUGBUG: DeInitMAPI() at end of session
	// DeInitMAPI();

exit:
	if (hFile)	CloseHandle(hFile);
	if (pText)	GlobalFree((HGLOBAL) pText);
	return err;
}

#endif	/* WIN32 */
