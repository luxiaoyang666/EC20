/*********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  comport.c
 *    Description:  The file contains the functions of opening, closing, 
 *                  initializing, and communicating with the serial port.
 *                 
 *        Version:  1.0.0(11/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "11/07/20 15:07:25"
 *                 
 ********************************************************************************/
#include "comport.h"

int comport_open(ComportAttr *comport)
{
    int                retval = -1;

    if(NULL == comport)
    {
        printf("%s,Invalid parameter\n",__func__);
        return retval;
    }

    /* 
     * O_NOCTTY表示打开的是一个终端设备，程序不会成为该
     * 端口的控制终端,O_NONBLOCK使得read处于非阻塞模式 
     *
     * */
    comport->fd = open(comport->SerialName,O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(comport->fd < 0)
    {
        printf("%s,Open %s failed:%s\n",__func__,comport->SerialName,strerror(errno));
        return -1;
    }

    /* 检查串口是否处于阻塞态 */
    if((retval = fcntl(comport->fd,F_SETFL,0)) < 0)
    {
        printf("%s,Fcntl check faile.\n",__func__);
        return -2;
    }

    printf("Starting serial communication process ");

    /* 检查该文件描述符是否对应了终端设备 */
    if(0 == isatty(comport->fd))
    {
        printf("%s:[%d] is not a Terminal equipment.\n",comport->SerialName,comport->fd);
        return -3;
    }

    printf("Open %s successfully.\n",comport->SerialName);

    return 0;
}

int comport_close(ComportAttr *comport)
{
    if(tcflush(comport->fd,TCIOFLUSH))  //清零用于串口通信的缓冲区
    {
        printf("%s,Tcflush faile:%s\n",__func__,strerror(errno));
        return -1;
    }

    /* 将串口设置为原有属性 */
    if(tcsetattr(comport->fd,TCSANOW,&(comport->OldTermios)))
    {
        printf("%s,Set old options failed:%s\n",__func__,strerror(errno));
        return -2;
    }

    close(comport->fd);

    free(comport);

    return 0;
}

/* 初始化串口属性，设置串口用于通信 */
int comport_init(ComportAttr *comport)
{
    char                  baudrate[32] = {0};
    struct termios        NewTermios;


    memset(&NewTermios,0,sizeof(struct termios));
    memset(&(comport->OldTermios),0,sizeof(struct termios));
    if(!comport)
    {
        printf("Invalid parameter.\n");
        return -1;
    }

    /* 获取串口原始属性，这部分是备份 */
    if(tcgetattr(comport->fd,&(comport->OldTermios)))
    {
        printf("%s,Get termios to OldTermios failure:%s\n",__func__,strerror(errno));
        return -2;
    }

    /* 获取串口原始属性，这部分用于设置新属性 */
    if(tcgetattr(comport->fd,&NewTermios))
    {
        printf("%s,Get termios to NewTermios failure:%s\n",__func__,strerror(errno));
        return -3;
    }


    /* 修改控制模式，保证程序不会占用串口 */
    NewTermios.c_cflag |= CLOCAL;

/*  For example:
 *   
 *      c_cflag:   0 0 0 0 1 0 0 0
 *      CLOCAL:  | 0 0 0 1 0 0 0 0
 *              --------------------
 *                 0 0 0 1 1 0 0 0
 *                
 *  Finally:
 *
 *     c_flag = 0 0 0 1 1 0 0 0;
 *
 * */


    /* 启动接收器，能够从串口中读取输入数据 */
    NewTermios.c_cflag |= CREAD;


    /*  CSIZE字符大小掩码，将与设置databits相关的标致位置零 */
    NewTermios.c_cflag &= ~CSIZE;


/* For example:
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
 *     c_cflag = 0 0 0 0 1 1 0 0
 *
 * */

    NewTermios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
/* 
 * ICANON: 标准模式
 * ECHO:   回显所输入的字符
 * ECHOE:  如果同时设置了ICANON标志，ERASE字符删除前一个所输入的字符，WERASE删除前一个输入的单词
 * ISIG:   当接收到INTR/QUIT/SUSP/DSUSP字符，生成一个相应的信号
 *
 * */

    
    NewTermios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
/* 
 * BRKINT: BREAK将会丢弃输入和输出队列中的数据(flush)，并且如果终端为前台进程组的控制终端，则BREAK将会产生一个SIGINT信号发送到这个前台进程组
 * ICRNL:  将输入中的CR转换为NL
 * INPCK:  允许奇偶校验
 * ISTRIP: 剥离第8个bits
 * IXON:   允许输出端的XON/XOF流控
 *
 * */

    /* OPOST: 表示处理后输出，按照原始数据输出 */
    NewTermios.c_oflag &= ~(OPOST);

    if(comport->BaudRate)
    {
        sprintf(baudrate,"B%d",comport->BaudRate);
        cfsetispeed(&NewTermios,(int)baudrate); //设置输入输出波特率
        cfsetospeed(&NewTermios,(int)baudrate);
    }
    else
    {
        cfsetispeed(&NewTermios,B115200);
        cfsetospeed(&NewTermios,B115200);
    }

    /* 设置数据位 */
    switch(comport->DataBits)
    {
        case '5':
            NewTermios.c_cflag |= CS5;
            break;

        case '6':
            NewTermios.c_cflag |= CS6;
            break;

        case '7':
            NewTermios.c_cflag |= CS7;
            break;

        case '8':
            NewTermios.c_cflag |= CS8;
            break;

        default:
            NewTermios.c_cflag |= CS8;  //默认数据位为8
            break;
    }

    /* 设置校验方式 */
    switch(comport->Parity)
    {
        /* 无校验 */
        case 'n':
        case 'N':
            NewTermios.c_cflag &= ~PARENB;
            NewTermios.c_iflag &= ~INPCK;
            break;

        /* 偶校验 */
        case 'e':
        case 'E':
            NewTermios.c_cflag |= PARENB;
            NewTermios.c_cflag &= ~PARODD;
            NewTermios.c_iflag |= INPCK;
            break;

        /* 奇校验 */
        case 'o':
        case 'O':
            NewTermios.c_cflag |= PARENB;
            NewTermios.c_cflag |= PARODD;
            NewTermios.c_iflag |= INPCK;

        /* 设置为空格 */
        case 's':
        case 'S':
            NewTermios.c_cflag &= ~PARENB;
            NewTermios.c_cflag &= ~CSTOPB;

        /* 默认无校验 */
        default:
            NewTermios.c_cflag &= ~PARENB;
            NewTermios.c_iflag &= ~INPCK;
            break;
    }

    /* 设置停止位 */
    switch(comport->StopBits)
    {
        case '1':
            NewTermios.c_cflag &= ~CSTOPB;
            break;

        case '2':
            NewTermios.c_cflag |= CSTOPB;
            break;

        default:
            NewTermios.c_cflag &= ~CSTOPB;  //默认使用1位作为停止位
            break;
    }

    NewTermios.c_cc[VTIME] = 0;  //最长等待时间
    NewTermios.c_cc[VMIN] = 2;  //最小接收字符 

    comport->mSend_Len = 128;  //若命令长度大于mSend_Len,则每次最多发送为mSend_Len

    /*  清空用于串口通信的输入输出缓存区 */
    if(tcflush(comport->fd,TCIFLUSH))
    {
        printf("%s,Failed to clear the cache:%s\n",__func__,strerror(errno));
        return -4;
    }

    /* 设置串口属性，除了查看波特率的函数，其余用于串口通信的函数成功均返回0 */
    if(tcsetattr(comport->fd,TCSANOW,&NewTermios) != 0)
    {
        printf("%s,tcsetattr failure:%s\n",__func__,strerror(errno));
        return -5;
    }

    printf("Comport Init Successfully......\n");

    return 0;

}


/* 向串口发送相关指令 */
int comport_send(ComportAttr *comport,char *sbuf,int sbuf_len)
{
    char     *ptr,*end;
    int       retval;

    if(!comport || !sbuf || sbuf_len <= 0)
    {
        printf("%s,Invalid parameter.\n",__func__);
        return -1;
    }

    if(sbuf_len > comport->mSend_Len)  //指令长度实际长度大于单次发送的最大长度，则每次发送单次发送的最大长度
    {
        ptr = sbuf;
        end = sbuf + sbuf_len;

        do
        {
            if(comport->mSend_Len < (end - ptr))  //剩余长度大于单次发送的最大长度
            {
                retval = write(comport->fd,ptr,comport->mSend_Len);
                if(retval <= 0 || retval != comport->mSend_Len)
                {
                    printf("Write to com port[%d] failed:%s\n",comport->fd,strerror(errno));
                    return -2;
                }

                ptr += comport->mSend_Len;
            }
            else
            {
                retval = write(comport->fd,ptr,(end - ptr));  //剩余长度可一次性发送
                if(retval <= 0 || retval != (end - ptr))
                {
                    printf("Write to com port[%d] failed:%s\n",comport->fd,strerror(errno));
                    return -3;
                }

                ptr += (end - ptr);
            }
        }while(end > ptr);

    }

    else
    {
        retval = write(comport->fd,sbuf,sbuf_len);
        if(retval <= 0 || retval != sbuf_len)
        {
            printf("Write to com port[[%d] failed:%s\n",comport->fd,strerror(errno));
            return -4;
        }
    }

    return retval;
}

int comport_recv(ComportAttr *comport,char *rbuf,int rbuf_len,int timeout)
{
    int                   retval;
    fd_set                rset;
    struct timeval        time_out;

    if(!rbuf || rbuf_len <= 0)
    {
        printf("%s,Invalid parameter.\n",__func__);
        return -1;
    }

    if(timeout) //如果传入该参数，则调用select指定读的阻塞时间
    {
        time_out.tv_sec = (time_t)timeout;
        time_out.tv_usec = 0;

        FD_ZERO(&rset);
        FD_SET(comport->fd,&rset);

        retval = select(comport->fd + 1,&rset,NULL,NULL,&time_out);
        if(retval < 0)
        {
            printf("%s,Select failed:%s\n",__func__,strerror(errno));
            return -2;
        }

        else if(0 == retval)
        {
            printf("Time Out.\n");
            return 0;
        }

    }

    /* 延时，避免数据还未到 */
    usleep(1000);

    retval = read(comport->fd,rbuf,rbuf_len);
    if( retval <= 0)
    {
        printf("%s,Read failed:%s\n",__func__,strerror(errno));
        return -3;
    }

    return retval;

}



