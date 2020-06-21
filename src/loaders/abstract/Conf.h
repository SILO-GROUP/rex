/*
    Examplar - An automation and testing framework.

    © SURRO INDUSTRIES and Chris Punches, 2017.

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

#ifndef FTESTS_CONF_H
#define FTESTS_CONF_H
#include "../low_level/JSON_Loader.h"
#include <exception>
#include "../../Logger/Logger.h"


#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
# define IMPL_CONFIG_VERSION 3
# define VERSION_STRING STRINGIZE(IMPL_CONFIG_VERSION)

class Conf: public JSON_Loader
{
private:
    Json::Value plan_path;
    Json::Value units_path;
    Json::Value execution_context;
    Json::Value config_version;
    Json::Value env_vars_file;

    // flag to indicate if execution context should be overriden in config file
    // if set to true Examplar should use whats in the config file for current working directory
    // if set to false, Examplar should use the current working directory at time of execution
    Json::Value override_execution_context;

    bool override_context;
    std::string execution_context_literal;

public:
    Conf( std::string filename, int LOG_LEVEL );

    bool has_context_override();

    std::string get_plan_path();
    std::string get_units_path();
    std::string get_execution_context();

    void set_execution_context( std::string );

    std::string get_env_vars_file();
private:
    int LOG_LEVEL;
    Logger slog;

};

#endif //FTESTS_CONF_H
