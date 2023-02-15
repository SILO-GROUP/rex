#include "string_expansion.h"

// convert a string to a char** representing our artificial argv to be consumed by execvp
char ** expand_env(const std::string& var, int flags )
{
    std::vector<std::string> vars;

    wordexp_t p;
    if(!wordexp(var.c_str(), &p, flags))
    {
        if(p.we_wordc)
            for(char** exp = p.we_wordv; *exp; ++exp)
                vars.push_back(exp[0]);
        wordfree(&p);
    }

    char ** arr = new char*[vars.size() + 1];
    for(size_t i = 0; i < vars.size(); i++){
        arr[i] = new char[vars[i].size() + 1];
        strcpy(arr[i], vars[i].c_str());
    }
    arr[vars.size()] = (char * ) nullptr;
    return arr;
}
