#include "liblcpex.h"
#include <cstring>
#include <stdexcept>
#include <signal.h>
#include <string>
#include "../logger/Logger.h"
#include "FileHandle.h"
#define BUFFER_SIZE 1024


// Initialize logger
static Logger logger(E_INFO, "lcpex");
std::string user = logger.get_user_name();
std::string group = logger.get_group_name();

void signal_handler(int sig) {
    std::string command = "signal_handler";        // Command context

    // Create an instance of Logger (you can change the log level and mask accordingly)
    // Log the signal handling
    if (sig == SIGCHLD) {
        // Log that SIGCHLD was received, but leave the exit status to the parent
        logger.log_to_json_file("E_INFO", "SIGCHLD received. A child process ended.", user, group, command);
    } else if (sig == SIGINT) {
        logger.log_to_json_file("E_FATAL", "SIGINT received. Gracefully terminating...", user, group, command);
    } else if (sig == SIGTERM) {
        logger.log_to_json_file("E_FATAL", "SIGTERM received. Terminating...", user, group, command);
    } else if (sig == SIGSEGV) {
        logger.log_to_json_file("E_FATAL", "SIGSEGV received. Possible segmentation fault.", user, group, command);
    } else {
        logger.log_to_json_file("E_FATAL", "Unhandled signal received", user, group, command);
    }
}

// Setup signal registrations
void setup_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = signal_handler; // <-- handler function
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    std::string command = "setup_signal_handlers"; // Command context

    // SIGCHLD
    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
        std::string error_message = "Failed to set SIGCHLD handler: " + std::string(strerror(errno));
        logger.log_to_json_file("E_FATAL", error_message, user, group, command); // Log to JSON file
    }

    // SIGINT
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        std::string error_message = "Failed to set SIGINT handler: " + std::string(strerror(errno));
        logger.log_to_json_file("E_FATAL", error_message, user, group, command); // Log to JSON file
    }

    // SIGTERM
    if (sigaction(SIGTERM, &sa, nullptr) == -1) {
        std::string error_message = "Failed to set SIGTERM handler: " + std::string(strerror(errno));
        logger.log_to_json_file("E_FATAL", error_message, user, group, command); // Log to JSON file
    }

    // SIGSEGV
    if (sigaction(SIGSEGV, &sa, nullptr) == -1) {
        std::string error_message = "Failed to set SIGSEGV handler: " + std::string(strerror(errno));
        logger.log_to_json_file("E_FATAL", error_message, user, group, command); // Log to JSON file
    }
}

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
        if ( shell_execution_arg != "" )
        {
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

    // Log the message to JSON file
    logger.log_to_json_file("E_INFO", "LAUNCHER: " + prefix, user, group, command);
    //logger.log(E_INFO, "LAUNCHER: " + prefix);
    return prefix;

}


int lcpex(
        std::string command,
        FILE * stdout_log_fh,
        FILE * stderr_log_fh,
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
    setup_signal_handlers();

    // if we are forcing a pty, then we will use the vpty library
    if( force_pty )
    {
        return exec_pty( command, stdout_log_fh, stderr_log_fh, context_override, context_user, context_group, supply_environment );
    }

    // otherwise, we will use the execute function
    return execute( command, stdout_log_fh, stderr_log_fh, context_override, context_user, context_group, supply_environment );
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
    std::string command = "set_cloexec_flag";
    if (fcntl(fd, F_SETFD, FD_CLOEXEC) == -1)
    {
        std::string error_message = "fcntl(F_SETFD) failed for fd " + std::to_string(fd);

        logger.log_to_json_file("E_FATAL", error_message, user, group, command);

        throw std::runtime_error("fcntl(F_SETFD) failed for fd " + std::to_string(fd));
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
    // Close unused pipe ends to avoid leaks

    close(fd_child_stdout_pipe[READ_END]);
    close(fd_child_stdout_pipe[WRITE_END]);
    close(fd_child_stderr_pipe[READ_END]);
    close(fd_child_stderr_pipe[WRITE_END]);
    std::string command = "run_child_process";    // Command context

    if ( context_override ) {
        int context_result = set_identity_context(context_user, context_group);
        switch (context_result) {
            case IDENTITY_CONTEXT_ERRORS::ERROR_NONE:
                break;
            case IDENTITY_CONTEXT_ERRORS::ERROR_NO_SUCH_USER:
                logger.log(E_FATAL, "Aborting: context user not found: " + std::string(context_user));
                logger.log_to_json_file("E_FATAL", "Aborting: context user not found: " + std::string(context_user), user, group, command);
                _exit(1);
                break;
            case IDENTITY_CONTEXT_ERRORS::ERROR_NO_SUCH_GROUP:
                logger.log(E_FATAL, "Aborting: context group not found: " + std::string(context_group));
                logger.log_to_json_file("E_FATAL", "Aborting: context group not found: " + std::string(context_group), user, group, command);
                _exit(1);
                break;
            case IDENTITY_CONTEXT_ERRORS::ERROR_SETGID_FAILED:
                logger.log(E_FATAL, "Aborting: Setting GID failed: " + std::string(context_user) + "/" + std::string(context_group));
                logger.log_to_json_file("E_FATAL", "Aborting: Setting GID failed: " + std::string(context_user) + "/" + std::string(context_group), user, group, command);
                _exit(1);
                break;
            case IDENTITY_CONTEXT_ERRORS::ERROR_SETUID_FAILED:
                logger.log(E_FATAL, "Aborting: Setting UID failed: " + std::string(context_user) + "/" + std::string(context_group));
                logger.log_to_json_file("E_FATAL", "Aborting: Setting UID failed: " + std::string(context_user) + "/" + std::string(context_group), user, group, command);
                _exit(1);
                break;
            default:
                logger.log(E_FATAL, "Aborting: Unknown error while setting identity context.");
                logger.log_to_json_file("E_FATAL", "Aborting: Unknown error while setting identity context.", user, group, command);
                _exit(1);
                break;
        }
    }
    int exit_code = execvp(processed_command[0], processed_command);
    logger.log_to_json_file("E_FATAL", "failed on execvp in child", user, group, command);
    logger.log(E_FATAL, "failed on execvp in child");
    _Exit(exit_code);
}

int execute(
        std::string command,
        FILE * stdout_log_fh,
        FILE * stderr_log_fh,
        bool context_override,
        std::string context_user,
        std::string context_group,
        bool environment_supplied
){

    try {
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
        // this breaks reuse of env variables in between executions
        //clearenv();
    }

    // create the pipes for the child process to write and read from using its stdin/stdout/stderr
    int fd_child_stdout_pipe[2];
    int fd_child_stderr_pipe[2];

    if ( pipe(fd_child_stdout_pipe ) == -1 ) {
        throw std::runtime_error("Failed to create stdout pipe");
    }

    if ( pipe( fd_child_stderr_pipe ) == -1 ) {
        close(fd_child_stdout_pipe[0]);
        close(fd_child_stdout_pipe[1]);
        throw std::runtime_error("Failed to create stderr pipe");
    }

    // using O_CLOEXEC to ensure that the child process closes the file descriptors
    set_cloexec_flag( fd_child_stdout_pipe[READ_END] );
    set_cloexec_flag( fd_child_stderr_pipe[READ_END] );
    set_cloexec_flag( fd_child_stdout_pipe[WRITE_END] );
    set_cloexec_flag( fd_child_stderr_pipe[WRITE_END] );

    // status result basket for the parent process to capture the child's exit status
    int status;
    pid_t pid = fork();

    switch( pid ) {
        case -1:
        {
            // fork failed
            logger.log(E_FATAL, "fork failure: " + std::string(strerror(errno)));
            logger.log_to_json_file("E_FATAL", "fork failure: " + std::string(strerror(errno)), user, group, command);
            close(fd_child_stdout_pipe[0]);      // Pipe Leaks on Error Paths
            close(fd_child_stdout_pipe[1]);      // Pipe Leaks on Error Paths
            close(fd_child_stderr_pipe[0]);      // Pipe Leaks on Error Paths
            close(fd_child_stderr_pipe[1]);      // Pipe Leaks on Error Paths
            throw std::runtime_error("fork() failed: " + std::string(strerror(errno)));
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

            FdGuard stdout_read_guard(fd_child_stdout_pipe[READ_END]);
            FdGuard stderr_read_guard(fd_child_stderr_pipe[READ_END]);
            FdGuard stdout_write_guard(fd_child_stdout_pipe[WRITE_END]);
            FdGuard stderr_write_guard(fd_child_stderr_pipe[WRITE_END]);

            // We don't need WRITE_ENDs in parent, close them now
            stdout_write_guard.reset();  // Closes and sets fd to -1
            stderr_write_guard.reset();

            // The parent process has no need to access the entrance to the pipe, so fd_child_*_pipe[1|0] should be closed
            // within that process too:


            // attempt to write to stdout,stderr from child as well as to write each to file
            char buf[BUFFER_SIZE] = {0};
            std::strncpy(buf, command.c_str(), BUFFER_SIZE - 1);
            buf[BUFFER_SIZE - 1] = '\0';
            // contains the byte count of the last read from the pipe
            ssize_t byte_count;

            // watched_fds for poll() to wait on
            struct pollfd watched_fds[2];

            // populate the watched_fds array

            // child STDOUT to parent
            watched_fds[CHILD_PIPE_NAMES::STDOUT_READ].fd = stdout_read_guard.get();
            watched_fds[CHILD_PIPE_NAMES::STDOUT_READ].events = POLLIN;

            // child STDERR to parent
            watched_fds[CHILD_PIPE_NAMES::STDERR_READ].fd = stderr_read_guard.get();
            watched_fds[CHILD_PIPE_NAMES::STDERR_READ].events = POLLIN;

            // number of files poll() reports as ready
            int num_files_readable;

            // loop flag
            bool break_out = false;

            // loop until we've read all the data from the child process
            while ( ! break_out ) {
                num_files_readable = poll(watched_fds, sizeof(watched_fds) / sizeof(watched_fds[0]), -1);
                // after the poll() call, add a check to see if both pipes are closed
                if (!(watched_fds[CHILD_PIPE_NAMES::STDOUT_READ].events & POLLIN) &&
                    !(watched_fds[CHILD_PIPE_NAMES::STDERR_READ].events & POLLIN)) {
                    break_out = true;
                }

                if (num_files_readable == -1) {
                    // error occurred in poll()
                    logger.log(E_FATAL, "poll() failed: " + std::string(strerror(errno)));
                    logger.log_to_json_file("E_FATAL", "poll() failed", user, group, command);
                    throw std::runtime_error("poll() failed");
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
                            if (errno == EAGAIN) { continue; } else {
                                // error reading from pipe
                                logger.log(E_FATAL, "Error while reading: " + std::string(strerror(errno)));
                                logger.log_to_json_file("E_FATAL", "Error while reading from pipe", user, group, command);
                                stdout_read_guard.reset(-1);
                                stderr_read_guard.reset(-1);
                                exit(EXIT_FAILURE);
                            }

                        } else if (byte_count == 0) {
                            // reached EOF on one of the streams but not a HUP
                            // we've read all we can this cycle, so go to the next fd in the for loop
                            continue;
                        } else {
                            // byte count was sane
                            // write to stdout,stderr
                            if (this_fd == CHILD_PIPE_NAMES::STDOUT_READ) {
                                // the child's stdout pipe is readable
                                write_all(stdout_log_fh->_fileno, buf, byte_count);
                                write_all(STDOUT_FILENO, buf, byte_count);
                            } else if (this_fd == CHILD_PIPE_NAMES::STDERR_READ) {
                                // the child's stderr pipe is readable
                                write_all(stderr_log_fh->_fileno, buf, byte_count);
                                write_all(STDERR_FILENO, buf, byte_count);
                            } else {
                                // this should never happen
                                logger.log(E_FATAL, "Logic error: unexpected pipe index.");
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                    if (watched_fds[this_fd].revents & POLLERR) {
                        if (this_fd == CHILD_PIPE_NAMES::STDOUT_READ) {
                            stdout_read_guard.reset(-1);
                        } else if (this_fd == CHILD_PIPE_NAMES::STDERR_READ) {
                            stderr_read_guard.reset(-1);
                        }
                        break_out = true;
                    }
                    if (watched_fds[this_fd].revents & POLLHUP) {
                        // this pipe has hung up
                        // don't close the file descriptor yet, there might still be data to read
                        // instead, remove the POLLIN event to avoid getting a POLLHUP event in the next poll() call
                        watched_fds[this_fd].events &= ~POLLIN;
                    }
                }
            }
            // wait for child to exit, capture status
            waitpid(pid, &status, 0);

            // Drain the pipes before exiting
            while ((byte_count = read(stdout_read_guard.get(), buf, BUFFER_SIZE)) > 0) {
                write_all(stdout_log_fh->_fileno, buf, byte_count);
                write_all(STDOUT_FILENO, buf, byte_count);
            }
            while ((byte_count = read(stderr_read_guard.get(), buf, BUFFER_SIZE)) > 0) {
                write_all(stderr_log_fh->_fileno, buf, byte_count);
                write_all(STDERR_FILENO, buf, byte_count);
            }

            int status;
            pid_t pid = waitpid(-1, &status, WNOHANG);  // Non-blocking wait for child process
            if (pid > 0) {  // If a child process has terminated
                // Check if the child process exited normally
                if (WIFEXITED(status)) {
                    int exit_code = WEXITSTATUS(status);
                    logger.log(E_INFO, "Child exited with status " + std::to_string(exit_code));
                    logger.log_to_json_file("E_INFO", "Child exited with status " + std::to_string(exit_code), user, group, command);
                }
                // Check if the child process was terminated by a signal
                else if (WIFSIGNALED(status)) {
                    int signal_number = WTERMSIG(status);
                    logger.log(E_FATAL, "Process terminated by signal: " + std::to_string(signal_number));
                    logger.log_to_json_file("E_FATAL", "Process terminated by signal: " + std::to_string(signal_number), user, group, command);
                    // Reset signal handler to default before exiting
                    signal(signal_number, SIG_DFL);
                    // Return 128 + signal number as the exit code for signal termination
                    return 128 + signal_number;  // POSIX exit code format
                }
                // Handle unexpected exit conditions
                else {
                    logger.log(E_WARN, "Unknown child exit condition.");
                    logger.log_to_json_file("E_WARN", "Unknown child exit condition", user, group, command);
                }
            }
        }
    }
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "[LCPEX ERROR] " << ex.what() << std::endl;
        return -999;
}
}

