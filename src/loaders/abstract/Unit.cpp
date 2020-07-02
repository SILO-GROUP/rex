/*
    Examplar - An automation and testing framework.

    © SURRO INDUSTRIES and Chris Punches, 2017.

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
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <pwd.h>
#include <grp.h>

/// UnitException -
class UnitException: public std::exception
{
public:
    /** Constructor (C strings).
     *  @param message C-style string error message.
     *                 The string contents are copied upon construction.
     *                 Hence, responsibility for deleting the char* lies
     *                 with the caller.
     */
    explicit UnitException(const char* message):
            msg_(message)
    {
    }

    /** Constructor (C++ STL strings).
     *  @param message The error message.
     */
    explicit UnitException(const std::string& message):
            msg_(message)
    {}

    /** Destructor.
     * Virtual to allow for subclassing.
     */
    virtual ~UnitException() throw (){}

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

/// Unit::Unit - Constructor for Unit type.  The Unit is a definition of an automation task.  Each Unit has:
/// name, used for identification and retrieval.
/// target, which is the filepath of an executable to trigger.
/// output, which is the desired output of the execution of target to STDOUT in determinations of of success or failure
/// (in addition to 0|non-0 exit code). If the output is set to look for "0" then it uses the exit code.
///
/// There is also:
/// rectifier, which is the path to an executable in the event of a non-0 exit code or a failure to get the desired
/// output.
/// required, which is used as a flag to halt or continue if rectifier does not heal the system in such a way that
/// target can run successfully.
/// rectify, which is used as a flag to determine in the rectifier runs.
Unit::Unit( int LOG_LEVEL ): JSON_Loader( LOG_LEVEL ), slog( LOG_LEVEL, "e_unit" )
{
    this->LOG_LEVEL;
}

/// Unit::load_root - Takes a JSON::Value and assigns the members to the Unit being populated.
///
/// \param loader_root - The JSON::Value object to use to populate unit from.  Usually supplied as the Suite's buffer
/// member.
/// \return  - Boolean representation of success or failure.
int Unit::load_root(Json::Value loader_root)
{
    // TODO this needs reworked to have errmsg actually end up as a null return from json::value.get()
    std::string errmsg = "SOMETHING WENT TERRIBLY WRONG IN PARSING";

    // TODO this pattern is 'working but broken'.  need to use a datastructure for required members and iterate
    // do NOT replace this with a switch case pattern
    if ( loader_root.isMember("name") )
    { this->name = loader_root.get("name", errmsg).asString(); } else
        throw UnitException("No name attribute specified when loading a unit.");

    if ( loader_root.isMember("target") )
    { this->target = loader_root.get("target", errmsg).asString(); } else
        throw UnitException("No target attribute specified when loading a unit.");

    if ( loader_root.isMember("rectifier") )
    { this->rectifier = loader_root.get("rectifier", errmsg).asString(); } else
        throw UnitException("No rectifier executable attribute specified when loading a unit.");

    if ( loader_root.isMember("active") )
    { this->active = loader_root.get("active", errmsg).asBool(); } else
        throw UnitException("No activation attribute specified when loading a unit.");

    if ( loader_root.isMember("required") )
    { this->required = loader_root.get("required", errmsg).asBool(); } else
        throw UnitException("No required attribute specified when loading a unit.");

    if ( loader_root.isMember("rectify") )
    { this->rectify = loader_root.get("rectify", errmsg).asBool(); } else
        throw UnitException("No rectify boolean attribute specified when loading a unit.");

    // TODO functionize this
    int uid = getuid();
    struct passwd * upw;

    std::string errmsg_user;

    // if no user field is specified then default to the currently executing user
    if ( ( upw = getpwuid(uid) ) == NULL )
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
    if ( ( grp = getgrgid( gid ) ) == NULL )
    {
        throw UnitException("Could not retrieve current group");
    } else {
        errmsg_group = grp->gr_name;
    }

    if ( loader_root.isMember( "group" ) )
    { this->group = loader_root.get( "group", errmsg_group ).asString(); } else this->group = grp->gr_name;

    if ( loader_root.isMember( "shell" ) )
    { this->shell = loader_root.get( "shell", errmsg ).asString(); } else this->shell = "sh";

    this->populated = true;

    return 0;
}

/// Unit::load_string - populates a Unit object from a supplies JSON-formatted string.  It's stellar.
///
/// \param json_val - JSON-formatted string to populate from.  See Unit::load_root() for details on required structure.
/// \return - The bool representation of success or failure.
int Unit::load_string(std::string json_val)
{
    // serialize
    this->load_json_string( json_val );

    // deserialize
    this->load_root( this->json_root );

    return 0;
}

/// Unit::get_name - retrieves the name of the unit.
///
/// \return the name of the unit.
std::string Unit::get_name()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->name;
}

/// Unit::get_target - retrieves the target of the unit.
///
/// \return the target of the unit.
std::string Unit::get_target()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->target;
}

/// Unit::get_output - retrieves the output of the unit.
///
/// \return the output of the unit.
std::string Unit::get_output()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->output;
}

/// Unit::get_rectifier - retrieves the rectifier of the unit.
///
/// \return the rectifier of the unit.
std::string Unit::get_rectifier()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->rectifier;
}

/// Unit::get_active - retrieves the armed status of the unit.
///
/// \return the armed status of the unit.
bool Unit::get_active()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->active;
}

/// Unit::get_required - retrieves the requirement status of the unit.
///
/// \return the requirement status of the unit.
bool Unit::get_required()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->required;
}

/// Unit::get_rectify - retrieves the rectification status of the unit.
///
/// \return the rectification status of the unit.
bool Unit::get_rectify()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->rectify;
}

/// Unit::get_user - retrieves the user context for the unit.
///
/// \return the string value of the user name.
std::string Unit::get_user()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->user;
}
/// Unit::get_group - retrieves the group context for the unit.
///
/// \return the string value of the group name.
std::string Unit::get_group()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->group;
}

/// Unit::get_shell - retrieves the shell path to use for the unit execution.
///
/// \return the string value of the shell path.
std::string Unit::get_shell()
{
    if ( ! this->populated ) { throw UnitException("Attempted to access an unpopulated unit."); }
    return this->shell;
}
