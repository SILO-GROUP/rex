#include <iostream>
#include <fstream>
#include "src/json/json.h"
#include <string>
#include "src/json/json-forwards.h"
#include "src/loaders.h"

/*configuration load_config_file( std::string filename )
{
    json j_conf = json_file_to_json_obj( filename );
    configuration appconfig = j_conf;
    return appconfig;
}*/

int main()
{
    Conf jsonLoader = Conf("config.json");

    std::cout << jsonLoader.get_units_path();
    return 0;
}