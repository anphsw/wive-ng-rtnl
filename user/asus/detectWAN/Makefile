LIB = libdetectWAN.so
OBJS = detectWAN.o

SHDIR = $(ROOTDIR)/user/shared
USERDIR = $(ROOTDIR)/user

CFLAGS  += -Wall -DASUS -DASUS_NVRAM -DBCMNVRAM -I. -I$(SHDIR)/include -I$(ROOTDIR)/linux-2.6.21.x/include 

all: $(LIB)	

$(LIB): $(OBJS)
	$(CC) -shared -o $@ $^ $(CFLAGS) 
	$(STRIP) $(LIB)
	
romfs:
	$(ROMFSINST) /lib/$(LIB)
	
.PHONY: clean
clean:
	rm -f *~ *.o *.so 
