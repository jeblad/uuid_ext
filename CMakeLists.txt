cmake_minimum_required(VERSION 3.10)
project(uuid_ext VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Ensure C++20 is strictly required

# Get git hash for versioning
execute_process(
  COMMAND git rev-parse --short HEAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)
if(NOT GIT_HASH)
    set(GIT_HASH "unknown")
endif()

execute_process(
  COMMAND git diff --quiet
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  RESULT_VARIABLE IS_DIRTY
  ERROR_QUIET
)

set(UUID_EXT_VERSION_MAJOR 1)
set(UUID_EXT_VERSION_MINOR 0)
set(UUID_EXT_VERSION "${UUID_EXT_VERSION_MAJOR}.${UUID_EXT_VERSION_MINOR}.${GIT_HASH}")
if(IS_DIRTY)
    set(UUID_EXT_VERSION "${UUID_EXT_VERSION}-dirty")
endif()

configure_file(version.h.in "${CMAKE_CURRENT_BINARY_DIR}/include/uuid_ext/version.h")

add_library(uuid_ext src/uuid.cpp)
target_include_directories(uuid_ext PUBLIC include "${CMAKE_CURRENT_BINARY_DIR}/include")
target_compile_features(uuid_ext PUBLIC cxx_std_20) # Explicitly set C++20 features

# Enkel test-executable
add_executable(uuid_test tests/test_uuid.cpp)
target_include_directories(uuid_test PRIVATE include) # Explicitly add include for test
target_link_libraries(uuid_test uuid_ext)
