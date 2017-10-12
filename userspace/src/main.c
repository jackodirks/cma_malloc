// System include files
#include <stdio.h>                  // printf etc
#include <fcntl.h>                  // Open
#include <string.h>                 // strerror
#include <errno.h>                  // Errno
#include <stdlib.h>                 // Exit
#include <sys/ioctl.h>              // ioctl
#include <unistd.h>                 // Sleep
#include <math.h>                   // Power
// User include files
#include "cma_malloc.h"

int main(){
    size_t reqSize = pow(2, 27);
    struct cma_space_request_struct req = {
        size        :      (1.5)*reqSize
    };

    int fd = open(CMA_MALLOC_DEVICE_COMPLETE_FILENAME, 0);
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
    printf("I got the following:\n");
    printf("\tSize:\t\t\t\t%zu KiB\n", req.size/1024);
    printf("\tReal (physical) address:\t%p\n", (void*)req.real_addr);
    printf("\tKernels virtal address:\t\t%p\n", (void*)req.kern_addr);
    printf("\tUsers virtual address:\t\t%p\n", (void*)req.user_addr);
    sleep(5);
    retval = ioctl(fd, CMA_MALLOC_FREE, &req);
    if (retval != 0){
        printf("ioctl CMA_MALLOC_FREE failed: %d(%s)\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}
