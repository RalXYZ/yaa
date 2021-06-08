#ifndef YAA_NAIVE_ALLOCATOR_H
#define YAA_NAIVE_ALLOCATOR_H

namespace yaa {
    template <typename T>
    class naive_allocator {
    public:
        typedef T value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        [[nodiscard]] constexpr T* allocate(std::size_t n);

        constexpr void deallocate(T* p, std::size_t);
    };

    template <typename T>
    constexpr T* naive_allocator<T>::allocate(std::size_t n) {
        /*
         * since C++11, std::allocator<T>::allocate
         * throws std::bad_array_new_length if
         * std::numeric_limits<std::size_t>::max() / sizeof(T) < n.
         */
        if (std::numeric_limits<std::size_t>::max() / sizeof(T) < n) {
            throw std::bad_array_new_length();
        }

        auto buffer = reinterpret_cast<T*>(::operator new(n * sizeof(T)));

        /*
         * throws std::bad_alloc if allocation fails.
         */
        if (buffer == nullptr) {
            throw std::bad_alloc();
        }

        return buffer;
    }

    template <typename T>
    constexpr void naive_allocator<T>::deallocate(T* p, std::size_t) {
        ::operator delete(p);
    }
}

#endif //YAA_NAIVE_ALLOCATOR_H
