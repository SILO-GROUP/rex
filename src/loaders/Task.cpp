#include "Task.h"


Task::Task() {}

Task::Task( Json::Value loader_root )
{
    this->load_root( loader_root );
}

int Task::load_root(Json::Value loader_root)
{
    this->name = loader_root.get("name", "?").asString();
    this->dependencies = loader_root.get("depends on", "");
    this->has_succeeded = false;
}


std::string Task::get_name()
{
    return this->name;
}

bool Task::isDone()
{
    return this->has_succeeded;
}

void Task::finish()
{
    this->has_succeeded = true;
}

// returns Json::Value for dependencies
Json::Value Task::get_dependencies()
{
    return this->dependencies;
}

Json::Value Task::set_dependencies()
{

}