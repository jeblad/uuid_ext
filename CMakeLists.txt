cmake_minimum_required(VERSION 3.19)

# Read version from package.json
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/package.json" PACKAGE_JSON_CONTENT)
string(JSON PROJECT_VERSION_FROM_PACKAGE_JSON GET "${PACKAGE_JSON_CONTENT}" "version")

project(uuid_ext VERSION ${PROJECT_VERSION_FROM_PACKAGE_JSON})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable CTest testing infrastructure
include(CTest)

# Set up versioning metadata
set(UUID_EXT_FULL_VERSION ${PROJECT_VERSION})
string(TIMESTAMP CURRENT_YEAR "%Y")

# Automated Versioning Pipeline: Install Git pre-commit hook
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    set(HOOK_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/scripts/pre-commit.sh")
    set(HOOK_DEST "${CMAKE_CURRENT_SOURCE_DIR}/.git/hooks/pre-commit")

    if(EXISTS "${HOOK_SOURCE}")
        if(UNIX)
            execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${HOOK_SOURCE}" "${HOOK_DEST}")
            file(CHMOD "${HOOK_SOURCE}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        else()
            execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${HOOK_SOURCE}" "${HOOK_DEST}")
        endif()
        message(STATUS "Git pre-commit hook installed/updated.")
    endif()
endif()

# Generate the single-header file from template
configure_file(uuid_ext.hpp.in "${CMAKE_CURRENT_SOURCE_DIR}/include/uuid_ext/uuid_ext.hpp" @ONLY)

# Create a symbolic link in the build directory to the generated header
file(CREATE_LINK
    "${CMAKE_CURRENT_SOURCE_DIR}/include/uuid_ext/uuid_ext.hpp"
    "${CMAKE_CURRENT_BINARY_DIR}/uuid_ext.hpp"
    SYMBOLIC)

# Define as an interface (header-only) library
add_library(uuid_ext INTERFACE)
target_include_directories(uuid_ext INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}")
target_compile_features(uuid_ext INTERFACE cxx_std_20)

# Enkel test-executable
add_executable(uuid_test tests/test_uuid.cpp)
target_link_libraries(uuid_test uuid_ext)

# Register the test for CTest
add_test(NAME uuid_test COMMAND uuid_test)
