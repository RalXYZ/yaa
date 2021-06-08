#ifndef YAA_NAIVE_ALLOCATOR_H
#define YAA_NAIVE_ALLOCATOR_H

namespace yaa {
    template <typename T>
    class naive_allocator {
    public:
        typedef T value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        [[nodiscard]] constexpr T* allocate(std::size_t n) {
            auto buffer = reinterpret_cast<T*>(::operator new[](n * sizeof(T)));
            if (buffer == nullptr) {
                throw std::bad_alloc();
            }
            return buffer;
        }

        constexpr void deallocate(T* p, std::size_t) {
            ::operator delete[](p);
        }
    };
}

#endif //YAA_NAIVE_ALLOCATOR_H
