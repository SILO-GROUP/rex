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

#ifndef REX_TASK_H
#define REX_TASK_H
#include "../json_support/JSON.h"
#include "../suite/Unit.h"
#include "../suite/Suite.h"
#include "../config/Config.h"
#include "../misc/helpers.h"
#include "../lcpex/liblcpex.h"
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>


class Task
{
    protected:
        // the name of this task
        std::string name;

        // names of tasks that must have successfully executed before this task can execute its own unit.
        std::vector<std::string> dependencies;

        // private member to store the definition of this task once found in a Suite by Plan.
        // populated by load_definition
        Unit definition;

        // the status of this task
        bool complete;

        // the readiness of this task to execute
        bool defined;

        bool prepare_logs( std::string task_name, std::string logs_root );


public:
        // constructor
        Task( int LOG_LEVEL );

        // load a json::value into task members (second stage deserialization)
        void load_root( Json::Value loader_root );

        // appends definition unit as child member
        void load_definition( Unit definition );

        bool is_complete();
        bool has_definition();

        // fetch the name of a task
        std::string get_name();

        // execute this task's definition
        void execute( Conf * configuration );

        void mark_complete();

        // returns a pointer to the dependencies vector
        std::vector<std::string> get_dependencies();


    private:
        Logger slog;
        int LOG_LEVEL;
};

#endif //REX_TASK_H