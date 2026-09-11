#pragma once

#include <array>
#include <cassert>
#include <cstdint>

namespace rivoss {
    namespace cpu {

        enum Register {
            x0,
            x1,
            x2,
            x3,
            x4,
            x5,
            x6,
            x7,
            x8,
            x9,
            x10,
            x11,
            x12,
            x13,
            x14,
            x15,
            x16,
            x17,
            x18,
            x19,
            x20,
            x21,
            x22,
            x23,
            x24,
            x25,
            x26,
            x27,
            x28,
            x29,
            x30,
            x31
        };

        class Cpu {
          public:
            Cpu()  = default;
            ~Cpu() = default;

            void cycle();

            std::array<uint32_t, 32>    registerFile;
            std::uint32_t               pc;
            std::array<uint8_t, 65'536> memory;
        };

    }; // namespace cpu

} // namespace rivoss
