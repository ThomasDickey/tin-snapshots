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

#define iKeyActiveYes 'y'
#define iKeyActiveAll 'Y'
#define iKeyActiveNone 'N'
#define iKeyActiveNo 'n'

/* feed.c */

#define iKeyFeedArt 'a'
#define iKeyFeedThd 't'
#define iKeyFeedTag 'T'
#define iKeyFeedHot 'h'
#define iKeyFeedPat 'p'

/* filter.c */

#define iKeyFilterEdit 'e'
#define iKeyFilterQuit 'q'
#define iKeyFilterQuit2 ESC
#define iKeyFilterSave 's'

/* group.c */

#define iKeyGroupShell '!'
#define iKeyGroupLastPage '$'
#define iKeyGroupLastViewed '-'
#define iKeyGroupPipe '|'
#define iKeyGroupSetRange '#'
#define iKeyGroupFSearchSubj '/'
#define iKeyGroupBSearchSubj '?'
#define iKeyGroupFSearchBody 'B'
#define iKeyGroupReadBasenote '\r'
#define iKeyGroupReadBasenote2 '\n'
#define iKeyGroupNextUnreadArtOrGrp '\t'
#define iKeyGroupPageDown ' '
#define iKeyGroupPageDown2 ctrl('D')
#define iKeyGroupPageDown3 ctrl('F')
#define iKeyGroupAutosel ctrl('A')
#define iKeyGroupKill ctrl('K')
#define iKeyGroupQuickAutosel '['
#define iKeyGroupQuickKill ']'
#define iKeyGroupRedrawScr ctrl('L')
#define iKeyGroupRedrawScr2 ctrl('R')
#define iKeyGroupRedrawScr3 ctrl('W')
#define iKeyGroupDown ctrl('N')
#define iKeyGroupDown2 'j'
#define iKeyGroupUp ctrl('P')
#define iKeyGroupUp2 'k'
#define iKeyGroupPageUp 'b'
#define iKeyGroupPageUp2 ctrl('U')
#define iKeyGroupPageUp3 ctrl('B')
#define iKeyGroupFSearchAuth 'a'
#define iKeyGroupBSearchAuth 'A'
#define iKeyGroupCatchup 'c'
#define iKeyGroupCatchupGotoNext 'C'
#define iKeyGroupToggleSubjDisplay 'd'
#define iKeyGroupGoto 'g'
#define iKeyGroupHelp 'h'
#define iKeyGroupToggleHelpDisplay 'H'
#define iKeyGroupToggleInverseVideo 'I'
#define iKeyGroupKillThd 'K'
#define iKeyGroupListThd 'l'
#define iKeyGroupMail 'm'
#define iKeyGroupOptionMenu 'M'
#define iKeyGroupNextGroup 'n'
#define iKeyGroupNextUnreadArt 'N'
#define iKeyGroupPrint 'o'
#define iKeyGroupPrevGroup 'p'
#define iKeyGroupPrevUnreadArt 'P'
#define iKeyGroupQuit 'q'
/* #define iKeyGroupQuit2 'i' */
#define iKeyGroupQuitTin 'Q'
#define iKeyGroupToggleReadDisplay 'r'
#define iKeyGroupBugReport 'R'
#define iKeyGroupSave 's'
#define iKeyGroupSaveTagged 'S'
#define iKeyGroupTag 't'
#define iKeyGroupToggleThreading 'u'
#define iKeyGroupUntag 'U'
#define iKeyGroupVersion 'v'
#define iKeyGroupPost 'w'
#define iKeyGroupDisplayPostHist 'W'
#define iKeyGroupRepost 'x'
#define iKeyGroupMarkArtUnread 'z'
#define iKeyGroupMarkThdUnread 'Z'
#define iKeyGroupSelThd '*'
#define iKeyGroupToggleThdSel '.'
#define iKeyGroupReverseSel '@'
#define iKeyGroupUndoSel '~'
#define iKeyGroupSelPattern '='
#define iKeyGroupSelThdIfUnreadSelected ';'
#define iKeyGroupMarkUnselArtRead 'X'
#define iKeyGroupDoAutoSel '+'
#define iKeyGroupDisplaySubject 'i'

/* help.c */

#define iKeyHelpPageDown ctrl('D')
#define iKeyHelpPageDown2 ctrl('F')
#define iKeyHelpPageDown3 ' '
#define iKeyHelpPageDown4 'j'
#define iKeyHelpPageUp ctrl('U')
#define iKeyHelpPageUp2 ctrl('B')
#define iKeyHelpPageUp3 'b'
#define iKeyHelpPageUp4 'k'
#define iKeyHelpHome ctrl('R')
#define iKeyHelpHome2 'g'
#define iKeyHelpEnd '$'
#define iKeyHelpEnd2 'G'

/* page.c */

#define iKeyPageShell '!'
#define iKeyPageLastPage '$'
#define iKeyPageLastPage2 'G'
#define iKeyPageLastViewed '-'
#define iKeyPagePipe '|'
#define iKeyPageFSearchSubj '/'
#define iKeyPageBSearchBody 'B'
#define iKeyPageTopThd '<'
#define iKeyPageBotThd '>'
#define iKeyPagePageDown ' '
#define iKeyPagePageDown2 ctrl('D')
#define iKeyPagePageDown3 ctrl('F')
#define iKeyPageNextThd '\r'
#define iKeyPageNextThd2 '\n'
#define iKeyPageNextUnread '\t'
#define iKeyPagePGPCheckArticle ctrl('G')
#define iKeyPageDisplayHeaders ctrl('H')
#define iKeyPageToggleTex2iso '\"'
#define iKeyPageAutoSel ctrl('A')
#define iKeyPageAutoKill ctrl('K')
#define iKeyPageRedrawScr ctrl('L')
#define iKeyPageRestartArt ctrl('R')
#define iKeyPageRestartArt2 'g'
#define iKeyPageToggleTabs ctrl('T')
#define iKeyPageToggleRot ctrl('X')
#define iKeyPageToggleRot2 '%'
#define iKeyPageToggleRot3 'd'
#define iKeyPageFsearchAuth 'a'
#define iKeyPageBsearchAuth 'A'
#define iKeyPagePageUp 'b'
#define iKeyPagePageUp2 ctrl('U')
#define iKeyPagePageUp3 ctrl('B')
#define iKeyPageCatchup 'c'
#define iKeyPageCatchupGotoNext 'C'
#define iKeyPageDelete 'D'
#define iKeyPageEdit 'e'
#define iKeyPageFollowupQuote 'f'
#define iKeyPageFollowup 'F'
#define iKeyPageHelp 'h'
#define iKeyPageToggleHelpDisplay 'H'
#define iKeyPageQuit 'q'
#define iKeyPageToggleInverseVideo 'I'
#define iKeyPageKillArt 'k'
#define iKeyPageKillThd 'K'
#define iKeyPageMail 'm'
#define iKeyPageOptionMenu 'M'
#define iKeyPageNextArt 'n'
#define iKeyPageNextUnreadArt 'N'
#define iKeyPagePrint 'o'
#define iKeyPagePrevArt 'p'
#define iKeyPagePrevUnreadArt 'P'
#define iKeyPageQuickQuit 'Q'
#define iKeyPageReplyQuote 'r'
#define iKeyPageReply 'R'
#define iKeyPageSave 's'
#define iKeyPageTag 't'
#define iKeyPageGroupSel 'T'
#define iKeyPageVersion 'v'
#define iKeyPagePost 'w'
#define iKeyPagePostHist 'W'
#define iKeyPageRepost 'x'
#define iKeyPageMarkArtUnread 'z'
#define iKeyPageSkipIncludedText ':'
#define iKeyPageDisplaySubject 'i'

/* post.c */

#define iKeyPostEdit 'e'
#define iKeyPostQuit 'q'
#define iKeyPostQuit2 ESC
#define iKeyPostIspell 'i'
#define iKeyPostPost 'p'
#define iKeyPostSend 's'
#define iKeyPostDelete 'd'
#define iKeyPostPGP 'g'

/* prompt.c */

#define iKeyPromptYes 'y'
#define iKeyPromptNo 'n'

/* save.c */

#define iKeySaveAppendFile 'a'
#define iKeySaveOverwriteFile 'o'
#define iKeySaveDontSaveFile 'q'
#define iKeySaveDontSaveFile2 ESC

#define iKeyPProcShar 's'
#define iKeyPProcUUDecode 'u'
#define iKeyPProcListZoo 'l'
#define iKeyPProcExtractZoo 'e'
#define iKeyPProcListZip 'L'
#define iKeyPProcExtractZip 'E'

/* select.c */

#define iKeySelectShell '!'
#define iKeySelectLastPage '$'
#define iKeySelectSetRange '#'
#define iKeySelectFSearchSubj '/'
#define iKeySelectBSearchSubj '?'
#define iKeySelectReadGrp '\r'
#define iKeySelectReadGrp2 '\n'
#define iKeySelectEnterNextUnreadGrp '\t'
#define iKeySelectEnterNextUnreadGrp2 'n'
#define iKeySelectPageDown ' '
#define iKeySelectPageDown2 ctrl('D')
#define iKeySelectPageDown3 ctrl('F')
#define iKeySelectRedrawScr ctrl('L')
#define iKeySelectDown ctrl('N')
#define iKeySelectDown2 'j'
#define iKeySelectUp ctrl('P')
#define iKeySelectUp2 'k'
#define iKeySelectResetNewsrc ctrl('R')
#define iKeySelectColor 'a'
#define iKeySelectPageUp 'b'
#define iKeySelectPageUp2 ctrl('U')
#define iKeySelectPageUp3 ctrl('B')
#define iKeySelectCatchup 'c'
#define iKeySelectCatchupGotoNext 'C'
#define iKeySelectToggleSubjDisplay 'd'
#define iKeySelectGoto 'g'
#define iKeySelectHelp 'h'
#define iKeySelectToggleHelpDisplay 'H'
#define iKeySelectDisplayGroupInfo 'i'
#define iKeySelectToggleInverseVideo 'I'
#define iKeySelectMoveGrp 'm'
#define iKeySelectOptionsMenu 'M'
#define iKeySelectNextUnreadGrp 'N'
#define iKeySelectQuit 'q'
#define iKeySelectQuit2 'Q'
#define iKeySelectToggleReadDisplay 'r'
#define iKeySelectBugReport 'R'
#define iKeySelectSubscribe 's'
#define iKeySelectSubscribePat 'S'
#define iKeySelectUnsubscribe 'u'
#define iKeySelectUnsubscribePat 'U'
#define iKeySelectVersion 'v'
#define iKeySelectPost 'w'
#define iKeySelectPostHist 'W'
#define iKeySelectYankActive 'y'
#define iKeySelectSyncWithActive 'Y'
#define iKeySelectMarkGrpUnread 'z'

/* thread.c */

#define iKeyThreadLastPage '$'
#define iKeyThreadSetRange '#'
#define iKeyThreadReadArt '\r'
#define iKeyThreadReadArt2 '\n'
#define iKeyThreadReadNextArtOrThread '\t'
#define iKeyThreadPageDown ' '
#define iKeyThreadPageDown2 ctrl('D')
#define iKeyThreadPageDown3 ctrl('F')
#define iKeyThreadRedrawScr ctrl('L')
#define iKeyThreadRedrawScr2 ctrl('R')
#define iKeyThreadRedrawScr3 ctrl('W')
#define iKeyThreadDown ctrl('N')
#define iKeyThreadDown2 'j'
#define iKeyThreadUp ctrl('P')
#define iKeyThreadUp2 'k'
#define iKeyThreadPageUp 'b'
#define iKeyThreadPageUp2 ctrl('U')
#define iKeyThreadPageUp3 ctrl('B')
#define iKeyThreadCatchupConditional 'c'
#define iKeyThreadCatchup 'K'
#define iKeyThreadToggleSubjDisplay 'd'
#define iKeyThreadHelp 'h'
#define iKeyThreadToggleHelpDisplay 'H'
#define iKeyThreadToggleInverseVideo 'I'
#define iKeyThreadQuit 'q'
#define iKeyThreadQuitTin 'Q'
#define iKeyThreadTag 't'
#define iKeyThreadBugReport 'R'
#define iKeyThreadVersion 'v'
#define iKeyThreadMarkArtUnread 'z'
#define iKeyThreadMarkThdUnread 'Z'
#define iKeyThreadMarkArtSel '*'
#define iKeyThreadToggleArtSel '.'
#define iKeyThreadReverseSel '@'
#define iKeyThreadUndoSel '~'
#define iKeyThreadDisplaySubject 'i'
 
/* virtual.c */

#define iKeyVirtualLastPage '$'
#define iKeyVirtualSelect '\r'
#define iKeyVirtualSelect2 '\n'
#define iKeyVirtualPageDown ' '
#define iKeyVirtualPageDown2 ctrl('D')
#define iKeyVirtualPageDown3 ctrl('F')
#define iKeyVirtualRedrawScr ctrl('L')
#define iKeyVirtualDown ctrl('N')
#define iKeyVirtualDown2 'j'
#define iKeyVirtualUp ctrl('P')
#define iKeyVirtualUp2 'k'
#define iKeyVirtualPageUp 'b'
#define iKeyVirtualPageUp2 ctrl('U')
#define iKeyVirtualPageUp3 ctrl('B')
#define iKeyVirtualHelp 'h'
#define iKeyVirtualToggleHelpDisplay 'H'
#define iKeyVirtualToggleInverseVideo 'I'
#define iKeyVirtualQuit 'q'
#define iKeyVirtualQuitTin 'Q'
#define iKeyVirtualBugReport 'R'
#define iKeyVirtualVersion 'v'
