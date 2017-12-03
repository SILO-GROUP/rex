#include "Task.h"
#include <unistd.h>
#include <stdio.h>
#include "../sproc/Sproc.h"

/// Task_InvalidDataStructure - Exception thrown when a Task is defined with invalid JSON.
class Task_InvalidDataStructure: public std::runtime_error {
public:
    Task_InvalidDataStructure(): std::runtime_error("Task: Attempted to access a member of a Task that is not set.") {}
};

/// Task_InvalidDataStructure - Exception thrown when a Task is defined with invalid JSON.
class Task_NotReady: public std::runtime_error {
public:
    Task_NotReady(): std::runtime_error("Task: Attempted to access a unit of a Task that is not defined.") {}
};

/// Task_RequiredButFailedTask - Exception thrown when a Task is failed but required, and rectification also failed.
class Task_RequiredButFailedTask: public std::runtime_error {
public:
    Task_RequiredButFailedTask(): std::runtime_error("Task: Attempted to execute a Task that failed and was required.") {}
};

/// Task_RequiredButFailedTask - Exception thrown when a Task is failed but required, and rectification also failed but returned with a zero exit code (dont try to fool the check).
class Task_RequiredButRectifierDoesNotHeal: public std::runtime_error {
public:
    Task_RequiredButRectifierDoesNotHeal(): std::runtime_error("Task: The rectification script was executed and reported success, but did not actually heal the faulty condition of the Task target.") {}
};

/// Task::Task() - Constructor for the Task class.  The Task is the building block of a Plan indicating of which Unit to
/// execute, and its dependencies on other units to have already been completed successfully.
Task::Task()
{
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
    if ( loader_root.isMember("name") ) {
        this->name = loader_root.get("name", "?").asString();
    }
    else {
        throw Task_InvalidDataStructure();
    }

    // fetch as Json::Value array obj
    Json::Value des_dep_root = loader_root.get("dependencies", 0);

    // iterate through each member of that obj
    for ( int i = 0; i < des_dep_root.size(); i++ ) {
        // add each string to dependencies
        if ( des_dep_root[i].asString() != "" ) {
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
    if ( verbose ) {
        std::cout << "Loaded definition \"" << selected_unit.get_name() << "\" for task \"" << this->get_name() << "\"." << std::endl;
    }
    this->defined = true;
}

/// Task::is_complete - Indicator if the task executed successfully.
bool Task::is_complete()
{
    return this->complete;
}


/// Task::mark_complete - Marks the task complete..
void Task::mark_complete()
{
    this->complete = true;
}


/// Task::get_dependencies - returns a pointer to the dependencies vector.
std::vector<std::string> Task::get_dependencies()
{
    return this->dependencies;
}

/// Task::has_definition - Indicator if the task has attached its definition from a Suite.
bool Task::has_definition()
{
    return this->defined;
}

/// Task::execute - execute a task's unit definition.
/// See the design document for what flow control needs to look like here.
/// \param verbose - Verbosity level - not implemented yet.
void Task::execute( bool verbose )
{
    // DUFFING - If Examplar is broken it's probably going to be in this block.

    // PREWORK
    // throw if unit not coupled to all necessary values since Task is stateful (yes, stateful is okay)
    if (! this->has_definition() ) {
        throw Task_NotReady();
    }

    // get the name
    std::string task_name = this->definition.get_name();
    // END PREWORK


    // get the target execution command
    std::string target_command = this->definition.get_target();

    // if we're in verbose mode, do some verbose things
    if ( verbose ) {
        std::cout << "\tUsing unit \"" << task_name << "\"." << std::endl;
        std::cout << "\tExecuting target \"" << target_command << "\"." << std::endl;
    }

    // execute target
    int return_code = Sproc::execute( target_command );

    // d[0] check exit code of target
    if (return_code == 0) {
        // Zero d[0] return from target execution, good to return
        if ( verbose ) {
            std::cout << "\tTarget " << task_name << " succeeded." << std::endl;
        }
        this->mark_complete();
        // next
    } else {
        // Non-Zero d[0] from initial target execution, get to d[1]
        std::cout << "\tTarget \"" << task_name << "\" failed with exit code " << return_code << "." << std::endl;

        // check if rectify pattern is enabled d[1]
        if ( this->definition.get_rectify() ) {
            // yes d[1]
            std::cout << "\tRectification pattern is enabled for \"" << task_name << "\"." << std::endl;
            // execute RECTIFIER
            std::string rectifier_command = this->definition.get_rectifier();
            std::cout << "\tExecuting rectification: " << rectifier_command << "." << std::endl;
            int rectifier_error = Sproc::execute( rectifier_command );

            // d[3] check exit code of rectifier
            if (rectifier_error) {
                //d[3] non-zero

                std::cout << "\tRectification of \"" << task_name << "\" failed with exit code " << rectifier_error << "." << std::endl;
                // d[2] check if REQUIRED
                if ( this->definition.get_required() ) {
                    // d[2] yes
                    // halt/exception
                    throw Task_RequiredButFailedTask();
                }
                // d[2] no
                // next
            }
            // d[3] zero

            // execute target
            std::cout << "\tRe-Executing target \"" << this->definition.get_target() << "\"." << std::endl;
            int retry_code = Sproc::execute( target_command );

            // d[4] exit code of target retry
            if (retry_code == 0) {
                // d[4] zero
            }
            // d[4] non-zero
            // d[5] required check
            if ( this->definition.get_required() ) {
                // d[5] yes
                std::cout << "\tTask \"" << task_name << "\" is required but rectification did not heal." << std::endl;
                throw Task_RequiredButRectifierDoesNotHeal();
            }
            // d[5] no
            // next
        }
        // no d[1]
        std::cout << "\tRectification is not enabled for \"" << task_name << "\"." << std::endl;
        // required d[2]
        if ( this->definition.get_required() ) {
            // d[2] yes
            // This is executing.....
            std::cout << "\tThis task is required to continue the plan." << std::endl;
            // but these are NOT executing?????
            throw Task_RequiredButFailedTask();
        }             // d[2] no
        std::cout << "\tThis task is not required to continue the plan." << std::endl;
    }
}