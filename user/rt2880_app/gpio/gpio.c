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
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************
 *
 * $Id: gpio.c,v 1.11 2008-07-14 06:40:46 winfred Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "linux/ralink_gpio.h"

int wr_gpio_num = 11;
int wr_gpio_flag = 0;

int gpio_set_dir(int dir)
{
	int fd;

	fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
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

int gpio_set_dir_out(int dir)
{
        int fd;

        fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
                perror(GPIO_DEV);
                return -1;
        }
        if (ioctl(fd, RALINK_GPIO_SET_DIR_OUT, dir) < 0) {
                perror("ioctl");
                close(fd);
                return -1;
        }
        close(fd);
        return 0;
}

int gpio_read_bit(int idx, int *value)
{
	int fd, req;

	*value = 0;
	fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
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

int gpio_write_bit(int idx, int value)
{
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
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

int gpio_read_byte(int idx, int *value)
{
	int fd, req;

	*value = 0;
	fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
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

int gpio_write_byte(int idx, int value)
{
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
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

int gpio_read_int(int *value)
{
	int fd;

	*value = 0;
	fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_GPIO_READ_INT, value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int gpio_write_int(int value)
{
	int fd;

	fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
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

int gpio_enb_irq(void)
{
	int fd;

	fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
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

int gpio_dis_irq(void)
{
	int fd;

	fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
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

int gpio_reg_info(int gpio_num)
{
	int fd;
	ralink_gpio_reg_info info;

	fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	info.pid = getpid();
	info.irq = gpio_num;
	if (ioctl(fd, RALINK_GPIO_REG_IRQ, &info) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

void gpio_write2(int val)
{
	gpio_set_dir(RALINK_GPIO_DIR_ALLOUT);

	gpio_write_int(val);
}


void gpio_test_write(void)
{
	int i = 0;

	//set gpio direction to output
	gpio_set_dir(RALINK_GPIO_DIR_ALLOUT);

	//turn off LEDs
	gpio_write_int(RALINK_GPIO_DATA_MASK);
	sleep(1);

	//turn on 1 LED each time (LEDs are active low)
	for (i = 0; i < RALINK_GPIO_DATA_LEN; i++) {
		gpio_write_bit(i, 0);
		sleep(1);
	}

	//turn off 4 LEDs each time
	for (i = 0; i < RALINK_GPIO_DATA_LEN/8; i++) {
		gpio_write_byte(i, 0xFF);
		sleep(1);
	}

	sleep(1);
	//turn on all LEDs
	gpio_write_int(0);
}

void gpio_test_write2()
{
        gpio_set_dir(RALINK_GPIO_DIR_ALLOUT);

        gpio_write_int(RALINK_GPIO_DATA_MASK);

        if(wr_gpio_flag == 1){  // turn off
                printf("gpio test write byte %d as 0xFF\n", wr_gpio_num);
                gpio_write_byte(wr_gpio_num, 0xFF);
        } else if(wr_gpio_flag == 0){   // turn on
                printf("gpio test write byte %d as 0\n", wr_gpio_num);
                gpio_write_byte(wr_gpio_num, 0);
        } else
                printf("err\n");
}

void gpio_test_read(void)
{
	int i, d;

	//set gpio direction to input
	gpio_set_dir(RALINK_GPIO_DIR_ALLIN);

	//read integer
	gpio_read_int(&d);
	printf("0x%x = ", d);

	//read bytes
	for (i = RALINK_GPIO_DATA_LEN/8; i > 0; i--) {
		gpio_read_byte(i-1, &d);
		printf("%02x ", d);
	}

	//read bits
	printf("= ");
	for (i = RALINK_GPIO_DATA_LEN; i > 0; i--) {
		gpio_read_bit(i-1, &d);
		printf("%d", d);
		if (1 == i % 4)
			printf(" ");
	}
	printf("\n");
}

int
ra_gpio_read_bit(int idx)
{
        int fd, req, value;

        value = 0;
        fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
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

	printf("gpio read bit: bit %d is %d\n", idx, value);
        return value;
}

int 
ra_gpio_write_bit(int idx, int value)
{
        int fd, req;

	//gpio_set_dir(RALINK_GPIO_DIR_ALLOUT);

        fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
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

	//gpio_set_dir(RALINK_GPIO_DIR_ALLIN);

        return 0;
}

void signal_handler(int signum)
{
	printf("gpio tester: signal ");
	if (signum == SIGUSR1)
		printf("SIGUSR1");
	else if (signum == SIGUSR2)
		printf("SIGUSR2");
	else
	     printf(" received %d\n", signum);
}

void gpio_test_intr(int gpio_num)
{
	//set gpio direction to input
	gpio_set_dir(RALINK_GPIO_DIR_ALLIN);

	//enable gpio interrupt
	gpio_enb_irq();

	//register my information
	gpio_reg_info(gpio_num);

	//issue a handler to handle SIGUSR1
	signal(SIGUSR1, signal_handler);
	signal(SIGUSR2, signal_handler);

	//wait for signal
	pause();

	//disable gpio interrupt
	gpio_dis_irq();
}

void gpio_set_led(int argc, char *argv[])
{
	int fd;
	ralink_gpio_led_info led;

	led.gpio = atoi(argv[2]);
	if (led.gpio < 0 || led.gpio >= RALINK_GPIO_NUMBER) {
		printf("gpio number %d out of range (should be 0 ~ %d)\n", led.gpio, RALINK_GPIO_NUMBER);
		return;
	}
	led.on = (unsigned int)atoi(argv[3]);
	if (led.on > RALINK_GPIO_LED_INFINITY) {
		printf("on interval %d out of range (should be 0 ~ %d)\n", led.on, RALINK_GPIO_LED_INFINITY);
		return;
	}
	led.off = (unsigned int)atoi(argv[4]);
	if (led.off > RALINK_GPIO_LED_INFINITY) {
		printf("off interval %d out of range (should be 0 ~ %d)\n", led.off, RALINK_GPIO_LED_INFINITY);
		return;
	}
	led.blinks = (unsigned int)atoi(argv[5]);
	if (led.blinks > RALINK_GPIO_LED_INFINITY) {
		printf("number of blinking cycles %d out of range (should be 0 ~ %d)\n", led.blinks, RALINK_GPIO_LED_INFINITY);
		return;
	}
	led.rests = (unsigned int)atoi(argv[6]);
	if (led.rests > RALINK_GPIO_LED_INFINITY) {
		printf("number of resting cycles %d out of range (should be 0 ~ %d)\n", led.rests, RALINK_GPIO_LED_INFINITY);
		return;
	}
	led.times = (unsigned int)atoi(argv[7]);
	if (led.times > RALINK_GPIO_LED_INFINITY) {
		printf("times of blinking %d out of range (should be 0 ~ %d)\n", led.times, RALINK_GPIO_LED_INFINITY);
		return;
	}

	fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		perror(GPIO_DEV);
		return;
	}
	if (ioctl(fd, RALINK_GPIO_LED_SET, &led) < 0) {
		perror("ioctl");
		close(fd);
		return;
	}
	close(fd);
}

void usage(char *cmd)
{
	printf("Usage: %s w - writing test (output)\n", cmd);
	printf("       %s r - reading test (input)\n", cmd);
	printf("       %s i (<gpio>) - interrupt test for gpio number\n", cmd);
	printf("       %s l <gpio> <on> <off> <blinks> <rests> <times>\n", cmd);
	printf("            - set led on <gpio>(0~%d) on/off interval, no. of blinking/resting cycles, times of blinking\n", RALINK_GPIO_NUMBER);
	printf("       %s W [byte] [flag]\n", cmd);
	printf("       %s b [bit]\n", cmd);
	printf("       %s s [bit] [val]\n", cmd);
	printf("       %s d [bit] [val]\n", cmd);
	exit(0);
}

int main(int argc, char *argv[])
{
	int idx, val;
	if (argc < 2)
		usage(argv[0]);

	switch (argv[1][0]) {
	case 'w':
		gpio_test_write();
		break;
	case 'W':
                if(argv[2]){
                        wr_gpio_num = atoi(argv[2]);
                        if(argv[3]){
                                wr_gpio_flag = atoi(argv[3]);
                                printf("test wr num is %d, flag is %d\n", wr_gpio_num, wr_gpio_flag);
                        }
                }
                gpio_test_write2();
		break;
	case 'r':
		gpio_test_read();
		break;
	case 'b':
		idx = atoi(argv[2]);
		ra_gpio_read_bit(idx);
		break;
	case 's':
		idx = atoi(argv[2]);
		val = atoi(argv[3]);
		printf("set bit %d as %d\n", idx, val);	
		ra_gpio_write_bit(idx, val);
		ra_gpio_read_bit(idx);
		break;
        case 'd':
                idx = atoi(argv[2]);
                val = atoi(argv[3]);
                printf("set bit %d's dir as %d(%s)\n", idx, val, (val?"out":"in"));
		if(val)
                	gpio_set_dir_out((1<<idx));
		else
			gpio_set_dir(RALINK_GPIO_DIR_ALLIN);
                break;
	case 'i':
		if (argc == 3)
			gpio_test_intr(atoi(argv[2]));
		else
			gpio_test_intr(0);
		break;
	case 'l':
		if (argc != 8)
			usage(argv[0]);
		gpio_set_led(argc, argv);
		break;
	default:
		usage(argv[0]);
	}

	return 0;
}

