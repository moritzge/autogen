cmake_minimum_required(VERSION 3.0)

# set name of the project
project(codegen CXX)

set(AUTOGEN_GENERATED_CODE_FOLDER ${CMAKE_CURRENT_LIST_DIR}/generated-code CACHE PATH "path to folder where generated code should be saved to")

function(add_codegen name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} AutoGenLib)
    target_compile_definitions(${name} PUBLIC AUTOGEN_GENERATED_CODE_FOLDER="${AUTOGEN_GENERATED_CODE_FOLDER}")
endfunction(add_codegen)

file(GLOB files "*.cpp")
foreach(file ${files})
    get_filename_component(name ${file} NAME_WE)
    add_codegen(${name})
    message(STATUS "found codgen ${name}")
endforeach()
