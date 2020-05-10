cmake_minimum_required (VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project (autowx)
file(GLOB_RECURSE AUTOWX_CPPS src/*.cpp src/*.c)

add_executable(autowx ${AUTOWX_CPPS})

target_include_directories(autowx PUBLIC src)

find_package(Liquid REQUIRED)
target_include_directories(autowx PUBLIC ${LIQUID_INCLUDE_DIR})
target_link_libraries(autowx PUBLIC ${LIQUID_LIBRARIES})

find_package(RTLSDR REQUIRED)
target_include_directories(autowx PUBLIC ${RTLSDR_INCLUDE_DIR})
target_link_libraries(autowx PUBLIC ${RTLSDR_LIBRARIES})

find_package(Volk REQUIRED)
target_include_directories(autowx PUBLIC ${VOLK_INCLUDE_DIR})
target_link_libraries(autowx PUBLIC ${VOLK_LIBRARIES})

#find_package(CURL REQUIRED)
#target_include_directories(autowx PUBLIC ${CURL_INCLUDE_DIR})
#target_link_libraries(autowx PUBLIC ${CURL_LIBRARIES})

find_package(Spdlog REQUIRED)
target_include_directories(autowx PUBLIC ${SPDLOG_INCLUDE_DIR})
target_link_libraries(autowx PUBLIC Spdlog::Spdlog)

find_package(yaml-cpp REQUIRED)
target_include_directories(autowx PUBLIC ${YAML_CPP_INCLUDE_DIR})
target_link_libraries(autowx PUBLIC ${YAML_CPP_LIBRARIES})

add_subdirectory(external/libpredict)
target_include_directories(autowx PUBLIC ${LIBPREDICT_INCLUDE_DIR})
target_link_libraries(autowx PUBLIC predict)

add_subdirectory(external/curlpp)
#target_include_directories(autowx PUBLIC ${LIBPREDICT_INCLUDE_DIR})
target_link_libraries(autowx PUBLIC curlpp)