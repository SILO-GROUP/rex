#ifndef LCPEX_CONTEXTS_H
#define LCPEX_CONTEXTS_H

#include <string>
#include <csignal>
#include <pwd.h>
#include <grp.h>
#include <iostream>

enum IDENTITY_CONTEXT_ERRORS {
    ERROR_NONE = 0,
    ERROR_NO_SUCH_USER,
    ERROR_NO_SUCH_GROUP,
    ERROR_NO_SUCH_USER_IN_GROUP,
    ERROR_SETGID_FAILED,
    ERROR_SETUID_FAILED,
};

/**
 * @brief Converts a username to a user ID (UID)
 *
 * @param username The username to convert
 * @param uid A reference to the resulting UID
 *
 * @return A boolean indicating whether the conversion was successful
 *
 * This function takes a username as a string and returns the corresponding UID in the `uid` parameter.
 * The function returns a boolean indicating whether the conversion was successful.
 * If the username is not found, the function returns false and the value of `uid` is unchanged.
 */
int username_to_uid( std::string username, int & uid );


/**
 * @brief Converts a group name to a group ID (GID)
 *
 * @param groupname The group name to convert
 * @param gid A reference to the resulting GID
 *
 * @return A boolean indicating whether the conversion was successful
 *
 * This function takes a group name as a string and returns the corresponding GID in the `gid` parameter.
 * The function returns a boolean indicating whether the conversion was successful.
 * If the group name is not found, the function returns false and the value of `gid` is unchanged.
 */
int groupname_to_gid( std::string groupname, int & gid );


/**
 * @brief Sets the execution context for a process
 *
 * @param user_name The username to use for the execution context
 * @param group_name The group name to use for the execution context
 *
 * @return An error code indicating the result of the context setting
 *
 * This function takes a username and group name as input and sets the execution context for the process.
 * The function converts the username and group name to UID and GID respectively, and sets the process's
 * user and group identity using the setuid() and setgid() functions.
 *
 * If either the username or group name is not found, the function returns `ERROR_NO_SUCH_USER` or `ERROR_NO_SUCH_GROUP` respectively.
 * If the call to setgid() fails, the function returns `ERROR_SETGID_FAILED`.
 * If the call to setuid() fails, the function returns `ERROR_SETUID_FAILED`.
 * If the context setting is successful, the function returns `ERROR_NONE`.
 */
 int set_identity_context( std::string user_name, std::string group_name );


#endif //LCPEX_CONTEXTS_H
