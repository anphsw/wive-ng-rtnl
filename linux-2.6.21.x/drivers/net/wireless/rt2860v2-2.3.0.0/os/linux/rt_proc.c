#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#include "rt_config.h"

int wl_proc_init(void);
int wl_proc_exit(void);

#if defined (CONFIG_RALINK_RT2880)
#define PROCREG_DIR             "rt2880"
#elif defined (CONFIG_RALINK_RT3052)
#define PROCREG_DIR             "rt3052"
#elif defined (CONFIG_RALINK_RT2883)
#define PROCREG_DIR             "rt2883"
#elif defined (CONFIG_RALINK_RT3883)
#define PROCREG_DIR             "rt3883"
#else
#define PROCREG_DIR             "rt2880"
#endif

#ifdef CONFIG_PROC_FS
static struct proc_dir_entry *procRegDir;


int wl_proc_init(void)
{
	if (procRegDir == NULL)
		procRegDir = proc_mkdir(PROCREG_DIR, NULL);

	if (procRegDir) {
	}

	return 0;
}

int wl_proc_exit(void)
{

	
	return 0;
}
#else
int wl_proc_init(void)
{
	return 0;
}

int wl_proc_exit(void)
{

	return 0;
}
#endif // CONFIG_PROC_FS //

