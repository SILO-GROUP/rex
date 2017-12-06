/*
    Examplar - An automation and testing framework.

    © SURRO INDUSTRIES and Chris Punches, 2017.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <iostream>
#include "src/json/json.h"
#include "src/loaders/loaders.h"
#include <unistd.h>

#include <syslog.h>

/*
 * TODO Commandline switches
 */

void print_usage()
{
    printf("examplar [ -h ] [ -v ] [ -c CONFIG_PATH ]");
    exit(0);
}

int main( int argc, char * argv[] )
{
    int flags, opt;
    bool verbose = false;
    bool show_help = false;
    std::string config_path = "/etc/Examplar/config.json";

    // commandline switches:
    // -h help
    // -v verbose
    // -c CONFIG_FILE_PATH -- defaults to '/etc/Examplar/config.json'

    while ( ( opt = getopt( argc, argv, "hvc:" ) ) != -1 )
    {
        switch(opt)
        {
            case 'h':
                show_help = true;
            case 'v':
                verbose = true;
                break;
            case 'c':
                config_path = std::string(optarg);
                break;
            default:
                break;
        }
    }

    if ( show_help == true )
    {
        print_usage();
    }

    setlogmask( LOG_UPTO( LOG_INFO ) );

    openlog( "Examplar", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_PERROR | LOG_LOCAL1 );

    // A Plan is made up of Tasks, and a Suite is made up of Units.
    // A Plan declares what units are executed and a Suite declares the definitions of those units.
    Conf configuration = Conf(config_path, verbose );

    // load the configuration file which contains filepaths to definitions of a plan and definitions of units.
    std::string definitions_file = configuration.get_units_path();
    std::string plan_file = configuration.get_plan_path();

    Suite available_definitions;
    available_definitions.load_units_file( definitions_file, verbose );

    Plan plan;
    plan.load_plan_file( plan_file, verbose );

    plan.load_definitions( available_definitions, verbose );

    std::cout << "Ready to execute all tasks in Plan." << std::endl;

    try
    {
        plan.execute( verbose );
    }

    catch ( std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        syslog( LOG_ERR, e.what() );
        closelog();
        return 1;
    }

    closelog();
    return 0;
}