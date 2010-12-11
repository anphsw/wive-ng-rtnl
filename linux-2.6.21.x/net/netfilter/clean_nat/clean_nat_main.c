/* clean_nat.c: The module for clean nat.

	Pronets Technologies Corp.
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/stat.h>
#include <asm/errno.h>
#include <linux/proc_fs.h>
#ifdef CONFIG_IP_NF_CONNTRACK
#ifdef CONFIG_NF_CONNTRACK_SUPPORT
extern void ip_ct_record_cleanup();
#else
extern void ip_conntrack_flush(void);
#endif
static int proc_dbg_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
	if(buffer[0] == '0')
	{
	    #ifdef CONFIG_NF_CONNTRACK_SUPPORT
	    ip_ct_record_cleanup();
	    #else
	    ip_conntrack_flush();
	    #endif
	    printk("clean_nat_conn: clean all of the connection track\n");
	}
	return count;
}
static int proc_dbg_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{

	printk("Clean NAT session module load.\n");
	return 0;
}
static void dbg_exit(void)
{
	remove_proc_entry("cleannat",NULL);
}
static int dbg_init(void)
{
	struct proc_dir_entry *res;
	res = create_proc_entry("cleannat",S_IWUSR | S_IRUGO,NULL);
	if(!res)
		return -ENOMEM;
	res->read_proc=proc_dbg_read;
	res->write_proc=proc_dbg_write;
}
static int init_clean_nat(void)
{
	dbg_init();
	printk("===>Enter clean nat module\n");
	return 0;
}

static void unlod_clean_nat(void)
{
	dbg_exit();
	printk("Exit clean nat module<===\n");
}

module_init(init_clean_nat);
module_exit(unlod_clean_nat);
MODULE_LICENSE("GPL");
#endif
