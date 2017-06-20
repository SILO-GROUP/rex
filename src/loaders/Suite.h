#ifndef FTESTS_UNITS_H
#define FTESTS_UNITS_H

#include <vector>
#include "../json/json.h"
#include "JSON_Loader.h"
#include "Unit.h"

class Suite: public JSON_Loader
{
    private:
        // storage for the definitions we are amassing from the unit definition files
        std::vector<Unit> units;

    public:
        // constructor, empty
        Suite();

        // load a unit definitions file and add valid unit definitions to this->units
        void load_units_file( std::string filename, bool verbose );

        // returns the unit identified by name
        // void get_unit(Unit & result, std::string provided_name);
};

#endif //FTESTS_UNITS_H
