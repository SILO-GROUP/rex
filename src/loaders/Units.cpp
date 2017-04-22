//
// Created by phanes on 4/22/17.
//

#include "Units.h"
Unit::Unit( Json::Value loader_root )
{
    this->name      = loader_root.get("name", "?").asString();
    this->target    = loader_root.get("target", "?").asString();
    this->output    = loader_root.get("output", "?").asString();
    this->rectifier = loader_root.get("rectifier", "?").asString();
    this->active    = loader_root.get("active", "?").asString();
    this->required  = loader_root.get("required", "?").asString();
    this->rectify   = loader_root.get("rectify", "?").asString();
}


std::string Unit::get_name()        { return this->name;        }
std::string Unit::get_target()      { return this->target;      }
std::string Unit::get_output()      { return this->output;      }
std::string Unit::get_rectifier()   { return this->rectifier;   }
std::string Unit::get_active()      { return this->active;      }
std::string Unit::get_required()    { return this->required;    }
std::string Unit::get_rectify()     { return this->rectify;     }

UnitHolder::UnitHolder( std::string filename ): JLoader( filename )
{
/*  UnitHolder loads a file and deserializes the Unit JSON object to Unit types as a vector member
 *  UnitHolder { vector<Unit> }
 */
    Json::Value raw_units = this->get_root()["units"];

    for ( int index = 0; index < raw_units.size(); index++ )
    {
        this->units.push_back(Unit( raw_units[index] ));
    }
};

Unit UnitHolder::select_unit(std::string provided_name)
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
        std::cerr << "Unit name \"" << provided_name << "\" was referenced in a plan task but not defined!" << std::endl;
        std::exit(1);
    }

    return * returnable;
}
