#pragma once

#include <cstdint>

namespace rivoss {
    namespace utils {

        // Extracts bits from a integer, then shifts them right to the LSB.
        inline constexpr uint32_t extractBits(uint32_t val, uint8_t lowBit, uint8_t width) {
            return (val >> lowBit) & ((1u << width) - 1);
        };

    } // namespace utils

} // namespace rivoss
