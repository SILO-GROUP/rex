#ifndef LCPEX_TTY_FUNCTIONS_H
#define LCPEX_TTY_FUNCTIONS_H


#include <termios.h>
#include <cstdio>
#include <iostream>
#include <unistd.h>

/**
 * @brief Place terminal referred to by 'fd' in cbreak mode
 *
 * This function places the terminal referred to by the file descriptor 'fd'
 * in cbreak mode (noncanonical mode with echoing turned off). It assumes that
 * the terminal is currently in cooked mode (i.e., it should not be called
 * if the terminal is currently in raw mode, since it does not undo all of
 * the changes made by the ttySetRaw() function).
 *
 * @param fd File descriptor of the terminal
 * @param prevTermios Buffer to store the previous terminal settings
 *
 * @return 0 on success, or -1 on error
 *
 * If 'prevTermios' is non-NULL, then the buffer pointed to by it will be used
 * to return the previous terminal settings.
 */
int ttySetCbreak(int fd, struct termios * prevTermios);

/**
 * @brief Place terminal referred to by 'fd' in raw mode
 *
 * This function places the terminal referred to by the file descriptor 'fd'
 * in raw mode.
 *
 * @param fd File descriptor of the terminal
 * @param prevTermios Buffer to store the previous terminal settings
 *
 * @return 0 on success, or -1 on error
 *
 * If 'prevTermios' is non-NULL, then the buffer pointed to by it will be used
 * to return the previous terminal settings.
 */
int ttySetRaw(int fd, struct termios * prevTermios);


/**
 * @brief Reset terminal mode on program exit
 *
 * @param ttyOrig Original terminal mode to be restored
 *
 * This function resets the terminal mode when the program exits by using the
 * tcsetattr function. If the tcsetattr function returns an error, the perror
 * function is called to print an error message.
 */
void ttyResetExit( struct termios * ttyOrig );



#endif //LCPEX_TTY_FUNCTIONS_H
