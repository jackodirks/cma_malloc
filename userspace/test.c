#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "contiguousMalloc.h"

int main(){
    size_t reqSize = 1.5*pow(2, 27); //192 MiB
    uintptr_t phys_addr;
    void* userspace = mallocContiguous(reqSize, &phys_addr);
    if (userspace == NULL){
        printf("Failed to aquire memory!\n");
        exit(EXIT_FAILURE);
    }
    printf("Aquired %zu MiB memory!\n", (size_t)(reqSize/pow(2, 20)));
    printf("\tPhysical address: 0x%p\n", (void*)phys_addr);
    printf("\tUserspace address: 0x%p\n", userspace);
    if (freeContiguous(phys_addr, userspace, reqSize) != 0){
        printf("Failed to free memory!\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}
