#include "libclpex_tty.h"

/**
 * @brief Safely prints a message and exits the program
 *
 * @param msg The message to print
 * @param ttyOrig A pointer to a struct termios representing the original terminal settings
 *
 * This function takes a message `msg` and a pointer to a struct termios `ttyOrig` as input.
 * The function first prints the `msg` to `stderr`.
 * Then, it calls `ttyResetExit()` with `ttyOrig` as its argument to reset the terminal settings.
 * Finally, the function exits the program with exit code 1.
 */
void safe_perror( const char * msg, struct termios * ttyOrig )
{
    std::cerr << msg << std::endl;
    ttyResetExit( ttyOrig );
    exit(1);
}


/**
 * @brief Executes the child process
 *
 * @param fd_child_stderr_pipe A file descriptor array for the child process's stderr pipe
 * @param processed_command An array of char pointers representing the command and its arguments to be executed
 * @param ttyOrig A pointer to a struct termios representing the original terminal settings
 * @param context_override A flag indicating whether to override the process's execution context
 * @param context_user The username to use for the execution context if `context_override` is `true`
 * @param context_group The group name to use for the execution context if `context_override` is `true`
 *
 * This function takes an array of file descriptors `fd_child_stderr_pipe` for the child process's stderr pipe,
 * an array of char pointers `processed_command` representing the command and its arguments to be executed,
 * a pointer to a struct termios `ttyOrig` representing the original terminal settings,
 * a flag `context_override` indicating whether to override the process's execution context,
 * a string `context_user` representing the username to use for the execution context if `context_override` is `true`,
 * and a string `context_group` representing the group name to use for the execution context if `context_override` is `true`.
 *
 * The function first redirects the child process's stderr to the write end of the stderr pipe.
 * If `context_override` is `true`, the function sets the process's execution context using `set_identity_context()`.
 * If `context_override` is `false`, the function does nothing.
 * Finally, the function executes the command specified in `processed_command` using `execvp()`.
 * If the execution of `execvp()` fails, the function calls `safe_perror()` to print a message and exit the program.
 */
void run_child_process( int fd_child_stderr_pipe[2], char * processed_command[], struct termios * ttyOrig, bool context_override, std::string context_user, std::string context_group )
{
    // redirect stderr to the write end of the stderr pipe
    // close the file descriptor STDERR_FILENO if it was previously open, then (re)open it as a copy of
    while ((dup2(fd_child_stderr_pipe[WRITE_END], STDERR_FILENO) == -1) && (errno == EINTR)) {}

    // close the write end of the stderr pipe
    close( fd_child_stderr_pipe[WRITE_END] );

    // if the user has specified a context override, set the context
    if ( context_override )
    {
        int context_result = set_identity_context(context_user, context_group);
        switch (context_result) {
            case IDENTITY_CONTEXT_ERRORS::ERROR_NONE:
                break;
            case IDENTITY_CONTEXT_ERRORS::ERROR_NO_SUCH_USER:
                std::cerr << "lcpex: Aborting: context user not found: " << context_user << std::endl;
                exit(1);
                break;
            case IDENTITY_CONTEXT_ERRORS::ERROR_NO_SUCH_GROUP:
                std::cerr << "lcpex: Aborting: context group not found: " << context_group << std::endl;
                exit(1);
                break;
            case IDENTITY_CONTEXT_ERRORS::ERROR_SETGID_FAILED:
                std::cerr << "lcpex: Aborting: Setting GID failed: " << context_user << "/" << context_group << std::endl;
                exit(1);
                break;
            case IDENTITY_CONTEXT_ERRORS::ERROR_SETUID_FAILED:
                std::cerr << "lcpex: Aborting: Setting UID failed: " << context_user << "/" << context_group << std::endl;
                exit(1);
                break;
            default:
                std::cerr << "lcpex: Aborting: Unknown error while setting identity context." << std::endl;
                exit(1);
                break;
        }
    }

    // execute the dang command, print to stdout, stderr (of parent), and dump to file for each!!!!
    // (and capture exit code in parent)
    int exit_code = execvp(processed_command[0], processed_command);
    safe_perror("failed on execvp in child", ttyOrig );
    exit(exit_code);
}

// this does three things:
//  - execute a dang string as a subprocess command
//  - capture child stdout/stderr to respective log files
//  - TEE child stdout/stderr to parent stdout/stderr
int exec_pty(
        std::string command,
        std::string stdout_log_file,
        std::string stderr_log_file,
        bool context_override,
        std::string context_user,
        std::string context_group,
        bool environment_supplied
) {
    // initialize the terminal settings obj
    struct termios ttyOrig;

    // if the user chose to supply the environment, then we need to clear the environment
    // before we fork the process, so that the child process will inherit the environment
    // from the parent process
    if ( environment_supplied ) {
        clearenv();
    }

    // turn our command string into something execvp can consume
    char ** processed_command = expand_env(command );

    // open file handles to the two log files we need to create for each execution
    FILE * stdout_log_fh = fopen( stdout_log_file.c_str(), "a+" );
    FILE * stderr_log_fh = fopen( stderr_log_file.c_str(), "a+" );

    // create the pipes for the child process to write and read from using its stderr
    int fd_child_stderr_pipe[2];

    if ( pipe( fd_child_stderr_pipe ) == -1 ) {
        safe_perror( "child stderr pipe", &ttyOrig );
        exit( 1 );
    }

    // using O_CLOEXEC to ensure that the child process closes the file descriptors
    if ( fcntl( fd_child_stderr_pipe[READ_END], F_SETFD, FD_CLOEXEC ) == -1 ) { perror("fcntl"); exit(1); }

    //    // same for stderr write
    if ( fcntl( fd_child_stderr_pipe[WRITE_END], F_SETFD, FD_CLOEXEC ) == -1 ) { perror("fcntl"); exit(1); }

    // status result basket for the parent process to capture the child's exit status
    int status = 616;

    // start ptyfork integration
    char slaveName[MAX_SNAME];
    int masterFd;
    struct winsize ws;

    /* Retrieve the attributes of terminal on which we are started */
    if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1)
        safe_perror("tcgetattr", &ttyOrig);
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
        safe_perror("ioctl-TIOCGWINSZ", &ttyOrig );

    pid_t pid = ptyFork( &masterFd, slaveName, MAX_SNAME, &ttyOrig, &ws );

    switch( pid ) {
        case -1:
        {
            // fork failed
            safe_perror("ptyfork failure", &ttyOrig );
            exit(1);
        }

        case 0:
        {
            // child process
            run_child_process( fd_child_stderr_pipe, processed_command, &ttyOrig, context_override, context_user, context_group );
        }

        default:
        {
            // parent process
            // start ptyfork integration
            ttySetRaw(STDIN_FILENO, &ttyOrig);


            // The parent process has no need to access the entrance to the pipe
            close(fd_child_stderr_pipe[WRITE_END]);

            // attempt to write to stdout,stderr from child as well as to write each to file
            char buf[BUFFER_SIZE];

            // contains the byte count of the last read from the pipe
            ssize_t byte_count;

            // watched_fds for poll() to wait on
            struct pollfd watched_fds[3];

            // populate the watched_fds array

            // child in to parent
            watched_fds[0].fd = STDIN_FILENO;
            watched_fds[0].events = POLLIN;

            // child pty to parent (stdout)
            watched_fds[1].fd = masterFd;
            watched_fds[1].events = POLLIN;

            // child stderr to parent
            watched_fds[2].fd = fd_child_stderr_pipe[READ_END];
            watched_fds[2].events = POLLIN;

            // number of files poll() reports as ready
            int num_files_readable;

            // loop flag
            bool break_out = false;

            // loop until we've read all the data from the child process
            while ( ! break_out ) {
                num_files_readable = poll(watched_fds, sizeof(watched_fds) / sizeof(watched_fds[0]), -1);

                if (num_files_readable == -1) {
                    // error occurred in poll()
                    safe_perror("poll", &ttyOrig );
                    exit(1);
                }

                if (num_files_readable == 0) {
                    // poll reports no files readable
                    break_out = true;
                    break;
                }

                for (int this_fd = 0; this_fd < 3; this_fd++) {
                    if (watched_fds[this_fd].revents & POLLIN) {
                        // this pipe is readable
                        byte_count = read(watched_fds[this_fd].fd, buf, BUFFER_SIZE);

                        if (byte_count == -1) {
                            // error reading from pipe
                            safe_perror("read", &ttyOrig );
                            exit(EXIT_FAILURE);
                        } else if (byte_count == 0) {
                            // reached EOF on one of the streams but not a HUP
                            // we've read all we can this cycle, so go to the next fd in the for loop
                            continue;
                        } else {
                            // byte count was sane
                            // write to stdout,stderr
                            if (this_fd == 0) {
                                // parent stdin received, write to child pty (stdin)
                                write(masterFd, buf, byte_count);
                            } else if (this_fd == 1 ) {
                                // child pty sent some stuff, write to parent stdout and log
                                write(stdout_log_fh->_fileno, buf, byte_count);
                                write(STDOUT_FILENO, buf, byte_count);
                            } else if ( this_fd == 2 ){
                                //the child's stderr pipe sent some stuff, write to parent stderr and log
                                write(stderr_log_fh->_fileno, buf, byte_count);
                                write(STDERR_FILENO, buf, byte_count);
                            } else {
                                // this should never happen
                                perror("Logic error!");
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                    if (watched_fds[this_fd].revents & POLLERR) {
                        close(watched_fds[this_fd].fd);
                        //break_out = true;
                        continue;
                    }
                    if (watched_fds[this_fd].revents & POLLHUP) {
                        // this pipe has hung up
                        close(watched_fds[this_fd].fd);
                        break_out = true;
                        break;
                    }
                }
            }
            // wait for child to exit, capture status
            waitpid(pid, &status, 0);

            // close the log file handles
            fclose(stdout_log_fh);
            fclose(stderr_log_fh);
            ttyResetExit( &ttyOrig);
            if WIFEXITED(status) {
                return WEXITSTATUS(status);
            } else {
                return -617;
            }
        }
    }
}

