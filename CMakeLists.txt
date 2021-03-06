cmake_minimum_required(VERSION 2.8)

project(blinkControl)


if ( NOT ( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "Release" ) )
    message(FATAL_ERROR "Build type not specified! Use -DCMAKE_BUILD_TYPE=Debug or -DCMAKE_BUILD_TYPE=Release to fix this.")
endif()

MESSAGE("${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -Wno-overloaded-virtual -Wno-deprecated-register -fsanitize=address -fno-omit-frame-pointer")

set(CMAKE_CXX_FLAGS "-std=c++11 -pthread -DUSE_HIDAPI -O3 ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-DUSE_HIDAPI ${CMAKE_C_FLAGS}")

set(SOURCE_DIR "src")

set(CMAKE_MODULE_PATH "/usr/local/share/Logger/;/usr/local/share/MiscUtils/;/usr/local/share/SFML/cmake/Modules;${CMAKE_MODULE_PATH}")

set(EXECUTABLE_NAME "blinkControl")

add_executable(${EXECUTABLE_NAME}
               ${SOURCE_DIR}/main.cpp
               ${SOURCE_DIR}/Pattern.cpp
               ${SOURCE_DIR}/FileReader.cpp
               ${SOURCE_DIR}/GTK.cpp
               ${SOURCE_DIR}/TrayIcon.cpp
               ${SOURCE_DIR}/IFTTT.cpp
               ${SOURCE_DIR}/PatternMaker.cpp
               ${SOURCE_DIR}/Row.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(CURL libcurl)

link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE_NAME} ${GTKMM_LIBRARIES})

link_directories(${CURL_LIBRARY_DIRS})
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE_NAME} ${CURL_LIBRARIES})

find_package(Logger REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} Logger)

find_package(MiscUtils REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} MiscUtils)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_DIR}/blink1-lib/)
target_link_libraries(${EXECUTABLE_NAME} :blink1-lib.so)
