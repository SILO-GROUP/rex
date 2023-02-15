#include "helpers.h"

/**
 * @brief Determines if a file or directory exists
 *
 * This function takes a file or directory name as input and uses the `stat` function to determine if the
 * specified file or directory exists.
 *
 * @param name The name of the file or directory to check
 *
 * @return `true` if the file or directory exists, `false` otherwise
 */
bool exists(const std::string& name)
{
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}


/**
 * @brief Returns the current working directory as a string
 *
 * This function uses the `getcwd` function to obtain the current working directory. If the `getcwd`
 * function fails, an empty string is returned.
 *
 * @return The current working directory as a string
 */
std::string get_working_path()
{
    char temp[MAXPATHLEN];
    if (!getcwd(temp, MAXPATHLEN)) {
        return "";
    }
    return temp;
}


/**
 * @brief Determines if a given file path is a regular file
 *
 * This function takes a file path as input and uses the `stat` function to obtain information about the
 * file or directory. The `st_mode` member of the `stat` structure is then checked to determine if the
 * file is a regular file or not.
 *
 * @param path The file path to be checked
 *
 * @return `true` if the file path is a regular file, `false` otherwise
 */
bool is_file( std::string path)
{
    struct stat buf;
    stat( path.c_str(), &buf );
    return S_ISREG(buf.st_mode);
}


/**
 * @brief Determines if a given file path is a directory
 *
 * This function takes a file path as input and uses the `stat` function to obtain information about the
 * file or directory. The `st_mode` member of the `stat` structure is then checked to determine if the
 * file is a directory or not.
 *
 * @param path The file path to be checked
 *
 * @return `true` if the file path is a directory, `false` otherwise
 */
bool is_dir( std::string path )
{
    struct stat buf;
    stat( path.c_str(), &buf );
    return S_ISDIR(buf.st_mode);
}


/**
 * @brief Returns a string representation of the current date and time in the format "YYYY-MM-DD_HH:MM:SS"
 *
 * This function uses the `std::chrono` library to obtain the current system time and convert it to a
 * time_t value. It then uses the `strftime` function to format the date and time into a string with the
 * format "YYYY-MM-DD_HH:MM:SS". The resulting string is returned as the function result.
 *
 * @return A string representation of the current date and time
 */
std::string get_8601()
{
    auto now = std::chrono::system_clock::now();
    auto itt = std::chrono::system_clock::to_time_t(now);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d_%H:%M:%S", localtime(&itt));
    return buf;
}


/**
 * @brief Interpolates the environment variables in the input text
 *
 * This function takes a string reference as input and replaces all occurrences of
 * environment variables in the format `${VAR_NAME}` with their corresponding values.
 * If an environment variable is not set, it is replaced with an empty string.
 *
 * @param text The input text to be processed
 */
void interpolate( std::string & text )
{
    static std::regex env( "\\$\\{([^}]+)\\}" );
    std::smatch match;
    while ( std::regex_search( text, match, env ) )
    {
        const char * s = getenv( match[1].str().c_str() );
        const std::string var( s == NULL ? "" : s );
        text.replace( match[0].first, match[0].second, var );
    }
}
