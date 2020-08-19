cmake_minimum_required (VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)

project (APRSPlugin)
file(GLOB_RECURSE APRSPlugin_CPPS src/*.cpp src/*.c)

add_library(APRSPlugin SHARED ${APRSPlugin_CPPS})

target_include_directories(APRSPlugin PUBLIC src)
