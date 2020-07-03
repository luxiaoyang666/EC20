#ifndef  _COMPORTSWAP_H_
#define  _COMPORTSWAP_H_

#include <stdio.h>
#include <termios.h> 
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include "ComportInit.h"

int comport_send(struct _st_mystructure *mystructure, char *buf, int buf_size);
int comport_recv(struct _st_mystructure *mystructure, char *buf, int buf_size, int timeout);


#endif   /* ----- #ifndef _COMPORTSWAP_H_  ----- */
