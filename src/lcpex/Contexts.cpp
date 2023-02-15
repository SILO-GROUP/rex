#include "Contexts.h"


// converts username to UID
int username_to_uid( std::string username, int & uid )
{
    // assume failure unless proven otherwise
    int r_code = false;

    struct passwd * pw;
    if ( ( pw = getpwnam( username.c_str() ) ) != NULL )
    {
        // successful user lookup
        r_code = true;
        uid = pw->pw_uid;
    } else {
        // failed lookup, do nothing, assumed failure
    }
    return r_code;
};

// converts group name to GID
int groupname_to_gid( std::string groupname, int & gid )
{
    int r_code = false;

    struct group * gp;
    if ( ( gp = getgrnam( groupname.c_str() ) ) != NULL )
    {
        r_code = true;
        gid = gp->gr_gid;
    } else {
        // failed lookup, do nothing, assumed failure
    }
    return r_code;
}

// SET PROCESS TO A CERTAIN IDENTITY CONTEXT
int set_identity_context( std::string user_name, std::string group_name ) {
    // the UID and GID for the username and groupname provided for context setting
    int context_user_id;
    int context_group_id;

    int res = 0;

    // convert username to UID
    if (! ( res = username_to_uid(user_name, context_user_id ) ) )
    {
        return ERROR_NO_SUCH_USER;
    }

    // convert group name to GID
    if (! ( res = groupname_to_gid(group_name, context_group_id ) ) )
    {
        return ERROR_NO_SUCH_GROUP;
    }

    if ( ( res = setgid(context_group_id) ) ) {
        perror("lcpex: setgid failed");
        return ERROR_SETGID_FAILED;
    }

    if ( ( res = setuid(context_user_id) ) ) {
        perror("lcpex: setuid failed");
        return ERROR_SETUID_FAILED;
    }

    return ERROR_NONE;
}

