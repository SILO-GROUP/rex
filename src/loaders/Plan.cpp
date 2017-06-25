#include "Plan.h"
/// Plan_InvalidTaskIndex - Exception thrown when a Plan tries to access a contained Task's value by index not present
/// in the Unit.
class Plan_InvalidTaskIndex: public std::runtime_error { public:
    Plan_InvalidTaskIndex(): std::runtime_error("Plan: Attempted to access a Task using an invalid index.") {}
};

/// Plan_InvalidTaskName - Exception thrown when a Plan tries to access a contained Task's value by name not present
/// in the Unit.
class Plan_InvalidTaskName: public std::runtime_error { public:
    Plan_InvalidTaskName(): std::runtime_error("Plan: Attempted to access a Task using an invalid name.") {}
};

/// Plan::Plan() - Constructor for Plan class.  A Plan is a managed container for a Task vector.  These tasks reference
/// Units that are defined in the Units files (Suite).  If Units are definitions, Tasks are selections of those
/// definitions to execute, and if Units together form a Suite, Tasks together form a Plan.
Plan::Plan(): JSON_Loader() {};

/// Plan::load_plan_file - Uses the json_root buffer on each run to append intact Units as they're deserialized from
/// the provided file.
///
/// \param filename - The filename to load the plan from.
/// \param verbose  - Whether to print verbose output to STDOUT.
void Plan::load_plan_file(std::string filename, bool verbose)
{
    // plan always loads from file
    this->load_json_file( filename, verbose );

    // staging buffer
    Json::Value jbuff;

    // fill the jbuff staging buffer wih a json::value object in the supplied filename
    if ( this->get_serialized( jbuff, "plan", verbose ) == 0 )
    {
        this->json_root = jbuff;
    }

    // iterate through the json::value members that have been loaded.  append to this->tasks vector
    // buffer for tasks to append:
    Task tmp_T;
    for ( int index = 0; index < this->json_root.size(); index++ )
    {
        tmp_T.load_root( this->json_root[ index ], verbose );
        this->tasks.push_back( tmp_T );
        if ( verbose ) {
            std::cout << "Added task \"" << tmp_T.get_name() << "\" to Plan." << std::endl;
        }
    }
}

/// Plan::get_task - Retrieves a task by index.
///
/// \param result - The variable receiving the value.
/// \param index  - The numerical index in the Task vector to retrieve a value for.
/// \param verbose  - Whether to print verbose output to STDOUT.
void Plan::get_task(Task & result, int index, bool verbose)
{
    if ( index <= this->tasks.size() )
    {
        result = this->tasks[ index ];
    } else {
        throw Plan_InvalidTaskIndex();
    }
}

/// Plan::get_task - Retrieves a task by name.
///
/// \param result - The variable receiving the value.
/// \param provided_name - The name to find a task by.
/// \param verbose - Whether to print verbose output to STDOUT.
void Plan::get_task(Task & result, std::string provided_name, bool verbose)
{
        bool foundMatch = false;

        for ( int i = 0; i < this->tasks.size(); i++ )
        {
            if ( this->tasks[i].get_name() == provided_name )
            {
                result = this->tasks[i];
                foundMatch = true;
                break;
            }
        }
        if (! foundMatch )
        {
            std::cerr << "Task name \"" << provided_name << "\" was referenced but not defined!" << std::endl;
            throw Plan_InvalidTaskName();
        }
}