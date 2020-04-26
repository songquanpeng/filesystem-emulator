cmake_minimum_required(VERSION 3.15)
project(file_system_emulator)

set(CMAKE_CXX_STANDARD 11)
include_directories (include)
aux_source_directory (src SRC)
add_library (main "${SRC}")
add_executable(emulator main.cpp)
target_link_libraries (emulator main)