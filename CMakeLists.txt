cmake_minimum_required(VERSION 3.15)
project(file_system_emulator)

set(CMAKE_CXX_STANDARD 11)

FILE(GLOB Sources src/*.cpp)
add_executable(emulator
    main.cpp
    ${Sources}
)
target_include_directories(emulator PRIVATE include)