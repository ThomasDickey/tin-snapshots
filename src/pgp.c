/*
 *  Project   : tin - a Usenet reader
 *  Module    : pgp.c
 *  Author    : Steven J. Madsen
 *  Created   : 1995-05-12
 *  Updated   : 1997-12-31
 *  Notes     : PGP support for article posting and mailing
 *  Copyright : (c) 1995-99 by Steven J. Madsen
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include "tin.h"
#include "extern.h"
#include "tcurses.h"

#ifdef HAVE_PGP

#	ifndef HAVE_PGP_5
#		define HAVE_PGP_2
#		define PGP_PUBRING "pubring.pgp"
#	else
#		define PGP_PUBRING "pubring.pkr"
#	endif /* !HAVE_PGP_5 */

#	define PGP_SIG_TAG "-----BEGIN PGP SIGNED MESSAGE-----\n"
#	define PGP_KEY_TAG "-----BEGIN PGP PUBLIC KEY BLOCK-----\n"

/*
 * name of environment variable of options to pass to PGP
 */
#	define PGPNAME "pgp"
#	define PGPOPTS "PGPOPTS"

#	define HEADERS	"%stin-%d.h"
#	ifdef HAVE_LONG_FILE_NAMES
#		define PLAINTEXT	"%stin-%d.pt"
#		define CIPHERTEXT	"%stin-%d.pt.asc"
#		define KEYFILE	"%stin-%d.k.asc"
#	else
#		define PLAINTEXT	"%stn-%d.p"
#		define CIPHERTEXT	"%stn-%d.p.asc"
#		define KEYFILE	"%stn-%d.k.asc"
#	endif /* HAVE_LONG_FILE_NAMES */


#	define PGP_SIGN 0x01
#	define PGP_ENCRYPT 0x02

/*
 * local prototypes
 */
static t_bool pgp_available (void);
static void do_pgp (int what, char *file, char *mail_to);
static void join_files (char *file);
static void pgp_append_public_key (char *file);
static void split_file (char *file);

char pgp_data[PATH_LEN];
const char *pgpopts = "";
static char hdr[PATH_LEN], pt[PATH_LEN], ct[PATH_LEN];

static void
join_files (
	char *file)
{
	FILE *art, *header, *text;
	char buf[LEN];

	if ((header = fopen(hdr, "r")) == (FILE *) 0)
		return;

	if ((text = fopen(ct, "r")) == (FILE *) 0) {
		fclose(header);
		return;
	}
	if ((art = fopen(file, "w")) == (FILE *) 0) {
		fclose(art);
		fclose(text);
		return;
	}
	fgets(buf, LEN, header);
	while (!feof(header)) {
		fputs(buf, art);
		fgets(buf, LEN, header);
	}
	fclose(header);
	fgets(buf, LEN, text);
	while (!feof(text)) {
		fputs(buf, art);
		fgets(buf, LEN, text);
	}
	fclose(text);
	fclose(art);
}


static void
split_file (
	char *file)
{
	FILE *art, *header, *text;
	char buf[LEN];
	int pid;
	mode_t mask;

	pid = getpid();
	sprintf(hdr, HEADERS, TMPDIR, pid);
	sprintf(pt, PLAINTEXT, TMPDIR, pid);
	sprintf(ct, CIPHERTEXT, TMPDIR, pid);
	mask = umask((mode_t) (S_IRWXO|S_IRWXG));
	if ((art = fopen(file, "r")) == (FILE *) 0)
		return;

	if ((header = fopen(hdr, "w")) == (FILE *) 0) {
		fclose(art);
		return;
	}
	if ((text = fopen(pt, "w")) == (FILE *) 0) {
		fclose(art);
		fclose(header);
		return;
	}
	fgets(buf, LEN, art);
	while (strcmp(buf, "\n")) {
		fputs(buf, header);
		fgets(buf, LEN, art);
	}
	fputs(buf, header);
	fclose(header);
	fgets(buf, LEN, art);
	while (!feof(art)) {
		fputs(buf, text);
		fgets(buf, LEN, art);
	}
	fclose(art);
	fclose(text);
	umask(mask);
}


static void
do_pgp (
	int what,
	char *file,
	char *mail_to)
{
	char cmd[LEN], options[10];
#ifndef HAVE_PGP_5
	char address[LEN];
#endif /* !HAVE_PGP_5 */

	split_file(file);
	strcpy (options, "-at");

#	ifdef HAVE_PGP_2
	if (what & PGP_ENCRYPT)
		strcat(options, "e");
	if (what & PGP_SIGN)
		strcat(options, "s");

	if (*mail_address) {
		strip_address (mail_address, address);
		sh_format (cmd, sizeof(cmd), "%s %s %s %s %s -u %s", PGPNAME, pgpopts, options, pt, mail_to ? mail_to : "", address);
	} else
		sh_format (cmd, sizeof(cmd), "%s %s %s %s %s", PGPNAME, pgpopts, options, pt, mail_to ? mail_to : "");

#	else /* FIXME - check mail_address */
#		ifdef HAVE_PGP_5
	if (what & PGP_ENCRYPT && what & PGP_SIGN) {
		strcat (options, "s");
		sh_format (cmd, sizeof(cmd), "%se %s %s %s %s", PGPNAME, pgpopts, options, pt, mail_to ? mail_to : "");
	} else
		sh_format (cmd, sizeof(cmd), "%s%s %s %s %s %s", PGPNAME, (what & PGP_ENCRYPT ? "e" : "s"), pgpopts, options, pt, mail_to ? mail_to : "");
#		endif /* HAVE_PGP_5 */
#	endif /* HAVE_PGP_2 */

	invoke_cmd(cmd);
	join_files(file);
	unlink(pt);
	unlink(hdr);
	unlink(ct);
}


static void
pgp_append_public_key (
	char *file)
{
	FILE *f, *key;
	char keyfile[PATH_LEN], cmd[LEN], buf[LEN];

	if (*mail_address)
		strip_address (mail_address, buf);
	else
		sprintf(buf, "%s@%s", userid, host_name);

	sprintf(keyfile, KEYFILE, TMPDIR, (char)getpid());

#	ifdef HAVE_PGP_2
	sh_format (cmd, sizeof(cmd), "%s %s -kxa %s %s", PGPNAME, pgpopts, buf, keyfile);
#	else
#		ifdef HAVE_PGP_5
	sh_format (cmd, sizeof(cmd), "%sk %s -xa %s -o %s", PGPNAME, pgpopts, buf, keyfile);
#		endif /* HAVE_PGP_5 */
#	endif /* HAVE_PGP_2 */

	if (invoke_cmd (cmd)) {
		if ((f = fopen(file, "a")) == (FILE *) 0)
			return;
		if ((key = fopen(keyfile, "r")) == (FILE *) 0) {
			fclose(f);
			return;
		}
		fputc('\n', f);
		fgets(buf, LEN, key);
		while (!feof(key)) {
			fputs(buf, f);
			fgets(buf, LEN, key);
		}
		fclose(f);
		fclose(key);
		unlink(keyfile);
	}
}


static t_bool
pgp_available (
	void)
{
	FILE *f;
	char keyring[PATH_LEN];

	joinpath(keyring, pgp_data, PGP_PUBRING);
	if ((f = fopen(keyring, "r")) == (FILE *) 0)
		return FALSE;
	else {
		fclose(f);
		return TRUE;
	}
}


void
invoke_pgp_mail (
	char *nam,
	char *mail_to)
{
	char ch, ch_default = 's';

	if (!pgp_available()) {
		info_message(txt_pgp_not_avail);
		return;
	}
	ch = prompt_slk_response(ch_default, "beqs\033", txt_pgp_mail);
	switch (ch) {
		case '\033':
		case 'q':
			break;

		case 's':
#ifdef HAVE_PGP_5
			ClearScreen();
			MoveCursor (cLINES - 7, 0);
#endif /* HAVE_PGP_5 */
			do_pgp(PGP_SIGN, nam, NULL);
			break;

		case 'b':
#ifdef HAVE_PGP_5
			ClearScreen();
			MoveCursor (cLINES - 7, 0);
#endif /* HAVE_PGP_5 */
			do_pgp(PGP_SIGN | PGP_ENCRYPT, nam, mail_to);
			break;

		case 'e':
			do_pgp(PGP_ENCRYPT, nam, mail_to);
			break;

		default:
			break;
	}
}


void
invoke_pgp_news (
	char *the_article)
{
	char ch, ch_default = 's';

	if (!pgp_available()) {
		info_message(txt_pgp_not_avail);
		return;
	}
	ch = prompt_slk_response(ch_default, "iqs\033", txt_pgp_news);
	switch (ch) {
		case '\033':
		case 'q':
			break;

		case 's':
#ifdef HAVE_PGP_5
			info_message (" ");
			MoveCursor (cLINES - 7, 0);
			my_printf("\n");
#endif /* HAVE_PGP_5 */
			do_pgp(PGP_SIGN, the_article, NULL);
			break;

		case 'i':
#ifdef HAVE_PGP_5
			info_message (" ");
			MoveCursor (cLINES - 7, 0);
			my_printf("\n");
#endif /* HAVE_PGP_5 */
			do_pgp(PGP_SIGN, the_article, NULL);
			pgp_append_public_key(the_article);
			break;

		default:
			break;
	}
}


int
pgp_check_article (
	void)
{
	FILE *art;
	char the_article[PATH_LEN], buf[LEN], cmd[LEN];
	t_bool pgp_signed = FALSE;
	t_bool pgp_key = FALSE;

	if (!pgp_available()) {
		info_message(txt_pgp_not_avail);
		return 0;
	}
	joinpath(the_article, homedir, ".article");

#	ifdef APPEND_PID
	sprintf (the_article+strlen(the_article), ".%d", process_id);
#	endif /* APPEND_PID */

	if ((art = fopen(article, "w")) == (FILE *) 0) {
		info_message(txt_cannot_open, the_article);
		return 0;
	}
	fseek(note_fp, mark_body, SEEK_SET);
	fgets(buf, LEN, note_fp);
	while (!feof(note_fp)) {
		if (!strcmp(buf, PGP_SIG_TAG))
			pgp_signed = TRUE;
		if (!strcmp(buf, PGP_KEY_TAG))
			pgp_key = TRUE;
		fputs(buf, art);
		fgets(buf, LEN, note_fp);
	}
	fclose(art);
	if (!(pgp_signed || pgp_key)) {
		info_message(txt_pgp_nothing);
		return 0;
	}
	ClearScreen();
	if (pgp_signed) {
		Raw(FALSE);

#	ifdef HAVE_PGP_2
		sprintf(cmd, "%s <%s %s %s -f", PGPNAME, the_article, REDIRECT_PGP_OUTPUT, pgpopts);
#	else
#		ifdef	HAVE_PGP_5
		sprintf(cmd, "%sv <%s %s %s -f", PGPNAME, the_article, REDIRECT_PGP_OUTPUT, pgpopts);
#		endif /* HAVE_PGP_5 */
#	endif /* HAVE_PGP_2 */

		system(cmd);
		my_printf("\n");
		Raw(TRUE);
	}
	if (pgp_key) { /* FIXME: -> lang.c */
		strcpy (buf, "Add key(s) to public keyring? ");
		if (prompt_yn (cLINES, buf, FALSE) == 1) {
			Raw (FALSE);

#	ifdef HAVE_PGP_2
			sprintf (cmd, "%s %s -ka %s", PGPNAME, pgpopts, the_article);
#	else
#		ifdef HAVE_PGP_5
			sprintf (cmd, "%sk %s -a %s", PGPNAME, pgpopts, the_article);
#		endif /* HAVE_PGP_5 */
#	endif /* HAVE_PGP_2 */

			system (cmd);
			my_printf ("\n");
			Raw (TRUE);
		}
	}

	continue_prompt ();
	unlink(the_article);
	return (1);
}
#endif /* HAVE_PGP */
