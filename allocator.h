#ifndef YAA_ALLOCATOR_H
#define YAA_ALLOCATOR_H

namespace yaa {
    template <typename T>
    class allocator {
    public:
        typedef T value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        [[nodiscard]] T* allocate(std::size_t n) {

        }

        void deallocate(T*, std::size_t);
    };

    template <typename T>
    void allocator::deallocate(T* p, std::size_t n) {

    }
}

#endif //YAA_ALLOCATOR_H
