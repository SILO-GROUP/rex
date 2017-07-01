#include "Task.h"
#include <unistd.h>
#include <stdio.h>

/// Task_InvalidDataStructure - Exception thrown when a Task is defined with invalid JSON.
class Task_InvalidDataStructure: public std::runtime_error { public:
    Task_InvalidDataStructure(): std::runtime_error("Task: Attempted to access a member of a Task that is not set.") {}
};

/// Task_InvalidDataStructure - Exception thrown when a Task is defined with invalid JSON.
class Task_NotReady: public std::runtime_error { public:
    Task_NotReady(): std::runtime_error("Task: Attempted to access a unit of a Task that is not defined.") {}
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
            if ( verbose )
            {
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

/// Task::execute - execute a task's unit definition.
void Task::execute( bool verbose )
{
    // throw if unit not coupled
    if (! this->has_definition() ) { throw Task_NotReady(); }

    if ( verbose )
    {
        std::cout << "\t Using unit \"" << this->definition.get_name() << "\"." << std::endl;
        std::cout << "\t Executing target \"" << this->definition.get_target() << "\"." << std::endl;
    }

    this->do_raw_command();
}

int Task::do_raw_command( std::string command, std::string & STDOUT, std::string & STDIN );
{

    /*
     * since pipes are unidirectional, we need two pipes. one for data to flow from parent's stdout to child's stdin and
     * the other for child's stdout to flow to parent's stdin
     */

    #define NUM_PIPES          2

    #define PARENT_WRITE_PIPE  0
    #define PARENT_READ_PIPE   1

    int pipes[NUM_PIPES][2];

    /*
     * always in a pipe[], pipe[0] is for read and pipe[1] is for write
     */

    #define READ_FD  0
    #define WRITE_FD 1
    #define PARENT_READ_FD  ( pipes[PARENT_READ_PIPE][READ_FD]   )
    #define PARENT_WRITE_FD ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )

    #define CHILD_READ_FD   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
    #define CHILD_WRITE_FD  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )

    int outfd[2];
    int infd[2];

    // pipes for parent to write and read
    pipe(pipes[PARENT_READ_PIPE]);
    pipe(pipes[PARENT_WRITE_PIPE]);

    if(!fork()) {
        char *argv[]={ "/usr/bin/bc", "-q", 0};

        dup2(CHILD_READ_FD, STDIN_FILENO);
        dup2(CHILD_WRITE_FD, STDOUT_FILENO);

        /* Close fds not required by child. Also, we don't
           want the exec'ed program to know these existed */
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        close(PARENT_READ_FD);
        close(PARENT_WRITE_FD);

        execv(argv[0], argv);
    } else {
        char buffer[100];
        int count;

        /* close fds not required by parent */
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);

        // Write to child’s stdin
        write(PARENT_WRITE_FD, "2^32\n", 5);

        // Read from child’s stdout
        count = read(PARENT_READ_FD, buffer, sizeof(buffer)-1);
        if (count >= 0) {
            buffer[count] = 0;
            printf("%s", buffer);
        } else {
            printf("IO Error\n");
        }
    }
}