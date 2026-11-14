#include "sum.hpp"
#include <array>
#include <xsimd/xsimd.hpp>

float sum_simd(std::span<const float> nums) {
    using batch = xsimd::batch<float>;
    constexpr std::size_t width = batch::size;
    constexpr std::size_t chains = 8;
    constexpr std::size_t step = width * chains;
    std::size_t simd_end = nums.size() - nums.size() % step;

    std::array<batch, chains> acc; // <type of elements, # of elements>
    acc.fill(batch(0.f)); // float 0s fill the batch

    for (std::size_t i = 0; i < simd_end; i += step)
        for (std::size_t k = 0; k < chains; ++k)
            acc[k] += batch::load_unaligned(nums.data() + i + k * width);
    batch total(0.f);
    for (const batch& a : acc) total += a;
    float sum = xsimd::reduce_add(total);
    for (std::size_t i = simd_end; i < nums.size(); ++i) sum += nums[i];
    return sum;
}