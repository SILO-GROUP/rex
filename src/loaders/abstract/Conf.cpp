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

/// ConfigLoadException - General exception handler for the Conf class.
class ConfigLoadException: public std::exception
{
public:
    /** Constructor (C strings).
     *  @param message C-style string error message.
     *                 The string contents are copied upon construction.
     *                 Hence, responsibility for deleting the char* lies
     *                 with the caller.
     */
    explicit ConfigLoadException(const char* message):
            msg_(message)
    {
    }

    /** Constructor (C++ STL strings).
     *  @param message The error message.
     */
    explicit ConfigLoadException(const std::string& message):
            msg_(message)
    {}

    /** Destructor.
     * Virtual to allow for subclassing.
     */
    virtual ~ConfigLoadException() throw (){}

    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *          is in posession of the Exception object. Callers must
     *          not attempt to free the memory.
     */
    virtual const char* what() const throw (){
        return msg_.c_str();
    }

protected:
    /** Error message.
     */
    std::string msg_;
};


/// Conf::Conf - Constructor for Conf type.  Loads the configuration for the application.
/// TODO Expand to detect when a directory path is supplied for units_path or plan_path and import all Tasks and Units.
///
/// \param filename - The filename to load the configuration from.
Conf::Conf( std::string filename, int LOG_LEVEL ): JSON_Loader( LOG_LEVEL ), slog( LOG_LEVEL, "e_conf" )
{
    this->LOG_LEVEL = LOG_LEVEL;

    // prepare context spaghetti
    this->override_context = false;

    try {
        // load the test file.
        this->load_json_file( filename );
    }
    catch (std::exception) {
        this->slog.log( E_FATAL, "Unable to locate configuration file: '" + filename + "'." );
        throw ConfigLoadException("Config file not found.");
    }

    if (this->get_serialized(this->config_version, "config_version" ) != 0)
    {
        throw ConfigLoadException("config_version string is not set in the config file supplied: " + filename);
    }
    if ( this->config_version.asString() != VERSION_STRING )
    {
        throw ConfigLoadException("config_version string expected was " + std::string(VERSION_STRING) + " in: " + filename);
    }

    // find the path to the plan file
    if ( this->get_serialized(this->plan_path, "plan_path" ) != 0 )
    {
        throw ConfigLoadException("plan_path string is not set in the config file supplied:" + filename);
    }

    // find the path to the unit definitions file
    if (this->get_serialized(this->units_path, "units_path" ) != 0 )
    {
        throw ConfigLoadException("units_path string is not set in the config file supplied: " + filename);
    }

    if ( this->get_serialized(this->override_execution_context, "execution_context_override" ) != 0 )
    {
        throw ConfigLoadException("execution_context_override boolean is not set in the config file supplied: " + filename);
    } else {
        this->override_context = true;
    }

    if ( this->get_serialized(this->execution_context, "execution_context" ) != 0 )
    {
        throw ConfigLoadException("execution_context string is not set in the config file supplied: " + filename);
    } else {
            this->execution_context_literal = this->execution_context.asString();
    }
};

/// Conf::has_context_override - Specifies whether or not the override context function is enabled in the conf file.
bool Conf::has_context_override() {
    return this->override_execution_context.asBool();
}

/// Conf::get_execution_context - Specifies the path to the current working directory to set for all unit executions.
std::string Conf::get_execution_context() {
    return this->execution_context_literal;
}

/// Conf::get_plan_path - Retrieves the path to the Plan definition file from the application configuration file.
std::string Conf::get_plan_path() { return this->plan_path.asString(); }

/// Conf::get_units_path - Retrieves the path to the Unit definition file from the application configuration file.
std::string Conf::get_units_path() { return this->units_path.asString(); }

/// Conf::set_execution_context- Sets the execution context.
void Conf::set_execution_context( std::string execution_context )
{
    this->execution_context_literal = execution_context;
}

