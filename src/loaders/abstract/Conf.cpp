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
Conf::Conf(std::string filename, int LOG_LEVEL ): JSON_Loader(LOG_LEVEL ), slog(LOG_LEVEL, "_conf_" )
{
    this->LOG_LEVEL = LOG_LEVEL;
    std::string jval_s;
    bool jval_b;

    // prepare context spaghetti
    this->override_context = false;

    interpolate( filename);

    try {
        // load the test file.
        this->load_json_file( filename );
    }
    catch (std::exception) {
        this->slog.log( E_FATAL, "Unable to locate configuration file: '" + filename + "'." );
        throw ConfigLoadException("Config file not found.");
    }

    // find the path to the unit definitions file
    if (this->get_string(jval_s, "units_path") != 0 )
    { throw ConfigLoadException("units_path string is not set in the config file supplied: " + filename); } else {
        interpolate( jval_s );
        this->units_path = jval_s;
    }
    jval_s = {0};

    // find the path to logs directory
    if (this->get_string(jval_s, "logs_path") != 0 )
    { throw ConfigLoadException("logs_path string is not set in the config file supplied: " + filename); } else {
        interpolate( jval_s );
        this->logs_path = jval_s;
    }
    jval_s = {0};

    if (this->get_bool(jval_b, "execution_context_override") != 0 )
    { throw ConfigLoadException("execution_context_override boolean is not set in the config file supplied: " + filename); } else {
        this->override_context = jval_b;
    }
    jval_b = {0};

    if (this->get_string(jval_s, "execution_context") != 0 )
    { throw ConfigLoadException("execution_context string is not set in the config file supplied: " + filename); } else {
            interpolate( jval_s );
            if ( ! is_dir( jval_s ) ) { throw ConfigLoadException( "The execution context supplied is an invalid directory."); } else {
                this->execution_context = jval_s;
            }
    }
    jval_s = {0};

};

/// Conf::has_context_override - Specifies whether or not the override context function is enabled in the Conf file.
bool Conf::has_context_override() {
    return this->override_execution_context;
}

/// Conf::get_execution_context - Specifies the path to the current working directory to set for all unit executions.
std::string Conf::get_execution_context() {
    return this->execution_context;
}

/// Conf::get_units_path - Retrieves the path to the Unit definition file from the application configuration file.
std::string Conf::get_units_path() { return this->units_path; }

/// Conf::get_units_path - Retrieves the path to the Unit definition file from the application configuration file.
std::string Conf::get_logs_path() { return this->logs_path; }

/// Conf::set_execution_context- Sets the execution context.
void Conf::set_execution_context(std::string execution_context )
{
    this->execution_context = execution_context;
}

