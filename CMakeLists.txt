cmake_minimum_required(VERSION 3.10)
project(SUSILibTest)

enable_testing()

# Add the subdirectory for Google Test
add_subdirectory(test/googletest)

# Include the source files of the library
include_directories(src test)

# Find all source files in the src directory
file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")

# Create a test executable
add_executable(run_tests
  test/test_main.cpp
  test/mock_hal.cpp
  ${LIB_SOURCES}
  test/e2e_tests.cpp
  test/unit_tests.cpp
)

# Link the test executable with Google Test
target_link_libraries(run_tests gtest_main)

# Add a compile definition to enable testing-only code
target_compile_definitions(run_tests PRIVATE TESTING)

# Add command to run tests
add_test(NAME run_tests COMMAND run_tests)
