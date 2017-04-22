//
// Created by phanes on 4/22/17.
//

#include "JLoader.h"
#include "helpers.h"

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
        std::exit( 1 );
    } else {
        std::cout << "Parsed " << filename << " with " << this->json_root.size() << " elements." << std::endl;
    }
}

Json::Value JLoader::get_root()
{
    return this->json_root;
}