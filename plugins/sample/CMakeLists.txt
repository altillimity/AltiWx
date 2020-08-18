cmake_minimum_required (VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)

project (samplePlugin)
file(GLOB_RECURSE samplePlugin_CPPS src/*.cpp src/*.c)

add_library(samplePlugin SHARED ${samplePlugin_CPPS})

target_include_directories(samplePlugin PUBLIC src)