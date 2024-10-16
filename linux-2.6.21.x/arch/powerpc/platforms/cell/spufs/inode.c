/*
 * SPU file system
 *
 * (C) Copyright IBM Deutschland Entwicklung GmbH 2005
 *
 * Author: Arnd Bergmann <arndb@de.ibm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/file.h>
#include <linux/fs.h>
#include <linux/backing-dev.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <linux/pagemap.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/parser.h>

#include <asm/prom.h>
#include <asm/semaphore.h>
#include <asm/spu.h>
#include <asm/uaccess.h>

#include "spufs.h"

static struct kmem_cache *spufs_inode_cache;
char *isolated_loader;

static struct inode *
spufs_alloc_inode(struct super_block *sb)
{
	struct spufs_inode_info *ei;

	ei = kmem_cache_alloc(spufs_inode_cache, GFP_KERNEL);
	if (!ei)
		return NULL;

	ei->i_gang = NULL;
	ei->i_ctx = NULL;

	return &ei->vfs_inode;
}

static void
spufs_destroy_inode(struct inode *inode)
{
	kmem_cache_free(spufs_inode_cache, SPUFS_I(inode));
}

static void
spufs_init_once(void *p, struct kmem_cache * cachep, unsigned long flags)
{
	struct spufs_inode_info *ei = p;

	if (flags & SLAB_CTOR_CONSTRUCTOR) {
		inode_init_once(&ei->vfs_inode);
	}
}

static struct inode *
spufs_new_inode(struct super_block *sb, int mode)
{
	struct inode *inode;

	inode = new_inode(sb);
	if (!inode)
		goto out;

	inode->i_mode = mode;
	inode->i_uid = current->fsuid;
	inode->i_gid = current->fsgid;
	inode->i_blocks = 0;
	inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
out:
	return inode;
}

static int
spufs_setattr(struct dentry *dentry, struct iattr *attr)
{
	struct inode *inode = dentry->d_inode;

	if ((attr->ia_valid & ATTR_SIZE) &&
	    (attr->ia_size != inode->i_size))
		return -EINVAL;
	return inode_setattr(inode, attr);
}


static int
spufs_new_file(struct super_block *sb, struct dentry *dentry,
		const struct file_operations *fops, int mode,
		struct spu_context *ctx)
{
	static struct inode_operations spufs_file_iops = {
		.setattr = spufs_setattr,
	};
	struct inode *inode;
	int ret;

	ret = -ENOSPC;
	inode = spufs_new_inode(sb, S_IFREG | mode);
	if (!inode)
		goto out;

	ret = 0;
	inode->i_op = &spufs_file_iops;
	inode->i_fop = fops;
	inode->i_private = SPUFS_I(inode)->i_ctx = get_spu_context(ctx);
	d_add(dentry, inode);
out:
	return ret;
}

static void
spufs_delete_inode(struct inode *inode)
{
	struct spufs_inode_info *ei = SPUFS_I(inode);

	if (ei->i_ctx)
		put_spu_context(ei->i_ctx);
	if (ei->i_gang)
		put_spu_gang(ei->i_gang);
	clear_inode(inode);
}

static void spufs_prune_dir(struct dentry *dir)
{
	struct dentry *dentry, *tmp;

	mutex_lock(&dir->d_inode->i_mutex);
	list_for_each_entry_safe(dentry, tmp, &dir->d_subdirs, d_u.d_child) {
		spin_lock(&dcache_lock);
		spin_lock(&dentry->d_lock);
		if (!(d_unhashed(dentry)) && dentry->d_inode) {
			dget_locked(dentry);
			__d_drop(dentry);
			spin_unlock(&dentry->d_lock);
			simple_unlink(dir->d_inode, dentry);
			spin_unlock(&dcache_lock);
			dput(dentry);
		} else {
			spin_unlock(&dentry->d_lock);
			spin_unlock(&dcache_lock);
		}
	}
	shrink_dcache_parent(dir);
	mutex_unlock(&dir->d_inode->i_mutex);
}

/* Caller must hold parent->i_mutex */
static int spufs_rmdir(struct inode *parent, struct dentry *dir)
{
	/* remove all entries */
	spufs_prune_dir(dir);

	return simple_rmdir(parent, dir);
}

static int spufs_fill_dir(struct dentry *dir, struct tree_descr *files,
			  int mode, struct spu_context *ctx)
{
	struct dentry *dentry;
	int ret;

	while (files->name && files->name[0]) {
		ret = -ENOMEM;
		dentry = d_alloc_name(dir, files->name);
		if (!dentry)
			goto out;
		ret = spufs_new_file(dir->d_sb, dentry, files->ops,
					files->mode & mode, ctx);
		if (ret)
			goto out;
		files++;
	}
	return 0;
out:
	spufs_prune_dir(dir);
	return ret;
}

static int spufs_dir_close(struct inode *inode, struct file *file)
{
	struct spu_context *ctx;
	struct inode *parent;
	struct dentry *dir;
	int ret;

	dir = file->f_path.dentry;
	parent = dir->d_parent->d_inode;
	ctx = SPUFS_I(dir->d_inode)->i_ctx;

	mutex_lock(&parent->i_mutex);
	ret = spufs_rmdir(parent, dir);
	mutex_unlock(&parent->i_mutex);
	WARN_ON(ret);

	/* We have to give up the mm_struct */
	spu_forget(ctx);

	return dcache_dir_close(inode, file);
}

const struct inode_operations spufs_dir_inode_operations = {
	.lookup = simple_lookup,
};

const struct file_operations spufs_context_fops = {
	.open		= dcache_dir_open,
	.release	= spufs_dir_close,
	.llseek		= dcache_dir_lseek,
	.read		= generic_read_dir,
	.readdir	= dcache_readdir,
	.fsync		= simple_sync_file,
};
EXPORT_SYMBOL_GPL(spufs_context_fops);

static int
spufs_mkdir(struct inode *dir, struct dentry *dentry, unsigned int flags,
		int mode)
{
	int ret;
	struct inode *inode;
	struct spu_context *ctx;

	ret = -ENOSPC;
	inode = spufs_new_inode(dir->i_sb, mode | S_IFDIR);
	if (!inode)
		goto out;

	if (dir->i_mode & S_ISGID) {
		inode->i_gid = dir->i_gid;
		inode->i_mode &= S_ISGID;
	}
	ctx = alloc_spu_context(SPUFS_I(dir)->i_gang); /* XXX gang */
	SPUFS_I(inode)->i_ctx = ctx;
	if (!ctx)
		goto out_iput;

	ctx->flags = flags;
	inode->i_op = &spufs_dir_inode_operations;
	inode->i_fop = &simple_dir_operations;
	if (flags & SPU_CREATE_NOSCHED)
		ret = spufs_fill_dir(dentry, spufs_dir_nosched_contents,
					 mode, ctx);
	else
		ret = spufs_fill_dir(dentry, spufs_dir_contents, mode, ctx);

	if (ret)
		goto out_free_ctx;

	d_instantiate(dentry, inode);
	dget(dentry);
	dir->i_nlink++;
	dentry->d_inode->i_nlink++;
	goto out;

out_free_ctx:
	put_spu_context(ctx);
out_iput:
	iput(inode);
out:
	return ret;
}

static int spufs_context_open(struct dentry *dentry, struct vfsmount *mnt)
{
	int ret;
	struct file *filp;

	ret = get_unused_fd();
	if (ret < 0) {
		dput(dentry);
		mntput(mnt);
		goto out;
	}

	filp = dentry_open(dentry, mnt, O_RDONLY);
	if (IS_ERR(filp)) {
		put_unused_fd(ret);
		ret = PTR_ERR(filp);
		goto out;
	}

	filp->f_op = &spufs_context_fops;
	fd_install(ret, filp);
out:
	return ret;
}

static int spufs_create_context(struct inode *inode,
			struct dentry *dentry,
			struct vfsmount *mnt, int flags, int mode)
{
	int ret;

	ret = -EPERM;
	if ((flags & SPU_CREATE_NOSCHED) &&
	    !capable(CAP_SYS_NICE))
		goto out_unlock;

	ret = -EINVAL;
	if ((flags & (SPU_CREATE_NOSCHED | SPU_CREATE_ISOLATE))
	    == SPU_CREATE_ISOLATE)
		goto out_unlock;

	ret = -ENODEV;
	if ((flags & SPU_CREATE_ISOLATE) && !isolated_loader)
		goto out_unlock;

	ret = spufs_mkdir(inode, dentry, flags, mode & S_IRWXUGO);
	if (ret)
		goto out_unlock;

	/*
	 * get references for dget and mntget, will be released
	 * in error path of *_open().
	 */
	ret = spufs_context_open(dget(dentry), mntget(mnt));
	if (ret < 0) {
		WARN_ON(spufs_rmdir(inode, dentry));
		mutex_unlock(&inode->i_mutex);
		spu_forget(SPUFS_I(dentry->d_inode)->i_ctx);
		goto out;
	}

out_unlock:
	mutex_unlock(&inode->i_mutex);
out:
	dput(dentry);
	return ret;
}

static int spufs_rmgang(struct inode *root, struct dentry *dir)
{
	/* FIXME: this fails if the dir is not empty,
	          which causes a leak of gangs. */
	return simple_rmdir(root, dir);
}

static int spufs_gang_close(struct inode *inode, struct file *file)
{
	struct inode *parent;
	struct dentry *dir;
	int ret;

	dir = file->f_path.dentry;
	parent = dir->d_parent->d_inode;

	ret = spufs_rmgang(parent, dir);
	WARN_ON(ret);

	return dcache_dir_close(inode, file);
}

const struct file_operations spufs_gang_fops = {
	.open		= dcache_dir_open,
	.release	= spufs_gang_close,
	.llseek		= dcache_dir_lseek,
	.read		= generic_read_dir,
	.readdir	= dcache_readdir,
	.fsync		= simple_sync_file,
};

static int
spufs_mkgang(struct inode *dir, struct dentry *dentry, int mode)
{
	int ret;
	struct inode *inode;
	struct spu_gang *gang;

	ret = -ENOSPC;
	inode = spufs_new_inode(dir->i_sb, mode | S_IFDIR);
	if (!inode)
		goto out;

	ret = 0;
	if (dir->i_mode & S_ISGID) {
		inode->i_gid = dir->i_gid;
		inode->i_mode &= S_ISGID;
	}
	gang = alloc_spu_gang();
	SPUFS_I(inode)->i_ctx = NULL;
	SPUFS_I(inode)->i_gang = gang;
	if (!gang)
		goto out_iput;

	inode->i_op = &spufs_dir_inode_operations;
	inode->i_fop = &simple_dir_operations;

	d_instantiate(dentry, inode);
	dget(dentry);
	dir->i_nlink++;
	dentry->d_inode->i_nlink++;
	return ret;

out_iput:
	iput(inode);
out:
	return ret;
}

static int spufs_gang_open(struct dentry *dentry, struct vfsmount *mnt)
{
	int ret;
	struct file *filp;

	ret = get_unused_fd();
	if (ret < 0) {
		dput(dentry);
		mntput(mnt);
		goto out;
	}

	filp = dentry_open(dentry, mnt, O_RDONLY);
	if (IS_ERR(filp)) {
		put_unused_fd(ret);
		ret = PTR_ERR(filp);
		goto out;
	}

	filp->f_op = &spufs_gang_fops;
	fd_install(ret, filp);
out:
	return ret;
}

static int spufs_create_gang(struct inode *inode,
			struct dentry *dentry,
			struct vfsmount *mnt, int mode)
{
	int ret;

	ret = spufs_mkgang(inode, dentry, mode & S_IRWXUGO);
	if (ret)
		goto out;

	/*
	 * get references for dget and mntget, will be released
	 * in error path of *_open().
	 */
	ret = spufs_gang_open(dget(dentry), mntget(mnt));
	if (ret < 0)
		WARN_ON(spufs_rmgang(inode, dentry));

out:
	mutex_unlock(&inode->i_mutex);
	dput(dentry);
	return ret;
}


static struct file_system_type spufs_type;

long spufs_create(struct nameidata *nd, unsigned int flags, mode_t mode)
{
	struct dentry *dentry;
	int ret;

	ret = -EINVAL;
	/* check if we are on spufs */
	if (nd->dentry->d_sb->s_type != &spufs_type)
		goto out;

	/* don't accept undefined flags */
	if (flags & (~SPU_CREATE_FLAG_ALL))
		goto out;

	/* only threads can be underneath a gang */
	if (nd->dentry != nd->dentry->d_sb->s_root) {
		if ((flags & SPU_CREATE_GANG) ||
		    !SPUFS_I(nd->dentry->d_inode)->i_gang)
			goto out;
	}

	dentry = lookup_create(nd, 1);
	ret = PTR_ERR(dentry);
	if (IS_ERR(dentry))
		goto out_dir;

	ret = -EEXIST;
	if (dentry->d_inode)
		goto out_dput;

	mode &= ~current->fs->umask;

	if (flags & SPU_CREATE_GANG)
		return spufs_create_gang(nd->dentry->d_inode,
					dentry, nd->mnt, mode);
	else
		return spufs_create_context(nd->dentry->d_inode,
					dentry, nd->mnt, flags, mode);

out_dput:
	dput(dentry);
out_dir:
	mutex_unlock(&nd->dentry->d_inode->i_mutex);
out:
	return ret;
}

/* File system initialization */
enum {
	Opt_uid, Opt_gid, Opt_err,
};

static match_table_t spufs_tokens = {
	{ Opt_uid, "uid=%d" },
	{ Opt_gid, "gid=%d" },
	{ Opt_err, NULL  },
};

static int
spufs_parse_options(char *options, struct inode *root)
{
	char *p;
	substring_t args[MAX_OPT_ARGS];

	while ((p = strsep(&options, ",")) != NULL) {
		int token, option;

		if (!*p)
			continue;

		token = match_token(p, spufs_tokens, args);
		switch (token) {
		case Opt_uid:
			if (match_int(&args[0], &option))
				return 0;
			root->i_uid = option;
			break;
		case Opt_gid:
			if (match_int(&args[0], &option))
				return 0;
			root->i_gid = option;
			break;
		default:
			return 0;
		}
	}
	return 1;
}

static void
spufs_init_isolated_loader(void)
{
	struct device_node *dn;
	const char *loader;
	int size;

	dn = of_find_node_by_path("/spu-isolation");
	if (!dn)
		return;

	loader = get_property(dn, "loader", &size);
	if (!loader)
		return;

	/* kmalloc should align on a 16 byte boundary..* */
	isolated_loader = kmalloc(size, GFP_KERNEL);
	if (!isolated_loader)
		return;

	memcpy(isolated_loader, loader, size);
	printk(KERN_INFO "spufs: SPU isolation mode enabled\n");
}

static int
spufs_create_root(struct super_block *sb, void *data)
{
	struct inode *inode;
	int ret;

	ret = -ENOMEM;
	inode = spufs_new_inode(sb, S_IFDIR | 0775);
	if (!inode)
		goto out;

	inode->i_op = &spufs_dir_inode_operations;
	inode->i_fop = &simple_dir_operations;
	SPUFS_I(inode)->i_ctx = NULL;

	ret = -EINVAL;
	if (!spufs_parse_options(data, inode))
		goto out_iput;

	ret = -ENOMEM;
	sb->s_root = d_alloc_root(inode);
	if (!sb->s_root)
		goto out_iput;

	return 0;
out_iput:
	iput(inode);
out:
	return ret;
}

static int
spufs_fill_super(struct super_block *sb, void *data, int silent)
{
	static struct super_operations s_ops = {
		.alloc_inode = spufs_alloc_inode,
		.destroy_inode = spufs_destroy_inode,
		.statfs = simple_statfs,
		.delete_inode = spufs_delete_inode,
		.drop_inode = generic_delete_inode,
	};

	sb->s_maxbytes = MAX_LFS_FILESIZE;
	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = SPUFS_MAGIC;
	sb->s_op = &s_ops;

	return spufs_create_root(sb, data);
}

static int
spufs_get_sb(struct file_system_type *fstype, int flags,
		const char *name, void *data, struct vfsmount *mnt)
{
	return get_sb_single(fstype, flags, data, spufs_fill_super, mnt);
}

static struct file_system_type spufs_type = {
	.owner = THIS_MODULE,
	.name = "spufs",
	.get_sb = spufs_get_sb,
	.kill_sb = kill_litter_super,
};

static int __init spufs_init(void)
{
	int ret;

	ret = -ENOMEM;
	spufs_inode_cache = kmem_cache_create("spufs_inode_cache",
			sizeof(struct spufs_inode_info), 0,
			SLAB_HWCACHE_ALIGN, spufs_init_once, NULL);

	if (!spufs_inode_cache)
		goto out;
	if (spu_sched_init() != 0) {
		kmem_cache_destroy(spufs_inode_cache);
		goto out;
	}
	ret = register_filesystem(&spufs_type);
	if (ret)
		goto out_cache;
	ret = register_spu_syscalls(&spufs_calls);
	if (ret)
		goto out_fs;
	ret = register_arch_coredump_calls(&spufs_coredump_calls);
	if (ret)
		goto out_fs;

	spufs_init_isolated_loader();

	return 0;
out_fs:
	unregister_filesystem(&spufs_type);
out_cache:
	kmem_cache_destroy(spufs_inode_cache);
out:
	return ret;
}
module_init(spufs_init);

static void __exit spufs_exit(void)
{
	spu_sched_exit();
	unregister_arch_coredump_calls(&spufs_coredump_calls);
	unregister_spu_syscalls(&spufs_calls);
	unregister_filesystem(&spufs_type);
	kmem_cache_destroy(spufs_inode_cache);
}
module_exit(spufs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arnd Bergmann <arndb@de.ibm.com>");

