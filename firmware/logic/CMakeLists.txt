set(LIB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(TESTS_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/tests")

file(GLOB_RECURSE LIB_SRC "${LIB_SRC_DIR}/*.c" "${LIB_SRC_DIR}/*.cpp")

if(NOT MSVC)
  set_source_files_properties(${LIB_SRC} PROPERTIES COMPILE_FLAGS "-Wall -Werror")
endif()

add_library(logic ${LIB_SRC})
target_include_directories(logic PUBLIC ${LIB_INC_DIR})

if(DESKTOP_TESTING)
  enable_testing()

  file(GLOB_RECURSE TESTS_SRC "${TESTS_DIR}/*.c" "${TESTS_DIR}/*.cpp")

  add_executable(logic_test ${TESTS_SRC})
  target_link_libraries(logic_test logic)

  target_link_libraries(logic_test gtest_main)

  include(GoogleTest)
  gtest_discover_tests(logic_test)
endif()

