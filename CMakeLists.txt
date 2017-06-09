cmake_minimum_required(VERSION 3.5)
project(ftests)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES examplar.cpp src/loaders/loaders.cpp src/loaders/loaders.h src/json/jsoncpp.cpp src/loaders/JSON_Loader.cpp src/loaders/JSON_Loader.h src/loaders/helpers.cpp src/loaders/helpers.h src/loaders/Suite.cpp src/loaders/Suite.h src/loaders/Plan.cpp src/loaders/Plan.h src/loaders/Conf.cpp src/loaders/Conf.h src/loaders/Unit.cpp src/loaders/Unit.h src/loaders/Task.cpp src/loaders/Task.h)
add_executable(ftests ${SOURCE_FILES})