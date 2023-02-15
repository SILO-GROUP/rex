#ifndef LCPEX_STRING_EXPANSION_H
#define LCPEX_STRING_EXPANSION_H

#include <wordexp.h>
#include <vector>
#include <cstring>
#include <iostream>

/**
 * @brief Convert a string to a 'char**' representing an argument list
 *
 * This function takes a string 'var' and converts it into a 'char**'
 * representing an argument list. The argument list can be consumed by
 * functions like 'execvp'.
 *
 * @param[in] var The string to be converted
 * @param[in] flags Flags controlling the behavior of the conversion
 *
 * @return A 'char**' representing the argument list
 *
 * The function uses the 'wordexp' function to perform the conversion. The
 * 'flags' argument controls the behavior of the 'wordexp' function. The
 * returned 'char**' is dynamically allocated, and must be freed by the
 * caller using 'delete[]'.
 */
char ** expand_env(const std::string& var, int flags = 0);


#endif //LCPEX_STRING_EXPANSION_H
