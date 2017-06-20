#include "Unit.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

Unit::Unit() {}

int Unit::load_root(Json::Value loader_root)
{
    this->name      = loader_root.get("name", "?").asString();
    this->target    = loader_root.get("target", "?").asString();
    this->output    = loader_root.get("output", "?").asString();
    this->rectifier = loader_root.get("rectifier", "?").asString();
    this->active    = loader_root.get("active", "?").asString();
    this->required  = loader_root.get("required", "?").asString();
    this->rectify   = loader_root.get("rectify", "?").asString();

    return EXIT_SUCCESS;
}

// TODO CHANGE HOW THIS WORKS
int Unit::load_string(std::string json_val)
{
    // reads from a string into a Json::Value type.
    Json::Reader json_reader;

    // the deserialized json type to contain what's read by the reader
    Json::Value serialized;

    // create the ifstream file handle for the parser method to consume
    std::ifstream json_file_ifstream( json_val.c_str(), std::ifstream::binary );

    // use the reader to parse the ifstream to the local property
    bool parsingSuccessful = json_reader.parse( json_file_ifstream, serialized );

    if (! parsingSuccessful )
    {
        std::cerr << "Failed to parse adhoc JSON value." << std::endl << json_val << std::endl << std::endl << json_reader.getFormattedErrorMessages();
        throw JSON_Loader_InvalidJSON();

    } else {
        // if in verbose mode, give the user an "it worked" message
        if ( verbose )
        {
            std::cout << "Successfully parsed JSON string with " << this->json_root.size() << " elements.  Value:" << std::endl;
            std::cout << json_val << std::endl << std::endl;
        }
    }
    // exit successfully
    this->populated = true;
}


// getters for Unit type.
std::string Unit::get_name()
{
    return this->name;
}

std::string Unit::get_target()
{
    return this->target;
}

std::string Unit::get_output()
{
    return this->output;
}

std::string Unit::get_rectifier()
{
    return this->rectifier;
}

std::string Unit::get_active()
{
    return this->active;
}

std::string Unit::get_required()
{
    return this->required;
}

std::string Unit::get_rectify()
{
    return this->rectify;
}