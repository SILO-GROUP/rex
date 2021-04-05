//
// Created by bagira on 6/13/20.
//

#include "Logger.h"

Logger::Logger( int LOG_LEVEL, std::string mask )
{
    this->LOG_LEVEL = LOG_LEVEL;
    this->mask = mask;

    setlogmask( LOG_UPTO( this->LOG_LEVEL ) );
    openlog( "rex", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_PERROR | LOG_LOCAL1 );

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
        syslog( this->LOG_LEVEL, s_msg.c_str() );

        if ( LOG_LEVEL == E_FATAL | LOG_LEVEL == E_WARN )
        {
            std::cerr << "[" << get_8601() << "] [" << ERR << "] " << "[" << this->mask << "] " << msg.c_str() << std::endl;
        } else {
            std::cout << "[" << get_8601() << "] [" << ERR << "] " << "[" << this->mask << "] " << msg.c_str() << std::endl;
        }
    }
}

