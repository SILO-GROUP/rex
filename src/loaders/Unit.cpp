#include "Unit.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

Unit::Unit() {}

// where the serialized json is broken down into object members
int Unit::load_root(Json::Value loader_root)
{

    this->name      = loader_root.get("name", "?").asString();

    this->target    = loader_root.get("target", "?").asString();

    this->output    = loader_root.get("output", "?").asString();

    this->rectifier = loader_root.get("rectifier", "?").asString();

    this->active    = loader_root.get("active", "?").asString();

    this->required  = loader_root.get("required", "?").asString();

    this->rectify   = loader_root.get("rectify", "?").asString();

    this->populated = true;

    return 0;
}

// populates from a string json object
int Unit::load_string(std::string json_val)
{
    // serialize
    this->load_json_string( json_val, true );

    // deserialize
    this->load_root( this->json_root );

    // exit successfully
    this->populated = true;

    return 0;
}

// getters for Unit type.
std::string Unit::get_name()
{
    return this->name;
}

std::string Unit::get_target()
{
    return this->target;
}

std::string Unit::get_output()
{
    return this->output;
}

std::string Unit::get_rectifier()
{
    return this->rectifier;
}

std::string Unit::get_active()
{
    return this->active;
}

std::string Unit::get_required()
{
    return this->required;
}

std::string Unit::get_rectify()
{
    return this->rectify;
}