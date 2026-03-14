#include <linux/sched/mm.h>
#include <linux/mm.h>
#include <linux/sysfs.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/fcntl.h>
#include <linux/stat.h>
#include <linux/err.h>
#include <linux/gfp_types.h>
#include <linux/init.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
#include <linux/stddef.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/cleanup.h>
DEFINE_FREE(vfree, void *, if (_T) vfree(_T));

const char* prc_name = "game_process";

bool DEBUG_MODE = true;

static int __init entry(void) {

    // hiding module from EAC
    if (!DEBUG_MODE) {
        // hides from lsmod, /proc/modules
        list_del_init(&THIS_MODULE->list);

        // hides from /sys/module/
        kobject_del(&THIS_MODULE->mkobj.kobj);

        // Remove section attributes and notes to prevent sysfs scanners from finding remnants
        if (THIS_MODULE->sect_attrs) {
            sysfs_remove_group(&THIS_MODULE->mkobj.kobj, THIS_MODULE->sect_attrs);
            kfree(THIS_MODULE->sect_attrs);
            THIS_MODULE->sect_attrs = NULL;
        }

        if (THIS_MODULE->notes_attrs) {
            sysfs_remove_group(&THIS_MODULE->mkobj.kobj, THIS_MODULE->notes_attrs);
            kfree(THIS_MODULE->notes_attrs);
            THIS_MODULE->notes_attrs = NULL;
        }

        //THIS_MODULE->mkobj.kobj.state_initialized = 0; // Dangerous on some kernels, optional
    }

    //prepare start
    struct file *filp;

    filp = filp_open("/tmp/some.txt", O_RDONLY, 0);

    if (IS_ERR(filp)) {
        DEBUG_MODE ? 0 : -ENOENT;
    }

    struct kstat fls;
    auto rett = vfs_getattr(&filp->f_path, &fls, STATX_SIZE, AT_STATX_SYNC_AS_STAT);

    if (rett) return DEBUG_MODE ? 0 : rett;

    char *buf = vzalloc(fls.size);


    loff_t pos = 0;
    ssize_t ret;

    ret = kernel_read(filp, buf, 0x40, &pos);

    filp_close(filp, NULL);

    if (ret > 40) {
        vfree(buf);
        return DEBUG_MODE ? 0 : -EAGAIN;
    }

    if (buf[0] != 0x45 || buf[1] != 0x4c || buf[2] != 0x46) {
        vfree(buf);
        return DEBUG_MODE ? 0 : -EBADF;
    }

    struct task_struct *prc;
    struct mm_struct *mm;

    rcu_read_lock();

    for_each_process(prc) {
        if (strstr(prc->comm, prc_name)) {
            mm = prc->mm;
            break;
        }
    };

    rcu_read_unlock();

    if (mm && prc)
        mm = get_task_mm(prc); // just for sure
    else{
        vfree(buf);
        return -ESRCH;
    }

    //prepare end

    // final checks
    if (sizeof(buf) > 80 ){

    }// header + sections header

    vfree(buf);
    return 0;
}

static void __exit exit(void) {
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zynarix");
MODULE_DESCRIPTION("I wish it work, but it not");
