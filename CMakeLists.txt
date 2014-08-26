cmake_minimum_required(VERSION 2.8)

project(blinkControl)

set(CMAKE_CXX_FLAGS "-std=c++11 -pthread -Wall ${CMAKE_CXX_FLAGS}")

set(SOURCE_DIR "src")

set(CMAKE_MODULE_PATH "/usr/local/share/Logger/;/usr/local/share/MiscUtils/;/usr/local/share/SFML/cmake/Modules;${CMAKE_MODULE_PATH}")

set(EXECUTABLE_NAME "blinkControl")

add_executable(${EXECUTABLE_NAME}
               ${SOURCE_DIR}/main.cpp
               ${SOURCE_DIR}/Pattern.cpp
               ${SOURCE_DIR}/FileReader.cpp)

find_package(Logger REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} Logger)

find_package(MiscUtils REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} MiscUtils)
