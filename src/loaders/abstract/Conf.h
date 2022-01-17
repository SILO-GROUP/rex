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

#ifndef REX_CONF_H
#define REX_CONF_H
#include "../low_level/JSON_Loader.h"
#include <exception>
#include "../../Logger/Logger.h"
#include "../misc/helpers.h"

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
# define IMPL_CONFIG_VERSION 5
# define VERSION_STRING STRINGIZE(IMPL_CONFIG_VERSION)

class Conf: public JSON_Loader
{
private:
    std::string plan_path;
    std::string units_path;
    std::string execution_context;
    std::string logs_path;

    // flag to indicate if execution context should be overriden in config file
    // if set to true rex should use whats in the config file for current working directory
    // if set to false, rex should use the current working directory at time of execution
    bool override_execution_context;

    bool override_context;

public:
    Conf( std::string filename, int LOG_LEVEL );

    bool has_context_override();

    std::string get_units_path();
    std::string get_execution_context();
    std::string get_logs_path();

    void set_execution_context( std::string );

private:
    int LOG_LEVEL;
    Logger slog;

};

#endif //REX_CONF_H
