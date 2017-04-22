//
// Created by phanes on 4/22/17.
//

#include "Conf.h"
#include "JLoader.h"
#include <string>


Conf::Conf( std::string filename ): JLoader( filename )
{
    this->plan_path = this->get_root()["plan_path"].asString();
    this->units_path = this->get_root()["units_path"].asString();
};

std::string Conf::get_plan_path()
{
    return this->plan_path;
}

std::string Conf::get_units_path()
{
    return this->units_path;
}
