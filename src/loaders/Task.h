#ifndef FTESTS_TASK_H
#define FTESTS_TASK_H
#include <string>
#include "../json/json.h"
#include "Unit.h"
#include "Suite.h"

class Task
{
    protected:
        // the name of this task
        std::string name;

        // names of tasks that must have successfully executed before this task can execute its own unit.
        std::vector<std::string> dependencies;

        // private member to store the definition of this task once found in a Suite by Plan.
        // populated by load_definition
        Unit definition;

        // the status of this task
        bool complete;

    public:
        // constructor
        Task();

        // load a json::value into task members (second stage deserialization)
        void load_root( Json::Value loader_root, bool verbose );

        // register a dependency
        void add_dependency( std::string dependency_name );

        // appends definition unit as child member
        void load_definition( Unit definition );


        bool is_complete();

        // fetch the name of a task
        std::string get_name();
};

#endif //FTESTS_TASK_H
