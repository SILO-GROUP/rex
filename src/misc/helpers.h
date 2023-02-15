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

#endif //REX_HELPERS_H
