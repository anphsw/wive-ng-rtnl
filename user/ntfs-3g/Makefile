
CFLAGS += -DHAVE_CONFIG_H

SUBDIRS = libfuse-lite src

all: 
	@for dir in ${SUBDIRS}; do \
		echo "Making all in $$dir"; \
		(cd $$dir; $(MAKE) all) || exit 1; \
	done
	
clean:
	@for dir in ${SUBDIRS}; do \
		echo "Cleaning in $$dir"; \
		(cd $$dir; $(MAKE) clean) || exit 1; \
	done

romfs:
	@for dir in ${SUBDIRS}; do \
		(cd $$dir; $(MAKE) romfs) || exit 1; \
	done