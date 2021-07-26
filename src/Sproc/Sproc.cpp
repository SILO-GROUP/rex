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

#include "Sproc.h"
#include "sys/stat.h"

#define PARENT default

enum PIPE_FILE_DESCRIPTORS {
    READ_END = 0,
    WRITE_END = 1
};

enum READ_RESULTS {
    READ_EOF = 0,
    READ_PIPEOPEN_O_NONBLOCK = -1
};

enum FORK_STATES {
    FORK_FAILURE = -1,
    CHILD = 0
};

/* ------------------
 * HELPERS
 * ------------------ */

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

// SET PROCESS TO A CERTAIN IDENTITY CONTEXT
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
int Sproc::execute(std::string shell, std::string environment_file, std::string user_name, std::string group_name, std::string command, int LOG_LEVEL, std::string task_name, bool log_to_file, std::string logs_dir )
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
    if (! is_dir( logs_dir ) ) {
        int check = mkdir( logs_dir.c_str(), 0777 );
        if (! check ) {
            slog.log( E_FATAL, "Sprocket couldn't create the logs parent directory." );
        }
    }

    std::string timestamp = get_8601();

    std::string contained_dir = logs_dir + "/" + task_name;
    if (! is_dir( contained_dir ) ) {
        int check = mkdir( contained_dir.c_str(), 0777 );
        if (! check ) {
            slog.log( E_FATAL, "Sprocket couldn't create the instance log directory.");
        }
    }
    // set up the "Tee" with the parent
    std::string child_stdout_log_path = contained_dir + "/" + timestamp + ".stdout.log";
    std::string child_stderr_log_path = contained_dir + "/" + timestamp + ".stderr.log";

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

    // These cause a segfault when used with the I/O redirection happening around fork, pipe, dup2, execl...
    //std::cout.rdbuf( tee_out.rdbuf() );
    //std::cerr.rdbuf( tee_err.rdbuf() );
    // ....and I don't know why.

    // build the command to execute in the shell
    std::string sourcer = ". " + environment_file + " && " + command;

    // Show the user a debug print of what is going to be executed in the shell.
    slog.log(E_INFO, "[ '" + task_name + "' ] Shell call for loading: ``" + sourcer + "``.");

    // file descriptors for parent/child i/o
    int child_stdout_pipe[2];
    int child_stderr_pipe[2];

    // no longer really needed, but may be handy for more verbosity settings later
    //slog.log( E_DEBUG, "[ '" + task_name + "' ] STDIN/STDOUT/STDERR file descriptors created." );

    // man 3 pipe
    if (pipe(child_stdout_pipe) == -1 ) {
        slog.log(E_FATAL, "[ '" + task_name + "' ] STDOUT PIPE FAILED");
        return SPROC_RETURN_CODES::PIPE_FAILED;
    } else {
        // slog.log(E_DEBUG, "[ '" + task_name + "' ] file descriptors piped.");
    }

    // man 3 pipe
    if (pipe(child_stderr_pipe) == -1 ) {
        slog.log(E_FATAL, "[ '" + task_name + "' ] STDERR PIPE FAILED");
        return SPROC_RETURN_CODES::PIPE_FAILED;
    } else {
        slog.log(E_DEBUG, "[ '" + task_name + "' ] file descriptors piped.");
    }

    if (fcntl(child_stdout_pipe[READ_END], F_SETFD, FD_CLOEXEC) == -1) {
        perror("fcntl");
        exit(1);
    }

    if (fcntl(child_stdout_pipe[WRITE_END], F_SETFD, FD_CLOEXEC) == -1) {
        perror("fcntl");
        exit(1);
    }

    if (fcntl(child_stderr_pipe[READ_END], F_SETFD, FD_CLOEXEC) == -1) {
        perror("fcntl");
        exit(1);
    }

    if (fcntl(child_stderr_pipe[WRITE_END], F_SETFD, FD_CLOEXEC) == -1) {
        perror("fcntl");
        exit(1);
    }

    // fork a process
    pid_t pid = fork();
    //slog.log( E_DEBUG, "[ '" + task_name + "' ] Process forked. Reporting. (PID: " + std::to_string(pid) + ")" );

    switch ( pid ) {
        case FORK_STATES::FORK_FAILURE:
        {
            // fork failed
            slog.log(E_FATAL, "[ '" + task_name + "' ] Fork Failed.");
            exit( FORK_FAILED );
            break;
        }
        case FORK_STATES::CHILD:
        {
            // enter child process
            slog.log(E_DEBUG, "[ '" + task_name + "' ] Entering child process.");

            while ((dup2(child_stdout_pipe[WRITE_END], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
            while ((dup2(child_stderr_pipe[WRITE_END], STDERR_FILENO) == -1) && (errno == EINTR)) {}

            close( child_stdout_pipe[WRITE_END] );
            close( child_stdout_pipe[READ_END] );

            close( child_stderr_pipe[WRITE_END] );
            close( child_stderr_pipe[READ_END] );

            slog.log(E_INFO, "[ '" + task_name + "' ] TEE Logging enabled.");
            //slog.log(E_DEBUG, "[ '" + task_name + "' ] DUP2: child_*_pipe[1]->STD*_FILENO");

            // set identity context
            // set gid and uid
            int context_status = set_identity_context(task_name, user_name, group_name, slog);
            if (!(context_status)) {
                slog.log(E_FATAL, "[ '" + task_name + "' ] Identity context set failed.");
                return context_status;
            } else {
                slog.log( E_INFO, "[ '" + task_name + "' ] Identity context set as user '" + user_name + "' and group '" + group_name + "'." );
            }

            slog.log( E_DEBUG, "[ '" + task_name + "' ] EXECL_CALL_PARAM: " + sourcer.c_str() );
            slog.log( E_DEBUG,  "[ '" + task_name + "' ] CWD: " + get_current_dir_name() );

            // execute our big nasty thing
            // int ret = execlp( shell.c_str(), shell.c_str(), "-c", sourcer.c_str(), (char*)NULL );

            int ret = execl(shell.c_str(), shell.c_str(), "-c", sourcer.c_str(), (char*)NULL );

            // print something useful to debug with if execl fails
            slog.log(E_FATAL, "[ '" + task_name + "' ] ret code: " + std::to_string(ret) + "; errno: " + strerror(errno));
            // exit child -- if this is executing, you've had a failure

            exit(exit_code_raw);
        }

        PARENT:
        {
            // enter the parent process
            close(child_stdout_pipe[WRITE_END]);
            close(child_stderr_pipe[WRITE_END]);

            // buffers for reading from child fd's
            char stdout_buf[4096] = {0};
            char stderr_buf[4096] = {0};

            // will contain a set of file descriptors to monitor representing stdout and stderr of the child process
            fd_set readfds;

            // loop completion flags
            bool set_stdout_break = false;
            bool set_stderr_break = false;

            // read from fd until child completes -- signaled by stdout_break and stderr_break flags
            while ((! set_stderr_break ) or (! set_stdout_break)) {
                // clear it out to make sure it's clean
                FD_ZERO( & readfds );

                // add child stdout and stderr pipes (read end)
                FD_SET( child_stdout_pipe[READ_END], & readfds );
                FD_SET( child_stderr_pipe[READ_END], & readfds );

                // for some reason select needs the highest number of the fd +1 of its own input
                int highest_fd = child_stderr_pipe[READ_END] > child_stdout_pipe[READ_END] ? child_stderr_pipe[READ_END] : child_stdout_pipe[READ_END];

                // wait for one of the fd's to become readable
                if ( select( highest_fd + 1, &readfds, NULL, NULL, NULL ) >= 0 )
                { // can read any
                    if ( FD_ISSET( child_stdout_pipe[READ_END], &readfds ) )
                    { // can read child stdout pipe
                        // read and return the byte size of what was read
                        int stdout_count = read(child_stdout_pipe[READ_END], stdout_buf, sizeof(stdout_buf) - 1);

                        switch(stdout_count) { // switch on the count size to allow for error return handling
                            case READ_PIPEOPEN_O_NONBLOCK:
                                if ( errno == EINTR ) {
                                    continue;
                                } else {
                                    slog.log(E_FATAL, "Pipe reading issue with child STDOUT.");
                                    exit( PIPE_FAILED2 );
                                }
                            case READ_EOF:
                                // signal that STDOUT is complete
                                set_stdout_break = true;
                                break;
                            default:
                                if ( log_to_file ) {
                                    tee_out.write( stdout_buf, stdout_count );
                                    tee_out.flush();
                                } else {
                                    std::cout.write( stdout_buf, stdout_count );
                                    std::cout.flush();
                                }
                                // clear the buffer to prevent artifacts from previous loop
                                memset( &stdout_buf[0], 0, sizeof( stdout_buf ) -1 );
                        }
                    }
                    if ( FD_ISSET(  child_stderr_pipe[READ_END], & readfds ) ) {
                        // can read child stderr pipe
                        // so do so

                        // read and return the byte size of what was read
                        int stderr_count = read( child_stderr_pipe[READ_END], stderr_buf, sizeof( stderr_buf ) -1 );

                        // switch on the count size to allow for error return handling
                        switch( stderr_count ) {
                            case READ_RESULTS::READ_PIPEOPEN_O_NONBLOCK:
                                if ( errno == EINTR ) {
                                    continue;
                                } else {
                                    perror( "read stderr" );
                                    slog.log( E_FATAL, "Pipe reading issue with child STDERR." );
                                    exit( PIPE_FAILED3 );
                                }
                            case READ_RESULTS::READ_EOF:
                                // let the loop know the STDERR criteria has been met
                                set_stderr_break = true;
                                // continue the loop
                                continue;
                            default:
                                // write the buffer contents to the STDERR Tee
                                tee_err.write( stderr_buf, stderr_count );
                                // flush the TEE
                                tee_err.flush();
                                // clear the buffer to prevent artifacts from previous loop
                                memset( &stderr_buf[0], 0, sizeof( stderr_buf ) -1 );
                        }
                    }
                } else {
                    // select error, fatal, throw
                    slog.log( E_FATAL, "Fatal error, Unknown.");
                } // end select/if
            }

            close( child_stderr_pipe[READ_END] );
            close( child_stdout_pipe[READ_END] );

            // clean up Tee
            stdout_log.close();
            stderr_log.close();

            // wait for the child to exit
            while ( ( pid = waitpid(pid, &exit_code_raw, 0 ) ) == -1 ) {}

        }
    }
    return WEXITSTATUS( exit_code_raw );
}
