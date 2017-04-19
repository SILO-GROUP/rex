#include <iostream>
#include "src/json/json.h"
#include "src/loaders.h"

int main()
{
    Conf configuration = Conf("config.json");
    UnitHolder unitHolder = UnitHolder( configuration.get_units_path() );
    Plan plan = Plan( configuration.get_plan_path() );


    for ( int i = 0; i < plan.tasks.size(); ++i )
    {
        std::cout <<  unitHolder.select_unit( plan.tasks[i].get_name() ).get_target() << std::endl;
    }



    return 0;
}