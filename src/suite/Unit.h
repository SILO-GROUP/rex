/*
    Rex - A configuration management and workflow automation tool that
    compiles and runs in minimal environments.
    © SILO GROUP and Chris Punches, 2020.
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


#ifndef REX_UNIT_H
#define REX_UNIT_H

#include <string>
#include "../json_support/JSON.h"
#include "../logger/Logger.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <pwd.h>
#include <grp.h>


/*
 * Unit is a type that represents a safely deserialized JSON object which defines what actions are taken as rex
 * iterates through it's Tasks in it's given Plan.  They only define the behaviour on execution, while the tasks define
 * which Units are executed and in what order (and which Units a given Task depends on.
 */
class Unit: JSON_Loader
{
    private:
        // the name of the test
        std::string name;

        // the path of the executable this test executes when run
        std::string target;

        // bool to indicate if the target is a shell command or not
        bool is_shell_command;

        // the shell profile to use defined in the shell definitions file
        std::string shell_definition;

        // bool to indicate if the target should be run in a pty
        bool force_pty;

        // bool to indicate if the working directory should be set for this execution
        bool set_working_directory;

        // the working directory to set for this execution
        std::string working_directory;

        // indicator of whether the rectifier executable should be run on test failures.
        // if rectifier exits on non-zero return code, it should be trigger the behaviour indicated by required
        bool rectify;

        // the path of the executable this test runs when the target executable fails to produce output or a 0 exit code.
        std::string rectifier;

        // an indicator of whether the test is active or not
        // this is used as a way to give definers a way to force executors to edit arbitrary fields or prevent
        // execution of potentially dangerous or intrusive tests
        bool active;

        // an indicator of whether or not this test is required to pass.
        // intended to be used as a flag to halt execution of further tests on failure
        bool required;

        // an indicator of whether or not the user context should be set for this execution
        bool set_user_context;

        // user to run process as.
        // not intended for protected accounts, handle your own security
        std::string user;

        // group to run process as.
        // not intended for protected accounts, handle your own security
        std::string group;

        // an indicator of whether or not the environment should be set for this execution
        // REQUIRES this->is_shell_command == true
        bool supply_environment;

        // the path to a file containing environment variables or functions to be set for this execution
        std::string env_vars_file;

    public:
        Unit( int LOG_LEVEL );

        // loads a serialized jason::value object as a unit
        int load_root( Json::Value loader_root );

        // getters
        std::string get_name();
        std::string get_target();
        bool get_is_shell_command();
        std::string get_shell_definition();
        bool get_force_pty();
        bool get_set_working_directory();
        std::string get_working_directory();
        bool get_rectify();
        std::string get_rectifier();
        bool get_active();
        bool get_required();
        bool get_set_user_context();
        std::string get_user();
        std::string get_group();
        bool get_supply_environment();
        std::string get_environment_file();

    private:
        int LOG_LEVEL;
        Logger slog;
};

#endif //REX_UNIT_H