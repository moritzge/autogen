cmake_minimum_required(VERSION 3.0)

# set name of the project
project(Tests CXX)

include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB test_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

add_executable(${PROJECT_NAME} ${test_sources})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${PROJECT_NAME}
    AutoGenLib
    gtest
    -lpthread
) # need pthread for google test. TODO: does this link on Windows?

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        ${EIGEN3_INCLUDE_DIR}
)

enable_testing()
add_test(${PROJECT_NAME} ${PROJECT_NAME})
