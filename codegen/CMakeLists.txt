cmake_minimum_required(VERSION 3.0)

# set name of the project
project(codegen CXX)

set(AUTOGEN_GENERATED_CODE_FOLDER ${CMAKE_CURRENT_LIST_DIR}/generated-code CACHE PATH "path to folder where generated code should be saved to")

# example angle axis
set(app-name "ExpCoords-ddR")
add_executable(${app-name} ${app-name}.cpp)
set_target_properties(${app-name} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${app-name} AutoGenLib)
target_include_directories(${app-name} PUBLIC ${EIGEN3_INCLUDE_DIR})
target_compile_definitions(${app-name} PUBLIC AUTOGEN_GENERATED_CODE_FOLDER="${AUTOGEN_GENERATED_CODE_FOLDER}")