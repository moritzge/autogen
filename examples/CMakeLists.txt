cmake_minimum_required(VERSION 3.0)

# set name of the project
project(Examples CXX)

find_package(Eigen3 REQUIRED)

# example1
file(GLOB src_ex1 ${CMAKE_CURRENT_SOURCE_DIR}/example1.cpp)
add_executable(Example1 ${src_ex1})
set_target_properties(Example1 PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Example1 AutoGenLib)
target_include_directories(Example1 PUBLIC ${EIGEN3_INCLUDE_DIR})

# example2
file(GLOB src_ex2 ${CMAKE_CURRENT_SOURCE_DIR}/example2.cpp)
add_executable(Example2 ${src_ex2})
set_target_properties(Example2 PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Example2 AutoGenLib)
target_include_directories(Example2 PUBLIC ${EIGEN3_INCLUDE_DIR})

# example3
file(GLOB src_ex3 ${CMAKE_CURRENT_SOURCE_DIR}/example3.cpp)
add_executable(Example3 ${src_ex3})
set_target_properties(Example3 PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Example3 AutoGenLib)
target_include_directories(Example3 PUBLIC ${EIGEN3_INCLUDE_DIR})

# example4
file(GLOB src_ex4 ${CMAKE_CURRENT_SOURCE_DIR}/example4.cpp)
add_executable(Example4 ${src_ex4})
set_target_properties(Example4 PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Example4 AutoGenLib)
target_include_directories(Example4 PUBLIC ${EIGEN3_INCLUDE_DIR})

# example exp coords cg
set(app-name "example-ddR-cg")
add_executable(${app-name} example-ddR-cg.cpp ExpCoords.h)
set_target_properties(${app-name} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${app-name} AutoGenLib)
target_include_directories(${app-name} PUBLIC ${EIGEN3_INCLUDE_DIR})

# example exp coords test
set(app-name "example-ddR-test")
add_executable(${app-name} example-ddR-test.cpp ExpCoords.h ddR_i_j.h)
set_target_properties(${app-name} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${app-name} AutoGenLib)
target_include_directories(${app-name} PUBLIC ${EIGEN3_INCLUDE_DIR})

# example exp coords test
set(app-name "example-motor-djac")
add_executable(${app-name} example-motor-djac.cpp ExpCoords.h)
set_target_properties(${app-name} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${app-name} AutoGenLib)
target_include_directories(${app-name} PUBLIC ${EIGEN3_INCLUDE_DIR})
