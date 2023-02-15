#ifndef LCPEX_HELPERS_H
#define LCPEX_HELPERS_H

// helper for sanity
enum PIPE_ENDS {
    READ_END = 0,
    WRITE_END = 1
};

// helper for sanity
// these do NOT represent fd values
enum CHILD_PIPE_NAMES {
    STDOUT_READ = 0,
    STDERR_READ = 1
};

#define BUFFER_SIZE 1024



#endif //LCPEX_HELPERS_H
