set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB_RECURSE SRC "${SRC_DIR}/*.c"
                      "${SRC_DIR}/*.cpp")

set_source_files_properties(${SRC} PROPERTIES
                                   COMPILE_FLAGS "-Wall -Wextra -Werror -pedantic")

add_library(mouselib ${SRC})

target_include_directories(mouselib PUBLIC ${INC_DIR})

target_link_libraries(mouselib PUBLIC pico_stdlib
                                      pico_multicore
                                      hardware_pwm
                                      hardware_spi
                                      hardware_irq)
