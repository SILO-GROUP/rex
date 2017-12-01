#ifndef FTESTS_SPROC_H
#define FTESTS_SPROC_H

#include <string>
#include <iostream>

// executes a subprocess and captures STDOUT, STDERR, and return code.
// should be able to recieve path of binary to be executed as well as any parameters
class Sproc {
    public:
        // call the object.  returnvalue is enum representing external execution attempt not binary exit code
        static int execute( std::string input );
};

#endif //FTESTS_SPROC_H
