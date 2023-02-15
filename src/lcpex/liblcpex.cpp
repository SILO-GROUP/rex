#include "liblcpex.h"

std::string prefix_generator(
        std::string command,
        bool is_shell_command,
        std::string shell_path,
        std::string shell_execution_arg,
        bool supply_environment,
        std::string shell_source_subcommand,
        std::string environment_file_path
) {
    std::string prefix = "";
    if ( is_shell_command ) {

        // it's a shell command, so we need to set up a shell execution of the command
        prefix = shell_path;

        // if the shell takes an argument to execute a command, add it enclosed in quotes
        if (shell_execution_arg != "") {
            // add the execution arg
            prefix += " " + shell_execution_arg + " ";
        } else {
            // no execution arg, so add a space
            prefix += " ";
        }

        if (supply_environment) {
            // add the source subcommand
            prefix += "'" + shell_source_subcommand + " " + environment_file_path + " && " + command + "'";
        } else {
            // no environment supplied, so just add the command
            prefix += "'" + command + "'";
        }
    } else {
        // it's not a shell command, so we can just execute it directly
        prefix = command;
    }
    std::cout << "prefix: " << prefix << std::endl;
    return prefix;
}


int lcpex(
        std::string command,
        std::string stdout_log_file,
        std::string stderr_log_file,
        bool context_override,
        std::string context_user,
        std::string context_group,
        bool force_pty,
        bool is_shell_command,
        std::string shell_path,
        std::string shell_execution_arg,
        bool supply_environment,
        std::string shell_source_subcommand,
        std::string environment_file_path
) {

    // generate the prefix
    std::string prefix = prefix_generator(
            command,
            is_shell_command,
            shell_path,
            shell_execution_arg,
            supply_environment,
            shell_source_subcommand,
            environment_file_path
    );
    command = prefix;

    // if we are forcing a pty, then we will use the vpty library
    if( force_pty )
    {
        return exec_pty( command, stdout_log_file, stderr_log_file, context_override, context_user, context_group, supply_environment );
    }

    // otherwise, we will use the execute function
    return execute( command, stdout_log_file, stderr_log_file, context_override, context_user, context_group, supply_environment );
}

/**
 * @brief Set the close-on-exec flag of a file descriptor
 *
 * This function uses the `fcntl` function to set the close-on-exec flag
 * of the specified file descriptor `fd`. If the `fcntl` call fails,
 * the function will print an error message to `stderr` and exit with status 1.
 *
 * @param fd The file descriptor for which to set the close-on-exec flag
 */
void set_cloexec_flag(int fd)
{
    if (fcntl(fd, F_SETFD, FD_CLOEXEC) == -1)
    {
        perror("fcntl");
        exit(1);
    }
}

/**
 * @brief This function is executed by the child process to run a shell command
 *
 * @param context_override Indicates whether to override the current execution context
 * @param context_user The user to switch to for execution context
 * @param context_group The group to switch to for execution context
 * @param processed_command The command to be executed, after processing
 * @param fd_child_stdout_pipe The file descriptor for the child process's standard output pipe
 * @param fd_child_stderr_pipe The file descriptor for the child process's standard error pipe
 *
 * The run_child_process() function takes the parameters context_override, context_user, context_group, processed_command,
 * fd_child_stdout_pipe and fd_child_stderr_pipe.
 *
 * If context_override is set to true, the child process will run under the specified context_user and context_group.
 * If either the context_user or context_group does not exist, an error message will be displayed and the process will exit.
 *
 * The function first redirects the child process's standard output and standard error to pipes.
 *
 * If context_override is set to true, the child process sets its identity context using the set_identity_context() function.
 * If an error occurs while setting the identity context, a message will be displayed and the process will exit.
 *
 * Finally, the child process calls execvp() with the processed_command to run the shell command.
 * If the execvp() function fails, an error message is displayed.
 */
void run_child_process(bool context_override, const char* context_user, const char* context_group, char* processed_command[], int fd_child_stdout_pipe[], int fd_child_stderr_pipe[]) {
    while ((dup2(fd_child_stdout_pipe[WRITE_END], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
    while ((dup2(fd_child_stderr_pipe[WRITE_END], STDERR_FILENO) == -1) && (errno == EINTR)) {}

    if ( context_override ) {
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

    int exit_code = execvp(processed_command[0], processed_command);
    perror("failed on execvp in child");
    exit(exit_code);
}


int execute(
        std::string command,
        std::string stdout_log_file,
        std::string stderr_log_file,
        bool context_override,
        std::string context_user,
        std::string context_group,
        bool environment_supplied
){
    // this does three things:
    //  - execute a dang string as a subprocess command
    //  - capture child stdout/stderr to respective log files
    //  - TEE child stdout/stderr to parent stdout/stderr

    // turn our command string into something execvp can consume
    char ** processed_command = expand_env(command );

    // if the user chose to supply the environment, then we need to clear the environment
    // before we fork the process, so that the child process will inherit the environment
    // from the parent process
    if ( environment_supplied ) {
        clearenv();
    }

    // open file handles to the two log files we need to create for each execution
    FILE * stdout_log_fh = fopen( stdout_log_file.c_str(), "a+" );
    FILE * stderr_log_fh = fopen( stderr_log_file.c_str(), "a+" );

    // create the pipes for the child process to write and read from using its stdin/stdout/stderr
    int fd_child_stdout_pipe[2];
    int fd_child_stderr_pipe[2];

    if ( pipe(fd_child_stdout_pipe ) == -1 ) {
        perror( "child stdout pipe" );
        exit( 1 );
    }

    if ( pipe( fd_child_stderr_pipe ) == -1 ) {
        perror( "child stderr pipe" );
        exit( 1 );
    }

    // using O_CLOEXEC to ensure that the child process closes the file descriptors
    set_cloexec_flag( fd_child_stdout_pipe[READ_END] );
    set_cloexec_flag( fd_child_stderr_pipe[READ_END] );
    set_cloexec_flag( fd_child_stdout_pipe[WRITE_END] );
    set_cloexec_flag( fd_child_stderr_pipe[WRITE_END] );

    // status result basket for the parent process to capture the child's exit status
    int status = 616;
    pid_t pid = fork();

    switch( pid ) {
        case -1:
        {
            // fork failed
            perror("fork failure");
            exit(1);
        }

        case 0:
        {
            // child process
            run_child_process(
                    context_override,
                    context_user.c_str(),
                    context_group.c_str(),
                    processed_command,
                    fd_child_stdout_pipe,
                    fd_child_stderr_pipe
            );
        }

        default:
        {
            // parent process

            // The parent process has no need to access the entrance to the pipe, so fd_child_*_pipe[1|0] should be closed
            // within that process too:
            close(fd_child_stdout_pipe[WRITE_END]);
            close(fd_child_stderr_pipe[WRITE_END]);

            // attempt to write to stdout,stderr from child as well as to write each to file
            char buf[BUFFER_SIZE];

            // contains the byte count of the last read from the pipe
            ssize_t byte_count;

            // watched_fds for poll() to wait on
            struct pollfd watched_fds[2];

            // populate the watched_fds array

            // child STDOUT to parent
            watched_fds[CHILD_PIPE_NAMES::STDOUT_READ].fd = fd_child_stdout_pipe[READ_END];
            watched_fds[CHILD_PIPE_NAMES::STDOUT_READ].events = POLLIN;

            // child STDERR to parent
            watched_fds[CHILD_PIPE_NAMES::STDERR_READ].fd = fd_child_stderr_pipe[READ_END];
            watched_fds[CHILD_PIPE_NAMES::STDERR_READ].events = POLLIN;

            // number of files poll() reports as ready
            int num_files_readable;

            // loop flag
            bool break_out = false;

            // loop until we've read all the data from the child process
            while ( ! break_out ) {
                num_files_readable = poll(watched_fds, sizeof(watched_fds) / sizeof(watched_fds[0]), -1);
                if (num_files_readable == -1) {
                    // error occurred in poll()
                    perror("poll");
                    exit(1);
                }
                if (num_files_readable == 0) {
                    // poll reports no files readable
                    break_out = true;
                    break;
                }
                for (int this_fd = 0; this_fd < 2; this_fd++) {
                    if (watched_fds[this_fd].revents & POLLIN) {
                        // this pipe is readable
                        byte_count = read(watched_fds[this_fd].fd, buf, BUFFER_SIZE);

                        if (byte_count == -1) {
                            // error reading from pipe
                            perror("read");
                            exit(EXIT_FAILURE);
                        } else if (byte_count == 0) {
                            // reached EOF on one of the streams but not a HUP
                            // we've read all we can this cycle, so go to the next fd in the for loop
                            continue;
                        } else {
                            // byte count was sane
                            // write to stdout,stderr
                            if (this_fd == CHILD_PIPE_NAMES::STDOUT_READ) {
                                // the child's stdout pipe is readable
                                write(stdout_log_fh->_fileno, buf, byte_count);
                                write(STDOUT_FILENO, buf, byte_count);
                            } else if (this_fd == CHILD_PIPE_NAMES::STDERR_READ) {
                                // the child's stderr pipe is readable
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
                        break_out = true;
                    }
                    if (watched_fds[this_fd].revents & POLLHUP) {
                        // this pipe has hung up
                        close(watched_fds[this_fd].fd);
                        break_out = true;
                    }

                }
            }
            // wait for child to exit, capture status
            waitpid(pid, &status, 0);

            // close the log file handles
            fclose(stdout_log_fh);
            fclose(stderr_log_fh);
            if WIFEXITED(status) {
                return WEXITSTATUS(status);
            } else {
                return -617;
            }
        }
    }
}

