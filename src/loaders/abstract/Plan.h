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

#ifndef FTESTS_PLAN_H
#define FTESTS_PLAN_H

#include <string>
#include "../../json/json.h"
#include "../low_level/JSON_Loader.h"
#include "Task.h"
#include "Conf.h"
#include "../../Logger/Logger.h"

class Plan: public JSON_Loader
{
    private:
        // storage for the tasks that make up the plan
        std::vector<Task> tasks;
        Conf * configuration;

    public:
        Plan( Conf * configuration, int LOG_LEVEL );

        // append this->tasks from JSON file
        void load_plan_file( std::string filename );

        // fetch a task from this->tasks
        void get_task( Task & result, std::string provided_name );

        // fetch a task from this->tasks
        void get_task( Task & result, int index );

        // load unit definitions from a provided suite and import them into individual tasks
        void load_definitions( Suite unit_definitions );

        // fetch a corresponding Unit to a Task
        // void get_definition_from_task(Unit & result, Task input, bool verbose );

        // execute all tasks in this plan
        void execute();

        bool all_dependencies_complete(std::string name);

private:
    int LOG_LEVEL;
    Logger slog;
};

#endif //FTESTS_PLAN_H
