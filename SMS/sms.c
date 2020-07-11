/*********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  comport.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(09/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "09/07/20 09:04:41"
 *
 ********************************************************************************/
#include "comport.h"

int comport_open(Serial_attr *attr)
{
    int                i;
    int                retval = -1;

    if(NULL == attr)
    {
        printf("%s,Invalid parameter\n",__func__);
        return retval;
    }

    /* O_NOCTTY表示打开的是一个终端设备，程序不会成为该
     * 端口的控制终端,O_NONBLOCK使得read处于非阻塞模式 */
    attr->fd = open(attr->SerialName,O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(attr->fd < 0)
    {
        printf("%s,Open %s failed:%s\n",__func__,attr->SerialName,strerror(errno));
        return -1;
    }

    /* 检查串口是否处于阻塞态 */
    if((retval = fcntl(attr->fd,F_SETFL,0)) < 0)
    {
        printf("%s,Fcntl check faile.\n",__func__);
        return -2;
    }

    if(0 == isatty(attr->fd))
    {
        printf("%s:[%d] is not a Terminal equipment.\n",attr->SerialName,attr->fd);
        return -3;
    }

    printf("Open %s successfully.\n",attr->SerialName);

    return 0;
}

int comport_close(Serial_attr *attr)
{
    if(tcflush(attr->fd,TCIOFLUSH))  //清零用于串口通信的缓冲区
    {
        printf("%s,Tcflush faile:%s\n",__func__,strerror(errno));
        return -1;
    }

    /* 将串口设置为原有属性 */
    if(tcsetattr(attr->fd,TCSANOW,&(attr->OldTermios)))
    {
        printf("%s,Set old options failed:%s\n",__func__,strerror(errno));
        return -2;
    }

    close(attr->fd);

    return 0;
}

int comport_init(Serial_attr *attr)
{
    int                   retval;
    char                  baudrate[32] = {0};
    struct termios        NewTermios;


    memset(&NewTermios,0,sizeof(struct termios));
    memset(&(attr->OldTermios),0,sizeof(struct termios));
    if(!attr)
    {
        printf("Invalid parameter.\n");
        return -1;
    }

    if(tcgetattr(attr->fd,&(attr->OldTermios)))
    {
        printf("%s,Get termios to OldTermios failure:%s\n",__func__,strerror(errno));
        return -2;
    }

    if(tcgetattr(attr->fd,&NewTermios))
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
 *     c_cflag = 0 0 0 0 1 1 0 0
 *
 * */

    NewTermios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /* 
     * ICANON: 标准模式
     * ECHO: 回显所输入的字符
     * ECHOE: 如果同时设置了ICANON标志，ERASE字符删除前一个所输入的字符，WERASE删除前一个输入的单词
     * ISIG: 当接收到INTR/QUIT/SUSP/DSUSP字符，生成一个相应的信号
     *
     * */

    NewTermios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* 
     * BRKINT: BREAK将会丢弃输入和输出队列中的数据(flush)，并且如果终端为前台进程组的控制终端，则BREAK将会产生一个SIGINT信号发送到这个前台进程组
     * ICRNL: 将输入中的CR转换为NL
     * INPCK: 允许奇偶校验
     * ISTRIP: 剥离第8个bits
     * IXON: 允许输出端的XON/XOF流控
     *
     * */

    /* OPOST: 表示处理后输出，按照原始数据输出 */
    NewTermios.c_oflag &= ~(OPOST);

    if(attr->BaudRate)
    {
        sprintf(baudrate,"B%d",attr->BaudRate);
        cfsetispeed(&NewTermios,(int)baudrate); //设置输入输出波特率
        cfsetospeed(&NewTermios,(int)baudrate);
    }
    else
    {
        cfsetispeed(&NewTermios,B115200);
        cfsetospeed(&NewTermios,B115200);
    }

    /* 设置数据位 */
    switch(attr->DataBits)
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
    switch(attr->Parity)
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
    switch(attr->StopBits)
    {
        case '1':
            NewTermios.c_cflag &= ~CSTOPB;
            break;

        case '2':
            NewTermios.c_cflag |= CSTOPB;
            break;

        default:
            NewTermios.c_cflag &= ~CSTOPB;
            break;
    }

    NewTermios.c_cc[VTIME] = 0;  //最长等待时间
    NewTermios.c_cc[VMIN] = 2;  //最小接收字符 

    attr->mSend_Len = 128;  //若命令长度大于mSend_Len,则每次最多发送为mSend_Len

    if(tcflush(attr->fd,TCIFLUSH))
    {
        printf("%s,Failed to clear the cache:%s\n",__func__,strerror(errno));
        return -4;
    }

    if(tcsetattr(attr->fd,TCSANOW,&NewTermios) != 0)
    {
        printf("%s,tcsetattr failure:%s\n",__func__,strerror(errno));
        return -5;
    }

    printf("Comport Init Successfully......\n");

    return 0;

}



int comport_send(Serial_attr *attr,char *sbuf,int sbuf_len)
{
    char     *ptr,*end;
    int       retval;

    if(!attr || !sbuf || sbuf_len <= 0)
    {
        printf("%s,Invalid parameter.\n",__func__);
        return -1;
    }

    if(sbuf_len > attr->mSend_Len)
    {
        ptr = sbuf;
        end = sbuf + sbuf_len;

        do
        {
            if(attr->mSend_Len < (end - ptr))
            {
                retval = write(attr->fd,ptr,attr->mSend_Len);
                if(retval <= 0 || retval != attr->mSend_Len)
                {
                    printf("Write to com port[%d] failed:%s\n",attr->fd,strerror(errno));
                    return -2;
                }

                ptr += attr->mSend_Len;
            }
            else
            {
                retval = write(attr->fd,ptr,(end - ptr));
                if(retval <= 0 || retval != (end - ptr))
                {
                    printf("Write to com port[%d] failed:%s\n",attr->fd,strerror(errno));
                    return -3;
                }

                ptr += (end - ptr);
            }
        }while(end > ptr);

    }

    else
    {
        retval = write(attr->fd,sbuf,sbuf_len);
        if(retval <= 0 || retval != sbuf_len)
        {
            printf("Write to com port[[%d] failed:%s\n",attr->fd,strerror(errno));
            return -4;
        }
    }

    return retval;
}

int comport_recv(Serial_attr *attr,char *rbuf,int rbuf_len,int timeout)
{
    int                   retval;
    fd_set                rset;
    struct timeval        time_out;

    if(!rbuf || rbuf_len <= 0)
    {
        printf("%s,Invalid parameter.\n",__func__);
        return -1;
    }

    if(timeout) //指定延时等待
    {
        time_out.tv_sec = (time_t)timeout;
        time_out.tv_usec = 0;

        FD_ZERO(&rset);
        FD_SET(attr->fd,&rset);

        retval = select(attr->fd + 1,&rset,NULL,NULL,&time_out);
        if(retval < 0)
        {
            printf("%s,Select failed:%s\n",strerror(errno));
            return -2;
        }

        else if(0 == retval)
        {
            printf("Time Out.\n");
            return 0;
        }

    }

    usleep(1000);

    retval = read(attr->fd,rbuf,rbuf_len);
    if( retval <= 0)
    {
        printf("%s,Read failed:%s\n",__func__,strerror(errno));
        return -3;
    }

    return retval;

}

int text_sms(Serial_attr *attr,char *sms_buf,char *phone_num)
{
    char          rbuf[128] = {0};
    char          sbuf[128] = {0};

    if(!sms_buf || !phone_num)
    {
        printf("Invalid parameter\n");
        return -1;
    }

    if(comport_send(attr,"AT+CMGF=1\r",10) < 0)
    {
        printf("\"AT+CMGF=1\" send failed.\n");
        return -2;
    }
    usleep(10000);

    if(comport_recv(attr,rbuf,sizeof(rbuf),0) <= 0 || !strstr(rbuf,"OK") )
    {
        printf("Recving from serial (AT+CMGF) failed\n");
        return -3;
    }
    sprintf(sbuf,"AT+CMGS=\"%s\"\r",phone_num);
   
    if(comport_send(attr,sbuf,sizeof(sbuf)) < 0)
    {
        printf("\"AT+CMGS\" send failed.\n");
        return -4;
    }
    usleep(10000);

    if(comport_recv(attr,rbuf,sizeof(rbuf),0) <= 0 || !strstr(rbuf,">") )
    {
        printf("Recving from serial (AT+CMGS) failed\n");
        return -5;
    }

    strcat(sms_buf,"\x1a");
    if(comport_send(attr,sms_buf,128) < 0)
    {   
        printf("Send SMS failed.\n");
        return -6;
    }  
 
    printf("Sending.......\n");
    if(comport_recv(attr,rbuf,sizeof(rbuf),10) <= 0 || !strstr(rbuf,"OK") )
    {
        printf("%s",rbuf);
        printf("Send over buf can't recieve OK\n");
        return -7;
    }

    printf("Over\n");
    return 0;
}

