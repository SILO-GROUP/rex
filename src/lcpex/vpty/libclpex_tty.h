//
// Created by phanes on 2/6/23.
//

#ifndef LCPEX_LIBCLPEX_TTY_H
#define LCPEX_LIBCLPEX_TTY_H

#include <cstdio>
#include <cstdlib>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include "../string_expansion/string_expansion.h"
#include "../helpers.h"
#include "../Contexts.h"
#include "../vpty/pty_fork_mod/tty_functions.h"
#include "../vpty/pty_fork_mod/pty_fork.h"
#include <sys/ioctl.h>
#include <string>

/**
 * @brief Execute a string as a subprocess command, capture its stdout/stderr to log files, and TEE its output to the parent process's stdout/stderr.
 * This function performs the following three tasks:
 * - Execute a string as a subprocess command.
 * - Capture the child process's stdout and stderr to respective log files.
 * - TEE the child process's stdout and stderr to the parent process's stdout and stderr.
 *
 * @param command The command to be executed as a subprocess.
 * @param stdout_log_file The file path where the child process's stdout will be logged.
 * @param stderr_log_file The file path where the child process's stderr will be logged.
 * @param context_override Specify whether to override the process's execution context.
 * @param context_user The user context to run the process as, if context_override is true.
 * @param context_group The group context to run the process as, if context_override is true.
 * @param environment_supplied Specify whether the environment is supplied.
 * @return The exit status of the child process. If the child process terminated due to a signal, returns -617.
 */
int exec_pty(
        std::string command,
        std::string stdout_log_file,
        std::string stderr_log_file,
        bool context_override,
        std::string context_user,
        std::string context_group,
        bool environment_supplied
);


#endif //LCPEX_LIBCLPEX_TTY_H
