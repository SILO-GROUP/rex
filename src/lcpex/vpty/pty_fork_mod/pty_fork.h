#ifndef LCPEX_PTY_FORK_H
#define LCPEX_PTY_FORK_H

#include <sys/types.h>
#include "./pty_master_open.h"
#include <cstdio>
#include "./tty_functions.h"
#include <sys/ioctl.h>

/* Maximum size for pty slave name */
#define MAX_SNAME 1000

/**
 * @brief Fork a child process with a pseudo-terminal
 *
 * This function forks a child process and returns the child's process ID. The
 * child process is attached to a pseudo-terminal and its standard input,
 * output, and error streams are redirected to the slave side of the pseudo-terminal.
 *
 * @param[out] masterFd Pointer to store the file descriptor of the master pty
 * @param[out] slaveName Buffer to store the name of the slave pty
 * @param[in] snLen Length of the 'slaveName' buffer
 * @param[in] slaveTermios Terminal attributes for the slave pty (optional)
 * @param[in] slaveWS Window size for the slave pty (optional)
 *
 * @return Process ID of the child process on success, or -1 on error
 *
 * On success, the file descriptor of the master pty is stored in the location
 * pointed to by 'masterFd', and the name of the slave pty is stored in the buffer
 * pointed to by 'slaveName'. If the buffer is too small to hold the name of the
 * slave, an error of type 'EOVERFLOW' is returned. If 'slaveTermios' is non-NULL,
 * the terminal attributes specified by it will be set for the slave pty. If
 * 'slaveWS' is non-NULL, the window size specified by it will be set for the slave
 * pty.
 */
pid_t ptyFork(int *masterFd, char *slaveName, size_t snLen, const struct termios *slaveTermios, const struct winsize *slaveWS);


#endif //LCPEX_PTY_FORK_H
