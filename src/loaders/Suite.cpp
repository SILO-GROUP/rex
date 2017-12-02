#include <string.h>
#include "Suite.h"

/// Suite_InvalidUnitMember - Exception thrown when a Suite tries to access a contained Unit's value that is not
/// present in the Unit.
class Suite_InvalidUnitMember: public std::runtime_error { public:
    // TODO rework this to accept the name of the member not able to be fetched.
    Suite_InvalidUnitMember(): std::runtime_error("Suite: Attempted to access a member of a Unit that is not set.") {}
};


/// Suite::Suite() - Constructor for Suite class.  The Suite class is simply a managed container for a Unit vector.
/// Once instantiated, all methods will require either a JSON file or string to be loaded as deserialized Unit types
/// before being called or will simply throw an exception.
///
/// From the high level, a Suite contains the full definitions of all potential Units to execute defined in the Unit
/// definition files that it is loading.  It is meant to be used in such a way that as the application iterates through
/// the Task objects contained by the application Plan, it will iterate through the appplication's Suite, which contains
/// the definition of all available Tasks.  In this manner, defining units and executing units are split into separate
/// human processes to allow modularly developed profiles of test suites.  As inferred, Unit is expected to be one of
/// the two types that are only instantiated once per application run, though it is designed to be used more than once
/// if the implementor so desires.
Suite::Suite(): JSON_Loader() {};

/// Suite::load_units_file - Uses the json_root buffer on each run to append intact Units as they're
/// deserialized from the provided file.
///
/// \param filename - The file to pull the JSON-formatted units from.
/// \param verbose - Whether to print verbose output to STDOUT.
void Suite::load_units_file( std::string filename, bool verbose )
{
    // will use json_root buffer on each run to append to this->units vector as valid units are found.
    this->load_json_file( filename, verbose );

    // staging buffer
    Json::Value jbuff;

    // fill the jbuff staging buffer with a json::value object in the supplied filename
    if ( this->get_serialized( jbuff, "units", verbose ) == 0)
    {
        this->json_root = jbuff;
    }

    // iterate through the json::value members that have been loaded.  append to this->units vector
    // buffer for units to append:
    Unit tmp_U;
    for ( int index = 0; index < this->json_root.size(); index++ )
    {
        // assemble the unit from json_root using the built-in value operator
        tmp_U.load_root( this->json_root[ index ] );
        if ( tmp_U.get_active() ) {
            // append to this->units
            this->units.push_back( tmp_U );
            if ( verbose ) {
                std::cout << "Added unit \"" << tmp_U.get_name() << "\" to Suite." << std::endl;
            }
        }
    }
}

/// Suite::get_unit - returns a contained Unit identified by name attribute.
///
/// \param result - the unit type receiving the unit's value
/// \param provided_name - The name of the unit being fetched.
void Suite::get_unit(Unit & result, std::string provided_name)
{
    bool foundMatch = false;

    for ( int i = 0; i < this->units.size(); i++ )
    {
        std::string unit_name = this->units[i].get_name();
        if ( unit_name == provided_name )
        {
            result = this->units[i];
            foundMatch = true;
            break;
        }
    }

    if (! foundMatch )
    {
        std::cerr << "Unit name \"" << provided_name << "\" was referenced but not defined!" << std::endl;
        throw Suite_InvalidUnitMember();
    }
}

