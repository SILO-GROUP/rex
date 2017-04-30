#include <iostream>
#include "src/json/json.h"
#include "src/loaders/loaders.h"

int main()
{
    // A Plan is made up of Tasks, and a Suite is made up of Units.
    // A Plan declares what units are executed and a Suite declares the definitions of those units.
    Conf configuration = Conf("config.json");

    // load the configuration file which contains filepaths to definitions of a plan and definitions of units.
    std::string definitions_file = configuration.get_units_path();
    std::string plan_file = configuration.get_plan_path();

    Suite unit_definitions = Suite( definitions_file );

    Plan plan = Plan( plan_file );

    for ( int i = 0; i < plan.num_tasks(); ++i )
    {
        Task current_task = plan.get_task(i);

        Unit current_unit = unit_definitions.get_unit(current_task.get_name());

        std::cout << "Found task name in \"" << configuration.get_plan_path() << "\":\t" << current_task.get_name() << std::endl << std::endl;

        std::cout << "Associated Unit name:\t\t"    << current_unit.get_name()      << std::endl;
        std::cout << "Associated Unit target:\t\t"  << current_unit.get_target()    << std::endl;
        std::cout << "Associated Unit healer:\t\t"  << current_unit.get_rectifier() << std::endl;
        std::cout << "Associated Unit heals:\t\t"   << current_unit.get_rectify()   << std::endl;

        Json::Value deps = current_task.get_dependencies();

        for ( int j = 0; j < deps.size(); ++j )
        {
            std::cout << "Associated Dependency:\t\t" << deps[j] << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}