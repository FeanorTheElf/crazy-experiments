#pragma once
#include <stdint.h>

struct uint128_t {
    uint64_t upper;
    uint64_t lower;
};

inline uint128_t operator+(uint128_t lhs, uint128_t rhs) {
    uint128_t result;
    result.lower = lhs.lower + rhs.lower;
    result.upper = lhs.upper + rhs.upper + static_cast<uint64_t>(result.lower < lhs.lower);
    return result;
}

inline uint128_t complete_mul(uint64_t lhs, uint64_t rhs) {
    const uint64_t lhs_low_low = lhs & 0xFFFFFFFF;
    const uint64_t rhs_low_low = rhs & 0xFFFFFFFF;
    const uint64_t lhs_low_up = lhs >> 32;
    const uint64_t rhs_low_up = rhs >> 32;

    const uint64_t result_lower = lhs_low_low * rhs_low_low;
    const uint64_t result_middle_1 = lhs_low_up * rhs_low_low;
    const uint64_t result_middle_2 = lhs_low_low * rhs_low_up;
    const uint64_t result_middle = result_middle_1 + result_middle_2;
    const uint64_t result_upper = lhs_low_up * rhs_low_up;

    const uint64_t result_middle_as_lower = result_middle << 32;
    const uint64_t result_middle_as_upper = result_middle >> 32;
    uint128_t result;
    result.lower = result_lower + result_middle_as_lower;
    result.upper = result_upper + result_middle_as_upper + static_cast<uint64_t>(result.lower < result_lower) + static_cast<uint64_t>(result_middle < result_middle_1) << 32;
    return result;
}

inline uint128_t operator*(uint128_t lhs, uint128_t rhs) {
    uint128_t result = complete_mul(lhs.lower, rhs.lower);
    result.upper += rhs.upper * lhs.lower;
    result.upper += lhs.upper * rhs.lower;
    return result;
}

inline uint128_t operator*(uint128_t lhs, uint64_t rhs) {
    uint128_t result = complete_mul(lhs.lower, rhs);
    result.upper += lhs.upper * rhs;
    return result;
}

inline uint128_t operator*(uint64_t lhs, uint128_t rhs) {
    uint128_t result = complete_mul(lhs, rhs.lower);
    result.upper += rhs.upper * lhs;
    return result;
}