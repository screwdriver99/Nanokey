#ifndef NVMEM_H
#define NVMEM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"

#define NV_INVALID_ID 0xffffffff

// function return codes:
#define NV_OK 0
#define NV_ERR_INVALID_ID 1
#define NV_ERR_WRITE 2
#define NV_ERR_NOTFOUND 3
#define NV_ERR_MEM_FULL 4
#define NV_ERR_ERASE 5

    // Initialize the non-volatile storage. Return the number of found variables
    uint32_t nvInit();

    // Format the entire memory region. Reset the internal cache.
    // This function is callable before or after init.
    uint32_t nvFormat();

    // Read a variable from the storage
    uint32_t nvRead(uint32_t id, uint64_t* data);

    // Write a variable to the storage
    uint32_t nvWrite(uint32_t id, uint64_t data);

    // Remove a variable from the storage. The effective removal will be made by nvCleanup()
    uint32_t nvRemove(uint32_t id);

    // Cleanup the storage, removing all the unnecessary or invalidated items
    uint32_t nvCleanup();

#ifdef __cplusplus
}
#endif

#endif /* NVMEM_H */
