#include "test.h"
#include "allocator.h"

/*
 * feel free to modify the value these constants
 */
const int TEST_SIZE = 10'000;
const int PICK_SIZE = 1000;

auto main() -> int {
    std::cout << "The following allocation procedure uses std::allocator." << std::endl;
    test<std::allocator>();
    std::cout << std::endl;

    std::cout << "The following allocation is written by myself."<< std::endl;
    std::cout << "It might be a little bit slower than the STL implementation,"<< std::endl;
    std::cout << "but I really tried my best and did all the work on my own,"<< std::endl;
    std::cout << "and I think it is hard to surpass STL."<< std::endl;
    test<yaa::allocator>();

    return 0;
}
