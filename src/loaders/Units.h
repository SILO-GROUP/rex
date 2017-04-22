//
// Created by phanes on 4/22/17.
//

#ifndef FTESTS_UNITS_H
#define FTESTS_UNITS_H

#include <vector>
#include "../json/json.h"
#include "JLoader.h"

class Unit
{
private:
    std::string name;
    std::string target;
    std::string output;
    std::string rectifier;
    std::string active;
    std::string required;
    std::string rectify;

public:
    Unit( Json::Value loader_root );
    std::string get_name();
    std::string get_target();
    std::string get_output();
    std::string get_rectifier();
    std::string get_active();
    std::string get_required();
    std::string get_rectify();
};

class UnitHolder: public JLoader
{
public:
    using JLoader::JLoader;
    std::vector<Unit> units;
    UnitHolder( std::string filename );
    Unit select_unit( std::string provided_name );
};



#endif //FTESTS_UNITS_H
