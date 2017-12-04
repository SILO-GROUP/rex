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


/// Plan_Task_GeneralExecutionException - Wrapper exception to catch exceptions thrown by the execution of Tasks.
class Plan_Task_GeneralExecutionException: public std::exception
{
public:
    /** Constructor (C strings).
     *  @param message C-style string error message.
     *                 The string contents are copied upon construction.
     *                 Hence, responsibility for deleting the char* lies
     *                 with the caller.
     */
    explicit Plan_Task_GeneralExecutionException(const char* message):
            msg_(message)
    {
    }

    /** Constructor (C++ STL strings).
     *  @param message The error message.
     */
    explicit Plan_Task_GeneralExecutionException(const std::string& message):
            msg_(message)
    {}

    /** Destructor.
     * Virtual to allow for subclassing.
     */
    virtual ~Plan_Task_GeneralExecutionException() throw (){}

    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *          is in posession of the Exception object. Callers must
     *          not attempt to free the memory.
     */
    virtual const char* what() const throw (){
        return msg_.c_str();
    }

protected:
    /** Error message.
     */
    std::string msg_;
};


/// Plan_Task_Missing_Dependency - Exception thrown when a Plan tries to access a contained Task's value by name not present
/// in the Unit.
class Plan_Task_Missing_Dependency: public std::exception
{
public:
    /** Constructor (C strings).
     *  @param message C-style string error message.
     *                 The string contents are copied upon construction.
     *                 Hence, responsibility for deleting the char* lies
     *                 with the caller.
     */
    explicit Plan_Task_Missing_Dependency(const char* message):
            msg_(message)
    {
    }

    /** Constructor (C++ STL strings).
     *  @param message The error message.
     */
    explicit Plan_Task_Missing_Dependency(const std::string& message):
            msg_(message)
    {}

    /** Destructor.
     * Virtual to allow for subclassing.
     */
    virtual ~Plan_Task_Missing_Dependency() throw (){}

    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *          is in posession of the Exception object. Callers must
     *          not attempt to free the memory.
     */
    virtual const char* what() const throw (){
        return msg_.c_str();
    }

protected:
    /** Error message.
     */
    std::string msg_;
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
void Plan::get_task(Task & result, int index )
{
    if ( index <= this->tasks.size() )
    {
        result = this->tasks[ index ];
    } else {
        throw Plan_InvalidTaskIndex();
    }
}


/// Plan::load_definitions - Load the units corresponding to each task in plan from the given Suite.
///
/// \param unit_definitions - The Suite to load definitions from.
/// \param verbose - Whether to print verbose information to STDOUT.
void Plan::load_definitions( Suite unit_definitions, bool verbose )
{
    // placeholder Unit
    Unit tmp_U;

    // for every task in the plan:
    for (int i = 0; i < this->tasks.size(); i++ )
    {
        // load the tmp_U corresponding to that task name
        unit_definitions.get_unit( tmp_U, this->tasks[i].get_name() );

        // then have that task attach a copy of tmp_U
        this->tasks[i].load_definition( tmp_U, verbose );
    }
}

/// Plan::get_task - Retrieves a task by name.
///
/// \param result - The variable receiving the value.
/// \param provided_name - The name to find a task by.
/// \param verbose - Whether to print verbose output to STDOUT.
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
        std::cerr << "Task name \"" << provided_name << "\" was referenced but not defined!" << std::endl;
        throw Plan_InvalidTaskName();
    }
}


/// Plan::all_dependencies_complete
///
/// \param name - The name of the task in the plan to check met dependencies for.
/// \return - boolean representation of whether all dependencies are complete or not.
bool Plan::all_dependencies_complete(std::string name)
{
    // get the task by name
    Task named_task;
    this->get_task( named_task, name );

    // get the dependencies of that task
    std::vector<std::string> deps = named_task.get_dependencies();

    // create an empty task to assign values to during iteration
    Task tmpTask;
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

/// Plan::execute() - Iterates through all tasks in a plan and executes them.
///
/// \param verbose
void Plan::execute( bool verbose )
{
    // for each task in this plan
    for ( int i = 0; i < this->tasks.size(); i++ )
    {
        if (this->all_dependencies_complete(this->tasks[i].get_name()) )
        {
            if ( verbose )
            {
                std::cout << "Executing task \"" << this->tasks[i].get_name() << "\"." << std::endl;
            }
            try {
                this->tasks[i].execute( verbose );
            }
            catch (std::exception& e) {
                throw Plan_Task_GeneralExecutionException( "Plan Task: \"" + this->tasks[i].get_name() + "\" reported: " + e.what() );
            }
        } else {
            // not all deps met for this task
            throw Plan_Task_Missing_Dependency( "Plan Task \"" + this->tasks[i].get_name() + "\" was specified in the Plan but not executed due to missing dependencies.  Please revise your plan." );
        }
    }
}

