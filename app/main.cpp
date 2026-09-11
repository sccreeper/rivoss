#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>

#include "core/elf.hpp"
#include "core/instructions.hpp"

namespace r = rivoss;

int main(int argc, char const *argv[]) {

    if (argc <= 1) {
        throw std::invalid_argument("Not enough arguments!");
    }

    r::elf::ElfFile file = r::elf::ElfFile{std::string{argv[1]}};
    for (const auto &i : file.sectionNames()) {
        std::cout << i << std::endl;
    }

    if (file.hasSection(".text")) {

        auto textSection = file.getSection(".text");

        std::span<char const> codeBuffer{textSection->get_data(), textSection->get_size()};

        for (size_t offset = 0; offset < textSection->get_size(); offset += 4) {

            auto chunk = codeBuffer.subspan(offset, 4);

            uint32_t instruction = static_cast<uint32_t>(static_cast<uint8_t>(chunk[0])) |
                                   static_cast<uint32_t>(static_cast<uint8_t>(chunk[1])) << 8 |
                                   static_cast<uint32_t>(static_cast<uint8_t>(chunk[2])) << 16 |
                                   static_cast<uint32_t>(static_cast<uint8_t>(chunk[3])) << 24;

            r::instr::DecodedInstruction decodedInstruction = r::instr::decodeInstruction(instruction);

            std::cout << r::instr::instructionToString(decodedInstruction) << std::endl;
        }

    } else {
        throw std::invalid_argument("No .text section found!");
    }

    return 0;
}
