#include "libclpex_tty.h"
#include <cstring>
#include <stdexcept>
#include <signal.h>
#include <string>
#include "../../logger/Logger.h"
#include "../FileHandle.h"
#define BUFFER_SIZE 1024

// Initialize logger
static Logger tty_logger(E_INFO, "lcpex_tty");
std::string tty_user = tty_logger.get_user_name();
std::string tty_group = tty_logger.get_group_name();

// Global variables for signal handler context
static std::string g_tty_task_context = "";
static std::string g_tty_log_directory = "";

void tty_signal_handler(int sig) {
    std::string command = "tty_signal_handler";        // Command context
    // Create an instance of Logger (you can change the log level and mask accordingly)
    // Log the signal handling
    if (sig == SIGCHLD) {
        // Log that SIGCHLD was received, but leave the exit status to the parent
        tty_logger.log_to_json_file("E_INFO", "SIGCHLD received. A child process ended.", tty_user, tty_group, command, g_tty_task_context, g_tty_log_directory);
    } else if (sig == SIGINT) {
        tty_logger.log_to_json_file("E_FATAL", "SIGINT received. Gracefully terminating...", tty_user, tty_group, command, g_tty_task_context, g_tty_log_directory);
    } else if (sig == SIGTERM) {
        tty_logger.log_to_json_file("E_FATAL", "SIGTERM received. Terminating...", tty_user, tty_group, command, g_tty_task_context, g_tty_log_directory);
    } else if (sig == SIGSEGV) {
        tty_logger.log_to_json_file("E_FATAL", "SIGSEGV received. Possible segmentation fault.", tty_user, tty_group, command, g_tty_task_context, g_tty_log_directory);
    } else {
        tty_logger.log_to_json_file("E_FATAL", "Unhandled signal received", tty_user, tty_group, command, g_tty_task_context, g_tty_log_directory);
    }
}
// Setup signal registrations
void setup_tty_signal_handlers(std::string task_context = "", std::string log_directory = "") {
    // Set global variables for signal handler context
    g_tty_task_context = task_context;
    g_tty_log_directory = log_directory;
    
    struct sigaction sa;
    sa.sa_handler = tty_signal_handler; // <-- handler function
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    std::string command = "setup_tty_signal_handlers"; // Command context

    // SIGCHLD
    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
        std::string error_message = "Failed to set SIGCHLD handler: " + std::string(strerror(errno));
        tty_logger.log_to_json_file("E_FATAL", error_message, tty_user, tty_group, command, task_context, log_directory); // Log to JSON file
    }

    // SIGINT
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        std::string error_message = "Failed to set SIGINT handler: " + std::string(strerror(errno));
        tty_logger.log_to_json_file("E_FATAL", error_message, tty_user, tty_group, command, task_context, log_directory); // Log to JSON file
    }

    // SIGTERM
    if (sigaction(SIGTERM, &sa, nullptr) == -1) {
        std::string error_message = "Failed to set SIGTERM handler: " + std::string(strerror(errno));
        tty_logger.log_to_json_file("E_FATAL", error_message, tty_user, tty_group, command, task_context, log_directory); // Log to JSON file
    }

    // SIGSEGV
    if (sigaction(SIGSEGV, &sa, nullptr) == -1) {
        std::string error_message = "Failed to set SIGSEGV handler: " + std::string(strerror(errno));
        tty_logger.log_to_json_file("E_FATAL", error_message, tty_user, tty_group, command, task_context, log_directory); // Log to JSON file
    }
}

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

void set_tty_cloexec_flag(int fd, std::string task_context = "", std::string log_directory = "")
{
    std::string command = "set_tty_cloexec_flag";
    if (fcntl(fd, F_SETFD, FD_CLOEXEC) == -1)
    {
        std::string error_message = "fcntl(F_SETFD) failed for fd " + std::to_string(fd);
        tty_logger.log_to_json_file("E_FATAL", error_message, tty_user, tty_group, command, task_context, log_directory);
        throw std::runtime_error("fcntl(F_SETFD) failed for fd " + std::to_string(fd));
    }
}

void safe_perror( const char * msg, struct termios * ttyOrig, std::string task_context = "", std::string log_directory = "" )
{
    std::string command = "safe_perror";  // Command context
    tty_logger.log_to_json_file("E_FATAL", msg, tty_user, tty_group, command, task_context, log_directory);  // Log the message before exiting
    std::cerr << msg << std::endl;
    ttyResetExit( ttyOrig );
    throw std::runtime_error(std::string("TTY Error: ") + msg);
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
void run_child_process( int fd_child_stderr_pipe[2], char * processed_command[], struct termios * ttyOrig, bool context_override, std::string context_user, std::string context_group, std::string task_context = "", std::string log_directory = "" )
{
    std::string command = "run_child_process_tty";  // Command context
    // redirect stderr to the write end of the stderr pipe
    // close the file descriptor STDERR_FILENO if it was previously open, then (re)open it as a copy of
    while ((dup2(fd_child_stderr_pipe[WRITE_END], STDERR_FILENO) == -1) && (errno == EINTR)) {}

    // close the write end of the stderr pipe and read end to avoid leaks
    close( fd_child_stderr_pipe[WRITE_END] );
    close( fd_child_stderr_pipe[READ_END] );

    // if the user has specified a context override, set the context
    if ( context_override )
    {
        int context_result = set_identity_context(context_user, context_group);
        switch (context_result) {
            case IDENTITY_CONTEXT_ERRORS::ERROR_NONE:
                break;
            case IDENTITY_CONTEXT_ERRORS::ERROR_NO_SUCH_USER:
                tty_logger.log(E_FATAL, "Aborting: context user not found: " + context_user);
                tty_logger.log_to_json_file("E_FATAL", "Context user not found: " + context_user, tty_user, tty_group, command, task_context, log_directory);
                _exit(1);
                break;
            case IDENTITY_CONTEXT_ERRORS::ERROR_NO_SUCH_GROUP:
                tty_logger.log(E_FATAL, "Aborting: context group not found: " + context_group);
                tty_logger.log_to_json_file("E_FATAL", "Context group not found: " + context_group, tty_user, tty_group, command, task_context, log_directory);
                _exit(1);
                break;
            case IDENTITY_CONTEXT_ERRORS::ERROR_SETGID_FAILED:
                tty_logger.log(E_FATAL, "Aborting: Setting GID failed: " + context_user + "/" + context_group);
                tty_logger.log_to_json_file("E_FATAL", "Setting GID failed for: " + context_user + "/" + context_group, tty_user, tty_group, command, task_context, log_directory);
                _exit(1);
                break;
            case IDENTITY_CONTEXT_ERRORS::ERROR_SETUID_FAILED:
                tty_logger.log(E_FATAL, "Aborting: Setting UID failed: " + context_user + "/" + context_group);
                tty_logger.log_to_json_file("E_FATAL", "Setting UID failed for: " + context_user + "/" + context_group, tty_user, tty_group, command, task_context, log_directory);
                _exit(1);
                break;
            default:
                tty_logger.log(E_FATAL, "Aborting: Unknown error while setting identity context.");
                tty_logger.log_to_json_file("E_FATAL", "Unknown error while setting identity context.", tty_user, tty_group, command, task_context, log_directory);
                _exit(1);
                break;
        }
    }

    // execute the dang command, print to stdout, stderr (of parent), and dump to file for each!!!!
    // (and capture exit code in parent)
    int exit_code = execvp( processed_command[0], processed_command );
    tty_logger.log_to_json_file("E_FATAL", "failed on execvp in child", tty_user, tty_group, command, task_context, log_directory);
    tty_logger.log(E_FATAL, "failed on execvp in child");
    _Exit(exit_code);

}

// this does three things:
//  - execute a dang string as a subprocess command
//  - capture child stdout/stderr to respective log files
//  - TEE child stdout/stderr to parent stdout/stderr
int exec_pty(
        std::string command,
        FILE * stdout_log_fh,
        FILE * stderr_log_fh,
        bool context_override,
        std::string context_user,
        std::string context_group,
        bool environment_supplied,
        std::string task_context,
        std::string log_directory
) {
    try {
        std::string exec_command = "exec_pty";  // Command context
        tty_logger.log_to_json_file("E_INFO", "TTY LAUNCHER: " + command, tty_user, tty_group, exec_command, task_context, log_directory);
        // Setup signal handlers
        setup_tty_signal_handlers(task_context, log_directory);
    // initialize the terminal settings obj
    struct termios ttyOrig;

    // if the user chose to supply the environment, then we need to clear the environment
    // before we fork the process, so that the child process will inherit the environment
    // from the parent process
    if ( environment_supplied ) {
        //clearenv();
    }

    // turn our command string into something execvp can consume
    char ** processed_command = expand_env( command );

    if ( stdout_log_fh == NULL ) {
        tty_logger.log_to_json_file("E_FATAL", "Error opening STDOUT log file. Aborting.", tty_user, tty_group, exec_command, task_context, log_directory);
        throw std::runtime_error("Error opening STDOUT log file");
    }

    if ( stderr_log_fh == NULL ) {
        tty_logger.log_to_json_file("E_FATAL", "Error opening STDERR log file. Aborting.", tty_user, tty_group, exec_command, task_context, log_directory);
        throw std::runtime_error("Error opening STDERR log file");
    }


    // create the pipes for the child process to write and read from using its stderr
    int fd_child_stderr_pipe[2];

    if ( pipe( fd_child_stderr_pipe ) == -1 ) {
        std::string error_message = "Failed to create stderr pipe: " + std::string(strerror(errno));
        tty_logger.log_to_json_file("E_FATAL", error_message, tty_user, tty_group, exec_command, task_context, log_directory);
        throw std::runtime_error("Failed to create stderr pipe");
    }
        // using O_CLOEXEC to ensure that the child process closes the file descriptors
        set_tty_cloexec_flag( fd_child_stderr_pipe[READ_END], task_context, log_directory );
        set_tty_cloexec_flag( fd_child_stderr_pipe[WRITE_END], task_context, log_directory );
    // status result basket for the parent process to capture the child's exit status
    int status = 616;

    // start ptyfork integration
    char slaveName[MAX_SNAME];
    int masterFd;
    struct winsize ws;

    /* Retrieve the attributes of terminal on which we are started */
    if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1){
        tty_logger.log_to_json_file("E_FATAL", "tcgetattr failed", tty_user, tty_group, exec_command, task_context, log_directory);
        close(fd_child_stderr_pipe[READ_END]);      // Pipe Leaks on Error Paths
        close(fd_child_stderr_pipe[WRITE_END]);     // Pipe Leaks on Error Paths
        throw std::runtime_error("tcgetattr failed");
}
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
{
        tty_logger.log_to_json_file("E_FATAL", "ioctl-TIOCGWINSZ failed", tty_user, tty_group, exec_command, task_context, log_directory);
        close(fd_child_stderr_pipe[READ_END]);      // Pipe Leaks on Error Paths
        close(fd_child_stderr_pipe[WRITE_END]);     // Pipe Leaks on Error Paths
        throw std::runtime_error("ioctl-TIOCGWINSZ failed");
}
    pid_t pid = ptyFork( &masterFd, slaveName, MAX_SNAME, &ttyOrig, &ws );
    switch( pid ) {
        case -1:
        {
            // fork failed
            tty_logger.log(E_FATAL, "ptyfork failure: " + std::string(strerror(errno)));
            tty_logger.log_to_json_file("E_FATAL", "ptyfork failure: " + std::string(strerror(errno)), tty_user, tty_group, exec_command, task_context, log_directory);
            close(fd_child_stderr_pipe[READ_END]);      // Pipe Leaks on Error Paths
            close(fd_child_stderr_pipe[WRITE_END]);     // Pipe Leaks on Error Paths
            throw std::runtime_error("ptyfork() failed: " + std::string(strerror(errno)));
        }
        case 0:
        {
            // child process
            run_child_process( fd_child_stderr_pipe, processed_command, &ttyOrig, context_override, context_user, context_group, task_context, log_directory );
        }
        default:
        {
            // parent process
            // start ptyfork integration
            ttySetRaw(STDIN_FILENO, &ttyOrig);

            // RAII guards for file descriptors
            FdGuard stderr_read_guard(fd_child_stderr_pipe[READ_END]);
            FdGuard stderr_write_guard(fd_child_stderr_pipe[WRITE_END]);
            FdGuard master_fd_guard(masterFd);

            // We don't need WRITE_END in parent, close it now
            stderr_write_guard.reset();  // Closes and sets fd to -1
            // attempt to write to stdout,stderr from child as well as to write each to file
            char buf[BUFFER_SIZE] = {0};
            std::strncpy(buf, command.c_str(), BUFFER_SIZE - 1);
            buf[BUFFER_SIZE - 1] = '\0';

            // contains the byte count of the last read from the pipe
            ssize_t byte_count;

            // watched_fds for poll() to wait on
            struct pollfd watched_fds[3];

            // populate the watched_fds array

            // child in to parent
            watched_fds[0].fd = STDIN_FILENO;
            watched_fds[0].events = POLLIN;

            // child pty to parent (stdout)
            watched_fds[1].fd =  master_fd_guard.get();
            watched_fds[1].events = POLLIN;

            // child stderr to parent
            watched_fds[2].fd = stderr_read_guard.get();
            watched_fds[2].events = POLLIN;

            // number of files poll() reports as ready
            int num_files_readable;

            // loop flag
            bool break_out = false;

            // loop until we've read all the data from the child process
            while ( ! break_out ) {
                num_files_readable = poll(watched_fds, sizeof(watched_fds) / sizeof(watched_fds[0]), -1);
                // after the poll() call, add a check to see if both pipes are closed
                if (!(watched_fds[1].events & POLLIN) &&
                    !(watched_fds[2].events & POLLIN)) {
                    break_out = true;
                }

                if (num_files_readable == -1) {
                    // error occurred in poll()
                    tty_logger.log(E_FATAL, "poll() failed: " + std::string(strerror(errno)));
                    tty_logger.log_to_json_file("E_FATAL", "poll() failed", tty_user, tty_group, exec_command, task_context, log_directory);
                    ttyResetExit( &ttyOrig);
                    throw std::runtime_error("poll() failed");
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
                            if (errno == EAGAIN) { continue; } else {
                                // error reading from pipe
                                tty_logger.log(E_FATAL, "Error while reading: " + std::string(strerror(errno)));
                                tty_logger.log_to_json_file("E_FATAL", "Error while reading from pipe", tty_user, tty_group, exec_command, task_context, log_directory);
                                ttyResetExit( &ttyOrig);
                                stderr_read_guard.reset(-1);
                                master_fd_guard.reset(-1);
                                exit(EXIT_FAILURE);
                            }
                        } else if (byte_count == 0) {
                            // reached EOF on one of the streams but not a HUP
                            // we've read all we can this cycle, so go to the next fd in the for loop
                            continue;
                        } else {
                            // byte count was sane
                            // write to stdout,stderr

                            if (this_fd == 0) {
                                // parent stdin received, write to child pty (stdin)
                                write_all(masterFd, buf, byte_count);
                            } else if (this_fd == 1 ) {
                                // child pty sent some stuff, write to parent stdout and log
                                write_all(stdout_log_fh->_fileno, buf, byte_count);
                                write_all(STDOUT_FILENO, buf, byte_count);
                            } else if ( this_fd == 2 ){
                                //the child's stderr pipe sent some stuff, write to parent stderr and log
                                write_all(stderr_log_fh->_fileno, buf, byte_count);
                                write_all(STDERR_FILENO, buf, byte_count);
                            } else {
                                // this should never happen
                                tty_logger.log(E_FATAL, "Logic error: unexpected pipe index.");
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                    if (watched_fds[this_fd].revents & POLLERR) {
                        if (this_fd == 1) {
                            master_fd_guard.reset(-1);
                        } else if (this_fd == 2) {
                            stderr_read_guard.reset(-1);
                        }
                        //close(watched_fds[this_fd].fd);
                        break_out = true;
                        //continue;
                    }
//                    if (watched_fds[this_fd].revents & POLLHUP) {
//                        // this pipe has hung up
//                        close(watched_fds[this_fd].fd);
//                        break_out = true;
//                        break;
//                    }
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

            while ((byte_count = read(master_fd_guard.get(), buf, BUFFER_SIZE)) > 0) {
                write_all(stdout_log_fh->_fileno, buf, byte_count);
                write_all(STDOUT_FILENO, buf, byte_count);
            }

            while ((byte_count = read(stderr_read_guard.get(), buf, BUFFER_SIZE)) > 0) {
                write_all(stderr_log_fh->_fileno, buf, byte_count);
                write_all(STDERR_FILENO, buf, byte_count);
            }
            // Handle child process status with proper signal handling
            pid_t child_pid = waitpid(-1, &status, WNOHANG);  // Non-blocking wait for child process
            if (child_pid > 0) {  // If a child process has terminated
                // Check if the child process exited normally
                if (WIFEXITED(status)) {
                    int exit_code = WEXITSTATUS(status);
                    tty_logger.log(E_INFO, "Child exited with status " + std::to_string(exit_code));
                    tty_logger.log_to_json_file("E_INFO", "Child exited with status " + std::to_string(exit_code), tty_user, tty_group, exec_command, task_context, log_directory);
                    ttyResetExit( &ttyOrig);
                    return exit_code;
                }
                // Check if the child process was terminated by a signal
                else if (WIFSIGNALED(status)) {
                    int signal_number = WTERMSIG(status);
                    tty_logger.log(E_FATAL, "Process terminated by signal: " + std::to_string(signal_number));
                    tty_logger.log_to_json_file("E_FATAL", "Process terminated by signal: " + std::to_string(signal_number), tty_user, tty_group, exec_command, task_context, log_directory);
                    // Reset signal handler to default before exiting
                    signal(signal_number, SIG_DFL);
                    ttyResetExit( &ttyOrig);
                    return 128 + signal_number;  // POSIX exit code format
                }
                // Handle unexpected exit conditions
                else {
                    tty_logger.log(E_WARN, "Unknown child exit condition.");
                    tty_logger.log_to_json_file("E_WARN", "Unknown child exit condition", tty_user, tty_group, exec_command, task_context, log_directory);
                }
            }

            ttyResetExit( &ttyOrig);
            if WIFEXITED(status) {
                return WEXITSTATUS(status);
            } else {
                return -617;
            }
        }
    }
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "[LCPEX TTY ERROR] " << ex.what() << std::endl;
        return -999;
    }
}