#include "Suite.h"

Suite::Suite(): JSON_Loader()
{
    // empty
};

void Suite::load_units_file( std::string filename, bool verbose )
{
    // will use json_root staging buffer on each run to append to this->units vector as valid units are found.
    this->load_json_file( filename, verbose );


    // refill the json_root buffer with a json object in the
    if ( this->get_serialized( Json::Value jbuf, "units", verbose ) != 0)
    {
        this->json_root = jbuf;
    }


}

int Suite::load_file(std::string filename): JSON_Loader( filename )
{
    Json::Value raw_units = this->get_serialized("")
    for ( int index = 0; index < raw_units.size(); index++ )
    {
        this->units.push_back( Unit( raw_units[ index ] ) );
    }
    std::cout << raw_units.size() << std::endl;

    return EXIT_SUCCESS;
}

/* Unit Suite::get_unit(std::string provided_name)

 * returns a unit from a unitholder object by name
 * this will need reworked.  maybe should return int, populate a pointer.
 * error handling is the concern here.

{
    Unit * returnable;
    bool foundMatch = false;

    for ( int i = 0; i < this->units.size(); i++ )
    {
        std::string unit_name = this->units[i].get_name();
        if ( unit_name == provided_name )
        {
            returnable = & this->units[i];
            foundMatch = true;
            break;
        }
    }
    if (! foundMatch )
    {
        std::cerr << "Unit name \"" << provided_name << "\" was referenced but not defined!" << std::endl;
        std::exit(1);
    }
    return * returnable;
}
*/



ca