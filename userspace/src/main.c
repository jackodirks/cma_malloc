// System include file2
#include <stdio.h>                  // printf etc
#include <fcntl.h>                  // Open
#include <string.h>                 // strerror
#include <errno.h>                  // Errno
#include <stdlib.h>                 // Exit
#include <sys/ioctl.h>              // ioctl
#include <math.h>                   // Power
#include <sys/mman.h>               // mmap
#include <unistd.h>                 // close
// User include files
#include "cma_malloc.h"

int main(){
    size_t reqSize = pow(2, 27);
    struct cma_space_request_struct req = {
        size        :      (1.5)*reqSize
    };

    int fd = open(CMA_MALLOC_DEVICE_COMPLETE_FILENAME, O_RDWR);
    if (fd < 0){
        printf("Open failed! Error: %d (%s)\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    // Check all return values
    int retval = ioctl(fd, CMA_MALLOC_ALLOC, &req);
    if (retval != 0){
        printf("ioctl CMA_MALLOC_ALLOC failed: %d (%s)\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    // Do a mmap
    void* map_addr = mmap(
            NULL,
            reqSize,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            fd,
            req.real_addr
            );
    if (map_addr == MAP_FAILED) printf("Unable to mmap: %d (%s)\n", errno, strerror(errno));
    printf("I got the following:\n");
    printf("\tSize:\t\t\t\t%zu KiB\n", req.size/1024);
    printf("\tReal (physical) address:\t%p\n", (void*)req.real_addr);
    printf("\tKernels virtal address:\t\t%p\n", (void*)req.kern_addr);
    printf("\tUsers' virtual address:\t\t%p\n", map_addr);
    if (map_addr != MAP_FAILED){
        retval = munmap(map_addr, reqSize);
        if (retval != 0){
            printf("munmap failed: %d(%s)\n", errno, strerror(errno));
        }
    } else {
        printf("munmap omitted since mmap failed\n");
    }
    retval = ioctl(fd, CMA_MALLOC_FREE, &req);
    if (retval != 0){
        printf("ioctl CMA_MALLOC_FREE failed: %d(%s)\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (close(fd)){
        printf("Failed to close file: %d (%s)", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}
