cmake_minimum_required(VERSION 3.20)

project(co_de)

# creates a variable PROGRAM_NAME with the value co_de
set(PROGRAM_NAME co_de)

add_executable(${PROGRAM_NAME}
        include/lzw.hpp
        include/huffman.hpp
        src/lzw.cpp
        src/huffman.cpp
)

target_sources(${PROGRAM_NAME}
    PRIVATE
    main.cpp
    src/huffman.cpp
    src/lzw.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")


target_include_directories(${PROGRAM_NAME} PRIVATE include/)

target_compile_features(${PROGRAM_NAME} PRIVATE cxx_std_20)
set_target_properties(${PROGRAM_NAME} PROPERTIES CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# Sanitizers
target_compile_options(${PROGRAM_NAME} PRIVATE
    -fsanitize=address,undefined
)
target_link_options(${PROGRAM_NAME} PRIVATE
    -fsanitize=address,undefined
)

# Copy input files to build directory
# Text files
configure_file(Harry_Potter.txt Harry_Potter.txt COPYONLY)
# Folder
file(COPY test DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Warnings and optimizations
target_compile_options(${PROGRAM_NAME} PRIVATE
    # Set warnings for all build types
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror -Wshadow -Wnon-virtual-dtor>
    # O3 optimization for Release
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Release>>:-O3>
    # No optimization in Debug
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Debug>>:-O0 -g -fno-omit-frame-pointer>
    # Sanitizers for DebugWithSanitizers
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:DebugWithSanitizers>>:-O0 -g -fno-omit-frame-pointer -fsanitize=address,undefined>
)