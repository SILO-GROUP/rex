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

#include "Unit.h"


/**
 * @class UnitException
 * @brief Exception class for Unit related errors.
 *
 * This class provides an exception type for Unit related errors.
 */
class UnitException: public std::exception
{
    public:
        /**
         * @brief Constructor for UnitException (C strings).
         *
         * @param message C-style string error message. The string contents are copied upon construction.
         *                Responsibility for deleting the char* lies with the caller.
         */
        explicit UnitException(const char* message): msg_(message) {}

        /**
         * @brief Constructor for UnitException (C++ STL strings).
         *
         * @param message The error message.
         */
        explicit UnitException(const std::string& message): msg_(message) {}

        /**
         * @brief Destructor for UnitException.
         *
         * Virtual to allow for subclassing.
         */
        virtual ~UnitException() throw (){}

        /**
         * @brief Returns a pointer to the (constant) error description.
         *
         * @return A pointer to a const char*. The underlying memory is in posession of the Exception object.
         *         Callers must not attempt to free the memory.
         */
        virtual const char* what() const throw () { return msg_.c_str(); }

    protected:
        /**
         * @brief Error message.
         */
        std::string msg_;
};


/**
 * @class Unit
 * @brief Definition of an automation task.
 *
 * The Unit is a definition of an automation task. Each Unit has:
 *   - name, used for identification and retrieval.
 *   - target, which is the filepath of an executable to trigger.
 *   - output, which is the desired output of the execution of target to STDOUT in determinations of success or failure
 *     (in addition to 0|non-0 exit code). If the output is set to look for "0" then it uses the exit code.
 *   - rectifier, which is the path to an executable in the event of a non-0 exit code or a failure to get the desired
 *     output.
 *   - required, which is used as a flag to halt or continue if rectifier does not heal the system in such a way that
 *     target can run successfully.
 *   - rectify, which is used as a flag to determine if the rectifier runs.
 *
 * @param LOG_LEVEL The log level to be used.
 */
Unit::Unit( int LOG_LEVEL ): JSON_Loader( LOG_LEVEL ), slog( LOG_LEVEL, "_unit_" )
{
    this->LOG_LEVEL;
}


/**
 * @brief Unit::load_root - Takes a JSON::Value and assigns the members to the Unit being populated.
 *
 * @param loader_root - The JSON::Value object to use to populate unit from.  Usually supplied as the Suite's buffer
 * member.
 * @return  - Boolean representation of success or failure.
 */
int Unit::load_root(Json::Value loader_root)
{
    // TODO this needs reworked to have errmsg actually end up as a null return from json::value.get()
    std::string errmsg = "SOMETHING WENT TERRIBLY WRONG IN PARSING";

    // TODO this pattern is 'working but broken'.  need to use a datastructure for required members and iterate
    // do NOT replace this with a switch case pattern
    if ( loader_root.isMember("name") )
    { this->name = loader_root.get("name", errmsg).asString(); } else
        throw UnitException("No 'name' attribute specified when loading a unit.");

    if ( loader_root.isMember("target") )
    { this->target = loader_root.get("target", errmsg).asString(); } else
        throw UnitException("No 'target' attribute specified when loading a unit.");

    if ( loader_root.isMember("is_shell_command") )
    { this->is_shell_command = loader_root.get("is_shell_command", errmsg).asBool(); } else
        throw UnitException("No 'is_shell_command' attribute specified when loading a unit.");

    if ( loader_root.isMember("shell_definition") )
    { this->shell_definition = loader_root.get("shell_definition", errmsg).asString(); } else
        throw UnitException("No 'shell_definition' attribute specified when loading a unit.");

    if ( loader_root.isMember("force_pty") )
    { this->force_pty = loader_root.get("force_pty", errmsg).asBool(); } else
        throw UnitException("No 'force_pty' attribute specified when loading a unit.");

    if ( loader_root.isMember("set_working_directory") )
    { this->set_working_directory = loader_root.get("set_working_directory", errmsg).asBool(); } else
        throw UnitException("No 'set_working_directory' attribute specified when loading a unit.");

    if ( loader_root.isMember("rectify") )
    { this->rectify = loader_root.get("rectify", errmsg).asBool(); } else
        throw UnitException("No 'rectify' boolean attribute specified when loading a unit.");

    if ( loader_root.isMember("rectifier") )
    { this->rectifier = loader_root.get("rectifier", errmsg).asString(); } else
        throw UnitException("No 'rectifier' executable attribute specified when loading a unit.");

    if ( loader_root.isMember("active") )
    { this->active = loader_root.get("active", errmsg).asBool(); } else
        throw UnitException("No 'active' attribute specified when loading a unit.");

    if ( loader_root.isMember("required") )
    { this->required = loader_root.get("required", errmsg).asBool(); } else
        throw UnitException("No 'required' attribute specified when loading a unit.");

    if ( loader_root.isMember("set_user_context") )
    { this->set_user_context = loader_root.get("set_user_context", errmsg).asBool(); } else
        throw UnitException("No 'set_user_context' attribute specified when loading a unit.");

    // TODO functionize this
    int uid = getuid();
    struct passwd * upw;

    std::string errmsg_user;

    // if no user field is specified then default to the currently executing user
    if ( ( upw = getpwuid(uid) ) == nullptr )
    {
        throw UnitException( "Could not retrieve current user." );
    } else {
        errmsg_user = upw->pw_name;
    }
    // -TODO

    if ( loader_root.isMember( "user" ) )
    { this->user = loader_root.get( "user", errmsg_user ).asString(); } else this->user = errmsg_user;

    // TODO functionalize this
    // get the current context gid as a backup value
    int gid = getgid();
    // declare the grp object to pull the name from once populated
    struct group * grp;
    // storage for backup value once retrieved
    std::string errmsg_group;

    // get the backup value and store it to errmsg_group
    if ( ( grp = getgrgid( gid ) ) == nullptr )
    {
        throw UnitException("Could not retrieve current group");
    } else {
        errmsg_group = grp->gr_name;
    }

    if ( loader_root.isMember( "group" ) )
    { this->group = loader_root.get( "group", errmsg_group ).asString(); } else this->group = grp->gr_name;

    if ( loader_root.isMember("supply_environment") )
    {
        this->supply_environment = loader_root.get("supply_environment", errmsg).asBool();
    } else {
        throw UnitException("No 'supply_environment' attribute specified when loading a unit.");
    }

    if ( loader_root.isMember("environment") )
    {
        this->env_vars_file = loader_root.get("environment", errmsg).asString();
    } else {
        throw UnitException("No 'environment' attribute specified when loading a unit.");
    }

    this->populated = true;

    return 0;
}


/**
 * @brief Retrieves the name of the unit.
 *
 * @return The name of the unit.
 *
 * @throws UnitException if the unit has not been populated.
 */
std::string Unit::get_name()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->name;
}


/**
 * @brief Retrieves the target of the unit.
 *
 * @return The target of the unit.
 *
 * @throws UnitException if the unit has not been populated.
 */
std::string Unit::get_target()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->target;
}


/**
 * @brief Retrieves whether the unit is a shell command.
 *
 * @return True if the unit is a shell command, false otherwise.
 *
 * @throws UnitException if the unit has not been populated.
 */
bool Unit::get_is_shell_command()
{
    if (!this->populated) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->is_shell_command;
}


/**
 * @brief Retrieves the shell definition of the unit.
 *
 * @return The shell definition of the unit.
 *
 * @throws UnitException if the unit has not been populated.
 */
std::string Unit::get_shell_definition() {
    if (!this->populated) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->shell_definition;
}


/**
 * @brief Retrieves whether the unit requires a PTY.
 *
 * @return True if the unit requires a PTY, false otherwise.
 *
 * @throws UnitException if the unit has not been populated.
 */
bool Unit::get_force_pty() {
    if (!this->populated) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->force_pty;
}


/**
 * @brief Retrieves whether the working directory should be set for the unit.
 *
 * @return True if the working directory should be set, false otherwise.
 *
 * @throws UnitException if the unit has not been populated.
 */
bool Unit::get_set_working_directory() {
    if (!this->populated) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->set_working_directory;
}


/**
 * @brief Retrieves the working directory of the unit.
 *
 * @return The working directory of the unit.
 *
 * @throws UnitException if the unit has not been populated.
 */
std::string Unit::get_working_directory() {
    if (!this->populated) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->working_directory;
}


/**
 * @brief Retrieves the rectification status of the unit.
 *
 * @return The rectification status of the unit.
 *
 * @throws UnitException if the unit has not been populated.
 */
bool Unit::get_rectify() {
    if (!this->populated) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->rectify;
}


/**
 * @brief Retrieves the rectifier of the unit.
 *
 * @return The rectifier of the unit.
 *
 * @throws UnitException if the unit has not been populated.
 */
std::string Unit::get_rectifier()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->rectifier;
}


/**
 * @brief Retrieves the armed status of the unit.
 *
 * @return The armed status of the unit.
 *
 * @throws UnitException if the unit has not been populated.
 */
bool Unit::get_active()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->active;
}


/**
 * @brief Retrieves the requirement status of the unit.
 *
 * @return The requirement status of the unit.
 *
 * @throws UnitException if the unit has not been populated.
 */
bool Unit::get_required()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->required;
}


/**
 * @brief Retrieves whether the user context should be set for the unit.
 *
 * @return True if the user context should be set, false otherwise.
 *
 * @throws UnitException if the unit has not been populated.
 */
bool Unit::get_set_user_context() {
    if (!this->populated) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->set_user_context;
}


/**
 * @brief Retrieves the user context for the unit.
 *
 * @return The string value of the user name.
 *
 * @throws UnitException if the unit has not been populated.
 */
std::string Unit::get_user()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->user;
}


/**
 * @brief Retrieves the group context for the unit.
 *
 * @return The string value of the group name.
 *
 * @throws UnitException if the unit has not been populated.
 */
std::string Unit::get_group()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->group;
}


/**
 * @brief Retrieves whether the environment should be supplied to the unit.
 *
 * @return True if the environment should be supplied, false otherwise.
 *
 * @throws UnitException if the unit has not been populated.
 */
bool Unit::get_supply_environment() {
    if (!this->populated) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->supply_environment;
}


/**
 * @brief Retrieves the environment file for the unit.
 *
 * @return The environment file for the unit.
 *
 * @throws UnitException if the unit has not been populated.
 */
std::string Unit::get_environment_file()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->env_vars_file;
}

