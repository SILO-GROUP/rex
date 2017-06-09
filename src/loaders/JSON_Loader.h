//
// Created by phanes on 4/22/17.
//

#ifndef FTESTS_JLOADER_H
#define FTESTS_JLOADER_H
#include "../json/json.h"
#include <iostream>
#include <fstream>
#include <cstdlib>


class JSON_Loader
{
private:
    Json::Value json_root;
public:
    // constructor
    JSON_Loader();

    // load from json file
    int load_json_file( std::string filename, bool verbose );

    // load from std::string json
    int load_json_string( std::string input, bool verbose );

    // return as a JSONCPP serialized object
    Json::Value as_serialized();
    std::string as_string();
};
#endif //FTESTS_JLOADER_H
