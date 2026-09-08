#include <iostream>
#include <map>
#include <ranges>
#include <stdexcept>
#include <string>

#include "core/elf.hpp"

int main(int argc, char const *argv[]) {

    if (argc <= 1) {
        throw std::invalid_argument("Not enough arguments!");
    }

    rivoss::elf::ElfFile file = rivoss::elf::ElfFile{std::string{argv[1]}};
    for (const auto &i : file.sectionNames()) {
        std::cout << i << std::endl;
    }

    return 0;
}
