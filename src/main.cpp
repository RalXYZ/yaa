#include "test.h"
#include "allocator.h"
#include "naive_allocator.h"

auto main() -> int {
    test<yaa::naive_allocator>();

    std::cout << "my allocate function start" << std::endl;
    test<yaa::allocator>();

    return 0;
}
