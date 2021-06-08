#ifndef YAA_ALLOCATOR_H
#define YAA_ALLOCATOR_H

const std::size_t BYTES_PER_BLOCK = 16;  // 16 bytes


namespace yaa {
    template <typename T>
    class allocator {
    public:
        typedef T value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        [[nodiscard]] T* allocate(std::size_t);

        void deallocate(T*, std::size_t);
    };

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

        auto applied_bytes = n * sizeof(T);

        // division in C/C++ rounds down the result, but we want to round up, so we perform "+1"
        auto block_amount = applied_bytes / BYTES_PER_BLOCK + 1;

        auto buffer = reinterpret_cast<T*>(::operator new(block_amount * BYTES_PER_BLOCK));

        /*
         * throws std::bad_alloc if allocation fails.
         */
        if (buffer == nullptr) {
            throw std::bad_alloc();
        }

        return buffer;
    }

    template <typename T>
    void allocator<T>::deallocate(T* p, std::size_t) {
        ::operator delete(p);
    }
}

#endif //YAA_ALLOCATOR_H
