#pragma once

#include <cstdint>
#include <iosfwd>
#include <map>
#include <ranges>
#include <span>
#include <string>
#include <vector>

#include <elfio/elfio.hpp>
namespace ELFIO {
    class section;
}

namespace rivoss {
    namespace elf {
        const int machineIdRiscV = 243;

        // Wrappers around elfio library
        class ElfFile {
          public:
            ElfFile(std::string fileName);
            ElfFile(std::ifstream &file);
            ~ElfFile() = default;

            const ELFIO::section *getSection(const std::string &name);
            bool                  hasSection(const std::string &name);
            auto                  sectionNames() const { return allSections | std::views::keys; }
            uint32_t              loadIntoMemory(std::span<uint8_t> mem, uint32_t memOffset);

            std::vector<uint8_t> bytes;

          private:
            ELFIO::elfio               reader;
            std::map<std::string, int> allSections;
            uint32_t                   size;
            uint32_t                   entry;

            void instantiate(std::ifstream &file);
        };

    } // namespace elf

} // namespace rivoss
