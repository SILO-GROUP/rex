#include "Conf.h"

Conf::Conf( std::string filename ): JSON_Loader( filename )
{
    this->plan_path = this->as_serialized()["plan_path"].asString();
    this->units_path = this->as_serialized()["units_path"].asString();
};

std::string Conf::get_plan_path()
{
    return this->plan_path;
}

std::string Conf::get_units_path()
{
    return this->units_path;
}
