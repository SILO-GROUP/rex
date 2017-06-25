#include "Task.h"

/// Task_InvalidDataStructure - Exception thrown when a Task is defined with invalid JSON.
class Task_InvalidDataStructure: public std::runtime_error { public:
    Task_InvalidDataStructure(): std::runtime_error("Task: Attempted to access a member of a Task that is not set.") {}
};

/// Task::Task() - Constructor for the Task class.  The Task is the building block of a Plan indicating of which Unit to
/// execute, and its dependencies on other units to have already been completed successfully.
Task::Task() {}

int Task::load_root(Json::Value loader_root)
{
    if ( loader_root.isMember("name") )
    {
        this->name = loader_root.get("name", "?").asString();
    } else {
        throw Task_InvalidDataStructure();
    }


    // this->dependencies = loader_root.get("depends on", "");
    //this->has_succeeded = false;

}

std::string Task::get_name()
{
    return this->name;
}


