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
        std::string current_task_name = plan.tasks[i].get_name();
        std::cout << "Found task name in \"" << configuration.get_plan_path() << "\":\t" << current_task_name << std::endl << std::endl;

        Unit current_unit = unitHolder.select_unit( current_task_name );

        std::cout << "Associated Unit name:\t\t" << current_unit.get_name() << std::endl;
        std::cout << "Associated Unit target:\t\t" << current_unit.get_target() << std::endl;
        std::cout << "Associated Unit healer:\t\t" << current_unit.get_rectifier() << std::endl;
        std::cout << "Associated Unit heals:\t\t" << current_unit.get_rectify() << std::endl << std::endl;
    }

    return 0;
}