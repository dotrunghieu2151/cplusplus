cmake_minimum_required(VERSION 3.25.2)

# set(CMAKE_C_COMPILER "/usr/bin/clang")
# set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(bin)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_BUILD_TYPE, Debug)

# install external package from git START
include(FetchContent) 

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

FetchContent_Declare(
  tracyProfiler
  GIT_REPOSITORY https://github.com/wolfpld/tracy
  GIT_TAG "v0.10"
  GIT_SHALLOW TRUE
  GIT_PROGRESS ON
  SYSTEM
)

FetchContent_MakeAvailable(tracyProfiler)

# install external package from git END

enable_testing()

add_compile_options(-Wall)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
add_compile_options(-Weffc++)
add_compile_options(-Wextra)
add_compile_options(-Wshadow)
add_compile_options(-Wsign-conversion)
add_compile_options(-fdiagnostics-color)
# add_compile_options(-fcolor-diagnostics)
# add_compile_options(-fansi-escape-codes)
# add_compile_options(-pedantic-errors)
# add_compile_options(-stdlib=libc++)

# add_compile_options(-Ofast)

add_library(myLib "")
add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(tests)