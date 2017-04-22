//
// Created by phanes on 4/22/17.
//

#ifndef FTESTS_JLOADER_H
#define FTESTS_JLOADER_H
#include "../json/json.h"
#include <iostream>
#include <fstream>
#include <cstdlib>


class JLoader
{
private:
    Json::Value json_root;
public:
    JLoader( std::string filename );
    Json::Value get_root();
};
#endif //FTESTS_JLOADER_H
