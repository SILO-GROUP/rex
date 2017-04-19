#include <iostream>
#include "src/json/json.h"
#include "src/loaders.h"

int main()
{
    Conf jsonLoader = Conf("config.json");
    UnitHolder units = UnitHolder( jsonLoader.get_units_path() );

    return 0;
}