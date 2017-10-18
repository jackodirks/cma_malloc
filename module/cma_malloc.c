// Linux headers
#include <linux/fs.h>               // Because this is a character device
#include <linux/miscdevice.h>       // For the MISC_DYNAMIC_MINOR macro and the miscdevice struct
#include <linux/stat.h>             // Has permission macro's
#include <linux/module.h>           // Contains the THIS_MODULE macro
#include <linux/mutex.h>            // Declares the spinlock
#include <linux/slab.h>             // Kmalloc/kfree
#include <linux/uaccess.h>          // copy_from_user and copy_to_user
#include <linux/dma-mapping.h>      // dma_alloc_*, dma_free
#include <asm/uaccess.h>            // access_ok

#include "../cma_malloc.h"          // Contains module specific information like magic number

/**
 * YOU DO NOT WANT TO USE CMA LIKE THIS
 *
 * There is no good reason to have physically continious memory unless a different device is involved.
 * If a different device is involved, you want to write a driver.
 *
 * This code is a nice exercise and can be used while developing hardware. Try not to use it in production.
 */

static DEFINE_MUTEX(cma_lock);
static dma_addr_t dma_handle;
static struct device* dma_dev;
static size_t size;
static void* cpu_addr;


// CMA_space should be valid on call
static long allocate(struct cma_space_request_struct* req){
    long retval;
    void* virt_addr;
    if ((void*)dma_handle != NULL) return ENOMEM;
    mutex_lock_interruptible(&cma_lock);
    printk("Requested amount of memory: %zu\n", req->size);
    virt_addr = dma_alloc_coherent(dma_dev, req->size, &dma_handle, GFP_USER);
    if (virt_addr == NULL){
        retval = ENOMEM;
    } else {
        cpu_addr = virt_addr;
        size = req->size;
        req->real_addr = dma_handle;
        req->kern_addr = (u64)virt_addr;
        req->user_addr = (u64)virt_addr;
        retval = 0;
    }
    mutex_unlock(&cma_lock);
    return retval;
}

static long deallocate(struct cma_space_request_struct* req){
    long retval = 0;
    mutex_lock_interruptible(&cma_lock);
    if (dma_handle == req->real_addr){
        dma_free_coherent(dma_dev, size, cpu_addr, dma_handle);
        dma_handle = 0;
        retval = 0;
    } else {
        retval = EINVAL;
    }
    mutex_unlock(&cma_lock);
    return retval;
}

static long cma_malloc_ioctl(struct file* fptr, const unsigned int cmd, const unsigned long arg){
    struct cma_space_request_struct *userReq, req;
    long retval;
    userReq = (void*)arg;
    if (!access_ok(VERIFY_WRITE, userReq, sizeof(struct cma_space_request_struct))) return -EFAULT;
    if (copy_from_user(&req, userReq, sizeof(struct cma_space_request_struct)) != 0) return -EBADE;
    switch(cmd){
        case CMA_MALLOC_ALLOC:
            retval = -1*allocate(&req);
            if (retval == 0){
                if (copy_to_user(userReq, &req, sizeof(struct cma_space_request_struct)) != 0) return -EBADE;
            }
            return retval;
        case CMA_MALLOC_FREE:
            return -1*deallocate(&req);
            return 0;
        default:
            return -EINVAL;
            break;
    }
    return retval;
}

static struct file_operations cma_malloc_fileops = {
    owner           :   THIS_MODULE,
    unlocked_ioctl  :   cma_malloc_ioctl
};

static struct miscdevice cma_malloc_miscdevice = {
    minor           :   MISC_DYNAMIC_MINOR,
    name            :   CMA_MALLOC_DEVICE_FILENAME,
    fops            :   &cma_malloc_fileops,
    mode            :   S_IRUGO | S_IWUGO,
};

static int __init cma_malloc_init(void){
    int ret;
    ret = misc_register(&cma_malloc_miscdevice);
    if (ret){
        printk("Misc register failed: %d\n", ret);
    } else {
        dma_dev = cma_malloc_miscdevice.this_device;
        dma_dev->coherent_dma_mask = DMA_BIT_MASK(32);
        dma_dev->dma_mask = &dma_dev->coherent_dma_mask;
    }
    return ret;
}

static void __exit cma_malloc_exit(void){
    misc_deregister(&cma_malloc_miscdevice);
}

module_init(cma_malloc_init);
module_exit(cma_malloc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("J. A. Dirks <jacko.dirks@gmail.com>");
MODULE_DESCRIPTION("Provides a way to pass a chunk of CMA space to userspace");
MODULE_VERSION("0.1");
