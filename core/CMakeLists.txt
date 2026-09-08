add_library(core 
    elf.cpp
    elf.hpp
)
target_link_libraries(core PUBLIC elfio::elfio)
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR})