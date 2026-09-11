#pragma once

#include <array>
#include <cassert>
#include <cstdint>

namespace rivoss {
    namespace cpu {

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
