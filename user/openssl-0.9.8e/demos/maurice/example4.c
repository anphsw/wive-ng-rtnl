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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <openssl/evp.h>

#define STDIN     	0
#define STDOUT    	1
#define BUFLEN	  	512 

static const char *usage = "Usage: example4 [-d]\n";

void do_encode(void);
void do_decode(void);

int main(int argc, char *argv[])
{
	if ((argc == 1))	
	{
		do_encode();
	}	
	else if ((argc == 2) && !strcmp(argv[1],"-d"))
	{
		do_decode();
	}
	else
	{
		fprintf(stderr,"%s", usage);
		exit(1);
	}

	return 0;		
}

void do_encode()
{
	char buf[BUFLEN];
	char ebuf[BUFLEN+24];
	unsigned int ebuflen;
	EVP_ENCODE_CTX ectx;
        
	EVP_EncodeInit(&ectx);

	while(1)
	{
		int readlen = read(STDIN, buf, sizeof(buf));
	
		if (readlen <= 0)
		{
			if (!readlen)
			   break;
			else
			{
				perror("read");
				exit(1);
			}
		}

		EVP_EncodeUpdate(&ectx, ebuf, &ebuflen, buf, readlen);

		write(STDOUT, ebuf, ebuflen);
	}

        EVP_EncodeFinal(&ectx, ebuf, &ebuflen); 

	write(STDOUT, ebuf, ebuflen);
}

void do_decode()
{
 	char buf[BUFLEN];
 	char ebuf[BUFLEN+24];
	unsigned int ebuflen;
	EVP_ENCODE_CTX ectx;
        
	EVP_DecodeInit(&ectx);

	while(1)
	{
		int readlen = read(STDIN, buf, sizeof(buf));
		int rc;	
	
		if (readlen <= 0)
		{
			if (!readlen)
			   break;
			else
			{
				perror("read");
				exit(1);
			}
		}

		rc = EVP_DecodeUpdate(&ectx, ebuf, &ebuflen, buf, readlen);
		if (rc <= 0)
		{
			if (!rc)
			{
				write(STDOUT, ebuf, ebuflen);
				break;
			}

			fprintf(stderr, "Error: decoding message\n");
			return;
		}

		write(STDOUT, ebuf, ebuflen);
	}

        EVP_DecodeFinal(&ectx, ebuf, &ebuflen); 

	write(STDOUT, ebuf, ebuflen); 
}

