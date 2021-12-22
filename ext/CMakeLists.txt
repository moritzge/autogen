include(FetchContent)

# set(FETCHCONTENT_QUIET      OFF)

macro(fetch_lib lib)
    FetchContent_GetProperties("${lib}")
    if(NOT ${${lib}_POPULATED})
        string(LENGTH ${lib} length)
        math(EXPR value "20-${length}" OUTPUT_FORMAT DECIMAL)
        string(REPEAT "." ${value} tab)
        execute_process(COMMAND ${CMAKE_COMMAND} -E echo_append "-- fetching ${lib} ${tab} ")
        FetchContent_Populate(${lib})
        execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
    endif()
endmacro(fetch_lib)

# googletest
FetchContent_Declare(
    googletest
    URL                 https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz
    URL_HASH            MD5=e8a8df240b6938bb6384155d4c37d937
)
fetch_lib(googletest)
add_subdirectory(${googletest_SOURCE_DIR} googletest)

# Eigen
FetchContent_Declare(
    eigen
    URL                 https://gitlab.com/libeigen/eigen/-/archive/3.3.9/eigen-3.3.9.zip
)
fetch_lib(eigen)
add_library(eigen INTERFACE)
target_include_directories(eigen INTERFACE ${eigen_SOURCE_DIR})

