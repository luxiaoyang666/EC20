/********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  PUD.h
 *    Description:  This head file of PDU.c
 *
 *        Version:  1.0.0(09/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "09/07/20 010:58:23"
 *                 
 ********************************************************************************/
#ifndef  _PDU_H_
#define  _PDU_H_

#include <stdio.h>
#include <string.h>
#include <iconv.h>
#include <stdlib.h>
#include "comport.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>

/* 字节流转字符流 */
void Hex2Str( const char *sSrc,  char *sDest, int nSrcLen );

/* UTF8 转 Unicode */
int utf8_to_unicode(char* utf8_buf,char* unic_buf);

/* 处理收件人电话号码 */
void Handling_phone_number(char *phone_number);

/*  处理短信中心号码 */
void Handling_center_number(char *center_number);

/* 发送PDU格式短信 */
int  pdu_sms(Serial_attr attr,char *sms_buf,char *center_number,char *phone_number);

#endif   /*  ----- #ifndef _PDU_H_  ----- */
