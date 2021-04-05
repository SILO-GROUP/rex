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

