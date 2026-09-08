#include "elf.hpp"

#include <format>
#include <fstream>
#include <ranges>
#include <stdexcept>

#include <elfio/elfio.hpp>
#include <elfio/elfio_section.hpp>
#include <stddef.h>

namespace rivoss {

    namespace elf {

        ElfFile::ElfFile(std::string fileName) {
            if (!reader.load(fileName)) {
                throw std::runtime_error("Unable to open file");
            }
            validateReader();
        };

        ElfFile::ElfFile(std::ifstream &file) {
            if (!reader.load(file)) {
                throw std::runtime_error("Unable to open file");
            }
            validateReader();
        };

        // Do the rest of the loading
        void ElfFile::validateReader() {
            auto machineId = reader.get_machine();

            if (machineId != machineIdRiscV) {
                throw std::runtime_error(std::format("Invalid machine code, {}", machineId));
            }

            auto numSections = reader.sections.size();
            for (size_t i = 0; i < numSections; i++) {
                allSections[reader.sections[i]->get_name()] = i;
            }
        }

        const ELFIO::section *ElfFile::getSection(const std::string &name) {
            return reader.sections[allSections[name]];
        };

        bool ElfFile::hasSection(const std::string &name) { return allSections.contains(name); }

    } // namespace elf

} // namespace rivoss
