//
// Created by phanes on 4/16/17.
//

#ifndef FTESTS_LOADERS_H
#define FTESTS_LOADERS_H

#include "loaders.h"
#include "json/json.h"
#include "json/json-forwards.h"
#include <iostream>
#include <fstream>

inline bool exists (const std::string& name);

class JLoader
{
    private:
        Json::Value json_root;
    public:
        JLoader( std::string filename );
        Json::Value get_root();
};

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
        Unit select_unit( std::string name );
};

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

#endif //FTESTS_LOADERS_H
