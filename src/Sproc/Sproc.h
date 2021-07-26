/*
    Rex - A configuration management and workflow automation tool that
    compiles and runs in minimal environments.

    © SILO GROUP and Chris Punches, 2020.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef REX_SPROCKET_H
#define REX_SPROCKET_H

#include "../Logger/Logger.h"
#include <iostream>
#include <stdio.h>
#include "errno.h"
#include <string>
#include <cstring>
#include <streambuf>
#include "unistd.h"
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#include <fstream>
#include "fcntl.h"
#include "../loaders/misc/helpers.h"


// exit codes for Rex
enum SPROC_RETURN_CODES {
    SUCCESS = true,
    UID_NOT_FOUND = -404,
    GID_NOT_FOUND = -405,
    SET_GID_FAILED = -401,
    SET_UID_FAILED = -404,
    EXEC_FAILURE_GENERAL = -666,
    PIPE_FAILED3 = -996,
    PIPE_FAILED2 = -997,
    PIPE_FAILED = -998,
    DUP2_FAILED = -999,
    FORK_FAILED = -1000
};

// executes a subprocess and captures STDOUT, STDERR, and return code.
// should be able to receive path of binary to be executed as well as any parameters
class Sproc {
    public:
        // call the object.  return value is enum representing external execution attempt not binary exit code
        static int execute(
                std::string shell,
                std::string environment_file,
                std::string user_name,
                std::string group_name,
                std::string command,
                int LOG_LEVEL,
                std::string task_name,
                bool log_to_file,
                std::string logs_dir
        );
};

// a teebuff implementation
class teebuf: public std::streambuf
{
    public:
        // Construct a streambuf which tees output to both input
        // streambufs.
        teebuf(std::streambuf * sb1, std::streambuf * sb2);
    private:
        virtual int overflow(int c);
        virtual int sync();
        std::streambuf * sb1;
        std::streambuf * sb2;
};

// a simple helper class to create a tee stream from two input streams
class teestream : public std::ostream
{
    public:
        // Construct an ostream which tees output to the supplied ostreams.
        teestream( std::ostream & o1, std::ostream & o2);
        teebuf tbuf;
};

#endif //REX_SPROCKET_H
