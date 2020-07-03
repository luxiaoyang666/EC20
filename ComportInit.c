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

int comport_init(struct _st_mystructure *mystructure)
{
    int                         retval;
    struct termios              Newopts;

    retval = tcgetattr(mystructure->fd,&(mystructure->Oldopts));
    if(-1 == retval)
    {
        printf("Get termios of serial[%d] failure:%s.\n",mystructure->fd,strerror(errno));
        return -1;
    }

    retval = tcgetattr(mystructure->fd,&Newopts);
    if(-1 == retval)                               
    {                                 
        printf("Get termios of serial[%d] failure:%s.\n",mystructure->fd,strerror(errno));
        return -2;    
    } 
    
    
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


    /* ICANON:正规模式 ECHO:回显输入字符 */
    Newopts.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Input*/
    Newopts.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    Newopts.c_oflag  &= ~(OPOST);   /* Output*/

    /* OPOST 表示处理后输出，这里按照原始数据输出 */


    /* 设置输入输出波特率 */
    if(cfsetispeed(&Newopts,mystructure->BaudRate) || cfsetospeed(&Newopts,mystructure->BaudRate))
    {
        printf("Set BaudRate failure:%s.\n",strerror(errno));
        return -3;
    }

    if(1)
        printf("Set BaudRate successfully.\n");


    /* 设置数据位 */
    switch(mystructure->databits)
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
            Newopts.c_cflag |= CS8;
            break;
    }


    /* 设置奇偶检验位 */
    switch(mystructure->parity)
    {
        /* 无校验 */
        case 0:
            Newopts.c_cflag &= ~PARENB;
            break;

        /* 奇校验 */
        case 1:
            Newopts.c_cflag |= (PARODD | PARENB);
            Newopts.c_cflag |= (INPCK | ISTRIP);
            
            break;

        /* 使用偶检验 */
        case 2:
            Newopts.c_cflag |= PARENB;
            Newopts.c_cflag &= ~PARODD;
            Newopts.c_cflag |= (INPCK | ISTRIP);
            break;

        default:
            Newopts.c_cflag &= ~PARENB;
            break;

    }

    /* 设置停止位,不是2就是1 */
    switch(mystructure->stopbits)
    {
        case 1:
            Newopts.c_cflag &= ~CSTOPB;
            break;

        case 2:
            Newopts.c_cflag |= CSTOPB;
            break;

        default:
            Newopts.c_cflag &= ~CSTOPB;
            break;

    }

    mystructure->frag_size = 128;

    /* 设置read函数的等待时间与最小接收字节 */
    Newopts.c_cc[VTIME] = 2;  //0.1s
    Newopts.c_cc[VMIN] = 1;  //最少一个字节,若超时前数据达到MIN个字节则read返回，否则返回实际读到的字节数

    /* 清空输入缓冲区 */
    if(tcflush(mystructure->fd,TCIFLUSH))
    {
        printf("Failed to clear the cache:%s\n",strerror(errno));
        return -7;
    }

    printf("All Newopts are ready to set!\n");

    if(tcsetattr(mystructure->fd,TCSANOW,&Newopts) != 0)
    {
        printf("tcsetattr failure:%s\n",strerror(errno));
        return -8;
    }

    printf("Comport Init Over!\n");

    return 0;
}



