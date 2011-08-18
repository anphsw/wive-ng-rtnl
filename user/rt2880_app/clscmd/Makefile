EXEC = clscmd

			
OBJS = clscmd.o 

CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/net/ra_classifier/

 
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	rm -f core *.o $(EXEC)
