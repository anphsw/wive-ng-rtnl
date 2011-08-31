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
/* vi: set sw=4 ts=4 sts=4: */
/*
 *	wireless.c -- Wireless Settings 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: storage.c,v 1.10 2008-03-19 11:11:05 chhung Exp $
 */

#include	<stdlib.h>

#include	"utils.h"
#include	"storage.h"

static void storageAdm(webs_t wp, char_t *path, char_t *query);
static void StorageAddUser(webs_t wp, char_t *path, char_t *query);
static void StorageEditUser(webs_t wp, char_t *path, char_t *query);
static void storageFtpSrv(webs_t wp, char_t *path, char_t *query);
static void storageSmbSrv(webs_t wp, char_t *path, char_t *query);
static int GetNthNullUser();

#define DEBUG(x) do{fprintf(stderr, #x); fprintf(stderr, ": %s\n", x); }while(0)

void formDefineStorage(void) {
	websFormDefine(T("storageAdm"), storageAdm);
	websFormDefine(T("StorageAddUser"), StorageAddUser);
	websFormDefine(T("StorageEditUser"), StorageEditUser);
#ifdef CONFIG_FTPD
	websFormDefine(T("storageFtpSrv"), storageFtpSrv);
#endif
#if defined(CONFIG_USER_SAMBA) || defined(CONFIG_USER_SAMBA3)
	websFormDefine(T("storageSmbSrv"), storageSmbSrv);
#endif
}

/* goform/storageAdm */
static void storageAdm(webs_t wp, char_t *path, char_t *query)
{
	char_t *user_select, *submit;
	char feild[20];

	feild[0] = '\0';
	user_select = websGetVar(wp, T("storage_user_select"), T(""));
	submit = websGetVar(wp, T("hiddenButton"), T(""));

	if (strcmp(submit, "delete") == 0)
	{
		nvram_init(RT2860_NVRAM);
		sprintf(feild, "User%s", user_select);
		doSystem("storage.sh del \"%s\"", nvram_bufget(RT2860_NVRAM, feild));
		nvram_bufset(RT2860_NVRAM, feild, "");
		sprintf(feild, "Upw%s", user_select);
		nvram_bufset(RT2860_NVRAM, feild, "");
		sprintf(feild, "Umax%s", user_select);
		nvram_bufset(RT2860_NVRAM, feild, "");
		sprintf(feild, "Umode%s", user_select);
		nvram_bufset(RT2860_NVRAM, feild, "");
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);

		websRedirect(wp, "storage/management.asp");
	}
	else if (strcmp(submit, "apply") == 0)
	{
#ifdef CONFIG_FTPD
	    doSystem("storage.sh ftp");
#endif
#if defined(CONFIG_USER_SAMBA) || defined(CONFIG_USER_SAMBA3)
    	    doSystem("service samba restart");
#endif
	}
}

/* goform/StorageAddUser */
static void StorageAddUser(webs_t wp, char_t *path, char_t *query)
{
	char_t *name, *password, *mul_logins, *max_logins,
		   *download, *upload, *overwrite, *erase;
	char mode[6], feild[20];
	int index; 

	mode[0] = '\0';
	feild[0] = '\0';
	// fetch from web input
	name = websGetVar(wp, T("adduser_name"), T(""));
	password = websGetVar(wp, T("adduser_pw"), T(""));
	mul_logins = websGetVar(wp, T("adduser_mullogins"), T(""));
	max_logins = websGetVar(wp, T("adduser_maxlogins"), T("1"));
	download = websGetVar(wp, T("adduser_download"), T(""));
	upload = websGetVar(wp, T("adduser_upload"), T(""));
	overwrite = websGetVar(wp, T("adduser_overwrite"), T(""));
	erase = websGetVar(wp, T("adduser_erase"), T(""));
	/*
	DEBUG(name);
	DEBUG(password);
	DEBUG(mul_logins);
	DEBUG(max_logins);
	DEBUG(download);
	DEBUG(upload);
	DEBUG(overwrite);
	DEBUG(erase);
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
	// get null user feild form nvram
	index = GetNthNullUser();
	// fprintf(stderr, "index: %d\n", index);

	// set to nvram
	if (index != 0)
	{
		nvram_init(RT2860_NVRAM);

		sprintf(feild, "User%d", index);
		nvram_bufset(RT2860_NVRAM, feild, name);
		sprintf(feild, "Upw%d", index);
		nvram_bufset(RT2860_NVRAM, feild, password);
		sprintf(feild, "Umax%d", index);
		nvram_bufset(RT2860_NVRAM, feild, max_logins);
		sprintf(feild, "Umode%d", index);
		nvram_bufset(RT2860_NVRAM, feild, mode);

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
}

/* goform/StorageEditUser */
static void StorageEditUser(webs_t wp, char_t *path, char_t *query)
{
	char_t *index, *password, *mul_logins, *max_logins,
		   *download, *upload, *overwrite, *erase;
	char mode[6], feild[20];

	mode[0] = '\0';
	feild[0] = '\0';
	// fetch from web input
	index = websGetVar(wp, T("hiddenIndex"), T(""));
	password = websGetVar(wp, T("edituser_pw"), T(""));
	mul_logins = websGetVar(wp, T("edituser_mullogins"), T(""));
	max_logins = websGetVar(wp, T("edituser_maxlogins"), T("1"));
	download = websGetVar(wp, T("edituser_download"), T(""));
	upload = websGetVar(wp, T("edituser_upload"), T(""));
	overwrite = websGetVar(wp, T("edituser_overwrite"), T(""));
	erase = websGetVar(wp, T("edituser_erase"), T(""));
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

	// set to nvram
	nvram_init(RT2860_NVRAM);

	sprintf(feild, "Upw%s", index);
	nvram_bufset(RT2860_NVRAM, feild, password);
	sprintf(feild, "Umax%s", index);
	nvram_bufset(RT2860_NVRAM, feild, max_logins);
	sprintf(feild, "Umode%s", index);
	nvram_bufset(RT2860_NVRAM, feild, mode);

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);
}

#ifdef CONFIG_FTPD
/* goform/storageFtpSrv */
static void storageFtpSrv(webs_t wp, char_t *path, char_t *query)
{
	char_t *ftp, *anonymous;
	char_t *port, *max_users, *login_timeout, *stay_timeout;

	// fetch from web input
	ftp = websGetVar(wp, T("ftp_enabled"), T(""));
	anonymous = websGetVar(wp, T("ftp_anonymous"), T(""));
	port = websGetVar(wp, T("ftp_port"), T(""));
	max_users = websGetVar(wp, T("ftp_max_users"), T(""));
	login_timeout = websGetVar(wp, T("ftp_login_timeout"), T(""));
	stay_timeout = websGetVar(wp, T("ftp_stay_timeout"), T(""));

	// set to nvram
	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "FtpEnabled", ftp);
	nvram_bufset(RT2860_NVRAM, "FtpAnonymous", anonymous);
	nvram_bufset(RT2860_NVRAM, "FtpPort", port);
	nvram_bufset(RT2860_NVRAM, "FtpMaxUsers", max_users);
	nvram_bufset(RT2860_NVRAM, "FtpLoginTimeout", login_timeout);
	nvram_bufset(RT2860_NVRAM, "FtpStayTimeout", stay_timeout);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	// setup device
	doSystem("storage.sh ftp");

	// debug print
	websHeader(wp);
	websWrite(wp, T("<h2>ftp_enabled: %s</h2><br>\n"), ftp);
	websWrite(wp, T("ftp_anonymous: %s<br>\n"), anonymous);
	websWrite(wp, T("ftp_port: %s<br>\n"), port);
	websWrite(wp, T("ftp_max_users: %s<br>\n"), max_users);
	websWrite(wp, T("ftp_login_timeout: %s<br>\n"), login_timeout);
	websWrite(wp, T("ftp_stay_timeout: %s<br>\n"), stay_timeout);
	websFooter(wp);
	websDone(wp, 200);
}
#endif

#if defined(CONFIG_USER_SAMBA) || defined(CONFIG_USER_SAMBA3)
/* goform/storageSmbSrv */
static void storageSmbSrv(webs_t wp, char_t *path, char_t *query)
{
	char_t *smb, *wg, *netbios, *str;

	// fetch from web input
	smb = websGetVar(wp, T("smb_enabled"), T(""));
	wg = websGetVar(wp, T("smb_workgroup"), T(""));
	netbios = websGetVar(wp, T("smb_netbios"), T(""));
	str = websGetVar(wp, T("smb_string"), T(""));

	// set to nvram
	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "SmbEnabled", smb);
	nvram_bufset(RT2860_NVRAM, "WorkGroup", wg);
	nvram_bufset(RT2860_NVRAM, "SmbNetBIOS", netbios);
	nvram_bufset(RT2860_NVRAM, "SmbString", str);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	// restart samba service
	doSystem("service samba restart");

	// debug print
	websHeader(wp);
	websWrite(wp, T("<h2>smb_enabled: %s</h2><br>\n"), smb);
	websWrite(wp, T("smb_workgroup: %s<br>\n"), wg);
	websWrite(wp, T("smb_netbios: %s<br>\n"), netbios);
	websWrite(wp, T("smb_string: %s<br>\n"), str);
	websFooter(wp);
	websDone(wp, 200);
}
#endif

static int GetNthNullUser()
{
	char *feild, *user_name;
	int result = 0, index;

	nvram_init(RT2860_NVRAM);

	for (index = 1; index < 9; index++)
	{
		sprintf(feild, "User%d", index);
		user_name = nvram_bufget(RT2860_NVRAM, feild);
		if (strlen(user_name) == 0)
		{
			result = index;
			break;
		}
	}

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	return result;
}
