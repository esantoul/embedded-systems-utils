Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(embedded_system_utils_tests)

target_sources(embedded_system_utils_tests PRIVATE
  # Add sources here
)

set_target_properties(embedded_system_utils_tests PROPERTIES
  ${ESUTILS_COMMON_PROPERTIES}
)

target_compile_options(embedded_system_utils_tests PRIVATE
  ${ESUTILS_COMMON_COMPILE_OPTIONS}
)

target_link_libraries(embedded_system_utils_tests PRIVATE
  Catch2::Catch2WithMain
  embedded_system_utils
)

message("Configured target embedded_system_utils_tests")

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)
catch_discover_tests(embedded_system_utils_tests)

add_custom_target(embedded_system_utils_tests_run ALL ctest --output-on-failure)
add_dependencies(embedded_system_utils_tests_run embedded_system_utils_tests)
