#include <iostream>
#include "src/json/json.h"
#include "src/loaders/loaders.h"

int main( )
{
    bool verbose = true;
    // A Plan is made up of Tasks, and a Suite is made up of Units.
    // A Plan declares what units are executed and a Suite declares the definitions of those units.
    Conf configuration = Conf("/home/phanes/development/internal/Examplar/conf/config.json", verbose );

    // load the configuration file which contains filepaths to definitions of a plan and definitions of units.
    std::string definitions_file = configuration.get_units_path();
    std::string plan_file = configuration.get_plan_path();

    Suite available_definitions;
    available_definitions.load_units_file( definitions_file, verbose );

    Plan plan;
    plan.load_plan_file( plan_file, verbose );

    plan.load_definitions( available_definitions, verbose );

    std::cout << "Ready to execute all tasks in Plan." << std::endl;

    try {
        plan.execute( verbose );
    }
    catch ( std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}