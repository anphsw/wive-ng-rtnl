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
#include <fcntl.h>
#include "pcm_ctrl.h"

pcm_record_type pcm_record;
char buffer[4096*1024];

void usage(char *cmd)
{
	printf("Usage: 0 - pcm dma start\n");
	printf("       1 - pcm dma stop\n");
	printf("       2 N (frames) I (CH) - number of frames (0~10000) for pcm record\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	FILE* fp_pcm;
	int pcm_fd, nframe, nLen, chid;
	int total = 0;
	if (argc < 2)
		usage(argv[0]);
	
	pcm_fd = open("dev/PCM", O_RDWR); 
	if(pcm_fd < 0)
    {
    	printf("open pcm driver failed (%d)...exit\n",pcm_fd);
    	return -1;
    }
    
	switch (argv[1][0]) {
	case '0':
		ioctl(pcm_fd, PCM_START, NULL);
		break;
	case '1':
		ioctl(pcm_fd, PCM_STOP, NULL);
		close(pcm_fd);
		break;
	case '2':
		if (argc < 4)
			usage(argv[0]);
		total = atoi(argv[2]);
		if((total < 0)||(total > 10000))
			usage(argv[0]);
		chid = atoi(argv[3]);
		if((chid < 0)||(chid > 2))
			usage(argv[0]);
						
		pcm_record.pcmbuf = buffer;
		if(pcm_record.pcmbuf<=0)
		{
			printf("mmap failed=%d\n",pcm_record.pcmbuf);
			return -1;
		}	
		fp_pcm = fopen("/mnt/record.pcm","wb");
	    if(fp_pcm==NULL)
	    {
	    	printf("open pcm file failed..exit\n");
	    	return -1;
	    }
		ioctl(pcm_fd, PCM_SET_RECORD, chid);
		nframe = 0;
		while(nframe < total)
		{
			ioctl(pcm_fd, PCM_READ_PCM, &pcm_record);
			if(pcm_record.size>0)
			{
				nLen = fwrite(pcm_record.pcmbuf, 1, pcm_record.size, fp_pcm);
				pcm_record.size = 0;
				nframe++;
			}
			sleep(0);
		}
		ioctl(pcm_fd, PCM_SET_UNRECORD, chid);
		fclose(fp_pcm);
		close(pcm_fd);
		break;
	default:
		{
		usage(argv[0]);
		close(pcm_fd);
		}
	}

	//close(pcm_fd);
	//printf("close pcm command\n");
	
	
    return 0;
}