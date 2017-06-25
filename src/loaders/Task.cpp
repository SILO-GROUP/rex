#include "Task.h"

/// Task_InvalidDataStructure - Exception thrown when a Task is defined with invalid JSON.
class Task_InvalidDataStructure: public std::runtime_error { public:
    Task_InvalidDataStructure(): std::runtime_error("Task: Attempted to access a member of a Task that is not set.") {}
};

/// Task::Task() - Constructor for the Task class.  The Task is the building block of a Plan indicating of which Unit to
/// execute, and its dependencies on other units to have already been completed successfully.
Task::Task() {}

/// Task::load_root() - loads json values to private members
///
/// \param loader_root - the Json::Value to populate from.
/// \param verbose - Whether to print verbose information to STDOUT.
void Task::load_root(Json::Value loader_root, bool verbose )
{
    if ( loader_root.isMember("name") )
    {
        this->name = loader_root.get("name", "?").asString();
    } else {
        throw Task_InvalidDataStructure();
    }

    // fetch as Json::Value array obj
    Json::Value des_dep_root = loader_root.get("dependencies", 0);

    // iterate through each member of that obj
    for ( int i = 0; i < des_dep_root.size(); i++ )
    {
        // add each string to dependencies
        if ( des_dep_root[i].asString() != "" )
        {
            this->dependencies.push_back( des_dep_root[i].asString() );
            if ( verbose ) {
                std::cout << "Added dependency \"" << des_dep_root[i].asString() << "\" to task \"" << this->get_name() << "\"." << std::endl;
            }
        }
    }
}

std::string Task::get_name()
{
    return this->name;
}


