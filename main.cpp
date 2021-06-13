#include <iostream>
#include <random>
#include <vector>
#include <chrono>

#include "allocator.h"
#include "naive_allocator.h"

using point_2d = std::pair<int, int>;

const int TEST_SIZE = 5'000;
const int PICK_SIZE = 100;

auto generate_random() -> int {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, TEST_SIZE);
    /*
    std::poisson_distribution<> dis(2);
    auto result = dis(gen);
    if (result == 0) {
        result = 1;
    }
    return result * 4;
     */
    return dis(gen);
}

template <template <typename> typename Alloc>
void test() {
    using microseconds = std::chrono::microseconds;
    auto start = std::chrono::system_clock::now();

    // vector creation
    using int_vec = std::vector<int, Alloc<int>>;
    std::vector<int_vec, Alloc<int_vec>> vec_ints(TEST_SIZE);
    for (auto i = 0; i < TEST_SIZE; i++) {
        vec_ints.at(i).resize(generate_random());
    }

    using point_vec = std::vector<point_2d, Alloc<point_2d>>;
    std::vector<point_vec, Alloc<point_vec>> vec_pts(TEST_SIZE);
    for (auto i = 0; i < TEST_SIZE; i++) {
        vec_pts.at(i).resize(generate_random());
    }

    // vector resize
    for (auto i = 0; i < PICK_SIZE; i++) {
        auto idx = generate_random() - 1;
        auto size = generate_random();
        vec_ints.at(idx).resize(size);
        vec_pts.at(idx).resize(size);
    }

    // vector element assignment
    {
        auto val = 10;
        auto idx1 = generate_random() - 1;
        auto idx2 = vec_ints.at(idx1).size() / 2;
        vec_ints.at(idx1).at(idx2) = val;
        if (vec_ints.at(idx1).at(idx2) == val)
            std::cout << "correct assignment in vec_ints: " << idx1 << std::endl;
        else
            std::cout << "incorrect assignment in vec_ints: " << idx1 << std::endl;
    }
    {
        point_2d val(11, 15);
        auto idx1 = generate_random() - 1;
        auto idx2 = vec_pts.at(idx1).size() / 2;
        vec_pts.at(idx1).at(idx2) = val;
        if (vec_pts.at(idx1).at(idx2) == val)
            std::cout << "correct assignment in vec_pts: " << idx1 << std::endl;
        else
            std::cout << "incorrect assignment in vec_pts: " << idx1 << std::endl;
    }

    auto end = std::chrono::system_clock::now();

    std::cout << "cost "
            << duration_cast<microseconds>(end - start).count()
            << " microseconds"
            << std::endl;
}

int main() {
    test<yaa::naive_allocator>();
    test<yaa::allocator>();

    return 0;
}
