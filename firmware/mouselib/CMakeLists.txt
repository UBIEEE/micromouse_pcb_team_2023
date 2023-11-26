# Source directories.
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Source files.
file(GLOB_RECURSE SRC "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")

# Build library.
add_library(mouselib ${SRC})

# Include directories.
target_include_directories(mouselib PUBLIC ${INC_DIR})

# Link libraries.
target_link_libraries(mouselib PUBLIC
                                 pico_stdlib
                                 hardware_pwm
                                 hardware_spi)
