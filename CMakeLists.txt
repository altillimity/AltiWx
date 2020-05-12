cmake_minimum_required (VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project (altix)
file(GLOB_RECURSE ALTIWX_CPPS src/*.cpp src/*.c)

add_executable(altix ${ALTIWX_CPPS})

target_include_directories(altix PUBLIC src)

find_package(Liquid REQUIRED)
target_include_directories(altix PUBLIC ${LIQUID_INCLUDE_DIR})
target_link_libraries(altix PUBLIC ${LIQUID_LIBRARIES})

find_package(RTLSDR REQUIRED)
target_include_directories(altix PUBLIC ${RTLSDR_INCLUDE_DIR})
target_link_libraries(altix PUBLIC ${RTLSDR_LIBRARIES})

find_package(Volk REQUIRED)
target_include_directories(altix PUBLIC ${VOLK_INCLUDE_DIR})
target_link_libraries(altix PUBLIC ${VOLK_LIBRARIES})

find_package(Spdlog REQUIRED)
target_include_directories(altix PUBLIC ${SPDLOG_INCLUDE_DIR})
target_link_libraries(altix PUBLIC Spdlog::Spdlog)

find_package(yaml-cpp REQUIRED)
target_include_directories(altix PUBLIC ${YAML_CPP_INCLUDE_DIR})
target_link_libraries(altix PUBLIC ${YAML_CPP_LIBRARIES})

add_subdirectory(external/libpredict)
target_include_directories(altix PUBLIC ${LIBPREDICT_INCLUDE_DIR})
target_link_libraries(altix PUBLIC predict)

find_package(Threads REQUIRED)
target_link_libraries(altix PUBLIC Threads::Threads)

add_subdirectory(external/curlpp)
target_link_libraries(altix PUBLIC curlpp)

find_package(Lua REQUIRED)
target_link_libraries(altix PUBLIC ${LUA_LIBRARIES})
target_include_directories(altix PUBLIC ${LUA_INCLUDE_DIR})

find_package(RTLSDR REQUIRED)
target_include_directories(altix PUBLIC ${RTLSDR_INCLUDE_DIR})
target_link_libraries(altix PUBLIC ${RTLSDR_LIBRARIES})

find_package(Liquid REQUIRED)
target_include_directories(altix PUBLIC ${LIQUID_INCLUDE_DIR})
target_link_libraries(altix PUBLIC ${LIQUID_LIBRARIES})

target_link_libraries(altix PUBLIC stdc++fs)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/scripts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
