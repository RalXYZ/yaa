#include "test.h"
#include "allocator.h"

/*
 * feel free to modify the value these constants
 */
const int TEST_SIZE = 10'000;
const int PICK_SIZE = 1000;

bool test_fragment = false;

auto main() -> int {
    std::cout << "The following allocation procedure uses std::allocator." << std::endl;
    test<std::allocator>();
    std::cout << std::endl;

    std::cout << "The following allocation is written by myself." << std::endl;
    std::cout << "It might be a little bit slower than the STL implementation," << std::endl;
    std::cout << "but I really tried my best and did all the work on my own," << std::endl;
    std::cout << "and I think it is hard to surpass STL." << std::endl;
    test<yaa::allocator>();
    std::cout << std::endl;

    std::cout << "-----------------------bonus test case-------------------------" << std::endl;
    test_fragment = true;
    std::cout << std::endl;
    std::cout << "Next, I will show the allocator's capability to allocate small memory." << std::endl;
    std::cout << "I will resize every vector to 2 elements.";

    std::cout << std::endl;
    std::cout << "The following allocation procedure uses std::allocator." << std::endl;
    test<std::allocator>();
    std::cout << std::endl;

    std::cout << "The following allocation is written by myself." << std::endl;
    test<yaa::allocator>();
    std::cout << std::endl;

    std::cout << "In the bonus test shown above, the performance of the allocator implemented by myself" << std::endl;
    std::cout << "performs almost the same, and sometimes even better compared to std::allocator," << std::endl;
    std::cout << "which implies my allocator performs better when the allocated memory is relatively small." << std::endl;

    std::cout << std::endl;
    std::cout << "This is the end of the demonstration. Thank you for your time. Have a nice day ;)" << std::endl;

    return 0;
}
