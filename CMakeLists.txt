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

find_package(SoapySDR "0.6.0" NO_MODULE REQUIRED)
include_directories(${SOAPY_SDR_INCLUDE_DIR})
target_link_libraries(altiwx PUBLIC ${SOAPY_SDR_LIBRARY})  

find_package(Spdlog REQUIRED)
target_include_directories(altiwx PUBLIC ${SPDLOG_INCLUDE_DIR})
target_link_libraries(altiwx PUBLIC Spdlog::Spdlog)

find_package(fmt REQUIRED)
target_include_directories(altiwx PUBLIC ${fmt_INCLUDE_DIRS})
target_link_libraries(altiwx PUBLIC fmt::fmt)

pkg_check_modules(libpqxx REQUIRED libpqxx)
target_include_directories(altiwx PUBLIC ${PQXX_INCLUDE_DIRS})
target_link_libraries(altiwx PUBLIC "-lpqxx -lpq")

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

find_package(pybind11 REQUIRED)
target_link_libraries(altiwx PUBLIC pybind11::embed)
target_include_directories(altiwx PUBLIC ${PYBIND11_INCLUDE_DIR})

find_package(ZeroMQ REQUIRED)
target_link_libraries(altiwx PUBLIC ${ZeroMQ_LIBRARIES})
target_include_directories(altiwx PUBLIC ${ZeroMQ_INCLUDE_DIR})

find_package(Onion REQUIRED)
target_link_libraries(altiwx PUBLIC ${ONION_LIBRARIES})
target_include_directories(altiwx PUBLIC ${ONION_INCLUDE_DIR})

target_link_libraries(altiwx PUBLIC stdc++fs)
target_link_libraries(altiwx PUBLIC dl)

# DSP Settings
add_compile_definitions(DSP_BUFFER_SIZE=8192)

# Paths
add_compile_definitions(CONFIG_FILE_PATH="${CMAKE_INSTALL_PREFIX}/etc/altiwx/config.yml")
add_compile_definitions(PLUGIN_FOLDER_PATH="${CMAKE_INSTALL_PREFIX}/opt/altiwx/plugins")
add_compile_definitions(SCRIPTS_FOLDER_PATH="${CMAKE_INSTALL_PREFIX}/opt/altiwx/scripts")
add_compile_definitions(LOG_FOLDER_PATH="${CMAKE_INSTALL_PREFIX}/opt/altiwx/logs")
add_compile_definitions(WEB_DATA_PATH="${CMAKE_INSTALL_PREFIX}/opt/altiwx/web")

set(SOAPY_VIRTUAL_DEVICE 1)

# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/scripts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/web DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.yml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src-soapy)
add_subdirectory(plugins)

# Install AltiWx
install(TARGETS altiwx DESTINATION bin)
install(DIRECTORY web DESTINATION opt/altiwx)
install(DIRECTORY scripts DESTINATION opt/altiwx)
install(FILES config.yml DESTINATION etc/altiwx)

# Create uninstall target
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
@ONLY)

add_custom_target(uninstall
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )