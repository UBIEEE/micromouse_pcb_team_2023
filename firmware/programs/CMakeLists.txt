#
# Builds a program.
#
function(build_program name)
  set(PROGRAM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${name}")

  # Make sure directory exist.
  if(NOT EXISTS ${PROGRAM_DIR})
    message(FATAL_ERROR "Program '${name}' does not exist")
  endif()

  # Source directories.
  set(SRC_DIR "${PROGRAM_DIR}/src")
  set(INC_DIR "${PROGRAM_DIR}/include")

  # Source files.
  file(GLOB_RECURSE SRC "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")

  # Make sure the program has source files.
  # Better to fail here than for the linker to fail dramatically later on.
  if(NOT SRC)
    message(FATAL_ERROR "Program '${name}' has no source files")
  endif()

  # Set compile flags for source files.
  # I can't set it normally because the Pico SDK can't handle it.
  set_source_files_properties(${SRC} PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Werror -pedantic")

  # Build executable.
  add_executable(${name} ${SRC})

  # Include directories.
  target_include_directories(${name} PRIVATE ${INC_DIR})

  # Link to the main library.
  target_link_libraries(${name} mouselib)

  # Enable USB output and disable UART output.
  pico_enable_stdio_usb(${name} 1)
  pico_enable_stdio_uart(${name} 0)

  # Generate UF2 file.
  pico_add_extra_outputs(${name})
endfunction()

#
# Optionally builds a program.
#
function(add_program name default)
  string(TOUPPER ${name} NAME_UPPER)

  # Create option.
  option(PROGRAM_${NAME_UPPER} "Build program '${name}'" ${default})

  # Check if option is set.
  if(PROGRAM_${NAME_UPPER})
    message(STATUS "Building program '${name}'")
    build_program(${name})
  endif()
endfunction()

#
# Programs
#
add_program(main ON)
add_program(hello OFF)
add_program(imu_test OFF)
add_program(encoder_test OFF)
