#include "JSON_Loader.h"
#include "helpers.h"
#include <stdexcept>

class JSON_Loader_NotReady: public std::runtime_error
{
public:
    JSON_Loader_NotReady(): std::runtime_error("JSON_Loader: Tried to access JSON without actually populating JSON.") {}
};

class JSON_Loader_FileNotFound: public std::runtime_error
{
public:
    JSON_Loader_FileNotFound(): std::runtime_error("JSON_Loader: The requested file could not be found.") {}
};

class JSON_Loader_InvalidJSON: public std::runtime_error
{
public:
    JSON_Loader_InvalidJSON(): std::runtime_error("JSON_Loader: The JSON provided could not be parsed.") {}
};

JSON_Loader::JSON_Loader()
{
    this->populated = false;
}

// loads json from a file into a deserializable type and sets to private member json_root
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
    // exit successfully and set flag that this can be used now.
    this->populated = true;
}

// loads json from std::string into a serialized type and sets to private member json_root
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
    // exit successfully
    this->populated = true;
}


// returns the serialized representation of json_root
Json::Value JSON_Loader::as_serialized()
{
    if ( ! this->populated ) { throw JSON_Loader_NotReady(); }

    return this->json_root;
}

// returns the string representation of json_root
std::string JSON_Loader::as_string()
{
    if ( ! this->populated ) { throw JSON_Loader_NotReady(); }

    return this->json_root.asString();
}

// returns the serialized representation of the value of a key
// the Jason::Value object to assign the fetched value to
// verbosity flag
// exit or not on failure
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