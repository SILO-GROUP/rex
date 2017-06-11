#include "Conf.h"

class CONF_PLANPATH_INVALID: public std::runtime_error
{
public:
    CONF_PLANPATH_INVALID(): std::runtime_error("conf: The supplied path for the plan definition file is invalid.") {}
};

class CONF_UNITSPATH_INVALID: public std::runtime_error
{
public:
    CONF_UNITSPATH_INVALID(): std::runtime_error("conf: The supplied path for the unit definition file is invalid.") {}
};


Conf::Conf( std::string filename ): JSON_Loader()
{
    // load the conf file.
    this->load_json_file( filename, true );

    // find the path to the plan file
    if ( this->get_key( this->plan_path, "plan_path", true) != 0 ) { throw CONF_PLANPATH_INVALID(); }

    // find the path to the unit definitions file
    if ( this->get_key( this->units_path, "units_path", true) != 0 ) { throw CONF_UNITSPATH_INVALID(); }
};

std::string Conf::get_plan_path()
{
    return this->plan_path.asString();
}

std::string Conf::get_units_path()
{
    return this->units_path.asString();
}
