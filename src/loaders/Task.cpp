#include "Task.h"

/// Task_InvalidDataStructure - Exception thrown when a Task is defined with invalid JSON.
class Task_InvalidDataStructure: public std::runtime_error { public:
    Task_InvalidDataStructure(): std::runtime_error("Task: Attempted to access a member of a Task that is not set.") {}
};

/// Task::Task() - Constructor for the Task class.  The Task is the building block of a Plan indicating of which Unit to
/// execute, and its dependencies on other units to have already been completed successfully.
Task::Task() {
    // it hasn't executed yet.
    this->complete = false;

    // it hasn't been matched with a definition yet.
    this->defined = false;
}

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

/// Task::get_name - Retrieves the name of the current Task.
std::string Task::get_name()
{
    return this->name;
}

/// Task::load_definition - Loads a unit to a local member.  Used to tie Units to Tasks.
///
/// \param selected_unit - The unit to attach.
/// \param verbose - Whether to print to STDOUT.
void Task::load_definition( Unit selected_unit, bool verbose )
{
    this->definition = selected_unit;
    if ( verbose )
    {
        std::cout << "Loaded definition \"" << selected_unit.get_name() << "\" for task \"" << this->get_name() << "\"." << std::endl;
    }
    this->defined = true;
}

/// Task::is_complete - Indicator if the task executed successfully.
bool Task::is_complete() {
    return this->complete;
}

/// Task::has_definition - Indicator if the task has attached its definition from a Suite.
bool Task::has_definition() {
    return this->defined;
}