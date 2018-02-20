cmake_minimum_required(VERSION 3.5)

# set name of the project
project(AutoGen CXX)

set (CMAKE_CXX_STANDARD 11)

set(AUTOGEN_EIGEN_NO_DEBUG FALSE CACHE BOOL "Disable Eigen asserts")
if (AUTOGEN_EIGEN_NO_DEBUG)
    add_definitions(-DEIGEN_NO_DEBUG)
endif()

# dependencies project-wide
find_package(Eigen3 3.3.4 REQUIRED)

# find_package(OpenMP)
# if (OPENMP_FOUND)
#     set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#     set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#     set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# endif()


# enable parallel build for Visual Studio
if (WIN32)
    add_compile_options(/MP)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O1")

# AutoGen Library ################################################################
# add source files
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_library(${PROJECT_NAME} STATIC ${sources})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# include_directories("${EIGEN3_INCLUDE_DIR}")

target_include_directories(
	${PROJECT_NAME} 
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/src
		${EIGEN3_INCLUDE_DIR}
)

# Tests ##########################################################################
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB test_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h
    )
add_executable(${PROJECT_NAME}_test ${test_sources})
set_target_properties(${PROJECT_NAME}_test PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROJECT_NAME}_test
    ${PROJECT_NAME}
    ${GTEST_BOTH_LIBRARIES}
    -lpthread) # need pthread for google test. TODO: does this link on Windows?
add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)

# Examples #######################################################################

# example1
file(GLOB src_ex1 ${CMAKE_CURRENT_SOURCE_DIR}/examples/example1.cpp)
add_executable(Example1 ${src_ex1})
set_target_properties(Example1 PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Example1
    ${PROJECT_NAME}
)

# example1
file(GLOB src_ex2 ${CMAKE_CURRENT_SOURCE_DIR}/examples/example2.cpp)
add_executable(Example2 ${src_ex2})
set_target_properties(Example2 PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Example2
    ${PROJECT_NAME}
)
