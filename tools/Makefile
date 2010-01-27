CC=gcc
STRIP=strip

all :	install

install :
	mkdir bin
	make -C sstrip
	make -C mksquash_lzma-3.2 install


clean:
	make -C sstrip clean
	make -C mksquash_lzma-3.2 clean
	rm -rf ./bin

mrproper:
	echo "OK"