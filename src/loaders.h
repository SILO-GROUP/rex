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

class Unit: public JLoader
{
    private:
        Json::Value json_root;
    public:
        using JLoader::JLoader;
        Unit( std::string filename );
};

class Plan: public JLoader
{
    private:
        Json::Value json_root;
    public:
        using JLoader::JLoader;
        Plan( std::string filename );
};


class Conf: public JLoader
{
    private:
        Json::Value json_root;
    public:
        using JLoader::JLoader;
        Conf( std::string filename );
};


#endif //FTESTS_LOADERS_H
