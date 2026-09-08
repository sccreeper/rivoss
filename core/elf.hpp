#pragma once

#include <string>
#include <vector>

#include <elfio/elfio.hpp>

namespace rivoss {
    namespace elf {
        // Wrappers around elfio library
        class ElfFile {
          public:
            ElfFile(std::string fileName);
            ElfFile(std::ifstream &file);
            ~ElfFile() = default;

            void getSection(const std::string &name);

          private:
            ELFIO::elfio reader;
            bool         loadSuccessful = false;
        };

    } // namespace elf

} // namespace rivoss
