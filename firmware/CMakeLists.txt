cmake_minimum_required(VERSION 3.5)

set(CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include("${CMAKE_DIR}/pico_sdk_import.cmake")

project(firmware LANGUAGES C CXX ASM)

# Language standards.
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Disable in-source builds.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()

# Default to release.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  # Generate compile commands.
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# Pico SDK.
pico_sdk_init()

# Compile options.
add_compile_options(-Wall -Werror -Wno-format -Wno-unused-function)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wno-maybe-uninitialized)
endif()

# Build library
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/mouselib")

# Build programs.
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/programs")
