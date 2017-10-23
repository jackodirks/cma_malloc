#pragma once
#include <stdint.h>     // uintptr_t
#include <stdint.h>     // uintptr_t
/**
 * @brief Tries to allocate a pyhiscally continuous piece of memory of size size
 * @param size The size you try to allocate (should be a multiple of the kernel page size)
 * @param phys_addr The physical address of the allocated area is defined here
 * @return A userspace pointer to the allocated area
 *
 * @warning There is no automatic cleanup
 */
void* mallocContiguous(const size_t size, uintptr_t* const phys_addr);

/**
 * Frees the earlier 
 */
int freeContiguous(const uintptr_t phys_addr, void* const ptr, const size_t length);
