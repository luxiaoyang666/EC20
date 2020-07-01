#ifndef  _COMPORTOPEN_H_
#define  _COMPORTOPEN_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

int comport_open(char *serial_path);
void comport_close(int serial_fd,struct termios *Oldopts);

#endif   /* ----- #ifndef _COMPORTOPEN_H_  ----- */
