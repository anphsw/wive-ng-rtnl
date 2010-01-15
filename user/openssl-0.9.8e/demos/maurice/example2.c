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
/* NOCW */
/*
        Please read the README file for condition of use, before
        using this software.

        Maurice Gittens  <mgittens@gits.nl>   January 1997
*/

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

#include "loadkeys.h"

#define PUBFILE   "cert.pem"
#define PRIVFILE  "privkey.pem"
#define STDIN     0
#define STDOUT    1 

int main()
{
        char *ct = "This the clear text";
	char *buf;   
	char *buf2;
  	EVP_PKEY *pubKey;
  	EVP_PKEY *privKey;
	int len;

        ERR_load_crypto_strings();

        privKey = ReadPrivateKey(PRIVFILE);
        if (!privKey) 
	{  
		ERR_print_errors_fp (stderr);    
		exit (1);  
	}

        pubKey = ReadPublicKey(PUBFILE);  
	if(!pubKey)
	{
	   EVP_PKEY_free(privKey);   
           fprintf(stderr,"Error: can't load public key");
	   exit(1);
	}

	/* No error checking */
        buf = malloc(EVP_PKEY_size(pubKey));
        buf2 = malloc(EVP_PKEY_size(pubKey));

	len = RSA_public_encrypt(strlen(ct)+1, ct, buf, pubKey->pkey.rsa,RSA_PKCS1_PADDING);

	if (len != EVP_PKEY_size(pubKey))
	{
	    fprintf(stderr,"Error: ciphertext should match length of key\n");
	    exit(1);
	}

	RSA_private_decrypt(len, buf, buf2, privKey->pkey.rsa,RSA_PKCS1_PADDING);

	printf("%s\n", buf2);

	EVP_PKEY_free(privKey);
	EVP_PKEY_free(pubKey);
	free(buf);
	free(buf2);
        return 0;
}
