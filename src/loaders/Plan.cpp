//
// Created by phanes on 4/22/17.
//
#include "Plan.h"

Task::Task( Json::Value loader_root )
{
    this->name = loader_root.get("name", "?").asString();
    this->dependencies = loader_root.get("depends on", "");
    this->has_succeeded = false;
}

std::string Task::get_name()    { return this->name;            }
bool Task::isDone()             { return this->has_succeeded;   }
void Task::finish()             { this->has_succeeded = true;   }

// returns Json::Value for dependencies
Json::Value Task::get_dependencies()    { return this->dependencies;}



Json::Value Task::set_dependencies()
{

}


int Plan::num_tasks()
{
    return (int)this->tasks.size();
}


Task Plan::select_task_index(int index) {
    return this->tasks[index];
}

Task Plan::select_task( std::string provided_name )
{
/*
 * returns a task from a Plan object by name
 * this will need reworked.  maybe should return int, populate a pointer.
 * error handling is the concern here.
 */
    {
        Task * returnable;
        bool foundMatch = false;

        for ( int i = 0; i < this->tasks.size(); i++ )
        {
            std::string task_name = this->tasks[i].get_name();
            if ( task_name == provided_name )
            {
                returnable = & this->tasks[i];
                foundMatch = true;
                break;
            }
        }
        if (! foundMatch )
        {
            std::cerr << "Task name \"" << provided_name << "\" was referenced but not defined!" << std::endl;
            std::exit(1);
        }

        return * returnable;
    }
}

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
