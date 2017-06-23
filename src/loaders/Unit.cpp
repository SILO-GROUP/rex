#include "Unit.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>

/// Unit_NotPopulated - Meant to be thrown when a Unit type is not populated before being used.
/// Signaled by use of the 'populated' boolean member of the Unit class.
class Unit_NotPopulated: public std::runtime_error
{
public:
    Unit_NotPopulated(): std::runtime_error("Unit: Attempted to access a member before loading values.") {}
};

Unit::Unit() {}

// where the serialized json is broken down into object members
int Unit::load_root(Json::Value loader_root)
{
// this needs reworked to throw an Exception if any of the values aren't loadable.
    this->name      = loader_root.get("name", "").asString();

    this->target    = loader_root.get("target", "").asString();

    this->output    = loader_root.get("output", "").asString();

    this->rectifier = loader_root.get("rectifier", "").asString();

    this->active    = loader_root.get("active", "").asString();

    this->required  = loader_root.get("required", "").asString();

    this->rectify   = loader_root.get("rectify", "").asString();

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

    return 0;
}

// getters for Unit type.
std::string Unit::get_name()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->name;
}

std::string Unit::get_target()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->target;
}

std::string Unit::get_output()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->output;
}

std::string Unit::get_rectifier()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->rectifier;
}

std::string Unit::get_active()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->active;
}

std::string Unit::get_required()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->required;
}

std::string Unit::get_rectify()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->rectify;
}