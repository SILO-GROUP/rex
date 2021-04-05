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

/* Unit.h
 * Unit is a type that represents a safely deserialized JSON object which defines what actions are taken as rex
 * iterates through it's Tasks in it's given Plan.  They only define the behaviour on execution, while the tasks define
 * which Units are executed and in what order (and which Units a given Task depends on.
 */
#ifndef FTEST_UNIT_H
#define FTEST_UNIT_H
#include <string>
#include "../../json/json.h"
#include "../low_level/JSON_Loader.h"
#include "../../Logger/Logger.h"

class Unit: JSON_Loader
{
private:
    // the name of the test
    std::string name;

    // the path of the executable this test executes when run
    std::string target;

    // the desired output
    // poll:  would an empty value be good to indicate to rely solely on zero/non-zero exit code?
    std::string output;

    // the path of the executable this test runs when the target executable fails to produce output or a 0 exit code.
    std::string rectifier;

    // an indicator of whether the test is active or not
    // this is used as a way to give definers a way to force executors to edit arbitrary fields or prevent
    // execution of potentially dangerous or intrusive tests
    bool active;

    // an indicator of whether or not this test is required to pass.
    // intended to be used as a flag to halt execution of further tests on failure
    bool required;

    // indicator of whether the rectifier executable should be run on test failures.
    // if rectifier exits on non-zero return code, it should be trigger the behaviour indicated by required
    bool rectify;

    //indicator of whether stdout should log to file.  used mainly to handle glitchy TUI systems when logs are being tailed.
    bool stdout_log_flag;

    // user to run process as.
    // not intended for protected accounts, handle your own security
    std::string user;

    // group to run process as.
    // not intended for protected accounts, handle your own security
    std::string group;

    // shell to use for env
    std::string shell;

    std::string env_vars_file;

public:
    Unit( int LOG_LEVEL );

    // loads a serialized jason::value object as a unit
    int load_root( Json::Value loader_root );

    // loads a string as a json string and deserializes that.
    int load_string( std::string json_val );

    // getters
    std::string get_name();
    std::string get_target();
    std::string get_output();
    std::string get_rectifier();
    std::string get_env_vars_file();

    bool get_active();
    bool get_required();
    bool get_rectify();
    bool get_stdout_log_flag();
    std::string get_user();
    std::string get_group();
    std::string get_shell();

private:
    int LOG_LEVEL;
    Logger slog;
};

#endif //FTEST_UNIT_H
