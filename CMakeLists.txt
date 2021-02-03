cmake_minimum_required (VERSION 3.0.0)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT MINGW)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

project (AltiWx)
file(GLOB_RECURSE AltiWx_CPPS src/*.cpp src/*.c)

if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
endif()

add_executable(AltiWx ${AltiWx_CPPS})
target_include_directories(AltiWx PUBLIC src)
target_link_libraries(AltiWx PUBLIC stdc++fs dl)

# Plugins
add_subdirectory(plugins)

# Libraries
find_package(PkgConfig REQUIRED) 

# Spdlog
find_package(Spdlog REQUIRED)
target_include_directories(AltiWx PUBLIC ${SPDLOG_INCLUDE_DIR})
target_link_libraries(AltiWx PUBLIC Spdlog::Spdlog)

# RTL-SDR
find_package(RTLSDR REQUIRED)
target_include_directories(AltiWx PUBLIC ${RTLSDR_INCLUDE_DIR})
target_link_libraries(AltiWx PUBLIC ${RTLSDR_LIBRARIES})

# Liquid-DSP
find_package(LIQUID REQUIRED)
target_include_directories(AltiWx PUBLIC ${LIQUID_INCLUDE_DIR})
target_link_libraries(AltiWx PUBLIC ${LIQUID_LIBRARIES})

# Threads
find_package(Threads REQUIRED)
target_link_libraries(AltiWx PUBLIC Threads::Threads)

# Predict
pkg_check_modules(predict REQUIRED predict)
target_include_directories(AltiWx PUBLIC ${PREDICT_INCLUDE_DIR})
target_link_libraries(AltiWx PUBLIC predict)

# Yaml-CPP
find_package(yaml-cpp REQUIRED)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
target_include_directories(AltiWx PUBLIC ${YAML_CPP_INCLUDE_DIR})
target_link_libraries(AltiWx PUBLIC ${YAML_CPP_LIBRARIES})

# LibDSP
find_library(DSP_LIBRARY dsp REQUIRED)
target_link_libraries(AltiWx PUBLIC ${DSP_LIBRARY})

# PyBind11
find_package(pybind11 REQUIRED)
target_link_libraries(AltiWx PUBLIC pybind11::embed)
target_include_directories(AltiWx PUBLIC ${PYBIND11_INCLUDE_DIR})

# FMT
find_package(fmt REQUIRED)
target_include_directories(AltiWx PUBLIC ${fmt_INCLUDE_DIRS})
target_link_libraries(AltiWx PUBLIC fmt::fmt)

# Mingw requires some manual stuff... There definitely is a better way to do it
if(MINGW)
    target_link_libraries(AltiWx PUBLIC yaml-cpp ws2_32)
endif()