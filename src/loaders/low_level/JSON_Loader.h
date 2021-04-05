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
#ifndef REX_JLOADER_H
#define REX_JLOADER_H
#include "../../json/json.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include "../misc/helpers.h"
#include "../../Logger/Logger.h"

class JSON_Loader
{
    protected:
        Json::Value json_root;
        bool populated;

    public:
        // constructor
        JSON_Loader( int LOG_LEVEL );

        // load from json file
        void load_json_file( std::string filename );

        // load from std::string json
        void load_json_string( std::string input );

        // return as a JSONCPP serialized object
        // deprecated -- these aren't really used.
        // Json::Value as_serialized();
        std::string as_string();

        // safely handle deserialized type retrieval (if we want it to be safe)
        int get_serialized(Json::Value &input, std::string key );

private:
    Logger slog;
    int LOG_LEVEL;
};
#endif //REX_JLOADER_H
