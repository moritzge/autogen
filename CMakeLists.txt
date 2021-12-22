cmake_minimum_required(VERSION 3.12)

# set name of the project
project(AutoGen CXX)

set (CMAKE_CXX_STANDARD 11)

option(AUTOGEN_EIGEN_NO_DEBUG FALSE "Disable Eigen asserts")
if (AUTOGEN_EIGEN_NO_DEBUG)
    add_definitions(-DEIGEN_NO_DEBUG)
endif()

# enable parallel build for Visual Studio
if (WIN32)
    add_compile_options(/MP)
endif()

# make release if unspecified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# TODO: if g++
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O1")

add_subdirectory(ext)
add_subdirectory(src)
add_subdirectory(codegen)

# Tests
option(AUTOGEN_BUILD_TESTS "Build AutoGen tests" ON)
message(STATUS "Building AutoGen Tests: ${AUTOGEN_BUILD_TESTS}")
if (AUTOGEN_BUILD_TESTS)
        enable_testing()
        add_subdirectory(tests)
        # add_test(Tests Tests)
endif()

# Examples
option(AUTOGEN_BUILD_EXAMPLES "Build AutoGen examples" OFF)
message(STATUS "Building AutoGen Examples: ${AUTOGEN_BUILD_EXAMPLES}")
if (AUTOGEN_BUILD_EXAMPLES)
        add_subdirectory(examples)
endif(AUTOGEN_BUILD_EXAMPLES)
