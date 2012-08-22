/* vi: set sw=4 ts=4 sts=4: */
/*
 *	usb.c -- USB Application Settings
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: usb.c,v 1.7.2.1 2009-04-08 11:27:39 chhung Exp $
 */

#include	<stdlib.h>
#include	<dirent.h>
#include	<arpa/inet.h>

#include 	"utils.h"
#include	"usb.h"
#include 	"internet.h"
#include	"helpers.h"

static void storageAdm(webs_t wp, char_t *path, char_t *query);
static void StorageAddUser(webs_t wp, char_t *path, char_t *query);
static void StorageEditUser(webs_t wp, char_t *path, char_t *query);
static void storageDiskAdm(webs_t wp, char_t *path, char_t *query);
static void storageDiskPart(webs_t wp, char_t *path, char_t *query);
static void storageFtpSrv(webs_t wp, char_t *path, char_t *query);
static int GetNthFree(char *entry);
static int ShowPartition(int eid, webs_t wp, int argc, char_t **argv);
static int ShowAllDir(int eid, webs_t wp, int argc, char_t **argv);
static int getCount(int eid, webs_t wp, int argc, char_t **argv);
static int getMaxVol(int eid, webs_t wp, int argc, char_t **argv);
#ifdef CONFIG_USER_USHARE
static void storageMediaSrv(webs_t wp, char_t *path, char_t *query);
static void MediaDirAdd(webs_t wp, char_t *path, char_t *query);
static int ShowMediaDir(int eid, webs_t wp, int argc, char_t **argv);
static void fetchMediaConfig(void);
static void RunMediaSrv();
#endif
#ifdef CONFIG_USER_UVC_STREAM
static void webcamra(webs_t wp, char_t *path, char_t *query);
#endif
#ifdef CONFIG_USER_P910ND
static void printersrv(webs_t wp, char_t *path, char_t *query);
#endif
#ifdef CONFIG_USB_MODESWITCH
static void usbmodem(webs_t wp, char_t *path, char_t *query);
static int modemShowStatus(int eid, webs_t wp, int argc, char_t **argv);
#endif
#ifdef CONFIG_USER_TRANSMISSION
static void transmission(webs_t wp, char_t *path, char_t *query);
#endif


#define	LSDIR_INFO		"/tmp/lsdir"
#define	MOUNT_INFO		"/proc/mounts"

#define USB_STORAGE_PATH	"/media"
#define USB_STORAGE_SIGN	"/media/sd"

#define DEBUG(x) do{fprintf(stderr, #x); fprintf(stderr, ": %s\n", x); }while(0)

void formDefineUSB(void) {
	websAspDefine(T("ShowPartition"), ShowPartition);
	websAspDefine(T("ShowAllDir"), ShowAllDir);
	websAspDefine(T("getCount"), getCount);
	websAspDefine(T("getMaxVol"), getMaxVol);
#ifdef CONFIG_USER_USHARE
	websAspDefine(T("ShowMediaDir"), ShowMediaDir);
#endif

	websFormDefine(T("storageAdm"), storageAdm);
	websFormDefine(T("StorageAddUser"), StorageAddUser);
	websFormDefine(T("StorageEditUser"), StorageEditUser);
	websFormDefine(T("storageDiskAdm"), storageDiskAdm);
	websFormDefine(T("storageDiskPart"), storageDiskPart);
#ifdef CONFIG_FTPD
	websFormDefine(T("storageFtpSrv"), storageFtpSrv);
#endif
#ifdef CONFIG_USER_USHARE
	websFormDefine(T("storageMediaSrv"), storageMediaSrv);
	websFormDefine(T("MediaDirAdd"), MediaDirAdd);
#endif
#ifdef CONFIG_USER_UVC_STREAM
	websFormDefine(T("webcamra"), webcamra);
#endif
#ifdef CONFIG_USER_P910ND
	websFormDefine(T("printersrv"), printersrv);
#endif
#ifdef CONFIG_USB_MODESWITCH
	websFormDefine(T("usbmodem"), usbmodem);
	websAspDefine(T("modemShowStatus"), modemShowStatus);
#endif
#ifdef CONFIG_USER_TRANSMISSION
websFormDefine(T("formTrans"), transmission);
#endif
}

static int dir_count;
static int part_count;
static int media_dir_count;
static char first_part[12];

/* goform/storageAdm */
static void storageAdm(webs_t wp, char_t *path, char_t *query)
{
	char_t *submit;

	submit = websGetVar(wp, T("hiddenButton"), T(""));

	if (0 == strcmp(submit, "delete"))
	{
		char_t *user_select;
		char feild[20];

		nvram_init(RT2860_NVRAM);

		feild[0] = '\0';
		user_select = websGetVar(wp, T("storage_user_select"), T(""));
		sprintf(feild, "User%s", user_select);
		doSystem("storage.sh deldir %s/home/%s", first_part, nvram_bufget(RT2860_NVRAM, feild));

		nvram_bufset(RT2860_NVRAM, feild, "");
		sprintf(feild, "FtpUser%s", user_select);
		nvram_bufset(RT2860_NVRAM, feild, "");
		sprintf(feild, "UserPasswd%s", user_select);
		nvram_bufset(RT2860_NVRAM, feild, "");
		sprintf(feild, "FtpMaxLogins%s", user_select);
		nvram_bufset(RT2860_NVRAM, feild, "");
		sprintf(feild, "FtpMode%s", user_select);
		nvram_bufset(RT2860_NVRAM, feild, "");
		sprintf(feild, "SmbUser%s", user_select);
		nvram_bufset(RT2860_NVRAM, feild, "");

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
	else if (0 == strcmp(submit, "apply"))
	{
		initUSB();
	}
	websRedirect(wp, "usb/STORAGEuser_admin.asp");
}

/* goform/StorageAddUser */
static void StorageAddUser(webs_t wp, char_t *path, char_t *query)
{
	char_t *name, *password;
	char_t *mul_logins, *max_logins, *download, *upload, *overwrite, *erase;
	char_t *user_ftp_enable, *user_smb_enable;
	char mode[6], feild[20];
	int index; 

	mode[0] = '\0';
	feild[0] = '\0';

	// fetch from web input
	name = websGetVar(wp, T("adduser_name"), T(""));
	password = websGetVar(wp, T("adduser_pw"), T(""));
	user_ftp_enable = websGetVar(wp, T("adduser_ftp"), T(""));
	mul_logins = websGetVar(wp, T("adduser_mullogins"), T(""));
	max_logins = websGetVar(wp, T("adduser_maxlogins"), T("1"));
	download = websGetVar(wp, T("adduser_download"), T(""));
	upload = websGetVar(wp, T("adduser_upload"), T(""));
	overwrite = websGetVar(wp, T("adduser_overwrite"), T(""));
	erase = websGetVar(wp, T("adduser_erase"), T(""));
	user_smb_enable = websGetVar(wp, T("adduser_smb"), T(""));
	/*
	DEBUG(name);
	DEBUG(password);
	DEBUG(user_ftp_enable);
	DEBUG(mul_logins);
	DEBUG(max_logins);
	DEBUG(download);
	DEBUG(upload);
	DEBUG(overwrite);
	DEBUG(erase);
	DEBUG(user_smb_enable);
	*/
	if (0 == strcmp(mul_logins, "1"))
		sprintf(mode, "%sM", mode);
	if (0 == strcmp(download, "1"))
		sprintf(mode, "%sD", mode);
	if (0 == strcmp(upload, "1"))
		sprintf(mode, "%sU", mode);
	if (0 == strcmp(overwrite, "1"))
		sprintf(mode, "%sO", mode);
	if (0 == strcmp(erase, "1"))
		sprintf(mode, "%sE", mode);
	// DEBUG(mode);
	// get null user feild form nvram
	index = GetNthFree("User");
	// fprintf(stderr, "index: %d\n", index);

	// set to nvram
	if (0 != index)
	{
		nvram_init(RT2860_NVRAM);

		sprintf(feild, "User%d", index);
		nvram_bufset(RT2860_NVRAM, feild, name);
		sprintf(feild, "UserPasswd%d", index);
		nvram_bufset(RT2860_NVRAM, feild, password);

		sprintf(feild, "FtpUser%d", index);
		nvram_bufset(RT2860_NVRAM, feild, user_ftp_enable);
		sprintf(feild, "FtpMaxLogins%d", index);
		nvram_bufset(RT2860_NVRAM, feild, max_logins);
		sprintf(feild, "FtpMode%d", index);
		nvram_bufset(RT2860_NVRAM, feild, mode);

		sprintf(feild, "SmbUser%d", index);
		nvram_bufset(RT2860_NVRAM, feild, user_smb_enable);

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
}

/* goform/StorageEditUser */
static void StorageEditUser(webs_t wp, char_t *path, char_t *query)
{
	char_t *index, *password; 
	char_t *mul_logins, *max_logins, *download, *upload, *overwrite, *erase;
	char_t *user_ftp_enable, *user_smb_enable;
	char mode[6], feild[20];

	mode[0] = '\0';
	feild[0] = '\0';
	// fetch from web input
	index = websGetVar(wp, T("hiddenIndex"), T(""));
	password = websGetVar(wp, T("edituser_pw"), T(""));
	user_ftp_enable = websGetVar(wp, T("edituser_ftp"), T(""));
	mul_logins = websGetVar(wp, T("edituser_mullogins"), T(""));
	max_logins = websGetVar(wp, T("edituser_maxlogins"), T("1"));
	download = websGetVar(wp, T("edituser_download"), T(""));
	upload = websGetVar(wp, T("edituser_upload"), T(""));
	overwrite = websGetVar(wp, T("edituser_overwrite"), T(""));
	erase = websGetVar(wp, T("edituser_erase"), T(""));
	user_smb_enable = websGetVar(wp, T("edituser_smb"), T(""));
	/*
	DEBUG(index);
	DEBUG(password);
	DEBUG(user_ftp_enable);
	DEBUG(mul_logins);
	DEBUG(max_logins);
	DEBUG(download);
	DEBUG(upload);
	DEBUG(overwrite);
	DEBUG(erase);
	DEBUG(user_smb_enable);
	*/
	if (strcmp(mul_logins, "1") == 0)
		sprintf(mode, "%sM", mode);
	if (strcmp(download, "1") == 0)
		sprintf(mode, "%sD", mode);
	if (strcmp(upload, "1") == 0)
		sprintf(mode, "%sU", mode);
	if (strcmp(overwrite, "1") == 0)
		sprintf(mode, "%sO", mode);
	if (strcmp(erase, "1") == 0)
		sprintf(mode, "%sE", mode);
	// DEBUG(mode);

	// set to nvram
	nvram_init(RT2860_NVRAM);

	sprintf(feild, "UserPasswd%s", index);
	nvram_bufset(RT2860_NVRAM, feild, password);
	sprintf(feild, "FtpUser%s", index);
	// DEBUG(feild);
	nvram_bufset(RT2860_NVRAM, feild, user_ftp_enable);
	if (0 == strcmp(user_ftp_enable, "1"))
	{
		sprintf(feild, "FtpMaxLogins%s", index);
		nvram_bufset(RT2860_NVRAM, feild, max_logins);
		sprintf(feild, "FtpMode%s", index);
		nvram_bufset(RT2860_NVRAM, feild, mode);
	}
	sprintf(feild, "SmbUser%s", index);
	// DEBUG(feild);
	nvram_bufset(RT2860_NVRAM, feild, user_smb_enable);

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	websRedirect(wp, "usb/STORAGEdisk_admin.asp");
}

static void storageDiskAdm(webs_t wp, char_t *path, char_t *query)
{
	char_t *submit;

	submit = websGetVar(wp, T("hiddenButton"), T(""));

	if (0 == strcmp(submit, "delete"))
	{
		char_t *dir_path = websGetVar(wp, T("dir_path"), T(""));
		doSystem("storage.sh deldir \"%s\"", dir_path);
		websRedirect(wp, "usb/STORAGEdisk_admin.asp");
	}
	else if (0 == strcmp(submit, "add"))
	{
		char_t *dir_name, *disk_part;

		dir_name = websGetVar(wp, T("adddir_name"), T(""));
		disk_part = websGetVar(wp, T("disk_part"), T(""));
		doSystem("storage.sh adddir \"%s/%s\"", disk_part, dir_name);
	}
	else if (0 == strcmp(submit, "format"))
	{
		char_t *disk_part = websGetVar(wp, T("disk_part"), T(""));
		FILE *fp_mount = NULL;
		char part[30], path[30];

		if (NULL == (fp_mount = fopen("/proc/mounts", "r")))
		{
			perror(__FUNCTION__);
			return;
		}
		while(EOF != fscanf(fp_mount, "%s %s %*s %*s %*s %*s\n", part, path))
		{
			if (0 == strcmp(path, disk_part))
				break;
		}
		fclose(fp_mount);
		doSystem("storage.sh %s %s %s",submit, part, path);
		websRedirect(wp, "usb/STORAGEdisk_admin.asp");
	}
	else if (0 == strcmp(submit, "remove"))
	{
		FILE *fp_mount = NULL;
		char part[30];

		if (NULL == (fp_mount = fopen("/proc/mounts", "r")))
		{
			perror(__FUNCTION__);
			return;
		}
		while(EOF != fscanf(fp_mount, "%s %*s %*s %*s %*s %*s\n", part))
		{
			if (NULL != strstr(part, "/dev/sd"))
				doSystem("umount -lf %s", part);
		}
		fclose(fp_mount);
	}
}

static void storageDiskPart(webs_t wp, char_t *path, char_t *query)
{
	char_t *part1_vol, *part2_vol, *part3_vol, *part4_vol;
	FILE *fp_mount = NULL;
	char part[30];

	if (NULL == (fp_mount = fopen("/proc/mounts", "r")))
	{
		perror(__FUNCTION__);
		return;
	}
	while(EOF != fscanf(fp_mount, "%s %*s %*s %*s %*s %*s\n", part))
	{
		fprintf(stderr, "chhung part: %s\n", part);
		if (NULL != strstr(part, "/dev/sd"))
			doSystem("umount -fl %s", part);
	}
	part1_vol = websGetVar(wp, T("part1_vol"), T(""));
	part2_vol = websGetVar(wp, T("part2_vol"), T(""));
	part3_vol = websGetVar(wp, T("part3_vol"), T(""));
	part4_vol = websGetVar(wp, T("part4_vol"), T(""));

	doSystem("storage.sh reparted %s %s %s %s", 
			  part1_vol, part2_vol, part3_vol, part4_vol);
	fclose(fp_mount);
}

#ifdef CONFIG_FTPD
// FTP setup
const parameter_fetch_t ftp_server_args[] =
{
	{ T("ftp_port"), "FtpPort", 0, T("") },
	{ T("ftp_rootdir"), "FtpRootDir", 0, T("") },
	{ T("ftp_idle_timeout"), "FtpIdleTime", 0, T("") },
	{ NULL, NULL, 0, NULL } // Terminator
};
/* goform/storageFtpSrv */
static void storageFtpSrv(webs_t wp, char_t *path, char_t *query)
{
	char_t *ftp_enable = websGetVar(wp, T("ftp_enabled"), T("0"));
	if (ftp_enable == NULL)
		ftp_enable = "0";
		
	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "RemoteFTP", ftp_enable);

	if (CHK_IF_DIGIT(ftp_enable, 1) || CHK_IF_DIGIT(ftp_enable, 2))
		setupParameters(wp, ftp_server_args, 0);

	nvram_close(RT2860_NVRAM);
	
	//restart some services instead full reload
	doSystem("service inetd restart");
	doSystem("service iptables restart");

	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	if (submitUrl != NULL)
		websRedirect(wp, submitUrl);
	else
		websDone(wp, 200);
}
#endif

#ifdef CONFIG_USER_USHARE
static void storageMediaSrv(webs_t wp, char_t *path, char_t *query)
{
	char_t *submit;

	submit =  websGetVar(wp, T("hiddenButton"), T(""));

	if (0 == strcmp(submit, "delete"))
	{
		int index;
		FILE *fp;
		char media_config_file[25];

		// strcpy(smb_config, "/var/.smb_config");
		sprintf(media_config_file, "%s/.media_config", first_part);
		fp = fopen(media_config_file, "w");

		if (NULL == fp) {
			perror(__FUNCTION__);
			return;
		}

		index = atoi(websGetVar(wp, T("media_dir"), T("")));
		memset(&media_conf[index].path, 0, sizeof(media_conf[index].path));	
		fwrite(media_conf, sizeof(media_conf), 1, fp);
		fclose(fp);
		websRedirect(wp, "usb/USHAREmediasrv.asp");
	} 
	else if (0 == strcmp(submit, "apply"))
	{
		char_t *media_enabled, *media_name;
		int i;

		// fetch from web input
		media_enabled = websGetVar(wp, T("media_enabled"), T(""));
		media_name = websGetVar(wp, T("media_name"), T(""));

		// set to nvram
		nvram_init(RT2860_NVRAM);
		nvram_bufset(RT2860_NVRAM, "mediaSrvEnabled", media_enabled);
		nvram_bufset(RT2860_NVRAM, "mediaSrvName", media_name);
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);

		// setup device
		if (0 == strcmp(media_enabled, "0"))
			memset(media_conf, 0, sizeof(media_conf));
		RunMediaSrv();

		// debug print
		websHeader(wp);
		websWrite(wp, T("<h2>Media Server Settings</h2><br>\n"));
		websWrite(wp, T("media_enabled: %s<br>\n"), media_enabled);
		websWrite(wp, T("media_name: %s<br>\n"), media_name);
		for(i=0;i<4;i++)
			websWrite(wp, T("media dir%d path: %s<br>\n"), i, media_conf[i].path);
		websFooter(wp);
		websDone(wp, 200);
	}
}

static void MediaDirAdd(webs_t wp, char_t *path, char_t *query)
{
	char_t *dir_path;
	FILE *fp;
	char media_config_file[25];
	int index = 0;

	sprintf(media_config_file, "%s/.media_config", first_part);
	fp = fopen(media_config_file, "w");

	if (NULL == fp) {
		perror(__FUNCTION__);
		return;
	}

	// fetch from web input
	dir_path = websGetVar(wp, T("dir_path"), T(""));
	/*
	DEBUG(dir_path);
	*/
	while (4 > index)
	{
		if (0 == strcmp(dir_path, media_conf[index].path))
		{
			fprintf(stderr, "Existed Media Shared Dir: %s\n", dir_path);
			fwrite(media_conf, sizeof(media_conf), 1, fp);
			fclose(fp);
			return;
		}
		index++;
	}
	index = 0;
	while (4 > index)
	{
		if (0 == strlen(media_conf[index].path))
		{
			strcpy(media_conf[index].path, dir_path);
			break;
		}
		index++;
	}
	if (index == 5)
	{
		perror("Media Server Shared Dirs exceed 4");
		fclose(fp);
		return;
	}
	fwrite(media_conf, sizeof(media_conf), 1, fp);
	fclose(fp);
}

static int ShowMediaDir(int eid, webs_t wp, int argc, char_t **argv)
{
	int index;

	fetchMediaConfig();
	media_dir_count = 0;
	for(index=0;index<4;index++)
	{
		if (0 != strlen(media_conf[index].path)) 
		{
			websWrite(wp, T("<tr align=\"center\">"));
			websWrite(wp, T("<td><input type=\"radio\" name=\"media_dir\" value=\"%d\"></td>"), 
					  index);
			websWrite(wp, T("<td>%s</td>"), media_conf[index].path);
			websWrite(wp, T("</tr>"));
			media_dir_count++;
		}
	}

	return 0;
}

static void fetchMediaConfig(void)
{
	FILE *fp = NULL;
	char media_config_file[25];

	memset(media_conf, 0, sizeof(media_conf));
	sprintf(media_config_file, "%s/.media_config", first_part);
	if (NULL == (fp = fopen(media_config_file, "r")))
	{
		perror(__FUNCTION__);
		return;
	}
	fread(media_conf, sizeof(media_conf), 1, fp);
	fclose(fp);
}

static void RunMediaSrv()
{
	char mediasrv_dir[160];
	int i;

	memset(mediasrv_dir, 0, sizeof(mediasrv_dir));
	for(i=0;i<4;i++)
		if (0 != strlen(media_conf[i].path))
			sprintf(mediasrv_dir, "%s %s", mediasrv_dir, media_conf[i].path);

	doSystem("storage.sh media %s", mediasrv_dir);
}
#endif

#ifdef CONFIG_USER_UVC_STREAM
/* goform/webcamra */
static void webcamra(webs_t wp, char_t *path, char_t *query)
{
	char_t *enable, *resolution, *fps, *port;

	// fetch from web input
	enable = websGetVar(wp, T("enabled"), T(""));
	resolution = websGetVar(wp, T("resolution"), T(""));
	fps = websGetVar(wp, T("fps"), T(""));
	port = websGetVar(wp, T("port"), T(""));

	// set to nvram
	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "WebCamEnabled", enable);
	nvram_bufset(RT2860_NVRAM, "WebCamResolution", resolution);
	nvram_bufset(RT2860_NVRAM, "WebCamFPS", fps);
	nvram_bufset(RT2860_NVRAM, "WebCamPort", port);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	// setup device
	doSystem("killall -q uvc_stream");
	if (0 == strcmp(enable, "1"))
	{
		doSystem("uvc_stream -r %s -f %s -p %s -b", 
				  resolution, fps, port);
	}

	// debug print
	websHeader(wp);
	websWrite(wp, T("<h2>Web Camera Settings</h2><br>\n"));
	websWrite(wp, T("enabled: %s<br>\n"), enable);
	websWrite(wp, T("resolution: %s<br>\n"), resolution);
	websWrite(wp, T("fps: %s<br>\n"), fps);
	websWrite(wp, T("port: %s<br>\n"), port);
	websFooter(wp);
	websDone(wp, 200);
}
#endif

#ifdef CONFIG_USER_P910ND
static void printersrv(webs_t wp, char_t *path, char_t *query)
{
	char_t *enable;
	char_t *bidirect;
	char *submitUrl;

	// fetch from web input
	enable = websGetVar(wp, T("enabled"), T(""));
	bidirect = websGetVar(wp, T("bdenabled"), T(""));
	// set to nvram
	nvram_set(RT2860_NVRAM, "PrinterSrvEnabled", enable);
	nvram_set(RT2860_NVRAM, "PrinterSrvBidir", bidirect);

submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	if (! submitUrl[0])
	{

	// debug print
	websHeader(wp);
	websWrite(wp, T("<h2>Printer Server Settings</h2><br>\n"));
	websWrite(wp, T("enabled: %s<br>\n"), enable);
	websFooter(wp);
	websDone(wp, 200);
	}
	else
		websRedirect(wp, submitUrl);
}
#endif

#ifdef CONFIG_USB_MODESWITCH

const parameter_fetch_t usb_modem_args[] =
{
	{ T("modem_type"), "MODEMTYPE", 0, T("0") },
	{ T("modem_port"), "WMODEMPORT", 0, T("AUTO") },
	{ T("modem_speed"), "MODEMSPEED", 0, T("AUTO") },
	{ T("modem_mtu"), "MODEMMTU", 0, T("AUTO") },
	{ T("modem_user"), "MODEMUSERNAME", 0, T("") },
	{ T("modem_pass"), "MODEMPASSWORD", 0, T("") },
	{ T("modem_dialn"), "MODEMDIALNUMBER", 0, T("") },
	{ T("modem_apn"), "APN", 0, T("") },
	{ T("at_enabled"), "MODEMATENABLED", 2, T("off") },
	{ T("mdebug_enabled"), "MODEMDEBUG", 2, T("off") },
	{ T("modem_at1"), "MODEMAT1", 0, T("") },
	{ T("modem_at2"), "MODEMAT2", 0, T("") },
	{ T("modem_at3"), "MODEMAT3", 0, T("") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void usbmodem(webs_t wp, char_t *path, char_t *query)
{
	char *submitUrl;
	char_t *submit;

	submit = websGetVar(wp, T("hiddenButton"), T(""));
	
	if (0 == strcmp(submit, "apply"))
		{
			char_t *modem_enabled = websGetVar(wp, T("modem_enabled"), T("0"));
			if (modem_enabled == NULL)
				modem_enabled = "0";
		
			nvram_init(RT2860_NVRAM);
			nvram_bufset(RT2860_NVRAM, "MODEMENABLED", modem_enabled);

			if (CHK_IF_DIGIT(modem_enabled, 1))
				setupParameters(wp, usb_modem_args, 0);

			nvram_close(RT2860_NVRAM);
		}
	else if (0 == strcmp(submit, "connect"))
		{
			doSystem("service modemhelper start &");
		}
	else if (0 == strcmp(submit, "disconnect"))
		{
			doSystem("service modemhelper stop &");
		}

submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	if (submitUrl != NULL)
		websRedirect(wp, submitUrl);
	else
		websDone(wp, 200);
}

/*** USB modem statuses ***/
typedef struct modem_status_t
{
	const char_t *status;
	long          color;
} modem_status_t;

/*** Modem statuses ***/
const modem_status_t modem_statuses[] =
{

	{ "disabled",     0x808080        },
	{ "offline",      0xff0000        },
//	{ "connecting",   0xff8000        },
	{ "online",       0x00ff00        }
};
/*** Show Modem Connect status ***/
static int modemShowStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	int status = 0; // Status is 'disabled'
	const modem_status_t *st_table = modem_statuses;

	// Get value
	char *modem_enabled = nvram_get(RT2860_NVRAM, "MODEMENABLED");
	
	// Do not perform other checks if modem is turned off
	if (strcmp(modem_enabled, "1")==0)
	{

			// Status is at least 'offline' now
			status++;

			// Try to find pppd
			int found = procps_count("pppd");
			
			if (found>0)
			{
				// Now status is at least 'connecting'
				//status++;

				// Try to search for 'ppp_modem' device
				FILE * fd = fopen(_PATH_PROCNET_DEV, "r");

				if (fd != NULL)
				{
					//int ppp_id;
					char_t line[256];

					// Read all ifaces and check match
					while (fgets(line, 255, fd)!=NULL)
					{
						if(strstr(line,"ppp_modem")!=NULL)
						{
							status++; // Status is set to 'connected'
							break; // Do not search more
						}
					}
					fclose(fd);
				}
				else
				{
					fprintf(stderr, "Warning: cannot open %s (%s).\n",
						_PATH_PROCNET_DEV, strerror(errno));
				}
			}
			else if (found<0)
			{
				fprintf(stderr, "Warning: cannot serach process 'pppd': %s\n",
						strerror(-found));
			}
	}

	// Output connection status
	const modem_status_t *st = &st_table[status];
	websWrite(
		wp,
		T("<b>Status: <font color=\"#%06x\">%s</font></b>\n"),
		st->color, st->status
	);

	return 0;
}
#endif

#ifdef CONFIG_USER_TRANSMISSION

const parameter_fetch_t transmission_args[] =
{
	{ T("transRPCPort"), "TransRPCPort", 0, T("") },
	{ T("transAccess"), "TransAccess", 0, T("") },
	{ T("transAuthor"), "TransAuthor", 0, T("") },
	{ T("transLog"), "TransLogin", 0, T("") },
	{ T("transPass"), "TransPass", 0, T("") },
	{ T("transInPort"), "TransInPort", 0, T("") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void transmission(webs_t wp, char_t *path, char_t *query)
{
	char *submitUrl;
	char_t *submit;
	
	submit = websGetVar(wp, T("hiddenButton"), T(""));
	
	if (0 == strcmp(submit, "apply"))
		{
			char_t *trans_enabled = websGetVar(wp, T("TransEnabled"), T("0"));
			if (trans_enabled == NULL)
			trans_enabled = "0";
		
			nvram_init(RT2860_NVRAM);
			nvram_bufset(RT2860_NVRAM, "TransmissionEnabled", trans_enabled);

			if (CHK_IF_DIGIT(trans_enabled, 1))
			setupParameters(wp, transmission_args, 0);

			nvram_close(RT2860_NVRAM);
			doSystem("service iptables restart");
			doSystem("service transmission restart");
		}
	else if (0 == strcmp(submit, "start"))
		{
			doSystem("service transmission start");
		}
	else if (0 == strcmp(submit, "stop"))
		{
			doSystem("service transmission stop");
		}	
	else if (0 == strcmp(submit, "reload"))
		{
			doSystem("service transmission reload");
		}		
		
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	if (submitUrl != NULL)
		websRedirect(wp, submitUrl);
	else
		websDone(wp, 200);
}	
#endif

int initUSB(void)
{
#ifdef CONFIG_USER_USHARE
	fetchMediaConfig();
	RunMediaSrv();
#endif
#ifdef CONFIG_USER_UVC_STREAM
	printf("UVC init\n");
	char *webcamebl = nvram_get(RT2860_NVRAM, "WebCamEnabled");
	doSystem("killall -q uvc_stream");
	if (0 == strcmp(webcamebl, "1"))
	{
		printf("UVC start\n");
			doSystem("uvc_stream -r %s -f %s -p %s -b", 
				  nvram_get(RT2860_NVRAM, "WebCamResolution"), 
				  nvram_get(RT2860_NVRAM, "WebCamFPS"), 
				  nvram_get(RT2860_NVRAM, "WebCamPort"));
	}
#endif
	return 0;
}

static int getCount(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char count[3];

	if (2 > ejArgs(argc, argv, T("%d %s"), &type, &field)) 
	{
		return websWrite(wp, T("Insufficient args\n"));
	}

	if (0 == strcmp(field, "AllDir"))
	{
		sprintf(count, "%d", dir_count);
		// fprintf(stderr,"AllDir: %s\n", count);
	}
	else if (0 == strcmp(field, "AllPart"))
	{
		sprintf(count, "%d", part_count);
		// fprintf(stderr,"AllPart: %s\n", count);
	}
	else if (0 == strcmp(field, "AllMediaDir"))
	{
		sprintf(count, "%d", media_dir_count);
		// fprintf(stderr,"AllPart: %s\n", count);
	}

	if (1 == type) {
		if (!strcmp(count, ""))
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), count);
	}
	if (!strcmp(count, ""))
		ejSetResult(eid, "0");
	ejSetResult(eid, count);

	return 0;
}

static int GetNthFree(char *entry)
{
	int index = 1;
	char feild[20], *user_name;

	feild[0] = '\0';
	do
	{
		sprintf(feild, "%s%d", entry, index);
		user_name = nvram_get(RT2860_NVRAM, feild);
		if (strlen(user_name) == 0)
			return index;
		index++;
	} while (index <= 20);

	return 0;
}

static int ShowAllDir(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp_mount = fopen(MOUNT_INFO, "r");
	char part[50], path[30];
	char dir_name[30];
	int dir_len = 0;

	if (NULL == fp_mount) {
        perror(__FUNCTION__);
		return -1;
	}

	dir_count = 0;

	while(EOF != fscanf(fp_mount, "%s %s %*s %*s %*s %*s\n", part, path))
	{
		DIR *dp;
		struct dirent *dirp;
		struct stat statbuf;

		if (0 != strncmp(path, "/media/sd", 9))
		{
			continue;
		}
		if (NULL == (dp = opendir(path)))
		{
			fprintf(stderr, "open %s error\n", path);
			return -1;
		}
		chdir(path);
		while(NULL != (dirp = readdir(dp)))
		{
			lstat(dirp->d_name, &statbuf);
			if(S_ISDIR(statbuf.st_mode))
			{
				if (0 == strncmp(dirp->d_name, ".", 1) ||
					0 == strcmp(dirp->d_name, "home"))
					continue;
				strcpy(dir_name, dirp->d_name);
				dir_len = strlen(dir_name);
				if (dir_len < 30 && dir_len > 0)
				{
					websWrite(wp, T("<tr><td><input type=\"radio\" name=\"dir_path\" value=\"%s/%s\"></td>"), 
							  path, dir_name);
					websWrite(wp, T("<td>%s/%s</td>"), path, dir_name);
					websWrite(wp, T("<input type=\"hidden\" name=\"dir_part\" value=\"%s\">"), 
							  part);
					websWrite(wp, T("<td>%s</td>"), part);
					websWrite(wp, T("</tr>"));
					dir_count++;
				}
			}
		}
		chdir("/");
		closedir(dp);
	}
	fclose(fp_mount);
	// fprintf(stderr, "dir_count: %d\n", dir_count);

	return 0;
}

static int ShowPartition(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp = fopen(MOUNT_INFO, "r");
	char part[50], path[30]; 
	if (NULL == fp) {
        perror(__FUNCTION__);
		return -1;
	}
	part_count = 0;

	while(EOF != fscanf(fp, "%s %s %*s %*s %*s %*s\n", part, path))
	{
		// if (strncmp(path, "/var", 4) != 0)
		if (0 != strncmp(path, "/media/sd", 9))
		{
			continue;
		}
		websWrite(wp, T("<tr align=center>"));
		websWrite(wp, T("<td><input type=\"radio\" name=\"disk_part\" value=\"%s\"></td>"), 
				  path);
		websWrite(wp, T("<td>%s</td>"), part);
		websWrite(wp, T("<td>%s</td>"), path);
		websWrite(wp, T("</tr>"));
		part_count++;
	}
	fclose(fp);
	// fprintf(stderr, "part_count: %d\n", part_count);

	return 0;
}

static int getMaxVol(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *pp = popen("fdisk -l /dev/sda", "r");
	char maxvol[5], unit[5];
	double transfer, result=0;

	fscanf(pp, "%*s %*s %s %s %*s %*s\n", maxvol, unit);
	pclose(pp);

	transfer = atof(maxvol);
	if (0 == strcmp(unit, "GB,"))
	{
		result = transfer*1000;
	}
	else if (0 == strcmp(unit, "MB,"))
	{
		result = transfer;
	}

	return websWrite(wp, T("%d"), result);
}

static int isStorageExist(void)
{
	char buf[256];
	FILE *fp = fopen("/proc/mounts", "r");
	if(!fp){
		perror(__FUNCTION__);
		return 0;
	}

	while(fgets(buf, sizeof(buf), fp)){
		if(strstr(buf, USB_STORAGE_SIGN)){
			fclose(fp);
			return 1;
		}
	}

	fclose(fp);
	printf("no usb disk found\n.");
	return 0;
}
