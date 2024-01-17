#
# Builds a program.
#
function(build_program name)
  set(PROGRAM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${name}")

  if(NOT EXISTS ${PROGRAM_DIR})
    message(FATAL_ERROR "Program '${name}' does not exist")
  endif()

  set(SRC_DIR "${PROGRAM_DIR}/src")
  set(INC_DIR "${PROGRAM_DIR}/include")

  file(GLOB_RECURSE SRC "${SRC_DIR}/*.c"
                        "${SRC_DIR}/*.cpp")

  if(NOT SRC)
    message(FATAL_ERROR "Program '${name}' has no source files")
  endif()

  set_source_files_properties(${SRC} PROPERTIES
                                     COMPILE_FLAGS "-Wall -Wextra -Werror -pedantic")

  add_executable(${name} ${SRC})

  target_include_directories(${name} PRIVATE ${INC_DIR})

  target_link_libraries(${name} mouselib)

  pico_enable_stdio_usb(${name} 1)
  pico_enable_stdio_uart(${name} 0)

  pico_add_extra_outputs(${name}) # Generate UF2 file.
endfunction()

#
# Optionally builds a program.
#
function(add_program name default)
  string(TOUPPER ${name} NAME_UPPER)

  option(PROGRAM_${NAME_UPPER} "Build program '${name}'" ${default})

  if(PROGRAM_${NAME_UPPER})
    message(STATUS "Building program '${name}'")
    build_program(${name})
  endif()
endfunction()

#
# Programs.
#
add_program(main ON)
add_program(hello OFF)
add_program(test_imu OFF)
add_program(test_encoders OFF)
