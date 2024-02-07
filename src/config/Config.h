/*
    Rex - A configuration management and workflow automation tool that
    compiles and runs in minimal environments.

    © SILO GROUP LLC, 2023.

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

#ifndef REX_CONF_H
#define REX_CONF_H

#include <exception>
#include <string>
#include <string>
#include "../json_support/JSON.h"
#include "../logger/Logger.h"
#include "../misc/helpers.h"
#include "../shells/shells.h"

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define IMPL_CONFIG_VERSION 6
#define VERSION_STRING STRINGIZE(IMPL_CONFIG_VERSION)

/**
 * @class Conf
 * @brief This class loads configuration from a JSON file and provides access to the configuration values.
 *
 * The class `Conf` extends the `JSON_Loader` class and implements a constructor that takes the filename of the configuration file and the LOG_LEVEL to be used by the internal `Logger` instance.
 * The class provides getter methods to access the `units_path`, `logs_path`, and `project_root` values.
 */
class Conf: public JSON_Loader {
public:
    /**
     * @brief Constructs a Conf object and loads the configuration from the specified file
     *
     * @param filename The name of the configuration file
     * @param LOG_LEVEL The log level to be used by the internal Logger instance
     */
    Conf(std::string filename, int LOG_LEVEL);

    /**
     * @brief Returns the path to the units directory
     *
     * @return The path to the units directory
     */
    std::string get_units_path();

    /**
     * @brief Returns the path to the logs directory
     *
     * @return The path to the logs directory
     */
    std::string get_logs_path();

    /**
     * @brief Returns the root directory of the project
     *
     * @return The root directory of the project
     */
    std::string get_project_root();

    /**
     * @brief Returns the Shell object with the specified name
     *
     * @param name The name of the Shell object
     *
     * @return The Shell object with the specified name
     */
    Shell get_shell_by_name(std::string name);

private:
    /**
     * @brief The path to the units directory
     */
    std::string units_path;

    /**
     * @brief The path to the logs directory
     */
    std::string logs_path;

    /**
     * @brief The root directory of the project
     */
    std::string project_root;

    /**
     * @brief The path to the shell definitions
     */
    std::string shell_definitions_path;

    /**
     * @brief The vector of Shell objects
     */
    std::vector<Shell> shells;

    /**
     * @brief Checks if the specified path exists
     *
     * @param keyname The name of the key in the JSON file
     * @param path The path to be checked
     */
    void checkPathExists(std::string keyname, const std::string &path);

    /**
     * @brief The log level to be used by the internal Logger instance
     */
    int LOG_LEVEL;

    /**
     * @brief The internal Logger instance
     */
    Logger slog;

    /**
     * @brief Prepends the project root to a relative path
     *
     * @param relative_path The relative path to be modified
     *
     * @return The modified path with the project root prepended
     */
    std::string prepend_project_root(std::string relative_path);

    /**
     * @brief Sets a string object member from a JSON file
     *
     * @param keyname The name of the key in the JSON file
     * @param object_member The string object member to be set
     * @param filename The name of the JSON file
     */
    void set_object_s(std::string keyname, std::string &object_member, std::string filename);

    /**
     * @brief Sets a string object member from a JSON file, with the path derived from the project root
     *
     * @param keyname The name of the key in the JSON file
     * @param object_member The string object member to be set
     * @param filename The name of the JSON file
     */
    void set_object_s_derivedpath(std::string keyname, std::string &object_member, std::string filename);

    /**
     * @brief Sets a boolean object member from a JSON file
     *
     * @param keyname The name of the key in the JSON file
     * @param object_member The boolean object member to be set
     * @param filename The name of the JSON file
     */
    void set_object_b(std::string keyname, bool &object_member, std::string filename);

    /**
     * @brief Loads the shell definitions from the specified file
     */
    void load_shells();
};

#endif //REX_CONF_H

