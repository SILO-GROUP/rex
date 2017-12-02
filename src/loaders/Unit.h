/* Unit.h
 * Unit is a type that represents a safely deserialized JSON object which defines what actions are taken as Examplar
 * iterates through it's Tasks in it's given Plan.  They only define the behaviour on execution, while the tasks define
 * which Units are executed and in what order (and which Units a given Task depends on.
 */
#ifndef FTESTS_UNIT_H
#define FTESTS_UNIT_H
#include <string>
#include "../json/json.h"
#include "JSON_Loader.h"

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

public:
    Unit();

    // loads a serialized jason::value object as a unit
    int load_root( Json::Value loader_root );

    // loads a string as a json string and deserializes that.
    int load_string( std::string json_val );

    // getters
    std::string get_name();
    std::string get_target();
    std::string get_output();
    std::string get_rectifier();
    bool get_active();
    bool get_required();
    bool get_rectify();
};

#endif //FTESTS_UNIT_H
