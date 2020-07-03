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

int comport_open(struct _st_mystructure *mystructure)
{
    int          retval;
    /* O_NOCTTY表示打开的是一个终端设备，程序不会成为该端口的控制终端 */
    /* O_NONBLOCK 非阻塞 */
    mystructure->fd = open(mystructure->dev_name,O_RDWR | O_NOCTTY | O_NONBLOCK);

    if(mystructure->fd < 0)
    {
        printf("Open %s failure:%s.\n",mystructure->dev_name,strerror(errno));
        return -1;
    }

    if(1)
        printf("Open %s successfully.\n",mystructure->dev_name);

    /* 判断串口是否为阻塞状态 */
    if((retval = fcntl(mystructure->fd,F_SETFL,0)) < 0)
    {
        printf("Fcntl failure!\n");
        return -2;
    }

    printf("Fcntl = %d\n",retval);
    printf("开始\n");

    /* 检查该文件描述符是否是终端设备 */
    if(0 == isatty(mystructure->fd))
    {
        printf("FD[%d] is not an open file descriptor referring to a terminal.\n",mystructure->fd);
        return -3;
    }


    printf("isatty ok\n");

    return 0;
}

void comport_close(struct _st_mystructure *mystructure)
{
    tcflush(mystructure->fd,TCIOFLUSH);

    if((tcsetattr(mystructure->fd,TCSANOW,&(mystructure->Oldopts))) != 0)
    {
        printf("Set to the original property failure:%s\n",strerror(errno));
        exit(-1);
    }

    if(mystructure->fd > 0)
        close(mystructure->fd);

}
