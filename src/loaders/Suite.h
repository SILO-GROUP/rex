//
// Created by phanes on 4/22/17.
//

#ifndef FTESTS_UNITS_H
#define FTESTS_UNITS_H

#include <vector>
#include "../json/json.h"
#include "JLoader.h"
#include "Unit.h"


class Suite: public JLoader
{
    private:
        std::vector<Unit> units;

    public:
        // constructor
        Suite( std::string filename );

        // returns the unit type identified by name or null
        Unit get_unit(std::string provided_name);
};



#endif //FTESTS_UNITS_H
