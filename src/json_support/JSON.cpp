/*
    Rex - A configuration management and workflow automation tool that
    compiles and runs in minimal environments.

    © SILO GROUP LLC, 2023.

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
#include "JSON.h"


/**
 * @class JSON_Loader_NotReady
 * @brief Exception thrown when a member function is called before data is populated.
 *
 * This class is derived from the standard library class `std::runtime_error` and is thrown
 * when a member function is called before the JSON data has been populated.
 */
class JSON_Loader_NotReady : public std::runtime_error
{
    public:
        /**
         * @brief Constructor for the JSON_Loader_NotReady exception.
         *
         * This constructor creates an instance of the JSON_Loader_NotReady exception and sets its error message.
         */
        JSON_Loader_NotReady() : std::runtime_error("JSON_Loader: Tried to access JSON without actually populating JSON.") {}
};

/**
 * @class JSON_Loader_FileNotFound
 * @brief Exception thrown when the requested file could not be found.
 *
 * This class is derived from the standard library class `std::runtime_error` and is thrown
 * when the file specified in the JSON_Loader constructor could not be found.
 */
class JSON_Loader_FileNotFound : public std::runtime_error
{
    public:
        /**
         * @brief Constructor for the JSON_Loader_FileNotFound exception.
         *
         * This constructor creates an instance of the JSON_Loader_FileNotFound exception and sets its error message.
         */
        JSON_Loader_FileNotFound() : std::runtime_error("JSON_Loader: The requested file could not be found.") {}
};

/**
 * @class JSON_Loader_InvalidJSON
 * @brief Exception thrown when the provided JSON could not be parsed.
 *
 * This class is derived from the standard library class `std::runtime_error` and is thrown
 * when the JSON data provided to the JSON_Loader could not be parsed.
 */
class JSON_Loader_InvalidJSON : public std::runtime_error
{
    public:
        /**
         * @brief Constructor for the JSON_Loader_InvalidJSON exception.
         *
         * This constructor creates an instance of the JSON_Loader_InvalidJSON exception and sets its error message.
         */
        JSON_Loader_InvalidJSON() : std::runtime_error("JSON_Loader: The JSON provided could not be parsed.") {}
};

/**
 * @class JSON_Loader
 * @brief The base class for loading and parsing JSON data.
 *
 * The JSON_Loader class provides the functionalities shared between its derived classes, Suite and Plan.
 * It is responsible for initializing the JSON data to an unpopulated state.
 */

/**
 * @brief Constructor for the JSON_Loader base class.
 *
 * This constructor initializes the JSON data to an unpopulated state and sets the logging level.
 *
 * @param LOG_LEVEL The logging level for the JSON_Loader instance.
 */
JSON_Loader::JSON_Loader(int LOG_LEVEL) : slog(LOG_LEVEL, "_json_")
{
    this->populated = false;
    this->LOG_LEVEL = LOG_LEVEL;
}

/**
 * @brief Loads a JSON-formatted string into the JSON_Loader instance.
 *
 * This function takes a JSON-formatted string as input, parses it using a Json::Reader, and stores the result in the protected member `json_root`.
 * If the parsing is successful, the `populated` flag is set to `true`.
 *
 * @param input The JSON-formatted string to be loaded into the JSON_Loader instance.
 * @throws JSON_Loader_InvalidJSON if the input string could not be parsed.
 */
void JSON_Loader::load_json_string(std::string input)
{
    Json::Reader json_reader;

    std::ifstream json_file_ifstream(input.c_str(), std::ifstream::binary);

    bool parsingSuccessful = json_reader.parse(json_file_ifstream, this->json_root);

    if (!parsingSuccessful)
    {
        this->slog.log(E_FATAL, "Failed to parse adhoc JSON value: " + json_reader.getFormattedErrorMessages());
        throw JSON_Loader_InvalidJSON();
    }
    else
    {
        this->slog.log(E_DEBUG, "Successfully parsed JSON string with " + std::to_string(this->json_root.size()) + " elements. Value: '" + input + "'.");
    }

    this->populated = true;
}

/**
 * @brief Loads a JSON-formatted file into the JSON_Loader instance.
 *
 * This function takes a file path as input, parses the file using a Json::Reader, and stores the result in the protected member `json_root`.
 * If the parsing is successful, the `populated` flag is set to `true`.
 *
 * @param filename The file path to the JSON-formatted file to be loaded into the JSON_Loader instance.
 * @throws JSON_Loader_FileNotFound if the file specified in `filename` could not be found.
 * @throws JSON_Loader_InvalidJSON if the contents of the file could not be parsed.
 */
void JSON_Loader::load_json_file(std::string filename)
{
    Json::Reader json_reader;

    if (!exists(filename))
    {
        this->slog.log_task(E_FATAL, "LOAD", "File '" + filename + "' does not exist.");
        throw JSON_Loader_FileNotFound();
    }

    std::ifstream json_file_ifstream(filename, std::ifstream::binary);

    bool parsingSuccessful = json_reader.parse(json_file_ifstream, this->json_root);

    if (!parsingSuccessful)
    {
        this->slog.log_task(E_FATAL, "PARSING", "Failed to parse file '" + filename + "': " + json_reader.getFormattedErrorMessages());
        throw JSON_Loader_InvalidJSON();
    }
    else
    {
        this->slog.log_task(E_DEBUG, "PARSING", "Parsed '" + filename + "' with " + std::to_string(this->json_root.size()) + " element(s).");
    }

    this->populated = true;
}


/**
 * @brief Returns the string representation of the `json_root` member.
 *
 * This function returns the string representation of the `json_root` member, which contains the parsed JSON data.
 *
 * @return The string representation of the `json_root` member.
 * @throws JSON_Loader_NotReady if the JSON data has not yet been populated.
 */
std::string JSON_Loader::as_string()
{
    if (!this->populated)
    {
        throw JSON_Loader_NotReady();
    }

    return this->json_root.asString();
}

/**
 * @brief Assigns the serialized representation of a specified key to a passed Json::Value reference.
 *
 * This function takes a reference to a Json::Value object as input, along with a string representing the key to search for.
 * If the key is found in the `json_root` member, its value is assigned to the passed Json::Value reference.
 *
 * @param input A reference to the Json::Value object to receive the value of the specified key.
 * @param key The key name to search for in the `json_root` member.
 * @return 0 if the key was found and its value was assigned to the passed Json::Value reference, 1 if the key was not found.
 * @throws JSON_Loader_NotReady if the JSON data has not yet been populated.
 */
int JSON_Loader::get_serialized(Json::Value &input, std::string key)
{
    if (!this->populated)
    {
        throw JSON_Loader_NotReady();
    }

    if (this->json_root.isMember(key))
    {
        input = this->json_root[key];
        return 0;
    }

    this->slog.log_task(E_FATAL, "PARSING", "Failed to find key '" + key + "'.");

    return 1;
}

/**
 * @brief Assigns the string representation of a specified key to a passed string reference.
 *
 * This function takes a reference to a string object as input, along with a string representing the key to search for.
 * If the key is found in the `json_root` member, its string representation is assigned to the passed string reference.
 *
 * @param input A reference to the string object to receive the string representation of the specified key.
 * @param key The key name to search for in the `json_root` member.
 * @return 0 if the key was found and its string representation was assigned to the passed string reference, 1 if the key was not found.
 * @throws JSON_Loader_NotReady if the JSON data has not yet been populated.
 */
int JSON_Loader::get_string(std::string &input, std::string key)
{
    if (!this->populated)
    {
        throw JSON_Loader_NotReady();
    }

    if (this->json_root.isMember(key))
    {
        input = this->json_root[key].asString();
        return 0;
    }

    this->slog.log_task(E_FATAL, "PARSING", "Failed to find key '" + key + "'.");

    return 1;
}

/**
 * @brief Assigns the boolean representation of a specified key to a passed bool reference.
 *
 * This function takes a reference to a bool object as input, along with a string representing the key to search for.
 * If the key is found in the `json_root` member, its boolean representation is assigned to the passed bool reference.
 *
 * @param input A reference to the bool object to receive the boolean representation of the specified key.
 * @param key The key name to search for in the `json_root` member.
 * @return 0 if the key was found and its boolean representation was assigned to the passed bool reference, 1 if the key was not found.
 * @throws JSON_Loader_NotReady if the JSON data has not yet been populated.
 */
int JSON_Loader::get_bool(bool &input, std::string key)
{
    if (!this->populated)
    {
        throw JSON_Loader_NotReady();
    }

    if (this->json_root.isMember(key))
    {
        input = this->json_root[key].asBool();
        return 0;
    }

    this->slog.log_task(E_FATAL, "PARSING", "Failed to find key '" + key + "'.");

    return 1;
}