OBJS = wanduck.o shutils.o
BIN = wanduck
SHDIR = $(ROOTDIR)/user/shared #added by gary
                                                                                                                                               
CFLAGS  += -I. -I$(SRCBASE)/include -I$(ROOTDIR)/linux-2.6.21.x/include -I$(ROOTDIR)/linux-2.6.21.x/drivers/net/raeth # -I$(TOP)/httpd -Wall
CFLAGS  += -s -Os -DASUS_NVRAM

LDFLAGS += -L$(SHDIR) -lshared -s #changed by gary


#vpath httpd.h /root/WL520gu/src/router/httpd/

all: ${BIN}
	${STRIP} ${BIN}
                                                                                                                                               
#install: all
#	install -D ${BIN} ${INSTALLDIR}/usr/sbin/${BIN}
#	install -D start_wanduck ${INSTALLDIR}/usr/sbin/start_wanduck
#	chmod 555 ${INSTALLDIR}/usr/sbin/start_wanduck

${BIN}: ${OBJS}
	$(CC) $(LDFLAGS) -o $@ $^

romfs:
	$(ROMFSINST) /usr/sbin/$(BIN)


clean:
	rm -f *.o ${BIN}
