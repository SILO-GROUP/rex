cmake_minimum_required(VERSION 3.5)
project(ftests)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp src/loaders.cpp src/loaders.h src/jsoncpp.cpp)
add_executable(ftests ${SOURCE_FILES})