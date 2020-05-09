cmake_minimum_required (VERSION 3.0.0)

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

find_package(Spdlog REQUIRED)
target_include_directories(autowx PUBLIC ${SPDLOG_INCLUDE_DIR})
target_link_libraries(autowx PUBLIC Spdlog::Spdlog)

target_link_libraries(autowx PUBLIC m)