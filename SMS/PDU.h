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
#include <unistd.h>
#include <errno.h>
#include <string.h>



/* Convert byte stream data to a string */
int Hex2Str(const char *sSrc,char *sDest,int nSrcLen);

/* UTF8 -> Unicode */
int utf8_to_unicode(char* utf8_buf,char* unic_buf);

/* Process Recipient Number */
int Process_phone_number(char *phone_number);

/* Processing SMS Center Number */
int Process_center_number(char *center_number);

/* PDU encoding */
int pdu_encod(char *sms_buf,char *center_number,char *phone_number,char *pdu_buf,int *cmgs);


#endif   /*  ----- #ifndef _PDU_H_  ----- */
