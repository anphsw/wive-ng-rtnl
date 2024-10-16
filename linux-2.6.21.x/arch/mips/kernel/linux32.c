/*
 * Conversion between 32-bit and 64-bit native system calls.
 *
 * Copyright (C) 2000 Silicon Graphics, Inc.
 * Written by Ulf Carlsson (ulfc@engr.sgi.com)
 * sys32_execve from ia64/ia32 code, Feb 2000, Kanoj Sarcar (kanoj@sgi.com)
 */
#include <linux/compiler.h>
#include <linux/mm.h>
#include <linux/errno.h>
#include <linux/file.h>
#include <linux/smp_lock.h>
#include <linux/highuid.h>
#include <linux/dirent.h>
#include <linux/resource.h>
#include <linux/highmem.h>
#include <linux/time.h>
#include <linux/times.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/skbuff.h>
#include <linux/filter.h>
#include <linux/shm.h>
#include <linux/sem.h>
#include <linux/msg.h>
#include <linux/icmpv6.h>
#include <linux/syscalls.h>
#include <linux/sysctl.h>
#include <linux/utime.h>
#include <linux/utsname.h>
#include <linux/personality.h>
#include <linux/dnotify.h>
#include <linux/module.h>
#include <linux/binfmts.h>
#include <linux/security.h>
#include <linux/compat.h>
#include <linux/vfs.h>

#include <net/sock.h>
#include <net/scm.h>

#include <asm/compat-signal.h>
#include <asm/ipc.h>
#include <asm/sim.h>
#include <asm/uaccess.h>
#include <asm/mmu_context.h>
#include <asm/mman.h>

/* Use this to get at 32-bit user passed pointers. */
/* A() macro should be used for places where you e.g.
   have some internal variable u32 and just want to get
   rid of a compiler warning. AA() has to be used in
   places where you want to convert a function argument
   to 32bit pointer or when you e.g. access pt_regs
   structure and want to consider 32bit registers only.
 */
#define A(__x) ((unsigned long)(__x))
#define AA(__x) ((unsigned long)((int)__x))

#ifdef __MIPSEB__
#define merge_64(r1, r2) ((((r1) & 0xffffffffUL) << 32) + ((r2) & 0xffffffffUL))
#endif
#ifdef __MIPSEL__
#define merge_64(r1, r2) ((((r2) & 0xffffffffUL) << 32) + ((r1) & 0xffffffffUL))
#endif

/*
 * Revalidate the inode. This is required for proper NFS attribute caching.
 */

int cp_compat_stat(struct kstat *stat, struct compat_stat __user *statbuf)
{
	struct compat_stat tmp;

	if (!new_valid_dev(stat->dev) || !new_valid_dev(stat->rdev))
		return -EOVERFLOW;

	memset(&tmp, 0, sizeof(tmp));
	tmp.st_dev = new_encode_dev(stat->dev);
	tmp.st_ino = stat->ino;
	if (sizeof(tmp.st_ino) < sizeof(stat->ino) && tmp.st_ino != stat->ino)
		return -EOVERFLOW;
	tmp.st_mode = stat->mode;
	tmp.st_nlink = stat->nlink;
	SET_UID(tmp.st_uid, stat->uid);
	SET_GID(tmp.st_gid, stat->gid);
	tmp.st_rdev = new_encode_dev(stat->rdev);
	tmp.st_size = stat->size;
	tmp.st_atime = stat->atime.tv_sec;
	tmp.st_mtime = stat->mtime.tv_sec;
	tmp.st_ctime = stat->ctime.tv_sec;
#ifdef STAT_HAVE_NSEC
	tmp.st_atime_nsec = stat->atime.tv_nsec;
	tmp.st_mtime_nsec = stat->mtime.tv_nsec;
	tmp.st_ctime_nsec = stat->ctime.tv_nsec;
#endif
	tmp.st_blocks = stat->blocks;
	tmp.st_blksize = stat->blksize;
	return copy_to_user(statbuf, &tmp, sizeof(tmp)) ? -EFAULT : 0;
}

asmlinkage unsigned long
sys32_mmap2(unsigned long addr, unsigned long len, unsigned long prot,
         unsigned long flags, unsigned long fd, unsigned long pgoff)
{
	struct file * file = NULL;
	unsigned long error;

	error = -EINVAL;
	if (pgoff & (~PAGE_MASK >> 12))
		goto out;
	pgoff >>= PAGE_SHIFT-12;

	if (!(flags & MAP_ANONYMOUS)) {
		error = -EBADF;
		file = fget(fd);
		if (!file)
			goto out;
	}
	flags &= ~(MAP_EXECUTABLE | MAP_DENYWRITE);

	down_write(&current->mm->mmap_sem);
	error = do_mmap_pgoff(file, addr, len, prot, flags, pgoff);
	up_write(&current->mm->mmap_sem);
	if (file)
		fput(file);

out:
	return error;
}


asmlinkage int sys_truncate64(const char __user *path, unsigned int high,
			      unsigned int low)
{
	if ((int)high < 0)
		return -EINVAL;
	return sys_truncate(path, ((long) high << 32) | low);
}

asmlinkage int sys_ftruncate64(unsigned int fd, unsigned int high,
			       unsigned int low)
{
	if ((int)high < 0)
		return -EINVAL;
	return sys_ftruncate(fd, ((long) high << 32) | low);
}

/*
 * sys_execve() executes a new program.
 */
asmlinkage int sys32_execve(nabi_no_regargs struct pt_regs regs)
{
	int error;
	char * filename;

	filename = getname(compat_ptr(regs.regs[4]));
	error = PTR_ERR(filename);
	if (IS_ERR(filename))
		goto out;
	error = compat_do_execve(filename, compat_ptr(regs.regs[5]),
				 compat_ptr(regs.regs[6]), &regs);
	putname(filename);

out:
	return error;
}

#define RLIM_INFINITY32	0x7fffffff
#define RESOURCE32(x) ((x > RLIM_INFINITY32) ? RLIM_INFINITY32 : x)

struct rlimit32 {
	int	rlim_cur;
	int	rlim_max;
};

#ifdef __MIPSEB__
asmlinkage long sys32_truncate64(const char __user * path, unsigned long __dummy,
	int length_hi, int length_lo)
#endif
#ifdef __MIPSEL__
asmlinkage long sys32_truncate64(const char __user * path, unsigned long __dummy,
	int length_lo, int length_hi)
#endif
{
	loff_t length;

	length = ((unsigned long) length_hi << 32) | (unsigned int) length_lo;

	return sys_truncate(path, length);
}

#ifdef __MIPSEB__
asmlinkage long sys32_ftruncate64(unsigned int fd, unsigned long __dummy,
	int length_hi, int length_lo)
#endif
#ifdef __MIPSEL__
asmlinkage long sys32_ftruncate64(unsigned int fd, unsigned long __dummy,
	int length_lo, int length_hi)
#endif
{
	loff_t length;

	length = ((unsigned long) length_hi << 32) | (unsigned int) length_lo;

	return sys_ftruncate(fd, length);
}

static inline long
get_tv32(struct timeval *o, struct compat_timeval __user *i)
{
	return (!access_ok(VERIFY_READ, i, sizeof(*i)) ||
		(__get_user(o->tv_sec, &i->tv_sec) |
		 __get_user(o->tv_usec, &i->tv_usec)));
}

static inline long
put_tv32(struct compat_timeval __user *o, struct timeval *i)
{
	return (!access_ok(VERIFY_WRITE, o, sizeof(*o)) ||
		(__put_user(i->tv_sec, &o->tv_sec) |
		 __put_user(i->tv_usec, &o->tv_usec)));
}

extern struct timezone sys_tz;

asmlinkage int
sys32_gettimeofday(struct compat_timeval __user *tv, struct timezone __user *tz)
{
	if (tv) {
		struct timeval ktv;
		do_gettimeofday(&ktv);
		if (put_tv32(tv, &ktv))
			return -EFAULT;
	}
	if (tz) {
		if (copy_to_user(tz, &sys_tz, sizeof(sys_tz)))
			return -EFAULT;
	}
	return 0;
}

static inline long get_ts32(struct timespec *o, struct compat_timeval __user *i)
{
	long usec;

	if (!access_ok(VERIFY_READ, i, sizeof(*i)))
		return -EFAULT;
	if (__get_user(o->tv_sec, &i->tv_sec))
		return -EFAULT;
	if (__get_user(usec, &i->tv_usec))
		return -EFAULT;
	o->tv_nsec = usec * 1000;
		return 0;
}

asmlinkage int
sys32_settimeofday(struct compat_timeval __user *tv, struct timezone __user *tz)
{
	struct timespec kts;
	struct timezone ktz;

 	if (tv) {
		if (get_ts32(&kts, tv))
			return -EFAULT;
	}
	if (tz) {
		if (copy_from_user(&ktz, tz, sizeof(ktz)))
			return -EFAULT;
	}

	return do_sys_settimeofday(tv ? &kts : NULL, tz ? &ktz : NULL);
}

asmlinkage int sys32_llseek(unsigned int fd, unsigned int offset_high,
			    unsigned int offset_low, loff_t __user * result,
			    unsigned int origin)
{
	return sys_llseek(fd, offset_high, offset_low, result, origin);
}

/* From the Single Unix Spec: pread & pwrite act like lseek to pos + op +
   lseek back to original location.  They fail just like lseek does on
   non-seekable files.  */

asmlinkage ssize_t sys32_pread(unsigned int fd, char __user * buf,
			       size_t count, u32 unused, u64 a4, u64 a5)
{
	return sys_pread64(fd, buf, count, merge_64(a4, a5));
}

asmlinkage ssize_t sys32_pwrite(unsigned int fd, const char __user * buf,
			        size_t count, u32 unused, u64 a4, u64 a5)
{
	return sys_pwrite64(fd, buf, count, merge_64(a4, a5));
}

asmlinkage int sys32_sched_rr_get_interval(compat_pid_t pid,
	struct compat_timespec __user *interval)
{
	struct timespec t;
	int ret;
	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);
	ret = sys_sched_rr_get_interval(pid, (struct timespec __user *)&t);
	set_fs(old_fs);
	if (put_user (t.tv_sec, &interval->tv_sec) ||
	    __put_user(t.tv_nsec, &interval->tv_nsec))
		return -EFAULT;
	return ret;
}

#ifdef CONFIG_SYSVIPC

asmlinkage long
sys32_ipc(u32 call, int first, int second, int third, u32 ptr, u32 fifth)
{
	int version, err;

	version = call >> 16; /* hack for backward compatibility */
	call &= 0xffff;

	switch (call) {
	case SEMOP:
		/* struct sembuf is the same on 32 and 64bit :)) */
		err = sys_semtimedop(first, compat_ptr(ptr), second, NULL);
		break;
	case SEMTIMEDOP:
		err = compat_sys_semtimedop(first, compat_ptr(ptr), second,
					    compat_ptr(fifth));
		break;
	case SEMGET:
		err = sys_semget(first, second, third);
		break;
	case SEMCTL:
		err = compat_sys_semctl(first, second, third, compat_ptr(ptr));
		break;
	case MSGSND:
		err = compat_sys_msgsnd(first, second, third, compat_ptr(ptr));
		break;
	case MSGRCV:
		err = compat_sys_msgrcv(first, second, fifth, third,
					version, compat_ptr(ptr));
		break;
	case MSGGET:
		err = sys_msgget((key_t) first, second);
		break;
	case MSGCTL:
		err = compat_sys_msgctl(first, second, compat_ptr(ptr));
		break;
	case SHMAT:
		err = compat_sys_shmat(first, second, third, version,
				       compat_ptr(ptr));
		break;
	case SHMDT:
		err = sys_shmdt(compat_ptr(ptr));
		break;
	case SHMGET:
		err = sys_shmget(first, (unsigned)second, third);
		break;
	case SHMCTL:
		err = compat_sys_shmctl(first, second, compat_ptr(ptr));
		break;
	default:
		err = -ENOSYS;
		break;
	}

	return err;
}

#else

asmlinkage long
sys32_ipc(u32 call, int first, int second, int third, u32 ptr, u32 fifth)
{
	return -ENOSYS;
}

#endif /* CONFIG_SYSVIPC */

#ifdef CONFIG_MIPS32_N32
asmlinkage long sysn32_semctl(int semid, int semnum, int cmd, u32 arg)
{
	/* compat_sys_semctl expects a pointer to union semun */
	u32 __user *uptr = compat_alloc_user_space(sizeof(u32));
	if (put_user(arg, uptr))
		return -EFAULT;
	return compat_sys_semctl(semid, semnum, cmd, uptr);
}

asmlinkage long sysn32_msgsnd(int msqid, u32 msgp, unsigned msgsz, int msgflg)
{
	return compat_sys_msgsnd(msqid, msgsz, msgflg, compat_ptr(msgp));
}

asmlinkage long sysn32_msgrcv(int msqid, u32 msgp, size_t msgsz, int msgtyp,
			      int msgflg)
{
	return compat_sys_msgrcv(msqid, msgsz, msgtyp, msgflg, IPC_64,
				 compat_ptr(msgp));
}
#endif

struct sysctl_args32
{
	compat_caddr_t name;
	int nlen;
	compat_caddr_t oldval;
	compat_caddr_t oldlenp;
	compat_caddr_t newval;
	compat_size_t newlen;
	unsigned int __unused[4];
};

#ifdef CONFIG_SYSCTL_SYSCALL

asmlinkage long sys32_sysctl(struct sysctl_args32 __user *args)
{
	struct sysctl_args32 tmp;
	int error;
	size_t oldlen;
	size_t __user *oldlenp = NULL;
	unsigned long addr = (((unsigned long)&args->__unused[0]) + 7) & ~7;

	if (copy_from_user(&tmp, args, sizeof(tmp)))
		return -EFAULT;

	if (tmp.oldval && tmp.oldlenp) {
		/* Duh, this is ugly and might not work if sysctl_args
		   is in read-only memory, but do_sysctl does indirectly
		   a lot of uaccess in both directions and we'd have to
		   basically copy the whole sysctl.c here, and
		   glibc's __sysctl uses rw memory for the structure
		   anyway.  */
		if (get_user(oldlen, (u32 __user *)A(tmp.oldlenp)) ||
		    put_user(oldlen, (size_t __user *)addr))
			return -EFAULT;
		oldlenp = (size_t __user *)addr;
	}

	lock_kernel();
	error = do_sysctl((int __user *)A(tmp.name), tmp.nlen, (void __user *)A(tmp.oldval),
			  oldlenp, (void __user *)A(tmp.newval), tmp.newlen);
	unlock_kernel();
	if (oldlenp) {
		if (!error) {
			if (get_user(oldlen, (size_t __user *)addr) ||
			    put_user(oldlen, (u32 __user *)A(tmp.oldlenp)))
				error = -EFAULT;
		}
		copy_to_user(args->__unused, tmp.__unused, sizeof(tmp.__unused));
	}
	return error;
}

#endif /* CONFIG_SYSCTL_SYSCALL */

asmlinkage long sys32_newuname(struct new_utsname __user * name)
{
	int ret = 0;

	down_read(&uts_sem);
	if (copy_to_user(name, utsname(), sizeof *name))
		ret = -EFAULT;
	up_read(&uts_sem);

	if (current->personality == PER_LINUX32 && !ret)
		if (copy_to_user(name->machine, "mips\0\0\0", 8))
			ret = -EFAULT;

	return ret;
}

asmlinkage int sys32_personality(unsigned long personality)
{
	int ret;
	personality &= 0xffffffff;
	if (personality(current->personality) == PER_LINUX32 &&
	    personality == PER_LINUX)
		personality = PER_LINUX32;
	ret = sys_personality(personality);
	if (ret == PER_LINUX32)
		ret = PER_LINUX;
	return ret;
}

/* ustat compatibility */
struct ustat32 {
	compat_daddr_t	f_tfree;
	compat_ino_t	f_tinode;
	char		f_fname[6];
	char		f_fpack[6];
};

extern asmlinkage long sys_ustat(dev_t dev, struct ustat __user * ubuf);

asmlinkage int sys32_ustat(dev_t dev, struct ustat32 __user * ubuf32)
{
	int err;
	struct ustat tmp;
	struct ustat32 tmp32;
	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);
	err = sys_ustat(dev, (struct ustat __user *)&tmp);
	set_fs(old_fs);

	if (err)
		goto out;

	memset(&tmp32, 0, sizeof(struct ustat32));
	tmp32.f_tfree = tmp.f_tfree;
	tmp32.f_tinode = tmp.f_tinode;

	err = copy_to_user(ubuf32, &tmp32, sizeof(struct ustat32)) ? -EFAULT : 0;

out:
	return err;
}

asmlinkage int sys32_sendfile(int out_fd, int in_fd, compat_off_t __user *offset,
	s32 count)
{
	mm_segment_t old_fs = get_fs();
	int ret;
	off_t of;

	if (offset && get_user(of, offset))
		return -EFAULT;

	set_fs(KERNEL_DS);
	ret = sys_sendfile(out_fd, in_fd, offset ? (off_t __user *)&of : NULL, count);
	set_fs(old_fs);

	if (offset && put_user(of, offset))
		return -EFAULT;

	return ret;
}

asmlinkage ssize_t sys32_readahead(int fd, u32 pad0, u64 a2, u64 a3,
                                   size_t count)
{
	return sys_readahead(fd, merge_64(a2, a3), count);
}

asmlinkage long sys32_sync_file_range(int fd, int __pad,
	unsigned long a2, unsigned long a3,
	unsigned long a4, unsigned long a5,
	int flags)
{
	return sys_sync_file_range(fd,
			merge_64(a2, a3), merge_64(a4, a5),
			flags);
}

save_static_function(sys32_clone);
__used noinline static int
_sys32_clone(nabi_no_regargs struct pt_regs regs)
{
	unsigned long clone_flags;
	unsigned long newsp;
	int __user *parent_tidptr, *child_tidptr;

	clone_flags = regs.regs[4];
	newsp = regs.regs[5];
	if (!newsp)
		newsp = regs.regs[29];
	parent_tidptr = (int __user *) regs.regs[6];

	/* Use __dummy4 instead of getting it off the stack, so that
	   syscall() works.  */
	child_tidptr = (int __user *) __dummy4;
	return do_fork(clone_flags, newsp, &regs, 0,
	               parent_tidptr, child_tidptr);
}
