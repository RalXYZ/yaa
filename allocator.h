#ifndef YAA_ALLOCATOR_H
#define YAA_ALLOCATOR_H

#include <iostream>

const std::size_t BLOCK_NUM_IN_POOL = 10'000;
using data_block = uint64_t;
const std::size_t BYTES_PER_BLOCK = sizeof(data_block) / sizeof(uint8_t);

union block {
    data_block blk;
    block *next_ptr;
};

block *pool = nullptr;
bool *is_ptr = nullptr;
block *pool_start_ptr = nullptr;
block *pool_end_ptr = nullptr;

namespace yaa {
    template <typename T>
    class allocator {
        [[nodiscard]]
        static constexpr auto calc_block_amount(std::size_t) noexcept -> std::size_t;

    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        constexpr allocator();

        [[nodiscard]]
        constexpr auto allocate(std::size_t) -> T*;
        constexpr auto deallocate(T*, std::size_t) -> void;
    };

    template<typename T>
    constexpr auto allocator<T>::calc_block_amount(std::size_t bytes_num) noexcept -> std::size_t {
        const auto applied_bytes = bytes_num * sizeof(T);

        // division in C/C++ rounds down the result, but we want to round up, so we perform "+1"
        return applied_bytes / BYTES_PER_BLOCK + (applied_bytes % BYTES_PER_BLOCK == 0 ? 0 : 1);
    }

    template <typename T>
    constexpr allocator<T>::allocator() {
        if (pool == nullptr) {
            pool = new block[BLOCK_NUM_IN_POOL];
            pool_start_ptr = pool;
            pool_end_ptr = pool_start_ptr;

            is_ptr = new bool[BLOCK_NUM_IN_POOL];
            for (auto i = 0; i < BLOCK_NUM_IN_POOL; i++) {
                is_ptr[i] = false;
            }

            std::cout << "pool start at " << pool << std::endl;
        }
    }

    template <typename T>
    constexpr auto allocator<T>::allocate(std::size_t n) -> T* {

        std::cout << "allocate function in" << std::endl;

        /*
         * since C++11, std::allocator<T>::allocate
         * throws std::bad_array_new_length if
         * std::numeric_limits<std::size_t>::max() / sizeof(T) < n.
         */
        if (std::numeric_limits<std::size_t>::max() / sizeof(T) < n) {
            throw std::bad_array_new_length();
        }

        const auto block_amount = calc_block_amount(n);

        /*
         * we need to step over the data blocks
         * before the buffer_to_return, there is a pointer,
         * which points to the first free data_block after the allocated data_block.
         * this pointer will be stored at step_start_ptr
         */
        block* step_start_ptr = nullptr;
        auto buffer_to_return = pool_start_ptr;
        {
            auto iter = buffer_to_return;
            while (true) {
                if (buffer_to_return + (block_amount - 1) > pool + BLOCK_NUM_IN_POOL) {
                    throw std::bad_alloc();
                }
                bool do_not_need_jump = true;

                /*
                 * -1 because we do not care about what is in the last data_block to allocate.
                 * if the last data_block stores is empty, then nothing will happen.
                 * if the last data_block stores a pointer, this means the data_block after this data_block stores data,
                 * this means the newly allocated buffer is adjacent to the next buffer.
                 */
                for (int i = 0; i < block_amount - 1; i++, iter++) {
                    if (is_ptr[iter - pool]) {
                        do_not_need_jump = false;
                        break;
                    }
                }
                if (do_not_need_jump) {
                    break;
                }

                step_start_ptr = iter;
                buffer_to_return = iter->next_ptr;
                iter = buffer_to_return;
            }
        }

        auto step_end_ptr = buffer_to_return + block_amount;
        const auto buffer_to_return_end_block = buffer_to_return - pool + (block_amount - 1);
        if (is_ptr[buffer_to_return_end_block]) {
            is_ptr[buffer_to_return_end_block] = false;
            step_end_ptr = pool[buffer_to_return_end_block].next_ptr;
        }

        // update pool_end_ptr if necessary
        pool_end_ptr = step_end_ptr > pool_end_ptr ? step_end_ptr : pool_end_ptr;

        if (step_start_ptr == nullptr) {  // the newly allocated buffer is at the start
            pool_start_ptr = step_end_ptr;
        } else {
            pool[step_start_ptr - pool].next_ptr = step_end_ptr;
        }

        std::cout << "allocate function out" << std::endl;

        return reinterpret_cast<T*>(buffer_to_return);
    }

    template <typename T>
    constexpr auto allocator<T>::deallocate(T* p, std::size_t n) -> void {
        const auto block_amount = calc_block_amount(n);
        const auto block_front_ptr = reinterpret_cast<block*>(p);
        const auto block_back_ptr = block_front_ptr + block_amount - 1;

        std::cout << "deallocate function in" << std::endl;

        auto step_start_ptr = block_front_ptr;
        for (; step_start_ptr >= pool; step_start_ptr--) {
            if (is_ptr[step_start_ptr - pool]) {
                break;
            }
        }

        auto step_end_ptr = step_start_ptr < pool ? pool_start_ptr : step_start_ptr->next_ptr;
        /*
         * step_start_ptr does not exist, meaning
         * there are adjacent data blocks
         * all the way to the front of the pool
         */
        if (step_start_ptr < pool) {
            pool_start_ptr = block_front_ptr;
        } else {
            if (step_start_ptr == block_front_ptr - 1) {
                is_ptr[step_start_ptr - pool] = false;
            } else {
                step_start_ptr->next_ptr = block_front_ptr;
            }
        }

        if (block_back_ptr + 1 == pool_end_ptr) {
            pool_end_ptr = block_front_ptr;
        }

        if (step_end_ptr > block_back_ptr + 1) {
            is_ptr[block_back_ptr - pool] = true;
            block_back_ptr->next_ptr = step_end_ptr;
        }
        std::cout << "block back_ptr at " << block_back_ptr << std::endl;
        std::cout << "pool end_ptr at " << pool_end_ptr << std::endl;
        std::cout << "deallocate function out" << std::endl;
    }
}

#endif //YAA_ALLOCATOR_H
