/*
 *  Project   : tin - a Usenet reader
 *  Module    : policy.h
 *  Author    : R.Doeblitz
 *  Created   : 1999-01-12
 *  Updated   :
 *  Notes     : #defines and static data for policy configuration
 *  Copyright : (c) Copyright 1999 by Ralf Doeblitz
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */


/*
 * tables for TLD search in misc.c, gnksa domain checking
 */

#ifndef TIN_POLICY_H
#	define TIN_POLICY_H 1
/*
 * known two letter country codes
 */
static char gnksa_country_codes[26*26] = {
/*      A B C D E  F G H I J  K L M N O  P Q R S T  U V W X Y Z */
/* A */ 0,0,1,1,1, 1,1,0,1,0, 0,1,1,1,1, 0,1,1,1,1, 1,0,1,0,0,1,
/* B */ 1,1,0,1,1, 1,1,1,1,1, 0,0,1,1,1, 0,0,1,1,1, 0,1,1,0,1,1,
/* C */ 1,0,1,1,0, 1,1,1,1,0, 1,1,1,1,1, 0,0,1,0,0, 1,1,0,1,1,1,
/* D */ 0,0,0,0,1, 0,0,0,0,1, 1,0,1,0,1, 0,0,0,0,0, 0,0,0,0,0,1,
/* E */ 0,0,1,0,1, 0,1,1,0,0, 0,0,0,0,0, 0,0,1,1,1, 0,0,0,0,0,0,
/* F */ 0,0,0,0,0, 0,0,0,1,1, 1,0,1,0,1, 0,0,1,0,0, 0,0,0,1,0,0,
/* G */ 1,1,0,1,1, 1,1,1,1,0, 0,1,1,1,0, 1,1,1,1,1, 1,0,1,0,1,0,
/* H */ 0,0,0,0,0, 0,0,0,0,0, 1,0,1,1,0, 0,0,1,0,1, 1,0,0,0,0,0,
/* I */ 0,0,0,1,1, 0,0,0,0,0, 0,1,1,1,1, 0,1,1,1,1, 0,0,0,0,0,0,
/* J */ 0,0,0,0,1, 0,0,0,0,0, 0,0,1,0,1, 1,0,0,0,0, 0,0,0,0,0,0,
/* K */ 0,0,0,0,1, 0,1,0,1,0, 0,0,1,1,0, 1,0,1,0,0, 0,0,1,0,1,1,
/* L */ 1,1,1,0,0, 0,0,0,1,0, 1,0,0,0,0, 0,0,1,1,1, 1,1,0,0,1,0,
/* M */ 1,0,1,1,0, 0,1,1,0,0, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1,1,
/* N */ 1,0,1,0,1, 1,1,0,1,0, 0,1,0,0,1, 1,0,1,0,0, 1,0,0,0,0,1,
/* O */ 0,0,0,0,0, 0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0, 0,0,0,0,0,0,
/* P */ 1,0,0,0,1, 1,1,1,0,0, 1,1,1,1,0, 0,0,1,0,1, 0,0,1,0,1,0,
/* Q */ 1,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,0,
/* R */ 0,0,0,0,1, 0,0,0,0,0, 0,0,0,0,1, 0,0,0,0,0, 1,0,1,0,0,0,
/* S */ 1,1,1,1,1, 0,1,1,1,1, 1,1,1,1,1, 0,0,1,0,1, 1,1,0,0,1,1,
/* T */ 0,0,1,1,0, 1,1,1,0,1, 1,0,1,1,1, 1,0,1,0,1, 0,1,1,0,0,1,
/* U */ 1,0,0,0,0, 0,1,0,0,0, 1,0,1,0,0, 0,0,0,1,0, 0,0,0,0,1,1,
/* V */ 1,0,1,0,1, 0,1,0,1,0, 0,0,0,1,0, 0,0,0,0,0, 1,0,0,0,0,0,
/* W */ 0,0,0,0,0, 1,0,0,0,0, 0,0,0,0,0, 0,0,0,1,0, 0,0,0,0,0,0,
/* X */ 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,0,
/* Y */ 0,0,0,0,1, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,1, 1,0,0,0,0,0,
/* Z */ 1,0,0,0,0, 0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0, 0,0,0,0,0,1
/*      A B C D E  F G H I J  K L M N O  P Q R S T  U V W X Y Z */
};

/*
 * valid domains with 3 or more characters
 *
 * later add: nom, rec, web, arts, firm, info, shop
 */
static const char *gnksa_domain_list[] = {
	"com",
	"edu",
	"gov",
	"int",
	"mil",
	"net",
	"org",
	"arpa",
	"uucp",
	"bitnet",
#	if 0
	/* the new domain names, not yet valid */
	"nom",
	"rec",
	"web",
	"arts",
	"firm",
	"info",
	"shop",
#	endif /* 0 */
#	ifdef TINC_DNS
	"bofh",
#	endif /* TINC_DNS */
	/* for forged addresses */
	"invalid",
	/* sentinel */
	""
};
#endif /* !TIN_POLICY_H */

/* end of file */
