#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "contiguousMalloc.h"

int main(){
    size_t reqSize = pow(2, 26); //64 MiB
    uintptr_t phys_addr;
    void* userspace = mallocContiguous(reqSize, &phys_addr);
    if (userspace == NULL){
        printf("Failed to aquire memory!\n");
        exit(EXIT_FAILURE);
    }
    memset(userspace, 1, reqSize);
    printf("Aquired %zu MiB memory!\n", (size_t)(reqSize/pow(2, 20)));
    printf("\tPhysical address: %p\n", (void*)phys_addr);
    printf("\tUserspace address: %p\n", userspace);
    if (freeContiguous(phys_addr, userspace, reqSize) != 0){
        printf("Failed to free memory!\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}
