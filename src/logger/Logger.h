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

#ifndef REX_LOGGER_H
#define REX_LOGGER_H

#include <string>
#include <iostream>
#include <fstream>
#include <fcntl.h>  // For O_RDONLY, O_WRONLY, open()
#include <ctime>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <memory>
#include <cstdarg>  // for va_list in FileGuard::printf
#include "../misc/helpers.h"

enum L_LVL {
    E_FATAL,
    E_WARN,
    E_INFO,
    E_DEBUG
};

class Logger {
    public:
    // Constructor and Destructor
    Logger(int LOG_LEVEL, std::string mask);
    ~Logger();  // Added Destructor
    // Logging methods
    void log(int LOG_LEVEL, std::string msg);
    void log_task(int LOG_LEVEL, std::string task_name, std::string msg);
    void log_to_json_file(const std::string& log_level, const std::string& message,
                           const std::string& user, const std::string& group,
                           const std::string& command, bool log_to_console = true);
    // Helper methods
    std::string get_current_timestamp();
    std::string get_user_name();
    std::string get_group_name();

    private:
        int LOG_LEVEL;
        std::string mask;
    // Internal helper methods
    void create_json_log_entry(char* buffer, size_t buffer_size,
                                const char* log_level, const char* message,
                                const char* user, const char* group,
                                const char* command);
};

#endif //REX_LOGGER_H
