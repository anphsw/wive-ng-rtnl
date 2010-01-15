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
/* pkread.c */

#include <stdio.h>
#include <stdlib.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>

/* Simple PKCS#12 file reader */

int main(int argc, char **argv)
{
	FILE *fp;
	EVP_PKEY *pkey;
	X509 *cert;
	STACK_OF(X509) *ca = NULL;
	PKCS12 *p12;
	int i;
	if (argc != 4) {
		fprintf(stderr, "Usage: pkread p12file password opfile\n");
		exit (1);
	}
	SSLeay_add_all_algorithms();
	ERR_load_crypto_strings();
	if (!(fp = fopen(argv[1], "rb"))) {
		fprintf(stderr, "Error opening file %s\n", argv[1]);
		exit(1);
	}
	p12 = d2i_PKCS12_fp(fp, NULL);
	fclose (fp);
	if (!p12) {
		fprintf(stderr, "Error reading PKCS#12 file\n");
		ERR_print_errors_fp(stderr);
		exit (1);
	}
	if (!PKCS12_parse(p12, argv[2], &pkey, &cert, &ca)) {
		fprintf(stderr, "Error parsing PKCS#12 file\n");
		ERR_print_errors_fp(stderr);
		exit (1);
	}
	PKCS12_free(p12);
	if (!(fp = fopen(argv[3], "w"))) {
		fprintf(stderr, "Error opening file %s\n", argv[1]);
		exit(1);
	}
	if (pkey) {
		fprintf(fp, "***Private Key***\n");
		PEM_write_PrivateKey(fp, pkey, NULL, NULL, 0, NULL, NULL);
	}
	if (cert) {
		fprintf(fp, "***User Certificate***\n");
		PEM_write_X509_AUX(fp, cert);
	}
	if (ca && sk_num(ca)) {
		fprintf(fp, "***Other Certificates***\n");
		for (i = 0; i < sk_X509_num(ca); i++) 
		    PEM_write_X509_AUX(fp, sk_X509_value(ca, i));
	}
	fclose(fp);
	return 0;
}
