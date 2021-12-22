cmake_minimum_required(VERSION 3.0)

# set name of the project
project(AutoGenLib CXX)

# expose to source code which compiler is used
add_definitions(-DCXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID})

# add source files
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} STATIC ${sources})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(${PROJECT_NAME} PUBLIC AUTOGEN_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(
    ${PROJECT_NAME}
    ${CMAKE_DL_LIBS}
    eigen
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
