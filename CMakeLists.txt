cmake_minimum_required(VERSION 3.5)
project(examplar)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++1z -O0 -DDEBUG=1")
set(SOURCE_FILES examplar.cpp src/loaders/abstract/loaders.cpp src/loaders/abstract/loaders.h src/json/jsoncpp.cpp src/loaders/low_level/JSON_Loader.cpp src/loaders/low_level/JSON_Loader.h src/loaders/misc/helpers.cpp src/loaders/misc/helpers.h src/loaders/abstract/Suite.cpp src/loaders/abstract/Suite.h src/loaders/abstract/Plan.cpp src/loaders/abstract/Plan.h src/loaders/abstract/Conf.cpp src/loaders/abstract/Conf.h src/loaders/abstract/Unit.cpp src/loaders/abstract/Unit.h src/loaders/abstract/Task.cpp src/loaders/abstract/Task.h src/Sproc/Sproc.cpp src/Sproc/Sproc.h src/Logger/Logger.cpp src/Logger/Logger.h)

add_executable(examplar ${SOURCE_FILES})