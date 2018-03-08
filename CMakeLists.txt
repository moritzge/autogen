cmake_minimum_required(VERSION 3.0)

# set name of the project
project(AutoGen CXX)

# directory 'cmake' contains macros to find thirdparty libraries
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

set (CMAKE_CXX_STANDARD 11)

# let's use some colors!
if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

set(AUTOGEN_EIGEN_NO_DEBUG FALSE CACHE BOOL "Disable Eigen asserts")
if (AUTOGEN_EIGEN_NO_DEBUG)
    add_definitions(-DEIGEN_NO_DEBUG)
endif()

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

if(UNIX)
  set(CMAKE_CXX_FLAGS "-Wall -Wextra")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O1")
endif()

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
set(AUTOGEN_BUILD_TESTS TRUE CACHE BOOL "Build AutoGen tests")
message(STATUS "Building AutoGen Tests: ${BoldCyan} ${AUTOGEN_BUILD_TESTS} ${ColourReset}")
if (AUTOGEN_BUILD_TESTS)
  
    enable_testing()
    if(NOT DEFINED GTEST_ROOT)
      set(GTEST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../libs/gtest-1.7.0" CACHE PATH "Path to googletest")
    endif()
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})

    find_package(Eigen3 3 REQUIRED)

    file(GLOB test_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h
    )
    add_executable(${PROJECT_NAME}_test ${test_sources})
    set_target_properties(${PROJECT_NAME}_test PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(${PROJECT_NAME}_test
        ${PROJECT_NAME}
        ${GTEST_BOTH_LIBRARIES}
        -lpthread
        ${CMAKE_DL_LIBS}
        ) # need pthread for google test. TODO: does this link on Windows?
    add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)
endif()
# Examples #######################################################################

set(AUTOGEN_BUILD_EXAMPLES FALSE CACHE BOOL "Build AutoGen examples")
message(STATUS "Building AutoGen Examples: ${BoldCyan} ${AUTOGEN_BUILD_EXAMPLES} ${ColourReset}")
if (AUTOGEN_BUILD_EXAMPLES)

    find_package(Eigen3 REQUIRED)

    # example1
    file(GLOB src_ex1 ${CMAKE_CURRENT_SOURCE_DIR}/examples/example1.cpp)
    add_executable(Example1 ${src_ex1})
    set_target_properties(Example1 PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(Example1
        ${PROJECT_NAME}
    )

    # example2
    file(GLOB src_ex2 ${CMAKE_CURRENT_SOURCE_DIR}/examples/example2.cpp)
    add_executable(Example2 ${src_ex2})
    set_target_properties(Example2 PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(Example2
        ${PROJECT_NAME}
    )

    # example3
    file(GLOB src_ex3 ${CMAKE_CURRENT_SOURCE_DIR}/examples/example3.cpp)
    add_executable(Example3 ${src_ex3})
    set_target_properties(Example3 PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(Example3
        ${PROJECT_NAME}
    )
endif()
