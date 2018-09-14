cmake_minimum_required(VERSION 3.0)

# set name of the project
project(Examples CXX)

find_package(Eigen3 REQUIRED)

function(add_cpp_as_exe CPP_FILE_NAME)
	file(GLOB src ${CMAKE_CURRENT_SOURCE_DIR}/${CPP_FILE_NAME}.cpp)
	add_executable(${CPP_FILE_NAME} ${src})
	set_target_properties(${CPP_FILE_NAME} PROPERTIES LINKER_LANGUAGE CXX)
	target_link_libraries(${CPP_FILE_NAME} AutoGenLib)
	target_include_directories(${CPP_FILE_NAME} PUBLIC ${EIGEN3_INCLUDE_DIR})

	add_custom_target(${CPP_FILE_NAME}-run
        COMMAND ${CPP_FILE_NAME}
        DEPENDS ${CPP_FILE_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction(add_cpp_as_exe)

# example1
# add_cpp_as_exe(example1)
# add_cpp_as_exe(example2)
# add_cpp_as_exe(example3)
# add_cpp_as_exe(example4)
# add_cpp_as_exe(example5)
add_cpp_as_exe(example_autodiff)
add_cpp_as_exe(example_codegen)
add_cpp_as_exe(example_codegen_easy)
