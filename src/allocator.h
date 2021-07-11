#ifndef YAA_ALLOCATOR_H
#define YAA_ALLOCATOR_H

#include <vector>
#include <memory>
#include <iostream>

#include "page.h"

namespace yaa {
    /*
     * standard declaration of allocator in C++20
     */
    template <typename T>
    class allocator {
        // page* pool = nullptr;
        static std::vector<page*> page_vec;

        [[nodiscard]]
        static constexpr auto calc_block_amount(std::size_t) noexcept -> std::size_t;
        [[nodiscard]]
        auto get_page(T*) -> page*;

    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        constexpr allocator();

        [[nodiscard]]
        constexpr auto allocate(std::size_t) -> T*;
        constexpr auto deallocate(T*, std::size_t) -> void;
    };

    template <typename T>
    std::vector<page*> allocator<T>::page_vec = std::vector<page*>();


    template<typename T>
    constexpr auto allocator<T>::calc_block_amount(std::size_t bytes_num) noexcept -> std::size_t {
        const auto applied_bytes = bytes_num * sizeof(T);

        // division in C/C++ rounds down the result, but we want to round up, so we perform "+1"
        return applied_bytes / BYTES_PER_BLOCK + (applied_bytes % BYTES_PER_BLOCK == 0 ? 0 : 1);
    }

    template <typename T>
    auto allocator<T>::get_page(T* p) -> page* {
        auto block_begin_ptr = reinterpret_cast<page::block*>(p);
        for (const auto page : this->page_vec) {
            const auto pool_block = page->pool_ptr;
            if (pool_block <= block_begin_ptr && block_begin_ptr <= pool_block + BLOCK_NUM_IN_POOL) {
                return page;
            } else {
                throw std::out_of_range("get page out of range");
            }
        }
        throw std::out_of_range("get page out of range");
    }

    template <typename T>
    constexpr allocator<T>::allocator() {
        if (page_vec.empty()) {
            auto pool = new page;
            page_vec.push_back(pool);
        }
    }

    template <typename T>
    constexpr auto allocator<T>::allocate(std::size_t n) -> T* {
#ifdef DEBUG
        std::cout << "allocate function in" << std::endl;
#endif
        /*
         * since C++11, std::allocator<T>::allocate
         * throws std::bad_array_new_length if
         * std::numeric_limits<std::size_t>::max() / sizeof(T) < n.
         */
        if (std::numeric_limits<std::size_t>::max() / sizeof(T) < n) {
            throw std::bad_array_new_length();
        }

        if (n * sizeof(T) > ALLOCATE_UPPER_BOUND) {
            return reinterpret_cast<T*>(::operator new(n * sizeof(T)));
        }

        const auto block_amount = calc_block_amount(n);

        /*
         * we need to step over the data blocks
         * before the buffer_to_return, there is a pointer,
         * which points to the first free data_block after the allocated data_block.
         * this pointer will be stored at step_begin_ptr
         */
        page::block* step_begin_ptr = nullptr;

        const auto pool = this->page_vec.front();  // FIXME

        auto buffer_to_return = pool->pool_begin_ptr;
        {
            auto iter = buffer_to_return;
            while (true) {
                if (buffer_to_return + (block_amount - 1) > pool->pool_ptr + BLOCK_NUM_IN_POOL) {
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
                    if (pool->is_ptr[iter - pool->pool_ptr]) {
                        do_not_need_jump = false;
                        break;
                    }
                }
                if (do_not_need_jump) {
                    break;
                }

                step_begin_ptr = iter;
                buffer_to_return = iter->next_ptr;
                iter = buffer_to_return;
            }
        }

        /*
         * calculate end ptr
         */
        auto step_end_ptr = buffer_to_return + block_amount;
        const auto buffer_to_return_end_block = buffer_to_return - pool->pool_ptr + (block_amount - 1);
        /*
         * if the end of block is a pointer
         * (which is still an unallocated block)
         */
        if (pool->is_ptr[buffer_to_return_end_block]) {
            pool->is_ptr[buffer_to_return_end_block] = false;
            step_end_ptr = pool->pool_ptr[buffer_to_return_end_block].next_ptr;
        }

        // update pool_end_ptr if necessary
        pool->pool_end_ptr = step_end_ptr > pool->pool_end_ptr ? step_end_ptr : pool->pool_end_ptr;

        if (step_begin_ptr == nullptr) {  // the newly allocated buffer is at the start
            pool->pool_begin_ptr = step_end_ptr;
        } else {
            pool->pool_ptr[step_begin_ptr - pool->pool_ptr].next_ptr = step_end_ptr;
        }
#ifdef DEBUG
        std::cout << "allocate function out" << std::endl;
#endif
        return reinterpret_cast<T*>(buffer_to_return);
    }

    template <typename T>
    constexpr auto allocator<T>::deallocate(T* p, std::size_t n) -> void {
        const auto block_amount = calc_block_amount(n);
        const auto block_begin_ptr = reinterpret_cast<page::block*>(p);
        const auto block_end_ptr = block_begin_ptr + block_amount - 1;
#ifdef DEBUG
        std::cout << "deallocate function in" << std::endl;
#endif
        if (n * sizeof(T) > ALLOCATE_UPPER_BOUND) {
            ::operator delete(p);
            return;
        }

        auto pool = get_page(p);

        /*
         * find the nearest pointer above the block
         * which is to be deallocated
         */
        auto step_start_ptr = block_begin_ptr;
        for (; step_start_ptr >= pool->pool_ptr; step_start_ptr--) {
            if (pool->is_ptr[step_start_ptr - pool->pool_ptr]) {
                break;
            }
        }

        auto step_end_ptr = step_start_ptr < pool->pool_ptr ? pool->pool_begin_ptr : step_start_ptr->next_ptr;
        /*
         * step_start_ptr does not exist, meaning
         * there are adjacent data blocks
         * all the way to the front of the pool_ptr
         */
        if (step_start_ptr < pool->pool_ptr) {
            pool->pool_begin_ptr = block_begin_ptr;
        } else {
            if (step_start_ptr == block_begin_ptr - 1) {
                pool->is_ptr[step_start_ptr - pool->pool_ptr] = false;
            } else {
                step_start_ptr->next_ptr = block_begin_ptr;
            }
        }

        /*
         * update pool_end_ptr if the block
         * is at the end of the pool
         */
        if (block_end_ptr + 1 == pool->pool_end_ptr) {
            pool->pool_end_ptr = block_begin_ptr;
        }

        /*
         * check if necessary to change
         * the ending deallocated block to a pointer,
         * and point it to the nearest unallocated block below.
         */
        if (step_end_ptr > block_end_ptr + 1) {
            pool->is_ptr[block_end_ptr - pool->pool_ptr] = true;
            block_end_ptr->next_ptr = step_end_ptr;
        }
#ifdef DEBUG
        std::cout << "block back_ptr at " << block_end_ptr << std::endl;
        std::cout << "pool_ptr end_ptr at " << pool->pool_end_ptr << std::endl;
        std::cout << "deallocate function out" << std::endl;
#endif
    }
}

#endif //YAA_ALLOCATOR_H
