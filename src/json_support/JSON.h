#ifndef REX_JSON_H
#define REX_JSON_H

#include "jsoncpp/json.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include "../misc/helpers.h"
#include "../logger/Logger.h"

/**
 * @class JSON_Loader
 * @brief Loads and parses JSON data
 *
 * This class is responsible for loading and parsing JSON data from a file or a string. It uses the `JsonCpp` library to
 * handle the parsing and provides functions to access the data in a safe and convenient way.
 *
 * @param LOG_LEVEL - The log level to use for logging messages.
 */
class JSON_Loader {
    protected:
        /// The root of the JSON data
        Json::Value json_root;
        /// Indicates whether the JSON data has been successfully loaded and parsed
        bool populated;

    public:
        /**
         * @brief Constructor for the JSON_Loader class
         *
         * This constructor initializes the JSON_Loader object and sets the log level to use for logging messages.
         *
         * @param LOG_LEVEL - The log level to use for logging messages.
         */
        JSON_Loader(int LOG_LEVEL);

        /**
         * @brief Loads JSON data from a file
         *
         * This function loads JSON data from a file and parses it using the `JsonCpp` library.
         *
         * @param filename - The path to the file to load the JSON data from.
         */
        void load_json_file(std::string filename);

        /**
         * @brief Loads JSON data from a string
         *
         * This function parses JSON data from a string using the `JsonCpp` library.
         *
         * @param input - The string containing the JSON data.
         */
        void load_json_string(std::string input);

        /**
         * @brief Returns the JSON data as a string
         *
         * This function returns the JSON data in string form.
         *
         * @return The JSON data as a string.
         */
        std::string as_string();

        /**
         * @brief Gets the value of a key in the JSON data
         *
         * This function retrieves the value of a key in the JSON data and returns it in a `Json::Value` object. If the
         * key does not exist, an error is logged and a default `Json::Value` object is returned.
         *
         * @param input - A reference to a `Json::Value` object to store the result.
         * @param key - The key to retrieve the value for.
         *
         * @return 0 if the key was found and its value was retrieved successfully, 1 otherwise.
         */
        int get_serialized(Json::Value &input, std::string key);

        /**
         * @brief Gets the string value of a key in the JSON data
         *
         * This function retrieves the string value of a key in the JSON data and returns it in a `std::string` object.
         * If the key does not exist or its value is not a string, an error is logged and a default `std::string` object
         * is returned.
         *
         * @param input - A reference to a `std::string` object to store the result.
         * @param key - The key to retrieve the string value for.
         *
         * @return 0 if the key was found and its string value was retrieved successfully, 1 otherwise.
        */
        int get_string(std::string &input, std::string key);

        /**
         * @brief Gets the boolean value of a key in the JSON data
         *
         * This function retrieves the boolean value of a key in the JSON data and returns it in a `bool` object. If the
         * key does not exist or its value is not a boolean, an error is logged and a default `bool` object is returned.
         *
         * @param input - A reference to a `bool` object to store the result.
         * @param key - The key to retrieve the boolean value for.
         *
         * @return 0 if the key was found and its boolean value was retrieved successfully, 1 otherwise.
         */
        int get_bool(bool &input, std::string key);

    private:
        /// The logger object used for logging messages
        Logger slog;
        /// The log level to use for logging messages
        int LOG_LEVEL;
};

#endif //REX_JSON_H
