cmake_minimum_required(VERSION 3.0)

# set name of the project
project(Tests CXX)

find_package(Eigen3 REQUIRED)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB test_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

add_executable(${PROJECT_NAME} ${test_sources})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${PROJECT_NAME}
    AutoGenLib
    ${GTEST_BOTH_LIBRARIES}
    -lpthread
) # need pthread for google test. TODO: does this link on Windows?

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        ${EIGEN3_INCLUDE_DIR}
)

enable_testing()
add_test(${PROJECT_NAME} ${PROJECT_NAME})

# add_custom_target(test-run COMMAND
add_custom_target(Tests-run-verbose
	COMMAND ctest -V
	DEPENDS ${PROJECT_NAME}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../src/Matrix.h" ${CMAKE_CURRENT_BINARY_DIR}
)
#   WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
#   DEPENDS ${PROJECT_NAME}
#   COMMENT "Copying Matrix.h"
#   VERBATIM
# )
