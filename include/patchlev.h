/*
 *  Project   : tin - a Usenet reader
 *  Module    : patchlev.h
 *  Author    : I.Lea
 *  Created   : 01-04-91
 *  Updated   : 12-06-96
 *  Notes     :
 *  Copyright : (c) Copyright 1991-94 by Iain Lea
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#define VERSION		"1.3 unoff BETA"
#define RELEASEDATE	"960822"

/* This is NO official release, but a patched version.
 * Including patches from:
 *
 * 	Greg Berigan		<gberigan@cse.unl.edu>
 * 	Enrik Berkhan		<enrik@akk.uni-karlsruhe.de>
 *	Juergen Bernau		<pas@freitag.muc.de>
 * 	Chris Blum		<chris@phil.uni-sb.de>
 *	Lars Dannenberg		<lars@vendetta.han.de>
 *	Giuseppe De Marco	<gdemarco@freenet.hut.fi>
 *	T.E. Dickey		<dickey@clark.net>
 *	Bernd Eckenfels		<ecki@lina.inka.de>
 *	Jason Faultless		<jason@radar.demon.co.uk>
 *	Ulli Horlacher		<framstag@moep.bb.bawue.de>
 * 	Urs Janssen		<urs@akk.uni-karlsruhe.de>
 *	Olaf Kaluza		<olaf@criseis.ruhr.de>
 *	Tomasz Kloczko		<kloczek@rudy.mif.pg.gda.pl>
 *	Alexander Lehmann	<alex@hal.rhein-main.de>
 *	Daniel Naber		<dnaber@pluto.teuto.de>
 *	Torsten Neumann		<torsten@imke.infodrom.north.de>
 *	Dirk Nimmich		<nimmich@uni-muenster.de>
 *	Seven Paulus		<sven@oops.sub.de>
 *	Branden Robinson	<branden@purdue.edu>
 *	Roland Rosenfeld	<roland@spinnaker.rhein.de>
 *	Volker Schmidt		<volker@illuminatus.mz.rhein-main.de>
 * 	Stefan Scholl		<stesch@sks.inka.de>
 *	Jungshik Shin		<jshin@pantheon.yale.edu>
 *	Dieter Stueken		<stueken@uni-muenster.de>
 *	Dean Takemori		<dean@uhheph.phys.hawaii.edu>
 *
 */

#ifdef M_AMIGA
#	define	OS	"AMIGA"
#	define	AMIVER	"1.3B"
#endif
#ifdef M_OS2
#	define	OS	"OS/2"
#endif
#ifdef M_UNIX
#	define	OS	"UNIX"
#endif
#ifdef WIN32
#	define	OS	"Windows/NT"
#endif
