// System include files
#include <string.h>     // strerror
#include <stdio.h>      // printf etc
#include <errno.h>      // Errno
#include <fcntl.h>      // Open
#include <unistd.h>     // Close
#include <sys/ioctl.h>  // ioctl
#include <sys/mman.h>   // mmap

// User include files
#include "cma_malloc.h"
#include "contiguousMalloc.h"

#ifdef DEBUG
    #define DEBUGPRINT 1
#else
    #define DEBUGPRINT 0
#endif //DEBUG

#define maybe_printf(fmt, ...) do { if (DEBUGPRINT) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

void* mallocContiguous(const size_t size, uintptr_t* const phys_addr){
    struct cma_space_request_struct req = {
        .size = size
    };
    int fd = open(CMA_MALLOC_DEVICE_COMPLETE_FILENAME, O_RDWR);
    if (fd < 0){
        maybe_printf("Open failed! Error: %d (%s)\n", errno, strerror(errno));
        return NULL;
    }
    if (ioctl(fd, CMA_MALLOC_ALLOC, &req) != 0){
        maybe_printf("ioctl CMA_MALLOC_ALLOC failed: %d (%s)\n", errno, strerror(errno));
        return NULL;
    }
    // We have everything, map the kernel address to userspace and be done!
    *phys_addr = req.real_addr;
    void* map_addr = mmap(
            NULL,
            size,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            fd,
            req.real_addr
            );
    if (map_addr == MAP_FAILED){
        maybe_printf("mmap failed: %d (%s)\n", errno, strerror(errno));
        return NULL;
    }
    if (close(fd)){
        maybe_printf("Closing the file descriptor failed: %d (%s)", errno, strerror(errno));
    }
    return map_addr;
}

int freeContiguous(const uintptr_t phys_addr, void* const ptr, const size_t length){
    // The kernel module requires only the physical address to release the contiguous memory
    struct cma_space_request_struct req = {
        .real_addr = phys_addr
    };
    if (munmap(ptr, length) != 0){
        maybe_printf("Munmap failed: %d (%s)\n", errno, strerror(errno));
        return -1;
    }
    int fd = open(CMA_MALLOC_DEVICE_COMPLETE_FILENAME, O_RDWR);
    if (fd < 0){
        maybe_printf("Open failed! Error: %d (%s)\n", errno, strerror(errno));
        return -1;
    }
    if (ioctl(fd, CMA_MALLOC_FREE, &req) != 0){
        maybe_printf("ioctl CMA_MALLOC_FREE failed: %d(%s)\n", errno, strerror(errno));
        return -1;
    }
    close(fd);
    return 0;
}
