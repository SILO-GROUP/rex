//
// Created by phanes on 4/30/17.
//

#ifndef FTESTS_TASK_H
#define FTESTS_TASK_H
#include <string>
#include "../json/json.h"

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

#endif //FTESTS_TASK_H
