#pragma once

#include <iosfwd>
#include <map>
#include <ranges>
#include <string>

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

          private:
            ELFIO::elfio               reader;
            std::map<std::string, int> allSections;

            void validateReader();
        };

    } // namespace elf

} // namespace rivoss
