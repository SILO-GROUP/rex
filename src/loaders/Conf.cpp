#include "Conf.h"

Conf::Conf( std::string filename ): JSON_Loader()
{
    // Conf is always loaded from file.
    this->load_json_file( filename, true );

    // always load plan_path, it is required to function.
    this->get_key( this->plan_path, "plan_path", true, false );

    // always load units_path, it is required to function.
    this->get_key( this->units_path, "units_path", true, false );
};

std::string Conf::get_plan_path()
{
    return this->plan_path.asString();
}

std::string Conf::get_units_path()
{
    return this->units_path.asString();
}
