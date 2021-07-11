#ifndef YAA_PAGE_H
#define YAA_PAGE_H

#include <cstdint>

/*
 * this defines the amount of block in a pool
 */
const std::size_t BLOCK_NUM_IN_POOL = 100'000;

/*
 * If the requested memory is greater than this size,
 * then the allocator will not in responsible for managing it.
 * ::operator new will be called.
 * The implementation of deallocation procedure is similar.
 */
const std::size_t ALLOCATE_UPPER_BOUND = 0x1000;

/*
 * size of a data block
 */
using data_block = uint64_t;

const std::size_t BYTES_PER_BLOCK = sizeof(data_block) / sizeof(uint8_t);

struct page {
    union block {
        data_block blk;   // stores data
        block *next_ptr;  // stores a pointer to the next free block
    };

    page();

    /*
     * This value will be set true if an allocation on this page failed
     * because of lack of space, and set false when any deallocation
     * operation has been executed. If this value has been set false,
     * it doesn't means that no space are left in the pool; what it
     * means is that the possibility of one successful allocation in
     * this page is relatively small, so it will efficient to try to
     * allocate in other pages.
     */
    bool is_full = false;

    /*
     * TODO:
     * this value counts the allocated block in the current pool,
     * but it is unused currently
     */
    unsigned int allocated_block = 0;

    /*
     * this pointer points to the allocated pool, which is a array
     */
    block *pool_ptr = nullptr;

    /*
     * Since every block is a union, we need to store whether a block
     * stores data or a pointer, which is the significance of this
     * member variable.
     * This pointer points to a bool array.
     */
    bool *is_ptr = nullptr;

    block *pool_begin_ptr = nullptr;
    block *pool_end_ptr = nullptr;
};

#endif //YAA_PAGE_H
