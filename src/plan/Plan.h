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

#ifndef REX_PLAN_H
#define REX_PLAN_H

#include "../json_support/JSON.h"
#include "../logger/Logger.h"
#include "../config/Config.h"
#include "Task.h"
#include <string>

/**
 * @class Plan
 * @brief A managed container for a Task vector.
 *
 * A Plan is a managed container for a Task vector. These tasks reference Units that are defined in the Units files (Suite).
 * If Units are definitions, Tasks are selections of those definitions to execute, and if Units together form a Suite, Tasks
 * together form a Plan.
 */
class Plan: public JSON_Loader
{
    private:
        // storage for the tasks that make up the plan
        std::vector<Task> tasks;
        Conf * configuration;
        int LOG_LEVEL;
        Logger slog;

    public:
        /**
         * @brief Constructor for Plan class.
         *
         * @param configuration Configuration object.
         * @param LOG_LEVEL Logging level.
         */
        Plan(Conf * configuration, int LOG_LEVEL );

        /**
         * @brief Load the plan from a file and append tasks to the task vector.
         *
         * @param filename The filename to load the plan from.
         */
        void load_plan_file( std::string filename );

        /**
         * @brief Retrieve a task by name.
         *
         * @param result The variable receiving the value.
         * @param provided_name The name to find a task by.
         *
         * @throws Plan_InvalidTaskName if a task with the provided name is not found.
         */
        void get_task( Task & result, std::string provided_name );

        /**
         * @brief Retrieve a task by index.
         *
         * @param result The variable receiving the value.
         * @param index The numerical index in the Task vector to retrieve a value for.
         *
         * @throws Plan_InvalidTaskIndex if the index is greater than the size of the Task vector.
         */
        void get_task( Task & result, int index );

        /**
         * @brief Load the units corresponding to each task in the plan from the given Suite.
         *
         * @param unit_definitions The Suite to load definitions from.
         */
        void load_definitions( Suite unit_definitions );

        /**
         * @brief Check whether all dependencies for a task with the given name are complete.
         *
         * @param name The name of the task in the plan to check met dependencies for.
         *
         * @return Boolean representation of whether all dependencies are complete or not.
         */
        bool all_dependencies_complete(std::string name);

        /**
         * @brief Iterate through all tasks in the plan and execute them.
         */
        void execute();
};
#endif //REX_PLAN_H
