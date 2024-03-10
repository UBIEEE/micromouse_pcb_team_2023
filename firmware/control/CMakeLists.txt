option(ODOMETRY_DEAD_RECKONING "Use dead reckoning for pose estimation (instead of using encoder distances)" OFF)
if(ODOMETRY_DEAD_RECKONING)
  add_definitions(-DODOMETRY_DEAD_RECKONING=1)
else()
  add_definitions(-DODOMETRY_ENCODERS=1)
endif()

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB_RECURSE SRC "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")

set_source_files_properties(${SRC} PROPERTIES COMPILE_FLAGS "-Wall -Werror")

add_library(control ${SRC})

target_include_directories(control PUBLIC ${INC_DIR})

target_link_libraries(control PUBLIC logic
                                     pico_stdlib
                                     pico_multicore
                                     hardware_pwm
                                     hardware_spi
                                     hardware_irq)

