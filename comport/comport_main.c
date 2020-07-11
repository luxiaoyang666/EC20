/*********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  comport_main.c
 *    Description:  This file is Used for serial communication
 *                 
 *        Version:  1.0.0(04/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "04/07/20 03:02:28"
 *                 
 ********************************************************************************/
#include "comport.h"
#include <signal.h>
#include <getopt.h>

int           g_stop = 0;
ComportAttr   comport;

/* 捕捉 Ctrl+c 让程序完成清理工作和复位工作后再退出 */
void sig_handler1(int signum)
{
    if(signum == SIGINT)
        g_stop = 1;
}

/* 捕捉 Ctrl+Z 发送短信 */
void sig_handler2(int signum)
{
    if(signum == SIGTSTP)
        write(comport.fd,"\x1a",2);  //短信结束标致
}

/* 处理存储了AT指令的数组 */
void at_cmd_procl(char* buf,int buf_len);

/* 处理存储了短信的数组 */
void sms_procl(char* buf,int buf_len);

/* 打印程序的帮助信息 */
static inline void help_information();

int main(int argc, char *argv[])
{
    int                 ch;
    int                 retval;
    int                 send_flag = 0;
    char                sbuf[512] = {0};
    char                rbuf[512] = {0};
    fd_set              rset;

    struct option    options[] = {
        {"help",no_argument,NULL,'h'},
        {"baudrate",required_argument,NULL,'b'},
        {"databits",required_argument,NULL,'d'},
        {"parity",required_argument,NULL,'p'},
        {"stopbits",required_argument,NULL,'s'},
        {"name",required_argument,NULL,'n'},
        {NULL,0,NULL,0}
    };

    memset(&comport,0,sizeof(ComportAttr));
    
    /* 获取命令行参数 */
    while((ch = getopt_long(argc,argv,"hb:d:p:s:n:",options,NULL)) != -1)
    {
        switch(ch)
        {
            case 'h':
                help_information();
                return 0;

            case 'b':
                comport.BaudRate = atoi(optarg);
                break;

            case 'd':
                comport.DataBits = atoi(optarg);
                break;

            case 'p':
                comport.Parity = optarg[0];
                break;

            case 's':
                comport.StopBits = atoi(optarg);
                break;

            case 'n':
                strncpy(comport.SerialName,optarg,SERIALNAME_LEN);
                break;
        }
    }


    /* 没有传入串口名 */
    if(strlen(comport.SerialName) == 0)
    {
        printf("Parameter warning:\n");
        printf("\tAt least need to enter the serial port name,You can specify the serial port name with -n.\n");
        return 0;
    }

    if(comport_open(&comport) != 0)
    {
        printf("Open %s failed!\n",comport.SerialName);
        return -1;
    }

    retval = comport_init(&comport);
    if(retval < 0)
        goto cleanup;

    signal(SIGINT,sig_handler1);
    signal(SIGTSTP,sig_handler2);
    
    fflush(stdin);
    printf("Start to communicate with com port......\n");

    while(!g_stop)
    {
        FD_ZERO(&rset);
        FD_SET(STDIN_FILENO,&rset);
        FD_SET(comport.fd,&rset);


        /*  使用多路复用监听标准输入和串口fd */
        retval = select(comport.fd + 1,&rset,NULL,NULL,0);
        if(retval < 0)
        {
            continue;
        }

        if(retval == 0)
        {
            printf("Time Out!.\n");
            goto cleanup;
        }

        if(FD_ISSET(STDIN_FILENO,&rset))
        {
            memset(sbuf,0,sizeof(sbuf));

            /* 从标准输入读取命令或短信内容 */
            fgets(sbuf,sizeof(sbuf),stdin);

            /* send_flag == 1 时表示此时为普通AT指令，否则为短信内容 */
            if(send_flag == 1)
                sms_procl(sbuf,sizeof(sbuf));
            else 
                at_cmd_procl(sbuf,sizeof(sbuf));

            if(comport_send(&comport,sbuf,strlen(sbuf)) < 0)
            {
                printf("Write failed.\n");
                goto cleanup;
            }

            /* 清理标准输入 */
            fflush(stdin);
        }

        /* 串口有数据到来 */
        if(FD_ISSET(comport.fd,&rset))
        {
            memset(rbuf,0,sizeof(rbuf));

            retval = comport_recv(&comport,rbuf,sizeof(rbuf),0);
            if(retval <= 0)
            {
                printf("Read failed:%s\n",strerror(errno));
                break;
            }

            printf("%s",rbuf);

            /* 判断是否为短信输入模式 */
            if(strstr(rbuf,">"))
                send_flag = 1;
            else 
                send_flag = 0;

            fflush(stdout);
        }
        
    }

cleanup:
    comport_close(&comport);

    printf("Byebye!\n");

    return 0;

}

/* 修改fgets()带来的LF为CR */
void at_cmd_procl(char *buf,int buf_len)
{
    int      i = 0;
    for(i; i < buf_len; i++)
    {
        if(buf[i] == 0x0a)
            buf[i] = 0x0d;
    }
}

/*  修改fgets()带来的LF为 '\0' */
void sms_procl(char *buf,int buf_len)
{
    int      i = 0;
    for(i; i < buf_len; i++)
    {    
        if(buf[i] == 0x0a)
            buf[i] = 0;
    }
}


static inline void help_information()
{
    printf("\t-b   Set BaudRate\n");
    printf("\t-d   Set Databits\n");
    printf("\t-p   Set Parity,N,n for no parity,O,o for Odd parity,E,e for Evev parity,S,s for space\n");
    printf("\t-s   Set StopBits\n");
    printf("\t-n   Set the name of the serial port you want to use\n");
    printf("\tCtrl + c to exit the program\n");
    printf("\n\tIf you do not specify parameters,The default parameters of the program are as follows:\n");
    printf("\tBaudRate: 1115200\n\tDatabits: 8bits\n\tParity:   no parity\n\tStopBits: 1bits\n");
}

