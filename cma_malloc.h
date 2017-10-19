#ifndef __KERNEL__
#include <stdint.h>     // uintptr_t
#include <stdlib.h>     // size_t
#endif //__KERNEL__


#define CMA_MALLOC_MAGIC 0xBB
#define CMA_MALLOC_DEVICE_FILENAME "cma_malloc"
#define CMA_MALLOC_DEVICE_COMPLETE_FILENAME "/dev/"CMA_MALLOC_DEVICE_FILENAME

struct cma_space_request_struct {
    size_t size;                /* Size in bytes */
    uintptr_t real_addr;        /* The physical address */
    uintptr_t kern_addr;        /* The virtual address in kernel space */
};

#define CMA_MALLOC_ALLOC _IOWR(CMA_MALLOC_MAGIC, 0x1, struct cma_space_request_struct)
#define CMA_MALLOC_FREE _IOW(CMA_MALLOC_MAGIC, 0x2, struct cma_space_request_struct)
#define CMA_MALLOC_WRONG _IOW(CMA_MALLOC_MAGIC, 0x3, struct cma_space_request_struct)

