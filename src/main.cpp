#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
#include <span>
#include <cmath>
#include "sum.hpp"

bool sums_match(std::span<const float> v) {
    double reference = 0;
    for (float x : v) reference += x;
    
    double tolerance = 1e-4 * std::max(1.0, std::abs(reference));
    bool regular_ok = std::abs(sum_regular(v) - reference) <= tolerance;
    bool simd_ok = std::abs(sum_simd(v) - reference) <= tolerance;
    return regular_ok && simd_ok;
}

int main()
{
    constexpr int N = 1'000'000;

    std::vector<float> nums(N);
    for (std::size_t i = 0; i < nums.size(); ++i)
    {
        nums[i] = static_cast<float>(i % 7) * 0.25f;
    }

    for (std::size_t len = 0; len <= 100; ++len) {
        if (!sums_match(std::span<const float>(nums).first(len))) {
            std::cout << "MISMATCH at size " << len << "\n";
            return 1;
        }
    }
    if (!sums_match(nums)) {
        std::cout << "MISMATCH at full size\n";
        return 1;
    }
    std::cout << "Correctness check passed\n";

    using Clock = std::chrono::steady_clock;

    auto benchmark = [&](const char *name, auto fn)
    {
        double best = 1e18;
        double total = 0;
        float result = 0;

        for (int run = 0; run < 500; ++run)
        {
            auto start = Clock::now();
            result = fn(nums);
            auto end = Clock::now();

            double us = std::chrono::duration<double, std::micro>(end - start).count();
            total += us;
            best = std::min(best, us);
        }

        std::cout << name << " best : " << best
                  << " us" << "\naverage : " << total/500.0 << "\nsum=" << result << '\n';
    };

    benchmark("Regular", sum_regular);
    benchmark("SIMD", sum_simd);
}