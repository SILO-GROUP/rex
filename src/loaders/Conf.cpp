#include "Conf.h"

Conf::Conf( std::string filename ): JSON_Loader()
{
    this->load_json_file( filename, true );

    this->plan_path = this->get_key("plan_path", true, false).asString();
    this->units_path = this->get_key("units_path", true, false).asString();
};

std::string Conf::get_plan_path()
{
    return this->plan_path;
}

std::string Conf::get_units_path()
{
    return this->units_path;
}
