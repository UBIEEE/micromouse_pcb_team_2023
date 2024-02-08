set(LIB_SRC_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_INC_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(TEST_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/test")

file(GLOB_RECURSE LIB_SRC "${LIB_SRC_DIR}/*.c" "${LIB_SRC_DIR}/*.cpp")

if(NOT MSVC)
  set_source_files_properties(${LIB_SRC} PROPERTIES COMPILE_FLAGS "-Wall -Werror")
endif()

add_library(logic ${LIB_SRC})
target_include_directories(logic PUBLIC ${LIB_INC_DIR})

if(DESKTOP_TESTING)
  file(GLOB_RECURSE TEST_SRC "${TEST_DIR}/*.c" "${TEST_DIR}/*.cpp")

  add_executable(logic_test ${TEST_SRC})
  target_include_directories(logic_test PUBLIC ${TEST_DIR})

  target_link_libraries(logic_test logic)
endif()

