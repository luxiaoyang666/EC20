/*********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  ComportOpen.c
 *    Description:  This file is open the EC20 4G serial and return the file descriptor
 *                 
 *        Version:  1.0.0(30/06/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "30/06/20 13:56:56"
 *                 
 ********************************************************************************/
#include "ComportOpen.h"

int comport_open(char *serial_path)
{
    int          fd;
    int          retval;
    /* O_NOCTTY表示打开的是一个终端设备，程序不会成为该端口的控制终端 */
    /* O_NDELAY表示不关心DCD信号线所处的状态（端口的另一端是否激活或者停止） */
    fd = open(serial_path,O_RDWR | O_NOCTTY | O_NDELAY);

    if(-1 == fd)
    {
        printf("Open %s failure:%s.\n",serial_path,strerror(errno));
        return -1;
    }

    if(1)
        printf("Open %s successfully.\n",serial_path);

    /* 判断串口是否为阻塞状态 */
    if((retval = fcntl(fd,F_SETFL,0)) < 0)
    {
        printf("Fcntl failure!\n");
        return -2;
    }

    printf("Fcntl = %d\n",retval);

    /* 检查该文件描述符是否是终端设备 */
    if(0 == isatty(fd))
    {
        printf("FD[%d] is not an open file descriptor referring to a terminal.\n",fd);
        return -3;
    }

    printf("isatty ok\n");

    return fd;
}

void comport_close(int serial_fd,struct termios *Oldopts)
{
    tcflush(serial_fd,TCIOFLUSH);

    if((tcsetattr(serial_fd,TCSANOW,Oldopts)) != 0)
    {
        printf("Set to the original property failure:%s\n",strerror(errno));
        exit(-1);
    }

    close(serial_fd);
}
