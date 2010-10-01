#include "upload.cgi.h"
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

#if 1
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
#endif

	munmap(ptr, len);
	close(ifd);

	return 1;
}


#endif /* UPLOAD_FIRMWARE_SUPPORT */

int main (int argc, char *argv[])
{
	int file_begin, file_end;
	int line_begin, line_end;
	char err_msg[256];
	char *boundary; int boundary_len;
	char *filename = getenv("UPLOAD_FILENAME");

	html_header();

	line_begin = 0;
	if ((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1)
	{
		html_error(RFC_ERROR);
		return -1;
	}

	boundary_len = line_end - line_begin;
	boundary = getMemInFile(filename, line_begin, boundary_len);

	// sth like this..
	// Content-Disposition: form-data; name="filename"; filename="\\192.168.3.171\tftpboot\a.out"
	//
	char *line, *semicolon, *user_filename;
	line_begin = line_end + 2;
	if ((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1)
	{
		html_error(RFC_ERROR);
		goto err;
	}

	line = getMemInFile(filename, line_begin, line_end - line_begin);
	if (strncasecmp(line, "content-disposition: form-data;", strlen("content-disposition: form-data;")))
	{
		html_error(RFC_ERROR);
		goto err;
	}

	semicolon = line + strlen("content-disposition: form-data;") + 1;
	if (!(semicolon = strchr(semicolon, ';')))
	{
		html_error("We dont support multi-field upload.");
		goto err;
	}

	user_filename = semicolon + 2;
	if (strncasecmp(user_filename, "filename=", strlen("filename=")))
	{
		html_error(RFC_ERROR);
		goto err;
	}

	user_filename += strlen("filename=");
	//until now we dont care about what the true filename is.
	free(line);

	// We may check a string  "Content-Type: application/octet-stream" here,
	// but if our firmware extension name is the same with other known ones, 
	// the browser would use other content-type instead.
	// So we dont check Content-type here...
	line_begin = line_end + 2;

	if ((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1)
	{
		html_error(RFC_ERROR);
		goto err;
	}

	line_begin = line_end + 2;
	if ((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1)
	{
		html_error(RFC_ERROR);
		goto err;
	}

	file_begin = line_end + 2;
	if ((file_end = findStrInFile(filename, file_begin, boundary, boundary_len)) == -1)
	{
		html_error(RFC_ERROR);
		goto err;
	}

	file_end -= 2;		// back 2 chars.(\r\n);

    // examination
#if defined (UPLOAD_FIRMWARE_SUPPORT)
	if (!check(filename, file_begin, file_end - file_begin, err_msg) )
	{
		html_error("Not a valid firmware.");
		goto err;
	}

	/*
	 * write the current linux version into flash.
	 */
	write_flash_kernel_version(filename, file_begin);

	// flash write
	if ( mtd_write_firmware(filename, file_begin, file_end - file_begin) == -1)
	{
		html_error("mtd_write fatal error! The corrupted image has ruined the flash!!");
		goto err;
	}

#elif defined (UPLOAD_BOOTLOADER_SUPPORT)
	mtd_write_bootloader(filename, file_begin, file_end - file_begin);
#else
#error "no upload support defined!"
#endif

	html_success("25000");
	free(boundary);
	fflush(stdout);
	fclose(stdout);

	system("sleep 3 && reboot &");
	exit(0);

err:
	free(boundary);
	exit(-1);
}

