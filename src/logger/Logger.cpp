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
#include "Logger.h"
#include <string>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <unistd.h>  // For FdGuard
#include <fcntl.h>  // For O_RDONLY, O_WRONLY, open()
#include "../lcpex/FileHandle.h"   // Include the FdGuard header

Logger::Logger( int LOG_LEVEL, std::string mask )
{
    this->LOG_LEVEL = LOG_LEVEL;
    this->mask = mask;
}

// Destructor added in the update
Logger::~Logger() {

}

void Logger::log( int LOG_LEVEL, std::string msg )
{
    std::string ERR = "XXXX";

    if ( LOG_LEVEL <= this->LOG_LEVEL )
    {
        switch ( LOG_LEVEL )
        {
            case E_DEBUG:   ERR = "DBUG";  break;
            case E_FATAL:   ERR = "FATL";  break;
            case E_INFO:    ERR = "INFO";  break;
            case E_WARN:    ERR = "WARN";  break;
        }

        std::string s_msg = "[" + ERR + "] " + msg;

        if ( LOG_LEVEL == E_FATAL || LOG_LEVEL == E_WARN )
        {
            std::cerr << "[" << get_8601() << "] [" << ERR << "] " << "[" << this->mask << "] " << msg.c_str() << std::endl;
        } else {
            std::cout << "[" << get_8601() << "] [" << ERR << "] " << "[" << this->mask << "] " << msg.c_str() << std::endl;
        }
    }
}

void Logger::log_task( int LOG_LEVEL, std::string task_name, std::string msg )
{
     // Added memory management from the updated file

    size_t task_msg_len = task_name.length() + msg.length() + 4;
    char* task_msg = (char*)malloc(task_msg_len);

    if (task_msg) {
        snprintf(task_msg, task_msg_len, "[%s] %s", task_name.c_str(), msg.c_str());
        log(LOG_LEVEL, task_msg);
        free(task_msg);
    }
}

// JSON logging method
void Logger::create_json_log_entry(char* buffer, size_t buffer_size,
                                  const char* log_level, const char* message,
                                  const char* user, const char* group,
                                  const char* command)
{
    std::string timestamp = get_current_timestamp();
    snprintf(buffer, buffer_size,
        "{\n"
        "  \"timestamp\": \"%s\",\n"
        "  \"log_level\": \"%s\",\n"
        "  \"message\": \"%s\",\n"
        "  \"context\": {\n"
        "    \"user\": \"%s\",\n"
        "    \"group\": \"%s\",\n"
        "    \"command\": \"%s\"\n"
        "  }\n"
        "}",
        timestamp.c_str(), log_level, message, user, group, command);
}

// Add methods for user/group name and timestamp
std::string Logger::get_user_name()
{
    struct passwd* pw = getpwuid(getuid());
    if (pw) {
        return std::string(pw->pw_name);
    }
    return "unknown";
}

std::string Logger::get_group_name()
{
    struct group* grp = getgrgid(getgid());
    if (grp) {
        return std::string(grp->gr_name);
    }
    return "unknown";
}

std::string Logger::get_current_timestamp()
{
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H:%M:%S", timeinfo);
    return std::string(buffer);
}

//Log to JSON file
void Logger::log_to_json_file(const std::string& log_level, const std::string& message,
                               const std::string& user, const std::string& group,
                               const std::string& command, bool log_to_console)
{
    // Log to console if requested
    const char* log_level_str = "UNKNOWN";
    if (log_level == "E_INFO") log_level_str = "INFO";
    else if (log_level == "E_FATAL") log_level_str = "FATAL";
    else if (log_level == "E_WARN") log_level_str = "WARN";
    else if (log_level == "E_DEBUG") log_level_str = "DEBUG";

    if (log_to_console && (log_level_str == "INFO" || log_level_str == "FATAL")) {
        std::string timestamp = get_current_timestamp();
        printf("[%s] [%s] [%s] %s\n", timestamp.c_str(), log_level_str, this->mask.c_str(), message.c_str());
        fflush(stdout);
    }

    // 2. Create JSON log entry
    char json_log[2048];
    create_json_log_entry(json_log, sizeof(json_log), log_level_str, message.c_str(), user.c_str(), group.c_str(), command.c_str());

    // 3. Generate filename
    std::string timestamp = get_current_timestamp();
    std::string filename = "log_" + timestamp + ".json";

    // 4. Use FdGuard for file handling (to manage file opening and closing)
    FdGuard check_file(open(filename.c_str(), O_RDONLY));
    bool file_empty = true;

    if (check_file.get() != -1) {
        off_t file_size = lseek(check_file.get(), 0, SEEK_END);
        file_empty = (file_size == 0);
    }

    if (file_empty) {
        // First entry - create new JSON array
        FdGuard json_file(open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644));
        if (json_file.get() != -1) {
            dprintf(json_file.get(), "[\n  %s\n]\n", json_log);
        } else {
            fprintf(stderr, "Failed to open log file for writing: %s\n", filename.c_str());
        }
    } else {
        // Subsequent entries - handle JSON array properly
        FdGuard read_file(open(filename.c_str(), O_RDONLY));
        if (read_file.get() == -1) {
            fprintf(stderr, "Failed to open log file for reading: %s\n", filename.c_str());
            return;
        }

        off_t file_size = lseek(read_file.get(), 0, SEEK_END);
        lseek(read_file.get(), 0, SEEK_SET);
        char* content = (char*)malloc(file_size + 1);
        if (!content) {
            fprintf(stderr, "Failed to allocate memory for file content\n");
            return;
        }
        read(read_file.get(), content, file_size);
        content[file_size] = '\0';
        // Find and remove the last ']'
        char* last_bracket = strrchr(content, ']');
        if (last_bracket) {
            *last_bracket = '\0';
        }
        FdGuard write_file(open(filename.c_str(), O_WRONLY | O_TRUNC, 0644));
        if (write_file.get() != -1) {
            dprintf(write_file.get(), "%s,\n  %s\n]\n", content, json_log);
        } else {
            fprintf(stderr, "Failed to open log file for writing: %s\n", filename.c_str());
        }
        free(content);
    }
}
