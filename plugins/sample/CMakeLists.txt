cmake_minimum_required (VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)

project (SamplePlugin)
file(GLOB_RECURSE SamplePlugin_CPPS src/*.cpp src/*.c)

add_library(SamplePlugin SHARED ${SamplePlugin_CPPS})

target_include_directories(SamplePlugin PUBLIC src)