#include "shells.h"

class ShellException: public std::exception
{
    public:
        explicit ShellException(const char* message):
                msg_(message)
        {}

        explicit ShellException(const std::string& message):
                msg_(message)
        {}

        virtual ~ShellException() throw (){}

        virtual const char* what() const throw (){
            return msg_.c_str();
        }

    protected:
        std::string msg_;
};


Shell::Shell(int LOG_LEVEL): JSON_Loader( LOG_LEVEL ), slog( LOG_LEVEL, "_shell" )
{
    this->LOG_LEVEL = LOG_LEVEL;
}


int Shell::load_root( Json::Value loader_root )
{
    // TODO this needs reworked to have errmsg actually end up as a null return from json::value.get()
    std::string errmsg = "SOMETHING WENT TERRIBLY WRONG IN PARSING";

    if ( loader_root.isMember("name") )
    {
        this->name = loader_root.get( "name", errmsg ).asString();
    } else {
        throw ShellException("No name attribute specified when loading a shell definition.");
    }

    if ( loader_root.isMember("path") )
    {
        this->path = loader_root.get( "path", errmsg ).asString();
    } else {
        throw ShellException("No path attribute specified when loading a shell definition.");
    }

    if ( loader_root.isMember("execution_arg") )
    {
        this->execution_arg = loader_root.get( "execution_arg", errmsg ).asString();
    } else {
        throw ShellException("No execution_arg attribute specified when loading a shell definition.");
    }

    if ( loader_root.isMember("source_cmd") )
    {
        this->source_cmd = loader_root.get( "source_cmd", errmsg ).asString();
    } else {
        throw ShellException("No source_cmd attribute specified when loading a shell definition.");
    }
    return 0;
}