#include "loaders.h"
#include <sys/stat.h>

inline bool exists(const std::string& name)
{
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

JLoader::JLoader( std::string filename )
{
    Json::Value json_root;
    Json::Reader reader;

    if (! exists( filename ) )
    {
        std::cerr << "File '" << filename << "' does not exist.";
        exit(1);
    }

    std::ifstream jfile( filename, std::ifstream::binary );

    bool parsingSuccessful = reader.parse( jfile, this->json_root );
    if (! parsingSuccessful )
    {
        std::cerr << "Failed to parse " << filename << ":\n\t" << reader.getFormattedErrorMessages();
        exit( 1 );
    } else {
        std::cout << "Parsed " << filename << " with " << this->json_root.size() << " elements." << std::endl;
    }
}

Json::Value JLoader::get_root()
{
    return this->json_root;
}


Unit::Unit( Json::Value loader_root )
{
    this->name      = loader_root.get("name", "?").asString();
    this->target    = loader_root.get("target", "?").asString();
    this->output    = loader_root.get("output", "?").asString();
    this->rectifier = loader_root.get("rectifier", "?").asString();
    this->active    = loader_root.get("active", "?").asString();
    this->required  = loader_root.get("required", "?").asString();
    this->rectify   = loader_root.get("rectify", "?").asString();
}

std::string Unit::get_name()        { return this->name;        }
std::string Unit::get_target()      { return this->target;      }
std::string Unit::get_output()      { return this->output;      }
std::string Unit::get_rectifier()   { return this->rectifier;   }
std::string Unit::get_active()      { return this->active;      }
std::string Unit::get_required()    { return this->required;    }
std::string Unit::get_rectify()     { return this->rectify;     }

UnitHolder::UnitHolder( std::string filename ): JLoader( filename )
{
/*  UnitHolder loads a file and deserializes the Unit JSON object to Unit types as a vector member
 *  UnitHolder { vector<Unit> }
 */
    Json::Value raw_units = this->get_root()["units"];

    for ( int index = 0; index < raw_units.size(); index++ )
    {
        this->units.push_back(Unit( raw_units[index] ));
    }
};

Unit UnitHolder::select_unit(std::string name)
{
    /*
     *  TODO: Implement fetch unit by name method.
     */
    for ( int i = 0; i < this->units.size(); ++i )
    {
        if ( this->units[i].get_name() == name )
        {
            return this->units[i];
        } else {
            std::cerr << "Logic error in UnitHolder::select_unit.  This is a bug.  Please report it." << std::endl;
            exit(1);
        }
    }
}


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
 *  TODO: Ensure FIFO order.
 */
    Json::Value raw_tasks = this->get_root()["plan"];

    for ( int index = 0; index < raw_tasks.size(); index++ )
    {
        this->tasks.push_back( Task( raw_tasks[index] ) );
    }
};


Conf::Conf( std::string filename ): JLoader( filename )
{
    this->plan_path = this->get_root()["plan_path"].asString();
    this->units_path = this->get_root()["units_path"].asString();
};

std::string Conf::get_plan_path()
{
    return this->plan_path;
}

std::string Conf::get_units_path()
{
    return this->units_path;
}

