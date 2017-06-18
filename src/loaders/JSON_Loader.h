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
        bool populated;

    public:
        // constructor
        JSON_Loader();

        // load from json file
        void load_json_file( std::string filename, bool verbose );

        // load from std::string json
        void load_json_string( std::string input, bool verbose );

        // return as a JSONCPP serialized object
        // deprecated -- these aren't really used.
        // Json::Value as_serialized();
        // std::string as_string();

        // safely handle deserialized type retrieval (if we want it to be safe)
        int get_serialized(Json::Value &input, std::string key, bool verbose);
};
#endif //FTESTS_JLOADER_H
