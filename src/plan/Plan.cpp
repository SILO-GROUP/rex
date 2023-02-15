/*
    Rex - A configuration management and workflow automation tool that
    compiles and runs in minimal environments.

    © SILO GROUP and Chris Punches, 2020.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
#include "Plan.h"


/**
 * @class Plan_InvalidTaskIndex
 * @brief Exception thrown when a Plan tries to access a contained Task's value by index not present in the Unit.
 *
 * This class is derived from std::runtime_error and is used to indicate that a Plan has tried to access a Task's value by index that is not present in the Unit.
 * The constructor creates an error message with the description "Plan: Attempted to access a Task using an invalid index."
 */
class Plan_InvalidTaskIndex : public std::runtime_error {
    public:
        /**
         * @brief Constructs a Plan_InvalidTaskIndex object with a default error message.
         *
         * The default error message is "Plan: Attempted to access a Task using an invalid index."
         */
        Plan_InvalidTaskIndex() : std::runtime_error("Plan: Attempted to access a Task using an invalid index.") {}
};


/**
 * @class Plan_InvalidTaskName
 * @brief Exception thrown when a Plan tries to access a contained Task's value by name not present in the Unit.
 *
 * This class is derived from std::runtime_error and is used to indicate that a Plan has tried to access a Task's value by name that is not present in the Unit.
 * The constructor creates an error message with the description "Plan: Attempted to access a Task using an invalid name."
 */
class Plan_InvalidTaskName : public std::runtime_error {
    public:
        /**
         * @brief Constructs a Plan_InvalidTaskName object with a default error message.
         *
         * The default error message is "Plan: Attempted to access a Task using an invalid name."
         */
        Plan_InvalidTaskName() : std::runtime_error("Plan: Attempted to access a Task using an invalid name.") {}
};


/**
 * @class Plan_Task_GeneralExecutionException
 * @brief Wrapper exception to catch exceptions thrown by the execution of Tasks.
 *
 * This class is derived from std::exception and is used as a wrapper to catch exceptions thrown by the execution of Tasks.
 * It has two constructors, one that takes a C-style string error message and another that takes a C++ STL string error message.
 * The error message is stored in the `msg_` member variable and can be accessed using the `what()` function.
 */
class Plan_Task_GeneralExecutionException : public std::exception {
    public:
        /**
         * @brief Constructs a Plan_Task_GeneralExecutionException object with a C-style string error message.
         *
         * The error message is passed as a C-style string and is copied upon construction.
         * The responsibility for deleting the char* lies with the caller.
         *
         * @param message C-style string error message.
         */
        explicit Plan_Task_GeneralExecutionException(const char* message) : msg_(message) {}

        /**
         * @brief Constructs a Plan_Task_GeneralExecutionException object with a C++ STL string error message.
         *
         * The error message is passed as a C++ STL string.
         *
         * @param message The error message.
         */
        explicit Plan_Task_GeneralExecutionException(const std::string& message) : msg_(message) {}

        /**
         * @brief Virtual destructor.
         *
         * The destructor is virtual to allow for subclassing.
         */
        virtual ~Plan_Task_GeneralExecutionException() throw() {}

        /**
         * @brief Returns a pointer to the (constant) error description.
         *
         * Returns a pointer to the constant error description stored in the `msg_` member variable.
         * The underlying memory is in possession of the Exception object. Callers must not attempt to free the memory.
         *
         * @return A pointer to a const char*.
         */
        virtual const char* what() const throw() { return msg_.c_str(); }

    protected:
        /**
         * @brief Error message.
         */
        std::string msg_;
};


/**
 * @class Plan_Task_Missing_Dependency
 * @brief Exception thrown when a Task is missing a required dependency.
 *
 * This class is derived from std::exception and is used to indicate that a Task is missing a required dependency.
 * It has two constructors, one that takes a C-style string error message and another that takes a C++ STL string error message.
 * The error message is stored in the `msg_` member variable and can be accessed using the `what()` function.
 */
class Plan_Task_Missing_Dependency : public std::exception {
public:
    /**
     * @brief Constructs a Plan_Task_Missing_Dependency object with a C-style string error message.
     *
     * The error message is passed as a C-style string and is copied upon construction.
     * The responsibility for deleting the char* lies with the caller.
     *
     * @param message C-style string error message.
     */
    explicit Plan_Task_Missing_Dependency(const char* message) : msg_(message) {}

    /**
     * @brief Constructs a Plan_Task_Missing_Dependency object with a C++ STL string error message.
     *
     * The error message is passed as a C++ STL string.
     *
     * @param message The error message.
     */
    explicit Plan_Task_Missing_Dependency(const std::string& message) : msg_(message) {}

    /**
     * @brief Virtual destructor.
     *
     * The destructor is virtual to allow for subclassing.
     */
    virtual ~Plan_Task_Missing_Dependency() throw() {}

    /**
     * @brief Returns a pointer to the (constant) error description.
     *
     * Returns a pointer to the constant error description stored in the `msg_` member variable.
     * The underlying memory is in possession of the Exception object. Callers must not attempt to free the memory.
     *
     * @return A pointer to a const char*.
     */
    virtual const char* what() const throw() { return msg_.c_str(); }

protected:
    /**
     * @brief Error message.
     */
    std::string msg_;
};


/**
 * @brief Constructor for Plan class.
 *
 * A Plan is a managed container for a Task vector. These tasks reference Units that are defined in the Units files (Suite).
 * If Units are definitions, Tasks are selections of those definitions to execute, and if Units together form a Suite,
 * Tasks together form a Plan.
 *
 * @param configuration A pointer to a Conf object that holds the configuration information.
 * @param LOG_LEVEL The logging level for the plan.
 */
Plan::Plan(Conf * configuration, int LOG_LEVEL ): JSON_Loader(LOG_LEVEL ), slog(LOG_LEVEL, "_plan_" )
{
    this->configuration = configuration;
    this->LOG_LEVEL = LOG_LEVEL;
}


/**
 * @brief Load the plan file and append intact Units as they're deserialized from the provided file.
 *
 * @param filename The filename to load the plan from.
 */
void Plan::load_plan_file( std::string filename )
{
    // plan always loads from file
    this->load_json_file( filename );

    // staging buffer
    Json::Value jbuff;

    // fill the jbuff staging buffer with a json::value object in the supplied filename
    if ( this->get_serialized( jbuff, "plan" ) == 0 )
    {
        this->json_root = jbuff;
    }

    // iterate through the json::value members that have been loaded.  append to this->tasks vector
    // buffer for tasks to append:
    Task tmp_T = Task( this->LOG_LEVEL );
    for ( int index = 0; index < this->json_root.size(); index++ )
    {
        tmp_T.load_root( this->json_root[ index ] );
        this->tasks.push_back( tmp_T );
        this->slog.log( LOG_INFO, "Added task \"" + tmp_T.get_name() + "\" to Plan." );
    }
}


/**
 * @brief Retrieve a task by index.
 *
 * @param result The variable receiving the value.
 * @param index The numerical index in the Task vector to retrieve a value for.
 *
 * @throws Plan_InvalidTaskIndex if the index is greater than the size of the Task vector.
 */
void Plan::get_task(Task & result, int index )
{
    if ( index <= this->tasks.size() )
    {
        result = this->tasks[ index ];
    } else {
        throw Plan_InvalidTaskIndex();
    }
}


/**
 * @brief Load the units corresponding to each task in the plan from the given Suite.
 *
 * @param unit_definitions The Suite to load definitions from.
 */
void Plan::load_definitions( Suite unit_definitions )
{
    // placeholder Unit
    Unit tmp_U = Unit( this->LOG_LEVEL );

    // for every task in the plan:
    for (int i = 0; i < this->tasks.size(); i++ )
    {
        // load the tmp_U corresponding to that task name
        unit_definitions.get_unit( tmp_U, this->tasks[i].get_name() );

        // then have that task attach a copy of tmp_U
        this->tasks[i].load_definition( tmp_U );
    }
}


/**
 * @brief Retrieve a task by name.
 *
 * @param result The variable receiving the value.
 * @param provided_name The name to find a task by.
 *
 * @throws Plan_InvalidTaskName if a task with the provided name is not found.
 */
void Plan::get_task(Task & result, std::string provided_name )
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
        this->slog.log( E_FATAL, "Task name \"" + provided_name + "\" was referenced but not defined!" );
        throw Plan_InvalidTaskName();
    }
}


/**
 * @brief Check whether all dependencies for a task with the given name are complete.
 *
 * @param name The name of the task in the plan to check met dependencies for.
 *
 * @return Boolean representation of whether all dependencies are complete or not.
 */
bool Plan::all_dependencies_complete(std::string name)
{
    // get the task by name
    Task named_task = Task( this->LOG_LEVEL );
    this->get_task( named_task, name );

    // get the dependencies of that task
    std::vector<std::string> deps = named_task.get_dependencies();

    // create an empty task to assign values to during iteration
    Task tmpTask = Task( this->LOG_LEVEL );
    // iterate through its dependencies
    for ( int i = 0; i < deps.size(); i++ )
    {
        this->get_task( tmpTask, deps[i]);
        if (! tmpTask.is_complete() )
        {
            // error message?
            return false;
        }
    }
    return true;
}


/**
 * @brief Iterate through all tasks in the plan and execute them.
 */
void Plan::execute()
{
    // for each task in this plan
    for ( int i = 0; i < this->tasks.size(); i++ )
    {
        if (this->all_dependencies_complete(this->tasks[i].get_name()) )
        {

            this->slog.log( E_INFO, "[ '" + this->tasks[i].get_name() + "' ] Executing..." );
            try {
                this->tasks[i].execute( this->configuration );
            }
            catch (std::exception& e) {
                this->slog.log( E_FATAL, "[ '" + this->tasks[i].get_name() + "' ] Report: " + e.what() );
                throw Plan_Task_GeneralExecutionException("Could not execute task.");
            }
        } else {
            // not all deps met for this task
            this->slog.log( E_FATAL, "[ '" + this->tasks[i].get_name() + "' ] This task was specified in the Plan but not executed due to missing dependencies.  Please revise your plan."  );
            throw Plan_Task_Missing_Dependency( "Unmet dependency for task." );
        }
    }
}
