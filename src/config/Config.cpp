#include "Config.h"


/**
 * @brief General exception class for the Conf class.
 *
 * This exception class can be thrown to indicate an error in the Conf class. It provides an error message describing the problem.
 */
class ConfigLoadException: public std::exception
{
    public:
        /**
         * @brief Constructor that takes a C-style string error message.
         *
         * The string contents are copied upon construction. The caller is responsible for deleting the char*.
         *
         * @param message C-style string error message.
         */
        explicit ConfigLoadException(const char* message):
                msg_(message)
        {}

        /**
         * @brief Constructor that takes a C++ STL string error message.
         *
         * @param message The error message.
         */
        explicit ConfigLoadException(const std::string& message):
                msg_(message)
        {}

        /**
         * @brief Virtual destructor to allow for subclassing.
         */
        virtual ~ConfigLoadException() throw (){}

        /**
         * @brief Returns a pointer to the error description.
         *
         * @return A pointer to a const char*. The underlying memory is in posession of the Exception object. Callers must not attempt to free the memory.
         */
        virtual const char* what() const throw (){
            return msg_.c_str();
        }

    protected:
        /**
         * @brief Error message.
         */
        std::string msg_;
};


/**
 * @brief Set the string value of a key
 *
 * This method sets the value of a key as a string in a member variable.
 * It first retrieves the string value of the key using the `get_string` method.
 * If the key is not found, a `ConfigLoadException` is thrown.
 * If the key is found, the string value is interpolated using the `interpolate` method and then assigned to the member variable.
 * The method logs the task in the log file using the `slog.log_task` method.
 *
 * @param keyname The name of the key to retrieve
 * @param object_member The reference to the member variable to store the value
 * @param filename The name of the configuration file
 *
 * @throws ConfigLoadException If the key is not found in the configuration file
 */
void Conf::set_object_s(std::string keyname, std::string & object_member, std::string filename )
{
    std::string jval_s;
    if ( this->get_string(jval_s, keyname) != 0) {
        throw ConfigLoadException( "'" + keyname + "' string is not set in the config file supplied: " + filename );
    } else {
        interpolate(jval_s);
        object_member = jval_s;
    }
    this->slog.log_task( E_DEBUG, "SET_PROPERTY", "'" + keyname + "': " + object_member );
}


/**
 * @brief Set the string value of a key with a derived path
 *
 * This method sets the value of a key as a string in a member variable with a derived path.
 * It first retrieves the string value of the key using the `get_string` method.
 * If the key is not found, a `ConfigLoadException` is thrown.
 * If the key is found, the string value is interpolated using the `interpolate` method and then passed to the `prepend_project_root` method to derive the path.
 * The derived path is then assigned to the member variable.
 * The method logs the task in the log file using the `slog.log_task` method.
 *
 * @param keyname The name of the key to retrieve
 * @param object_member The reference to the member variable to store the value
 * @param filename The name of the configuration file
 *
 * @throws ConfigLoadException If the key is not found in the configuration file
 */
void Conf::set_object_s_derivedpath(std::string keyname, std::string & object_member, std::string filename )
{
    std::string jval_s;
    if ( this->get_string(jval_s, keyname) != 0) {
        throw ConfigLoadException( "'" + keyname + "' string is not set in the config file supplied: " + filename );
    } else {
        interpolate(jval_s);
        object_member = prepend_project_root( jval_s );
    }
    this->slog.log_task( E_DEBUG, "SET_PROPERTY", "'" + keyname + "': " + object_member );
}


/**
 * @brief Set the boolean value of a key
 *
 * This method sets the value of a key as a boolean in a member variable.
 * It first retrieves the boolean value of the key using the `get_bool` method.
 * If the key is not found, a `ConfigLoadException` is thrown.
 * If the key is found, the boolean value is assigned to the member variable.
 * The method logs the task in the log file using the `slog.log_task` method.
 *
 * @param keyname The name of the key to retrieve
 * @param object_member The reference to the member variable to store the value
 * @param filename The name of the configuration file
 *
 * @throws ConfigLoadException If the key is not found in the configuration file
 */
void Conf::set_object_b(std::string keyname, bool & object_member, std::string filename )
{
    bool jval_b;
    if ( this->get_bool(jval_b, keyname) != 0) {
        throw ConfigLoadException( "'" + keyname + "' boolean is not set in the config file supplied: " + filename );
    } else {
        object_member = jval_b;
    }
    this->slog.log_task( E_DEBUG, "SET_PROPERTY", "'" + keyname + "' " + std::to_string(object_member));
}

/**
 * @brief Prepend the project root to a relative path
 *
 * This method prepends the project root to a given relative path.
 * The project root is stored as a member variable in the `Conf` object.
 * The method concatenates the project root and the relative path separated by a forward slash (/) and returns the result.
 *
 * @param relative_path The relative path to prepend the project root to
 *
 * @return The concatenated path with the project root and the relative path
 */
std::string Conf::prepend_project_root( std::string relative_path)
{
    return this->project_root + "/" + relative_path;
}

/**
 * @brief Get the absolute path from a relative path
 *
 * This function takes a relative path and returns the corresponding absolute path.
 * The absolute path is obtained by calling the `realpath` function.
 * If the `realpath` function returns a null pointer, an error message is printed to the standard error stream and an empty string is returned.
 *
 * @param relative_path The relative path to be converted to an absolute path
 *
 * @return The absolute path corresponding to the relative path
 */
std::string get_absolute_path(const std::string &relative_path)
{
    char resolved_path[1024];
    memset(resolved_path, 0, sizeof(resolved_path));

    if( realpath( relative_path.c_str(), resolved_path) == nullptr ) {
        std::cerr << "Error resolving path: " << relative_path << std::endl;
        return "";
    }

    return std::string(resolved_path);
}

/**
 * @brief Check if a path exists
 *
 * This method checks if a given path exists.
 * If the path exists, a log entry is made in the log file using the `slog.log_task` method with log level `E_DEBUG`.
 * If the path does not exist, a log entry is made in the log file with log level `E_FATAL` and a `ConfigLoadException` is thrown.
 *
 * @param keyname The name of the key to be used in the log entry
 * @param path The path to be checked
 *
 * @throws ConfigLoadException If the path does not exist
 */
void Conf::checkPathExists( std::string keyname, const std::string &path ) {
    if ( exists( path ) ) {
        this->slog.log_task( E_DEBUG, "SANITY_CHECKS", "'" + keyname + "' exists ('" + path + "')" );
    } else {
        this->slog.log_task( E_FATAL, "SANITY_CHECKS", "'" + keyname + "' does not exist ('" + path + "')" );
        throw ConfigLoadException("Path does not exist.");
    }
}

/**
 * @brief Load the shells
 *
 * This method loads the shell definitions from a file.
 * The path to the shell definition file is stored as a member variable in the `Conf` object.
 * The method first logs the task of loading the shells using the `slog.log_task` method.
 * The method then tries to load the JSON file using the `load_json_file` method.
 * If an exception occurs while loading the file, a log entry with log level `E_FATAL` is made and a `ConfigLoadException` is thrown.
 * The method then retrieves the serialized value of the "shells" key using the `get_serialized` method.
 * If the "shells" key is not found, a log entry with log level `E_FATAL` is made and a `ConfigLoadException` is thrown.
 * The method then loops through the serialized values, loads each shell definition using the `load_root` method of the `Shell` class, and stores each shell in the `shells` vector.
 * The method logs each loaded shell using the `slog.log_task` method.
 *
 * @throws ConfigLoadException If there is an error parsing the shell definition file
 */
void Conf::load_shells() {
    this->slog.log_task( E_DEBUG, "SHELLS", "Loading shells..." );
    try {
        // load the test file.
        this->load_json_file( this->shell_definitions_path );
    } catch (std::exception& e) {
        this->slog.log_task( E_FATAL, "SHELLS", "Unable to load shell definition file: '" + this->shell_definitions_path + "'. Error: " + e.what());
        throw ConfigLoadException("Parsing error in shell definitions file.");
    }
    Json::Value jbuff;

    if ( this-> get_serialized( jbuff, "shells" ) != 0 ) {
        this->slog.log_task( E_FATAL, "SHELLS", "Parsing error: '" + this->shell_definitions_path + "'. Error: 'shells' key not found." );
        throw ConfigLoadException("Parsing error in shell definitions file.");
    }

    Shell tmp_S = Shell( this->LOG_LEVEL );
    for ( int index = 0; index < jbuff.size(); index++ )
    {
        tmp_S.load_root( jbuff[index] );
        this->shells.push_back( tmp_S );
        this->slog.log_task( E_DEBUG, "SHELLS", "Loaded shell: '" + tmp_S.name + "' (" + tmp_S.path + ")" );
    }
}

/**
 * @brief Get a shell by name
 *
 * This method retrieves a `Shell` object by its name from the `shells` vector.
 * The method loops through the `shells` vector and compares each shell's name to the input name.
 * If a match is found, the matching shell is returned.
 * If no match is found, a `ConfigLoadException` is thrown.
 *
 * @param name The name of the shell to retrieve
 *
 * @return The `Shell` object corresponding to the input name
 *
 * @throws ConfigLoadException If the input name does not match any shell in the `shells` vector
 */
Shell Conf::get_shell_by_name( std::string name ) {
    for (auto &shell: this->shells) {
        if (shell.name == name) {
            return shell;
        }
    }
    throw ConfigLoadException("The shell specified ('" + name + "') is not defined in the shell definitions file.");
}


/**
 * @class Conf
 * @brief Loads the configuration for the application
 *
 * This class is responsible for loading the configuration for the application. It uses the `JSON_Loader` class to
 * parse the JSON configuration file and stores the relevant information.
 *
 * @note Currently, the class only supports loading a single configuration file. However, it can be expanded to
 * support loading multiple files or detecting when a directory path is supplied for the units_path or plan_path and
 * importing all Tasks and Units.
 *
 * @param filename - The filename to load the configuration from.
 * @param LOG_LEVEL - The log level to use for logging messages.
 */
Conf::Conf(std::string filename, int LOG_LEVEL ): JSON_Loader(LOG_LEVEL ), slog(LOG_LEVEL, "_conf_" )
{
    this->LOG_LEVEL = LOG_LEVEL;
    this->slog.log_task( E_DEBUG, "LOAD", "Loading configuration file: " + filename );

    interpolate( filename );

    try {
        // load the test file.
        this->load_json_file( filename );
    } catch (std::exception& e) {
        this->slog.log( E_FATAL, "Unable to load configuration file: '" + filename + "'. Error: " + e.what());
        throw ConfigLoadException("Parsing error in configuration file.");
    }
    Json::Value jbuff;

    if ( this->get_serialized( jbuff, "config" ) != 0) {
        this->slog.log_task( E_FATAL, "LOAD", "Unable to locate 'config' object in configuration file: " + filename );
        throw ConfigLoadException("Unable to locate 'config' object in configuration file.");
    } else {
        this->slog.log_task( E_DEBUG, "LOAD", "Found 'config' object in configuration file: " + filename );
        this->json_root = jbuff;
    }

    set_object_s(               "project_root",     this->project_root,           filename );
    // convert to an absolute path after all the interpolation is done.
    this->project_root = get_absolute_path( this->project_root );

    // all other paths are relative to project_root
    set_object_s_derivedpath(   "units_path",       this->units_path,             filename );
    set_object_s_derivedpath(   "logs_path",        this->logs_path,              filename );
    set_object_s_derivedpath(   "shells_path",      this->shell_definitions_path, filename );

    // ensure these paths exists, with exception to the logs_path, which will be created at runtime
    this->slog.log_task( E_DEBUG, "SANITY_CHECKS", "Checking for sanity..." );
    checkPathExists(            "project_root",     this->project_root );
    checkPathExists(            "units_path",       this->units_path );
    checkPathExists(            "shells_path",      this->shell_definitions_path );

    // shells are scoped beyond plan so they need to be considered part of config
    load_shells();

    this->slog.log_task( E_DEBUG, "LOAD", "CONFIGURATION LOADED." );
}

/**
 * @brief Gets the path to the Unit definition file
 *
 * This function returns the path to the Unit definition file that was specified in the configuration file.
 *
 * @return The path to the Unit definition file.
 */
std::string Conf::get_units_path() { return this->units_path; }

/**
 * @brief Gets the path to the logs directory
 *
 * This function returns the path to the logs directory that was specified in the configuration file.
 *
 * @return The path to the logs directory.
 */
std::string Conf::get_logs_path() { return this->logs_path; }

/**
 * @brief Gets the project root directory
 *
 * This function returns the path to the project root directory that was specified in the configuration file.
 *
 * @return The path to the project root directory.
 */
std::string Conf::get_project_root() { return this->project_root; }