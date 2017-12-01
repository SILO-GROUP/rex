#include "Sproc.h"
#include <unistd.h>
#include <cstring>

/// Sproc::execute
///
/// \param input - The commandline input to execute.
/// \return - The return code of the execution of input in the calling shell.
int Sproc::execute(std::string input) {
    std::cout << std::endl << "made it to subprocess execution but not implemented yet" << std::endl << std::endl;

    int PIPE_READ = 0;
    int PIPE_WRITE = 1;

    int stdin_pipe[2];
    // int aStderrPipe[2];
    int stdout_pipe[2];
    int child_pid;
    char nChar;
    int child_exit_code;

    if (pipe(stdin_pipe) < 0)
    {
        perror("allocating pipe for child input redirect");
        return -1;
    }

/*
    if (pipe(aStderrPipe < 0)) {
        close(aStderrPipe[PIPE_READ]);
        close(aStderrPipe[PIPE_WRITE]);
        perror("allocating pipe for error redirect");
        return -1;
    }
*/

    if (pipe(stdout_pipe) < 0)
    {
        close(stdin_pipe[PIPE_READ]);
        close(stdin_pipe[PIPE_WRITE]);
        perror("allocating pipe for child output redirect");
        return -1;
    }

    child_pid = fork();
    if (0 == child_pid)
    {
        // child continues here

        // redirect stdin
        if (dup2(stdin_pipe[PIPE_READ], STDIN_FILENO) == -1) {
            exit(errno);
        }

        // redirect stdout
        if (dup2(stdout_pipe[PIPE_WRITE], STDOUT_FILENO) == -1) {
            exit(errno);
        }

        // redirect stderr
        if (dup2(stdout_pipe[PIPE_WRITE], STDERR_FILENO) == -1) {
            exit(errno);
        }

        // all these are for use by parent only
        close(stdin_pipe[PIPE_READ]);
        close(stdin_pipe[PIPE_WRITE]);
        close(stdout_pipe[PIPE_READ]);
        close(stdout_pipe[PIPE_WRITE]);

        // run child process image
        // replace this with any exec* function find easier to use ("man exec")
        child_exit_code = system( input.c_str() );

        // if we get here at all, an error occurred, but we are in the child
        // process, so just exit
        exit(child_exit_code);
    } else if (child_pid > 0) {
        // parent continues here

        // close unused file descriptors, these are for child only
        close(stdin_pipe[PIPE_READ]);
        close(stdout_pipe[PIPE_WRITE]);

        // da fuq?
/*        // Include error check here
        if (NULL != szMessage)
        {
            write(stdin_pipe[PIPE_WRITE], szMessage, strlen(szMessage));
        }
*/

        // Just a char by char read here, you can change it accordingly
        while (read(stdout_pipe[PIPE_READ], &nChar, 1) == 1)
        {
            write(STDOUT_FILENO, &nChar, 1);
        }

        // done with these in this example program, you would normally keep these
        // open of course as long as you want to talk to the child
        close(stdin_pipe[PIPE_WRITE]);
        close(stdout_pipe[PIPE_READ]);
    } else {
        // failed to create child
        close(stdin_pipe[PIPE_READ]);
        close(stdin_pipe[PIPE_WRITE]);
        close(stdout_pipe[PIPE_READ]);
        close(stdout_pipe[PIPE_WRITE]);
    }
    return child_exit_code;
}