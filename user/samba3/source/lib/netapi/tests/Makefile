KRB5LIBS=
LDAP_LIBS=
LIBS=-lcrypt -lresolv -ldl -lnetapi -ltdb -ltalloc
DEVELOPER_CFLAGS=
FLAGS=-I../ -L../../../bin  -Os -pipe -mips32r2 -march=mips32r2 -mtune=mips32r2 -D__MIPSEL__ -Wall -Wno-trigraphs -Wno-long-long -Wno-format-security -Wno-strict-aliasing -Wno-pointer-sign -Wno-unused-but-set-variable -Wall -Wno-trigraphs -Wno-long-long -Wno-format-security -Wno-strict-aliasing -Wno-pointer-sign -Wno-unused-but-set-variable -fomit-frame-pointer -fno-stack-protector -femit-struct-debug-baseonly -fno-var-tracking -funit-at-a-time -ffast-math -fforce-addr -fconserve-stack -fno-strict-overflow -fno-aggressive-loop-optimizations -fno-var-tracking-assignments -fno-tree-slsr -fno-delete-null-pointer-checks -pipe -Dlinux -D__linux__ -Dunix -DEMBED -I/opt/Wive-RTNL -I/opt/Wive-RTNL/lib/include_shared -I/opt/Wive-RTNL/lib/include -I/opt/Wive-RTNL/linux/include -I/opt/Wive-RTNL/lib/include_shared/curl -I/opt/Wive-RTNL/lib/include_shared/event2 -I/opt/Wive-RTNL/lib/include_shared/libusb -I/opt/Wive-RTNL/lib/include_shared/openssl -DIPV6=1 -DINET6 -O -D_SAMBA_BUILD_=3 $(GTK_FLAGS)
CC=/opt/Wive-RTNL/toolchain/bin/mipsel-linux-uclibc-gcc
PICFLAG=-fPIC
LDFLAGS= -Wl,--fatal-warnings -L/opt/Wive-RTNL/lib/lib -L./bin
DYNEXP=-Wl,--export-dynamic
NETAPI_LIBS=$(LIBS) $(KRB5LIBS) $(LDAP_LIBS)
CMDLINE_LIBS=$(NETAPI_LIBS) $(POPT_OBJ)

# Compile a source file.
COMPILE_CC = $(CC) -I. $(FLAGS) $(PICFLAG) -c $< -o $@
COMPILE = $(COMPILE_CC)

PROGS = bin/netapitest

all: $(PROGS)

MAKEDIR = || exec false; \
	  if test -d "$$dir"; then :; else \
	  echo mkdir "$$dir"; \
	  mkdir -p "$$dir" >/dev/null 2>&1 || \
	  test -d "$$dir" || \
	  mkdir "$$dir" || \
	  exec false; fi || exec false

BINARY_PREREQS = bin/.dummy

bin/.dummy:
	@if (: >> $@ || : > $@) >/dev/null 2>&1; then :; else \
	  dir=bin $(MAKEDIR); fi
	@: >> $@ || : > $@ # what a fancy emoticon!

.c.o:
	@if (: >> $@ || : > $@) >/dev/null 2>&1; then rm -f $@; else \
	 dir=`echo $@ | sed 's,/[^/]*$$,,;s,^$$,.,'` $(MAKEDIR); fi
	@echo Compiling $*.c
	@$(COMPILE) && exit 0;\
		echo "The following command failed:" 1>&2;\
		echo "$(COMPILE_CC)" 1>&2;\
		$(COMPILE_CC) >/dev/null 2>&1

CMDLINE_OBJ = common.o
NETAPIBUFFER_OBJ = netapibuffer.o
NETAPITEST_OBJ = netapitest.o netlocalgroup.o netuser.o netgroup.o netdisplay.o netshare.o netfile.o $(CMDLINE_OBJ)

bin/netapitest: $(BINARY_PREREQS) $(NETAPITEST_OBJ)
	@echo Linking $@
	@$(CC) $(FLAGS) -o $@ $(NETAPITEST_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

clean:
	-rm -f $(PROGS)
	-rm -f core */*~ *~ \
		*/*.o */*/*.o */*/*/*.o

