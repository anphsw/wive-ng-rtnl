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
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <limits.h>
#include <shutils.h>	// for eval()
#include <nvram/bcmnvram.h>

#include "disk_io_tools.h"
#include "disk_initial.h"

extern disk_info_t *read_disk_data(){
	disk_info_t *disk_info_list = NULL, *new_disk_info, **follow_disk_info_list;
	FILE *fp = fopen(PARTITION_FILE, "r");
	char line[32], device_path[64], device_name[64];
	u32 major, minor, device_order, partition_number;
	u64 size_in_kilobytes;
	disk_info_t *parent_disk_info;
	partition_info_t *new_partition_info;
	
	if(fp == NULL){
		csprintf("Failed to open \"%s\"!!\n", PARTITION_FILE);
		return disk_info_list;
	}
	
	memset(line, 0, 32);
	if(fgets(line, 32, fp) == NULL){
		csprintf("Failed to read \"%s\"!!\n", PARTITION_FILE);
		fclose(fp);
		return disk_info_list;
	}

	if(strncmp(line, PROC_PARTITIONS_HEADER, strlen(PROC_PARTITIONS_HEADER))){
		csprintf("\"%s\" is incorrect: invalid header.\n", PARTITION_FILE);
		fclose(fp);
		return disk_info_list;
	}
	fgets(line, 32, fp);
	
	memset(device_name, 0, 64);
	while(fgets(line, 32, fp) != NULL){
		if(sscanf(line, " %u %u %llu %[^\n ]", &major, &minor, &size_in_kilobytes, device_name) != 4)
			continue;
		if(major != 22 && major != 8)	// Harddisk's major = 22, USBdisk's major = 8.
			continue;
		
		if(!isdigit(device_name[strlen(device_name)-1])){	// Disk
			printf("case A (no digit)\n");
			// Found a disk device.
			memset(device_path, 0, 64);
			sprintf(device_path, "/dev/%s", device_name);
			
			// start get disk information
			new_disk_info = (disk_info_t *)malloc(sizeof(disk_info_t));
			if(new_disk_info == NULL){
				csprintf("No memory!!(new_disk_info)\n");
				//printf("new disk info is NULL\n");	// tmp test
				fclose(fp);
				return disk_info_list;
			}
			
			int device_path_length = strlen(device_path);
			new_disk_info->device = (char *)malloc(sizeof(char)*device_path_length+1);
			if(new_disk_info->device == NULL){
				csprintf("No memory!!(new_disk_info->device)\n");
				free(new_disk_info);
				fclose(fp);
				return disk_info_list;
			}
			strcpy(new_disk_info->device, device_path);
			new_disk_info->device[device_path_length] = 0;
			
			FILE *info_fp;
			char *ide_model_file, ide_model[64];
			int len;
			
			device_order = 0;
			if(!strcmp(device_name, "hdd")){
				new_disk_info->vendor = NULL;

				//printf("dev name is hdd\n");	// tmp test
				
				len = strlen(IDE_INFO_DIR)+strlen("//model")+strlen(device_name);
				ide_model_file = (char *)malloc(sizeof(char)*len+1);
				if(ide_model_file == NULL){
					csprintf("No memory!!(ide_model_file)\n");
					free(new_disk_info);
					fclose(fp);
					return disk_info_list;
				}
				sprintf(ide_model_file, "%s/%s/model", IDE_INFO_DIR, device_name);
				ide_model_file[len] = 0;
				
				info_fp = fopen(ide_model_file, "r");
				memset(ide_model, 0, 64);
				if(info_fp != NULL && fgets(ide_model, 64, info_fp) != NULL){
					len = strlen(ide_model)-1;	// del the char "\n".
					new_disk_info->model = (char *)malloc(sizeof(char)*len+1);
					if(new_disk_info->model == NULL){
						csprintf("No memory!!(new_disk_info->model)\n");
						free(new_disk_info);
						fclose(fp);
						return disk_info_list;
					}
					strncpy(new_disk_info->model, ide_model, len);
					new_disk_info->model[len] = 0;
					
					new_disk_info->tag = (char *)malloc(sizeof(char)*len+1);
					if(new_disk_info->tag == NULL){
						csprintf("No memory!!(new_disk_info->tag)\n");
						free(new_disk_info);
						fclose(fp);
						return disk_info_list;
					}
					strncpy(new_disk_info->tag, ide_model, len);
					new_disk_info->tag[len] = 0;
				}
				else{
					new_disk_info->model = NULL;
					
					len = strlen(DEFAULT_SATE_TAG);
					new_disk_info->tag = (char *)malloc(sizeof(char)*len+1);
					if(new_disk_info->tag == NULL){
						csprintf("No memory!!(new_disk_info->tag)\n");
						free(new_disk_info);
						fclose(fp);
						return disk_info_list;
					}
					strcpy(new_disk_info->tag, DEFAULT_SATE_TAG);
					new_disk_info->tag[len] = 0;
				}
				
				len = strlen(DEFAULT_SATE_PORT);
				new_disk_info->port = (char *)malloc(sizeof(char)*len+1);
				if(new_disk_info->port == NULL){
					csprintf("No memory!!(new_disk_info->port)\n");
					free(new_disk_info);
					fclose(fp);
					return disk_info_list;
				}
				
				strcpy(new_disk_info->port, DEFAULT_SATE_PORT);
				new_disk_info->port[len] = 0;
			}
			else if(!strcmp(device_name, "hdc")){
				new_disk_info->vendor = NULL;
				
				//printf("dev name is hdc\n");	// tmp test
				len = strlen(IDE_INFO_DIR)+strlen("//model")+strlen(device_name);
				ide_model_file = (char *)malloc(sizeof(char)*len+1);
				if(ide_model_file == NULL){
					csprintf("No memory!!(ide_model_file)\n");
					free(new_disk_info);
					fclose(fp);
					return disk_info_list;
				}
				sprintf(ide_model_file, "%s/%s/model", IDE_INFO_DIR, device_name);
				ide_model_file[len] = 0;
				
				info_fp = fopen(ide_model_file, "r");
				memset(ide_model, 0, 64);
				if(info_fp != NULL && fgets(ide_model, 64, info_fp) != NULL){
					len = strlen(ide_model)-1;	// del the char "\n".
					new_disk_info->model = (char *)malloc(sizeof(char)*len+1);
					if(new_disk_info->model == NULL){
						csprintf("No memory!!(new_disk_info->model)\n");
						free(new_disk_info);
						fclose(fp);
						return disk_info_list;
					}
					strncpy(new_disk_info->model, ide_model, len);
					new_disk_info->model[len] = 0;
					
					new_disk_info->tag = (char *)malloc(sizeof(char)*len+1);
					if(new_disk_info->tag == NULL){
						csprintf("No memory!!(new_disk_info->tag)\n");
						free(new_disk_info);
						fclose(fp);
						return disk_info_list;
					}
					strncpy(new_disk_info->tag, ide_model, len);
					new_disk_info->tag[len] = 0;
				}
				else{
					new_disk_info->model = NULL;
					
					len = strlen(DEFAULT_E_SATE_TAG);
					new_disk_info->tag = (char *)malloc(sizeof(char)*len+1);
					if(new_disk_info->tag == NULL){
						csprintf("No memory!!(new_disk_info->tag)\n");
						free(new_disk_info);
						fclose(fp);
						return disk_info_list;
					}
					strcpy(new_disk_info->tag, DEFAULT_E_SATE_TAG);
					new_disk_info->tag[len] = 0;
				}
				
				len = strlen(DEFAULT_E_SATE_PORT);
				new_disk_info->port = (char *)malloc(sizeof(char)*len+1);
				if(new_disk_info->port == NULL){
					csprintf("No memory!!(new_disk_info->port)\n");
					free(new_disk_info);
					fclose(fp);
					return disk_info_list;
				}
				
				strcpy(new_disk_info->port, DEFAULT_E_SATE_PORT);
				new_disk_info->port[len] = 0;
			}
			else if(!strncmp(device_name, "sd", 2)){
				// get the USB device order
				char *SCSI_FILE_INFO, *follow_info, target[32];
				
				//printf("dev name is sd x\n");	// tmp test
				memset(target, 0, 32);
				sprintf(target, "Diskname: %s Host: ", device_name);
				
				SCSI_FILE_INFO = read_whole_file(SCSI_FILE);
				if(SCSI_FILE_INFO == NULL){
					csprintf("Failed to open \"%s\"!!\n", SCSI_FILE);
					free(new_disk_info);
					fclose(fp);
					return disk_info_list;
				}
				
				if((follow_info = strstr(SCSI_FILE_INFO, target)) == NULL){
					//printf("sd fail 1\n");	// tmp test
					csprintf("There was not \"%s\" in \"%s\"!!\n", device_name, SCSI_FILE);
					free(SCSI_FILE_INFO);
					free(new_disk_info);
					fclose(fp);
					return disk_info_list;
				}
				follow_info += strlen(target)+4;	// 4 = strlen("scsi")
				
				if(sscanf(follow_info, "%u", &device_order) != 1){
					//printf("sd fail 2\n");	// tmp test
					csprintf("Couldn't get the device order of %s!!\n", device_path);
					free(SCSI_FILE_INFO);
					free(new_disk_info);
					fclose(fp);
					return disk_info_list;
				}
				
				// start get model
				char *vendor_follow, *model_follow, *rev_follow;
				char *vendor, *model;
				u32 vendor_len, model_len;
				
				vendor_follow = strstr(follow_info, "Vendor: ");
				if(vendor_follow == NULL){
					csprintf("\"%s\" is incorrect!\n", SCSI_FILE);
					//printf("sd fail 3\n");	// tmp test
					free(SCSI_FILE_INFO);
					free(new_disk_info);
					fclose(fp);
					return disk_info_list;
				}
				model_follow = strstr(vendor_follow, " Model: ");
				if(model_follow == NULL){
					csprintf("\"%s\" is incorrect!\n", SCSI_FILE);
					//printf("sd fail 4\n");	// tmp test
					free(SCSI_FILE_INFO);
					free(new_disk_info);
					fclose(fp);
					return disk_info_list;
				}
				rev_follow = strstr(model_follow, " Rev: ");
				if(rev_follow == NULL){
					//printf("sd fail 5\n");	// tmp test
					csprintf("\"%s\" is incorrect!\n", SCSI_FILE);
					free(SCSI_FILE_INFO);
					free(new_disk_info);
					fclose(fp);
					return disk_info_list;
				}
				vendor_follow += strlen("Vendor: ");
				model_follow += strlen(" Model: ");
				rev_follow += strlen(" Rev: ");
				
				vendor_len = model_follow-vendor_follow-strlen(" Model: ");
				if(vendor_len > 0){
					//printf("sd 1\n");	// tmp test
					vendor = (char *)malloc(sizeof(char)*vendor_len+1);
					if(vendor == NULL){
						csprintf("No memory!!(vendor)\n");
						free(SCSI_FILE_INFO);
						free(new_disk_info);
						fclose(fp);
						return disk_info_list;
					}
					strncpy(vendor, vendor_follow, vendor_len);
					vendor[vendor_len] = 0;
					strntrim(vendor, vendor_len);
				}
				else
				{
					//printf("sd vendor is NULL\n");	// tmp test
					vendor = NULL;
				}
				new_disk_info->vendor = vendor;
				
				model_len = rev_follow-model_follow-strlen(" Rev: ");
				if(model_len > 0){
					model = (char *)malloc(sizeof(char)*model_len+1);
					if(model == NULL){
						csprintf("No memory!!(model)\n");
						free(SCSI_FILE_INFO);
						free(new_disk_info);
						fclose(fp);
						return disk_info_list;
					}
					strncpy(model, model_follow, model_len);
					model[model_len] = 0;
					strntrim(model, model_len);
				}
				else
				{
					//printf("sd model is NULL\n");	// tmp test
					model = NULL;
				}
				new_disk_info->model = model;
				
				free(SCSI_FILE_INFO);
				
				// get USB's tag
				new_disk_info->port = find_usb_port(device_order);
				int len;
				
				//printf("get usb tag\n");	// tmp test
				if(model != NULL && (model_len = strlen(model)) > 0){
					//printf("sd get tag 0\n");	// tmp test
					new_disk_info->tag = (char *)malloc(sizeof(char)*model_len+1);
					if(new_disk_info->tag == NULL){
						csprintf("No memory!!(new_disk_info->tag)\n");
						free(new_disk_info);
						fclose(fp);
						return disk_info_list;
					}
					strcpy(new_disk_info->tag, model);
					new_disk_info->tag[model_len] = 0;
				}
				else if(!strcmp(new_disk_info->port, "1")){
					len = strlen(DEFAULT_FRONT_USB_TAG);
					
					//printf("sd get tag 1\n");	// tmp test
					new_disk_info->tag = (char *)malloc(sizeof(char)*len+1);
					if(new_disk_info->tag == NULL){
						csprintf("No memory!!(new_disk_info->tag)\n");
						free(new_disk_info);
						fclose(fp);
						return disk_info_list;
					}
					strcpy(new_disk_info->tag, DEFAULT_FRONT_USB_TAG);
					new_disk_info->tag[len] = 0;
				}
				else if(!strcmp(new_disk_info->port, "1.1")){
					len = strlen(DEFAULT_BACK_TOP_USB_TAG);
					
					//printf("sd get tag 2\n");	// tmp test
					new_disk_info->tag = (char *)malloc(sizeof(char)*len+1);
					if(new_disk_info->tag == NULL){
						csprintf("No memory!!(new_disk_info->tag)\n");
						free(new_disk_info);
						fclose(fp);
						return disk_info_list;
					}
					strcpy(new_disk_info->tag, DEFAULT_BACK_TOP_USB_TAG);
					new_disk_info->tag[len] = 0;
				}
				else if(!strcmp(new_disk_info->port, "1.2")){
					len = strlen(DEFAULT_BACK_FOOT_USB_TAG);
					
					//printf("sd get tag 3\n");	// tmp test
					new_disk_info->tag = (char *)malloc(sizeof(char)*len+1);
					if(new_disk_info->tag == NULL){
						csprintf("No memory!!(new_disk_info->tag)\n");
						free(new_disk_info);
						fclose(fp);
						return disk_info_list;
					}
					strcpy(new_disk_info->tag, DEFAULT_BACK_FOOT_USB_TAG);
					new_disk_info->tag[len] = 0;
				}
				else{
					len = strlen(DEFAULT_USB_TAG);
					
					//printf("sd get tag 4\n");	// tmp test
					new_disk_info->tag = (char *)malloc(sizeof(char)*len+1);
					if(new_disk_info->tag == NULL){
						csprintf("No memory!!(new_disk_info->tag)\n");
						free(new_disk_info);
						fclose(fp);
						return disk_info_list;
					}
					strcpy(new_disk_info->tag, DEFAULT_USB_TAG);
					new_disk_info->tag[len] = 0;
				}
			}
			
			new_disk_info->major = major;
			new_disk_info->minor = minor;
			new_disk_info->mounted_number = 0;
			new_disk_info->device_order = device_order;
			new_disk_info->size_in_kilobytes = size_in_kilobytes;
			new_disk_info->partitions = NULL;
			new_disk_info->next = NULL;
			
			if(!strncmp(device_name, "sd", 2)){
				//printf("read test 1\n");	// tmp test
				new_partition_info = create_record_for_existing_partition(
						new_disk_info, device_name, size_in_kilobytes, device_order, 1);
/* chk dead
				//printf("chk used: %llu\n", new_partition_info->used_kilobytes);	// tmp test
				char chkbuf[32];
				memset(chkbuf, 0, sizeof(chkbuf));
				sprintf(chkbuf, "%.llu", new_partition_info->used_kilobytes);
				printf("sprintf pass:\n");	// tmp test
				printf("chk used %s\n", chkbuf);	// tmp test
*/
				if(new_partition_info == NULL)
					new_disk_info->partitions = NULL;
			}
			
			follow_disk_info_list = &disk_info_list;
			while(*follow_disk_info_list != NULL)
				follow_disk_info_list = &((*follow_disk_info_list)->next);
			*follow_disk_info_list = new_disk_info;
		}
		else if(is_partition_name(device_name, &partition_number) == 1){	// Partition
			// Found a partition device.
			// Find the parent disk.
			//printf("is partition name: %s\n", device_name);	// tmp test
			parent_disk_info = disk_info_list;
			while(1){
				const char *disk_device;
				
				if(parent_disk_info == NULL){
					csprintf("Error while parsing %s: found "
									"partition '%s' but haven't seen the disk device "
									"of which it is a part.\n", PARTITION_FILE, device_name);
					fclose(fp);
					return disk_info_list;
				}
				
				disk_device = parent_disk_info->device;
				if(!strncmp(device_name, disk_device+5, 3)){
					device_order = parent_disk_info->device_order;
					break;
				}
				parent_disk_info = parent_disk_info->next;
			}
			new_partition_info = create_record_for_existing_partition(
					parent_disk_info, device_name, size_in_kilobytes, device_order, partition_number);
			if(new_partition_info == NULL){
				csprintf("Can't read the partition's information out!!");
				fclose(fp);
				return disk_info_list;
			}
		}
	}
	
	fclose(fp);
	//printf("read disk end\n");	// tmp test
	return disk_info_list;
}

extern void free_disk_data(disk_info_t **disk_info_list){
	disk_info_t *follow_disk;
	
	follow_disk = *disk_info_list;
	while(follow_disk != NULL){
		disk_info_t *old_disk = follow_disk;
		partition_info_t *follow_partition;
		
		free(old_disk->tag);
		if(follow_disk->vendor != NULL && strlen(follow_disk->vendor) > 0)
			free(old_disk->vendor);
		if(follow_disk->model != NULL && strlen(follow_disk->model) > 0)
			free(old_disk->model);
		free(follow_disk->device);
		free(follow_disk->port);
		
		follow_partition = follow_disk->partitions;
		while(follow_partition != NULL){
			partition_info_t *old_partition = follow_partition;
			
			free(follow_partition->device);
			if(follow_partition->mount_point != NULL && strlen(follow_partition->mount_point) > 0)
				free(follow_partition->mount_point);
			if(follow_partition->file_system != NULL && strlen(follow_partition->file_system) > 0 && strcmp(follow_partition->file_system, "unknown"))
				free(follow_partition->file_system);
			
			follow_partition = follow_partition->next;
			free(old_partition);
		}
		
		follow_disk = follow_disk->next;
		free(old_disk);
	}
	
	*disk_info_list = NULL;
}

extern int is_partition_name(const char *device_name, u32 *partition_number){
	// get the partition number in the device_name
	*partition_number = (u32)strtol(device_name+3, NULL, 10);
	
	if(*partition_number <= 0 || *partition_number == LONG_MIN || *partition_number == LONG_MAX)
		return 0;
	
	return 1;
}

extern partition_info_t *create_record_for_existing_partition(
		disk_info_t *disk, char *device_name, u64 size_in_kilobytes,
		u32 device_order, u32 partition_number)
{
	u64 total_kilobytes = 0, used_kilobytes = 0;
	partition_info_t *result;
	partition_info_t **follow_partition_info_list;
	char *mount_info = read_whole_file(MOUNT_FILE);
	int len;
	
	//printf("create record for existing partition (%d)\n", sizeof(u64));	// tmp test
	if(mount_info == NULL){
		csprintf("Failed to open \"%s\"!!\n", MOUNT_FILE);
		return NULL;
	}
	
	result = (partition_info_t *)malloc(sizeof(partition_info_t));
	if(result == NULL){
		csprintf("No memory!!(result)\n");
		free(mount_info);
		return NULL;
	}
	
	result->mount_point = find_mount_point(device_name, mount_info);
	if(result->mount_point != NULL){
		++(disk->mounted_number);
		result->file_system = find_file_system(result->mount_point, mount_info);
		check_disk_free(result->mount_point, &total_kilobytes, &used_kilobytes);
	}
	else{
		if(!isdigit(device_name[strlen(device_name)-1])){	// Disk
			free(mount_info);
			free(result);
			return NULL;
		}
		else{
			result->file_system = "unknown";
			used_kilobytes = 0;
		}
	}
	free(mount_info);
	
	len = strlen("/dev/")+strlen(device_name);
	result->device = (char *)malloc(sizeof(char)*len+1);
	if(result->device == NULL){
		csprintf("No memory!!(result->device)\n");
		free(result);
		return NULL;
	}
	sprintf(result->device, "/dev/%s", device_name);
	(result->device)[len] = 0;
	
	result->disk = disk;
	result->partition_number = partition_number;
	result->used_kilobytes = used_kilobytes;
	result->size_in_kilobytes = size_in_kilobytes;
	result->next = NULL;
	
	//printf("(chk 1) used kilobytes is %llu\n", used_kilobytes);	// tmp test
	// Insert the new record at the end of the partition list.
	follow_partition_info_list = &(disk->partitions);
	while(*follow_partition_info_list != NULL)
		follow_partition_info_list = &((*follow_partition_info_list)->next);
	*follow_partition_info_list = result;
	
	return result;
}

extern char *find_usb_port(u32 device_order){
	char usb_info_file[32], *usb_info, *follow_info;
	char port_str[4], *port;
	int len;
	
	memset(usb_info_file, 0, 32);
	sprintf(usb_info_file, "%s/%u", USB_INFO_DIR, device_order);
	
	usb_info = read_whole_file(usb_info_file);
	if(usb_info == NULL){
		csprintf("Cannot open the file \"%s\".\n", usb_info_file);
		return NULL;
	}
	
	if((follow_info = strstr(usb_info, "Port: ")) == NULL){
		csprintf("There was not the Port information with scsi%u.\n", device_order);
		free(usb_info);
		return NULL;
	}
	follow_info += strlen("Port: ");
	
	memset(port_str, 0, 4);
	if(sscanf(follow_info, "%s", port_str) != 1){
		csprintf("There was not enough information in the usb status file!!\n");
		free(usb_info);
		return NULL;
	}
	free(usb_info);
	
	len = strlen(port_str);
	port = (char *)malloc(sizeof(len)+1);
	if(port == NULL){
		csprintf("No memory!!(port)\n");
		return NULL;
	}
	strcpy(port, port_str);
	port[len] = 0;
	
	return port;
}

extern char *find_mount_point(const char *device_name, const char *const mount_info){
	char *start, *follow_info, *mount_point, bak;
	int len;
	
	start = strstr(mount_info, device_name);
	if(start == NULL){
		csprintf("\"%s\" don't in \"%s\"!\n", device_name, MOUNT_FILE);
		return NULL;
	}
	
	start += strlen(device_name)+1;
	if(start[0] != '/'){
		csprintf("\"%s\" don't in \"%s\"!\n", device_name, MOUNT_FILE);
		return NULL;
	}
	
	follow_info = start;
	while(*follow_info != 0 && !isspace(*follow_info))
		++follow_info;
	bak = *follow_info;
	*follow_info = 0;
	
	len = strlen(start);
	mount_point = (char *)malloc(sizeof(char)*len+1);
	if(mount_point == NULL){
		csprintf("No memory!!(mount_point)\n");
		return NULL;
	}
	strncpy(mount_point, start, len);
	mount_point[len] = 0;
	
	*follow_info = bak;
	
	return mount_point;
}

extern char *find_file_system(const char *mount_point, const char *const mount_info){
	char *start, *follow_info, *file_system, bak;
	int len;
	
	start = strstr(mount_info, mount_point);
	if(start == NULL){
		csprintf("\"%s\" is incorrect or\n", MOUNT_FILE);
		csprintf("\"%s\" don't in \"%s\"!\n", mount_point, MOUNT_FILE);
		return NULL;
	}
	start += strlen(mount_point)+1;
	follow_info = start;
	while(*follow_info != 0 && !isspace(*follow_info))
		++follow_info;
	bak = *follow_info;
	*follow_info = 0;
	
	len = strlen(start);
	file_system = (char *)malloc(sizeof(char)*len+1);
	if(file_system == NULL){
		csprintf("No memory!!(file_system)\n");
		return NULL;
	}
	strncpy(file_system, start, len);
	file_system[len] = 0;
	
	*follow_info = bak;
	
	return file_system;
}

extern int check_disk_free(const char *mount_point, u64 *total_kilobytes, u64 *used_kilobytes){
	u64 total_size, free_size, used_size;
	struct statfs fsbuf;
	
	//printf("### chk disk free:%s\n", mount_point);	// tmp test
	if(statfs(mount_point, &fsbuf)){
		csprintf("Can't get device's stat!!\n");
		return 0;
	}
	
	//printf("[libdisk chk] (%lu)(%lu)\n", fsbuf.f_blocks, fsbuf.f_bfree);  // tmp test
	total_size = (u64)((u64)fsbuf.f_blocks*(u64)fsbuf.f_bsize);
	free_size = (u64)((u64)fsbuf.f_bfree*(u64)fsbuf.f_bsize);
	used_size = total_size-free_size;
	//printf("[blocks:%lu] [bfree:%lu] [bsize:%lu]\n", fsbuf.f_blocks, fsbuf.f_bfree, fsbuf.f_bsize);	// tmp test
	//printf("[total:%llu] [free:%llu] [used:%llu]\n", total_size, free_size, used_size);	// tmp test
	
	*total_kilobytes = total_size/1024;
	*used_kilobytes = used_size/1024;
	//printf("(chk 2) used kilobytes is %llu (used_size = %llu, totalkbytes=%llu, free=%llu)\n", *used_kilobytes, used_size, *total_kilobytes, free_size);	// tmp test
	
	if(fsbuf.f_bfree == 0)
		return 0;
	else
		return 1;
}

extern int is_valid_fat32_disk_name(const char *name){
	char *follow;

	if(name == NULL || strlen(name) == 0)
		return 0;
	
	follow = (char *)name;
	while(*follow != 0){
		if(!isalnum(*follow) && *follow != ' ' && *follow != '_' && *follow != '-')
			return 0;
		++follow;
	}
	
	if((follow-name) > 11)
		return 0;
	
	return 1;
}

extern int make_fdisk_command(const char *tmpfile, const int new_pool_order, const char *new_pool_size, const char *file_system){
	FILE *fp;
	char *fdisk_input = NULL, *temp_input = NULL;
	int len;
	
	if(new_pool_order == 1){
		len = strlen("o\n");
		fdisk_input = (char *)malloc(sizeof(char)*(len+1));
		if(fdisk_input == NULL){
			csprintf("No memory!(fdisk_input)\n");
			return -1;
		}
		strcpy(fdisk_input, "o\n");
		fdisk_input[len] = 0;
	}
	else{
		fdisk_input = (char *)malloc(sizeof(char));
		if(fdisk_input == NULL){
			csprintf("No memory!(fdisk_input)\n");
			return -1;
		}
		(fdisk_input)[0] = 0;
	}
	
	len = strlen(fdisk_input)+strlen("n\n");
	temp_input = (char *)malloc(sizeof(char)*(len+1));
	if(temp_input == NULL){
		csprintf("No memory!(temp_input)\n");
		free(fdisk_input);
		return -1;
	}
	sprintf(temp_input, "%sn\n", fdisk_input);
	temp_input[len] = 0;
	free(fdisk_input);
	fdisk_input = temp_input;
	
	if(new_pool_order < 4){
		len = strlen(fdisk_input);
		if(!strcmp(new_pool_size, "full"))
			len += strlen("p\nN\n\n\n");
		else
			len += strlen("p\nN\n\n+M\n")+strlen(new_pool_size);
		temp_input = (char *)malloc(sizeof(char)*(len+1));
		if(temp_input == NULL){
			csprintf("No memory!(temp_input)\n");
			free(fdisk_input);
			return -1;
		}
		if(!strcmp(new_pool_size, "full"))
			sprintf(temp_input, "%sp\n%d\n\n\n", fdisk_input, new_pool_order);
		else
			sprintf(temp_input, "%sp\n%d\n\n+%sM\n", fdisk_input, new_pool_order, new_pool_size);
		temp_input[len] = 0;
		free(fdisk_input);
		fdisk_input = temp_input;
	}
	else if(new_pool_order == 4){
		len = strlen(fdisk_input)+strlen("e\n+100000M\n\nn\n\n+M\n")+strlen(new_pool_size);
		temp_input = (char *)malloc(sizeof(char)*(len+1));
		if(temp_input == NULL){
			csprintf("No memory!(temp_input)\n");
			free(fdisk_input);
			return -1;
		}
		sprintf(temp_input, "%se\n+100000M\n\nn\n\n+%sM\n", fdisk_input, new_pool_size);
		temp_input[len] = 0;
		free(fdisk_input);
		fdisk_input = temp_input;
	}
	else{
		len = strlen(fdisk_input)+strlen("\n+M\n")+strlen(new_pool_size);
		temp_input = (char *)malloc(sizeof(char)*(len+1));
		if(temp_input == NULL){
			csprintf("No memory!(temp_input)\n");
			free(fdisk_input);
			return -1;
		}
		sprintf(temp_input, "%s\n+%sM\n", fdisk_input, new_pool_size);
		temp_input[len] = 0;
		free(fdisk_input);
		fdisk_input = temp_input;
	}
	
	if(new_pool_order == 1)
		len = strlen(fdisk_input)+strlen("t\n");
	else
		len = strlen(fdisk_input)+strlen("t\nN\n");
	temp_input = (char *)malloc(sizeof(char)*(len+1));
	if(temp_input == NULL){
		csprintf("No memory!(temp_input)\n");
		free(fdisk_input);
		return -1;
	}
	if(new_pool_order == 1)
		sprintf(temp_input, "%st\n", fdisk_input);
	else if(new_pool_order == 4)
		sprintf(temp_input, "%st\n5\n", fdisk_input);
	else
		sprintf(temp_input, "%st\n%d\n", fdisk_input, new_pool_order);
	temp_input[len] = 0;
	free(fdisk_input);
	fdisk_input = temp_input;
	
	if(!strcmp(file_system, "vfat")
			|| !strcmp(file_system, "fat32")
			|| !strcmp(file_system, "msdos")){
		len = strlen(fdisk_input)+strlen("b\n");
		temp_input = (char *)malloc(sizeof(char)*(len+1));
		if(temp_input == NULL){
			csprintf("No memory!(temp_input)\n");
			free(fdisk_input);
			return -1;
		}
		sprintf(temp_input, "%sb\n", fdisk_input);
		temp_input[len] = 0;
		free(fdisk_input);
		fdisk_input = temp_input;
	}
	else if(!strcmp(file_system, "ext2")
			|| !strcmp(file_system, "ext3")){
		len = strlen(fdisk_input)+strlen("83\n");
		temp_input = (char *)malloc(sizeof(char)*(len+1));
		if(temp_input == NULL){
			csprintf("No memory!(temp_input)\n");
			free(fdisk_input);
			return -1;
		}
		sprintf(temp_input, "%s83\n", fdisk_input);
		temp_input[len] = 0;
		free(fdisk_input);
		fdisk_input = temp_input;
	}
	else{
		csprintf("Not support the filesystem, '%s'.\n", file_system);
		free(fdisk_input);
		return -1;
	}
	
	len = strlen(fdisk_input)+strlen("w\n");
	temp_input = (char *)malloc(sizeof(char)*(len+1));
	if(temp_input == NULL){
		csprintf("No memory!(temp_input)\n");
		free(fdisk_input);
		return -1;
	}
	sprintf(temp_input, "%sw\n", fdisk_input);
	temp_input[len] = 0;
	free(fdisk_input);
	fdisk_input = temp_input;
	
	fp = fopen(tmpfile, "w");
	if(fp == NULL){
		csprintf("Can't write the temporary file!\n");
		free(fdisk_input);
		return -1;
	}
	fprintf(fp, "%s", fdisk_input);
	fclose(fp);
	free(fdisk_input);
	
	return 0;
}

//extern int fdisk_disk(disk_info_t *disk, const int new_pool_order, const char *new_pool_size, const char *file_system){
extern int fdisk_disk(const char *device_path, const int new_pool_order, const char *new_pool_size, const char *file_system){
	char *command, *tmpfile = "/tmp/tmp-fdisk-command";
	int len, result;
	
	if(new_pool_order > 7){
		csprintf("Can't create more than 7 partitions!\n");
		return -1;
	}
	
	len = strlen("/usr/sbin/fdisk  < ")+strlen(device_path)+strlen(tmpfile);
	command = (char *)malloc(sizeof(char)*(len+1));
	if(command == NULL){
		csprintf("No memory!!(command)\n");
		return -1;
	}
	sprintf(command, "/usr/sbin/fdisk %s < %s", device_path, tmpfile);
	command[len] = 0;
	result = system(command);
	free(command);//*/
	
	if(result == -1)
		return -1;
	
	return 0;
}

extern int format_disk(const char *device_path, const int new_pool_order, const char *new_pool, const char *file_system){
	char *new_partition, *follow_info;
	u32 len;
	int result = 0;
	char *command[8];
	
	if(strcmp(file_system, "vfat")
			&& strcmp(file_system, "fat32")
			&& strcmp(file_system, "msdos")
			&& strcmp(file_system, "ext2")
			&& strcmp(file_system, "ext3")){
		csprintf("Not support the filesystem, '%s'.\n", file_system);
		return -1;
	}
	
	len = strlen(device_path)-strlen("disc")+strlen("partN");
	new_partition = (char *)malloc(sizeof(char)*(len+1));
	if(new_partition == NULL){
		csprintf("No memory!(new_partition)\n");
		return -1;
	}
	strcpy(new_partition, device_path);
	follow_info = new_partition;
	follow_info += strlen(new_partition)-strlen("disc");
	sprintf(follow_info, "part%d", new_pool_order);
	new_partition[len] = 0;
	
	if(!strcmp(file_system, "vfat")
			|| !strcmp(file_system, "fat32")
			|| !strcmp(file_system, "msdos")){
		command[0] = "/usr/sbin/mkdosfs";
		command[1] = "-F";
		command[2] = "32";
		command[3] = "-n";
		command[4] = (char *)new_pool;
		command[5] = new_partition;
		command[6] = NULL;
	}
	else if(!strcmp(file_system, "ext2")){
		command[0] = "/usr/sbin/mke2fs";
		command[1] = "-L";
		command[2] = (char *)new_pool;
		command[3] = new_partition;
		command[4] = "-I";
		command[5] = "128";
		command[6] = NULL;
	}
	else if(!strcmp(file_system, "ext3")){
		command[0] = "/usr/sbin/mke2fs";
		command[1] = "-j";
		command[2] = "-L";
		command[3] = (char *)new_pool;
		command[4] = new_partition;
		command[5] = "-I";
		command[6] = "128";
		command[7] = NULL;
	}
	
	result = _eval(command, "/dev/console", 0, NULL);//*/
	free(new_partition);
	
	if(result == -1)
		return -1;
	
	return 0;
}

extern int test_fdisk(const char *device_path, const int new_pool_order, const char *new_pool_size, const char *file_system){
	char *tmpfile = "/tmp/tmp-fdisk-command";
	int result;
	
	csprintf("test a.\n");
	
	make_fdisk_command(tmpfile, new_pool_order, new_pool_size, file_system);
	
	result = system("/usr/sbin/fdisk /dev/discs/disc0/disc < /www/claim_fat32.txt");
	//int result = system("/bin/cat /www/claim_fat32.txt | /usr/sbin/fdisk /dev/discs/disc0/disc");
	csprintf("test b. result = %d.\n", result);
	return 0;
}
