#include <iostream>
#include <random>
#include <vector>

#include "allocator.h"
#include "naive_allocator.h"

template <typename T>
using my_allocator = yaa::allocator<T>;

using point_2d = std::pair<int, int>;

const int TEST_SIZE = 10;
const int PICK_SIZE = 10;

void test() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, TEST_SIZE);

    // vector creation
    using int_vec = std::vector<int, my_allocator<int>>;
    std::vector<int_vec, my_allocator<int_vec>> vec_ints(TEST_SIZE);
    for (auto i = 0; i < TEST_SIZE; i++) {
        vec_ints.at(i).resize(dis(gen));
    }

    using point_vec = std::vector<point_2d, my_allocator<point_2d>>;
    std::vector<point_vec, my_allocator<point_vec>> vec_pts(TEST_SIZE);
    for (auto i = 0; i < TEST_SIZE; i++) {
        vec_pts.at(i).resize(dis(gen));
    }

    // vector resize
    for (auto i = 0; i < PICK_SIZE; i++) {
        auto idx = dis(gen) - 1;
        auto size = dis(gen);
        vec_ints.at(idx).resize(size);
        vec_pts.at(idx).resize(size);
    }

    // vector element assignment
    {
        auto val = 10;
        auto idx1 = dis(gen) - 1;
        auto idx2 = vec_ints.at(idx1).size() / 2;
        vec_ints.at(idx1).at(idx2) = val;
        if (vec_ints.at(idx1).at(idx2) == val)
            std::cout << "correct assignment in vec_ints: " << idx1 << std::endl;
        else
            std::cout << "incorrect assignment in vec_ints: " << idx1 << std::endl;
    }
    {
        point_2d val(11, 15);
        auto idx1 = dis(gen) - 1;
        auto idx2 = vec_pts.at(idx1).size() / 2;
        vec_pts.at(idx1).at(idx2) = val;
        if (vec_pts.at(idx1).at(idx2) == val)
            std::cout << "correct assignment in vec_pts: " << idx1 << std::endl;
        else
            std::cout << "incorrect assignment in vec_pts: " << idx1 << std::endl;
    }
}

int main() {
    test();
    using int_vec = std::vector<int, yaa::naive_allocator<int>>;
    int_vec vec(TEST_SIZE);

    return 0;
}
