//
// Created by phanes on 4/22/17.
//

#include "helpers.h"

bool exists(const std::string& name)
{
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}