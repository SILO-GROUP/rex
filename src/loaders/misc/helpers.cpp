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


#include "helpers.h"

bool exists(const std::string& name)
{
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

std::string get_working_path()
{
    char temp[MAXPATHLEN];
    return ( getcwd(temp, MAXPATHLEN) ? std::string( temp ) : std::string("") );
}

bool is_file( std::string path)
{
    struct stat buf;
    stat( path.c_str(), &buf );
    return S_ISREG(buf.st_mode);
}

bool is_dir( std::string path )
{
    struct stat buf;
    stat( path.c_str(), &buf );
    return S_ISDIR(buf.st_mode);
}

std::string get_8601()
{
    auto now = std::chrono::system_clock::now();
    auto itt = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ss;
    // ss << std::put_time(gmtime(&itt), "%FT%TZ");
    ss << std::put_time(localtime(&itt), "%Y-%m-%d_%H:%M:%S");
    return ss.str();
}