//
// Created by phanes on 4/22/17.
//

#include "JSON_Loader.h"
#include "helpers.h"

JSON_Loader::JSON_Loader()
{
    // empty
}

// loads json from a file into a deserializable type and sets to private member json_root
int JSON_Loader::load_json_file( std::string filename, bool verbose )
{
    // reads from a file into a Json::Value type.
    Json::Reader json_reader;

    // the a deserialized json type to contain what's read by the reader
    Json::Value json_root;

    // first, check if the file exists
    if (! exists( filename ) )
    {
        std::cerr << "File '" << filename << "' does not exist.";
        // exit with failure signal
        return 1;
    }

    // create the ifstream file handle
    std::ifstream json_file_ifstream( filename, std::ifstream::binary );

    // use the reader to parse the ifstream to the local property
    bool parsingSuccessful = json_reader.parse( json_file_ifstream, this->json_root );

    if (! parsingSuccessful )
    {
        std::cerr << "Failed to parse " << filename << ":\n\t" << json_reader.getFormattedErrorMessages();
        // exit with failure signal
        return 1;
    } else {
        // if in verbose mode, give the user an "it worked" message
        if (verbose)
        {
            std::cout << "Parsed " << filename << " with " << this->json_root.size() << " elements." << std::endl;
        }
    }
    // exit successfully
    return 0;
}

// loads json from std::string into a serialized type and sets to private member json_root
int JSON_Loader::load_json_string( std::string input, bool verbose )
{
    // reads from a string into a Json::Value type.
    Json::Reader json_reader;

    // the deserialized json type to contain what's read by the reader
    Json::Value json_root;

    // create the ifstream file handle
    std::ifstream json_file_ifstream( input.c_str(), std::ifstream::binary );

    // use the reader to parse the ifstream to the local property
    bool parsingSuccessful = json_reader.parse( json_file_ifstream, this->json_root );

    if (! parsingSuccessful )
    {
        std::cerr << "Failed to parse adhoc JSON value." << std::endl << input << std::endl << std::endl << json_reader.getFormattedErrorMessages();
        // exit with failure signal
        return 1;
    } else {
        // if in verbose mode, give the user an "it worked" message
        if (verbose)
        {
            std::cout << "Successfully parsed JSON string with " << this->json_root.size() << " elements.  Value:" << std::endl;
            std::cout << input << std::endl;
        }
    }
    // exit successfully
    return 0;
}


// returns the serialized representation of json_root
Json::Value JSON_Loader::as_serialized()
{
    return this->json_root;
}

// returns the string representation of json_root
std::string JSON_Loader::as_string()
{
    return this->json_root.asString();
}