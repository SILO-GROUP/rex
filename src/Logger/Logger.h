/*
    Rex - A configuration management and workflow automation tool that
    compiles and runs in minimal environments.

    © SILO GROUP and Chris Punches, 2020.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

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
