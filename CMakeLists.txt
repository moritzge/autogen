cmake_minimum_required(VERSION 3.5)

# set name of the project
project(AutoGen CXX)

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


# Tests
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



# set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_CMakeTargets")
