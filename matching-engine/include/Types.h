#pragma once

#include <chrono>
#include <cstdint>

namespace engine {
    using Price = int64_t;
    using Quantity = uint32_t;
    using OrderId = uint64_t;
}

enum class Side : uint8_t {
    Buy,
    Sell
};

enum class OrderType : uint8_t {
    Limit,
    Market
};

using Timestamp = std::chrono::steady_clock::time_point;

inline Timestamp now() {
    return std::chrono::steady_clock::now();
}