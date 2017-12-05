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

#ifndef FTESTS_UNITS_H
#define FTESTS_UNITS_H

#include <vector>
#include "../json/json.h"
#include "JSON_Loader.h"
#include "Unit.h"


class Suite: public JSON_Loader
{
    protected:
        // storage for the definitions we are amassing from the unit definition files
        std::vector<Unit> units;

    public:
        // constructor, empty
        Suite();

        // load a unit definitions file and add valid unit definitions to this->units
        void load_units_file( std::string filename, bool verbose );

        // returns the unit identified by name
        void get_unit(Unit & result, std::string provided_name);
};

#endif //FTESTS_UNITS_H
