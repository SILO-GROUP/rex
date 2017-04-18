#include <iostream>
#include <fstream>
#include "src/json/json.h"
#include <string>
#include "src/json/json-forwards.h"
#include "src/loaders.h"


void json_file_to_json_obj(std::string filename)
{
    Conf jsonLoader = Conf(filename);
    Json::Value root = jsonLoader.get_root();

    const std::string planpath = root.get("plan_path", NULL).asString();

    std::cout << planpath;
}

/*configuration load_config_file( std::string filename )
{
    json j_conf = json_file_to_json_obj( filename );
    configuration appconfig = j_conf;
    return appconfig;
}*/

int main()
{
    // configuration appconf = load_config_file( "./config.json" );
    // std::cout << appconf.plan_path << std::endl;
    // std::cout << appconf.units_path << std::endl;

    json_file_to_json_obj( "config.json" );
    return 0;
}