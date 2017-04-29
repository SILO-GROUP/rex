//
// Created by phanes on 4/22/17.
//

#ifndef FTESTS_PLAN_H
#define FTESTS_PLAN_H
#include <string>
#include "../json/json.h"
#include "JLoader.h"

class Task
{
    private:
        std::string name;
        Json::Value dependencies;
        bool has_succeeded;

    public:
        Task( Json::Value loader_root );
        std::string get_name();
        Json::Value get_dependencies();
        Json::Value set_dependencies();
        bool isDone();
        void finish();
};

class Plan: public JLoader
{
    private:
        std::vector<Task> tasks;

    public:
        Plan( std::string filename );
        Task select_task( std::string provided_name );
        Task select_task_index( int index );
        int num_tasks();
};

#endif //FTESTS_PLAN_H
