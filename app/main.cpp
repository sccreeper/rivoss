#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

#include <cxxopts.hpp>

#include "core/cpu.hpp"
#include "core/elf.hpp"
#include "core/instructions.hpp"

namespace r = rivoss;

int main(int argc, char const *argv[]) {

    cxxopts::Options options("rivoss", "A RISC-V based operating system simulator");
    options.add_options()("s,show", "Show the metadata & instructions etc. in a RISC-V ELF")(
        "r,run", "Run a RISC-V ELF file")("f,file", "File path", cxxopts::value<std::string>());

    auto result = options.parse(argc, argv);

    if (result.count("show") && result.count("run")) {
        throw std::invalid_argument("show and run are mutually exclusive");
    }

    if (result.count("file") == 0) {
        throw std::invalid_argument("no file");
    }

    r::elf::ElfFile file = r::elf::ElfFile{std::string{result["file"].as<std::string>()}};

    if (result.count("show")) {
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

    } else {
        std::array<uint8_t, 1'048'576> memory;
        uint32_t                       start = file.loadIntoMemory(memory, 0);

        r::cpu::Cpu cpu = r::cpu::Cpu{0, memory};
        cpu.pc          = start;

        uint32_t counter = 0;

        while (true) {
            std::cout << "\033[2J\033[H" << std::flush;
            std::cout << counter << std::endl;
            std::cout << "Program counter: " << cpu.pc << std::endl;
            std::cout << "Registers: ";

            for (size_t i = 0; i < 32; i++) {
                std::cout << r::instr::getRegisterName(static_cast<r::instr::Register>(i)) << "="
                          << cpu.registerFile[i] << std::endl;
            }

            std::cout << std::endl;

            std::cout << "Current instruction: "
                      << r::instr::instructionToString(
                             r::instr::decodeInstruction(cpu.getCurrentInstruction()))
                      << std::endl;

            std::cin.get();
            counter++;

            cpu.cycle();
        }
    }

    return 0;
}
