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

#ifndef FTESTS_HELPERS_H
#define FTESTS_HELPERS_H
#include <string>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>


bool exists (const std::string& name);

std::string get_working_path();
bool is_file( std::string );
bool is_dir( std::string );

#endif //FTESTS_HELPERS_H