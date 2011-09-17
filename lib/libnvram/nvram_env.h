extern		void  nvram_printenv(int index, int argc, char *argv[]);
extern unsigned char *nvram_getenv  (int index, unsigned char *name);
extern		int   nvram_setenv  (int index, int argc, char *argv[]);
extern		int   nvram_buff_setenv  (int index, int argc, char *argv[]);
extern 		int   do_nvram_commit(int index);
extern unsigned	long  crc32	 (unsigned long, const unsigned char *, unsigned);
