#ifndef YAA_TEST_H
#define YAA_TEST_H

#include <iostream>
#include <random>
#include <vector>
#include <chrono>

const int TEST_SIZE = 5'000;
const int PICK_SIZE = 100;

static auto generate_random(int) -> int;

template <typename Tp, template <typename> typename Alloc>
static auto test_arg(Tp&, Tp&) -> void;

template <template <typename> typename Alloc>
auto test() -> void;

/***************************************************************************/

auto generate_random(const int max = TEST_SIZE) -> int {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, max);
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

template <typename Tp, template <typename> typename Alloc>
auto test_arg(const Tp& test_val, const Tp& fill_value) -> void {
    using one_dim_vec = std::vector<Tp, Alloc<Tp>>;
    using two_dim_vec = std::vector<one_dim_vec, Alloc<one_dim_vec>>;
    two_dim_vec vec(TEST_SIZE);
    for (auto i = 0; i < TEST_SIZE; i++) {
        vec.at(i).resize(generate_random());
    }

    // vector resize
    for (auto i = 0; i < PICK_SIZE; i++) {
        auto idx = generate_random() - 1;
        auto size = generate_random();
        vec.at(idx).resize(size);
    }

    /*
     * This is a validation code block.
     * We need to validate if the value stores
     * in an element hasn't been affected
     * after changes on other arbitrary elements.
     */
    {
        // randomly generates the index of first dimension
        const auto idx_1 = generate_random() - 1;
        // randomly generates the index of the second dimension
        const auto idx_2 = generate_random(vec.at(idx_1).size()) - 1;
        // assign val to the random position generated above
        vec.at(idx_1).at(idx_2) = test_val;

        for (int i = 0; i < 10000; i++) {  // assign INT32_MAX to some arbitrary elements in 2D vector
            const auto temp_1 = generate_random() - 1;
            const auto temp_2 = generate_random(vec.at(temp_1).size()) - 1;
            if (idx_1 == temp_1 && idx_2 == temp_2) {
                continue;
            }
            vec.at(temp_1).at(temp_2) = fill_value;
        }

        // validation
        if (vec.at(idx_1).at(idx_2) == test_val) {  // check if the value hasn't been affected
            std::cout << "correct assignment in vec_ints: " << idx_1 << std::endl;
        }
        else {
            std::cerr << "incorrect assignment in vec_ints: " << idx_1 << std::endl;
        }
    }
}

template <template <typename> typename Alloc>
auto test() -> void {
    using microseconds = std::chrono::microseconds;
    auto start = std::chrono::system_clock::now();

    test_arg<int, Alloc>(10, INT32_MAX);

    using point_2d = std::pair<int, int>;
    test_arg<point_2d, Alloc>(
            std::move(point_2d(10, 10)),
            std::move(point_2d(INT32_MAX, INT32_MAX))
            );

    auto end = std::chrono::system_clock::now();

    std::cout << "cost "
              << duration_cast<microseconds>(end - start).count()
              << " microseconds"
              << std::endl;
}

#endif //YAA_TEST_H
