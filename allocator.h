#ifndef YAA_ALLOCATOR_H
#define YAA_ALLOCATOR_H

// #include <variant>

const std::size_t BLOCK_NUM_IN_POOL = 10'000;
using block = uint64_t;
const std::size_t BYTES_PER_BLOCK = sizeof(block) / sizeof(uint8_t);

union asd {
    block blk;
    asd *next_ptr;
};

asd *pool = nullptr;
bool *is_block_ptr = nullptr;
asd *pool_start_ptr = nullptr;
asd *pool_end_ptr = nullptr;

namespace yaa {
    template <typename T>
    class allocator {

    public:
        typedef T value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        constexpr allocator();

        [[nodiscard]] T* allocate(std::size_t);
        void deallocate(T*, std::size_t);
    };

    template <typename T>
    constexpr allocator<T>::allocator() {
        if (pool == nullptr) {
            pool = new asd[BLOCK_NUM_IN_POOL];
            pool_start_ptr = pool;
            pool_end_ptr = pool_start_ptr;

            is_block_ptr = new bool[BLOCK_NUM_IN_POOL];
            for (auto i = 0; i < BLOCK_NUM_IN_POOL; i++) {
                is_block_ptr[i] = false;
            }
        }
    }

    template <typename T>
    T* allocator<T>::allocate(std::size_t n) {
        /*
         * since C++11, std::allocator<T>::allocate
         * throws std::bad_array_new_length if
         * std::numeric_limits<std::size_t>::max() / sizeof(T) < n.
         */
        if (std::numeric_limits<std::size_t>::max() / sizeof(T) < n) {
            throw std::bad_array_new_length();
        }

        const auto applied_bytes = n * sizeof(T);

        // division in C/C++ rounds down the result, but we want to round up, so we perform "+1"
        const auto block_amount = applied_bytes / BYTES_PER_BLOCK + (applied_bytes % BYTES_PER_BLOCK == 0 ? 0 : 1);

        /*
         * we need to step over the data blocks
         * before the buffer_to_return, there is a pointer,
         * which points to the first free block after the allocated block.
         * this pointer will be stored at step_start_ptr
         */
        asd* step_start_ptr = nullptr;
        auto buffer_to_return = pool_start_ptr;
        for (auto iter = buffer_to_return; ; step_start_ptr = iter, buffer_to_return = iter->next_ptr) {
            if (buffer_to_return + (block_amount - 1) > pool + BLOCK_NUM_IN_POOL) {
                throw std::bad_alloc();
            }
            bool do_not_need_jump = true;

            /*
             * -1 because we do not care about what is in the last block to allocate.
             * if the last block stores is empty, then nothing will happen.
             * if the last block stores a pointer, this means the block after this block stores data,
             * this means the newly allocated buffer is adjacent to the next buffer.
             */
            for (int i = 0; i < block_amount - 1; i++, iter++) {
                if (is_block_ptr[iter - pool]) {
                    do_not_need_jump = false;
                    break;
                }
            }
            if (do_not_need_jump) {
                break;
            }
        }

        auto step_end_ptr = buffer_to_return + block_amount;
        const auto buffer_to_return_end_block = buffer_to_return - pool + (block_amount - 1);
        if (is_block_ptr[buffer_to_return_end_block]) {
            is_block_ptr[buffer_to_return_end_block] = false;
            step_end_ptr = pool[buffer_to_return_end_block].next_ptr;
        }

        // update pool_end_ptr if necessary
        pool_end_ptr = step_end_ptr > pool_end_ptr ? step_end_ptr : pool_end_ptr;

        if (step_start_ptr == nullptr) {  // the newly allocated buffer is at the start
            pool_start_ptr = step_end_ptr;
        } else {
            pool[step_start_ptr - pool].next_ptr = step_end_ptr;
        }

        return reinterpret_cast<T*>(buffer_to_return);
    }

    template <typename T>
    void allocator<T>::deallocate(T* p, std::size_t) {
        // ::operator delete(p);
    }
}

#endif //YAA_ALLOCATOR_H
