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
/*
 * Copyright 2004, ASUSTek Inc.
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND ASUS GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: watchdog.c,v 1.1.1.1 2007/01/25 12:52:21 jiahao_jhou Exp $
 */

 
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <shutils.h>
#include <rc.h>
#include <stdarg.h>
#include <ra3052.h>

typedef unsigned char   bool;   // 1204 ham

#include <wlioctl.h>
#include <syslog.h>
#include <nvram/bcmnvram.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
#include <sys/wait.h>
#include <nvram/bcmutils.h>

#include <sys/ioctl.h>
#include "ralink_gpio.h"

#define GPIO_DEV	"/dev/gpio"

#ifndef MAX_NVPARSE
#define MAX_NVPARSE	256
#endif

#define BCM47XX_SOFTWARE_RESET  0x40		/* GPIO 6 */
#define RESET_WAIT		3		/* seconds */
#define RESET_WAIT_COUNT	RESET_WAIT * 10 /* 10 times a second */

#define TEST_PERIOD		100		/* second */
#define NORMAL_PERIOD		1		/* second */
#define URGENT_PERIOD		100 * 1000	/* microsecond */	
#define RUSHURGENT_PERIOD	50 * 1000	/* microsecond */

#ifdef WL330GE
#define QUICK_PERIOD		250 * 1000	/* microsecond */
#endif
/*
#define RTN12_PWRLED_GPIO_IRQ	7
#define RTN12_RESETDF_GPIO_IRQ	10
#define RTN12_EZSETUP_GPIO_IRQ	0
#define RTN12_SW1		9
#define RTN12_SW2		13
#define RTN12_SW3		11
*/
#define GPIO0 0x0001
#define GPIO1 0x0002
#define GPIO2 0x0004
#define GPIO3 0x0008
#define GPIO4 0x0010
#define GPIO5 0x0020
#define GPIO6 0x0040
#define GPIO7 0x0080
#define GPIO15 0x8000

//#define LED_ON	0	// low active (all 5xx series)
//#define LED_OFF	1

#if 0
#define LED_POWER	GPIO0
#define BTN_RESET	GPIO2 
#define BTN_SETUP	GPIO3
#endif

//#define LED_POWER	RTN12_PWRLED_GPIO_IRQ
//#define BTN_RESET	RTN12_RESETDF_GPIO_IRQ 

#ifdef BTN_SETUP
#define SETUP_WAIT		3		/* seconds */
#define SETUP_WAIT_COUNT	SETUP_WAIT * 10 /* 10 times a second */
#define SETUP_TIMEOUT		60 		/* seconds */
#define SETUP_TIMEOUT_COUNT	SETUP_TIMEOUT * 10 /* 60 times a second */
#endif //BTN_SETUP

#ifdef WL330GE
#define WIF "eth1"
char buf[WLC_IOCTL_MAXLEN];
#define GPIO11		0x0800
#define LED_AIR2	GPIO11
int RSSI_TH_A=0;
int RSSI_TH_B=0;
int RSSI_TH_C=0;
int RSSI_TH_D=0;
int RSSI_TH_E=0;
int assoc_after_boot=-1;
static int rssi_g=-32768;
static int rssi_g_old=-32768;
static int stacheck_timer=0;
char AIR_LED_OFF[]={0x67, 0x70, 0x69, 0x6f, 0x74, 0x69, 0x6d, 0x65, 0x72, 0x76, 0x61, 0x6c, 0x0, 0x0, 0x10,  0x0, 0x0};
char AIR_LED_ON[]= {0x67, 0x70, 0x69, 0x6f, 0x74, 0x69, 0x6d, 0x65, 0x72, 0x76, 0x61, 0x6c, 0x0, 0x0, 0x00, 0x64, 0x0};
#endif

static int ddns_timer=0;
int dog_stop_wan=0;

/* Global containing the file descriptor of the GPIO kernel drivers */
static int bcmgpio_fd;
/* Global containing information about each GPIO pin */
/* Static function prototypes */

//#define LED_CONTROL(led,flag)	ra_gpio_write_spec(led, flag)

struct itimerval itv;
int watchdog_period=0;
static int btn_pressed=0;
static int btn_count=0;
long sync_interval=-1; // every 30 seconds a unit
int sync_flag=0;
long timestamp_g=0;
int stacheck_interval=-1;
#ifdef BTN_SETUP
int btn_pressed_setup=0;
int btn_pressed_flag=0;
int btn_count_setup=0;
int btn_count_timeout=0;
int wsc_timeout=0;
//int btn_stage=0;	// no need. use WPS.
int btn_count_setup_second=0;
#endif

#ifdef CDMA
int cdma_down=0;
int cdma_connect=0;
#endif

int reboot_count=0;
static int no_need_to_start_wps=0;
int delay_sec=0;
int upnp_delay_time=0;
int usb_chk_time=1;

int
http_check(const char *server, char *buf, size_t count, off_t offset)
{
	char *pid_file = "/var/run/httpd.pid";
	char pid_buf[10], proc_path[32];
	int fd, pid;
	struct stat f_st;

	if((fd=open(pid_file, O_RDONLY)) <= 0)
		return 0;
	memset(pid_buf, '\0', sizeof(pid_buf));
	read(fd, pid_buf, sizeof(pid_buf));
	close(fd);
	if((pid = atoi(pid_buf)) <= 0)
		return 0;

	memset(proc_path, '\0', sizeof(proc_path));
	sprintf(proc_path, "/proc/%d", pid);

	lstat(proc_path, &f_st);
	if(!S_ISDIR(f_st.st_mode))
		return 0;

	return 1;
}

int ra_gpio_set_dir(int dir)
{
	int fd;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_GPIO_SET_DIR, dir) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

int ra_gpio_read_bit(int idx, int *value)
{
	int fd, req;

	*value = 0;
        fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (0L <= idx && idx < RALINK_GPIO_DATA_LEN)
		req = RALINK_GPIO_READ_BIT | (idx << RALINK_GPIO_DATA_LEN);
	else {
		close(fd);
		printf("gpio_read_bit: index %d out of range\n", idx);
		return -1;
	}
	if (ioctl(fd, req, value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int 
ra_gpio_read(int idx)
{
	int fd, req, value;

	value = 0;
	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (0L <= idx && idx < RALINK_GPIO_DATA_LEN)
		req = RALINK_GPIO_READ_BIT | (idx << RALINK_GPIO_DATA_LEN);
	else {
		close(fd);
		printf("gpio_read_bit: index %d out of range\n", idx);
		return -1;
	}
	if (ioctl(fd, req, &value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	return value;
}

int ra_gpio_write_bit(int idx, int value)
{
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	value &= 1;
	if (0L <= idx && idx < RALINK_GPIO_DATA_LEN)
		req = RALINK_GPIO_WRITE_BIT | (idx << RALINK_GPIO_DATA_LEN);
	else {
		close(fd);
		printf("gpio_write_bit: index %d out of range\n", idx);
		return -1;
	}
	if (ioctl(fd, req, value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

int ra_gpio_read_byte(int idx, int *value)
{
	int fd, req;

	*value = 0;
	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (0L <= idx && idx < RALINK_GPIO_DATA_LEN/8)
		req = RALINK_GPIO_READ_BYTE | (idx << RALINK_GPIO_DATA_LEN);
	else {
		close(fd);
		printf("gpio_read_byte: index %d out of range\n", idx);
		return -1;
	}
	if (ioctl(fd, req, value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int ra_gpio_write_byte(int idx, int value)
{
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	value &= 0xFF;
	if (0L <= idx && idx < RALINK_GPIO_DATA_LEN/8)
		req = RALINK_GPIO_WRITE_BYTE | (idx << RALINK_GPIO_DATA_LEN);
	else {
		close(fd);
		printf("gpio_write_byte: index %d out of range\n", idx);
		return -1;
	}
	if (ioctl(fd, req, value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int ra_gpio_read_int(int *value)
{
	int fd;

	*value = 0;
	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_GPIO_READ, value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return value;
}

int ra_gpio_write_int(int value)
{
	int fd;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_GPIO_WRITE_INT, value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int ra_gpio_enb_irq(void)
{
	int fd;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_GPIO_ENABLE_INTP) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int ra_gpio_dis_irq(void)
{
        int fd;

        fd = open(GPIO_DEV, O_RDONLY);
        if (fd < 0) {
                perror(GPIO_DEV);
                return -1;
        }
        if (ioctl(fd, RALINK_GPIO_DISABLE_INTP) < 0) {
                perror("ioctl");
                close(fd);
                return -1;
        }
        close(fd);
        return 0;
}

int ra_gpio_reg_info(int gpio_num)
{
	int fd;

	ralink_gpio_reg_info info;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	info.pid = getpid();
	if((gpio_num < 0) || (gpio_num > 24))	// 0830 add
		gpio_num = 0;
	info.irq = gpio_num;
	if (ioctl(fd, RALINK_GPIO_REG_IRQ, &info) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

void ra_gpio_write_spec(bit_idx, flag)
{
	//set gpio direction to output
	//ra_gpio_set_dir(RALINK_GPIO_DIR_ALLOUT);

	//ra_gpio_write_int(RALINK_GPIO_DATA_MASK);

	ra_gpio_write_bit(bit_idx, flag);
}

void 
ra_gpio_init()
{
	//set gpio direction to output
	ra_gpio_set_dir(0x80);	// bit 7 as power led

	//turn off all gpio (set all to high)
	//ra_gpio_write_int(RALINK_GPIO_DATA_MASK);

	//ra_gpio_write_bit(RTN12_PWRLED_GPIO_IRQ, LED_OFF);
	//ra_gpio_write_bit(RTN12_RESETDF_GPIO_IRQ, LED_OFF);
	//ra_gpio_write_bit(RTN12_EZSETUP_GPIO_IRQ, LED_OFF);
}

static void
alarmtimer(unsigned long sec,unsigned long usec)
{
	itv.it_value.tv_sec  = sec;
	itv.it_value.tv_usec = usec;
	itv.it_interval = itv.it_value;
	setitimer(ITIMER_REAL, &itv, NULL);
}

int	pre_sw_mode=0, sw_mode=0;
int	flag_sw_mode=0;
int	tmp_sw_mode=0;
int	count_stable=0;
//struct timeval tv_ref, tv_now;

void 
sw_mode_check()
{
	int val;

	pre_sw_mode = sw_mode;
	ra_gpio_read_int(&val);
	//printf("sw_mode val is %x\n", val);

	if(val & (1 << RTN12_SW1))
		sw_mode = 1;
	if(val & (1 << RTN12_SW2))
		sw_mode = 2;
	if(val & (1 << RTN12_SW3))
		sw_mode = 3;

	if(sw_mode != pre_sw_mode)
	{
		char tmp[10];
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%d", sw_mode);
		nvram_set("sw_mode", tmp);

		if (nvram_invmatch("sw_mode", nvram_safe_get("sw_mode_ex")) && nvram_invmatch("asus_mfg", "1"))
		{	/* different from working mode */
//			gettimeofday(&tv_ref, NULL);
			flag_sw_mode=1;
			count_stable=0;
			tmp_sw_mode=sw_mode;
		}
		else
			flag_sw_mode=0;
	}
	else if (flag_sw_mode==1 && nvram_invmatch("asus_mfg", "1"))
	{
		if (tmp_sw_mode==sw_mode)
		{
			if (++count_stable > 4)	// stable for more than 5 seconds
			{
				fprintf(stderr, "Reboot to switch sw mdoe...\n");
				flag_sw_mode=0;
				kill(1, SIGTERM);

//				gettimeofday(&tv_now, NULL);
//				if ((tv_now.tv_usec - tv_ref.tv_usec) >= 0)
//					fprintf(stderr, "sec: %ld, msec: %ld\n", tv_now.tv_sec-tv_ref.tv_sec, (tv_now.tv_usec-tv_ref.tv_usec)/1000);
//				else
//					fprintf(stderr, "sec: %ld, msec: %ld\n", tv_now.tv_sec-tv_ref.tv_sec-1, (1000000+tv_now.tv_usec-tv_ref.tv_usec)/1000);
			}
		}
		else
			flag_sw_mode=0;
	}
	//printf("sw mode is %d\n", sw_mode);
}

#if 0
#define DNS_ALIVE	0
#define UPNP_ALIVE	1
int palive = 0;

void 
process_check()
{
	char line[128];
	FILE *fp;

	unlink("/tmp/tmp_ps");
	system("ps > /tmp/tmp_ps");
	if((fp = fopen("/tmp/tmp_ps", "r"))!=NULL)
	{
		while(!feof(fp))
		{
			if(fgets(line, sizeof(line), fp) == NULL)
				break;
			if(strstr(line, "dproxy"))
				palive |= (1 << DNS_ALIVE);
			if(strstr(line, "upnpd"))
				palive |= (1 << UPNP_ALIVE);
			memset(line, 0, sizeof(line));
		}
		fclose(fp);
	}
	printf("[wd] process check:%d\n", palive);	// tmp test

	if(((1<<DNS_ALIVE) & palive) == 0)
	{
		printf("**[wd] restart dns\n");
		stop_dns();
		start_dns();
	}
/*
        if(((1<<UPNP_ALIVE) & palive) == 0)
        {
                printf("**[wd] restart upnpd\n");
                stop_upnp();
                start_upnp();
        }
*/
	unlink("/tmp/tmp_ps");
}
#endif

void btn_check(void)
{
#ifdef BTN_SETUP
#if 0	// use WPS
	if (btn_pressed_flag)	// seems not work here
	{
		btn_pressed_flag++;

		if(btn_pressed_flag==8)
		{
			start_ots();/* Cherry Cho unmarked for using EZsetup in 2007/2/12. */
		}
		else if(btn_pressed_flag==10)
		{		
			btn_pressed_flag=0;
			btn_pressed_setup=BTNSETUP_START;
			btn_count_setup=0;
			alarmtimer(0, RUSHURGENT_PERIOD);
		}
	}
#endif
	if (btn_pressed_setup==BTNSETUP_NONE)
	{
#endif

	//if (!gpio_read("/dev/gpio/in", BTN_RESET))
	if (!ra_gpio_read(RTN12_RESETDF_GPIO_IRQ))	// reset button is on low phase
	{
	/*--------------- Add BTN_RST MFG test ------------------------*/
		if (nvram_invmatch("asus_mfg", "0"))
		{
			nvram_set("btn_rst", "1");
		}
		else
		{
			if (!btn_pressed)
			{
				btn_pressed=1;
				btn_count=0;
				alarmtimer(0, URGENT_PERIOD);
			}
			else
			{	/* Whenever it is pushed steady */
				if( ++btn_count > RESET_WAIT_COUNT )
				{
					btn_pressed=2;
					stop_wan();	// ham 0414
				}
				if (btn_pressed==2)
				{
				/* 0123456789 */
				/* 0011100111 */
					if ((btn_count%10)<1 || ((btn_count%10)>4 && (btn_count%10)<7))
						LED_CONTROL(LED_POWER, LED_OFF);
					else
						LED_CONTROL(LED_POWER, LED_ON);
				}
			}
		} //end BTN_RST MFG test
	}
	else
	{
		if(btn_pressed==1)
		{
			btn_count = 0;
			btn_pressed=0;
			LED_CONTROL(LED_POWER, LED_ON);
			alarmtimer(NORMAL_PERIOD, 0);
		}
		else if(btn_pressed==2)
		{
			LED_CONTROL(LED_POWER, LED_OFF);
			alarmtimer(0, 0);
			eval("erase", "/dev/mtd1");
			kill(1, SIGTERM);
		}
	}

#ifdef BTN_SETUP
	}
	if (btn_pressed!=0) return;

	if (btn_pressed_setup<BTNSETUP_START)
	{
		//if (!gpio_read("/dev/gpio/in", BTN_SETUP))
		if (!no_need_to_start_wps && !ra_gpio_read(RTN12_EZSETUP_GPIO_IRQ))
		{
			/* Add BTN_EZ MFG test */
			if (nvram_invmatch("asus_mfg", "0"))
			{
				nvram_set("btn_ez", "1");
			}
			else
			{
				if (btn_pressed_setup==BTNSETUP_NONE)
				{
					btn_pressed_setup=BTNSETUP_DETECT;
					btn_count_setup=0;
					alarmtimer(0, RUSHURGENT_PERIOD);
				}
				else
				{	/* Whenever it is pushed steady */
					if( ++btn_count_setup > SETUP_WAIT_COUNT )
					{
						if (nvram_match("sw_mode_ex", "1"))
						{
#ifndef W7_LOGO
							nvram_set("wps_triggered", "1");	// psp fix
#endif

							btn_pressed_setup=BTNSETUP_START;
							btn_count_setup=0;
							btn_count_setup_second=0;
							//start_ots();
							start_wsc_pbc();
							wsc_timeout=120*20;
							nvram_set("stop_wps_led", "0");
							dog_stop_wan = 1;	// ham 0414
							printf("\n**watchdog stop wan\n");
							stop_wan();
						}
						else
						{
/*
							btn_pressed_setup=BTNSETUP_START;
							btn_count_setup=0;
							btn_count_setup_second=0;
							sta_wps_pbc();
							wsc_timeout=120*20;
							nvram_set("stop_wps_led", "0");

							if (nvram_match("wan_route_X", "IP_Routed"))
							{
								dog_stop_wan = 1;
								printf("\n**watchdog stop wan\n");
								stop_wan();
							}
*/
                                                        if (!sta_wps_pbc())
                                                        {
                                                                btn_pressed_setup=BTNSETUP_START;
                                                                btn_count_setup=0;
                                                                btn_count_setup_second=0;
                                                                wsc_timeout=120*20;
                                                                nvram_set("stop_wps_led", "0");

                                                                if (nvram_match("wan_route_X", "IP_Routed"))
                                                                {
                                                                        dog_stop_wan = 1;
                                                                        printf("\n**watchdog stop wan\n");
                                                                        stop_wan();
                                                                }
                                                        }
						}	
					}
				}
			} //end BTN_EZ MFG test
		} 
		else if(btn_pressed_setup==BTNSETUP_DETECT)
		{
			btn_pressed_setup = BTNSETUP_NONE;
			btn_count_setup = 0;
			LED_CONTROL(LED_POWER, LED_ON);
#ifndef WL330GE
			alarmtimer(NORMAL_PERIOD, 0);
#else
			alarmtimer(0, QUICK_PERIOD);
#endif
		}
	}
	else 
	{
		if (!no_need_to_start_wps && !ra_gpio_read(RTN12_EZSETUP_GPIO_IRQ))
		{
			/* Whenever it is pushed steady, again... */
			if( ++btn_count_setup_second > SETUP_WAIT_COUNT )
			{
				if (nvram_match("sw_mode_ex", "1"))
				{
#ifndef W7_LOGO
					nvram_set("wps_triggered", "1");	// psp fix
#endif
					fprintf(stderr, "pushed again...\n");
					btn_pressed_setup=BTNSETUP_START;
					btn_count_setup_second=0;
					//start_ots();
					start_wsc_pbc();
					wsc_timeout=121*20;
					nvram_set("stop_wps_led", "0");
				}
				else
				{
					fprintf(stderr, "pushed again... do nothing...\n");
/*
					btn_pressed_setup=BTNSETUP_START;
					btn_count_setup_second=0;
					sta_wps_pbc();
					wsc_timeout=121*20;
					nvram_set("stop_wps_led", "0");
*/
				}
			}
		}

		if (!no_need_to_start_wps && (--wsc_timeout==0 || nvram_match("stop_wps_led", "1")))
		{
			if (nvram_match("sw_mode_ex", "1"))	// Gateway
			{
#ifndef W7_LOGO
				nvram_set("wps_triggered", "1");// psp fix
#endif
				wsc_timeout=0;
				nvram_set("stop_wps_led", "0");

				btn_pressed_setup = BTNSETUP_NONE;
				btn_count_setup = 0;
				LED_CONTROL(LED_POWER, LED_ON);
				alarmtimer(NORMAL_PERIOD, 0);
#ifndef W7_LOGO
				stop_wsc();			// psp fix
				nvram_set("wps_enable", "0");	// psp fix
#endif
			}
			else if (nvram_match("sw_mode_ex", "2"))// URE
			{
				wsc_timeout=0;
				nvram_set("stop_wps_led", "0");

				btn_pressed_setup = BTNSETUP_NONE;
				btn_count_setup = 0;
				LED_CONTROL(LED_POWER, LED_ON);
				alarmtimer(NORMAL_PERIOD, 0);
				sta_wps_stop();
				nvram_set("wps_enable", "0");
			}
			else if (nvram_match("sw_mode_ex", "3"))// AP
			{
				wsc_timeout=0;
				nvram_set("stop_wps_led", "0");

				btn_pressed_setup = BTNSETUP_NONE;
				btn_count_setup = 0;
				LED_CONTROL(LED_POWER, LED_ON);
				alarmtimer(NORMAL_PERIOD, 0);
				ap_wps_stop();
				nvram_set("wps_enable", "0");
			}

			if(dog_stop_wan == 1)	// ham 0414
			{
				char *startWan_argv[] = {"startWan", NULL};
				char *wan_proto;
				int wpid, status;

				dog_stop_wan = 0;
				printf("\n**watchdog start wan\n");
				//start_wan();

				wan_proto = nvram_get("wan_proto");
				printf("wan proto is %s\n", wan_proto);

				if (!wan_proto || !strcmp(wan_proto, "disabled"))
				{
					printf("current wan is disabled\n");
				}
				else if (strcmp(wan_proto, "static") == 0)
				{
					start_wan();
				}
				else	// udhcpc
				{
					_eval(startWan_argv, NULL, 0, &wpid);	// avoid zombie
					wait(&status);
				}
			}

			return;
		}

		++btn_count_setup;
		btn_count_setup = (btn_count_setup%20);

		/* 0123456789 */
		/* 1010101010 */
#if 0		// no need. use WPS.
		if(btn_count_setup==0 && nvram_match("bs_mode", "1"))
		{
			btn_pressed_setup = BTNSETUP_NONE;
			btn_count_setup = 0;
			LED_CONTROL(LED_POWER, LED_ON);
#ifndef WL330GE
			alarmtimer(NORMAL_PERIOD, 0);
#else
			alarmtimer(0, QUICK_PERIOD);
#endif
			nvram_set("bs_mode", "");
//			btn_stage=0;	// no need. use WPS.
		}
		else
#endif
		{
#if 0
			if (btn_stage) // second stage with different led
			{
				if ((btn_count_setup%2)==0) LED_CONTROL(LED_POWER, LED_ON);
				else LED_CONTROL(LED_POWER, LED_OFF);
			}
			else
#endif
			{
				if ((btn_count_setup%2)==0&&(btn_count_setup>10)) LED_CONTROL(LED_POWER, LED_ON);
				else LED_CONTROL(LED_POWER, LED_OFF);
			}
		}
	}
#endif
}

int
kill_pidfile_sb(char *pidfile, int sig)
{
	FILE *fp = fopen(pidfile, "r");
	char buf[256];
	extern errno;

	if (fp && fgets(buf, sizeof(buf), fp)) {
		pid_t pid = strtoul(buf, NULL, 0);
		fclose(fp);
		return kill(pid, sig);
	} else
		return errno;
}

void refresh_ntpc(void)
{
	printf("refresh ntp client\n");
	eval("killall","ntpclient");
	kill_pidfile_sb("/var/run/ntp.pid", SIGUSR1);	
}

int ntp_first_refresh = 1;

int ntp_timesync(void)
{
	time_t now;
	struct tm tm;	
	struct tm gm, local;
	struct timezone tz;

	if (sync_interval!=-1)
	{
		sync_interval--;

		if (sync_interval==0)
		{
			/* Update kernel timezone */
			setenv("TZ", nvram_safe_get("time_zone_x"), 1);
			time(&now);
			gmtime_r(&now, &gm);
			localtime_r(&now, &local);
			tz.tz_minuteswest = (mktime(&gm) - mktime(&local)) / 60;
			settimeofday(NULL, &tz);
			memcpy(&tm, localtime(&now), sizeof(struct tm));

		   	if (tm.tm_year>100) // More than 2000 
			{	 
				sync_interval=60*60/5;
				logmessage("ntp client", "time is synchronized to %s", nvram_safe_get("ntp_servers"));
				//printf("## [%d] time is synchronized to %s\n", ntp_first_refresh, nvram_safe_get("ntp_servers"));
				if(ntp_first_refresh != 1)
				{
					stop_upnp();
					start_upnp();
				}
				else
					printf("\nwe don't do upnp refresh at first ntp_refresh\n");

				ntp_first_refresh = 0;
			}	
			else sync_interval=6;

			refresh_ntpc();	
		}
	}
}

enum 
{
	URLACTIVE=0,
	WEBACTIVE,
	RADIOACTIVE,
	ACTIVEITEMS
} ACTIVE;

int svcStatus[ACTIVEITEMS] = { -1, -1, -1};
int extStatus[ACTIVEITEMS] = { 0, 0, 0};
char svcDate[ACTIVEITEMS][10];
char *svcTime[ACTIVEITEMS][20];

#define DAYSTART (0)
#define DAYEND (60*60*23+60*59+59) //86399

int timecheck_item(char *activeDate, char *activeTime)
{
	int current, active, activeTimeStart, activeTimeEnd, i;
	time_t now;
	struct tm *tm;

	time(&now);
	tm = localtime(&now);
	current = tm->tm_hour*60 + tm->tm_min;

	active=0;

	//fprintf("[wd] timecheck, active: [date]:%s, [time]:%s\n", activeDate, activeTime);

	activeTimeStart=((activeTime[0]-'0')*10+(activeTime[1]-'0'))*60 + (activeTime[2]-'0')*10 + (activeTime[3]-'0');
		
	activeTimeEnd=((activeTime[4]-'0')*10+(activeTime[5]-'0'))*60 + (activeTime[6]-'0')*10 + (activeTime[7]-'0');

	if (activeDate[tm->tm_wday] == '1')
	{
		if (activeTimeEnd<activeTimeStart)
		{
			if ((current>=activeTimeStart && current<=DAYEND) ||
			   (current>=DAYSTART && current<=activeTimeEnd))
			{
				active = 1;
			}
			else
			{
				active = 0;
			}
		}
		else
		{
			if (current>=activeTimeStart &&
			current<=activeTimeEnd)
			{
				active = 1;
			}
			else
			{
				active = 0;
			}
		}
	}
	return(active);
}

/* Check for time-dependent service 	*/
/* 1. URL filter 			*/
/* 2. WEB Camera Security filter 	*/

int svc_timecheck(void)
{
	int activeFlag, activeNow;

	//printf("[wdog] svc timeCheck \n");
	activeFlag = 0;

	/* Initialize */
	if (svcStatus[URLACTIVE]==-1 && nvram_invmatch("url_enable_x", "0"))
	{
		strcpy(svcDate[URLACTIVE], nvram_safe_get("url_date_x"));
		strcpy(svcTime[URLACTIVE], nvram_safe_get("url_time_x"));
		svcStatus[URLACTIVE] = -2;
	}

	if (svcStatus[URLACTIVE]!=-1)
	{
		activeNow = timecheck_item(svcDate[URLACTIVE], svcTime[URLACTIVE]);
		if (activeNow!=svcStatus[URLACTIVE])
		{
			fprintf(stderr,  "[wd] url time change: %d, restart dns\n", activeNow);
			svcStatus[URLACTIVE] = activeNow;
			//stop_dns();
			//start_dns();
			restart_dns();
		}
	}

	if (svcStatus[WEBACTIVE]==-1 && 
		nvram_invmatch("usb_webenable_x", "0") &&
		nvram_invmatch("usb_websecurity_x", "0"))
	{
		strcpy(svcDate[WEBACTIVE], nvram_safe_get("usb_websecurity_date_x"));
		strcpy(svcTime[WEBACTIVE], nvram_safe_get("usb_websecurity_time_x"));
		svcStatus[WEBACTIVE] = -2;
	}

	if (svcStatus[WEBACTIVE]!=-1)
	{
		activeNow = timecheck_item(svcDate[WEBACTIVE], svcTime[WEBACTIVE]);
		if (activeNow!=svcStatus[WEBACTIVE])
		{
			svcStatus[WEBACTIVE] = activeNow;

			//if (!notice_rcamd(svcStatus[WEBACTIVE])) svcStatus[WEBACTIVE]=-1;
		}
	}

	if (svcStatus[RADIOACTIVE]==-1 && nvram_invmatch("wl_radio_x", "0"))
	{
		strcpy(svcDate[RADIOACTIVE], nvram_safe_get("WL_radio_date_x"));
		strcpy(svcTime[RADIOACTIVE], nvram_safe_get("WL_radio_time_x"));
		svcStatus[RADIOACTIVE] = -2;
	}

	if (svcStatus[RADIOACTIVE]!=-1)
	{
		activeNow = timecheck_item(svcDate[RADIOACTIVE], svcTime[RADIOACTIVE]);
		if (activeNow!=svcStatus[RADIOACTIVE])
		{
			svcStatus[RADIOACTIVE] = activeNow;

			if (activeNow) 
			{
				nvram_set("sw_radio", "1");
				eval("iwpriv", "ra0", "set", "RadioOn=1");
				eval("iwpriv", "ra0", "set", "TxPower=100");
			}
			else
			{
				nvram_set("sw_radio", "0");
				eval("iwpriv", "ra0", "set", "TxPower=0");
				eval("iwpriv", "ra0", "set", "RadioOn=0");
			}
		}
	}

	return 0;
}

/* Sometimes, httpd becomes inaccessible, try to re-run it */
int http_processcheck(void)
{
	char http_cmd[32];
	char buf[256];

	sprintf(http_cmd, "http://127.0.0.1/");
	if (	!http_check(http_cmd, buf, sizeof(buf), 0) &&
		nvram_invmatch("ap_scanning", "1") &&
		nvram_invmatch("updating", "1") &&
		nvram_invmatch("ap_selecting", "1") &&
#ifdef ASUS_DDNS //2007.03.27 Yau add for prevent httpd die when doing hostname check
		nvram_invmatch("httpd_check_ddns", "1") &&
#endif
		nvram_invmatch("uploading", "1")
	)
	{
		printf("http rerun\n");
		//kill_pidfile("/var/run/httpd.pid");
		if(nvram_match("httpd_die_reboot", "1")){
			nvram_set("httpd_die_reboot", "");
			eval("reboot");
		}
		start_httpd();
	}
	return 0;
}

static void catch_sig(int sig)
{
	if (sig == SIGUSR1)
	{
		//dprintf("Catch Reset to Default Signal\n");
		fprintf(stderr, "[wd] Catch Reset to Default Signal 1\n");
	}
	else if (sig == SIGUSR2)
	{
		fprintf(stderr, "[wd] Catch Reset to Default Signal 2\n");
	}
	else if (sig == SIGTSTP && nvram_match("sw_mode_ex", "1"))
	{
		if(nvram_match("wps_oob_flag", "1"))	// 0901 w7 update
		{
#ifndef W7_LOGO
			nvram_set("wps_triggered", "1");	// psp fix
#endif
			nvram_set("wps_oob_flag", "0");	// 0901 w7 update
			wsc_timeout=10;
			btn_pressed_setup = BTNSETUP_NONE;
			btn_count_setup = 0;
			LED_CONTROL(LED_POWER, LED_ON);
			alarmtimer(NORMAL_PERIOD, 0);

			btn_pressed_setup=BTNSETUP_START;
			btn_count_setup=0;
			wps_oob();
			wsc_timeout=120*20;
			alarmtimer(0, RUSHURGENT_PERIOD);
		}
		//2009.01 magic{
		else if(nvram_match("wps_start_flag", "3")){	// let the SW push button be the same with the HW push button.
#ifndef W7_LOGO
			nvram_set("wps_triggered", "1");	// psp fix
#endif
			nvram_set("wps_start_flag", "0");
			alarmtimer(NORMAL_PERIOD, 0); //2009.01 magic
			btn_pressed_setup=BTNSETUP_START;
			btn_count_setup=0;
			btn_count_setup_second=0;
			start_wsc_pbc(); //2009.01 magic
			wsc_timeout=120*20;
			alarmtimer(0, RUSHURGENT_PERIOD); //2009.01 magic
			nvram_set("stop_wps_led", "0");
		}
		//2009.01 magic}
		else if (nvram_match("wps_enable", "0"))
		{
#ifndef W7_LOGO
			nvram_set("wps_triggered", "1");	// psp fix
#endif
			wsc_timeout=1;
			btn_pressed_setup = BTNSETUP_NONE;
			btn_count_setup = 0;
			LED_CONTROL(LED_POWER, LED_ON);
			alarmtimer(NORMAL_PERIOD, 0);
			stop_wsc();
		}
		else if(nvram_match("wps_start_flag", "1"))
		{
#ifndef W7_LOGO
			nvram_set("wps_triggered", "1");	// psp fix
#endif
			nvram_set("wps_start_flag", "0");
			alarmtimer(NORMAL_PERIOD, 0);
			btn_pressed_setup=BTNSETUP_START;
			btn_count_setup=0;
			start_wsc();
			wsc_timeout=120*20;
			alarmtimer(0, RUSHURGENT_PERIOD);
		}
//		else
		else if(nvram_match("wps_start_flag", "2"))
		{
#ifndef W7_LOGO
			nvram_set("wps_triggered", "1");	// psp fix
#endif
			nvram_set("wps_start_flag", "0");
			alarmtimer(NORMAL_PERIOD, 0);
			btn_pressed_setup=BTNSETUP_START;
			btn_count_setup=0;

			if (nvram_match("wps_mode", "1"))
			{
				if (nvram_match("wps_pin_web", ""))
					wps_pin(0);
				else
					wps_pin(atoi(nvram_safe_get("wps_pin_web")));
			}
			else
				wps_pbc();

			wsc_timeout=120*20;
			alarmtimer(0, RUSHURGENT_PERIOD);
		}
	}
}

void sta_check(void)
{
	return;
}

int chk_dns = 0;
/* wathchdog is runned in NORMAL_PERIOD, 1 seconds
 * check in each NORMAL_PERIOD
 *	1. button
 *
 * check in each NORAML_PERIOD*5
 *
 *      1. ntptime, 
 *      2. time-dependent service
 *      3. http-process
 *      4. usb hotplug status
 */
void watchdog(void)
{
	time_t now;
	char *dns_list;

	/* check if swap time for mem issue */
	if(strcmp(nvram_safe_get("hotplug_usb_mem_cric"), "0")!=0)
		return;

	/* handle button */
	btn_check();

	sw_mode_check();

	/* if timer is set to less than 1 sec, then bypass the following */
#ifndef WL330GE
	if (itv.it_value.tv_sec==0) return;
#endif

	if (nvram_match("asus_mfg", "1"))
	{
		eval("killall", "ntp");
		eval("killall", "ntpclient");
		eval("killall", "udhcpc");
		eval("killall", "ots");
		stop_logger();
		stop_upnp();	// it may cause upnp cannot run
		stop_dhcpd();
		stop_dns();
		stop_wsc();
		stop_lltd();	// 1017 add
		nvram_set("asus_mfg", "2");
//		eval("ated");
		char* utelnetd_argv[] = {"utelnetd", NULL};
		pid_t pid;
		_eval(utelnetd_argv, NULL, 0, &pid);
	}

	// reboot signal checking
	if(nvram_match("reboot", "1"))
	{
		printf("[wd] nvram match reboot\n");
		reboot_count++;
		if(reboot_count>=2) kill(1, SIGTERM);
	}
	else if(nvram_match("reboot", "0")) return;

	/* dns chk */
	if(nvram_match("wan_dnsenable_x", "1") && chk_dns == 0)
	{
		dns_list = nvram_safe_get("wan0_dns");
		if(!dns_list || (strlen(dns_list) == 0))
		{
			dns_list = nvram_safe_get("wanx_dns");
		}
		if(dns_list && strlen(dns_list) > 0)
		{
			chk_dns = 1;
			update_resolvconf();
		}
	}
#ifdef WL330GE
	watchdog_period = (watchdog_period+1) % 40;
#else
	watchdog_period = (watchdog_period+1) %10;
#endif

	if (watchdog_period) return;

	time(&now);

	ddns_timer = (ddns_timer+1) % 4320;

#ifdef BTN_SETUP
	if (btn_pressed_setup>=BTNSETUP_START) return;
#endif

	if(nvram_invmatch("asus_mfg", "0"))
		return;

        if(delay_sec < 10)
        {
                ++delay_sec;
                if(delay_sec == upnp_delay_time)
                {
			if(strcmp(nvram_safe_get("upnp_running"), "1") != 0)	// for lan start
			{
        			char *wan_proto = nvram_safe_get("wan_proto");
        			if (strcmp(wan_proto, "pppoe") == 0 ||
            			strcmp(wan_proto, "pptp") == 0 ||
            			strcmp(wan_proto, "l2tp") == 0)
        			{
                        		printf("\n[wd] Delay run upnp\n"); 
                        		stop_upnp();
                        		start_upnp();
				}
			}

			//process_check();
                }
		if(delay_sec == usb_chk_time)
		{
			printf("[wd] chk usb plugin\n");	// tmp test
			system("kill -SIGTTIN 1");
		}
        }

/*
	if(delay_sec > upnp_delay_time)
	{
		if(palive == 0)
			process_check();
	}
*/
	if(is_phyconnected() == 0 || !nvram_safe_get("wan_ipaddr_t"))   // 1022 add
		return;

//	if(nvram_match("wan_nat_X", "1"))
	if(nvram_match("wan_route_X", "IP_Routed"))
	{
	/* sync time to ntp server if necessary */
		if(nvram_invmatch("wan0_dns", ""))
			ntp_timesync();

		if (nvram_invmatch("router_disable", "1") &&
		nvram_match("ddns_enable_x", "1") &&
		(nvram_get("wan_ipaddr_t")!=NULL && nvram_invmatch("wan_ipaddr_t", "")) &&
		(nvram_match("ddns_updated", "0")||ddns_timer==0))
		{
			start_ddns();
		}
	}

	/* check for time-dependent services */
	svc_timecheck();

	/* http server check */
#ifdef WL330GE
	if (	nvram_invmatch("ap_scanning", "1") &&
		nvram_invmatch("updating", "1") &&
		nvram_invmatch("ap_selecting", "1") &&
		nvram_invmatch("uploading", "1")
	)
#endif
	http_processcheck();

#ifdef USB_SUPPORT

	/* web cam process */

	/* storage process */
	if (nvram_invmatch("usb_storage_device", ""))
	{
#ifdef HOT_USB
		hotplug_usb_mass(nvram_safe_get("usb_storage_device"));
		nvram_set("usb_storage_device", "");
#endif	// HOT_USB
	}
#endif

#ifdef CDMA
	/* CDMA_DOWN = 99, none
	 * CDMA_DOWN = 1, currently down
	 * CDMA_DOWN = 2, currently up
	 * CDMA_DOWN = 0, currently trying to connect
	 */
	if (nvram_match("cdma_down", "1"))
	{
		logmessage("CDMA client", "usb modem is down(%d).", cdma_down);

		++cdma_down;
		cdma_connect=0;

                if(cdma_down==1)
                {
			printf("restart wan\n");	// tmp test
                        stop_wan();
                        start_wan();
                }
                else if(cdma_down >= 2) // 20 seconds timeout for retry
                {
                        cdma_down=0;
                }
	}
	else if(nvram_match("cdma_down", "0"))
	{
                logmessage("CDMA client", "USB Modem: Try to connect(%d)!", cdma_connect+1); // HSDPA
                cdma_down=0;
                ++cdma_connect;

                if(cdma_connect >= 2) /* 20 seconds timeout for connecting */ // HSDPA
                {
                        nvram_set("cdma_down", "1");
                }
	}
        else if (nvram_match("cdma_down", "3"))
        {
                eval("killall", "chat");
                /* eval("killall", "pppd"); */
        }
        else
        {
                cdma_down=0;
                cdma_connect=0;
        }
#endif
}

int
gpio_main(int ledin)
{
#ifdef BTN_SETUP
	//printf("BTN:%d,%d", gpio_read("/dev/gpio/in", BTN_RESET), gpio_read("/dev/gpio/in", BTN_SETUP));
	printf("BTN:%d,%d", ra_gpio_read(BTN_RESET), ra_gpio_read(RTN12_EZSETUP_GPIO_IRQ));
#else
	//printf("BTN:%d,0", gpio_read("/dev/gpio/in", BTN_RESET));
	printf("BTN:%d,0", ra_gpio_read(BTN_RESET));
#endif
}

int update_resolvconf(void);
extern int chk_flag;

int 
watchdog_main(int argc, char *argv[])
{
	FILE *fp;
	char *proto;

#ifdef REMOVE
	/* Run it under background */
	switch (fork()) {
	case -1:
		exit(0);
		break;
	case 0:	
		// start in a new session
		(void) setsid();
		break;
	default:
		/* parent process should just die */
		_exit(0);
	}
#endif

	/* write pid */
	if ((fp=fopen("/var/run/watchdog.pid", "w"))!=NULL)
	{
		fprintf(fp, "%d", getpid());
		fclose(fp);
	}

	/* Start GPIO function */
	ra_gpio_init();

	LED_CONTROL(RTN12_PWRLED_GPIO_IRQ, LED_ON);

	/* set the signal handler */
	signal(SIGUSR1, catch_sig);
	signal(SIGUSR2, catch_sig);
	signal(SIGTSTP, catch_sig);
	signal(SIGALRM, watchdog);

	nvram_set("btn_rst", "0");      // ate test init
	nvram_set("btn_ez", "0");       // ate test init

	nvram_set("sw_radio", nvram_safe_get("wl_radio_x"));
	nvram_set("stop_wps_led", "0");

	if ((	nvram_match("WL_auth_mode", "shared") ||
		nvram_match("WL_auth_mode", "wpa") ||
		nvram_match("WL_auth_mode", "wpa2") ||
		nvram_match("WL_auth_mode", "radius")) &&
		nvram_match("sw_mode_ex", "1"))
		no_need_to_start_wps=1;

	if (nvram_match("wl_radio_x", "0"))	// Jiahao 0424
		radio_main(0);

	/* Start sync time */
	sync_interval=1;
	start_ntpc();

	/* decide delay time */
	proto = nvram_safe_get("wan_proto");
	if((strcmp(proto, "pptp") == 0) || (strcmp(proto, "l2tp") == 0))
		upnp_delay_time = 8;
		//upnp_delay_time = 3;
	else
		upnp_delay_time = 1;

	printf("upnp will be delayed %d secs\n", upnp_delay_time * 10);	// tmp test

	if(nvram_match("ots_running", "0"))
		start_ots();

#ifdef WL330GE
	RSSI_TH_A=atoi(nvram_safe_get("threshold_a"));
	RSSI_TH_B=atoi(nvram_safe_get("threshold_b"));
	RSSI_TH_C=atoi(nvram_safe_get("threshold_c"));
	RSSI_TH_D=atoi(nvram_safe_get("threshold_d"));
	RSSI_TH_E=atoi(nvram_safe_get("threshold_e"));
#endif

	/* set timer */
#ifdef WL330GE
	alarmtimer(0, QUICK_PERIOD);
#else
	alarmtimer(NORMAL_PERIOD, 0);
#endif

#ifdef WL330GE
	nvram_set("ap_scanning", "0");
	nvram_set("ap_selecting", "0");
	nvram_set("updating", "0");
	nvram_set("RESET_PRESSED", "0");
	nvram_set("more_mem", "0");	// save memory for ATE function
	nvram_set("uploading", "0");
	rssi_g=rssi_g_old=-32768;
#endif

	ddns_timer=1;

	if (//	nvram_match("wan_nat_X", "1") &&
		nvram_match("wan_route_X", "IP_Routed") &&
		(nvram_match("wan0_proto", "pppoe") || nvram_match("wan0_proto", "pptp"))
	)
	{
		//char *ping_argv[] = { "ping", "140.113.1.1", NULL};
		//int pid;
		//_eval(ping_argv, NULL, 0, &pid);
		system("ping 140.113.1.1 -c 3");
		//printf("[wd] ping 140\n");	// tmp test
	}

	/* Most of time it goes to sleep */
	while(1)
	{
		pause();
	}

	return 0;
}

#define LED_RADIO GPIO0

int radio_main(int ctrl)
{
	if (!ctrl)
	{
		nvram_set("sw_radio", "0");
		eval("iwpriv", "ra0", "set", "TxPower=0");
		eval("iwpriv", "ra0", "set", "RadioOn=0");
	}
	else
	{
		nvram_set("sw_radio", "1");
		eval("iwpriv", "ra0", "set", "RadioOn=1");
		eval("iwpriv", "ra0", "set", "TxPower=100");
	}
}
