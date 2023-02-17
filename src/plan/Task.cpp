#include "Task.h"

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

#include "Task.h"


/**
 * @class Task_InvalidDataStructure
 * @brief Exception thrown when a Task is defined with invalid JSON.
 *
 * This exception is derived from the standard runtime_error exception and is thrown
 * when a Task object is defined with an invalid JSON structure.
 */
class Task_InvalidDataStructure: public std::runtime_error {
    public:
        /**
         * @brief Constructs a Task_InvalidDataStructure object.
         *
         * This constructor creates a Task_InvalidDataStructure object with an error message
         * indicating that a task was attempted to be accessed with an invalid data structure.
         */
        Task_InvalidDataStructure(): std::runtime_error("Task: Attempted to access a member of a Task that is not set.") {}
};


/**
 * @class Task_NotReady
 * @brief Exception thrown when a Task is not ready to be executed.
 *
 * This exception is derived from the standard runtime_error exception and is thrown
 * when a Task object is not well defined and cannot be executed.
 */
class Task_NotReady: public std::runtime_error {
public:
    /**
     * @brief Constructs a Task_NotReady object.
     *
     * This constructor creates a Task_NotReady object with an error message
     * indicating that a task was attempted to be executed but its Unit was not well defined.
     */
    Task_NotReady(): std::runtime_error("Task: Attempted to execute a Task whose Unit is not well defined.") {}
};


/**
 * @class TaskException
 * @brief Exception thrown for errors related to Tasks.
 *
 * This exception is a base class for exceptions related to Tasks and is derived
 * from the standard exception class.
 */
class TaskException: public std::exception
{
    public:
        /**
         * @brief Constructs a TaskException object with a C-style string error message.
         *
         * This constructor creates a TaskException object with a C-style string error message.
         * The string contents are copied upon construction and the responsibility for deleting
         * the char* lies with the caller.
         *
         * @param message C-style string error message.
         */
        explicit TaskException(const char* message): msg_(message) {}

        /**
         * @brief Constructs a TaskException object with a C++ STL string error message.
         *
         * This constructor creates a TaskException object with a C++ STL string error message.
         *
         * @param message The error message.
         */
        explicit TaskException(const std::string& message): msg_(message) {}

        /**
         * @brief Virtual destructor.
         *
         * This destructor is virtual to allow for subclassing.
         */
        virtual ~TaskException() throw () {}

        /**
         * @brief Returns a pointer to the error description.
         *
         * This function returns a pointer to the error description.
         * The underlying memory is in posession of the Exception object and callers must not attempt
         * to free the memory.
         *
         * @return A pointer to a const char*.
         */
        virtual const char* what() const throw () { return msg_.c_str(); }

    protected:
        /**
         * @brief Error message.
         */
        std::string msg_;
};


/**
 * @class Task
 * @brief The building block of a Plan indicating of which Unit to execute and its dependencies.
 *
 * The Task class represents a single unit of work in a Plan. It specifies which Unit should be executed
 * and any dependencies that must be satisfied before it can be executed.
 */

/**
 * @brief Constructor for the Task class.
 *
 * This constructor initializes a Task object with a specified log level and creates log and definition objects.
 * The task is set as not complete and not defined by default.
 *
 * @param LOG_LEVEL The log level for this Task object.
 */
Task::Task( int LOG_LEVEL ):
        slog( LOG_LEVEL, "_task_" ),
        definition( LOG_LEVEL )
{
    // it hasn't executed yet.
    this->complete = false;

    // it hasn't been matched with a definition yet.
    this->defined = false;

    this->LOG_LEVEL = LOG_LEVEL;
}


/**
 * @brief Loads JSON values into private members.
 *
 * This function loads JSON values into the private members of a Task object.
 *
 * @param loader_root The Json::Value to populate from.
 * @throws Task_InvalidDataStructure if the "name" member is not present in the JSON object.
 */
void Task::load_root(Json::Value loader_root )
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
        if ( des_dep_root[i].asString() != "" )
        {
            this->dependencies.push_back( des_dep_root[i].asString() );
            this->slog.log( E_INFO,  "Added dependency \"" + des_dep_root[i].asString() + "\" to task \"" + this->get_name() + "\"." );
        }
    }
}

/**
 * @brief Retrieves the name of the current Task.
 *
 * @return The name of the Task as a string.
 */
std::string Task::get_name()
{
    return this->name;
}

/**
 * @brief Loads a unit to a local member. Used to tie Units to Tasks.
 *
 * @param selected_unit The unit to attach.
 * @param verbose Whether to print to STDOUT.
 */
void Task::load_definition( Unit selected_unit )
{
    this->definition = selected_unit;
    this->slog.log( E_INFO, "Loaded definition \"" + selected_unit.get_name() + "\" as task in configured plan.");
    this->defined = true;
}

/**
 * @brief Indicates if the task executed successfully.
 *
 * @return True if the task completed successfully, false otherwise.
 */
bool Task::is_complete()
{
    return this->complete;
}


/**
 * @brief Marks the task as complete.
 */
void Task::mark_complete()
{
    this->complete = true;
}


/**
 * @brief Returns a pointer to the dependencies vector.
 *
 * @return A pointer to the vector containing the task's dependencies.
 */
std::vector<std::string> Task::get_dependencies()
{
    return this->dependencies;
}


/**
 * @brief Indicates if the task has attached its definition from a Suite.
 *
 * @return True if the task has a definition, false otherwise.
 */
bool Task::has_definition()
{
    return this->defined;
}

bool is_abs_path( const std::string &path )
{
    if ( path[0] == '/' )
    {
        return true;
    }
    return false;
}


bool abspathExists(const std::string &path) {
    if ( is_abs_path( path ) )
    {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    } else {
        return false;
    }
}

std::string path_from_str( const std::string &inputString )
{
    std::string result = "";

    // Find the position of the first space character in the input string
    size_t spacePosition = inputString.find(' ');

    // If there is a space in the input string, extract the substring up to the space
    if (spacePosition != std::string::npos) {
        result = inputString.substr(0, spacePosition);
    }
        // Otherwise, just return the entire input string
    else {
        result = inputString;
    }

    return result;
}


bool createDirectory(const std::string& path) {
    // Check if the directory already exists
    struct stat info;
    if (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode)) {
        return true; // Directory already exists
    }

    // Create the directory recursively
    size_t pos = 0;
    std::string dir;
    while ((pos = path.find_first_of('/', pos + 1)) != std::string::npos) {
        dir = path.substr(0, pos);
        if (stat(dir.c_str(), &info) != 0) { // Directory does not exist
            if (mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
                return false; // Failed to create directory
            }
        } else if (!S_ISDIR(info.st_mode)) {
            return false; // Path segment exists but is not a directory
        }
    }

    // Create the final directory
    if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
        return false; // Failed to create directory
    }

    return true;
}

bool Task::prepare_logs( std::string task_name, std::string logs_root )
{
    std::string full_path = logs_root + "/" + task_name;
    bool ret = false;
    ret = createDirectory( full_path );

    if (ret)
    {
        this->slog.log_task( E_INFO, "LOG_CREATE", "Logging will be at '" + full_path + "'." );
    } else {
        this->slog.log_task( E_FATAL, "LOG_CREATE", "Creation of directory path '" + full_path + "' failed." );
    }

    return ret;
}

/// Task::execute - execute a task's unit definition.
/// See the design document for what flow control needs to look like here.
/// \param verbose - Verbosity level - not implemented yet.
void Task::execute( Conf * configuration )
{

    if ( ! this->has_definition() )
    {
        throw Task_NotReady();
    }

    bool override_working_dir = this->definition.get_set_working_directory();
    bool is_shell_command = this->definition.get_is_shell_command();
    bool supply_environment = this->definition.get_supply_environment();
    bool rectify = this->definition.get_rectify();
    bool active = this->definition.get_active();
    bool required = this->definition.get_required();
    bool set_user_context = this->definition.get_set_user_context();
    bool force_pty = this->definition.get_force_pty();

    std::string task_name = this->definition.get_name();
    std::string command = this->definition.get_target();
    std::string shell_name = this->definition.get_shell_definition();
    Shell shell_definition = configuration->get_shell_by_name( shell_name );
    std::string new_working_dir = this->definition.get_working_directory();
    std::string rectifier = this->definition.get_rectifier();
    std::string user = this->definition.get_user();
    std::string group = this->definition.get_group();
    std::string environment_file = this->definition.get_environment_file();
    std::string logs_root = configuration->get_logs_path();

    this->slog.log_task( E_DEBUG, task_name, "Using unit definition: \"" + task_name + "\"." );

    // sanitize all path inputs from unit definition to be either absolute paths or relative to
    // project_root

    if ( supply_environment )
    {
        if (! is_shell_command )
        {
            throw TaskException("Garbage input: Supplied a shell environment file for a non-shell target.");
        }

    }

    if ( ! active )
    {
        throw TaskException("Somehow tried to execute a task with an inactive unit definition.");
    }

    if (! is_abs_path( path_from_str( command ) ) )
    {
        command = configuration->get_project_root() + "/" + command;
    }

    if ( rectify )
    {
        if (! is_abs_path( path_from_str( rectifier ) ) )
        {
            rectifier = configuration->get_project_root() + "/" + rectifier;
        }
    }

    if ( supply_environment )
    {
        if (! is_abs_path( environment_file ) )
        {
            environment_file = configuration->get_project_root() + "/" + environment_file;
        }
    }

    if ( override_working_dir )
    {
        if (! is_abs_path( new_working_dir ) )
        {
            new_working_dir = configuration->get_project_root() + "/" + new_working_dir;
        }
    }

    if (! is_abs_path( configuration->get_logs_path() ) )
    {
        logs_root = configuration->get_project_root() + "/" + logs_root;
    }


    // set these first so the pre-execution logs get there.
    /*
     * create the logs dir here
     */

    if (! this->prepare_logs( task_name, logs_root ) )
    {
        throw TaskException("Could not prepare logs for task execution at '" + logs_root + "'.");
    }

    std::string timestamp = get_8601();
    std::string stdout_log_file = logs_root + "/" + task_name + "/" + timestamp + ".stdout.log";
    std::string stderr_log_file = logs_root + "/" + task_name + "/" + timestamp + ".stderr.log";

    // open file handles to the two log files we need to create for each execution
    FILE * stdout_log_fh = fopen( stdout_log_file.c_str(), "a+" );
    FILE * stderr_log_fh = fopen( stderr_log_file.c_str(), "a+" );

    // check if working directory is to be set
    if ( override_working_dir )
    {
        // if so, set the CWD.
        chdir( new_working_dir.c_str() );
        this->slog.log_task( E_INFO, task_name, "Setting working directory: " + new_working_dir );
    }

    if ( is_shell_command )
    {
        this->slog.log_task( E_INFO, task_name, "Vars file: " + environment_file );
        this->slog.log_task( E_INFO, task_name, "Shell: " + shell_definition.path );
    }

    // a[0] execute target
    // TODO ...sourcing on the shell for variables and environment population doesn't have a good smell.
    // it does prevent unexpected behaviour from reimplementing what bash does though

    this->slog.log_task( E_INFO, task_name, "Executing target: \"" + command + "\"." );

    int return_code = lcpex(
            command,
            stdout_log_fh,
            stderr_log_fh,
            set_user_context,
            user,
            group,
            force_pty,
            is_shell_command,
            shell_definition.path,
            shell_definition.execution_arg,
            supply_environment,
            shell_definition.source_cmd,
            environment_file
    );

    // **********************************************
    // d[0] Error Code Check
    // **********************************************
    if ( return_code == 0 )
    {
        // d[0].0 ZERO
        this->slog.log_task( E_INFO, task_name, "Target succeeded.  Marking as complete." );

        this->mark_complete();

        // a[1] NEXT
        return;
    }

    if ( return_code != 0 )
    {
        // d[0].1 NON-ZERO
        this->slog.log_task( E_WARN,  task_name, "Target failed with exit code " + std::to_string( return_code ) + "." );

        // **********************************************
        // d[1] Rectify Check
        // **********************************************
        if (! this->definition.get_rectify() )
        {
            // d[1].0 FALSE
            // **********************************************
            // d[2] Required Check
            // **********************************************
            if (! required )
            {
                // d[2].0 FALSE
                // a[2] NEXT
                this->slog.log_task(  E_INFO, task_name, "This task is not required to continue the plan. Moving on." );
                return;
            } else {
                // d[2].1 TRUE
                // a[3] EXCEPTION
                this->slog.log_task( E_FATAL, task_name, "Task is required, and failed, and rectification is not enabled." );
                throw TaskException( "Task failed: " + task_name );
            }
            // **********************************************
            // end - d[2] Required Check
            // **********************************************
        }


        if ( rectify )
        {
            // d[1].1 TRUE (Rectify Check)
            this->slog.log_task( E_INFO, task_name, "Rectification pattern is enabled." );

            // a[4] Execute RECTIFIER
            this->slog.log_task( E_INFO, task_name, "Executing rectification: " + rectifier + "." );
            int rectifier_error = lcpex(
                    rectifier,
                    stdout_log_fh,
                    stderr_log_fh,
                    set_user_context,
                    user,
                    group,
                    force_pty,
                    is_shell_command,
                    shell_definition.path,
                    shell_definition.execution_arg,
                    supply_environment,
                    shell_definition.source_cmd,
                    environment_file
            );

            // **********************************************
            // d[3] Error Code Check for Rectifier
            // **********************************************
            if ( rectifier_error != 0 )
            {
                // d[3].1 Non-Zero
                this->slog.log_task(  E_WARN, task_name, "Rectification failed with exit code " + std::to_string( rectifier_error ) + "." );

                // **********************************************
                // d[4] Required Check
                // **********************************************
                if ( ! required ) {
                    // d[4].0 FALSE
                    // a[5] NEXT
                    this->slog.log_task( E_INFO,  task_name, "This task is not required to continue the plan. Moving on." );
                    return;
                } else {
                    // d[4].1 TRUE
                    // a[6] EXCEPTION
                    this->slog.log_task( E_FATAL, task_name, "Task is required, but failed, and rectification failed.  Lost cause." );
                    throw TaskException( "Lost cause, task failure." );
                }
                // **********************************************
                // end - d[4] Required Check
                // **********************************************
            }

            // d[3] check exit code of rectifier
            if ( rectifier_error == 0 )
            {
                // d[3].0 Zero
                this->slog.log_task( E_INFO, task_name, "Rectification returned successfully." );

                // a[7] Re-execute Target
                this->slog.log_task( E_INFO, task_name, "Re-Executing target '" + command + "'." );

                int retry_code = lcpex(
                        command,
                        stdout_log_fh,
                        stderr_log_fh,
                        set_user_context,
                        user,
                        group,
                        force_pty,
                        is_shell_command,
                        shell_definition.path,
                        shell_definition.execution_arg,
                        supply_environment,
                        shell_definition.source_cmd,
                        environment_file
                );

                // **********************************************
                // d[5] Error Code Check
                // **********************************************
                if ( retry_code == 0 )
                {
                    // d[5].0 ZERO
                    // a[8] NEXT
                    this->slog.log_task( E_INFO, task_name, "Re-execution was successful." );
                    return;
                } else {
                    // d[5].1 NON-ZERO
                    this->slog.log_task( E_WARN, task_name, "Re-execution failed with exit code " + std::to_string( retry_code ) + "." );

                    // **********************************************
                    // d[6] Required Check
                    // **********************************************
                    if ( ! required )
                    {
                        // d[6].0 FALSE
                        // a[9] NEXT
                        this->slog.log_task( E_INFO, task_name, "This task is not required to continue the plan. Moving on." );
                        return;
                    } else {
                        // d[6].1 TRUE
                        // a[10] EXCEPTION
                        this->slog.log_task( E_FATAL, task_name, "Task is required, and failed, then rectified but rectifier did not heal the condition causing the target to fail.  Cannot proceed with Plan." );
                        throw TaskException( "Lost cause, task failure." );
                    }
                    // **********************************************
                    // end - d[6] Required Check
                    // **********************************************
                }

            }
        }
        // **********************************************
        // end d[1] Rectify Check
        // **********************************************
    }
    // close the log file handles
    fclose(stdout_log_fh);
    fclose(stderr_log_fh);
}