//
// Created by bagira on 6/13/20.
//

#ifndef EXAMPLAR_LOGGER_H
#define EXAMPLAR_LOGGER_H

#include <syslog.h>
#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

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
    std::string get_8601();
};



#endif //EXAMPLAR_LOGGER_H
