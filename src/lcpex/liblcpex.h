#ifndef LCPEX_LIBLCPEX_H
#define LCPEX_LIBLCPEX_H

#include <cstdio>
#include <cstdlib>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include "string_expansion/string_expansion.h"
#include "helpers.h"
#include "Contexts.h"
#include "vpty/pty_fork_mod/tty_functions.h"
#include "vpty/pty_fork_mod/pty_fork.h"
#include "vpty/libclpex_tty.h"


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
int execute(
        std::string command,
        std::string stdout_log_file,
        std::string stderr_log_file,
        bool context_override,
        std::string context_user,
        std::string context_group,
        bool environment_supplied
);


/**
 * @brief Executes a command with logging and optional context switching
 *
 * @param command The command to be executed
 * @param stdout_log_file The file to log the standard output of the command
 * @param stderr_log_file The file to log the standard error of the command
 * @param context_override Indicates whether to override the current execution context
 * @param context_user The user to switch to for execution context
 * @param context_group The group to switch to for execution context
 * @param force_pty Indicates whether to force a pseudoterminal (pty) for the command execution
 * @param is_shell_command Indicates whether the command is a shell command
 * @param shell_path The path to the shell executable
 * @param shell_execution_arg The argument used to execute a command in the shell
 * @param supply_environment Indicates whether to supply an environment
 * @param shell_source_subcommand The shell subcommand used to source the environment file
 * @param environment_file_path The path to the environment file
 *
 * @return The exit status of the executed command
 *
 * This function executes a command with logging and optional context switching. The function generates
 * a prefix for the command using the `prefix_generator` function, which sets up a shell execution if
 * the command is a shell command. If a pseudoterminal (pty) is forced, the `exec_pty` function is used
 * to execute the command, otherwise the `execute` function is used. The standard output and standard
 * error of the command are logged to the provided log files. If context overriding is requested, the
 * execution context is switched to the provided user and group.
 */
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
);

/**
 * @brief Generates a command prefix for execution
 *
 * @param command The main command to be executed
 * @param is_shell_command Indicates whether the command is a shell command
 * @param shell_path The path to the shell executable
 * @param shell_execution_arg The argument used to execute a command in the shell
 * @param supply_environment Indicates whether to supply an environment
 * @param shell_source_subcommand The shell subcommand used to source the environment file
 * @param environment_file_path The path to the environment file
 *
 * @return The generated command prefix
 *
 * This function generates a prefix for a command to be executed, based on the provided parameters.
 * If the command is a shell command, the prefix will be set up for a shell execution. If the shell
 * takes an argument to execute a command, it will be added to the prefix. If an environment is to be
 * supplied, the shell subcommand for sourcing the environment file and the environment file path will
 * be added to the prefix. If the command is not a shell command, the command will be returned as is.
 */
std::string prefix_generator(
        std::string command,
        bool is_shell_command,
        std::string shell_path,
        std::string shell_execution_arg,
        bool supply_environment,
        std::string shell_source_subcommand,
        std::string environment_file_path
);

#endif //LCPEX_LIBLCPEX_H
