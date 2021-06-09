#ifndef YAA_NAIVE_ALLOCATOR_H
#define YAA_NAIVE_ALLOCATOR_H

namespace yaa {
    template <typename T>
    class naive_allocator {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        [[nodiscard]]
        constexpr auto allocate(std::size_t) -> T*;
        constexpr auto deallocate(T*, std::size_t);
    };

    template <typename T>
    constexpr auto naive_allocator<T>::allocate(std::size_t n) -> T* {
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
    constexpr auto naive_allocator<T>::deallocate(T* p, std::size_t) {
        ::operator delete(p);
    }
}

#endif //YAA_NAIVE_ALLOCATOR_H
