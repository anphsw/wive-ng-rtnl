
CONFOPTS=--host=mipsel-linux --prefix=/usr 
export ac_cv_linux_vers=2

DIR=chillispot-1.1.0

all:
	(mkdir -p chillispot; cd chillispot; \
		ac_cv_func_malloc_0_nonnull=yes \
		ac_cv_func_memcmp_working=yes \
		ac_cv_func_setvbuf_reversed=no \
		../$(DIR)/configure $(CONFOPTS) --enable-shared && \
		 make && make DESTDIR=`pwd`/../staging install)

romfs:
	$(ROMFSINST) -S staging/usr/sbin/chilli /bin/chilli
