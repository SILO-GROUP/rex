//
// Created by phanes on 4/22/17.
//

#ifndef FTESTS_PLAN_H
#define FTESTS_PLAN_H
#include <string>
#include "../json/json.h"
#include "JLoader.h"

class Task
{
private:
    std::string name;
    Json::Value dependencies;

public:
    Task( Json::Value loader_root );
    std::string get_name();
    Json::Value get_dependencies();
};

class Plan: public JLoader
{
public:
    using JLoader::JLoader;
    std::vector<Task> tasks;
    Plan( std::string filename );
};



#endif //FTESTS_PLAN_H
