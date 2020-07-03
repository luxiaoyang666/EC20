#ifndef  _COMPORTINIT_H_
#define  _COMPORTINIT_H_

#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define DEVNAME_LEN  32
#define BaudRate1    B9600
#define BaudRate2    B115200

typedef struct _st_mystructure
{ 
    int               BaudRate;
    int               databits;
    /* 0 for no parity,1 for Odd parity,2 for Evev parity */
    int               parity;
    int               stopbits;
    int               fd;
    int               frag_size;
    char              dev_name[DEVNAME_LEN];
    struct termios    Oldopts;
}MyStructure;

int comport_init(struct _st_mystructure *mytermios);

#endif   /* ----- #ifndef _COMPORTINIT_H_  ----- */
