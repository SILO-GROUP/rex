#ifndef FTESTS_UNITS_H
#define FTESTS_UNITS_H

#include <vector>
#include "../json/json.h"
#include "JSON_Loader.h"
#include "Unit.h"

class Suite: public JSON_Loader
{
    private:
        std::vector<Unit> units;

    public:
        // constructor
        Suite( std::string filename );

        int load_file( std::string filename );
        // returns the unit type identified by name or null
        Unit get_unit(std::string provided_name);
};

#endif //FTESTS_UNITS_H
