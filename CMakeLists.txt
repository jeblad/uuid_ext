cmake_minimum_required(VERSION 3.10)
project(uuid_ext VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Ensure C++20 is strictly required

configure_file(include/uuid_ext/version.h.in include/uuid_ext/version.h)

add_library(uuid_ext src/uuid.cpp)
set_target_properties(uuid_ext PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(uuid_ext PUBLIC 
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)
target_compile_features(uuid_ext PUBLIC cxx_std_20) # Explicitly set C++20 features

# Enkel test-executable
add_executable(uuid_test tests/test_uuid.cpp)
target_include_directories(uuid_test PRIVATE include) # Explicitly add include for test
target_link_libraries(uuid_test uuid_ext)
