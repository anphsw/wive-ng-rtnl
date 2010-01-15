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
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <fcntl.h>
#include "i2s_ctrl.h"

int i2s_fd;
void *shbuf;
struct stat i2s_stat;

void usage(char *cmd)
{
	printf("Usage: [cmd] [srate] [vol] < playback file\n");
	printf("       cmd = 0|1 - i2s raw|mp3 playback\n");
	printf("       srate = 8000|16000|32000|44100|48000 Hz playback sampling rate\n");
	printf("       vol = -10~2 db playback volumn\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	FILE* fa;
	void *fdm;
	char* pBuf;
	int pos;
	int nRet=0,nLen,i=0;
	int index = 0;
	
    printf("\n\rThis is Ralink I2S Command Program...\n");
       
    if (fstat(STDIN_FILENO, &i2s_stat) == -1 ||i2s_stat.st_size == 0)
		return -1;
		
    i2s_fd = open("dev/I2S", O_RDWR|O_SYNC); 
    if(i2s_fd<0)
    {
    	printf("i2scmd:open i2s driver failed (%d)...exit\n",i2s_fd);
    	return -1;
    }

	fdm = mmap(0, i2s_stat.st_size, PROT_READ, MAP_SHARED, STDIN_FILENO, 0);
	if (fdm == MAP_FAILED)
		return -1;
		
    shbuf = mmap(0, I2S_PAGE_SIZE*MAX_I2S_PAGE, PROT_WRITE, MAP_SHARED, i2s_fd, 0);
	if (shbuf == MAP_FAILED)
	{
		printf("i2scmd:failed to mmap..\n");
		return -1;
	}
	
    switch(argv[1][0])
    {
    case '0':
	    ioctl(i2s_fd, I2S_SRATE, strtoul(argv[2], NULL ,10));
    	ioctl(i2s_fd, I2S_VOL, strtoul(argv[3], NULL ,10));
    	ioctl(i2s_fd, I2S_ENABLE, 0);
    	pos = 0;

    	while((pos+I2S_PAGE_SIZE)<=i2s_stat.st_size)
    	{
    		ioctl(i2s_fd, I2S_GET_WBUF, &index);
    		pBuf = (char*)shbuf + index*I2S_PAGE_SIZE;
    		memcpy(pBuf, (char*)fdm+pos, I2S_PAGE_SIZE);    		
    		pos+=I2S_PAGE_SIZE;	
    	}

    	ioctl(i2s_fd, I2S_DISABLE, 0);
    	break;	
    case '1':		
    	//mad_main();
    	break;
    default:
    	break;	
    }
  

EXIT:
 	munmap(fdm, i2s_stat.st_size);
 	munmap(i2s_fd, I2S_PAGE_SIZE*MAX_I2S_PAGE);	 
   	close(i2s_fd);

    printf("i2scmd ...quit\n");
    return 0;
}
