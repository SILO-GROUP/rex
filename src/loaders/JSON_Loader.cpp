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

#include "JSON_Loader.h"
#include "helpers.h"
#include <stdexcept>

/// JSON_Loader_NotReady - Exception thrown when a member function is called before data is populated.
class JSON_Loader_NotReady: public std::runtime_error { public:
    JSON_Loader_NotReady(): std::runtime_error("JSON_Loader: Tried to access JSON without actually populating JSON.") {}
};

/// JSON_Loader_FileNotfound - Exception thrown when JSON_Loader can not find the file it is told to parse.
class JSON_Loader_FileNotFound: public std::runtime_error { public:
    JSON_Loader_FileNotFound(): std::runtime_error("JSON_Loader: The requested file could not be found.") {}
};

/// JSON_Loader_InvalidJSON - Exception thrown when JSON_Loader fails to parse the JSON provided.
class JSON_Loader_InvalidJSON: public std::runtime_error { public:
    JSON_Loader_InvalidJSON(): std::runtime_error("JSON_Loader: The JSON provided could not be parsed.") {}
};

/// JSON_Loader::JSON_Loader - Constructor for JSON_Loader base class.  Simply inits to an unpopulated state.
///
/// The JSON_Loader type is a base type.  It is meant to provide the functionalities shared between Suite and Plan.
JSON_Loader::JSON_Loader()
{
    this->populated = false;
}

/// JSON_Loader::load_json_file - Loads JSON from a filepath into a serialized representation assigned as a local member
/// intended to be used as a buffer for further operations by base methods and derived class methods.
///
/// \param filename -
/// \param verbose
void JSON_Loader::load_json_file( std::string filename, bool verbose )
{
    // reads from a file into a Json::Value type.
    Json::Reader json_reader;

    // the a deserialized json type to contain what's read by the reader
    Json::Value json_root;

    // first, check if the file exists
    if (! exists( filename ) )
    {
        std::cerr << "File '" << filename << "' does not exist." << std::endl;
        throw JSON_Loader_FileNotFound();
    }

    // create the ifstream file handle
    std::ifstream json_file_ifstream( filename, std::ifstream::binary );

    // use the reader to parse the ifstream to the local property
    bool parsingSuccessful = json_reader.parse( json_file_ifstream, this->json_root );

    if (! parsingSuccessful )
    {
        std::cerr << "Failed to parse '" << filename << "':\n\t" << json_reader.getFormattedErrorMessages();
        throw JSON_Loader_InvalidJSON();

    } else {
        // if in verbose mode, give the user an "it worked" message
        if (verbose)
        {
            std::cout << "Parsed '" << filename << "' with " << this->json_root.size() << " element(s)." << std::endl;
        }
    }
    // Flag as ready for consumption.
    this->populated = true;
}

/// JSON_Loader::load_json_string - loads json from std::string into a json::value type and sets to protected member
/// 'json_root'.
///
/// \param input - The JSON-formatted string to serialize
/// \param verbose - Whether or not to print verbose information to STDOUT.
void JSON_Loader::load_json_string( std::string input, bool verbose )
{
    // reads from a string into a Json::Value type.
    Json::Reader json_reader;

    // the deserialized json type to contain what's read by the reader
    // Json::Value json_root;

    // create the ifstream file handle
    std::ifstream json_file_ifstream( input.c_str(), std::ifstream::binary );

    // use the reader to parse the ifstream to the local property
    bool parsingSuccessful = json_reader.parse( json_file_ifstream, this->json_root );

    if (! parsingSuccessful )
    {
        std::cerr << "Failed to parse adhoc JSON value." << std::endl << input << std::endl << std::endl << json_reader.getFormattedErrorMessages();
        throw JSON_Loader_InvalidJSON();

    } else {
        // if in verbose mode, give the user an "it worked" message
        if ( verbose )
        {
            std::cout << "Successfully parsed JSON string with " << this->json_root.size() << " elements.  Value:" << std::endl;
            std::cout << input << std::endl << std::endl;
        }
    }
    // flag as ready for consumption
    this->populated = true;
}


/// JSON_Loader::as_string -  returns the string representation of json_root
std::string JSON_Loader::as_string()
{
    if ( ! this->populated ) { throw JSON_Loader_NotReady(); }

    return this->json_root.asString();
}

/// JSON_Loader::get_serialized - assigns the serialized representation of the value of a key (json::value)
///
/// \param input - A reference to the json::value object to receive the new value.
/// \param key - The JSON key name to assign the value to (the root of the json::value object by name)
/// \param verbose - Whether or not to print verbose output to STDOUT.
/// \return - Boolean indicator of success or failure (0|1)
int JSON_Loader::get_serialized(Json::Value &input, std::string key, bool verbose)
{
    // throw if the class is not ready to be used.
    if ( ! this->populated ) { throw JSON_Loader_NotReady(); }

    if ( this->json_root.isMember( key ) )
    {
        // key was found so return it to the passed input ref
        input = this->json_root[ key ];
        return 0;
    }

    // key was not found
    if ( verbose )
    {
        // verbose mode tells the user what key we were looking for.
        std::cerr << "Failed to find key '" << key << "'." << std::endl;
    }
    // exit code for failure
    return 1;
}