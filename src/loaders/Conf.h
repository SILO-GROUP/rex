//
// Created by phanes on 4/22/17.
//

#ifndef FTESTS_CONF_H
#define FTESTS_CONF_H
#include "JLoader.h"

class Conf: public JLoader
{
private:
    std::string plan_path;
    std::string units_path;

public:
    using JLoader::JLoader;
    Conf( std::string filename );
    std::string get_plan_path();
    std::string get_units_path();
};

#endif //FTESTS_CONF_H
