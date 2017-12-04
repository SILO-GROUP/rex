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

#ifndef FTESTS_SPROC_H
#define FTESTS_SPROC_H

#include <string>
#include <iostream>

// executes a subprocess and captures STDOUT, STDERR, and return code.
// should be able to recieve path of binary to be executed as well as any parameters
class Sproc {
    public:
        // call the object.  returnvalue is enum representing external execution attempt not binary exit code
        static int execute( std::string input );
};

#endif //FTESTS_SPROC_H
