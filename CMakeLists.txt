cmake_minimum_required(VERSION 3.25.2)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(bin)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_options(-Wall)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
add_compile_options(-Weffc++)
add_compile_options(-Wextra)
add_compile_options(-Wshadow)
add_compile_options(-Wsign-conversion)
add_compile_options(-fcolor-diagnostics)
add_compile_options(-fansi-escape-codes)
add_compile_options(-pedantic-errors)
add_compile_options(-stdlib=libc++)

# add_compile_options(-Ofast)

add_library(myLib "")
add_subdirectory(lib)
add_subdirectory(src)