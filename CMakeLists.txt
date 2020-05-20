cmake_minimum_required (VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project (altiwx)
file(GLOB_RECURSE ALTIWX_CPPS src/*.cpp src/*.c)

add_executable(altiwx ${ALTIWX_CPPS})

target_include_directories(altiwx PUBLIC src)

find_package(PkgConfig REQUIRED) 

find_package(Liquid REQUIRED)
target_include_directories(altiwx PUBLIC ${LIQUID_INCLUDE_DIR})
target_link_libraries(altiwx PUBLIC ${LIQUID_LIBRARIES})

find_package(RTLSDR REQUIRED)
target_include_directories(altiwx PUBLIC ${RTLSDR_INCLUDE_DIR})
target_link_libraries(altiwx PUBLIC ${RTLSDR_LIBRARIES})

find_package(Spdlog REQUIRED)
target_include_directories(altiwx PUBLIC ${SPDLOG_INCLUDE_DIR})
target_link_libraries(altiwx PUBLIC Spdlog::Spdlog)

find_package(yaml-cpp REQUIRED)
target_include_directories(altiwx PUBLIC ${YAML_CPP_INCLUDE_DIR})
target_link_libraries(altiwx PUBLIC ${YAML_CPP_LIBRARIES})

pkg_check_modules(predict REQUIRED predict)
target_include_directories(altiwx PUBLIC ${PREDICT_INCLUDE_DIR})
target_link_libraries(altiwx PUBLIC predict)

find_package(Threads REQUIRED)
target_link_libraries(altiwx PUBLIC Threads::Threads)

find_package(CURL REQUIRED)
target_link_libraries(altiwx PUBLIC ${CURL_LIBRARIES})
target_include_directories(altiwx PUBLIC ${CURL_INCLUDE_DIR})

pkg_check_modules(curlpp REQUIRED curlpp)
target_include_directories(altiwx PUBLIC ${CURLPP_INCLUDE_DIR})
target_link_libraries(altiwx PUBLIC curlpp)

find_package(Lua REQUIRED)
target_link_libraries(altiwx PUBLIC ${LUA_LIBRARIES})
target_include_directories(altiwx PUBLIC ${LUA_INCLUDE_DIR})

find_package(ZeroMQ REQUIRED)
target_link_libraries(altiwx PUBLIC ${ZeroMQ_LIBRARIES})
target_include_directories(altiwx PUBLIC ${ZeroMQ_INCLUDE_DIR})

target_link_libraries(altiwx PUBLIC stdc++fs)

set(SOAPY_VIRTUAL_DEVICE 1)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/scripts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src-soapy)