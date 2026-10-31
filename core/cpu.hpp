#pragma once

#include <array>
#include <cstdint>
#include <span>

namespace rivoss {
    namespace cpu {

        enum Csr : uint32_t {

            // Machine status
            MSTATUS = 0x300,
            // Machine status higher
            MSTATUSH = 0x310,
            // Machine ISA
            MISA = 0x301,
            // Machine exception delegation
            MEDELEG = 0x302,
            // Machine exception delegation higher
            MEDELEGH = 0x312,
            // Machine interrupt delegation
            MIDELEG = 0x303,
            // Machine interrupt enable
            MIE = 0x304,
            // Maching trap-handler base
            MTVEC = 0x305,
            // Machine counter enable
            MCOUNTEREN = 0x306,

            // Machine vendor
            MVENDORID = 0xF11,
            // Machine architecture
            MARCHID = 0xF12,
            // Machine implementation
            MIMPID = 0xF13,
            // Machine hardware thread ID
            MHARTID = 0xF14,
            // Machine config pointer
            MCONFIGPTR = 0xF15

        };

        class Cpu {
          public:
            Cpu(uint32_t hartId, std::span<uint8_t> memory);
            ~Cpu() = default;

            void     cycle();
            uint32_t getCurrentInstruction();

            std::array<uint32_t, 32>    registerFile;
            std::uint32_t               pc;
            std::span<uint8_t>          memory;
            std::array<uint32_t, 4'096> csrs;
        };

    }; // namespace cpu

} // namespace rivoss
