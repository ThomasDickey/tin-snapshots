/*
 *  Project   : tin - a Usenet reader
 *  Module    : menukey.h
 *  Author    : S.Robbins
 *  Created   : 15-08-94
 *  Updated   : 21-12-94
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Steve Robbins & Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

/* active.c */

#define iKeyActiveNone 'N'
#define iKeyActiveAll 'Y'
#define iKeyActiveNo 'n'
#define iKeyActiveYes 'y'

/* feed.c */

#define iKeyFeedTag 'T'
#define iKeyFeedArt 'a'
#define iKeyFeedHot 'h'
#define iKeyFeedPat 'p'
#define iKeyFeedThd 't'

/* filter.c */

#define iKeyFilterQuit ESC
#define iKeyFilterEdit 'e'
#define iKeyFilterQuit2 'q'
#define iKeyFilterSave 's'

/* group.c */

#define iKeyGroupAutosel ctrl('A')
#define iKeyGroupPageUp ctrl('B')
#define iKeyGroupPageDown ctrl('D')
#define iKeyGroupPageDown2 ctrl('F')
#define iKeyGroupNextUnreadArtOrGrp '\t'
#define iKeyGroupReadBasenote '\n'
#define iKeyGroupKill ctrl('K')
#define iKeyGroupRedrawScr ctrl('L')
#define iKeyGroupReadBasenote2 '\r'
#define iKeyGroupDown ctrl('N')
#define iKeyGroupUp ctrl('P')
/* itchy trigger finger on next line */
#define iKeyGroupRedrawScr2 ctrl('R')
#define iKeyGroupPageUp2 ctrl('U')
/* itchy trigger finger on next line */
#define iKeyGroupRedrawScr3 ctrl('W')
#define iKeyGroupPageDown3 ' '
#define iKeyGroupShell '!'
#define iKeyGroupSetRange '#'
#define iKeyGroupLastPage '$'
#define iKeyGroupToggleColor '&'
#define iKeyGroupSelThd '*'
#define iKeyGroupDoAutoSel '+'
#define iKeyGroupLastViewed '-'
#define iKeyGroupToggleThdSel '.'
#define iKeyGroupFSearchSubj '/'
/* digits 0-9 in use */
#define iKeyGroupSelThdIfUnreadSelected ';'
#define iKeyGroupSelPattern '='
#define iKeyGroupBSearchSubj '?'
#define iKeyGroupReverseSel '@'
#define iKeyGroupBSearchAuth 'A'
#define iKeyGroupFSearchBody 'B'
#define iKeyGroupCatchupGotoNext 'C'
#define iKeyGroupToggleHelpDisplay 'H'
#define iKeyGroupToggleInverseVideo 'I'
#define iKeyGroupMarkThdRead 'K'
#define iKeyGroupOptionMenu 'M'
#define iKeyGroupNextUnreadArt 'N'
#define iKeyGroupPrevUnreadArt 'P'
#define iKeyGroupQuitTin 'Q'
#define iKeyGroupBugReport 'R'
#define iKeyGroupSaveTagged 'S'
#define iKeyGroupUntag 'U'
#define iKeyGroupDisplayPostHist 'W'
#define iKeyGroupMarkUnselArtRead 'X'
#define iKeyGroupMarkThdUnread 'Z'
#define iKeyGroupQuickAutosel '['
#define iKeyGroupQuickKill ']'
#define iKeyGroupFirstPage '^'
#define iKeyGroupFSearchAuth 'a'
#define iKeyGroupPageUp3 'b'
#define iKeyGroupCatchup 'c'
#define iKeyGroupToggleSubjDisplay 'd'
#define iKeyGroupGoto 'g'
#define iKeyGroupHelp 'h'
#define iKeyGroupDisplaySubject 'i'
#define iKeyGroupDown2 'j'
#define iKeyGroupUp2 'k'
#define iKeyGroupListThd 'l'
#define iKeyGroupMail 'm'
#define iKeyGroupNextGroup 'n'
#define iKeyGroupPrint 'o'
#define iKeyGroupPrevGroup 'p'
#define iKeyGroupQuit 'q'
#define iKeyGroupToggleReadDisplay 'r'
#define iKeyGroupSave 's'
#define iKeyGroupTag 't'
#define iKeyGroupToggleThreading 'u'
#define iKeyGroupVersion 'v'
#define iKeyGroupPost 'w'
#define iKeyGroupRepost 'x'
#define iKeyGroupMarkArtUnread 'z'
#define iKeyGroupPipe '|'
#define iKeyGroupUndoSel '~'

/* help.c */

#define iKeyHelpPageUp ctrl('B')
#define iKeyHelpPageDown ctrl('D')
#define iKeyHelpPageDown2 ctrl('F')
#define iKeyHelpPageUp2 ctrl('U')
/* itchy trigger finger on next line */
#define iKeyHelpHome ctrl('R')
#define iKeyHelpEnd '$'
#define iKeyHelpPageDown3 ' '
#define iKeyHelpEnd2 'G'
#define iKeyHelpHome2 '^'
#define iKeyHelpPageUp3 'b'
#define iKeyHelpHome3 'g'
#define iKeyHelpPageDown4 'j'
#define iKeyHelpPageUp4 'k'
/* all other keys exit help */

/* page.c */

#define iKeyPageAutoSel ctrl('A')
#define iKeyPagePageUp ctrl('B')
#define iKeyPagePageDown ctrl('D')
#define iKeyPagePageDown2 ctrl('F')
#define iKeyPagePGPCheckArticle ctrl('G')
#define iKeyPageDisplayHeaders ctrl('H')
#define iKeyPageNextUnread '\t'
#define iKeyPageNextThd '\n'
#define iKeyPageAutoKill ctrl('K')
#define iKeyPageRedrawScr ctrl('L')
#define iKeyPageNextThd2 '\r'
/* itchy trigger finger on next line */
#define iKeyPageFirstPage ctrl('R')
#define iKeyPageToggleTabs ctrl('T')
#define iKeyPagePageUp2 ctrl('U')
#define iKeyPageToggleRot ctrl('X')
#define iKeyPagePageDown3 ' '
#define iKeyPageShell '!'
#define iKeyPageToggleTex2iso '\"'
#define iKeyPageLastPage '$'
#define iKeyPageToggleRot2 '%'
#define iKeyPageToggleColor '&'
#define iKeyPageLastViewed '-'
#define iKeyPageFSearchSubj '/'
/* digits 0-9 in use */
#define iKeyPageSkipIncludedText ':'
#define iKeyPageTopThd '<'
#define iKeyPageBotThd '>'
#define iKeyPageBsearchAuth 'A'
#define iKeyPageBSearchBody 'B'
#define iKeyPageCatchupGotoNext 'C'
#define iKeyPageDelete 'D'
#define iKeyPageFollowup 'F'
#define iKeyPageLastPage2 'G'
#define iKeyPageToggleHelpDisplay 'H'
#define iKeyPageToggleInverseVideo 'I'
#define iKeyPageKillThd 'K'
#define iKeyPageOptionMenu 'M'
#define iKeyPageNextUnreadArt 'N'
#define iKeyPagePrevUnreadArt 'P'
#define iKeyPageQuitTin 'Q'
#define iKeyPageReply 'R'
#define iKeyPageGroupSel 'T'
#define iKeyPagePostHist 'W'
#define iKeyPageFirstPage2 '^'
#define iKeyPageFsearchAuth 'a'
#define iKeyPagePageUp3 'b'
#define iKeyPageCatchup 'c'
#define iKeyPageToggleRot3 'd'
#define iKeyPageEdit 'e'
#define iKeyPageFollowupQuote 'f'
#define iKeyPageFirstPage3 'g'
#define iKeyPageHelp 'h'
#define iKeyPageDisplaySubject 'i'
#define iKeyPageKillArt 'k'
#define iKeyPageMail 'm'
#define iKeyPageNextArt 'n'
#define iKeyPagePrint 'o'
#define iKeyPagePrevArt 'p'
#define iKeyPageQuit 'q'
#define iKeyPageReplyQuote 'r'
#define iKeyPageSave 's'
#define iKeyPageTag 't'
#define iKeyPageVersion 'v'
#define iKeyPagePost 'w'
#define iKeyPageRepost 'x'
#define iKeyPageMarkArtUnread 'z'
#define iKeyPagePipe '|'

/* post.c */

#define iKeyPostQuit ESC
#define iKeyPostDelete 'd'
#define iKeyPostEdit 'e'
#define iKeyPostPGP 'g'
#define iKeyPostIspell 'i'
#define iKeyPostPost 'p'
#define iKeyPostQuit2 'q'
#define iKeyPostSend 's'

/* prompt.c */

#define iKeyPromptYes 'y'
#define iKeyPromptNo 'n'

/* save.c */

#define iKeySaveDontSaveFile ESC
#define iKeySaveAppendFile 'a'
#define iKeySaveOverwriteFile 'o'
#define iKeySaveDontSaveFile2 'q'

#define iKeyPProcExtractZip 'E'
#define iKeyPProcListZip 'L'
#define iKeyPProcExtractZoo 'e'
#define iKeyPProcListZoo 'l'
#define iKeyPProcShar 's'
#define iKeyPProcUUDecode 'u'

/* select.c */

#define iKeySelectPageUp ctrl('B')
#define iKeySelectPageDown ctrl('D')
#define iKeySelectPageDown2 ctrl('F')
#define iKeySelectEnterNextUnreadGrp '\t'
#define iKeySelectReadGrp '\n'
#define iKeySelectRedrawScr ctrl('L')
#define iKeySelectReadGrp2 '\r'
#define iKeySelectDown ctrl('N')
#define iKeySelectUp ctrl('P')
#define iKeySelectResetNewsrc ctrl('R')
#define iKeySelectPageUp2 ctrl('U')
#define iKeySelectPageDown3 ' '
#define iKeySelectShell '!'
#define iKeySelectSetRange '#'
#define iKeySelectLastPage '$'
#define iKeySelectToggleColor '&'
#define iKeySelectFSearchSubj '/'
#define iKeySelectBSearchSubj '?'
#define iKeySelectCatchupGotoNext 'C'
#define iKeySelectToggleHelpDisplay 'H'
#define iKeySelectToggleInverseVideo 'I'
#define iKeySelectOptionsMenu 'M'
#define iKeySelectNextUnreadGrp 'N'
#define iKeySelectQuit2 'Q'
#define iKeySelectBugReport 'R'
#define iKeySelectSubscribePat 'S'
#define iKeySelectUnsubscribePat 'U'
#define iKeySelectPostHist 'W'
#define iKeySelectSyncWithActive 'Y'
#define iKeySelectFirstPage '^'
#define iKeySelectPageUp3 'b'
#define iKeySelectCatchup 'c'
#define iKeySelectToggleSubjDisplay 'd'
#define iKeySelectGoto 'g'
#define iKeySelectHelp 'h'
#define iKeySelectDisplayGroupInfo 'i'
#define iKeySelectDown2 'j'
#define iKeySelectUp2 'k'
#define iKeySelectMoveGrp 'm'
#define iKeySelectEnterNextUnreadGrp2 'n'
#define iKeySelectQuit 'q'
#define iKeySelectToggleReadDisplay 'r'
#define iKeySelectSubscribe 's'
#define iKeySelectUnsubscribe 'u'
#define iKeySelectVersion 'v'
#define iKeySelectPost 'w'
#define iKeySelectYankActive 'y'
#define iKeySelectMarkGrpUnread 'z'

/* thread.c */

#define iKeyThreadPageUp ctrl('B')
#define iKeyThreadPageDown ctrl('D')
#define iKeyThreadPageDown2 ctrl('F')
#define iKeyThreadReadNextArtOrThread '\t'
#define iKeyThreadReadArt '\n'
#define iKeyThreadRedrawScr ctrl('L')
#define iKeyThreadReadArt2 '\r'
#define iKeyThreadDown ctrl('N')
#define iKeyThreadUp ctrl('P')
/* itchy trigger finger on next line */
#define iKeyThreadRedrawScr2 ctrl('R')
#define iKeyThreadPageUp2 ctrl('U')
/* itchy trigger finger on next line */
#define iKeyThreadRedrawScr3 ctrl('W')
#define iKeyThreadPageDown3 ' '
#define iKeyThreadShell '!'
#define iKeyThreadSetRange '#'
#define iKeyThreadLastPage '$'
#define iKeyThreadToggleColor '&'
#define iKeyThreadMarkArtSel '*'
#define iKeyThreadToggleArtSel '.'
/* digits 0-9 in use */
#define iKeyThreadReverseSel '@'
#define iKeyThreadCatchup 'C'
#define iKeyThreadToggleHelpDisplay 'H'
#define iKeyThreadToggleInverseVideo 'I'
#define iKeyThreadMarkArtRead 'K'
#define iKeyThreadQuitTin 'Q'
#define iKeyThreadBugReport 'R'
#define iKeyThreadMarkThdUnread 'Z'
#define iKeyThreadFirstPage '^'
#define iKeyThreadPageUp3 'b'
#define iKeyThreadCatchupConditional 'c'
#define iKeyThreadToggleSubjDisplay 'd'
#define iKeyThreadHelp 'h'
#define iKeyThreadDisplaySubject 'i'
#define iKeyThreadDown2 'j'
#define iKeyThreadUp2 'k'
#define iKeyThreadQuit 'q'
#define iKeyThreadTag 't'
#define iKeyThreadVersion 'v'
#define iKeyThreadMarkArtUnread 'z'
#define iKeyThreadUndoSel '~'
