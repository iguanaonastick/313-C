#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "lbnToPbn.h"

static inline uint64_t get_pointer(block_t block, uint64_t index)
{
    return ((uint64_t *)&block.data)[index];
}

uint64_t lbnToPbn(CS313inode_t *inode, uint64_t lbn, uint64_t block_size, block_t (*getBlock)(uint64_t block_no))
{
    long amountOfAddresses = block_size / 8;

    if (lbn * block_size >= inode->i_size) {
        return BAD_PBN;
    }

    printf("%ld \n", lbn);

    if (lbn >= 0 && lbn < 6) {
        uint64_t pbnDirect = inode->i_direct[lbn];
        printf("%ld \n", pbnDirect);
        return pbnDirect;
    }

    if (lbn > 5 && lbn < amountOfAddresses*amountOfAddresses + 6) {
        block_t block = getBlock(inode->i_2indirect);  // get double-indirect block
        long indirectPointer = (lbn - 6) / amountOfAddresses;  // index
        uint64_t getPointer = get_pointer(block, indirectPointer);  // index into double indirect block; pointer to an indirect block
        if (getPointer == 0) {
            return HOLE_PBN;
        }
        block_t secondBlock = getBlock(getPointer);  // block for the indirect
        long indirectPointer1 = (lbn - 6) % amountOfAddresses;
        uint64_t getPointer1 = get_pointer(secondBlock, indirectPointer1);
        if (getPointer1 == 0) {
            return HOLE_PBN;
        }
        return getPointer1;
    }

    return BAD_PBN;
}
