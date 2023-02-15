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

#ifndef REX_SUITE_H
#define REX_SUITE_H
#include <vector>
#include "../json_support/JSON.h"
#include "../logger/Logger.h"
#include "Unit.h"
#include "../misc/helpers.h"
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <dirent.h>


/**
 * @class Suite
 * @brief Loads unit definition files and provides access to the unit definitions.
 *
 * The Suite class extends the JSON_Loader class and loads unit definition files. It stores the valid unit definitions
 * in the `units` vector. The `get_unit` method can be used to retrieve a specific unit by name. The `load_units_file`
 * method is used to load a unit definition file and add valid unit definitions to the `units` vector. The
 * `get_units_from_dir` method is used to get a list of unit definition files from a directory. The `LOG_LEVEL` and
 * `slog` member variables are used to control logging.
 */
class Suite: public JSON_Loader
{
    protected:
        /// storage for the definitions we are amassing from the unit definition files
        std::vector<Unit> units;

    public:
        /**
         * @brief Constructor for Suite class.
         *
         * @param LOG_LEVEL The logging level to use.
         */
        Suite( int LOG_LEVEL );

        /**
         * @brief Load a unit definitions file and add valid unit definitions to `units` vector.
         *
         * @param filename The path to the unit definitions file.
         */
        void load_units_file( std::string filename );

        /**
         * @brief Retrieve a unit by name.
         *
         * @param result The Unit object that will be set to the unit with the provided name.
         * @param provided_name The name of the unit to retrieve.
         */
        void get_unit(Unit & result, std::string provided_name);

    private:
        /**
         * @brief Get a list of unit definition files from a directory.
         *
         * @param files A pointer to a vector of strings that will be set to the list of unit definition files.
         * @param path The path to the directory containing the unit definition files.
         */
        void get_units_from_dir( std::vector<std::string> * files, std::string path );


    private:
        /// The logging level to use.
        int LOG_LEVEL;
        /// A logger for logging messages.
        Logger slog;
};
#endif //REX_SUITE_H