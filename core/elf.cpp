#include "elf.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <memory>
#include <stdexcept>

#include <elfio/elfio.hpp>
#include <elfio/elfio_section.hpp>
#include <elfio/elfio_segment.hpp>
#include <stddef.h>

namespace rivoss {

    namespace elf {

        ElfFile::ElfFile(std::string fileName) {

            std::ifstream file(fileName, std::ios::binary);
            instantiate(file);
        };

        ElfFile::ElfFile(std::ifstream &file) { instantiate(file); };

        // Validation and other loading
        void ElfFile::instantiate(std::ifstream &file) {
            file.seekg(0, std::ios::end);
            this->size = file.tellg();
            file.seekg(0, std::ios::beg);

            // Copy to a vector
            this->bytes.reserve(this->size);
            if (!file.read(reinterpret_cast<char *>(this->bytes.data()), size)) {
                throw std::runtime_error("Unable to open file");
            }

            if (!reader.load(file)) {
                throw std::runtime_error("Unable to open file");
            }

            auto machineId = reader.get_machine();

            if (machineId != machineIdRiscV) {
                throw std::runtime_error(std::format("Invalid machine code, {}", machineId));
            }

            if (reader.get_class() != ELFIO::ELFCLASS32 || reader.get_encoding() != ELFIO::ELFDATA2LSB ||
                reader.get_type() != ELFIO::ET_EXEC) {
                throw std::runtime_error("Invalid class, encoding, or not executable");
            }

            auto numSections = reader.sections.size();
            for (size_t i = 0; i < numSections; i++) {
                allSections[reader.sections[i]->get_name()] = i;
            }

            this->entry = reader.get_entry();
        }

        const ELFIO::section *ElfFile::getSection(const std::string &name) {
            return reader.sections[allSections[name]];
        };

        bool ElfFile::hasSection(const std::string &name) { return allSections.contains(name); }

        // TODO: memOffset isn't ignored
        // Loads an ELF file into a span, and returns an entry point
        uint32_t ElfFile::loadIntoMemory(std::span<uint8_t> mem, uint32_t memOffset) {

            std::uint32_t base = UINT32_MAX, end = 0;
            for (const auto &segment : reader.segments) {
                if (segment->get_type() != ELFIO::PT_LOAD) {
                    continue;
                }

                base = std::min<uint32_t>(base, segment->get_virtual_address());
                end  = std::max<uint32_t>(end, segment->get_virtual_address() + segment->get_memory_size());
            }
            if (base >= end) {
                throw std::runtime_error("no loadable segments");
            }

            // Clear region in memory
            for (size_t i = base; i < end; i++) {
                mem[i] = 0;
            }

            for (auto &segment : reader.segments) {
                if (segment->get_type() != ELFIO::PT_LOAD) {
                    continue;
                }

                auto virtualAddress = segment->get_virtual_address();
                auto fileSize       = segment->get_file_size();
                auto memorySize     = segment->get_memory_size();

                if (fileSize > memorySize) {
                    throw std::runtime_error(
                        std::format("Segment is malformed, file size of {} is greater than memory size of {}",
                                    fileSize, memorySize));
                }
                if (fileSize == 0) {
                    continue;
                }

                auto destination = virtualAddress - base;
                if (destination + memorySize > mem.size()) {
                    throw std::runtime_error("out of bounds");
                }

                std::copy_n(reinterpret_cast<const std::uint8_t *>(segment->get_data()), fileSize,
                            mem.begin() + destination);
            }

            uint32_t entryVirtualAddress = reader.get_entry();
            uint32_t entryIndex          = entryVirtualAddress - base;

            return entryIndex;
        }

    } // namespace elf

} // namespace rivoss
