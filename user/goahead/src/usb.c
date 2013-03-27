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

static void storageFtpSrv(webs_t wp, char_t *path, char_t *query);
#ifdef CONFIG_USER_SAMBA3
static void storageSmbSrv(webs_t wp, char_t *path, char_t *query);
#endif
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
#ifdef CONFIG_USER_USHARE
	websAspDefine(T("ShowMediaDir"), ShowMediaDir);
#endif
#ifdef CONFIG_FTPD
	websFormDefine(T("storageFtpSrv"), storageFtpSrv);
#endif
#ifdef CONFIG_USER_SAMBA3
	websFormDefine(T("storageSmbSrv"), storageSmbSrv);
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

#ifdef CONFIG_USER_SAMBA3
//------------------------------------------------------------------------------
// Samba/CIFS setup
const parameter_fetch_t service_samba_flags[] =
{
	{ T("WorkGroup"), "WorkGroup", 0, T("") },
	{ T("SmbNetBIOS"), "SmbNetBIOS", 0, T("") },
	{ T("SmbString"), "SmbString", 0, T("") },
	{ T("SmbOsLevel"), "SmbOsLevel", 0, T("") },
	{ T("SmbTimeserver"), "SmbTimeserver", 0, T("0") },
	{ NULL, NULL, 0, NULL } // Terminator
};
/* goform/storageSmbSrv */
static void storageSmbSrv(webs_t wp, char_t *path, char_t *query)
{
	char_t *smb_enabled = websGetVar(wp, T("SmbEnabled"), T("0"));
	if (smb_enabled == NULL)
		smb_enabled = "0";

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "SmbEnabled", smb_enabled);

	if (CHK_IF_DIGIT(smb_enabled, 1))
		setupParameters(wp, service_samba_flags, 0);

	nvram_close(RT2860_NVRAM);

	//restart some services instead full reload
	doSystem("service sysctl restart");
	doSystem("service dhcpd restart");
	doSystem("service iptables restart");
	doSystem("service samba restart");

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

