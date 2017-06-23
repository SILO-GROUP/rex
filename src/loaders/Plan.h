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

        void load_plan_file( std::string filename, bool verbose );

        void get_task(Task & result, std::string provided_name, bool verbose);

        void  get_task(Task & result, int index. bool verbose);

        int num_tasks();
};

#endif //FTESTS_PLAN_H
