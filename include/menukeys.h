/*
 *  Project   : tin - a Usenet reader
 *  Module    : menukeys.h
 *  Author    : S.Robbins
 *  Created   : 1994-08-15
 *  Updated   : 1995-12-21
 *  Notes     :
 *  Copyright : (c) Copyright 1991-99 by Steve Robbins & Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#ifndef MENUKEYS_H
#	define MENUKEYS_H 1


/* Revised 9 October 1996 by Branden Robinson in ASCII order
 *
 *        Oct   Dec   Hex   Char           Oct   Dec   Hex   Char
 *        ------------------------------------------------------------
 *        000   0     00    NUL '\0'       100   64    40    @
 *        001   1     01    SOH      ^A    101   65    41    A
 *        002   2     02    STX      ^B    102   66    42    B
 *        003   3     03    ETX      ^C    103   67    43    C
 *        004   4     04    EOT      ^D    104   68    44    D
 *        005   5     05    ENQ      ^E    105   69    45    E
 *        006   6     06    ACK      ^F    106   70    46    F
 *        007   7     07    BEL '\a' ^G    107   71    47    G
 *        010   8     08    BS  '\b' ^H    110   72    48    H
 *        011   9     09    HT  '\t' ^I    111   73    49    I
 *        012   10    0A    LF  '\n' ^J    112   74    4A    J
 *        013   11    0B    VT  '\v' ^K    113   75    4B    K
 *        014   12    0C    FF  '\f' ^L    114   76    4C    L
 *        015   13    0D    CR  '\r' ^M    115   77    4D    M
 *        016   14    0E    SO       ^N    116   78    4E    N
 *        017   15    0F    SI       ^O    117   79    4F    O
 *        020   16    10    DLE      ^P    120   80    50    P
 *        021   17    11    DC1      ^Q    121   81    51    Q
 *        022   18    12    DC2      ^R    122   82    52    R
 *        023   19    13    DC3      ^S    123   83    53    S
 *        024   20    14    DC4      ^T    124   84    54    T
 *        025   21    15    NAK      ^U    125   85    55    U
 *        026   22    16    SYN      ^V    126   86    56    V
 *        027   23    17    ETB      ^W    127   87    57    W
 *        030   24    18    CAN      ^X    130   88    58    X
 *        031   25    19    EM       ^Y    131   89    59    Y
 *        032   26    1A    SUB      ^Z    132   90    5A    Z
 *        033   27    1B    ESC            133   91    5B    [
 *        034   28    1C    FS             134   92    5C    \   '\\'
 *        035   29    1D    GS             135   93    5D    ]
 *        036   30    1E    RS             136   94    5E    ^
 *        037   31    1F    US             137   95    5F    _
 *        040   32    20    SPACE          140   96    60    `
 *        041   33    21    !              141   97    61    a
 *        042   34    22    "              142   98    62    b
 *        043   35    23    #              143   99    63    c
 *        044   36    24    $              144   100   64    d
 *        045   37    25    %              145   101   65    e
 *        046   38    26    &              146   102   66    f
 *        047   39    27    '              147   103   67    g
 *        050   40    28    (              150   104   68    h
 *        051   41    29    )              151   105   69    i
 *        052   42    2A    *              152   106   6A    j
 *        053   43    2B    +              153   107   6B    k
 *        054   44    2C    ,              154   108   6C    l
 *        055   45    2D    -              155   109   6D    m
 *        056   46    2E    .              156   110   6E    n
 *        057   47    2F    /              157   111   6F    o
 *        060   48    30    0              160   112   70    p
 *        061   49    31    1              161   113   71    q
 *        062   50    32    2              162   114   72    r
 *        063   51    33    3              163   115   73    s
 *        064   52    34    4              164   116   74    t
 *        065   53    35    5              165   117   75    u
 *        066   54    36    6              166   118   76    v
 *        067   55    37    7              167   119   77    w
 *        070   56    38    8              170   120   78    x
 *        071   57    39    9              171   121   79    y
 *        072   58    3A    :              172   122   7A    z
 *        073   59    3B    ;              173   123   7B    {
 *        074   60    3C    <              174   124   7C    |
 *        075   61    3D    =              175   125   7D    }
 *        076   62    3E    >              176   126   7E    ~
 *        077   63    3F    ?              177   127   7F    DEL
 *
 * Above chart reprinted from Linux manual page.
 *
 * When adding key functionality, be aware of key functions in the "big five"
 * levels of tin operation: top (group selection), group, thread, article
 * (pager), and help.  If possible, when adding a key to any of these levels,
 * check the others to make sure that the key doesn't do something
 * non-analogous elsewhere.  For instance, having "^R" map to "redraw screen"
 * at article level and "reset .newsrc" (a drastic and unreversible action)
 * at top level is a bad idea.
 *
 * [make emacs happy: "]
 */

/*
 * Global keys
 */

#define iKeyAbort ESC
#define iKeyPageDown3 ' '
#ifndef NO_SHELL_ESCAPE
#	define iKeyShellEscape '!'
#endif /* !NO_SHELL_ESCAPE */
#define iKeySetRange '#'
#define iKeyLastPage '$'
#define iKeyToggleColor '&'
#define iKeySearchSubjF '/'
#define iKeySearchSubjB '?'
#define iKeySearchAuthB 'A'
#define iKeySearchBody 'B'
#define iKeyLookupMessage 'L'
#define iKeyOptionMenu 'M'
#define iKeyPostponed2 'O'
#define iKeyQuitTin 'Q'
#define iKeyDisplayPostHist 'W'
#define iKeyFirstPage '^'
#define iKeySearchAuthF 'a'
#define iKeyPageUp3 'b'
#define iKeyToggleInfoLastLine 'i'
#define iKeyDown2 'j'
#define iKeyUp2 'k'
#define iKeyQuit 'q'
#define iKeyVersion 'v'
#define iKeyPageUp ctrl('B')
#define iKeyPageDown ctrl('D')
#define iKeyPageDown2 ctrl('F')
#define iKeyRedrawScr ctrl('L')
#define iKeyDown ctrl('N')
#define iKeyPostponed ctrl('O')
#define iKeyUp ctrl('P')
#define iKeyPageUp2 ctrl('U')

/* config.c */

#define iKeyConfigSelect '\n'
#define iKeyConfigSelect2 '\r'
#define iKeyConfigLastPage 'G'
#define iKeyConfigNoSave 'Q'
#define iKeyConfigFirstPage 'g'

/* feed.c */

#define iKeyFeedTag 'T'
#define iKeyFeedArt 'a'
#define iKeyFeedHot 'h'
#define iKeyFeedPat 'p'
#define iKeyFeedRepost 'r'
#define iKeyFeedSupersede 's'
#define iKeyFeedThd 't'

/* filter.c */

#define iKeyFilterEdit 'e'
#define iKeyFilterSave 's'

/* group.c */

#define iKeyGroupAutoSel ctrl('A')
#define iKeyGroupNextUnreadArtOrGrp '\t'
#define iKeyGroupReadBasenote '\n'
#define iKeyGroupKill ctrl('K')
#define iKeyGroupReadBasenote2 '\r'
#define iKeyGroupSelThd '*'
#define iKeyGroupDoAutoSel '+'
#define iKeyGroupLastViewed '-'
#define iKeyGroupToggleThdSel '.'
/* digits 0-9 in use */
#define iKeyGroupSelThdIfUnreadSelected ';'
#define iKeyGroupSelPattern '='
#define iKeyGroupReverseSel '@'
#define iKeyGroupCatchupNextUnread 'C'
#define iKeyGroupToggleGetartLimit 'G'
#define iKeyGroupToggleHelpDisplay 'H'
#define iKeyGroupToggleInverseVideo 'I'
#define iKeyGroupMarkThdRead 'K'
#define iKeyGroupNextUnreadArt 'N'
#define iKeyGroupPrevUnreadArt 'P'
#define iKeyGroupBugReport 'R'
#define iKeyGroupAutoSaveTagged 'S'
#define iKeyGroupTagParts 'T'
#define iKeyGroupUntag 'U'
#define iKeyGroupMarkUnselArtRead 'X'
#define iKeyGroupMarkThdUnread 'Z'
#define iKeyGroupQuickAutoSel '['
#define iKeyGroupQuickKill ']'
#define iKeyGroupCatchup 'c'
#define iKeyGroupToggleSubjDisplay 'd'
#define iKeyGroupGoto 'g'
#define iKeyGroupHelp 'h'
#define iKeyGroupListThd 'l'
#define iKeyGroupMail 'm'
#define iKeyGroupNextGroup 'n'
#ifndef DISABLE_PRINTING
#	define iKeyGroupPrint 'o'
#endif /* !DISABLE_PRINTING */
#define iKeyGroupPrevGroup 'p'
#define iKeyGroupToggleReadUnread 'r'
#define iKeyGroupSave 's'
#define iKeyGroupTag 't'
#define iKeyGroupToggleThreading 'u'
#define iKeyGroupPost 'w'
#define iKeyGroupRepost 'x'
#define iKeyGroupMarkArtUnread 'z'
#define iKeyGroupPipe '|'
#define iKeyGroupUndoSel '~'

/* help.c */

#define iKeyHelpLastPage 'G'
#define iKeyHelpFirstPage 'g'
/* all other keys exit help */

/* nrctbl.c */

#define iKeyNrctblCreate 'c'
#define iKeyNrctblDefault 'd'
#define iKeyNrctblAlternative 'a'
#define iKeyNrctblQuit 'q'

/* page.c */

#define iKeyPageAutoSel ctrl('A')
#define iKeyPageReplyQuoteHeaders ctrl('E')
#ifdef HAVE_PGP
#	define iKeyPagePGPCheckArticle ctrl('G')
#endif /* HAVE_PGP */
#define iKeyPageToggleHeaders ctrl('H')
#define iKeyPageNextUnread '\t'
#define iKeyPageNextThd '\n'
#define iKeyPageAutoKill ctrl('K')
#define iKeyPageNextThd2 '\r'
#define iKeyPageToggleTabs ctrl('T')
#define iKeyPageFollowupQuoteHeaders ctrl('W')
#define iKeyPageToggleTex2iso '\"'
#define iKeyPageToggleRot '%'
#ifdef HAVE_COLOR
#	define iKeyPageToggleHighlight '_'
#endif /* HAVE_COLOR */
#define iKeyPageLastViewed '-'
/* digits 0-9 in use */
#define iKeyPageSkipIncludedText ':'
#define iKeyPageTopThd '<'
#define iKeyPageBotThd '>'
#define iKeyPageCatchupNextUnread 'C'
#define iKeyPageCancel 'D'
#define iKeyPageFollowup 'F'
#define iKeyPageLastPage2 'G'
#define iKeyPageToggleHelpDisplay 'H'
#define iKeyPageToggleInverseVideo 'I'
#define iKeyPageKillThd 'K'
#define iKeyPageNextUnreadArt 'N'
#define iKeyPagePrevUnreadArt 'P'
#define iKeyPageReply 'R'
#define iKeyPageAutoSaveTagged 'S'
#define iKeyPageGroupSel 'T'
#define iKeyPageMarkThdUnread 'Z'
#define iKeyPageCatchup 'c'
#define iKeyPageToggleRot2 'd'
#define iKeyPageEdit 'e'
#define iKeyPageFollowupQuote 'f'
#define iKeyPageFirstPage2 'g'
#define iKeyPageHelp 'h'
#define iKeyPageKillArt 'k' /* overrides iKeyUp3, which has no effect at pager level */
#define iKeyPageListThd 'l'
#define iKeyPageMail 'm'
#define iKeyPageNextArt 'n'
#ifndef DISABLE_PRINTING
#	define iKeyPagePrint 'o'
#endif /* !DISABLE_PRINTING */
#define iKeyPagePrevArt 'p'
#define iKeyPageReplyQuote 'r'
#define iKeyPageSave 's'
#define iKeyPageTag 't'
#define iKeyPageGotoParent 'u'
#define iKeyPagePost 'w'
#define iKeyPageRepost 'x'
#define iKeyPageMarkArtUnread 'z'
#define iKeyPagePipe '|'

/* post.c */

#define iKeyPostCancel 'd'
#define iKeyPostEdit 'e'
#ifdef HAVE_PGP
#	define iKeyPostPGP 'g'
#endif /* HAVE_PGP */
#ifdef HAVE_ISPELL
#	define iKeyPostIspell 'i'
#endif /* HAVE_ISPELL */
#define iKeyPostContinue 'c'
#define iKeyPostAbort 'a'
#define iKeyPostIgnore 'i'
#define iKeyPostPost 'p'
#define iKeyPostPost2 'y'
#define iKeyPostSend 's'
#define iKeyPostSend2 'y'
#define iKeyPostSupersede 's'
#define iKeyPostPostpone 'o'

#define iKeyPostponeYes 'y'
#define iKeyPostponeYesOverride 'Y'
#define iKeyPostponeYesAll 'A'
#define iKeyPostponeNo 'n'

/* prompt.c */

#define iKeyPromptYes 'y'
#define iKeyPromptNo 'n'

/* save.c */

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

#define iKeySelectEnterNextUnreadGrp '\t'
#define iKeySelectReadGrp '\n'
#define iKeySelectReadGrp2 '\r'
#define iKeySelectResetNewsrc ctrl('R')
#define iKeySelectCatchupNextUnread 'C'
#define iKeySelectToggleHelpDisplay 'H'
#define iKeySelectToggleInverseVideo 'I'
#define iKeySelectNextUnreadGrp 'N'
#define iKeySelectBugReport 'R'
#define iKeySelectSubscribePat 'S'
#define iKeySelectUnsubscribePat 'U'
#define iKeySelectQuitNoWrite 'X'
#define iKeySelectSyncWithActive 'Y'
#define iKeySelectMarkGrpUnread2 'Z'
#define iKeySelectCatchup 'c'
#define iKeySelectToggleDescriptions 'd'
#define iKeySelectGoto 'g'
#define iKeySelectHelp 'h'
#define iKeySelectMoveGrp 'm'
#define iKeySelectEnterNextUnreadGrp2 'n'
#define iKeySelectToggleReadDisplay 'r'
#define iKeySelectSubscribe 's'
#define iKeySelectUnsubscribe 'u'
#define iKeySelectPost 'w'
#define iKeySelectYankActive 'y'
#define iKeySelectMarkGrpUnread 'z'

/* thread.c */

#define iKeyThreadReadNextArtOrThread '\t'
#define iKeyThreadReadArt '\n'
#define iKeyThreadReadArt2 '\r'
#define iKeyThreadMarkArtSel '*'
#define iKeyThreadLastViewed '-'
#define iKeyThreadToggleArtSel '.'
/* digits 0-9 in use */
#define iKeyThreadReverseSel '@'
#define iKeyThreadCatchupNextUnread 'C'
#define iKeyThreadToggleHelpDisplay 'H'
#define iKeyThreadToggleInverseVideo 'I'
#define iKeyThreadMarkArtRead 'K'
#define iKeyThreadBugReport 'R'
#define iKeyThreadAutoSaveTagged 'S'
#define iKeyThreadUntag 'U'
#define iKeyThreadMarkThdUnread 'Z'
#define iKeyThreadQuickAutoSel '['		/* Merge with same keys in Group ? */
#define iKeyThreadQuickKill ']'
#define iKeyThreadCatchup 'c'
#define iKeyThreadToggleSubjDisplay 'd'
#define iKeyThreadHelp 'h'
#define iKeyThreadMail 'm'
#define iKeyThreadSave 's'
#define iKeyThreadTag 't'
#define iKeyThreadPost 'w'
#define iKeyThreadMarkArtUnread 'z'
#define iKeyThreadUndoSel '~'

#endif /* !MENUKEYS_H */
