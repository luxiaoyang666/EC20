/********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  ComportSwap.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(10/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "10/07/20 16:40:48"
 *                 
 ********************************************************************************/
#ifndef  _COMPORTSWAP_H_
#define  _COMPORTSWAP_H_

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "ComportOpen.h"


/* 收发串口指令，也可用于发送短信内容 */
int comport_send(MyAttr *attr,char *sbuf,int sbuf_len);
int comport_recv(MyAttr *attr,char *rbuf,int rbuf_len,int timeout);

#endif   /* ----- #ifndef _COMPORTSWAP_H_  ----- */
