#pragma once
#include <cstdint>
#include <span>

float sum_regular(std::span<const float> nums);
float sum_simd(std::span<const float> nums);