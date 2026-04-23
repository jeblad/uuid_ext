cmake_minimum_required(VERSION 3.10)
project(uuid_ext VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Ensure C++20 is strictly required

add_library(uuid_ext src/uuid.cpp)
target_include_directories(uuid_ext PUBLIC include)
target_compile_features(uuid_ext PUBLIC cxx_std_20) # Explicitly set C++20 features

# Enkel test-executable
add_executable(uuid_test tests/test_uuid.cpp)
target_include_directories(uuid_test PRIVATE include) # Explicitly add include for test
target_link_libraries(uuid_test uuid_ext)
