/*
    Examplar - An automation and testing framework.

    © SURRO INDUSTRIES and Chris Punches, 2017.

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

#ifndef FTESTS_SPROC_H
#define FTESTS_SPROC_H

#include <string>
#include <iostream>
#include <stdio.h>
#include "../Logger/Logger.h"
#include <streambuf>
#include "unistd.h"

// executes a subprocess and captures STDOUT, STDERR, and return code.
// should be able to recieve path of binary to be executed as well as any parameters
class Sproc {
    public:
        // call the object.  returnvalue is enum representing external execution attempt not binary exit code
        static int execute(std::string shell, std::string enviornment_file, std::string user_name, std::string group_name, std::string command, int LOG_LEVEL, std::string task_name );
};


// mostly lifted from:
// http://wordaligned.org/articles/cpp-streambufs
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

#endif //FTESTS_SPROC_H

