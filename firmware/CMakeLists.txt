cmake_minimum_required(VERSION 3.5)

option(DESKTOP_TESTING "Build for desktop testing" OFF)

if(DESKTOP_TESTING)
  message(STATUS "~~~~~~~ Building for desktop testing ~~~~~~~")

  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        f8d7d77
  )

  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(googletest)
else()
  message(STATUS "~~~~~~~ Building for mouse ~~~~~~~")
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  include(pico_sdk_import)
endif()

project(firmware)

enable_language(C)
enable_language(CXX)
enable_language(ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()

if(NOT CMAKE_BUILD_TYPE)
  if(DESKTOP_TESTING)
    set(CMAKE_BUILD_TYPE "Debug")
  else()
    set(CMAKE_BUILD_TYPE "Release")
  endif()
endif()

if(DESKTOP_TESTING AND CMAKE_BUILD_TYPE STREQUAL "Release")
  message(WARNING "Building desktop testing in release mode is not allowed. Switching to debug mode.")
  set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

if(NOT DESKTOP_TESTING)
  pico_sdk_init()

  if(NOT CMAKE_C_COMPILER_ID STREQUAL "GNU") # Shouldn't be a problem, but just in case...
    message(FATAL_ERROR "Only GCC is supported.")
  endif()
endif()

message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/logic")

if(NOT DESKTOP_TESTING)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/control")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/programs")
endif()
