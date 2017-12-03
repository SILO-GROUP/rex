#include "Sproc.h"
#include <unistd.h>
#include <cstring>
#include <wait.h>

/// Sproc::execute
///
/// \param input - The commandline input to execute.
/// \return - The return code of the execution of input in the calling shell.
int Sproc::execute(std::string input) {
    int child_exit_code = -666;
    child_exit_code = system( input.c_str() );
    child_exit_code = WEXITSTATUS( child_exit_code );
    return child_exit_code;
}