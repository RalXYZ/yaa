#ifndef YAA_PAGE_H
#define YAA_PAGE_H

#include <cstdint>

const std::size_t BLOCK_NUM_IN_POOL = 100'000;
const std::size_t ALLOCATE_UPPER_BOUND = 0x1000;
using data_block = uint64_t;
const std::size_t BYTES_PER_BLOCK = sizeof(data_block) / sizeof(uint8_t);

struct page {
    union block {
        data_block blk;
        block *next_ptr;
    };

    page();

    block *pool_ptr = nullptr;
    bool *is_ptr = nullptr;
    block *pool_begin_ptr = nullptr;
    block *pool_end_ptr = nullptr;
};

#endif //YAA_PAGE_H
