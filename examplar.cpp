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
#include <unistd.h>
#include <getopt.h>

#include "src/json/json.h"
#include "src/loaders/abstract/loaders.h"
#include "src/Logger/Logger.h"
#include "src/loaders/misc/helpers.h"

void print_usage()
{
    printf("examplar [ -h | --help ] [ -v | --verbose ] [ -e | --execution-context EXECUTION_CONTEXT ][ -c | --config CONFIG_PATH ]\n\n");
}

int main( int argc, char * argv[] )
{
    int opt;
    bool verbose = false;
    bool show_help = false;

    // indicator of whether examplar should use a commandline argument for overriding the context
    // instead of what's supplied in the test file
    bool cli_context_supplied = false;

    std::string config_path = "/etc/Examplar/config.json";
    std::string execution_context;

    // commandline switches:
    // -h help
    // -v verbose
    // -c CONFIG_FILE_PATH -- defaults to '/etc/Examplar/config.json'
    // -e EXECUTION_CONTEXT -- current working directory when executing unit targets

    while (1)
    {
        static struct option long_options[] =
        {
            {"verbose",             no_argument,            0, 'v'},
            {"help",                no_argument,            0, 'h'},
            {"config",              required_argument,      0, 'c'},
            {"execution-context",   required_argument,      0, 'e'},
            {0, 0}
        };

        int option_index = 0;

        opt = getopt_long( argc, argv, "vhec:", long_options, &option_index );

        if ( opt == -1 )
          break;

        switch ( opt )
        {
            case 0:
            if ( long_options[option_index].flag !=0 )
                break;
            case 'h':
                show_help = true;
            case 'v':
                verbose = true;
                break;
            case 'c':
                config_path = std::string( optarg );
                break;
            case '?':
                print_usage();
                exit( 1 );
            case 'e':
                cli_context_supplied = true;
                execution_context = std::string( optarg );
                break;
            default:
                break;
        }
    }

    if ( show_help )
    {
        print_usage();
        exit( 0 );
    }

    int L_LEVEL = E_INFO;
    if ( verbose )
    {
        L_LEVEL = E_DEBUG;
        std::cout << "Verbosity is DBUG." << std::endl;
    } else {
        L_LEVEL = E_INFO;
        std::cout << "Verbosity is INFO." << std::endl;
    }

    Logger slog = Logger( L_LEVEL, "examplar" );

    // A Plan is made up of Tasks, and a Suite is made up of Units.
    // A Plan declares what units are executed and a Suite declares the definitions of those units.
    Conf configuration = Conf(config_path, L_LEVEL );

    // check if context override

    if ( configuration.has_context_override() )
    {
        // if so, set the CWD.
        chdir( configuration.get_execution_context().c_str() );
        slog.log( E_DEBUG, "Set execution context: " + get_working_path() );
    }

    // if the user set this option as a commandline argument
    if ( cli_context_supplied )
    {
        // override the test file's specified execution context
        configuration.set_execution_context( execution_context );
        slog.log( E_DEBUG, "Set execution context from commandline: " + execution_context );
    }

    // load the filepaths to definitions of a plan and definitions of units.
    std::string definitions_file = configuration.get_units_path();
    std::string plan_file = configuration.get_plan_path();

    Suite available_definitions = Suite( L_LEVEL );
    available_definitions.load_units_file( definitions_file );

    Plan plan = Plan( &configuration, L_LEVEL );
    plan.load_plan_file( plan_file );

    plan.load_definitions( available_definitions );

    slog.log( E_DEBUG, "Ready to execute all tasks in Plan." );

    try
    {
        plan.execute();
    }

    catch ( std::exception& e)
    {
        slog.log( E_FATAL, e.what() );
        return 1;
    }

    return 0;
}