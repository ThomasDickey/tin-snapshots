/*
 *  Project   : tin - a Usenet reader
 *  Module    : policy.h
 *  Author    : Ralf Doeblitz <doeblitz@gmx.de>
 *  Created   : 1999-01-12
 *  Updated   : 2003-03-11
 *  Notes     : #defines and static data for policy configuration
 *
 * Copyright (c) 1999-2003 Ralf Doeblitz <doeblitz@gmx.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by Ralf Doeblitz.
 * 4. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/*
 * tables for TLD search in misc.c, gnksa domain checking
 */

#ifndef TIN_POLICY_H
#	define TIN_POLICY_H 1
/*
 * known two letter country codes
 *
 * .ac  Ascension Island               .ad  Andorra
 * .ae  United Arab Emirates           .af  Afghanistan
 * .ag  Antigua and Barbuda            .ai  Anguilla
 * .al  Albania                        .am  Armenia
 * .an  Netherlands Antilles           .ao  Angola
 * .aq  Antarctica                     .ar  Argentina
 * .as  American Samoa                 .at  Austria
 * .au  Australia                      .aw  Aruba
 * .az  Azerbaijan
 *
 * .ba  Bosnia and Herzegowina         .bb  Barbados
 * .bd  Bangladesh                     .be  Belgium
 * .bf  Burkina Faso                   .bg  Bulgaria
 * .bh  Bahrain                        .bi  Burundi
 * .bj  Benin                          .bm  Bermuda
 * .bn  Brunei Darussalam              .bo  Bolivia
 * .br  Brazil                         .bs  Bahamas
 * .bt  Bhutan                         .bv  Bouvet Island
 * .bw  Botswana                       .by  Belarus
 * .bz  Belize
 *
 * .ca  Canada                         .cc  Cocos (Keeling) Islands
 * .cd  Congo, Democratic People's Republic
 * .cf  Central African Republic       .cg  Congo, Republic of
 * .ch  Switzerland                    .ci  Côte d'Ivoire
 * .ck  Cook Islands                   .cl  Chile
 * .cm  Cameroon                       .cn  China
 * .co  Colombia                       .cr  Costa Rica
 * .cu  Cuba                           .cv  Cape Verde
 * .cx  Christmas Island               .cy  Cyprus
 * .cz  Czech Republic
 *
 * .de  Germany                        .dj  Djibouti
 * .dk  Denmark                        .dm  Dominica
 * .do  Dominican Republic             .dz  Algeria
 *
 * .ec  Ecuador                        .ee  Estonia
 * .eg  Egypt                          .eh  Western Sahara
 * .er  Eritrea                        .es  Spain
 * .et  Ethiopia
 *
 * .fi  Finland                        .fj  Fiji
 * .fk  Falkland Islands (Malvina)     .fm  Micronesia, Federal State of
 * .fo  Faroe Islands                  .fr  France
 *
 * .ga  Gabon                          .gb  United Kingdom
 * .gd  Grenada                        .ge  Georgia
 * .gf  French Guiana                  .gg  Guernsey
 * .gh  Ghana                          .gi  Gibraltar
 * .gl  Greenland                      .gm  Gambia
 * .gn  Guinea                         .gp  Guadelope
 * .gq  Equatorial Guinea              .gr  Greece
 * .gs  South Georgia and the South Sandwich Islands
 * .gt  Guatemala                      .gu  Guam
 * .gw  Guinea-Bissau                  .gy  Guyana
 *
 * .hk  Hong Kong                      .hm  Heard and McDonald Islands
 * .hn  Honduras                       .hr  Croatia/Hrvatska
 * .ht  Haiti                          .hu  Hungary
 *
 * .id  Indonesia                      .ie  Ireland
 * .il  Israel                         .im  Isle of Man
 * .in  India                          .io  British Indian Ocean Territory
 * .iq  Iraq                           .ir  Iran
 * .is  Iceland                        .it  Italy
 *
 * .je  Jersey                         .jm  Jamaica
 * .jo  Jordan                         .jp  Japan
 *
 * .ke  Kenya                          .kg  Kyrgystan
 * .kh  Cambodia                       .ki  Kiribati
 * .km  Comoros                        .kn  Saint Kitts and Nevis
 * .kp  Korea, Democratic People's Republic
 * .kr  Korea, Republic of             .kw  Kuwait
 * .ky  Cayman Islands                 .kz  Kazakhstan
 *
 * .la  Laos (People's Democratic Republic)
 * .lb  Lebanon                        .lc  Saint Lucia
 * .li  Liechtenstein                  .lk  Sri Lanka
 * .lr  Liberia                        .ls  Lesotho
 * .lt  Lithuania                      .lu  Luxembourg
 * .lv  Latvia                         .ly  Libyan Arab Jamahiriya
 *
 * .ma  Morocco                        .mc  Monaco
 * .md  Moldova, Republic of           .mg  Madagascar
 * .mh  Marshall Islands               .mk  Macedonia
 * .ml  Mali                           .mm  Myanmar
 * .mn  Mongolia                       .mo  Macau
 * .mp  Northern Mariana Islands       .mq  Martinique
 * .mr  Mauritania                     .ms  Montserrat
 * .mt  Malta                          .mu  Mauritius
 * .mv  Maldives                       .mw  Malawi
 * .mx  Mexico                         .my  Malaysia
 * .mz  Mozambique
 *
 * .na  Namibia                        .nc  New Caledonia
 * .ne  Niger                          .nf  Norfolk Island
 * .ng  Nigeria                        .ni  Nicaragua
 * .nl  The Netherlands                .no  Norway
 * .np  Nepal                          .nr  Nauru
 * .nu  Niue                           .nz  New Zealand
 *
 * .om  Oman
 *
 * .pa  Panama                         .pe  Peru
 * .pf  French Polynesia               .pg  Papua New Guinea
 * .ph  Philippines                    .pk  Pakistan
 * .pl  Poland                         .pm  St. Pierre and Miquelon
 * .pn  Pitcairn Island                .pr  Puerto Rico
 * .ps  Palestinian Territories        .pt  Portugal
 * .pw  Palau                          .py  Paraguay
 *
 * .qa  Qatar
 *
 * .re  Reunion                        .ro  Romania
 * .ru  Russian Federation             .rw  Rwanda
 *
 * .sa  Saudi Arabia                   .sb  Solomon Islands
 * .sc  Seychelles                     .sd  Sudan
 * .se  Sweden                         .sg  Singapore
 * .sh  St. Helena                     .si  Slovenia
 * .sj  Svalbard and Jan Mayen Islands .sk  Slovakia
 * .sl  Sierra Leone                   .sm  San Marino
 * .sn  Senegal                        .so  Somalia
 * .sr  Surinam                        .st  Sao Tome and Principe
 * .su  Soviet Union (former)          .sv  El Salvador
 * .sy  Syrian Arab Republic           .sz  Swaziland
 *
 * .tc  The Turks & Caicos Islands     .td  Chad
 * .tf  French Southern Territories    .tg  Togo
 * .th  Thailand                       .tj  Tajikistan
 * .tk  Tokelau                        .tm  Turkmenistan
 * .tn  Tunisia                        .to  Tonga
 * .tp  East Timor                     .tr  Turkey
 * .tt  Trinidad and Tobago            .tv  Tuvalu
 * .tw  Taiwan                         .tz  Tanzania
 *
 * .ua  Ukraine                        .ug  Uganda
 * .uk  United Kingdom
 * .um  United States Minor Outlying Islands
 * .us  United States                  .uy  Uruguay
 * .uz  Uzbekistan
 *
 * .va  Holy See (Vatican City State)  .vc  Saint Vincent and the Grenadines
 * .ve  Venezuela                      .vg  Virgin Islands (British)
 * .vi  Virgin Islands (U.S)           .vn  Vietnam
 * .vu  Vanuatu
 *
 * .wf  Wallis and Futuna Islands      .ws  Western Samoa
 *
 * .ye  Yemen                          .yt  Mayotte
 * .yu  Yugoslavia
 *
 * .za  South Africa                   .zm  Zambia
 * .zw  Zimbabwe
 *
 *
 * invalid TLDs:
 * .cs  former Czechoslovakia, now: .cz and .sk
 * .zr  former Zaire, now: .cd
 * .fx  France, Metropolitan
 *
 * unused TLDs:
 * .nt  Neutral Zone
 *
 * requested new TLDs:
 * .eu  European Union
 *
 * 'expireing' TLDs:
 *(.hk  Hong Kong, now: .cn)
 * .su  former USSR, now: .ru
 */

static char gnksa_country_codes[26*26] = {
/*      A B C D E  F G H I J  K L M N O  P Q R S T  U V W X Y Z */
/* A */ 0,0,1,1,1, 1,1,0,1,0, 0,1,1,1,1, 0,1,1,1,1, 1,0,1,0,0,1,
/* B */ 1,1,0,1,1, 1,1,1,1,1, 0,0,1,1,1, 0,0,1,1,1, 0,1,1,0,1,1,
/* C */ 1,0,1,1,0, 1,1,1,1,0, 1,1,1,1,1, 0,0,1,0,0, 1,1,0,1,1,1,
/* D */ 0,0,0,0,1, 0,0,0,0,1, 1,0,1,0,1, 0,0,0,0,0, 0,0,0,0,0,1,
/* E */ 0,0,1,0,1, 0,1,1,0,0, 0,0,0,0,0, 0,0,1,1,1, 0,0,0,0,0,0,
/* F */ 0,0,0,0,0, 0,0,0,1,1, 1,0,1,0,1, 0,0,1,0,0, 0,0,0,0,0,0,
/* G */ 1,1,0,1,1, 1,1,1,1,0, 0,1,1,1,0, 1,1,1,1,1, 1,0,1,0,1,0,
/* H */ 0,0,0,0,0, 0,0,0,0,0, 1,0,1,1,0, 0,0,1,0,1, 1,0,0,0,0,0,
/* I */ 0,0,0,1,1, 0,0,0,0,0, 0,1,1,1,1, 0,1,1,1,1, 0,0,0,0,0,0,
/* J */ 0,0,0,0,1, 0,0,0,0,0, 0,0,1,0,1, 1,0,0,0,0, 0,0,0,0,0,0,
/* K */ 0,0,0,0,1, 0,1,1,1,0, 0,0,1,1,0, 1,0,1,0,0, 0,0,1,0,1,1,
/* L */ 1,1,1,0,0, 0,0,0,1,0, 1,0,0,0,0, 0,0,1,1,1, 1,1,0,0,1,0,
/* M */ 1,0,1,1,0, 0,1,1,0,0, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1,1,
/* N */ 1,0,1,0,1, 1,1,0,1,0, 0,1,0,0,1, 1,0,1,0,0, 1,0,0,0,0,1,
/* O */ 0,0,0,0,0, 0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0, 0,0,0,0,0,0,
/* P */ 1,0,0,0,1, 1,1,1,0,0, 1,1,1,1,0, 0,0,1,1,1, 0,0,1,0,1,0,
/* Q */ 1,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,0,
/* R */ 0,0,0,0,1, 0,0,0,0,0, 0,0,0,0,1, 0,0,0,0,0, 1,0,1,0,0,0,
/* S */ 1,1,1,1,1, 0,1,1,1,1, 1,1,1,1,1, 0,0,1,0,1, 1,1,0,0,1,1,
/* T */ 0,0,1,1,0, 1,1,1,0,1, 1,0,1,1,1, 1,0,1,0,1, 0,1,1,0,0,1,
/* U */ 1,0,0,0,0, 0,1,0,0,0, 1,0,1,0,0, 0,0,0,1,0, 0,0,0,0,1,1,
/* V */ 1,0,1,0,1, 0,1,0,1,0, 0,0,0,1,0, 0,0,0,0,0, 1,0,0,0,0,0,
/* W */ 0,0,0,0,0, 1,0,0,0,0, 0,0,0,0,0, 0,0,0,1,0, 0,0,0,0,0,0,
/* X */ 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,0,
/* Y */ 0,0,0,0,1, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,1, 1,0,0,0,0,0,
/* Z */ 1,0,0,0,0, 0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0, 0,0,1,0,0,0
/*      A B C D E  F G H I J  K L M N O  P Q R S T  U V W X Y Z */
};


/*
 * valid domains with 3 or more characters
 */
static const char *gnksa_domain_list[] = {
	"biz",	/* Businesses */
	"com",	/* Commercial */
	"edu",	/* Educational */
	"gov",	/* US Government */
	"int",	/* International Organizations */
	"mil",	/* US Dept of Defense */
	"net",	/* Networks */
	"org",	/* Organizations */
	"pro",	/* Accountants, lawyers, and physicians */
	"aero",	/* Air-transport industry */
	"arpa",	/* Address and Routing Parameter Area */
	"coop",	/* Non-profit cooperatives */
	"info",	/* Unrestricted use */
	"name",	/* For registration by individuals */
	"museum",	/* Museums */
	/* 3 more gTLDs to come in 2003 (.travel is likely to be one of them) */
#	if 0		/* $DEAD */
	"uucp",
	"bitnet",
#	endif /* 0 */
	/* the next four are defined in RFC 2606 */
	"invalid",
#	if 0
	/* but three of them shoudn't be used on usenet */
	"test",
	"example",
	"localhost",
#	endif /* 0 */
#	ifdef TINC_DNS
	"bofh",	/* There Is No Cabal */
#	endif /* TINC_DNS */
	/* sentinel */
	""
};
#endif /* !TIN_POLICY_H */
