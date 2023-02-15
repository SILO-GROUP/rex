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


#include "Suite.h"

/**
 * @class SuiteException
 * @brief Exception thrown when a Suite tries to access a contained Unit's value that is not present in the Unit.
 *
 * This exception is thrown when a Suite object tries to access a value in a contained Unit object that is not present in the Unit. The error message is stored in the `msg_` member variable and can be retrieved with the `what` method.
 */
class SuiteException: public std::exception
{
    public:
        /**
         * @brief Constructor for SuiteException class (C-style string).
         *
         * @param message C-style string error message. The string contents are copied upon construction. The caller is responsible for deleting the char*.
         */
        explicit SuiteException(const char* message):
                msg_(message)
        {
        }

        /**
         * @brief Constructor for SuiteException class (C++ STL string).
         *
         * @param message The error message.
         */
        explicit SuiteException(const std::string& message):
                msg_(message)
        {}

        /**
         * @brief Destructor for SuiteException class.
         *
         * Virtual to allow for subclassing.
         */
        virtual ~SuiteException() throw (){}

        /**
         * @brief Get the error message.
         *
         * @return A pointer to a const char*. The underlying memory is in the possession of the Exception object. Callers should not attempt to free the memory.
         */
        virtual const char* what() const throw (){
            return msg_.c_str();
        }

    protected:
        /// Error message.
        std::string msg_;
};


/**
 * @brief Constructor for Suite class.
 *
 * The Suite class is simply a managed container for a Unit vector.
 * Once instantiated, all methods will require either a JSON file or string to be loaded as deserialized Unit types
 * before being called or will simply throw an exception.

 * From the high level, a Suite contains the full definitions of all potential Units to execute defined in the Unit
 * definition files that it is loading.  It is meant to be used in such a way that as the application iterates through
 * the Task objects contained by the application Plan, it will iterate through the appplication's Suite, which contains
 * the definition of all available Tasks.  In this manner, defining units and executing units are split into separate
 * human processes to allow modularly developed profiles of test suites.  As inferred, Unit is expected to be one of
 * the two types that are only instantiated once per application run, though it is designed to be used more than once
 * if the implementor so desires.
 */
 Suite::Suite( int LOG_LEVEL ): JSON_Loader( LOG_LEVEL ), slog( LOG_LEVEL, "_suite" )
{
    this->LOG_LEVEL;
}


/**
 * @brief Get the list of unit files in a directory
 *
 * This function retrieves all the files with a ".units" extension in the specified directory.
 *
 * @param[out] files The list of unit files
 * @param[in] path The path of the directory to search
 *
 * @return None
 */
void Suite::get_units_from_dir( std::vector<std::string> * files, std::string path )
{
    // TODO: this whole method probably needs rewritten
    DIR* dirFile = opendir( path.c_str() );
    if ( dirFile )
    {
        std::string entry;

        struct dirent* hFile;
        errno = 0;
        while (( hFile = readdir( dirFile )) != NULL )
        {
            if ( !strcmp( hFile->d_name, "."  ))
            {
                continue;
            }
            if ( !strcmp( hFile->d_name, ".." ))
            {
                continue;
            }

            // hidden files
            if ( hFile->d_name[0] == '.' )
            {
                continue;
            }

            // dirFile.name is the name of the file. Do whatever string comparison
            // you want here. Something like:
            if ( strstr( hFile->d_name, ".units" ))
            {
                std::string full_path = path + hFile->d_name;
                files->push_back( full_path );
            }
        }
        closedir( dirFile );
    } else {
        this->slog.log( E_DEBUG, "File not found: " + path );
    }
}


/**
 * @brief Load units from a file or directory
 *
 * This function loads units from a file or directory containing unit files.
 * The unit files must be in JSON format.
 *
 * @param[in] units_path The path to the file or directory containing unit files
 *
 * @return None
 */
void Suite::load_units_file( std::string units_path )
{
    std::vector<std::string> unit_files;

    if ( is_dir( units_path ) )
    {
        // we have a directory path.  find all files ending in *.units and load them into a vector<std::string>
        get_units_from_dir( &unit_files, units_path );
    }

    if ( is_file( units_path ) )
    {
        unit_files.push_back( units_path );
    }

    this->slog.log( E_INFO, "Unit files found: " + std::to_string( unit_files.size() ) );

    for ( int i = 0; i < unit_files.size(); i++ )
    {
        // will use json_root buffer on each run to append to this->units vector as valid units are found.
        this->load_json_file( unit_files[i] );

        // staging buffer
        Json::Value jbuff;

        // fill the jbuff staging buffer with a json::value object in the supplied units_path
        if ( this->get_serialized( jbuff, "units" ) == 0)
        {
            this->json_root = jbuff;
        }

        // iterate through the json::value members that have been loaded.  append to this->units vector
        // buffer for units to append:
        Unit tmp_U = Unit( this->LOG_LEVEL );

        for ( int index = 0; index < this->json_root.size(); index++ )
        {
            // assemble the unit from json_root using the built-in value operator
            tmp_U.load_root( this->json_root[ index ] );
            if ( tmp_U.get_active() ) {
                // append to this->units
                this->units.push_back( tmp_U );
                this->slog.log( E_INFO, "Added unit \"" + tmp_U.get_name() + "\" to Suite.");
            }
        }
    }
}


/**
 * @brief Returns a contained Unit identified by the `provided_name` attribute.
 *
 * @param result The unit type that receives the unit's value.
 * @param provided_name The name of the unit being fetched.
 *
 * @throws SuiteException if the unit with the specified name is not found.
 */
void Suite::get_unit(Unit & result, std::string provided_name)
{
    bool foundMatch = false;

    for ( int i = 0; i < this->units.size(); i++ )
    {
        std::string unit_name = this->units[i].get_name();
        if ( unit_name == provided_name )
        {
            result = this->units[i];
            foundMatch = true;
            break;
        }
    }

    if (! foundMatch )
    {
        this->slog.log( E_FATAL, "Unit name \"" + provided_name + "\" was referenced but not defined!" );
        throw SuiteException( "Undefined unit referenced." );
    }
}