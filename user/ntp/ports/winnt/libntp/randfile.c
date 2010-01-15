/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/*
 * Make sure that there is a good source of random characters
 * so that OpenSSL can work properly and securely.
 */

/* Skip asynch rpc inclusion */
#ifndef __RPCASYNC_H__
#define __RPCASYNC_H__
#endif

#include <windows.h>
#include <wincrypt.h>

#include <stdio.h>

unsigned int	getrandom_chars(int desired, unsigned char *buf, int lenbuf);
BOOL		create_random_file(char *filename);

BOOL
init_randfile()
{
	FILE *rf;
	char *randfile;
	char *homedir;
	char tmp[256];
	/* See if the environmental variable RANDFILE is defined
	 * and the file exists
	 */
	randfile = getenv("RANDFILE");
	if (randfile != NULL) {
		rf = fopen(randfile, "rb");
		if (rf != NULL) {
			fclose(rf);
			return (TRUE);
		}
		else {
			/* The environmental variable exists but not the file */
			return (create_random_file(randfile));
		}
	}
	/*
	 * If the RANDFILE environmental variable does not exist,
	 * see if the HOME enviromental variable exists and
	 * a .rnd file is in there.
	 */
	homedir = getenv("HOME");
	if (homedir != NULL) {
		strcpy(tmp, homedir);
		strcat(tmp, "\\.rnd");
		rf = fopen(tmp, "rb");
		if (rf != NULL) {
			fclose(rf);
			return (TRUE);
		}
		else {
			/* The HOME environmental variable exists but not the file */
			return (create_random_file(tmp));
		}
	}
	/*
	 * Final try. Look for it on the C:\ directory
	 * NOTE: This is a really bad place for it security-wise
	 * However, OpenSSL looks for it there if it can't find it elsewhere
	 */
	rf = fopen("C:\\.rnd", "rb");
	if (rf != NULL) {
		fclose(rf);
		return (TRUE);
	}
	/* The file does not exist */
	return (create_random_file("C:\\.rnd"));
}
/*
 * Routine to create the random file with 1024 random characters
 */
BOOL
create_random_file(char *filename) {
	FILE *rf;
	int nchars;
	unsigned char buf[1025];

	nchars = getrandom_chars(1024, buf, sizeof(buf));
	rf = fopen(filename, "wb");
	if (rf == NULL)
		return (FALSE);
	fwrite(buf, sizeof(unsigned char), nchars, rf);
	fclose(rf);
	return (TRUE);
}

unsigned int
getrandom_chars(int desired, unsigned char *buf, int lenbuf) {
	HCRYPTPROV hcryptprov;
	BOOL err;

	if (buf == NULL || lenbuf <= 0 || desired > lenbuf)
		return (0);
	/*
	 * The first time we just try to acquire the context
	 */
	err = CryptAcquireContext(&hcryptprov, NULL, NULL, PROV_RSA_FULL,
				  CRYPT_VERIFYCONTEXT);
	if (!err){
		return (0);
	}
	if (!CryptGenRandom(hcryptprov, desired, buf)) {
		CryptReleaseContext(hcryptprov, 0);
		return (0);
	}

	CryptReleaseContext(hcryptprov, 0);
	return (desired);
}

