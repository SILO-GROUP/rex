//
// Created by phanes on 4/22/17.
//

#ifndef FTESTS_CONF_H
#define FTESTS_CONF_H
#include "JSON_Loader.h"

class Conf: public JSON_Loader
{
private:
    Json::Value plan_path;
    Json::Value units_path;

public:
    Conf( std::string filename );
    std::string get_plan_path();
    std::string get_units_path();
};

#endif //FTESTS_CONF_H
