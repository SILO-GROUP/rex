//
// Created by phanes on 4/16/17.
//

#include "loaders.h"
#include "json/json.h"
#include "json/json-forwards.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>


inline bool exists(const std::string& name)
{
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

JLoader::JLoader( std::string filename )
{
    Json::Value json_root;
    Json::Reader reader;

    if (! exists( filename ) )
    {
        std::cout << "File '" << filename << "' does not exist.";
        exit(1);
    }

    std::ifstream jfile( filename, std::ifstream::binary );

    bool parsingSuccessful = reader.parse( jfile, this->json_root );
    if (! parsingSuccessful )
    {
        std::cout << "Failed to parse " << filename << ":\n\t" << reader.getFormattedErrorMessages();
        exit(1);
    } else {
        std::cout << "Parsed " << filename << " with " << this->json_root.size() << " elements." << std::endl;
    }
}




Json::Value JLoader::get_root()
{
    return this->json_root;
}




Unit::Unit( std::string filename ): JLoader( filename )
{

};
