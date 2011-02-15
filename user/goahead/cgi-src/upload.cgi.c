#include "upload.h"
#include "../options.h"

#if defined (UPLOAD_FIRMWARE_SUPPORT)

/*
 *  taken from "mkimage -l" with few modified....
 */
int check(char *imagefile, int offset, int len, char *err_msg)
{
	struct stat sbuf;

	int  data_len;
	char *data;
	unsigned char *ptr;
	unsigned long checksum;

	image_header_t header;
	image_header_t *hdr = &header;

	int ifd;

	if ((unsigned)len < sizeof(image_header_t)) {
		sprintf (err_msg, "Bad size: \"%s\" is no valid image\n", imagefile);
		return 0;
	}

	ifd = open(imagefile, O_RDONLY);
	if(!ifd){
		sprintf (err_msg, "Can't open %s: %s\n", imagefile, strerror(errno));
		return 0;
	}

	if (fstat(ifd, &sbuf) < 0) {
		close(ifd);
		sprintf (err_msg, "Can't stat %s: %s\n", imagefile, strerror(errno));
		return 0;
	}

	ptr = (unsigned char *) mmap(0, sbuf.st_size, PROT_READ, MAP_SHARED, ifd, 0);
	if ((caddr_t)ptr == (caddr_t)-1) {
		close(ifd);
		sprintf (err_msg, "Can't mmap %s: %s\n", imagefile, strerror(errno));
		return 0;
	}
	ptr += offset;

	/*
	 *  handle Header CRC32
	 */
	memcpy (hdr, ptr, sizeof(image_header_t));

	if (ntohl(hdr->ih_magic) != IH_MAGIC)
	{
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "Bad Magic Number: \"%s\" is no valid image\n", imagefile);
		return 0;
	}

	data = (char *)hdr;

	checksum = ntohl(hdr->ih_hcrc);
	hdr->ih_hcrc = htonl(0);	/* clear for re-calculation */

	if (crc32 (0, data, sizeof(image_header_t)) != checksum)
	{
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "*** Warning: \"%s\" has bad header checksum!\n", imagefile);
		return 0;
	}

	/*
	 *  handle Data CRC32
	 */
	data = (char *)(ptr + sizeof(image_header_t));
	data_len  = len - sizeof(image_header_t) ;

	if (crc32 (0, data, data_len) != ntohl(hdr->ih_dcrc))
	{
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "*** Warning: \"%s\" has corrupted data!\n", imagefile);
		return 0;
	}

	/*
	 * compare MTD partition size and image size
	 */
#if defined (CONFIG_RT2880_ROOTFS_IN_RAM)
	if(len > getMTDPartSize("\"Kernel\"")){
		munmap(ptr, len);
		close(ifd);
		sprintf(err_msg, "*** Warning: the image file(0x%x) is bigger than Kernel MTD partition.\n", len);
		return 0;
	}
#elif defined (CONFIG_RT2880_ROOTFS_IN_FLASH)
  #ifdef CONFIG_ROOTFS_IN_FLASH_NO_PADDING
	if(len > getMTDPartSize("\"Kernel_RootFS\"")){
		munmap(ptr, len);
		close(ifd);
		sprintf(err_msg, "*** Warning: the image file(0x%x) is bigger than Kernel_RootFS MTD partition.\n", len);
		return 0;
	}
  #else
	if(len < CONFIG_MTD_KERNEL_PART_SIZ){
		munmap(ptr, len);
		close(ifd);
		sprintf(err_msg, "*** Warning: the image file(0x%x) size doesn't make sense.\n", len);
		return 0;
	}

	if((len - CONFIG_MTD_KERNEL_PART_SIZ) > getMTDPartSize("\"RootFS\"")){
		munmap(ptr, len);
		close(ifd);
		sprintf(err_msg, "*** Warning: the image file(0x%x) is bigger than RootFS MTD partition.\n", len - CONFIG_MTD_KERNEL_PART_SIZ);
		return 0;
	}
  #endif
#else
#error "goahead: no CONFIG_RT2880_ROOTFS defined!"
#endif
	munmap(ptr, len);
	close(ifd);

	return 1;
}

#endif /* UPLOAD_FIRMWARE_SUPPORT */

int main (int argc, char *argv[])
{
	char err_msg[256];
	long file_size = 0;
	long file_begin = 0, file_end = 0;
	
	// Get multipart file data separator
	char separator[MAX_SEPARATOR_LEN];
	
	html_header();
	
	if (get_content_separator(separator, sizeof(separator), &file_size) < 0)
	{
		html_error(RFC_ERROR);
		return -1;
	}
	
	// Get multipart file name
	char *filename = getenv("UPLOAD_FILENAME");
	if (filename == NULL)
	{
		html_error(RFC_ERROR);
		return -1;
	}
	
	// Wait until file is completely uploaded
	int tries = 0;
	while (tries>5)
	{
		struct stat filestat;
		if (stat(filename, &filestat)>0)
		{
			if (filestat.st_size >= file_size) // Size ok?
				break;
		}
		
		sleep(1000);
		tries++;
	}
	
	// Open file
	FILE *fd = fopen(filename, "r");
	if (fd == NULL)
	{
		html_error(RFC_ERROR);
		return -1;
	}
	
	// Parse parameters
	parameter_t *params;
	if (read_parameters(fd, separator, &params)<0)
	{
		fclose(fd);
		html_error(RFC_ERROR);
		return -1;
	}
	
	fclose(fd);
	
	// Find parameter containing NVRAM reset flag
	parameter_t *find = find_parameter(params, "reset_rwfs");
	int reset_rwfs = 0;
	if (find != NULL)
	{
		if (find->value != NULL)
			reset_rwfs = (strcasecmp(find->value, "on")==0);
	}

	// Find parameter containing file
	find = find_parameter(params, "filename");
	if (find != NULL)
	{
		// Check if parameter is correct
		if (find->content_type == NULL)
		{
			html_error(RFC_ERROR);
			return -1;
		}
		if (!check_binary_content_type(find->content_type))
		{
			sprintf(err_msg, "Unsupported content-type for binary data: %s", find->content_type);
			html_error(err_msg);
			return -1;
		}

		file_begin = find->start_pos;
		file_end   = find->end_pos;
	}
	else
	{
		html_error("No firmware binary file");
		return -1;
	}

	release_parameters(params);
	
	sync();

	// examination
#if defined (UPLOAD_FIRMWARE_SUPPORT)

	if (!check(filename, (int)file_begin, (int)(file_end - file_begin), err_msg) )
	{
		html_error("Not a valid firmware.");
		return -1;
	}
	
	// flash write
	if ( mtd_write_firmware(filename, (int)file_begin, (file_end - file_begin) == -1))
	{
		html_error("mtd_write fatal error! The corrupted image has ruined the flash!!");
		return -1;
	}
	
#elif defined (UPLOAD_BOOTLOADER_SUPPORT)
	mtd_write_bootloader(filename, (int)file_begin, (int)(file_end - file_begin));
#else
#error "no upload support defined!"
#endif
	
	// Reset NVRAM if needed
	if (reset_rwfs)
	{
		html_success("100000");
		fflush(stdout);
		system("echo \"1234567890\" > /dev/mtdblock5");
	}
	else
	{
		html_success("65000");
		fflush(stdout);
	}

	system("sleep 5 && reboot &");
	return 0;
}

