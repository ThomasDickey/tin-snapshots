/*
 *  Project   : tin - a Usenet reader
 *  Module    : pgp.c
 *  Author    : Steven J. Madsen
 *  Created   : 12-05-95
 *  Updated   : 19-06-95
 *  Notes     : PGP support for article posting and mailing
 *  Copyright : (c) 1995 by Steven J. Madsen
 *              You may  freely  copy or  redistribute  this software,
 *              so  long as there is no profit made from its use, sale
 *              trade or  reproduction.  You may not change this copy-
 *              right notice, and it must be included in any copy made
 */

#include	"tin.h"

#ifdef HAVE_PGP

/* 
 * name of environment variable of options to pass to PGP 
 */
#define PGPNAME "pgp"
#define PGPOPTS "PGPOPTS"

#define HEADERS	"%stin-%d.h"
#define PLAINTEXT  "%stin-%d.pt"
#define CIPHERTEXT "%stin-%d.pt.asc"
#define KEYFILE	"%stin-%d.k.asc"

#define PGP_SIG_TAG "-----BEGIN PGP SIGNED MESSAGE-----\n"
#define PGP_KEY_TAG "-----BEGIN PGP PUBLIC KEY BLOCK-----\n"

#define SIGN 0x01
#define ENCRYPT 0x02

static char hdr[PATH_LEN], pt[PATH_LEN], ct[PATH_LEN];
char *pgpopts = (char *)NULL, pgp_data[PATH_LEN];

static void join_files P_((char *file));
static void split_file P_((char *file));
static void do_pgp P_((int what, char *file, char *mail_to));
static void pgp_append_public_key P_((char *file));
static int pgp_available P_((void));

static void
join_files (file)
	char *file;
{
	char buf[LEN];
	FILE *art, *header, *text;

	if ((header = fopen(hdr, "r")) == NULL) {
		return;
	}
	if ((text = fopen(ct, "r")) == NULL) {
		fclose(header);
		return;
	}
	if ((art = fopen(file, "w")) == NULL) {
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
split_file (file)
	char *file;
{
	int pid, mask;
	char buf[LEN];
	FILE *art, *header, *text;
	
	pid = getpid();
	sprintf(hdr, HEADERS, TMPDIR, pid);
	sprintf(pt, PLAINTEXT, TMPDIR, pid);
	sprintf(ct, CIPHERTEXT, TMPDIR, pid);
	mask = umask(077);
	if ((art = fopen(file, "r")) == NULL) {
		return;
	}
	if ((header = fopen(hdr, "w")) == NULL) {
		fclose(art);
		return;
	}
	if ((text = fopen(pt, "w")) == NULL) {
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
do_pgp (what, file, mail_to)
	int what;
	char *file;
	char *mail_to;
{
	char cmd[LEN], options[10];

	split_file(file);
	strcpy(options, "-at");
	if (what & ENCRYPT)
		strcat(options, "e");
	if (what & SIGN)
		strcat(options, "s");
	sprintf(cmd, "%s %s %s %s %s", PGPNAME, pgpopts, options, pt,
		mail_to ? mail_to : "");
	invoke_cmd(cmd);
	join_files(file);
	unlink(pt);
	unlink(hdr);
	unlink(ct);
}

static void 
pgp_append_public_key (file)
	char *file;
{
	FILE *f, *key;
	char keyfile[PATH_LEN], cmd[LEN], user[50], host[50], buf[LEN];

	get_host_name(host);
	sprintf(user, "%s@%s", userid, host);
	sprintf(keyfile, KEYFILE, TMPDIR, (char)getpid());
	sprintf(cmd, "%s %s -kxa %s %s", PGPNAME, pgpopts, user, keyfile);
	if (invoke_cmd(cmd)) {
		if ((f = fopen(file, "a")) == NULL)
			return;
		if ((key = fopen(keyfile, "r")) == NULL) {
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

static int 
pgp_available ()
{
	FILE *f;
	char keyring[PATH_LEN];

	joinpath(keyring, pgp_data, "pubring.pgp");
	if ((f = fopen(keyring, "r")) == NULL) {
		return (0);
	} else {
		fclose(f);
		return (1);
	}
}

void 
invoke_pgp_mail (nam, mail_to)
	char *nam;
	char *mail_to;
{
	char ch, ch_default = 's';
	
	if (!pgp_available()) {
		info_message(txt_pgp_not_avail);
		return;
	}
	do {
		sprintf(msg, "%s%c", txt_pgp_mail, ch_default);
		wait_message(msg);
		MoveCursor(cLINES, (int) strlen(txt_pgp_mail));
		if ((ch = (char) ReadCh()) == '\r' || ch == '\n')
			ch = ch_default;
	} while (! strchr("beqs\033", ch));
	switch (ch) {
		case 'b':
			do_pgp(SIGN | ENCRYPT, nam, mail_to);
			break;

		case 'e':
			do_pgp(ENCRYPT, nam, mail_to);
			break;

		case '\033':
		case 'q':
			break;

		case 's':
			do_pgp(SIGN, nam, NULL);
			break;
	}
}

void
invoke_pgp_news(the_article)
	char *the_article;
{
	char ch, ch_default = 's';
	
	if (!pgp_available()) {
		info_message(txt_pgp_not_avail);
		return;
	}
	do {
		sprintf(msg, "%s%c", txt_pgp_news, ch_default);
		wait_message(msg);
		MoveCursor(cLINES, (int) strlen(txt_pgp_news));
		if ((ch = (char) ReadCh()) == '\n' || ch == '\r')
			ch = ch_default;
	} while (! strchr("iqs\033", ch));
	switch (ch) {
		case '\033':
		case 'q':
			break;

		case 's':
			do_pgp(SIGN, the_article, NULL);
			break;

		case 'i':
			do_pgp(SIGN, the_article, NULL);
			pgp_append_public_key(the_article);
			break;
	}
}

int 
pgp_check_article()
{
	FILE *art;
	char the_article[PATH_LEN], buf[LEN], cmd[LEN], ch;
	int pgp_signed = 0, pgp_key = 0;

	if (!pgp_available()) {
		info_message(txt_pgp_not_avail);
		return (0);
	}
	joinpath(the_article, homedir, ".article");
	if ((art = fopen(article, "w")) == NULL) {
		sprintf(buf, txt_cannot_open, the_article);
		info_message(buf);
		return (0);
	}
	fseek(note_fp, note_mark[0], 0);
	fgets(buf, LEN, note_fp);
	while (!feof(note_fp)) {
		if (!strcmp(buf, PGP_SIG_TAG))
			pgp_signed = 1;
		if (!strcmp(buf, PGP_KEY_TAG))
			pgp_key = 1;
		fputs(buf, art);
		fgets(buf, LEN, note_fp);
	}
	fclose(art);
	if (!pgp_signed && !pgp_key) {
		info_message(txt_pgp_nothing);
		return (0);
	}
	set_alarm_clock_off();
	ClearScreen();
	if (pgp_signed) {
		Raw(FALSE);
		sprintf(cmd, "%s <%s %s %s -f", PGPNAME, the_article, REDIRECT_PGP_OUTPUT, pgpopts);
		system(cmd);
		printf("\n");
		Raw(TRUE);
	}
	if (pgp_key) {
		strcpy(buf, "Add key(s) to public keyring?  n");
		do {
			wait_message(buf);
			MoveCursor(cLINES, (int) strlen(buf) - 1);
			if ((ch = (char) ReadCh()) == '\r' || ch == '\n')
			ch = 'n';
		} while (! strchr("YyNn", ch));
		if (tolower(ch) == 'y') {
			Raw(FALSE);
			printf("Yes\n\n");
			sprintf(cmd, "%s %s -ka %s", PGPNAME, pgpopts, the_article);
			system(cmd);
			printf("\n");
			Raw(TRUE);
		} else {
			printf("No\n\n");
		}
	}
	continue_prompt();
	set_alarm_clock_on();
	return (1);
}
#endif /* HAVE_PGP */
