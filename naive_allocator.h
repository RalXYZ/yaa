#ifndef YAA_NAIVE_ALLOCATOR_H
#define YAA_NAIVE_ALLOCATOR_H

namespace yaa {
    template <typename T>
    class naive_allocator {
        typedef T value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

    public:
        [[nodiscard]] constexpr T* allocate(std::size_t n) {
            auto buf = reinterpret_cast<T*>(::operator new[](n * sizeof(T)));
            if (buf == 0) {
                throw std::bad_alloc();
            }
            return buf;
        }

        constexpr void deallocate(T* buf, std::size_t) {
            ::operator delete[](buf);
        }
    };
}


#endif //YAA_NAIVE_ALLOCATOR_H
