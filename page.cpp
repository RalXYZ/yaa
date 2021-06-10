#include "page.h"

page::page() {
    pool_ptr = new block[BLOCK_NUM_IN_POOL];
    pool_start_ptr = pool_ptr;
    pool_end_ptr = pool_start_ptr;

    is_ptr = new bool[BLOCK_NUM_IN_POOL];
    for (auto i = 0; i < BLOCK_NUM_IN_POOL; i++) {
        is_ptr[i] = false;
    }
}
