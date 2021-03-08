#include "Sproc.h"
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#include <fstream>
#include "../Logger/Logger.h"
#include "errno.h"
#include <cstring>
#include "fcntl.h"


// converts username to UID
// returns false on failure
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

// converts group name to GID
// returns false on failure
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

// teebuf constructor
teebuf::teebuf(std::streambuf *sb1, std::streambuf *sb2): sb1(sb1), sb2(sb2)
{}

// teebuf overflow method
int teebuf::overflow( int c )
{
    if (c == EOF)
    {
        return !EOF;
    }
    else
    {
        int const r1 = sb1->sputc(c);
        int const r2 = sb2->sputc(c);
        return r1 == EOF || r2 == EOF ? EOF : c;
    }
}

// teebuf sync method
int teebuf::sync()
{
    int const r1 = sb1->pubsync();
    int const r2 = sb2->pubsync();
    return r1 == 0 && r2 == 0 ? 0 : -1;
}

// teestream impl constructor
teestream::teestream(std::ostream &o1, std::ostream &o2) : std::ostream( &tbuf), tbuf( o1.rdbuf(), o2.rdbuf() )
{}

enum PIPE_FILE_DESCRIPTORS {
    READ_FD = 0,
    WRITE_FD = 1
};


enum SPROC_RETURN_CODES {
    SUCCESS = true,
    UID_NOT_FOUND = -404,
    GID_NOT_FOUND = -405,
    SET_GID_FAILED = -401,
    SET_UID_FAILED = -404,
    EXEC_FAILURE_GENERAL = -666,
    DUP2_FAILED = -999,
    PIPE_FAILED = -998
};

enum FORK_STATES {
    FORK_FAILURE = -1,
    CHILD = 0
};

int set_identity_context( std::string task_name, std::string user_name, std::string group_name, Logger slog ) {
    // the UID and GID for the username and groupname provided for context setting
    int context_user_id;
    int context_group_id;

    // show the user something usable in debug mode
    slog.log( E_DEBUG, "[ '" + task_name + "' ] Attempt: Running as user '" + user_name + "'.");
    slog.log( E_DEBUG, "[ '" + task_name + "' ] Attempt: Running as group_name '" + group_name + "'.");

    // convert username to UID
    if ( username_to_uid(user_name, context_user_id ) )
    {
        slog.log( E_DEBUG, "[ '" + task_name + "' ] UID of '" + user_name + "' is  '" + std::to_string(context_user_id ) + "'." );
    } else {
        slog.log( E_FATAL, "[ '" + task_name + "' ] Failed to look up UID for '" + user_name + "'.");
        return SPROC_RETURN_CODES::UID_NOT_FOUND;
    }

    // convert group name to GID
    if ( groupname_to_gid(group_name, context_group_id ) )
    {
        slog.log( E_DEBUG, "[ '" + task_name + "' ] GID of '" + group_name + "' is '" + std::to_string(context_group_id ) + "'." );
    } else {
        slog.log( E_FATAL, "[ '" + task_name + "' ] Failed to look up GID for '" + group_name + "'.");
        return SPROC_RETURN_CODES::GID_NOT_FOUND;
    }

    if (setgid(context_group_id) == 0) {
        slog.log(E_DEBUG,
                 "[ '" + task_name + "' ] Successfully set GID to '" + std::to_string(context_group_id) + "' (" +
                 group_name + ").");
    } else {
        slog.log(E_FATAL, "[ '" + task_name + "' ] Failed to set GID.  Panicking.");
        return SPROC_RETURN_CODES::SET_GID_FAILED;
    }

    if (setuid(context_user_id) == 0) {
        slog.log(E_DEBUG,
                 "[ '" + task_name + "' ] Successfully set UID to '" + std::to_string(context_user_id) + "' (" +
                 user_name + ").");
    } else {
        slog.log(E_FATAL, "[ '" + task_name + "' ] Failed to set UID.  Panicking.");
        return SPROC_RETURN_CODES::SET_UID_FAILED;
    }

    return SPROC_RETURN_CODES::SUCCESS;
}

/// Sproc::execute
///
/// \param input - The commandline input to execute.
/// \return - The return code of the execution of input in the calling shell.
int Sproc::execute(std::string shell, std::string environment_file, std::string user_name, std::string group_name, std::string command, int LOG_LEVEL, std::string task_name )
{
    // the logger
    Logger slog = Logger( LOG_LEVEL, "_sproc" );

    // if you get this return value, it's an issue with this method and not your
    // called executable.
    int exit_code_raw = SPROC_RETURN_CODES::EXEC_FAILURE_GENERAL;

    // An explanation is due here:
    // We want to log the STDOUT and STDERR of the child process, while still displaying them in the parent, in a way
    // that does not interfere with, for example libcurses compatibility.

    // To simplify the handling of I/O, we will "tee" STDOUT and STDERR of the parent to respective log files.

    // Then fork(), and exec() the command to execute in the child, and link its STDOUT/STDERR to the parents' in
    // realtime.

    // Since the parent has a Tee between STDOUT/STDOUT_LOG and another between STDERR/STDERR_LOG, simply piping the
    // child STDOUT/STDERR to the parent STDOUT/STDERR should simplify I/O redirection happening here without
    // potentially corrupting user interaction with TUIs in the processes.  This should give us our log and our output
    // in as hands off a way as possible with as few assumptions as possible, while still doing this in a somewhat C++-y
    // way.


    // set up the "Tee" with the parent
    std::string child_stdout_log_path = "./stdout.log";
    std::string child_stderr_log_path = "./stderr.log";

    std::ofstream stdout_log;
    std::ofstream stderr_log;

    stdout_log.open( child_stdout_log_path.c_str(), std::ofstream::out | std::ofstream::app );
    stderr_log.open( child_stderr_log_path.c_str(), std::ofstream::out | std::ofstream::app );

    // avoid cyclic dependencies between stdout and tee_out
    std::ostream tmp_stdout( std::cout.rdbuf() );
    std::ostream tmp_stderr( std::cerr.rdbuf() );

    // writing to this ostream derivative will write to stdout log file and std::cout
    teestream tee_out(tmp_stdout, stdout_log);
    teestream tee_err(tmp_stderr, stderr_log);

    // pop the cout/cerr buffers to the appropriate Tees' buffers
    std::cout.rdbuf( tee_out.rdbuf() );
    std::cerr.rdbuf( tee_err.rdbuf() );
    // end - "set up the 'Tee' with the parent"
    slog.log( E_INFO, "Tee Logging enabled for \"" + task_name + "\"");


    // build the command to execute in the shell
    std::string sourcer = ". " + environment_file + " && " + command;
    // Show the user a debug print of what is going to be executed in the shell.
    slog.log(E_DEBUG, "[ '" + task_name + "' ] Shell call for loading: ``" + sourcer + "``.");

    // file descriptors for parent/child i/o
    int stdout_filedes[2];
    slog.log( E_DEBUG, "[ '" + task_name + "' ] STDIN/STDOUT/STDERR file descriptors created." );

    // man 3 pipe
    if (pipe(stdout_filedes) == -1 ) {
        slog.log(E_FATAL, "[ '" + task_name + "' ] PIPE FAILED");
        return SPROC_RETURN_CODES::PIPE_FAILED;
    } else {
        slog.log(E_DEBUG, "[ '" + task_name + "' ] file descriptors piped.");
    }


 //   // avoids the need to take any explicit action within the child process to close file descriptors
  //  if (fcntl(stdout_filedes[READ_FD], F_SETFD, FD_CLOEXEC) == -1) {
   //     perror("fcntl");
    //    exit(1);
   // }

    // fork a process
    pid_t pid = fork();
    slog.log( E_DEBUG, "[ '" + task_name + "' ] Process forked. Reporting. (PID: " + std::to_string(pid) + ")" );


    switch ( pid ) {
        case FORK_STATES::FORK_FAILURE:
        {
            // fork failed
            slog.log(E_FATAL, "[ '" + task_name + "' ] Fork Failed.");
            break;
        }
        case FORK_STATES::CHILD:
        {
            // enter child process
            slog.log(E_DEBUG, "[ '" + task_name + "' ] Entering child process.");

            while ((dup2(stdout_filedes[WRITE_FD], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
            close( stdout_filedes[WRITE_FD] );
            close( stdout_filedes[READ_FD] );
            slog.log(E_DEBUG, "[ '" + task_name + "' ] DUP2 on stdout_filedes[1]->STDOUT_FILENO in child.");


            // set identity context
            // set gid and uid
            int context_status = set_identity_context(task_name, user_name, group_name, slog);
            if (!(context_status)) {
                slog.log(E_FATAL, "[ '" + task_name + "' ] Identity context switch failed.");
                return context_status;
            }


            // exit_code_raw = system( sourcer.c_str() );
            int ret = execl("/bin/sh", "/bin/sh", "-c", sourcer.c_str(), (char *) NULL);

            // print something useful to debug with if execl fails
            slog.log(E_FATAL, "ret code: " + std::to_string(ret) + "; errno: " + strerror(errno));
            // exit child -- if this is executing, you've had a failure

            exit(exit_code_raw);
        }

        default:
        {
            // parent process
            close(stdout_filedes[WRITE_FD]);
            // ---
            // clean up Tee
            stdout_log.close();
            stderr_log.close();


            char buffer[1000] = {0};
            std::cout.flush();

            // read from fd until child completes
            while ( 1 ) {
                ssize_t count = read(stdout_filedes[READ_FD], buffer, sizeof(buffer));
                if (count == -1) {
                    if (errno == EINTR) {
                        continue;
                    } else {
                        perror("read");
                        exit(1);
                    }
                } else if (count == 0) {
                    break;
                } else {
                    std::cout << buffer;
                    std::cout.flush();
                    memset(&buffer[0], 0, sizeof(buffer));
                    //                    handle_child_process_output(buffer, count);
                }
            }


            while ((pid = waitpid(pid, &exit_code_raw, 0)) == -1) {}
            //waitpid( pid, &exit_code_raw, 0 );


        }
    }
    return WEXITSTATUS( exit_code_raw );
}
