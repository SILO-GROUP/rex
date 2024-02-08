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

#ifndef REX_HELPERS_H
#define REX_HELPERS_H

#include <string>
#include <cstring>
#include <string.h>

#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include <chrono>
#include <sstream>
#include <syslog.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <regex>


bool exists (const std::string& name);

std::string get_working_path();

bool is_file( std::string );

bool is_dir( std::string );

// expand environment variables in string
void interpolate( std::string & text);

std::string get_8601();

const char * command2args( std::string input_string );

/**
 * @brief Get the absolute path from a relative path
 *
 * This function takes a relative path and returns the corresponding absolute path.
 * The absolute path is obtained by calling the `realpath` function.
 * If the `realpath` function returns a null pointer, an error message is printed to the standard error stream and an empty string is returned.
 *
 * @param relative_path The relative path to be converted to an absolute path
 *
 * @return The absolute path corresponding to the relative path
 */
std::string get_absolute_path(const std::string &relative_path);

#endif //REX_HELPERS_H
