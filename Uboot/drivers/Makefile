#
# (C) Copyright 2000-2004
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

#CFLAGS += -DET_DEBUG -DDEBUG

LIB	= libdrivers.a

OBJS	= i2c_drv.o mii_mgr.o

ifeq ($(CONFIG_ATHEROS_PHY),y)
ifeq ($(CONFIG_AG7100),y)
OBJS	+= ag7100.o athrs16_phy.o meminit.o
else
OBJS	+= athrs16_phy.o rt2880_eth.o
endif
else
OBJS	+= rt2880_eth.o
endif

ifeq ($(CFG_ENV_IS), IN_FLASH)
OBJS	+= spi_drv.o 
endif

ifeq ($(CFG_ENV_IS), IN_SPI)
OBJS	+= spi_flash.o 
 ifeq ($(RT3883_MP), y)
 OBJS	+= spi_drv.o 
 endif
 ifeq ($(RT3352_MP), y)
 OBJS	+= spi_drv.o 
 endif
endif

ifeq ($(CFG_ENV_IS), IN_NAND)
OBJS	+= nand_flash.o 
 ifeq ($(RT3883_MP), y)
 OBJS	+= spi_drv.o 
 endif
endif

ifeq ($(RALINK_OHCI), ON)
OBJS	+= ohci-hcd.o
endif

ifeq ($(RALINK_EHCI), ON)
OBJS	+= ehci-hcd.o ehci-ra.o
endif

all:	$(LIB)

$(LIB): $(OBJS)
	$(AR) crv $@ $(OBJS)

#########################################################################

.depend:	Makefile $(OBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
