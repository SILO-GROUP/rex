//
// Created by phanes on 4/29/17.
//

#ifndef FTESTS_UNIT_H
#define FTESTS_UNIT_H
#include <string>
#include "../json/json.h"

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

#endif //FTESTS_UNIT_H
