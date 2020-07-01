#ifndef  _COMPORTINIT_H_
#define  _COMPORTINIT_H_

#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define BaudRate1 B9600
#define BaudRate2 B115200

typedef struct MyTermios
{
    int BaudRate;
    int databits;
    /* 0 for no parity,1 for Odd parity,2 for Evev parity */
    int parity;
    int stopbits;
}Mytermios;

int comport_init(int serial_fd,Mytermios* mytermios,struct termios* Oldopts);

#endif   /* ----- #ifndef _COMPORTINIT_H_  ----- */
