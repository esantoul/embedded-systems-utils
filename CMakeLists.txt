cmake_minimum_required(VERSION 3.25)

project(embedded_system_utils VERSION 1.0 LANGUAGES CXX)

set(COMMON_PROPERTIES
  C_STANDARD 11
  C_STANDARD_REQUIRED ON
  C_EXTENSIONS OFF
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

set(COMMON_COMPILE_OPTIONS
  $<$<COMPILE_LANGUAGE:CXX>:
  -Wall
  -Wextra
  -Werror
  -Wpedantic
  -Wshadow
  -Wnon-virtual-dtor
  -Wold-style-cast
  -Wcast-align # <-- this one may be tricky to solve, please ask someone who knows if you are stuck
  >
)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

add_library(${PROJECT_NAME} STATIC ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
  ${COMMON_PROPERTIES}
)
target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_options(${PROJECT_NAME} PRIVATE
  ${COMMON_COMPILE_OPTIONS}
)

message("Configured target ${PROJECT_NAME}")

option(ESUTILS_WITH_TESTS "Include tests" OFF)
if(ESUTILS_WITH_TESTS)
  add_subdirectory(tests)
endif()