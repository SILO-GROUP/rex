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
#include "Conf.h"

/// CONF_PLANPATH_INVALID - Exception thrown when the Conf type can not load the supplied path for the Plan definition
/// file.
class CONF_PLANPATH_INVALID: public std::runtime_error { public:
    CONF_PLANPATH_INVALID(): std::runtime_error("conf: The supplied path for the plan definition file is invalid.") {}
};

/// CONF_UNITSPATH_INVALID - Exception thrown when the Conf type can not load the supplied path for the Unit definition
/// files.
class CONF_UNITSPATH_INVALID: public std::runtime_error { public:
    CONF_UNITSPATH_INVALID(): std::runtime_error("conf: The supplied path for the unit definition file is invalid.") {}
};

/// Conf::Conf - Constructor for Conf type.  Loads the configuration for the application.
/// TODO Expand to detect when a directory path is supplied for units_path or plan_path and import all Tasks and Units.
///
/// \param filename - The filename to load the configuration from.
Conf::Conf( std::string filename, bool verbose ): JSON_Loader()
{
    // load the conf file.
    this->load_json_file( filename, verbose );

    // find the path to the plan file
    if (this->get_serialized(this->plan_path, "plan_path", true) != 0 ) { throw CONF_PLANPATH_INVALID(); }

    // find the path to the unit definitions file
    if (this->get_serialized(this->units_path, "units_path", true) != 0 ) { throw CONF_UNITSPATH_INVALID(); }
};

/// Conf::get_plan_path - Retrieves the path to the Plan definition file from the application configuration file.
std::string Conf::get_plan_path() { return this->plan_path.asString(); }

/// Conf::get_units_path - Retrieves the path to the Unit definition file from the application configuration file.
std::string Conf::get_units_path() { return this->units_path.asString(); }
