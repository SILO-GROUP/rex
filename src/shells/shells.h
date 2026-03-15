#ifndef REX_SHELLS_H
#define REX_SHELLS_H

#include "../json_support/JSON.h"
#include "../misc/helpers.h"
#include <string>
#include <string.h>
#include <vector>
#include <dirent.h>

class Shell: public JSON_Loader {
    public:
        Shell( int LOG_LEVEL );
        int load_root( Json::Value loader_root );

        std::string name;
        std::string path;
        std::string execution_arg;
        std::string source_cmd;

    private:
        std::string shell_definitions_path;

        int LOG_LEVEL;
        Logger slog;


};

void get_shells_from_dir( std::vector<std::string> * files, std::string path );

#endif //REX_SHELLS_H
