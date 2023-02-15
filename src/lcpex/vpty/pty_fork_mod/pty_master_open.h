#ifndef LCPEX_PTY_MASTER_OPEN_H
#define LCPEX_PTY_MASTER_OPEN_H

#include <sys/types.h>
#include <errno.h>
#include <cstdlib>
#include <fcntl.h>
#include <csignal>
#include <cstring>
#include <stdlib.h>
#include <fcntl.h>


/**
 * @brief Open a pseudo-terminal master
 *
 * This function opens a pseudo-terminal master and returns the file descriptor
 * for the master. The name of the corresponding pseudo-terminal slave is also
 * returned in the buffer pointed to by 'slaveName'.
 *
 * @param[out] slaveName Buffer to store the name of the slave pty
 * @param[in] snLen Length of the 'slaveName' buffer
 *
 * @return File descriptor of the master pty on success, or -1 on error
 *
 * On success, the name of the corresponding pseudo-terminal slave is stored in
 * the buffer pointed to by 'slaveName'. If the buffer is too small to hold the
 * name of the slave, an error of type 'EOVERFLOW' is returned.
 */
int ptyMasterOpen(char *slaveName, size_t snLen);

#endif //LCPEX_PTY_MASTER_OPEN_H
