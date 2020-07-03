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
#include "ComportInit.h"

int comport_open(struct _st_mystructure *mystructure);
void comport_close(struct _st_mystructure *mystructure);

#endif   /* ----- #ifndef _COMPORTOPEN_H_  ----- */
