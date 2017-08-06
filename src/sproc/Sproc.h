#ifndef FTESTS_SPROC_H
#define FTESTS_SPROC_H

#include "string"


struct ExecutionInput {
    std::string executionString;
//    std::string STDIN;
//    std::vector<KeyValuePair> EnvironmentVariables;
};

struct Execution {
    //input
    ExecutionInput input;

    // output
    int return_code;

    std::string STDOUT;
    std::string STDERR;
};

// executes a subprocess and captures STDOUT, STDERR, and return code.
// should be able to recieve path of binary to be executed as well as any parameters
class Sproc {
    public:
        // call the object.  returnvalue is enum representing external execution attempt not binary exit code
        static int execute( ExecutionInput input, Execution & output );
};

#endif //FTESTS_SPROC_H
