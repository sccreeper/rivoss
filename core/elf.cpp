#include "elf.hpp"

#include <stdexcept>

#include <elfio/elfio.hpp>
#include <elfio/elfio_dump.hpp>

namespace rivoss {

    namespace elf {

        ElfFile::ElfFile(std::string fileName) {
            if (!reader.load(fileName)) {
                throw std::runtime_error("Unable to open file");
            } else {
                loadSuccessful = true;
            }
        };

        ElfFile::ElfFile(std::ifstream &file) {
            if (!reader.load(file)) {
                throw std::runtime_error("Unable to open file");
            } else {
                loadSuccessful = true;
            }
        };

        void ElfFile::getSection(const std::string &name) {

            auto numSections = reader.sections.size();
            for (size_t i = 0; i < numSections; i++) {
                auto sec = reader.sections[i];
                std::cout << sec->get_name();
            }
        };

    } // namespace elf

} // namespace rivoss
