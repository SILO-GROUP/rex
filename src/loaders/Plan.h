#ifndef FTESTS_PLAN_H
#define FTESTS_PLAN_H

#include <string>
#include "../json/json.h"
#include "JSON_Loader.h"
#include "Task.h"

class Plan: public JSON_Loader
{
    private:
        // storage for the tasks that make up the plan
        std::vector<Task> tasks;

    public:
        Plan();

        // append this->tasks from JSON file
        void load_plan_file( std::string filename, bool verbose );

        // fetch a task from this->tasks
        void get_task( Task & result, std::string provided_name );

        // fetch a task from this->tasks
        void get_task( Task & result, int index );

        // load unit definitions from a provided suite and import them into individual tasks
        void load_definitions( Suite unit_definitions, bool verbose );

        // fetch a corresponding Unit to a Task
        // void get_definition_from_task(Unit & result, Task input, bool verbose );

        // execute all tasks in this plan
        void execute( bool verbose );

        bool all_dependencies_complete(std::string name);
};

#endif //FTESTS_PLAN_H
