#ifndef FTESTS_PLAN_H
#define FTESTS_PLAN_H

#include <string>
#include "../json/json.h"
#include "JLoader.h"
#include "Task.h"


class Plan: public JLoader
{
    private:
        std::vector<Task> tasks;

    public:
        Plan( std::string filename );
        Task get_task(std::string provided_name);
        Task get_task(int index);
        int num_tasks();
};

#endif //FTESTS_PLAN_H
