#include "sum.hpp"

float sum_regular(std::span<const float> nums) {
    float sum = 0;
    for (std::size_t i = 0; i < nums.size(); i++) {
        sum += nums[i];
    }
    return sum;
}