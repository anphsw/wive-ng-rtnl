#ifndef _NVRAM_H
#define _NVRAM_H 	1

#define RT2860_NVRAM    0
#define RTINIC_NVRAM    1
#define RT2561_NVRAM    2

void nvram_init(int index);
void nvram_close(int index);

int nvram_set(int index, char *name, char *value);
char *nvram_get(int index, char *name);
int nvram_bufset(int index, char *name, char *value);
char *nvram_bufget(int index, char *name);

void nvram_buflist(int index);
int nvram_commit(int index);
int nvram_clear(int index);
int nvram_erase(int index);

int getNvramNum(void);
unsigned int getNvramOffset(int index);
unsigned int getNvramBlockSize(int index);
char *getNvramName(int index);
unsigned int getNvramIndex(char *name);
void toggleNvramDebug(void);

#endif
