
#include <linux/netfilter_ipv4/ipt_STUN.h>

int		*stun_list_account=NULL;
struct stun_nat_helper	*stun_helper=NULL;
static spinlock_t		stun_spinlock=SPIN_LOCK_UNLOCKED;	/* spinlock */

/* return -1 when error */
int		register_stun_nat(struct stun_nat_helper *stun_nat_helper, 
						int	*nat_list_count)
{
	if(stun_nat_helper||!stun_helper) {
		spin_lock(&stun_spinlock);
		stun_helper=stun_nat_helper;
		stun_list_account=nat_list_count;
		spin_unlock(&stun_spinlock);
		return	0;
	}
	return	-1;
}

void	unregister_stun_nat(struct stun_nat_helper *stun_nat_helper)
{
	if(stun_nat_helper==NULL||stun_nat_helper==stun_helper) {
		spin_lock(&stun_spinlock);
		stun_helper=NULL;
		stun_list_account=NULL;
		spin_unlock(&stun_spinlock);
		return;
	} else {
		printk("Unregister STUN fails!\n");	
		return;
	}
}

EXPORT_SYMBOL(unregister_stun_nat);
EXPORT_SYMBOL(register_stun_nat);
