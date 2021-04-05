/*
    rex - An automation and testing framework.

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

#ifndef REX_UNITS_H
#define REX_UNITS_H

#include <vector>
#include "../../json/json.h"
#include "../low_level/JSON_Loader.h"
#include "Unit.h"
#include "../../Logger/Logger.h"
#include "../misc/helpers.h"


class Suite: public JSON_Loader
{
    protected:
        // storage for the definitions we are amassing from the unit definition files
        std::vector<Unit> units;

    public:
        // constructor
        Suite( int LOG_LEVEL );

        // load a unit definitions file and add valid unit definitions to this->units
        void load_units_file( std::string filename );

        // returns the unit identified by name
        void get_unit(Unit & result, std::string provided_name);

    private:
        void get_units_from_dir( std::vector<std::string> * files, std::string path );


private:
        int LOG_LEVEL;
        Logger slog;
};

#endif //REX_UNITS_H
