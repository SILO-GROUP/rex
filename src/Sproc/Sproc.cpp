#include "Sproc.h"
#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#include "../Logger/Logger.h"

int username_to_uid( std::string username, int & uid )
{
    // assume failure unless proven otherwise
    int r_code = false;

    struct passwd * pw;
    if ( ( pw = getpwnam( username.c_str() ) ) != NULL )
    {
        // successful user lookup
        r_code = true;
        uid = pw->pw_uid;
    } else {
        // failed lookup, do nothing, assumed failure
    }
    return r_code;
};

int groupname_to_gid( std::string groupname, int & gid )
{
    int r_code = false;

    struct group * gp;
    if ( ( gp = getgrnam( groupname.c_str() ) ) != NULL )
    {
        r_code = true;
        gid = gp->gr_gid;
    } else {
        // failed lookup, do nothing, assumed failure
    }
    return r_code;
}

/// Sproc::execute
///
/// \param input - The commandline input to execute.
/// \return - The return code of the execution of input in the calling shell.
int Sproc::execute(std::string run_as, std::string group, std::string command )
{
    Logger slog = Logger( E_INFO, "_sproc" );

    // the run_as_uid to capture the run_as_uid to run as
    int run_as_uid;
    int run_as_gid;

    slog.log( E_DEBUG, "Attempt: Running as user '" + run_as + "'.");
    slog.log( E_DEBUG, "Attempt: Running as group '" + group + "'.");

    if ( username_to_uid( run_as, run_as_uid ) )
    {
        slog.log( E_DEBUG, "UID of '" + run_as + "' is  '" + std::to_string( run_as_uid ) + "'." );
    } else {
        slog.log( E_FATAL, "Failed to look up UID for '" + run_as + "'.");
        return -404;
    }

    if ( groupname_to_gid( group, run_as_gid ) )
    {
        slog.log( E_DEBUG, "GID of '" + group + "' is '" + std::to_string( run_as_gid ) + "'." );
    } else {
        slog.log( E_FATAL, "Failed to look up DID for '" + group + "'.");
        return -404;
    }

    // if you get this return value, it's an issue with this method and not your
    // called executable.
    int exit_code_raw = -666;

    // fork a process
    int pid = fork();

    if ( pid == 0 )
    {
        // child process
        int setegidval = setegid( run_as_gid );
        if ( setegidval == 0 )
        {
            slog.log( E_INFO, "Successfully set GID to '" + std::to_string(run_as_gid) + "' (" + group + ")." );
        } else {
            slog.log( E_FATAL, "Failed to set GID.  Panicking. (setegid: " + std::to_string( setegidval ) + "/" + std::to_string(errno) + ")" );
            return -401;
        }

        if ( seteuid( run_as_uid ) == 0 )
        {
            slog.log( E_INFO, "Successfully set UID to '" + std::to_string(run_as_uid) + "' (" + run_as + ")." );
        } else {
            slog.log( E_FATAL, "Failed to set UID.  Panicking." );
            return -401;
        }

        exit_code_raw = system( command.c_str() );
        exit( WEXITSTATUS( exit_code_raw ) );
    } else if ( pid > 0 )
    {
        // parent process
        while ( ( pid = waitpid( pid, &exit_code_raw, 0 ) ) == -1 ) {}
    } else {
        // fork failed
        slog.log( E_FATAL, "Fork Failed");
    }
    return WEXITSTATUS( exit_code_raw );


}