#include "Unit.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>

/// Unit_NotPopulated - Meant to be thrown when a Unit type is not populated before being used.
/// Signaled by use of the 'populated' boolean member of the Unit class.
class Unit_NotPopulated: public std::runtime_error { public:
    Unit_NotPopulated(): std::runtime_error("Unit: Attempted to access a member before loading values.") {}
};

/// Unit_DataStructureException - Meant to be thrown when a Unit type is accessing a member that does not exist.
class Unit_DataStructureException: public std::runtime_error { public:
    // TODO rework this to accept the key name being fetched
    Unit_DataStructureException(): std::runtime_error("Unit: Attempted to access a member not present in defined Unit.") {}
};

/// Unit::Unit - Constructor for Unit type.  The Unit is a definition of an automation task.  Each Unit has:
/// name, used for identification and retrieval.
/// target, which is the filepath of an executable to trigger.
/// output, which is the desired output of the execution of target to STDOUT in determinations of of success or failure
/// (in addition to 0|non-0 exit code). If the output is set to look for "0" then it uses the exit code.
///
/// There is also:
/// rectifier, which is the path to an executable in the event of a non-0 exit code or a failure to get the desired
/// output.
/// required, which is used as a flag to halt or continue if rectifier does not heal the system in such a way that
/// target can run successfully.
/// rectify, which is used as a flag to determine in the rectifier runs.
Unit::Unit() {}

/// Unit::load_root - Takes a JSON::Value and assigns the members to the Unit being populated.
///
/// \param loader_root - The JSON::Value object to use to populate unit from.  Usually supplied as the Suite's buffer
/// member.
/// \return  - Boolean representation of success or failure.
int Unit::load_root(Json::Value loader_root)
{
    // TODO this needs reworked to have errmsg actually end up as a null return from json::value.get()
    std::string errmsg = "SOMETHING WENT TERRIBLY WRONG IN PARSING";

    // TODO this pattern is 'working but broken'.  need to use a datastructure for required members and iterate
    // do NOT replace this with a switch case pattern
    if ( loader_root.isMember("name") )
    { this->name = loader_root.get("name", errmsg).asString(); } else throw Unit_DataStructureException();

    if ( loader_root.isMember("target") )
    { this->target = loader_root.get("target", errmsg).asString(); } else throw Unit_DataStructureException();

    if ( loader_root.isMember("output") )
    { this->output = loader_root.get("output", errmsg).asString(); } else throw Unit_DataStructureException();

    if ( loader_root.isMember("rectifier") )
    { this->rectifier = loader_root.get("rectifier", errmsg).asString(); } else throw Unit_DataStructureException();

    if ( loader_root.isMember("active") )
    { this->active = loader_root.get("active", errmsg).asString(); } else throw Unit_DataStructureException();

    if ( loader_root.isMember("required") )
    { this->required = loader_root.get("required", errmsg).asString(); } else throw Unit_DataStructureException();

    if ( loader_root.isMember("rectify") )
    { this->rectify = loader_root.get("rectify", errmsg).asString(); } else throw Unit_DataStructureException();

    this->populated = true;

    return 0;
}

/// Unit::load_string - populates a Unit object from a supplies JSON-formatted string.  It's stellar.
///
/// \param json_val - JSON-formatted string to populate from.  See Unit::load_root() for details on required structure.
/// \return - The bool representation of success or failure.
int Unit::load_string(std::string json_val)
{
    // serialize
    this->load_json_string( json_val, true );

    // deserialize
    this->load_root( this->json_root );

    return 0;
}

/// Unit::get_name - retrieves the name of the unit.
///
/// \return the name of the unit.
std::string Unit::get_name()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->name;
}

/// Unit::get_target - retrieves the target of the unit.
///
/// \return the target of the unit.
std::string Unit::get_target()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->target;
}

/// Unit::get_output - retrieves the output of the unit.
///
/// \return the output of the unit.
std::string Unit::get_output()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->output;
}

/// Unit::get_rectifier - retrieves the rectifier of the unit.
///
/// \return the rectifier of the unit.
std::string Unit::get_rectifier()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->rectifier;
}

/// Unit::get_active - retrieves the armed status of the unit.
///
/// \return the armed status of the unit.
std::string Unit::get_active()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->active;
}

/// Unit::get_required - retrieves the requirement status of the unit.
///
/// \return the requirement status of the unit.
std::string Unit::get_required()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->required;
}

/// Unit::get_rectify - retrieves the rectification status of the unit.
///
/// \return the rectification status of the unit.
std::string Unit::get_rectify()
{
    if ( ! this->populated ) { throw Unit_NotPopulated(); }
    return this->rectify;
}