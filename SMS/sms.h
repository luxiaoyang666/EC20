/********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  comport.h
 *    Description:  This head file of comport
 *
 *        Version:  1.0.0(09/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "09/07/20 08:58:33"
 *                 
 ********************************************************************************/
#ifndef  _COMPORT_H_
#define  _COMPORT_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <getopt.h>
#include <strings.h>

#define SERIALNAME_LEN 128
#define SMSLENTH       128

typedef struct _st_SerialAttr {

    int               fd;        //串口文件描述符
    int               BaudRate;  //波特率
    int               DataBits;  //数据位
    char              Parity;    //奇偶校验位
    int               StopBits;  //停止位
    int               mSend_Len; //单次最大发送长度
    char              SerialName[SERIALNAME_LEN];  //串口名称
    struct termios    OldTermios;  //串口的原始属性
}Serial_attr;

/* 打开串口 */
int comport_open(Serial_attr *attr);

/* 关闭串口 */
int comport_close(Serial_attr *attr);

/* 初始化串口 */
int comport_init(Serial_attr *attr);

/* 向串口发送指令 */
int comport_send(Serial_attr *attr,char *sbuf,int sbuf_len);

/* 接收串口指令 */
int comport_recv(Serial_attr *attr,char *rbuf,int rbuf_len,int timeout);

/* 发送TEXT格式短信 */
int text_sms(Serial_attr *attr,char *sms_buf,char *phone_num);

#endif   /*   ----- #ifndef _COMPORT_H_  ----- */







