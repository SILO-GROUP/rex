/*
    Rex - A unit-based automation, workflow, and testing system.

    © SILO GROUP and Chris Punches, 2017.

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

#include "src/loaders/abstract/loaders.h"
#include "src/Logger/Logger.h"

void version_info()
{
    std::cout << "pre-release alpha" << std::endl;
}

void print_usage()
{
    // commandline switches:
    // -h help                 OPTIONAL
    // -v verbose              OPTIONAL
    // -c CONFIG_FILE          REQUIRED
    // -p PLAN_FILE            REQUIRED

    fprintf( stderr, "\nUsage:\n\trex [ -h | --help ] [ -v | --verbose ] ( ( ( -c | --config ) CONFIG_PATH ) ( -p | plan ) PLAN_PATH ) )\n" );

    fprintf( stderr, "\nOptional Arguments:\n");
    fprintf( stderr, "\t-h | --help\n\t\tThis usage screen.  Mutually exclusive to all other options.\n");
    fprintf( stderr, "\t-v | --verbose\n\t\tSets verbose output.  Generally more than you want to see.\n");
    fprintf( stderr, "\nRequired Arguments:\n");
    fprintf( stderr, "\t-c | --config\n\t\tSupply the directory path for the configuration file.\n");
    fprintf( stderr, "\t-p | --plan\n\t\tSupply the directory path for the plan file to execute.\n\n");
}

int main( int argc, char * argv[] )
{
    // default verbosity setting
    int verbose_flag = false;

    // whether to show usage screen
    int help_flag = false;

    // did the user supply an argument to config
    int config_flag = false;

    // did the user supply an argument to plan
    int plan_flag = false;

    // did the user ask for the version info
    int version_flag = false;

    // default config path
    std::string config_path;

    // default plan path
    std::string plan_path;

    // initialise for commandline argument processing
    int c;
    int digit_optind = 0;

    if ( argc <= 1 )
    {
        help_flag = true;
    }

    // process commandline arguments
    while ( 1 )
    {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;

        // commandline argument structure
        static struct option long_options[] = {
                {"verbose_flag", no_argument,        0,   'v' },
                {"version_info", no_argument,        0,   'i' },
                {"help",         no_argument,        0,      'h' },
                {"config",       required_argument,  0,    'c' },
                {"plan",         required_argument,  0,      'p' },
                {0,0,0,0}
        };

        c = getopt_long(argc, argv, "vihc:p:", long_options, &option_index );
        if ( c == -1 )
        {
            break;
        }

        switch ( c )
        {
            case 'i':
                version_flag = true;
            case 'h':
                help_flag = true;
                break;
            case 'v':
                verbose_flag = true;
                break;
            case 'c':
                config_flag = true;
                config_path = std::string( optarg );
                break;
            case 'p':
                plan_flag = true;
                plan_path = std::string( optarg );
                break;
            case '?':
                help_flag = true;
                break;
            default:
                break;
        } // end switch
    } // end opts while

    if ( version_flag ) {
        version_info();
        exit(0);
    }

    // if the user supplied no config file, there's nothing to do but teach the user how to use this tool
    if (! config_flag ) {
        std::cerr << "NOT SUPPLIED: CONFIG_PATH" << std::endl;
        help_flag = true;
    }

    // if the user supplied no plan file, there's nothing to do but teach the user how to use this tool
    if (! plan_flag ) {
        std::cerr << "NOT SUPPLIED: PLAN_PATH" << std::endl;
        help_flag = true;
    }

    // if the user wants the help screen, just show it and leave
    if ( (help_flag) | (! config_flag) | (! plan_flag) )
    {
        print_usage();
        exit( 0 );
    }

    // default logging level
    int L_LEVEL = E_INFO;

    // if set to verbose_flag mode, output with DEBUG level verbosity
    if ( verbose_flag )
    {
        std::cout << "Setting verbosity level to 'DBUG'..." << std::endl;
        L_LEVEL = E_DEBUG;
    }

    // the main scope logger
    Logger slog = Logger( L_LEVEL, "_main_" );
    slog.log( E_INFO, "* Initialising Logging...");

    // configuration object that reads from config_path
    Conf configuration = Conf( config_path, L_LEVEL );

    // check if context override is set in the config file
    if ( configuration.has_context_override() )
    {
        // if so, set the CWD.
        chdir( configuration.get_execution_context().c_str() );
        slog.log( E_DEBUG, "* Setting execution context: " + get_working_path() );
    }

    // The Rex Paradigm:
    //  - A Suite is made up of Units.
    //  - A Unit is a definition of an executable along with various options surrounding its context and behaviour.
    //  - A Plan is made up of Tasks.
    //  - A Unit becomes a Task when it is added to a Plan.

    // A Plan contains what units are executed and a Suite contains the definitions of those units.
    std::string plan_file = plan_path;

    // load the filepaths to definitions of a plan and definitions of units.
    std::string definitions_file = configuration.get_units_path();

    // initialise an empty suite (unit definitions library)
    slog.log( E_DEBUG, "* Initialising Suite...");
    Suite available_definitions = Suite( L_LEVEL );

    // load units into suite
    slog.log( E_INFO, "* Loading all actionable Units into Suite..." );
    available_definitions.load_units_file( definitions_file );

    // initialise an empty plan
    slog.log( E_DEBUG, "* Initialising Plan..." );
    Plan plan = Plan( &configuration, L_LEVEL );

    // load the plan the user supplied
    slog.log( E_INFO, "* Loading Plan...");
    plan.load_plan_file( plan_file );

    // ingest the suitable Tasks from the Suite into the Plan
    slog.log( E_INFO, "* Loading planned Tasks from Suite to Plan." );
    plan.load_definitions( available_definitions );

    slog.log( E_INFO, "* Ready to execute all actionable Tasks in Plan." );

    try
    {
        plan.execute();
    }

    catch ( std::exception& e)
    {
        slog.log( E_FATAL, "Caught exception.");
        slog.log( E_FATAL, e.what() );
        return 1;
    }

    return 0;
}