//
// Created by bagira on 6/13/20.
//

#ifndef REX_LOGGER_H
#define REX_LOGGER_H

#include <syslog.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "../loaders/misc/helpers.h"

enum L_LVL {
    E_FATAL,
    E_WARN,
    E_INFO,
    E_DEBUG
};

class Logger {
public:
    Logger( int LOG_LEVEL, std::string mask );
    void log( int LOG_LEVEL, std::string msg );

private:
    int LOG_LEVEL;
    std::string mask;
};



#endif //REX_LOGGER_H
