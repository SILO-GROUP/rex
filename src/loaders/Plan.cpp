#include "Plan.h"

Plan::Plan(): JSON_Loader() {};

void Plan::load_plan_file(std::string filename, bool verbose)
{
    // plan always loads from file
    this->load_json_file( filename );

    Json::Value raw_tasks = this->as_serialized()["plan"];

    for ( int index = 0; index < raw_tasks.size(); index++ )
    {
        this->tasks.push_back( Task( raw_tasks[index] ) );
    }

}

Task Plan::get_task(int index)
// returns a task from its parent Plan by index
{
    return this->tasks[index];
}

Task Plan::get_task(std::string provided_name)
/* returns a task from a Plan object by name
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