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

#ifndef FTESTS_TASK_H
#define FTESTS_TASK_H
#include <string>
#include <unistd.h>
#include "../../json/json.h"
#include "Unit.h"
#include "Suite.h"
#include "Conf.h"
#include <stdio.h>
#include "../../Sproc/Sproc.h"
#include "../misc/helpers.h"

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

#endif //FTESTS_TASK_H
