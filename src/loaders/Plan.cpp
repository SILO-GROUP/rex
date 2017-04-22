//
// Created by phanes on 4/22/17.
//

#include "Plan.h"
Task::Task( Json::Value loader_root )
{
    this->name = loader_root.get("name", "?").asString();
    this->dependencies = loader_root.get("depends on", "");
}
std::string Task::get_name()            { return this->name;        }
Json::Value Task::get_dependencies()    { return this->dependencies;}

Plan::Plan( std::string filename ): JLoader( filename )
{
/*  Plan loads a file and deserializes the Unit JSON object to Task types as a vector member
 *  Plan { vector<Task> }
 */
    Json::Value raw_tasks = this->get_root()["plan"];

    for ( int index = 0; index < raw_tasks.size(); index++ )
    {
        this->tasks.push_back( Task( raw_tasks[index] ) );
    }
};