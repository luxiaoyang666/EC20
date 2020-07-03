/*********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  comport.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(30/06/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "30/06/20 14:29:45"
 *                 
 ********************************************************************************/
#include "ComportOpen.h"
#include "ComportInit.h"
#include "ComportSwap.h"
#include <signal.h>
#include <getopt.h>
#include <sys/ioctl.h>


int g_stop = 0;

void handler(int sig_num)
{
    if(sig_num == SIGINT)
        g_stop = 1;
}
void text(char *buf);
void adjust_buf(char* buf);
void help_information();

int main(int argc, char *argv[])
{
    int               rv;
    int               retval;
    int               ch;
    char              send_buf[128] = {0};
    char              recv_buf[128] = {0};
    MyStructure       mystructure;
    struct timeval    timeout;
    fd_set            rset;
    struct option     options[] = {
        {"help",no_argument,NULL,'h'},
        {"baudrate",required_argument,NULL,'b'},
        {"databits",required_argument,NULL,'d'},
        {"parity",required_argument,NULL,'p'},
        {"stopbits",required_argument,NULL,'s'},
        {"name",required_argument,NULL,'n'},
        {NULL,0,NULL,0}
    };

    memset(&mystructure,0,sizeof(mystructure));

    /*  默认模特率  */
    mystructure.BaudRate = B115200;
    while((ch = getopt_long(argc,argv,"hb:d:p:s:n:",options,NULL)) != -1)
    {
        switch(ch)
        {
            case 'h':
                help_information();
                return 0;

            case 'b':
                mystructure.BaudRate = atoi(optarg);
                break;

            case 'd':
                mystructure.databits = atoi(optarg);
                break;

            case 'p':
                mystructure.parity = atoi(optarg);
                break;

            case 's':
                mystructure.stopbits = atoi(optarg);
                break;

            case 'n':
                strcpy(mystructure.dev_name,optarg);
                printf("%s\n",mystructure.dev_name);
                break;
                
        }
    }

//    timeout.tv_sec = 20;
//    timeout.tv_usec = 0;

    if(strlen(mystructure.dev_name) == 0)
    {
        printf("Parameter warning:\n");
        printf("\tAt least need to enter the serial port name,You can specify the serial port name with -n.\n");
        return 0;
    }
    if((retval = comport_open(&mystructure)) != 0)
    {
        printf("Open failure!\n");
        return -1;
    }

    printf("打开ttyUSB2成功!\n");

    retval = comport_init(&mystructure);
    if(retval == 0)
        printf("\nComport Init successfully!\n");
    else 
        goto cleanup;

    signal(SIGINT,handler);
    fflush(stdin);
    printf("Start to communicate with com port....\n");

    while(!g_stop)
    {
        FD_ZERO(&rset);
        FD_SET(STDIN_FILENO,&rset);
        FD_SET(mystructure.fd,&rset);
        
        rv = select(mystructure.fd + 1,&rset,NULL,NULL,NULL);
        if(rv < 0)
        {
            printf("Select failure\n");
            break;
        }

        if(rv == 0)
        {
            printf("Timeout!\n");
            goto cleanup;
        }

        if(FD_ISSET(STDIN_FILENO,&rset))
        {
            memset(send_buf,0,sizeof(send_buf));
            fgets(send_buf,sizeof(send_buf),stdin);

            adjust_buf(send_buf);
            if(comport_send(&mystructure,send_buf,strlen(send_buf)) < 0)
            {
                printf("Write faliure:%s\n",strerror(errno));
                goto cleanup;
            }
            fflush(stdin);
        }

        if(FD_ISSET(mystructure.fd,&rset))
        {
            memset(recv_buf,0,sizeof(send_buf));
            rv = comport_recv(&mystructure,recv_buf,sizeof(recv_buf),0);
            if(rv <= 0) 
            {
                printf("Recieve failure:%s\n",strerror(errno));
                break;

            }

            printf("%s",recv_buf);
        }


    }

    printf("Byebye\n");
            

cleanup:
    comport_close(&mystructure);
    
    return 0;
}

void adjust_buf(char *buf)
{
    int i = strlen(buf);
    strcpy(&buf[i-1],"\r");
}

void help_information()
{
    printf("\t-b   Set BaudRate\n");
    printf("\t-d   Set Databits\n");
    printf("\t-p   Set Parity,0 for no parity,1 for Odd parity,2 for Evev parity\n");
    printf("\t-s   Set StopBits\n");
    printf("\t-n   Set the name of the serial port you want to use\n");
    printf("\t     Ctrl + c to exit the program\n");

    printf("\n\tIf you do not specify parameters,The default parameters of the program are as follows:\n");
    printf("\tBaudRate: 1115200\n\tDatabits: 8bits\n\tParity:   no parity\n\tStopBits: 1bits\n");

}

