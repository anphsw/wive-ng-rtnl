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

#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <strings.h>
#include <stdlib.h>

#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

EVP_PKEY * ReadPublicKey(const char *certfile)
{
  FILE *fp = fopen (certfile, "r");   
  X509 *x509;
  EVP_PKEY *pkey;

  if (!fp) 
     return NULL; 

  x509 = PEM_read_X509(fp, NULL, 0, NULL);

  if (x509 == NULL) 
  {  
     ERR_print_errors_fp (stderr);
     return NULL;   
  }

  fclose (fp);
  
  pkey=X509_extract_key(x509);

  X509_free(x509);

  if (pkey == NULL) 
     ERR_print_errors_fp (stderr);

  return pkey; 
}

EVP_PKEY *ReadPrivateKey(const char *keyfile)
{
	FILE *fp = fopen(keyfile, "r");
	EVP_PKEY *pkey;

	if (!fp)
		return NULL;

	pkey = PEM_read_PrivateKey(fp, NULL, 0, NULL);

	fclose (fp);

  	if (pkey == NULL) 
		ERR_print_errors_fp (stderr);   

	return pkey;
}


