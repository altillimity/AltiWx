cmake_minimum_required (VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)

project (NoaaDSB)
file(GLOB_RECURSE NoaaDSB_CPPS src/*.cpp src/*.c)

add_library(NoaaDSB SHARED ${NoaaDSB_CPPS})

target_include_directories(NoaaDSB PUBLIC src)

# LibDSP
find_library(DSP_LIBRARY dsp REQUIRED)
target_link_libraries(NoaaDSB PUBLIC ${DSP_LIBRARY})
