#ifndef  _COMPORTOPEN_H_
#define  _COMPORTOPEN_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int comport_open(char *serial_path);
void comport_close(int serial_fd);

#endif   /* ----- #ifndef _COMPORTOPEN_H_  ----- */
