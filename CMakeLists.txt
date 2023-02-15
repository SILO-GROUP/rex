cmake_minimum_required(VERSION 3.23)
project(rex)

set(CMAKE_CXX_STANDARD 14)

add_executable(rex Rex.cpp src/json_support/jsoncpp/json.h src/json_support/jsoncpp/json-forwards.h src/json_support/jsoncpp/jsoncpp.cpp src/logger/Logger.cpp src/logger/Logger.h src/json_support/JSON.cpp src/json_support/JSON.h src/misc/helpers.cpp src/misc/helpers.h src/config/Config.cpp src/config/Config.h src/suite/Suite.cpp src/suite/Suite.h src/suite/Unit.cpp src/suite/Unit.h src/shells/shells.cpp src/shells/shells.h src/plan/Plan.cpp src/plan/Plan.h src/plan/Task.cpp src/plan/Task.h src/lcpex/liblcpex.h src/lcpex/liblcpex.cpp src/lcpex/vpty/libclpex_tty.h src/lcpex/vpty/libclpex_tty.cpp src/lcpex/Contexts.h src/lcpex/Contexts.cpp src/lcpex/helpers.h src/lcpex/string_expansion/string_expansion.h src/lcpex/string_expansion/string_expansion.cpp src/lcpex/vpty/pty_fork_mod/pty_fork.h src/lcpex/vpty/pty_fork_mod/pty_fork.cpp src/lcpex/vpty/pty_fork_mod/pty_master_open.h src/lcpex/vpty/pty_fork_mod/pty_master_open.cpp src/lcpex/vpty/pty_fork_mod/tty_functions.h src/lcpex/vpty/pty_fork_mod/tty_functions.cpp )
