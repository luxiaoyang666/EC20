/*********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  ComportInit.c
 *    Description:  This file Init the Serial
 *                 
 *        Version:  1.0.0(30/06/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "30/06/20 16:19:30"
 *                 
 ********************************************************************************/
#include "ComportInit.h"

int comport_init(int serial_fd,Mytermios* mytermios,struct termios* Oldopts)
{
    if(!mytermios || ! Oldopts)
    {
        printf("Invalid arguments.\n");
        return -1;
    }

    int                         retval;
    struct termios              Newopts;

    memset(Oldopts,0,sizeof(Newopts));
    retval = tcgetattr(serial_fd,Oldopts);
    if(-1 == retval)
    {
        printf("Get termios of serial[%d] failure:%s.\n",serial_fd,strerror(errno));
        return -1;
    }

    Newopts = *Oldopts;

    /* 设置输入输出波特率 */
    if(cfsetispeed(&Newopts,mytermios->BaudRate) || cfsetospeed(&Newopts,mytermios->BaudRate))
    {
        printf("Set BaudRate failure:%s.\n",strerror(errno));
        return -2;
    }

    if(1)
        printf("Set BaudRate successfully.\n");

    /* 修改控制模式，保证程序不会占用串口 */
    Newopts.c_cflag |= CLOCAL;  //按位或后赋值

    /* 启动接收器，能够读取串口中读取输入数据 */
    Newopts.c_cflag |= CREAD;

/* For example:
 *
 *     c_cflag:   0 0 0 0 1 0 0 0
 *     CLOCAL:  | 0 0 0 1 0 0 0 0
 *             --------------------
 *                0 0 0 1 1 0 0 0
 *
 * Finally:
 *
 *     c_flag = 0 0 0 1 1 0 0 0;
 *
 * */


    /* CSIZE字符大小掩码，屏蔽其他标志位将与databits相关的标志位置0(默认值) */
    Newopts.c_cflag &= ~CSIZE;


/*  For example:
 *
 *      CSIZE = 0 1 1 1 0 0 0 0 ---> ~CSIZE = 1 0 0 0 1 1 1 1
 *
 *      c_cflag:    0 0 1 0 1 1 0 0
 *      ~CSIZE:  &  1 0 0 0 1 1 1 1     
 *              -----------------------
 *                  0 0 0 0 1 1 0 0
 *
 * Finally:
 *
 *     c_cflag = 0 0 0 0 1 1 00
 *
 * */

    /* 设置数据位 */
    switch(mytermios->databits)
    {
        case 5:
            Newopts.c_cflag |= CS5;
            break;

        case 6:
            Newopts.c_cflag |= CS6;
            break;

        case 7:
            Newopts.c_cflag |= CS7;
            break;

        case 8:
            Newopts.c_cflag |= CS8;
            break;

        default:
            printf("Set databits failure,Unrecognized parameters.\n");
            return -3;
    }


    /* 设置奇偶检验位 */
    switch(mytermios->parity)
    {
        /* 无校验 */
        case 0:
            Newopts.c_cflag &= ~PARENB;
            Newopts.c_iflag &= ~INPCK;
            break;

        /* 奇校验 */
        case 1:
            Newopts.c_cflag |= (PARODD | PARENB);
            Newopts.c_iflag |= INPCK;  
            break;

        /* 使用偶检验 */
        case 2:
            Newopts.c_cflag |= PARENB;
            Newopts.c_cflag &= ~PARODD;
            Newopts.c_iflag |= INPCK;
            break;

        default:
            printf("Set parity failure,Unrecognized parameters.\n");
            return -4;

    }

    /* 设置停止位,不是2就是1 */
    switch(mytermios->stopbits)
    {
        case 1:
            Newopts.c_cflag &= ~CSTOPB;
            break;

        case 2:
            Newopts.c_cflag |= CSTOPB;
            break;

        default:
            printf("Set stopbits failure,Unrecognized parameters.\n");
            return -5;

    }

    /* OPOST 表示处理后输出，这里按照原始数据输出 */
    Newopts.c_oflag &= ~OPOST;

    /* 设置read函数的等待时间与最小接收字节 */
    Newopts.c_cc[VTIME] = 1;  //0.1s
    Newopts.c_cc[VMIN] = 1;  //最少一个字节,若超时前数据达到MIN个字节则read返回，否则返回实际读到的字节数

    /* 清空输入输出缓冲区 */
    if(tcflush(serial_fd,TCIFLUSH))
    {
        printf("Failed to clear the cache:%s\n",strerror(errno));
        return -6;
    }

    if(1)
        printf("All Newopts are ready to set!\n");

    if(tcsetattr(serial_fd,TCSANOW,&Newopts) != 0)
    {
        printf("tcsetattr failure:%s\n",strerror(errno));
        return -7;
    }

    return 1;
}



