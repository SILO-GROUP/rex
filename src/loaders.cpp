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
        std::cerr << "File '" << filename << "' does not exist.";
        exit(1);
    }

    std::ifstream jfile( filename, std::ifstream::binary );

    bool parsingSuccessful = reader.parse( jfile, this->json_root );
    if (! parsingSuccessful )
    {
        std::cerr << "Failed to parse " << filename << ":\n\t" << reader.getFormattedErrorMessages();
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

Plan::Plan( std::string filename ): JLoader( filename )
{

};

Conf::Conf( std::string filename ): JLoader( filename )
{
    this->plan_path = this->get_root()["plan_path"].asString();
    this->units_path = this->get_root()["units_path"].asString();
};

std::string Conf::get_plan_path()
{
    return this->plan_path;
}

std::string Conf::get_units_path()
{
    return this->units_path;
}

